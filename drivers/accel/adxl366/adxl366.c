/***************************************************************************//**
 *   @file   adxl366.c
 *   @brief  Implementation of ADXL366 Driver.
 *   @author Marco Ramirez (marco.ramirez@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "adxl366.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_util.h"

/**
 * @brief Write device register via SPI.
 */
int adxl366_reg_write(struct adxl366_dev *dev, uint8_t reg_addr,
		      uint8_t reg_data)
{
	uint8_t buf[3];

	if (!dev || !dev->spi_desc)
		return -EINVAL;

	buf[0] = ADXL366_WRITE_REG;
	buf[1] = reg_addr;
	buf[2] = reg_data;

	return no_os_spi_write_and_read(dev->spi_desc, buf, 3);
}

/**
 * @brief Read device register via SPI.
 */
int adxl366_reg_read(struct adxl366_dev *dev, uint8_t reg_addr,
		     uint8_t *reg_data)
{
	uint8_t buf[3];
	int ret;

	if (!dev || !dev->spi_desc || !reg_data)
		return -EINVAL;

	buf[0] = ADXL366_READ_REG;
	buf[1] = reg_addr;
	buf[2] = 0x00;  /* Dummy byte */

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 3);
	if (ret < 0)
		return ret;

	*reg_data = buf[2];

	return 0;
}

/**
 * @brief Read multiple device registers via SPI.
 */
int adxl366_reg_read_multiple(struct adxl366_dev *dev, uint8_t reg_addr,
			      uint8_t *reg_data, uint16_t count)
{
	uint8_t buf[258];  /* Max: 2 cmd bytes + 256 data bytes */
	int ret;

	if (!dev || !dev->spi_desc || !reg_data || count == 0 || count > 256)
		return -EINVAL;

	buf[0] = ADXL366_READ_REG;
	buf[1] = reg_addr;
	memset(&buf[2], 0x00, count);  /* Dummy bytes */

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, count + 2);
	if (ret < 0)
		return ret;

	memcpy(reg_data, &buf[2], count);

	return 0;
}

/**
 * @brief Perform software reset.
 */
int adxl366_software_reset(struct adxl366_dev *dev)
{
	int ret;

	ret = adxl366_reg_write(dev, ADXL366_REG_SOFT_RESET, ADXL366_RESET_KEY);
	if (ret < 0)
		return ret;

	/* Wait for reset to complete */
	no_os_mdelay(10);

	return 0;
}

/**
 * @brief Set power mode.
 */
int adxl366_set_power_mode(struct adxl366_dev *dev, uint8_t mode)
{
	uint8_t power_ctl;
	int ret;

	/* Read current power control register */
	ret = adxl366_reg_read(dev, ADXL366_REG_POWER_CTL, &power_ctl);
	if (ret < 0)
		return ret;

	/* Update mode bits */
	power_ctl &= ~ADXL366_POWER_CTL_MODE_MSK;
	power_ctl |= (mode & ADXL366_POWER_CTL_MODE_MSK);

	return adxl366_reg_write(dev, ADXL366_REG_POWER_CTL, power_ctl);
}

/**
 * @brief Get power mode.
 */
int adxl366_get_power_mode(struct adxl366_dev *dev, uint8_t *mode)
{
	uint8_t power_ctl;
	int ret;

	if (!dev || !mode)
		return -EINVAL;

	ret = adxl366_reg_read(dev, ADXL366_REG_POWER_CTL, &power_ctl);
	if (ret < 0)
		return ret;

	*mode = power_ctl & ADXL366_POWER_CTL_MODE_MSK;

	return 0;
}

/**
 * @brief Read accelerometer data (14-bit).
 */
int adxl366_read_accel(struct adxl366_dev *dev, int16_t *x, int16_t *y,
		       int16_t *z)
{
	uint8_t buf[6];
	int ret;

	if (!dev || !x || !y || !z)
		return -EINVAL;

	/* Read 6 bytes starting from XDATA_H */
	ret = adxl366_reg_read_multiple(dev, ADXL366_REG_XDATA_H, buf, 6);
	if (ret < 0)
		return ret;

	/* Combine high and low bytes (14-bit data in upper 14 bits) */
	*x = ((int16_t)buf[0] << 8) | buf[1];
	*y = ((int16_t)buf[2] << 8) | buf[3];
	*z = ((int16_t)buf[4] << 8) | buf[5];

	/* Extract 14-bit value from bits [15:2] - arithmetic right shift sign-extends */
	*x = *x >> 2;
	*y = *y >> 2;
	*z = *z >> 2;

	return 0;
}

