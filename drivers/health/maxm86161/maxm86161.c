/***************************************************************************//**
 *   @file   maxm86161.c
 *   @brief  Implementation of MAXM86161 optical pulse oximeter driver.
 *   @author Jan Carlo Roleda (jancarlo.roleda@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include <stdlib.h>
#include "maxm86161.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_irq.h"
#include "no_os_print_log.h"

int maxm86161_reg_read(struct maxm86161_dev *dev, uint8_t reg_addr,
		       uint8_t *reg_data)
{
	int ret;

	if (!dev || !reg_data)
		return -EINVAL;

	ret = no_os_i2c_write(dev->i2c_desc, &reg_addr, 1, 0);
	if (ret)
		return ret;

	return no_os_i2c_read(dev->i2c_desc, reg_data, 1, 1);
}

int maxm86161_reg_write(struct maxm86161_dev *dev, uint8_t reg_addr,
			uint8_t reg_data)
{
	uint8_t buf[2];

	if (!dev)
		return -EINVAL;

	buf[0] = reg_addr;
	buf[1] = reg_data;

	return no_os_i2c_write(dev->i2c_desc, buf, 2, 1);
}

int maxm86161_reg_update_bits(struct maxm86161_dev *dev, uint8_t reg_addr,
			      uint8_t mask, uint8_t data)
{
	int ret;
	uint8_t reg_val;

	ret = maxm86161_reg_read(dev, reg_addr, &reg_val);
	if (ret)
		return ret;

	reg_val &= ~mask;
	reg_val |= no_os_field_prep(mask, data);

	return maxm86161_reg_write(dev, reg_addr, reg_val);
}

int maxm86161_burst_reg_read(struct maxm86161_dev *dev, uint8_t reg_addr,
			     uint8_t *reg_data, uint16_t count)
{
	int ret;

	if (!dev || !reg_data)
		return -EINVAL;

	ret = no_os_i2c_write(dev->i2c_desc, &reg_addr, 1, 0);
	if (ret)
		return ret;

	for (int offset = 0; offset < count;) {
		uint32_t chunk_len = no_os_min(count - offset, MAXM86161_FIFO_BURST_RD_CAP);

		ret = no_os_i2c_read(dev->i2c_desc, reg_data, chunk_len, 1);
		if (ret)
			return ret;

		offset += chunk_len;
	}

	return 0;
}

int maxm86161_burst_reg_write(struct maxm86161_dev *dev, uint8_t reg_addr,
			      uint8_t *reg_data, uint16_t count)
{
	int ret;
	uint8_t *buf;

	if (!dev || !reg_data)
		return -EINVAL;

	buf = no_os_calloc(1, count + 1);
	if (!buf)
		return -ENOMEM;

	buf[0] = reg_addr;
	memcpy(&buf[1], reg_data, count);

	ret = no_os_i2c_write(dev->i2c_desc, buf, count + 1, 1);

	no_os_free(buf);
	return ret;
}

int maxm86161_verify_id(struct maxm86161_dev *dev)
{
	int ret;
	uint8_t part_id;

	if (!dev)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_PART_ID, &part_id);
	if (ret)
		return ret;

	if (part_id != MAXM86161_PART_ID_VAL)
		return -ENODEV;

	dev->part_id = part_id;

	return maxm86161_reg_read(dev, MAXM86161_REG_REV_ID, &dev->rev_id);
}

int maxm86161_reset(struct maxm86161_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = maxm86161_reg_write(dev, MAXM86161_REG_SYS_CTRL,
				  MAXM86161_SYS_CTRL_RESET);
	if (ret)
		return ret;

	no_os_mdelay(MAXM86161_RESET_DELAY_MS);

	return 0;
}

int maxm86161_shutdown(struct maxm86161_dev *dev)
{
	return maxm86161_set_shutdown(dev, true);
}

int maxm86161_set_shutdown(struct maxm86161_dev *dev, bool enable)
{
	if (!dev)
		return -EINVAL;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_SYS_CTRL,
					 MAXM86161_SYS_CTRL_SHDN, enable);
}

int maxm86161_set_low_power_mode(struct maxm86161_dev *dev, bool enable)
{
	if (!dev)
		return -EINVAL;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_SYS_CTRL,
					 MAXM86161_SYS_CTRL_LP_MODE, enable);
}

int maxm86161_set_sample_rate(struct maxm86161_dev *dev, uint8_t ppg_sr)
{
	int ret;

	if (!dev || ppg_sr > 0x13)
		return -EINVAL;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_CFG_2,
					MAXM86161_PPG_CFG2_PPG_SR_MSK, ppg_sr);
	if (ret)
		return ret;

	dev->ppg_config.sample_rate = ppg_sr;
	return 0;
}

int maxm86161_get_sample_rate(struct maxm86161_dev *dev, uint8_t *ppg_sr)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !ppg_sr)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_PPG_CFG_2, &reg_val);
	if (ret)
		return ret;

	*ppg_sr = no_os_field_get(MAXM86161_PPG_CFG2_PPG_SR_MSK, reg_val);
	return 0;
}

int maxm86161_set_integration_time(struct maxm86161_dev *dev,
				   enum maxm86161_ppg_tint tint)
{
	int ret;

	if (!dev || tint > MAXM86161_TINT_117_3_US)
		return -EINVAL;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_CFG_1,
					MAXM86161_PPG_CFG1_PPG_TINT_MSK,
					(uint8_t)tint);
	if (ret)
		return ret;

	dev->ppg_config.integration_time = tint;
	return 0;
}

int maxm86161_get_integration_time(struct maxm86161_dev *dev,
				   enum maxm86161_ppg_tint *tint)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !tint)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_PPG_CFG_1, &reg_val);
	if (ret)
		return ret;

	*tint = no_os_field_get(MAXM86161_PPG_CFG1_PPG_TINT_MSK, reg_val);
	return 0;
}

int maxm86161_set_adc_range(struct maxm86161_dev *dev,
			    enum maxm86161_adc_range range)
{
	int ret;

	if (!dev || range > MAXM86161_ADC_RGE_32UA)
		return -EINVAL;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_CFG_1,
					MAXM86161_PPG_CFG1_ADC_RGE_MSK,
					(uint8_t)range);
	if (ret)
		return ret;

	dev->ppg_config.adc_range = range;
	return 0;
}

int maxm86161_get_adc_range(struct maxm86161_dev *dev,
			    enum maxm86161_adc_range *range)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !range)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_PPG_CFG_1, &reg_val);
	if (ret)
		return ret;

	*range = no_os_field_get(MAXM86161_PPG_CFG1_ADC_RGE_MSK, reg_val);
	return 0;
}

int maxm86161_set_sample_averaging(struct maxm86161_dev *dev,
				   enum maxm86161_smp_ave avg)
{
	int ret;

	if (!dev || avg > MAXM86161_SMP_AVE_128)
		return -EINVAL;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_CFG_2,
					MAXM86161_PPG_CFG2_SMP_AVE_MSK,
					(uint8_t)avg);
	if (ret)
		return ret;

	dev->ppg_config.sample_avg = avg;
	return 0;
}

int maxm86161_get_sample_averaging(struct maxm86161_dev *dev,
				   enum maxm86161_smp_ave *avg)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !avg)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_PPG_CFG_2, &reg_val);
	if (ret)
		return ret;

	*avg = no_os_field_get(MAXM86161_PPG_CFG2_SMP_AVE_MSK, reg_val);
	return 0;
}

int maxm86161_set_alc_disable(struct maxm86161_dev *dev, bool disable)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_CFG_1,
					MAXM86161_PPG_CFG1_ALC_DISABLE, disable);
	if (ret)
		return ret;

	dev->ppg_config.alc_disable = disable;
	return 0;
}

int maxm86161_get_alc_disable(struct maxm86161_dev *dev, bool *disable)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !disable)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_PPG_CFG_1, &reg_val);
	if (ret)
		return ret;

	*disable = !!(reg_val & MAXM86161_PPG_CFG1_ALC_DISABLE);
	return 0;
}

int maxm86161_set_add_offset(struct maxm86161_dev *dev, bool enable)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_CFG_1,
					MAXM86161_PPG_CFG1_ADD_OFFSET, enable);
	if (ret)
		return ret;

	dev->ppg_config.add_offset = enable;
	return 0;
}

int maxm86161_get_add_offset(struct maxm86161_dev *dev, bool *enable)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !enable)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_PPG_CFG_1, &reg_val);
	if (ret)
		return ret;

	*enable = !!(reg_val & MAXM86161_PPG_CFG1_ADD_OFFSET);
	return 0;
}

int maxm86161_set_led_settling(struct maxm86161_dev *dev,
			       enum maxm86161_led_settling settling)
{
	int ret;

	if (!dev || settling > MAXM86161_LED_SETLNG_12_US)
		return -EINVAL;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_CFG_3,
					MAXM86161_PPG_CFG3_LED_SETLNG_MSK,
					(uint8_t)settling);
	if (ret)
		return ret;

	dev->ppg_config.led_settling = settling;
	return 0;
}

int maxm86161_get_led_settling(struct maxm86161_dev *dev,
			       enum maxm86161_led_settling *settling)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !settling)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_PPG_CFG_3, &reg_val);
	if (ret)
		return ret;

	*settling = no_os_field_get(MAXM86161_PPG_CFG3_LED_SETLNG_MSK, reg_val);
	return 0;
}

int maxm86161_set_digital_filter(struct maxm86161_dev *dev,
				 enum maxm86161_dig_filt_sel filter)
{
	int ret;

	if (!dev || filter > MAXM86161_DIG_FILT_FDM)
		return -EINVAL;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_CFG_3,
					MAXM86161_PPG_CFG3_DIG_FILT_MSK,
					(uint8_t)filter);
	if (ret)
		return ret;

	dev->ppg_config.dig_filter_sel = filter;
	return 0;
}

int maxm86161_get_digital_filter(struct maxm86161_dev *dev,
				 enum maxm86161_dig_filt_sel *filter)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !filter)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_PPG_CFG_3, &reg_val);
	if (ret)
		return ret;

	*filter = no_os_field_get(MAXM86161_PPG_CFG3_DIG_FILT_MSK, reg_val);
	return 0;
}

int maxm86161_set_pd_bias(struct maxm86161_dev *dev,
			  enum maxm86161_pd_bias bias)
{
	if (!dev)
		return -EINVAL;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_PD_BIAS,
					 MAXM86161_PD_BIAS_MSK, (uint8_t)bias);
}

int maxm86161_get_pd_bias(struct maxm86161_dev *dev,
			  enum maxm86161_pd_bias *bias)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !bias)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_PD_BIAS, &reg_val);
	if (ret)
		return ret;

	*bias = no_os_field_get(MAXM86161_PD_BIAS_MSK, reg_val);
	return 0;
}

int maxm86161_set_led_sequence(struct maxm86161_dev *dev, uint8_t slot,
			       enum maxm86161_led_src src)
{
	uint8_t reg_addr;
	uint8_t mask;

	if (!dev || slot == 0 || slot > MAXM86161_NUM_LED_SEQ)
		return -EINVAL;

	slot = slot - 1;

	reg_addr = MAXM86161_REG_LED_SEQ_1 + (slot / 2);

	if (slot % 2 == 0)
		mask = MAXM86161_LED_SEQ_ODD_MSK;
	else
		mask = MAXM86161_LED_SEQ_EVEN_MSK;

	dev->led_seq.sequence[slot] = src;

	return maxm86161_reg_update_bits(dev, reg_addr, mask, (uint8_t)src);
}

int maxm86161_get_led_sequence(struct maxm86161_dev *dev, uint8_t slot,
			       enum maxm86161_led_src *src)
{
	int ret;
	uint8_t reg_addr;
	uint8_t reg_val;

	if (!dev || !src || slot == 0 || slot > MAXM86161_NUM_LED_SEQ)
		return -EINVAL;

	slot = slot - 1;
	reg_addr = MAXM86161_REG_LED_SEQ_1 + (slot / 2);

	ret = maxm86161_reg_read(dev, reg_addr, &reg_val);
	if (ret)
		return ret;

	if (slot % 2 == 0)
		*src = no_os_field_get(MAXM86161_LED_SEQ_ODD_MSK, reg_val);
	else
		*src = no_os_field_get(MAXM86161_LED_SEQ_EVEN_MSK, reg_val);

	return 0;
}

int maxm86161_configure_led_sequence(struct maxm86161_dev *dev,
				     struct maxm86161_led_seq_config *config)
{
	int ret;
	uint8_t i;

	if (!dev || !config)
		return -EINVAL;

	for (i = 0; i < MAXM86161_NUM_LED_SEQ; i++) {
		ret = maxm86161_set_led_sequence(dev, i + 1, config->sequence[i]);
		if (ret)
			return ret;
	}

	return 0;
}

int maxm86161_set_led_pulse_amplitude(struct maxm86161_dev *dev,
				      enum maxm86161_led_num led_num, uint8_t amplitude)
{
	uint8_t reg_addr;

	if (!dev || led_num == 0 || led_num > MAXM86161_LED_NUM_PILOT_GREEN)
		return -EINVAL;

	reg_addr = MAXM86161_REG_LED1_PA + (led_num - 1);
	dev->led_config.pulse_amplitude[led_num - 1] = amplitude;

	return maxm86161_reg_write(dev, reg_addr, amplitude);
}

int maxm86161_get_led_pulse_amplitude(struct maxm86161_dev *dev,
				      enum maxm86161_led_num led_num, uint8_t *amplitude)
{
	uint8_t reg_addr;

	if (!dev || !amplitude || led_num == 0
	    || led_num > MAXM86161_LED_NUM_PILOT_GREEN)
		return -EINVAL;

	reg_addr = MAXM86161_REG_LED1_PA + (led_num - 1);

	return maxm86161_reg_read(dev, reg_addr, amplitude);
}

int maxm86161_set_led_pilot_pa(struct maxm86161_dev *dev, uint8_t amplitude)
{
	if (!dev)
		return -EINVAL;

	dev->led_config.pilot_pa = amplitude;

	return maxm86161_reg_write(dev, MAXM86161_REG_LED_PILOT_PA, amplitude);
}

int maxm86161_get_led_pilot_pa(struct maxm86161_dev *dev, uint8_t *amplitude)
{
	if (!dev || !amplitude)
		return -EINVAL;

	return maxm86161_reg_read(dev, MAXM86161_REG_LED_PILOT_PA, amplitude);
}

int maxm86161_set_led_range(struct maxm86161_dev *dev,
			    enum maxm86161_led_num led_num,
			    enum maxm86161_led_range range)
{
	uint8_t mask;

	if (!dev || led_num == 0 || led_num > MAXM86161_LED_NUM_RED ||
	    range > MAXM86161_LED_RGE_124MA)
		return -EINVAL;

	switch (led_num) {
	case MAXM86161_LED_NUM_GREEN:
		mask = MAXM86161_LED1_RGE_MSK;
		break;
	case MAXM86161_LED_NUM_IR:
		mask = MAXM86161_LED2_RGE_MSK;
		break;
	case MAXM86161_LED_NUM_RED:
		mask = MAXM86161_LED3_RGE_MSK;
	}

	dev->led_config.range[led_num - 1] = range;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_LED_RANGE_1,
					 mask, (uint8_t)range);
}

int maxm86161_get_led_range(struct maxm86161_dev *dev,
			    enum maxm86161_led_num led_num,
			    enum maxm86161_led_range *range)
{
	int ret;
	uint8_t reg_val;
	uint8_t led_mask;

	if (!dev || !range || led_num == 0 || led_num > MAXM86161_LED_NUM_RED)
		return -EINVAL;

	switch (led_num) {
	case MAXM86161_LED_NUM_GREEN:
		led_mask = MAXM86161_LED1_RGE_MSK;
		break;
	case MAXM86161_LED_NUM_IR:
		led_mask = MAXM86161_LED2_RGE_MSK;
		break;
	case MAXM86161_LED_NUM_RED:
		led_mask = MAXM86161_LED3_RGE_MSK;
	}

	ret = maxm86161_reg_read(dev, MAXM86161_REG_LED_RANGE_1, &reg_val);
	if (ret)
		return ret;

	*range = no_os_field_get(led_mask, reg_val);
	return 0;
}

int maxm86161_set_led_cfg(struct maxm86161_dev *dev,
			  enum maxm86161_led_num led_num,
			  enum maxm86161_led_range range, uint8_t led_seq_num, uint8_t amplitude)
{
	int ret;

	const uint8_t led_seq_code[] = {
		[MAXM86161_LED_NUM_GREEN]	= MAXM86161_LED_SRC_GREEN,
		[MAXM86161_LED_NUM_IR]		= MAXM86161_LED_SRC_IR,
		[MAXM86161_LED_NUM_RED]		= MAXM86161_LED_SRC_RED,
		[MAXM86161_LED_NUM_PILOT_GREEN]	= MAXM86161_LED_SRC_PILOT_GREEN,
	};

	ret = maxm86161_set_led_range(dev, led_num, range);
	if (ret)
		return ret;

	ret = maxm86161_set_led_sequence(dev, led_seq_num, led_seq_code[led_num]);
	if (ret)
		return ret;

	return maxm86161_set_led_pulse_amplitude(dev, led_num, amplitude);
}

int maxm86161_set_hires_dac(struct maxm86161_dev *dev, uint8_t slot,
			    bool override, uint8_t dac_val)
{
	uint8_t reg_addr;
	uint8_t reg_val;

	if (!dev || slot == 0 || slot > MAXM86161_NUM_LED_SEQ)
		return -EINVAL;

	reg_addr = MAXM86161_REG_S1_HIRES_DAC + (slot - 1);
	reg_val = (override ? MAXM86161_HIRES_DAC_OVR : 0) |
		  (dac_val & NO_OS_GENMASK(5, 0));

	return maxm86161_reg_write(dev, reg_addr, reg_val);
}

int maxm86161_get_hires_dac(struct maxm86161_dev *dev, uint8_t slot,
			    bool *override, uint8_t *dac_val)
{
	int ret;
	uint8_t reg_addr;
	uint8_t reg_val;

	if (!dev || !override || !dac_val || slot == 0 ||
	    slot > MAXM86161_NUM_LED_SEQ)
		return -EINVAL;

	reg_addr = MAXM86161_REG_S1_HIRES_DAC + (slot - 1);

	ret = maxm86161_reg_read(dev, reg_addr, &reg_val);
	if (ret)
		return ret;

	*override = !!(reg_val & MAXM86161_HIRES_DAC_OVR);
	*dac_val = no_os_field_get(MAXM86161_HIRES_DAC_VAL_MSK, reg_val);
	return 0;
}

int maxm86161_set_fifo_watermark(struct maxm86161_dev *dev, uint8_t watermark)
{
	int ret;

	if (!dev || watermark > 127)
		return -EINVAL;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_FIFO_CFG_1,
					MAXM86161_FIFO_A_FULL_MSK, watermark);
	if (ret)
		return ret;

	dev->fifo_config.watermark = watermark;
	return 0;
}

int maxm86161_get_fifo_watermark(struct maxm86161_dev *dev, uint8_t *watermark)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !watermark)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_FIFO_CFG_1, &reg_val);
	if (ret)
		return ret;

	*watermark = no_os_field_get(MAXM86161_FIFO_A_FULL_MSK, reg_val);
	return 0;
}

int maxm86161_fifo_flush(struct maxm86161_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_FIFO_CFG_2,
					 MAXM86161_FIFO_CFG2_FLUSH, 1);
}

int maxm86161_set_fifo_rollover(struct maxm86161_dev *dev, bool enable)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_FIFO_CFG_2,
					MAXM86161_FIFO_CFG2_FIFO_RO, enable);
	if (ret)
		return ret;

	dev->fifo_config.rollover = enable;
	return 0;
}

int maxm86161_get_fifo_rollover(struct maxm86161_dev *dev, bool *enable)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !enable)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_FIFO_CFG_2, &reg_val);
	if (ret)
		return ret;

	*enable = !!(reg_val & MAXM86161_FIFO_CFG2_FIFO_RO);
	return 0;
}

int maxm86161_set_fifo_a_full_type(struct maxm86161_dev *dev, bool type)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_FIFO_CFG_2,
					MAXM86161_FIFO_CFG2_A_FULL_TYPE, type);
	if (ret)
		return ret;

	dev->fifo_config.a_full_type = type;
	return 0;
}

int maxm86161_get_fifo_a_full_type(struct maxm86161_dev *dev, bool *type)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !type)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_FIFO_CFG_2, &reg_val);
	if (ret)
		return ret;

	*type = !!(reg_val & MAXM86161_FIFO_CFG2_A_FULL_TYPE);
	return 0;
}

int maxm86161_set_fifo_stat_clr(struct maxm86161_dev *dev, bool enable)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_FIFO_CFG_2,
					MAXM86161_FIFO_CFG2_STAT_CLR, enable);
	if (ret)
		return ret;

	dev->fifo_config.stat_clr = enable;
	return 0;
}

int maxm86161_get_fifo_count(struct maxm86161_dev *dev, uint8_t *count)
{
	if (!dev || !count)
		return -EINVAL;

	return maxm86161_reg_read(dev, MAXM86161_REG_FIFO_DATA_COUNT, count);
}

int maxm86161_fifo_overflow_check(struct maxm86161_dev *dev,
				  uint8_t *overflow_count)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !overflow_count)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_OVF_COUNTER, &reg_val);
	if (ret)
		return ret;

	*overflow_count = reg_val & NO_OS_GENMASK(6, 0);
	return 0;
}

int maxm86161_read_fifo_data(struct maxm86161_dev *dev, uint8_t *data,
			     uint16_t num_samples)
{
	if (!dev || !data || num_samples == 0)
		return -EINVAL;

	return maxm86161_burst_reg_read(dev, MAXM86161_REG_FIFO_DATA, data,
					num_samples * MAXM86161_FIFO_DATA_SIZE);
}

int maxm86161_decode_fifo_sample(uint8_t *raw, uint8_t *tag, uint32_t *data)
{
	uint32_t raw_val;

	if (!raw || !tag || !data)
		return -EINVAL;

	/* data is received in big endian format. convert to single raw value */
	raw_val = ((uint32_t)raw[0] << 16) | ((uint32_t)raw[1] << 8) | raw[2];

	*tag = no_os_field_get(MAXM86161_FIFO_TAG_MSK, raw_val);
	*data = no_os_field_get(MAXM86161_FIFO_DATA_MSK, raw_val);

	return 0;
}

