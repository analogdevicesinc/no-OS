/***************************************************************************//**
 *   @file   cn0391.c
 *   @brief  Implementation of CN0391 board support.
 *   @author Mircea Vlasin (mircea.vlasin@analog.com)
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
#include "cn0391.h"
#include "no_os_thermocouple.h"
#include "no_os_rtd.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_print_log.h"

/* ADC channel triplets indexed by IIO channel (CH0=0 .. CH3=3) */
static const struct cn0391_adc_ch_map cn0391_adc_map[CN0391_NUM_IIO_CHANNELS] =
{
	[0] = { .tc = 9,  .r5 = 10, .rtd = 11, .iout_ain = 7 }, /* IIO CH0: AIN6, AIN8, AIN7 */
	[1] = { .tc = 6,  .r5 = 7,  .rtd = 8,  .iout_ain = 5 }, /* IIO CH1: AIN4, AIN8, AIN5 */
	[2] = { .tc = 3,  .r5 = 4,  .rtd = 5,  .iout_ain = 3 }, /* IIO CH2: AIN2, AIN8, AIN3 */
	[3] = { .tc = 0,  .r5 = 1,  .rtd = 2,  .iout_ain = 1 }, /* IIO CH3: AIN0, AIN8, AIN1 */
};

/**
 * @brief Read the next conversion result and verify it belongs to the
 *        expected channel. Called after single conversion mode is started
 *        with only 3 channels enabled (tc, r5, rtd) in ascending order,
 *        so each call is guaranteed to return the next channel in sequence.
 * @param dev - AD7124 device descriptor.
 * @param channel - Expected channel number.
 * @param code - Pointer to store the raw ADC code.
 * @return 0 on success, -EINVAL if the result is from an unexpected channel,
 *         or negative error code otherwise.
 */
static int cn0391_read_channel(struct ad7124_dev *dev, uint8_t channel,
			       int32_t *code)
{
	int ret;
	int32_t read_data;
	uint32_t status;

	ret = ad7124_wait_for_conv_ready(dev, 10000);
	if (ret)
		return ret;

	ret = ad7124_read_data(dev, &read_data);
	if (ret)
		return ret;

	ret = ad7124_get_read_chan_id(dev, &status);
	if (ret)
		return ret;

	if (status != channel)
		return -EINVAL;

	*code = read_data;

	return 0;
}

int cn0391_init(struct cn0391_dev **dev,
		struct cn0391_init_param *init_param)
{
	struct cn0391_dev *d;
	int ret;

	d = (struct cn0391_dev *)no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	ret = ad7124_setup(&d->ad7124_dev, init_param->ad7124_init);
	if (ret)
		goto error_free;

	/* Configure IOUT0 = 750µA; pin is switched per-channel at read time */
	d->ad7124_dev->regs[AD7124_IOCon1].value =
		AD7124_IO_CTRL1_REG_IOUT0(5) |
		AD7124_IO_CTRL1_REG_IOUT_CH0(cn0391_adc_map[CN0391_CH0_ID].iout_ain);
	ret = ad7124_write_register(d->ad7124_dev,
				    d->ad7124_dev->regs[AD7124_IOCon1]);
	if (ret)
		goto error_ad7124;

	/* Stop the continuous conversion started by ad7124_setup; measurements
	 * are triggered on demand via single conversion in cn0391_read_temperature. */
	ret = ad7124_set_adc_mode(d->ad7124_dev, AD7124_IDLE);
	if (ret)
		goto error_ad7124;

	d->cache_ch = -1;
	*dev = d;

	return 0;

error_ad7124:
	ad7124_remove(d->ad7124_dev);
error_free:
	no_os_free(d);

	return ret;
}

int cn0391_remove(struct cn0391_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad7124_remove(dev->ad7124_dev);
	no_os_free(dev);

	return ret;
}

