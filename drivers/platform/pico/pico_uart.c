/***************************************************************************//**
 *   @file   pico/pico_uart.c
 *   @brief  Source file of UART driver for pico
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_error.h"
#include "no_os_uart.h"
#include "no_os_lf256fifo.h"
#include "no_os_alloc.h"
#include "pico_uart.h"
#include "pico_irq.h"
#include "pico/stdlib.h"
#include "hardware/irq.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief UART RX callback handler
 * @param context  - Callback context, cntains uart descriptor.
 */
void uart_rx_callback(void *context)
{
	struct no_os_uart_desc *d = context;
	struct pico_uart_desc *pico_uart = d->extra;

	uint8_t ch = uart_getc(pico_uart->uart_instance);
	lf256fifo_write(d->rx_fifo, ch);
}

/**
 * @brief Initialize the UART communication peripheral.
 * @param desc  - The UART descriptor.
 * @param param - The structure that contains the UART parameters.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t pico_uart_init(struct no_os_uart_desc **desc,
			      struct no_os_uart_init_param *param)
{
	struct pico_uart_desc *pico_uart;
	struct pico_uart_init_param *pico_uart_ip;
	struct no_os_uart_desc *descriptor;
	int ret;
	uint8_t data_bits;
	uint8_t stop_bits;
	uint8_t parity;

	if (!desc || !param || !param->extra)
		return -EINVAL;

	descriptor = (struct no_os_uart_desc *) no_os_malloc(sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	pico_uart = (struct pico_uart_desc *) no_os_malloc(sizeof(*pico_uart));
	if (!pico_uart) {
		ret = -ENOMEM;
		goto error;
	}

	switch(param->device_id) {
	case 0:
		pico_uart->uart_instance = uart0;
		break;
	case 1:
		pico_uart->uart_instance = uart1;
		break;
	default:
		ret = -EINVAL;
		goto error;
	}

	descriptor->device_id = param->device_id;
	descriptor->irq_id = param->irq_id;
	descriptor->extra = pico_uart;
	pico_uart_ip = param->extra;
	stdio_uart_init_full(pico_uart->uart_instance, param->baud_rate,
			     pico_uart_ip->uart_tx_pin, pico_uart_ip->uart_rx_pin);

	uart_set_hw_flow(pico_uart->uart_instance, false, false);

	switch (param->size) {
	case NO_OS_UART_CS_5:
		data_bits = 5;
		break;
	case NO_OS_UART_CS_6:
		data_bits = 6;
		break;
	case NO_OS_UART_CS_7:
		data_bits = 7;
		break;
	case NO_OS_UART_CS_8:
		data_bits = 8;
		break;
	default:
		ret = -EINVAL;
		goto error;
	};

	switch (param->parity) {
	case NO_OS_UART_PAR_NO:
		parity= UART_PARITY_NONE;
		break;
	case NO_OS_UART_PAR_ODD:
		parity = UART_PARITY_ODD;
		break;
	case NO_OS_UART_PAR_EVEN:
		parity = UART_PARITY_EVEN;
		break;
	default:
		ret = -EINVAL;
		goto error;
	};

	stop_bits = param->stop == NO_OS_UART_STOP_1_BIT ? 1 : 2;
	uart_set_format(pico_uart->uart_instance, data_bits, stop_bits, parity);

	/* Turn off FIFO's - we want to do this character by character */
	uart_set_fifo_enabled(pico_uart->uart_instance, false);

	*desc = descriptor;

	if(param->asynchronous_rx) {
		ret = lf256fifo_init(&descriptor->rx_fifo);
		if (ret)
			goto error;

		struct no_os_irq_init_param nvic_ip = {
			.platform_ops = &pico_irq_ops,
		};

		ret = no_os_irq_ctrl_init(&pico_uart->nvic, &nvic_ip);
		if (ret)
			goto error;

		struct no_os_callback_desc uart_rx_cb = {
			.callback = uart_rx_callback,
			.ctx = descriptor,
			.event = NO_OS_EVT_UART_RX_COMPLETE,
			.peripheral = NO_OS_UART_IRQ,
			.handle = pico_uart->uart_instance,
		};

		pico_uart->rx_callback = uart_rx_cb;

		uint8_t uart_irq_id = pico_uart->uart_instance == uart0 ? UART0_IRQ : UART1_IRQ;

		ret = no_os_irq_register_callback(pico_uart->nvic,
						  uart_irq_id,
						  &pico_uart->rx_callback);
		if (ret)
			goto error_nvic;

		/* Now enable the UART to send interrupts - RX only */
		uart_set_irq_enables(pico_uart->uart_instance, true, false);

		no_os_irq_enable(pico_uart->nvic, uart_irq_id);
	}

	return 0;

error_nvic:
	no_os_irq_ctrl_remove(pico_uart->nvic);
error:
	no_os_free(descriptor);
	no_os_free(pico_uart);

	return ret;
}

/**
 * @brief Free the resources allocated by pico_uart_init().
 * @param desc - The UART descriptor.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t pico_uart_remove(struct no_os_uart_desc *desc)
{
	struct pico_uart_desc *pico_uart;

	if (!desc)
		return -EINVAL;

	pico_uart = desc->extra;
	uart_deinit(pico_uart->uart_instance);

	if (desc->rx_fifo) {
		no_os_irq_disable(pico_uart->nvic, desc->irq_id);
		lf256fifo_remove(desc->rx_fifo);
		desc->rx_fifo = NULL;
		no_os_irq_unregister_callback(pico_uart->nvic, desc->irq_id,
					      &pico_uart->rx_callback);
		no_os_irq_ctrl_remove(pico_uart->nvic);
	}

	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
};

/**
 * @brief Write data to UART device.
 * @param desc         - Instance of UART.
 * @param data         - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t pico_uart_write(struct no_os_uart_desc *desc,
			       const uint8_t *data,
			       uint32_t bytes_number)
{
	struct pico_uart_desc *pico_uart;

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	if (!bytes_number)
		return 0;

	pico_uart = desc->extra;

	uart_write_blocking(pico_uart->uart_instance, data, bytes_number);

	return bytes_number;
}

/**
 * @brief Read data from UART device.
 * @param desc         - Instance of UART.
 * @param data         - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return positive number of received bytes in case of success, error code otherwise.
 */
static int32_t pico_uart_read(struct no_os_uart_desc *desc, uint8_t *data,
			      uint32_t bytes_number)
{
	struct pico_uart_desc *pico_uart;
	int ret;
	uint32_t i;

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	if (!bytes_number)
		return 0;

	pico_uart = desc->extra;

	if (desc->rx_fifo) {
		for (i = 0; i < bytes_number; i++) {
			ret = lf256fifo_read(desc->rx_fifo, &data[i]);
			if (ret)
				return i ? (int32_t)i : -EAGAIN;
		}
		return i;
	}

	uart_read_blocking(pico_uart->uart_instance, data, bytes_number);

	return bytes_number;
}

/**
 * @brief pico platform specific UART platform ops structure
 */
const struct no_os_uart_platform_ops pico_uart_ops = {
	.init = &pico_uart_init,
	.read = &pico_uart_read,
	.write = &pico_uart_write,
	.remove = &pico_uart_remove
};
