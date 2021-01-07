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
#include "uart.h"
#include "stm32_uart.h"
#include "stm32_hal.h"

/**
 * @brief Initialize the UART communication peripheral.
 * @param desc - The UART descriptor.
 * @param param - The structure that contains the UART parameters.
 * @return SUCCESS in case of success, error code otherwise.
 */
int32_t uart_init(struct uart_desc **desc, struct uart_init_param *param)
{
	struct stm32_uart_init_param *suip;
	struct stm32_uart_desc *sud;
	struct uart_desc *descriptor;
	USART_TypeDef *base = NULL;
	int ret;

	if (!desc || !param)
		return -EINVAL;

	descriptor = (struct uart_desc *) calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	sud = (struct stm32_uart_desc *) calloc(1, sizeof(struct stm32_uart_desc));
	if (!sud) {
		ret = -ENOMEM;
		goto error;
	}

	descriptor->extra = sud;
	suip = param->extra;

	switch (param->device_id) {
#ifdef USART1
	case 1:
		base = USART1;
		break;
#endif
#ifdef USART2
	case 2:
		base = USART2;
		break;
#endif
#ifdef USART3
	case 3:
		base = USART3;
		break;
#endif
#ifdef USART4
	case 4:
		base = USART4;
		break;
#endif
#ifdef USART5
	case 5:
		base = USART5;
		break;
#endif
#ifdef USART6
	case 6:
		base = USART6;
		break;
#endif
#ifdef USART7
	case 7:
		base = USART7;
		break;
#endif
#ifdef USART8
	case 8:
		base = USART8;
		break;
#endif
	default:
		ret = -EINVAL;
		goto error;
	};

	sud->huart.Instance = base;
	sud->huart.Init.BaudRate = param->baud_rate;
	switch (param->size) {
	case UART_CS_8:
		sud->huart.Init.WordLength = UART_WORDLENGTH_8B;
		break;
	case UART_CS_9:
		sud->huart.Init.WordLength = UART_WORDLENGTH_9B;
		break;
	default:
		ret = -EINVAL;
		goto error;
	};
	switch (param->parity) {
	case UART_PAR_NO:
		sud->huart.Init.Parity = UART_PARITY_NONE;
		break;
	case UART_PAR_ODD:
		sud->huart.Init.Parity = UART_PARITY_ODD;
		break;
	case UART_PAR_EVEN:
		sud->huart.Init.Parity = UART_PARITY_EVEN;
		break;
	default:
		ret = -EINVAL;
		goto error;
	};
	sud->huart.Init.StopBits = param->stop == UART_STOP_1 ? UART_STOPBITS_1 :
				   UART_STOPBITS_2;
	sud->huart.Init.Mode = suip->mode;
	sud->huart.Init.HwFlowCtl = suip->hw_flow_ctl;
	sud->huart.Init.OverSampling = suip->over_sampling;
	ret = HAL_UART_Init(&sud->huart);
	if (ret != HAL_OK) {
		ret = -EIO;
		goto error;
	}

	*desc = descriptor;

	return 0;
error:
	free(descriptor);
	free(sud);

	return ret;
}

/**
 * @brief Free the resources allocated by uart_init().
 * @param desc - The UART descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t uart_remove(struct uart_desc *desc)
{
	struct stm32_uart_desc *sud;

	if (!desc)
		return -EINVAL;

	sud = desc->extra;
	HAL_UART_DeInit(&sud->huart);
	free(desc->extra);
	free(desc);

	return 0;
};

/**
 * @brief Write data to UART device.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t uart_write(struct uart_desc *desc, const uint8_t *data,
		   uint32_t bytes_number)
{
	struct stm32_uart_desc *sud;
	int32_t ret;

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	if (!bytes_number)
		return 0;

	sud = desc->extra;
	ret = HAL_UART_Transmit(&sud->huart, (uint8_t *)data, bytes_number,
				HAL_MAX_DELAY);
	if (ret != HAL_OK)
		return -EIO;

	return 0;
}

/**
 * @brief Read data from UART device.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return positive number of received bytes in case of success, negative error code otherwise.
 */
int32_t uart_read(struct uart_desc *desc, uint8_t *data,
		  uint32_t bytes_number)
{
	struct stm32_uart_desc *sud;
	int32_t ret;

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	if (!bytes_number)
		return 0;

	sud = desc->extra;

	ret = HAL_UART_Receive(&sud->huart, (uint8_t *)data, bytes_number,
			       HAL_MAX_DELAY);
	if (ret != HAL_OK)
		return -EIO;

	return bytes_number;
}