/***************************************************************************//**
 *   @file   maxim_uart_stdio.c
 *   @brief  Implementation file of MAX32660 UART driver stdout/stdin redirection.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
 *
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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
