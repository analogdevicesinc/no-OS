/***************************************************************************//**
 *   @file   iio_maxm86161.c
 *   @brief  Implementation of MAXM86161 IIO driver.
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
#include <stdio.h>
#include <string.h>
#include "iio_maxm86161.h"
#include "maxm86161.h"
#include "no_os_alloc.h"
#include "no_os_circular_buffer.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_irq.h"
#include "no_os_mutex.h"
#include "no_os_util.h"

/*
 * Circular-buffer element width. Must equal the scan_type storagebits (32 bits)
 * because the INTB handler stores a packed 32-bit transport word
 * (tag[23:19] | data[18:0]) per sample. Writing fewer bytes than storagebits
 * desyncs the stream against what the IIO client expects.
 */
#define MAXM86161_IIO_SAMPLE_SIZE sizeof(uint32_t)

enum maxm86161_iio_attr_id {
	/* PPG Configuration */
	MAXM86161_IIO_SAMPLE_RATE,
	MAXM86161_IIO_INTEGRATION_TIME,
	MAXM86161_IIO_ADC_RANGE,
	MAXM86161_IIO_SAMPLE_AVERAGING,
	MAXM86161_IIO_ALC_DISABLE,
	MAXM86161_IIO_ADD_OFFSET,
	MAXM86161_IIO_LED_SETTLING,
	MAXM86161_IIO_DIG_FILTER,

	/* FIFO Configuration */
	MAXM86161_IIO_FIFO_WATERMARK,
	MAXM86161_IIO_FIFO_ROLLOVER,
	MAXM86161_IIO_FIFO_A_FULL_TYPE,
	MAXM86161_IIO_FIFO_COUNT,

	/* LED Sequence (6 slots) */
	MAXM86161_IIO_LED_SEQ1,
	MAXM86161_IIO_LED_SEQ2,
	MAXM86161_IIO_LED_SEQ3,
	MAXM86161_IIO_LED_SEQ4,
	MAXM86161_IIO_LED_SEQ5,
	MAXM86161_IIO_LED_SEQ6,

	/* LED Amplitude */
	MAXM86161_IIO_LED1_PA,
	MAXM86161_IIO_LED2_PA,
	MAXM86161_IIO_LED3_PA,
	MAXM86161_IIO_LED_PILOT_PA,

	/* LED Range */
	MAXM86161_IIO_LED1_RANGE,
	MAXM86161_IIO_LED2_RANGE,
	MAXM86161_IIO_LED3_RANGE,

	/* Photo Diode */
	MAXM86161_IIO_PD_BIAS,

	/* Proximity */
	MAXM86161_IIO_PROX_THRESHOLD,

	/* Picket Fence */
	MAXM86161_IIO_PF_ENABLE,
	MAXM86161_IIO_PF_ORDER,

	/* Power/System */
	MAXM86161_IIO_SHUTDOWN,
	MAXM86161_IIO_LP_MODE,
	MAXM86161_IIO_BURST_EN,
	MAXM86161_IIO_BURST_RATE,

	/* Status (Read-Only) */
	MAXM86161_IIO_PART_ID,
	MAXM86161_IIO_REV_ID,
	MAXM86161_IIO_INT_STATUS,
	MAXM86161_IIO_DIE_TEMP,

	/* Streaming enable */
	MAXM86161_IIO_BUFFER_ENABLE,
};