/**
 * @brief Initialize the device.
 */
int adxl366_init(struct adxl366_dev **device,
		 struct adxl366_init_param init_param)
{
	struct adxl366_dev *dev;
	uint8_t dev_id_ad, dev_id_mst, part_id;
	int ret;

	if (!device)
		return -EINVAL;

	dev = (struct adxl366_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* Initialize SPI */
	ret = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);
	if (ret < 0)
		goto error_dev;

	/* Small delay after SPI init */
	no_os_mdelay(10);

	/* Read and verify device IDs */
	ret = adxl366_reg_read(dev, ADXL366_REG_DEVID_AD, &dev_id_ad);
	if (ret < 0)
		goto error_spi;

	ret = adxl366_reg_read(dev, ADXL366_REG_DEVID_MST, &dev_id_mst);
	if (ret < 0)
		goto error_spi;

	ret = adxl366_reg_read(dev, ADXL366_REG_PARTID, &part_id);
	if (ret < 0)
		goto error_spi;

	/* Verify device IDs */
	if (dev_id_ad != ADXL366_DEVID_AD_VAL ||
	    dev_id_mst != ADXL366_DEVID_MST_VAL ||
	    part_id != ADXL366_PARTID_VAL) {
		ret = -ENODEV;
		goto error_spi;
	}

	dev->dev_id = part_id;

	/* Perform soft reset */
	ret = adxl366_software_reset(dev);
	if (ret < 0)
		goto error_spi;

	/* Set to measurement mode */
	ret = adxl366_set_power_mode(dev, ADXL366_POWER_CTL_MEASURE);
	if (ret < 0)
		goto error_spi;

	*device = dev;

