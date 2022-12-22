/***************************************************************************//**
 *   @file   hmc6300.h
 *   @brief  hmc6300 device driver.
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
#include "hmc6300.h"
#include "no_os_delay.h"

#define HMC6300_BITBANG_DELAY_US 1
#define HMC6300_SETTLING_DELAY_MS 1

struct hmc6300_dev {
	struct hmc6300_vco vco;
	struct no_os_gpio_desc *en;
	struct no_os_gpio_desc *clk;
	struct no_os_gpio_desc *data;
	struct no_os_gpio_desc *scanout;
};

const uint8_t hmc6300_default_regmap[] = {
	0x00, 0x4a, 0xf6, 0xf6, 0x00, 0xbf, 0x6c, 0x0f,
	0x8f, 0x00, 0x53, 0x03, 0x00, 0x00, 0x00, 0x00,
	0x36, 0xbb, 0x46, 0x02, 0x22, 0x12, 0x00, 0x62
};

/* Generate pairs of valid VCO frequencies vs their corresponding FBDIV values. */
static int _hmc6300_generate_vco(struct hmc6300_dev *d, uint64_t f_start,
				 uint64_t f_step, uint8_t fbdiv_start, uint8_t entries)
{
	int ret, e;

	d->vco.freqs = (uint64_t *) calloc(entries, sizeof(uint64_t));
	if (!d->vco.freqs)
		return -ENOMEM;

	d->vco.fbdiv = (uint8_t *) calloc(entries, sizeof(uint8_t));
	if (!d->vco.fbdiv) {
		ret = -ENOMEM;
		goto error;
	}

	for(e = 0; e < entries; e++) {
		d->vco.freqs[e] = f_start + e * f_step;
		d->vco.fbdiv[e] = fbdiv_start + e;
	}

	d->vco.entries = entries;

	return 0;
error:
	free(d->vco.freqs);
	return ret;
}