static int maxm86161_iio_get_attr(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct maxm86161_iio_desc *iio_dev = device;
	struct maxm86161_dev *dev = iio_dev->drv_dev;
	enum maxm86161_iio_attr_id attr = (enum maxm86161_iio_attr_id)priv;
	enum maxm86161_led_src led_src;
	enum maxm86161_led_range led_range;
	enum maxm86161_led_settling settling;
	enum maxm86161_dig_filt_sel filter;
	enum maxm86161_pd_bias pd_bias;
	int32_t temperature;
	int ret;
	uint16_t int_status;
	uint8_t val8;
	bool val_bool;

	switch (attr) {
	case MAXM86161_IIO_SAMPLE_RATE:
		return snprintf(buf, len, "%u", dev->ppg_config.sample_rate);

	case MAXM86161_IIO_INTEGRATION_TIME:
		return snprintf(buf, len, "%u", dev->ppg_config.integration_time);

	case MAXM86161_IIO_ADC_RANGE:
		return snprintf(buf, len, "%u", dev->ppg_config.adc_range);

	case MAXM86161_IIO_SAMPLE_AVERAGING:
		return snprintf(buf, len, "%u", dev->ppg_config.sample_avg);

	case MAXM86161_IIO_ALC_DISABLE:
		ret = maxm86161_get_alc_disable(dev, &val_bool);
		if (ret)
			return ret;
		return snprintf(buf, len, "%d", val_bool);

	case MAXM86161_IIO_ADD_OFFSET:
		ret = maxm86161_get_add_offset(dev, &val_bool);
		if (ret)
			return ret;
		return snprintf(buf, len, "%d", val_bool);

	case MAXM86161_IIO_LED_SETTLING:
		ret = maxm86161_get_led_settling(dev, &settling);
		if (ret)
			return ret;
		return snprintf(buf, len, "%u", settling);

	case MAXM86161_IIO_DIG_FILTER:
		ret = maxm86161_get_digital_filter(dev, &filter);
		if (ret)
			return ret;
		return snprintf(buf, len, "%u", filter);

	case MAXM86161_IIO_FIFO_WATERMARK:
		return snprintf(buf, len, "%u", dev->fifo_config.watermark);

	case MAXM86161_IIO_FIFO_ROLLOVER:
		ret = maxm86161_get_fifo_rollover(dev, &val_bool);
		if (ret)
			return ret;
		return snprintf(buf, len, "%d", val_bool);

	case MAXM86161_IIO_FIFO_A_FULL_TYPE:
		ret = maxm86161_get_fifo_a_full_type(dev, &val_bool);
		if (ret)
			return ret;
		return snprintf(buf, len, "%d", val_bool);

	case MAXM86161_IIO_FIFO_COUNT:
		ret = maxm86161_get_fifo_count(dev, &val8);
		if (ret)
			return ret;
		return snprintf(buf, len, "%u", val8);

	case MAXM86161_IIO_LED_SEQ1:
	case MAXM86161_IIO_LED_SEQ2:
	case MAXM86161_IIO_LED_SEQ3:
	case MAXM86161_IIO_LED_SEQ4:
	case MAXM86161_IIO_LED_SEQ5:
	case MAXM86161_IIO_LED_SEQ6:
		ret = maxm86161_get_led_sequence(dev,
						 attr - MAXM86161_IIO_LED_SEQ1 + 1,
						 &led_src);
		if (ret)
			return ret;
		return snprintf(buf, len, "%u", led_src);

	case MAXM86161_IIO_LED1_PA:
		return snprintf(buf, len, "%u",
				dev->led_config.pulse_amplitude[0]);

	case MAXM86161_IIO_LED2_PA:
		return snprintf(buf, len, "%u",
				dev->led_config.pulse_amplitude[1]);

	case MAXM86161_IIO_LED3_PA:
		return snprintf(buf, len, "%u",
				dev->led_config.pulse_amplitude[2]);

	case MAXM86161_IIO_LED_PILOT_PA:
		ret = maxm86161_get_led_pilot_pa(dev, &val8);
		if (ret)
			return ret;
		return snprintf(buf, len, "%u", val8);

	case MAXM86161_IIO_LED1_RANGE:
	case MAXM86161_IIO_LED2_RANGE:
	case MAXM86161_IIO_LED3_RANGE:
		ret = maxm86161_get_led_range(dev,
					      (attr - MAXM86161_IIO_LED1_RANGE) + 1,
					      &led_range);
		if (ret)
			return ret;
		return snprintf(buf, len, "%u", led_range);

	case MAXM86161_IIO_PD_BIAS:
		ret = maxm86161_get_pd_bias(dev, &pd_bias);
		if (ret)
			return ret;
		return snprintf(buf, len, "%u", pd_bias);

	case MAXM86161_IIO_PROX_THRESHOLD:
		ret = maxm86161_get_prox_threshold(dev, &val8);
		if (ret)
			return ret;
		return snprintf(buf, len, "%u", val8);

	case MAXM86161_IIO_PF_ENABLE:
		return snprintf(buf, len, "%d", dev->pf_config.enable);

	case MAXM86161_IIO_PF_ORDER:
		return snprintf(buf, len, "%d", dev->pf_config.order);

	case MAXM86161_IIO_SHUTDOWN:
		ret = maxm86161_reg_read(dev, MAXM86161_REG_SYS_CTRL, &val8);
		if (ret)
			return ret;
		return snprintf(buf, len, "%d",
				!!(val8 & MAXM86161_SYS_CTRL_SHDN));

	case MAXM86161_IIO_LP_MODE:
		ret = maxm86161_reg_read(dev, MAXM86161_REG_SYS_CTRL, &val8);
		if (ret)
			return ret;
		return snprintf(buf, len, "%d",
				!!(val8 & MAXM86161_SYS_CTRL_LP_MODE));

	case MAXM86161_IIO_BURST_EN:
		ret = maxm86161_reg_read(dev, MAXM86161_REG_PPG_CFG_3, &val8);
		if (ret)
			return ret;
		return snprintf(buf, len, "%d",
				!!(val8 & MAXM86161_PPG_CFG3_BURST_EN));

	case MAXM86161_IIO_BURST_RATE:
		ret = maxm86161_reg_read(dev, MAXM86161_REG_PPG_CFG_3, &val8);
		if (ret)
			return ret;
		return snprintf(buf, len, "%u",
				no_os_field_get(MAXM86161_PPG_CFG3_BURST_RATE_MSK,
						val8));

	case MAXM86161_IIO_PART_ID:
		return snprintf(buf, len, "0x%02X", dev->part_id);

	case MAXM86161_IIO_REV_ID:
		return snprintf(buf, len, "0x%02X", dev->rev_id);

	case MAXM86161_IIO_INT_STATUS:
		ret = maxm86161_get_interrupt_status(dev, &int_status);
		if (ret)
			return ret;
		return snprintf(buf, len, "0x%04X", int_status);

	case MAXM86161_IIO_DIE_TEMP:
		ret = maxm86161_read_die_temperature(dev, &temperature);
		if (ret)
			return ret;
		/* temperature is in micro-degrees Celsius (1 degC = 1000000). */
		return snprintf(buf, len, "%ld", (long)temperature);

	case MAXM86161_IIO_BUFFER_ENABLE:
		return snprintf(buf, len, "%d", iio_dev->data_capture ? 1 : 0);

	default:
		return -EINVAL;
	}
}

