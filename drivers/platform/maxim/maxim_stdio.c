/**
 * @file	stdio.c
 * @brief   Low level implementation of standard I/O functions
 */

/*******************************************************************************
 * Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *
 * $Date: 2019-08-02 10:43:59 -0500 (Fri, 02 Aug 2019) $
 * $Revision: 45191 $
 *
 ******************************************************************************/

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "maxim_stdio.h"

#include "mxc_config.h"
#include "mxc_sys.h"
#include "board.h"
#include "no-os/uart.h"
#include "uart.h"

#define MXC_UARTn   MXC_UART_GET_UART(CONSOLE_UART)
#define UART_FIFO   MXC_UART_GET_FIFO(CONSOLE_UART)

#include <unistd.h>
#include <sys/stat.h>

#define STDIN_FILENO	0   /**> Definition of stdin */
#define STDOUT_FILENO   1   /**> Definition of stdout */
#define STDERR_FILENO   2   /**> Definition of stderr */
#define EBADF		  -1   /**> Error code for EBADf */

static struct uart_desc *mdesc;

void maxim_uart_stdio(struct uart_desc *desc)
{
	if(!desc || !desc->extra)
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
		if(ret < 0) {
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
		if(ret < 0) {
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

