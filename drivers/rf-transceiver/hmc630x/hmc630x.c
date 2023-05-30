/***************************************************************************//**
 *   @file   hmc630x.c
 *   @brief  hmc6300 and hmc6301 device driver implementation.
 *   @author Darius Berghe (darius.berghe@analog.com)
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
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include <errno.h>
#include <stdlib.h>
#include "hmc630x.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"

#define HMC630X_ARRAY_ADDRESS_MASK NO_OS_GENMASK(13, 8)
#define HMC630X_RW_MASK NO_OS_BIT(14)
#define HMC630X_ADDRESS_MASK NO_OS_GENMASK(17, 15)
#define HMC6300_ADDRESS 0x6
#define HMC6301_ADDRESS 0x7
#define HMC630X_FRAME_SIZE 18

#define HMC6300_BITBANG_DELAY_US 1
#define HMC6300_SETTLING_DELAY_MS 1

/* VCO frequencies computed by hmc6300_init() based on reference clock. */
struct hmc630x_vco {
	uint64_t *freqs;
	uint8_t *fbdiv;
	uint8_t entries;
};

/* Device descriptor created by hmc6300_init() and used by the rest of the driver API. */
struct hmc630x_dev {
	enum hmc630x_type type;
	uint8_t address;
	struct hmc630x_vco vco;
	struct no_os_gpio_desc *en;
	struct no_os_gpio_desc *clk;
	struct no_os_gpio_desc *data;
	struct no_os_gpio_desc *scanout;
};

/* Default values for registers as listed in datasheet, written to device at startup. */
static const uint8_t hmc6300_default_regmap[] = {
	0x00, 0x4a, 0xf6, 0xf6, 0x00, 0xbf, 0x6c, 0x0f,
	0x8f, 0x00, 0x53, 0x03, 0x00, 0x00, 0x00, 0x00,
	0x36, 0xbb, 0x46, 0x02, 0x22, 0x12, 0x00, 0x62
};
static const uint8_t hmc6301_default_regmap[] = {
	0x00, 0x10, 0x00, 0x03, 0x9f, 0x0f, 0xbf, 0x6d,
	0x80, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x36, 0xbb, 0x46, 0x02, 0x22, 0x12, 0x00, 0x62
};

/* Generate pairs of valid VCO frequencies vs their corresponding FBDIV values. */
static int _hmc630x_generate_vco(struct hmc630x_dev *d, uint64_t f_start,
				 uint64_t f_step, uint8_t fbdiv_start, uint8_t entries)
{
	int ret, e;

	d->vco.freqs = (uint64_t *)no_os_calloc(entries, sizeof(*d->vco.freqs));
	if (!d->vco.freqs)
		return -ENOMEM;

	d->vco.fbdiv = (uint8_t *)no_os_calloc(entries, sizeof(*d->vco.fbdiv));
	if (!d->vco.fbdiv) {
		ret = -ENOMEM;
		goto error;
	}

	for (e = 0; e < entries; e++) {
		d->vco.freqs[e] = f_start + e * f_step;
		d->vco.fbdiv[e] = fbdiv_start + e;
	}

	d->vco.entries = entries;

	return 0;
error:
	no_os_free(d->vco.freqs);
	return ret;
}