static void maxm86161_iio_reset_buffer(struct maxm86161_iio_desc *iio_dev)
{
	if (!iio_dev || !iio_dev->fifo_buf)
		return;

	no_os_irq_disable(iio_dev->irq_ctrl, iio_dev->irq_id);

	iio_dev->fifo_buf->write = (struct no_os_cb_ptr) {
		0
	};
	iio_dev->fifo_buf->read  = (struct no_os_cb_ptr) {
		0
	};

	no_os_mutex_lock(iio_dev->samples_lock);
	iio_dev->samples_available = 0;
	no_os_mutex_unlock(iio_dev->samples_lock);

	no_os_irq_enable(iio_dev->irq_ctrl, iio_dev->irq_id);
}

static int maxm86161_iio_set_buffer_enable(struct maxm86161_iio_desc *iio_dev,
		bool enable)
{
	int ret;

	if (enable) {
		maxm86161_iio_reset_buffer(iio_dev);
		iio_dev->data_capture = true;

		ret = maxm86161_set_shutdown(iio_dev->drv_dev, false);
		if (ret) {
			iio_dev->data_capture = false;
			return ret;
		}

		return 0;
	}

	ret = maxm86161_set_shutdown(iio_dev->drv_dev, true);
	if (ret)
		return ret;

	iio_dev->data_capture = false;
	maxm86161_iio_reset_buffer(iio_dev);

	return 0;
}

