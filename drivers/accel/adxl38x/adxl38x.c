/***************************************************************************//**
 *   @file   adxl38x.c
 *   @brief  Implementation of ADXL38x Driver.
 *   @author Balarupini Rajendran (balarupini.rajendran@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include <errno.h>
#include "adxl38x.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_util.h"

/******************************************************************************/
/********************Variable & Constants Declarations ************************/
/******************************************************************************/
static const uint8_t adxl38x_scale_mul[3] = {1, 2, 4};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
static uint32_t adxl38x_accel_array_conv(uint8_t *raw_array);
static int64_t adxl38x_accel_conv(struct adxl38x_dev *dev, uint16_t raw_accel);
static int adxl38x_set_to_standby(struct adxl38x_dev *dev);
static int64_t adxl38x_accel_two_comp_conv_s64(uint16_t raw_accel);

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Reads from the device.
 *
 * @param dev          - The device structure.
 * @param base_address - Address of the base register.
 * @param size         - The number of bytes to be read and returned in read_data.
 * @param read_data    - The read data buffer
 *
 * @return ret         - Result of the reading procedure.
*******************************************************************************/
int adxl38x_read_device_data(struct adxl38x_dev *dev, uint8_t base_address,
			     uint16_t size, uint8_t *read_data)
{
	int ret;
	uint8_t buffer[64] = {0};

	if (dev->comm_type == ADXL38x_SPI_COMM) {
		buffer[0] = (base_address << 1) | ADXL38x_SPI_READ  ;
		ret = no_os_spi_write_and_read(dev->com_desc.spi_desc, buffer,
					       size+1);
		for (uint16_t idx = 0; idx < size; idx++)
			read_data[idx] = buffer[idx+1];
	} else {
		ret = no_os_i2c_write(dev->com_desc.i2c_desc, &base_address, 1, 0);
		if (ret)
			return ret;
		ret = no_os_i2c_read(dev->com_desc.i2c_desc, read_data, size, 1);
	}

	return ret;
}

/***************************************************************************//**
 * @brief Writes to the device
 *
 * @param dev          - The device structure.
 * @param base_address - Address of the base register.
 * @param size         - The number of bytes to be written. It is the size
 *                       of the write_data buffer.
 * @param write_data   - The data which is going to be written.
 *
 * @return ret         - Result of the writing procedure.
*******************************************************************************/
int adxl38x_write_device_data(struct adxl38x_dev *dev, uint8_t base_address,
			      uint16_t size, uint8_t *write_data)
{
	int ret;
	uint8_t buffer[64] = {0};

	for (uint16_t idx = 0; idx < size; idx++)
		buffer[1+idx] = write_data[idx];

	if (dev->comm_type == ADXL38x_SPI_COMM) {
		buffer[0] = ADXL38x_SPI_WRITE | (base_address << 1);
		ret = no_os_spi_write_and_read(dev->com_desc.spi_desc, buffer,
					       size + 1);
	} else {
		dev->comm_buff[0] = base_address;
		ret = no_os_i2c_write(dev->com_desc.i2c_desc, dev->comm_buff, size + 1, 1);
	}

	return ret;
}


/***************************************************************************//**
 * @brief Initializes the device and checks for valid peripheral communication
 *
 * @param device          	- The device structure.
 * @param init_param 	- Structure containing initialization parameters
 *
 * @return ret         	- Result of the initialization
*******************************************************************************/
int adxl38x_init(struct adxl38x_dev **device,
		 struct adxl38x_init_param init_param)
{
	struct adxl38x_dev *dev;
	int ret;
	uint8_t reg_value;

	switch (init_param.dev_type) {
	case ID_ADXL380:
	case ID_ADXL382:
		dev->dev_type = init_param.dev_type;
		break;
	default:
		return -EINVAL;
	}

	dev = (struct adxl38x_dev *)no_os_calloc(1, sizeof(*dev));

	if (!dev)
		return -ENOMEM;

	dev->comm_type = init_param.comm_type;

	if (dev->comm_type == ADXL38x_SPI_COMM) {
		ret = no_os_spi_init(&dev->com_desc.spi_desc, &init_param.comm_init.spi_init);
		if (ret)
			goto error_dev;
	} else {
		ret = no_os_i2c_init(&dev->com_desc.i2c_desc, &init_param.comm_init.i2c_init);
		if (ret)
			goto error_dev;
	}

	ret = adxl38x_read_device_data(dev, ADXL38x_DEVID_AD,
				       1, &reg_value);
	if (ret || (reg_value != ADXL38x_RESET_DEVID_AD))
		goto error_com;

