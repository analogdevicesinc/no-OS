/***************************************************************************//**
 *   @file   stm32/stm32_uart_stdio.c
 *   @brief  Implementation file of stm32 UART driver stdout/stdin redirection.
 *   @author Darius Berghe (darius.berghe@analog.com)
 *           Credit to Carmine Noviello for stdio redirection in this file
 * 	     that was inspired from his "Mastering STM32" book.
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/times.h>
#include <limits.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include "no_os_uart.h"
#include "stm32_uart_stdio.h"
#include "stm32_hal.h"

#if !defined(OS_USE_SEMIHOSTING)

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

static struct no_os_uart_desc *guart = NULL;

void no_os_uart_stdio(struct no_os_uart_desc *desc)
{
	if(!desc || !desc->extra)
		return;

	guart = desc;

	/* Disable I/O buffering for STDOUT stream, so that
	* chars are sent out as soon as they are printed. */
	setvbuf(stdout, NULL, _IONBF, 0);
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
	int ret;

	if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
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
	int ret;

	if (fd == STDIN_FILENO) {
		ret = no_os_uart_read(guart, (uint8_t *)ptr, 1);
		if (ret < 0) {
			errno = -ret;
			return -1;
		}

		return ret;
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
	return 0;
}

#endif //#if !defined(OS_USE_SEMIHOSTING)
