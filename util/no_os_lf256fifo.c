#include <errno.h>
#include "no_os_lf256fifo.h"

struct lf256fifo {
    char * data; // pointer to memory area where the buffer will be stored
	uint8_t ffilled; // the index where the data starts (\note Read it as 'first-filled-slot')
	uint8_t fempty; // the index where empty/non-used area starts (\note Read it as 'first-empty-slot')
};

int lf256fifo_init(struct lf256fifo **fifo)
{
	if (fifo == NULL)
		return -EINVAL;

	struct lf256fifo *b = calloc(1, sizeof(struct lf256fifo));
	if (b == NULL)
		return -ENOMEM;

	b->data = calloc(1, 256);
	if (b->data == NULL)
		return -ENOMEM;

	*fifo = b;

	return 0;
}

bool lf256fifo_is_full(struct lf256fifo *fifo)
{
	return (fifo->fempty + 1) == fifo->ffilled; // intended overflow at 256 (data size is 256)
}

bool lf256fifo_is_empty(struct lf256fifo *fifo)
{ 
	return fifo->fempty == fifo->ffilled;
}

int lf256fifo_read(struct lf256fifo * fifo, char *c)
{
	if (lf256fifo_is_empty(fifo))
		return -1; // buffer empty

	*c = fifo->data[fifo->ffilled];
	fifo->ffilled++; // intended overflow at 256 (data size is 256)

	return 0;
}

int lf256fifo_write(struct lf256fifo *fifo, char c)
{
	if (lf256fifo_is_full(fifo))
		return -1; // buffer full

	fifo->data[fifo->fempty] = c;
	fifo->fempty++; // intended overflow at 256 (data size is 256)

	return 0; // return success
}

void lf256fifo_flush(struct lf256fifo *fifo)
{
	fifo->ffilled = fifo->fempty;
}

