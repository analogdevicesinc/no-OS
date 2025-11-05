/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file maxim_uart_stdio.h
 * @brief Header file for MAX32655 CAPI UART driver stdout/stdin redirection
 * @author Claude Code (noreply@anthropic.com)
 */

#ifndef MAXIM_CAPI_UART_STDIO_H_
#define MAXIM_CAPI_UART_STDIO_H_

#include <sys/stat.h>
#include "capi_uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set up CAPI UART handle for stdio redirection
 * @param handle CAPI UART handle to use for printf/scanf
 */
void capi_uart_stdio(struct capi_uart_handle *handle);

/* System call redirections for newlib */
int _isatty(int file);
int _write(int file, char *ptr, int len);
int _close(int file);
int _lseek(int file, off_t offset, int whence);
int _read(int file, char *ptr, int len);
int _fstat(int file, struct stat *st);

#ifdef __cplusplus
}
#endif

#endif /* MAXIM_CAPI_UART_STDIO_H_ */