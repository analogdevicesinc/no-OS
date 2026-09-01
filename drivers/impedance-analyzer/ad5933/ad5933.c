/***************************************************************************//**
 *   @file   AD5933.c
 *   @brief  Implementation of AD5933 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include "no_os_alloc.h"
#include "no_os_util.h"
#include "no_os_delay.h"
#include "ad5933.h"

#define DIV_EPSILON 1e-12

const int32_t pow_2_27 = 134217728ul;      // 2 to the power of 27

static int ad5933_wait_powerup(struct ad5933_dev *dev);

/***************************************************************************//**
 * @brief Initializes the communication peripheral and the initial Values for
 *        AD5933 Board.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return ret - The result of the initialization procedure.
 *               Example: -1 - I2C peripheral was not initialized or the
 *                             device is not present.
 *                         0 - I2C peripheral was initialized and the
 *                             device is present.
*******************************************************************************/
int ad5933_init(struct ad5933_dev **device,
		struct ad5933_init_param *init_param)
{
	struct ad5933_dev *dev;
	int ret;

	if (!device || !init_param)
		return -EINVAL;

	dev = (struct ad5933_dev *)no_os_calloc(1, sizeof(*dev));

	if (!dev)
		return -ENOMEM;

	ret = no_os_i2c_init(&dev->i2c_desc, &init_param->i2c_init);

	if (ret)
		goto error_dev;

	ret = ad5933_wait_powerup(dev);

	if (ret)
		goto error_i2c;

	dev->clock_source = init_param->clock_source;

	if (dev->clock_source == AD5933_CLOCK_SOURCE_INTERNAL)
		dev->clock_freq = AD5933_INTERNAL_SYS_CLK;
	else
		dev->clock_freq = init_param->clock_freq;

	dev->pga_gain = init_param->pga_gain;
	dev->output_range = init_param->output_range;
	dev->settling_cycles = init_param->settling_cycles;

	ret = ad5933_setup(dev);

	if (ret)
		goto error_i2c;

	*device = dev;

