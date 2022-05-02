/*******************************************************************************
 *   @file   adxl314.c
 *   @brief  Implementation of ADXL314 Driver.
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
 * OF THIS SOFTWARE, EVEN IFadxl314_init ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include "adxl314.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include "no_os_delay.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
static int64_t adxl314_accel_conv(uint16_t raw_accel);

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
int adxl314_read_device_data(struct adxl314_dev *dev, uint8_t base_address,
			     uint16_t size, uint8_t *read_data)
{
	int ret;

	if (dev->comm_type == ADXL314_SPI_COMM) {
		dev->comm_buff[0] = ADXL314_SPI_READ | base_address;
		if (size > 1)
			dev->comm_buff[0] |= ADXL314_MULTIBIT;
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
int adxl314_write_device_data(struct adxl314_dev *dev, uint8_t base_address,
			      uint16_t size, uint8_t *write_data)
{
	int ret;

	for (uint16_t idx = 0; idx < size; idx++)
		dev->comm_buff[1+idx] = write_data[idx];

	if (dev->comm_type == ADXL314_SPI_COMM) {
		dev->comm_buff[0] = ADXL314_SPI_WRITE | base_address;
		if (size > 1)
			dev->comm_buff[0] |= ADXL314_MULTIBIT;
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
int adxl314_reg_write_msk(struct adxl314_dev *dev,
			  uint8_t reg_addr,
			  uint8_t data,
			  uint8_t mask)
{
	int ret;
	uint8_t reg_data = 0x00;

	ret = adxl314_read_device_data(dev, reg_addr, 1, &reg_data);
	if (ret)
		return ret;

	reg_data &= ~mask;
	reg_data |= data;

	return adxl314_write_device_data(dev, reg_addr, 1, &reg_data);
}

/*******************************************************************************
 * @brief Initialize the communication peripheral and check if the ADXL314
 *        part is present.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       		   parameters.
 *
 * @return ret       - Result of the initialization - 0 in case of success,
 *                     negative error code otherwise.
*******************************************************************************/
int adxl314_init(struct adxl314_dev **device,
		 struct adxl314_init_param init_param)
{
	struct adxl314_dev *dev;
	int ret;
	uint8_t reg_value = 0;

	dev = (struct adxl314_dev *)calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->comm_type = init_param.comm_type;

	if (dev->comm_type == ADXL314_SPI_COMM)
		ret = no_os_spi_init(&dev->com_desc.spi_desc, &(init_param.comm_init.spi_init));
	else
		ret = no_os_i2c_init(&dev->com_desc.i2c_desc, &init_param.comm_init.i2c_init);

	if (ret)
		goto error_dev;

	/* Check for device ID. */
	ret = adxl314_read_device_data(dev, ADXL314_DEVID_AD, 1, &reg_value);
	if (ret || (reg_value != ADXL314_DEVID))
		goto error_com;

	*device = dev;

	/* Set op. mode to Standby. */
	ret = adxl314_set_op_mode(dev, ADXL314_STDBY);
	if (ret)
		goto error_com;

	/* Get Low Power mode status. */
	ret = adxl314_read_device_data(dev, ADXL314_REG_BW_RATE, 1, &reg_value);
	if (ret)
		goto error_com;

	if (reg_value & ADXL314_LOW_POWER_OP)
		dev->lp_mode = ADXL314_LP_MODE_ON;
	else
		dev->lp_mode = ADXL314_LP_MODE_NONE;

	/* Get ODR. */
	ret = adxl314_read_device_data(dev, ADXL314_REG_BW_RATE, 1, &reg_value);
	if (ret)
		goto error_com;
	dev->odr = reg_value & ADXL314_RATE_MSK;

	/* Get offsets. */
	ret = adxl314_read_device_data(dev, ADXL314_REG_OFS_AXIS(ADXL314_X_AXIS), 1,
				       &reg_value);
	if (ret)
		goto error_com;
	dev->x_offset = reg_value;
	ret = adxl314_read_device_data(dev, ADXL314_REG_OFS_AXIS(ADXL314_Y_AXIS), 1,
				       &reg_value);
	if (ret)
		goto error_com;
	dev->y_offset = reg_value;
	ret = adxl314_read_device_data(dev, ADXL314_REG_OFS_AXIS(ADXL314_Z_AXIS), 1,
				       &reg_value);
	if (ret)
		goto error_com;
	dev->z_offset = reg_value;

