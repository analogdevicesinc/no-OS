#ifndef _MAXIM_UART_STDIO_H_
#define _MAXIM_UART_STDIO_H_

#include <sys/stat.h>
#include "no-os/uart.h"

void maxim_uart_stdio(struct uart_desc *);
int _open(const char *, int, int);
int _isatty(int);
int _write(int, char *, int);
int _close(int);
int _lseek(int, off_t, int);
int _read(int, char *, int);
int _fstat(int, struct stat *);

#endif

