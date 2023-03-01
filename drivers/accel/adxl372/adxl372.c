/***************************************************************************//**
 *   @file   adxl372.c
 *   @brief  Implementation of adxl372 Core Driver.
 *   @author SPopa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
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
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "adxl372.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * Wrapper used to read device registers.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxl372_read_reg(struct adxl372_dev *dev,
			 uint8_t reg_addr,
			 uint8_t *reg_data)
{
	return dev->reg_read(dev, reg_addr, reg_data);
}

/**
 * Wrapper used to write to device registers.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxl372_write_reg(struct adxl372_dev *dev,
			  uint8_t reg_addr,
			  uint8_t reg_data)
{
	return dev->reg_write(dev, reg_addr, reg_data);
}

/**
 * Wrapper used of multibyte reads.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @param count - Number of bytes to read.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxl372_read_reg_multiple(struct adxl372_dev *dev,
				  uint8_t reg_addr,
				  uint8_t *reg_data,
				  uint16_t count)
{
	return dev->reg_read_multiple(dev, reg_addr, reg_data, count);
}

/**
 * Write to device using a mask.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - The mask.
 * @param data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxl372_write_mask(struct adxl372_dev *dev,
			   uint8_t reg_addr,
			   uint32_t mask,
			   uint8_t data)
{
	uint8_t reg_data;
	int32_t ret;

	ret = adxl372_read_reg(dev, reg_addr, &reg_data);
	if (ret < 0)
		return ret;

	reg_data &= ~mask;
	reg_data |= data;

	return adxl372_write_reg(dev, reg_addr, reg_data);
}

/**
 * Set the threshold for activity detection for all 3-axis
 * @param dev - The device structure.
 * @param act - Type of activity.
 *		Accepted values: ADXL372_ACTIVITY
 *				 ADXL372_ACTIVITY2
 *				 ADXL372_INACTIVITY
 * @param thresh - 11-bit unsigned value sets the threshold for activity,
 * 		   activity2 or inactivity detection.
 * @param referenced - Selects referenced or absolute activity processing.
 * @param enable - Enable activity detection using all 3-axis data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxl372_set_activity_threshold(struct adxl372_dev *dev,
				       enum adxl372_th_activity act,
				       uint16_t thresh,
				       bool referenced,
				       bool enable)
{
	uint8_t th_val_h, th_val_l, i;
	int32_t ret = 0;

	th_val_h = (thresh >> 3);
	th_val_l = (thresh << 5) | (referenced << 1) | enable;

	for (i = 0; i < 3; i++) {
		ret = adxl372_write_reg(dev,
					adxl372_th_reg_addr_h[act][i],
					th_val_h);
		if (ret < 0)
			return ret;

		ret = adxl372_write_reg(dev,
					adxl372_th_reg_addr_l[act][i],
					th_val_l);
		if (ret < 0)
			return ret;
	}

	return ret;
}

/**
 * Set the mode of operation.
 * @param dev - The device structure.
 * @param op_mode - Mode of operation.
 *		Accepted values: ADXL372_STANDBY
 *				 ADXL372_WAKE_UP
 *				 ADXL372_INSTANT_ON
 *				 ADXL372_FULL_BW_MEASUREMENT
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxl372_set_op_mode(struct adxl372_dev *dev,
			    enum adxl372_op_mode op_mode)
{
	return adxl372_write_mask(dev,
				  ADXL372_POWER_CTL,
				  ADXL372_POWER_CTL_MODE_MSK,
				  ADXL372_POWER_CTL_MODE(op_mode));
}

/**
 * Autosleep. When set to 1, autosleep is enabled, and the device enters
 * wake-up mode automatically upon detection of inactivity.
 * @param dev - The device structure.
 * @param enable - Accepted values: true
 *				    false
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxl372_set_autosleep(struct adxl372_dev *dev, bool enable)
{
	return adxl372_write_mask(dev,
				  ADXL372_MEASURE,
				  ADXL372_MEASURE_AUTOSLEEP_MSK,
				  ADXL372_MEASURE_AUTOSLEEP_MODE(enable));
}

/**
 * Select the desired output signal bandwidth.
 * @param dev - The device structure.
 * @param bw - bandwidth.
 *		Accepted values: ADXL372_BW_200HZ
 *				 ADXL372_BW_400HZ
 *				 ADXL372_BW_800HZ
 *				 ADXL372_BW_1600HZ
 *				 ADXL372_BW_3200HZ
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxl372_set_bandwidth(struct adxl372_dev *dev,
			      enum adxl372_bandwidth bw)
{
	int32_t ret;

	ret = adxl372_write_mask(dev,
				 ADXL372_MEASURE,
				 ADXL372_MEASURE_BANDWIDTH_MSK,
				 ADXL372_MEASURE_BANDWIDTH_MODE(bw));
	if (ret < 0)
		return ret;

	dev->bw = bw;

	return ret;
}

/**
 * Link/Loop Activity Processing.
 * @param dev - The device structure.
 * @param mode - Mode of operation.
 *		Accepted values: ADXL372_DEFAULT
 *				 ADXL372_LINKED
 *				 ADXL372_LOOPED
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxl372_set_act_proc_mode(struct adxl372_dev *dev,
				  enum adxl372_act_proc_mode mode)
{
	int32_t ret;

	ret = adxl372_write_mask(dev,
				 ADXL372_MEASURE,
				 ADXL372_MEASURE_LINKLOOP_MSK,
				 ADXL372_MEASURE_LINKLOOP_MODE(mode));
	if (ret < 0)
		return ret;

	dev->act_proc_mode = mode;

	return ret;
}

/**
 * Set Output data rate.
 * @param dev - The device structure.
 * @param odr - Output data rate.
 *		Accepted values: ADXL372_ODR_400HZ
 *				 ADXL372_ODR_800HZ
 *				 ADXL372_ODR_1600HZ
 *				 ADXL372_ODR_3200HZ
 *				 ADXL372_ODR_6400HZ
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxl372_set_odr(struct adxl372_dev *dev,
			enum adxl372_odr odr)
{
	int32_t ret;

	ret = adxl372_write_mask(dev,
				 ADXL372_TIMING,
				 ADXL372_TIMING_ODR_MSK,
				 ADXL372_TIMING_ODR_MODE(odr));
	if (ret < 0)
		return ret;

	dev->odr = odr;

	return ret;
}

/**
 * Select instant on threshold
 * @param dev - The device structure.
 * @param mode - 0 = low threshold, 1 = high threshold.
 *		Accepted values: ADXL372_INSTANT_ON_LOW_TH
 *				 ADXL372_INSTANT_ON_HIGH_TH
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxl372_set_instant_on_th(struct adxl372_dev *dev,
				  enum adxl372_instant_on_th_mode mode)
{
	int32_t ret;

	ret = adxl372_write_mask(dev,
				 ADXL372_POWER_CTL,
				 ADXL372_POWER_CTL_INSTANT_ON_TH_MSK,
				 ADXL372_POWER_CTL_INSTANT_ON_TH_MODE(mode));
	if (ret < 0)
		return ret;

	dev->th_mode = mode;

	return ret;
}

/**
 * Set the Timer Rate for Wake-Up Mode.
 * @param dev - The device structure.
 * @param wur - wake up mode rate
 *		Accepted values: ADXL372_WUR_52ms
 *				 ADXL372_WUR_104ms
 *				 ADXL372_WUR_208ms
 *				 ADXL372_WUR_512ms
 *				 ADXL372_WUR_2048ms
 *				 ADXL372_WUR_4096ms
 *				 ADXL372_WUR_8192ms
 *				 ADXL372_WUR_24576ms
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxl372_set_wakeup_rate(struct adxl372_dev *dev,
				enum adxl372_wakeup_rate wur)
{
	int32_t ret;

	ret = adxl372_write_mask(dev,
				 ADXL372_TIMING,
				 ADXL372_TIMING_WAKE_UP_RATE_MSK,
				 ADXL372_TIMING_WAKE_UP_RATE_MODE(wur));
	if (ret < 0)
		return ret;

	dev->wur = wur;

	return ret;
}

/**
 * Set the activity timer
 * @param dev - The device structure.
 * @param time - The value set in this register.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxl372_set_activity_time(struct adxl372_dev *dev, uint8_t time)
{
	return adxl372_write_reg(dev, ADXL372_TIME_ACT, time);
}

/**
 * Set the inactivity timer
 * @param dev - The device structure.
 * @param time - is the 16-bit value set by the TIME_INACT_L register
 * 		(eight LSBs) and the TIME_INACT_H register (eight MSBs).
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxl372_set_inactivity_time(struct adxl372_dev *dev, uint16_t time)
{
	int32_t ret;

	ret = adxl372_write_reg(dev, ADXL372_TIME_INACT_H, time >> 8);
	if (ret < 0)
		return ret;

	return adxl372_write_reg(dev, ADXL372_TIME_INACT_L, time & 0xFF);
}

/**
 * Set the filter settling period.
 * @param dev - The device structure.
 * @param mode - settle period
 *		Accepted values: ADXL372_FILTER_SETTLE_370
 *				 ADXL372_FILTER_SETTLE_16
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxl372_set_filter_settle(struct adxl372_dev *dev,
				  enum adxl372_filter_settle mode)
{
	return adxl372_write_mask(dev,
				  ADXL372_POWER_CTL,
				  ADXL372_POWER_CTL_FIL_SETTLE_MSK,
				  ADXL372_POWER_CTL_FIL_SETTLE_MODE(mode));
}

/**
 * Configure the INT1 and INT2 interrupt pins.
 * @param dev - The device structure.
 * @param int1 -  INT1 interrupt pins.
 * @param int2 -  INT2 interrupt pins.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxl372_interrupt_config(struct adxl372_dev *dev,
				 struct adxl372_irq_config int1,
				 struct adxl372_irq_config int2)
{
	uint8_t int1_config, int2_config;
	int32_t ret;

	int1_config = (ADXL372_INT1_MAP_DATA_RDY_MODE(int1.data_rdy) |
		       ADXL372_INT1_MAP_FIFO_RDY_MODE(int1.fifo_rdy) |
		       ADXL372_INT1_MAP_FIFO_FULL_MODE(int1.fifo_full) |
		       ADXL372_INT1_MAP_FIFO_OVR_MODE(int1.fifo_ovr) |
		       ADXL372_INT1_MAP_INACT_MODE(int1.inactivity) |
		       ADXL372_INT1_MAP_ACT_MODE(int1.activity) |
		       ADXL372_INT1_MAP_AWAKE_MODE(int1.awake) |
		       ADXL372_INT1_MAP_LOW_MODE(int1.low_operation));

	int2_config = (ADXL372_INT2_MAP_DATA_RDY_MODE(int2.data_rdy) |
		       ADXL372_INT2_MAP_FIFO_RDY_MODE(int2.fifo_rdy) |
		       ADXL372_INT2_MAP_FIFO_FULL_MODE(int2.fifo_full) |
		       ADXL372_INT2_MAP_FIFO_OVR_MODE(int1.fifo_ovr) |
		       ADXL372_INT2_MAP_INACT_MODE(int2.inactivity) |
		       ADXL372_INT2_MAP_ACT_MODE(int2.activity) |
		       ADXL372_INT2_MAP_AWAKE_MODE(int2.awake) |
		       ADXL372_INT2_MAP_LOW_MODE(int2.low_operation));

	ret = adxl372_write_reg(dev, ADXL372_INT1_MAP, int1_config);
	if (ret < 0)
		return ret;

	return adxl372_write_reg(dev, ADXL372_INT2_MAP, int2_config);
}

/**
 * Get the STATUS, STATUS2, FIFO_ENTRIES and FIFO_ENTRIES2 registers data
 * @param dev - The device structure.
 * @param status1 - Data stored in the STATUS1 register
 * @param status2 - Data stored in the STATUS2 register
 * @param fifo_entries - Number of valid data samples present in the
 *			 FIFO buffer (0 to 512)
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxl372_get_status(struct adxl372_dev *dev,
			   uint8_t *status1,
			   uint8_t *status2,
			   uint16_t *fifo_entries)
{
	uint8_t buf[4];
	int32_t ret;

	ret = adxl372_read_reg_multiple(dev, ADXL372_STATUS_1, buf,
					NO_OS_ARRAY_SIZE(buf));
	if (ret < 0)
		return ret;

	*status1 = buf[0];
	*status2 = buf[1];
	*fifo_entries = ((buf[2] & 0x3) << 8) | buf[3];

	return ret;
}

/**
 * Software reset.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxl372_reset(struct adxl372_dev *dev)
{
	int32_t ret;

	ret = adxl372_set_op_mode(dev, ADXL372_STANDBY);
	if (ret < 0)
		return ret;

	/* Writing code 0x52 resets the device */
	ret = adxl372_write_reg(dev, ADXL372_RESET, ADXL372_RESET_CODE);
	if (ret < 0)
		return ret;

	no_os_mdelay(1);

	return ret;
}