	/* Get FIFO mode setting. */
	ret = adxl314_read_device_data(dev, ADXL314_REG_FIFO_CTL, 1, &reg_value);
	if (ret)
		goto error_com;
	dev->fifo_mode = (reg_value & ADXL314_REG_FIFO_CTL_MODE_MSK) >> 6;

	/* Get FIFO samples setting. */
	ret = adxl314_read_device_data(dev, ADXL314_REG_FIFO_CTL, 1, &reg_value);
	if (ret)
		goto error_com;
	dev->fifo_samples = (reg_value & ADXL314_REG_FIFO_CTL_SAMPLES_MSK);

	/* Get dev. threshold for detecting activity. */
	ret = adxl314_read_device_data(dev, ADXL314_REG_THRESH_ACT, 1, &reg_value);
	if (ret)
		goto error_com;
	dev->act_thr = reg_value;

	/* Get dev. threshold for detecting inactivity. */
	ret = adxl314_read_device_data(dev, ADXL314_REG_THRESH_INACT, 1, &reg_value);
	if (ret)
		goto error_com;
	dev->inact_thr = reg_value;

	/* Get dev. time threshold for detecting inactivity. */
	ret = adxl314_read_device_data(dev, ADXL314_REG_TIME_INACT, 1, &reg_value);
	if (ret)
		goto error_com;
	dev->time_inact = reg_value;

	/* Get dev. ACT_INACT_CTL register values. */
	ret = adxl314_read_device_data(dev, ADXL314_REG_ACT_INACT_CTL, 1, &reg_value);
	if (ret)
		goto error_com;
	dev->act_inact_ctl.value = reg_value;

	/* Put device in Measure mode. */
	ret = adxl314_set_op_mode(dev, ADXL314_MEAS);
	if (ret)
		goto error_com;

	return ret;
error_com:
	if (dev->comm_type == ADXL314_SPI_COMM)
		no_os_spi_remove(dev->com_desc.spi_desc);
	else
		no_os_i2c_remove(dev->com_desc.i2c_desc);
	free(dev);
	return -EPIPE;
error_dev:
	free(dev);
	return -ENODEV;
}

