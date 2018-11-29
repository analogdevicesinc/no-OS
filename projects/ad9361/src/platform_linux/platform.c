/***************************************************************************//**
 *   @file   Platform.c
 *   @brief  Implementation of Platform Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "../util.h"
#include "parameters.h"
#include "platform.h"
#include "adc_core.h"
#include "dac_core.h"

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
int spidev_fd;
#ifdef FMCOMMS5
int spidev_b_fd;
#endif

/***************************************************************************//**
 * @brief spi_init
*******************************************************************************/
int32_t spi_init(uint32_t device_id,
			uint8_t  clk_pha,
			uint8_t  clk_pol)
{
	uint8_t mode = SPI_CPHA;
	uint8_t bits = 8;
	uint32_t speed = 10000000;
	int ret;

	if (device_id) {
		// Unused variable - fix compiler warning
	}
	if (clk_pha) {
		// Unused variable - fix compiler warning
	}
	if (clk_pol) {
		// Unused variable - fix compiler warning
	}

	spidev_fd = open(SPIDEV_DEV, O_RDWR);
	if (spidev_fd < 0) {
		printf("%s: Can't open device\n\r", __func__);
		return -1;
	}

	ret = ioctl(spidev_fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1) {
		printf("%s: Can't set spi mode\n\r", __func__);
		return ret;
	}

	ret = ioctl(spidev_fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1) {
		printf("%s: Can't set bits per word\n\r", __func__);
		return ret;
	}
	
	ret = ioctl(spidev_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1) {
		printf("%s: Can't set max speed hz\n\r", __func__);
		return ret;
	}
#ifdef FMCOMMS5
	spidev_b_fd = open(SPIDEV_B_DEV, O_RDWR);
	if (spidev_b_fd < 0) {
		printf("%s: Can't open device\n\r", __func__);
		return -1;
	}

	ret = ioctl(spidev_b_fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1) {
		printf("%s: Can't set spi mode\n\r", __func__);
		return ret;
	}

	ret = ioctl(spidev_b_fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1) {
		printf("%s: Can't set bits per word\n\r", __func__);
		return ret;
	}
	
	ret = ioctl(spidev_b_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1) {
		printf("%s: Can't set max speed hz\n\r", __func__);
		return ret;
	}
#endif

	return 0;
}



/***************************************************************************//**
 * @brief spi_write_then_read
*******************************************************************************/
int spi_write_then_read(struct spi_device *spi,
		const unsigned char *txbuf, unsigned n_tx,
		unsigned char *rxbuf, unsigned n_rx)
{
	int ret = 0;

	if (spi) {
		// Unused variable - fix compiler warning
	}

	struct spi_ioc_transfer tr[2] = {
		{
			.tx_buf = (unsigned long)txbuf,
			.len = n_tx,
		}, {
			.rx_buf = (unsigned long)rxbuf,
			.len = n_rx,
		},
	};

	if (spi->id_no == 0) {
		ret = ioctl(spidev_fd, SPI_IOC_MESSAGE(2), &tr);
		if (ret == 1) {
			printf("%s: Can't send spi message\n\r", __func__);
			return -EIO;
		}
	} else {
#ifdef FMCOMMS5
		ret = ioctl(spidev_b_fd, SPI_IOC_MESSAGE(2), &tr);
		if (ret == 1) {
			printf("%s: Can't send spi message\n\r", __func__);
			return -EIO;
		}
#endif
	}

	return ret;
}

/***************************************************************************//**
 * @brief gpio_init
*******************************************************************************/
void gpio_init(uint32_t device_id)
{
	int fd, len;
	char buf[11];
	int ret;

	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (fd < 0) {
		printf("%s: Can't export GPIO\n\r", __func__);
		return;
	}
	
	len = snprintf(buf, sizeof(buf), "%d", device_id);
	ret = write(fd, buf, len);
	if (ret == -1) {
		// Unused variable - fix compiler warning
	}

	close(fd);
}

/***************************************************************************//**
 * @brief gpio_direction
*******************************************************************************/
void gpio_direction(uint16_t pin, uint8_t direction)
{
	int fd;
	char buf[60];
	int ret;

	snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/direction", pin);

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		printf("%s: Can't set GPIO direction\n\r", __func__);
		return;
	}

	if (direction == 1)
		ret = write(fd, "out", 4);
	else
		ret = write(fd, "in", 3);
	if (ret == -1) {
		// Unused variable - fix compiler warning
	}

	close(fd);
}