/* Create hmc6300 device and initialize it from init params. */
int hmc630x_init(struct hmc630x_dev **dev, struct hmc630x_init_param *init)
{
	int ret;
	struct hmc630x_dev *d;
	const uint8_t *regmap;

	if (!dev || !init)
		return -EINVAL;

	d = (struct hmc630x_dev *) no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	switch (init->type) {
	case HMC6300:
		regmap = hmc6300_default_regmap;
		d->address = HMC6300_ADDRESS;
		break;
	case HMC6301:
		regmap = hmc6301_default_regmap;
		d->address = HMC6301_ADDRESS;
		break;
	default:
		ret = -EINVAL;
		goto error_0;
		break;
	}
	d->type = init->type;

	switch (init->ref_clk) {
	case HMC6300_REF_CLK_71p42857MHz:
		ret = _hmc630x_generate_vco(d, 56500000000, 250000000, 0x22, 31);
		break;
	case HMC6300_REF_CLK_75MHz:
		ret = _hmc630x_generate_vco(d, 55125000000, 262500000, 0x12, 43);
		break;
	case HMC6300_REF_CLK_142p8571MHz:
		ret = _hmc630x_generate_vco(d, 56500000000, 500000000, 0x11, 16);
		break;
	case HMC6300_REF_CLK_154p2857MHz:
		ret = _hmc630x_generate_vco(d, 57240000000, 540000000, 0x0a, 17);
		break;
	default:
		ret = -EINVAL;
	}

	if (ret)
		goto error_0;

	ret = no_os_gpio_get(&d->en, &init->en);
	if (ret)
		goto error_1;

	ret = no_os_gpio_get(&d->clk, &init->clk);
	if (ret)
		goto error_2;

	ret = no_os_gpio_get(&d->data, &init->data);
	if (ret)
		goto error_3;

	ret = no_os_gpio_get(&d->scanout, &init->scanout);
	if (ret)
		goto error_4;

	ret = no_os_gpio_direction_output(d->en, NO_OS_GPIO_HIGH);
	if (ret)
		goto error;

	ret = no_os_gpio_direction_output(d->clk, NO_OS_GPIO_LOW);
	if (ret)
		goto error;

	ret = no_os_gpio_direction_output(d->data, NO_OS_GPIO_LOW);
	if (ret)
		goto error;

	ret = no_os_gpio_direction_input(d->scanout);
	if (ret)
		goto error;

	ret = hmc630x_write_regmap(d, regmap);
	if (ret)
		goto error;

	ret = hmc630x_set_enable(d, init->enabled);
	if (ret)
		goto error;

	ret = hmc630x_set_temp_en(d, init->temp_en);
	if (ret)
		goto error;

	ret = hmc630x_set_if_attn(d, init->if_attn);
	if (ret)
		goto error;

	if (init->type == HMC6300) {
		ret = hmc6300_set_rf_attn(d, init->tx.rf_attn);
		if (ret)
			goto error;
	} else {
		ret = hmc6301_set_lna_gain(d, init->rx.lna_attn);
		if (ret)
			goto error;

		ret = hmc6301_set_bb_attn(d, init->rx.bb_attn1, init->rx.bb_attn2);
		if (ret)
			goto error;

		ret = hmc6301_set_bb_attn_fine(d, init->rx.bb_attni_fine,
					       init->rx.bb_attnq_fine);
		if (ret)
			goto error;

		ret = hmc6301_set_bb_lpc_hpc(d, init->rx.bb_lpc, init->rx.bb_hpc);
		if (ret)
			goto error;
	}

	if (init->enabled) {
		ret = hmc630x_set_vco(d, init->vco);
		if (ret)
			goto error;
	}

	*dev = d;
	return 0;
error:
	no_os_gpio_remove(d->scanout);
error_4:
	no_os_gpio_remove(d->data);
error_3:
	no_os_gpio_remove(d->clk);
error_2:
	no_os_gpio_remove(d->en);
error_1:
	no_os_free(d->vco.freqs);
	no_os_free(d->vco.fbdiv);
error_0:
	no_os_free(d);
	return ret;
}

/* Remove the hmc6300 device by deallocating resources. */
int hmc630x_remove(struct hmc630x_dev *dev)
{
	int ret;

	no_os_free(dev->vco.freqs);
	dev->vco.freqs = NULL;
	no_os_free(dev->vco.fbdiv);
	dev->vco.fbdiv = NULL;

	ret = no_os_gpio_remove(dev->en);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->clk);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->data);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->scanout);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}

enum hmc630x_type hmc630x_type(struct hmc630x_dev *dev)
{
	if (dev)
		return dev->type;
	else
		return -EINVAL;
}

