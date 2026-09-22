/***************************************************************************//**
 *   @file   altera/altera_uart.h
 *   @brief  Altera / Nios V Avalon JTAG-UART driver header.
 *   @author Mihaela-Georgeta Petrea (Mihaela-georgeta.Petrea@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#ifndef ALTERA_UART_H_
#define ALTERA_UART_H_

#include <stdint.h>
#include <stdbool.h>
#include "no_os_uart.h"

/** Depth of the software RX ring drained by the ISR / polled read. */
#define ALTERA_UART_RX_RING_SIZE	512

/**
 * @struct altera_uart_init_param
 * @brief  Altera / Nios V JTAG-UART specific initialization parameters.
 */
struct altera_uart_init_param {
	/** Avalon JTAG-UART register-block base address. */
	uint32_t base_address;
	/** Qsys IRQ line number of the JTAG UART (e.g. SYS_UART_IRQ). */
	uint32_t irq_id;
	/** HAL interrupt controller id (e.g. SYS_UART_IRQ_INTERRUPT_CONTROLLER_ID). */
	uint32_t irq_ctrl_id;
};

/**
 * @struct altera_uart_desc
 * @brief  Altera / Nios V JTAG-UART specific descriptor.
 */
struct altera_uart_desc {
	/** Avalon JTAG-UART register-block base address. */
	uint32_t base_address;
	/** Qsys IRQ line number of the JTAG UART. */
	uint32_t irq_id;
	/** HAL interrupt controller id. */
	uint32_t irq_ctrl_id;
	/** True when RX is interrupt driven (asynchronous_rx requested). */
	bool irq_rx;
	/** Software RX ring buffer (single producer ISR / single consumer read). */
	volatile uint8_t rx_ring[ALTERA_UART_RX_RING_SIZE];
	/** RX ring producer index (written by ISR / polled fill). */
	volatile uint32_t rx_head;
	/** RX ring consumer index (written by read()). */
	volatile uint32_t rx_tail;
	/** Count of bytes dropped because the RX ring was full. */
	volatile uint32_t rx_overruns;
};

/**
 * @brief Altera / Nios V specific UART platform ops structure.
 */
extern const struct no_os_uart_platform_ops altera_uart_ops;

#endif /* ALTERA_UART_H_ */
