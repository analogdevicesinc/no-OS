/***************************************************************************//**
 *   @file   ad9172.c
 *   @brief  Implementation of AD9172 Driver.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
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
#include <string.h>
#include "ad9172.h"
#include <inttypes.h>
#include "no_os_alloc.h"

/**
 * Setup the device.
 * @param st - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t ad9172_setup(struct ad9172_state *st)
{
	uint8_t revision[3] = {0, 0, 0};
	uint8_t pll_lock_status = 0, dll_lock_stat = 0;
	adi_chip_id_t dac_chip_id;
	int32_t ret;
	uint64_t dac_rate_Hz;
	uint64_t dac_clkin_Hz, lane_rate_kHz;
	ad917x_jesd_link_stat_t link_status;
	ad917x_handle_t *ad917x_h = &st->dac_h;
	uint64_t pll_mult;
	uint8_t dac_mask, chan_mask;

	st->interpolation = st->dac_interpolation *
			    st->channel_interpolation;

	/*Initialise DAC Module*/
	ret = ad917x_init(ad917x_h);
	if (ret != 0) {
		printf("ad917x_init failed (%"PRIi32")\n", ret);
		return ret;
	}

	ret = ad917x_reset(ad917x_h, 0);
	if (ret != 0) {
		printf("ad917x_reset failed (%"PRIi32")\n", ret);
		return ret;
	}

	ret = ad917x_get_chip_id(ad917x_h, &dac_chip_id);
	if (ret != 0) {
		printf("ad917x_get_chip_id failed (%"PRIi32")\n", ret);
		return ret;
	}

	ret = ad917x_get_revision(ad917x_h, &revision[0], &revision[1],
				  &revision[2]);
	if (ret != 0)
		return ret;

	printf("AD917x DAC Chip ID: %d\n", dac_chip_id.chip_type);
	printf("AD917x DAC Product ID: %x\n", dac_chip_id.prod_id);
	printf("AD917x DAC Product Grade: %d\n", dac_chip_id.prod_grade);
	printf("AD917x DAC Product Revision: %d\n",
	       dac_chip_id.dev_revision);
	printf("AD917x Revision: %d.%d.%d\n",
	       revision[0], revision[1], revision[2]);

	dac_clkin_Hz = st->dac_clkin_Hz;

	printf("PLL Input rate %"PRIu64"\n", dac_clkin_Hz);

	pll_mult = NO_OS_DIV_ROUND_CLOSEST(st->dac_rate_khz, dac_clkin_Hz / 1000);

	ret = ad917x_set_dac_clk(ad917x_h, (uint64_t)dac_clkin_Hz * pll_mult,
				 1, dac_clkin_Hz);
	if (ret != 0) {
		printf("ad917x_set_dac_clk failed (%"PRIi32")\n", ret);
		return ret;
	}

	no_os_mdelay(100); /* Wait 100 ms for PLL to lock */

	ret = ad917x_get_dac_clk_status(ad917x_h,
					&pll_lock_status, &dll_lock_stat);
	if (ret != 0) {
		printf("ad917x_get_dac_clk_status failed (%"PRIi32")\n", ret);
		return ret;
	}

	printf("PLL lock status %x,  DLL lock status: %x\n",
	       pll_lock_status, dll_lock_stat);

	if (st->clock_output_config) {
		/* DEBUG: route DAC clock to output, so we can meassure it */
		ret = ad917x_set_clkout_config(ad917x_h,
					       st->clock_output_config);
		if (ret != 0) {
			printf("ad917x_set_clkout_config failed (%"PRIi32")\n",
			       ret);
			return ret;
		}
	}

	ret = ad917x_jesd_config_datapath(ad917x_h, st->jesd_dual_link_mode,
					  st->jesd_link_mode,
					  st->channel_interpolation,
					  st->dac_interpolation);
	if (ret != 0) {
		printf("ad917x_jesd_config_datapath failed (%"PRIi32")\n", ret);
		return ret;
	}
	ret = ad917x_jesd_get_cfg_param(ad917x_h, &st->appJesdConfig);
	if (ret != 0) {
		printf("ad917x_jesd_get_cfg_param failed (%"PRIi32")\n", ret);
		return ret;
	}

	ret = ad917x_jesd_set_scrambler_enable(ad917x_h, 1);
	if (ret != 0) {
		printf("ad917x_jesd_set_scrambler_enable failed (%"PRIi32")\n",
		       ret);
		return ret;
	}

	ret = ad917x_jesd_enable_datapath(ad917x_h, 0xFF, 0x1, 0x1);
	if (ret != 0) {
		printf("ad917x_jesd_enable_datapath failed (%"PRIi32")\n", ret);
		return ret;
	}

	ret = ad917x_jesd_set_syncoutb_enable(ad917x_h, SYNCOUTB_0, 1);
	if (ret != 0) {
		printf("ad917x_jesd_set_syncoutb_enable failed (%"PRIi32")\n",
		       ret);
		return ret;
	}

	no_os_mdelay(100);

	ret = ad917x_jesd_get_pll_status(ad917x_h, &pll_lock_status);
	if (ret != 0) {
		printf("ad917x_jesd_get_pll_status failed (%"PRIi32")\n", ret);
		return ret;
	}

	printf("Serdes PLL %s (stat: %x)\n",
	       ((pll_lock_status & 0x1) == 0x1) ?
	       "Locked" : "Unlocked",  pll_lock_status);

	ad917x_get_dac_clk_freq(ad917x_h, &dac_rate_Hz);

	lane_rate_kHz = dac_rate_Hz * 20 * st->appJesdConfig.jesd_M;
	no_os_do_div(&lane_rate_kHz, st->appJesdConfig.jesd_L *
		     st->interpolation * 1000);

	ret = ad917x_jesd_set_sysref_enable(ad917x_h, 0); /* subclass 0 */
	if (ret != 0) {
		printf("DAC:MODE:JESD: ERROR : Sysref enable failed\n");
		return -EIO;
	}

	/*Enable Link*/
	ret = ad917x_jesd_enable_link(ad917x_h, JESD_LINK_ALL, 0x1);
	if (ret != 0) {
		printf("DAC:MODE:JESD: ERROR : Enable Link failed\n");
		return -EIO;
	}

	no_os_mdelay(100);

	ret = ad917x_jesd_get_link_status(ad917x_h, JESD_LINK_0, &link_status);
	if (ret != 0) {
		printf("DAC:MODE:JESD: ERROR : Get Link status failed \r\n");
		return -EIO;
	}

	printf("code_grp_sync: %x\n", link_status.code_grp_sync_stat);
	printf("frame_sync_stat: %x\n", link_status.frame_sync_stat);
	printf("good_checksum_stat: %x\n",
	       link_status.good_checksum_stat);
	printf("init_lane_sync_stat: %x\n",
	       link_status.init_lane_sync_stat);
	printf("%d lanes @ %"PRIu64" kBps\n",
	       st->appJesdConfig.jesd_L, lane_rate_kHz);

	if (st->jesd_dual_link_mode || st->interpolation == 1)
		dac_mask = AD917X_DAC0 | AD917X_DAC1;
	else
		dac_mask = AD917X_DAC0;

	if (st->interpolation > 1) {
		chan_mask = NO_OS_GENMASK(st->appJesdConfig.jesd_M / 2, 0);
		ret = ad917x_set_page_idx(ad917x_h, AD917X_DAC_NONE, chan_mask);
		if (ret != 0)
			return -EIO;
		ret = ad917x_set_channel_gain(ad917x_h, 2048); /* GAIN = 1 */
		if (ret != 0)
			return -EIO;

		st->nco_main_enable = dac_mask;

		ad917x_nco_enable(ad917x_h, st->nco_main_enable, 0);
	}

	ret = ad917x_set_page_idx(ad917x_h, dac_mask, AD917X_CH_NONE);
	if (ret != 0)
		return -EIO;

	ad917x_register_write(ad917x_h, 0x596, 0x1c); /* SPI turn on TXENx feature */

	return 0;
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

/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad9172_init(ad9172_dev **device,
		    ad9172_init_param * init_param)
{
	int32_t ret;
	ad9172_dev *dev;

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
	st->syncoutb_type = init_param->syncoutb_type;
	st->sysref_coupling = init_param->sysref_coupling;

	st->dac_h.user_data = dev->spi_desc;
	st->dac_h.sdo = SPI_SDO;
	st->dac_h.dev_xfer = ad9172_spi_xfer;
	st->dac_h.delay_us = ad9172_delay_us;
	st->dac_h.tx_en_pin_ctrl = NULL;
	st->dac_h.reset_pin_ctrl = NULL;
	st->dac_h.syncoutb = st->syncoutb_type;
	st->dac_h.sysref = st->sysref_coupling;

	ret = ad9172_setup(st);
	if (ret < 0) {
		printf("Failed to setup device\n");
		goto error_3;
	}

	printf("%s : AD936x Rev %d successfully initialized\n", __func__, 1);
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
