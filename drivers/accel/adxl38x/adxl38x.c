/***************************************************************************//**
 *   @file   adxl38x.c
 *   @brief  Implementation of ADXL38X Driver.
 *   @author Balarupini Rajendran (balarupini.rajendran@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
static int64_t adxl38x_accel_conv(struct adxl38x_dev *dev, uint16_t raw_accel);
static int adxl38x_set_to_standby(struct adxl38x_dev *dev);

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
	uint16_t idx;

	if (dev->comm_type == ADXL38X_SPI_COMM) {
		buffer[0] = (base_address << 1) | ADXL38X_SPI_READ;
		ret = no_os_spi_write_and_read(dev->com_desc.spi_desc, buffer,
					       size + 1);
		if(ret)
			return ret;
		for (idx = 0; idx < size; idx++)
			read_data[idx] = buffer[idx + 1];
	} else {
		ret = no_os_i2c_write(dev->com_desc.i2c_desc, &base_address, 1, 0);
		if(ret)
			return ret;
		ret = no_os_i2c_read(dev->com_desc.i2c_desc, read_data, size, 1);
		if(ret)
			return ret;
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
	uint8_t buffer[64] = {0};
	uint16_t idx;

	for (idx = 0; idx < size; idx++)
		buffer[1 + idx] = write_data[idx];

	if (dev->comm_type == ADXL38X_SPI_COMM) {
		buffer[0] = ADXL38X_SPI_WRITE | (base_address << 1);
		return no_os_spi_write_and_read(dev->com_desc.spi_desc, buffer,
						size + 1);
	} else {
		dev->comm_buff[0] = base_address;
		return no_os_i2c_write(dev->com_desc.i2c_desc, dev->comm_buff, size + 1, 1);
	}
}

/***************************************************************************//**
 * @brief Updates register with specified bits using given mask
 *
 * @param dev          	- The device structure.
 * @param reg_addr 		- Address of the register to update.
 * @param mask         	- Mask for the update field.
 * @param update_val   	- Value to be updated.
 *
 * @return ret         	- Result of the writing procedure.
*******************************************************************************/
int adxl38x_register_update_bits(struct adxl38x_dev *dev, uint8_t reg_addr,
				 uint8_t mask,
				 uint8_t update_val)
{
	int ret;
	uint8_t data;

	ret = adxl38x_read_device_data(dev, reg_addr, 1, &data);
	if (ret)
		return ret;

	// Handle case where the OP_MODE register is accessed. Needs to go to
	// standby before changing any bits of the register. Needed for a safe
	// transition.
	if(reg_addr == ADXL38X_OP_MODE)
		adxl38x_set_to_standby(dev);

	data &= ~mask;
	data |= update_val;

	return adxl38x_write_device_data(dev, reg_addr, 1, &data);
}

/***************************************************************************//**
 * @brief Initializes the device and checks for valid peripheral communication
 *
 * @param device        - The device structure.
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

	dev = (struct adxl38x_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	switch (init_param.dev_type) {
	case ID_ADXL380:
	case ID_ADXL382:
		dev->dev_type = init_param.dev_type;
		break;
	default:
		return -EINVAL;
	}

	dev->comm_type = init_param.comm_type;
	if (dev->comm_type == ADXL38X_SPI_COMM) {
		ret = no_os_spi_init(&dev->com_desc.spi_desc, &init_param.comm_init.spi_init);
		if (ret)
			goto error_dev;
	} else {
		ret = no_os_i2c_init(&dev->com_desc.i2c_desc, &init_param.comm_init.i2c_init);
		if (ret)
			goto error_dev;
	}

	ret = adxl38x_read_device_data(dev, ADXL38X_DEVID_AD,
				       1, &reg_value);
	if (ret || (reg_value != ADXL38X_RESET_DEVID_AD))
		goto error_com;

	ret = adxl38x_read_device_data(dev, ADXL38X_DEVID_MST,
				       1, &reg_value);
	if (ret || (reg_value != ADXL38X_RESET_DEVID_MST))
		goto error_com;

	ret = adxl38x_read_device_data(dev, ADXL38X_PART_ID,
				       1, &reg_value);
	if (ret || (reg_value != ADXL38X_RESET_PART_ID))
		goto error_com;

	/* Set device type 380/382 */
	ret = adxl38x_read_device_data(dev, ADXL38X_MISC0,
				       1, &reg_value);
	if (ret)
		goto error_com;
	if ((reg_value & NO_OS_BIT(7))) {
		dev->dev_type = ID_ADXL380;
	} else {
		dev->dev_type = ID_ADXL382;
	}

	/* Set default values to the device structure */
	dev->range = ADXL380_RANGE_4G;
	dev->op_mode = ADXL38X_MODE_STDBY;
	*device = dev;

	return 0;

