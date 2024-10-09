/***************************************************************************//**
 *   @file   lattice_uart_stdio.c
 *   @brief  Source file of STDIO to UART linker for Lattice RISC-V RX
 *   @author Vilmos-Csaba Jozsa (vilmoscsaba.jozsa@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include "lattice_uart.h"
#include "hal.h"

/******************************************************************************/
/************************ Variable Definitions ********************************/
/******************************************************************************/

static struct no_os_uart_desc *stdio_desc = 0;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Blocking putc for stdio
 */
static int stdio_putc(char c, FILE *file)
{
	if (!stdio_desc)
		return -EFAULT;

	while (1) {
		if (no_os_uart_write(stdio_desc, &c, 1) == 1) {
			if ('\n' == c) {
				c = '\r';
				continue;
			}
			return 0;
		} else {
			break;
		}
	}

	return EOF;
}

/**
 * @brief Blocking getc for stdio
 */
static int stdio_getc(FILE *file)
{
	if (!stdio_desc)
		return -EFAULT;

	uint8_t c;
	int ret;

	if (no_os_uart_read(stdio_desc, &c, 1) == 1) {
		return c;
	} else {
		return 0;
	}

	return EOF;
}

/**
 * @brief Dummy flush for stdio
 */
static int stdio_flush(FILE *file)
{
	(void) file;
	return 0;
}

/**
 * @brief Lattice risc-v rx specific stdio to UART linker
 * @param desc         - Instance of UART.
 */
void no_os_uart_stdio(struct no_os_uart_desc *desc){

	stdio_desc = desc;
	iob_init(stdio_putc, stdio_getc, stdio_flush);
}