int maxm86161_get_active_led_count(struct maxm86161_dev *dev, uint8_t *count)
{
	uint8_t i;
	uint8_t active = 0;

	if (!dev || !count)
		return -EINVAL;

	for (i = 0; i < MAXM86161_NUM_LED_SEQ; i++) {
		if (dev->led_seq.sequence[i] != MAXM86161_LED_SRC_NONE)
			active++;
		else
			break;
	}

	*count = active;
	return 0;
}

int maxm86161_get_interrupt_status(struct maxm86161_dev *dev, uint16_t *status)
{
	int ret;
	uint8_t stat1, stat2;

	if (!dev || !status)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_INT_STATUS_1, &stat1);
	if (ret)
		return ret;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_INT_STATUS_2, &stat2);
	if (ret)
		return ret;

	*status = ((uint16_t)stat2 << 8) | stat1;
	return 0;
}

int maxm86161_clear_interrupt_status(struct maxm86161_dev *dev)
{
	int ret;
	uint8_t dummy;

	if (!dev)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_INT_STATUS_1, &dummy);
	if (ret)
		return ret;

	return maxm86161_reg_read(dev, MAXM86161_REG_INT_STATUS_2, &dummy);
}

int maxm86161_set_irq_handler(struct maxm86161_dev *dev,
			      enum maxm86161_irq_sources int_source,
			      bool enable, maxm86161_int_cb_t callback,
			      void *cb_args)
{
	int ret;
	uint8_t int_mask = 0;
	uint8_t int_reg;
	struct maxm86161_int_handler handler = {
		.callback = callback,
		.args = cb_args,
	};

	if (!dev || int_source < 0 || int_source >= MAXM86161_INT_COUNT)
		return -EINVAL;

	int_reg = MAXM86161_REG_INT_EN_1;

	switch (int_source) {
	case MAXM86161_INT_PWR_RDY:
		/* PWR_RDY has no enable bit; just register the callback */
		dev->int_handlers[int_source] = handler;
		return 0;
	case MAXM86161_INT_DIE_TEMP_RDY:
		int_mask = MAXM86161_INT1_DIE_TEMP_RDY_EN;
		break;
	case MAXM86161_INT_LED_COMPB:
		int_mask = MAXM86161_INT1_LED_COMPB_EN;
		break;
	case MAXM86161_INT_PROX_INT:
		int_mask = MAXM86161_INT1_PROX_INT_EN;
		break;
	case MAXM86161_INT_ALC_OVF:
		int_mask = MAXM86161_INT1_ALC_OVF_EN;
		break;
	case MAXM86161_INT_DATA_RDY:
		int_mask = MAXM86161_INT1_DATA_RDY_EN;
		break;
	case MAXM86161_INT_A_FULL:
		int_mask = MAXM86161_INT1_A_FULL_EN;
		break;
	case MAXM86161_INT_SHA_DONE:
		int_mask = MAXM86161_INT2_SHA_DONE_EN;
		int_reg = MAXM86161_REG_INT_EN_2;
		break;
	}

	ret = maxm86161_reg_update_bits(dev, int_reg, int_mask, enable);
	if (ret)
		return ret;

	dev->int_handlers[int_source] = handler;

	return 0;
}