	return 0;

error_i2c:
	no_os_i2c_remove(dev->i2c_desc);
error_dev:
	no_os_free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Sets up the AD5933 device with the current configuration values.
 *
 * @param dev     	- The device structure.
 *
 * @return ret - The result of the setup procedure.
*******************************************************************************/
int ad5933_setup(struct ad5933_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad5933_set_system_clk(dev, dev->clock_source,
				    dev->clock_freq);
	if (ret)
		return ret;

	ret = ad5933_set_range_and_gain(dev, dev->output_range, dev->pga_gain);
	if (ret)
		return ret;

	return ad5933_set_settling_time(dev, dev->settling_cycles);
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad5933_init().
 *
 * @param dev - The device structure.
 *
 * @return ret - The result of the remove procedure.
*******************************************************************************/
int ad5933_remove(struct ad5933_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = no_os_i2c_remove(dev->i2c_desc);

	no_os_free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Write a single byte into a register.
 *
 * @param dev - The device structure.
 * @param reg - Address of the register.
 * @param val - Byte value to write.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ad5933_reg_write(struct ad5933_dev *dev, uint8_t reg, uint8_t val)
{
	uint8_t buf[2];

	if (!dev)
		return -EINVAL;

	buf[0] = reg;
	buf[1] = val;

	return no_os_i2c_write(dev->i2c_desc, buf, 2, 1);
}

/***************************************************************************//**
 * @brief Read a single byte from a register.
 *
 * @param dev - The device structure.
 * @param reg - Address of the register.
 * @param val - Pointer to the read byte.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ad5933_reg_read(struct ad5933_dev *dev, uint8_t reg, uint8_t *val)
{
	uint8_t ptr[2];
	int ret;

	if (!dev || !val)
		return -EINVAL;

	ptr[0] = AD5933_ADDR_POINTER;
	ptr[1] = reg;
	ret = no_os_i2c_write(dev->i2c_desc, ptr, 2, 1);
	if (ret)
		return ret;

	return no_os_i2c_read(dev->i2c_desc, val, 1, 1);
}

/***************************************************************************//**
 * @brief Write a multi-byte value into a register.
 *
 * @param dev              - The device structure.
 * @param register_address - Address of the register.
 * @param register_value   - Data value to write.
 * @param bytes_number     - Number of bytes
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ad5933_set_register_value(struct ad5933_dev *dev,
			      uint8_t register_address,
			      uint32_t register_value,
			      uint8_t bytes_number)
{
	uint8_t byte;
	uint8_t i;
	int ret;

	if (!dev || bytes_number == 0 || bytes_number > 4)
		return -EINVAL;

	for (i = 0; i < bytes_number; i++) {
		byte = (uint8_t)((register_value >> ((bytes_number - i - 1) * 8)) &
				 0xFF);
		ret = ad5933_reg_write(dev, register_address + i, byte);
		if (ret)
			return ret;
	}

	return 0;
}

/***************************************************************************//**
 * @brief Reads a multi-byte value from a register.
 *
 * @param dev              - The device structure.
 * @param data             - Pointer to the read register value.
 * @param register_address - Address of the register.
 * @param bytes_number     - Number of bytes.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ad5933_get_register_value(struct ad5933_dev *dev,
			      uint32_t *data,
			      uint8_t register_address,
			      uint8_t bytes_number)
{
	uint32_t value = 0;
	uint8_t byte;
	uint8_t i;
	int ret;

	if (!dev || !data || bytes_number == 0 || bytes_number > 4)
		return -EINVAL;

	for (i = 0; i < bytes_number; i++) {
		ret = ad5933_reg_read(dev, register_address + i, &byte);
		if (ret)
			return ret;

		value = (value << 8) | byte;
	}

	*data = value;

	return 0;
}

/***************************************************************************//**
 * @brief Resets the device.
 *
 * @param dev             - The device structure.
*******************************************************************************/
int ad5933_reset(struct ad5933_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return ad5933_reg_write(dev, AD5933_REG_CONTROL_LB,
				AD5933_CONTROL_RESET | dev->clock_source);
}

/***************************************************************************//**
 * @brief Selects the source of the system clock.
 *
 * @param dev          - The device structure.
 * @param clk_source   - Selects the source of the system clock.
 *                     Example: AD5933_CONTROL_INT_SYSCLK
 *                              AD5933_CONTROL_EXT_SYSCLK
 * @param ext_clk_freq - Frequency value of the external clock, if used.
*******************************************************************************/
int ad5933_set_system_clk(struct ad5933_dev *dev,
			  enum ad5933_clock_source clk_source,
			  uint32_t ext_clk_freq)
{
	int ret;

	if (!dev)
		return -EINVAL;

	if (clk_source == AD5933_CLOCK_SOURCE_EXTERNAL &&
	    (!ext_clk_freq || ext_clk_freq > AD5933_INTERNAL_SYS_CLK))
		return -EINVAL;

	ret = ad5933_reg_write(dev, AD5933_REG_CONTROL_LB,
			       (uint8_t)clk_source);
	if (ret)
		return ret;

	dev->clock_source = clk_source;

	if (clk_source == AD5933_CLOCK_SOURCE_EXTERNAL)
		dev->clock_freq = ext_clk_freq;        	// External clock frequency
	else
		dev->clock_freq = AD5933_INTERNAL_SYS_CLK; // 16.776 MHz

	return 0;
}

/***************************************************************************//**
 * @brief Selects the range and gain of the device.
 *
 * @param dev   - The device structure.
 * @param range - Range option.
 *                Example: AD5933_RANGE_2000mVpp
 *                         AD5933_RANGE_200mVpp
 *                         AD5933_RANGE_400mVpp
 *                         AD5933_RANGE_1000mVpp
 * @param gain  - Gain option.
 *                Example: AD5933_GAIN_X5
 *                         AD5933_GAIN_X1
*******************************************************************************/
int ad5933_set_range_and_gain(struct ad5933_dev *dev,
			      enum ad5933_output_range range,
			      enum ad5933_pga_gain gain)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad5933_reg_write(dev, AD5933_REG_CONTROL_HB,
			       AD5933_CONTROL_FUNCTION(AD5933_FUNCTION_NOP) |
			       AD5933_CONTROL_RANGE(range) |
			       AD5933_CONTROL_PGA_GAIN(gain));
	if (ret)
		return ret;

	dev->output_range = range;
	dev->pga_gain = gain;

	return 0;
}

/***************************************************************************//**
 * @brief Selects the output excitation range of the device.
 *
 * @param dev   - The device structure.
 * @param range - Range option (see enum ad5933_output_range).
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ad5933_set_range(struct ad5933_dev *dev, enum ad5933_output_range range)
{
	if (!dev)
		return -EINVAL;

	return ad5933_set_range_and_gain(dev, range, dev->pga_gain);
}

/***************************************************************************//**
 * @brief Selects the PGA gain of the device.
 *
 * @param dev  - The device structure.
 * @param gain - Gain option.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ad5933_set_gain(struct ad5933_dev *dev, enum ad5933_pga_gain gain)
{
	if (!dev)
		return -EINVAL;

	return ad5933_set_range_and_gain(dev, dev->output_range, gain);
}

/***************************************************************************//**
 * @brief Reads the temperature from the part and returns the data in
 *        degrees Celsius.
 *
 * @param dev             - The device structure.
 * @param temperature     - Pointer to the temperature value in degrees Celsius.
*******************************************************************************/
int ad5933_get_temperature(struct ad5933_dev *dev, float *temperature)
{
	int32_t raw_temp = 0;
	int ret;

	if (!temperature)
		return -EINVAL;

	ret = ad5933_get_raw_temperature(dev, &raw_temp);
	if (ret)
		return ret;

	/* 14-bit two's-complement code sign-extended to 32-bit, 32 LSB per degree Celsius. */
	*temperature = raw_temp / 32.0f;

	return 0;
}

/***************************************************************************//**
 * @brief Configures the sweep parameters: Start frequency, Frequency increment
 *        and Number of increments.
 *
 * @param dev        		- The device structure.
 * @param start_freq 		- Start frequency in Hz;
 * @param inc_freq   		- Frequency increment in Hz;
 * @param num_increments    - Number of increments. Maximum value is 511(0x1FF).
*******************************************************************************/
int ad5933_config_sweep(struct ad5933_dev *dev,
			double start_freq,
			double inc_freq,
			uint16_t num_increments)
{
	uint32_t start_freq_reg = 0;
	uint32_t inc_freq_reg = 0;
	double end_freq;
	int ret;

	if (!dev)
		return -EINVAL;

	if (num_increments > AD5933_MAX_INC_NUM)
		return -EINVAL;

	if (start_freq < AD5933_MIN_OUTPUT_FREQ(dev->clock_freq)
	    || start_freq > AD5933_MAX_OUTPUT_FREQ(dev->clock_freq))
		return -EINVAL;

	/* Convert start frequency to binary code. */
	start_freq_reg = ad5933_convert_freq_to_reg(dev, start_freq);

	if (!start_freq_reg)
		return -EINVAL;

	if (inc_freq > AD5933_MAX_OUTPUT_FREQ(dev->clock_freq) || inc_freq < 0)
		return -EINVAL;

	/* Convert increment frequency to binary code. */
	inc_freq_reg = ad5933_convert_freq_to_reg(dev, inc_freq);

	end_freq = start_freq + inc_freq * num_increments;

	if (end_freq > AD5933_MAX_OUTPUT_FREQ(dev->clock_freq))
		return -EINVAL;

	ret = ad5933_set_register_value(dev,
					AD5933_REG_FREQ_START,
					start_freq_reg,
					3);
	if (ret)
		return ret;

	ret = ad5933_set_register_value(dev,
					AD5933_REG_FREQ_INC,
					inc_freq_reg,
					3);
	if (ret)
		return ret;

	ret = ad5933_set_register_value(dev,
					AD5933_REG_INC_NUM,
					(uint32_t) num_increments,
					2);
	if (ret)
		return ret;

	dev->start_freq = start_freq;
	dev->freq_increment = inc_freq;
	dev->num_increments = num_increments;
	dev->sweep_point = 0;
	dev->current_output_freq = start_freq;

	return 0;
}

/***************************************************************************//**
 * @brief Reads the real and imaginary value from register after requesting a frequency function
 *
 * @param dev                   - The device structure.
 * @param freq_function         - Frequency function.
 * @param real         			- Pointer to real data
 * @param imag         			- Pointer to imaginary data
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ad5933_get_data(struct ad5933_dev *dev,
		    uint8_t freq_function,
		    int16_t *real,
		    int16_t *imag)
{
	uint32_t real_raw = 0;
	uint32_t imag_raw = 0;
	int ret;

	if (!dev || !real || !imag)
		return -EINVAL;

	ret = ad5933_reg_write(dev, AD5933_REG_CONTROL_HB,
			       AD5933_CONTROL_FUNCTION(freq_function) |
			       AD5933_CONTROL_RANGE(dev->output_range) |
			       AD5933_CONTROL_PGA_GAIN(dev->pga_gain));
	if (ret)
		return ret;

	ret = ad5933_wait_status(dev, AD5933_STAT_DATA_VALID, NULL);
	if (ret)
		return ret;

	ret = ad5933_get_register_value(dev, &real_raw, AD5933_REG_REAL_DATA, 2);
	if (ret)
		return ret;

	ret = ad5933_get_register_value(dev, &imag_raw, AD5933_REG_IMAG_DATA, 2);
	if (ret)
		return ret;

	*real = (int16_t)real_raw;
	*imag = (int16_t)imag_raw;

	return 0;
}

/***************************************************************************//**
 * @brief Reads the real and the imaginary data and calculates the Gain Factor.
 *
 * @param dev                   - The device structure.
 * @param gain_factor           - The gain factor pointer.
 * @param calibration_impedance - The calibration impedance value.
 * @param measurement_mode		- 2-wire (default) or 4-wire.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ad5933_calculate_gain_factor(struct ad5933_dev *dev,
				 double *gain_factor,
				 double calibration_impedance,
				 enum meas_mode measurement_mode)
{
	double magnitude = 0;
	int ret;

	if (!gain_factor || calibration_impedance == 0)
		return -EINVAL;

	ret = ad5933_get_magnitude(dev, &magnitude);
	if (ret)
		return ret;

	if (fabs(magnitude) <= DIV_EPSILON)
		return -EINVAL;

	if (measurement_mode == AD5933_MEAS_MODE_2W)
		*gain_factor = 1.0 / (magnitude * calibration_impedance);
	else
		*gain_factor = calibration_impedance / magnitude;

	return 0;
}

/***************************************************************************//**
 * @brief Reads the real and the imaginary data and calculates the Impedance.
 *
 * @param dev           		- The device structure.
 * @param gain_factor   		- The gain factor.
 * @param impedance     		- Pointer to the Impedance in ohms.
 * @param measurement_mode		- 2-wire (default) or 4-wire.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ad5933_calculate_impedance(struct ad5933_dev *dev,
			       double gain_factor,
			       double *impedance,
			       enum meas_mode measurement_mode)
{
	double magnitude = 0;
	int ret;

	if (!dev || !impedance)
		return -EINVAL;

	ret = ad5933_get_magnitude(dev, &magnitude);
	if (ret)
		return ret;

	if (fabs(magnitude) <= DIV_EPSILON || fabs(gain_factor) <= DIV_EPSILON)
		return -EINVAL;

	if (measurement_mode == AD5933_MEAS_MODE_2W)
		*impedance = 1.0 / (magnitude * gain_factor);
	else
		*impedance = magnitude * gain_factor;

	return 0;
}

/***************************************************************************//**
 * @brief Selects the number of settling cycles of the device.
 *
 * @param dev		 - The device structure.
 * @param number_cycles	 - 9-bit number of cycles to wait before triggering ADC,
 * 							multiplier is inferred based on the number of cycles.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ad5933_set_settling_time(struct ad5933_dev *dev,
			     uint16_t number_cycles)
{
	int ret;
	uint32_t reg_val;

	if (!dev)
		return -EINVAL;

	number_cycles = no_os_clamp(number_cycles, 0, AD5933_MAX_SETTLING_CYCLES);

	if (number_cycles > 1022)
		reg_val = (number_cycles >> 2) | (AD5933_SETTLING_X4 << 9);
	else if (number_cycles > 511)
		reg_val = (number_cycles >> 1) | (AD5933_SETTLING_X2 << 9);
	else
		reg_val = number_cycles;
	ret = ad5933_set_register_value(dev,
					AD5933_REG_SETTLING_CYCLES,
					reg_val,
					2);

	if (ret)
		return ret;

	dev->settling_cycles = number_cycles;

	return 0;
}

/***************************************************************************//**
 * @brief Polls the status register until (status & mask) is set or timeout.
 *
 * @param dev        - The device structure.
 * @param mask       - Status bits to wait for.
 * @param status_out - Optional pointer to receive the last status read.
 *
 * @return 0 when the mask is satisfied, -EAGAIN on timeout, or another
 *         negative error code on I2C failure.
*******************************************************************************/
int ad5933_wait_status(struct ad5933_dev *dev, uint8_t mask,
		       uint8_t *status_out)
{
	uint32_t timeout = AD5933_MAX_RETRIES;
	uint8_t status;
	int ret;

	if (!dev)
		return -EINVAL;

	do {
		ret = ad5933_reg_read(dev, AD5933_REG_STATUS, &status);
		if (ret)
			return ret;

		if (status & mask) {
			if (status_out)
				*status_out = status;
			return 0;
		}

		no_os_mdelay(AD5933_STATUS_POLL_MS);
	} while (timeout--);

	return -EAGAIN;
}

/***************************************************************************//**
 * @brief Reads the raw temperature from the part.
 *
 * The temperature sensor returns a 14-bit two's-complement code (13 data bits
 * plus a sign bit); the caller converts to degrees Celsius by dividing by 32.
 *
 * @param dev             - The device structure.
 * @param temperature     - Pointer to the sign-extended raw temperature code.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ad5933_get_raw_temperature(struct ad5933_dev *dev, int32_t *temperature)
{
	uint32_t reg_value = 0;
	int ret;

	if (!dev || !temperature)
		return -EINVAL;

	ret = ad5933_reg_write(dev, AD5933_REG_CONTROL_HB,
			       AD5933_CONTROL_FUNCTION(AD5933_FUNCTION_MEASURE_TEMP) |
			       AD5933_CONTROL_RANGE(dev->output_range) |
			       AD5933_CONTROL_PGA_GAIN(dev->pga_gain));
	if (ret)
		return ret;

	ret = ad5933_wait_status(dev, AD5933_STAT_TEMP_VALID, NULL);
	if (ret)
		return ret;

	ret = ad5933_get_register_value(dev, &reg_value, AD5933_REG_TEMP_DATA, 2);
	if (ret)
		return ret;

	/* Bit 13 is the sign bit of the 14-bit temperature field. */
	*temperature = no_os_sign_extend32(reg_value, 13);

	return 0;
}

/***************************************************************************//**
 * @brief Reads the real and imaginary data and calculates the DFT magnitude.
 *
 * @param dev           - The device structure.
 * @param magnitude     - Pointer to the DFT magnitude sqrt(real^2 + imag^2).
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ad5933_get_magnitude(struct ad5933_dev *dev,
			 double *magnitude)
{
	int16_t real = 0;
	int16_t imag = 0;
	int ret;

	if (!dev || !magnitude)
		return -EINVAL;

	ret = ad5933_get_current_data(dev, &real, &imag);
	if (ret)
		return ret;

	/* Promote to double before squaring to avoid 32-bit int overflow. */
	*magnitude = sqrt((double)real * real + (double)imag * imag);

