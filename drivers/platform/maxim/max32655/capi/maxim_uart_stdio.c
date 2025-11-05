/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file maxim_uart_stdio.c
 * @brief Implementation file of MAX32655 CAPI UART driver stdout/stdin redirection
 * @author Claude Code (noreply@anthropic.com)
 */

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "maxim_uart_stdio.h"
#include "capi_uart.h"

#include <unistd.h>
#include <sys/stat.h>

#define STDIN_FILENO	0   /**< Definition of stdin */
#define STDOUT_FILENO   1   /**< Definition of stdout */
#define STDERR_FILENO   2   /**< Definition of stderr */

static struct capi_uart_handle *g_capi_uart = NULL;

/**
 * @brief Set up CAPI UART handle for stdio redirection
 * @param handle CAPI UART handle to use for printf/scanf
 */
void capi_uart_stdio(struct capi_uart_handle *handle)
{
	if (!handle)
		return;

	g_capi_uart = handle;

	/* Disable output buffering for immediate output */
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
}

/**
 * @brief Close file descriptor
 * @param file File descriptor
 * @return 0 on success, -1 on error
 */
int _close(int file)
{
	if (file >= STDIN_FILENO && file <= STDERR_FILENO)
		return 0;

	errno = EBADF;
	return -1;
}

/**
 * @brief Check if file descriptor is a terminal
 * @param file File descriptor
 * @return 1 if terminal, 0 otherwise
 */
int _isatty(int file)
{
	if (file >= STDIN_FILENO && file <= STDERR_FILENO)
		return 1;

	errno = EBADF;
	return 0;
}

/**
 * @brief Seek in file (not supported for UART)
 * @param file File descriptor
 * @param offset Offset
 * @param whence Seek mode
 * @return -1 (not supported)
 */
int _lseek(int file, off_t offset, int whence)
{
	(void) file;
	(void) offset;
	(void) whence;

	errno = EBADF;
	return -1;
}

/**
 * @brief Get file status
 * @param file File descriptor
 * @param st Status structure to fill
 * @return 0 on success, -1 on error
 */
int _fstat(int file, struct stat *st)
{
	if (!st) {
		errno = EINVAL;
		return -1;
	}

	if (file >= STDIN_FILENO && file <= STDERR_FILENO) {
		st->st_mode = S_IFCHR;  /* Character device */
		return 0;
	}

	errno = EBADF;
	return -1;
}

/**
 * @brief Read from file descriptor
 * @param file File descriptor
 * @param ptr Buffer to store data
 * @param len Number of bytes to read
 * @return Number of bytes read, or -1 on error
 */
int _read(int file, char *ptr, int len)
{
	int ret;

	if (!ptr || len <= 0) {
		errno = EINVAL;
		return -1;
	}

	if (file == STDIN_FILENO) {
		if (!g_capi_uart) {
			errno = ENODEV;
			return -1;
		}

		/* Read one byte at a time for stdin */
		ret = capi_uart_receive(g_capi_uart, (uint8_t *)ptr, 1);
		if (ret < 0) {
			errno = -ret;
			return -1;
		}

		return 1;  /* Always return 1 byte read for stdin */
	}

	errno = EBADF;
	return -1;
}

/**
 * @brief Write to file descriptor
 * @param file File descriptor
 * @param ptr Data to write
 * @param len Number of bytes to write
 * @return Number of bytes written, or -1 on error
 */
int _write(int file, char *ptr, int len)
{
	int ret;

	if (!ptr || len <= 0) {
		errno = EINVAL;
		return -1;
	}

	if (file == STDOUT_FILENO || file == STDERR_FILENO) {
		if (!g_capi_uart) {
			errno = ENODEV;
			return -1;
		}

		ret = capi_uart_transmit(g_capi_uart, (uint8_t *)ptr, len);
		if (ret < 0) {
			errno = -ret;
			return -1;
		}

		return len;  /* Return the requested length on success */
	}

	errno = EBADF;
	return -1;
}