	ret = adxl38x_read_device_data(dev, ADXL38x_DEVID_MST,
				       1, &reg_value);
	if (ret || (reg_value != ADXL38x_RESET_DEVID_MST))
		goto error_com;

	ret = adxl38x_read_device_data(dev, ADXL38x_PART_ID,
				       1, &reg_value);
	if (ret || (reg_value != ADXL38x_RESET_PART_ID))
		goto error_com;

	/* Set device type 380/382 */
	ret = adxl38x_read_device_data(dev, ADXL38x_MISC0,
				       1, &reg_value);
	if ((reg_value & 0x80) >> 7 == 0x00) {
		dev->dev_type = ID_ADXL380;
	} else {
		dev->dev_type = ID_ADXL382;
	}

	/* Set default values to the device structure */
	dev->range = ((ADXL38x_RESET_ZERO & ADXL38x_MASK_RANGE) >> 6) & 0x0F;
	dev->op_mode = ADXL38x_RESET_ZERO & ADXL38x_MASK_OP_MODE;

	*device = dev;

	return ret;
error_com:
	if (dev->comm_type == ADXL38x_SPI_COMM)
		no_os_spi_remove(dev->com_desc.spi_desc);
	else
		no_os_i2c_remove(dev->com_desc.i2c_desc);
	no_os_free(dev);
	return -1;
error_dev:
	no_os_free(dev);
	return ret;
}


/***************************************************************************//**
 * @brief Free the resources allocated by the init function.
 *
 * @param dev - The device structure.
 *
 * @return ret - Result of the remove procedure.
*******************************************************************************/
int adxl38x_remove(struct adxl38x_dev *dev)
{
	int ret;

	if (dev->comm_type == ADXL38x_SPI_COMM)
		ret = no_os_spi_remove(dev->com_desc.spi_desc);
	else
		ret = no_os_i2c_remove(dev->com_desc.i2c_desc);

	no_os_free(dev);

	return ret;
}


/***************************************************************************//**
 * TODO: Reset time delay not matching datasheet
 * @brief Performs a soft reset of the device.
 *
 * @param dev  - The device structure.
 *
 * @return ret - Result of the soft reset procedure.
*******************************************************************************/
int adxl38x_soft_reset(struct adxl38x_dev *dev)
{
	uint8_t reg_value;
	int ret;
	uint8_t data = ADXL38x_RESET_CODE;
	union adxl38x_sts_reg_flags flags;

	// Perform soft reset
	ret = adxl38x_write_device_data(dev, ADXL38x_REG_RESET, 1, &data);
	if (ret)
		return ret;
	// Delay is needed between soft reset and initialization
	no_os_udelay(1000);
	ret = adxl38x_read_device_data(dev, ADXL38x_DEVID_AD, 1, &reg_value);
	if (reg_value != 0xAD)
		return -EAGAIN;

	return ret;
}

/***************************************************************************//**
 * TODO: Change this function so that it works for multiple ranges and different modes
 * @brief Places the device into the given operation mode.
 *
 * @param dev     - The device structure.
 * @param op_mode - Operation mode mode.
 *
 * @return ret    - Result of the setting operation procedure.
*******************************************************************************/
int adxl38x_set_op_mode(struct adxl38x_dev *dev, enum adxl38x_op_mode op_mode)
{
	int ret;
	uint8_t op_mode_reg_val;

	ret = adxl38x_set_to_standby(dev);
	if (ret)
		return ret;
	ret = adxl38x_read_device_data(dev, ADXL38x_OP_MODE, 1, &op_mode_reg_val);
	op_mode_reg_val = (op_mode_reg_val & ~ADXL38x_MASK_OP_MODE) | op_mode;
	ret |= adxl38x_write_device_data(dev, ADXL38x_OP_MODE,
					 1, &op_mode_reg_val);

	if (!ret)
		dev->op_mode = op_mode;
	// 4ms wait in the standby mode
	no_os_mdelay(4);

	return ret;
}


/***************************************************************************//**
 * @brief Gets the current operation mode of the device
 *
 * @param dev     - The device structure.
 * @param op_mode - Read operation mode.
 *
 * @return ret    - Result of the reading operation procedure.
*******************************************************************************/
int adxl38x_get_op_mode(struct adxl38x_dev *dev,
			enum adxl38x_op_mode *op_mode)
{
	int ret;
	uint8_t op_mode_reg_val[1];