/* Write a device row using GPIO bit-banging. */
int hmc630x_write_row(struct hmc630x_dev *dev, uint8_t row, uint8_t val)
{
	int b;
	uint32_t send;

	if (!dev)
		return -EINVAL;

	send = val; // data
	send |= no_os_field_prep(HMC630X_ARRAY_ADDRESS_MASK, row);
	send |= no_os_field_prep(HMC630X_RW_MASK, 1);
	send |= no_os_field_prep(HMC630X_ADDRESS_MASK, dev->address);

	/* It's acceptable to not check return values here because we're providing
	 * correct parameters to these functions so we know for sure they'll return 0. */
	no_os_gpio_set_value(dev->en, NO_OS_GPIO_LOW);
	for (b = 0; b < HMC630X_FRAME_SIZE; b++) {
		no_os_gpio_set_value(dev->data, send & 0x1);
		no_os_udelay(HMC6300_BITBANG_DELAY_US);
		no_os_gpio_set_value(dev->clk, NO_OS_GPIO_HIGH);
		no_os_udelay(HMC6300_BITBANG_DELAY_US);
		no_os_gpio_set_value(dev->clk, NO_OS_GPIO_LOW);
		send >>= 1;
	}
	no_os_gpio_set_value(dev->data, NO_OS_GPIO_LOW);
	no_os_gpio_set_value(dev->en, NO_OS_GPIO_HIGH);

	return 0;
}

/* Read a device row using GPIO bit-banging. */
int hmc630x_read_row(struct hmc630x_dev *dev, uint8_t row, uint8_t *val)
{
	int b;
	uint8_t recv = 0;
	uint32_t send = 0;

	if (!dev || !val)
		return -EINVAL;

	send = no_os_field_prep(HMC630X_ARRAY_ADDRESS_MASK, row);
	send |= no_os_field_prep(HMC630X_ADDRESS_MASK, dev->address);

	/* It's acceptable to not check return values here because we're providing
	 * correct parameters to these functions so we know for sure they'll return 0. */

	// write the first 18 bits on data
	no_os_gpio_set_value(dev->en, NO_OS_GPIO_LOW);
	for (b = 0; b < HMC630X_FRAME_SIZE; b++) {
		no_os_gpio_set_value(dev->data, send & 0x1);
		no_os_udelay(HMC6300_BITBANG_DELAY_US);
		no_os_gpio_set_value(dev->clk, NO_OS_GPIO_HIGH);
		no_os_udelay(HMC6300_BITBANG_DELAY_US);
		no_os_gpio_set_value(dev->clk, NO_OS_GPIO_LOW);
		send >>= 1;
	}
	no_os_gpio_set_value(dev->data, NO_OS_GPIO_LOW);
	no_os_gpio_set_value(dev->en, NO_OS_GPIO_HIGH);

	// extra pulse while cs is high
	no_os_udelay(HMC6300_BITBANG_DELAY_US);
	no_os_gpio_set_value(dev->clk, NO_OS_GPIO_HIGH);
	no_os_udelay(HMC6300_BITBANG_DELAY_US);
	no_os_gpio_set_value(dev->clk, NO_OS_GPIO_LOW);
	no_os_udelay(HMC6300_BITBANG_DELAY_US);

	*val = 0;
	no_os_gpio_set_value(dev->en, NO_OS_GPIO_LOW);
	for (b = 0; b < 8; b++) {
		// scanout changes on sck rising edge
		no_os_gpio_set_value(dev->clk, NO_OS_GPIO_HIGH);
		no_os_udelay(HMC6300_BITBANG_DELAY_US);
		// sample scanout along with sck faling edge
		no_os_gpio_set_value(dev->clk, NO_OS_GPIO_LOW);
		no_os_gpio_get_value(dev->scanout, &recv);
		no_os_udelay(HMC6300_BITBANG_DELAY_US);
		*val |= recv << b;
	}
	no_os_gpio_set_value(dev->en, NO_OS_GPIO_HIGH);

	return 0;
}

/* Write the write-accessible registers of the register map. */
int hmc630x_write_regmap(struct hmc630x_dev *dev, const uint8_t *regmap)
{
	int ret, r, skip1, skip2;
	uint8_t reg;

	switch(dev->type) {
	case HMC6301:
		r = 0;
		skip1 = 10;
		skip2 = 15;
		break;
	default:
		r = 1;
		skip1 = 13;
		skip2 = 15;
		break;
	};

	for (; r < 24; r++) {
		if (r >= skip1 && r <= skip2)
			continue;
		reg = regmap[r];
		ret = hmc630x_write_row(dev, r, reg);
		if (ret < 0)
			return ret;
		ret = hmc630x_read_row(dev, r, &reg);
		if (ret < 0)
			return ret;
		if (reg != regmap[r])
			return -EFAULT;
	}

	return 0;
}

