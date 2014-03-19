/*
 * SPI user space helpers
 *
 * Copyright 2013 Analog Devices Inc.
 * Michael Hennerich (michael.hennerich@analog.com)
 *
 * Licensed under the GPL-2 or later
 */

#ifndef __LINUX_SPI_H__
#define __LINUX_SPI_H__

#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stdint.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

int fd;

/***************************************************************************//**
 * @brief spidev_init
*******************************************************************************/
int32_t spidev_init(char *device)
{
	uint8_t mode = SPI_CPHA;
	uint8_t bits = 8;
	uint32_t speed = 10000000;
	int ret;

	fd = open(device, O_RDWR);
	if (fd < 0) {
		perror("can't open device");
		return fd;
	}

	/*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		perror("can't set spi mode");

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		perror("can't set bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		perror("can't set max speed hz");


	return fd;
}

/***************************************************************************//**
 * @brief spidev_write_then_read
*******************************************************************************/
int spidev_write_then_read(const unsigned char *txbuf, unsigned n_tx,
			   unsigned char *rxbuf, unsigned n_rx)
{
	int ret;
	struct spi_ioc_transfer tr[2] = {
		{
			.tx_buf = (unsigned long)txbuf,
			.len = n_tx,
		}, {
			.rx_buf = (unsigned long)rxbuf,
			.len = n_rx,
		},
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(2), &tr);
	if (ret == 1) {
		perror("can't send spi message");
		return -EIO;
	}

	return ret;
}

#endif /* __LINUX_SPI_H__ */
