/***************************************************************************//**
 *   @file   maxim_uart_stdio.h
 *   @brief  Header file for UART driver stdout/stdin redirection.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef MAXIM_UART_STDIO_H_
#define MAXIM_UART_STDIO_H_

#include <sys/stat.h>
#include "no_os_uart.h"

void no_os_uart_stdio(struct no_os_uart_desc *);
int _isatty(int);
int _write(int, char *, int);
int _close(int);
int _lseek(int, off_t, int);
int _read(int, char *, int);
int _fstat(int, struct stat *);

#endif