	return 0;
}

/***************************************************************************//**
 * @brief Reads the data and calculates the impedance phase in radians. The system phase captured at calibration is subtracted to yield the
 * phase due to the impedance alone.
 *
 * @param dev           - The device structure.
 * @param system_phase  - The system phase captured at calibration in radians.
 * @param phase_rad     - Pointer to the impedance phase in radians.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ad5933_get_phase(struct ad5933_dev *dev,
		     double system_phase,
		     double *phase_rad)
{
	double phase;
	int16_t real = 0;
	int16_t imag = 0;
	int ret;

	if (!dev || !phase_rad)
		return -EINVAL;

	ret = ad5933_get_current_data(dev, &real, &imag);
	if (ret)
		return ret;

	if (real == 0 && imag == 0)
		phase = 0.0;
	else
		phase = atan2((double)imag, (double)real);


	*phase_rad = phase - system_phase;

	return 0;
}

/***************************************************************************//**
 * @brief Initializes the sweep sequence documented in the datasheet. Unknown impedance
 * is connected to the output and the start frequency is applied.
 * The device is now ready to start the sweep.
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ad5933_initialize_sweep(struct ad5933_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	/* 1. Enter standby mode. */
	ret = ad5933_standby(dev);
	if (ret)
		return ret;

	/* 2. Reset (clears status flags; sweep parameters are preserved). */
	ret = ad5933_reset(dev);
	if (ret)
		return ret;

	/* 3. Initialize with start frequency (excite, no measurement yet). */
	return ad5933_init_start_freq(dev);
}

