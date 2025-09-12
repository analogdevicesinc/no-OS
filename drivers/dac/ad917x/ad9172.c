/***************************************************************************//**
 *   @file   ad9172.c
 *   @brief  Implementation of AD9172 Driver.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
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

#include "no_os_print_log.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_clk.h"
#include <inttypes.h>
#include "jesd204.h"
#include <stdlib.h>
#include <string.h>
#include "ad9172.h"
#include <errno.h>
#include <stdio.h>

static int ad9172_link_status_get(struct ad9172_state *st,
				  unsigned long lane_rate_kHz)
{
	ad917x_jesd_link_stat_t link_status;
	int ret, i;

	for (i = JESD_LINK_0; i <= JESD_LINK_1; i++) {
		ret = ad917x_jesd_get_link_status(&st->dac_h, i, &link_status);
		if (ret != 0) {
			pr_err("DAC:MODE:JESD: ERROR : Get Link%d status failed \r\n", i);
			return -EIO;
		}

		pr_info("Link%d code_grp_sync: %x\n", i, link_status.code_grp_sync_stat);
		pr_info("Link%d frame_sync_stat: %x\n", i, link_status.frame_sync_stat);
		pr_info("Link%d good_checksum_stat: %x\n",
			i, link_status.good_checksum_stat);
		pr_info("Link%d init_lane_sync_stat: %x\n",
			i, link_status.init_lane_sync_stat);
		pr_info("Link%d %d lanes @ %lu kBps\n",
			i, st->appJesdConfig.jesd_L, lane_rate_kHz);

		if (no_os_hweight8(link_status.code_grp_sync_stat) != st->appJesdConfig.jesd_L
		    ||
		    link_status.code_grp_sync_stat != link_status.frame_sync_stat ||
		    link_status.code_grp_sync_stat != link_status.init_lane_sync_stat)
			return -EFAULT;

		if (!st->jesd_dual_link_mode)
			break;
	}

	return 0;
}

static int ad9172_finalize_setup(struct ad9172_state *st)
{
	ad917x_handle_t *ad917x_h = &st->dac_h;
	int ret;
	uint8_t dac_mask, chan_mask;

	if (st->jesd_dual_link_mode || st->interpolation == 1)
		dac_mask = AD917X_DAC0 | AD917X_DAC1;
	else
		dac_mask = AD917X_DAC0;

	if (st->interpolation > 1) {
		chan_mask = NO_OS_GENMASK(st->appJesdConfig.jesd_M / 2, 0);
		ret = ad917x_set_page_idx(ad917x_h, AD917X_DAC_NONE, chan_mask);
		if (ret)
			return ret;

		ret = ad917x_set_channel_gain(ad917x_h, 2048); /* GAIN = 1 */
		if (ret)
			return ret;

		st->nco_main_enable = dac_mask;

		ad917x_nco_enable(ad917x_h, st->nco_main_enable, 0);
	}

	ret = ad917x_set_page_idx(ad917x_h, dac_mask, AD917X_CH_NONE);
	if (ret != 0)
		return -EIO;

	return ad917x_register_write(ad917x_h, 0x596, 0x1c);
}

