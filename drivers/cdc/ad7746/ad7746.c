/***************************************************************************//**
 *   @file   ad7746.c
 *   @brief  Implementation of AD7746 Driver.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
 */
#include <stdlib.h>
#include <string.h>
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "ad7746.h"

/***************************************************************************//**
 * @brief Initialize the ad7606 device structure.
 *
 * Performs memory allocation of the device structure.
 *
 * @param device     - Pointer to location of device structure to write.
 * @param init_param - Pointer to the configuration of the driver.
 * @return ret - return code.
 *         Example: -ENOMEM - Memory allocation error.
 *                  -EIO - I2C communication error.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7746_init(struct ad7746_dev **device,
		    struct ad7746_init_param *init_param)
{
	int32_t ret;
	struct ad7746_dev *dev;

	dev = (struct ad7746_dev *)no_os_calloc(1, sizeof(struct ad7746_dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_i2c_init(&dev->i2c_dev, &init_param->i2c_init);
	if (ret < 0)
		goto error_1;

	dev->id = init_param->id;

	ret = ad7746_reset(dev);
	if (ret < 0)
		goto error_2;

	// the device does not acknowledge for max: 200us after a reset
	no_os_udelay(200);

	ret = ad7746_set_cap(dev, init_param->setup.cap);
	if (ret < 0)
		goto error_2;

	ret = ad7746_set_vt(dev, init_param->setup.vt);
	if (ret < 0)
		goto error_2;

	ret = ad7746_set_exc(dev, init_param->setup.exc);
	if (ret < 0)
		goto error_2;

	ret = ad7746_set_config(dev, init_param->setup.config);
	if (ret < 0)
		goto error_2;

	*device = dev;

	return 0;
error_2:
	no_os_i2c_remove(dev->i2c_dev);
error_1:
	no_os_free(dev);
	return ret;
}

/***************************************************************************//**
 * @brief Writes data into AD7746 registers, starting from the selected
 *        register address pointer.
 *
 * @param dev - Device descriptor pointer.
 * @param reg - The selected register address.
 * @param data - Pointer to data to transmit.
 * @param bytes_number - Number of bytes to send (typically 1, other values when using the address auto-increment).
 *
 * @return return code.
 *         Example: -EINVAL - Wrong input values.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7746_reg_write(struct ad7746_dev *dev, uint8_t reg,
			 uint8_t* data, uint16_t bytes_number)
{
	if (!data || bytes_number > AD7746_NUM_REGISTERS || reg >= AD7746_NUM_REGISTERS)
		return -EINVAL;

	dev->buf[0] = reg;
	memcpy(&dev->buf[1], data, bytes_number);

	return no_os_i2c_write(dev->i2c_dev, dev->buf, bytes_number + 1, 1);
}

/***************************************************************************//**
 * @brief Reads data from AD7746 registers, starting from the selected
 *        register address pointer.
 *
 * @param dev - Device descriptor pointer.
 * @param reg - The selected register address pointer.
 * @param data - Pointer to a buffer that will store the received data.
 * @param bytes_number - Number of bytes to read (typically 1, other values when using the address auto-increment).
 *
 * @return return code.
 *         Example: -EINVAL - Wrong input values.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7746_reg_read(struct ad7746_dev *dev,
			uint8_t reg,
			uint8_t* data,
			uint16_t bytes_number)
{
	int32_t ret;

	if (!data || bytes_number > AD7746_NUM_REGISTERS || reg >= AD7746_NUM_REGISTERS)
		return -EINVAL;

	ret = no_os_i2c_write(dev->i2c_dev, &reg, 1, 0);
	if (ret < 0)
		return ret;

	return no_os_i2c_read(dev->i2c_dev, data, bytes_number, 1);
}

/***************************************************************************//**
 * @brief Resets the AD7746.
 *
 * @param dev - Device descriptor pointer.
 * @return return code.
 *         Example: -EINVAL - Wrong input values.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7746_reset(struct ad7746_dev *dev)
{
	uint8_t cmd = AD7746_RESET_CMD;

	if (!dev)
		return -EINVAL;

	return no_os_i2c_write(dev->i2c_dev, &cmd, 1, 1);
}

/***************************************************************************//**
 * @brief Deinitialize the ad7746 driver and free all allocated resources.
 *
 * @param dev - Device descriptor pointer.
 * @return 0
*******************************************************************************/
int32_t ad7746_remove(struct ad7746_dev *dev)
{
	if (!dev)
		return 0;

	no_os_i2c_remove(dev->i2c_dev);
	dev->i2c_dev = NULL;
	no_os_free(dev);

	return 0;
}

