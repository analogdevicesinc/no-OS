/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ADE7913 example project
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "common_data.h"
#include "platform.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Toggle Led
 * @param gpio_led_desc - led descriptor
 * @return 0 in case of success, negative error code otherwise.
 */
int interface_toggle_led(struct no_os_gpio_desc *gpio_led_desc)
{
	int ret;
	uint8_t val;

	if (!gpio_led_desc)
		return -EINVAL;

	ret = no_os_gpio_get_value(gpio_led_desc, &val);
	if (ret)
		return ret;

	if (val == NO_OS_GPIO_LOW)
		val = NO_OS_GPIO_HIGH;
	else
		val = NO_OS_GPIO_LOW;

	return no_os_gpio_set_value(gpio_led_desc, val);
}

/**
 * @brief Saves the current and voltage values of device 1 in rms_adc structure
 * @param dev - device structure
 * @param rms - structure holding the measurements values
 * @return 0 in case of success, negative error code otherwise
 */
int rms_adc_values_read(struct ade7913_dev *dev, struct rms_adc_values *rms)
{
	int32_t i_val, v1_val, v2_val, v1_rms, v2_rms, i_rms;

	if (!dev)
		return -ENODEV;
	if (!rms)
		return -EINVAL;

	rms->v1_rms_adc[0] = (int32_t)rms_filter_v1((int32_t)dev->v1_wav_m[0]);
	rms->v2_rms_adc[0] = (int32_t)rms_filter_v2((int32_t)dev->v2_wav_m[0]);
	rms->i_rms_adc[0] = (int32_t)rms_filter_i((int32_t)dev->i_wav_m[0]);

	rms->i_rms[0] = (((int64_t)((int64_t)(int64_t)rms->i_rms_adc[0]))
			 * ADE7913_VREF_I) /
			(1 << 23);
	rms->v1_rms[0] = (((int64_t)((int64_t)(supply_scale_v((int64_t)
					       rms->v1_rms_adc[0]))))
			  * ADE7913_VREF_V) / (1 << 23);
	rms->v2_rms[0] = (((int64_t)((int64_t)(supply_scale_v((int64_t)
					       rms->v2_rms_adc[0]))))
			  * ADE7913_VREF_V) / (1 << 23);

	return 0;
}

/**
 * @brief Saves the current and voltage values of device 2 in rms_adc structure
 * @param dev - device structure
 * @param rms - structure holding the measurements values
 * @return 0 in case of success, negative error code otherwise
 */
int rms_adc_values_read_2(struct ade7913_dev *dev, struct rms_adc_values *rms)
{
	int32_t i_val, v1_val, v2_val, v1_rms, v2_rms, i_rms;

	if (!dev)
		return -ENODEV;
	if (!rms)
		return -EINVAL;

	rms->v1_rms_adc[1] = (int32_t)rms_filter_v1_2((int32_t)dev->v1_wav_m[1]);
	rms->v2_rms_adc[1] = (int32_t)rms_filter_v2_2((int32_t)dev->v2_wav_m[1]);
	rms->i_rms_adc[1] = (int32_t)rms_filter_i_2((int32_t)dev->i_wav_m[1]);

	rms->i_rms[1] = (((int64_t)((int64_t)(int64_t)rms->i_rms_adc[1]))
			 * ADE7913_VREF_I) /
			(1 << 23);
	rms->v1_rms[1] = (((int64_t)((int64_t)(supply_scale_v((int64_t)
					       rms->v1_rms_adc[1]))))
			  * ADE7913_VREF_V) / (1 << 23);
	rms->v2_rms[1] = (((int64_t)((int64_t)(supply_scale_v((int64_t)
					       rms->v2_rms_adc[1]))))
			  * ADE7913_VREF_V) / (1 << 23);

	return 0;
}

/**
 * @brief Saves the current and voltage values of device 3 in rms_adc structure
 * @param dev - device structure
 * @param rms - structure holding the measurements values
 * @return 0 in case of success, negative error code otherwise
 */
int rms_adc_values_read_3(struct ade7913_dev *dev, struct rms_adc_values *rms)
{
	int32_t i_val, v1_val, v2_val, v1_rms, v2_rms, i_rms;

	if (!dev)
		return -ENODEV;
	if (!rms)
		return -EINVAL;

	rms->v1_rms_adc[2] = (int32_t)rms_filter_v1_3((int32_t)dev->v1_wav_m[2]);
	rms->v2_rms_adc[2] = (int32_t)rms_filter_v2_3((int32_t)dev->v2_wav_m[2]);
	rms->i_rms_adc[2] = (int32_t)rms_filter_i_3((int32_t)dev->i_wav_m[2]);

	rms->i_rms[2] = (((int64_t)((int64_t)(int64_t)rms->i_rms_adc[2]))
			 * ADE7913_VREF_I) /
			(1 << 23);
	rms->v1_rms[2] = (((int64_t)((int64_t)(supply_scale_v((int64_t)
					       rms->v1_rms_adc[2]))))
			  * ADE7913_VREF_V) / (1 << 23);
	rms->v2_rms[2] = (((int64_t)((int64_t)(supply_scale_v((int64_t)
					       rms->v2_rms_adc[2]))))
			  * ADE7913_VREF_V) / (1 << 23);

	return 0;
}

/**
 * @brief Compute v1 rms value for device 1
 * @param sample - v1 sample dev 1
 * @return v1 rms value
 */