error_com:
	if (dev->comm_type == ADXL38X_SPI_COMM)
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
 * @param dev  - The device structure.
 *
 * @return ret - Result of the remove procedure.
*******************************************************************************/
int adxl38x_remove(struct adxl38x_dev *dev)
{
	int ret;

	if (dev->comm_type == ADXL38X_SPI_COMM)
		ret = no_os_spi_remove(dev->com_desc.spi_desc);
	else
		ret = no_os_i2c_remove(dev->com_desc.i2c_desc);
	no_os_free(dev);

	return ret;
}

/***************************************************************************//**
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
	uint8_t data = ADXL38X_RESET_CODE;

	// Perform soft reset
	ret = adxl38x_write_device_data(dev, ADXL38X_REG_RESET, 1, &data);
	if (ret)
		return ret;
	// Delay is needed between soft reset and initialization (From SOFT_RESET
	// bit description in REG_RESET)
	no_os_udelay(500);
	ret = adxl38x_read_device_data(dev, ADXL38X_DEVID_AD, 1, &reg_value);
	if (reg_value != 0xAD)
		return -EAGAIN;

	return 0;
}

/***************************************************************************//**
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

	ret = adxl38x_register_update_bits(dev, ADXL38X_OP_MODE,
					   ADXL38X_MASK_OP_MODE,
					   no_os_field_prep(ADXL38X_MASK_OP_MODE, op_mode));
	if (!ret)
		dev->op_mode = op_mode;
	// 2ms wait for op_mode to settle (See Operating Modes section of datasheet)
	no_os_mdelay(2);

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
int adxl38x_get_op_mode(struct adxl38x_dev *dev, enum adxl38x_op_mode *op_mode)
{
	int ret;
	uint8_t op_mode_reg_val;

	ret = adxl38x_read_device_data(dev, ADXL38X_OP_MODE, 1, &op_mode_reg_val);
	if (ret)
		return ret;
	*op_mode = no_os_field_get(ADXL38X_MASK_OP_MODE, op_mode_reg_val);

	if (!ret)
		dev->op_mode = *op_mode;

	return 0;
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

	ret = adxl38x_register_update_bits(dev, ADXL38X_OP_MODE,
					   ADXL38X_MASK_RANGE,
					   no_os_field_prep(ADXL38X_MASK_RANGE, range_val));
	if (!ret)
		dev->range = range_val;

	return ret;
}

/***************************************************************************//**
 * @brief Gets the current range setting of the device
 *
 * @param dev       - The device structure.
 * @param range_val - Read range.
 *
 * @return ret    	- Result of the reading operation procedure.
*******************************************************************************/
int adxl38x_get_range(struct adxl38x_dev *dev, enum adxl38x_range *range_val)
{
	int ret;
	uint8_t range_reg_val;

	ret = adxl38x_read_device_data(dev, ADXL38X_OP_MODE, 1, &range_reg_val);
	if (ret)
		return ret;
	*range_val = no_os_field_get(ADXL38X_MASK_RANGE, range_reg_val);

	if (!ret)
		dev->range = *range_val;

	return 0;
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
			 enum adxl38x_id *devID)
{
	int ret;
	uint8_t misc0_reg;

	ret = adxl38x_read_device_data(dev, ADXL38X_MISC0,1, &misc0_reg);
	if (misc0_reg & NO_OS_BIT(7))
		dev->dev_type = ID_ADXL382;
	else
		dev->dev_type = ID_ADXL380;
	*devID = dev->dev_type;

	return ret;
}