/* Write a device parameter (bit or bit-field of a certain row). */
int hmc630x_write(struct hmc630x_dev *dev, uint16_t param, uint8_t value)
{
	int ret;
	uint8_t reg;
	uint8_t row = HMC630X_ROW(param);
	uint8_t mask = HMC630X_MASK(param);

	ret = hmc630x_read_row(dev, row, &reg);
	if (ret)
		return ret;

	reg &= ~mask;
	reg |= no_os_field_prep(mask, value);
	return hmc630x_write_row(dev, row, reg);
}

/* Read a device parameter (bit or bit-field of a certain row). */
int hmc630x_read(struct hmc630x_dev *dev, uint16_t param, uint8_t *value)
{
	int ret;
	uint8_t reg;

	if (!value)
		return -EINVAL;

	ret = hmc630x_read_row(dev, HMC630X_ROW(param), &reg);
	if (ret)
		return ret;

	*value = no_os_field_get(HMC630X_MASK(param), reg);
	return 0;
}

/* Read the whole register map and store it into a 32-byte output buffer. */
int hmc630x_read_regmap(struct hmc630x_dev *dev, uint8_t *regmap)
{
	int ret, skip1, skip2;
	uint8_t r;

	switch(dev->type) {
	case HMC6301:
		r = 0;
		skip1 = 10;
		skip2 = 15;
		break;
	default:
	case HMC6300:
		r = 1;
		skip1 = 13;
		skip2 = 15;
		break;
	};

	for (; r < 32; r++) {
		regmap[r] = 0;

		if (r >= skip1 && r <= skip2)
			continue;

		ret = hmc630x_read_row(dev, r, &regmap[r]);
		if (ret)
			break;
	}

	return ret;
}

/* Enable temperature sensor. */
int hmc630x_set_temp_en(struct hmc630x_dev *dev, bool enable)
{
	int ret;
	uint16_t tempflash, tempsensor;

	switch (dev->type) {
	case HMC6301:
		tempflash = HMC6301_EN_TEMPFLASH;
		tempsensor = HMC6301_ENBAR_TEMPS;
		break;
	default:
	case HMC6300:
		tempflash = HMC6300_EN_TEMPFLASH;
		tempsensor = HMC6300_TEMPSENSOR_PWRDN;
		break;
	};

	ret = hmc630x_write(dev, tempflash, enable);
	if (ret)
		return ret;

	return hmc630x_write(dev, tempsensor, !enable);
}

/* Is temperature sensor enabled? */
int hmc630x_get_temp_en(struct hmc630x_dev *dev, bool *enable)
{
	int ret;
	uint16_t tempflash, tempsensor;
	uint8_t ena1, ena2;

	switch (dev->type) {
	case HMC6301:
		tempflash = HMC6301_EN_TEMPFLASH;
		tempsensor = HMC6301_ENBAR_TEMPS;
		break;
	default:
	case HMC6300:
		tempflash = HMC6300_EN_TEMPFLASH;
		tempsensor = HMC6300_TEMPSENSOR_PWRDN;
		break;
	};

	ret = hmc630x_read(dev, tempflash, &ena1);
	if (ret)
		return ret;

	ret = hmc630x_read(dev, tempsensor, &ena2);
	if (ret)
		return ret;

	*enable = ena1 && !ena2;

	return 0;
}

/* Get a temperature reading. */
int hmc630x_get_temp(struct hmc630x_dev *dev, uint8_t *temp)
{
	return hmc630x_read(dev, HMC630X_TEMPS, temp);
}

/* Enable FSK/MSK modulation inputs. */
int hmc6300_set_fm_en(struct hmc630x_dev *dev, bool enable)
{
	if (!dev)
		return -EINVAL;

	// this is a transmitter specific feature
	if (dev->type != HMC6300)
		return -ENOSYS;

	return hmc630x_write(dev, HMC6300_ENABLE_FM, (uint8_t)enable);
}