	ret = adxl38x_read_device_data(dev, ADXL38x_OP_MODE, 1, &op_mode_reg_val);
	*op_mode = op_mode_reg_val[0] & ADXL38x_MASK_OP_MODE;

	if (!ret)
		dev->op_mode = *op_mode;

	return ret;
}

/***************************************************************************//**
 * @brief Sets the measurement range register value.
 *
 * @param dev       - The device structure.
 * @param range_val - Selected range.
 *
 * @return ret      - Result of the writing procedure.
*******************************************************************************/
int adxl38x_set_range(struct adxl38x_dev *dev, enum adxl38x_range range_val)
{
	int ret;
	uint8_t range_reg_val[1];

	ret = adxl38x_read_device_data(dev, ADXL38x_OP_MODE, 1, &range_reg_val);
	if (ret)
		return ret;
	ret = adxl38x_set_to_standby(dev);
	if (ret)
		return ret;
	// Reset range bits
	range_reg_val[0] &= ~ADXL38x_MASK_RANGE;
	range_val = range_val << 6;
	// Set only range bits inside op_mode register
	range_reg_val[0] |= range_val & ADXL38x_MASK_RANGE;
	ret = adxl38x_write_device_data(dev, ADXL38x_OP_MODE, 1, &range_reg_val);

	if (!ret)
		dev->range = range_val >> 6;

	return ret;
}

/***************************************************************************//**
 * @brief Gets the current range setting of the device
 *
 * @param dev       - The device structure.
 * @param range_val - Read range.
 *
 * @return ret    - Result of the reading operation procedure.
*******************************************************************************/
int adxl38x_get_range(struct adxl38x_dev *dev, enum adxl38x_range *range_val)
{
	int ret;
	uint8_t range_reg_val[1];

	ret = adxl38x_read_device_data(dev, ADXL38x_OP_MODE, 1, &range_reg_val);
	*range_val = range_reg_val[0] >> 6;

	if (!ret)
		dev->range = *range_val;

	return ret;
}


/***************************************************************************//**
 * @brief Gets the current device type
 *
 * @param dev     - The device structure
 * @param devID   - Read Device ID
 *
 * @return ret    - Result of the reading operation procedure.
*******************************************************************************/
int adxl38x_get_deviceID(struct adxl38x_dev *dev,
			 enum adxl38x_device_variant *devID)
{
	int ret;
	uint8_t misc0_reg;

	ret = adxl38x_read_device_data(dev, ADXL38x_MISC0,1, &misc0_reg);
	if ((misc0_reg & 0x80) >> 7 == 0x00)
		dev->dev_type = ID_ADXL380;
	else
		dev->dev_type = ID_ADXL382;
	*devID = dev->dev_type;

	return ret;
}


/***************************************************************************//**
 * NOTE: It is recommended that the ADXL380 self-test should be performed in mid
 * and high gee ranges and in high power mode.
 * @brief Sets the part to execute self-test routine
 *
 * @param dev  - The device structure.
 * @param st_mode - Enable/disable bit for self test.
 * @param st_force - Enable/disable bit for forced self test.
 * @param st_dir - Direction bit for self test. 0 - Forward, 1 - Reverse.
 *
 * @return ret - Result of the writing procedure.
*******************************************************************************/
int adxl38x_set_self_test_registers(struct adxl38x_dev *dev, bool st_mode,
				    bool st_force, bool st_dir)
{
	int ret;
	uint8_t data;

	ret = adxl38x_read_device_data(dev, ADXL38x_SNSR_AXIS_EN, 1, &data);
	if(!ret)
		return ret;
	data = data & 0xE0;
	data = (st_mode << 7) | (st_force << 6) | (st_dir << 5) | data;

	ret = adxl38x_write_device_data(dev, ADXL38x_SNSR_AXIS_EN, 1, &data);

	return ret;
}


/***************************************************************************//**
 * NOTE: It is recommended that the ADXL380 self-test should be performed in mid
 * and high gee ranges and in high power mode.
 * @brief Resets the self test registers for the part.
 *
 * @param dev  - The device structure.
 *
 * @return ret - Result of the writing procedure.
*******************************************************************************/
int adxl38x_clear_self_test_registers(struct adxl38x_dev *dev)
{
	int ret;
	uint8_t data;

	ret = adxl38x_read_device_data(dev, ADXL38x_SNSR_AXIS_EN, 1, &data);
	if(!ret)
		return ret;
	data = data & 0xE0;
	ret = adxl38x_write_device_data(dev, ADXL38x_SNSR_AXIS_EN, 1, &data);
	if(!ret)
		return ret;

	return ret;
}


