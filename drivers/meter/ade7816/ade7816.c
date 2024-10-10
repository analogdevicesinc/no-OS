/***************************************************************************//**
 *   @file   ade7816.c
 *   @brief  Source file of ADE7816 Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "ade7816.h"

/**
 * @brief Register read wrapper function
 * @param desc - ADE7816 device descriptor.
 * @param reg - Register Address value.
 * @param val - Register Value to be read.
 * @return 0 in case of succes, negative error code otherwise.
 */
int ade7816_read_reg(struct ade7816_desc *desc, uint16_t reg, uint32_t *val)
{
	return desc->reg_read(desc, reg, val);
}

/**
 * @brief Register write wrapper function
 * @param desc - ADE7816 device descriptor
 * @param reg - Register Address value.
 * @param val - Register Value to be written.
 * @return 0 in case of succes, negative error code otherwise.
 */
int ade7816_write_reg(struct ade7816_desc *desc, uint16_t reg, uint32_t val)
{
	return desc->reg_write(desc, reg, val);
}

/**
 * @brief Register update function
 * @param desc - ADE7816 device descriptor.
 * @param addr - address of the register.
 * @param mask - bit mask of the field to be updated.
 * @param val - value of the masked field. Should be bit shifted by using
 * 		 no_os_field_prep(mask, val).
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7816_reg_update(struct ade7816_desc *desc, uint16_t reg, uint32_t mask,
		       uint32_t val)
{
	uint32_t reg_val;
	int ret;

	ret = ade7816_read_reg(desc, reg, &reg_val);
	if (ret)
		return ret;

	reg_val &= ~mask;
	reg_val |= val;

	return ade7816_write_reg(desc, reg, val);
}

int ade7816_sw_reset(struct ade7816_desc *desc)
{
	int ret;

	ret = ade7816_reg_update(desc, ADE7816_CONFIG_REG, ADE7816_SWRST_MASK,
				 no_os_field_prep(ADE7816_SWRST_MASK, 1));
	if (ret)
		return ret;

	no_os_mdelay(ADE7816_INIT_DELAY);

	return 0;
}

int ade7816_hw_reset(struct ade7816_desc *desc)
{
	int ret;

	ret = no_os_gpio_set_value(desc->reset_desc, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	no_os_udelay(ADE7816_HWRST_DELAY);

	ret = no_os_gpio_set_value(desc->reset_desc, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	no_os_mdelay(ADE7816_INIT_DELAY);

	return 0;
}

int ade7816_set_comm(struct ade7816_desc *desc, enum ade7816_comm_type type)
{
	int ret, i;

	switch (type) {
	case ADE7816_I2C:
		desc->reg_read = ade7816_i2c_reg_read;
		desc->reg_write = ade7816_i2c_reg_write;
		desc->comm_type = ADE7816_I2C;

		return ade7816_reg_update(desc, ADE7816_CONFIG2_REG,
					  ADE7816_I2C_LOCK_MASK,
					  no_os_field_prep(ADE7816_I2C_LOCK_MASK, 1));
	case ADE7816_SPI:
		desc->reg_read = ade7816_spi_reg_read;
		desc->reg_write = ade7816_spi_reg_write;
		desc->comm_type = ADE7816_SPI;

		for (i = 0; i < 3; i++) {
			ret = ade7816_write_reg(desc, 0xEBFF, 0);
			if (ret)
				return ret;
		}

		return ade7816_write_reg(desc, ADE7816_CONFIG2_REG, 0);
	default:
		return -EINVAL;
	}
}

int ade7816_read_active_energy(struct ade7816_desc *desc,
			       enum ade7816_channel chan, int32_t *val)
{
	uint32_t reg_val;
	int ret;

	if (desc->lcycle_mode) {
		ret = ade7816_reg_update(desc, ADE7816_LCYCMODE_REG,
					 ADE7816_RSTREAD_MASK,
					 no_os_field_prep(ADE7816_RSTREAD_MASK, 0));
		if (ret)
			return ret;
	}

	switch (chan) {
	case ADE7816_CHANNEL_A:
		ret = ade7816_read_reg(desc, ADE7816_AWATTHR_REG, &reg_val);
		if (ret)
			return ret;

		break;
	case ADE7816_CHANNEL_B:
		ret = ade7816_read_reg(desc, ADE7816_BWATTHR_REG, &reg_val);
		if (ret)
			return ret;

		break;
	case ADE7816_CHANNEL_C:
		ret = ade7816_read_reg(desc, ADE7816_CWATTHR_REG, &reg_val);
		if (ret)
			return ret;

		break;
	case ADE7816_CHANNEL_D:
		ret = ade7816_read_reg(desc, ADE7816_DWATTHR_REG, &reg_val);
		if (ret)
			return ret;

		break;
	case ADE7816_CHANNEL_E:
		ret = ade7816_read_reg(desc, ADE7816_EWATTHR_REG, &reg_val);
		if (ret)
			return ret;

		break;
	case ADE7816_CHANNEL_F:
		ret = ade7816_read_reg(desc, ADE7816_FWATTHR_REG, &reg_val);
		if (ret)
			return ret;

		break;
	default:
		return -EINVAL;
	}

	*val = no_os_sign_extend32(reg_val, 30);

	return 0;
}

int ade7816_set_active_thr(struct ade7816_desc *desc, uint16_t freq)
{
	uint32_t msb, lsb, tmp;
	int ret;

	if (freq > 8000)
		return -EINVAL;

	tmp = (0x2000000 / freq) * 8000;

	msb = no_os_field_get(ADE7816_THR_MSB_MASK, tmp);
	lsb = no_os_field_get(ADE7816_THR_LSB_MASK, tmp);

	ret = ade7816_write_reg(desc, ADE7816_WTHR1_REG, msb);
	if (ret)
		return ret;

	return ade7816_write_reg(desc, ADE7816_WTHR0_REG, lsb);
}

int ade7816_get_active_thr(struct ade7816_desc *desc, uint16_t *freq)
{
	uint32_t msb, lsb, tmp;
	int ret;

	ret = ade7816_read_reg(desc, ADE7816_WTHR1_REG, &msb);
	if (ret)
		return ret;

	ret = ade7816_read_reg(desc, ADE7816_WTHR0_REG, &lsb);
	if (ret)
		return ret;

	tmp = no_os_field_prep(ADE7816_THR_MSB_MASK, msb) |
	      no_os_field_prep(ADE7816_THR_LSB_MASK, lsb);

	*freq = (0x2000000 / tmp) * 8000;

	return 0;
}

int ade7816_read_reactive_energy(struct ade7816_desc *desc,
				 enum ade7816_channel chan, int32_t *val)
{
	uint32_t reg_val;
	int ret;

	if (desc->lcycle_mode) {
		ret = ade7816_reg_update(desc, ADE7816_LCYCMODE_REG,
					 ADE7816_RSTREAD_MASK,
					 no_os_field_prep(ADE7816_RSTREAD_MASK, 0));
		if (ret)
			return ret;
	}

	switch (chan) {
	case ADE7816_CHANNEL_A:
		ret = ade7816_read_reg(desc, ADE7816_AVARHR_REG, &reg_val);
		if (ret)
			return ret;

		break;
	case ADE7816_CHANNEL_B:
		ret = ade7816_read_reg(desc, ADE7816_BVARHR_REG, &reg_val);
		if (ret)
			return ret;

		break;
	case ADE7816_CHANNEL_C:
		ret = ade7816_read_reg(desc, ADE7816_CVARHR_REG, &reg_val);
		if (ret)
			return ret;

		break;
	case ADE7816_CHANNEL_D:
		ret = ade7816_read_reg(desc, ADE7816_DVARHR_REG, &reg_val);
		if (ret)
			return ret;

		break;
	case ADE7816_CHANNEL_E:
		ret = ade7816_read_reg(desc, ADE7816_EVARHR_REG, &reg_val);
		if (ret)
			return ret;

		break;
	case ADE7816_CHANNEL_F:
		ret = ade7816_read_reg(desc, ADE7816_FVARHR_REG, &reg_val);
		if (ret)
			return ret;

		break;
	default:
		return -EINVAL;
	}

	*val = no_os_sign_extend32(reg_val, 30);

	return 0;
}

int ade7816_set_reactive_thr(struct ade7816_desc *desc, uint16_t freq)
{
	uint32_t msb, lsb, tmp;
	int ret;

	if (freq > 8000)
		return -EINVAL;

	tmp = (0x2000000 / freq) * 8000;

	msb = no_os_field_get(ADE7816_THR_MSB_MASK, tmp);
	lsb = no_os_field_get(ADE7816_THR_LSB_MASK, tmp);

	ret = ade7816_write_reg(desc, ADE7816_VARTHR1_REG, msb);
	if (ret)
		return ret;

	return ade7816_write_reg(desc, ADE7816_VARTHR0_REG, lsb);
}

int ade7816_get_reactive_thr(struct ade7816_desc *desc, uint16_t *freq)
{
	uint32_t msb, lsb, tmp;
	int ret;

	ret = ade7816_read_reg(desc, ADE7816_VARTHR1_REG, &msb);
	if (ret)
		return ret;

	ret = ade7816_read_reg(desc, ADE7816_VARTHR0_REG, &lsb);
	if (ret)
		return ret;

	tmp = no_os_field_prep(ADE7816_THR_MSB_MASK, msb) |
	      no_os_field_prep(ADE7816_THR_LSB_MASK, lsb);

	*freq = (0x2000000 / tmp) * 8000;

	return 0;
}

int ade7816_set_lcycle_mode(struct ade7816_desc *desc, bool enable,
			    uint16_t cycles, bool lenergy)
{
	int ret;

	if (enable) {
		ret = ade7816_reg_update(desc, ADE7816_LCYCMODE_REG,
					 ADE7816_RSTREAD_MASK,
					 no_os_field_prep(ADE7816_RSTREAD_MASK, 0));
		if (ret)
			return ret;
	}

	ret = ade7816_reg_update(desc, ADE7816_LCYCMODE_REG, ADE7816_ZX_SEL_MASK,
				 no_os_field_prep(ADE7816_ZX_SEL_MASK, enable));
	if (ret)
		return ret;

	ret = ade7816_reg_update(desc, ADE7816_LCYCMODE_REG, ADE7816_LMASK,
				 no_os_field_prep(ADE7816_LMASK, ADE7816_LINECYC_VAL(enable)));
	if (ret)
		return ret;

	ret = ade7816_write_reg(desc, ADE7816_LINECYC_REG, enable ? cycles : 0);
	if (ret)
		return ret;

	return ade7816_reg_update(desc, ADE7816_MASK0_REG, ADE7816_LENERGY_MASK,
				  no_os_field_prep(ADE7816_LENERGY_MASK, lenergy));
}

int ade7816_read_rms(struct ade7816_desc *desc, enum ade7816_channel chan,
		     uint32_t *rms)
{
	switch (chan) {
	case ADE7816_CHANNEL_A:
		return ade7816_read_reg(desc, ADE7816_IARMS_REG, rms);
	case ADE7816_CHANNEL_B:
		return ade7816_read_reg(desc, ADE7816_IBRMS_REG, rms);
	case ADE7816_CHANNEL_C:
		return ade7816_read_reg(desc, ADE7816_ICRMS_REG, rms);
	case ADE7816_CHANNEL_D:
		return ade7816_read_reg(desc, ADE7816_IDRMS_REG, rms);
	case ADE7816_CHANNEL_E:
		return ade7816_read_reg(desc, ADE7816_IERMS_REG, rms);
	case ADE7816_CHANNEL_F:
		return ade7816_read_reg(desc, ADE7816_IFRMS_REG, rms);
	case ADE7816_CHANNEL_VOLTAGE:
		return ade7816_read_reg(desc, ADE7816_VRMS_REG, rms);
	default:
		return -EINVAL;
	}
}

int ade7816_set_no_load(struct ade7816_desc *desc, uint16_t voltage,
			uint16_t current, bool enable)
{
	int32_t apnoload;
	int ret;

	if (voltage > 10000 || current > 10000)
		return -EINVAL;

	if (enable)
		apnoload = NO_OS_DIV_ROUND_CLOSEST(0x1FF6A6B * voltage * current, MICRO * 100);
	else
		apnoload = -1;

	ret = ade7816_write_reg(desc, ADE7816_APNOLOAD_REG, apnoload);
	if (ret)
		return ret;

	return ade7816_write_reg(desc, ADE7816_VARNOLOAD_REG, apnoload);
}

int ade7816_set_gain(struct ade7816_desc *desc, enum ade7816_channel chan,
		     int16_t scale, enum ade7816_gain_type gain)
{
	int32_t gain_val;
	uint16_t reg;

	if (scale > 10000 || scale < -10000)
		return -EINVAL;

	gain_val = NO_OS_DIV_ROUND_CLOSEST_ULL(0x7FFFFF * scale, MILLI * 10);

	switch (chan) {
	case ADE7816_CHANNEL_A:
		switch (gain) {
		case ADE7816_CURRENT_GAIN:
			reg = ADE7816_IAGAIN_REG;
			break;
		case ADE7816_ACTIVE_POWER_GAIN:
			reg = ADE7816_AWGAIN_REG;
			break;
		case ADE7816_REACTIVE_POWER_GAIN:
			reg = ADE7816_AVARGAIN_REG;
			break;
		default:
			return -EINVAL;
		}
		break;
	case ADE7816_CHANNEL_B:
		switch (gain) {
		case ADE7816_CURRENT_GAIN:
			reg = ADE7816_IBGAIN_REG;
			break;
		case ADE7816_ACTIVE_POWER_GAIN:
			reg = ADE7816_BWGAIN_REG;
			break;
		case ADE7816_REACTIVE_POWER_GAIN:
			reg = ADE7816_BVARGAIN_REG;
			break;
		default:
			return -EINVAL;
		}
		break;
	case ADE7816_CHANNEL_C:
		switch (gain) {
		case ADE7816_CURRENT_GAIN:
			reg = ADE7816_ICGAIN_REG;
			break;
		case ADE7816_ACTIVE_POWER_GAIN:
			reg = ADE7816_CWGAIN_REG;
			break;
		case ADE7816_REACTIVE_POWER_GAIN:
			reg = ADE7816_CVARGAIN_REG;
			break;
		default:
			return -EINVAL;
		}
		break;
	case ADE7816_CHANNEL_D:
		switch (gain) {
		case ADE7816_CURRENT_GAIN:
			reg = ADE7816_IDGAIN_REG;
			break;
		case ADE7816_ACTIVE_POWER_GAIN:
			reg = ADE7816_DWGAIN_REG;
			break;
		case ADE7816_REACTIVE_POWER_GAIN:
			reg = ADE7816_DVARGAIN_REG;
			break;
		default:
			return -EINVAL;
		}
		break;
	case ADE7816_CHANNEL_E:
		switch (gain) {
		case ADE7816_CURRENT_GAIN:
			reg = ADE7816_IEGAIN_REG;
			break;
		case ADE7816_ACTIVE_POWER_GAIN:
			reg = ADE7816_EWGAIN_REG;
			break;
		case ADE7816_REACTIVE_POWER_GAIN:
			reg = ADE7816_EVARGAIN_REG;
			break;
		default:
			return -EINVAL;
		}
		break;
	case ADE7816_CHANNEL_F:
		switch (gain) {
		case ADE7816_CURRENT_GAIN:
			reg = ADE7816_IFGAIN_REG;
			break;
		case ADE7816_ACTIVE_POWER_GAIN:
			reg = ADE7816_FWGAIN_REG;
			break;
		case ADE7816_REACTIVE_POWER_GAIN:
			reg = ADE7816_FVARGAIN_REG;
			break;
		default:
			return -EINVAL;
		}
		break;
	case ADE7816_CHANNEL_VOLTAGE:
		reg = ADE7816_VGAIN_REG;
		break;
	default:
		return -EINVAL;
	}

	return ade7816_write_reg(desc, reg, gain_val);
}

int ade7816_get_gain(struct ade7816_desc *desc, enum ade7816_channel chan,
		     int16_t *scale, enum ade7816_gain_type gain)
{
	uint32_t reg_val;
	int32_t gain_val;
	uint16_t reg;
	int ret;

	switch (chan) {
	case ADE7816_CHANNEL_A:
		switch (gain) {
		case ADE7816_CURRENT_GAIN:
			reg = ADE7816_IAGAIN_REG;
			break;
		case ADE7816_ACTIVE_POWER_GAIN:
			reg = ADE7816_AWGAIN_REG;
			break;
		case ADE7816_REACTIVE_POWER_GAIN:
			reg = ADE7816_AVARGAIN_REG;
			break;
		default:
			return -EINVAL;
		}
		break;
	case ADE7816_CHANNEL_B:
		switch (gain) {
		case ADE7816_CURRENT_GAIN:
			reg = ADE7816_IBGAIN_REG;
			break;
		case ADE7816_ACTIVE_POWER_GAIN:
			reg = ADE7816_BWGAIN_REG;
			break;
		case ADE7816_REACTIVE_POWER_GAIN:
			reg = ADE7816_BVARGAIN_REG;
			break;
		default:
			return -EINVAL;
		}
		break;
	case ADE7816_CHANNEL_C:
		switch (gain) {
		case ADE7816_CURRENT_GAIN:
			reg = ADE7816_ICGAIN_REG;
			break;
		case ADE7816_ACTIVE_POWER_GAIN:
			reg = ADE7816_CWGAIN_REG;
			break;
		case ADE7816_REACTIVE_POWER_GAIN:
			reg = ADE7816_CVARGAIN_REG;
			break;
		default:
			return -EINVAL;
		}
		break;
	case ADE7816_CHANNEL_D:
		switch (gain) {
		case ADE7816_CURRENT_GAIN:
			reg = ADE7816_IDGAIN_REG;
			break;
		case ADE7816_ACTIVE_POWER_GAIN:
			reg = ADE7816_DWGAIN_REG;
			break;
		case ADE7816_REACTIVE_POWER_GAIN:
			reg = ADE7816_DVARGAIN_REG;
			break;
		default:
			return -EINVAL;
		}
		break;
	case ADE7816_CHANNEL_E:
		switch (gain) {
		case ADE7816_CURRENT_GAIN:
			reg = ADE7816_IEGAIN_REG;
			break;
		case ADE7816_ACTIVE_POWER_GAIN:
			reg = ADE7816_EWGAIN_REG;
			break;
		case ADE7816_REACTIVE_POWER_GAIN:
			reg = ADE7816_EVARGAIN_REG;
			break;
		default:
			return -EINVAL;
		}
		break;
	case ADE7816_CHANNEL_F:
		switch (gain) {
		case ADE7816_CURRENT_GAIN:
			reg = ADE7816_IFGAIN_REG;
			break;
		case ADE7816_ACTIVE_POWER_GAIN:
			reg = ADE7816_FWGAIN_REG;
			break;
		case ADE7816_REACTIVE_POWER_GAIN:
			reg = ADE7816_FVARGAIN_REG;
			break;
		default:
			return -EINVAL;
		}
		break;
	case ADE7816_CHANNEL_VOLTAGE:
		reg = ADE7816_VGAIN_REG;
		break;
	default:
		return -EINVAL;
	}

	ret = ade7816_read_reg(desc, reg, &reg_val);
	if (ret)
		return ret;

	gain_val = reg_val;

	*scale = NO_OS_DIV_ROUND_CLOSEST(gain_val * MILLI * 10, 0x7FFFFF);

	return 0;
}

int ade7816_set_offset(struct ade7816_desc *desc, enum ade7816_channel chan,
		       int16_t scale, enum ade7816_gain_type gain)
{
	int32_t offset;
	uint16_t reg;

	if (scale > 10000 || scale < -10000)
		return -EINVAL;

	offset = NO_OS_DIV_ROUND_CLOSEST_ULL(scale * MILLI * 10, 298);

	switch (chan) {
	case ADE7816_CHANNEL_A:
		switch (gain) {
		case ADE7816_ACTIVE_POWER_GAIN:
			reg = ADE7816_AWATTOS_REG;
			break;
		case ADE7816_REACTIVE_POWER_GAIN:
			reg = ADE7816_AVAROS_REG;
			break;
		default:
			return -EINVAL;
		}
		break;
	case ADE7816_CHANNEL_B:
		switch (gain) {
		case ADE7816_ACTIVE_POWER_GAIN:
			reg = ADE7816_BWATTOS_REG;
			break;
		case ADE7816_REACTIVE_POWER_GAIN:
			reg = ADE7816_BVAROS_REG;
			break;
		default:
			return -EINVAL;
		}
		break;
	case ADE7816_CHANNEL_C:
		switch (gain) {
		case ADE7816_ACTIVE_POWER_GAIN:
			reg = ADE7816_CWATTOS_REG;
			break;
		case ADE7816_REACTIVE_POWER_GAIN:
			reg = ADE7816_CVAROS_REG;
			break;
		default:
			return -EINVAL;
		}
		break;
	case ADE7816_CHANNEL_D:
		switch (gain) {
		case ADE7816_ACTIVE_POWER_GAIN:
			reg = ADE7816_DWATTOS_REG;
			break;
		case ADE7816_REACTIVE_POWER_GAIN:
			reg = ADE7816_DVAROS_REG;
			break;
		default:
			return -EINVAL;
		}
		break;
	case ADE7816_CHANNEL_E:
		switch (gain) {
		case ADE7816_ACTIVE_POWER_GAIN:
			reg = ADE7816_EWATTOS_REG;
			break;
		case ADE7816_REACTIVE_POWER_GAIN:
			reg = ADE7816_EVAROS_REG;
			break;
		default:
			return -EINVAL;
		}
		break;
	case ADE7816_CHANNEL_F:
		switch (gain) {
		case ADE7816_ACTIVE_POWER_GAIN:
			reg = ADE7816_FWATTOS_REG;
			break;
		case ADE7816_REACTIVE_POWER_GAIN:
			reg = ADE7816_FVAROS_REG;
			break;
		default:
			return -EINVAL;
		}
		break;
	default:
		return -EINVAL;
	}

	return ade7816_write_reg(desc, reg, offset);
}

int ade7816_get_offset(struct ade7816_desc *desc, enum ade7816_channel chan,
		       int16_t *scale, enum ade7816_gain_type gain)
{
	uint32_t reg_val;
	int32_t offset;
	uint16_t reg;
	int ret;

	switch (chan) {
	case ADE7816_CHANNEL_A:
		switch (gain) {
		case ADE7816_ACTIVE_POWER_GAIN:
			reg = ADE7816_AWATTOS_REG;
			break;
		case ADE7816_REACTIVE_POWER_GAIN:
			reg = ADE7816_AVAROS_REG;
			break;
		default:
			return -EINVAL;
		}
		break;
	case ADE7816_CHANNEL_B:
		switch (gain) {
		case ADE7816_ACTIVE_POWER_GAIN:
			reg = ADE7816_BWATTOS_REG;
			break;
		case ADE7816_REACTIVE_POWER_GAIN:
			reg = ADE7816_BVAROS_REG;
			break;
		default:
			return -EINVAL;
		}
		break;
	case ADE7816_CHANNEL_C:
		switch (gain) {
		case ADE7816_ACTIVE_POWER_GAIN:
			reg = ADE7816_CWATTOS_REG;
			break;
		case ADE7816_REACTIVE_POWER_GAIN:
			reg = ADE7816_CVAROS_REG;
			break;
		default:
			return -EINVAL;
		}
		break;
	case ADE7816_CHANNEL_D:
		switch (gain) {
		case ADE7816_ACTIVE_POWER_GAIN:
			reg = ADE7816_DWATTOS_REG;
			break;
		case ADE7816_REACTIVE_POWER_GAIN:
			reg = ADE7816_DVAROS_REG;
			break;
		default:
			return -EINVAL;
		}
		break;
	case ADE7816_CHANNEL_E:
		switch (gain) {
		case ADE7816_ACTIVE_POWER_GAIN:
			reg = ADE7816_EWATTOS_REG;
			break;
		case ADE7816_REACTIVE_POWER_GAIN:
			reg = ADE7816_EVAROS_REG;
			break;
		default:
			return -EINVAL;
		}
		break;
	case ADE7816_CHANNEL_F:
		switch (gain) {
		case ADE7816_ACTIVE_POWER_GAIN:
			reg = ADE7816_FWATTOS_REG;
			break;
		case ADE7816_REACTIVE_POWER_GAIN:
			reg = ADE7816_FVAROS_REG;
			break;
		default:
			return -EINVAL;
		}
		break;
	default:
		return -EINVAL;
	}

	ret = ade7816_read_reg(desc, reg, &reg_val);
	if (ret)
		return ret;

	offset = reg_val;

	*scale = NO_OS_DIV_ROUND_CLOSEST_ULL(offset * 298, MILLI * 10);

	return 0;
}

int ade7816_set_phase(struct ade7816_desc *desc, enum ade7816_channel chan,
		      enum ade7816_pcf_coeff pcf_coeff)
{
	switch (chan) {
	case ADE7816_CHANNEL_A:
		return ade7816_write_reg(desc, ADE7816_PCF_A_COEFF_REG, pcf_coeff);
	case ADE7816_CHANNEL_B:
		return ade7816_write_reg(desc, ADE7816_PCF_B_COEFF_REG, pcf_coeff);
	case ADE7816_CHANNEL_C:
		return ade7816_write_reg(desc, ADE7816_PCF_C_COEFF_REG, pcf_coeff);
	case ADE7816_CHANNEL_D:
		return ade7816_write_reg(desc, ADE7816_PCF_D_COEFF_REG, pcf_coeff);
	case ADE7816_CHANNEL_E:
		return ade7816_write_reg(desc, ADE7816_PCF_E_COEFF_REG, pcf_coeff);
	case ADE7816_CHANNEL_F:
		return ade7816_write_reg(desc, ADE7816_PCF_F_COEFF_REG, pcf_coeff);
	default:
		return -EINVAL;
	}
}

int ade7816_calib_rms(struct ade7816_desc *desc, enum ade7816_channel chan,
		      int32_t rms)
{
	uint32_t reg_val;
	uint16_t reg;
	int ret;

	switch (chan) {
	case ADE7816_CHANNEL_VOLTAGE:
		reg = ADE7816_VRMSOS_REG;
		break;
	case ADE7816_CHANNEL_A:
		reg = ADE7816_IARMSOS_REG;
		break;
	case ADE7816_CHANNEL_B:
		reg = ADE7816_IBRMSOS_REG;
		break;
	case ADE7816_CHANNEL_C:
		reg = ADE7816_ICRMSOS_REG;
		break;
	case ADE7816_CHANNEL_D:
		reg = ADE7816_IDRMSOS_REG;
		break;
	case ADE7816_CHANNEL_E:
		reg = ADE7816_IERMSOS_REG;
		break;
	case ADE7816_CHANNEL_F:
		reg = ADE7816_IFRMSOS_REG;
		break;
	default:
		return -EINVAL;
	}

	ret = ade7816_write_reg(desc, reg, 0);
	if (ret)
		return ret;

	ret = ade7816_read_rms(desc, chan, &reg_val);
	if (ret)
		return ret;

	return ade7816_write_reg(desc, reg,
				 NO_OS_DIV_ROUND_CLOSEST_ULL(rms*rms - reg_val * reg_val, 128));
}

int ade7816_group_sel(struct ade7816_desc *desc, enum ade7816_channel chan)
{
	switch (chan) {
	case ADE7816_CHANNEL_A:
	case ADE7816_CHANNEL_B:
	case ADE7816_CHANNEL_C:
		return ade7816_reg_update(desc, ADE7816_COMPMODE_REG,
					  ADE7816_CHANNEL_SEL_MASK,
					  no_os_field_prep(ADE7816_CHANNEL_SEL_MASK, 0));
	case ADE7816_CHANNEL_D:
	case ADE7816_CHANNEL_E:
	case ADE7816_CHANNEL_F:
		return ade7816_reg_update(desc, ADE7816_COMPMODE_REG,
					  ADE7816_CHANNEL_SEL_MASK,
					  no_os_field_prep(ADE7816_CHANNEL_SEL_MASK, 1));
	default:
		return -EINVAL;
	}

	desc->chan = chan;
}

int ade7816_zx_detect(struct ade7816_desc *desc, enum ade7816_channel chan)
{
	int ret;

	ret = ade7816_group_sel(desc, chan);
	if (ret)
		return ret;

	/* Delay required for settling after changing channel selection for zx
	 * detection.
	 */
	no_os_mdelay(10);

	return 0;
}

