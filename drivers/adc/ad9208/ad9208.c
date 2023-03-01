/***************************************************************************//**
 *   @file   ad9208.c
 *   @brief  Implementation of AD9208 Driver.
 *   @author Stefan Popa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "ad9208.h"
#include <inttypes.h>
#include "api_def.h"
#include "no_os_alloc.h"

/**
 * Spi write and read compatible with ad9208 API
 * @param user_data
 * @param wbuf Pointer to array with the data to be sent on the SPI
 * @param rbuf Pointer to array where the data to which the SPI will be written
 * @param len The size in bytes allocated for each of the indata and outdata arrays.
 * @return 0 for success, any non-zero value indicates an error
 */
static int ad9208_spi_xfer(void *user_data, uint8_t *wbuf,
			   uint8_t *rbuf, int len)
{
	struct no_os_spi_desc *spi = user_data;
	uint8_t * buffer = (uint8_t *) no_os_malloc(len);
	int32_t ret;

	memcpy(buffer, wbuf, 3);

	ret = no_os_spi_write_and_read(spi, buffer, len);
	if (ret < 0)
		printf("Read Error %"PRId32, ret);
	else
		memcpy(rbuf, buffer, len);

	no_os_free(buffer);

	return ret;
}

/**
 * Set the test mode compatible with ad9208 API
 * @param st - The device structure
 * @param channel - Channel number
 * @param mode - Test mode. Accepted values:
 * 		 AD9208_TESTMODE_OFF
 * 		 AD9208_TESTMODE_MIDSCALE_SHORT
 * 		 AD9208_TESTMODE_POS_FULLSCALE
 * 		 AD9208_TESTMODE_NEG_FULLSCALE
 * 		 AD9208_TESTMODE_ALT_CHECKERBOARD
 * 		 AD9208_TESTMODE_PN23_SEQ
 * 		 AD9208_TESTMODE_PN9_SEQ
 * 		 AD9208_TESTMODE_ONE_ZERO_TOGGLE
 * 		 AD9208_TESTMODE_USER
 * 		 AD9208_TESTMODE_RAMP
 * @return 0 for success, any non-zero value indicates an error
 */
static int ad9208_testmode_set(struct ad9208_state *st,
			       unsigned int chan,
			       unsigned int mode)
{
	ad9208_handle_t *ad9208_h = st->adc_h;
	int ret;

	ret = ad9208_adc_set_channel_select(ad9208_h, NO_OS_BIT(chan & 1));
	if (ret < 0)
		return ret;

	ret = ad9208_register_write(ad9208_h, AD9208_REG_TEST_MODE, mode);
	if (ret < 0)
		return ret;

	return ad9208_adc_set_channel_select(ad9208_h, AD9208_ADC_CH_ALL);
}

