/***************************************************************************//**
*   @file   ltc3350.c
*   @brief  Implementation of LTC3350 Driver
*   @authors Sankalp Khandkar (sankalp.khandkar@analog.com)
*            Ignacio Rojas (ignacio.rojas@analog.com)
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

#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

#include "no_os_util.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_i2c.h"

#include "ltc3350.h"

/***************************************************************************//**
 * @brief Initializes the communication peripheral and checks if the LTC3350
 *        part is present.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return ret       - Result of the initialization procedure.
*******************************************************************************/
int ltc3350_init(struct ltc3350_dev **device,
		 struct ltc3350_init_param *init_param)
{
	struct ltc3350_dev *dev;
	int ret;
	dev = (struct ltc3350_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_i2c_init(&dev->i2c_desc, &init_param->i2c_init);
	if (ret)
		goto err;

	*device = dev;
	return 0;

err:
	no_os_free(dev);
	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ltc3350_init().
 *
 * @param dev 	- The device structure.
 *
 * @return ret 	- Result of the remove procedure.
*******************************************************************************/
int ltc3350_remove(struct ltc3350_dev *dev)
{
	int ret;

	ret = no_os_i2c_remove(dev->i2c_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Reads data from the device (I2C).
 *
 * @param dev          - The device structure.
 * @param base_address - Address of the base register.
 * @param read_data    - The read data value
 *
 * @return ret         - Result of the reading procedure.
*******************************************************************************/
int ltc3350_read_device_data(struct ltc3350_dev *dev, uint8_t base_address,
			     uint16_t *read_data)
{
	int ret;
	uint8_t rx_buff[2] = {0};

	ret = no_os_i2c_write(dev->i2c_desc, &base_address, 1, 1);
	if (ret)
		return ret;

	ret = no_os_i2c_read(dev->i2c_desc, rx_buff, 2, 1);
	if (ret)
		return ret;

	*read_data = (rx_buff[1] << 8) | rx_buff[0];

	return ret;
}

/***************************************************************************//**
 * @brief Writes data to the device (I2C)
 *
 * @param dev          	- The device structure.
 * @param base_address 	- Address of the base register.
 * @param write_data   	- The data which is going to be written.
 *
 * @return ret         	- Result of the writing procedure.
*******************************************************************************/
int ltc3350_write_device_data(struct ltc3350_dev *dev, uint8_t base_address,
			      uint16_t write_data)
{
	uint8_t txbuff[3] = {0};

	txbuff[0] = base_address;
	txbuff[1] = write_data & 0xFF;
	txbuff[2] = write_data >> 8;

	return no_os_i2c_write(dev-> i2c_desc, txbuff, 3, 1);
}

#if	LTC3350_USE_MEASUREMENTS

/***************************************************************************//**
 * @brief  Gets number of capacitors configured based on PIN
 *
 * @param dev          	- The device structure.
 * @param num_caps	- Number of capacitors configured.
 *                        00-1 cap, 01 - 2 caps, 10 - 3caps and 11 - 4 caps
 *
 * @return ret         	- Result of the reading procedure.
*******************************************************************************/
int ltc3350_get_num_caps(struct ltc3350_dev *dev, uint8_t *num_caps)
{
	int ret;
	uint16_t value;

	ret = ltc3350_read_device_data(dev, LTC3350_AD_NUM_CAPS, &value);
	if (ret)
		return ret;

	*num_caps = value + 1;

	return 0;
}

/***************************************************************************//**
 * @brief Reads the ESR data.
 *
 * @param dev          	- The device structure.
 * @param value 	- Measured ESR.
 *
 * @return ret         	- Result of the reading procedure.
*******************************************************************************/
int ltc3350_get_esr(struct ltc3350_dev *dev, uint16_t *value)
{
	int ret;
	uint16_t read_value;

	ret = ltc3350_read_device_data(dev, LTC3350_AD_MEAS_ESR, &read_value);
	if (ret)
		return	ret;

	*value = read_value;

	return 0;
}

/***************************************************************************//**
 * @brief Reads the Vcap data for selected capacitor

 * @param dev          	- The device structure.
 * @param n_cap 	- Number of capacitor to read, 0 for stack voltage
 * @param value		- The read value
 *
 * @return ret         	- Result of the reading procedure.
*******************************************************************************/
int ltc3350_get_vcap(struct ltc3350_dev *dev, uint8_t n_cap, uint16_t *value)
{
	int ret;

	if (n_cap > 4)
		return -EINVAL;

	if (!n_cap) {
		return ltc3350_read_device_data(dev, LTC3350_AD_MEAS_VCAP, value);
	} else {
		return ltc3350_read_device_data(dev, LTC3350_AD_MEAS_VCAP1 + n_cap - 1,
						value);
	}

}

#endif

#if LTC3350_USE_ALARMS

/***************************************************************************//**
 * @brief Set the selected alarm and its related threshold
 *
 * @param dev          		- The device structure.
 * @param alarm 		- Alarm to be setted.
 * @param alarm_threshold   	- The desired threshold value to the selected alarm.
 *				  mV , Deg Celsius or uF
 * @return ret         		- Result of the alarm setting operation.
*******************************************************************************/
int ltc3350_set_alarm(struct ltc3350_dev *dev, enum ltc3350_enum_alarms alarm,
		      uint16_t alarm_threshold)
{
	int ret;
	uint16_t alarm_mask;
	uint16_t alarm_address;

	ret = ltc3350_read_device_data(dev, LTC3350_AD_MSK_ALARMS, &alarm_mask);
	if (ret)
		return ret;

	alarm_mask |= NO_OS_BIT(alarm);

	ret = ltc3350_write_device_data(dev, LTC3350_AD_MSK_ALARMS, alarm_mask);
	if (ret)
		return ret;

	alarm_address = LTC3350_AD_CAP_UV_LVL + alarm;

	return ltc3350_write_device_data(dev, alarm_address, alarm_threshold);
}

/***************************************************************************//**
 * @brief Mute an specific alarm
 *
 * @param dev          	- The device structure.
 * @param alarm 	- Alarm to be muted.
 *
 * @return ret         	- Result of the writing procedure.
*******************************************************************************/
int ltc3350_mute_alarm(struct ltc3350_dev *dev, enum ltc3350_enum_alarms alarm)
{
	int ret;
	uint16_t alarm_mask;

	ret = ltc3350_read_device_data(dev, LTC3350_AD_MSK_ALARMS, &alarm_mask);
	if (ret)
		return ret;

	alarm_mask &= ~NO_OS_BIT(alarm);

	return ltc3350_write_device_data(dev, LTC3350_AD_MSK_ALARMS, alarm_mask);
}

/***************************************************************************//**
 * @brief Get alarms status mask
 *
 * @param dev          	- The device structure.
 * @param status	- Status mask of the alarms
 *
 * @return ret         	- Result of the reading procedure.
*******************************************************************************/
int ltc3350_get_alarm_status_mask(struct ltc3350_dev *dev, uint16_t *alarm_mask)
{
	int ret;
	uint16_t read_val;

	ret = ltc3350_read_device_data(dev, LTC3350_AD_ALARM_REG, &read_val);
	if (ret)
		return ret;

	*alarm_mask = read_val;

	return 0;
}

/***************************************************************************//**
 * @brief Clear alarms status mask
 *
 * @param dev          - The device structure.
 *
 * @return ret         - Result of the writing procedure.
*******************************************************************************/
int ltc3350_clear_alarm_status_mask(struct ltc3350_dev *dev)
{
	return ltc3350_write_device_data(dev, LTC3350_AD_CLR_ALARMS, 0x0000);
}

#endif
