/***************************************************************************//**
 *   @file   max9611.c
 *   @brief  Implementation of max9611 Family Driver.
 *   @author MSosa (marcpaolo.sosa@analog.com)
 *   @author JSanbuen (jose.sanbuenaventura@analog.com)
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
#include "no_os_i2c.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "max9611.h"

/***************************************************************************//**
 * @brief Generates slave address based on A1 and A0 pin values
 * @param device_ip - init param structure
 * @param a0        - A0 logic level
 * @param a1        - A1 logic level
 * @return addr - Computed slave ID
*******************************************************************************/
int max9611_addr_gen(struct max9611_init_param *device_ip,
		     enum max9611_addr_lvls a0, enum max9611_addr_lvls a1)
{
	if (a0 < MAX9611_ZERO_VCC || a0 > MAX9611_100_VCC)
		return -EINVAL;

	switch (a1) {
	case MAX9611_ZERO_VCC:
		device_ip->i2c_init.slave_address = 0xE0;
		break;
	case MAX9611_33_VCC:
		device_ip->i2c_init.slave_address = 0xE8;
		break;
	case MAX9611_66_VCC:
		device_ip->i2c_init.slave_address = 0xF0;
		break;
	case MAX9611_100_VCC:
		device_ip->i2c_init.slave_address = 0xF8;
		break;
	default:
		return -EINVAL;
	}

	device_ip->i2c_init.slave_address += 2 * a0;
	device_ip->i2c_init.slave_address >>= 1;

	return 0;
}

/***************************************************************************//**
 * @brief Initializes the communication peripheral and checks if the device is
 *        present.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return ret - The result of the initialization procedure.
 *                  Example: -1 - I2C peripheral wasn't initialized or the
 *                                device is not present.
 *                            0 - I2C peripheral was initialized and the
 *                                device is present.
*******************************************************************************/
int max9611_init(struct max9611_dev **device,
		 struct max9611_init_param init_param)
{
	int ret;
	struct max9611_dev *dev;