/***************************************************************************//**
 * @brief  Enables the DDS to output the programmed start frequency for an indefinite time.
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ad5933_init_start_freq(struct ad5933_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return ad5933_reg_write(dev, AD5933_REG_CONTROL_HB,
				AD5933_CONTROL_FUNCTION(AD5933_FUNCTION_INIT_START_FREQ) |
				AD5933_CONTROL_RANGE(dev->output_range) |
				AD5933_CONTROL_PGA_GAIN(dev->pga_gain));
}

/***************************************************************************//**
 * @brief  ADC starts measuring after the programmed number of settling
 * time cycles has elapsed.
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ad5933_start_sweep(struct ad5933_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad5933_reg_write(dev, AD5933_REG_CONTROL_HB,
			       AD5933_CONTROL_FUNCTION(AD5933_FUNCTION_START_SWEEP) |
			       AD5933_CONTROL_RANGE(dev->output_range) |
			       AD5933_CONTROL_PGA_GAIN(dev->pga_gain));
	if (ret)
		return ret;

	dev->sweep_point = 0;

	return 0;
}

/***************************************************************************//**
 * @brief Step to the next frequency point in the sweep.
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ad5933_increment_freq(struct ad5933_dev *dev)
{
	int ret;
	int32_t sweep_done;

	if (!dev)
		return -EINVAL;

	ret = ad5933_sweep_done(dev, &sweep_done);

	if (ret)
		return ret;

	if (sweep_done)
		return 0;

	ret = ad5933_reg_write(dev, AD5933_REG_CONTROL_HB,
			       AD5933_CONTROL_FUNCTION(AD5933_FUNCTION_INC_FREQ) |
			       AD5933_CONTROL_RANGE(dev->output_range) |
			       AD5933_CONTROL_PGA_GAIN(dev->pga_gain));
	if (ret)
		return ret;

	dev->sweep_point++;
	dev->current_output_freq += dev->freq_increment;

	return 0;
}

/***************************************************************************//**
 * @brief Repeat the current frequency point measurement.
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ad5933_repeat_freq(struct ad5933_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return ad5933_reg_write(dev, AD5933_REG_CONTROL_HB,
				AD5933_CONTROL_FUNCTION(AD5933_FUNCTION_REPEAT_FREQ) |
				AD5933_CONTROL_RANGE(dev->output_range) |
				AD5933_CONTROL_PGA_GAIN(dev->pga_gain));
}

/***************************************************************************//**
 * @brief  Default state on power-up of the AD5933 is power-down mode.
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ad5933_power_down(struct ad5933_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return ad5933_reg_write(dev, AD5933_REG_CONTROL_HB,
				AD5933_CONTROL_FUNCTION(AD5933_FUNCTION_POWER_DOWN) |
				AD5933_CONTROL_RANGE(dev->output_range) |
				AD5933_CONTROL_PGA_GAIN(dev->pga_gain));
}

/***************************************************************************//**
 * @brief Powers up the part for general operation.
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ad5933_standby(struct ad5933_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return ad5933_reg_write(dev, AD5933_REG_CONTROL_HB,
				AD5933_CONTROL_FUNCTION(AD5933_FUNCTION_STANDBY) |
				AD5933_CONTROL_RANGE(dev->output_range) |
				AD5933_CONTROL_PGA_GAIN(dev->pga_gain));
}

/***************************************************************************//**
 * @brief  Converts a frequency value to the corresponding register binary value.
 *
 * @param dev - The device structure.
 * @param frequency - The frequency value in Hz.
 *
 * @return Binary code to write into the corresponding register.
*******************************************************************************/
uint32_t ad5933_convert_freq_to_reg(struct ad5933_dev *dev, double frequency)
{
	if (!dev || dev->clock_freq == 0)
		return 0;

	return (uint32_t)(frequency * 4 / dev->clock_freq *
			  pow_2_27);
}

