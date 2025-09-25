/*******************************************************************************
 *   @file   adxl371.c
 *   @brief  Implementation of adxl371 Core Driver.
 *   @author Brandon Hurst (brandon.hurst@analog.com)
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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "adxl371.h"
#include "no_os_alloc.h"
#include "no_os_error.h"

/**
 * @brief Wrapper used to read device registers
 *
 * @param dev - The device structure
 * @param reg_addr - The register address
 * @param reg_data - The register data
 * @return 0 in case of success, negative error code otherwise
 */
int adxl371_read_reg(struct adxl371_dev *dev,
		     uint8_t reg_addr,
		     uint8_t *reg_data)
{
	return (int)dev->reg_read(dev, reg_addr, reg_data);
}

/**
 * @brief Wrapper used to write to device registers
 *
 * @param dev - The device structure
 * @param reg_addr - The register address
 * @param reg_data - The register data
 * @return 0 in case of success, negative error code otherwise
 */
int adxl371_write_reg(struct adxl371_dev *dev,
		      uint8_t reg_addr,
		      uint8_t reg_data)
{
	return (int)dev->reg_write(dev, reg_addr, reg_data);
}

/**
 * @brief Wrapper used of multibyte reads
 *
 * @param dev - The device structure
 * @param reg_addr - The register address
 * @param reg_data - The register data
 * @param count - Number of bytes to read
 * @return 0 in case of success, negative error code otherwise
 */
int adxl371_read_reg_multiple(struct adxl371_dev *dev,
			      uint8_t reg_addr,
			      uint8_t *reg_data,
			      uint16_t count)
{
	return (int)dev->reg_read_multiple(dev, reg_addr, reg_data, count);
}

/**
 * @brief Write to device register  using a bitmask
 *
 * @param dev - The device structure
 * @param reg_addr - The register address
 * @param mask - The mask.
 * @param data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int adxl371_write_mask(struct adxl371_dev *dev,
		       uint8_t reg_addr,
		       uint32_t mask,
		       uint8_t data)
{
	uint8_t reg_data;
	int ret;

	ret = adxl371_read_reg(dev, reg_addr, &reg_data);
	if (ret < 0)
		return ret;

	reg_data &= ~mask;
	reg_data |= data;

	return adxl371_write_reg(dev, reg_addr, reg_data);
}

/**
 * @brief Set the threshold for activity detection for all 3-axis
 *
 * @param dev - The device structure.
 * @param act - Type of activity.
 *		Accepted values: ADXL371_ACTIVITY
 *				 ADXL371_ACTIVITY2
 *				 ADXL371_INACTIVITY
 * @param thresh - 11-bit unsigned value sets the threshold for activity,
 * 		   activity2 or inactivity detection.
 * @param referenced - Selects referenced or absolute activity processing.
 * @param enable - Enable activity detection using all 3-axis data.
 * @return 0 in case of success, negative error code otherwise.
 */
int adxl371_set_activity_threshold(struct adxl371_dev *dev,
				   enum adxl371_th_activity act,
				   uint16_t thresh,
				   bool referenced,
				   bool enable)
{
	uint8_t th_val_h, th_val_l, i;
	int ret = 0;

	th_val_h = (thresh >> 3);
	th_val_l = (thresh << 5) | (referenced << 1) | enable;

	for (i = 0; i < 3; i++) {
		ret = adxl371_write_reg(dev,
					adxl371_th_reg_addr_h[act][i],
					th_val_h);
		if (ret < 0)
			return ret;

		ret = adxl371_write_reg(dev,
					adxl371_th_reg_addr_l[act][i],
					th_val_l);
		if (ret < 0)
			return ret;
	}

	return ret;
}

/**
 * @brief Set the mode of operation
 *
 * @param dev - The device structure
 * @param op_mode - Mode of operation
 *		Accepted values: ADXL371_STANDBY
 *				 ADXL371_WAKE_UP
 *				 ADXL371_INSTANT_ON
 *				 ADXL371_FULL_BW_MEASUREMENT
 * @return 0 in case of success, negative error code otherwise.
 */
