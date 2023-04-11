/*******************************************************************************
 *   @file   adxl313.c
 *   @brief  Implementation of ADXL313 Driver.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include "adxl313.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
static int64_t adxl313_accel_conv(struct adxl313_dev *dev, int16_t raw_accel);
static void adxl313_compute_multiplier(struct adxl313_dev *dev);

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/*******************************************************************************
 * @brief Read data from the device.
 *
 * @param dev          - The device structure.
 * @param base_address - Address of the base register.
 * @param size         - The number of bytes to be read and returned in read_data.
 * @param read_data    - The read data buffer.
 *
 * @return ret         - Result of the reading operation (0 - success, negative
 *                       value for failure).
*******************************************************************************/
int adxl313_read(struct adxl313_dev *dev, uint8_t base_address,
		 uint16_t size, uint8_t *read_data)
{
	int ret;

	if (dev->comm_type == ADXL313_SPI_COMM) {
		dev->comm_buff[0] = ADXL313_SPI_READ | base_address;
		if (size > 1)
			dev->comm_buff[0] |= ADXL313_MULTIBIT;
		ret = no_os_spi_write_and_read(dev->com_desc.spi_desc, dev->comm_buff,
					       1 + size);
		for (uint16_t idx = 0; idx < size; idx++)
			read_data[idx] = dev->comm_buff[idx+1];
	} else {
		ret = no_os_i2c_write(dev->com_desc.i2c_desc, &base_address, 1, 0);
		if (ret)
			return ret;
		ret = no_os_i2c_read(dev->com_desc.i2c_desc, read_data, size, 1);
	}

	return ret;
}

/*******************************************************************************
 * @brief Write data to the device.
 *
 * @param dev          - The device structure.
 * @param base_address - Address of the base register.
 * @param size         - The number of bytes to be written. It is the size
 *                       of the write_data buffer.
 * @param write_data   - The data which is going to be written.
 *
 * @return ret         - Result of the writing operation (0 - success, negative
 *                       value for failure).
*******************************************************************************/
int adxl313_write(struct adxl313_dev *dev, uint8_t base_address,
		  uint16_t size, uint8_t *write_data)
{
	int ret;

	for (uint16_t idx = 0; idx < size; idx++)
		dev->comm_buff[1+idx] = write_data[idx];

	if (dev->comm_type == ADXL313_SPI_COMM) {
		dev->comm_buff[0] = ADXL313_SPI_WRITE | base_address;
		if (size > 1)
			dev->comm_buff[0] |= ADXL313_MULTIBIT;
		ret = no_os_spi_write_and_read(dev->com_desc.spi_desc, dev->comm_buff,
					       size + 1);
	} else {
		dev->comm_buff[0] = base_address;
		ret = no_os_i2c_write(dev->com_desc.i2c_desc, dev->comm_buff, size + 1, 1);
	}

	return ret;
}

/*******************************************************************************
 * @brief Perform a masked write to a register.
 *
 * @param dev      - The device structure.
 * @param reg_addr - The register address.
 * @param data     - The register data.
 * @param mask     - The mask.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_reg_write_msk(struct adxl313_dev *dev,
			  uint8_t reg_addr,
			  uint8_t data,
			  uint8_t mask)
{
	int ret;
	uint8_t reg_data = 0x00;

	ret = adxl313_read(dev, reg_addr, 1, &reg_data);
	if (ret)
		return ret;

	reg_data &= ~mask;
	reg_data |= data;

	return adxl313_write(dev, reg_addr, 1, &reg_data);
}

/*******************************************************************************
 * @brief Initialize the communication peripheral and check if the ADXL313
 *        part is present.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       		   parameters.
 *
 * @return ret       - Result of the initialization - 0 in case of success,
 *                     negative error code otherwise.
*******************************************************************************/
int adxl313_init(struct adxl313_dev **device,
		 struct adxl313_init_param init_param)
{
	struct adxl313_dev *dev;
	int ret;
	uint8_t reg_value = 0;


	dev = (struct adxl313_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->comm_type = init_param.comm_type;

	if (dev->comm_type == ADXL313_SPI_COMM)
		ret = no_os_spi_init(&dev->com_desc.spi_desc, &(init_param.comm_init.spi_init));
	else
		ret = no_os_i2c_init(&dev->com_desc.i2c_desc, &init_param.comm_init.i2c_init);

	if (ret)
		goto error_dev;

	/* Check for device ID. */
	ret = adxl313_read(dev, ADXL313_REG_DEVID_AD, 1, &reg_value);
	if (ret)
		goto error_com;

	/* Check if device ID matches ADXL312/4 ID in case of ADXL312/4. */
	if ((init_param.dev_type == ID_ADXL312)
	    || (init_param.dev_type == ID_ADXL314)) {
		if (reg_value == ADXL314_DEVID) {
			dev->dev_type = init_param.dev_type;
		} else {
			goto error_dev;
		}
	}

	/* Check if device ID matches ADXL313 ID in case of ADXL313. */
	if (init_param.dev_type == ID_ADXL313) {
		if (reg_value == ADXL313_DEVID) {
			ret = adxl313_read(dev, ADXL313_REG_DEVID1_AD, 1, &reg_value);
			if ((ret) || (reg_value != ADXL313_DEVID1))
				goto error_dev;
			ret = adxl313_read(dev, ADXL313_REG_PARTID, 1, &reg_value);
			if ((ret) || (reg_value != ADXL313_PARTID))
				goto error_dev;
			dev->dev_type = init_param.dev_type;
		} else {
			goto error_dev;
		}
	}

	/* Set operation mode to Standby. */
	ret = adxl313_set_op_mode(dev, ADXL313_STDBY);
	if (ret)
		goto error_com;

	/* Get the device range */
	if (dev->dev_type == ID_ADXL314) {
		dev->range = ADXL313_200G_RANGE;
		dev->resolution = ADXL313_13_BIT_RES;
	} else {
		ret = adxl313_read(dev, ADXL313_REG_DATA_FORMAT, 1, &reg_value);
		if (ret)
			goto error_com;

		if (dev->dev_type == ID_ADXL312) {
			/* ADXL312 */
			dev->range = (reg_value & ADXL313_REG_DATA_FORMAT_RANGE) + ADXL313_RANGE_FACTOR;
			/* Compute resolution for ADXL312 part depending on FULL_RES bit
			 * and range. */
			if (reg_value & ADXL313_REG_DATA_FORMAT_FULL_RES) {
				dev->resolution = dev->range - ADXL313_RANGE_FACTOR;
			} else {
				dev->resolution = ADXL313_10_BIT_RES;
			}
		} else {
			/* ADXL313 */
			dev->range = reg_value & ADXL313_REG_DATA_FORMAT_RANGE;

			/* Compute resolution for ADXL313 part depending on FULL_RES bit
			 * and range. */
			if (reg_value & ADXL313_REG_DATA_FORMAT_FULL_RES) {
				dev->resolution = dev->range;
			} else {
				dev->resolution = ADXL313_10_BIT_RES;
			}
		}
	}

	/* Get Low Power mode status. */
	ret = adxl313_read(dev, ADXL313_REG_BW_RATE, 1, &reg_value);
	if (ret)
		goto error_com;

	if (reg_value & ADXL313_LOW_POWER_OP)
		dev->lp_mode = ADXL313_LP_MODE_ON;
	else
		dev->lp_mode = ADXL313_LP_MODE_NONE;

	/* Get ODR. */
	ret = adxl313_read(dev, ADXL313_REG_BW_RATE, 1, &reg_value);
	if (ret)
		goto error_com;
	dev->odr = (reg_value & ADXL313_RATE_MSK) - ADXL313_ODR_OFFSET_VAL;

	/* Get offsets. */
	ret = adxl313_read(dev, ADXL313_REG_OFS_AXIS(ADXL313_X_AXIS), 1,
			   &reg_value);
	if (ret)
		goto error_com;
	dev->x_offset_raw = reg_value;
	ret = adxl313_read(dev, ADXL313_REG_OFS_AXIS(ADXL313_Y_AXIS), 1,
			   &reg_value);
	if (ret)
		goto error_com;
	dev->y_offset_raw = reg_value;
	ret = adxl313_read(dev, ADXL313_REG_OFS_AXIS(ADXL313_Z_AXIS), 1,
			   &reg_value);
	if (ret)
		goto error_com;
	dev->z_offset_raw = reg_value;

	/* Get FIFO mode setting. */
	ret = adxl313_read(dev, ADXL313_REG_FIFO_CTL, 1, &reg_value);
	if (ret)
		goto error_com;
	dev->fifo_mode = (reg_value & ADXL313_REG_FIFO_CTL_MODE_MSK) >> 6;

	/* Get FIFO samples setting. */
	ret = adxl313_read(dev, ADXL313_REG_FIFO_CTL, 1, &reg_value);
	if (ret)
		goto error_com;
	dev->fifo_samples = (reg_value & ADXL313_REG_FIFO_CTL_SAMPLES_MSK);

	/* Get dev. threshold for detecting activity. */
	ret = adxl313_read(dev, ADXL313_REG_THRESH_ACT, 1, &reg_value);
	if (ret)
		goto error_com;
	dev->act_thr = reg_value;

	/* Get dev. threshold for detecting inactivity. */
	ret = adxl313_read(dev, ADXL313_REG_THRESH_INACT, 1, &reg_value);
	if (ret)
		goto error_com;
	dev->inact_thr = reg_value;

	/* Get dev. time threshold for detecting inactivity. */
	ret = adxl313_read(dev, ADXL313_REG_TIME_INACT, 1, &reg_value);
	if (ret)
		goto error_com;
	dev->time_inact = reg_value;

	/* Get dev. ACT_INACT_CTL register values. */
	ret = adxl313_read(dev, ADXL313_REG_ACT_INACT_CTL, 1, &reg_value);
	if (ret)
		goto error_com;
	dev->act_inact_ctl.value = reg_value;

	/* Put device in Measure mode. */
	ret = adxl313_set_op_mode(dev, ADXL313_MEAS);
	if (ret)
		goto error_com;

	/* Compute and store multiplier for data conversion depending on
	 * range and resolution. */
	adxl313_compute_multiplier(dev);

	*device = dev;

	return ret;
error_com:
	if (dev->comm_type == ADXL313_SPI_COMM)
		no_os_spi_remove(dev->com_desc.spi_desc);
	else
		no_os_i2c_remove(dev->com_desc.i2c_desc);
	no_os_free(dev);
	return -EPIPE;
error_dev:
	no_os_free(dev);
	return -ENODEV;
}

/*******************************************************************************
 * @brief Free the resources allocated by adxl313_init().
 *
 * @param dev - The device structure.
 *
 * @return ret - Result of the remove action (0 - success, negative val. - failure).
*******************************************************************************/
int adxl313_remove(struct adxl313_dev *dev)
{
	int ret;

	if (dev->comm_type == ADXL313_SPI_COMM)
		ret = no_os_spi_remove(dev->com_desc.spi_desc);
	else
		ret = no_os_i2c_remove(dev->com_desc.i2c_desc);

	if (!ret)
		no_os_free(dev);

	return ret;
}

/*******************************************************************************
 * @brief Reset the device via communication
 *
 * @param dev     - The device structure.
 *
 * @return ret    - Result of the reset operation (0 - success,
 *                  negative value - failure).
*******************************************************************************/
int adxl313_software_reset(struct adxl313_dev *dev)
{
	int ret;
	uint8_t reset_cmd = ADXL313_RESET_KEY;

	if (dev->dev_type == ID_ADXL314)
		return -EACCES;

	ret = adxl313_write(dev, ADXL313_REG_SOFT_RESET, 1, &reset_cmd);

	if (!ret) {
		/* Reset defaults */
		dev->range = ADXL313_0_5G_RANGE;
		dev->resolution = ADXL313_10_BIT_RES;
		dev->op_mode = ADXL313_STDBY;
		dev->lp_mode = ADXL313_LP_MODE_NONE;
		dev->odr = ADXL313_ODR_100HZ;
		dev->x_offset_raw = 0;
		dev->y_offset_raw = 0;
		dev->z_offset_raw = 0;
		dev->fifo_mode = ADXL313_BYPAS_MODE;
		dev->fifo_samples = 0;
		dev->act_thr = 0;
		dev->inact_thr = 0;
		dev->time_inact = 0;
		dev->act_inact_ctl.value = 0;

		/* Compute and store multiplier for data conversion depending on
		 * range and resolution. */
		adxl313_compute_multiplier(dev);
	}

	return ret;
}

/*******************************************************************************
 * @brief Place the device into the given operation mode.
 *
 * @param dev     - The device structure.
 * @param op_mode - Operation mode mode (Standby or Measure).
 *
 * @return ret    - Result of the mode setting operation (0 - success,
 *                  negative value - failure).
*******************************************************************************/
int adxl313_set_op_mode(struct adxl313_dev *dev, enum adxl313_op_mode op_mode)
{
	int ret;
	uint8_t mask = ADXL313_POWER_CTL_MEASURE;
	uint8_t data = ENABLE_E;

	/* Set operation mode. */
	switch(op_mode) {
	case ADXL313_STDBY:
		data = DISABLE_E;
		break;
	case ADXL313_MEAS:
		break;
	default:
		pr_err("ADXL313 operation mode not supported!\n");
		return -ENOSYS;
	}

	ret = adxl313_reg_write_msk(dev,
				    ADXL313_REG_POWER_CTL,
				    no_os_field_prep(mask, data),
				    mask);

	if (!ret)
		dev->op_mode = op_mode;

	return ret;
}

/*******************************************************************************
 * @brief Get the current operation mode of the device.
 *
 * @param dev     - The device structure.
 * @param op_mode - Read operation mode.
 *
 * @return ret    - Result of the operation procedure (0 - success, negative
 *                  value - failure).
*******************************************************************************/
int adxl313_get_op_mode(struct adxl313_dev *dev,
			enum adxl313_op_mode *op_mode)
{
	int ret;
	uint8_t reg_value = 0x00;