/***************************************************************************//**
 * TODO: Update according to datasheet (Currently based on RevH)
 * @brief Reads the status registers
 *
 * @param dev          - The device structure.
 * @param status_flags - Combined Status value.
 *
 * @return ret         - 32-bit status value as a combination of status 0 to 3
 * 						 registers.
*******************************************************************************/
int adxl38x_get_sts_reg(struct adxl38x_dev *dev,
			union adxl38x_sts_reg_flags *status_flags)
{
	int ret;
	uint32_t status_reset = 0x80000400;
	//uint32_t status_value;
	uint8_t status_value[4];

	ret = adxl38x_read_device_data(dev, ADXL38x_STATUS0, 4, &status_value);
	if (ret != status_reset)
		status_flags->value = status_value[0]<<24 | status_value[1]<<16 |
				      status_value[2]<<8 | status_value[3];

	return ret;
}


/***************************************************************************//**
 * @brief Reads the raw output data using burst read.
 *
 * @param dev   - The device structure.
 * @param raw_x - X-axis's raw output data.
 * @param raw_y - Y-axis's raw output data.
 * @param raw_z - Z-axis's raw output data.
 *
 * @return ret  - Result of the reading procedure.
*******************************************************************************/
int adxl38x_get_raw_xyz(struct adxl38x_dev *dev, uint32_t *raw_x,
			uint32_t *raw_y, uint32_t *raw_z)
{
	uint8_t array_raw_data[6] = {0};
	int ret;
	uint8_t write_data;
	uint8_t op_mode;

	//Enable accel channels
	write_data = 0x70;
	ret = adxl38x_write_device_data(dev, ADXL38x_DIG_EN, 1, &write_data);
	if (ret)
		return ret;
	//Put part in measurement mode if not placed in it already
	ret = adxl38x_read_device_data(dev, ADXL38x_OP_MODE, 1, &op_mode);
	if (!ret) {
		ret = adxl38x_get_op_mode(dev, &op_mode);
		if(op_mode == 0) {
			adxl38x_set_op_mode(dev, ADXL38x_MODE_LP);
		}
	}
	//Read accel data channels
	ret = adxl38x_read_device_data(dev, ADXL38x_XDATA_H, 6, array_raw_data);
	if (ret)
		return ret;
	*raw_x = adxl38x_accel_array_conv(array_raw_data);
	*raw_y = adxl38x_accel_array_conv(array_raw_data + 2);
	*raw_z = adxl38x_accel_array_conv(array_raw_data + 4);

	return ret;
}


/***************************************************************************//**
 * @brief Reads the raw temperature data using burst read.
 *
 * @param dev   - The device structure.
 * @param raw_temp - Raw temperature data.
 *
 * @return ret  - Result of the reading procedure.
*******************************************************************************/
int adxl38x_get_temp(struct adxl38x_dev *dev,
		     struct adxl38x_fractional_val *raw_temp)
{
	uint8_t array_raw_data[2] = {0};
	int ret;
	int j = 0;
	uint8_t write_data;
	uint8_t op_mode;
	uint8_t dig_en;

	ret = adxl38x_read_device_data(dev, ADXL38x_DIG_EN, 1, &dig_en);
	write_data = dig_en;
	write_data = write_data & ~ADXL38x_MASK_CHEN_DIG_EN;
	write_data = write_data | ((ADXL38x_CH_EN_T << 4) & ADXL38x_MASK_CHEN_DIG_EN);

	ret = adxl38x_write_device_data(dev, ADXL38x_DIG_EN, 1, &write_data);
	if (ret)
		return ret;
	//Put part in measurement mode if not placed in it already
	ret = adxl38x_read_device_data(dev, ADXL38x_OP_MODE, 1, &op_mode);
	ret = adxl38x_get_op_mode(dev, &op_mode);
	if(op_mode == 0) {
		adxl38x_set_op_mode(dev, ADXL38x_MODE_HP);
	} else {
		adxl38x_set_op_mode(dev, op_mode);
	}

	//Read accel data channels
	ret = adxl38x_read_device_data(dev, ADXL38x_TDATA_H, 2, array_raw_data);
	if (ret)
		return ret;
	raw_temp->integer = adxl38x_accel_array_conv(array_raw_data);
	//LSB to C conversion
	raw_temp->integer = raw_temp->integer - ADXL38x_TEMP_OFFSET;
	raw_temp->fractional = raw_temp->integer % ADXL38x_TEMP_SCALE;
	raw_temp->integer = raw_temp->integer / ADXL38x_TEMP_SCALE;

