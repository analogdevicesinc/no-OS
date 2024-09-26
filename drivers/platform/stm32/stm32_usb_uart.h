/***************************************************************************//**
 *   @file   stm32/stm32_usb_uart.h
 *   @brief  UART over USB driver for stm32 as a no_os_uart implementation.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef _STM32_USB_UART_H_
#define _STM32_USB_UART_H_

#include <stdint.h>
#include <stdbool.h>
#include "no_os_uart.h"
#include "stm32_hal.h"
#include "usbd_cdc.h"

/**
 * @struct stm32_usb_uart_init_param
 * @brief Specific initialization parameters for stm32 UART over USB.
 */
struct stm32_usb_uart_init_param {
	/** USB UART instance */
	USBD_HandleTypeDef *husbdevice;
};

/**
 * @struct stm32_usb_uart_desc
 * @brief stm32 platform specific UART over USB descriptor.
 */
struct stm32_usb_uart_desc {
	/** USB UART instance */
	USBD_HandleTypeDef *husbdevice;
	/** FIFO */
	struct lf256fifo *fifo;
};

/**
 * @brief stm32 specific UART over USB platform ops structure.
 */
extern const struct no_os_uart_platform_ops stm32_usb_uart_ops;

#endif