/**
 * Setup the device.
 * @param st - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t ad9172_setup(struct ad9172_state *st)
{
	uint8_t pll_lock_status = 0, dll_lock_stat = 0;
	ad917x_handle_t *ad917x_h = &st->dac_h;
	uint64_t dac_rate_Hz, dac_clkin_Hz;
	uint8_t revision[3] = {0, 0, 0};
	unsigned long lane_rate_kHz;
	adi_chip_id_t dac_chip_id;
	unsigned long pll_mult;
	int ret, i;

	st->interpolation = st->dac_interpolation *
			    st->channel_interpolation;

	/*Initialise DAC Module*/
	ret = ad917x_init(ad917x_h);
	if (ret != 0) {
		pr_err("ad917x_init failed (%d)\n", ret);
		return ret;
	}

	ret = ad917x_reset(ad917x_h, 0);
	if (ret != 0) {
		pr_err("ad917x_reset failed (%d)\n", ret);
		return ret;
	}

	ret = ad917x_get_chip_id(ad917x_h, &dac_chip_id);
	if (ret != 0) {
		pr_err("ad917x_get_chip_id failed (%d)\n", ret);
		return ret;
	}

	ret = ad917x_get_revision(ad917x_h, &revision[0], &revision[1],
				  &revision[2]);
	if (ret != 0)
		return ret;

	pr_info("ad917x DAC Chip ID: %d\n", dac_chip_id.chip_type);
	pr_info("ad917x DAC Product ID: %x\n", dac_chip_id.prod_id);
	pr_info("ad917x DAC Product Grade: %d\n", dac_chip_id.prod_grade);
	pr_info("ad917x DAC Product Revision: %d\n",
		dac_chip_id.dev_revision);
	pr_info("ad917x Revision: %d.%d.%d\n",
		revision[0], revision[1], revision[2]);

	ret = no_os_clk_recalc_rate(st->dac_clk, &dac_clkin_Hz);
	if (ret) {
		pr_err("no_os_clk_recalc_rate failed (%d)\n", ret);
		return ret;
	}

	pr_info("CLK Input rate %lu\n", dac_clkin_Hz);

	if (!st->pll_bypass) {
		uint64_t tmp = dac_clkin_Hz / 1000;

		pll_mult = NO_OS_DIV_ROUND_CLOSEST_ULL(st->dac_rate_khz, tmp);

		ret = ad917x_set_dac_clk(ad917x_h, dac_clkin_Hz * pll_mult,
					 1, dac_clkin_Hz);
	} else {
		ret = ad917x_set_dac_clk(ad917x_h, dac_clkin_Hz, 0,
					 dac_clkin_Hz);
	}

	if (ret != 0) {
		pr_err("ad917x_set_dac_clk failed (%d)\n", ret);
		return ret;
	}

	no_os_mdelay(100); /* Wait 100 ms for PLL to lock */

	ret = ad917x_get_dac_clk_status(ad917x_h,
					&pll_lock_status, &dll_lock_stat);
	if (ret != 0) {
		pr_err("ad917x_get_dac_clk_status failed (%d)\n", ret);
		return ret;
	}

	pr_info("PLL lock status %x,  DLL lock status: %x\n",
		pll_lock_status, dll_lock_stat);

	if (st->clock_output_config) {
		/* DEBUG: route DAC clock to output, so we can meassure it */
		ret = ad917x_set_clkout_config(ad917x_h,
					       st->clock_output_config);
		if (ret != 0) {
			pr_err("ad917x_set_clkout_config failed (%d)\n",
			       ret);
			return ret;
		}
	}

	ret = ad917x_jesd_config_datapath(ad917x_h, st->jesd_dual_link_mode,
					  st->jesd_link_mode,
					  st->channel_interpolation,
					  st->dac_interpolation);
	if (ret != 0) {
		pr_err("ad917x_jesd_config_datapath failed (%d)\n", ret);
		return ret;
	}

	ret = ad917x_jesd_get_cfg_param(ad917x_h, &st->appJesdConfig);
	if (ret != 0) {
		pr_err("ad917x_jesd_get_cfg_param failed (%d)\n", ret);
		return ret;
	}

	ret = ad917x_jesd_set_scrambler_enable(ad917x_h, st->scrambling);
	if (ret != 0) {
		pr_err("ad917x_jesd_set_scrambler_enable failed (%d)\n",
		       ret);
		return ret;
	}

	/*Configure xbar*/
	for (i = 0; i < 8; i++) {
		ret = ad917x_jesd_set_lane_xbar(ad917x_h, i, st->logic_lanes[i]);
		if (ret != 0)
			return ret;
	}

	ret = ad917x_jesd_enable_datapath(ad917x_h, 0xFF, 0x1, 0x1);
	if (ret != 0) {
		pr_err("ad917x_jesd_enable_datapath failed (%d)\n", ret);
		return ret;
	}

	ret = ad917x_jesd_set_syncoutb_enable(ad917x_h, SYNCOUTB_0, 1);
	if (ret != 0) {
		pr_err("ad917x_jesd_set_syncoutb_enable failed (%d)\n",
		       ret);
		return ret;
	}

	no_os_mdelay(100);

	ret = ad917x_jesd_get_pll_status(ad917x_h, &pll_lock_status);
	if (ret != 0) {
		pr_err("ad917x_jesd_get_pll_status failed (%d)\n", ret);
		return ret;
	}

	pr_info("Serdes PLL %s (stat: %x)\n",
		((pll_lock_status & 0x1) == 0x1) ?
		"Locked" : "Unlocked",  pll_lock_status);

	/* No need to continue here when jesd204-fsm enabled */
	if (st->parent->jdev)
		return 0;

	ad917x_get_dac_clk_freq(ad917x_h, &dac_rate_Hz);

	lane_rate_kHz = no_os_div_u64(dac_rate_Hz * 20 * st->appJesdConfig.jesd_M,
				      st->appJesdConfig.jesd_L *
				      st->interpolation * 1000);

	ret = no_os_clk_set_rate(st->clk_data, lane_rate_kHz);
	if (ret < 0) {
		pr_err("Failed to set lane rate to %lu kHz: %d\n",
		       lane_rate_kHz, ret);
		return ret;
	}

	ret = no_os_clk_enable(st->clk_data);
	if (ret) {
		pr_err("Failed to enable JESD204 link: %d\n", ret);
		return ret;
	}

	ad917x_jesd_set_sysref_enable(ad917x_h, !!st->jesd_subclass);

	/*Enable Link*/
	ret = ad917x_jesd_enable_link(ad917x_h, JESD_LINK_ALL, 0x1);
	if (ret != 0) {
		pr_err("DAC:MODE:JESD: ERROR : Enable Link failed\n");
		return -EIO;
	}

	no_os_mdelay(100);

	ret = ad9172_link_status_get(st, lane_rate_kHz);
	if (ret != 0) {
		pr_err("DAC:MODE:JESD: ERROR : Link status failed\n");
		return ret;
	}

	return ad9172_finalize_setup(st);
}

