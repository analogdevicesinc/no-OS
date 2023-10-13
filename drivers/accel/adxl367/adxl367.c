/***************************************************************************//**
 *   @file   adxl367.c
 *   @brief  Implementation of ADXL367 Driver.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
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
#include "adxl367.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_print_log.h"

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/
static const uint8_t adxl367_scale_mul[3] = {1, 2, 4};
static uint8_t samples_per_set = 0;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Initializes communication with the device and checks if the part is
 *        present by reading the device id.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl367_init(struct adxl367_dev **device,
		 struct adxl367_init_param init_param)
{
	struct adxl367_dev *dev;
	uint8_t reg_value;
	int status;

	dev = (struct adxl367_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -1;

	dev->comm_type = init_param.comm_type;
	if (dev->comm_type == ADXL367_SPI_COMM) {
		/* SPI */
		status = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);
		if (status)
			goto comm_err;
	} else if (dev->comm_type == ADXL367_I2C_COMM) {
		/* I2C */
		status = no_os_i2c_init(&dev->i2c_desc, &init_param.i2c_init);
		if (status)
			goto comm_err;
		dev->i2c_slave_address = init_param.i2c_slave_address;
	} else {
		/* Communication type not defined.*/
		goto comm_err;
	}

	/* Perform soft reset */
	status = adxl367_software_reset(dev);
	if (status)
		goto err;

	/* Check for device ID */
	status = adxl367_get_register_value(dev, &reg_value, ADXL367_REG_DEVID_AD, 1);
	if (status)
		goto err;
	if(reg_value != ADXL367_DEVICE_AD)
		goto err;
	status = adxl367_get_register_value(dev, &reg_value, ADXL367_REG_DEVID_MST, 1);
	if (status)
		goto err;
	if(reg_value != ADXL367_DEVICE_MST)
		goto err;
	status = adxl367_get_register_value(dev, &reg_value, ADXL367_REG_PARTID, 1);
	if (status)
		goto err;
	if(reg_value != ADXL367_PART_ID)
		goto err;

	*device = dev;

	pr_info("ADXL367 successfully initialized\n");
	return 0;

err:
	if (dev->comm_type == ADXL367_SPI_COMM)
		no_os_spi_remove(dev->spi_desc);
	else
		no_os_i2c_remove(dev->i2c_desc);
comm_err:
	no_os_free(dev);
	pr_err("%s: Failed initialization.\n", __func__);
	return -1;
}