int64_t rms_filter_v1(int32_t sample)
{
	static int32_t rms = INITIAL;
	static int64_t sum_squares = 1UL * SAMPLES * INITIAL * INITIAL;

	sum_squares -= sum_squares / SAMPLES;
	sum_squares += (int64_t) sample * sample;
	if (rms == 0) rms = 1;    /* do not divide by zero */
	rms = (rms + sum_squares / SAMPLES / rms) / 2;
	return rms;
}

/**
 * @brief Compute v2 rms value for device 1
 * @param sample - v2 sample dev 1
 * @return v2 rms value
 */
int64_t rms_filter_v2(int32_t sample)
{
	static int32_t rms = INITIAL;
	static int64_t sum_squares = 1UL * SAMPLES * INITIAL * INITIAL;

	sum_squares -= sum_squares / SAMPLES;
	sum_squares += (int64_t) sample * sample;
	if (rms == 0) rms = 1;    /* do not divide by zero */
	rms = (rms + sum_squares / SAMPLES / rms) / 2;
	return rms;
}

/**
 * @brief Compute i rms value for device 1
 * @param sample - i sample dev 1
 * @return i rms value
 */
int64_t rms_filter_i(int32_t sample)
{
	static int32_t rms = INITIAL;
	static int64_t sum_squares = 1UL * SAMPLES * INITIAL * INITIAL;

	sum_squares -= sum_squares / SAMPLES;
	sum_squares += (int64_t) sample * sample;
	if (rms == 0) rms = 1;    /* do not divide by zero */
	rms = (rms + sum_squares / SAMPLES / rms) / 2;
	return rms;
}

/**
 * @brief Compute v1_2 rms value for device 2
 * @param sample - v1 sample dev 2
 * @return v1 rms value
 */
int64_t rms_filter_v1_2(int32_t sample)
{
	static int32_t rms = INITIAL;
	static int64_t sum_squares = 1UL * SAMPLES * INITIAL * INITIAL;

	sum_squares -= sum_squares / SAMPLES;
	sum_squares += (int64_t) sample * sample;
	if (rms == 0) rms = 1;    /* do not divide by zero */
	rms = (rms + sum_squares / SAMPLES / rms) / 2;
	return rms;
}

/**
 * @brief Compute v2_2 rms value for device 2
 * @param sample - v2 sample dev 2
 * @return v2 rms value
 */
int64_t rms_filter_v2_2(int32_t sample)
{
	static int32_t rms = INITIAL;
	static int64_t sum_squares = 1UL * SAMPLES * INITIAL * INITIAL;

	sum_squares -= sum_squares / SAMPLES;
	sum_squares += (int64_t) sample * sample;
	if (rms == 0) rms = 1;    /* do not divide by zero */
	rms = (rms + sum_squares / SAMPLES / rms) / 2;
	return rms;
}

/**
 * @brief Compute i_2 rms value for device 2
 * @param sample - i sample dev 2
 * @return i rms value
 */
int64_t rms_filter_i_2(int32_t sample)
{
	static int32_t rms = INITIAL;
	static int64_t sum_squares = 1UL * SAMPLES * INITIAL * INITIAL;

	sum_squares -= sum_squares / SAMPLES;
	sum_squares += (int64_t) sample * sample;
	if (rms == 0) rms = 1;    /* do not divide by zero */
	rms = (rms + sum_squares / SAMPLES / rms) / 2;
	return rms;
}

/**
 * @brief Compute v1_3 rms value for device 3
 * @param sample - v1 sample dev 3
 * @return v1 rms value
 */
int64_t rms_filter_v1_3(int32_t sample)
{
	static int32_t rms = INITIAL;
	static int64_t sum_squares = 1UL * SAMPLES * INITIAL * INITIAL;

	sum_squares -= sum_squares / SAMPLES;
	sum_squares += (int64_t) sample * sample;
	if (rms == 0) rms = 1;    /* do not divide by zero */
	rms = (rms + sum_squares / SAMPLES / rms) / 2;
	return rms;
}

/**
 * @brief Compute v2_3 rms value for device 3
 * @param sample - v2 sample dev 3
 * @return v2 rms value
 */
int64_t rms_filter_v2_3(int32_t sample)
{
	static int32_t rms = INITIAL;
	static int64_t sum_squares = 1UL * SAMPLES * INITIAL * INITIAL;

	sum_squares -= sum_squares / SAMPLES;
	sum_squares += (int64_t) sample * sample;
	if (rms == 0) rms = 1;    /* do not divide by zero */
	rms = (rms + sum_squares / SAMPLES / rms) / 2;
	return rms;
}

/**
 * @brief Compute i_3 rms value for device 3
 * @param sample - i sample dev 3
 * @return i rms value
 */
int64_t rms_filter_i_3(int32_t sample)
{
	static int32_t rms = INITIAL;
	static int64_t sum_squares = 1UL * SAMPLES * INITIAL * INITIAL;

	sum_squares -= sum_squares / SAMPLES;
	sum_squares += (int64_t) sample * sample;
	if (rms == 0) rms = 1;    /* do not divide by zero */
	rms = (rms + sum_squares / SAMPLES / rms) / 2;
	return rms;
}

/**
 * @brief Scale V value
 * @param val - value to be scaled
 * @return scaled value
 */
int64_t supply_scale_v(int32_t val)
{
	int64_t v;

	v = (int64_t)val;
	v = v * V_DIVIDER;

	return (int64_t)v;
}