/* Create hmc6300 device and initialize it from init params. */
int hmc6300_init(struct hmc6300_dev **dev, struct hmc6300_init_param *init)
{
	struct hmc6300_dev *d;
	int ret;

	if (!dev || !init)
		return -EINVAL;

	d = (struct hmc6300_dev *) calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	switch(init->ref_clk) {
	case HMC6300_REF_CLK_71p42857MHz:
		ret = _hmc6300_generate_vco(d, 56500000000, 250000000, 0x22, 31);
		break;
	case HMC6300_REF_CLK_75MHz:
		ret = _hmc6300_generate_vco(d, 55125000000, 262500000, 0x12, 43);
		break;
	case HMC6300_REF_CLK_142p8571MHz:
		ret = _hmc6300_generate_vco(d, 56500000000, 500000000, 0x11, 16);
		break;
	case HMC6300_REF_CLK_154p2857MHz:
		ret = _hmc6300_generate_vco(d, 57240000000, 540000000, 0x0a, 17);
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

	ret = hmc6300_write_regmap(d, hmc6300_default_regmap);
	if (ret)
		goto error;

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
	free(d->vco.freqs);
	free(d->vco.fbdiv);
error_0:
	free(d);
	return ret;
}

/* Remove the hmc6300 device by deallocating resources. */
int hmc6300_remove(struct hmc6300_dev *dev)
{
	free(dev->vco.freqs);
	free(dev->vco.fbdiv);
	no_os_gpio_remove(dev->en);
	no_os_gpio_remove(dev->clk);
	no_os_gpio_remove(dev->data);
	no_os_gpio_remove(dev->scanout);
	free(dev);
	return 0;
}

/* Write a device row using GPIO bit-banging. */
int hmc6300_write_row(struct hmc6300_dev *dev, uint8_t row, uint8_t val)
{
	int b;
	uint32_t send;

	if (!dev)
		return -EINVAL;

	send = val; // data
	send |= ((uint32_t)row & 0x3f) << 8; // array address
	send |= 1 << 14; // write
	send |= ((uint32_t)0x6) << 15;  // chip address

	no_os_gpio_set_value(dev->en, NO_OS_GPIO_LOW);
	for (b = 0; b < 18; b++) {
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
int hmc6300_read_row(struct hmc6300_dev *dev, uint8_t row, uint8_t *val)
{
	int b;
	uint8_t recv = 0;
	uint32_t send = 0;

	if (!dev || !val)
		return -EINVAL;

	send |= ((uint32_t)row & 0x3f) << 8;
	send |= ((uint32_t)0x6) << 15;

	// write the first 18 bits on data
	no_os_gpio_set_value(dev->en, NO_OS_GPIO_LOW);
	for (b = 0; b < 18; b++) {
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
int hmc6300_write_regmap(struct hmc6300_dev *dev, const uint8_t *regmap)
{
	int ret, r;
	uint8_t reg;

	for (r = 1; r < 24; r++) {
		if (r >= 13 && r <= 15)
			continue;
		reg = regmap[r];
		ret = hmc6300_write_row(dev, r, reg);
		if (ret < 0)
			return ret;
		ret = hmc6300_read_row(dev, r, &reg);
		if (ret < 0)
			return ret;
		if (reg != regmap[r])
			return -EFAULT;
	}

	return 0;
}

/* Write a device parameter (bit or bit-field of a certain row). */
int hmc6300_write(struct hmc6300_dev *dev, uint16_t param, uint8_t value)
{
	int ret;
	uint8_t reg;
	uint8_t row = HMC6300_ROW(param);
	uint8_t mask = HMC6300_MASK(param);

	ret = hmc6300_read_row(dev, row, &reg);
	if (ret)
		return ret;

	reg &= ~mask;
	reg |= no_os_field_prep(mask, value);
	return hmc6300_write_row(dev, row, reg);
}

/* Read a device parameter (bit or bit-field of a certain row). */
int hmc6300_read(struct hmc6300_dev *dev, uint16_t param, uint8_t *value)
{
	int ret;
	uint8_t reg;

	if (!value)
		return -EINVAL;

	ret = hmc6300_read_row(dev, HMC6300_ROW(param), &reg);
	if (ret)
		return ret;

	*value = no_os_field_get(HMC6300_MASK(param), reg);
	return 0;
}

/* Read the whole register map and store it into a 32-byte output buffer. */
int hmc6300_read_regmap(struct hmc6300_dev *dev, uint8_t *regmap)
{
	int r, ret;

	for (r = 0; r < 32; r++) {
		ret = hmc6300_read_row(dev, r, &regmap[r]);
		if (ret)
			break;
	}

	return ret;
}

/* Enable temperature sensor. */
int hmc6300_enable_temp(struct hmc6300_dev *dev, bool enable)
{
	int ret = hmc6300_write(dev, HMC6300_EN_TEMPFLASH, enable);
	if (ret)
		return ret;

	return hmc6300_write(dev, HMC6300_TEMPSENSOR_PWRDN, !enable);
}

/* Get a temperature reading. */
int hmc6300_get_temp(struct hmc6300_dev *dev, uint8_t *temp)
{
	return hmc6300_read(dev, HMC6300_TEMPS, temp);
}

/* Enable FSK/MSK modulation inputs. */
int hmc6300_enable_fm(struct hmc6300_dev *dev, bool enable)
{
	return hmc6300_write(dev, HMC6300_ENABLE_FM, (uint8_t)enable);
}

/* Power On/Off the chip. */
int hmc6300_enable(struct hmc6300_dev *dev, bool enable)
{
	int ret;
	uint8_t row4;

	row4 = enable ? 0 : 0xff;
	/*
	This collectively turns on/off the following:
		HMC6300_DRIVER_PWRDN
		HMC6300_UPMIXER_PWRDN
		HMC6300_IFVGA_PWRDN
		HMC6300_DIVIDER_PWRDN
		HMC6300_PA_PWRDN
		HMC6300_RFVGA_PWRDN
		HMC6300_TRIPLER_PWRDN
		HMC6300_IF_UPMIXER_PWRDN
	*/
	ret = hmc6300_write_row(dev, 4, row4);
	if (ret)
		return ret;

	return hmc6300_write(dev, HMC6300_EN_SYNTH_LDO, enable);
}

/* Set the IF attenuation in steps from 0 (highest gain) to 15.
 * For digital setting to dB correlation, see Figure 7 of datasheet.
 */
int hmc6300_set_if_attn(struct hmc6300_dev *dev, uint8_t attn)
{
	return hmc6300_write(dev, HMC6300_IFVGA_VGA_ADJ, attn);
}

/* Set the RF attenuation in steps from 0 (highest gain) to 15.
 * For digital setting to dB correlation, see Figure 5 of datasheet.
 */
int hmc6300_set_rf_attn(struct hmc6300_dev *dev, uint8_t attn)
{
	return hmc6300_write(dev, HMC6300_RFVGAGAIN, attn);
}

/* Set the VCO frequency (Hz). */
int hmc6300_set_vco(struct hmc6300_dev *dev, uint64_t frequency)
{
	int ret;
	int e;
	int b;
	int b1 = -1;
	int b2 = -1;
	uint8_t lock;

	/* Search the provided frequency in the generated set of available V-band frequencies. */
	for (e = dev->vco.entries; e >= 0; e--) {
		if (frequency == dev->vco.freqs[e])
			break;
	}

	/* Provided frequency not in available list. */
	if (e < 0)
		return -EINVAL;

	/* Set Feedback Divider Ratio for the Integer-N Synthesizer */
	ret = hmc6300_write(dev, HMC6300_FBDIV_CODE, dev->vco.fbdiv[e]);
	if (ret)
		return ret;

	for (b = 0; b < 20; b++) {
		lock = 0;

		/* try band b */
		ret = hmc6300_write(dev, HMC6300_VCO_BANDSEL, b);
		if (ret)
			return ret;

		/* TODO: update this when it'll be specified in the datasheet. */
		no_os_mdelay(HMC6300_SETTLING_DELAY_MS);

		/* Detect range of VCO_BANDSEL for which PLL locks. */
		ret = hmc6300_read(dev, HMC6300_LOCKDET, &lock);
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
	ret = hmc6300_write(dev, HMC6300_VCO_BANDSEL, (b1 + b2 + 1) / 2);
	if (ret)
		return ret;

	no_os_mdelay(HMC6300_SETTLING_DELAY_MS);

	/* Make sure it locks. */
	ret = hmc6300_read(dev, HMC6300_LOCKDET, &lock);
	if (ret)
		return ret;

	return lock ? 0 : -EFAULT;
}
