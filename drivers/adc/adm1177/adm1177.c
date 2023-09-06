/**************************************************************************//**
 *   @file   adm1177.c
 *   @brief  Source file of ADM1177 Driver
 *   @author Radu Sabau (radu.sabau@analog.com)
*******************************************************************************
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
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
******************************************************************************/
#include <stdlib.h>
#include "adm1177.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_error.h"

/**************************************************************************//**
 * @brief Initializes I2C.
 *
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return status - Initialization status.
 *                  Example:  negative value - Initialization failed.
 *                            0 - Initialization succeeded.
******************************************************************************/
int adm1177_init(struct adm1177_dev **device,
		 struct adm1177_init_param *init_param)
{
	struct adm1177_dev *dev;
	int status;

	dev = (struct adm1177_dev *)no_os_calloc(sizeof(*dev), 1);
	if (!dev)
		return -ENOMEM;

	/* Initialize I2C peripheral */
	status = no_os_i2c_init(&dev->i2c_desc, &init_param->i2c_init);
	if (status) {
		no_os_free(dev);
		return status;
	}

	*device = dev;

	return status;
}

/**************************************************************************//**
 * @brief Free the resources allocated by adm1177_init().
 *
 * @param device - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
******************************************************************************/
int adm1177_remove(struct adm1177_dev *device)
{
	int ret;

	ret = no_os_i2c_remove(device->i2c_desc);

	no_os_free(device);

	return ret;
}

/**************************************************************************//**
 * @brief - Sets the vrange off the ADM1177.
 *
 * @param device - The device structure.
 *
 * @param vrange - ADM1177_CMD_VRANGE for V Range of 6.65V, otherwise 26.65V
 * 		   will be used.
 *
 * @return 0 in case of success, negative error code otherwise.
******************************************************************************/
int adm1177_set_vrange(struct adm1177_dev *device, uint8_t vrange)
{
	uint8_t data;
	if (vrange & ADM1177_CMD_VRANGE) {
		device->vrange = ADM1177_VRANGE_HIGH;
		switch (device->last_command) {
		case ADM1177_CURRENT_EN:
			data = ADM1177_CMD_I_CONT | ADM1177_CMD_VRANGE;
			return no_os_i2c_write(device->i2c_desc, &data, 1, 1);
		case ADM1177_VOLTAGE_EN:
			data = ADM1177_CMD_V_CONT | ADM1177_CMD_VRANGE;
			return no_os_i2c_write(device->i2c_desc, &data, 1, 1);
		case ADM1177_VOLTAGE_AND_CURRENT_EN:
			data = ADM1177_CMD_V_CONT | ADM1177_CMD_I_CONT |
			       ADM1177_CMD_VRANGE;
			return no_os_i2c_write(device->i2c_desc, &data, 1, 1);
		default:
			return -EINVAL;
		}
	}
	return -EINVAL;
}

/**************************************************************************//**
 * @brief - In the write command byte operation, the master device sends a
 *          command byte to the slave device.
 *
 * @param device - The device structure.
 *
 * @param cmd - The command byte indicates which operation to be performed.
 *
 * @param reg_address - In case of a write extended command byte operation
 * 			one of the ADM1177_REG_ALERT_EN, ADM1177_REG_ALERT_TH
 * 			or ADM1177_REG_CONTROL will be used.
 * 			In case of a write command byte operation reg_address
 * 			will be set to 0.
 *
 * @return 0 in case of success, negative error code otherwise.
******************************************************************************/
int adm1177_write(struct adm1177_dev *device, uint8_t cmd, uint8_t reg_address)
{
	if (reg_address & ADM1177_EXT_CMD_MASK) {
		uint8_t data[2] = {reg_address, cmd};

		return no_os_i2c_write(device->i2c_desc, data, 2, 1);
	}
	if (cmd == ADM1177_CMD_I_CONT || cmd == ADM1177_CMD_I_ONCE) {
		device->last_command = ADM1177_CURRENT_EN;
		return no_os_i2c_write(device->i2c_desc, &cmd, 1, 1);
	}
	if (cmd == ADM1177_CMD_V_CONT || cmd == ADM1177_CMD_V_ONCE) {
		device->last_command = ADM1177_VOLTAGE_EN;
		return no_os_i2c_write(device->i2c_desc, &cmd, 1, 1);
	}
	if ((cmd == (ADM1177_CMD_I_CONT | ADM1177_CMD_V_CONT)) ||
	    (cmd == (ADM1177_CMD_I_ONCE | ADM1177_CMD_V_ONCE))) {
		device->last_command = ADM1177_VOLTAGE_AND_CURRENT_EN;
		return no_os_i2c_write(device->i2c_desc, &cmd, 1, 1);
	}
	return -EINVAL;
}

/**************************************************************************//**
 * @brief - Reads the status byte from the ADC if requested and transmits it
 * 	    through the status_byte pointer.
 *
 * @param device - The device structure.
 *
 * @param status_byte - The pointer through which the status byte is
 * 			transmitted.
 *
 * @return 0 in case of success, negative error code otherwise.
******************************************************************************/
int adm1177_read_status(struct adm1177_dev *device, uint8_t *status_byte)
{
	int ret;
	if (device->last_command == ADM1177_STATUS_EN) {
		if (status_byte == NULL)
			return -EINVAL;

		ret = no_os_i2c_read(device->i2c_desc, status_byte, 1, 1);
		if (ret)
			return ret;

		return 0;
	}
	return -EINVAL;
}