/***************************************************************************//**
 * NOTE: It is recommended that the ADXL380 self-test should be performed in mid
 * and high gee ranges and in high power mode.
 * @brief Sets the part to execute self-test routine
 *
 * @param dev  		- The device structure.
 * @param st_mode 	- Enable/disable bit for self test.
 * @param st_force 	- Enable/disable bit for forced self test.
 * @param st_dir 	- Direction bit for self test. 0 - Forward, 1 - Reverse.
 *
 * @return ret 		- Result of the writing procedure.
*******************************************************************************/
int adxl38x_set_self_test_registers(struct adxl38x_dev *dev, bool st_mode,
				    bool st_force, bool st_dir)
{
	int ret;
	uint8_t st_fields_value = 0;

	if(st_mode)
		st_fields_value |= NO_OS_BIT(7);
	if(st_force)
		st_fields_value |= NO_OS_BIT(6);
	if(st_dir)
		st_fields_value |= NO_OS_BIT(5);

	ret = adxl38x_register_update_bits(dev, ADXL38X_SNSR_AXIS_EN,
					   ADXL38X_SLF_TST_CTRL_MSK, st_fields_value);

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

	ret = adxl38x_register_update_bits(dev, ADXL38X_SNSR_AXIS_EN,
					   ADXL38X_SLF_TST_CTRL_MSK, ADXL38X_RESET_ZERO);

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
	uint8_t status_value[4];

	ret = adxl38x_read_device_data(dev, ADXL38X_STATUS0, 4, status_value);
	if(ret)
		return ret;
	status_flags->value = no_os_get_unaligned_be32(status_value);

	return 0;
}

/***************************************************************************//**
 * @brief Reads the raw output data using continuous read.
 *
 * @param dev   - The device structure.
 * @param raw_x - X-axis's raw output data.
 * @param raw_y - Y-axis's raw output data.
 * @param raw_z - Z-axis's raw output data.
 *
 * @return ret  - Result of the reading procedure.
*******************************************************************************/
int adxl38x_get_raw_xyz(struct adxl38x_dev *dev, uint16_t *raw_x,
			uint16_t *raw_y, uint16_t *raw_z)
{
	int ret;
	uint8_t array_raw_data[6] = {0};
	uint8_t op_mode;

	// Enable XYZ channels
	ret = adxl38x_register_update_bits(dev, ADXL38X_DIG_EN,
					   ADXL38X_MASK_CHEN_DIG_EN,
					   no_os_field_prep(ADXL38X_MASK_CHEN_DIG_EN, ADXL38X_CH_EN_XYZ));
	if (ret)
		return ret;

	// Put part in measurement mode if not placed in any measurement mode
	ret = adxl38x_get_op_mode(dev, &op_mode);
	if (ret)
		return ret;
	if(op_mode == 0) {
		adxl38x_set_op_mode(dev, ADXL38X_MODE_HP);
	}

	// Read accel data channels
	ret = adxl38x_read_device_data(dev, ADXL38X_XDATA_H, 6, array_raw_data);
	if (ret)
		return ret;
	*raw_x = no_os_get_unaligned_be16(array_raw_data);
	*raw_y = no_os_get_unaligned_be16(array_raw_data + 2);
	*raw_z = no_os_get_unaligned_be16(array_raw_data + 4);

	return 0;
}

/***************************************************************************//**
 * @brief Reads the raw temperature data using burst read.
 *
 * @param dev   		- The device structure.
 * @param temp_degC 	- Temperature data in degree Celcius
 *
 * @return ret  	- Result of the reading procedure.
*******************************************************************************/
int adxl38x_get_temp(struct adxl38x_dev *dev,
		     struct adxl38x_fractional_val *temp_degC)
{
	int ret;
	uint8_t op_mode;
	uint8_t array_raw_data[2] = {0};

	// Enable T channel only
	ret = adxl38x_register_update_bits(dev, ADXL38X_DIG_EN,
					   ADXL38X_MASK_CHEN_DIG_EN,
					   no_os_field_prep(ADXL38X_MASK_CHEN_DIG_EN, ADXL38X_CH_EN_T));
	if (ret)
		return ret;

	// Put part in measurement mode if not placed in any measurement mode
	ret = adxl38x_get_op_mode(dev, &op_mode);
	if (ret)
		return ret;
	if(op_mode == 0) {
		adxl38x_set_op_mode(dev, ADXL38X_MODE_HP);
	}

	//Read accel temperature channel
	ret = adxl38x_read_device_data(dev, ADXL38X_TDATA_H, 2, array_raw_data);
	if (ret)
		return ret;
	temp_degC->integer = no_os_get_unaligned_be16(array_raw_data) >> 4;

	//LSB to C conversion
	temp_degC->integer = temp_degC->integer - ADXL38X_TEMP_OFFSET;
	temp_degC->fractional = (temp_degC->integer * ADXL38X_TEMP_SCALE_DEN) %
				ADXL38X_TEMP_SCALE_NUM;
	temp_degC->integer = (temp_degC->integer * ADXL38X_TEMP_SCALE_DEN) /
			     ADXL38X_TEMP_SCALE_NUM;