	ret = adxl313_read(dev, ADXL313_REG_POWER_CTL, 1, &reg_value);
	if (ret)
		return ret;

	if (reg_value & ADXL313_POWER_CTL_MEASURE)
		*op_mode = ADXL313_MEAS;
	else
		*op_mode = ADXL313_STDBY;

	return ret;
}

/*******************************************************************************
 * @brief Set offset for each axis.
 *
 * @param dev       - The device structure.
 * @param offset_ug - Offset in g / 1000 000 (ug).
 * @param axis      - Axis to apply offset.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_set_offset(struct adxl313_dev *dev, int32_t offset_ug,
		       enum adxl313_axis axis)
{
	int32_t offset_val = 0x00;

	if (axis > ADXL313_Z_AXIS)
		return -EINVAL;

	switch (dev->dev_type) {
	case ID_ADXL312:
		offset_val = offset_ug / ADXL312_OFFSET_SCALE_FACTOR;
		break;
	case ID_ADXL313:
		offset_val = offset_ug / ADXL313_OFFSET_SCALE_FACTOR;
		break;
	case ID_ADXL314:
		offset_val =  offset_ug / ADXL314_OFFSET_SCALE_FACTOR;
		break;
	default:
		return -ENODEV;
	}

	switch (dev->dev_type) {
	case ID_ADXL312:
		if (dev->resolution == ADXL313_10_BIT_RES)
			offset_val = (offset_val * 4) >> (dev->range - ADXL313_RANGE_FACTOR);
		else
			offset_val = offset_val * 4;
		break;
	case ID_ADXL313:
		if (dev->resolution == ADXL313_10_BIT_RES) {
			offset_val = (offset_val * 4);
			offset_val = offset_val >> dev->range;
		} else
			offset_val = offset_val * 4;
		break;
	case ID_ADXL314:
		offset_val = offset_val * 4;
		break;
	default:
		return -ENODEV;
	}

	/* Write raw data to register for setting offset. */
	return adxl313_set_raw_offset(dev, offset_val, axis);
}

/*******************************************************************************
 * @brief Get offset for each axis.
 *
 * @param dev       - The device structure.
 * @param offset_ug - Offset read in g / 1000 000 (ug).
 * @param axis      - Axis to apply offset.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_get_offset(struct adxl313_dev *dev, int32_t *offset_ug,
		       enum adxl313_axis axis)
{
	int ret;
	int32_t raw_offset = 0x00;

	if (axis > ADXL313_Z_AXIS)
		return -EINVAL;

	/* Read data to register for getting offset register value. */
	ret = adxl313_get_raw_offset(dev, &raw_offset, axis);
	if (ret)
		return ret;

	switch (dev->dev_type) {
	case ID_ADXL312:
		if (dev->resolution == ADXL313_10_BIT_RES)
			raw_offset = (raw_offset * 2) >> (ADXL313_12G_RANGE - dev->range);
		else
			raw_offset = raw_offset / 4;
		break;
	case ID_ADXL313:
		if (dev->resolution == ADXL313_10_BIT_RES)
			raw_offset = (raw_offset * 2) >> (ADXL313_4G_RANGE - dev->range);
		else
			raw_offset = raw_offset / 4;
		break;
	case ID_ADXL314:
		raw_offset = raw_offset / 4;
		break;
	default:
		return -ENODEV;
	}

	switch (dev->dev_type) {
	case ID_ADXL312:
		*offset_ug = raw_offset * ADXL312_OFFSET_SCALE_FACTOR;
		break;
	case ID_ADXL313:
		*offset_ug = raw_offset * ADXL313_OFFSET_SCALE_FACTOR;
		break;
	case ID_ADXL314:
		*offset_ug = raw_offset * ADXL314_OFFSET_SCALE_FACTOR;
		break;
	default:
		return -ENODEV;
	}

	return ret;
}