void maxm86161_irq_handler(void *ctx)
{
	struct maxm86161_dev *dev = (struct maxm86161_dev *)ctx;
	uint16_t status;
	int ret, i;

	if (!dev)
		return;

	/* Disable IRQ during interrupt */
	ret = no_os_irq_disable(dev->irq_ctrl, dev->gpio_intb->number);
	if (ret)
		return;

	ret = maxm86161_get_interrupt_status(dev, &status);
	if (ret) {
		no_os_irq_enable(dev->irq_ctrl, dev->gpio_intb->number);
		return;
	}

	for (i = 0; i < MAXM86161_INT_COUNT; i++) {
		if (!no_os_field_get(maxm86161_irq_status_bits[i], status))
			continue;
		if (dev->int_handlers[i].callback)
			dev->int_handlers[i].callback(dev->int_handlers[i].args);
	}

	ret = no_os_irq_enable(dev->irq_ctrl, dev->gpio_intb->number);
	if (ret)
		return;
}

int maxm86161_set_prox_threshold(struct maxm86161_dev *dev, uint8_t threshold)
{
	if (!dev)
		return -EINVAL;

	return maxm86161_reg_write(dev, MAXM86161_REG_PROX_INT_THRESH,
				   threshold);
}

int maxm86161_get_prox_threshold(struct maxm86161_dev *dev, uint8_t *threshold)
{
	if (!dev || !threshold)
		return -EINVAL;

	return maxm86161_reg_read(dev, MAXM86161_REG_PROX_INT_THRESH, threshold);
}