/**
 * Delay microseconds, compatible with AD917x API
 * Performs a blocking or sleep delay for the specified time in microseconds
 * @param user_data
 * @param us - time to delay/sleep in microseconds.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t ad9172_delay_us(void *user_data, uint32_t us)
{
	no_os_udelay(us);

	return 0;
}

/**
 * Spi write and read compatible with
 * @param user_data
 * @param wbuf Pointer to array with the data to be sent on the SPI
 * @param rbuf Pointer to array where the data to which the SPI will be written
 * @param len The size in bytes allocated for each of the indata and outdata arrays.
 * @return 0 for success, any non-zero value indicates an error
 */
static int32_t ad9172_spi_xfer(void *user_data, uint8_t *wbuf,
			       uint8_t *rbuf, int32_t len)
{
	int32_t ret;
	struct no_os_spi_desc *spi = user_data;
	uint8_t * buffer = (uint8_t *) no_os_malloc(len);

	memcpy(buffer, wbuf, 3);
	ret = no_os_spi_write_and_read(spi, buffer, len);
	if (ret < 0) {
		printf("Read Error %"PRId32, ret);
	} else {
		memcpy(rbuf, buffer, len);
	}
	no_os_free(buffer);

	return ret;
}

struct ad9172_jesd204_link {
	unsigned int source_id;
	bool is_framer;
};

struct ad9172_jesd204_priv {
	struct ad9172_state *st;
};

static int ad9172_jesd204_link_init(struct jesd204_dev *jdev,
				    enum jesd204_state_op_reason reason,
				    struct jesd204_link *lnk)
{
	struct ad9172_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9172_state *st = priv->st;
	unsigned int id;
	uint64_t rate;
	int ret;

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	pr_debug("%s:%d link_num %u reason %s\n", __func__,
		 __LINE__, lnk->link_id, jesd204_state_op_reason_str(reason));