/* Are FSK/MSK modulation inputs enabled. */
int hmc6300_get_fm_en(struct hmc630x_dev *dev, bool *enable)
{
	if (!dev)
		return -EINVAL;

	// this is a transmitter specific feature
	if (dev->type != HMC6300)
		return -ENOSYS;

	return hmc630x_read(dev, HMC6300_ENABLE_FM, (uint8_t *)enable);
}

/* Power On/Off the chip. */
int hmc630x_set_enable(struct hmc630x_dev *dev, bool enable)
{
	int ret = -EFAULT;
	uint8_t row;

	if (dev->type == HMC6300) {
		/* row2 */
		ret = hmc630x_write(dev, HMC6300_PA_SEP_PA_PWRDWN_FAST, !enable);
		if (ret)
			return ret;
		ret = hmc630x_write(dev, HMC6300_PA_PWRDWN_FAST, !enable);
		if (ret)
			return ret;
		ret = hmc630x_write(dev, HMC6300_POWER_DET_PWRDN, !enable);
		if (ret)
			return ret;

		/* row4 */
		row = no_os_field_prep(HMC630X_MASK(HMC6300_DRIVER_PWRDN), !enable);
		row |= no_os_field_prep(HMC630X_MASK(HMC6300_UPMIXER_PWRDN), !enable);
		row |= no_os_field_prep(HMC630X_MASK(HMC6300_IFVGA_PWRDN), !enable);
		row |= no_os_field_prep(HMC630X_MASK(HMC6300_DIVIDER_PWRDN), !enable);
		row |= no_os_field_prep(HMC630X_MASK(HMC6300_PA_PWRDN), !enable);
		row |= no_os_field_prep(HMC630X_MASK(HMC6300_RFVGA_PWRDN), !enable);
		row |= no_os_field_prep(HMC630X_MASK(HMC6300_TRIPLER_PWRDN), !enable);
		row |= no_os_field_prep(HMC630X_MASK(HMC6300_IF_UPMIXER_PWRDN), !enable);
		ret = hmc630x_write_row(dev, HMC630X_ROW(HMC6300_DRIVER_PWRDN), row);
		if (ret)
			return ret;
	} else {
		/* row0 */
		row = enable ? 0 : 0xff;
		ret = hmc630x_write_row(dev, HMC630X_ROW(HMC6301_LNA_PWRDWN), row);
		if (ret)
			return ret;

		/* row1 */
		row = no_os_field_prep(HMC630X_MASK(HMC6301_BBAMP_SELL_ASK), enable);
		row |= no_os_field_prep(HMC630X_MASK(HMC6301_IPC_PWRDWN), !enable);
		row |= no_os_field_prep(HMC630X_MASK(HMC6301_IFMIX_PWRDN_Q), !enable);
		row |= no_os_field_prep(HMC630X_MASK(HMC6301_ASK_PWRDN), !enable);
		row |= no_os_field_prep(HMC630X_MASK(HMC6301_IF_BGMUX_PWRDN), !enable);
		ret = hmc630x_write_row(dev, HMC630X_ROW(HMC6301_BBAMP_SELL_ASK), row);
	}

	return hmc630x_write(dev, HMC630X_EN_SYNTH_LDO, enable);
}