static int maxm86161_iio_set_attr(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct maxm86161_iio_desc *iio_dev = device;
	struct maxm86161_dev *dev = iio_dev->drv_dev;
	enum maxm86161_iio_attr_id attr = (enum maxm86161_iio_attr_id)priv;
	uint32_t val;

	val = strtoul(buf, NULL, 0);

	switch (attr) {
	case MAXM86161_IIO_SAMPLE_RATE:
		return maxm86161_set_sample_rate(dev, (uint8_t)val);

	case MAXM86161_IIO_INTEGRATION_TIME:
		return maxm86161_set_integration_time(dev, (enum maxm86161_ppg_tint)val);

	case MAXM86161_IIO_ADC_RANGE:
		return maxm86161_set_adc_range(dev, (enum maxm86161_adc_range)val);

	case MAXM86161_IIO_SAMPLE_AVERAGING:
		return maxm86161_set_sample_averaging(dev, (enum maxm86161_smp_ave)val);

	case MAXM86161_IIO_ALC_DISABLE:
		return maxm86161_set_alc_disable(dev, (bool)val);

	case MAXM86161_IIO_ADD_OFFSET:
		return maxm86161_set_add_offset(dev, (bool)val);

	case MAXM86161_IIO_LED_SETTLING:
		return maxm86161_set_led_settling(dev, (enum maxm86161_led_settling)val);

	case MAXM86161_IIO_DIG_FILTER:
		return maxm86161_set_digital_filter(dev, (enum maxm86161_dig_filt_sel)val);

	case MAXM86161_IIO_FIFO_WATERMARK:
		return maxm86161_set_fifo_watermark(dev, (uint8_t)val);

	case MAXM86161_IIO_FIFO_ROLLOVER:
		return maxm86161_set_fifo_rollover(dev, (bool)val);

	case MAXM86161_IIO_FIFO_A_FULL_TYPE:
		return maxm86161_set_fifo_a_full_type(dev, (bool)val);

	case MAXM86161_IIO_LED_SEQ1:
	case MAXM86161_IIO_LED_SEQ2:
	case MAXM86161_IIO_LED_SEQ3:
	case MAXM86161_IIO_LED_SEQ4:
	case MAXM86161_IIO_LED_SEQ5:
	case MAXM86161_IIO_LED_SEQ6:
		return maxm86161_set_led_sequence(dev,
						  attr - MAXM86161_IIO_LED_SEQ1 + 1,
						  (enum maxm86161_led_src)val);

	case MAXM86161_IIO_LED1_PA:
		return maxm86161_set_led_pulse_amplitude(dev, 1, (uint8_t)val);

	case MAXM86161_IIO_LED2_PA:
		return maxm86161_set_led_pulse_amplitude(dev, 2, (uint8_t)val);

	case MAXM86161_IIO_LED3_PA:
		return maxm86161_set_led_pulse_amplitude(dev, 3, (uint8_t)val);

	case MAXM86161_IIO_LED_PILOT_PA:
		return maxm86161_set_led_pilot_pa(dev, (uint8_t)val);

	case MAXM86161_IIO_LED1_RANGE:
		return maxm86161_set_led_range(dev, 1, (enum maxm86161_led_range)val);

	case MAXM86161_IIO_LED2_RANGE:
		return maxm86161_set_led_range(dev, 2, (enum maxm86161_led_range)val);

	case MAXM86161_IIO_LED3_RANGE:
		return maxm86161_set_led_range(dev, 3, (enum maxm86161_led_range)val);

	case MAXM86161_IIO_PD_BIAS:
		return maxm86161_set_pd_bias(dev, (enum maxm86161_pd_bias)val);

	case MAXM86161_IIO_PROX_THRESHOLD:
		return maxm86161_set_prox_threshold(dev, (uint8_t)val);

	case MAXM86161_IIO_PF_ENABLE: {
		struct maxm86161_picket_fence_config pf = dev->pf_config;
		pf.enable = (bool)val;
		return maxm86161_set_picket_fence(dev, &pf);
	}

	case MAXM86161_IIO_PF_ORDER: {
		struct maxm86161_picket_fence_config pf = dev->pf_config;
		pf.order = (bool)val;
		return maxm86161_set_picket_fence(dev, &pf);
	}

	case MAXM86161_IIO_SHUTDOWN:
		return maxm86161_set_shutdown(dev, (bool)val);

	case MAXM86161_IIO_LP_MODE:
		return maxm86161_set_low_power_mode(dev, (bool)val);

	case MAXM86161_IIO_BURST_EN:
		return maxm86161_set_burst_mode(dev, (bool)val, MAXM86161_BURST_RATE_8HZ);

	case MAXM86161_IIO_BURST_RATE:
		return maxm86161_set_burst_mode(dev, true, (enum maxm86161_burst_rate)val);

	case MAXM86161_IIO_BUFFER_ENABLE:
		return maxm86161_iio_set_buffer_enable(iio_dev, (bool)val);

	default:
		return -EINVAL;
	}
}

