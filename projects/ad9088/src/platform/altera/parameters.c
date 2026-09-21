/***************************************************************************//**
 *   @file   altera/parameters.c
 *   @brief  Altera/Nios V platform resource instances for the ad9088 project
 *           (AD9084-EBZ on Agilex 5).
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "parameters.h"

/******************************************************************************/
/************************ Variable Definitions ********************************/
/******************************************************************************/
/*
 * Clock-domain SPI (ADF4382 / HMC7044 / ADF4030) on sys_spi @ 0x40.
 * Chip-selects come from parameters.h. Verified live over JTAG.
 */
struct altera_spi_init_param clk_spi_extra_ip = {
	.type = NIOS_II_SPI,
	.base_address = CLK_SPI_BASEADDR,
};

/* Apollo/AD9084 SPI on apollo_spi @ 0xEA000 (separate Avalon SPI core). */
struct altera_spi_init_param apollo_spi_extra_ip = {
	.type = NIOS_II_SPI,
	.base_address = APOLLO_SPI_BASEADDR,
};

/* apollo_gpio @ 0xE8000 - AD9084 reset line (and other board GPIOs). */
struct altera_gpio_init_param altera_gpio_param = {
	.type = NIOS_II_GPIO,
	.device_id = 0,
	.base_address = GPIO_BASEADDR,
};

/*
 * stdout routing for the AD9084-EBZ.
 *
 * pr_*()/printf() reach the console through newlib's _write() syscall. The
 * generic Nios V BSP supplies a _write() that targets the example design's
 * JTAG UART (ALT_STDOUT_BASE 0x90158), which does not exist on this board, so
 * console output was silently discarded. Override _write() here with a polled
 * writer to the EBZ JTAG UART (sys_uart @ UART_JTAG_BASEADDR 0x100) - the same
 * instance juart-terminal reads. A strong symbol in this object wins over the
 * BSP archive's weak/library _write().
 *
 * Avalon JTAG UART register map (word-addressed):
 *   +0x00 data    : write byte in bits[7:0]
 *   +0x04 control : write FIFO space (WSPACE) in bits[31:16]
 * Polled TX: wait for WSPACE != 0, then push the byte. If no host is draining
 * the FIFO (no juart attached), WSPACE stays 0; a bounded spin then drops the
 * remaining bytes instead of hanging the application.
 *
 * The budget must exceed an attached juart-terminal's FIFO-drain latency, or a
 * burst of console output (several long lines back-to-back) overruns the 64-byte
 * FIFO and the tail is dropped. Sized large so an attached host always drains
 * before timeout, yet still bounded so a host-less boot proceeds.
 */
#define JTAG_UART_WSPACE_SPINS		200000000UL

int _write(int fd, char *ptr, int len)
{
	volatile uint32_t *data = (volatile uint32_t *)(UART_JTAG_BASEADDR + 0x00);
	volatile uint32_t *ctrl = (volatile uint32_t *)(UART_JTAG_BASEADDR + 0x04);
	int i;

	(void)fd;

	for (i = 0; i < len; i++) {
		unsigned long spins = JTAG_UART_WSPACE_SPINS;

		while (((*ctrl) >> 16) == 0) {
			if (--spins == 0)
				return i; /* no host draining - drop the rest */
		}
		*data = (uint8_t)ptr[i];
	}

	return len;
}