/***************************************************************************//**
 * @brief Configure the capacitive setup register.
 *
 * @param dev - Device descriptor pointer.
 * @param cap - Capacitive setup settings.
 *
 * @return return code.
 *         Example: -EINVAL - Wrong input values.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7746_set_cap(struct ad7746_dev *dev, struct ad7746_cap cap)
{
	int32_t ret;
	uint8_t reg;

	if (!dev)
		return -EINVAL;

	reg = no_os_field_prep(AD7746_CAPSETUP_CAPEN_MSK, cap.capen) |
	      no_os_field_prep(AD7746_CAPSETUP_CIN2_MSK, cap.cin2) |
	      no_os_field_prep(AD7746_CAPSETUP_CAPDIFF_MSK, cap.capdiff) |
	      no_os_field_prep(AD7746_CAPSETUP_CAPCHOP_MSK, cap.capchop);

	ret = ad7746_reg_write(dev, AD7746_REG_CAP_SETUP, &reg, 1);
	if (ret < 0)
		return ret;

	dev->setup.cap = cap;

	return 0;
}

/***************************************************************************//**
 * @brief Configure the voltage/temperature setup register.
 *
 * @param dev - Device descriptor pointer.
 * @param vt - Voltage/Temperature setup settings.
 *
 * @return return code.
 *         Example: -EINVAL - Wrong input values.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7746_set_vt(struct ad7746_dev *dev, struct ad7746_vt vt)
{
	int32_t ret;
	uint8_t reg;

	if (!dev)
		return -EINVAL;

	reg = no_os_field_prep(AD7746_VTSETUP_VTEN_MSK, vt.vten) |
	      no_os_field_prep(AD7746_VTSETUP_VTMD_MSK, vt.vtmd) |
	      no_os_field_prep(AD7746_VTSETUP_EXTREF_MSK, vt.extref) |
	      no_os_field_prep(AD7746_VTSETUP_VTSHORT_MSK, vt.vtshort) |
	      no_os_field_prep(AD7746_VTSETUP_VTCHOP_MSK, vt.vtchop);

	ret = ad7746_reg_write(dev, AD7746_REG_VT_SETUP, &reg, 1);
	if (ret < 0)
		return ret;

	dev->setup.vt = vt;

	return 0;
}

/***************************************************************************//**
 * @brief Configure the excitation setup register.
 *
 * @param dev - Device descriptor pointer.
 * @param exc - Excitation setup settings.
 *
 * @return return code.
 *         Example: -EINVAL - Wrong input values.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7746_set_exc(struct ad7746_dev *dev, struct ad7746_exc exc)
{
	int32_t ret;
	uint8_t reg;

	if (!dev)
		return -EINVAL;

	reg = no_os_field_prep(AD7746_EXCSETUP_CLKCTRL_MSK, exc.clkctrl) |
	      no_os_field_prep(AD7746_EXCSETUP_EXCON_MSK, exc.excon) |
	      no_os_field_prep(AD7746_EXCSETUP_EXCB_MSK, exc.excb) |
	      no_os_field_prep(AD7746_EXCSETUP_EXCA_MSK, exc.exca) |
	      no_os_field_prep(AD7746_EXCSETUP_EXCLVL_MSK, exc.exclvl);

	ret = ad7746_reg_write(dev, AD7746_REG_EXC_SETUP, &reg, 1);
	if (ret < 0)
		return ret;

	dev->setup.exc = exc;

	return 0;
}

/***************************************************************************//**
 * @brief Set the configuration register.
 *
 * @param dev - Device descriptor pointer.
 * @param config - Configuration register settings.
 *
 * @return return code.
 *         Example: -EINVAL - Wrong input values.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7746_set_config(struct ad7746_dev *dev, struct ad7746_config config)
{
	int32_t ret;
	uint8_t reg;

	if (!dev)
		return -EINVAL;

	reg = no_os_field_prep(AD7746_CONF_VTF_MSK, config.vtf) |
	      no_os_field_prep(AD7746_CONF_CAPF_MSK, config.capf) |
	      no_os_field_prep(AD7746_CONF_MD_MSK, config.md);

	ret = ad7746_reg_write(dev, AD7746_REG_CFG, &reg, 1);
	if (ret < 0)
		return ret;

	dev->setup.config = config;

	return 0;
}

/***************************************************************************//**
 * @brief Set the DAC code and enable state for EXCA.
 *
 * @param dev - Device descriptor pointer.
 * @param enable - DAC_A Enable.
 * @param code - DAC_A register code.
 *
 * @return return code.
 *         Example: -EINVAL - Wrong input values.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7746_set_cap_dac_a(struct ad7746_dev *dev, bool enable, uint8_t code)
{
	uint8_t reg;

	if (!dev)
		return -EINVAL;

	reg = no_os_field_prep(AD7746_CAPDAC_DACEN_MSK, enable) |
	      no_os_field_prep(AD7746_CAPDAC_DACP_MSK, code);

	return ad7746_reg_write(dev, AD7746_REG_CAPDACA, &reg, 1);
}

/***************************************************************************//**
 * @brief Set the DAC code and enable state for EXCB.
 *
 * @param dev - Device descriptor pointer.
 * @param enable - DAC_B Enable.
 * @param code - DAC_B register code.
 *
 * @return return code.
 *         Example: -EINVAL - Wrong input values.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7746_set_cap_dac_b(struct ad7746_dev *dev, bool enable, uint8_t code)
{
	uint8_t reg;

	if (!dev)
		return -EINVAL;

	reg = no_os_field_prep(AD7746_CAPDAC_DACEN_MSK, enable) |
	      no_os_field_prep(AD7746_CAPDAC_DACP_MSK, code);

	return ad7746_reg_write(dev, AD7746_REG_CAPDACB, &reg, 1);
}

static inline int32_t _ad7746_write_2byte(struct ad7746_dev *dev, uint8_t reg,
		uint16_t val)
{
	uint8_t buf[2];

	if (!dev)
		return -EINVAL;

	buf[0] = (uint8_t)(val >> 8);
	buf[1] = (uint8_t)val;

	return ad7746_reg_write(dev, reg, buf, 2);
}

/***************************************************************************//**
 * @brief Set the capacitive offset.
 *
 * @param dev - Device descriptor pointer.
 * @param offset - Offset as raw code.
 *
 * @return return code.
 *         Example: -EINVAL - Wrong input values.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7746_set_cap_offset(struct ad7746_dev *dev, uint16_t offset)
{
	return _ad7746_write_2byte(dev, AD7746_REG_CAP_OFFH, offset);
}

/***************************************************************************//**
 * @brief Set the capacitive gain.
 *
 * @param dev - Device descriptor pointer.
 * @param gain - Gain as raw code.
 *
 * @return return code.
 *         Example: -EINVAL - Wrong input values.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7746_set_cap_gain(struct ad7746_dev *dev, uint16_t gain)
{
	return _ad7746_write_2byte(dev, AD7746_REG_CAP_GAINH, gain);
}

/***************************************************************************//**
 * @brief Set the voltage gain.
 *
 * @param dev - Device descriptor pointer.
 * @param gain - Gain as raw code.
 *
 * @return return code.
 *         Example: -EINVAL - Wrong input values.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7746_set_volt_gain(struct ad7746_dev *dev, uint16_t gain)
{
	return _ad7746_write_2byte(dev, AD7746_REG_VOLT_GAINH, gain);
}

/***************************************************************************//**
 * @brief Waits until a conversion on a voltage/temperature channel has been
 *        finished and returns the output data.
 *
 * @param dev - Device descriptor pointer.
 * @param vt_data - The content of the VT Data register.
 *
 * @return return code.
 *         Example: -EINVAL - Wrong input values.
 *                  -EIO - I2C Communication error.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7746_get_vt_data(struct ad7746_dev *dev, uint32_t *vt_data)
{
	int32_t ret;

	if (!dev || !vt_data)
		return -EINVAL;

	memset(dev->buf, 0, 3);

	dev->buf[0] = AD7746_STATUS_RDYVT_MSK;
	while(dev->buf[0] & AD7746_STATUS_RDYVT_MSK) {
		ret = ad7746_reg_read(dev, AD7746_REG_STATUS, dev->buf, 1);
		if (ret < 0)
			return ret;
	}

	ret = ad7746_reg_read(dev, AD7746_REG_VT_DATA_HIGH, dev->buf, 3);
	if (ret < 0)
		return ret;

	*vt_data = ((uint32_t)dev->buf[0] << 16) |
		   ((uint32_t)dev->buf[1] << 8) |
		   dev->buf[0];

	if (dev->setup.config.md == AD7746_MODE_SINGLE)
		dev->setup.config.md = AD7746_MODE_IDLE;

	return 0;
}

/***************************************************************************//**
 * @brief Waits until a conversion on the capacitive channel has been
 *        finished and returns the output data.
 *
 * @param dev - Device descriptor pointer.
 * @param cap_data - The content of the Capacitive Data register.
 *
 * @return return code.
 *         Example: -EINVAL - Wrong input values.
 *                  -EIO - I2C Communication error.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7746_get_cap_data(struct ad7746_dev *dev, uint32_t *cap_data)
{
	int32_t ret;

	if (!dev || !cap_data)
		return -EINVAL;

	memset(dev->buf, 0, 3);

	dev->buf[0] = AD7746_STATUS_RDYCAP_MSK;
	while(dev->buf[0] & AD7746_STATUS_RDYCAP_MSK) {
		ret = ad7746_reg_read(dev, AD7746_REG_STATUS,	dev->buf, 1);
		if (ret < 0)
			return ret;
	}

	ret = ad7746_reg_read(dev, AD7746_REG_CAP_DATA_HIGH, dev->buf, 3);
	if (ret < 0)
		return ret;

	*cap_data = ((uint32_t)dev->buf[0] << 16) |
		    ((uint32_t)dev->buf[1] << 8) |
		    dev->buf[0];

	if (dev->setup.config.md == AD7746_MODE_SINGLE)
		dev->setup.config.md = AD7746_MODE_IDLE;

	return 0;
}

/***************************************************************************//**
 * @brief Perform offset/gain calibration
 *
 * @param dev - Device descriptor pointer.
 * @param md - AD7746 calibration mode specifier.
 *
 * @return return code.
 *         Example: -EINVAL - Wrong input values.
 *                  -EIO - I2C Communication error.
 *                  0 - No errors encountered.
*******************************************************************************/
int32_t ad7746_calibrate(struct ad7746_dev *dev, enum ad7746_md md)
{
	struct ad7746_config *c = &dev->setup.config;
	int32_t ret, timeout = 10;
	uint8_t reg;

	if (md != AD7746_MODE_OFFSET_CALIB &&
	    md != AD7746_MODE_GAIN_CALIB)
		return -EINVAL;

	c->md = md;
	ret = ad7746_set_config(dev, *c);
	if (ret < 0)
		return ret;

	do {
		// Wait an arbitrary time to avoid too many reg reads.
		// This typically results in 1 read only.
		no_os_mdelay(20);

		ret = ad7746_reg_read(dev, AD7746_REG_CFG, &reg, 1);
		if (ret < 0)
			return ret;

		c->md = reg & AD7746_CONF_MD_MSK;
	} while ((c->md != AD7746_MODE_IDLE) && timeout--);

	if (!timeout)
		ret = -ETIMEDOUT;

	return ret;
}
