/***************************************************************************//**
 *   @file   mbed_uart.h
 *   @brief  Header containing extra types required for UART interface
********************************************************************************
 * Copyright (c) 2021-23 Analog Devices, Inc.
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

#ifndef MBED_UART_H_
#define MBED_UART_H_

// Platform support needs to be C-compatible to work with other drivers
#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdio.h>
#include <stdbool.h>

/******************************************************************************/
/********************** Variables and User defined data types *****************/
/******************************************************************************/

/*
 * Note: The structure members are not strongly typed, as this file is included
 *       in application specific '.c' files. The mbed code structure does not
 *       allow inclusion of mbed driver files (e.g. mbed.h) into '.c' files.
 *       All the members are hence typecasted to mbed specific type during
 *       uart init and read/write operations.
 **/

/**
 * @struct mbed_uart_init_param
 * @brief Structure holding the UART init parameters for mbed platform.
 */
struct mbed_uart_init_param {
	uint16_t uart_tx_pin;		/* UART Transmit Pin (only for UART comm) */
	uint16_t uart_rx_pin;		/* UART Receive Pin (only for UART comm) */
	uint16_t vendor_id;			/* USB VCOM Vendor ID (only for USB Virtual comm) */
	uint16_t product_id;		/* USB VCOM Product ID (only for USB Virtual comm) */
	char *serial_number;		/* USB VCOM serial number (only for USB Virtual comm) */
	bool is_console_stdio_port; /* Set the UART/USB port for console stdio operation */
};

/**
 * @struct mbed_uart_desc
 * @brief UART specific descriptor for the mbed platform.
 */
struct mbed_uart_desc {
	void *uart_port; 			/* UART port instance */
	bool virtual_com_enable;	/* To set port for console stdio operation */
	bool is_console_stdio_port; /* Set the UART/USB port for console stdio operation */
};

/**
* @brief Mbed platform specific UART platform ops structure
*/
extern const struct no_os_uart_platform_ops mbed_uart_ops;

/**
* @brief Mbed platform specific VCOM platform ops structure
*/
extern const struct no_os_uart_platform_ops mbed_virtual_com_ops;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

#ifdef __cplusplus // Closing extern c
}
#endif

#endif /* MBED_UART_H_ */
