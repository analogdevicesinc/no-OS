/***************************************************************************//**
 *   @file   adxl355.c
 *   @brief  Implementation of ADXL355 Driver.
 *   @author RBolboac (ramona.bolboaca@analog.com)
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
#include <errno.h>
#include "adxl355.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/
static uint8_t shadow_reg_val[5] = {0, 0, 0, 0, 0};
static const uint8_t adxl355_scale_mul[4] = {0, 1, 2, 4};
static const uint8_t adxl355_part_id[] = {
	[ID_ADXL355] = GET_ADXL355_RESET_VAL(ADXL355_PARTID),
	[ID_ADXL357] = GET_ADXL355_RESET_VAL(ADXL355_PARTID),
	[ID_ADXL359] = GET_ADXL355_RESET_VAL(ADXL359_PARTID),
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
static uint32_t adxl355_accel_array_conv(struct adxl355_dev *dev,
		uint8_t *raw_array);
static int64_t adxl355_accel_conv(struct adxl355_dev *dev, uint32_t raw_accel);
static int64_t adxl355_temp_conv(struct adxl355_dev *dev, uint16_t raw_temp);

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
int adxl355_read_device_data(struct adxl355_dev *dev, uint8_t base_address,
			     uint16_t size, uint8_t *read_data)
{
	int ret;

	if (dev->comm_type == ADXL355_SPI_COMM) {
		dev->comm_buff[0] = ADXL355_SPI_READ | (base_address << 1);
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
int adxl355_write_device_data(struct adxl355_dev *dev, uint8_t base_address,
			      uint16_t size, uint8_t *write_data)
{
	int ret;

	for (uint16_t idx = 0; idx < size; idx++)
		dev->comm_buff[1+idx] = write_data[idx];

	if (dev->comm_type == ADXL355_SPI_COMM) {
		dev->comm_buff[0] = ADXL355_SPI_WRITE | (base_address << 1);
		ret = no_os_spi_write_and_read(dev->com_desc.spi_desc, dev->comm_buff,
					       size + 1);
	} else {
		dev->comm_buff[0] = base_address;
		ret = no_os_i2c_write(dev->com_desc.i2c_desc, dev->comm_buff, size + 1, 1);
	}

	return ret;
}

/***************************************************************************//**
 * @brief Initializes the communication peripheral and checks if the ADXL355
 *        part is present.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       		   parameters.
 *
 * @return ret       - Result of the initialization procedure.
*******************************************************************************/
int adxl355_init(struct adxl355_dev **device,
		 struct adxl355_init_param init_param)
{
	struct adxl355_dev *dev;
	int ret;
	uint8_t reg_value;

	switch (init_param.dev_type) {
	case ID_ADXL355:
	case ID_ADXL357:
	case ID_ADXL359:
		break;
	default:
		return -EINVAL;
	}

	dev = (struct adxl355_dev *)no_os_calloc(1, sizeof(*dev));

	if (!dev)
		return -ENOMEM;

	dev->comm_type = init_param.comm_type;

	if (dev->comm_type == ADXL355_SPI_COMM) {
		ret = no_os_spi_init(&dev->com_desc.spi_desc, &(init_param.comm_init.spi_init));
		if (ret)
			goto error_dev;
	} else {
		ret = no_os_i2c_init(&dev->com_desc.i2c_desc, &init_param.comm_init.i2c_init);
		if (ret)
			goto error_dev;
	}

	dev->dev_type = init_param.dev_type;

	ret = adxl355_read_device_data(dev, ADXL355_ADDR(ADXL355_DEVID_AD),
				       GET_ADXL355_TRANSF_LEN(ADXL355_DEVID_AD), &reg_value);
	if (ret || (reg_value != GET_ADXL355_RESET_VAL(ADXL355_DEVID_AD)))
		goto error_com;

	ret = adxl355_read_device_data(dev, ADXL355_ADDR(ADXL355_DEVID_MST),
				       GET_ADXL355_TRANSF_LEN(ADXL355_DEVID_MST),&reg_value);
	if (ret || (reg_value != GET_ADXL355_RESET_VAL(ADXL355_DEVID_MST)))
		goto error_com;