	dev = (struct max9611_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_i2c_init(&dev->i2c_desc, &init_param.i2c_init);
	if (ret)
		goto error_dev;

	*device = dev;

	return 0;

error_dev:
	no_os_free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by max538x_init().
 * @param dev - The device structure.
 * @return ret - The result of the remove procedure.
*******************************************************************************/
int max9611_remove(struct max9611_dev *dev)
{
	int ret;

	ret = no_os_i2c_remove(dev->i2c_desc);
	no_os_free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Reads data from device register (I2C)
 * @param dev       - The device structure.
 * @param addr      - Starting / selected register address.
 * @param bytes     - Number of bytes to read from starting register address
 * @param read_data - Read data buffer
 * @return          - 0 if I2C read is successful, error otherwise.
*******************************************************************************/
int max9611_read(struct max9611_dev *dev, uint8_t addr,
		 uint8_t bytes, uint8_t *read_data)
{
	int ret;

	/* Check if valid register */
	if (addr < MAX9611_CSA_MSB || addr > MAX9611_CTR2)
		return -EINVAL;

	/* Avoid read to out of bounds register */
	if (addr + bytes > MAX9611_OUT_OF_BOUNDS)
		return -EINVAL;

	ret = no_os_i2c_write(dev->i2c_desc, &addr, 1, 0);
	if (ret)
		return ret;

	return no_os_i2c_read(dev->i2c_desc, read_data, bytes, 1);
}

/***************************************************************************//**
 * @brief Writes data to device register (I2C)
 * @param dev 		  - The device structure.
 * @param addr 		  - Starting / selected register address.
 * @param bytes 	  - Number of bytes to write from starting register address
 * @param write_data  - Write data buffer
 * @return            - 0 if I2C write is successful, error otherwise.
*******************************************************************************/
int max9611_write(struct max9611_dev *dev, uint8_t addr,
		  uint8_t bytes, uint8_t *write_data)
{
	int i;
	uint8_t buf[bytes + 1];

	/* Check if valid register */
	if (addr < MAX9611_CSA_MSB || addr > MAX9611_CTR2)
		return -EINVAL;

	/* Avoid write to out of bounds register */
	if (addr + bytes > MAX9611_OUT_OF_BOUNDS)
		return -EINVAL;

	/* Exit when writing to Read only registers */
	if (addr < MAX9611_CTR1)
		return -EINVAL;

	buf[0] = addr;

	for (i = 0; i < bytes; i++)
		buf[i + 1] = write_data[i];

	return no_os_i2c_write(dev->i2c_desc, buf, bytes + 1, 1);
}

/***************************************************************************//**
 * @brief Update selected I2C register contents
 * @param dev   	  - The device structure.
 * @param addr 		  - Starting / selected register address.
 * @param update_mask - Mask to update to selected register bit/s
 * @param update_val  - Value used to update on selected register
 * @return            - 0 if I2C write is successful, error otherwise.
*******************************************************************************/
int max9611_reg_update(struct max9611_dev *dev, uint8_t addr,
		       int update_mask, int update_val)
{
	uint8_t temp;
	int ret;

	ret = max9611_read(dev, addr, 1, &temp);
	if (ret)
		return ret;

	temp &= ~update_mask;
	temp |= update_val;

	return max9611_write(dev, addr, 1, &temp);
}

/***************************************************************************//**
 * @brief Configures MAX9611 operating mode
 * @param dev 		  - The device structure.
 * @param mode 		  - MODE configuration
 * @return            - 0 if successful, error otherwise.
*******************************************************************************/
int max9611_set_mode(struct max9611_dev *dev, enum max9611_mode_conf mode)
{
	if (mode != MAX9611_NORMAL_MODE && mode != MAX9611_OPAMP_MODE
	    && mode != MAX9611_COMPARATOR_MODE)
		return -EINVAL;

	return max9611_reg_update(dev, MAX9611_CTR1,  MAX9611_MODE_MASK,
				  no_os_field_prep(MAX9611_MODE_MASK, mode));
}

/***************************************************************************//**
 * @brief Get the MAX9611 operating mode
 * @param dev 		  - The device structure.
 * @param mode 		  - Pointer to store the obtained mode
 * @return            - 0 if successful, error otherwise.
*******************************************************************************/
int max9611_get_mode(struct max9611_dev *dev, enum max9611_mode_conf *mode)
{
	int ret;
	uint8_t temp;

	ret = max9611_read(dev, MAX9611_CTR1, 1, &temp);
	if (ret)
		return ret;

	*mode = no_os_field_get(MAX9611_MODE_MASK, temp);

	return 0;
}

/***************************************************************************//**
 * @brief Configures the internal MUX settings
 * @param dev 		  - The device structure.
 * @param mux 		  - MUX configuration
 * @return            - 0 if successful, error otherwise.
*******************************************************************************/
int max9611_set_mux(struct max9611_dev *dev, enum max9611_mux_conf mux)
{
	switch (mux) {
	case MAX9611_CONF_SENSE_1X:
		dev->capture_type = MAX9611_DATA_CSA;
		break;
	case MAX9611_CONF_SENSE_4X:
		dev->capture_type = MAX9611_DATA_CSA;
		break;
	case MAX9611_CONF_SENSE_8X:
		dev->capture_type = MAX9611_DATA_CSA;
		break;
	case MAX9611_CONF_IN_COM_MODE:
		dev->capture_type = MAX9611_DATA_RSP;
		break;
	case MAX9611_CONF_OUT_ADC:
		dev->capture_type = MAX9611_DATA_OUT;
		break;
	case MAX9611_CONF_SET_ADC:
		dev->capture_type = MAX9611_DATA_SET;
		break;
	case MAX9611_CONF_TEMP:
		dev->capture_type = MAX9611_DATA_TMP;
		break;
	case MAX9611_FAST_MODE:
		dev->capture_type = MAX9611_DATA_CSA;
		break;
	default:
		return -EINVAL;
	}

	return max9611_reg_update(dev, MAX9611_CTR1, MAX9611_MUX_MASK, mux);
}

/***************************************************************************//**
 * @brief Get the internal MUX settings
 * @param dev 		  - The device structure.
 * @param mux 		  - Pointer to store the obtained mux config
 * @return            - 0 if successful, error otherwise.
*******************************************************************************/
int max9611_get_mux(struct max9611_dev *dev, enum max9611_mux_conf *mux)
{
	int ret;
	uint8_t temp;

	ret = max9611_read(dev, MAX9611_CTR1, 1, &temp);
	if (ret)
		return ret;

	*mux = temp;

	return 0;
}

/***************************************************************************//**
 * @brief Device shutdown or power up
 * @param dev 		  - The device structure.
 * @param is_shdn	  - Set to true to shutdown device, false otherwise
 * @return            - 0 if successful, error otherwise.
*******************************************************************************/
int max9611_shutdown(struct max9611_dev *dev, bool is_shdn)
{
	if (is_shdn)
		return max9611_reg_update(dev, MAX9611_CTR1, MAX9611_SHDN_MASK,
					  no_os_field_prep(MAX9611_SHDN_MASK, 1));

	return max9611_reg_update(dev, MAX9611_CTR1, MAX9611_SHDN_MASK, 0);
}

/***************************************************************************//**
 * @brief Configure LR bit operation
 * @param dev 		  - The device structure.
 * @param is_normal	  - Set true for normal operation. False otherwise (refer to LR bit in data sheet)
 * @return            - 0 if successful, error otherwise.
*******************************************************************************/
int max9611_set_lr(struct max9611_dev *dev, bool is_normal)
{
	if (is_normal)
		return max9611_reg_update(dev, MAX9611_CTR1, MAX9611_LR_MASK, 0);

	return max9611_reg_update(dev, MAX9611_CTR1, MAX9611_LR_MASK,
				  no_os_field_prep(MAX9611_LR_MASK, 1));
}

/***************************************************************************//**
 * @brief Get LR bit value
 * @param dev 		  - The device structure.
 * @param lr	      - Pointer to store LR bit value
 * @return            - 0 if successful, error otherwise.
*******************************************************************************/
int max9611_get_lr(struct max9611_dev *dev, bool *lr)
{
	int ret;
	uint8_t temp;

	ret = max9611_read(dev, MAX9611_CTR1, 1, &temp);
	if (ret)
		return ret;

	*lr = (bool)no_os_field_get(MAX9611_LR_MASK, temp);

	return 0;
}

/***************************************************************************//**
 * @brief Configures MAX9611 delay time
 * @param dev 		  - The device structure.
 * @param dtime		  - Delay option
 * @return            - 0 if successful, error otherwise.
*******************************************************************************/
int max9611_set_delay(struct max9611_dev *dev, enum max9611_delay_time dtime)
{
	if (dtime < MAX9611_1MS || dtime > MAX9611_100US)
		return -EINVAL;

	dtime = no_os_field_prep(MAX9611_DTIME_MASK, dtime);

	return max9611_reg_update(dev, MAX9611_CTR2, dtime,
				  MAX9611_DTIME_MASK);
}

/***************************************************************************//**
 * @brief Get MAX9611 configured delay time
 * @param dev 		  - The device structure.
 * @param dtime	      - Pointer to store delay time read
 * @return            - 0 if successful, error otherwise.
*******************************************************************************/
int max9611_get_delay(struct max9611_dev *dev, enum max9611_delay_time *dtime)
{
	int ret;
	uint8_t temp;

	ret = max9611_read(dev, MAX9611_CTR2, 1, &temp);
	if (ret)
		return ret;

	*dtime = no_os_field_get(MAX9611_DTIME_MASK, temp);

	return 0;
}

/***************************************************************************//**
 * @brief Configures MAX9611 retry time
 * @param dev 		  - The device structure.
 * @param rtime		  - Retry time option
 * @return            - 0 if successful, error otherwise.
*******************************************************************************/
int max9611_set_retry(struct max9611_dev *dev, enum max9611_retry_time rtime)
{
	if (rtime < MAX9611_50MS || rtime > MAX9611_10MS)
		return -EINVAL;

	rtime = no_os_field_prep(MAX9611_RTIME_MASK, rtime);

	return max9611_reg_update(dev, MAX9611_CTR2, rtime,
				  MAX9611_RTIME_MASK);
}

/***************************************************************************//**
 * @brief Get MAX9611 configured retry time
 * @param dev 		  - The device structure.
 * @param rtime	      - Pointer to store retry time read
 * @return            - 0 if successful, error otherwise.
*******************************************************************************/
int max9611_get_retry(struct max9611_dev *dev, enum max9611_retry_time *rtime)
{
	int ret;
	uint8_t temp;

	ret = max9611_read(dev, MAX9611_CTR2, 1, &temp);
	if (ret)
		return ret;

	*rtime = no_os_field_get(MAX9611_RTIME_MASK, temp);

	return 0;
}

/***************************************************************************//**
 * @brief Receive data from ADC (single value only).
 * @param dev 		  - The device structure.
 * @param raw         - Pointer to store the raw 12-bit ADC reading
 * @return            - 0 if successful, error otherwise.
*******************************************************************************/
int max9611_get_raw(struct max9611_dev *dev, uint16_t *raw)
{
	int ret;
	uint8_t buf[2];

	switch (dev->capture_type) {
	case MAX9611_DATA_CSA:
		ret = max9611_read(dev, MAX9611_CSA_MSB, 2, buf);
		break;
	case MAX9611_DATA_RSP:
		ret = max9611_read(dev, MAX9611_RSP_MSB, 2, buf);
		break;
	case MAX9611_DATA_OUT:
		ret = max9611_read(dev, MAX9611_OUT_MSB, 2, buf);
		break;
	case MAX9611_DATA_SET:
		ret = max9611_read(dev, MAX9611_SET_MSB, 2, buf);
		break;
	case MAX9611_DATA_TMP:
		ret = max9611_read(dev, MAX9611_TMP_MSB, 2, buf);
		break;
	default:
		return -EINVAL;
	}

	if (ret)
		return ret;

	*raw = no_os_field_get(MAX9611_RAW_DATA_MASK, no_os_get_unaligned_be16(buf));

	return 0;
}
