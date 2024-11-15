/***************************************************************************//**
 *   @file   aducm3029_uart_stdio.c
 *   @brief  Source file to use printf over uart
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#if ENABLE_UART_STDIO
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include "no_os_uart.h"

static struct no_os_uart_desc *g_uart;

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

void no_os_uart_stdio(struct no_os_uart_desc *desc)
{
	g_uart = desc;
}

int _isatty(int fd)
{
	if (fd >= STDIN_FILENO && fd <= STDERR_FILENO)
		return 1;

	errno = EBADF;
	return 0;
}

int _write(int fd, char* ptr, int len)
{
	int32_t ret;

	if (!g_uart)
		return EIO;

	if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
		ret = no_os_uart_write(g_uart, (uint8_t *) ptr, len);
		if (ret < 0) {
			errno = ret;
			return EIO;
		}
		return len;
	}
	errno = EBADF;

	return -1;
}

int _close(int fd)
{
	if (fd >= STDIN_FILENO && fd <= STDERR_FILENO)
		return 0;

	errno = EBADF;
	return -1;
}

int _lseek(int fd, int ptr, int dir)
{
	(void) fd;
	(void) ptr;
	(void) dir;

	errno = EBADF;
	return -1;
}

int _read(int fd, char* ptr, int len)
{
	int32_t ret;

	if (!g_uart)
		return EIO;

	if (fd == STDIN_FILENO) {
		ret = no_os_uart_read(g_uart, (uint8_t *) ptr, len);
		if (ret < 0)
			return EIO;

		return len;
	}
	errno = EBADF;
	return -1;
}

int _fstat(int fd, struct stat* st)
{
	if (fd >= STDIN_FILENO && fd <= STDERR_FILENO) {
		st->st_mode = S_IFCHR;
		return 0;
	}

	errno = EBADF;
	return -1;
}
#endif