int ade7816_zx_timeout(struct ade7816_desc *desc, uint32_t timeout_us)
{
	return ade7816_write_reg(desc, ADE7816_ZXTOUT_REG,
				 NO_OS_DIV_ROUND_CLOSEST_ULL(timeout_us * 10, 625));
}

int ade7816_peak_detect(struct ade7816_desc *desc, enum ade7816_channel chan,
			enum ade7816_mmode_sel mmode, uint8_t no_of_cycles)
{
	int ret;

	ret = ade7816_group_sel(desc, chan);
	if (ret)
		return ret;

	ret = ade7816_write_reg(desc, ADE7816_MMODE_REG,
				no_os_field_prep(ADE7816_MMODE_MASK, mmode));
	if (ret)
		return ret;

	return ade7816_write_reg(desc, ADE7816_PEAKCYC_REG, no_of_cycles);
}

int ade7816_power_dir(struct ade7816_desc *desc, enum ade7816_channel chan)
{
	int ret;

	ret = ade7816_group_sel(desc, chan);
	if (ret)
		return ret;
	switch (chan) {
	case ADE7816_CHANNEL_A:
	case ADE7816_CHANNEL_B:
	case ADE7816_CHANNEL_C:
		return ade7816_reg_update(desc, ADE7816_ACCMODE_REG, ADE7816_REVSEL_MASK,
					  no_os_field_prep(ADE7816_REVSEL_MASK, 0));
	case ADE7816_CHANNEL_D:
	case ADE7816_CHANNEL_E:
	case ADE7816_CHANNEL_F:
		return ade7816_reg_update(desc, ADE7816_ACCMODE_REG, ADE7816_REVSEL_MASK,
					  no_os_field_prep(ADE7816_REVSEL_MASK, 0x11));
	default:
		return -EINVAL;
	}
}