/*******************************************************************************
 * @brief Set offset for each axis as raw value.
 *
 * @param dev        - The device structure.
 * @param offset_raw - Offset (8-bit raw value)
 * @param axis       - Axis to apply offset.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_set_raw_offset(struct adxl313_dev *dev, int32_t offset_raw,
			   enum adxl313_axis axis)
{
	int ret;
	int8_t reg_val = 0x00;
	uint8_t reg_val_unsigned;
	int16_t min_clamp_val, max_clamp_val;

	switch (dev->dev_type) {
	case ID_ADXL312:
		if (dev->resolution == ADXL313_10_BIT_RES) {
			min_clamp_val = (INT8_MIN * 4) >> (dev->range - ADXL313_RANGE_FACTOR);
			max_clamp_val = (INT8_MAX * 4) >> (dev->range - ADXL313_RANGE_FACTOR);
		} else {
			min_clamp_val = INT8_MIN * 4;
			max_clamp_val = INT8_MAX * 4;
		}
		break;
	case ID_ADXL313:
		if (dev->resolution == ADXL313_10_BIT_RES) {
			min_clamp_val = (INT8_MIN * 4) >> dev->range;
			max_clamp_val = (INT8_MAX * 4) >> dev->range;
		} else {
			min_clamp_val = INT8_MIN * 4;
			max_clamp_val = INT8_MAX * 4;
		}
		break;
	case ID_ADXL314:
		min_clamp_val = INT8_MIN * 4;
		max_clamp_val = INT8_MAX * 4;
		break;
	default:
		return -ENODEV;
	}

	if (no_os_clamp(offset_raw, min_clamp_val, max_clamp_val) != offset_raw)
		return -EINVAL;

	if (axis > ADXL313_Z_AXIS)
		return -EINVAL;

	switch (dev->dev_type) {
	case ID_ADXL312:
		if (dev->resolution == ADXL313_10_BIT_RES)
			reg_val = (offset_raw * 2) >> (ADXL313_12G_RANGE - dev->range);
		else
			reg_val = offset_raw / 4;
		break;
	case ID_ADXL313:
		if (dev->resolution == ADXL313_10_BIT_RES)
			reg_val = (offset_raw * 2) >> (ADXL313_4G_RANGE - dev->range);
		else
			reg_val = offset_raw / 4;
		break;
	case ID_ADXL314:
		reg_val = offset_raw / 4;
		break;
	default:
		return -ENODEV;
	}

	reg_val_unsigned = (uint8_t)reg_val;

	/* Write data to register for setting offset. */
	ret = adxl313_write(dev, ADXL313_REG_OFS_AXIS(axis), 1, &reg_val_unsigned);
	if (ret)
		return ret;

	switch (axis) {
	case ADXL313_X_AXIS:
		dev->x_offset_raw = reg_val;
		break;
	case ADXL313_Y_AXIS:
		dev->y_offset_raw = reg_val;
		break;
	case ADXL313_Z_AXIS:
		dev->z_offset_raw = reg_val;
		break;
	default:
		return -EINVAL;
	}

	return ret;
}

/*******************************************************************************
 * @brief Get raw offset value for each axis, depending on range and resolution.
 *
 * @param dev       - The device structure.
 * @param offset_ug - Offset read as raw value.
 * @param axis      - Axis to apply offset.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_get_raw_offset(struct adxl313_dev *dev, int32_t *offset_raw,
			   enum adxl313_axis axis)
{
	int ret;
	uint8_t reg_val = 0x00;
	int8_t signed_reg_val;

	if (axis > ADXL313_Z_AXIS)
		return -EINVAL;

	/* Read data to register for getting offset register value. */
	ret = adxl313_read(dev, ADXL313_REG_OFS_AXIS(axis), 1, &reg_val);
	if (!ret) {
		signed_reg_val = (int8_t)reg_val;
		switch (dev->dev_type) {
		case ID_ADXL312:
			if (dev->resolution == ADXL313_10_BIT_RES)
				*offset_raw = (signed_reg_val * 4) >> (dev->range - ADXL313_RANGE_FACTOR);
			else
				*offset_raw = signed_reg_val * 4;
			break;
		case ID_ADXL313:
			if (dev->resolution == ADXL313_10_BIT_RES)
				*offset_raw = (signed_reg_val * 4) >> dev->range;
			else
				*offset_raw = signed_reg_val * 4;
			break;
		case ID_ADXL314:
			*offset_raw = signed_reg_val * 4;
			break;
		default:
			return -ENODEV;
		}
	}

	return ret;
}

/*******************************************************************************
 * @brief Read the 3-axis raw data from the accelerometer.
 *
 * @param dev     - The device structure.
 * @param x_raw   - X-axis data (as two's complement).
 * @param y_raw   - Y-axis data (as two's complement).
 * @param z_raw   - Z-axis data (as two's complement).
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_get_raw_xyz(struct adxl313_dev *dev,
			int16_t *x_raw,
			int16_t *y_raw,
			int16_t *z_raw)
{
	int ret;
	uint8_t xyz_values[6] = {0};

	ret = adxl313_read(dev,
			   ADXL313_REG_DATA_AXIS(0),
			   ADXL313_REGS_PER_ENTRY,
			   xyz_values);
	if (ret)
		return ret;

	/* Result is 13 bits long with sign extended for ADXL314. */
	*x_raw = ((int16_t)xyz_values[1] << 8) + (xyz_values[0]);
	*y_raw = ((int16_t)xyz_values[3] << 8) + (xyz_values[2]);
	*z_raw = ((int16_t)xyz_values[5] << 8) + (xyz_values[4]);

	return ret;
}

/*******************************************************************************
 * @brief Read the raw output data of each axis and convert it to g.
 *
 * @param dev      - The device structure.
 * @param x_m_s2   - X-axis data.
 * @param y_m_s2   - Y-axis data.
 * @param z_m_s2   - Z-axis data.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_get_xyz(struct adxl313_dev *dev,
		    struct adxl313_frac_repr *x_m_s2,
		    struct adxl313_frac_repr *y_m_s2,
		    struct adxl313_frac_repr *z_m_s2)
{
	int ret;
	int16_t x_raw = 0;
	int16_t y_raw = 0;
	int16_t z_raw = 0;

	ret = adxl313_get_raw_xyz(dev, &x_raw, &y_raw, &z_raw);
	if (ret)
		return ret;

	x_m_s2->integer = no_os_div_s64_rem(adxl313_accel_conv(dev, x_raw),
					    ADXL313_ACC_SCALE_FACTOR_DIV, &(x_m_s2->fractional));
	y_m_s2->integer = no_os_div_s64_rem(adxl313_accel_conv(dev, y_raw),
					    ADXL313_ACC_SCALE_FACTOR_DIV, &(y_m_s2->fractional));
	z_m_s2->integer = no_os_div_s64_rem(adxl313_accel_conv(dev, z_raw),
					    ADXL313_ACC_SCALE_FACTOR_DIV, &(z_m_s2->fractional));

	return ret;
}

/*******************************************************************************
 * @brief Read the number of FIFO entries.
 *
 * @param dev     - The device structure.
 * @param entries - Entries number. Each entry contains data for the three axis.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_get_no_of_fifo_entries(struct adxl313_dev *dev,
				   uint8_t *entries_no)
{
	int ret;
	uint8_t reg_val = 0x00;

	ret = adxl313_read(dev, ADXL313_REG_FIFO_STATUS, 1, &reg_val);
	if (ret)
		return ret;

	*entries_no = reg_val & ADXL313_REG_FIFO_STS_ENTRIES_MSK;

	return 0;
}

/*******************************************************************************
 * @brief Set the number of FIFO sample sets.
 *
 * @param dev     - The device structure.
 * @param samples - Sample sets number.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_set_fifo_samples(struct adxl313_dev *dev,
			     uint8_t samples_no)
{
	int ret;

	if (samples_no > ADXL313_MAX_FIFO_SAMPLES_VAL)
		return -EINVAL;

	// write no of samples to ADXL313_REG_FIFO_SAMPLES
	ret = adxl313_reg_write_msk(dev,
				    ADXL313_REG_FIFO_CTL,
				    no_os_field_prep(ADXL313_REG_FIFO_CTL_SAMPLES_MSK, samples_no),
				    ADXL313_REG_FIFO_CTL_SAMPLES_MSK);
	if (ret)
		return ret;

	dev->fifo_samples = samples_no;

	return ret;
}

/*******************************************************************************
 * @brief Set FIFO mode.
 *
 * @param dev  - The device structure.
 * @param mode - FIFO mode.
 * 			   Accepted values: ADXL313_FIFO_DISABLED,
 *								ADXL313_OLDEST_SAVED,
 *								ADXL313_STREAM_MODE,
 *								ADXL313_TRIGGERED_MODE
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_set_fifo_mode(struct adxl313_dev *dev,
			  enum adxl313_fifo_mode mode)
{
	int ret;

	if (mode > ADXL313_TRIGGERED_MODE)
		return -EINVAL;

	ret = adxl313_reg_write_msk(dev,
				    ADXL313_REG_FIFO_CTL,
				    no_os_field_prep(ADXL313_REG_FIFO_CTL_MODE_MSK, mode),
				    ADXL313_REG_FIFO_CTL_MODE_MSK);
	if (ret)
		return ret;

	dev->fifo_mode = mode;

	return ret;
}

/*******************************************************************************
 * @brief Read FIFO data and return the raw values.
 *
 * @param dev          - The device structure.
 * @param entries      - The number of fifo entries.
 * @param raw_x        - Raw x-axis data.
 * @param raw_y        - Raw y-axis data.
 * @param raw_z        - Raw z-axis data.
 *
 * @return ret         - Result of the read operation.
 * 						 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_get_raw_fifo_data(struct adxl313_dev *dev, uint8_t *entries,
			      int16_t *x_raw, int16_t *y_raw, int16_t *z_raw)
{
	int ret;
	uint8_t buff_idx = 0;
	uint8_t xyz_values[ADXL313_MAX_FIFO_ENTRIES * ADXL313_REGS_PER_ENTRY] = {0};

	/* Get the number of FIFO entries */
	ret = adxl313_get_no_of_fifo_entries(dev, entries);
	if (ret)
		return ret;

	if ((*entries) > 0) {
		for (uint8_t idx = 0; idx < (*entries); idx++) {
			ret = adxl313_read(dev,
					   ADXL313_REG_DATA_AXIS(0),
					   ADXL313_REGS_PER_ENTRY,
					   &xyz_values[idx * ADXL313_REGS_PER_ENTRY]);
			if (ret)
				return ret;

			//wait 5us
			no_os_udelay(5);
		}

		for (uint8_t idx = 0; idx < (*entries); idx++) {
			buff_idx = idx * ADXL313_REGS_PER_ENTRY;
			x_raw[idx] = ((int16_t)xyz_values[buff_idx+1] << 8) + (xyz_values[buff_idx]);
			y_raw[idx] = ((int16_t)xyz_values[buff_idx+3] << 8) + (xyz_values[buff_idx+2]);
			z_raw[idx] = ((int16_t)xyz_values[buff_idx+5] << 8) + (xyz_values[buff_idx+4]);
		}
	}

	return ret;
}