int adxl371_set_op_mode(struct adxl371_dev *dev,
			enum adxl371_op_mode op_mode)
{
	return adxl371_write_mask(dev,
				  ADXL371_POWER_CTL,
				  ADXL371_POWER_CTL_MODE_MSK,
				  ADXL371_POWER_CTL_MODE(op_mode));
}

/**
 * @brief Get operating mode for the ADXL device
 * (standby, wakeup, instant on, full bandwidthw measurement)
 *
 * @param dev ADXL device data structure
 * @param op_mode pointer to an enum adxl371_op_mode which will be updated with
 * 			the current operating mode of the device
 * @return 0 in case of success, negative error code otherwise
 */
int adxl371_get_op_mode(struct adxl371_dev *dev, enum adxl371_op_mode *op_mode)
{
	uint8_t reg_data;
	int ret;

	ret = adxl371_read_reg(dev, ADXL371_POWER_CTL, &reg_data);
	if (ret < 0)
		return ret;

	reg_data &= ADXL371_POWER_CTL_MODE_MSK;
	*op_mode = (enum adxl371_op_mode)reg_data;
	return 0;
}

/**
 * @brief Autosleep. When set to 1, autosleep is enabled, and the device enters
 * wake-up mode automatically upon detection of inactivity.
 *
 * @param dev - The device structure.
 * @param enable - Accepted values: true
 *				    false
 * @return 0 in case of success, negative error code otherwise.
 */
int adxl371_set_autosleep(struct adxl371_dev *dev, bool enable)
{
	return adxl371_write_mask(dev,
				  ADXL371_MEASURE,
				  ADXL371_MEASURE_AUTOSLEEP_MSK,
				  ADXL371_MEASURE_AUTOSLEEP_MODE(enable));
}

/**
 * @brief Select the desired output signal bandwidth
 *
 * @param dev - The device structure
 * @param bw - bandwidth
 *		Accepted values: ADXL371_BW_200HZ
 *				 ADXL371_BW_400HZ
 *				 ADXL371_BW_800HZ
 *				 ADXL371_BW_1600HZ
 *				 ADXL371_BW_3200HZ
 * @return 0 in case of success, negative error code otherwise.
 */
int adxl371_set_bandwidth(struct adxl371_dev *dev,
			  enum adxl371_bandwidth bw)
{
	int ret;

	ret = adxl371_write_mask(dev,
				 ADXL371_MEASURE,
				 ADXL371_MEASURE_BANDWIDTH_MSK,
				 ADXL371_MEASURE_BANDWIDTH_MODE(bw));
	if (ret < 0)
		return ret;

	dev->bw = bw;

	return ret;
}

/**
 * @brief Link/Loop Activity Processing
 *
 * @param dev - The device structure
 * @param mode - Mode of operation
 *		Accepted values: ADXL371_DEFAULT
 *				 ADXL371_LINKED
 *				 ADXL371_LOOPED
 * @return 0 in case of success, negative error code otherwise.
 */
int adxl371_set_act_proc_mode(struct adxl371_dev *dev,
			      enum adxl371_act_proc_mode mode)
{
	int ret;

	ret = adxl371_write_mask(dev,
				 ADXL371_MEASURE,
				 ADXL371_MEASURE_LINKLOOP_MSK,
				 ADXL371_MEASURE_LINKLOOP_MODE(mode));
	if (ret < 0)
		return ret;

	dev->act_proc_mode = mode;

	return ret;
}

/**
 * @brief Set Output Data Rate (odr)
 *
 * @param dev - The device structure
 * @param odr - Output data rate
 *		Accepted values: ADXL371_ODR_400HZ
 *				 ADXL371_ODR_800HZ
 *				 ADXL371_ODR_1600HZ
 *				 ADXL371_ODR_3200HZ
 *				 ADXL371_ODR_6400HZ
 * @return 0 in case of success, negative error code otherwise.
 */