int ade7816_read_dir(struct ade7816_desc *desc, enum ade7816_channel chan,
		     bool *sign)
{
	uint32_t reg_val;
	int ret;

	if (desc->chan != chan)
		return -EINVAL;

	ret = ade7816_read_reg(desc, ADE7816_CHSIGN_REG, &reg_val);
	if (ret)
		return ret;

	*sign = no_os_field_get(ADE7816_SIGN_MASK(chan), reg_val);

	return 0;
}

int ade7816_angle_meas(struct ade7816_desc *desc, enum ade7816_channel chan,
		       enum ade7816_angle_sel sel)
{
	int ret;

	ret = ade7816_group_sel(desc, chan);
	if (ret)
		return ret;

	return ade7816_reg_update(desc, ADE7816_COMPMODE_REG,
				  ADE7816_ANGLESEL_MASK,
				  no_os_field_prep(ADE7816_ANGLESEL_MASK, sel));
}

int ade7816_read_period(struct ade7816_desc *desc, uint32_t *period_us)
{
	uint32_t reg_val;
	int ret;

	/* Delay required for internal filtering. */
	no_os_mdelay(40);

	ret = ade7816_read_reg(desc, ADE7816_PERIOD_REG, &reg_val);
	if (ret)
		return ret;

	*period_us = NO_OS_DIV_ROUND_CLOSEST(no_os_field_get(ADE7816_FIRST_BYTE_MASK |
					     ADE7816_SECOND_BYTE_MASK, reg_val) + 1, 0x256E3);

	return 0;
}