/**************************************************************************//**
 * @brief - Gets the voltage/current readback from the ADM1177s I2C ADC,
 *	    if the device is initialized right and transmits the requested
 *	    values.
 *
 * @param device - The device structure.
 *
 * @param conv_voltage - The variable through which the voltage read from the
 * 			 ADC is received if requested.
 * 			 If it is 0, it means it was not requested.
 *
 * @param conv_current - The variable through which the current read from the
 * 			 ADC is received if requested.
 * 			 If it is 0, it means it was not requested.
 *
 * @return 0 in case of success, negative error code otherwise.
******************************************************************************/
int adm1177_read_conv(struct adm1177_dev *device,
		      uint16_t* conv_voltage,
		      uint16_t* conv_current)
{
	int ret;
	uint8_t data[3];

	/* Time taken for device conversion (datasheet value = 150us) */
	no_os_udelay(150);

	switch (device->last_command) {
	case ADM1177_CURRENT_EN:
		if (conv_current == NULL)
			return -EINVAL;

		ret = no_os_i2c_read(device->i2c_desc, data, 2, 1);
		if (ret)
			return ret;

		/* First byte received contains the 8 MSBs of the
		   current and is left shifted by 4, then the second
		   byte contains the 4 LSBs of the current and is right
		   shifted by 4, and when the OR operation between the
		   two bytes happen we obtain the a 12-bit value of the
		   current. */
		*conv_current = (data[0] << 4) | (data[1] >> 4);

		return 0;
	case ADM1177_VOLTAGE_EN:
		if (conv_voltage == NULL)
			return -EINVAL;

		ret = no_os_i2c_read(device->i2c_desc, data, 2, 1);
		if (ret)
			return ret;

		/* First byte received contains the 8 MSBs of the
		   voltage and is left shifted by 4, then the second
		   byte contains the 4 LSBs of the voltage and is right
		   shifted by 4, and when the OR operation between the
		   two bytes happen we obtain the a 12-bit value of
		   the voltage. */
		*conv_voltage = (data[0] << 4) | (data[1] >> 4);

		return 0;
	case ADM1177_VOLTAGE_AND_CURRENT_EN:
		if (conv_voltage == NULL || conv_current == NULL)
			return -EINVAL;

		ret = no_os_i2c_read(device->i2c_desc, data, 3, 1);
		if (ret)
			return ret;

		/* In the data array we obtain 3 bytes from the read,
		   first one contains the 8 MSBs of the Voltage value,
		   second one contains the 8 MSBs of the Current value,
		   and the third one contains the 4 LSBs of the Voltage
		   then the Current value, respectively.
		   For the Voltage - By left shifting the first byte by
		   4 we are shifting the 8 MSBs of the voltage by 4,
		   and adding the third byte right shifted by 4 we add
		   the remaining 4 LSBs of the voltage.
		   For the Current -  By left shifting the second
		   byte by 4 we are shifting the 8 MSBs of the current
		   by 4, and adding the third byte masked by 0xF
		   we add the remaining 4 LSBs of the current.*/
		*conv_voltage = (data[0] << 4) | (data[2] >> 4);

		*conv_current = (data[1] << 4) | (data[2] & 0xF);

		return 0;
	default:
		return -EINVAL;
	}
}

/**************************************************************************//**
 * @brief - Converts the raw sample from the Readback into uV.
 *
 * @param device - The device structure.
 *
 * @param raw_sample - The raw sample of the Voltage Readback from the ADC.
 *
 * @param voltage_uv - The pointer through which the converted microvolt value
 * 		       is returned.
 *
 * @return - 0 in case of succes.
 * 	     negative error code if v_fullscale is set wrong.
******************************************************************************/
int adm1177_to_microvolts(struct adm1177_dev *device,
			  uint16_t raw_sample, uint64_t *voltage_uv)
{
	switch (device->vrange) {
	case ADM1177_VRANGE_LOW:
		*voltage_uv = (((uint64_t)ADM1177_V_FULLSCALE_0 *
				MICROVOLT_PER_VOLT) / ADM1177_RESOLUTION) /
			      MILLIVOLT_PER_VOLT * raw_sample;
		return 0;
	case ADM1177_VRANGE_HIGH:
		*voltage_uv = (((uint64_t)ADM1177_V_FULLSCALE_1 *
				MICROVOLT_PER_VOLT) / ADM1177_RESOLUTION) /
			      MILLIVOLT_PER_VOLT * raw_sample;
		return 0;
	default:
		return -EINVAL;
	}
}

/**************************************************************************//**
 * @brief - Converts the raw sample from the Readback into uA.
 *
 * @param raw_sample - The raw sample of the Current Readback from the ADC.
 *
 * @param current_ua - The pointer through which the converted microamper value
 * 		       is returned.
 *
 * @return - 0, because there is no actual return.
******************************************************************************/
int adm1177_to_microampers(uint16_t raw_sample, uint64_t *current_ua)
{
	*current_ua = (uint64_t)((((MILLIAMPER_PER_AMPER * ADM1177_I_FULLSCALE) /
				   ADM1177_RESOLUTION) * raw_sample) /
				 ADM1177_R_SENSE);
	return 0;
}