	return 0;

error_spi:
	no_os_spi_remove(dev->spi_desc);
error_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Set measurement range.
 */
int adxl366_set_range(struct adxl366_dev *dev, enum adxl366_range range)
{
	uint8_t filter_ctl;
	int ret;

	if (!dev || range > ADXL366_RANGE_8G)
		return -EINVAL;

	ret = adxl366_reg_read(dev, ADXL366_REG_FILTER_CTL, &filter_ctl);
	if (ret < 0)
		return ret;

	filter_ctl &= ~ADXL366_FILTER_CTL_RANGE_MSK;
	filter_ctl |= no_os_field_prep(ADXL366_FILTER_CTL_RANGE_MSK, range);

	return adxl366_reg_write(dev, ADXL366_REG_FILTER_CTL, filter_ctl);
}

/**
 * @brief Get measurement range.
 */
int adxl366_get_range(struct adxl366_dev *dev, enum adxl366_range *range)
{
	uint8_t filter_ctl;
	int ret;

	if (!dev || !range)
		return -EINVAL;

	ret = adxl366_reg_read(dev, ADXL366_REG_FILTER_CTL, &filter_ctl);
	if (ret < 0)
		return ret;

	*range = no_os_field_get(ADXL366_FILTER_CTL_RANGE_MSK, filter_ctl);

	return 0;
}

/**
 * @brief Set output data rate.
 */
int adxl366_set_odr(struct adxl366_dev *dev, enum adxl366_odr odr)
{
	uint8_t filter_ctl;
	int ret;

	if (!dev || odr > ADXL366_ODR_400HZ)
		return -EINVAL;

	ret = adxl366_reg_read(dev, ADXL366_REG_FILTER_CTL, &filter_ctl);
	if (ret < 0)
		return ret;

	filter_ctl &= ~ADXL366_FILTER_CTL_ODR_MSK;
	filter_ctl |= no_os_field_prep(ADXL366_FILTER_CTL_ODR_MSK, odr);

	return adxl366_reg_write(dev, ADXL366_REG_FILTER_CTL, filter_ctl);
}

/**
 * @brief Get output data rate.
 */
int adxl366_get_odr(struct adxl366_dev *dev, enum adxl366_odr *odr)
{
	uint8_t filter_ctl;
	int ret;

	if (!dev || !odr)
		return -EINVAL;

	ret = adxl366_reg_read(dev, ADXL366_REG_FILTER_CTL, &filter_ctl);
	if (ret < 0)
		return ret;

	*odr = no_os_field_get(ADXL366_FILTER_CTL_ODR_MSK, filter_ctl);

	return 0;
}

/**
 * @brief Read temperature sensor.
 */
int adxl366_read_temperature(struct adxl366_dev *dev, float *temp_c)
{
	uint8_t buf[2];
	int16_t temp_raw;
	int ret;

	if (!dev || !temp_c)
		return -EINVAL;

	ret = adxl366_reg_read_multiple(dev, ADXL366_REG_TEMP_H, buf, 2);
	if (ret < 0)
		return ret;

	/* Combine high and low bytes (12-bit signed in upper 12 bits) */
	temp_raw = ((int16_t)buf[0] << 8) | buf[1];
	temp_raw = temp_raw >> 4;  /* Extract 12-bit value */

	/* Convert to Celsius: 25°C at 0 LSB, 0.065°C/LSB */
	*temp_c = 25.0f + ((float)temp_raw * 0.065f);

	return 0;
}

/**
 * @brief Configure FIFO mode.
 */
int adxl366_set_fifo_mode(struct adxl366_dev *dev, enum adxl366_fifo_mode mode,
			  uint16_t watermark)
{
	uint8_t fifo_ctl;
	int ret;

	if (!dev || mode > ADXL366_FIFO_TRIGGERED || watermark > 255)
		return -EINVAL;

	ret = adxl366_reg_read(dev, ADXL366_REG_FIFO_CONTROL, &fifo_ctl);
	if (ret < 0)
		return ret;

	fifo_ctl &= ~ADXL366_FIFO_CONTROL_MODE_MSK;
	fifo_ctl |= no_os_field_prep(ADXL366_FIFO_CONTROL_MODE_MSK, mode);

	ret = adxl366_reg_write(dev, ADXL366_REG_FIFO_CONTROL, fifo_ctl);
	if (ret < 0)
		return ret;

	/* Set watermark (FIFO_SAMPLES register) */
	return adxl366_reg_write(dev, ADXL366_REG_FIFO_SAMPLES, (uint8_t)watermark);
}

/**
 * @brief Read FIFO entries count.
 */
int adxl366_get_fifo_entries(struct adxl366_dev *dev, uint16_t *entries)
{
	uint8_t buf[2];
	int ret;

	if (!dev || !entries)
		return -EINVAL;

	ret = adxl366_reg_read_multiple(dev, ADXL366_REG_FIFO_ENTRIES_L, buf, 2);
	if (ret < 0)
		return ret;

	*entries = ((uint16_t)buf[1] << 8) | buf[0];

	return 0;
}

/**
 * @brief Read FIFO data (up to 85 samples: 256 bytes / 3 bytes per sample).
 */
int adxl366_read_fifo(struct adxl366_dev *dev, int16_t *x, int16_t *y,
		      int16_t *z, uint16_t *entries)
{
	uint8_t buf[258];  /* 2 cmd bytes + 256 data bytes max */
	uint16_t fifo_entries, samples_to_read;
	int ret, i;

	if (!dev || !x || !y || !z || !entries)
		return -EINVAL;

	/* Read number of entries in FIFO */
	ret = adxl366_get_fifo_entries(dev, &fifo_entries);
	if (ret < 0)
		return ret;

	if (fifo_entries == 0) {
		*entries = 0;
		return 0;
	}

	/* Each sample is 3 bytes (X, Y, Z as 8-bit) - read up to 85 samples */
	samples_to_read = (fifo_entries > 85) ? 85 : fifo_entries;

	buf[0] = ADXL366_READ_FIFO;
	buf[1] = 0x00;  /* Dummy address */
	memset(&buf[2], 0x00, samples_to_read * 3);

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, samples_to_read * 3 + 2);
	if (ret < 0)
		return ret;

	/* Parse FIFO data (8-bit samples) */
	for (i = 0; i < samples_to_read; i++) {
		x[i] = (int8_t)buf[2 + i * 3];
		y[i] = (int8_t)buf[2 + i * 3 + 1];
		z[i] = (int8_t)buf[2 + i * 3 + 2];
	}

	*entries = samples_to_read;

	return 0;
}

/**
 * @brief Set activity threshold.
 */