int ade7816_set_interrupt(struct ade7816_desc *desc,
			  enum ade7816_status_int status_int, bool enable)
{
	uint16_t reg = ADE7816_MASK0_REG;

	if (status_int > 31) {
		status_int -= 32;
		reg = ADE7816_MASK1_REG;
	}

	return ade7816_reg_update(desc, reg, ADE7816_INT_MASK(status_int),
				  no_os_field_prep(ADE7816_INT_MASK(status_int), enable));
}

int ade7816_get_interrupt(struct ade7816_desc *desc,
			  enum ade7816_status_int status_int, bool *enable)
{
	uint32_t reg_val;
	int ret;

	ret = ade7816_read_reg(desc,
			       status_int > 31 ? ADE7816_MASK1_REG : ADE7816_MASK0_REG, &reg_val);
	if (ret)
		return ret;

	if (status_int > 31)
		status_int -= 32;

	*enable = no_os_field_get(ADE7816_INT_MASK(status_int), reg_val);

	return 0;
}

int ade7816_rms_to_micro(struct ade7816_desc *desc, enum ade7816_channel chan,
			 uint32_t rms, uint32_t *micro)
{
	uint32_t reg_val, reg;
	int ret;

	switch (chan) {
	case ADE7816_CHANNEL_A:
		reg = ADE7816_PCF_A_COEFF_REG;

		break;
	case ADE7816_CHANNEL_B:
		reg = ADE7816_PCF_B_COEFF_REG;

		break;
	case ADE7816_CHANNEL_C:
		reg = ADE7816_PCF_C_COEFF_REG;

		break;
	case ADE7816_CHANNEL_D:
		reg = ADE7816_PCF_D_COEFF_REG;

		break;
	case ADE7816_CHANNEL_E:
		reg = ADE7816_PCF_E_COEFF_REG;

		break;
	case ADE7816_CHANNEL_F:
		reg = ADE7816_PCF_F_COEFF_REG;

		break;
	case ADE7816_CHANNEL_VOLTAGE:
		reg_val = ADE7816_PCF_50HZ;

		goto exit;
	default:
		return -EINVAL;
	}

	ret = ade7816_read_reg(desc, reg, &reg_val);
	if (ret)
		return ret;

exit:
	rms /= 1000;
	rms *= 345;

	switch (reg_val) {
	case ADE7816_PCF_50HZ:
		*micro = NO_OS_DIV_ROUND_CLOSEST(rms * MILLI, 4192);

		return 0;
	case ADE7816_PCF_60HZ:
		*micro = NO_OS_DIV_ROUND_CLOSEST_ULL(rms * MILLI, 3493);

		return 0;
	default:
		return -EINVAL;
	}
}