/*******************************************************************************
 * @brief Read converted values from FIFO.
 *
 * @param dev       - The device structure.
 * @param entries   - Number of read entries.
 * @param x  	    - X axis fractional data buffer.
 * @param y  	    - Y axis fractional data buffer.
 * @param z  	    - Z axis fractional data buffer.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_get_fifo_data(struct adxl313_dev *dev,
			  uint8_t *entries,
			  struct adxl313_frac_repr *x,
			  struct adxl313_frac_repr *y,
			  struct adxl313_frac_repr *z)
{
	int ret;
	int16_t raw_x[ADXL313_MAX_FIFO_ENTRIES] = {0};
	int16_t raw_y[ADXL313_MAX_FIFO_ENTRIES] = {0};
	int16_t raw_z[ADXL313_MAX_FIFO_ENTRIES] = {0};

	ret = adxl313_get_raw_fifo_data(dev, entries, raw_x, raw_y, raw_z);
	if (ret)
		return ret;

	if ((*entries) > 0) {
		for (uint8_t idx = 0; idx < (*entries); idx++) {
			x[idx].integer = no_os_div_s64_rem(adxl313_accel_conv(dev, raw_x[idx]),
							   ADXL313_ACC_SCALE_FACTOR_DIV, &(x[idx].fractional));
			y[idx].integer = no_os_div_s64_rem(adxl313_accel_conv(dev, raw_y[idx]),
							   ADXL313_ACC_SCALE_FACTOR_DIV, &(y[idx].fractional));
			z[idx].integer = no_os_div_s64_rem(adxl313_accel_conv(dev, raw_z[idx]),
							   ADXL313_ACC_SCALE_FACTOR_DIV, &(z[idx].fractional));
		}
	}

	return ret;
}

/*******************************************************************************
 * @brief Configure the activity threshold register.
 *        The threshold is expressed in ug/LSB.
 *
 * @param dev        - The device structure.
 * @param act_thr_ug - Activity threshold value (in g / 1000 - mg).
 *                     ADXL313
 *                     Multiple of 15625 ug, otherwise truncation is performed.
 *                     ADXL314
 *                     Multiple of 780000 ug, otherwise truncation is performed.
 *
 * @return ret       - Result of the configuration action.
 *                     0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_conf_act_thr(struct adxl313_dev *dev, uint32_t act_thr_ug)
{
	int ret;
	uint8_t reg_val = (uint8_t)(act_thr_ug / ADXL313_THRESH_SCALE_FACTOR);

	switch (dev->dev_type) {
	case ID_ADXL312:
		reg_val = (uint8_t)(act_thr_ug / ADXL312_THRESH_SCALE_FACTOR);
		break;
	case ID_ADXL313:
		reg_val = (uint8_t)(act_thr_ug / ADXL313_THRESH_SCALE_FACTOR);
		break;
	case ID_ADXL314:
		reg_val = (uint8_t)(act_thr_ug / ADXL314_THRESH_SCALE_FACTOR);
		break;
	default:
		return -ENODEV;
	}

	ret = adxl313_write(dev, ADXL313_REG_THRESH_ACT, 1, &reg_val);

	if (!ret)
		dev->act_thr = reg_val;

	return ret;
}

/*******************************************************************************
 * @brief Get the value in the activity threshold register.
 *        The threshold is expressed in ug/LSB.
 *
 * @param dev        - The device structure.
 * @param act_thr_ug - Activity threshold value (in g / 1000 - mg).
 *                     ADXL313
 *                     Multiple of 15625 ug.
 *                     ADXL314
 *                     Multiple of 780000 ug.
 *
 * @return ret       - Result of the read action.
 *                     0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_get_act_thr(struct adxl313_dev *dev, uint32_t *act_thr_ug)
{
	int ret;
	uint8_t reg_val = 0x00;

	ret = adxl313_read(dev, ADXL313_REG_THRESH_ACT, 1, &reg_val);

	if (ret)
		return ret;

	switch (dev->dev_type) {
	case ID_ADXL312:
		*act_thr_ug = (uint32_t)reg_val * ADXL312_THRESH_SCALE_FACTOR;
		break;
	case ID_ADXL313:
		*act_thr_ug = (uint32_t)reg_val * ADXL313_THRESH_SCALE_FACTOR;
		break;
	case ID_ADXL314:
		*act_thr_ug = (uint32_t)reg_val * ADXL314_THRESH_SCALE_FACTOR;
		break;
	default:
		return -ENODEV;
	}

	return ret;
}

/*******************************************************************************
 * @brief Configure the inactivity threshold register.
 *        The threshold is expressed in ug/LSB.
 *
 * @param dev          - The device structure.
 * @param inact_thr_ug - Inactivity threshold value.
 * 					     ADXL313
 * 					     Multiple of 15625 ug, otherwise truncation is performed.
 * 					     ADXL314
 *                       Multiple of 780000 ug, otherwise truncation is performed.
 *
 * @return ret         - Result of the configuration action.
 *                       0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_conf_inact_thr(struct adxl313_dev *dev, uint32_t inact_thr_ug)
{
	int ret;
	uint8_t reg_val = (uint8_t)(inact_thr_ug / ADXL313_THRESH_SCALE_FACTOR);

	switch (dev->dev_type) {
	case ID_ADXL312:
		reg_val = (uint8_t)(inact_thr_ug / ADXL312_THRESH_SCALE_FACTOR);
		break;
	case ID_ADXL313:
		reg_val = (uint8_t)(inact_thr_ug / ADXL313_THRESH_SCALE_FACTOR);
		break;
	case ID_ADXL314:
		reg_val = (uint8_t)(inact_thr_ug / ADXL314_THRESH_SCALE_FACTOR);
		break;
	default:
		return -ENODEV;
	}

	ret = adxl313_write(dev, ADXL313_REG_THRESH_INACT, 1, &reg_val);

	if (!ret)
		dev->inact_thr = reg_val;

	return ret;
}

/*******************************************************************************
 * @brief Get the value in the inactivity threshold register.
 *        The threshold is expressed in ug/LSB.
 *
 * @param dev        - The device structure.
 * @param act_thr_ug - Inactivity threshold value (in g / 1000 - mg).
 *                     ADXL313
 *                     Multiple of 15625 ug.
 *                     ADXL314
 *                     Multiple of 780000 ug.
 *
 * @return ret       - Result of the read action.
 *                     0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_get_inact_thr(struct adxl313_dev *dev, uint32_t *inact_thr_ug)
{
	int ret;
	uint8_t reg_val = 0x00;

	ret = adxl313_read(dev, ADXL313_REG_THRESH_INACT, 1, &reg_val);

	if (ret)
		return ret;

	switch (dev->dev_type) {
	case ID_ADXL312:
		*inact_thr_ug = (uint32_t)reg_val * ADXL312_THRESH_SCALE_FACTOR;
		break;
	case ID_ADXL313:
		*inact_thr_ug = (uint32_t)reg_val * ADXL313_THRESH_SCALE_FACTOR;
		break;
	case ID_ADXL314:
		*inact_thr_ug = (uint32_t)reg_val * ADXL314_THRESH_SCALE_FACTOR;
		break;
	default:
		return -ENODEV;
	}

	return ret;
}

/*******************************************************************************
 * @brief Configure the time inactivity threshold register.
 *        The scale factor is 1 s/LSB.
 *
 * @param dev          - The device structure.
 * @param time_inact_s - Time inactivity value in seconds.
 *
 * @return ret         - Result of the configuration action.
 *                       0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_conf_time_inact(struct adxl313_dev *dev, uint8_t time_inact_s)
{
	int ret;
	uint8_t reg_val = time_inact_s;

	ret = adxl313_write(dev, ADXL313_REG_TIME_INACT, 1, &reg_val);

	if (!ret)
		dev->time_inact = time_inact_s;

	return ret;
}

/*******************************************************************************
 * @brief Get the value in the time inactivity threshold register.
 *        The scale factor is 1 s/LSB.
 *
 * @param dev          - The device structure.
 * @param time_inact_s - Time inactivity value in seconds.
 *
 * @return ret         - Result of the setting read action.
 *                       0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_get_time_inact(struct adxl313_dev *dev, uint8_t *time_inact_s)
{
	int ret;
	uint8_t reg_val = 0x00;

	ret = adxl313_read(dev, ADXL313_REG_TIME_INACT, 1, &reg_val);

	if (!ret)
		*time_inact_s = reg_val;

	return ret;
}

/*******************************************************************************
 * @brief Configure the ACT AC/DC and INACT AC/DC bits.
 *
 * @param dev         - The device structure.
 * @param config      - Configuration mapping.
 *
 * @return ret        - Result of the configuration action.
 *                      0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_conf_act_inact_ctl(struct adxl313_dev *dev,
			       union adxl313_act_inact_ctl_flags config)
{
	int ret;
	uint8_t reg_val = config.value;

	ret = adxl313_write(dev, ADXL313_REG_ACT_INACT_CTL, 1, &reg_val);

	if (!ret)
		dev->act_inact_ctl = config;

	return ret;
}

/*******************************************************************************
 * @brief Get the ACT AC/DC and INACT AC/DC bits.
 *
 * @param dev         - The device structure.
 * @param config      - Configuration mapping.
 *
 * @return ret        - Result of the read action.
 *                      0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_get_conf_act_inact_ctl(struct adxl313_dev *dev,
				   union adxl313_act_inact_ctl_flags *config)
{
	int ret;
	union adxl313_act_inact_ctl_flags reg_val = {.value = 0};

	ret = adxl313_read(dev, ADXL313_REG_ACT_INACT_CTL, 1, &reg_val.value);

	if (!ret)
		*config = reg_val;

	return ret;
}

/***************************************************************************//**
 * @brief Enable the activity detection.
 *
 * @param dev           - The device structure.
 * @param act_axes      - Axes which participate in detecting activity (ORed together).
 *                         Example: 0x0 - disables axes participation.
 *                                  ADXL313_X_EN - enables x-axis participation.
 *                                  ADXL313_Y_EN - enables y-axis participation.
 *                                  ADXL313_Z_EN - enables z-axis participation.
 * @param act_ac_dc     - Selects dc-coupled or ac-coupled operation.
 *                        Example: 0x00 - dc-coupled operation.
 *                                 0x01 - ac-coupled operation.
 * @param act_thresh_ug - Threshold value for detecting activity. The scale factor
                          is 15625 ug/LSB for ADXL313 and 780000 ug/LSB for ADXL314.
 * @param int_pin       - Interrupts pin.
 *                        Example: 0x00 - activity interrupts on INT1 pin.
 *                                 0x01 - activity interrupts on INT2 pin.
 *
 * @return None.
*******************************************************************************/
int adxl313_set_activity_detection(struct adxl313_dev *dev,
				   uint8_t act_axes,
				   uint8_t act_ac_dc,
				   uint32_t act_thresh_ug,
				   uint8_t int_pin)
{
	int ret = 0;
	union adxl313_act_inact_ctl_flags act_flags = {.value = 0};

	ret = adxl313_get_conf_act_inact_ctl(dev, &act_flags);
	if (ret)
		return ret;

	if (act_axes & ADXL313_X_EN)
		act_flags.fields.ACT_X_EN = ENABLE_E;
	else
		act_flags.fields.ACT_X_EN = DISABLE_E;
	if (act_axes & ADXL313_Y_EN)
		act_flags.fields.ACT_Y_EN = ENABLE_E;
	else
		act_flags.fields.ACT_Y_EN = DISABLE_E;
	if (act_axes & ADXL313_Z_EN)
		act_flags.fields.ACT_Z_EN = ENABLE_E;
	else
		act_flags.fields.ACT_Z_EN = DISABLE_E;

	act_flags.fields.ACT_AC_EN = act_ac_dc;

	ret = adxl313_conf_act_inact_ctl(dev, act_flags);
	if (ret)
		return ret;

	ret = adxl313_conf_act_thr(dev, act_thresh_ug);
	if (ret)
		return ret;

	ret = adxl313_activity_int_map(dev, int_pin);
	if (ret)
		return ret;

	return adxl313_activity_int_enable(dev);
}