/**
 * Setup the device.
 * @param st - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t ad9208_setup(struct ad9208_state *st)
{
	ad9208_handle_t *ad9208_h = st->adc_h;
	ad9208_adc_data_frmt_t input_fmt, output_fmt;
	adi_chip_id_t chip_id;
	uint64_t sample_rate, lane_rate_kbps;
	uint8_t dcm, pll_stat;
	int32_t timeout, i, ret;

	ret = ad9208_init(ad9208_h);
	if (ret < 0) {
		printf("ad9208 init failed (%d)\n", ret);
		return -ENODEV;
	}

	ret = ad9208_reset(ad9208_h, 0);
	if (ret < 0) {
		printf("ad9298 reset failed (%d)\n", ret);
		ret = -ENODEV;
		goto error;
	}

	ret = ad9208_get_chip_id(ad9208_h, &chip_id);
	if (ret < 0) {
		printf("ad9208_get_chip_id failed (%d)\n", ret);
		ret = -ENODEV;
		goto error;
	}

	if (chip_id.chip_type != AD9208_CHIP_TYPE) {
		printf("Wrong chip type (%X)\n", chip_id.chip_type);
		ret = -EINVAL;
		goto error;
	}

	if (chip_id.prod_id != AD9208_CHIP_ID) {
		printf("Unrecognized CHIP_ID (%X)\n", chip_id.prod_id);
		ret = -ENODEV;
		goto error;
	}

	ret = ad9208_adc_set_channel_select(ad9208_h, AD9208_ADC_CH_ALL);
	if (ret < 0) {
		printf( "Failed to select channels (%d)\n", ret);
		goto error;
	}

	ret = ad9208_set_pdn_pin_mode(ad9208_h, st->powerdown_pin_en,
				      st->powerdown_mode);
	if (ret < 0) {
		printf("Failed to set PWDN pin mode (%d)\n", ret);
		goto error;
	}

	ret = ad9208_set_input_clk_duty_cycle_stabilizer(ad9208_h,
			st->duty_cycle_stabilizer_en);
	if (ret < 0) {
		printf("Failed to set clk duty cycle stabilizer (%d)\n", ret);
		goto error;
	}

	sample_rate = st->sampling_frequency_hz * st->input_div;

	ret = ad9208_set_input_clk_cfg(ad9208_h, sample_rate,
				       st->input_div);
	if (ret < 0) {
		printf("Failed to set input clk config (%d)\n", ret);
		goto error;
	}

	ret = ad9208_adc_set_input_cfg(ad9208_h,
				       st->analog_input_mode ? COUPLING_DC : COUPLING_AC,
				       st->ext_vref_en, st->current_scale);
	if (ret < 0) {
		printf("Failed to set adc input config: %d\n", ret);
		goto error;
	}

	ret = ad9208_adc_set_input_buffer_cfg(ad9208_h, st->buff_curr_n,
					      st->buff_curr_p, AD9208_BUFF_CURR_600_UA);
	if (ret < 0) {
		printf("Failed to set input buffer config: %d\n", ret);
		goto error;
	}

	ret = ad9208_adc_set_fc_ch_mode(ad9208_h, st->fc_ch);
	if (ret < 0) {
		printf("Failed to set channel mode: %d\n", ret);
		goto error;
	}

	if (st->fc_ch == AD9208_FULL_BANDWIDTH_MODE) {
		dcm = 1; /* Full bandwidth */
	} else {
		dcm = st->ddc[0].decimation;
		for (i = 1; i < st->ddc_cnt; i++)
			dcm = no_os_min_t(uint8_t, dcm, st->ddc[i].decimation);
	}

	ret = ad9208_adc_set_dcm_mode(ad9208_h, dcm);
	if (ret < 0) {
		printf("Failed to set decimation mode: %d\n", ret);
		goto error;
	}

	/* DDC Setup */
	if (st->ddc_input_format_real_en)
		input_fmt = AD9208_DATA_FRMT_REAL;
	else
		input_fmt = AD9208_DATA_FRMT_COMPLEX;

	if (st->ddc_output_format_real_en)
		output_fmt = AD9208_DATA_FRMT_REAL;
	else
		output_fmt = AD9208_DATA_FRMT_COMPLEX;

	ret = ad9208_adc_set_data_format(ad9208_h, input_fmt, output_fmt);
	if (ret < 0) {
		printf("Failed to set data format: %d\n", ret);
		goto error;
	}

	for (i = 0; i < st->ddc_cnt; i++) {
		ret = ad9208_adc_set_ddc_gain(ad9208_h, i,
					      st->ddc[i].gain_db ? 6 : 0);
		if (ret < 0) {
			printf("Failed to set ddc gain: %d\n", ret);
			goto error;
		}

		ret = ad9208_adc_set_ddc_dcm(ad9208_h, i,
					     st->ddc[i].decimation);
		if (ret < 0) {
			printf("Failed to set ddc decimation mode: %d\n", ret);
			goto error;
		}

		ret = ad9208_adc_set_ddc_nco_mode(ad9208_h, i,
						  st->ddc[i].nco_mode);
		if (ret < 0) {
			printf("Failed to set ddc nco mode: %d\n", ret);
			goto error;
		}

		ret = ad9208_adc_set_ddc_nco(ad9208_h, i,
					     st->ddc[i].carrier_freq_hz);
		if (ret < 0) {
			printf("Failed to set ddc nco frequency: %d\n", ret);
			goto error;
		}

		ret = ad9208_adc_set_ddc_nco_phase(ad9208_h, i, st->ddc[i].po);
		if (ret < 0) {
			printf("Failed to set ddc nco phase: %d\n", ret);
			goto error;
		}
	}

	ret = ad9208_testmode_set(st, 0, st->test_mode_ch0);
	if (ret < 0) {
		printf("Failed to set test mode for ch 0: %d\n", ret);
		goto error;
	}

	ret = ad9208_testmode_set(st, 1, st->test_mode_ch1);
	if (ret < 0) {
		printf("Failed to set test mode for ch 1: %d\n", ret);
		goto error;
	}

	ret = ad9208_jesd_syref_lmfc_offset_set(ad9208_h,
						st->sysref_lmfc_offset);
	if (ret < 0) {
		printf("Failed to set SYSREF lmfc offset: %d\n", ret);
		goto error;
	}

	ret = ad9208_jesd_syref_config_set(ad9208_h, st->sysref_edge_sel,
					   st->sysref_clk_edge_sel,
					   st->sysref_neg_window_skew,
					   st->sysref_pos_window_skew);
	if (ret < 0) {
		printf("Failed to set SYSREF sig capture settings: %d\n", ret);
		goto error;
	}

	ret = ad9208_jesd_syref_mode_set(ad9208_h, st->sysref_mode,
					 st->sysref_count);
	if (ret < 0) {
		printf("Failed to Set JESD SYNCHRONIZATION Mode: %d\n", ret);
		goto error;
	}

	ret = ad9208_jesd_set_if_config(ad9208_h, (jesd_param_t )*st->jesd_param,
					&lane_rate_kbps);
	if (ret < 0) {
		printf("Failed to set JESD204 interface config (%d)\n", ret);
		goto error;
	}

	ret = ad9208_jesd_subclass_set(ad9208_h, st->jesd_subclass);
	if (ret < 0) {
		printf("Failed to set subclass (%d)\n", ret);
		goto error;
	}

	ret = ad9208_jesd_enable_scrambler(ad9208_h, 1);
	if (ret < 0) {
		printf("Failed to enable scrambler (%d)\n", ret);
		goto error;
	}

	ret = ad9208_jesd_enable_link(ad9208_h, 1);
	if (ret < 0) {
		printf("Failed to enabled JESD204 link (%d)\n", ret);
		goto error;
	}

	timeout = 10;

	do {
		no_os_mdelay(10);
		ret = ad9208_jesd_get_pll_status(ad9208_h, &pll_stat);
		if (ret < 0) {
			printf("Failed to get pll status (%d)\n", ret);
			goto error;
		}
	} while (!(pll_stat & AD9208_JESD_PLL_LOCK_STAT) && timeout--);

	printf("AD9208 PLL %s\n", (pll_stat & AD9208_JESD_PLL_LOCK_STAT) ?
	       "LOCKED" : "UNLOCKED");

	return 0;