static void ade7816_irq0_handler(void *dev)
{
	struct ade7816_desc *desc = dev;
	uint32_t reg_val;
	int ret;

	ret = ade7816_read_reg(desc, ADE7816_STATUS0_REG, &reg_val);

	desc->status0 = reg_val;

	ret = ade7816_write_reg(desc, ADE7816_STATUS0_REG, 0);
}

static void ade7816_irq1_handler(void *dev)
{
	struct ade7816_desc *desc = dev;
	uint32_t reg_val;
	int ret;

	ret = no_os_irq_disable(desc->irq_ctrl,
				desc->gpio_irq1_desc->number);

	ret = ade7816_read_reg(desc, ADE7816_STATUS1_REG, &reg_val);

	desc->status1 = reg_val;

	ret = ade7816_write_reg(desc, ADE7816_STATUS1_REG, 0);
	ret = no_os_irq_enable(desc->irq_ctrl, desc->irq_ctrl->irq_ctrl_id);
}

int ade7816_init(struct ade7816_desc **desc,
		 struct ade7816_init_param *init_param)
{
	struct no_os_callback_desc irq0_cb;
	struct no_os_callback_desc irq1_cb;
	struct ade7816_desc *descriptor;
	uint32_t reg_val;
	int ret;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	switch (init_param->active_irq) {
	case ADE7816_IRQ0:
		if (!init_param->irq_ctrl)
			goto remove_ade7816;

		irq0_cb.callback = &ade7816_irq0_handler;
		irq0_cb.ctx = descriptor;
		irq0_cb.event = NO_OS_EVT_GPIO;
		irq0_cb.peripheral = NO_OS_GPIO_IRQ;
		irq0_cb.handle = init_param->handle_irq0;

		if (init_param->irq0_callback)
			irq0_cb.callback = init_param->irq0_callback;

		ret = no_os_gpio_get_optional(&descriptor->gpio_irq0_desc,
					      init_param->gpio_irq0_param);
		if (ret)
			goto remove_ade7816;

		ret = no_os_gpio_direction_input(descriptor->gpio_irq0_desc);
		if (ret)
			goto remove_irq0;

		ret = no_os_gpio_get_optional(&descriptor->gpio_irq1_desc,
					      init_param->gpio_irq1_param);
		if (ret)
			goto remove_irq0;

		if (descriptor->gpio_irq1_desc) {
			ret = no_os_gpio_direction_input(descriptor->gpio_irq1_desc);
			if (ret)
				goto remove_irq1;
		}

		ret = no_os_irq_register_callback(init_param->irq_ctrl,
						  descriptor->gpio_irq0_desc->number, &irq0_cb);
		if (ret)
			goto remove_irq1;

		descriptor->irq0_cb = irq0_cb;

		ret = no_os_irq_trigger_level_set(init_param->irq_ctrl,
						  descriptor->gpio_irq0_desc->number, NO_OS_IRQ_EDGE_FALLING);
		if (ret)
			goto remove_irq0_cb;

		ret = no_os_irq_set_priority(init_param->irq_ctrl,
					     descriptor->gpio_irq0_desc->number, init_param->irq0_priority);
		if (ret)
			goto remove_irq0_cb;

		ret = no_os_irq_disable(init_param->irq_ctrl,
					descriptor->gpio_irq0_desc->number);
		if (ret)
			goto remove_irq0_cb;

		break;
	case ADE7816_IRQ1:
		if (!init_param->irq_ctrl)
			goto remove_ade7816;

		irq1_cb.callback = &ade7816_irq1_handler;
		irq1_cb.ctx = descriptor;
		irq1_cb.event = NO_OS_EVT_GPIO;
		irq1_cb.peripheral = NO_OS_GPIO_IRQ;
		irq0_cb.handle = init_param->handle_irq1;

		if (init_param->irq1_callback)
			irq1_cb.callback = init_param->irq1_callback;

		ret = no_os_gpio_get_optional(&descriptor->gpio_irq0_desc,
					      init_param->gpio_irq0_param);
		if (ret)
			goto remove_ade7816;

		if (descriptor->gpio_irq0_desc) {
			ret = no_os_gpio_direction_input(descriptor->gpio_irq0_desc);
			if (ret)
				goto remove_irq0;
		}

		ret = no_os_gpio_get(&descriptor->gpio_irq1_desc,
				     init_param->gpio_irq1_param);
		if (ret)
			goto remove_irq0;

		ret = no_os_gpio_direction_input(descriptor->gpio_irq1_desc);
		if (ret)
			goto remove_irq1;

		ret = no_os_irq_register_callback(init_param->irq_ctrl,
						  descriptor->gpio_irq1_desc->number, &irq1_cb);
		if (ret)
			goto remove_irq1;

		descriptor->irq1_cb = irq1_cb;

		ret = no_os_irq_trigger_level_set(init_param->irq_ctrl,
						  descriptor->gpio_irq1_desc->number, NO_OS_IRQ_EDGE_FALLING);
		if (ret)
			goto remove_irq1_cb;

		ret = no_os_irq_set_priority(init_param->irq_ctrl,
					     descriptor->gpio_irq1_desc->number, init_param->irq1_priority);
		if (ret)
			goto remove_irq1_cb;

		ret = no_os_irq_disable(init_param->irq_ctrl,
					descriptor->gpio_irq1_desc->number);
		if (ret)
			goto remove_irq1_cb;

		break;
	case ADE7816_IRQ0_IRQ1:
		if (!init_param->irq_ctrl)
			goto remove_ade7816;

		irq0_cb.callback = ade7816_irq0_handler;
		irq0_cb.ctx = descriptor;
		irq0_cb.event = NO_OS_EVT_GPIO;
		irq0_cb.peripheral = NO_OS_GPIO_IRQ;
		irq0_cb.handle = init_param->handle_irq0;

		if (init_param->irq0_callback)
			irq0_cb.callback = init_param->irq0_callback;

		irq1_cb.callback = ade7816_irq1_handler;
		irq1_cb.ctx = descriptor;
		irq1_cb.event = NO_OS_EVT_GPIO;
		irq1_cb.peripheral = NO_OS_GPIO_IRQ;
		irq0_cb.handle = init_param->handle_irq1;

		if (init_param->irq1_callback)
			irq1_cb.callback = init_param->irq1_callback;

		ret = no_os_gpio_get(&descriptor->gpio_irq0_desc,
				     init_param->gpio_irq0_param);
		if (ret)
			goto remove_ade7816;

		ret = no_os_gpio_direction_input(descriptor->gpio_irq0_desc);
		if (ret)
			goto remove_irq0;

		ret = no_os_gpio_get(&descriptor->gpio_irq1_desc,
				     init_param->gpio_irq1_param);
		if (ret)
			goto remove_irq0;

		ret = no_os_gpio_direction_input(descriptor->gpio_irq1_desc);
		if (ret)
			goto remove_irq1;

		ret = no_os_irq_register_callback(init_param->irq_ctrl,
						  descriptor->gpio_irq0_desc->number, &irq0_cb);
		if (ret)
			goto remove_irq1;

		descriptor->irq0_cb = irq0_cb;

		ret = no_os_irq_register_callback(init_param->irq_ctrl,
						  descriptor->gpio_irq1_desc->number, &irq1_cb);
		if (ret)
			goto remove_irq0_cb;

		descriptor->irq1_cb = irq1_cb;


		ret = no_os_irq_trigger_level_set(init_param->irq_ctrl,
						  descriptor->gpio_irq0_desc->number, NO_OS_IRQ_EDGE_FALLING);
		if (ret)
			goto remove_irq_cb;

		ret = no_os_irq_set_priority(init_param->irq_ctrl,
					     descriptor->gpio_irq0_desc->number, init_param->irq0_priority);
		if (ret)
			goto remove_irq_cb;

		ret = no_os_irq_disable(init_param->irq_ctrl,
					descriptor->gpio_irq0_desc->number);
		if (ret)
			goto remove_irq_cb;

		ret = no_os_irq_trigger_level_set(init_param->irq_ctrl,
						  descriptor->gpio_irq1_desc->number, NO_OS_IRQ_EDGE_FALLING);
		if (ret)
			goto remove_irq_cb;

		ret = no_os_irq_set_priority(init_param->irq_ctrl,
					     descriptor->gpio_irq1_desc->number, init_param->irq1_priority);
		if (ret)
			goto remove_irq_cb;

		ret = no_os_irq_disable(init_param->irq_ctrl,
					descriptor->gpio_irq1_desc->number);
		if (ret)
			goto remove_irq_cb;

		break;
	case ADE7816_NO_IRQ:
		ret = no_os_gpio_get_optional(&descriptor->gpio_irq0_desc,
					      init_param->gpio_irq0_param);
		if (ret)
			goto remove_ade7816;

		ret = no_os_gpio_direction_input(descriptor->gpio_irq0_desc);
		if (ret)
			goto remove_irq0;

		ret = no_os_gpio_get_optional(&descriptor->gpio_irq1_desc,
					      init_param->gpio_irq1_param);
		if (ret)
			goto remove_irq0;

		ret = no_os_gpio_direction_input(descriptor->gpio_irq1_desc);
		if (ret)
			goto remove_irq1;

		break;
	default:
		return -EINVAL;
	}
	descriptor->irq_ctrl = init_param->irq_ctrl;
	descriptor->active_irq = init_param->active_irq;