	return ret;
}

/***************************************************************************//**
 * @brief Reads the raw output data using burst read.
 *
 * @param dev   	- The device structure.
 * @param channels 	- Channels to enable.
 * @param raw_x 	- X-axis's raw output data.
 * @param raw_y 	- Y-axis's raw output data.
 * @param raw_z 	- Z-axis's raw output data.
 * @param raw_temp 	- Raw temp output data.
 *
 * @return ret  	- Result of the reading procedure.
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
	uint8_t op_mode;
	uint8_t tzyx, start_addr;
	uint16_t num_bytes = 0;

	// Enable given channel(s)
	ret = adxl38x_register_update_bits(dev, ADXL38X_DIG_EN,
					   ADXL38X_MASK_CHEN_DIG_EN,
					   no_os_field_prep(ADXL38X_MASK_CHEN_DIG_EN, channels));
	if (ret)
		return ret;

	// Put part in measurement mode if not placed in any measurement mode
	ret = adxl38x_get_op_mode(dev, &op_mode);
	if (ret)
		return ret;
	if(op_mode == 0) {
		adxl38x_set_op_mode(dev, ADXL38X_MODE_HP);
	}

	//Find the channels and number of bytes to read
	tzyx = (uint8_t)channels;
	if(tzyx & 0x01) {
		start_addr = ADXL38X_XDATA_H;
	} else if(tzyx & 0x02) {
		start_addr = ADXL38X_YDATA_H;
	} else if(tzyx & 0x04) {
		start_addr = ADXL38X_ZDATA_H;
	} else {
		start_addr = ADXL38X_TDATA_H;
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
			array_rearranged_data[i] = 0;
			array_rearranged_data[i+1] = 0;
		}
		tzyx >>= 1;
	}
	if(raw_x)
		*raw_x = no_os_get_unaligned_be16(array_rearranged_data);
	if(raw_y)
		*raw_y = no_os_get_unaligned_be16(array_rearranged_data + 2);
	if(raw_z)
		*raw_z = no_os_get_unaligned_be16(array_rearranged_data + 4);
	if(raw_temp)
		*raw_temp = no_os_get_unaligned_be16(array_rearranged_data + 6) >> 4;

	return ret;
}

/***************************************************************************//**
 * @brief Reads the raw output data of each axis and converts it to g.
 *
 * @param dev  		- The device structure.
 * @param channels 	- Channels to enable.
 * @param x    		- X-axis's output data.
 * @param y    		- Y-axis's output data.
 * @param z    		- Z-axis's output data.
 *
 * @return ret 		- Result of the reading procedure.
*******************************************************************************/
int adxl38x_get_xyz_gees(struct adxl38x_dev *dev,
			 enum adxl38x_ch_select channels,
			 struct adxl38x_fractional_val *x, struct adxl38x_fractional_val *y,
			 struct adxl38x_fractional_val *z)
{
	int ret;
	uint16_t raw_accel_x;
	uint16_t raw_accel_y;
	uint16_t raw_accel_z;

	ret = adxl38x_get_raw_data(dev, channels, &raw_accel_x, &raw_accel_y,
				   &raw_accel_z, NULL);
	if (ret)
		return ret;