/***************************************************************************//**
 * @brief Enables the inactivity detection.
 *
 * @param dev             - The device structure.
 * @param inact_axes      - Axes which participate in detecting inactivity.
 *                          Example: 0x0 - disables axes participation.
 *                                   ADXL313_X_EN - enables x-axis participation.
 *                                   ADXL313_Y_EN - enables y-axis participation.
 *                                   ADXL313_Z_EN - enables z-axis participation.
 * @param inact_ac_dc     - Selects dc-coupled or ac-coupled operation.
 *                          Example: 0x00 - dc-coupled operation.
 *                                   0x01 - ac-coupled operation.
 * @param inact_thresh_ug - Threshold value for detecting inactivity. The scale
                            factor is 62.5 mg/LSB.
 * @param inact_time_s    - Inactivity time. The scale factor is 1 sec/LSB.
 * @param int_pin         - Interrupts pin.
 *                          Example: 0x00 - activity interrupts on INT1 pin.
 *                                   0x01 - activity interrupts on INT2 pin.
 *
 * @return None.
*******************************************************************************/
int adxl313_set_inactivity_detection(struct adxl313_dev *dev,
				     uint8_t inact_axes,
				     uint8_t inact_ac_dc,
				     uint32_t inact_thresh_ug,
				     uint8_t inact_time_s,
				     uint8_t int_pin)
{
	int ret = 0;
	union adxl313_act_inact_ctl_flags inact_flags = {.value = 0};

	ret = adxl313_get_conf_act_inact_ctl(dev, &inact_flags);
	if (ret)
		return ret;

	if(inact_axes & ADXL313_X_EN)
		inact_flags.fields.INACT_X_EN = ENABLE_E;
	else
		inact_flags.fields.INACT_X_EN = DISABLE_E;
	if(inact_axes & ADXL313_Y_EN)
		inact_flags.fields.INACT_Y_EN = ENABLE_E;
	else
		inact_flags.fields.INACT_Y_EN = DISABLE_E;
	if(inact_axes & ADXL313_Z_EN)
		inact_flags.fields.INACT_Z_EN = ENABLE_E;
	else
		inact_flags.fields.INACT_Z_EN = DISABLE_E;

	inact_flags.fields.INACT_AC_EN = inact_ac_dc;

	ret = adxl313_conf_act_inact_ctl(dev, inact_flags);
	if (ret)
		return ret;

	ret = adxl313_conf_inact_thr(dev, inact_thresh_ug);
	if (ret)
		return ret;

	ret = adxl313_conf_time_inact(dev, inact_time_s);
	if (ret)
		return ret;

	ret = adxl313_inactivity_int_map(dev, int_pin);
	if (ret)
		return ret;

	return adxl313_inactivity_int_enable(dev);
}

/*******************************************************************************
 * @brief Set output data rate (ODR).
 *
 * @param dev     - The device structure.
 * @param odr     - Data rate.
 *
 * @return ret    - Result of the setting operation action.
 *                  0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_set_odr(struct adxl313_dev *dev, enum adxl313_odr odr)
{
	int ret;
	uint8_t reg_value = 0x00;
	uint8_t data = 0;

	if (odr > ADXL313_ODR_3200HZ)
		return -EINVAL;

	ret = adxl313_read(dev, ADXL313_REG_BW_RATE, 1, &reg_value);
	if (ret)
		return ret;

	/* Clear data rate value. */
	reg_value &= ~ADXL313_RATE_MSK;

	/* Set new register value. */
	data = (odr + ADXL313_ODR_OFFSET_VAL) & ADXL313_RATE_MSK;
	reg_value |= data;

	/* Write data to register for setting odr. */
	ret = adxl313_write(dev, ADXL313_REG_BW_RATE, 1, &reg_value);
	if (ret)
		return ret;

	dev->odr = odr;

	return ret;
}

/*******************************************************************************
 * @brief Get output data rate (ODR).
 *
 * @param dev     - The device structure.
 * @param odr     - Data rate.
 *
 * @return ret    - Result of the read operation action.
 *                  0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_get_odr(struct adxl313_dev *dev, enum adxl313_odr *odr)
{
	int ret;
	uint8_t reg_val = 0x00;
	uint8_t data = 0x00;

	ret = adxl313_read(dev, ADXL313_REG_BW_RATE, 1, &reg_val);

	if (!ret) {
		data = (reg_val & ADXL313_RATE_MSK) - ADXL313_ODR_OFFSET_VAL;
		*odr = data;
	}

	return ret;
}

/*******************************************************************************
 * @brief Disable I2C.
 *
 * @param dev     - The device structure.
 *
 * @return ret    - Result of the disable action.
 *                  0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_disable_i2c(struct adxl313_dev *dev)
{
	if (dev->dev_type == ID_ADXL313)
		return adxl313_reg_write_msk(dev,
					     ADXL313_REG_POWER_CTL,
					     no_os_field_prep(ADXL313_POWER_CTL_I2C_DISABLE, DISABLE_E),
					     ADXL313_POWER_CTL_I2C_DISABLE);
	else
		return -EACCES;
}

/*******************************************************************************
 * @brief Enable I2C.
 *
 * @param dev     - The device structure.
 *
 * @return ret    - Result of the enable action.
 *                  0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_enable_i2c(struct adxl313_dev *dev)
{
	if (dev->dev_type == ID_ADXL313)
		return adxl313_reg_write_msk(dev,
					     ADXL313_REG_POWER_CTL,
					     no_os_field_prep(ADXL313_POWER_CTL_I2C_DISABLE, ENABLE_E),
					     ADXL313_POWER_CTL_I2C_DISABLE);
	else
		return -EACCES;
}

/*******************************************************************************
 * @brief Activate/Deactivate LOW_POWER.
 *
 * @param dev     - The device structure.
 * @param enable  - Setting: 0 - disable low power mode;
 *                           1 - enable low power mode.
 *
 * @return ret    - Result of the setting operation action.
 *                  0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_set_low_power_mode(struct adxl313_dev *dev, enum bit_action enable)
{
	int ret;
	uint8_t mask = ADXL313_LOW_POWER_OP;
	uint8_t data = enable;

	/* Set STDBY operation mode. */
	ret = adxl313_set_op_mode(dev, ADXL313_STDBY);
	if (ret)
		return ret;

	if (enable == ENABLE_E) {
		/* If data rate greater than 400 Hz set to this val. */
		if (dev->odr > ADXL313_ODR_400HZ)
			ret = adxl313_set_odr(dev, ADXL313_ODR_400HZ);
		/* If data rate smaller than 12.5 Hz set to this val. */
		else if (dev->odr < ADXL313_ODR_12_5HZ)
			ret = adxl313_set_odr(dev, ADXL313_ODR_12_5HZ);
		if (ret)
			return ret;

		dev->lp_mode = ADXL313_LP_MODE_ON;
	} else {
		data = DISABLE_E;
		dev->lp_mode = ADXL313_LP_MODE_NONE;
	}

	/* Set/Clear the AUTOSLEEP bit. */
	ret = adxl313_reg_write_msk(dev,
				    ADXL313_REG_BW_RATE,
				    no_os_field_prep(mask, data),
				    mask);

	if (ret)
		return ret;

	/* Set MEAS operation mode. */
	return adxl313_set_op_mode(dev, ADXL313_MEAS);
}