	ret = no_os_gpio_get_optional(&descriptor->reset_desc,
				      init_param->reset_param);
	if (ret)
		goto remove_irq_all;

	if (descriptor->reset_desc) {
		ret = no_os_gpio_direction_output(descriptor->reset_desc,
						  NO_OS_GPIO_HIGH);
		if (ret)
			goto remove_reset;
	}

	ret = no_os_gpio_get_optional(&descriptor->ss_desc,
				      init_param->ss_param);
	if (ret)
		goto remove_reset;

	if (descriptor->ss_desc) {
		ret = no_os_gpio_direction_output(descriptor->ss_desc,
						  NO_OS_GPIO_LOW);
		if (ret)
			goto remove_ss;
	}

	switch (init_param->comm_type) {
	case ADE7816_SPI:
		descriptor->i2c_desc = NULL;

		ret = no_os_spi_init(&descriptor->spi_desc,
				     init_param->spi_param);
		if (ret)
			goto remove_ss;

		ret = ade7816_set_comm(descriptor, ADE7816_SPI);
		if (ret)
			goto remove_spi;

		break;
	case ADE7816_I2C:
		descriptor->spi_desc = NULL;

		ret = no_os_i2c_init(&descriptor->i2c_desc,
				     init_param->i2c_param);
		if (ret)
			goto remove_ss;

		ret = ade7816_set_comm(descriptor, ADE7816_I2C);
		if (ret)
			goto remove_i2c;

		break;
	default:
		ret = -EINVAL;
		goto remove_reset;
	}

