/***************************************************************************//**
 *   @file   maxim_uart_stdio.c
 *   @brief  Implementation file of MAX32660 UART driver stdout/stdin redirection.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
 *
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "maxim_uart_stdio.h"

#include "mxc_sys.h"
#include "no_os_uart.h"
#include "uart.h"

#include <unistd.h>
#include <sys/stat.h>

#define STDIN_FILENO	0   /**> Definition of stdin */
#define STDOUT_FILENO   1   /**> Definition of stdout */
#define STDERR_FILENO   2   /**> Definition of stderr */

static struct no_os_uart_desc *guart = NULL;

void no_os_uart_stdio(struct no_os_uart_desc *desc)
{
	if (!desc)
		return;
	guart = desc;

	setvbuf(stdout, NULL, _IONBF, 0);
}

int _close(int file)
{
	if (file >= STDIN_FILENO && file <= STDERR_FILENO)
		return 0;

	errno = EBADF;
	return -1;
}
int _isatty(int file)
{
	if (file >= STDIN_FILENO && file <= STDERR_FILENO)
		return 1;

	errno = EBADF;
	return 0;
}
int _lseek(int file, off_t offset, int whence)
{
	(void) file;
	(void) offset;
	(void) whence;

	errno = EBADF;
	return -1;
}
int _fstat(int file, struct stat *st)
{
	if (file >= STDIN_FILENO && file <= STDERR_FILENO) {
		st->st_mode = S_IFCHR;
		return 0;
	}

	errno = EBADF;
	return 0;
}

int _read(int file, char *ptr, int len)
{
	int ret;

	if (file == STDIN_FILENO) {
		ret = no_os_uart_read(guart, (uint8_t *)ptr, len);
		if (ret < 0) {
			errno = -ret;
			return -1;
		}

		return ret;
	}
	errno = EBADF;
	return -1;
}

int _write(int file, char *ptr, int len)
{
	int ret;

	if (file == STDOUT_FILENO || file == STDERR_FILENO) {
		ret = no_os_uart_write(guart, (uint8_t *)ptr, len);
		if (ret < 0) {
			errno = -ret;
			return -1;
		}

		return ret;
	}
	errno = EBADF;
	return -1;
}