int adxl366_set_activity_threshold(struct adxl366_dev *dev, uint16_t threshold)
{
	uint8_t thresh_h, thresh_l;
	int ret;

	if (!dev || threshold > 8191)  /* 13-bit value */
		return -EINVAL;

	thresh_h = (uint8_t)(threshold >> 5);
	thresh_l = (uint8_t)((threshold & 0x1F) << 3);

	ret = adxl366_reg_write(dev, ADXL366_REG_THRESH_ACT_H, thresh_h);
	if (ret < 0)
		return ret;

	return adxl366_reg_write(dev, ADXL366_REG_THRESH_ACT_L, thresh_l);
}

/**
 * @brief Set inactivity threshold.
 */
int adxl366_set_inactivity_threshold(struct adxl366_dev *dev,
				     uint16_t threshold)
{
	uint8_t thresh_h, thresh_l;
	int ret;

	if (!dev || threshold > 8191)  /* 13-bit value */
		return -EINVAL;

	thresh_h = (uint8_t)(threshold >> 5);
	thresh_l = (uint8_t)((threshold & 0x1F) << 3);

	ret = adxl366_reg_write(dev, ADXL366_REG_THRESH_INACT_H, thresh_h);
	if (ret < 0)
		return ret;

	return adxl366_reg_write(dev, ADXL366_REG_THRESH_INACT_L, thresh_l);
}

/**
 * @brief Set activity time.
 */
int adxl366_set_activity_time(struct adxl366_dev *dev, uint8_t time)
{
	if (!dev)
		return -EINVAL;

	return adxl366_reg_write(dev, ADXL366_REG_TIME_ACT, time);
}

/**
 * @brief Set inactivity time.
 */
int adxl366_set_inactivity_time(struct adxl366_dev *dev, uint16_t time)
{
	uint8_t time_h, time_l;
	int ret;

	if (!dev)
		return -EINVAL;

	time_h = (uint8_t)(time >> 8);
	time_l = (uint8_t)(time & 0xFF);

	ret = adxl366_reg_write(dev, ADXL366_REG_TIME_INACT_H, time_h);
	if (ret < 0)
		return ret;

	return adxl366_reg_write(dev, ADXL366_REG_TIME_INACT_L, time_l);
}

/**
 * @brief Enable/disable activity detection.
 */
int adxl366_set_activity_detection(struct adxl366_dev *dev, bool enable)
{
	uint8_t act_inact_ctl;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adxl366_reg_read(dev, ADXL366_REG_ACT_INACT_CTL, &act_inact_ctl);
	if (ret < 0)
		return ret;

	if (enable)
		act_inact_ctl |= ADXL366_ACT_INACT_CTL_ACT_EN;
	else
		act_inact_ctl &= ~ADXL366_ACT_INACT_CTL_ACT_EN;

	return adxl366_reg_write(dev, ADXL366_REG_ACT_INACT_CTL, act_inact_ctl);
}

/**
 * @brief Enable/disable inactivity detection.
 */
int adxl366_set_inactivity_detection(struct adxl366_dev *dev, bool enable)
{
	uint8_t act_inact_ctl;
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adxl366_reg_read(dev, ADXL366_REG_ACT_INACT_CTL, &act_inact_ctl);
	if (ret < 0)
		return ret;

	if (enable)
		act_inact_ctl |= ADXL366_ACT_INACT_CTL_INACT_EN;
	else
		act_inact_ctl &= ~ADXL366_ACT_INACT_CTL_INACT_EN;

	return adxl366_reg_write(dev, ADXL366_REG_ACT_INACT_CTL, act_inact_ctl);
}

/**
 * @brief Configure interrupt mapping.
 */
int adxl366_set_interrupt_map(struct adxl366_dev *dev, uint8_t int_pin,
			      uint8_t int_mask)
{
	uint8_t reg_addr;

	if (!dev || int_pin > 2)
		return -EINVAL;

	reg_addr = (int_pin == 1) ? ADXL366_REG_INTMAP1_LWR : ADXL366_REG_INTMAP2_LWR;

	return adxl366_reg_write(dev, reg_addr, int_mask);
}

/**
 * @brief Read status register.
 */
int adxl366_get_status(struct adxl366_dev *dev, uint8_t *status)
{
	if (!dev || !status)
		return -EINVAL;

	return adxl366_reg_read(dev, ADXL366_REG_STATUS, status);
}

/**
 * @brief Free the resources allocated by adxl366_init().
 */
int adxl366_remove(struct adxl366_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret < 0)
		return ret;

	no_os_free(dev);

	return 0;
}