	no_os_mdelay(ADE7816_INIT_DELAY);

	ret = ade7816_write_reg(descriptor, ADE7816_RUN_REG, 0x01);
	if (ret)
		goto remove_comm;

	*desc = descriptor;

	return 0;

remove_comm:
	no_os_spi_remove(descriptor->spi_desc);
	no_os_i2c_remove(descriptor->i2c_desc);

	goto remove_ss;
remove_i2c:
	no_os_i2c_remove(descriptor->i2c_desc);

	goto remove_ss;
remove_spi:
	no_os_spi_remove(descriptor->spi_desc);
remove_ss:
	no_os_gpio_remove(descriptor->ss_desc);
remove_reset:
	no_os_gpio_remove(descriptor->reset_desc);
remove_irq_all:
	switch (init_param->active_irq) {
	case ADE7816_NO_IRQ:
		goto remove_ade7816;
	case ADE7816_IRQ0:
		goto remove_irq1_cb;
	case ADE7816_IRQ1:
		no_os_irq_unregister_callback(init_param->irq_ctrl,
					      descriptor->gpio_irq1_desc->number, &irq1_cb);
		goto remove_irq0_cb;
	case ADE7816_IRQ0_IRQ1:
		break;
	}
remove_irq_cb:
	no_os_irq_unregister_callback(init_param->irq_ctrl,
				      descriptor->gpio_irq0_desc->number, &irq0_cb);
remove_irq1_cb:
	no_os_irq_unregister_callback(init_param->irq_ctrl,
				      descriptor->gpio_irq1_desc->number, &irq1_cb);