	ret = adxl355_read_device_data(dev, ADXL355_ADDR(ADXL355_PARTID),
				       GET_ADXL355_TRANSF_LEN(ADXL355_PARTID),&reg_value);

	if (ret || reg_value != adxl355_part_id[dev->dev_type])
		goto error_com;

	// Get shadow register values
	ret = adxl355_read_device_data(dev,
				       ADXL355_ADDR(ADXL355_SHADOW_REGISTER_BASE_ADDR),
				       GET_ADXL355_TRANSF_LEN(ADXL355_SHADOW_REGISTER_BASE_ADDR),
				       &shadow_reg_val[0]);
	if (ret)
		goto error_com;

	// Default range is set
	dev->range = GET_ADXL355_RESET_VAL(ADXL355_RANGE) & ADXL355_RANGE_FIELD_MSK;

	// Default value for FIFO SAMPLES
	dev->fifo_samples = GET_ADXL355_RESET_VAL(ADXL355_FIFO_SAMPLES);

	// Default value for POWER_CTL
	dev->op_mode = GET_ADXL355_RESET_VAL(ADXL355_POWER_CTL);

	// Default activity count value
	dev->act_cnt = GET_ADXL355_RESET_VAL(ADXL355_ACT_CNT);

	*device = dev;

	return ret;
error_com:
	if (dev->comm_type == ADXL355_SPI_COMM)
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
 * @brief Free the resources allocated by adxl355_init().
 *
 * @param dev - The device structure.
 *
 * @return ret - Result of the remove procedure.
*******************************************************************************/
int adxl355_remove(struct adxl355_dev *dev)
{
	int ret;

	if (dev->comm_type == ADXL355_SPI_COMM)
		ret = no_os_spi_remove(dev->com_desc.spi_desc);
	else
		ret = no_os_i2c_remove(dev->com_desc.i2c_desc);

	no_os_free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Places the device into the given operation mode.
 *
 * @param dev     - The device structure.
 * @param op_mode - Operation mode mode.
 *
 * @return ret    - Result of the setting operation procedure.
*******************************************************************************/
int adxl355_set_op_mode(struct adxl355_dev *dev, enum adxl355_op_mode op_mode)
{
	int ret;

	ret = adxl355_write_device_data(dev, ADXL355_ADDR(ADXL355_POWER_CTL),
					GET_ADXL355_TRANSF_LEN(ADXL355_POWER_CTL), &op_mode);

	if (!ret)
		dev->op_mode = op_mode;

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
int adxl355_get_op_mode(struct adxl355_dev *dev,
			enum adxl355_op_mode *op_mode)
{
	int ret;

	ret = adxl355_read_device_data(dev, ADXL355_ADDR(ADXL355_POWER_CTL),
				       GET_ADXL355_TRANSF_LEN(ADXL355_POWER_CTL), op_mode);

	if (!ret)
		dev->op_mode = *op_mode;

	return ret;
}

/***************************************************************************//**
 * @brief Performs a soft reset of the device.
 *
 * @param dev  - The device structure.
 *
 * @return ret - Result of the soft reset procedure.
*******************************************************************************/
int adxl355_soft_reset(struct adxl355_dev *dev)
{
	uint8_t register_values[5];
	int ret;
	uint8_t nb_of_retries = 255;
	uint8_t data = ADXL355_RESET_CODE;
	union adxl355_sts_reg_flags flags;

	// Perform soft reset
	ret = adxl355_write_device_data(dev, ADXL355_ADDR(ADXL355_RESET),
					GET_ADXL355_TRANSF_LEN(ADXL355_RESET), &data);
	if (ret)
		return ret;

	// After soft reset, the data in the shadow registers will be valid only after NVM is not busy anymore
	ret = adxl355_get_sts_reg(dev, &flags);
	while (flags.fields.NVM_BUSY && nb_of_retries) {
		ret = adxl355_get_sts_reg(dev, &flags);
		nb_of_retries--;
	}
	if ((!nb_of_retries) || ret)
		return -EAGAIN;

	// Delay is needed between soft reset command and shadow registers reading
	no_os_mdelay(1);

	// Read the shadow registers
	ret = adxl355_read_device_data(dev,
				       ADXL355_ADDR(ADXL355_SHADOW_REGISTER_BASE_ADDR),
				       GET_ADXL355_TRANSF_LEN(ADXL355_SHADOW_REGISTER_BASE_ADDR),
				       &register_values[0]);
	if (strncmp((const char*)register_values, (const char*)shadow_reg_val,
		    GET_ADXL355_TRANSF_LEN(ADXL355_SHADOW_REGISTER_BASE_ADDR)))
		ret = -EAGAIN;

	return ret;
}

/***************************************************************************//**
 * @brief Triggers the self-test feature.
 *
 * @param dev  - The device structure.
 *
 * @return ret - Result of the writing procedure.
*******************************************************************************/
int adxl355_set_self_test(struct adxl355_dev *dev)
{
	uint8_t data = ADXL355_SELF_TEST_TRIGGER_VAL;

	return adxl355_write_device_data(dev, ADXL355_ADDR(ADXL355_SELF_TEST),
					 GET_ADXL355_TRANSF_LEN(ADXL355_SELF_TEST), &data);
}

/***************************************************************************//**
 * @brief Sets the measurement range register value.
 *
 * @param dev       - The device structure.
 * @param range_val - Selected range.
 *
 * @return ret      - Result of the writing procedure.
*******************************************************************************/
int adxl355_set_range(struct adxl355_dev *dev, enum adxl355_range range_val)
{
	int ret;
	uint8_t range_reg;

	ret = adxl355_read_device_data(dev, ADXL355_ADDR(ADXL355_RANGE),
				       GET_ADXL355_TRANSF_LEN(ADXL355_RANGE), &range_reg);
	if (ret)
		return ret;

	// Reset range bits
	range_reg &= ~ADXL355_RANGE_FIELD_MSK;
	// Set only range bits inside range registers
	range_reg |= range_val & ADXL355_RANGE_FIELD_MSK;
	ret = adxl355_write_device_data(dev, ADXL355_ADDR(ADXL355_RANGE),
					GET_ADXL355_TRANSF_LEN(ADXL355_RANGE), &range_reg);

	if (!ret)
		dev->range = range_val;

	return ret;
}

/***************************************************************************//**
 * @brief Writes the low-pass filter settings.
 *
 * @param dev         - The device structure.
 * @param odr_lpf_val - Low-pass filter settings.
 *
 * @return ret        - Result of the writing procedure.
*******************************************************************************/
int adxl355_set_odr_lpf(struct adxl355_dev *dev,
			enum adxl355_odr_lpf odr_lpf_val)
{
	int ret;
	uint8_t reg_value;
	enum adxl355_op_mode current_op_mode;

	// The lpf settings cannot be changed when the device is in measurement mode.
	// The lpf settings can be changed only when the device's operation mode is
	// Standby.

	current_op_mode = dev->op_mode;

	switch(current_op_mode) {
	case ADXL355_MEAS_TEMP_ON_DRDY_ON:
	case ADXL355_MEAS_TEMP_OFF_DRDY_ON:
	case ADXL355_MEAS_TEMP_ON_DRDY_OFF:
	case ADXL355_MEAS_TEMP_OFF_DRDY_OFF:
		ret = adxl355_set_op_mode(dev, ADXL355_STDBY_TEMP_ON_DRDY_ON);
		if (ret)
			return ret;
		break;
	default:
		break;
	}

	ret = adxl355_read_device_data(dev, ADXL355_ADDR(ADXL355_FILTER),
				       GET_ADXL355_TRANSF_LEN(ADXL355_FILTER), &reg_value);
	if (ret)
		return ret;

	reg_value &= ~(ADXL355_ODR_LPF_FIELD_MSK);
	reg_value |= odr_lpf_val & ADXL355_ODR_LPF_FIELD_MSK;
	ret = adxl355_write_device_data(dev, ADXL355_ADDR(ADXL355_FILTER),
					GET_ADXL355_TRANSF_LEN(ADXL355_FILTER), &reg_value);
	if (ret)
		return ret;

	dev->odr_lpf = odr_lpf_val;

	return adxl355_set_op_mode(dev, current_op_mode);
}

/***************************************************************************//**
 * @brief Writes the high-pass filter settings.
 *
 * @param dev            - The device structure.
 * @param hpf_corner_val - High-pass filter settings.
 *
 * @return ret           - Result of the writing procedure.
*******************************************************************************/
int adxl355_set_hpf_corner(struct adxl355_dev *dev,
			   enum adxl355_hpf_corner hpf_corner_val)
{
	int ret;
	uint8_t reg_value;
	enum adxl355_op_mode current_op_mode;

	// Even though the hpf settings can be changed when the device is in
	// measurement mode, it has been observed that the measurements are not
	// correct when doing so. Because of this, the device will be set in standby
	// mode also when changing the hpf value.

	current_op_mode = dev->op_mode;

	switch(current_op_mode) {
	case ADXL355_MEAS_TEMP_ON_DRDY_ON:
	case ADXL355_MEAS_TEMP_OFF_DRDY_ON:
	case ADXL355_MEAS_TEMP_ON_DRDY_OFF:
	case ADXL355_MEAS_TEMP_OFF_DRDY_OFF:
		ret = adxl355_set_op_mode(dev, ADXL355_STDBY_TEMP_ON_DRDY_ON);
		if (ret)
			return ret;
		break;
	default:
		break;
	}

	ret = adxl355_read_device_data(dev, ADXL355_ADDR(ADXL355_FILTER),
				       GET_ADXL355_TRANSF_LEN(ADXL355_FILTER), &reg_value);
	if (ret)
		return ret;
	reg_value &= ~(ADXL355_HPF_FIELD_MSK);
	reg_value |= (hpf_corner_val << 4) & ADXL355_HPF_FIELD_MSK;

	ret = adxl355_write_device_data(dev, ADXL355_ADDR(ADXL355_FILTER),
					GET_ADXL355_TRANSF_LEN(ADXL355_FILTER), &reg_value);
	if (!ret)
		dev->hpf_corner = hpf_corner_val;

	return adxl355_set_op_mode(dev, current_op_mode);
}

/***************************************************************************//**
 * @brief Sets an offset value for each axis (Offset Calibration).
 *
 * @param dev      - The device structure.
 * @param x_offset - X-axis's offset.
 * @param y_offset - Y-axis's offset.
 * @param z_offset - Z-axis's offset.
 *
 * @return ret     - Result of the writing procedure.
*******************************************************************************/
int adxl355_set_offset(struct adxl355_dev *dev,uint16_t x_offset,
		       uint16_t y_offset, uint16_t z_offset)
{
	int ret;
	uint8_t data_offset_x[2] = {x_offset >> 8, (uint8_t)x_offset};
	uint8_t data_offset_y[2] = {y_offset >> 8, (uint8_t)y_offset};
	uint8_t data_offset_z[2] = {z_offset >> 8, (uint8_t)z_offset};

	ret = adxl355_write_device_data(dev, ADXL355_ADDR(ADXL355_OFFSET_X),
					GET_ADXL355_TRANSF_LEN(ADXL355_OFFSET_X),
					(uint8_t*)data_offset_x);
	if (ret)
		return ret;

	ret = adxl355_write_device_data(dev, ADXL355_ADDR(ADXL355_OFFSET_Y),
					GET_ADXL355_TRANSF_LEN(ADXL355_OFFSET_Y),
					(uint8_t*)data_offset_y);
	if (ret)
		return ret;

	ret = adxl355_write_device_data(dev, ADXL355_ADDR(ADXL355_OFFSET_Z),
					GET_ADXL355_TRANSF_LEN(ADXL355_OFFSET_Z),
					(uint8_t*)data_offset_z);

	if (!ret) {
		dev->x_offset = x_offset;
		dev->y_offset = y_offset;
		dev->z_offset = z_offset;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Reads the raw output data.
 *
 * @param dev   - The device structure.
 * @param raw_x - X-axis's raw output data.
 * @param raw_y - Y-axis's raw output data.
 * @param raw_z - Z-axis's raw output data.
 *
 * @return ret  - Result of the reading procedure.
*******************************************************************************/
int adxl355_get_raw_xyz(struct adxl355_dev *dev, uint32_t *raw_x,
			uint32_t *raw_y, uint32_t *raw_z)
{
	uint8_t array_raw_x[GET_ADXL355_TRANSF_LEN(ADXL355_XDATA)] = {0};
	uint8_t array_raw_y[GET_ADXL355_TRANSF_LEN(ADXL355_YDATA)] = {0};
	uint8_t array_raw_z[GET_ADXL355_TRANSF_LEN(ADXL355_ZDATA)] = {0};
	int ret;

	ret = adxl355_read_device_data(dev,ADXL355_ADDR(ADXL355_XDATA),
				       GET_ADXL355_TRANSF_LEN(ADXL355_XDATA),array_raw_x);
	if (ret)
		return ret;
	*raw_x = adxl355_accel_array_conv(dev, array_raw_x);

	ret = adxl355_read_device_data(dev,ADXL355_ADDR(ADXL355_YDATA),
				       GET_ADXL355_TRANSF_LEN(ADXL355_YDATA),array_raw_y);
	if (ret)
		return ret;
	*raw_y = adxl355_accel_array_conv(dev, array_raw_y);

	ret = adxl355_read_device_data(dev,ADXL355_ADDR(ADXL355_ZDATA),
				       GET_ADXL355_TRANSF_LEN(ADXL355_ZDATA), array_raw_z);
	if (ret)
		return ret;
	*raw_z = adxl355_accel_array_conv(dev, array_raw_z);

	return ret;
}

/***************************************************************************//**
 * @brief Reads the raw output data of each axis and converts it to g.
 *
 * @param dev  - The device structure.
 * @param x    - X-axis's output data.
 * @param y    - Y-axis's output data.
 * @param z    - Z-axis's output data.
 *
 * @return ret - Result of the reading procedure.
*******************************************************************************/
int adxl355_get_xyz(struct adxl355_dev *dev, struct adxl355_frac_repr *x,
		    struct adxl355_frac_repr *y, struct adxl355_frac_repr *z)
{
	uint32_t raw_accel_x;
	uint32_t raw_accel_y;
	uint32_t raw_accel_z;
	int ret;

	ret = adxl355_get_raw_xyz(dev, &raw_accel_x, &raw_accel_y, &raw_accel_z);
	if (ret)
		return ret;

	x->integer = no_os_div_s64_rem(adxl355_accel_conv(dev, raw_accel_x),
				       ADXL355_ACC_SCALE_FACTOR_DIV, &(x->fractional));
	y->integer = no_os_div_s64_rem(adxl355_accel_conv(dev, raw_accel_y),
				       ADXL355_ACC_SCALE_FACTOR_DIV, &(y->fractional));
	z->integer = no_os_div_s64_rem(adxl355_accel_conv(dev, raw_accel_z),
				       ADXL355_ACC_SCALE_FACTOR_DIV, &(z->fractional));

	return ret;
}

/***************************************************************************//**
 * @brief Reads the raw temperature.
 *
 * @param dev      - The device structure.
 * @param raw_temp - Raw temperature output data.
 *
 * @return ret     - Result of the reading procedure.
*******************************************************************************/
int adxl355_get_raw_temp(struct adxl355_dev *dev, uint16_t *raw_temp)
{
	uint8_t raw_data[2];
	int ret;

	ret = adxl355_read_device_data(dev, ADXL355_ADDR(ADXL355_TEMP),
				       GET_ADXL355_TRANSF_LEN(ADXL355_TEMP), raw_data);
	if (!ret)
		// raw_data[0] bits [7-4]: reserved
		// raw_data[0] bits [3-0]: DATA bits [11: 8]
		// raw_data[1] bits [7-0]: DATA bits [ 7: 0]
		*raw_temp = ((raw_data[0] & NO_OS_GENMASK(3, 0)) << 8) | raw_data[1];

	return ret;
}

/***************************************************************************//**
 * @brief Reads the raw temperature data and converts it to millidegrees Celsius.
 *
 * @param dev  - The device structure.
 * @param temp - Temperature output data.
 *
 * @return ret - Result of the reading procedure.
*******************************************************************************/
int adxl355_get_temp(struct adxl355_dev *dev, struct adxl355_frac_repr *temp)
{
	uint16_t raw_temp;
	int ret;
	int32_t divisor;

	ret = adxl355_get_raw_temp(dev, &raw_temp);
	if(ret)
		return ret;

	switch(dev->dev_type) {
	case ID_ADXL355:
	case ID_ADXL357:
		divisor = ADXL355_TEMP_OFFSET_DIV*ADXL355_TEMP_SCALE_FACTOR_DIV;
		break;
	case ID_ADXL359:
		divisor = ADXL359_TEMP_OFFSET_DIV*ADXL359_TEMP_SCALE_FACTOR_DIV;
		break;
	default:
		return -EINVAL;
	}

	temp->integer = no_os_div_s64_rem(adxl355_temp_conv(dev, raw_temp),
					  divisor,
					  &(temp->fractional));
	return 0;
}

/***************************************************************************//**
 * @brief Reads the status register value.
 *
 * @param dev          - The device structure.
 * @param status_flags - Register value.
 *
 * @return ret         - Result of the reading procedure.
*******************************************************************************/
int adxl355_get_sts_reg(struct adxl355_dev *dev,
			union adxl355_sts_reg_flags *status_flags)
{
	int ret;
	uint8_t reg_value;

	ret = adxl355_read_device_data(dev, ADXL355_ADDR(ADXL355_STATUS),
				       GET_ADXL355_TRANSF_LEN(ADXL355_STATUS), &reg_value);

	if (!ret)
		status_flags->value = reg_value;

	return ret;
}

/***************************************************************************//**
 * @brief Reads the number of FIFO entries register value.
 *
 * @param dev       - The device structure.
 * @param reg_value - Register value.
 *
 * @return ret      - Result of the reading procedure.
*******************************************************************************/
int adxl355_get_nb_of_fifo_entries(struct adxl355_dev *dev, uint8_t *reg_value)
{
	return adxl355_read_device_data(dev, ADXL355_ADDR(ADXL355_FIFO_ENTRIES),
					GET_ADXL355_TRANSF_LEN(ADXL355_FIFO_ENTRIES), reg_value);
}

/***************************************************************************//**
 * @brief Sets the number of FIFO samples register value.
 *
 * @param dev       - The device structure.
 * @param reg_value - Register value.
 *
 * @return ret      - Result of the writing procedure.
*******************************************************************************/
int adxl355_set_fifo_samples(struct adxl355_dev *dev, uint8_t reg_value)
{
	int ret;

	if (reg_value > ADXL355_MAX_FIFO_SAMPLES_VAL)
		return -EINVAL;

	ret = adxl355_write_device_data(dev, ADXL355_ADDR(ADXL355_FIFO_SAMPLES),
					GET_ADXL355_TRANSF_LEN(ADXL355_FIFO_SAMPLES), &reg_value);

	if (!ret)
		dev->fifo_samples = reg_value;

	return ret;
}

/***************************************************************************//**
 * @brief Reads fifo data and returns the raw values.
 *
 * @param dev          - The device structure.
 * @param fifo_entries - The number of fifo entries.
 * @param raw_x        - Raw x-axis data.
 * @param raw_y        - Raw y-axis data.
 * @param raw_z        - Raw z-axis data.
 *
 * @return ret         - Result of the configuration procedure.
*******************************************************************************/
int adxl355_get_raw_fifo_data(struct adxl355_dev *dev, uint8_t *fifo_entries,
			      uint32_t *raw_x, uint32_t *raw_y, uint32_t *raw_z)
{
	int ret;

	ret = adxl355_get_nb_of_fifo_entries(dev, fifo_entries);
	if (ret)
		return ret;

	if (*fifo_entries > 0) {

		ret = adxl355_read_device_data(dev, ADXL355_ADDR(ADXL355_FIFO_DATA),
					       *fifo_entries * 3, dev->comm_buff);
		if (ret)
			return ret;

		for (uint16_t idx = 0; idx < *fifo_entries * 3; idx = idx + 9) {
			if (((dev->comm_buff[idx+2] & 1) == 1)
			    && ((dev->comm_buff[idx+2] & 2) == 0)) {
				// This is x-axis
				// Process data
				raw_x[idx/9] = adxl355_accel_array_conv(dev, &dev->comm_buff[idx]);
				raw_y[idx/9] = adxl355_accel_array_conv(dev, &dev->comm_buff[idx+3]);
				raw_z[idx/9] = adxl355_accel_array_conv(dev, &dev->comm_buff[idx+6]);
			}
		}
	}

	return ret;
}

/***************************************************************************//**
 * @brief Reads fifo data and returns the values converted in m/s^2.
 *
 * @param dev          - The device structure.
 * @param fifo_entries - The number of fifo entries.
 * @param x            - Converted x-axis data.
 * @param y            - Converted y-axis data.
 * @param z            - Converted z-axis data.
 *
 * @return ret         - Result of the configuration procedure.
*******************************************************************************/
int adxl355_get_fifo_data(struct adxl355_dev *dev, uint8_t *fifo_entries,
			  struct adxl355_frac_repr *x,
			  struct adxl355_frac_repr *y,
			  struct adxl355_frac_repr *z)
{
	int ret;
	uint32_t raw_x[32];
	uint32_t raw_y[32];
	uint32_t raw_z[32];

	ret = adxl355_get_raw_fifo_data(dev, fifo_entries, raw_x, raw_y, raw_z);
	if (ret)
		return ret;

	if (*fifo_entries > 0) {
		for (uint8_t idx = 0; idx < *fifo_entries/3; idx++) {
			x[idx].integer = no_os_div_s64_rem(adxl355_accel_conv(dev, raw_x[idx]),
							   ADXL355_ACC_SCALE_FACTOR_DIV, &(x[idx].fractional));
			y[idx].integer = no_os_div_s64_rem(adxl355_accel_conv(dev, raw_y[idx]),
							   ADXL355_ACC_SCALE_FACTOR_DIV, &(y[idx].fractional));
			z[idx].integer = no_os_div_s64_rem(adxl355_accel_conv(dev, raw_z[idx]),
							   ADXL355_ACC_SCALE_FACTOR_DIV, &(z[idx].fractional));
		}
	}

	return ret;
}

/***************************************************************************//**
 * @brief Configures the activity enable register.
 *
 * @param dev         - The device structure.
 * @param act_config  - Activity enable mapping.
 *
 * @return ret         - Result of the configuration procedure.
*******************************************************************************/
int adxl355_conf_act_en(struct adxl355_dev *dev,
			union adxl355_act_en_flags act_config)
{
	int ret;
	uint8_t reg_val = act_config.value;

	ret = adxl355_write_device_data(dev, ADXL355_ADDR(ADXL355_ACT_EN),
					GET_ADXL355_TRANSF_LEN(ADXL355_ACT_EN), &reg_val);

	if (!ret)
		dev->act_en = act_config;

	return ret;
}

/***************************************************************************//**
 * @brief Configures the activity threshold registers.
 *
 * @param dev     - The device structure.
 * @param act_thr - Activity threshold value.
 *
 * @return ret    - Result of the configuration procedure.
*******************************************************************************/
int adxl355_conf_act_thr(struct adxl355_dev *dev, uint16_t act_thr)
{
	int ret;
	uint8_t data[2] = {act_thr >> 8, (uint8_t)act_thr};

	ret = adxl355_write_device_data(dev, ADXL355_ADDR(ADXL355_ACT_THRESH),
					GET_ADXL355_TRANSF_LEN(ADXL355_ACT_THRESH), (uint8_t*)data);

	if (!ret)
		dev->act_thr = act_thr;

	return ret;
}

/***************************************************************************//**
 * @brief Writes the activity count register value.
 *
 * @param dev     - The device structure.
 * @param act_cnt - Register value.
 *
 * @return ret    - Result of the writing procedure.
*******************************************************************************/
int adxl355_set_act_cnt_reg(struct adxl355_dev *dev, uint8_t act_cnt)
{
	int ret;
	ret = adxl355_write_device_data(dev, ADXL355_ADDR(ADXL355_ACT_CNT),
					GET_ADXL355_TRANSF_LEN(ADXL355_ACT_CNT), &act_cnt);
	if (!ret)
		dev->act_cnt = act_cnt;

	return ret;
}

/***************************************************************************//**
 * @brief Configures the interrupt map for INT1 and INT2 pins.
 *
 * @param dev      - The device structure.
 * @param int_conf - Interrupt mapping.
 *
 * @return ret     - Result of the configuration procedure.
*******************************************************************************/
int adxl355_config_int_pins(struct adxl355_dev *dev,
			    union adxl355_int_mask int_conf)
{
	uint8_t reg_val = int_conf.value;

	return adxl355_write_device_data(dev, ADXL355_ADDR(ADXL355_INT_MAP),
					 GET_ADXL355_TRANSF_LEN(ADXL355_INT_MAP), &reg_val);
}

/***************************************************************************//**
 * @brief Sets the interrupt polarity.
 *
 * @param dev     - The device structure.
 * @param int_pol - Interrupt polarity to be set.
 *
 * @return ret    - Result of the reading procedure.
*******************************************************************************/
int adxl355_set_int_pol(struct adxl355_dev *dev, enum adxl355_int_pol int_pol)
{
	int ret;
	uint8_t reg_value;

	ret = adxl355_read_device_data(dev, ADXL355_ADDR(ADXL355_RANGE),
				       GET_ADXL355_TRANSF_LEN(ADXL355_RANGE), &reg_value);
	if (ret)
		return ret;
	reg_value &= ~(ADXL355_INT_POL_FIELD_MSK);
	reg_value |= ((int_pol) << 6) & ADXL355_INT_POL_FIELD_MSK;
	ret = adxl355_write_device_data(dev, ADXL355_ADDR(ADXL355_RANGE),
					GET_ADXL355_TRANSF_LEN(ADXL355_RANGE), &reg_value);

	return ret;
}

/***************************************************************************//**
 * @brief Converts array of raw acceleration to uint32 data raw acceleration
 *
 * @param dev       - The device structure.
 * @param raw_array - Raw acceleration array.
 *
 * @return ret      - Converted data.
*******************************************************************************/
static uint32_t adxl355_accel_array_conv(struct adxl355_dev *dev,
		uint8_t *raw_array)
{
	uint32_t raw_accel;

	// raw_array[0] bits [7-0]: DATA bits [19:12]
	// raw_array[1] bits [7-0]: DATA bits [11: 4]
	// raw_array[2] bits [7-4]: DATA bits [ 3: 0]
	// raw_array[2] bits i[3-0]: reserved
	raw_accel = no_os_get_unaligned_be24(raw_array);

	return (raw_accel >> 4);
}

/***************************************************************************//**
 * @brief Converts raw acceleration value to m/s^2 value.
 *
 * @param dev       - The device structure.
 * @param raw_accel - Raw acceleration value.
 *
 * @return ret      - Converted data.
*******************************************************************************/
static int64_t adxl355_accel_conv(struct adxl355_dev *dev,
				  uint32_t raw_accel)
{
	int accel_data;

	// Raw acceleration is in two's complement
	// Convert from two's complement to int

	if ((raw_accel & NO_OS_BIT(19)) == NO_OS_BIT(19))
		accel_data = raw_accel | ADXL355_NEG_ACC_MSK;
	else
		accel_data = raw_accel;

	// Apply scale factor based on the selected range
	switch (dev->dev_type) {
	case ID_ADXL355:
		return ((int64_t)(accel_data * ADXL355_ACC_SCALE_FACTOR_MUL *
				  adxl355_scale_mul[dev->range]));
	case ID_ADXL357:
	case ID_ADXL359:
		return ((int64_t)(accel_data * ADXL359_ACC_SCALE_FACTOR_MUL *
				  adxl355_scale_mul[dev->range]));
	default:
		return 0;
	}
}

/***************************************************************************//**
 * @brief Converts raw temperature data to degrees Celsius data.
 *
 * @param dev      - The device structure.
 * @param raw_temp - Raw temperature data.
 *
 * @return ret     - Converted data.
*******************************************************************************/
static int64_t adxl355_temp_conv(struct adxl355_dev *dev, uint16_t raw_temp)
{
	switch(dev->dev_type) {
	case ID_ADXL355:
	case ID_ADXL357:
		return ((raw_temp*ADXL355_TEMP_OFFSET_DIV +  ADXL355_TEMP_OFFSET) *
			(int64_t)ADXL355_TEMP_SCALE_FACTOR);
	case ID_ADXL359:
		return ((raw_temp*ADXL359_TEMP_OFFSET_DIV +  ADXL359_TEMP_OFFSET) *
			(int64_t)ADXL359_TEMP_SCALE_FACTOR);
	default:
		return 0;
	}
}