/*******************************************************************************
 * @brief Activate/Deactivate AUTOSLEEP.
 *
 * @param dev        - The device structure.
 * @param enable     - Setting: 0 - disable autosleep;
 *                           1 - enable autosleep.
 * @param inact_thr  - Inactivity threshold value.
 * @param time_inact - Time inactivity value.
 *
 * @return ret       - Result of the setting operation action.
 *                     0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_set_autosleep_mode(struct adxl313_dev *dev, enum bit_action enable,
			       uint8_t inact_thr, uint8_t time_inact_s)
{
	int ret;
	uint8_t mask = ADXL313_POWER_CTL_LINK | ADXL313_POWER_CTL_AUTO_SLEEP;
	uint8_t data = ADXL313_POWER_CTL_LINK | ADXL313_POWER_CTL_AUTO_SLEEP;

	/* Set STDBY operation mode. */
	ret = adxl313_set_op_mode(dev, ADXL313_STDBY);
	if (ret)
		return ret;

	if (enable == ENABLE_E) {
		ret = adxl313_conf_inact_thr(dev, inact_thr);
		if (ret)
			return ret;

		ret = adxl313_conf_time_inact(dev, time_inact_s);
		if (ret)
			return ret;

		dev->lp_mode = ADXL313_LP_MODE_AUTOSLEEP;
	} else {
		ret = adxl313_conf_inact_thr(dev, 0);
		if (ret)
			return ret;

		ret = adxl313_conf_time_inact(dev, 0);
		if (ret)
			return ret;

		data = DISABLE_E;

		dev->lp_mode = ADXL313_LP_MODE_NONE;
	}

	/* Set the LINK and AUTOSLEEP bits. */
	ret = adxl313_reg_write_msk(dev,
				    ADXL313_REG_POWER_CTL,
				    data,
				    mask);
	if (ret)
		return ret;

	/* Set MEAS operation mode. */
	return adxl313_set_op_mode(dev, ADXL313_MEAS);
}

