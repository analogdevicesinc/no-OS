/***************************************************************************//**
 *   @file   adxl355_capi.c
 *   @brief  Implementation of ADXL355 CAPI Driver.
 *   @author Claude Code (noreply@anthropic.com)
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
#include "adxl355_capi.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"

static uint8_t shadow_reg_val[5] = {0, 0, 0, 0, 0};
static const uint8_t adxl355_capi_scale_mul[4] = {0, 1, 2, 4};
static const uint8_t adxl355_capi_part_id[] = {
	[ID_ADXL355_CAPI] = GET_ADXL355_RESET_VAL(ADXL355_PARTID),
	[ID_ADXL357_CAPI] = GET_ADXL355_RESET_VAL(ADXL355_PARTID),
	[ID_ADXL359_CAPI] = GET_ADXL355_RESET_VAL(ADXL359_PARTID),
};

static uint32_t adxl355_capi_accel_array_conv(struct adxl355_capi_dev *dev,
					       uint8_t *raw_array);
static int64_t adxl355_capi_accel_conv(struct adxl355_capi_dev *dev, uint32_t raw_accel);
static int64_t adxl355_capi_temp_conv(struct adxl355_capi_dev *dev, uint16_t raw_temp);

/***************************************************************************//**
 * @brief Reads from the device using CAPI SPI.
 *
 * @param dev          - The device structure.
 * @param base_address - Address of the base register.
 * @param size         - The number of bytes to be read and returned in read_data.
 * @param read_data    - The read data buffer
 *
 * @return ret         - Result of the reading procedure.
*******************************************************************************/
int adxl355_capi_read_device_data(struct adxl355_capi_dev *dev, uint8_t base_address,
				  uint16_t size, uint8_t *read_data)
{
	struct capi_spi_transfer transfer = {0};
	int ret;

	if (!dev || !dev->spi_device || !read_data)
		return -EINVAL;

	/* Prepare command byte for read operation */
	dev->comm_buff[0] = ADXL355_SPI_READ | (base_address << 1);

	/* Set up SPI transfer structure */
	transfer.tx_buf = dev->comm_buff;
	transfer.rx_buf = dev->comm_buff;
	transfer.tx_size = 1 + size;
	transfer.rx_size = 1 + size;

	/* Perform SPI transaction */
	ret = capi_spi_transceive(dev->spi_device, &transfer);
	if (ret)
		return ret;

	/* Copy received data (skip command byte) */
	for (uint16_t idx = 0; idx < size; idx++)
		read_data[idx] = dev->comm_buff[idx + 1];

	return 0;
}

/***************************************************************************//**
 * @brief Writes to the device using CAPI SPI
 *
 * @param dev          - The device structure.
 * @param base_address - Address of the base register.
 * @param size         - The number of bytes to be written. It is the size
 *                       of the write_data buffer.
 * @param write_data   - The data which is going to be written.
 *
 * @return ret         - Result of the writing procedure.
*******************************************************************************/
int adxl355_capi_write_device_data(struct adxl355_capi_dev *dev, uint8_t base_address,
				   uint16_t size, uint8_t *write_data)
{
	struct capi_spi_transfer transfer = {0};
	int ret;

	if (!dev || !dev->spi_device || !write_data)
		return -EINVAL;

	/* Prepare command byte for write operation */
	dev->comm_buff[0] = ADXL355_SPI_WRITE | (base_address << 1);

	/* Copy write data to communication buffer */
	for (uint16_t idx = 0; idx < size; idx++)
		dev->comm_buff[1 + idx] = write_data[idx];

	/* Set up SPI transfer structure */
	transfer.tx_buf = dev->comm_buff;
	transfer.rx_buf = NULL;  /* Write-only operation */
	transfer.tx_size = size + 1;
	transfer.rx_size = 0;

	/* Perform SPI transaction */
	ret = capi_spi_transceive(dev->spi_device, &transfer);

	return ret;
}