	return ret;
}


/***************************************************************************//**
 * @brief Reads the raw output data using burst read.
 *
 * @param dev   - The device structure.
 * @param channels - Channels to enable.
 * @param raw_x - X-axis's raw output data.
 * @param raw_y - Y-axis's raw output data.
 * @param raw_z - Z-axis's raw output data.
 * @param raw_temp - Raw temp output data.
 *
 * @return ret  - Result of the reading procedure.
*******************************************************************************/
int adxl38x_get_raw_data(struct adxl38x_dev *dev,
			 enum adxl38x_ch_select channels,
			 uint16_t *raw_x, uint16_t *raw_y,
			 uint16_t *raw_z, uint16_t *raw_temp)
{
	uint8_t array_raw_data[8] = {0};
	uint8_t array_rearranged_data[8] = {0};
	int ret;
	int j = 0;
	uint8_t write_data;
	uint8_t op_mode;
	uint8_t dig_en;
	uint8_t tzyx, start_addr;
	uint16_t num_bytes = 0;

	ret = adxl38x_read_device_data(dev, ADXL38x_DIG_EN, 1, &dig_en);
	write_data = dig_en;
	write_data = write_data & ~ADXL38x_MASK_CHEN_DIG_EN;
	write_data = write_data | ((channels << 4) & ADXL38x_MASK_CHEN_DIG_EN);

	ret = adxl38x_write_device_data(dev, ADXL38x_DIG_EN, 1, &write_data);
	if (ret)
		return ret;
	//Put part in measurement mode if not placed in it already
	ret = adxl38x_read_device_data(dev, ADXL38x_OP_MODE, 1, &op_mode);
	ret = adxl38x_get_op_mode(dev, &op_mode);
	if(op_mode == 0) {
		adxl38x_set_op_mode(dev, ADXL38x_MODE_HP);
	} else {
		adxl38x_set_op_mode(dev, op_mode);
	}

	//Find the channels and number of bytes to read
	tzyx = (uint8_t)channels;
	if(tzyx & 0x01) {
		start_addr = ADXL38x_XDATA_H;
	} else if(tzyx & 0x02) {
		start_addr = ADXL38x_YDATA_H;
	} else if(tzyx & 0x04) {
		start_addr = ADXL38x_ZDATA_H;
	} else {
		start_addr = ADXL38x_TDATA_H;
	}
	while(tzyx) {
		num_bytes += tzyx & 0x01;
		tzyx >>= 1;
	}
	num_bytes = num_bytes * 2;

	//Read accel data channels
	ret = adxl38x_read_device_data(dev, start_addr, num_bytes, array_raw_data);
	if (ret)
		return ret;
	//Re-assign raw data to corresponding channels
	tzyx = (uint8_t)channels;
	for(int i = 0; i <  8; i+=2) {
		if(tzyx & 0x01) {
			array_rearranged_data[i] = array_raw_data[j];
			array_rearranged_data[i+1] = array_raw_data[j+1];
			j += 2;
		} else {
			array_rearranged_data[i] = NULL;
			array_rearranged_data[i+1] = NULL;
		}
		tzyx >>= 1;
	}
	*raw_x = adxl38x_accel_array_conv(array_rearranged_data);
	*raw_y = adxl38x_accel_array_conv(array_rearranged_data + 2);
	*raw_z = adxl38x_accel_array_conv(array_rearranged_data + 4);
	*raw_temp = adxl38x_accel_array_conv(array_rearranged_data + 6);

	return ret;
}


/***************************************************************************//**
 * @brief Reads the raw output data of each axis and converts it to g.
 *
 * @param dev  - The device structure.
 * @param channels - Channels to enable.
 * @param x    - X-axis's output data.
 * @param y    - Y-axis's output data.
 * @param z    - Z-axis's output data.
 *
 * @return ret - Result of the reading procedure.
*******************************************************************************/
int adxl38x_get_xyz_gees(struct adxl38x_dev *dev,
			 enum adxl38x_ch_select channels,
			 struct adxl38x_fractional_val *x, struct adxl38x_fractional_val *y,
			 struct adxl38x_fractional_val *z)
{
	uint16_t raw_accel_x;
	uint16_t raw_accel_y;
	uint16_t raw_accel_z;
	uint16_t raw_accel_t;
	uint16_t dum;
	int ret;

	int64_t dummy;