static int32_t maxm86161_iio_reg_read(void *device, uint32_t reg, uint32_t *val)
{
	struct maxm86161_iio_desc *iio_dev = device;
	uint8_t reg_val;
	int ret;

	if (reg > MAXM86161_REG_PART_ID) {
		return -EINVAL;
	}

	ret = maxm86161_reg_read(iio_dev->drv_dev, (uint8_t)reg, &reg_val);
	if (ret)
		return ret;

	*val = reg_val;
	return 0;
}

static int32_t maxm86161_iio_reg_write(void *device, uint32_t reg, uint32_t val)
{
	struct maxm86161_iio_desc *iio_dev = device;

	if (reg > MAXM86161_REG_PART_ID || val > UINT8_MAX) {
		return -EINVAL;
	}

	return maxm86161_reg_write(iio_dev->drv_dev, (uint8_t)reg, (uint8_t)val);
}

static int32_t maxm86161_iio_submit(struct iio_device_data *dev_data)
{
	struct maxm86161_iio_desc *iio_dev;
	uint32_t count;
	uint32_t data;
	uint32_t samples_read = 0;
	int ret;
	bool overrun = false;

	if (!dev_data)
		return -EINVAL;

	iio_dev = (struct maxm86161_iio_desc *)dev_data->dev;
	if (!iio_dev->drv_dev)
		return -EINVAL;

	count = dev_data->buffer->samples;

	for (size_t i = 0; i < count; i++) {
		data = 0;
		ret = no_os_cb_read(iio_dev->fifo_buf, &data,
				    MAXM86161_IIO_SAMPLE_SIZE);
		if (ret == -NO_OS_EOVERRUN) {
			overrun = true;
		} else if (ret) {
			break;
		}

		samples_read++;

		ret = iio_buffer_push_scan(dev_data->buffer, &data);
		if (ret)
			break;
	}


	no_os_mutex_lock(iio_dev->samples_lock);
	if (overrun) {
		uint32_t cap = iio_dev->fifo_buf->size / MAXM86161_IIO_SAMPLE_SIZE;

		iio_dev->samples_available =
			(samples_read < cap) ? (cap - samples_read) : 0;
	} else {
		iio_dev->samples_available =
			(iio_dev->samples_available > samples_read) ?
			(iio_dev->samples_available - samples_read) : 0;
	}
	no_os_mutex_unlock(iio_dev->samples_lock);

	if (ret && ret != -NO_OS_EOVERRUN) {
		return ret;
	}

	return samples_read;
}

