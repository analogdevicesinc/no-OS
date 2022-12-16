/***************************************************************************//**
 *   @file   pico/pico_uart.h
 *   @brief  Header file of UART driver for pico
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
#ifndef _PICO_UART_H_
#define _PICO_UART_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_uart.h"
#include "no_os_irq.h"
#include "hardware/uart.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct pico_uart_desc
 * @brief pico platform specific UART descriptor
 */
struct pico_uart_desc {
	/** UART instance */
	uart_inst_t *uart_instance;
	/** Controller that handles UART interrupts */
	struct no_os_irq_ctrl_desc *nvic;
	/** RX complete callback */
	struct no_os_callback_desc rx_callback;
};

/**
 * @brief Available GP config for UART TX
 */
enum uart_tx_gp {
	/* UART0 available configurations */
	UART0_TX_GP0 = 0,
	UART0_TX_GP12 = 12,
	UART0_TX_GP16 = 16,
	/* UART1 available configurations */
	UART1_TX_GP4 = 4,
	UART1_TX_GP8 = 8,
};

/**
 * @brief Available GP config for UART RX
 */
enum uart_rx_gp {
	/* UART0 available configurations */
	UART0_RX_GP1 = 1,
	UART0_RX_GP13 = 13,
	UART0_RX_GP17 = 17,
	/* UART1 available configurations */
	UART1_RX_GP5 = 5,
	UART1_RX_GP9 = 9,
};

/**
 * @brief Additional UART config parameters
 */
struct pico_uart_init_param {
	/** UART TX pin configuration */
	enum uart_tx_gp uart_tx_pin;
	/** UART RX pin configuration */
	enum uart_rx_gp uart_rx_pin;
};

/**
 * @brief pico specific UART platform ops structure
 */
extern const struct no_os_uart_platform_ops pico_uart_ops;

#endif