int adxl371_set_odr(struct adxl371_dev *dev,
		    enum adxl371_odr odr)
{
	int ret;

	ret = adxl371_write_mask(dev,
				 ADXL371_TIMING,
				 ADXL371_TIMING_ODR_MSK,
				 ADXL371_TIMING_ODR_MODE(odr));
	if (ret < 0)
		return ret;

	dev->odr = odr;

	return ret;
}

/**
 * @brief Select instant on threshold
 *
 * @param dev - The device structure.
 * @param mode - 0 = low threshold, 1 = high threshold.
 *		Accepted values: ADXL371_INSTANT_ON_LOW_TH
 *				 ADXL371_INSTANT_ON_HIGH_TH
 * @return 0 in case of success, negative error code otherwise.
 */
int adxl371_set_instant_on_th(struct adxl371_dev *dev,
			      enum adxl371_instant_on_th_mode mode)
{
	int ret;

	ret = adxl371_write_mask(dev,
				 ADXL371_POWER_CTL,
				 ADXL371_POWER_CTL_INSTANT_ON_TH_MSK,
				 ADXL371_POWER_CTL_INSTANT_ON_TH_MODE(mode));
	if (ret < 0)
		return ret;

	dev->th_mode = mode;

	return ret;
}

/**
 * @brief Set the Timer Rate for Wake-Up Mode
 *
 * @param dev - The device structure.
 * @param wur - wake up mode rate
 *		Accepted values: ADXL371_WUR_52ms
 *				 ADXL371_WUR_104ms
 *				 ADXL371_WUR_208ms
 *				 ADXL371_WUR_512ms
 *				 ADXL371_WUR_2048ms
 *				 ADXL371_WUR_4096ms
 *				 ADXL371_WUR_8192ms
 *				 ADXL371_WUR_24576ms
 * @return 0 in case of success, negative error code otherwise.
 */
int adxl371_set_wakeup_rate(struct adxl371_dev *dev,
			    enum adxl371_wakeup_rate wur)
{
	int ret;

	ret = adxl371_write_mask(dev,
				 ADXL371_TIMING,
				 ADXL371_TIMING_WAKE_UP_RATE_MSK,
				 ADXL371_TIMING_WAKE_UP_RATE_MODE(wur));
	if (ret < 0)
		return ret;

	dev->wur = wur;

	return ret;
}

/**
 * @brief Set the activity timer
 *
 * @param dev - The device structure.
 * @param time - The value set in this register.
 * @return 0 in case of success, negative error code otherwise.
 */
int adxl371_set_activity_time(struct adxl371_dev *dev, uint8_t time)
{
	return adxl371_write_reg(dev, ADXL371_TIME_ACT, time);
}

/**
 * @brief Set the inactivity timer
 *
 * @param dev - The device structure
 * @param time - is the 16-bit value set by the TIME_INACT_L register
 * 		(eight LSBs) and the TIME_INACT_H register (eight MSBs)
 * @return 0 in case of success, negative error code otherwise
 */
int adxl371_set_inactivity_time(struct adxl371_dev *dev, uint16_t time)
{
	int ret;

	ret = adxl371_write_reg(dev, ADXL371_TIME_INACT_H, time >> 8);
	if (ret < 0)
		return ret;

	return adxl371_write_reg(dev, ADXL371_TIME_INACT_L, time & 0xFF);
}

/**
 * @brief Set the filter settling period
 *
 * @param dev - The device structure
 * @param mode - settle period
 *		Accepted values: ADXL371_FILTER_SETTLE_370
 *				 ADXL371_FILTER_SETTLE_16
 * @return 0 in case of success, negative error code otherwise
 */
int adxl371_set_filter_settle(struct adxl371_dev *dev,
			      enum adxl371_filter_settle mode)
{
	return adxl371_write_mask(dev,
				  ADXL371_POWER_CTL,
				  ADXL371_POWER_CTL_FIL_SETTLE_MSK,
				  ADXL371_POWER_CTL_FIL_SETTLE_MODE(mode));
}