void maxm86161_iio_fifo_irq_handler(void *ctx)
{
	struct maxm86161_iio_desc *iio_dev = (struct maxm86161_iio_desc *)ctx;
	uint32_t raw_data;
	uint8_t samples_written = 0;
	uint8_t fifo_count, overflow_count;
	uint8_t status1_data;
	uint8_t *sample_ptr;
	uint8_t tag;
	int ret;

	if (!iio_dev || !iio_dev->drv_dev)
		return;

	if (!iio_dev->data_capture)
		return;

	ret = maxm86161_reg_read(iio_dev->drv_dev, MAXM86161_REG_INT_STATUS_1,
				 &status1_data);
	if (ret)
		return;

	if (!no_os_field_get(MAXM86161_INT1_A_FULL | MAXM86161_INT1_DATA_RDY,
			     status1_data))
		return;

	ret = maxm86161_get_fifo_count(iio_dev->drv_dev, &fifo_count);
	if (ret)
		return;

	if (fifo_count == 0)
		return;

	if (fifo_count > MAXM86161_FIFO_DEPTH)
		fifo_count = MAXM86161_FIFO_DEPTH;

	ret = maxm86161_fifo_overflow_check(iio_dev->drv_dev, &overflow_count);
	if (ret)
		return;

	ret = maxm86161_read_fifo_data(iio_dev->drv_dev, iio_dev->fifo_read_buf,
				       fifo_count);
	if (ret)
		return;

	for (uint8_t i = 0; i < fifo_count; i++) {
		uint32_t packed;

		/* decode data */
		sample_ptr = &iio_dev->fifo_read_buf[i * MAXM86161_FIFO_DATA_SIZE];
		ret = maxm86161_decode_fifo_sample(sample_ptr, &tag, &raw_data);
		if (ret)
			continue;

		/*
		 * Transport contract (must match maxm86161_scan_type):
		 * one 32-bit software sample = tag[23:19] | data[18:0]. Repack the
		 * decoded tag and 19-bit measurement so the circular-buffer element
		 * width equals the IIO scan storagebits (32) and the tag survives to
		 * the host, which demuxes LED slots and reads the unsigned data field.
		 */
		packed = MAXM86161_FIFO_PACK(tag, raw_data);

		/* place into buffer */
		ret = no_os_cb_write(iio_dev->fifo_buf, &packed,
				     MAXM86161_IIO_SAMPLE_SIZE);
		if (ret) {
			iio_dev->sample_ovf_count++;
			continue;
		}

		samples_written++;
	}

	if (samples_written > 0) {
		uint32_t cap = iio_dev->fifo_buf->size / MAXM86161_IIO_SAMPLE_SIZE;

		no_os_mutex_lock(iio_dev->samples_lock);
		iio_dev->samples_available += samples_written;
		/* clamp samples if over max limit of buffer */
		if (iio_dev->samples_available > cap) {
			iio_dev->samples_available = cap;
		}
		no_os_mutex_unlock(iio_dev->samples_lock);
	}
}