/*******************************************************************************
 * @brief Free the resources allocated by adxl314_init().
 *
 * @param dev - The device structure.
 *
 * @return ret - Result of the remove action (0 - success, negative val. - failure).
*******************************************************************************/
int adxl314_remove(struct adxl314_dev *dev)
{
	int ret;

	if (dev->comm_type == ADXL314_SPI_COMM)
		ret = no_os_spi_remove(dev->com_desc.spi_desc);
	else
		ret = no_os_i2c_remove(dev->com_desc.i2c_desc);

	if (!ret)
		free(dev);

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
int adxl314_set_op_mode(struct adxl314_dev *dev, enum adxl314_op_mode op_mode)
{
	int ret;
	uint8_t mask = ADXL314_POWER_CTL_MEASURE;
	uint8_t data = ADXL314_POWER_CTL_MEASURE;

	/* Set operation mode. */
	switch(op_mode) {
	case ADXL314_STDBY:
		data = DISABLE_E;
		break;
	case ADXL314_MEAS:
		break;
	default:
		pr_err("ADXL314 operation mode not supported!\n");
		return -ENOSYS;
	}

	ret = adxl314_reg_write_msk(dev, ADXL314_REG_POWER_CTL, data, mask);

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
int adxl314_get_op_mode(struct adxl314_dev *dev,
			enum adxl314_op_mode *op_mode)
{
	int ret;
	uint8_t reg_value = 0x00;

	ret = adxl314_read_device_data(dev, ADXL314_REG_POWER_CTL, 1, &reg_value);
	if (ret)
		return ret;

	if (reg_value & ADXL314_POWER_CTL_MEASURE)
		*op_mode = ADXL314_MEAS;
	else
		*op_mode = ADXL314_STDBY;

	return ret;
}

/*******************************************************************************
 * @brief Set offset for each axis. Value is 8-bit two's complement.
 *        Scale factor of 1.56 g/LSB.
 *
 * @param dev      - The device structure.
 * @param offset   - Offset.
 * @param axis     - Axis to apply offset.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl314_set_offset(struct adxl314_dev *dev, uint8_t offset,
		       enum adxl314_axis axis)
{
	int ret;

	if (axis > ADXL314_Z_AXIS)
		return -EINVAL;

	/* Write data to register for setting odr. */
	ret = adxl314_write_device_data(dev, ADXL314_REG_OFS_AXIS(axis), 1, &offset);
	if (ret)
		return ret;

	switch (axis) {
	case ADXL314_X_AXIS:
		dev->x_offset = offset;
		break;
	case ADXL314_Y_AXIS:
		dev->y_offset = offset;
		break;
	case ADXL314_Z_AXIS:
		dev->z_offset = offset;
		break;
	default:
		return -EINVAL;
	}

	return ret;
}

/*******************************************************************************
 * @brief Read the 3-axis raw data from the accelerometer.
 *
 * @param dev - The device structure.
 * @param x   - X-axis data (as two's complement).
 * @param y   - Y-axis data (as two's complement).
 * @param z   - Z-axis data (as two's complement).
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl314_get_raw_xyz(struct adxl314_dev *dev,
			int16_t *x,
			int16_t *y,
			int16_t *z)
{
	int ret;
	uint8_t xyz_values[6] = {0};

	ret = adxl314_read_device_data(dev,
				       ADXL314_REG_DATA_AXIS(0),
				       ADXL314_REGS_PER_ENTRY,
				       xyz_values);
	if (ret)
		return ret;

	/* Result is 13 bits long with sign extended. */
	*x = ((int16_t)xyz_values[1] << 8) + (xyz_values[0]);
	*y = ((int16_t)xyz_values[3] << 8) + (xyz_values[2]);
	*z = ((int16_t)xyz_values[5] << 8) + (xyz_values[4]);

	return ret;
}