/**
 * @brief Configure the INT1 and INT2 interrupt pins.
 *
 * @param dev - The device structure.
 * @param int1 -  INT1 interrupt pins.
 * @param int2 -  INT2 interrupt pins.
 * @return 0 in case of success, negative error code otherwise.
 */
int adxl371_interrupt_config(struct adxl371_dev *dev,
			     struct adxl371_irq_config int1,
			     struct adxl371_irq_config int2)
{
	uint8_t int1_config, int2_config;
	int ret;

	int1_config = (ADXL371_INT1_MAP_DATA_RDY_MODE(int1.data_rdy) |
		       ADXL371_INT1_MAP_FIFO_RDY_MODE(int1.fifo_rdy) |
		       ADXL371_INT1_MAP_FIFO_FULL_MODE(int1.fifo_full) |
		       ADXL371_INT1_MAP_FIFO_OVR_MODE(int1.fifo_ovr) |
		       ADXL371_INT1_MAP_INACT_MODE(int1.inactivity) |
		       ADXL371_INT1_MAP_ACT_MODE(int1.activity) |
		       ADXL371_INT1_MAP_AWAKE_MODE(int1.awake) |
		       ADXL371_INT1_MAP_LOW_MODE(int1.low_operation));

	int2_config = (ADXL371_INT2_MAP_DATA_RDY_MODE(int2.data_rdy) |
		       ADXL371_INT2_MAP_FIFO_RDY_MODE(int2.fifo_rdy) |
		       ADXL371_INT2_MAP_FIFO_FULL_MODE(int2.fifo_full) |
		       ADXL371_INT2_MAP_FIFO_OVR_MODE(int1.fifo_ovr) |
		       ADXL371_INT2_MAP_INACT_MODE(int2.inactivity) |
		       ADXL371_INT2_MAP_ACT_MODE(int2.activity) |
		       ADXL371_INT2_MAP_AWAKE_MODE(int2.awake) |
		       ADXL371_INT2_MAP_LOW_MODE(int2.low_operation));

	ret = adxl371_write_reg(dev, ADXL371_INT1_MAP, int1_config);
	if (ret < 0)
		return ret;

	return adxl371_write_reg(dev, ADXL371_INT2_MAP, int2_config);
}

/**
 * @brief Get the STATUS, STATUS2, FIFO_ENTRIES and FIFO_ENTRIES2 registers data
 *
 * @param dev - The device structure.
 * @param status1 - Data stored in the STATUS1 register
 * @param status2 - Data stored in the STATUS2 register
 * @param fifo_entries - Number of valid data samples present in the
 *			 FIFO buffer (0 to 512)
 * @return 0 in case of success, negative error code otherwise
 */
int adxl371_get_status(struct adxl371_dev *dev,
		       uint8_t *status1,
		       uint8_t *status2,
		       uint16_t *fifo_entries)
{
	uint8_t buf[4];
	int ret;

	ret = adxl371_read_reg_multiple(dev, ADXL371_STATUS_1, buf,
					NO_OS_ARRAY_SIZE(buf));
	if (ret < 0)
		return ret;

	*status1 = buf[0];
	*status2 = buf[1];
	*fifo_entries = ((buf[2] & 0x3) << 8) | buf[3];

	return ret;
}

/**
 * @brief Software reset the adxl device
 *
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int adxl371_reset(struct adxl371_dev *dev)
{
	int ret;

	ret = adxl371_set_op_mode(dev, ADXL371_STANDBY);
	if (ret < 0)
		return ret;

	/* Writing code 0x52 resets the device */
	ret = adxl371_write_reg(dev, ADXL371_RESET, ADXL371_RESET_CODE);
	if (ret < 0)
		return ret;

	no_os_mdelay(1);

	return ret;
}

