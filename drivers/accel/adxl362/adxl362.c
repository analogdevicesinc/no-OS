/***************************************************************************//**
 *   @file   adxl362.c
 *   @brief  Implementation of ADXL362 Driver.
 *   @author DNechita(Dan.Nechita@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
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
#include "adxl362.h"
#include "no_os_alloc.h"

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
 * @return  0 - the initialization was successful and the device is present;
 *         -1 - an error occurred.
*******************************************************************************/
int32_t adxl362_init(struct adxl362_dev **device,
		     struct adxl362_init_param init_param)
{
	struct adxl362_dev *dev;
	uint8_t reg_value = 0;
	int32_t status = -1;

	dev = (struct adxl362_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* SPI */
	status = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);

	adxl362_get_register_value(dev, &reg_value, ADXL362_REG_PARTID, 1);
	if((reg_value != ADXL362_PART_ID))
		status = -1;

	dev->selected_range = 2; // Measurement Range: +/- 2g (reset default).

	*device = dev;

	return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by adxl362_init().
 *
 * @param dev - The device structure.
 *
 * @return ret - The result of the remove procedure.
*******************************************************************************/
int32_t adxl362_remove(struct adxl362_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_desc);

	no_os_free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Writes data into a register.
 *
 * @param dev              - The device structure.
 * @param register_value   - Data value to write.
 * @param register_address - Address of the register.
 * @param bytes_number     - Number of bytes. Accepted values: 0 - 1.
 *
 * @return None.
*******************************************************************************/
void adxl362_set_register_value(struct adxl362_dev *dev,
				uint16_t register_value,
				uint8_t register_address,
				uint8_t bytes_number)
{
	uint8_t buffer[4] = {0, 0, 0, 0};

	buffer[0] = ADXL362_WRITE_REG;
	buffer[1] = register_address;
	buffer[2] = (register_value & 0x00FF);
	buffer[3] = (register_value >> 8);
	no_os_spi_write_and_read(dev->spi_desc,
				 buffer,
				 bytes_number + 2);
}

/***************************************************************************//**
 * @brief Performs a burst read of a specified number of registers.
 *
 * @param dev              - The device structure.
 * @param read_data        - The read values are stored in this buffer.
 * @param register_address - The start address of the burst read.
 * @param bytes_number     - Number of bytes to read.
 *
 * @return None.
*******************************************************************************/
void adxl362_get_register_value(struct adxl362_dev *dev,
				uint8_t* read_data,
				uint8_t  register_address,
				uint8_t  bytes_number)
{
	uint8_t buffer[32];

	uint8_t index = 0;

	buffer[0] = ADXL362_READ_REG;
	buffer[1] = register_address;
	for(index = 0; index < bytes_number; index++)
		buffer[index + 2] = read_data[index];
	no_os_spi_write_and_read(dev->spi_desc,
				 buffer,
				 bytes_number + 2);
	for(index = 0; index < bytes_number; index++)
		read_data[index] = buffer[index + 2];
}

/***************************************************************************//**
 * @brief Reads multiple bytes from the device's FIFO buffer.
 *
 * @param dev          - The device structure.
 * @param buffer       - Stores the read bytes.
 * @param bytes_number - Number of bytes to read.
 *
 * @return None.
*******************************************************************************/
void adxl362_get_fifo_value(struct adxl362_dev *dev,
			    uint8_t  *buffer,
			    uint16_t bytes_number)
{
	uint8_t spi_buffer[512];

	uint16_t index = 0;

	spi_buffer[0] = ADXL362_WRITE_FIFO;
	for(index = 0; index < bytes_number; index++)
		spi_buffer[index + 1] = buffer[index];
	no_os_spi_write_and_read(dev->spi_desc,
				 spi_buffer,
				 bytes_number + 1);
	for(index = 0; index < bytes_number; index++)
		buffer[index] = spi_buffer[index + 1];
}

/***************************************************************************//**
 * @brief Resets the device via SPI communication bus.
 *
 * @param dev - The device structure.
 *
 * @return None.
*******************************************************************************/
void adxl362_software_reset(struct adxl362_dev *dev)
{
	adxl362_set_register_value(dev,
				   ADXL362_RESET_KEY,
				   ADXL362_REG_SOFT_RESET,
				   1);
}

/***************************************************************************//**
 * @brief Places the device into standby/measure mode.
 *
 * @param dev      - The device structure.
 * @param pwr_mode - Power mode.
 *                   Example: 0 - standby mode.
 *                            1 - measure mode.
 *
 * @return None.
*******************************************************************************/
void adxl362_set_power_mode(struct adxl362_dev *dev,
			    uint8_t pwr_mode)
{
	uint8_t old_power_ctl = 0;
	uint8_t new_power_ctl = 0;

	adxl362_get_register_value(dev,
				   &old_power_ctl,
				   ADXL362_REG_POWER_CTL,
				   1);
	new_power_ctl = old_power_ctl & ~ADXL362_POWER_CTL_MEASURE(0x3);
	new_power_ctl = new_power_ctl |
			(pwr_mode * ADXL362_POWER_CTL_MEASURE(ADXL362_MEASURE_ON));
	adxl362_set_register_value(dev,
				   new_power_ctl,
				   ADXL362_REG_POWER_CTL,
				   1);
}

/***************************************************************************//**
 * @brief Selects the measurement range.
 *
 * @param dev - The device structure.
 * @param g_range - Range option.
 *                  Example: ADXL362_RANGE_2G  -  +-2 g
 *                           ADXL362_RANGE_4G  -  +-4 g
 *                           ADXL362_RANGE_8G  -  +-8 g
 *
 * @return None.
*******************************************************************************/
void adxl362_set_range(struct adxl362_dev *dev,
		       uint8_t g_range)
{
	uint8_t old_filter_ctl = 0;
	uint8_t new_filter_ctl = 0;

	adxl362_get_register_value(dev,
				   &old_filter_ctl,
				   ADXL362_REG_FILTER_CTL,
				   1);
	new_filter_ctl = old_filter_ctl & ~ADXL362_FILTER_CTL_RANGE(0x3);
	new_filter_ctl = new_filter_ctl | ADXL362_FILTER_CTL_RANGE(g_range);
	adxl362_set_register_value(dev,
				   new_filter_ctl,
				   ADXL362_REG_FILTER_CTL,
				   1);
	dev->selected_range = (1 << g_range) * 2;
}

/***************************************************************************//**
 * @brief Selects the Output Data Rate of the device.
 *
 * @param dev      - The device structure.
 * @param out_rate - Output Data Rate option.
 *                   Example: ADXL362_ODR_12_5_HZ  -  12.5Hz
 *                            ADXL362_ODR_25_HZ    -  25Hz
 *                            ADXL362_ODR_50_HZ    -  50Hz
 *                            ADXL362_ODR_100_HZ   -  100Hz
 *                            ADXL362_ODR_200_HZ   -  200Hz
 *                            ADXL362_ODR_400_HZ   -  400Hz
 *
 * @return None.
*******************************************************************************/
void adxl362_set_output_rate(struct adxl362_dev *dev,
			     uint8_t out_rate)
{
	uint8_t old_filter_ctl = 0;
	uint8_t new_filter_ctl = 0;

	adxl362_get_register_value(dev,
				   &old_filter_ctl,
				   ADXL362_REG_FILTER_CTL,
				   1);
	new_filter_ctl = old_filter_ctl & ~ADXL362_FILTER_CTL_ODR(0x7);
	new_filter_ctl = new_filter_ctl | ADXL362_FILTER_CTL_ODR(out_rate);
	adxl362_set_register_value(dev,
				   new_filter_ctl,
				   ADXL362_REG_FILTER_CTL,
				   1);
}

/***************************************************************************//**
 * @brief Reads the 3-axis raw data from the accelerometer.
 *
 * @param dev - The device structure.
 * @param x   - Stores the X-axis data(as two's complement).
 * @param y   - Stores the Y-axis data(as two's complement).
 * @param z   - Stores the Z-axis data(as two's complement).
 *
 * @return None.
*******************************************************************************/
void adxl362_get_xyz(struct adxl362_dev *dev,
		     int16_t* x,
		     int16_t* y,
		     int16_t* z)
{
	uint8_t xyz_values[6] = {0, 0, 0, 0, 0, 0};

	adxl362_get_register_value(dev,
				   xyz_values,
				   ADXL362_REG_XDATA_L,
				   6);
	*x = ((int16_t)xyz_values[1] << 8) + xyz_values[0];
	*y = ((int16_t)xyz_values[3] << 8) + xyz_values[2];
	*z = ((int16_t)xyz_values[5] << 8) + xyz_values[4];
}

/***************************************************************************//**
 * @brief Reads the 3-axis raw data from the accelerometer and converts it to g.
 *
 * @param dev - The device structure.
 * @param x   - Stores the X-axis data.
 * @param y   - Stores the Y-axis data.
 * @param z   - Stores the Z-axis data.
 *
 * @return None.
*******************************************************************************/
void adxl362_get_g_xyz(struct adxl362_dev *dev,
		       float* x,
		       float* y,
		       float* z)
{
	uint8_t xyz_values[6] = {0, 0, 0, 0, 0, 0};

	adxl362_get_register_value(dev,
				   xyz_values,
				   ADXL362_REG_XDATA_L,
				   6);
	*x = ((int16_t)xyz_values[1] << 8) + xyz_values[0];
	*x /= (1000 / (dev->selected_range / 2));
	*y = ((int16_t)xyz_values[3] << 8) + xyz_values[2];
	*y /= (1000 / (dev->selected_range / 2));
	*z = ((int16_t)xyz_values[5] << 8) + xyz_values[4];
	*z /= (1000 / (dev->selected_range / 2));
}

/***************************************************************************//**
 * @brief Reads the temperature of the device.
 *
 * @param dev - The device structure.
 *
 * @return temp_celsius - The value of the temperature(degree s Celsius).
*******************************************************************************/
float adxl362_read_temperature(struct adxl362_dev *dev)
{
	uint8_t raw_temp_data[2] = {0, 0};
	int16_t signed_temp = 0;
	float temp_celsius = 0;

	adxl362_get_register_value(dev,
				   raw_temp_data,
				   ADXL362_REG_TEMP_L,
				   2);
	signed_temp = (int16_t)(raw_temp_data[1] << 8) + raw_temp_data[0];
	temp_celsius = (float)signed_temp * 0.065;

	return temp_celsius;
}

/***************************************************************************//**
 * @brief Configures the FIFO feature.
 *
 * @param dev          - The device structure.
 * @param mode         - Mode selection.
 *                       Example: ADXL362_FIFO_DISABLE      -  FIFO is disabled.
 *                                ADXL362_FIFO_OLDEST_SAVED -  Oldest saved mode.
 *                                ADXL362_FIFO_STREAM       -  Stream mode.
 *                                ADXL362_FIFO_TRIGGERED    -  Triggered mode.
 * @param water_mark_lvl - Specifies the number of samples to store in the FIFO.
 * @param en_temp_read   - Store Temperature Data to FIFO.
 *                         Example: 1 - temperature data is stored in the FIFO
 *                                      together with x-, y- and x-axis data.
 *                                  0 - temperature data is skipped.
 *
 * @return None.
*******************************************************************************/
void adxl362_fifo_setup(struct adxl362_dev *dev,
			uint8_t  mode,
			uint16_t water_mark_lvl,
			uint8_t  en_temp_read)
{
	uint8_t write_val = 0;

	write_val = ADXL362_FIFO_CTL_FIFO_MODE(mode) |
		    (en_temp_read * ADXL362_FIFO_CTL_FIFO_TEMP) |
		    ((0x100 & water_mark_lvl) >> 5);
	adxl362_set_register_value(dev,
				   write_val,
				   ADXL362_REG_FIFO_CTL,
				   1);
	adxl362_set_register_value(dev,
				   water_mark_lvl,
				   ADXL362_REG_FIFO_SAMPLES,
				   1);
}

/***************************************************************************//**
 * @brief Configures activity detection.
 *
 * @param dev         - The device structure.
 * @param ref_or_abs  - Referenced/Absolute Activity Select.
 *                    Example: 0 - absolute mode.
 *                             1 - referenced mode.
 * @param threshold - 11-bit unsigned value that the adxl362 samples are
 *                    compared to.
 * @param time      - 8-bit value written to the activity timer register. The
 *                    amount of time (in seconds) is: time / ODR, where ODR - is
 *                    the output data rate.
 *
 * @return None.
*******************************************************************************/
void adxl362_setup_activity_detection(struct adxl362_dev *dev,
				      uint8_t  ref_or_abs,
				      uint16_t threshold,
				      uint8_t  time)
{
	uint8_t old_act_inact_reg = 0;
	uint8_t new_act_inact_reg = 0;

	/* Configure motion threshold and activity timer. */
	adxl362_set_register_value(dev,
				   (threshold & 0x7FF),
				   ADXL362_REG_THRESH_ACT_L,
				   2);
	adxl362_set_register_value(dev,
				   time,
				   ADXL362_REG_TIME_ACT,
				   1);
	/* Enable activity interrupt and select a referenced or absolute
	   configuration. */
	adxl362_get_register_value(dev,
				   &old_act_inact_reg,
				   ADXL362_REG_ACT_INACT_CTL,
				   1);
	new_act_inact_reg = old_act_inact_reg & ~ADXL362_ACT_INACT_CTL_ACT_REF;
	new_act_inact_reg |= ADXL362_ACT_INACT_CTL_ACT_EN |
			     (ref_or_abs * ADXL362_ACT_INACT_CTL_ACT_REF);
	adxl362_set_register_value(dev,
				   new_act_inact_reg,
				   ADXL362_REG_ACT_INACT_CTL,
				   1);
}

/***************************************************************************//**
 * @brief Configures inactivity detection.
 *
 * @param dev         - The device structure.
 * @param ref_or_abs  - Referenced/Absolute Inactivity Select.
 *                      Example: 0 - absolute mode.
 *                               1 - referenced mode.
 * @param threshold - 11-bit unsigned value that the adxl362 samples are
 *                    compared to.
 * @param time      - 16-bit value written to the inactivity timer register. The
 *                    amount of time (in seconds) is: time / ODR, where ODR - is
 *                    the output data rate.
 *
 * @return None.
*******************************************************************************/
void adxl362_setup_inactivity_detection(struct adxl362_dev *dev,
					uint8_t  ref_or_abs,
					uint16_t threshold,
					uint16_t time)
{
	uint8_t old_act_inact_reg = 0;
	uint8_t new_act_inact_reg = 0;

	/* Configure motion threshold and inactivity timer. */
	adxl362_set_register_value(dev, (threshold & 0x7FF),
				   ADXL362_REG_THRESH_INACT_L,
				   2);
	adxl362_set_register_value(dev, time, ADXL362_REG_TIME_INACT_L, 2);
	/* Enable inactivity interrupt and select a referenced or absolute
	   configuration. */
	adxl362_get_register_value(dev,
				   &old_act_inact_reg,
				   ADXL362_REG_ACT_INACT_CTL,
				   1);
	new_act_inact_reg = old_act_inact_reg & ~ADXL362_ACT_INACT_CTL_INACT_REF;
	new_act_inact_reg |= ADXL362_ACT_INACT_CTL_INACT_EN |
			     (ref_or_abs * ADXL362_ACT_INACT_CTL_INACT_REF);
	adxl362_set_register_value(dev,
				   new_act_inact_reg,
				   ADXL362_REG_ACT_INACT_CTL,
				   1);
}