int maxm86161_set_picket_fence(struct maxm86161_dev *dev,
			       struct maxm86161_picket_fence_config *config)
{
	uint8_t reg_val;

	if (!dev || !config)
		return -EINVAL;

	reg_val = (config->enable ? MAXM86161_PF_ENABLE : 0) |
		  (config->order ? MAXM86161_PF_ORDER : 0) |
		  no_os_field_prep(MAXM86161_PF_IIR_TC_MSK, config->iir_tc) |
		  no_os_field_prep(MAXM86161_PF_IIR_INIT_MSK,
				   config->iir_init_value) |
		  no_os_field_prep(MAXM86161_PF_THRESH_SIGMA_MSK,
				   config->threshold_sigma_mult);

	dev->pf_config = *config;

	return maxm86161_reg_write(dev, MAXM86161_REG_PICKET_FENCE, reg_val);
}

int maxm86161_get_picket_fence(struct maxm86161_dev *dev,
			       struct maxm86161_picket_fence_config *config)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !config)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_PICKET_FENCE, &reg_val);
	if (ret)
		return ret;

	config->enable = !!(reg_val & MAXM86161_PF_ENABLE);
	config->order = !!(reg_val & MAXM86161_PF_ORDER);
	config->iir_tc = no_os_field_get(MAXM86161_PF_IIR_TC_MSK, reg_val);
	config->iir_init_value = no_os_field_get(MAXM86161_PF_IIR_INIT_MSK,
				 reg_val);
	config->threshold_sigma_mult = no_os_field_get(
					       MAXM86161_PF_THRESH_SIGMA_MSK, reg_val);

	return 0;
}