	x->integer = no_os_div_s64_rem((int64_t)adxl38x_accel_conv(dev, raw_accel_x),
				       ADXL38X_ACC_SCALE_FACTOR_GEE_DIV, &(x->fractional));
	y->integer = no_os_div_s64_rem((int64_t)adxl38x_accel_conv(dev, raw_accel_y),
				       ADXL38X_ACC_SCALE_FACTOR_GEE_DIV, &(y->fractional));
	z->integer = no_os_div_s64_rem((int64_t)adxl38x_accel_conv(dev, raw_accel_z),
				       ADXL38X_ACC_SCALE_FACTOR_GEE_DIV, &(z->fractional));

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
static int64_t adxl38x_accel_conv(struct adxl38x_dev *dev,
				  uint16_t raw_accel)
{
	int32_t accel_data;

	// Raw acceleration is in two's complement
	// Convert from two's complement to int
	if (raw_accel & NO_OS_BIT(15))
		accel_data = raw_accel | ADXL38X_NEG_ACC_MSK;
	else
		accel_data = raw_accel;

	// Apply scale factor based on the selected range
	switch (dev->dev_type) {
	case ID_ADXL380:
		return ((int64_t)(accel_data * ADXL380_ACC_SCALE_FACTOR_GEE_MUL *
				  adxl38x_scale_mul[dev->range]));
	case ID_ADXL382:
		return ((int64_t)(accel_data * ADXL382_ACC_SCALE_FACTOR_GEE_MUL *
				  adxl38x_scale_mul[dev->range]));
	default:
		return -EINVAL;
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
	uint8_t op_mode_reg_val;

	ret = adxl38x_read_device_data(dev, ADXL38X_OP_MODE, 1, &op_mode_reg_val);
	// Applying mask sets underlying op_mode to standby (0)
	op_mode_reg_val = (op_mode_reg_val & ~ADXL38X_MASK_OP_MODE);
	ret |= adxl38x_write_device_data(dev, ADXL38X_OP_MODE,
					 1, &op_mode_reg_val);
	if (!ret)
		dev->op_mode = no_os_field_get(ADXL38X_MASK_OP_MODE, op_mode_reg_val);

	return ret;
}

/***************************************************************************//**
 * @brief Executed Selftest on the sensing axes and returns the outcome of
 * the test.
 *
 * @param dev   	- The device structure.
 * @param op_mode  - Operation mode in which self-test is executed
 * @param st_x 		- Result of X-axis self test success/failure
 * @param st_y 		- Result of X-axis self test success/failure
 * @param st_z 		- Result of X-axis self test success/failure
 *
 * @return ret  	- Outcome of the selftest.
*******************************************************************************/
int adxl38x_selftest(struct adxl38x_dev *dev, enum adxl38x_op_mode op_mode,
		     bool *st_x, bool *st_y, bool *st_z)
{
	int ret;
	uint8_t array_raw_data[6] = {0};
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
	ret = adxl38x_set_op_mode(dev, ADXL38X_MODE_STDBY);

	/* Enable desired channels (except temperature) */
	if(op_mode == ADXL38X_MODE_HRT_SND) {
		// Only one axis
		ret = adxl38x_register_update_bits(dev, ADXL38X_DIG_EN,
						   ADXL38X_MASK_CHEN_DIG_EN,
						   no_os_field_prep(ADXL38X_MASK_CHEN_DIG_EN, ADXL38X_CH_EN_X));
	} else {
		ret = adxl38x_register_update_bits(dev, ADXL38X_DIG_EN,
						   ADXL38X_MASK_CHEN_DIG_EN,
						   no_os_field_prep(ADXL38X_MASK_CHEN_DIG_EN, ADXL38X_CH_EN_XYZ));
	}
	if (ret)
		return ret;

	/* Enable desired OP mode */
	ret = adxl38x_set_op_mode(dev, op_mode);
	if (ret)
		return ret;

	/* Enable ST mode, force and positive direction */
	ret = adxl38x_set_self_test_registers(dev, true, true, false);
	if (ret)
		return ret;

	/* Measure output on all three axes for at least 25 samples and compute average*/
	for(int k = 0; k < 25; k++) {
		no_os_mdelay(10);
		ret = adxl38x_read_device_data(dev, ADXL38X_XDATA_H, 6, array_raw_data);

		x_sum += (int64_t)(no_os_get_unaligned_be16(array_raw_data));
		y_sum += (int64_t)(no_os_get_unaligned_be16(array_raw_data + 2));
		z_sum += (int64_t)(no_os_get_unaligned_be16(array_raw_data + 4));
	}
	st_positive_data[0] = no_os_sign_extend32(no_os_div_s64(x_sum, 25), 15);
	st_positive_data[1] = no_os_sign_extend32(no_os_div_s64(y_sum, 25), 15);
	st_positive_data[2] = no_os_sign_extend32(no_os_div_s64(z_sum, 25), 15);

	/* Enable ST mode, force and negative direction */
	ret = adxl38x_set_self_test_registers(dev, true, true, true);
	if (ret)
		return ret;

	/* Measure output on all three axes for at lest 25 samples and compute average*/
	x_sum = 0;
	y_sum = 0;
	z_sum = 0;
	for(int k = 0; k < 25; k++) {
		no_os_mdelay(10);
		ret = adxl38x_read_device_data(dev, ADXL38X_XDATA_H, 6, array_raw_data);

		x_sum += (int64_t)(no_os_get_unaligned_be16(array_raw_data));
		y_sum += (int64_t)(no_os_get_unaligned_be16(array_raw_data + 2));
		z_sum += (int64_t)(no_os_get_unaligned_be16(array_raw_data + 4));
	}
	st_negative_data[0] = no_os_sign_extend32(no_os_div_s64(x_sum, 25), 15);
	st_negative_data[1] = no_os_sign_extend32(no_os_div_s64(y_sum, 25), 15);
	st_negative_data[2] = no_os_sign_extend32(no_os_div_s64(z_sum, 25), 15);

	/* Compute self-test delta */
	// Multiply self test denomitator for the values to be comparable
	for(int k = 0; k < 3; k++) {
		st_delta_data[k] = (int32_t) abs(st_positive_data[k] - st_negative_data[k]);
		st_delta_data[k] = st_delta_data[k] * ADXL38X_ST_LIMIT_DENOMINATOR;
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
	ret = adxl38x_set_self_test_registers(dev, false, false, false);
	if (ret)
		return ret;

	return 0;
}

/***************************************************************************//**
 * @brief Function to set the paramenters for FIFO mode
 *
 * @param dev        		- The device structure.
 * @param num_samples 		- Number of FIFO entries that FIFI_WATERMARK should set.
 * @param external_trigger 	- Enable/disable external trigger in FIFO stream mode.
 * @param fifo_mode        	- FIFO mode setting.
 * @param ch_ID_enable      - Enable/disable channel ID.
 * @param read_reset       	- reset read/write point and read state machine.
 *
 * @return ret      		- Result of the procedure.
*******************************************************************************/
int adxl38x_accel_set_FIFO( struct adxl38x_dev *dev, uint16_t num_samples,
			    bool external_trigger, enum adxl38x_fifo_mode fifo_mode, bool ch_ID_enable,
			    bool read_reset)
{
	int ret;
	uint8_t write_data = 0;
	uint8_t fifo_samples_low;
	uint8_t fifo_samples_high;
	uint8_t set_channels;

	// Obtain the channels enabled in DIG_EN register
	ret = adxl38x_read_device_data(dev, ADXL38X_DIG_EN, 1, &set_channels);
	if (ret)
		return ret;
	set_channels = no_os_field_get(ADXL38X_MASK_CHEN_DIG_EN, set_channels);

	// Check if number of samples provided is allowed
	if (num_samples > 320)
		return -EINVAL;
	else if((num_samples > 318) &&
		((!set_channels) || (set_channels == ADXL38X_CH_EN_XYZ) ||
		 (set_channels == ADXL38X_CH_EN_YZT)))
		return -EINVAL;

	// set FIFO_CFG1 register
	fifo_samples_low = (uint8_t) num_samples & 0xFF;
	ret = adxl38x_write_device_data(dev, ADXL38X_FIFO_CFG1, 1, &fifo_samples_low);
	if (ret)
		return ret;

	// building data for FIFO_CFG0 register
	fifo_samples_high = (uint8_t) num_samples >> 8;
	fifo_samples_high = fifo_samples_high & NO_OS_BIT(0);
	write_data = fifo_samples_high;

	fifo_mode = no_os_field_prep(ADXL38X_FIFOCFG_FIFOMODE_MSK, fifo_mode);
	write_data |= fifo_mode;

	if(read_reset)
		write_data |= NO_OS_BIT(7);

	if(ch_ID_enable)
		write_data |= NO_OS_BIT(6);

	if(external_trigger && fifo_mode == ADXL38X_FIFO_TRIGGER)
		write_data |= NO_OS_BIT(3);

	ret = adxl38x_write_device_data(dev, ADXL38X_FIFO_CFG0, 1, &write_data);

	return ret;
}

/***************************************************************************//**
 * @brief Function to convert accel data to gees
 *
 * @param dev        		- The device structure.
 * @param raw_accel_data 	- Raw data array of two bytes
 * @param data_frac        	- Fractional data in gees
 *
 * @return ret      		- Result of the procedure.
*******************************************************************************/
int adxl38x_data_raw_to_gees( struct adxl38x_dev *dev, uint8_t *raw_accel_data,
			      struct adxl38x_fractional_val *data_frac)
{
	int ret;
	uint16_t data = 0;

	data = no_os_get_unaligned_be16(raw_accel_data);
	data_frac->integer = no_os_div_s64_rem((int64_t)adxl38x_accel_conv(dev, data),
					       ADXL38X_ACC_SCALE_FACTOR_GEE_DIV, &(data_frac->fractional));

	return 0;
}