/**
 * @brief Configure the operating parameters for the FIFO.
 *
 * @param dev - The device structure.
 * @param mode - FIFO Mode. Specifies FIFO operating mode.
 *		Accepted values: ADXL371_FIFO_BYPASSED
 *				 ADXL371_FIFO_STREAMED
 *				 ADXL371_FIFO_TRIGGERED
 *				 ADXL371_FIFO_OLD_SAVED
 * @param format - FIFO Format. Specifies which data is stored in the FIFO buffer.
 *		Accepted values: ADXL371_XYZ_FIFO
 *				 ADXL371_X_FIFO
 *				 ADXL371_Y_FIFO
 *				 ADXL371_XY_FIFO
 *				 ADXL371_Z_FIFO
 *				 ADXL371_XZ_FIFO
 *				 ADXL371_YZ_FIFO
 *				 ADXL371_XYZ_PEAK_FIFO
 * @param fifo_samples - FIFO Samples. Watermark number of FIFO samples that
 *			triggers a FIFO_FULL condition when reached.
 *			Values range from 0 to 512.

 * @return 0 in case of success, negative error code otherwise.
 */
int adxl371_configure_fifo(struct adxl371_dev *dev,
			   enum adxl371_fifo_mode mode,
			   enum adxl371_fifo_format format,
			   uint16_t fifo_samples,
			   enum adxl371_op_mode op_mode)
{
	uint8_t fifo_config;
	int ret;

	if (fifo_samples > 512)
		return -EINVAL;

	/*
	 * All FIFO modes must be configured while in standby mode.
	 */
	ret = adxl371_set_op_mode(dev, ADXL371_STANDBY);
	if (ret < 0)
		return ret;

	fifo_config = (ADXL371_FIFO_CTL_FORMAT_MODE(format) |
		       ADXL371_FIFO_CTL_MODE_MODE(mode) |
		       ADXL371_FIFO_CTL_SAMPLES_MODE(fifo_samples));

	ret = adxl371_write_reg(dev, ADXL371_FIFO_CTL, fifo_config);
	if (ret < 0)
		return ret;

	ret = adxl371_write_reg(dev, ADXL371_FIFO_SAMPLES,
				fifo_samples & 0xFF);
	if (ret)
		return ret;

	/* Reset operating mode */
	return adxl371_set_op_mode(dev, op_mode);
}

/**
 * @brief Set timing register to external sync setting
 *
 * @param dev adxl371 device data structure
 * @return int 0 if success, else negative error code
 */
int adxl371_set_external_sync(struct adxl371_dev *dev)
{
	return adxl371_write_reg(dev,
				 ADXL371_TIMING,
				 ADXL371_TIMING_EXT_SYNC_MODE(1) |
				 ADXL371_TIMING_ODR_MODE(dev->odr)
				);
}

/**
 * @brief Set timing register to internal sync setting
 *
 * @param dev adxl371 device data structure
 * @return int 0 if success, else negative error code
 */
int adxl371_set_internal_sync(struct adxl371_dev *dev)
{
	return adxl371_write_reg(dev,
				 ADXL371_TIMING,
				 ADXL371_TIMING_EXT_SYNC_MODE(0) |
				 ADXL371_TIMING_ODR_MODE(dev->odr)
				);
}

/**
 * @brief May be used for workaround to detect device state. Device should
 * have internal sync set prior to calling get_fifo_xyz_data_workaround
 *
 * @param dev - The device structure.
 * @param internal_sync - pointer to a boolean which will be set to
 *			true if the adxl371 internal sync is enabled, false otherwise.
 * @return 0 in case of success, negative error code otherwise.
 */
int adxl371_get_internal_sync(struct adxl371_dev *dev, bool *internal_sync)
{
	uint8_t timing_reg = 0x0;
	int ret = adxl371_read_reg(dev, ADXL371_TIMING, &timing_reg);
	if (ret < 0)
		return ret;

	*internal_sync = (!(timing_reg & 1));
	return 0;
}