/* Is the chip power on? */
int hmc630x_get_enable(struct hmc630x_dev *dev, bool *enable)
{
	int ret = -EFAULT;
	uint8_t row, mask;
	bool ena1;
	bool ena2 = true;

	if (dev->type == HMC6300) {
		/* row2 */
		mask = no_os_field_prep(HMC630X_MASK(HMC6300_PA_SEP_PA_PWRDWN_FAST), 1);
		mask = no_os_field_prep(HMC630X_MASK(HMC6300_PA_PWRDWN_FAST), 1);
		mask = no_os_field_prep(HMC630X_MASK(HMC6300_POWER_DET_PWRDN), 1);
		ret = hmc630x_read_row(dev, HMC630X_ROW(HMC6300_PA_SEP_PA_PWRDWN_FAST), &row);
		if (ret)
			return ret;
		ena1 = (bool)(row & mask);

		/* row4 */
		ret = hmc630x_read_row(dev, HMC630X_ROW(HMC6300_DRIVER_PWRDN), &row);
		if (ret)
			return ret;
		ena1 = ena1 && (bool)row;
	} else {
		/* row0 */
		ret = hmc630x_read_row(dev, HMC630X_ROW(HMC6301_LNA_PWRDWN), &row);
		if (ret)
			return ret;

		ena1 = (bool)row;

		/* row1 */
		mask = no_os_field_prep(HMC630X_MASK(HMC6301_ASK_PWRDN), 1);
		mask |= no_os_field_prep(HMC630X_MASK(HMC6301_IF_BGMUX_PWRDN), 1);
		mask |= no_os_field_prep(HMC630X_MASK(HMC6301_IFMIX_PWRDN_Q), 1);
		mask |= no_os_field_prep(HMC630X_MASK(HMC6301_IPC_PWRDWN), 1);
		ret = hmc630x_read_row(dev, HMC630X_ROW(HMC6301_BBAMP_SELL_ASK), &row);
		if (ret)
			return ret;
		ena1 = ena1 && (bool)(row & mask);

		mask = no_os_field_prep(HMC630X_MASK(HMC6301_BBAMP_SELL_ASK), 1);
		ena2 = (bool)(row & mask);
	}

	ret = hmc630x_read(dev, HMC630X_EN_SYNTH_LDO, &row);
	if (ret)
		return ret;
	ena2 = ena2 && (bool)row;

	*enable = !ena1 && ena2;
	return 0;
}

/* Set the IF attenuation in steps from 0 (highest gain) to 15.
 * For digital setting to dB correlation, see the datasheet.
 */
int hmc630x_set_if_attn(struct hmc630x_dev *dev, uint8_t attn)
{
	uint16_t param;

	switch (dev->type) {
	case HMC6301:
		param = HMC6301_IFVGA_VGA_ADJ;
		break;
	default:
	case HMC6300:
		param = HMC6300_IFVGA_VGA_ADJ;
		break;
	};

	return hmc630x_write(dev, param, attn);
}

/* Get the IF attenuation in steps from 0 (highest gain) to 15.
 * For digital setting to dB correlation, see the datasheet.
 */
int hmc630x_get_if_attn(struct hmc630x_dev *dev, uint8_t *attn)
{
	uint16_t param;

	switch (dev->type) {
	case HMC6301:
		param = HMC6301_IFVGA_VGA_ADJ;
		break;
	default:
	case HMC6300:
		param = HMC6300_IFVGA_VGA_ADJ;
		break;
	};

	return hmc630x_read(dev, param, attn);
}

/* Set the RF attenuation in steps from 0 (highest gain) to 15.
 * For digital setting to dB correlation, see the datasheet.
 */
int hmc6300_set_rf_attn(struct hmc630x_dev *dev, uint8_t attn)
{
	if (!dev)
		return -EINVAL;

	// this is an transmitter specific feature
	if (dev->type != HMC6300)
		return -ENOSYS;

	return hmc630x_write(dev, HMC6300_RFVGAGAIN, attn);
}

/* Get the RF attenuation in steps from 0 (highest gain) to 15.
 * For digital setting to dB correlation, see the datasheet.
 */
int hmc6300_get_rf_attn(struct hmc630x_dev *dev, uint8_t *attn)
{
	if (!dev)
		return -EINVAL;

	// this is an transmitter specific feature
	if (dev->type != HMC6300)
		return -ENOSYS;

	return hmc630x_read(dev, HMC6300_RFVGAGAIN, attn);
}

