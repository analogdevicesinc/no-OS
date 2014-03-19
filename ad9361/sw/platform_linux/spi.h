/*
 * SPIDEV user space helpers
 *
 * Copyright 2013 Analog Devices Inc.
 * Michael Hennerich (michael.hennerich@analog.com)
 *
 * Licensed under the GPL-2 or later
 */

#ifndef __LINUX_SPI_H__
#define __LINUX_SPI_H__

int spidev_write_then_read(const unsigned char *txbuf, unsigned n_tx,
			   unsigned char *rxbuf, unsigned n_rx);
int32_t spidev_init(char *device);
#endif /* __LINUX_SPI_H__ */
