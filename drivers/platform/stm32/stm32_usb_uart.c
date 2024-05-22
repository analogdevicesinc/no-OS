/***************************************************************************//**
 *   @file   stm32/stm32_usb_uart.c
 *   @brief  UART over USB driver for stm32 as a no_os_uart implementation.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include <errno.h>
#include <stdlib.h>
#include "no_os_util.h"
#include "stm32_usb_uart.h"
#include "no_os_lf256fifo.h"
#include "no_os_alloc.h"
#include "usbd_cdc.h"

#ifndef STM32_USB_CDC_ACM_RXBUF_LEN
#define STM32_USB_CDC_ACM_RXBUF_LEN 512
#endif

#ifndef STM32_USB_CDC_ACM_TXBUF_LEN
#define STM32_USB_CDC_ACM_TXBUF_LEN 512
#endif

static uint8_t rxbuf[STM32_USB_CDC_ACM_RXBUF_LEN];
static uint8_t txbuf[STM32_USB_CDC_ACM_TXBUF_LEN];
extern USBD_HandleTypeDef hUsbDeviceFS;

static volatile int tx_pending;

/* This global FIFO is needed because of how the STM32 CDC API is implemented.
 * The function ops do not allow passing context parameters and so it is
 * impossible to pass this fifo to CDC_Receive_FS() in any other way.
 * The implication is that only 1 USB CDC ACM interface may be used at once
 * but this is an acceptable compromise, since having more than one doesn't
 * make much sense anyway. */
static struct lf256fifo *gfifo;

static int8_t CDC_Init_FS(void);
static int8_t CDC_DeInit_FS(void);
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Receive_FS(uint8_t* pbuf, uint32_t *Len);
static int8_t CDC_TransmitCplt_FS(uint8_t *pbuf, uint32_t *Len, uint8_t epnum);

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS = {
	CDC_Init_FS,
	CDC_DeInit_FS,
	CDC_Control_FS,
	CDC_Receive_FS,
	CDC_TransmitCplt_FS
};

static int8_t CDC_Init_FS(void)
{
	USBD_CDC_SetTxBuffer(&hUsbDeviceFS, txbuf, 0);
	USBD_CDC_SetRxBuffer(&hUsbDeviceFS, rxbuf);

	return USBD_OK;
}

static int8_t CDC_DeInit_FS(void)
{
	return USBD_OK;
}

static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
	return USBD_OK;
}

void stm32_on_usb_cdc_acm_rx(uint8_t* buf, uint32_t len)
{
	unsigned int i = 0;
	int ret;

	while(i < len) {
		ret = lf256fifo_write(gfifo, buf[i]);
		if (ret)
			break;

		i++;
	}

	return i;
}

static int8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len)
{
	stm32_on_usb_cdc_acm_rx(Buf, *Len);

	USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
	USBD_CDC_ReceivePacket(&hUsbDeviceFS);

	return USBD_OK;
}

static uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
	uint8_t result = USBD_OK;
	USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;
	if (hcdc->TxState != 0)
		return USBD_BUSY;

	USBD_CDC_SetTxBuffer(&hUsbDeviceFS, Buf, Len);
	result = USBD_CDC_TransmitPacket(&hUsbDeviceFS);

	return result;
}

static int8_t CDC_TransmitCplt_FS(uint8_t *Buf, uint32_t *Len, uint8_t epnum)
{
	tx_pending = 0;

	return USBD_OK;
}

/**
 * @brief Initialize the UART communication peripheral.
 * @param desc - The UART descriptor.
 * @param param - The structure that contains the UART parameters.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t stm32_usb_uart_init(struct no_os_uart_desc **desc,
				   struct no_os_uart_init_param *param)
{
	int ret;
	struct no_os_uart_desc *descriptor;
	struct stm32_usb_uart_desc *sdesc;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	sdesc = no_os_calloc(1, sizeof(*sdesc));
	if (!sdesc) {
		ret = -ENOMEM;
		goto err_sdesc;
	}
	descriptor->extra = sdesc;

	ret = lf256fifo_init(&sdesc->fifo);
	if (ret)
		goto err_fifo;

	gfifo = sdesc->fifo;

	*desc = descriptor;

	return 0;
err_fifo:
	no_os_free(sdesc);
err_sdesc:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free the resources allocated by stm32_uart_init().
 * @param desc - The UART descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t stm32_usb_uart_remove(struct no_os_uart_desc *desc)
{
	struct stm32_usb_uart_desc *sdesc = desc->extra;
	lf256fifo_remove(sdesc->fifo);
	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
};

/**
 * @brief Write data to UART device.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to write.
 * @return Positive number of transmitted bytes in case of success, negative error code otherwise.
 */
static int32_t stm32_usb_uart_write(struct no_os_uart_desc *desc,
				    const uint8_t *data,
				    uint32_t bytes_number)
{
	int ret;
	unsigned int len = no_os_min(bytes_number, STM32_USB_CDC_ACM_TXBUF_LEN);

	tx_pending = 1;
	ret = CDC_Transmit_FS(data, len);
	if (ret) {
		tx_pending = 0;
		return -EFAULT;
	}

	while(tx_pending)
		;

	return len;
}

/**
 * @brief Read data from UART device.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return positive number of received bytes in case of success, negative error code otherwise.
 */
static int32_t stm32_usb_uart_read(struct no_os_uart_desc *desc, uint8_t *data,
				   uint32_t bytes_number)
{
	int ret;
	unsigned int i = 0;
	struct stm32_usb_uart_desc *sdesc = desc->extra;

	while(i < bytes_number) {
		ret = lf256fifo_read(sdesc->fifo, &data[i]);
		if (ret)
			break;
		i++;
	}

	return i;
}

/**
 * @brief STM32 platform specific UART platform ops structure
 */
const struct no_os_uart_platform_ops stm32_usb_uart_ops = {
	.init = &stm32_usb_uart_init,
	.read = &stm32_usb_uart_read,
	.write = &stm32_usb_uart_write,
	.remove = &stm32_usb_uart_remove
};