static int maxm86161_trigger_temp_read(struct maxm86161_dev *dev)
{
	int ret;
	uint8_t reg_val;

	if (!dev)
		return -EINVAL;

	ret = maxm86161_reg_write(dev, MAXM86161_REG_DIE_TEMP_CFG,
				  MAXM86161_TEMP_EN);
	if (ret)
		return ret;

	/* wait for bit clear to indicate temp has been sampled. */
	for (int i = 0; i < MAXM86161_TEMP_MEAS_WAIT_TRIES; i++) {
		no_os_mdelay(MAXM86161_TEMP_MEAS_DELAY_MS);

		ret = maxm86161_reg_read(dev, MAXM86161_REG_DIE_TEMP_CFG,
					 &reg_val);
		if (ret)
			return ret;

		if (!no_os_field_get(MAXM86161_TEMP_EN, reg_val))
			return 0;
	}

	return -ETIMEDOUT;
}

int maxm86161_read_die_temperature(struct maxm86161_dev *dev,
				   int32_t *temperature)
{
	int ret;
	uint16_t temp_raw;
	uint8_t *temp_raw_ptr = (uint8_t *)&temp_raw;
	uint8_t temp_frac;
	int8_t signed_int;

	if (!dev || !temperature)
		return -EINVAL;

	ret = maxm86161_trigger_temp_read(dev);
	if (ret)
		return ret;

	ret = maxm86161_burst_reg_read(dev, MAXM86161_REG_DIE_TEMP_INT,
				       temp_raw_ptr, sizeof(temp_raw));
	if (ret)
		return ret;

	signed_int = (int8_t)no_os_field_get(MAXM86161_REG_DIE_TEMP_INT_MASK, temp_raw);
	temp_frac = no_os_field_get(
			    MAXM86161_REG_DIE_TEMP_FRAC_MASK << MAXM86161_DIE_TEMP_FRAC_OFFSET,
			    temp_raw);

	*temperature = (int32_t)signed_int * MAXM86161_DIE_TEMP_SCALE_UC +
		       (int32_t)temp_frac * MAXM86161_DIE_TEMP_FRAC_SCALE_UC;

	return 0;
}