	goto remove_irq1;
remove_irq0_cb:
	no_os_irq_unregister_callback(init_param->irq_ctrl,
				      descriptor->gpio_irq0_desc->number, &irq0_cb);
remove_irq1:
	no_os_gpio_remove(descriptor->gpio_irq1_desc);
remove_irq0:
	no_os_gpio_remove(descriptor->gpio_irq0_desc);

	return ret;
remove_ade7816:
	no_os_free(descriptor);

	return ret;
}

int ade7816_remove(struct ade7816_desc *desc)
{
	if (!desc)
		return -ENODEV;

	no_os_spi_remove(desc->spi_desc);
	no_os_i2c_remove(desc->i2c_desc);
	no_os_gpio_remove(desc->ss_desc);
	no_os_gpio_remove(desc->reset_desc);
	switch (desc->active_irq) {
	case ADE7816_NO_IRQ:
		break;
	case ADE7816_IRQ0:
		no_os_irq_unregister_callback(desc->irq_ctrl, desc->gpio_irq0_desc->number,
					      &desc->irq1_cb);

		break;
	case ADE7816_IRQ1:
		no_os_irq_unregister_callback(desc->irq_ctrl, desc->gpio_irq1_desc->number,
					      &desc->irq1_cb);

		break;
	case ADE7816_IRQ0_IRQ1:
		no_os_irq_unregister_callback(desc->irq_ctrl, desc->gpio_irq0_desc->number,
					      &desc->irq1_cb);
		no_os_irq_unregister_callback(desc->irq_ctrl, desc->gpio_irq1_desc->number,
					      &desc->irq1_cb);

		break;
	default:
		break;
	}
	no_os_gpio_remove(desc->gpio_irq1_desc);
	no_os_gpio_remove(desc->gpio_irq0_desc);
	no_os_free(desc);

	return 0;
}