static struct iio_attribute maxm86161_debug_attrs[] = {
	MAXM86161_ATTR("sample_rate", MAXM86161_IIO_SAMPLE_RATE),
	MAXM86161_ATTR("integration_time", MAXM86161_IIO_INTEGRATION_TIME),
	MAXM86161_ATTR("adc_range", MAXM86161_IIO_ADC_RANGE),
	MAXM86161_ATTR("sample_averaging", MAXM86161_IIO_SAMPLE_AVERAGING),
	MAXM86161_ATTR("alc_disable", MAXM86161_IIO_ALC_DISABLE),
	MAXM86161_ATTR("add_offset", MAXM86161_IIO_ADD_OFFSET),
	MAXM86161_ATTR("led_settling", MAXM86161_IIO_LED_SETTLING),
	MAXM86161_ATTR("dig_filter", MAXM86161_IIO_DIG_FILTER),
	MAXM86161_ATTR("fifo_watermark", MAXM86161_IIO_FIFO_WATERMARK),
	MAXM86161_ATTR("fifo_rollover", MAXM86161_IIO_FIFO_ROLLOVER),
	MAXM86161_ATTR("fifo_a_full_type", MAXM86161_IIO_FIFO_A_FULL_TYPE),
	MAXM86161_ATTR_RO("fifo_count", MAXM86161_IIO_FIFO_COUNT),
	MAXM86161_ATTR("led_seq1", MAXM86161_IIO_LED_SEQ1),
	MAXM86161_ATTR("led_seq2", MAXM86161_IIO_LED_SEQ2),
	MAXM86161_ATTR("led_seq3", MAXM86161_IIO_LED_SEQ3),
	MAXM86161_ATTR("led_seq4", MAXM86161_IIO_LED_SEQ4),
	MAXM86161_ATTR("led_seq5", MAXM86161_IIO_LED_SEQ5),
	MAXM86161_ATTR("led_seq6", MAXM86161_IIO_LED_SEQ6),
	MAXM86161_ATTR("led1_pa", MAXM86161_IIO_LED1_PA),
	MAXM86161_ATTR("led2_pa", MAXM86161_IIO_LED2_PA),
	MAXM86161_ATTR("led3_pa", MAXM86161_IIO_LED3_PA),
	MAXM86161_ATTR("led_pilot_pa", MAXM86161_IIO_LED_PILOT_PA),
	MAXM86161_ATTR("led1_range", MAXM86161_IIO_LED1_RANGE),
	MAXM86161_ATTR("led2_range", MAXM86161_IIO_LED2_RANGE),
	MAXM86161_ATTR("led3_range", MAXM86161_IIO_LED3_RANGE),
	MAXM86161_ATTR("pd_bias", MAXM86161_IIO_PD_BIAS),
	MAXM86161_ATTR("prox_threshold", MAXM86161_IIO_PROX_THRESHOLD),
	MAXM86161_ATTR("picket_fence_enable", MAXM86161_IIO_PF_ENABLE),
	MAXM86161_ATTR("picket_fence_order", MAXM86161_IIO_PF_ORDER),
	MAXM86161_ATTR("shutdown", MAXM86161_IIO_SHUTDOWN),
	MAXM86161_ATTR("low_power_mode", MAXM86161_IIO_LP_MODE),
	MAXM86161_ATTR("burst_enable", MAXM86161_IIO_BURST_EN),
	MAXM86161_ATTR("burst_rate", MAXM86161_IIO_BURST_RATE),
	MAXM86161_ATTR_RO("part_id", MAXM86161_IIO_PART_ID),
	MAXM86161_ATTR_RO("rev_id", MAXM86161_IIO_REV_ID),
	MAXM86161_ATTR_RO("interrupt_status", MAXM86161_IIO_INT_STATUS),
	MAXM86161_ATTR_RO("die_temperature", MAXM86161_IIO_DIE_TEMP),
	MAXM86161_ATTR("buffer_enable", MAXM86161_IIO_BUFFER_ENABLE),
	END_ATTRIBUTES_ARRAY
};

/*
 * Each scan element is a 32-bit word carrying the 5-bit FIFO tag in bits
 * [23:19] and the unsigned 19-bit measurement in bits [18:0] (24 valid bits
 * total). The channel is unsigned because the tag occupies the high bits and
 * MAXM86161 PPG data is unsigned; the host splits the tag from the data field.
 * See MAXM86161_FIFO_PACK() for the producer side in the INTB handler.
 */