/* Set the VCO frequency (Hz). */
int hmc630x_set_vco(struct hmc630x_dev *dev, uint64_t frequency)
{
	int ret;
	int e;
	int b;
	int b1 = -1;
	int b2 = -1;
	uint8_t lock;

	/* Search the provided frequency in the generated set of available V-band frequencies. */
	for (e = dev->vco.entries - 1; e >= 0; e--) {
		if (frequency == dev->vco.freqs[e])
			break;
	}

	/* Provided frequency not in available list. */
	if (e < 0)
		return -EINVAL;

	/* Set Feedback Divider Ratio for the Integer-N Synthesizer */
	ret = hmc630x_write(dev, HMC630X_FBDIV_CODE, dev->vco.fbdiv[e]);
	if (ret)
		return ret;

	for (b = 0; b < 20; b++) {
		lock = 0;

		/* try band b */
		ret = hmc630x_write(dev, HMC630X_VCO_BANDSEL, b);
		if (ret)
			return ret;

		/* TODO: update this when it'll be specified in the datasheet. */
		no_os_mdelay(HMC6300_SETTLING_DELAY_MS);

		/* Detect range of VCO_BANDSEL for which PLL locks. */
		ret = hmc630x_read(dev, HMC630X_LOCKDET, &lock);
		if (ret)
			return ret;
		if (lock) {
			b2 = b;
			if (b1 < 0)
				b1 = b;
		}
		if (!lock && (b2 >= 0)) {
			break;
		}
	}

	if (b2 < 0)
		return -EFAULT;

	/* Set the band that is in the middle of the range that achieves lock. */
	ret = hmc630x_write(dev, HMC630X_VCO_BANDSEL, (b1 + b2 + 1) / 2);
	if (ret)
		return ret;

	no_os_mdelay(HMC6300_SETTLING_DELAY_MS);

	/* Make sure it locks. */
	ret = hmc630x_read(dev, HMC630X_LOCKDET, &lock);
	if (ret)
		return ret;

	return lock ? 0 : -EFAULT;
}

/* Get the VCO frequency (Hz). */
int hmc630x_get_vco(struct hmc630x_dev *dev, uint64_t *frequency)
{
	int ret, e;
	uint8_t fbdiv;
	uint8_t lockdet;

	if (!dev || !frequency)
		return -EINVAL;

	ret = hmc630x_read(dev, HMC630X_LOCKDET, &lockdet);
	if (ret)
		return ret;

	/* Abort early if there is no lock. */
	if (!lockdet) {
		*frequency = 0;
		return 0;
	}

	/* Read Feedback Divider Ratio for the Integer-N Synthesizer */
	ret = hmc630x_read(dev, HMC630X_FBDIV_CODE, &fbdiv);
	if (ret)
		return ret;

	for (e = dev->vco.entries - 1; e >= 0; e--) {
		if (fbdiv == dev->vco.fbdiv[e])
			break;
	}

	/* Invalid fbdiv. */
	if (e < 0) {
		*frequency = 0;
		return 0;
	}

	*frequency = dev->vco.freqs[e];

	return 0;
}

/* Get the available VCO frequencies in the avail array with avail_num entries. */
int hmc630x_get_avail_vco(struct hmc630x_dev *dev, const uint64_t **avail,
			  uint8_t *avail_num)
{
	if (!dev || !avail || !avail_num)
		return -EINVAL;

	*avail = dev->vco.freqs;
	*avail_num = dev->vco.entries;

	return 0;
}

/* Set the receiver LNA gain. */
int hmc6301_set_lna_gain(struct hmc630x_dev *dev, enum hmc6301_lna_attn gain)
{
	if (!dev)
		return -EINVAL;

	// this is an receiver specific feature
	if (dev->type != HMC6301)
		return -ENOSYS;

	return hmc630x_write(dev, HMC6301_LNA_GAIN, (uint8_t)gain);
}

/* Get the receiver LNA gain. */
int hmc6301_get_lna_gain(struct hmc630x_dev *dev, enum hmc6301_lna_attn *gain)
{
	if (!dev)
		return -EINVAL;

	// this is an receiver specific feature
	if (dev->type != HMC6301)
		return -ENOSYS;

	return hmc630x_read(dev, HMC6301_LNA_GAIN, (uint8_t *)gain);
}

/* Set the receiver baseband attenuation. */
int hmc6301_set_bb_attn(struct hmc630x_dev *dev, enum hmc6301_bb_attn attn1,
			enum hmc6301_bb_attn attn2)
{
	int ret;

	if (!dev)
		return -EINVAL;

	// this is a receiver specific feature
	if (dev->type != HMC6301)
		return -ENOSYS;