	lnk->num_converters = st->appJesdConfig.jesd_M;
	lnk->num_lanes = st->appJesdConfig.jesd_L;
	lnk->octets_per_frame = st->appJesdConfig.jesd_F;
	lnk->frames_per_multiframe = st->appJesdConfig.jesd_K;
	lnk->device_id = st->appJesdConfig.jesd_DID;
	lnk->bank_id = st->appJesdConfig.jesd_BID;
	lnk->scrambling = st->scrambling;
	lnk->bits_per_sample = st->appJesdConfig.jesd_NP;
	lnk->converter_resolution = st->appJesdConfig.jesd_N;
	lnk->ctrl_bits_per_sample = st->appJesdConfig.jesd_CS;
	lnk->jesd_version = JESD204_VERSION_B;
	lnk->subclass = st->jesd_subclass ?
			JESD204_SUBCLASS_1 : JESD204_SUBCLASS_0;
	lnk->high_density = st->appJesdConfig.jesd_HD;
	lnk->samples_per_conv_frame = st->appJesdConfig.jesd_S;
	for (id = 0; id < lnk->num_lanes; id++)
		lnk->lane_ids[id] = id;

	ret = ad917x_get_dac_clk_freq(&st->dac_h, &rate);
	if (ret)
		return ret;

	lnk->sample_rate = rate;
	lnk->sample_rate_div = st->interpolation;
	lnk->jesd_encoder = JESD204_ENCODER_8B10B;
	lnk->is_transmit = true;

	if (st->sysref_mode == SYSREF_CONT)
		lnk->sysref.mode = JESD204_SYSREF_CONTINUOUS;
	else if (st->sysref_mode == SYSREF_ONESHOT)
		lnk->sysref.mode = JESD204_SYSREF_ONESHOT;

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9172_jesd204_link_enable(struct jesd204_dev *jdev,
				      enum jesd204_state_op_reason reason,
				      struct jesd204_link *lnk)
{
	struct ad9172_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9172_state *st = priv->st;
	int ret;

	pr_debug("%s:%d link_num %u reason %s\n", __func__,
		 __LINE__, lnk->link_id, jesd204_state_op_reason_str(reason));

	ad917x_jesd_set_sysref_enable(&st->dac_h, !!st->jesd_subclass);

