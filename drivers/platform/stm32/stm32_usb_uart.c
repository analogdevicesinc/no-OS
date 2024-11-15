/***************************************************************************//**
 *   @file   stm32/stm32_usb_uart.c
 *   @brief  UART over USB driver for stm32 as a no_os_uart implementation.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
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

USBD_HandleTypeDef *gusbdevice;

static volatile int tx_pending;

/* This global FIFO is needed because of how the STM32 CDC API is implemented.
 * The function ops do not allow passing context parameters and so it is
 * impossible to pass this fifo to CDC_Receive() in any other way.
 * The implication is that only 1 USB CDC ACM interface may be used at once
 * but this is an acceptable compromise, since having more than one doesn't
 * make much sense anyway. */
static struct lf256fifo *gfifo;

static int8_t CDC_Init(void);
static int8_t CDC_DeInit(void);
static int8_t CDC_Control(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Receive(uint8_t* pbuf, uint32_t *Len);
static int8_t CDC_TransmitCplt(uint8_t *pbuf, uint32_t *Len, uint8_t epnum);

USBD_CDC_ItfTypeDef USBD_Interface_fops = {
	CDC_Init,
	CDC_DeInit,
	CDC_Control,
	CDC_Receive,
	CDC_TransmitCplt
};

static int8_t CDC_Init(void)
{
	USBD_CDC_SetTxBuffer(gusbdevice, txbuf, 0);
	USBD_CDC_SetRxBuffer(gusbdevice, rxbuf);

	return USBD_OK;
}

static int8_t CDC_DeInit(void)
{
	return USBD_OK;
}

static int8_t CDC_Control(uint8_t cmd, uint8_t* pbuf, uint16_t length)
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
}

static int8_t CDC_Receive(uint8_t* Buf, uint32_t *Len)
{
	stm32_on_usb_cdc_acm_rx(Buf, *Len);

	USBD_CDC_SetRxBuffer(gusbdevice, &Buf[0]);
	USBD_CDC_ReceivePacket(gusbdevice);

	return USBD_OK;
}

static uint8_t CDC_Transmit(uint8_t* Buf, uint16_t Len)
{
	uint8_t result = USBD_OK;
	USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)gusbdevice->pClassData;
	if (hcdc->TxState != 0)
		return USBD_BUSY;

	USBD_CDC_SetTxBuffer(gusbdevice, Buf, Len);
	result = USBD_CDC_TransmitPacket(gusbdevice);

	return result;
}

static int8_t CDC_TransmitCplt(uint8_t *Buf, uint32_t *Len, uint8_t epnum)
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
	struct stm32_usb_uart_init_param *suip;
	struct no_os_uart_desc *descriptor;
	struct stm32_usb_uart_desc *sdesc;

	if (!param)
		return -EINVAL;

	if (!param->extra)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	sdesc = no_os_calloc(1, sizeof(*sdesc));
	if (!sdesc) {
		ret = -ENOMEM;
		goto err_sdesc;
	}

	descriptor->extra = sdesc;
	suip = param->extra;

	sdesc->husbdevice = suip->husbdevice;

	ret = lf256fifo_init(&sdesc->fifo);
	if (ret)
		goto err_fifo;

	gfifo = sdesc->fifo;
	gusbdevice = sdesc->husbdevice;

	/* Stop the USB before configuring */
	ret = USBD_Stop(gusbdevice);
	if (ret)
		goto err_fifo;

	/* Configure USB CDC with new operations */
	ret = USBD_CDC_RegisterInterface(gusbdevice, &USBD_Interface_fops);
	if (ret)
		goto err_fifo;

	/* Restart the USB */
	ret = USBD_Start(gusbdevice);
	if (ret)
		goto err_fifo;

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
	ret = CDC_Transmit(data, len);
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