	if ((attn1 > HMC6301_BB_ATTN_18dB) || (attn2 > HMC6301_BB_ATTN_18dB))
		return -EINVAL;

	ret = hmc630x_write(dev, HMC6301_BBAMP_ATTEN1, (uint8_t)attn1);
	if (ret)
		return ret;

	return hmc630x_write(dev, HMC6301_BBAMP_ATTEN2, (uint8_t)attn2);
}

/* Get the receiver baseband attenuation. */
int hmc6301_get_bb_attn(struct hmc630x_dev *dev, enum hmc6301_bb_attn *attn1,
			enum hmc6301_bb_attn *attn2)
{
	int ret;

	if (!dev)
		return -EINVAL;

	// this is a receiver specific feature
	if (dev->type != HMC6301)
		return -ENOSYS;

	ret = hmc630x_read(dev, HMC6301_BBAMP_ATTEN1, (uint8_t *)attn1);
	if (ret)
		return ret;

	return hmc630x_read(dev, HMC6301_BBAMP_ATTEN2, (uint8_t *)attn2);
}

/* Set the receiver fine baseband attenuation (0-5 dB). */
int hmc6301_set_bb_attn_fine(struct hmc630x_dev *dev,
			     enum hmc6301_bb_attn_fine attn_i,
			     enum hmc6301_bb_attn_fine attn_q)
{
	int ret;

	if (!dev)
		return -EINVAL;

	// this is a receiver specific feature
	if (dev->type != HMC6301)
		return -ENOSYS;

	if ((attn_i == HMC6301_BB_ATTN_FINE_RESERVED1)
	    || (attn_i >= HMC6301_BB_ATTN_FINE_RESERVED2)
	    || (attn_q == HMC6301_BB_ATTN_FINE_RESERVED1)
	    || (attn_q >= HMC6301_BB_ATTN_FINE_RESERVED2))
		return -EINVAL;

	ret = hmc630x_write(dev, HMC6301_BBAMP_ATTENFI, attn_i);
	if (ret)
		return ret;

	return hmc630x_write(dev, HMC6301_BBAMP_ATTENFQ, attn_q);
}

/* Get the receiver fine baseband attenuation (0-5 dB). */
int hmc6301_get_bb_attn_fine(struct hmc630x_dev *dev,
			     enum hmc6301_bb_attn_fine *attn_i,
			     enum hmc6301_bb_attn_fine *attn_q)
{
	int ret;

	if (!dev)
		return -EINVAL;

	// this is a receiver specific feature
	if (dev->type != HMC6301)
		return -ENOSYS;

	ret = hmc630x_read(dev, HMC6301_BBAMP_ATTENFI, attn_i);
	if (ret)
		return ret;

	return hmc630x_read(dev, HMC6301_BBAMP_ATTENFQ, attn_q);
}

/* Set the low-pass corner and high-pass corner of the baseband amplifiers. */
int hmc6301_set_bb_lpc_hpc(struct hmc630x_dev *dev, enum hmc6301_bb_lpc lpc,
			   enum hmc6301_bb_hpc hpc)
{
	int ret;

	if (!dev)
		return -EINVAL;

	// this is a receiver specific feature
	if (dev->type != HMC6301)
		return -ENOSYS;

	ret = hmc630x_write(dev, HMC6301_BBAMP_SELBW, (uint8_t)lpc);
	if (ret)
		return ret;

	return hmc630x_write(dev, HMC6301_BBAMP_SELFASTREC, (uint8_t)hpc);
}

/* Get the low-pass corner and high-pass corner of the baseband amplifiers. */
int hmc6301_get_bb_lpc_hpc(struct hmc630x_dev *dev, enum hmc6301_bb_lpc *lpc,
			   enum hmc6301_bb_hpc *hpc)
{
	int ret;

	if (!dev)
		return -EINVAL;

	// this is a receiver specific feature
	if (dev->type != HMC6301)
		return -ENOSYS;

	ret = hmc630x_read(dev, HMC6301_BBAMP_SELBW, (uint8_t *)lpc);
	if (ret)
		return ret;

	return hmc630x_read(dev, HMC6301_BBAMP_SELFASTREC, (uint8_t *)hpc);
}