/**
 * Configure the operating parameters for the FIFO.
 * @param dev - The device structure.
 * @param mode - FIFO Mode. Specifies FIFO operating mode.
 *		Accepted values: ADXL372_FIFO_BYPASSED
 *				 ADXL372_FIFO_STREAMED
 *				 ADXL372_FIFO_TRIGGERED
 *				 ADXL372_FIFO_OLD_SAVED
 * @param format - FIFO Format. Specifies which data is stored in the FIFO buffer.
 *		Accepted values: ADXL372_XYZ_FIFO
 *				 ADXL372_X_FIFO
 *				 ADXL372_Y_FIFO
 *				 ADXL372_XY_FIFO
 *				 ADXL372_Z_FIFO
 *				 ADXL372_XZ_FIFO
 *				 ADXL372_YZ_FIFO
 *				 ADXL372_XYZ_PEAK_FIFO
 * @param fifo_samples - FIFO Samples. Watermark number of FIFO samples that
 *			triggers a FIFO_FULL condition when reached.
 *			Values range from 0 to 512.

 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxl372_configure_fifo(struct adxl372_dev *dev,
			       enum adxl372_fifo_mode mode,
			       enum adxl372_fifo_format format,
			       uint16_t fifo_samples)
{
	uint8_t fifo_config;
	int32_t ret;

	if (fifo_samples > 512)
		return -1;

	/*
	 * All FIFO modes must be configured while in standby mode.
	 */
	ret = adxl372_set_op_mode(dev, ADXL372_STANDBY);
	if (ret < 0)
		return ret;

	fifo_config = (ADXL372_FIFO_CTL_FORMAT_MODE(format) |
		       ADXL372_FIFO_CTL_MODE_MODE(mode) |
		       ADXL372_FIFO_CTL_SAMPLES_MODE(fifo_samples));

	ret = adxl372_write_reg(dev, ADXL372_FIFO_CTL, fifo_config);
	if (ret < 0)
		return ret;

	ret = adxl372_write_reg(dev, ADXL372_FIFO_SAMPLES,
				fifo_samples & 0xFF);
	if (ret)
		return ret;

	dev->fifo_config.fifo_format = format;
	dev->fifo_config.fifo_mode = mode;
	dev->fifo_config.fifo_samples = fifo_samples;

	return ret;
}