/**
 * @brief Initialize the device.
 *
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 *		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int adxl371_init(struct adxl371_dev **device,
		 struct adxl371_init_param init_param)
{
	struct adxl371_dev	*dev;
	uint8_t dev_id, part_id, rev_id;
	int ret = 0;

	dev = (struct adxl371_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		goto error;

	dev->comm_type = init_param.comm_type;
	if (dev->comm_type == ADXL371_SPI) {
		/* SPI */
		ret = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);
		if (ret < 0)
			goto error;

		dev->reg_read = adxl371_spi_reg_read;
		dev->reg_write = adxl371_spi_reg_write;
		dev->reg_read_multiple = adxl371_spi_reg_read_multiple;
	} else { /* I2C */
		ret = no_os_i2c_init(&dev->i2c_desc, &init_param.i2c_init);
		if (ret < 0)
			goto error;

		dev->reg_read = adxl371_i2c_reg_read;
		dev->reg_write = adxl371_i2c_reg_write;
		dev->reg_read_multiple = adxl371_i2c_reg_read_multiple;

		ret = adxl371_read_reg(dev, ADXL371_REVID, &rev_id);
		if (ret < 0)
			goto error;
		/* Starting with the 3rd revision an I2C chip bug was fixed */
		if (rev_id < 3) {
			printf("I2C might not work properly with other "
			       "devices present on the bus\n");
		}
	}
	/* GPIO */
	ret = no_os_gpio_get(&dev->gpio_int1,
			     &init_param.gpio_int1);
	if (ret < 0)
		goto error;

	ret |= no_os_gpio_get(&dev->gpio_int2,
			      &init_param.gpio_int2);
	if (ret < 0)
		goto error;

	ret |= no_os_gpio_direction_input(dev->gpio_int1);
	if (ret < 0)
		goto error;

	ret |= no_os_gpio_direction_input(dev->gpio_int2);
	if (ret < 0)
		goto error;

	/* Query device presence */
	ret = adxl371_read_reg(dev, ADXL371_DEVID, &dev_id);
	if (ret < 0)
		goto error;

	ret = adxl371_read_reg(dev, ADXL371_PARTID, &part_id);
	if (ret < 0)
		goto error;

	if (dev_id != ADXL371_DEVID_VAL || part_id != ADXL371_PARTID_VAL) {
		printf("failed to read id (0x%X : 0x%X)\n", dev_id, part_id);
		ret = -ENODEV;
		goto error;
	}

	/* Device settings */
	ret = adxl371_set_op_mode(dev, ADXL371_STANDBY);
	if (ret < 0)
		goto error;
	ret |= adxl371_reset(dev);
	if (ret < 0)
		goto error;

	ret |= adxl371_set_bandwidth(dev, init_param.bw);
	if (ret < 0)
		goto error;

	ret |= adxl371_set_odr(dev, init_param.odr);
	if (ret < 0)
		goto error;

	ret |= adxl371_set_wakeup_rate(dev, init_param.wur);
	if (ret < 0)
		goto error;
	ret |= adxl371_set_act_proc_mode(dev, init_param.act_proc_mode);
	if (ret < 0)
		goto error;
	ret |= adxl371_set_instant_on_th(dev, init_param.th_mode);
	if (ret < 0)
		goto error;

	ret |= adxl371_set_activity_threshold(dev,
					      ADXL371_ACTIVITY,
					      init_param.activity_th.thresh,
					      init_param.activity_th.referenced,
					      init_param.activity_th.enable);
	if (ret < 0)
		goto error;
	ret |= adxl371_set_activity_threshold(dev,
					      ADXL371_ACTIVITY2,
					      init_param.activity2_th.thresh,
					      init_param.activity2_th.referenced,
					      init_param.activity2_th.enable);
	if (ret < 0)
		goto error;
	ret |= adxl371_set_activity_threshold(dev,
					      ADXL371_INACTIVITY,
					      init_param.inactivity_th.thresh,
					      init_param.inactivity_th.referenced,
					      init_param.inactivity_th.enable);
	if (ret < 0)
		goto error;
	ret |= adxl371_set_activity_time(dev, init_param.activity_time);
	if (ret < 0)
		goto error;
	ret |= adxl371_set_inactivity_time(dev, init_param.inactivity_time);
	if (ret < 0)
		goto error;
	ret |= adxl371_set_filter_settle(dev, init_param.filter_settle);
	if (ret < 0)
		goto error;

	/** NOTE: er001 workaround requires external sync here
	 *  Do not apply an external trigger signal on INT2 to keep the ADC off.
	*/
	ret |= adxl371_set_external_sync(dev);
	if (ret < 0)
		goto error;

	ret |= adxl371_configure_fifo(dev,
				      init_param.fifo_config.fifo_mode,
				      init_param.fifo_config.fifo_format,
				      init_param.fifo_config.fifo_samples,
				      init_param.op_mode);
	if (ret < 0)
		goto error;

	ret |= adxl371_interrupt_config(dev, init_param.int1_config,
					init_param.int2_config);
	if (ret < 0)
		goto error;

	ret |= adxl371_set_op_mode(dev, init_param.op_mode);
	if (ret < 0)
		goto error;

	*device = dev;
	printf("adxl371 successfully initialized\n");
	no_os_mdelay(1000);
	return ret;