/***************************************************************************//**
 * @brief Initializes the SPI communication peripheral and checks if the ADXL355
 *        part is present.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       		   parameters.
 *
 * @return ret       - Result of the initialization procedure.
*******************************************************************************/
int adxl355_capi_init(struct adxl355_capi_dev **device,
		      struct adxl355_capi_init_param init_param)
{
	struct adxl355_capi_dev *dev;
	int ret;
	uint8_t reg_value;

	if (!device)
		return -EINVAL;

	switch (init_param.dev_type) {
	case ID_ADXL355_CAPI:
	case ID_ADXL357_CAPI:
	case ID_ADXL359_CAPI:
		break;
	default:
		return -EINVAL;
	}

	dev = (struct adxl355_capi_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* Store SPI controller handle */
	dev->spi_controller = init_param.spi_controller;
	dev->spi_device = init_param.spi_dev;
	dev->dev_type = init_param.dev_type;

	/* Check device ID registers */
	ret = adxl355_capi_read_device_data(dev, ADXL355_ADDR(ADXL355_DEVID_AD),
					    GET_ADXL355_TRANSF_LEN(ADXL355_DEVID_AD), &reg_value);
	if (ret || (reg_value != GET_ADXL355_RESET_VAL(ADXL355_DEVID_AD)))
		goto error_dev;

	ret = adxl355_capi_read_device_data(dev, ADXL355_ADDR(ADXL355_DEVID_MST),
					    GET_ADXL355_TRANSF_LEN(ADXL355_DEVID_MST), &reg_value);
	if (ret || (reg_value != GET_ADXL355_RESET_VAL(ADXL355_DEVID_MST)))
		goto error_dev;

	ret = adxl355_capi_read_device_data(dev, ADXL355_ADDR(ADXL355_PARTID),
					    GET_ADXL355_TRANSF_LEN(ADXL355_PARTID), &reg_value);

	if (ret || reg_value != adxl355_capi_part_id[dev->dev_type])
		goto error_dev;

	/* Get shadow register values */
	ret = adxl355_capi_read_device_data(dev,
					    ADXL355_ADDR(ADXL355_SHADOW_REGISTER_BASE_ADDR),
					    GET_ADXL355_TRANSF_LEN(ADXL355_SHADOW_REGISTER_BASE_ADDR),
					    &shadow_reg_val[0]);
	if (ret)
		goto error_dev;

	/* Initialize device default values */
	dev->range = GET_ADXL355_RESET_VAL(ADXL355_RANGE) & ADXL355_RANGE_FIELD_MSK;
	dev->fifo_samples = GET_ADXL355_RESET_VAL(ADXL355_FIFO_SAMPLES);
	dev->op_mode = GET_ADXL355_RESET_VAL(ADXL355_POWER_CTL);
	dev->act_cnt = GET_ADXL355_RESET_VAL(ADXL355_ACT_CNT);

	*device = dev;

	return 0;

error_dev:
	no_os_free(dev);
	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by adxl355_capi_init().
 *
 * @param dev - The device structure.
 *
 * @return ret - Result of the remove procedure.
*******************************************************************************/
int adxl355_capi_remove(struct adxl355_capi_dev *dev)
{
	if (!dev)
		return -EINVAL;

	no_os_free(dev);

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
int adxl355_capi_set_op_mode(struct adxl355_capi_dev *dev, enum adxl355_capi_op_mode op_mode)
{
	int ret;
	uint8_t mode_val = (uint8_t)op_mode;

	if (!dev)
		return -EINVAL;

	ret = adxl355_capi_write_device_data(dev, ADXL355_ADDR(ADXL355_POWER_CTL),
					     GET_ADXL355_TRANSF_LEN(ADXL355_POWER_CTL), &mode_val);

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
int adxl355_capi_get_op_mode(struct adxl355_capi_dev *dev,
			     enum adxl355_capi_op_mode *op_mode)
{
	int ret;
	uint8_t mode_val;

	if (!dev || !op_mode)
		return -EINVAL;

	ret = adxl355_capi_read_device_data(dev, ADXL355_ADDR(ADXL355_POWER_CTL),
					    GET_ADXL355_TRANSF_LEN(ADXL355_POWER_CTL), &mode_val);

	if (!ret) {
		*op_mode = (enum adxl355_capi_op_mode)mode_val;
		dev->op_mode = *op_mode;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Performs a soft reset of the device.
 *
 * @param dev  - The device structure.
 *
 * @return ret - Result of the soft reset procedure.
*******************************************************************************/
int adxl355_capi_soft_reset(struct adxl355_capi_dev *dev)
{
	uint8_t register_values[5];
	int ret;
	uint8_t nb_of_retries = 255;
	uint8_t data = ADXL355_RESET_CODE;
	union adxl355_capi_sts_reg_flags flags;

	if (!dev)
		return -EINVAL;

	/* Perform soft reset */
	ret = adxl355_capi_write_device_data(dev, ADXL355_ADDR(ADXL355_RESET),
					     GET_ADXL355_TRANSF_LEN(ADXL355_RESET), &data);
	if (ret)
		return ret;

	/* After soft reset, the data in the shadow registers will be valid only after NVM is not busy anymore */
	ret = adxl355_capi_get_sts_reg(dev, &flags);
	while (flags.fields.NVM_BUSY && nb_of_retries) {
		ret = adxl355_capi_get_sts_reg(dev, &flags);
		if (ret)
			return ret;
		nb_of_retries--;
	}

	if (!nb_of_retries)
		return -ETIMEDOUT;

	/* Get shadow register values */
	ret = adxl355_capi_read_device_data(dev,
					    ADXL355_ADDR(ADXL355_SHADOW_REGISTER_BASE_ADDR),
					    GET_ADXL355_TRANSF_LEN(ADXL355_SHADOW_REGISTER_BASE_ADDR),
					    &register_values[0]);

	/* Update cached shadow register values */
	for (uint8_t i = 0; i < 5; i++)
		shadow_reg_val[i] = register_values[i];

	/* Update device default values after reset */
	dev->range = GET_ADXL355_RESET_VAL(ADXL355_RANGE) & ADXL355_RANGE_FIELD_MSK;
	dev->fifo_samples = GET_ADXL355_RESET_VAL(ADXL355_FIFO_SAMPLES);
	dev->op_mode = GET_ADXL355_RESET_VAL(ADXL355_POWER_CTL);
	dev->act_cnt = GET_ADXL355_RESET_VAL(ADXL355_ACT_CNT);

	return ret;
}

/***************************************************************************//**
 * @brief Triggers the self-test feature.
 *
 * @param dev  - The device structure.
 *
 * @return ret - Result of the self-test enable procedure.
*******************************************************************************/
int adxl355_capi_set_self_test(struct adxl355_capi_dev *dev)
{
	uint8_t data = ADXL355_SELF_TEST_TRIGGER_VAL;

	if (!dev)
		return -EINVAL;

	return adxl355_capi_write_device_data(dev, ADXL355_ADDR(ADXL355_SELF_TEST),
					      GET_ADXL355_TRANSF_LEN(ADXL355_SELF_TEST), &data);
}

/***************************************************************************//**
 * @brief Sets the measurement range register value.
 *
 * @param dev       - The device structure.
 * @param range_val - Range register value.
 *
 * @return ret      - Result of the setting measurement range procedure.
*******************************************************************************/
int adxl355_capi_set_range(struct adxl355_capi_dev *dev, enum adxl355_capi_range range_val)
{
	int ret;
	uint8_t range_reg_val;
	uint8_t hpf_val = (shadow_reg_val[3] & ADXL355_HPF_FIELD_MSK) >> 4;

	if (!dev)
		return -EINVAL;

	range_reg_val = (range_val & ADXL355_RANGE_FIELD_MSK) | (hpf_val << 4);

	ret = adxl355_capi_write_device_data(dev, ADXL355_ADDR(ADXL355_RANGE),
					     GET_ADXL355_TRANSF_LEN(ADXL355_RANGE), &range_reg_val);

	if (!ret) {
		dev->range = range_val;
		shadow_reg_val[3] = range_reg_val;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Writes the low-pass filter settings.
 *
 * @param dev         - The device structure.
 * @param odr_lpf_val - New ODR LPF value to be written.
 *
 * @return ret        - Result of the writing procedure.
*******************************************************************************/
int adxl355_capi_set_odr_lpf(struct adxl355_capi_dev *dev,
			     enum adxl355_capi_odr_lpf odr_lpf_val)
{
	int ret;
	uint8_t filter_reg_val;
	uint8_t hpf_val = (shadow_reg_val[2] & ADXL355_HPF_FIELD_MSK) >> 4;

	if (!dev)
		return -EINVAL;

	filter_reg_val = (odr_lpf_val & ADXL355_ODR_LPF_FIELD_MSK) | (hpf_val << 4);

	ret = adxl355_capi_write_device_data(dev, ADXL355_ADDR(ADXL355_FILTER),
					     GET_ADXL355_TRANSF_LEN(ADXL355_FILTER), &filter_reg_val);

	if (!ret) {
		dev->odr_lpf = odr_lpf_val;
		shadow_reg_val[2] = filter_reg_val;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Writes the high-pass filter settings.
 *
 * @param dev             - The device structure.
 * @param hpf_corner_val  - New HPF corner frequency value to be written.
 *
 * @return ret            - Result of the writing procedure.
*******************************************************************************/
int adxl355_capi_set_hpf_corner(struct adxl355_capi_dev *dev,
				enum adxl355_capi_hpf_corner hpf_corner_val)
{
	int ret;
	uint8_t filter_reg_val;
	uint8_t odr_lpf_val = shadow_reg_val[2] & ADXL355_ODR_LPF_FIELD_MSK;

	if (!dev)
		return -EINVAL;

	filter_reg_val = odr_lpf_val | ((hpf_corner_val << 4) & ADXL355_HPF_FIELD_MSK);

	ret = adxl355_capi_write_device_data(dev, ADXL355_ADDR(ADXL355_FILTER),
					     GET_ADXL355_TRANSF_LEN(ADXL355_FILTER), &filter_reg_val);

	if (!ret) {
		dev->hpf_corner = hpf_corner_val;
		shadow_reg_val[2] = filter_reg_val;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Sets an offset value for each axis (Offset Calibration).
 *
 * @param dev      - The device structure.
 * @param x_offset - X axis offset.
 * @param y_offset - Y axis offset.
 * @param z_offset - Z axis offset.
 *
 * @return ret     - Result of the setting offset procedure.
*******************************************************************************/
int adxl355_capi_set_offset(struct adxl355_capi_dev *dev, uint16_t x_offset,
			    uint16_t y_offset, uint16_t z_offset)
{
	int ret;
	uint8_t offset_array[2];

	if (!dev)
		return -EINVAL;

	/* X offset */
	offset_array[0] = (x_offset & 0xFF00) >> 8;
	offset_array[1] = x_offset & 0x00FF;
	ret = adxl355_capi_write_device_data(dev, ADXL355_ADDR(ADXL355_OFFSET_X),
					     GET_ADXL355_TRANSF_LEN(ADXL355_OFFSET_X), offset_array);
	if (ret)
		return ret;

	/* Y offset */
	offset_array[0] = (y_offset & 0xFF00) >> 8;
	offset_array[1] = y_offset & 0x00FF;
	ret = adxl355_capi_write_device_data(dev, ADXL355_ADDR(ADXL355_OFFSET_Y),
					     GET_ADXL355_TRANSF_LEN(ADXL355_OFFSET_Y), offset_array);
	if (ret)
		return ret;

	/* Z offset */
	offset_array[0] = (z_offset & 0xFF00) >> 8;
	offset_array[1] = z_offset & 0x00FF;
	ret = adxl355_capi_write_device_data(dev, ADXL355_ADDR(ADXL355_OFFSET_Z),
					     GET_ADXL355_TRANSF_LEN(ADXL355_OFFSET_Z), offset_array);

	if (!ret) {
		dev->x_offset = x_offset;
		dev->y_offset = y_offset;
		dev->z_offset = z_offset;
	}

	return ret;
}

/***************************************************************************//**
 * @brief Converts acceleration values from array to uint32.
 *
 * @param dev       - The device structure.
 * @param raw_array - Acceleration raw array.
 *
 * @return accel_data - Acceleration value as uint32.
*******************************************************************************/
static uint32_t adxl355_capi_accel_array_conv(struct adxl355_capi_dev *dev,
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
 * @brief Converts raw acceleration values to g.
 *
 * @param dev       - The device structure.
 * @param raw_accel - Raw acceleration value.
 *
 * @return accel_g  - Acceleration value in g * 10^9.
*******************************************************************************/
static int64_t adxl355_capi_accel_conv(struct adxl355_capi_dev *dev, uint32_t raw_accel)
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
	case ID_ADXL355_CAPI:
		return ((int64_t)(accel_data * ADXL355_ACC_SCALE_FACTOR_MUL *
				  adxl355_capi_scale_mul[dev->range]));
	case ID_ADXL357_CAPI:
	case ID_ADXL359_CAPI:
		return ((int64_t)(accel_data * ADXL359_ACC_SCALE_FACTOR_MUL *
				  adxl355_capi_scale_mul[dev->range]));
	default:
		return 0;
	}
}

/***************************************************************************//**
 * @brief Converts raw temperature to millidegrees Celsius.
 *
 * @param dev      - The device structure.
 * @param raw_temp - Raw temperature value.
 *
 * @return temp    - Temperature value in millidegrees Celsius * 10^6.
*******************************************************************************/
static int64_t adxl355_capi_temp_conv(struct adxl355_capi_dev *dev, uint16_t raw_temp)
{
	switch (dev->dev_type) {
	case ID_ADXL355_CAPI:
	case ID_ADXL357_CAPI:
		return ((raw_temp * ADXL355_TEMP_OFFSET_DIV +  ADXL355_TEMP_OFFSET) *
			(int64_t)ADXL355_TEMP_SCALE_FACTOR);
	case ID_ADXL359_CAPI:
		return ((raw_temp * ADXL359_TEMP_OFFSET_DIV +  ADXL359_TEMP_OFFSET) *
			(int64_t)ADXL359_TEMP_SCALE_FACTOR);
	default:
		return 0;
	}
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
int adxl355_capi_get_raw_xyz(struct adxl355_capi_dev *dev, uint32_t *raw_x,
			     uint32_t *raw_y, uint32_t *raw_z)
{
	uint8_t array_raw_x[GET_ADXL355_TRANSF_LEN(ADXL355_XDATA)] = {0};
	uint8_t array_raw_y[GET_ADXL355_TRANSF_LEN(ADXL355_YDATA)] = {0};
	uint8_t array_raw_z[GET_ADXL355_TRANSF_LEN(ADXL355_ZDATA)] = {0};
	int ret;

	ret = adxl355_capi_read_device_data(dev, ADXL355_ADDR(ADXL355_XDATA),
				       GET_ADXL355_TRANSF_LEN(ADXL355_XDATA), array_raw_x);
	if (ret)
		return ret;
	*raw_x = adxl355_capi_accel_array_conv(dev, array_raw_x);

	ret = adxl355_capi_read_device_data(dev, ADXL355_ADDR(ADXL355_YDATA),
				       GET_ADXL355_TRANSF_LEN(ADXL355_YDATA), array_raw_y);
	if (ret)
		return ret;
	*raw_y = adxl355_capi_accel_array_conv(dev, array_raw_y);

	ret = adxl355_capi_read_device_data(dev, ADXL355_ADDR(ADXL355_ZDATA),
				       GET_ADXL355_TRANSF_LEN(ADXL355_ZDATA), array_raw_z);
	if (ret)
		return ret;
	*raw_z = adxl355_capi_accel_array_conv(dev, array_raw_z);

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
int adxl355_capi_get_xyz(struct adxl355_capi_dev *dev, struct adxl355_capi_frac_repr *x,
			 struct adxl355_capi_frac_repr *y, struct adxl355_capi_frac_repr *z)
{
	uint32_t raw_accel_x;
	uint32_t raw_accel_y;
	uint32_t raw_accel_z;
	int ret;

	ret = adxl355_capi_get_raw_xyz(dev, &raw_accel_x, &raw_accel_y, &raw_accel_z);
	if (ret)
		return ret;

	x->integer = no_os_div_s64_rem(adxl355_capi_accel_conv(dev, raw_accel_x),
				       ADXL355_ACC_SCALE_FACTOR_DIV, &(x->fractional));
	y->integer = no_os_div_s64_rem(adxl355_capi_accel_conv(dev, raw_accel_y),
				       ADXL355_ACC_SCALE_FACTOR_DIV, &(y->fractional));
	z->integer = no_os_div_s64_rem(adxl355_capi_accel_conv(dev, raw_accel_z),
				       ADXL355_ACC_SCALE_FACTOR_DIV, &(z->fractional));

	return ret;
}

/***************************************************************************//**
 * @brief Reads the raw temperature data.
 *
 * @param dev      - The device structure.
 * @param raw_temp - Raw temperature data.
 *
 * @return ret     - Result of the reading procedure.
*******************************************************************************/
int adxl355_capi_get_raw_temp(struct adxl355_capi_dev *dev, uint16_t *raw_temp)
{
	int ret;
	uint8_t temp_array[2];

	if (!dev || !raw_temp)
		return -EINVAL;

	ret = adxl355_capi_read_device_data(dev, ADXL355_ADDR(ADXL355_TEMP),
					    GET_ADXL355_TRANSF_LEN(ADXL355_TEMP), temp_array);
	if (ret)
		return ret;

	*raw_temp = ((uint16_t)temp_array[0] << 8) | temp_array[1];

	return 0;
}

/***************************************************************************//**
 * @brief Reads the raw temperature data and converts it to millidegrees Celsius.
 *
 * @param dev  - The device structure.
 * @param temp - Converted temperature data.
 *
 * @return ret - Result of the reading procedure.
*******************************************************************************/
int adxl355_capi_get_temp(struct adxl355_capi_dev *dev, struct adxl355_capi_frac_repr *temp)
{
	uint16_t raw_temp;
	int ret;
	int32_t divisor;

	ret = adxl355_capi_get_raw_temp(dev, &raw_temp);
	if (ret)
		return ret;

	switch (dev->dev_type) {
	case ID_ADXL355_CAPI:
	case ID_ADXL357_CAPI:
		divisor = ADXL355_TEMP_OFFSET_DIV * ADXL355_TEMP_SCALE_FACTOR_DIV;
		break;
	case ID_ADXL359_CAPI:
		divisor = ADXL359_TEMP_OFFSET_DIV * ADXL359_TEMP_SCALE_FACTOR_DIV;
		break;
	default:
		return -EINVAL;
	}

	temp->integer = no_os_div_s64_rem(adxl355_capi_temp_conv(dev, raw_temp),
					  divisor,
					  &(temp->fractional));
	return 0;
}

/***************************************************************************//**
 * @brief Reads the status register value.
 *
 * @param dev          - The device structure.
 * @param status_flags - Status register flags.
 *
 * @return ret         - Result of the reading procedure.
*******************************************************************************/
int adxl355_capi_get_sts_reg(struct adxl355_capi_dev *dev,
			     union adxl355_capi_sts_reg_flags *status_flags)
{
	if (!dev || !status_flags)
		return -EINVAL;

	return adxl355_capi_read_device_data(dev, ADXL355_ADDR(ADXL355_STATUS),
					     GET_ADXL355_TRANSF_LEN(ADXL355_STATUS), &status_flags->value);
}

/***************************************************************************//**
 * @brief Reads the number of FIFO entries register value.
 *
 * @param dev       - The device structure.
 * @param reg_value - FIFO entries register value.
 *
 * @return ret      - Result of the reading procedure.
*******************************************************************************/
int adxl355_capi_get_nb_of_fifo_entries(struct adxl355_capi_dev *dev, uint8_t *reg_value)
{
	if (!dev || !reg_value)
		return -EINVAL;

	return adxl355_capi_read_device_data(dev, ADXL355_ADDR(ADXL355_FIFO_ENTRIES),
					     GET_ADXL355_TRANSF_LEN(ADXL355_FIFO_ENTRIES), reg_value);
}

/***************************************************************************//**
 * @brief Sets the number of FIFO samples register value.
 *
 * @param dev       - The device structure.
 * @param reg_value - FIFO samples register value.
 *
 * @return ret      - Result of the writing procedure.
*******************************************************************************/
int adxl355_capi_set_fifo_samples(struct adxl355_capi_dev *dev, uint8_t reg_value)
{
	int ret;

	if (!dev)
		return -EINVAL;

	if (reg_value > ADXL355_MAX_FIFO_SAMPLES_VAL)
		return -EINVAL;

	ret = adxl355_capi_write_device_data(dev, ADXL355_ADDR(ADXL355_FIFO_SAMPLES),
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
int adxl355_capi_get_raw_fifo_data(struct adxl355_capi_dev *dev, uint8_t *fifo_entries,
			      uint32_t *raw_x, uint32_t *raw_y, uint32_t *raw_z)
{
	int ret;

	ret = adxl355_capi_get_nb_of_fifo_entries(dev, fifo_entries);
	if (ret)
		return ret;

	*fifo_entries = (*fifo_entries / 3) * 3;

	if (*fifo_entries > 0) {

		ret = adxl355_capi_read_device_data(dev, ADXL355_ADDR(ADXL355_FIFO_DATA),
					       *fifo_entries * 3, dev->comm_buff);
		if (ret)
			return ret;

		for (uint16_t idx = 0; idx < *fifo_entries * 3; idx = idx + 9) {
			if (((dev->comm_buff[idx + 2] & 1) == 1)
			    && ((dev->comm_buff[idx + 2] & 2) == 0)) {
				// This is x-axis
				// Process data
				raw_x[idx / 9] = adxl355_capi_accel_array_conv(dev, &dev->comm_buff[idx]);
				raw_y[idx / 9] = adxl355_capi_accel_array_conv(dev, &dev->comm_buff[idx + 3]);
				raw_z[idx / 9] = adxl355_capi_accel_array_conv(dev, &dev->comm_buff[idx + 6]);
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
int adxl355_capi_get_fifo_data(struct adxl355_capi_dev *dev, uint8_t *fifo_entries,
			  struct adxl355_capi_frac_repr *x,
			  struct adxl355_capi_frac_repr *y,
			  struct adxl355_capi_frac_repr *z)
{
	int ret;
	uint32_t raw_x[32];
	uint32_t raw_y[32];
	uint32_t raw_z[32];

	ret = adxl355_capi_get_raw_fifo_data(dev, fifo_entries, raw_x, raw_y, raw_z);
	if (ret)
		return ret;

	if (*fifo_entries > 0) {
		for (uint8_t idx = 0; idx < *fifo_entries / 3; idx++) {
			x[idx].integer = no_os_div_s64_rem(adxl355_capi_accel_conv(dev, raw_x[idx]),
							   ADXL355_ACC_SCALE_FACTOR_DIV, &(x[idx].fractional));
			y[idx].integer = no_os_div_s64_rem(adxl355_capi_accel_conv(dev, raw_y[idx]),
							   ADXL355_ACC_SCALE_FACTOR_DIV, &(y[idx].fractional));
			z[idx].integer = no_os_div_s64_rem(adxl355_capi_accel_conv(dev, raw_z[idx]),
							   ADXL355_ACC_SCALE_FACTOR_DIV, &(z[idx].fractional));
		}
	}

	return ret;
}

/***************************************************************************//**
 * @brief Configures the activity enable register.
 *
 * @param dev        - The device structure.
 * @param act_config - Activity enable configuration.
 *
 * @return ret       - Result of the writing procedure.
*******************************************************************************/
int adxl355_capi_conf_act_en(struct adxl355_capi_dev *dev,
			     union adxl355_capi_act_en_flags act_config)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adxl355_capi_write_device_data(dev, ADXL355_ADDR(ADXL355_ACT_EN),
					     GET_ADXL355_TRANSF_LEN(ADXL355_ACT_EN), &act_config.value);

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
 * @return ret    - Result of the writing procedure.
*******************************************************************************/
int adxl355_capi_conf_act_thr(struct adxl355_capi_dev *dev, uint16_t act_thr)
{
	int ret;
	uint8_t act_thr_array[2];

	if (!dev)
		return -EINVAL;

	act_thr_array[0] = (act_thr & 0xFF00) >> 8;
	act_thr_array[1] = act_thr & 0x00FF;

	ret = adxl355_capi_write_device_data(dev, ADXL355_ADDR(ADXL355_ACT_THRESH),
					     GET_ADXL355_TRANSF_LEN(ADXL355_ACT_THRESH), act_thr_array);

	if (!ret)
		dev->act_thr = act_thr;

	return ret;
}

/***************************************************************************//**
 * @brief Writes the activity count register value.
 *
 * @param dev     - The device structure.
 * @param act_cnt - Activity count register value.
 *
 * @return ret    - Result of the writing procedure.
*******************************************************************************/
int adxl355_capi_set_act_cnt_reg(struct adxl355_capi_dev *dev, uint8_t act_cnt)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = adxl355_capi_write_device_data(dev, ADXL355_ADDR(ADXL355_ACT_CNT),
					     GET_ADXL355_TRANSF_LEN(ADXL355_ACT_CNT), &act_cnt);

	if (!ret)
		dev->act_cnt = act_cnt;

	return ret;
}

/***************************************************************************//**
 * @brief Configures the interrupt map for INT1 and INT2 pins.
 *
 * @param dev      - The device structure.
 * @param int_conf - Interrupt pins configuration.
 *
 * @return ret     - Result of the writing procedure.
*******************************************************************************/
int adxl355_capi_config_int_pins(struct adxl355_capi_dev *dev,
				 union adxl355_capi_int_mask int_conf)
{
	if (!dev)
		return -EINVAL;

	return adxl355_capi_write_device_data(dev, ADXL355_ADDR(ADXL355_INT_MAP),
					      GET_ADXL355_TRANSF_LEN(ADXL355_INT_MAP), &int_conf.value);
}

/***************************************************************************//**
 * @brief Configures the interrupt polarity.
 *
 * @param dev     - The device structure.
 * @param int_pol - Interrupt polarity to be set.
 *
 * @return ret    - Result of the writing procedure.
*******************************************************************************/
int adxl355_capi_set_int_pol(struct adxl355_capi_dev *dev, enum adxl355_capi_int_pol int_pol)
{
	int ret;
	uint8_t range_reg_val;
	uint8_t range_field_val = shadow_reg_val[3] & ADXL355_RANGE_FIELD_MSK;
	uint8_t hpf_field_val = (shadow_reg_val[3] & ADXL355_HPF_FIELD_MSK) >> 4;

	if (!dev)
		return -EINVAL;

	range_reg_val = range_field_val | (hpf_field_val << 4) | ((int_pol << 6) & ADXL355_INT_POL_FIELD_MSK);

	ret = adxl355_capi_write_device_data(dev, ADXL355_ADDR(ADXL355_RANGE),
					     GET_ADXL355_TRANSF_LEN(ADXL355_RANGE), &range_reg_val);

	if (!ret)
		shadow_reg_val[3] = range_reg_val;

	return ret;
}