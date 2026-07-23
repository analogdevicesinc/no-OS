/***************************************************************************//**
 *   @file   ad7291.c
 *   @brief  Implementation of AD7291 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
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
 *
*******************************************************************************/

#include "ad7291.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_util.h"

/**
 * @brief Read a 16-bit register.
 * @param desc - Device descriptor.
 * @param addr - Register address.
 * @param val - Pointer to store the read value.
 * @return 0 on success, negative error code otherwise.
 */
int ad7291_reg_read(struct ad7291_desc *desc, uint8_t addr, uint16_t *val)
{
	uint8_t buf[2];
	int ret;

	buf[0] = addr;
	ret = no_os_i2c_write(desc->i2c_desc, buf, 1, 0);
	if (ret)
		return ret;

	ret = no_os_i2c_read(desc->i2c_desc, buf, 2, 1);
	if (ret)
		return ret;

	*val = no_os_get_unaligned_be16(buf);

	return 0;
}

/**
 * @brief Write a 16-bit register.
 * @param desc - Device descriptor.
 * @param addr - Register address.
 * @param val - Value to write.
 * @return 0 on success, negative error code otherwise.
 */
int ad7291_reg_write(struct ad7291_desc *desc, uint8_t addr, uint16_t val)
{
	uint8_t buf[3];

	buf[0] = addr;
	no_os_put_unaligned_be16(val, &buf[1]);

	return no_os_i2c_write(desc->i2c_desc, buf, 3, 1);
}

/**
 * @brief Read a single channel voltage in millivolts.
 * @param desc - Device descriptor.
 * @param channel - Channel number (0-7).
 * @param millivolts - Pointer to store the result in mV.
 * @return 0 on success, negative error code otherwise.
 */
int ad7291_read_channel_voltage(struct ad7291_desc *desc, uint8_t channel,
				int32_t *millivolts)
{
	uint16_t cmd;
	uint16_t raw;
	uint8_t ch_addr;
	uint16_t code;
	int ret;

	if (channel >= AD7291_NUM_CHANNELS)
		return -EINVAL;

	ret = ad7291_reg_read(desc, AD7291_REG_COMMAND, &cmd);
	if (ret)
		return ret;

	/* Enable only the requested channel, keep DELAY bit set. */
	cmd &= ~(0xFF00);
	cmd |= AD7291_COMMAND_CH(channel) | AD7291_COMMAND_DELAY;

	ret = ad7291_reg_write(desc, AD7291_REG_COMMAND, cmd);
	if (ret)
		return ret;

	ret = ad7291_reg_read(desc, AD7291_REG_VOLTAGE, &raw);
	if (ret)
		return ret;

	ch_addr = no_os_field_get(AD7291_VOLTAGE_CH_MASK, raw);
	if (ch_addr != channel)
		return -EIO;

	code = no_os_field_get(AD7291_VOLTAGE_DATA_MASK, raw);
	*millivolts = (int32_t)code * desc->vref_mv / (1 << AD7291_RESOLUTION);

	return 0;
}

/**
 * @brief Read the internal temperature sensor in millidegrees Celsius.
 * @param desc - Device descriptor.
 * @param millidegrees - Pointer to store the result in m-deg-C.
 * @return 0 on success, negative error code otherwise.
 */
int ad7291_read_temp(struct ad7291_desc *desc, int32_t *millidegrees)
{
	uint16_t cmd;
	uint16_t raw;
	int32_t val;
	int ret;

	ret = ad7291_reg_read(desc, AD7291_REG_COMMAND, &cmd);
	if (ret)
		return ret;

	cmd |= AD7291_COMMAND_TSENSE;
	ret = ad7291_reg_write(desc, AD7291_REG_COMMAND, cmd);
	if (ret)
		return ret;

	ret = ad7291_reg_read(desc, AD7291_REG_T_SENSE, &raw);
	if (ret)
		return ret;

	raw = no_os_field_get(AD7291_TSENSE_DATA_MASK, raw);
	val = no_os_sign_extend32(raw, AD7291_TSENSE_SIGN_BIT);

	/* 1 LSB = 0.25 deg C -> multiply by 250 to get millidegrees */
	*millidegrees = val * 250;

	return 0;
}

/**
 * @brief Initialize the AD7291 device.
 * @param desc - Pointer to the device descriptor pointer.
 * @param init_param - Initialization parameters.
 * @return 0 on success, negative error code otherwise.
 */
int ad7291_init(struct ad7291_desc **desc,
		struct ad7291_init_param *init_param)
{
	struct ad7291_desc *d;
	int ret;

	d = no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	ret = no_os_i2c_init(&d->i2c_desc, &init_param->i2c_init);
	if (ret)
		goto free_desc;

	d->vref_mv = init_param->vref_mv;
	if (!d->vref_mv)
		d->vref_mv = AD7291_INTERNAL_VREF_MV;

	ret = ad7291_reg_write(d, AD7291_REG_COMMAND, AD7291_COMMAND_DELAY);
	if (ret)
		goto free_i2c;

	*desc = d;
	return 0;

free_i2c:
	no_os_i2c_remove(d->i2c_desc);
free_desc:
	no_os_free(d);
	return ret;
}

/**
 * @brief Remove the AD7291 device and free resources.
 * @param desc - Device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int ad7291_remove(struct ad7291_desc *desc)
{
	int ret;

	if (!desc)
		return -EINVAL;

	ret = no_os_i2c_remove(desc->i2c_desc);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}