static struct scan_type maxm86161_scan_type = {
	.sign = 'u',
	.realbits = 24,
	.storagebits = 32,
	.shift = 0,
	.is_big_endian = false,
};

static struct iio_channel maxm86161_channels[] = {
	{
		.ch_type = IIO_VOLTAGE,
		.ch_out = false,
		.indexed = true,
		.channel = 0,
		.scan_index = 0,
		.scan_type = &maxm86161_scan_type,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_device maxm86161_iio_device = {
	.num_ch = 1,
	.channels = maxm86161_channels,
	.debug_attributes = maxm86161_debug_attrs,
	.debug_reg_read = maxm86161_iio_reg_read,
	.debug_reg_write = maxm86161_iio_reg_write,
	.submit = maxm86161_iio_submit,
};

int maxm86161_iio_init(struct maxm86161_iio_desc **iio_desc,
		       struct maxm86161_iio_init_param *init_param)
{
	struct maxm86161_iio_desc *desc;
	uint32_t buf_size;
	int ret;

	if (!iio_desc ||
	    !init_param ||
	    !init_param->drv_init_param || /* base driver must be initializable */
	    !init_param->irq_ctrl
	    || /* IRQ controller and id are required for IIO buffer operation */
	    !init_param->drv_init_param->irq_init) /* IRQ must be enabled for IIO to be used */
		return -EINVAL;

	desc = no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	ret = maxm86161_init(&desc->drv_dev, init_param->drv_init_param);
	if (ret)
		goto err_free_desc;

	desc->irq_ctrl = init_param->irq_ctrl;
	desc->irq_id = init_param->irq_id;
	desc->data_capture = false;
	desc->samples_available = 0;

	buf_size = init_param->fifo_buf_size * MAXM86161_IIO_SAMPLE_SIZE;
	ret = no_os_cb_init(&desc->fifo_buf, buf_size);
	if (ret)
		goto err_remove_dev;

	no_os_mutex_init(&desc->samples_lock);

	ret = maxm86161_set_fifo_watermark(desc->drv_dev,
					   init_param->fifo_watermark);
	if (ret)
		goto err_remove_mutex;

	ret = maxm86161_set_irq_handler(desc->drv_dev, MAXM86161_INT_A_FULL,
					true, maxm86161_iio_fifo_irq_handler,
					(void *)desc);
	if (ret)
		goto err_remove_mutex;

	desc->iio_dev = &maxm86161_iio_device;
	*iio_desc = desc;

	ret = maxm86161_set_shutdown(desc->drv_dev, true);
	if (ret) {
		goto err_remove_mutex;
	}

	return 0;

	return 0;

err_remove_mutex:
	no_os_mutex_remove(desc->samples_lock);
	no_os_cb_remove(desc->fifo_buf);
err_remove_dev:
	maxm86161_remove(desc->drv_dev);
err_free_desc:
	no_os_free(desc);
	return ret;
}

int maxm86161_iio_remove(struct maxm86161_iio_desc *iio_desc)
{
	if (!iio_desc)
		return -EINVAL;

	/* Detach the FIFO callback from the driver-level IRQ dispatcher. */
	if (iio_desc->drv_dev)
		maxm86161_set_irq_handler(iio_desc->drv_dev, MAXM86161_INT_A_FULL,
					  false, NULL, NULL);

	if (iio_desc->fifo_buf)
		no_os_cb_remove(iio_desc->fifo_buf);

	no_os_mutex_remove(iio_desc->samples_lock);

	maxm86161_remove(iio_desc->drv_dev);
	no_os_free(iio_desc);

	return 0;
}

int maxm86161_iio_get_dev_descriptor(struct maxm86161_iio_desc *iio_desc,
				     struct iio_device **iio_dev)
{
	if (!iio_desc || !iio_dev)
		return -EINVAL;

	*iio_dev = iio_desc->iio_dev;
	return 0;
}