/***************************************************************************//**
 * @brief Frees the resources allocated by adxl367_init().
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl367_remove(struct adxl367_dev *dev)
{
	int ret;

	if (dev->comm_type == ADXL367_SPI_COMM)
		ret = no_os_spi_remove(dev->spi_desc);
	else
		ret = no_os_i2c_remove(dev->i2c_desc);

	no_os_free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Performs self test.
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl367_self_test(struct adxl367_dev *dev)
{
	int ret;
	int16_t x_axis_1, x_axis_2, dif, min, max;
	uint8_t read_val;
	uint32_t st_delay_ms;

	// 4 / ODR value in ms
	switch (dev->odr) {
	case ADXL367_ODR_12P5HZ:
		st_delay_ms = 320;
	case ADXL367_ODR_25HZ:
		st_delay_ms = 160;
	case ADXL367_ODR_50HZ:
		st_delay_ms = 80;
	case ADXL367_ODR_100HZ:
		st_delay_ms = 40;
	case ADXL367_ODR_200HZ:
		st_delay_ms = 20;
	case ADXL367_ODR_400HZ:
		st_delay_ms = 10;
	default:
		return -EINVAL;
	}

	ret = adxl367_set_power_mode(dev, ADXL367_OP_MEASURE);
	if (ret)
		return ret;

	ret = adxl367_reg_write_msk(dev, ADXL367_REG_SELF_TEST, ADXL367_SELF_TEST_ST,
				    ADXL367_SELF_TEST_ST);
	if (ret)
		return ret;
	// 4 / ODR delay
	no_os_mdelay(st_delay_ms);
	ret = adxl367_get_register_value(dev, &read_val, ADXL367_REG_XDATA_H, 1);
	if (ret)
		return ret;
	x_axis_1 = read_val << 6;
	ret = adxl367_get_register_value(dev, &read_val, ADXL367_REG_XDATA_L, 1);
	if (ret)
		return ret;
	x_axis_1 += read_val >> 2;
	// extend sign to 16 bits
	if (x_axis_1 & NO_OS_BIT(13))
		x_axis_1 |= NO_OS_GENMASK(15, 14);

	ret = adxl367_reg_write_msk(dev, ADXL367_REG_SELF_TEST,
				    ADXL367_SELF_TEST_ST_FORCE, ADXL367_SELF_TEST_ST_FORCE);
	if (ret)
		return ret;
	// 4 / ODR delay
	no_os_mdelay(st_delay_ms);
	ret = adxl367_get_register_value(dev, &read_val, ADXL367_REG_XDATA_H, 1);
	if (ret)
		return ret;
	x_axis_2 = read_val << 6;
	ret = adxl367_get_register_value(dev, &read_val, ADXL367_REG_XDATA_L, 1);
	if (ret)
		return ret;
	x_axis_2 += read_val >> 2;
	// extend sign to 16 bits
	if (x_axis_2 & NO_OS_BIT(13))
		x_axis_2 |= NO_OS_GENMASK(15, 14);

	ret = adxl367_set_power_mode(dev, ADXL367_OP_STANDBY);
	if (ret)
		return ret;

	ret = adxl367_reg_write_msk(dev, ADXL367_REG_SELF_TEST, 0,
				    ADXL367_SELF_TEST_ST_FORCE | ADXL367_SELF_TEST_ST);
	if (ret)
		return ret;

	dif = x_axis_2 - x_axis_1;
	min = ADXL367_SELF_TEST_MIN * adxl367_scale_mul[dev->range];
	max = ADXL367_SELF_TEST_MAX * adxl367_scale_mul[dev->range];

	if ((dif >= min) && (dif <= max)) {
		pr_info("ADXL367 passed self-test\n");
		return 0;
	} else {
		pr_err("%s: Failed self-test.\n", __func__);
		return -1;
	}
}

/***************************************************************************//**
 * @brief Writes data into a register.
 *
 * @param dev              - The device structure.
 * @param register_value   - Data value to write.
 * @param register_address - Address of the register.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl367_set_register_value(struct adxl367_dev *dev,
			       uint8_t register_value,
			       uint8_t register_address)
{
	uint8_t buffer[3] = {0};
	buffer[1] = register_address;
	buffer[2] = register_value;

	if (dev->comm_type == ADXL367_SPI_COMM) {
		buffer[0] = ADXL367_WRITE_REG;
		return no_os_spi_write_and_read(dev->spi_desc,
						buffer, 3);
	} else {
		buffer[0] = dev->i2c_slave_address + ADXL367_I2C_WRITE;
		return no_os_i2c_write(dev->i2c_desc, buffer, 3, 1);
	}

}

/***************************************************************************//**
 * @brief Performs a burst read of a specified number of registers.
 *
 * @param dev              - The device structure.
 * @param read_data        - The read values are stored in this buffer.
 * @param register_address - The start address of the burst read.
 * @param register_nb	   - Number of registers to be read in burst.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl367_get_register_value(struct adxl367_dev *dev,
			       uint8_t* read_data,
			       uint8_t  register_address,
			       uint8_t  register_nb)
{
	int ret;
	uint8_t buffer[0x45] = { 0 };
	uint8_t index;

	if (register_address > ADXL367_REG_STATUS_2)
		return -1;

	buffer[1] = register_address;

	if (dev->comm_type == ADXL367_SPI_COMM) {
		buffer[0] = ADXL367_READ_REG;
		ret = no_os_spi_write_and_read(dev->spi_desc, buffer, register_nb + 2);
		if (ret)
			return ret;
		for (index = 0; index < register_nb; index++)
			read_data[index] = buffer[index + 2];
	} else {
		buffer[0] = dev->i2c_slave_address + ADXL367_I2C_READ;
		ret = no_os_i2c_write(dev->i2c_desc, buffer, 2, 0);
		if (ret)
			return ret;
		ret = no_os_i2c_read(dev->i2c_desc, buffer, register_nb + 1, 1);
		if (ret)
			return ret;
		for (index = 0; index < register_nb; index++)
			read_data[index] = buffer[index + 1];
	}

	return 0;
}

/***************************************************************************//**
 * @brief Performs a burst read of FIFO buffer.
 *
 * @param dev        - The device structure.
 * @param read_data  - The read values are stored in this buffer.
 * @param bytes_nb	 - Number of bytes to be read in burst.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
static int adxl367_get_fifo_value(struct adxl367_dev *dev,
				  uint8_t* read_data,
				  uint16_t  bytes_nb)
{
	int ret;
	uint16_t index;

	if (dev->comm_type == ADXL367_SPI_COMM) {
		dev->fifo_buffer[0] = ADXL367_READ_FIFO;
		ret = no_os_spi_write_and_read(dev->spi_desc, dev->fifo_buffer, bytes_nb + 1);
		if (ret)
			return ret;
	} else {
		dev->fifo_buffer[0] = dev->i2c_slave_address + ADXL367_I2C_READ;
		dev->fifo_buffer[1] = ADXL367_REG_I2C_FIFO_DATA;
		ret = no_os_i2c_write(dev->i2c_desc, dev->fifo_buffer, 2, 0);
		if (ret)
			return ret;
		ret = no_os_i2c_read(dev->i2c_desc, dev->fifo_buffer, bytes_nb + 1, 1);
		if (ret)
			return ret;
	}

	for (index = 0; index < bytes_nb; index++)
		dev->fifo_buffer[index] = dev->fifo_buffer[index + 1];

	return 0;
}

/***************************************************************************//**
 * @brief Performs a masked write to a register.
 *
 * @param dev      - The device structure.
 * @param reg_addr - The register address.
 * @param data     - The register data.
 * @param mask     - The mask.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl367_reg_write_msk(struct adxl367_dev *dev,
			  uint8_t reg_addr,
			  uint8_t data,
			  uint8_t mask)
{
	int ret;
	uint8_t reg_data;

	ret = adxl367_get_register_value(dev, &reg_data, reg_addr, 1);
	if (ret)
		return ret;
	reg_data &= ~mask;
	reg_data |= data;
	return adxl367_set_register_value(dev, reg_data, reg_addr);
}

/***************************************************************************//**
 * @brief Performs soft-reset.
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl367_software_reset(struct adxl367_dev *dev)
{
	int ret;
	ret = adxl367_set_register_value(dev,
					 ADXL367_RESET_KEY,
					 ADXL367_REG_SOFT_RESET);
	if (ret)
		return ret;

	// Operating Mode: Standby (reset default).
	dev->op_mode = ADXL367_OP_STANDBY;
	// Measurement Range: +/- 2g (reset default).
	dev->range = ADXL367_2G_RANGE;
	// ODR: 100Hz (reset default).
	dev->odr = ADXL367_ODR_100HZ;
	// FIFO: FIFO disabled (reset default).
	dev->fifo_mode = ADXL367_FIFO_DISABLED;
	// FIFO Format: All axis (reset default).
	dev->fifo_format = ADXL367_FIFO_FORMAT_XYZ;
	// FIFO Read Mode : 14 bits + CH ID (reset default).
	dev->fifo_read_mode = ADXL367_14B_CHID;
	//Axis offset = 0 (reset default)
	dev->x_offset = 0;
	dev->y_offset = 0;
	dev->z_offset = 0;

	//initialization delay
	no_os_mdelay(20);

	return 0;
}

/***************************************************************************//**
 * @brief Places the device into standby/measure mode.
 *
 * @param dev      - The device structure.
 * @param mode     - Power mode.
 *                   Accepted values: 	ADXL367_OP_STANDBY = 0,
 *										ADXL367_OP_MEASURE = 2
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl367_set_power_mode(struct adxl367_dev *dev,
			   enum adxl367_op_mode mode)
{
	int ret;

	ret = adxl367_reg_write_msk(dev,
				    ADXL367_REG_POWER_CTL,
				    no_os_field_prep(ADXL367_POWER_CTL_MEASURE_MSK, mode),
				    ADXL367_POWER_CTL_MEASURE_MSK);
	if (ret)
		return ret;

	//100 ms wait time before reading acceleration data.
	if (mode == ADXL367_OP_MEASURE)
		no_os_mdelay(100);

	dev->op_mode = mode;

	return 0;
}

/***************************************************************************//**
 * @brief Selects the measurement range.
 *
 * @param dev 	  - The device structure.
 * @param range   - Range option.
 *				  	Accepted values: 	ADXL367_2G_RANGE, +/- 2g
 *										ADXL367_4G_RANGE, +/- 4g
 *										ADXL367_8G_RANGE  +/- 8g
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl367_set_range(struct adxl367_dev *dev,
		      enum adxl367_range range)
{
	int ret;

	ret = adxl367_reg_write_msk(dev,
				    ADXL367_REG_FILTER_CTL,
				    no_os_field_prep(ADXL367_FILTER_CTL_RANGE_MSK, range),
				    ADXL367_FILTER_CTL_RANGE_MSK);
	if (ret)
		return ret;

	dev->range = range;

	return 0;
}

/***************************************************************************//**
 * @brief Selects the Output Data Rate of the device.
 *
 * @param dev      - The device structure.
 * @param odr      - Output Data Rate option.
 *					 Accepted values: 	ADXL367_ODR_12P5HZ,
 *										ADXL367_ODR_25HZ,
 *										ADXL367_ODR_50HZ,
 *										ADXL367_ODR_100HZ,
 *										ADXL367_ODR_200HZ,
 *										ADXL367_ODR_400HZ
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl367_set_output_rate(struct adxl367_dev *dev,
			    enum adxl367_odr odr)
{
	int ret;

	ret = adxl367_reg_write_msk(dev,
				    ADXL367_REG_FILTER_CTL,
				    no_os_field_prep(ADXL367_FILTER_CTL_ODR_MSK, odr),
				    ADXL367_FILTER_CTL_ODR_MSK);
	if (ret)
		return ret;

	dev->odr = odr;

	return 0;
}

/***************************************************************************//**
 * @brief Sets offset for each axis.
 *
 * @param dev      - The device structure.
 * @param x_offset - X axis offset.
 * @param y_offset - Y axis offset.
 * @param z_offset - Z axis offset.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl367_set_offset(struct adxl367_dev *dev, uint16_t x_offset,
		       uint16_t y_offset, uint16_t z_offset)
{
	int ret;

	ret = adxl367_reg_write_msk(dev, ADXL367_REG_X_OFFSET,
				    x_offset & ADXL367_XYZ_AXIS_OFFSET_MASK,
				    ADXL367_XYZ_AXIS_OFFSET_MASK);
	if (ret)
		return ret;
	dev->x_offset = x_offset;

	ret = adxl367_reg_write_msk(dev, ADXL367_REG_Y_OFFSET,
				    y_offset & ADXL367_XYZ_AXIS_OFFSET_MASK,
				    ADXL367_XYZ_AXIS_OFFSET_MASK);
	if (ret)
		return ret;
	dev->y_offset = y_offset;

	ret = adxl367_reg_write_msk(dev, ADXL367_REG_Z_OFFSET,
				    z_offset & ADXL367_XYZ_AXIS_OFFSET_MASK,
				    ADXL367_XYZ_AXIS_OFFSET_MASK);
	if (ret)
		return ret;
	dev->z_offset = z_offset;

	return 0;
}

/***************************************************************************//**
 * @brief Reads the 3-axis raw data from the accelerometer.
 *
 * @param dev - The device structure.
 * @param x   - Stores the X-axis data(as two's complement).
 * @param y   - Stores the Y-axis data(as two's complement).
 * @param z   - Stores the Z-axis data(as two's complement).
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl367_get_raw_xyz(struct adxl367_dev *dev,
			int16_t* x,
			int16_t* y,
			int16_t* z)
{
	int ret;
	uint8_t xyz_values[6] = { 0 };
	uint8_t reg_data, nready = 1U;

	while (nready) {
		ret = adxl367_get_register_value(dev, &reg_data, ADXL367_REG_STATUS, 1);
		if (ret)
			return -1;
		if (reg_data & ADXL367_STATUS_DATA_RDY)
			nready = 0U;
	}

	ret = adxl367_get_register_value(dev,
					 xyz_values,
					 ADXL367_REG_XDATA_H,
					 6);

	// result is 14 bits long, ignore last 2 bits from low byte
	*x = ((int16_t)xyz_values[0] << 6) + (xyz_values[1] >> 2);
	*y = ((int16_t)xyz_values[2] << 6) + (xyz_values[3] >> 2);
	*z = ((int16_t)xyz_values[4] << 6) + (xyz_values[5] >> 2);
	// extend sign to 16 bits
	if (*x & NO_OS_BIT(13))
		*x |= NO_OS_GENMASK(15, 14);
	if (*y & NO_OS_BIT(13))
		*y |= NO_OS_GENMASK(15, 14);
	if (*z & NO_OS_BIT(13))
		*z |= NO_OS_GENMASK(15, 14);

	return 0;
}

/***************************************************************************//**
 * @brief Converts raw acceleration value to g value.
 *
 * @param dev       - The device structure.
 * @param raw_accel - Raw acceleration value.
 * @param g_val		- Struct to store converted data.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
static int adxl367_accel_conv(struct adxl367_dev *dev,
			      uint16_t raw_accel, struct adxl367_fractional_val* g_val)
{
	int64_t accel_data;

	accel_data = (int16_t)raw_accel;

	// Apply scale factor based on the selected range
	accel_data = accel_data * ADXL367_ACC_SCALE_FACTOR_MUL *
		     adxl367_scale_mul[dev->range];

	g_val->integer = no_os_div_s64_rem(accel_data,
					   ADXL367_ACC_SCALE_FACTOR_DIV, &(g_val->fractional));

	return 0;
}

/***************************************************************************//**
 * @brief Reads the 3-axis raw data from the accelerometer and converts it to g.
 *
 * @param dev - The device structure.
 * @param x   - Stores the X-axis data.
 * @param y   - Stores the Y-axis data.
 * @param z   - Stores the Z-axis data.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl367_get_g_xyz(struct adxl367_dev *dev,
		      struct adxl367_fractional_val* x,
		      struct adxl367_fractional_val* y,
		      struct adxl367_fractional_val* z)
{
	int ret;
	int16_t x_raw, y_raw, z_raw;

	ret = adxl367_get_raw_xyz(dev, &x_raw, &y_raw, &z_raw);
	if (ret)
		return ret;

	ret = adxl367_accel_conv(dev, x_raw, x);
	if (ret)
		return ret;
	ret = adxl367_accel_conv(dev, y_raw, y);
	if (ret)
		return ret;
	return adxl367_accel_conv(dev, z_raw, z);
}

/***************************************************************************//**
 * @brief Enables temperature reading.
 *
 * @param dev    - The device structure.
 * @param enable - 1 - ENABLE
 * 				   2 - DISABLE
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl367_temp_read_en(struct adxl367_dev *dev, bool enable)
{
	return adxl367_reg_write_msk(dev,
				     ADXL367_REG_TEMP_CTL,
				     enable ? ADXL367_TEMP_EN : 0U,
				     ADXL367_TEMP_EN);
}

/***************************************************************************//**
 * @brief Enables ADC reading. Disables temperature reading.
 *
 * @param dev    - The device structure.
 * @param enable - 1 - ENABLE
 * 				   2 - DISABLE
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl367_adc_read_en(struct adxl367_dev *dev, bool enable)
{
	int ret;

	//disable temp reading
	ret = adxl367_reg_write_msk(dev,
				    ADXL367_REG_TEMP_CTL,
				    0U,
				    ADXL367_TEMP_EN);
	if (ret)
		return ret;

	return adxl367_reg_write_msk(dev,
				     ADXL367_REG_ADC_CTL,
				     enable ? ADXL367_ADC_EN : 0U,
				     ADXL367_ADC_EN);
}

/***************************************************************************//**
 * @brief Reads the raw temperature of the device. If ADXL367_TEMP_EN is not
 *  set, use adxl367_temp_read_en() first to enable temperature reading.
 *
 * @param dev      - The device structure.
 * @param raw_temp - Raw value of temperature.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl367_read_raw_temp(struct adxl367_dev *dev, int16_t *raw_temp)
{
	int ret;
	uint8_t temp[2] = { 0 };
	uint8_t reg_data, nready = 1U;

	while (nready) {
		ret = adxl367_get_register_value(dev, &reg_data, ADXL367_REG_STATUS, 1);
		if (ret)
			return -1;
		if (reg_data & ADXL367_STATUS_DATA_RDY)
			nready = 0U;
	}

	ret = adxl367_get_register_value(dev,
					 temp,
					 ADXL367_REG_TEMP_H,
					 2);
	if (ret)
		return ret;

	*raw_temp = ((int16_t)temp[0] << 6) + (temp[1] >> 2);
	// extend sign to 16 bits
	if (*raw_temp & NO_OS_BIT(13))
		*raw_temp |= NO_OS_GENMASK(15, 14);

	return 0;
}

/***************************************************************************//**
 * @brief Converts raw temperature value to Celsius degree value.
 *
 * @param dev       - The device structure.
 * @param raw_temp  - Raw temperature value.
 * @param temp   	- Struct to store converted data.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
static int adxl367_temp_conv(struct adxl367_dev *dev,
			     int16_t raw_temp, struct adxl367_fractional_val* temp)
{
	int64_t temp_data;

	temp_data = (int16_t)raw_temp;
	temp_data = (temp_data + ADXL367_TEMP_OFFSET) * ADXL367_TEMP_SCALE;

	temp->integer = no_os_div_s64_rem(temp_data,
					  ADXL367_TEMP_SCALE_DIV,
					  &(temp->fractional));

	return 0;
}

/***************************************************************************//**
 * @brief Reads the temperature of the device.
 *
 * @param dev  - The device structure.
 * @param temp - Temperature output data.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl367_read_temperature(struct adxl367_dev *dev,
			     struct adxl367_fractional_val *temp)
{
	int16_t raw_temp;
	int ret;

	ret = adxl367_read_raw_temp(dev, &raw_temp);
	if (ret)
		return ret;

	return adxl367_temp_conv(dev, raw_temp, temp);
}

/***************************************************************************//**
 * @brief Reads ADC data. If ADXL367_ADC_EN is not set, use
 * 	adxl367_adc_read_en() first to enable ADC reading.
 *
 * @param dev      - The device structure.
 * @param data 	   - ADC value.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl367_read_adc(struct adxl367_dev *dev, int16_t *data)
{
	int ret;
	uint8_t raw[2] = {0, 0};
	uint8_t reg_data, nready = 1U;

	while (nready) {
		ret = adxl367_get_register_value(dev, &reg_data, ADXL367_REG_STATUS, 1);
		if (ret)
			return -1;
		if (reg_data & ADXL367_STATUS_DATA_RDY)
			nready = 0U;
	}

	ret = adxl367_get_register_value(dev,
					 raw,
					 ADXL367_REG_EX_ADC_H,
					 2);
	if (ret)
		return ret;

	*data = ((int16_t)raw[0] << 6) + (raw[1] >> 2);
	// extend sign to 16 bits
	if (*data & NO_OS_BIT(13))
		*data |= NO_OS_GENMASK(15, 14);

	return 0;
}

/***************************************************************************//**
 * @brief Reads the number of FIFO entries.
 *
 * @param dev     - The device structure.
 * @param entr_nb - Entries number. Different from sets number. For example,
 * 			if ADXL367_FIFO_FORMAT_XYZ is selected, one set contains 3 entries.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl367_get_nb_of_fifo_entries(struct adxl367_dev *dev,
				   uint16_t *entr_nb)
{
	int ret;
	uint8_t reg_val[2] = {0, 0};

	ret = adxl367_get_register_value(dev, reg_val, ADXL367_REG_FIFO_ENTRIES_L, 2);
	if (ret)
		return ret;
	// last 2 bits from FIFO_ENTRIES_H and 8 bits from FIFO_ENTRIES_L
	*entr_nb = ((reg_val[1] & 0x03) << 8) | reg_val[0];

	return 0;
}

/***************************************************************************//**
 * @brief Sets the number of FIFO sample sets.
 *
 * @param dev     - The device structure.
 * @param sets_nb - Sample sets number. For example, if ADXL367_FIFO_FORMAT_XYZ
 * 			is selected, a value of 2 will represent 6 entries.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl367_set_fifo_sample_sets_nb(struct adxl367_dev *dev,
				    uint16_t sets_nb)
{
	int ret;

	// bit 9 goes to FIFO_SAMPLES from ADXL367_REG_FIFO_CONTROL
	ret = adxl367_reg_write_msk(dev, ADXL367_REG_FIFO_CONTROL,
				    sets_nb & NO_OS_BIT(9) ? ADXL367_FIFO_CONTROL_FIFO_SAMPLES : 0U,
				    ADXL367_FIFO_CONTROL_FIFO_SAMPLES);
	if (ret)
		return ret;

	// write last 8 bits to ADXL367_REG_FIFO_SAMPLES
	return adxl367_set_register_value(dev, sets_nb & 0xFF,
					  ADXL367_REG_FIFO_SAMPLES);
}

/***************************************************************************//**
 * @brief Sets FIFO mode.
 *
 * @param dev  - The device structure.
 * @param mode - FIFO mode.
 * 			   Accepted values: ADXL367_FIFO_DISABLED,
 *								ADXL367_OLDEST_SAVED,
 *								ADXL367_STREAM_MODE,
 *								ADXL367_TRIGGERED_MODE
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl367_set_fifo_mode(struct adxl367_dev *dev,
			  enum adxl367_fifo_mode mode)
{
	int ret;

	ret = adxl367_reg_write_msk(dev,
				    ADXL367_REG_FIFO_CONTROL,
				    no_os_field_prep(ADXL367_FIFO_CONTROL_FIFO_MODE_MSK, mode),
				    ADXL367_FIFO_CONTROL_FIFO_MODE_MSK);
	if (ret)
		return ret;

	dev->fifo_mode = mode;

	return 0;
}

/***************************************************************************//**
 * @brief Sets FIFO read mode.
 *
 * @param dev  		- The device structure.
 * @param read_mode - FIFO read mode.
 *			   		Accepted values: ADXL367_12B_CHID,
 *									 ADXL367_8B,
 *									 ADXL367_12B,
 *									 ADXL367_14B_CHID
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl367_set_fifo_read_mode(struct adxl367_dev *dev,
			       enum adxl367_fifo_read_mode read_mode)
{
	int ret;

	ret = adxl367_reg_write_msk(dev,
				    ADXL367_REG_ADC_CTL,
				    no_os_field_prep(ADXL367_FIFO_8_12BIT_MSK, read_mode),
				    ADXL367_FIFO_8_12BIT_MSK);
	if (ret)
		return ret;

	dev->fifo_read_mode = read_mode;

	return 0;
}

/***************************************************************************//**
 * @brief Sets FIFO format.
 *
 * @param dev     - The device structure.
 * @param format  - FIFO format.
 * 				  Accepted values:  ADXL367_FIFO_FORMAT_XYZ,
 *									ADXL367_FIFO_FORMAT_X,
 *									ADXL367_FIFO_FORMAT_Y,
 *									ADXL367_FIFO_FORMAT_Z,
 *									ADXL367_FIFO_FORMAT_XYZT,
 *	 								ADXL367_FIFO_FORMAT_XT,
 *									ADXL367_FIFO_FORMAT_YT,
 *									ADXL367_FIFO_FORMAT_ZT,
 *									ADXL367_FIFO_FORMAT_XYZA,
 *									ADXL367_FIFO_FORMAT_XA,
 *									ADXL367_FIFO_FORMAT_YA,
 *									ADXL367_FIFO_FORMAT_ZA
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl367_set_fifo_format(struct adxl367_dev *dev,
			    enum adxl367_fifo_format format)
{
	int ret;

	ret = adxl367_reg_write_msk(dev,
				    ADXL367_REG_FIFO_CONTROL,
				    no_os_field_prep(ADXL367_FIFO_CONTROL_FIFO_CHANNEL_MSK, format),
				    ADXL367_FIFO_CONTROL_FIFO_CHANNEL_MSK);
	if (ret)
		return ret;

	dev->fifo_format = format;

	switch (dev->fifo_format) {
	case ADXL367_FIFO_FORMAT_XYZ:
		samples_per_set = 3;
		break;
	case ADXL367_FIFO_FORMAT_X:
	case ADXL367_FIFO_FORMAT_Y:
	case ADXL367_FIFO_FORMAT_Z:
		samples_per_set = 1;
		break;
	case ADXL367_FIFO_FORMAT_XYZT:
	case ADXL367_FIFO_FORMAT_XYZA:
		samples_per_set = 4;
		break;
	case ADXL367_FIFO_FORMAT_XT:
	case ADXL367_FIFO_FORMAT_YT:
	case ADXL367_FIFO_FORMAT_ZT:
	case ADXL367_FIFO_FORMAT_XA:
	case ADXL367_FIFO_FORMAT_YA:
	case ADXL367_FIFO_FORMAT_ZA:
		samples_per_set = 2;
		break;
	default:
		return -1;
	}

	return 0;
}

/***************************************************************************//**
 * @brief Configures the FIFO feature. Uses ADXL367_14B_CHID read mode as
 * 		default.
 *
 * @param dev          - The device structure.
 * @param mode         - FIFO mode selection.
 *                       Example: ADXL367_FIFO_DISABLED,
 *								  ADXL367_OLDEST_SAVED,
 *								  ADXL367_STREAM_MODE,
 *								  ADXL367_TRIGGERED_MODE
 * @param format	   - FIFO format selection.
 * 				         Example:  ADXL367_FIFO_FORMAT_XYZ,
 *								   ADXL367_FIFO_FORMAT_X,
 *								   ADXL367_FIFO_FORMAT_Y,
 *								   ADXL367_FIFO_FORMAT_Z,
 *								   ADXL367_FIFO_FORMAT_XYZT,
 *	 							   ADXL367_FIFO_FORMAT_XT,
 *								   ADXL367_FIFO_FORMAT_YT,
 *								   ADXL367_FIFO_FORMAT_ZT,
 *								   ADXL367_FIFO_FORMAT_XYZA,
 *								   ADXL367_FIFO_FORMAT_XA,
 *								   ADXL367_FIFO_FORMAT_YA,
 *								   ADXL367_FIFO_FORMAT_ZA
 * @param sets_nb 	   - Specifies the number of samples sets to store in the FIFO.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl367_fifo_setup(struct adxl367_dev *dev,
		       enum adxl367_fifo_mode mode,
		       enum adxl367_fifo_format format,
		       uint8_t sets_nb)
{
	int ret;

	ret = adxl367_set_fifo_mode(dev, mode);
	if (ret)
		return ret;

	ret = adxl367_set_fifo_format(dev, format);
	if (ret)
		return ret;

	ret = adxl367_set_fifo_sample_sets_nb(dev, sets_nb);
	if (ret)
		return ret;

	return adxl367_set_fifo_read_mode(dev, ADXL367_14B_CHID);
}

/***************************************************************************//**
 * @brief Reads all available raw values from FIFO. If, after setting FIFO mode,
 * 		any of x, y, z, temp or adc aren't selected, assign NULL pointer.
 * 		Uses ADXL367_14B_CHID read mode as default.
 *
 * @param dev       - The device structure.
 * @param x  	    - X axis raw data buffer. If not used, assign NULL.
 * @param y  	    - Y axis raw data buffer. If not used, assign NULL.
 * @param z  	    - Z axis raw data buffer. If not used, assign NULL.
 * @param temp_adc  - Data buffer for temperature or adc raw data. If not
 * 				used assign NULL.
 * @param entries   - Number of read entries.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl367_read_raw_fifo(struct adxl367_dev *dev, int16_t *x, int16_t *y,
			  int16_t *z, int16_t *temp_adc, uint16_t *entries)
{
	int ret;
	uint16_t i, stored_entr = 0;
	uint8_t id;

	ret = adxl367_get_nb_of_fifo_entries(dev, &stored_entr);
	if (ret)
		return ret;

	*entries = stored_entr;

	ret = adxl367_get_fifo_value(dev, dev->fifo_buffer, stored_entr * 2);
	if (ret)
		return -1;

	// MSB = 6 data bits + 2 bits for CH ID
	for (i = 0; i < (stored_entr * 2); i += 2 ) {
		id = dev->fifo_buffer[i] >> 6;

		switch (id) {
		case (ADXL367_FIFO_X_ID) :
			if (x == NULL)
				return -1;
			*x = ((dev->fifo_buffer[i] & 0x3F) << 8) + dev->fifo_buffer[i+1];
			//extend sign
			if (*x & NO_OS_BIT(13))
				*x |= NO_OS_GENMASK(15, 14);
			x++;
			break;

		case (ADXL367_FIFO_Y_ID) :
			if (y == NULL)
				return -1;
			*y = ((dev->fifo_buffer[i] & 0x3F) << 8) + dev->fifo_buffer[i+1];
			//extend sign
			if (*y & NO_OS_BIT(13))
				*y |= NO_OS_GENMASK(15, 14);
			y++;
			break;

		case (ADXL367_FIFO_Z_ID) :
			if (z == NULL)
				return -1;
			*z = ((dev->fifo_buffer[i] & 0x3F) << 8) + dev->fifo_buffer[i+1];
			//extend sign
			if (*z & NO_OS_BIT(13))
				*z |= NO_OS_GENMASK(15, 14);
			z++;
			break;

		case (ADXL367_FIFO_TEMP_ADC_ID) :
			if (temp_adc == NULL)
				return -1;
			*temp_adc = (int16_t)((dev->fifo_buffer[i] & 0x3F) << 8) + dev->fifo_buffer[i
					+1];
			//extend sign
			if (*temp_adc & NO_OS_BIT(13))
				*temp_adc |= NO_OS_GENMASK(15, 14);
			temp_adc++;
			break;

		default :
			pr_err("%s: FIFO read invalid channel ID.\n", __func__);
			return -1;
		}

	}

	return 0;
}

/***************************************************************************//**
 * @brief Reads converted values from FIFO. If, after setting FIFO mode, any of
 *      x, y, z, temp or adc aren't selected, assign NULL pointer. Uses
 *      ADXL367_14B_CHID read mode as default.
 *
 * @param dev       - The device structure.
 * @param x  	    - X axis fractional data buffer. If not used, assign NULL.
 * @param y  	    - Y axis fractional data buffer. If not used, assign NULL.
 * @param z  	    - Z axis fractional data buffer. If not used, assign NULL.
 * @param temp_adc  - Fractional buffer for temperature. If ADC is used, ADC's
 * 				value will be stored to integer field. If not used, assign NULL.
 * @param entries   - Number of read entries.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl367_read_converted_fifo(struct adxl367_dev *dev,
				struct adxl367_fractional_val *x, struct adxl367_fractional_val *y,
				struct adxl367_fractional_val *z, struct adxl367_fractional_val *temp_adc,
				uint16_t *entries)
{
	int ret;
	int16_t x_raw[513], y_raw[513], z_raw[513], temp_adc_raw[513];
	uint16_t index, sets_nb;

	ret = adxl367_read_raw_fifo(dev, x_raw, y_raw, z_raw, temp_adc_raw, entries);
	if (ret)
		return ret;

	sets_nb = *entries / samples_per_set;

	// convert raw data to g value
	for (index = 0; index < sets_nb; index++) {
		ret = adxl367_accel_conv(dev, x_raw[index], x);
		if (ret)
			return ret;
		x++;
		ret = adxl367_accel_conv(dev, y_raw[index], y);
		if (ret)
			return ret;
		y++;
		ret = adxl367_accel_conv(dev, z_raw[index], z);
		if (ret)
			return ret;
		z++;
		if (dev->fifo_format >= ADXL367_FIFO_FORMAT_XYZT
		    && dev->fifo_format <= ADXL367_FIFO_FORMAT_ZT) {
			ret = adxl367_temp_conv(dev, temp_adc_raw[index], temp_adc);
			if (ret)
				return ret;
			temp_adc++;
		} else if (dev->fifo_format >= ADXL367_FIFO_FORMAT_XYZA) {
			temp_adc->integer = temp_adc_raw[index];
			temp_adc++;
		}
	}

	return 0;
}

/***************************************************************************//**
 * @brief Enables specified events to interrupt pin.
 *
 * @param dev - The device structure.
 * @param map - Structure with enabled events.
 * @param pin - INT pin number.
 * 			  Accepted values: 1 - INT1
 * 			  				   2 - INT2
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl367_int_map(struct adxl367_dev *dev, struct adxl367_int_map *map,
		    uint8_t pin)
{
	int ret;
	uint8_t reg_val = 0;

	if (!pin || pin > 2)
		return -1;

	reg_val = (map->err_fuse << 7) +
		  (map->err_user_regs << 6) +
		  (map->kpalv_timer << 4) +
		  (map->temp_adc_hi << 3) +
		  (map->temp_adc_low << 2) +
		  (map->tap_two << 1) +
		  map->tap_one;

	ret = adxl367_reg_write_msk(dev,
				    pin == 1 ? ADXL367_REG_INTMAP1_UPPER : ADXL367_REG_INTMAP2_UPPER, reg_val,
				    ADXL367_INTMAPX_UPPER_MASK);
	if (ret)
		return ret;

	reg_val = (map->int_low << 7) +
		  (map->awake << 6) +
		  (map->inact << 5) +
		  (map->act << 4) +
		  (map->fifo_overrun << 3) +
		  (map->fifo_watermark << 2) +
		  (map->fifo_ready << 1) +
		  map->data_ready;

	return adxl367_set_register_value(dev, reg_val,
					  pin == 1 ? ADXL367_REG_INTMAP1_LWR : ADXL367_REG_INTMAP1_LWR);
}

/***************************************************************************//**
 * @brief Configures activity detection.
 *
 * @param dev         - The device structure.
 * @param ref_or_abs  - Referenced/Absolute Activity Select.
 *                    Example: 0 - absolute mode.
 *                             1 - referenced mode.
 * @param threshold   - 13-bit unsigned value samples are
 *                    compared to.
 * @param time        - 8-bit value written to the timed activity register.
 * 					  Value = number of samples. For example, at 100Hz ODR,
 * 					  100 value translates to 1 second.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl367_setup_activity_detection(struct adxl367_dev *dev,
				     uint8_t  ref_or_abs,
				     uint16_t threshold,
				     uint8_t  time)
{
	int ret;

	switch (ref_or_abs) {
	case ADXL367_ABSOLUTE :
		ret = adxl367_reg_write_msk(dev,
					    ADXL367_REG_ACT_INACT_CTL,
					    no_os_field_prep(ADXL367_ACT_INACT_CTL_ACT_EN_MSK, ADXL367_ACTIVITY_ENABLE),
					    ADXL367_ACT_INACT_CTL_ACT_EN_MSK);
		break;
	case ADXL367_REFERENCED :
		ret = adxl367_reg_write_msk(dev,
					    ADXL367_REG_ACT_INACT_CTL,
					    no_os_field_prep(ADXL367_ACT_INACT_CTL_ACT_EN_MSK,
							    ADXL367_REFERENCED_ACTIVITY_ENABLE),
					    ADXL367_ACT_INACT_CTL_ACT_EN_MSK);
		break;
	default :
		return -1;
	}

	if (ret)
		return ret;

	//set threshold
	ret = adxl367_reg_write_msk(dev, ADXL367_REG_THRESH_ACT_H, threshold >> 6,
				    ADXL367_THRESH_H);
	if (ret)
		return ret;
	ret = adxl367_reg_write_msk(dev, ADXL367_REG_THRESH_ACT_L,
				    (threshold & 0x3F) << 2, ADXL367_THRESH_L);
	if (ret)
		return ret;

	//set time
	return adxl367_set_register_value(dev, time, ADXL367_REG_TIME_ACT);
}

/***************************************************************************//**
 * @brief Configures inactivity detection.
 *
 * @param dev         - The device structure.
 * @param ref_or_abs  - Referenced/Absolute Inactivity Select.
 *                    Example: 0 - absolute mode.
 *                             1 - referenced mode.
 * @param threshold   - 13-bit unsigned value samples are
 *                    compared to.
 * @param time        - 16-bit value written to the timed inactivity register.
 * 					  Value = number of samples. For example, at 100Hz ODR,
 * 					  100 value translates to 1 second.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl367_setup_inactivity_detection(struct adxl367_dev *dev,
				       uint8_t  ref_or_abs,
				       uint16_t threshold,
				       uint16_t  time)
{
	int ret;

	switch (ref_or_abs) {
	case ADXL367_ABSOLUTE :
		ret = adxl367_reg_write_msk(dev,
					    ADXL367_REG_ACT_INACT_CTL,
					    no_os_field_prep(ADXL367_ACT_INACT_CTL_INACT_EN_MSK, ADXL367_INACTIVITY_ENABLE),
					    ADXL367_ACT_INACT_CTL_INACT_EN_MSK);
		break;
	case ADXL367_REFERENCED :
		ret = adxl367_reg_write_msk(dev,
					    ADXL367_REG_ACT_INACT_CTL,
					    no_os_field_prep(ADXL367_ACT_INACT_CTL_INACT_EN_MSK,
							    ADXL367_REFERENCED_INACTIVITY_ENABLE),
					    ADXL367_ACT_INACT_CTL_INACT_EN_MSK);
		break;
	default :
		return -1;
	}

	if (ret)
		return ret;

	//set threshold
	ret = adxl367_reg_write_msk(dev, ADXL367_REG_THRESH_INACT_H, threshold >> 6,
				    ADXL367_THRESH_H);
	if (ret)
		return ret;
	ret = adxl367_reg_write_msk(dev, ADXL367_REG_THRESH_INACT_L,
				    (threshold & 0x3F) << 2, ADXL367_THRESH_L);
	if (ret)
		return ret;

	//set time
	ret = adxl367_set_register_value(dev, time >> 8, ADXL367_REG_TIME_INACT_H);
	if (ret)
		return ret;
	return adxl367_set_register_value(dev, time & 0xFF, ADXL367_REG_TIME_INACT_L);
}