/***************************************************************************//**
 * @brief Reads the real and imaginary value from register.
 *
 * The caller must have waited for the point to become valid.
 *
 * @param dev  - The device structure.
 * @param real - Pointer to the signed 16-bit real component.
 * @param imag - Pointer to the signed 16-bit imaginary component.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ad5933_get_current_data(struct ad5933_dev *dev, int16_t *real,
			    int16_t *imag)
{
	uint32_t real_raw = 0;
	uint32_t imag_raw = 0;
	int ret;

	if (!dev || !real || !imag)
		return -EINVAL;

	ret = ad5933_get_register_value(dev, &real_raw, AD5933_REG_REAL_DATA, 2);
	if (ret)
		return ret;

	ret = ad5933_get_register_value(dev, &imag_raw, AD5933_REG_IMAG_DATA, 2);
	if (ret)
		return ret;

	*real = (int16_t)real_raw;
	*imag = (int16_t)imag_raw;

	return 0;
}

/**
 * @brief Check if a frequency sweep has completed.
 *
 * Reads the STATUS register and extracts the SWEEP_DONE bit to indicate
 * whether the AD5933 has finished the current frequency sweep.
 *
 * @param dev  The device structure.
 * @param done Pointer to integer that will be non-zero if sweep done,
 *             0 otherwise.
 *
 * @return 0 on success, negative error code on failure.
 */