int cn0391_read_temperature(struct cn0391_dev *dev, uint8_t ch_idx,
			    double *hot_junction_temp,
			    double *cold_junction_temp,
			    double *thermocouple_voltage,
			    double *rtd_resistance)
{
	int ret;
	int32_t thermo_code, ref_code, cjc_code;
	double v_tc, v_r5, current, v_r1, r_rtd;
	double t_cold, v_cj, v_total;
	const struct cn0391_adc_ch_map *map;
	uint8_t i;

	if (ch_idx >= CN0391_NUM_IIO_CHANNELS)
		return -EINVAL;

	map = &cn0391_adc_map[ch_idx];

	/* Switch IOUT0 to the RTD excitation pin for this channel.
	 * The Sinc4 settling time of the TC conversion that follows provides
	 * adequate settling time for the RTD current source. */
	dev->ad7124_dev->regs[AD7124_IOCon1].value =
		AD7124_IO_CTRL1_REG_IOUT0(5) |
		AD7124_IO_CTRL1_REG_IOUT_CH0(map->iout_ain);
	ret = ad7124_write_register(dev->ad7124_dev,
				    dev->ad7124_dev->regs[AD7124_IOCon1]);
	if (ret)
		return ret;

	/* Pause any ongoing conversion before reconfiguring channels */
	ret = ad7124_set_adc_mode(dev->ad7124_dev, AD7124_IDLE);
	if (ret)
		return ret;

	/*
	 * Enable only the 3 ADC channels needed for this IIO channel and
	 * disable all others. This avoids waiting through the full 12-channel
	 * rotation in continuous mode.
	 */
	for (i = 0; i < CN0391_NUM_IIO_CHANNELS * CN0391_ADC_CHANNELS_PER_IIO_CH; i++) {
		ret = ad7124_set_channel_status(dev->ad7124_dev, i,
						i == map->tc ||
						i == map->r5  ||
						i == map->rtd);
		if (ret)
			return ret;
	}

	/*
	 * Single conversion mode: the ADC converts each enabled channel exactly
	 * once in ascending channel-number order (tc < r5 < rtd), then returns
	 * to standby automatically. No retries are needed in cn0391_read_channel
	 * since the results arrive in the expected sequence.
	 */
	ret = ad7124_set_adc_mode(dev->ad7124_dev, AD7124_SINGLE);
	if (ret)
		return ret;

	ret = cn0391_read_channel(dev->ad7124_dev, map->tc, &thermo_code);
	if (ret)
		return ret;

	ret = cn0391_read_channel(dev->ad7124_dev, map->r5, &ref_code);
	if (ret)
		return ret;

	ret = cn0391_read_channel(dev->ad7124_dev, map->rtd, &cjc_code);
	if (ret)
		return ret;

	/* Thermocouple voltage (mV) */
	v_tc = CN0391_ADC_REF_VOLTAGE *
	       (double)(thermo_code - CN0391_ADC_HALF_RESOLUTION) /
	       ((double)CN0391_ADC_HALF_RESOLUTION * CN0391_GAIN) * 1000.0;

	/* Reference resistor voltage -> current */
	v_r5 = CN0391_ADC_REF_VOLTAGE *
	       (double)(ref_code - CN0391_ADC_HALF_RESOLUTION) /
	       ((double)CN0391_ADC_HALF_RESOLUTION * CN0391_RTD_GAIN);
	current = v_r5 / CN0391_THERMO_RES;

	/* RTD voltage -> resistance */
	v_r1 = CN0391_ADC_REF_VOLTAGE *
	       (double)(cjc_code - CN0391_ADC_HALF_RESOLUTION) /
	       ((double)CN0391_ADC_HALF_RESOLUTION * CN0391_RTD_GAIN);
	r_rtd = v_r1 / current;

	/* Cold junction temperature from RTD */
	t_cold = no_os_pt1000_resistance_to_temp(r_rtd);

	/* Cold junction compensation */
	v_cj = no_os_typek_temp_to_voltage(t_cold);
	v_total = v_tc + v_cj;

	/* Hot junction temperature */
	*hot_junction_temp = no_os_typek_voltage_to_temp(v_total);
	*cold_junction_temp = t_cold;
	*thermocouple_voltage = v_tc;
	*rtd_resistance = r_rtd;

	return 0;
}