int maxm86161_start_calibration(struct maxm86161_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_DAC_CAL_EN,
					 MAXM86161_START_CAL, 1);
}

int maxm86161_get_calibration_status(struct maxm86161_dev *dev,
				     bool *complete, bool *oor)
{
	int ret;
	uint8_t reg_val;

	if (!dev || !complete || !oor)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_DAC_CAL_EN, &reg_val);
	if (ret)
		return ret;

	*complete = !!(reg_val & MAXM86161_CAL_DAC_COMPLETE);
	*oor = !!(reg_val & MAXM86161_CAL_DAC1_OOR);

	return 0;
}

int maxm86161_set_burst_mode(struct maxm86161_dev *dev, bool enable,
			     enum maxm86161_burst_rate rate)
{
	int ret;

	if (!dev || rate > MAXM86161_BURST_RATE_256HZ)
		return -EINVAL;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_CFG_3,
					MAXM86161_PPG_CFG3_BURST_EN, enable);
	if (ret)
		return ret;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_CFG_3,
					 MAXM86161_PPG_CFG3_BURST_RATE_MSK,
					 (uint8_t)rate);
}

int maxm86161_set_timestamp_enable(struct maxm86161_dev *dev, bool enable)
{
	if (!dev)
		return -EINVAL;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_SYNC_CTRL,
					 MAXM86161_PPG_SYNC_TIMESTAMP_EN, enable);
}