	ret = adxl38x_get_raw_data(dev, channels, &raw_accel_x, &raw_accel_y,
				   &raw_accel_z, &raw_accel_t);

	if (ret)
		return ret;

	x->integer = no_os_div_s64_rem((int64_t)adxl38x_accel_conv(dev, raw_accel_x),
				       ADXL38x_ACC_SCALE_FACTOR_GEE_DIV, &(x->fractional));
	y->integer = no_os_div_s64_rem((int64_t)adxl38x_accel_conv(dev, raw_accel_y),
				       ADXL38x_ACC_SCALE_FACTOR_GEE_DIV, &(y->fractional));
	z->integer = no_os_div_s64_rem((int64_t)adxl38x_accel_conv(dev, raw_accel_z),
				       ADXL38x_ACC_SCALE_FACTOR_GEE_DIV, &(z->fractional));

	return ret;
}


/***************************************************************************//**
 * @brief Converts array of raw acceleration to uint16 data raw acceleration
 *
 * @param dev       - The device structure.
 * @param raw_array - Raw acceleration array.
 *
 * @return ret      - Converted data.
*******************************************************************************/
static uint32_t adxl38x_accel_array_conv(uint8_t *raw_array)
{
	return raw_array[1] | ((uint16_t)raw_array[0] << 8);
}

/***************************************************************************//**
 * @brief Converts raw acceleration value to m/s^2 value.
 *
 * @param dev       - The device structure.
 * @param raw_accel - Raw acceleration value.
 *
 * @return ret      - Converted data.
*******************************************************************************/
static int64_t adxl38x_accel_conv(struct adxl38x_dev *dev,
				  uint16_t raw_accel)
{
	int32_t accel_data;

	// Raw acceleration is in two's complement
	// Convert from two's complement to int
	if ((raw_accel & NO_OS_BIT(15)) == NO_OS_BIT(15))
		accel_data = raw_accel | ADXL38x_NEG_ACC_MSK;
	else
		accel_data = raw_accel;

	// Apply scale factor based on the selected range
	switch (dev->dev_type) {
	case ID_ADXL380:
		//return accel_data;
		return ((int64_t)(accel_data * ADXL380_ACC_SCALE_FACTOR_GEE_MUL *
				  adxl38x_scale_mul[dev->range]));
	case ID_ADXL382:
		return ((int64_t)(accel_data * ADXL382_ACC_SCALE_FACTOR_GEE_MUL *
				  adxl38x_scale_mul[dev->range]));
	default:
		return 0;
	}
}

/***************************************************************************//**
 * @brief Puts the part in a safe state before setting important register values.
 *
 * @param dev       - The device structure.
 *
 * @return ret      - Result of setting to stanby mode.
*******************************************************************************/
static int adxl38x_set_to_standby(struct adxl38x_dev *dev)
{
	int ret;
	enum adxl38x_op_mode op_mode = ADXL38x_MODE_STDBY;
	uint8_t op_mode_reg_val;

	ret = adxl38x_read_device_data(dev, ADXL38x_OP_MODE, 1, &op_mode_reg_val);
	op_mode_reg_val = (op_mode_reg_val & ~ADXL38x_MASK_OP_MODE) | op_mode;
	ret |= adxl38x_write_device_data(dev, ADXL38x_OP_MODE,
					 1, &op_mode_reg_val);
	// 10ms wait in the standby mode
	no_os_mdelay(10);

	if (!ret)
		dev->op_mode = op_mode;

	return ret;
}