/*******************************************************************************
 * @brief Read the raw output data of each axis and convert it to g.
 *
 * @param dev - The device structure.
 * @param x   - X-axis data.
 * @param y   - Y-axis data.
 * @param z   - Z-axis data.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl314_get_xyz(struct adxl314_dev *dev,
		    struct adxl314_frac_repr *x,
		    struct adxl314_frac_repr *y,
		    struct adxl314_frac_repr *z)
{
	int ret;
	int16_t raw_accel_x = 0;
	int16_t raw_accel_y = 0;
	int16_t raw_accel_z = 0;

	ret = adxl314_get_raw_xyz(dev, &raw_accel_x, &raw_accel_y, &raw_accel_z);
	if (ret)
		return ret;

	x->integer = no_os_div_s64_rem(adxl314_accel_conv(raw_accel_x),
				       ADXL314_ACC_SCALE_FACTOR_DIV, &(x->fractional));
	y->integer = no_os_div_s64_rem(adxl314_accel_conv(raw_accel_y),
				       ADXL314_ACC_SCALE_FACTOR_DIV, &(y->fractional));
	z->integer = no_os_div_s64_rem(adxl314_accel_conv(raw_accel_z),
				       ADXL314_ACC_SCALE_FACTOR_DIV, &(z->fractional));

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
int adxl314_get_nb_of_fifo_entries(struct adxl314_dev *dev,
				   uint8_t *entries)
{
	int ret;
	uint8_t reg_val = 0x00;

	ret = adxl314_read_device_data(dev, ADXL314_REG_FIFO_STATUS, 1, &reg_val);
	if (ret)
		return ret;

	*entries = reg_val & ADXL314_REG_FIFO_STS_ENTRIES_MSK;

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
int adxl314_set_fifo_samples(struct adxl314_dev *dev,
			     uint8_t samples)
{
	int ret;

	if (samples > ADXL314_MAX_FIFO_SAMPLES_VAL)
		return -EINVAL;

	// write no of samples to ADXL314_REG_FIFO_SAMPLES
	ret = adxl314_reg_write_msk(dev,
				    ADXL314_REG_FIFO_CTL,
				    no_os_field_prep(ADXL314_REG_FIFO_CTL_SAMPLES_MSK, samples),
				    ADXL314_REG_FIFO_CTL_SAMPLES_MSK);
	if (ret)
		return ret;

	dev->fifo_samples = samples;

	return ret;
}

/*******************************************************************************
 * @brief Set FIFO mode.
 *
 * @param dev  - The device structure.
 * @param mode - FIFO mode.
 * 			   Accepted values: ADXL314_FIFO_DISABLED,
 *								ADXL314_OLDEST_SAVED,
 *								ADXL314_STREAM_MODE,
 *								ADXL314_TRIGGERED_MODE
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl314_set_fifo_mode(struct adxl314_dev *dev,
			  enum adxl314_fifo_mode mode)
{
	int ret;

	if (mode > ADXL314_TRIGGERED_MODE)
		return -EINVAL;

	ret = adxl314_reg_write_msk(dev,
				    ADXL314_REG_FIFO_CTL,
				    no_os_field_prep(ADXL314_REG_FIFO_CTL_MODE_MSK, mode),
				    ADXL314_REG_FIFO_CTL_MODE_MSK);
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
int adxl314_get_raw_fifo_data(struct adxl314_dev *dev, uint8_t *entries,
			      int16_t *raw_x, int16_t *raw_y, int16_t *raw_z)
{
	int ret;
	uint8_t buff_idx = 0;
	uint8_t xyz_values[ADXL314_MAX_FIFO_ENTRIES * ADXL314_REGS_PER_ENTRY] = {0};

	/* Get the number of FIFO entries */
	ret = adxl314_get_nb_of_fifo_entries(dev, entries);
	if (ret)
		return ret;

	if ((*entries) > 0) {
		for (uint8_t idx = 0; idx < (*entries); idx++) {
			ret = adxl314_read_device_data(dev,
						       ADXL314_REG_DATA_AXIS(0),
						       ADXL314_REGS_PER_ENTRY,
						       &xyz_values[idx * ADXL314_REGS_PER_ENTRY]);
			if (ret)
				return ret;

			//wait 5us
			no_os_udelay(5);
		}

		for (uint8_t idx = 0; idx < (*entries); idx++) {
			buff_idx = idx * ADXL314_REGS_PER_ENTRY;
			raw_x[idx] = ((int16_t)xyz_values[buff_idx+1] << 8) + (xyz_values[buff_idx]);
			raw_y[idx] = ((int16_t)xyz_values[buff_idx+3] << 8) + (xyz_values[buff_idx+2]);
			raw_z[idx] = ((int16_t)xyz_values[buff_idx+5] << 8) + (xyz_values[buff_idx+4]);
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
int adxl314_get_fifo_data(struct adxl314_dev *dev,
			  uint8_t *entries,
			  struct adxl314_frac_repr *x,
			  struct adxl314_frac_repr *y,
			  struct adxl314_frac_repr *z)
{
	int ret;
	int16_t raw_x[ADXL314_MAX_FIFO_ENTRIES] = {0};
	int16_t raw_y[ADXL314_MAX_FIFO_ENTRIES] = {0};
	int16_t raw_z[ADXL314_MAX_FIFO_ENTRIES] = {0};

	ret = adxl314_get_raw_fifo_data(dev, entries, raw_x, raw_y, raw_z);
	if (ret)
		return ret;

	if ((*entries) > 0) {
		for (uint8_t idx = 0; idx < (*entries); idx++) {
			x[idx].integer = no_os_div_s64_rem(adxl314_accel_conv(raw_x[idx]),
							   ADXL314_ACC_SCALE_FACTOR_DIV, &(x[idx].fractional));
			y[idx].integer = no_os_div_s64_rem(adxl314_accel_conv(raw_y[idx]),
							   ADXL314_ACC_SCALE_FACTOR_DIV, &(y[idx].fractional));
			z[idx].integer = no_os_div_s64_rem(adxl314_accel_conv(raw_z[idx]),
							   ADXL314_ACC_SCALE_FACTOR_DIV, &(z[idx].fractional));
		}
	}

	return ret;
}

/*******************************************************************************
 * @brief Configure the activity threshold register.
 *        The scale factor is 780 mg/LSB.
 *
 * @param dev     - The device structure.
 * @param act_thr - Activity threshold value.
 *
 * @return ret    - Result of the configuration action.
 *                  0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl314_conf_act_thr(struct adxl314_dev *dev, uint8_t act_thr)
{
	int ret;
	uint8_t reg_val = act_thr;

	ret = adxl314_write_device_data(dev, ADXL314_REG_THRESH_ACT, 1, &reg_val);

	if (!ret)
		dev->act_thr = act_thr;

	return ret;
}

/*******************************************************************************
 * @brief Configure the inactivity threshold register.
 *        The scale factor is 780 mg/LSB.
 *
 * @param dev       - The device structure.
 * @param inact_thr - Inactivity threshold value.
 *
 * @return ret      - Result of the configuration action.
 *                    0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl314_conf_inact_thr(struct adxl314_dev *dev, uint8_t inact_thr)
{
	int ret;
	uint8_t reg_val = inact_thr;

	ret = adxl314_write_device_data(dev, ADXL314_REG_THRESH_INACT, 1, &reg_val);

	if (!ret)
		dev->inact_thr = inact_thr;

	return ret;
}

/*******************************************************************************
 * @brief Configure the time inactivity threshold register.
 *        The scale factor is 1 s/LSB.
 *
 * @param dev        - The device structure.
 * @param time_inact - Time inactivity value.
 *
 * @return ret    - Result of the configuration action.
 *                  0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl314_conf_time_inact(struct adxl314_dev *dev, uint8_t time_inact)
{
	int ret;
	uint8_t reg_val = time_inact;

	ret = adxl314_write_device_data(dev, ADXL314_REG_TIME_INACT, 1, &reg_val);

	if (!ret)
		dev->time_inact = time_inact;

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
int adxl314_conf_act_inact_ctl(struct adxl314_dev *dev,
			       union adxl314_act_inact_ctl_flags config)
{
	int ret;
	uint8_t reg_val = config.value;

	ret = adxl314_write_device_data(dev, ADXL314_REG_ACT_INACT_CTL, 1, &reg_val);

	if (!ret)
		dev->act_inact_ctl = config;

	return ret;
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
int adxl314_set_odr(struct adxl314_dev *dev, enum adxl314_odr odr)
{
	int ret;
	uint8_t reg_value = 0x00;
	uint8_t data = 0;

	if (odr > ADXL314_ODR_3200HZ)
		return -EINVAL;

	ret = adxl314_read_device_data(dev, ADXL314_REG_BW_RATE, 1, &reg_value);
	if (ret)
		return ret;

	/* Clear data rate value. */
	reg_value &= ~ADXL314_RATE_MSK;

	/* Set new register value. */
	data = (odr + ADXL314_ODR_OFFSET_VAL) & ADXL314_RATE_MSK;
	reg_value |= data;

	/* Write data to register for setting odr. */
	ret = adxl314_write_device_data(dev, ADXL314_REG_BW_RATE, 1, &reg_value);
	if (ret)
		return ret;

	dev->odr = odr;

	return ret;
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
int adxl314_set_low_power_mode(struct adxl314_dev *dev, enum bit_action enable)
{
	int ret;
	uint8_t mask = ADXL314_LOW_POWER_OP;
	uint8_t data = ADXL314_LOW_POWER_OP;

	/* Set STDBY operation mode. */
	ret = adxl314_set_op_mode(dev, ADXL314_STDBY);
	if (ret)
		return ret;

	if (enable == ENABLE_E) {
		/* If data rate greater than 400 Hz set to this val. */
		if (dev->odr > ADXL314_ODR_400HZ)
			ret = adxl314_set_odr(dev, ADXL314_ODR_400HZ);
		/* If data rate smaller than 12.5 Hz set to this val. */
		else if (dev->odr < ADXL314_ODR_12_5HZ)
			ret = adxl314_set_odr(dev, ADXL314_ODR_12_5HZ);
		if (ret)
			return ret;

		dev->lp_mode = ADXL314_LP_MODE_ON;
	} else {
		data = DISABLE_E;
		dev->lp_mode = ADXL314_LP_MODE_NONE;
	}

	/* Set/Clear the AUTOSLEEP bit. */
	ret = adxl314_reg_write_msk(dev, ADXL314_REG_BW_RATE, data, mask);

	if (ret)
		return ret;

	/* Set MEAS operation mode. */
	return adxl314_set_op_mode(dev, ADXL314_MEAS);
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
int adxl314_set_autosleep_mode(struct adxl314_dev *dev, enum bit_action enable,
			       uint8_t inact_thr, uint8_t time_inact)
{
	int ret;
	uint8_t mask = ADXL314_POWER_CTL_LINK | ADXL314_POWER_CTL_AUTO_SLEEP;
	uint8_t data = ADXL314_POWER_CTL_LINK | ADXL314_POWER_CTL_AUTO_SLEEP;

	/* Set STDBY operation mode. */
	ret = adxl314_set_op_mode(dev, ADXL314_STDBY);
	if (ret)
		return ret;

	if (enable == ENABLE_E) {
		ret = adxl314_conf_inact_thr(dev, inact_thr);
		if (ret)
			return ret;

		ret = adxl314_conf_time_inact(dev, time_inact);
		if (ret)
			return ret;

		dev->lp_mode = ADXL314_LP_MODE_AUTOSLEEP;
	} else {
		ret = adxl314_conf_inact_thr(dev, 0);
		if (ret)
			return ret;

		ret = adxl314_conf_time_inact(dev, 0);
		if (ret)
			return ret;

		data = DISABLE_E;

		dev->lp_mode = ADXL314_LP_MODE_NONE;
	}

	/* Set the LINK and AUTOSLEEP bits. */
	ret = adxl314_reg_write_msk(dev, ADXL314_REG_POWER_CTL, data, mask);
	if (ret)
		return ret;

	/* Set MEAS operation mode. */
	return adxl314_set_op_mode(dev, ADXL314_MEAS);
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
int adxl314_conf_int_enable(struct adxl314_dev *dev,
			    union adxl314_int_en_reg_flags en_ctl)
{
	uint8_t reg_val = en_ctl.value;

	return adxl314_write_device_data(dev, ADXL314_REG_INT_EN, 1, &reg_val);
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
int adxl314_conf_int_map(struct adxl314_dev *dev,
			 union adxl314_int_map_reg_flags int_map)
{
	uint8_t reg_val = int_map.value;

	return adxl314_write_device_data(dev, ADXL314_REG_INT_MAP, 1, &reg_val);
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
int adxl314_get_int_source_reg(struct adxl314_dev *dev,
			       union adxl314_int_src_reg_flags *int_status_flags)
{
	int ret;
	uint8_t reg_value = 0x00;

	ret = adxl314_read_device_data(dev, ADXL314_REG_INT_SRC, 1, &reg_value);

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
int adxl314_set_int_pol(struct adxl314_dev *dev,
			enum adxl314_int_pol int_pol)
{
	uint8_t mask = ADXL314_REG_DATA_FORMAT_INT_INV;
	uint8_t data = ADXL314_REG_DATA_FORMAT_INT_INV;

	if (int_pol == ADXL314_INT_ACTIVE_HIGH)
		data = DISABLE_E;

	return adxl314_reg_write_msk(dev, ADXL314_REG_DATA_FORMAT, data, mask);
}

/*******************************************************************************
 * @brief Performs self test.
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int adxl314_self_test(struct adxl314_dev *dev)
{
	int ret;
	int16_t x[ADXL314_MAX_FIFO_ENTRIES] = {0};
	int16_t y[ADXL314_MAX_FIFO_ENTRIES] = {0};
	int16_t z[ADXL314_MAX_FIFO_ENTRIES] = {0};
	int16_t zst_off = 0;
	int16_t zst_on = 0;
	int16_t zst = 0;
	uint8_t fifo_entries = 0;
	uint8_t mask = ADXL314_REG_DATA_FORMAT_SELF_TEST;
	uint8_t data = ADXL314_REG_DATA_FORMAT_SELF_TEST;

	/* 1. Set the device ODR to 100 Hz. */
	/* Set dev. in standby mode. */
	ret = adxl314_set_op_mode(dev, ADXL314_STDBY);
	if (ret)
		return ret;

	ret = adxl314_set_odr(dev, ADXL314_ODR_100HZ);
	if (ret)
		return ret;

	/* 2. Clear the LOW_POWER bit (Bit D4) in the BW_RATE register. */
	ret = adxl314_set_low_power_mode(dev, DISABLE_E);
	if (ret)
		return ret;

	/* 3. Read samples from FIFO. (Recommended starting point is 0.1 sec worth
	 * of data for data rates of 100 Hz or greater).  */

	/* Set FIFO mode to FIFO */
	ret = adxl314_set_fifo_mode(dev, ADXL314_FIFO_MODE);
	if (ret)
		return ret;

	/* Set dev. in measure mode. */
	ret = adxl314_set_op_mode(dev, ADXL314_MEAS);
	if (ret)
		return ret;

	/* Output settling time, min 4 samples. 10ms per sample at 100Hz. */
	no_os_mdelay(50);

	/* 4. Read and store the 10 samples in the FIFO. */
	ret = adxl314_get_raw_fifo_data(dev, &fifo_entries, x, y, z);
	if (ret)
		return ret;

	/* Compute average (10 samples). */
	for (uint8_t idx = 0; idx < ADXL314_ST_SAMPLES; idx++) {
		zst_off += z[idx];
	}
	zst_off /= ADXL314_ST_SAMPLES;

	/* 5. Enable self-test. */
	/* Set dev in standby mode. */
	ret = adxl314_set_op_mode(dev, ADXL314_STDBY);
	if (ret)
		return ret;

	/* Set self-test bit. */
	ret = adxl314_reg_write_msk(dev, ADXL314_REG_DATA_FORMAT, data, mask);
	if (ret)
		return ret;

	/* Set dev. in measure mode. */
	ret = adxl314_set_op_mode(dev, ADXL314_MEAS);
	if (ret)
		return ret;

	/* Output settling time, min 4 samples. 10ms per sample at 100Hz. */
	no_os_mdelay(50);

	/* 6. Read and store samples in the FIFO. */
	ret = adxl314_get_raw_fifo_data(dev, &fifo_entries, x, y, z);
	if (ret)
		return ret;

	/* 7. Compute average (10 samples). */
	for (uint8_t idx = 0; idx < ADXL314_ST_SAMPLES; idx++) {
		zst_on += z[idx];
	}
	zst_on /= ADXL314_ST_SAMPLES;

	/* 8. Disable self-test. */
	/* Set dev. in standby mode. */
	ret = adxl314_set_op_mode(dev, ADXL314_STDBY);
	if (ret)
		return ret;

	/* Reset self-test bit. */
	data = DISABLE_E;
	ret = adxl314_reg_write_msk(dev, ADXL314_REG_DATA_FORMAT, data, mask);
	if (ret)
		return ret;

	/* Set dev. in measure mode. */
	ret = adxl314_set_op_mode(dev, ADXL314_MEAS);
	if (ret)
		return ret;

	zst = zst_on - zst_off;

	if ((zst * ADXL314_ST_MULT) < ADXL314_ST_TYP_DEVIATION)
		pr_info("Self-test passed.\n");
	else
		pr_info("Self-test failed.\n");

	return ret;
}

/*******************************************************************************
 * @brief Convert raw acceleration value to g value.
 *
 * @param raw_accel - Raw acceleration value.
 *
 * @return ret      - Converted data.
*******************************************************************************/
static int64_t adxl314_accel_conv(uint16_t raw_accel)
{
	int64_t accel_data;

	/* Convert from 16-bit unsigned to 16-bit signed number. */
	accel_data = (int16_t)raw_accel;

	/* Apply scale factor based on the selected range. */
	return (accel_data * ADXL314_ACC_SCALE_FACTOR_MUL);
}