int maxm86161_set_dac_code_tag(struct maxm86161_dev *dev, bool enable)
{
	if (!dev)
		return -EINVAL;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_SYNC_CTRL,
					 MAXM86161_PPG_SYNC_DAC_CODE_TAG, enable);
}

int maxm86161_set_gpio_ctrl(struct maxm86161_dev *dev,
			    enum maxm86161_gpio_ctrl ctrl)
{
	if (!dev)
		return -EINVAL;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_SYNC_CTRL,
					 MAXM86161_PPG_SYNC_GPIO_CTRL_MSK,
					 (uint8_t)ctrl);
}

int maxm86161_force_sync(struct maxm86161_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_PPG_SYNC_CTRL,
					 MAXM86161_PPG_SYNC_SW_FORCE, 1);
}

int maxm86161_sha_enable(struct maxm86161_dev *dev, bool enable)
{
	if (!dev)
		return -EINVAL;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_SHA_CFG,
					 MAXM86161_SHA_EN, enable);
}

int maxm86161_sha_start(struct maxm86161_dev *dev, uint8_t cmd)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = maxm86161_reg_write(dev, MAXM86161_REG_SHA_CMD, cmd);
	if (ret)
		return ret;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_SHA_CFG,
					 MAXM86161_SHA_START, true);
}


/* To be reviewed */
int maxm86161_sha_write_challenge(struct maxm86161_dev *dev,
				  uint8_t *challenge, uint8_t len)
{
	int ret;
	uint8_t i;

	if (!dev || !challenge || len == 0)
		return -EINVAL;

	/* challenge must be a 160-bit value */
	if (len != 20)
		return -EINVAL;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_MEM_CTRL,
					MAXM86161_MEM_WR_EN, true);
	if (ret)
		return ret;

	ret = maxm86161_reg_write(dev, MAXM86161_REG_MEM_IDX,
				  MAXM86161_SHA_MEM_START);
	if (ret)
		return ret;

	return maxm86161_burst_reg_write(dev, MAXM86161_REG_MEM_DATA,
					 challenge, len);
}

int maxm86161_sha_read_response(struct maxm86161_dev *dev,
				uint8_t *response, uint8_t len)
{
	int ret;
	uint8_t i;

	if (!dev || !response || len == 0)
		return -EINVAL;

	ret = maxm86161_reg_write(dev, MAXM86161_REG_MEM_IDX, MAXM86161_SHA_MEM_START);
	if (ret)
		return ret;

	return maxm86161_burst_reg_read(dev, MAXM86161_REG_MEM_DATA, response, len);
}

int maxm86161_memory_read(struct maxm86161_dev *dev, uint16_t addr,
			  uint8_t *data)
{
	int ret;
	uint8_t bank;

	if (!dev || !data || addr > 0x17F)
		return -EINVAL;

	bank = (addr > 0xFF) ? 1 : 0;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_MEM_CTRL,
					MAXM86161_MEM_BANK_SEL, bank);
	if (ret)
		return ret;

	ret = maxm86161_reg_write(dev, MAXM86161_REG_MEM_IDX,
				  (uint8_t)(addr & 0xFF));
	if (ret)
		return ret;

	return maxm86161_reg_read(dev, MAXM86161_REG_MEM_DATA, data);
}

