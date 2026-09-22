/***************************************************************************//**
 *   @file   altera/altera_irq.h
 *   @brief  Altera / Nios V (CLIC/CLINT) IRQ controller driver header.
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

#ifndef ALTERA_IRQ_H_
#define ALTERA_IRQ_H_

#include <stdint.h>
#include "no_os_irq.h"

/**
 * @struct altera_irq_desc
 * @brief Altera / Nios V platform specific IRQ descriptor.
 *
 * The Nios V internal interrupt controller (CLIC or CLINT) is core-local and
 * addressed through the BSP HAL, so the descriptor only needs to remember the
 * controller id the HAL expects (ignored by most HAL entry points, but carried
 * for completeness / symmetry with the other platforms).
 */
struct altera_irq_desc {
	/** Interrupt controller id passed to the BSP HAL alt_ic_* calls. */
	uint32_t ic_id;
};

/**
 * @brief Altera / Nios V specific IRQ platform ops structure.
 */
extern const struct no_os_irq_platform_ops altera_irq_ops;

/**
 * @brief One-shot bring-up of the Nios V core-local interrupt controller.
 *
 * Wraps the BSP HAL alt_irq_init() so it runs exactly once for the lifetime of
 * the application. The HAL routine clears MIE and re-initializes the CLIC on
 * every call, so it must not run again after a peripheral (e.g. the JTAG UART)
 * has enabled its interrupt line. Both the IRQ controller driver and the UART
 * driver call this instead of alt_irq_init() directly, making them order
 * independent inside iio_app (uart_setup runs before irq_setup).
 */
void altera_irq_hw_init_once(void);

#endif /* ALTERA_IRQ_H_ */