/*******************************************************************************
 * @brief Enable Link Mode for activity inactivity functions.
 *
 * @param dev         - The device structure.
 *
 * @return ret        - Result of the configuration action.
 *                      0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_link_mode_enable(struct adxl313_dev *dev)
{
	uint8_t mask = ADXL313_POWER_CTL_LINK;
	uint8_t data = ENABLE_E;

	return adxl313_reg_write_msk(dev,
				     ADXL313_REG_POWER_CTL,
				     no_os_field_prep(mask, data),
				     mask);
}

/*******************************************************************************
 * @brief Disable Link Mode for activity inactivity functions.
 *
 * @param dev         - The device structure.
 *
 * @return ret        - Result of the configuration action.
 *                      0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_link_mode_disable(struct adxl313_dev *dev)
{
	uint8_t mask = ADXL313_POWER_CTL_LINK;
	uint8_t data = DISABLE_E;

	return adxl313_reg_write_msk(dev,
				     ADXL313_REG_POWER_CTL,
				     no_os_field_prep(mask, data),
				     mask);
}

/*******************************************************************************
 * @brief Put device in Sleep mode andd set up wake-up frequency.
 *
 * @param dev          - The device structure.
 * @param wake_up_f_hz - Wake-up frequency (8, 4, 2, 1 Hz)
 *
 * @return ret         - Result of the configuration action.
 *                       0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_sleep(struct adxl313_dev *dev, enum adxl313_wake_up_f wake_up_f_hz)
{
	int ret;

	if (wake_up_f_hz > ADXL313_WU_1_HZ)
		return -EINVAL;

	ret = adxl313_reg_write_msk(dev,
				    ADXL313_REG_POWER_CTL,
				    no_os_field_prep(ADXL313_POWER_CTL_WAKEUP(wake_up_f_hz), wake_up_f_hz),
				    ADXL313_POWER_CTL_WAKEUP(wake_up_f_hz));
	if (ret)
		return ret;

	return adxl313_reg_write_msk(dev,
				     ADXL313_REG_POWER_CTL,
				     no_os_field_prep(ADXL313_POWER_CTL_SLEEP, ENABLE_E),
				     ADXL313_POWER_CTL_SLEEP);

}

/*******************************************************************************
 * @brief Exit Sleep mode.
 *
 * @param dev          - The device structure.
 * @param wake_up_f_hz - Wake-up frequency (8, 4, 2, 1 Hz)
 *
 * @return ret         - Result of the configuration action.
 *                       0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_exit_sleep(struct adxl313_dev *dev)
{
	int ret;

	ret = adxl313_set_op_mode(dev, ADXL313_STDBY);
	if (ret)
		return ret;

	ret = adxl313_reg_write_msk(dev,
				    ADXL313_REG_POWER_CTL,
				    no_os_field_prep(ADXL313_POWER_CTL_SLEEP, DISABLE_E),
				    ADXL313_POWER_CTL_SLEEP);

	if (ret)
		return ret;

	return adxl313_set_op_mode(dev, ADXL313_MEAS);
}

/*******************************************************************************
 * @brief Configure interrupt enable control.
 *
 * @param dev         - The device structure.
 * @param en_ctl      - Enable control configuration.
 *
 * @return ret        - Result of the configuration action.
 *                      0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_conf_int_enable(struct adxl313_dev *dev,
			    union adxl313_int_en_reg_flags en_ctl)
{
	uint8_t reg_val = en_ctl.value;

	return adxl313_write(dev, ADXL313_REG_INT_EN, 1, &reg_val);
}

/*******************************************************************************
 * @brief Configure interrupt enable control for activity.
 *
 * @param dev         - The device structure.
 *
 * @return ret        - Result of the configuration action.
 *                      0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_activity_int_enable(struct adxl313_dev *dev)
{
	int ret;
	union adxl313_int_en_reg_flags en_ctl = {.value = 0};

	ret = adxl313_read(dev, ADXL313_REG_INT_EN, 1, &en_ctl.value);
	if (ret)
		return ret;

	en_ctl.fields.ACTIVITY = ENABLE_E;

	return adxl313_conf_int_enable(dev, en_ctl);
}

/*******************************************************************************
 * @brief Configure interrupt disable control for activity.
 *
 * @param dev         - The device structure.
 *
 * @return ret        - Result of the configuration action.
 *                      0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_activity_int_disable(struct adxl313_dev *dev)
{
	int ret;
	union adxl313_int_en_reg_flags en_ctl = {.value = 0};

	ret = adxl313_read(dev, ADXL313_REG_INT_EN, 1, &en_ctl.value);
	if (ret)
		return ret;

	en_ctl.fields.ACTIVITY = DISABLE_E;

	return adxl313_conf_int_enable(dev, en_ctl);
}

/*******************************************************************************
 * @brief Configure interrupt enable control for inactivity.
 *
 * @param dev         - The device structure.
 *
 * @return ret        - Result of the configuration action.
 *                      0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_inactivity_int_enable(struct adxl313_dev *dev)
{
	int ret;
	union adxl313_int_en_reg_flags en_ctl = {.value = 0};

	ret = adxl313_read(dev, ADXL313_REG_INT_EN, 1, &en_ctl.value);
	if (ret)
		return ret;

	en_ctl.fields.INACTIVITY = ENABLE_E;

	return adxl313_conf_int_enable(dev, en_ctl);
}

/*******************************************************************************
 * @brief Configure interrupt disable control for inactivity.
 *
 * @param dev         - The device structure.
 *
 * @return ret        - Result of the configuration action.
 *                      0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_inactivity_int_disable(struct adxl313_dev *dev)
{
	int ret;
	union adxl313_int_en_reg_flags en_ctl = {.value = 0};

	ret = adxl313_read(dev, ADXL313_REG_INT_EN, 1, &en_ctl.value);
	if (ret)
		return ret;

	en_ctl.fields.INACTIVITY = DISABLE_E;

	return adxl313_conf_int_enable(dev, en_ctl);
}

/*******************************************************************************
 * @brief Configure interrupt mapping control.
 *
 * @param dev         - The device structure.
 * @param int_map     - Interrupt configuration mapping (0 - INT1, 1 - INT2).
 *
 * @return ret        - Result of the configuration action.
 *                      0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_conf_int_map(struct adxl313_dev *dev,
			 union adxl313_int_map_reg_flags int_map)
{
	uint8_t reg_val = int_map.value;

	return adxl313_write(dev, ADXL313_REG_INT_MAP, 1, &reg_val);
}

/*******************************************************************************
 * @brief Set pin for DATA_READY interrupt.
 *
 * @param dev         - The device structure.
 * @param int_pin     - Interrupt configuration mapping (0 - INT1, 1 - INT2).
 *
 * @return ret        - Result of the configuration action.
 *                      0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_data_ready_int_map(struct adxl313_dev *dev, uint8_t int_pin)
{
	int ret;
	union adxl313_int_map_reg_flags map_ctl = {.value = 0};

	ret = adxl313_read(dev, ADXL313_REG_INT_MAP, 1, &map_ctl.value);
	if (ret)
		return ret;

	map_ctl.fields.DATA_READY = int_pin;

	return adxl313_conf_int_map(dev, map_ctl);
}

/*******************************************************************************
 * @brief Set pin for activity interrupt.
 *
 * @param dev         - The device structure.
 * @param int_pin     - Interrupt configuration mapping (0 - INT1, 1 - INT2).
 *
 * @return ret        - Result of the configuration action.
 *                      0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_activity_int_map(struct adxl313_dev *dev, uint8_t int_pin)
{
	int ret;
	union adxl313_int_map_reg_flags map_ctl = {.value = 0};

	ret = adxl313_read(dev, ADXL313_REG_INT_MAP, 1, &map_ctl.value);
	if (ret)
		return ret;

	map_ctl.fields.ACTIVITY = int_pin;

	return adxl313_conf_int_map(dev, map_ctl);
}

/*******************************************************************************
 * @brief Set pin for inactivity interrupt.
 *
 * @param dev         - The device structure.
 * @param int_pin     - Interrupt configuration mapping (0 - INT1, 1 - INT2).
 *
 * @return ret        - Result of the configuration action.
 *                      0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_inactivity_int_map(struct adxl313_dev *dev, uint8_t int_pin)
{
	int ret;
	union adxl313_int_map_reg_flags map_ctl = {.value = 0};

	ret = adxl313_read(dev, ADXL313_REG_INT_MAP, 1, &map_ctl.value);
	if (ret)
		return ret;

	map_ctl.fields.INACTIVITY = int_pin;

	return adxl313_conf_int_map(dev, map_ctl);
}

/*******************************************************************************
 * @brief Set pin for watermark interrupt.
 *
 * @param dev         - The device structure.
 * @param int_pin     - Interrupt configuration mapping (0 - INT1, 1 - INT2).
 *
 * @return ret        - Result of the configuration action.
 *                      0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_watermark_int_map(struct adxl313_dev *dev, uint8_t int_pin)
{
	int ret;
	union adxl313_int_map_reg_flags map_ctl = {.value = 0};

	ret = adxl313_read(dev, ADXL313_REG_INT_MAP, 1, &map_ctl.value);
	if (ret)
		return ret;

	map_ctl.fields.WATERMARK = int_pin;

	return adxl313_conf_int_map(dev, map_ctl);
}

/*******************************************************************************
 * @brief Set pin for overrun interrupt.
 *
 * @param dev         - The device structure.
 * @param int_pin     - Interrupt configuration mapping (0 - INT1, 1 - INT2).
 *
 * @return ret        - Result of the configuration action.
 *                      0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_overrun_int_map(struct adxl313_dev *dev, uint8_t int_pin)
{
	int ret;
	union adxl313_int_map_reg_flags map_ctl = {.value = 0};

	ret = adxl313_read(dev, ADXL313_REG_INT_MAP, 1, &map_ctl.value);
	if (ret)
		return ret;

	map_ctl.fields.OVERRUN = int_pin;

	return adxl313_conf_int_map(dev, map_ctl);
}

/*******************************************************************************
 * @brief Read the INT_SOURCE register value. Get the source of interrupts.
 *
 * @param dev              - The device structure.
 * @param int_status_flags - Register value.
 *
 * @return ret             - Result of the reading action.
 *                           0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_get_int_source_reg(struct adxl313_dev *dev,
			       union adxl313_int_src_reg_flags *int_status_flags)
{
	int ret;
	uint8_t reg_value = 0x00;

	ret = adxl313_read(dev, ADXL313_REG_INT_SRC, 1, &reg_value);

	if (!ret)
		int_status_flags->value = reg_value;

	return ret;
}

/*******************************************************************************
 * @brief Set the interrupt polarity.
 *
 * @param dev     - The device structure.
 * @param int_pol - Interrupt polarity to be set.
 *
 * @return ret    - Result of the configuration action.
 *                  0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_set_int_pol(struct adxl313_dev *dev,
			enum adxl313_int_pol int_pol)
{
	uint8_t mask = ADXL313_REG_DATA_FORMAT_INT_INV;
	uint8_t data = ENABLE_E;

	if (int_pol == ADXL313_INT_ACTIVE_HIGH)
		data = DISABLE_E;

	return adxl313_reg_write_msk(dev,
				     ADXL313_REG_DATA_FORMAT,
				     no_os_field_prep(mask, data),
				     mask);
}

/*******************************************************************************
 * @brief Enable full resolution for the ADXL312/ADXL313 device.
 *
 * @param dev     - The device structure.
 *
 * @return ret    - Result of the resolution configuration action.
 *                  0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_enable_full_res(struct adxl313_dev *dev)
{
	int ret;

	if (dev->dev_type == ID_ADXL314)
		return -EACCES;

	ret = adxl313_reg_write_msk(dev,
				    ADXL313_REG_DATA_FORMAT,
				    no_os_field_prep(ADXL313_REG_DATA_FORMAT_FULL_RES, ENABLE_E),
				    ADXL313_REG_DATA_FORMAT_FULL_RES);

	if (ret)
		return ret;

	return adxl313_set_range(dev, dev->range);
}

/*******************************************************************************
 * @brief Disable full resolution for the ADXL312/ADXL313 device.
 *
 * @param dev     - The device structure.
 *
 * @return ret    - Result of the resolution configuration action.
 *                  0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_disable_full_res(struct adxl313_dev *dev)
{
	int ret;

	if (dev->dev_type == ID_ADXL314)
		return -EACCES;

	ret = adxl313_reg_write_msk(dev,
				    ADXL313_REG_DATA_FORMAT,
				    no_os_field_prep(ADXL313_REG_DATA_FORMAT_FULL_RES, DISABLE_E),
				    ADXL313_REG_DATA_FORMAT_FULL_RES);

	if (ret)
		return ret;

	return adxl313_set_range(dev, dev->range);
}

/*******************************************************************************
 * @brief Get the ADXL312/ADXL313 FULL_RES setting.
 *
 * @param dev      - The device structure.
 * @param full_res - Device full resolution setting (0 - full_res disabled,
 * 			         other values - full_res enabled).
 *
 * @return ret     - Result of the FULL_RES setting read action.
 *                   0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_get_full_res_setting(struct adxl313_dev *dev, uint8_t *full_res)
{
	int ret;
	uint8_t reg_value = 0x00;

	if (dev->dev_type == ID_ADXL314)
		return -EACCES;

	ret = adxl313_read(dev, ADXL313_REG_DATA_FORMAT, 1, &reg_value);
	if (!ret)
		*full_res = reg_value & ADXL313_REG_DATA_FORMAT_FULL_RES;

	return ret;
}

/*******************************************************************************
 * @brief Set the ADXL312/ADXL313 device range.
 *
 * @param dev     - The device structure.
 * @param range   - Range to be set.
 *
 * @return ret    - Result of the range configuration action.
 *                  0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_set_range(struct adxl313_dev *dev, enum adxl313_range range)
{
	int ret;
	uint8_t mask = ADXL313_REG_DATA_FORMAT_RANGE;
	uint8_t full_res = 0;
	uint8_t range_val = range;

	if (dev->dev_type == ID_ADXL314)
		return -EACCES;

	if (dev->dev_type == ID_ADXL312) {
		if ((range_val < ADXL313_1_5G_RANGE) || (range_val > ADXL313_12G_RANGE))
			return -EINVAL;

		range_val -= ADXL313_RANGE_FACTOR;
	}

	if (dev->dev_type == ID_ADXL313)
		if (range_val > ADXL313_4G_RANGE)
			return -EINVAL;

	ret = adxl313_reg_write_msk(dev,
				    ADXL313_REG_DATA_FORMAT,
				    no_os_field_prep(mask, range_val),
				    mask);
	if (!ret) {
		dev->range = range;

		ret = adxl313_get_full_res_setting(dev, &full_res);
		if (ret)
			return ret;

		if (full_res)
			dev->resolution = range_val;
		else
			dev->resolution = ADXL313_10_BIT_RES;
	}

	adxl313_compute_multiplier(dev);

	return ret;
}

/*******************************************************************************
 * @brief Get the ADXL312/ADXL313 device range.
 *
 * @param dev     - The device structure.
 * @param range   - Device range.
 *
 * @return ret    - Result of the range read action.
 *                  0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_get_range(struct adxl313_dev *dev, enum adxl313_range *range)
{
	int ret;
	uint8_t reg_value = 0x00;

	if (dev->dev_type == ID_ADXL314) {
		*range = ADXL313_200G_RANGE;
		return 0;
	} else {
		ret = adxl313_read(dev, ADXL313_REG_DATA_FORMAT, 1, &reg_value);
		if (!ret) {
			if (dev->dev_type == ID_ADXL313) {
				/* ADXL313 */
				*range = reg_value & ADXL313_REG_DATA_FORMAT_RANGE;
			} else {
				/* ADXL312 */
				*range = (reg_value & ADXL313_REG_DATA_FORMAT_RANGE) + ADXL313_RANGE_FACTOR;
			}
		}

		return ret;
	}
}

