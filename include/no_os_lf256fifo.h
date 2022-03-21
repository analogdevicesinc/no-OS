#ifndef __LFFIFO_H
#define __LFFIFO_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

struct lf256fifo;

int lf256fifo_init(struct lf256fifo **);
bool lf256fifo_is_full(struct lf256fifo *);
bool lf256fifo_is_empty(struct lf256fifo *);
int lf256fifo_read(struct lf256fifo *, char *);
int lf256fifo_write(struct lf256fifo *, char);
void lf256fifo_flush(struct lf256fifo *);

#endif

