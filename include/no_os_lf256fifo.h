/***************************************************************************//**
 *   @file   no_os_lf256fifo.h
 *   @brief  SPSC lock-free fifo of fixed size (256), specialized for UART.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 *   @copyright
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __LFFIFO_H
#define __LFFIFO_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

struct lf256fifo;

int lf256fifo_init(struct lf256fifo **);
bool lf256fifo_is_full(struct lf256fifo *);
bool lf256fifo_is_empty(struct lf256fifo *);
int lf256fifo_read(struct lf256fifo *, uint8_t *);
int lf256fifo_write(struct lf256fifo *, uint8_t);
void lf256fifo_flush(struct lf256fifo *);
void lf256fifo_remove(struct lf256fifo *fifo);

#endif