error:
	printf("adxl371 initialization error (%d)\n", ret);
	no_os_free(dev);
	no_os_mdelay(1000);
	return ret;
}

/**
 * @brief Free heap data associated with device data structures
 *
 * @param dev ADXL371 device data structure
 * @return int 0 if success; else negative error code
 */
int adxl371_remove(struct adxl371_dev *dev)
{
	int ret;

	if (dev->comm_type == ADXL371_SPI) {
		ret = no_os_spi_remove(dev->spi_desc);
	} else {
		ret = no_os_i2c_remove(dev->i2c_desc);
	}
	if (ret < 0)
		return ret;

	ret |= no_os_gpio_remove(dev->gpio_int1);
	if (ret < 0)
		return ret;
	ret |= no_os_gpio_remove(dev->gpio_int2);
	if (ret < 0)
		return ret;

	no_os_free(dev);

	return ret;
}

/**
 * @brief Get the data stored in FIFO (accounting for FIFO misalignment)
 * NOTE: REQUIRES timing reg (0x3D) set to internal sync prior to calling
 *
 * @param dev - The device structure.
 * @param samples - pointer to the raw data stored in the ADXL371_FIFO_DATA
 * @param cnt - How many samples should be retrieved from the FIFO DATA reg
 * @return 0 in case of success, negative error code otherwise.
 */
int adxl371_get_fifo_xyz_data_workaround(struct adxl371_dev *dev,
		struct adxl371_xyz_accel_data *samples,
		uint16_t cnt)
{
	bool internal_sync = false;
	int ret;
	uint16_t i;

	if (cnt > 512)
		return -EINVAL;

	// @workaround must be in internal sync mode
	ret = adxl371_get_internal_sync(dev, &internal_sync);
	if (ret < 0)
		return ret;
	if (!internal_sync)
		return -EFAULT;

	enum adxl371_op_mode op_mode;
	ret = adxl371_get_op_mode(dev, &op_mode);
	if (ret < 0)
		return ret;

	// @workaround Set the timing register to external sync along with odr
	adxl371_set_external_sync(dev);

	// Read samples from FIFO (samples are 2 bytes wide)
	ret = adxl371_read_reg_multiple(dev,
					ADXL371_FIFO_DATA,
					dev->fifo_raw,
					cnt * 2);
	if (ret < 0)
		return ret;

	// Copy samples from buffer
	for (i = 0; i < cnt * 2; i += 6) {
		samples->x = ((dev->fifo_raw[i] << 4) | (dev->fifo_raw[i + 1] >> 4));
		samples->y = ((dev->fifo_raw[i + 2] << 4) | (dev->fifo_raw[i + 3] >> 4));
		samples->z = ((dev->fifo_raw[i + 4] << 4) | (dev->fifo_raw[i + 5] >> 4));
		samples++;
	}

	// @workaround clear the fifo (bypass mode)
	ret = adxl371_configure_fifo(dev, ADXL371_FIFO_BYPASSED,
				     dev->fifo_config.fifo_format,
				     dev->fifo_config.fifo_samples,
				     op_mode
				    );
	if (ret < 0)
		return ret;

	// @workaround set the desired fifo mode
	ret = adxl371_configure_fifo(dev, dev->fifo_config.fifo_mode,
				     dev->fifo_config.fifo_format,
				     dev->fifo_config.fifo_samples,
				     op_mode
				    );

	// @workaround set internal sync
	return adxl371_set_internal_sync(dev);
}