	/*Enable Link*/
	ret = ad917x_jesd_enable_link(&st->dac_h, JESD_LINK_ALL,
				      reason == JESD204_STATE_OP_REASON_INIT);
	if (ret != 0) {
		pr_err("Failed to enable JESD204 link (%d)\n", ret);
		return -EFAULT;
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9172_jesd204_link_running(struct jesd204_dev *jdev,
				       enum jesd204_state_op_reason reason,
				       struct jesd204_link *lnk)
{
	struct ad9172_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9172_state *st = priv->st;
	unsigned long lane_rate_khz;
	int ret;

	if (reason != JESD204_STATE_OP_REASON_INIT)
		return JESD204_STATE_CHANGE_DONE;

	pr_debug("%s:%d link_num %u reason %s\n", __func__,
		 __LINE__, lnk->link_id, jesd204_state_op_reason_str(reason));

	ret = jesd204_link_get_rate_khz(lnk, &lane_rate_khz);
	if (ret) {
		pr_err("Failed JESD204 link get rate (%d)\n", ret);
		return ret;
	}

	ret = ad9172_link_status_get(st, lane_rate_khz);
	if (ret) {
		pr_err("Failed JESD204 link status (%d)\n", ret);
		return ret;
	}

	return JESD204_STATE_CHANGE_DONE;
}

static int ad9172_jesd204_post_running_stage(struct jesd204_dev *jdev,
		enum jesd204_state_op_reason reason)
{
	struct ad9172_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9172_state *st = priv->st;
	int ret;

	ret = ad9172_finalize_setup(st);

	if (ret < 0)
		return ret;

	return JESD204_STATE_CHANGE_DONE;
}

static const struct jesd204_dev_data jesd204_ad9172_init = {
	.state_ops = {
		[JESD204_OP_LINK_INIT] = {
			.per_link = ad9172_jesd204_link_init,
		},
		[JESD204_OP_LINK_ENABLE] = {
			.per_link = ad9172_jesd204_link_enable,
			.post_state_sysref = true,
		},
		[JESD204_OP_LINK_RUNNING] = {
			.per_link = ad9172_jesd204_link_running,
		},
		[JESD204_OP_OPT_POST_RUNNING_STAGE] = {
			.per_device = ad9172_jesd204_post_running_stage,
			.mode = JESD204_STATE_OP_MODE_PER_DEVICE,
		},
	},
	.max_num_links = 1,
	.num_retries = 2,
	.sizeof_priv = sizeof(struct ad9172_jesd204_priv),
};

/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad9172_init(ad9172_dev **device, ad9172_init_param * init_param)
{
	struct ad9172_jesd204_priv *priv;
	ad9172_dev *dev;
	int32_t ret, i;

	dev = (ad9172_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	struct ad9172_state *st = (struct ad9172_state *)no_os_calloc(1, sizeof(*st));
	if (!st) {
		ret = -ENOMEM;
		goto error_1;
	}

	dev->st = st;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret < 0)
		goto error_2;

	/* GPIO */
	ret |= no_os_gpio_get(&dev->gpio_reset, &init_param->gpio_reset);
	ret |= no_os_gpio_get(&dev->gpio_txen0, &init_param->gpio_txen0);
	ret |= no_os_gpio_get(&dev->gpio_txen1, &init_param->gpio_txen1);

	ret |= no_os_gpio_direction_output(dev->gpio_reset, NO_OS_GPIO_HIGH);
	ret |= no_os_gpio_direction_output(dev->gpio_txen0, NO_OS_GPIO_HIGH);
	ret |= no_os_gpio_direction_output(dev->gpio_txen1, NO_OS_GPIO_HIGH);

	ret |= no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_HIGH);
	ret |= no_os_gpio_set_value(dev->gpio_txen0, NO_OS_GPIO_HIGH);
	ret |= no_os_gpio_set_value(dev->gpio_txen1, NO_OS_GPIO_HIGH);

	if (ret < 0)
		goto error_3;

	st->dac_rate_khz = init_param->dac_rate_khz;
	st->dac_clkin_Hz = init_param->dac_clkin_Hz;
	st->jesd_link_mode = init_param->jesd_link_mode;
	st->jesd_subclass = init_param->jesd_subclass;
	st->dac_interpolation = init_param->dac_interpolation;
	st->channel_interpolation = init_param->channel_interpolation;
	st->clock_output_config = init_param->clock_output_config;
	st->scrambling = init_param->scrambling;
	st->sysref_mode = init_param->sysref_mode;
	st->pll_bypass = init_param->pll_bypass;
	st->syncoutb_type = init_param->syncoutb_type;
	st->sysref_coupling = init_param->sysref_coupling;

	if (!init_param->logic_lanes_mapping)
		for (i = 0; i < sizeof(st->logic_lanes); i++)
			st->logic_lanes[i] = i;
	else
		for (i = 0; i < sizeof(st->logic_lanes); i++)
			st->logic_lanes[i] = init_param->logic_lanes[i];

	st->dac_clk = init_param->dac_clk;
	st->clk_data = init_param->clk_data;
	st->parent = dev;

	st->dac_h.user_data = dev->spi_desc;
	st->dac_h.sdo = SPI_SDO;
	st->dac_h.dev_xfer = ad9172_spi_xfer;
	st->dac_h.delay_us = ad9172_delay_us;
	st->dac_h.tx_en_pin_ctrl = NULL;
	st->dac_h.reset_pin_ctrl = NULL;
	st->dac_h.syncoutb = st->syncoutb_type;
	st->dac_h.sysref = st->sysref_coupling;

	if (init_param->use_jesd_fsm) {
		ret = jesd204_dev_register(&dev->jdev, &jesd204_ad9172_init);
		if (ret)
			goto error_3;

		priv = jesd204_dev_priv(dev->jdev);
		priv->st = dev->st;
	}

	ret = ad9172_setup(st);
	if (ret < 0) {
		printf("Failed to setup device\n");
		goto error_3;
	}

	printf("%s : AD917x Rev %d successfully initialized\n", __func__, 1);

	*device = dev;

	return 0;

error_3:
	no_os_spi_remove(dev->spi_desc);
error_2:
	no_os_free(st);
error_1:
	no_os_free(dev);

	return ret;
}

/**
 * Remove the device - release resources.
 * @param device - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad9172_remove(ad9172_dev *device)
{
	int32_t ret;

	ret = no_os_spi_remove(device->spi_desc);
	ret += no_os_gpio_remove(device->gpio_reset);
	ret += no_os_gpio_remove(device->gpio_txen0);
	ret += no_os_gpio_remove(device->gpio_txen1);

	no_os_free(device->st);

	no_os_free(device);

	return ret;
}