error:
	ad9208_deinit(ad9208_h);
	return ret;
}


/**
 * Delay microseconds, compatible with AD9208 API
 * Performs a blocking or sleep delay for the specified time in microseconds
 * @param user_data
 * @param us - time to delay/sleep in microseconds.
 */
static int ad9208_udelay(void *user_data, unsigned int us)
{
	no_os_udelay(us);

	return 0;
}

/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad9208_initialize(ad9208_dev **device, ad9208_init_param *init_param)
{
	ad9208_dev *dev;
	int32_t i, ret;

	dev = (ad9208_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->st = (struct ad9208_state *)no_os_calloc(1, sizeof(*dev->st));
	if (!dev->st) {
		ret = -ENOMEM;
		goto error;
	}
	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret < 0)
		goto error;

	/* GPIO */
	ret = no_os_gpio_get(&dev->gpio_powerdown, &init_param->gpio_powerdown);
	if (ret < 0)
		goto error;

	ret = no_os_gpio_direction_output(dev->gpio_powerdown, NO_OS_GPIO_LOW);
	if (ret < 0)
		goto error;

	ret = no_os_gpio_set_value(dev->gpio_powerdown, NO_OS_GPIO_LOW);
	if (ret < 0)
		goto error;

	struct ad9208_state *st = (struct ad9208_state *)no_os_calloc(1, sizeof(*st));
	if (!st) {
		ret = -ENOMEM;
		goto error;
	}

	st->jesd_param = (jesd_param_t *)no_os_calloc(1, sizeof(*st->jesd_param));
	if (!st->jesd_param) {
		ret = -ENOMEM;
		goto error;
	}

	st->adc_h = (ad9208_handle_t *)no_os_calloc(1, sizeof(*st->adc_h));
	if (!st->adc_h) {
		ret = -ENOMEM;
		goto error;
	}

	st->adc_h->user_data = dev->spi_desc;
	st->adc_h->dev_xfer = ad9208_spi_xfer;
	st->adc_h->delay_us = ad9208_udelay;

	st->sampling_frequency_hz = init_param->sampling_frequency_hz;
	st->input_div = init_param->input_div;
	st->duty_cycle_stabilizer_en = init_param->duty_cycle_stabilizer_en;
	st->powerdown_mode = init_param->powerdown_mode;
	st->powerdown_pin_en = init_param->powerdown_pin_en;
	st->current_scale = init_param->current_scale;
	st->analog_input_mode = init_param->analog_input_mode;
	st->ext_vref_en = init_param->ext_vref_en;
	st->buff_curr_n = init_param->buff_curr_n;
	st->buff_curr_p = init_param->buff_curr_p;
	st->fc_ch = init_param->fc_ch;
	st->ddc_cnt = init_param->ddc_cnt;
	for (i = 0; i < st->ddc_cnt; i++) {
		st->ddc[i].decimation = init_param->ddc[i].decimation;
		st->ddc[i].nco_mode = init_param->ddc[i].nco_mode;
		st->ddc[i].carrier_freq_hz = init_param->ddc[i].carrier_freq_hz;
		st->ddc[i].gain_db = init_param->ddc[i].gain_db;
		st->ddc[i].po = init_param->ddc[i].po;
	}
	st->ddc_output_format_real_en = init_param->ddc_output_format_real_en;
	st->ddc_input_format_real_en = init_param->ddc_input_format_real_en;
	st->test_mode_ch0 = init_param->test_mode_ch0;
	st->test_mode_ch1 = init_param->test_mode_ch1;

	/* SYSREF Config */
	st->sysref_lmfc_offset = init_param->sysref_lmfc_offset;
	st->sysref_clk_edge_sel = init_param->sysref_clk_edge_sel;
	st->sysref_edge_sel = init_param->sysref_edge_sel;
	st->sysref_neg_window_skew = init_param->sysref_neg_window_skew;
	st->sysref_pos_window_skew = init_param->sysref_pos_window_skew;
	st->sysref_mode = init_param->sysref_mode;
	st->sysref_count = init_param->sysref_count;

	st->jesd_param = init_param->jesd_param;
	st->jesd_subclass = init_param->jesd_subclass;

	ret = ad9208_setup(st);
	if (ret < 0) {
		printf("Failed to setup device\n");
		goto error;
	}

	dev->st = st;

	printf("AD9208 successfully initialized\n");

	*device = dev;

	return 0;

error:
	if (st->adc_h)
		no_os_free(st->adc_h);
	if (st->jesd_param)
		no_os_free(st->jesd_param);
	if (st)
		no_os_free(st);
	if (dev->gpio_powerdown)
		no_os_gpio_remove(dev->gpio_powerdown);
	if (dev->spi_desc)
		no_os_spi_remove(dev->spi_desc);
	if (dev->st)
		no_os_free(dev->st);
	if (dev)
		no_os_free(dev);

	return ret;
}

int32_t ad9208_remove(ad9208_dev *device)
{
	int32_t ret;

	ret = no_os_gpio_remove(device->gpio_powerdown);
	ret |= no_os_spi_remove(device->spi_desc);

	if (device->st->adc_h)
		no_os_free(device->st->adc_h);
	if (device->st)
		no_os_free(device->st);
	if (device)
		no_os_free(device);

	return ret;
}