/**
 * Retrieve data stored in FIFO. Can be used in polling mode,
 * but works best when interrupts are used
 * @param dev - The device structure.
 * @param fifo_data - pointer to an array of type adxl372_xyz_accel_data
 *		      where (x, y, z) values will be stored. Array max size
 *		      should be 170, as the FIFO can hold up to 512 samples.
 * @param fifo_entries - pointer which will store the number of valid data
 *			 samples present in the FIFO buffer
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxl372_service_fifo_ev(struct adxl372_dev *dev,
				struct adxl372_xyz_accel_data *fifo_data,
				uint16_t *fifo_entries)
{
	uint8_t status1, status2;
	int32_t ret;

	ret = adxl372_get_status(dev, &status1, &status2, fifo_entries);
	if (ret)
		return ret;

	if (ADXL372_STATUS_1_FIFO_OVR(status1)) {
		printf("FIFO overrun\n");
		return -1;
	}

	if (dev->fifo_config.fifo_mode != ADXL372_FIFO_BYPASSED) {
		if ((ADXL372_STATUS_1_FIFO_RDY(status1)) ||
		    (ADXL372_STATUS_1_FIFO_FULL(status1))) {
			/*
			 * When reading data from multiple axes from the FIFO,
			 * to ensure that data is not overwritten and stored out
			 * of order, at least one sample set must be left in the
			 * FIFO after every read.
			 */
			*fifo_entries -= 3;
			ret = adxl372_get_fifo_xyz_data(dev, fifo_data,
							*fifo_entries);
			if (ret < 0)
				return ret;
		}
	}

	return ret;
}

