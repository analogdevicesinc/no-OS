/***************************************************************************//**
 *   @file   stm32/stm32_uart.c
 *   @brief  Source file of UART driver for STM32
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
#include "no_os_uart.h"
#include "no_os_irq.h"
#include "no_os_lf256fifo.h"
#include "stm32_irq.h"
#include "stm32_uart.h"
#include "stm32_hal.h"

static uint8_t c;
uint32_t errors = 0;

void uart_rx_callback(void *context)
{
	struct no_os_uart_desc *d = context;
	lf256fifo_write(d->rx_fifo, c);
	HAL_UART_Receive_IT(((struct stm32_uart_desc *)d->extra)->huart, &c, 1);
}

/**
 * @brief Initialize the UART communication peripheral.
 * @param desc - The UART descriptor.
 * @param param - The structure that contains the UART parameters.
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_uart_init(struct no_os_uart_desc **desc,
			struct no_os_uart_init_param *param)
{
	struct stm32_uart_init_param *suip;
	struct stm32_uart_desc *sud;
	struct no_os_uart_desc *descriptor;
	struct no_os_irq_ctrl_desc *nvic;
	int ret;

	if (!desc || !param)
		return -EINVAL;

	descriptor = (struct no_os_uart_desc *) calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	sud = (struct stm32_uart_desc *) calloc(1, sizeof(struct stm32_uart_desc));
	if (!sud) {
		ret = -ENOMEM;
		goto error;
	}

	descriptor->irq_id = param->irq_id;
	descriptor->extra = sud;
	suip = param->extra;

	sud->huart = suip->huart;
	sud->huart->Init.BaudRate = param->baud_rate;
	switch (param->size) {
	case NO_OS_UART_CS_8:
		sud->huart->Init.WordLength = UART_WORDLENGTH_8B;
		break;
	case NO_OS_UART_CS_9:
		sud->huart->Init.WordLength = UART_WORDLENGTH_9B;
		break;
	default:
		ret = -EINVAL;
		goto error;
	};
	switch (param->parity) {
	case NO_OS_UART_PAR_NO:
		sud->huart->Init.Parity = UART_PARITY_NONE;
		break;
	case NO_OS_UART_PAR_ODD:
		sud->huart->Init.Parity = UART_PARITY_ODD;
		break;
	case NO_OS_UART_PAR_EVEN:
		sud->huart->Init.Parity = UART_PARITY_EVEN;
		break;
	default:
		ret = -EINVAL;
		goto error;
	};
	sud->huart->Init.StopBits = param->stop == NO_OS_UART_STOP_1_BIT ?
				    UART_STOPBITS_1 :
				    UART_STOPBITS_2;
	sud->huart->Init.Mode = suip->huart->Init.Mode;
	sud->huart->Init.HwFlowCtl = suip->huart->Init.HwFlowCtl;
	sud->huart->Init.OverSampling = suip->huart->Init.OverSampling;
	ret = HAL_UART_Init(sud->huart);
	if (ret != HAL_OK) {
		ret = -EIO;
		goto error;
	}

	sud->timeout = suip->timeout ? suip->timeout : HAL_MAX_DELAY;

	*desc = descriptor;

	// nonblocking uart_read
	if(param->asynchronous_rx) {
		ret = lf256fifo_init(&descriptor->rx_fifo);
		if (ret < 0)
			goto error;

		struct no_os_irq_init_param nvic_ip = {
			.platform_ops = &stm32_irq_ops,
			.extra = sud->huart,
		};
		ret = no_os_irq_ctrl_init(&nvic, &nvic_ip);
		if (ret < 0)
			goto error;

		struct no_os_callback_desc uart_rx_cb = {
			.callback = uart_rx_callback,
			.ctx = descriptor,
			.event = NO_OS_EVT_UART_RX_COMPLETE,
			.peripheral = NO_OS_UART_IRQ,
		};
		ret = no_os_irq_register_callback(nvic, descriptor->irq_id, &uart_rx_cb);
		if (ret < 0)
			goto error_nvic;

		ret = no_os_irq_enable(nvic, descriptor->irq_id);
		if (ret < 0)
			goto error_nvic;

		HAL_UART_Receive_IT(sud->huart, (uint8_t *)&c, 1);
		if (ret != HAL_OK) {
			ret = -EIO;
			goto error_nvic;
		}
	}

	return 0;
error_nvic:
	no_os_irq_ctrl_remove(nvic);
error:
	free(descriptor);
	free(sud);

	return ret;
}

/**
 * @brief Free the resources allocated by no_os_uart_init().
 * @param desc - The UART descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_uart_remove(struct no_os_uart_desc *desc)
{
	struct stm32_uart_desc *sud;

	if (!desc)
		return -EINVAL;

	sud = desc->extra;
	HAL_UART_DeInit(sud->huart);
	free(desc->extra);
	free(desc);

	return 0;
};

/**
 * @brief Write data to UART device.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_uart_write(struct no_os_uart_desc *desc, const uint8_t *data,
			 uint32_t bytes_number)
{
	struct stm32_uart_desc *sud;
	int32_t ret;

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	if (!bytes_number)
		return 0;

	sud = desc->extra;
	ret = HAL_UART_Transmit(sud->huart, (uint8_t *)data, bytes_number,
				sud->timeout);

	switch (ret) {
	case HAL_OK:
		break;
	case HAL_BUSY:
		return -EBUSY;
	case HAL_TIMEOUT:
		return -ETIMEDOUT;
	default:
		return -EIO;
	};

	return bytes_number;
}

/**
 * @brief Read data from UART device.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return positive number of received bytes in case of success, negative error code otherwise.
 */
int32_t no_os_uart_read(struct no_os_uart_desc *desc, uint8_t *data,
			uint32_t bytes_number)
{
	struct stm32_uart_desc *sud;
	uint32_t i = 0;
	int32_t ret;

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	if (!bytes_number)
		return 0;

	sud = desc->extra;

	if (desc->rx_fifo) {
		while(i < bytes_number) {
			ret = lf256fifo_read(desc->rx_fifo, &data[i]);
			if (ret < 0)
				break;
			i++;
		}
		return i;
	} else {
		ret = HAL_UART_Receive(sud->huart, (uint8_t *)data, bytes_number,
				       sud->timeout);

		switch (ret) {
		case HAL_OK:
			break;
		case HAL_BUSY:
			return -EBUSY;
		case HAL_TIMEOUT:
			return -ETIMEDOUT;
		default:
			return -EIO;
		};
	}

	return bytes_number;
}