/**
 * @brief Retrieve data stored in FIFO. Can be used in polling mode,
 * but works best when interrupts are used
 *
 * @param dev - The device structure.
 * @param fifo_data - pointer to an array of type adxl371_xyz_accel_data
 *				where (x, y, z) values will be stored. Array max size
 *				should be 170, since FIFO maximum single-axis samples is 512.
 * @param fifo_entries - pointer which will store the number of valid data
 *				samples present in the FIFO buffer
 * @return 0 in case of success, negative error code otherwise
 */
int adxl371_service_fifo_ev(struct adxl371_dev *dev,
			    struct adxl371_xyz_accel_data *fifo_data,
			    uint16_t *fifo_entries)
{
	uint8_t status1, status2;
	int ret;

	ret = adxl371_get_status(dev, &status1, &status2, fifo_entries);
	if (ret)
		return ret;

	if (ADXL371_STATUS_1_FIFO_OVR(status1)) {
		printf("FIFO overrun\n");
		return -EFAULT;
	}

	if (dev->fifo_config.fifo_mode != ADXL371_FIFO_BYPASSED) {
		if ((ADXL371_STATUS_1_FIFO_RDY(status1)) ||
		    (ADXL371_STATUS_1_FIFO_FULL(status1))) {
			/*
			 * When reading data from multiple axes from the FIFO,
			 * to ensure that data is not overwritten and stored out
			 * of order, at least one sample set must be left in the
			 * FIFO after every read.
			 */
			*fifo_entries -= 3;
			ret = adxl371_get_fifo_xyz_data_workaround(dev, fifo_data,
					*fifo_entries);
			if (ret < 0)
				return ret;
		}
	}

	return ret;
}

/**
 * @brief Retrieve the highest magnitude (x, y, z) sample recorded since the last
 * read of the MAXPEAK registers
 *
 * @param dev - The device structure.
 * @param max_peak - pointer to a variable of type adxl371_xyz_accel_data
 *		      where (x, y, z) max values will be stored.
 * @return 0 in case of success, negative error code otherwise.
 */
int adxl371_get_highest_peak_data(struct adxl371_dev *dev,
				  struct adxl371_xyz_accel_data *max_peak)
{
	uint8_t buf[6];
	uint8_t status1, status2;
	uint16_t fifo_entries;
	int ret;

	do {
		ret = adxl371_get_status(dev, &status1,
					 &status2, &fifo_entries);
		if (ret < 0)
			return ret;
	} while (!(ADXL371_STATUS_1_DATA_RDY(status1)));

	ret = adxl371_read_reg_multiple(dev, ADXL371_X_MAXPEAK_H, buf,
					NO_OS_ARRAY_SIZE(buf));
	if (ret)
		return ret;

	max_peak->x = (buf[0] << 4) | (buf[1] >> 4);
	max_peak->y = (buf[2] << 4) | (buf[3] >> 4);
	max_peak->z = (buf[4] << 4) | (buf[5] >> 4);

	return ret;
}