/***************************************************************************//**
 * @brief Executed Selftest on the sensing axes and returns the outcome of
 * the test.
 *
 * @param dev   - The device structure.
 * @param op_mode  - Operation mode in which self-test is executed
 * @param st_x 	- Result of X-axis self test success/failure
 * @param st_y 	- Result of X-axis self test success/failure
 * @param st_z 	- Result of X-axis self test success/failure
 *
 * @return ret  - Outcome of the selftest.
*******************************************************************************/
int adxl38x_selftest(struct adxl38x_dev *dev, enum adxl38x_op_mode op_mode,
		     bool *st_x, bool *st_y, bool *st_z)
{
	int ret;
	uint8_t write_data;
	uint8_t array_raw_data[6] = {0};
	uint8_t array_rearranged_data[6] = {0};
	int32_t low_limit_xy, low_limit_z;
	int32_t up_limit_xy, up_limit_z;
	int32_t st_delta_data[3] = {0};
	int32_t st_positive_data[3] = {0};
	int32_t st_negative_data[3] = {0};
	int64_t x_sum = 0;
	int64_t y_sum = 0;
	int64_t z_sum = 0;
	*st_x = false;
	*st_y = false;
	*st_z = false;

	/* Enter standby mode */
	ret = adxl38x_set_op_mode(dev, ADXL38x_MODE_STDBY);

	/* Enable desired channels (except temperature) */
	if(op_mode == ADXL38x_MODE_HRT_SND) {
		write_data = (ADXL38x_CH_EN_X << 4);  //Only one axis
	} else
		write_data = (ADXL38x_CH_EN_XYZ << 4);
	ret = adxl38x_write_device_data(dev, ADXL38x_DIG_EN, 1, &write_data);
	if (ret)
		return ret;

	/* Enable desired OP mode */
	ret = adxl38x_set_op_mode(dev, op_mode);

	/* Enable ST mode, force and positive direction */
	ret = adxl38x_read_device_data(dev, ADXL38x_SNSR_AXIS_EN, 1, &write_data);
	write_data = (write_data & 0x3F) | 0xC0;
	ret = adxl38x_write_device_data(dev, ADXL38x_SNSR_AXIS_EN, 1, &write_data);

	/* Measure output on all three axes for at least 25 samples and compute average*/
	for(int k = 0; k < 25; k++) {
		no_os_mdelay(10);
		ret = adxl38x_read_device_data(dev, ADXL38x_XDATA_H, 6, array_raw_data);
		int j = 0;
		for(int i = 0; i <  6; i+=2) {
			array_rearranged_data[i] = array_raw_data[j];
			array_rearranged_data[i+1] = array_raw_data[j+1];
			j += 2;
		}
		x_sum += adxl38x_accel_two_comp_conv_s64(array_rearranged_data[1] | ((
					uint16_t)array_rearranged_data[0] << 8));
		y_sum += adxl38x_accel_two_comp_conv_s64(array_rearranged_data[3] | ((
					uint16_t)array_rearranged_data[2] << 8));
		z_sum += adxl38x_accel_two_comp_conv_s64(array_rearranged_data[5] | ((
					uint16_t)array_rearranged_data[4] << 8));
	}
	st_positive_data[0] = (int32_t)no_os_div_s64(x_sum, 25);
	st_positive_data[1] = (int32_t)no_os_div_s64(y_sum, 25);
	st_positive_data[2] = (int32_t)no_os_div_s64(z_sum, 25);

	/* Enable ST mode, force and negative direction */
	ret = adxl38x_read_device_data(dev, ADXL38x_SNSR_AXIS_EN, 1, &write_data);
	write_data = (write_data & 0x3F) | 0xE0;
	ret = adxl38x_write_device_data(dev, ADXL38x_SNSR_AXIS_EN, 1, &write_data);

	/* Measure output on all three axes for at lest 25 samples and compute average*/
	x_sum = 0;
	y_sum = 0;
	z_sum = 0;
	for(int k = 0; k < 25; k++) {
		no_os_mdelay(10);
		ret = adxl38x_read_device_data(dev, ADXL38x_XDATA_H, 6, array_raw_data);
		int j = 0;
		for(int i = 0; i <  6; i+=2) {
			array_rearranged_data[i] = array_raw_data[j];
			array_rearranged_data[i+1] = array_raw_data[j+1];
			j += 2;
		}
		x_sum += adxl38x_accel_two_comp_conv_s64(array_rearranged_data[1] | ((
					uint16_t)array_rearranged_data[0] << 8));
		y_sum += adxl38x_accel_two_comp_conv_s64(array_rearranged_data[3] | ((
					uint16_t)array_rearranged_data[2] << 8));
		z_sum += adxl38x_accel_two_comp_conv_s64(array_rearranged_data[5] | ((
					uint16_t)array_rearranged_data[4] << 8));
		no_os_mdelay(10);
	}
	st_negative_data[0] = (int32_t)no_os_div_s64(x_sum, 25);
	st_negative_data[1] = (int32_t)no_os_div_s64(y_sum, 25);
	st_negative_data[2] = (int32_t)no_os_div_s64(z_sum, 25);

	/* Compute self-test delta */
	// Multiply self test denomitator for the values to be comparable
	for(int k = 0; k < 3; k++) {
		st_delta_data[k] = (int32_t) abs(st_positive_data[k] - st_negative_data[k]);
		st_delta_data[k] = st_delta_data[k] * ADXL38x_ST_LIMIT_DENOMINATOR;
	}

	/* Compare self-test delta magnitude with Datasheet values */
	// Apply scale factor based on the selected range
	switch (dev->dev_type) {
	case ID_ADXL380:
		low_limit_xy = (ADXL380_XY_ST_LIMIT_MIN * ADXL380_ACC_SENSITIVITY) >>
			       (dev->range);
		up_limit_xy = (ADXL380_XY_ST_LIMIT_MAX * ADXL380_ACC_SENSITIVITY) >>
			      (dev->range);
		low_limit_z = (ADXL380_Z_ST_LIMIT_MIN * ADXL380_ACC_SENSITIVITY) >>
			      (dev->range);
		up_limit_z = (ADXL380_Z_ST_LIMIT_MAX * ADXL380_ACC_SENSITIVITY) >> (dev->range);
		break;
	case ID_ADXL382:
		low_limit_xy = (ADXL382_XY_ST_LIMIT_MIN * ADXL382_ACC_SENSITIVITY) >>
			       (dev->range);
		up_limit_xy = (ADXL382_XY_ST_LIMIT_MAX * ADXL382_ACC_SENSITIVITY) >>
			      (dev->range);
		low_limit_z = (ADXL382_Z_ST_LIMIT_MIN * ADXL382_ACC_SENSITIVITY) >>
			      (dev->range);
		up_limit_z = (ADXL382_Z_ST_LIMIT_MAX * ADXL382_ACC_SENSITIVITY) >> (dev->range);
		break;
	default:
		return -1;
	}

	if(st_delta_data[0] >= low_limit_xy && st_delta_data[0] <= up_limit_xy)
		*st_x = true;
	if(st_delta_data[1] >= low_limit_xy && st_delta_data[1] <= up_limit_xy)
		*st_y = true;
	if(st_delta_data[2] >= low_limit_z && st_delta_data[2] <= up_limit_z)
		*st_z = true;

	/* Reset ST bits */
	ret = adxl38x_read_device_data(dev, ADXL38x_SNSR_AXIS_EN, 1, &write_data);
	write_data = (write_data & 0x1F);
	ret = adxl38x_write_device_data(dev, ADXL38x_SNSR_AXIS_EN, 1, &write_data);

	return ret;
}


