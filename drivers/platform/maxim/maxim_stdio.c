/***************************************************************************//**
 *   @file   maxim/maxim_stdio.c
 *   @brief  Implementation file of MAX32660 UART driver stdout/stdin redirection.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
 *
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

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "maxim_stdio.h"

#include "mxc_config.h"
#include "mxc_sys.h"
#include "board.h"
#include "no-os/uart.h"
#include "uart.h"

#define CONSOLE_UART	(1)
#define MXC_UARTn   MXC_UART_GET_UART(CONSOLE_UART)
#define UART_FIFO   MXC_UART_GET_FIFO(CONSOLE_UART)

#include <unistd.h>
#include <sys/stat.h>

#define STDIN_FILENO	0   /**> Definition of stdin */
#define STDOUT_FILENO   1   /**> Definition of stdout */
#define STDERR_FILENO   2   /**> Definition of stderr */

static struct uart_desc *mdesc;

void maxim_uart_stdio(struct uart_desc *desc)
{
	if (!desc)
		return;
	mdesc = desc;
}

int _open(const char *name, int flags, int mode)
{
	return -1;
}
int _close(int file)
{
	return -1;
}
int _isatty(int file)
{
	return -1;
}
int _lseek(int file, off_t offset, int whence)
{
	return -1;
}
int _fstat(int file, struct stat *st)
{
	return -1;
}

int _read(int file, char *ptr, int len)
{
	int ret = 0;

	switch (file) {
	case STDIN_FILENO:
		ret = uart_read(mdesc, (uint8_t *)ptr, len);
		if (ret < 0) {
			errno = ret;
			return -1;
		}
		break;
	default:
		errno = EBADF;
		return -1;
	}
	return ret;
}

int _write(int file, char *ptr, int len)
{
	int32_t ret = 0;
	switch (file) {
	case STDOUT_FILENO:
	case STDERR_FILENO:
		ret = uart_write(mdesc, (const uint8_t *)ptr, len);
		if (ret < 0) {
			errno = ret;
			return -1;
		}

		break;
	default:
		errno = EBADF;
		return -1;
	}

	return len;
}