int maxm86161_memory_write(struct maxm86161_dev *dev, uint16_t addr,
			   uint8_t data)
{
	int ret;
	uint8_t bank;

	if (!dev || addr > 0x17F)
		return -EINVAL;

	bank = (addr > 0xFF) ? 1 : 0;

	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_MEM_CTRL,
					MAXM86161_MEM_WR_EN |
					MAXM86161_MEM_BANK_SEL,
					MAXM86161_MEM_WR_EN |
					(bank ? MAXM86161_MEM_BANK_SEL : 0));
	if (ret)
		return ret;

	ret = maxm86161_reg_write(dev, MAXM86161_REG_MEM_IDX,
				  (uint8_t)(addr & 0xFF));
	if (ret)
		return ret;

	return maxm86161_reg_write(dev, MAXM86161_REG_MEM_DATA, data);
}

int maxm86161_memory_bank_set(struct maxm86161_dev *dev, bool enable)
{
	if (!dev)
		return -EINVAL;

	return maxm86161_reg_update_bits(dev, MAXM86161_REG_MEM_CTRL,
					 MAXM86161_MEM_BANK_SEL, enable);
}

int maxm86161_memory_bank_get(struct maxm86161_dev *dev, bool *enable)
{

	int ret;
	uint8_t reg_val;

	if (!dev)
		return -EINVAL;

	ret = maxm86161_reg_read(dev, MAXM86161_REG_MEM_CTRL, &reg_val);
	if (ret)
		return ret;

	*enable = no_os_field_get(MAXM86161_MEM_BANK_SEL, reg_val);

	return 0;
}

int maxm86161_init(struct maxm86161_dev **device,
		   struct maxm86161_init_param *init_param)
{
	struct maxm86161_dev *dev;
	int ret;

	if (!device || !init_param)
		return -EINVAL;

	dev = no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_i2c_init(&dev->i2c_desc, &init_param->i2c_init);
	if (ret)
		goto err_free_dev;

	ret = maxm86161_verify_id(dev);
	if (ret)
		goto err_bus;

	ret = maxm86161_reset(dev);
	if (ret)
		goto err_bus;

	ret = maxm86161_set_shutdown(dev, true);
	if (ret)
		goto err_bus;

	ret = maxm86161_clear_interrupt_status(dev);
	if (ret)
		goto err_bus;

	/*
	 * MAXM86161 is single-channel only. set to 1 defined by datasheet spec,
	 * but is generally ignored for MAXM86161.
	 */
	ret = maxm86161_reg_update_bits(dev, MAXM86161_REG_SYS_CTRL,
					MAXM86161_SYS_CTRL_SINGLE_PPG, 1);
	if (ret)
		goto err_bus;

	if (init_param->gpio_intb_init) {
		ret = no_os_gpio_get(&dev->gpio_intb,
				     init_param->gpio_intb_init);
		if (ret)
			goto err_bus;

		ret = no_os_gpio_direction_input(dev->gpio_intb);
		if (ret)
			goto err_gpio;
	}

	if (init_param->irq_init) {
		struct no_os_callback_desc irq_cb = {
			.callback = maxm86161_irq_handler,
			.ctx = dev,
			.event = NO_OS_EVT_GPIO,
			.peripheral = NO_OS_GPIO_IRQ,
			.handle = init_param->irq_handle,
		};

		ret = no_os_irq_ctrl_init(&dev->irq_ctrl, init_param->irq_init);
		if (ret)
			goto err_gpio;

		ret = no_os_irq_register_callback(dev->irq_ctrl, dev->gpio_intb->number,
						  &irq_cb);
		if (ret)
			goto err_irq;

		ret = no_os_irq_trigger_level_set(dev->irq_ctrl, dev->gpio_intb->number,
						  NO_OS_IRQ_EDGE_FALLING);
		if (ret)
			goto err_irq;

		ret = no_os_irq_set_priority(dev->irq_ctrl, dev->gpio_intb->number, 7);
		if (ret)
			goto err_irq;

		ret = no_os_irq_enable(dev->irq_ctrl, dev->gpio_intb->number);
		if (ret)
			goto err_irq;
	}

	*device = dev;
	return 0;

err_irq:
	if (dev->irq_ctrl)
		no_os_irq_ctrl_remove(dev->irq_ctrl);
err_gpio:
	if (dev->gpio_intb)
		no_os_gpio_remove(dev->gpio_intb);
err_bus:
	ret = no_os_i2c_remove(dev->i2c_desc);
err_free_dev:
	no_os_free(dev);
	return ret;
}

int maxm86161_remove(struct maxm86161_dev *dev)
{
	struct no_os_callback_desc irq_cb = { 0 };
	int ret = 0;
	int r;

	if (!dev)
		return -EINVAL;

	maxm86161_shutdown(dev);

	if (dev->irq_ctrl) {
		no_os_irq_disable(dev->irq_ctrl, dev->gpio_intb->number);
		irq_cb.callback = maxm86161_irq_handler;
		irq_cb.ctx = dev;
		irq_cb.event = NO_OS_EVT_GPIO;
		irq_cb.peripheral = NO_OS_GPIO_IRQ;
		no_os_irq_unregister_callback(dev->irq_ctrl, dev->gpio_intb->number,
					      &irq_cb);
		no_os_irq_ctrl_remove(dev->irq_ctrl);
	}

	if (dev->gpio_intb) {
		r = no_os_gpio_remove(dev->gpio_intb);
		if (r)
			ret = r;
	}

	r = no_os_i2c_remove(dev->i2c_desc);
	if (r)
		ret = r;

	no_os_free(dev);

	return ret;
}