/**
 * Get the data stored in FIFO.
 * @param dev - The device structure.
 * @param samples - pointer to the raw data stored in the ADXL372_FIFO_DATA
 * @param cnt - How many samples should be retrieved from the FIFO DATA reg
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxl372_get_fifo_xyz_data(struct adxl372_dev *dev,
				  struct adxl372_xyz_accel_data *samples,
				  uint16_t cnt)
{
	uint8_t buf[1024];
	uint16_t i;
	int32_t ret;


	if (cnt > 512)
		return -1;
	/*
	 * The FIFO can hold up to 512 samples.
	 * Each sample is 2 bytes, that's why we read (cnt * 2) bytes
	 */
	ret = adxl372_read_reg_multiple(dev,
					ADXL372_FIFO_DATA,
					buf,
					cnt * 2);
	if (ret < 0)
		return ret;

	for (i = 0; i < cnt * 2; i += 6) {
		samples->x = ((buf[i] << 4) | (buf[i+1] >> 4));
		samples->y = ((buf[i+2] << 4) | (buf[i+3] >> 4));
		samples->z = ((buf[i+4] << 4) | (buf[i+5] >> 4));
		samples++;
	}

	return ret;
}

/**
 * Retrieve the highest magnitude (x, y, z) sample recorded since the last
 * read of the MAXPEAK registers
 * @param dev - The device structure.
 * @param max_peak - pointer to a variable of type adxl372_xyz_accel_data
 *		      where (x, y, z) max values will be stored.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxl372_get_highest_peak_data(struct adxl372_dev *dev,
				      struct adxl372_xyz_accel_data *max_peak)
{
	uint8_t buf[6];
	uint8_t status1, status2;
	uint16_t fifo_entries;
	int32_t ret;

	do {
		ret = adxl372_get_status(dev, &status1,
					 &status2, &fifo_entries);
		if (ret < 0)
			return ret;
	} while(!(ADXL372_STATUS_1_DATA_RDY(status1)));

	ret = adxl372_read_reg_multiple(dev, ADXL372_X_MAXPEAK_H, buf,
					NO_OS_ARRAY_SIZE(buf));
	if (ret)
		return ret;

	max_peak->x = (buf[0] << 4) | (buf[1] >> 4);
	max_peak->y = (buf[2] << 4) | (buf[3] >> 4);
	max_peak->z = (buf[4] << 4) | (buf[5] >> 4);

	return ret;
}

/**
 * Retrieve 3-axis acceleration data
 * @param dev - The device structure.
 * @param accel_data - pointer to a variable of type adxl372_xyz_accel_data
 *		      where (x, y, z) acceleration data will be stored.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxl372_get_accel_data(struct adxl372_dev *dev,
			       struct adxl372_xyz_accel_data *accel_data)
{
	uint8_t buf[6];
	uint8_t status1, status2;
	uint16_t fifo_entries;
	int32_t ret;

	do {
		ret = adxl372_get_status(dev, &status1,
					 &status2, &fifo_entries);
		if (ret < 0)
			return ret;
	} while(!(ADXL372_STATUS_1_DATA_RDY(status1)));

	ret = adxl372_read_reg_multiple(dev,
					ADXL372_X_DATA_H,
					buf, NO_OS_ARRAY_SIZE(buf));
	if (ret)
		return ret;

	accel_data->x = (buf[0] << 4) | (buf[1] >> 4);
	accel_data->y = (buf[2] << 4) | (buf[3] >> 4);
	accel_data->z = (buf[4] << 4) | (buf[5] >> 4);

	return ret;
}

/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 *		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adxl372_init(struct adxl372_dev **device,
		     struct adxl372_init_param init_param)
{
	struct adxl372_dev	*dev;
	uint8_t dev_id, part_id, rev_id;
	int32_t ret;

	dev = (struct adxl372_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		goto error;

	dev->comm_type = init_param.comm_type;
	if (dev->comm_type == SPI) {
		/* SPI */
		ret = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);
		if (ret < 0)
			goto error;

		dev->reg_read = adxl372_spi_reg_read;
		dev->reg_write = adxl372_spi_reg_write;
		dev->reg_read_multiple = adxl372_spi_reg_read_multiple;
	} else { /* I2C */
		ret = no_os_i2c_init(&dev->i2c_desc, &init_param.i2c_init);
		if (ret < 0)
			goto error;

		dev->reg_read = adxl372_i2c_reg_read;
		dev->reg_write = adxl372_i2c_reg_write;
		dev->reg_read_multiple = adxl372_i2c_reg_read_multiple;

		ret = adxl372_read_reg(dev, ADXL372_REVID, &rev_id);
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
	ret = adxl372_read_reg(dev, ADXL372_DEVID, &dev_id);
	if (ret < 0)
		goto error;

	ret = adxl372_read_reg(dev, ADXL372_PARTID, &part_id);
	if (ret < 0)
		goto error;

	if (dev_id != ADXL372_DEVID_VAL || part_id != ADXL372_PARTID_VAL) {
		printf("failed to read id (0x%X : 0x%X)\n", dev_id, part_id);
		goto error;
	}

	/* Device settings */
	ret = adxl372_set_op_mode(dev, ADXL372_STANDBY);
	ret |= adxl372_reset(dev);
	ret |= adxl372_set_bandwidth(dev, init_param.bw);
	ret |= adxl372_set_odr(dev, init_param.odr);
	ret |= adxl372_set_wakeup_rate(dev, init_param.wur);
	ret |= adxl372_set_act_proc_mode(dev, init_param.act_proc_mode);
	ret |= adxl372_set_instant_on_th(dev, init_param.th_mode);
	ret |= adxl372_set_activity_threshold(dev,
					      ADXL372_ACTIVITY,
					      init_param.activity_th.thresh,
					      init_param.activity_th.referenced,
					      init_param.activity_th.enable);
	ret |= adxl372_set_activity_threshold(dev,
					      ADXL372_ACTIVITY2,
					      init_param.activity2_th.thresh,
					      init_param.activity2_th.referenced,
					      init_param.activity2_th.enable);
	ret |= adxl372_set_activity_threshold(dev,
					      ADXL372_INACTIVITY,
					      init_param.inactivity_th.thresh,
					      init_param.inactivity_th.referenced,
					      init_param.inactivity_th.enable);
	ret |= adxl372_set_activity_time(dev, init_param.activity_time);
	ret |= adxl372_set_inactivity_time(dev, init_param.inactivity_time);
	ret |= adxl372_set_filter_settle(dev, init_param.filter_settle);
	ret |= adxl372_configure_fifo(dev,
				      init_param.fifo_config.fifo_mode,
				      init_param.fifo_config.fifo_format,
				      init_param.fifo_config.fifo_samples);

	ret |= adxl372_interrupt_config(dev, init_param.int1_config,
					init_param.int2_config);

	ret |= adxl372_set_op_mode(dev, init_param.op_mode);

	if (!ret) {
		*device = dev;
		printf("adxl372 successfully initialized\n");
		no_os_mdelay(1000);
		return ret;
	}
error:
	printf("adxl372 initialization error (%d)\n", ret);
	no_os_free(dev);
	no_os_mdelay(1000);
	return ret;
}