/***************************************************************************//**
 * @brief gpio_is_valid
*******************************************************************************/
bool gpio_is_valid(int number)
{
	if(number >= 0)
		return 1;
	else
		return 0;
}

/***************************************************************************//**
 * @brief gpio_data
*******************************************************************************/
void gpio_data(uint16_t pin, uint8_t data)
{
	int fd;
	char buf[60];
	int ret;

	snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/value", pin);

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		printf("%s: Can't set GPIO value\n\r", __func__);
		return;
	}

	if (data)
		ret = write(fd, "1", 2);
	else
		ret = write(fd, "0", 2);
	if (ret == -1) {
		// Unused variable - fix compiler warning
	}

	close(fd);
}

/***************************************************************************//**
 * @brief gpio_set_value
*******************************************************************************/
void gpio_set_value(unsigned gpio, int value)
{
	gpio_data(gpio, value);
}

/***************************************************************************//**
 * @brief udelay
*******************************************************************************/
void udelay(unsigned long usecs)
{
	usleep(usecs);
}

/***************************************************************************//**
 * @brief mdelay
*******************************************************************************/
void mdelay(unsigned long msecs)
{
	usleep(msecs * 1000);
}

/***************************************************************************//**
 * @brief msleep_interruptible
*******************************************************************************/
unsigned long msleep_interruptible(unsigned int msecs)
{
	usleep(msecs * 1000);
	return 0;
}

/***************************************************************************//**
 * @brief axiadc_init
*******************************************************************************/
void axiadc_init(struct ad9361_rf_phy *phy)
{
	adc_init(phy);
	dac_init(phy, DATA_SEL_DDS, 0);
}

/***************************************************************************//**
 * @brief axiadc_post_setup
*******************************************************************************/
int axiadc_post_setup(struct ad9361_rf_phy *phy)
{
	return ad9361_post_setup(phy);
}

/***************************************************************************//**
 * @brief axiadc_read
*******************************************************************************/
unsigned int axiadc_read(struct axiadc_state *st, unsigned long reg)
{
	unsigned int val;

	adc_read(st->phy, reg, &val);

	return val;
}

/***************************************************************************//**
 * @brief axiadc_write
*******************************************************************************/
void axiadc_write(struct axiadc_state *st, unsigned reg, unsigned val)
{
	adc_write(st->phy, reg, val);
}

/***************************************************************************//**
 * @brief axiadc_set_pnsel
*******************************************************************************/
int axiadc_set_pnsel(struct axiadc_state *st, int channel, enum adc_pn_sel sel)
{
	unsigned reg;

	uint32_t version = axiadc_read(st, 0x4000);

	if (PCORE_VERSION_MAJOR(version) > 7) {
		reg = axiadc_read(st, ADI_REG_CHAN_CNTRL_3(channel));
		reg &= ~ADI_ADC_PN_SEL(~0);
		reg |= ADI_ADC_PN_SEL(sel);
		axiadc_write(st, ADI_REG_CHAN_CNTRL_3(channel), reg);
	} else {
		reg = axiadc_read(st, ADI_REG_CHAN_CNTRL(channel));

		if (sel == ADC_PN_CUSTOM) {
			reg |= ADI_PN_SEL;
		} else if (sel == ADC_PN9) {
			reg &= ~ADI_PN23_TYPE;
			reg &= ~ADI_PN_SEL;
		} else {
			reg |= ADI_PN23_TYPE;
			reg &= ~ADI_PN_SEL;
		}

		axiadc_write(st, ADI_REG_CHAN_CNTRL(channel), reg);
	}

	return 0;
}

/***************************************************************************//**
 * @brief axiadc_idelay_set
*******************************************************************************/
void axiadc_idelay_set(struct axiadc_state *st,
				unsigned lane, unsigned val)
{
	if (PCORE_VERSION_MAJOR(st->pcore_version) > 8) {
		axiadc_write(st, ADI_REG_DELAY(lane), val);
	} else {
		axiadc_write(st, ADI_REG_DELAY_CNTRL, 0);
		axiadc_write(st, ADI_REG_DELAY_CNTRL,
				ADI_DELAY_ADDRESS(lane)
				| ADI_DELAY_WDATA(val)
				| ADI_DELAY_SEL);
	}
}