/*******************************************************************************
 * @brief Performs self test.
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl313_self_test(struct adxl313_dev *dev)
{
	int ret;
	int16_t x[ADXL313_MAX_FIFO_ENTRIES] = {0};
	int16_t y[ADXL313_MAX_FIFO_ENTRIES] = {0};
	int16_t z[ADXL313_MAX_FIFO_ENTRIES] = {0};
	int16_t zst_off = 0;
	int16_t zst_on = 0;
	int16_t zst = 0;
	uint8_t fifo_entries = 0;
	uint8_t mask = ADXL313_REG_DATA_FORMAT_SELF_TEST;
	uint8_t data = ENABLE_E;

	/* 1. Set the device ODR to 100 Hz. */
	/* Set dev. in standby mode. */
	ret = adxl313_set_op_mode(dev, ADXL313_STDBY);
	if (ret)
		return ret;

	ret = adxl313_set_odr(dev, ADXL313_ODR_100HZ);
	if (ret)
		return ret;

	/* If ADXL312, ADXL313, set device to full resolution, +/- 12g, 4g range, respectively. */
	if (dev->dev_type == ID_ADXL312 || dev->dev_type == ID_ADXL313) {
		ret = adxl313_enable_full_res(dev);
		if (ret)
			return ret;
	}

	if (dev->dev_type == ID_ADXL312) {
		ret = adxl313_set_range(dev, ADXL313_12G_RANGE);
	} else if (dev->dev_type == ID_ADXL313) {
		ret = adxl313_set_range(dev, ADXL313_4G_RANGE);
	}
	if (ret)
		return ret;

	/* 2. Clear the LOW_POWER bit (Bit D4) in the BW_RATE register. */
	ret = adxl313_set_low_power_mode(dev, DISABLE_E);
	if (ret)
		return ret;

	/* 3. Read samples from FIFO. (Recommended starting point is 0.1 sec worth
	 * of data for data rates of 100 Hz or greater).  */

	/* Set dev. in standby mode. */
	ret = adxl313_set_op_mode(dev, ADXL313_STDBY);
	if (ret)
		return ret;

	/* Set FIFO mode to FIFO */
	ret = adxl313_set_fifo_mode(dev, ADXL313_FIFO_MODE);
	if (ret)
		return ret;

	/* Set dev. in measure mode. */
	ret = adxl313_set_op_mode(dev, ADXL313_MEAS);
	if (ret)
		return ret;

	/* Output settling time, min 4 samples. 10ms per sample at 100Hz. */
	no_os_mdelay(50);

	/* 4. Read and store the 10 samples in the FIFO. */
	ret = adxl313_get_raw_fifo_data(dev, &fifo_entries, x, y, z);
	if (ret)
		return ret;

	/* Compute average (10 samples). */
	for (uint8_t idx = 0; idx < ADXL313_SELF_TEST_SAMPLES; idx++) {
		zst_off += z[idx];
	}
	zst_off /= ADXL313_SELF_TEST_SAMPLES;

	/* 5. Enable self-test. */
	/* Set dev in standby mode. */
	ret = adxl313_set_op_mode(dev, ADXL313_STDBY);
	if (ret)
		return ret;

	/* Set self-test bit. */
	ret = adxl313_reg_write_msk(dev,
				    ADXL313_REG_DATA_FORMAT,
				    no_os_field_prep(mask, data),
				    mask);
	if (ret)
		return ret;

	/* Set dev. in measure mode. */
	ret = adxl313_set_op_mode(dev, ADXL313_MEAS);
	if (ret)
		return ret;

	/* Output settling time, min 4 samples. 10ms per sample at 100Hz. */
	no_os_mdelay(50);

	/* 6. Read and store samples in the FIFO. */
	ret = adxl313_get_raw_fifo_data(dev, &fifo_entries, x, y, z);
	if (ret)
		return ret;

	/* 7. Compute average (10 samples). */
	for (uint8_t idx = 0; idx < ADXL313_SELF_TEST_SAMPLES; idx++) {
		zst_on += z[idx];
	}
	zst_on /= ADXL313_SELF_TEST_SAMPLES;

	/* 8. Disable self-test. */
	/* Set dev. in standby mode. */
	ret = adxl313_set_op_mode(dev, ADXL313_STDBY);
	if (ret)
		return ret;

	/* Reset self-test bit. */
	data = DISABLE_E;
	ret = adxl313_reg_write_msk(dev,
				    ADXL313_REG_DATA_FORMAT,
				    no_os_field_prep(mask, data),
				    mask);
	if (ret)
		return ret;

	/* Set dev. in measure mode. */
	ret = adxl313_set_op_mode(dev, ADXL313_MEAS);
	if (ret)
		return ret;

	zst = zst_on - zst_off;

	if (dev->dev_type == ID_ADXL314) {
		zst = (zst * ADXL314_SELF_TEST_MULT) / ADXL313_SELF_TEST_DIV;
		if ((zst < ADXL314_SELF_TEST_MAX_DEVIATION) &&
		    (zst > ADXL314_SELF_TEST_MIN_DEVIATION))
			pr_info("Self-test passed.\n");
		else
			pr_info("Self-test failed.\n");
	} else if (dev->dev_type == ID_ADXL313) {
		zst = (zst * ADXL313_SELF_TEST_MULT) / ADXL313_SELF_TEST_DIV;
		if ((zst < ADXL313_SELF_TEST_MAX_DEVIATION) &&
		    (zst > ADXL313_SELF_TEST_MIN_DEVIATION))
			pr_info("Self-test passed.\n");
		else
			pr_info("Self-test failed.\n");
	} else {
		zst = (zst * ADXL312_SELF_TEST_MULT) / ADXL313_SELF_TEST_DIV;
		if ((zst < ADXL312_SELF_TEST_MAX_DEVIATION) &&
		    (zst > ADXL312_SELF_TEST_MIN_DEVIATION))
			pr_info("Self-test passed.\n");
		else
			pr_info("Self-test failed.\n");
	}

	return ret;
}

/*******************************************************************************
 * @brief Convert raw acceleration value to g value.
 *
 * @param dev       - The device structure.
 * @param raw_accel - Raw acceleration value.
 *
 * @return ret      - Converted data.
*******************************************************************************/
static int64_t adxl313_accel_conv(struct adxl313_dev *dev, int16_t raw_accel)
{
	/* Apply scale factor based on the selected range. */
	return (int64_t)raw_accel * dev->scale_factor_mult;
}

/*******************************************************************************
 * @brief Compute the factors for converting raw value to m/s^2 depending on device,
 *        depending on its range, and on its resolution.
 *
 * @param dev       - The device structure.
 *
 * @return ret      - Converted data.
*******************************************************************************/
static void adxl313_compute_multiplier(struct adxl313_dev *dev)
{
	if (dev->dev_type == ID_ADXL312) {
		if (dev->resolution == ADXL313_10_BIT_RES)
			/* We have 10-bit resolution, 2.9 for +/-1.5 g, 5.8 for +/-3 g, 11.6  for +/-6 g
			 * and 23.2 for +/-12 g (mg/LSB). */
			dev->scale_factor_mult = (ADXL312_ACC_SCALE_FACTOR_MUL_FULL_RES <<
						  (dev->range - ADXL313_RANGE_FACTOR));
		else
			/* We have full resolution for each one of the ranges. */
			dev->scale_factor_mult = ADXL312_ACC_SCALE_FACTOR_MUL_FULL_RES;
	} else {
		if (dev->dev_type == ID_ADXL313) {
			if (dev->resolution == ADXL313_10_BIT_RES)
				/* We have 10-bit resolution, 1024 for +/-0.5 g, 512 for +/-1 g, 256  for +/-2 g
				 * and 128 for +/-4 g (LSB/g). */
				dev->scale_factor_mult = (ADXL313_ACC_SCALE_FACTOR_MUL_FULL_RES <<
							  (dev->range)) /
							 ADXL313_ACC_SCALE_FACTOR_MUL_DIVIDER;
			else
				/* We have full resolution for each one of the ranges. */
				dev->scale_factor_mult = ADXL313_ACC_SCALE_FACTOR_MUL_FULL_RES /
							 ADXL313_ACC_SCALE_FACTOR_MUL_DIVIDER;
		} else {
			/* ADXL314 */
			dev->scale_factor_mult = ADXL314_ACC_SCALE_FACTOR_MUL;
		}
	}
}
