/***************************************************************************//**
 *   @file   lattice_uart_stdio.c
 *   @brief  Source file of STDIO to UART linker for Lattice RISC-V RX
 *   @author Vilmos-Csaba Jozsa (vilmoscsaba.jozsa@analog.com)
********************************************************************************
 * Copyright 2024-2026(c) Analog Devices, Inc.
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

#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include "no_os_error.h"
#include "no_os_uart.h"

/******************************************************************************/
/************************ Variable Definitions ********************************/
/******************************************************************************/

#define STDIN_FILENO  0   /**> Definition of stdin */
#define STDOUT_FILENO 1   /**> Definition of stdout */
#define STDERR_FILENO 2   /**> Definition of stderr */

static struct no_os_uart_desc *guart = NULL;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

#if defined(NEWLIB)
/**
 * @brief Initialize UART for stdio
 * @param desc - UART descriptor to use for stdio
 */
void no_os_uart_stdio(struct no_os_uart_desc *desc)
{
	if (!desc)
		return;
	
	guart = desc;
	
	/* Disable I/O buffering for STDOUT stream */
	setvbuf(stdout, NULL, _IONBF, 0);
}

/**
 * @brief Check if file descriptor is a TTY
 * @param fd - File descriptor
 * @return 1 if TTY, 0 otherwise
 */
int _isatty(int fd)
{
	if (fd >= STDIN_FILENO && fd <= STDERR_FILENO)
		return 1;

	errno = EBADF;
	return 0;
}

/**
 * @brief Write data to file descriptor
 * @param fd - File descriptor
 * @param ptr - Data buffer
 * @param len - Number of bytes to write
 * @return Number of bytes written or -1 on error
 */
int _write(int fd, char *ptr, int len)
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

/**
 * @brief Close file descriptor
 * @param fd - File descriptor
 * @return 0 on success, -1 on error
 */
int _close(int fd)
{
	if (fd >= STDIN_FILENO && fd <= STDERR_FILENO)
		return 0;

	errno = EBADF;
	return -1;
}

/**
 * @brief Seek in file
 * @param fd - File descriptor
 * @param offset - Offset
 * @param whence - Whence
 * @return -1 (not supported)
 */
int _lseek(int fd, off_t offset, int whence)
{
	(void)fd;
	(void)offset;
	(void)whence;

	errno = EBADF;
	return -1;
}

/**
 * @brief Read data from file descriptor
 * @param fd - File descriptor
 * @param ptr - Data buffer
 * @param len - Number of bytes to read
 * @return Number of bytes read or -1 on error
 */
int _read(int fd, char *ptr, int len)
{
	int chunk;
	int ret;
	bool has_async_fifo;

	if (fd != STDIN_FILENO) {
		errno = EBADF;
		return -1;
	}

	if (!guart || !ptr || len <= 0)
		return 0;

	has_async_fifo = !!guart->rx_fifo;
	chunk = has_async_fifo ? len : 1;

	while (true) {
		ret = no_os_uart_read(guart, (uint8_t *)ptr, chunk);
		if (ret == -EAGAIN || ret == 0)
			continue;
		if (ret < 0) {
			errno = -ret;
			return -1;
		}
		/* Return as soon as at least one byte is available. */
		return ret;
	}
}

/**
 * @brief Get file status
 * @param fd - File descriptor
 * @param st - Stat structure
 * @return 0 on success, -1 on error
 */
int _fstat(int fd, struct stat *st)
{
	if (fd >= STDIN_FILENO && fd <= STDERR_FILENO) {
		st->st_mode = S_IFCHR;
		return 0;
	}

	errno = EBADF;
	return -1;
}
#endif

#ifdef PICOLIBC
/**
 * @brief Blocking putc for stdio
 */
static int stdio_putc(char c, FILE *file)
{
	if (!guart)
		return -EFAULT;

	while (1) {
		if (no_os_uart_write(guart, &c, 1) == 1) {
			if ('\n' == c) {
				c = '\r';
				continue;
			}
			return 0;
		} else {
			return EOF;
		}
	}
}

/**
 * @brief Blocking getc for stdio
 */
static int stdio_getc(FILE *file)
{
	if (!guart)
		return -EFAULT;

	uint8_t c;
	int ret;
	while (true) {
		ret = no_os_uart_read(guart, &c, 1);
		if (ret == -EAGAIN || ret == 0) {
			continue;
		}
		if (ret < 0) {
			return EOF;
		}
		return c;
	}
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
void no_os_uart_stdio(struct no_os_uart_desc *desc)
{
	guart = desc;

	iob_init(stdio_putc, stdio_getc, stdio_flush);
}
#endif