/***************************************************************************//**
 * @brief Converts raw acceleration value to m/s^2 value.
 *
 * @param dev       - The device structure.
 * @param raw_accel - Raw acceleration value.
 *
 * @return ret      - Converted data.
*******************************************************************************/
static int64_t adxl38x_accel_two_comp_conv_s64(uint16_t raw_accel)
{
	// Raw acceleration is in two's complement
	// Convert from two's complement to int
	int64_t accel_data;
	if ((raw_accel & NO_OS_BIT(15)) == NO_OS_BIT(15))
		accel_data = raw_accel | 0xffffffffffff0000;
	else
		accel_data = raw_accel;

	return accel_data;
}



/***************************************************************************//**
 * @brief Function to set the paramenters for FIFO mode
 *
 * @param dev        - The device structure.
 * @param numSamples - Number of FIFO entries that FIFI_WATERMARK should set.
 * @param externalTrigger - Enable/disable external trigger in FIFO stream mode.
 * @param fifoMode        - FIFO mode setting.
 * @param chIDEnable      - Enable/disable channel ID.
 * @param readReset       - reset read/write point and read state machine.
 *
 * @return ret      - Converted data.
*******************************************************************************/
int adxl38x_accel_set_FIFO( struct adxl38x_dev *dev, uint16_t numSamples,
			    bool externalTrigger, uint8_t fifoMode, bool chIDEnable,
			    bool readReset)
{
	int ret;
	uint8_t write_data = 0;
	uint8_t fifo_samples_low;
	uint8_t fifo_samples_high;

	// set FIFO_CFG1 register
	fifo_samples_low = (uint8_t) numSamples;
	ret = adxl38x_write_device_data(dev, ADXL38x_FIFO_CFG1, 1, &fifo_samples_low);
	// building data for FIFO_CFG0 register
	fifoMode = (fifoMode << 4) & 0x30;
	fifo_samples_high = (uint8_t) numSamples >> 8;
	fifo_samples_high = fifo_samples_high & 0x01;
	if(readReset)
		write_data = 1u << 7;
	if(chIDEnable)
		write_data |= 1u << 6;
	write_data |= fifoMode;
	if(externalTrigger && fifoMode == 3)
		write_data |= 1u << 3;
	write_data |= fifo_samples_high;
	ret |= adxl38x_write_device_data(dev, ADXL38x_FIFO_CFG0, 1, &write_data);

	return ret;
}

