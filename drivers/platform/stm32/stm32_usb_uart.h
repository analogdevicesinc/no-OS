/***************************************************************************//**
 *   @file   stm32/stm32_usb_uart.h
 *   @brief  UART over USB driver for stm32 as a no_os_uart implementation.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
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