int ad5933_sweep_done(struct ad5933_dev *dev, int32_t *done)
{
	uint8_t status;
	int ret;

	if (!dev || !done)
		return -EINVAL;

	ret = ad5933_reg_read(dev, AD5933_REG_STATUS, &status);
	if (ret)
		return ret;

	*done = status & AD5933_STAT_SWEEP_DONE;

	return 0;
}

/***************************************************************************//**
 * @brief Wait for the AD5933 to acknowledge on the I2C bus after power-up.
 *
 * On a cold start the AD5933 is powered from the controller board's supply
 * rail, which ramps for a while after the host MCU has already booted and
 * configured its I2C peripheral. Until the part's VDD is valid it NACKs its
 * slave address, so the first register access fails. This probes the
 * read-only STATUS register with retries until the device responds or a
 * timeout elapses. On a warm start (device already powered) it succeeds on
 * the first attempt, so no time is wasted.
 *
 * @param dev - The device structure.
 *
 * @return 0 once the device acknowledges, -ETIMEDOUT otherwise.
*******************************************************************************/
static int ad5933_wait_powerup(struct ad5933_dev *dev)
{
	uint32_t retries = AD5933_POWERUP_RETRIES;
	uint8_t status;

	if (!dev)
		return -EINVAL;

	do {
		if (!ad5933_reg_read(dev, AD5933_REG_STATUS, &status))
			return 0;
		no_os_mdelay(AD5933_POWERUP_RETRY_MS);
	} while (retries--);

	return -ETIMEDOUT;
}
