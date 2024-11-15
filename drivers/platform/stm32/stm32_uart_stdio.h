/***************************************************************************//**
 *   @file   stm32/stm32_uart_stdio.h
 *   @brief  Header file of stm32 UART driver stdout/stdin redirection.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _STM32_UART_STDIO_H_
#define _STM32_UART_STDIO_H_

#include <sys/stat.h>
#include "no_os_uart.h"
#include "stm32_uart.h"

void no_os_uart_stdio(struct no_os_uart_desc *desc);
int _isatty(int fd);
int _write(int fd, char* ptr, int len);
int _close(int fd);
int _lseek(int fd, int ptr, int dir);
int _read(int fd, char* ptr, int len);
int _fstat(int fd, struct stat* st);

#endif
