/***************************************************************************//**
 *   @file   maxim_uart_stdio.h
 *   @brief  stdio.h stubs implementation for printing/reading over UART.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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