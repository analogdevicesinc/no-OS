/***************************************************************************//**
 * @file ad9680.c
 * @brief Implementation of AD9680 Driver.
 * @author DBogdan (dragos.bogdan@analog.com)
 ********************************************************************************
 * Copyright 2014-2016(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 * - Neither the name of Analog Devices, Inc. nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * - The use of this software may or may not infringe the patent rights
 * of one or more patent holders. This license does not release you
 * from the requirement that you obtain separate licenses from these
 * patent holders to use this software.
 * - Use of the software either in source or binary form, must be run
 * on or directly connected to an Analog Devices Inc. component.
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
#include <stdlib.h>
#include <stdio.h>
#include "ad9680.h"
#include "no_os_error.h"
#include "no_os_print_log.h"
#include "no_os_alloc.h"

struct ad9680_jesd204_priv {
	struct ad9680_dev *dev;
};

/***************************************************************************//**
 * @brief ad9680_spi_read
 *******************************************************************************/
int32_t ad9680_spi_read(struct ad9680_dev *dev,
			uint16_t reg_addr,
			uint8_t *reg_data)
{
	uint8_t buf[3];

	int32_t ret;

	buf[0] = 0x80 | (reg_addr >> 8);
	buf[1] = reg_addr & 0xFF;
	buf[2] = 0x00;

	ret = no_os_spi_write_and_read(dev->spi_desc,
				       buf,
				       3);
	*reg_data = buf[2];

	return ret;
}

/***************************************************************************//**
 * @brief ad9680_spi_write
 *******************************************************************************/
int32_t ad9680_spi_write(struct ad9680_dev *dev,
			 uint16_t reg_addr,
			 uint8_t reg_data)
{
	uint8_t buf[3];

	int32_t ret;

	buf[0] = reg_addr >> 8;
	buf[1] = reg_addr & 0xFF;
	buf[2] = reg_data;

	ret = no_os_spi_write_and_read(dev->spi_desc,
				       buf,
				       3);

	return ret;
}

/*******************************************************************************
 * @brief ad9680_test
 *******************************************************************************/
int32_t ad9680_test(struct ad9680_dev *dev,
		    uint32_t test_mode)
{
	ad9680_spi_write(dev,
			 AD9680_REG_ADC_TEST_MODE,
			 test_mode);
	if (test_mode == AD9680_TEST_OFF)
		ad9680_spi_write(dev,
				 AD9680_REG_OUTPUT_MODE,
				 AD9680_FORMAT_2S_COMPLEMENT);
	else
		ad9680_spi_write(dev,
				 AD9680_REG_OUTPUT_MODE,
				 AD9680_FORMAT_OFFSET_BINARY);
	return(0);
}

static int ad9680_jesd204_link_init(struct jesd204_dev *jdev,
				    enum jesd204_state_op_reason reason,
				    struct jesd204_link *lnk)
{
	struct ad9680_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9680_dev *dev = priv->dev;
	struct jesd204_link *link;

	switch (reason) {
	case JESD204_STATE_OP_REASON_INIT:
		break;
	default:
		return JESD204_STATE_CHANGE_DONE;
	}

	pr_debug("%s:%d link_num %u reason %s\n", __func__,
		 __LINE__, lnk->link_id, jesd204_state_op_reason_str(reason));

	link = &dev->jesd204_link;

	jesd204_copy_link_params(lnk, link);

	lnk->sample_rate = dev->sampling_frequency_hz;
	lnk->sample_rate_div = dev->dcm;
	lnk->jesd_encoder = JESD204_ENCODER_8B10B;

	if (dev->sysref_mode == AD9680_SYSREF_CONT)
		lnk->sysref.mode = JESD204_SYSREF_CONTINUOUS;
	else if (dev->sysref_mode == AD9680_SYSREF_ONESHOT)
		lnk->sysref.mode = JESD204_SYSREF_ONESHOT;

	return JESD204_STATE_CHANGE_DONE;
}

int ad9680_jesd_enable_link(struct ad9680_dev *dev, uint8_t en)
{
	int err;
	uint8_t tmp_reg;

	if (dev == NULL)
		return -ENODEV;

	if (en > 1)
		return -EINVAL;

	err = ad9680_spi_read(dev, AD9680_REG_JESD_LINK_CTRL1_REG, &tmp_reg);
	if (err)
		return err;
	tmp_reg &= ~AD9680_JESD_LINK_PDN;
	tmp_reg |= en ? 0 : AD9680_JESD_LINK_PDN;
	return ad9680_spi_write(dev, AD9680_REG_JESD_LINK_CTRL1_REG, tmp_reg);
}

static int ad9680_jesd204_clks_enable(struct jesd204_dev *jdev,
				      enum jesd204_state_op_reason reason,
				      struct jesd204_link *lnk)
{
	struct ad9680_jesd204_priv *priv = jesd204_dev_priv(jdev);
	struct ad9680_dev *dev = priv->dev;
	int ret;

	pr_debug("%s:%d link_num %u reason %s\n", __func__,
		 __LINE__, lnk->link_id, jesd204_state_op_reason_str(reason));

	ret = ad9680_jesd_enable_link(dev,
				      reason == JESD204_STATE_OP_REASON_INIT);
	if (ret < 0) {
		pr_err("Failed to enabled JESD204 link (%d)\n", ret);
		return ret;
	}

	return JESD204_STATE_CHANGE_DONE;
}


static int ad9680_jesd204_link_enable(struct jesd204_dev *jdev,
				      enum jesd204_state_op_reason reason,
				      struct jesd204_link *lnk)
{
	pr_debug("%s:%d link_num %u reason %s\n", __func__,
		 __LINE__, lnk->link_id, jesd204_state_op_reason_str(reason));

	return JESD204_STATE_CHANGE_DONE;
}

static const struct jesd204_dev_data jesd204_ad9680_init = {
	.state_ops = {
		[JESD204_OP_LINK_INIT] = {
			.per_link = ad9680_jesd204_link_init,
		},
		[JESD204_OP_CLOCKS_ENABLE] = {
			.per_link = ad9680_jesd204_clks_enable,
		},
		[JESD204_OP_LINK_ENABLE] = {
			.per_link = ad9680_jesd204_link_enable,
			.post_state_sysref = true,
		},
	},

	.max_num_links = 1,
	.num_retries = 3,
	.sizeof_priv = sizeof(struct ad9680_jesd204_priv),
};

/***************************************************************************//**
 * @brief ad9680_setup
 *******************************************************************************/
int32_t ad9680_setup(struct ad9680_dev **device,
		     const struct ad9680_init_param *init_param)
{
	uint8_t chip_id;
	uint8_t pll_stat;
	int32_t ret;
	struct ad9680_dev *dev;

	ret = 0;

	dev = (struct ad9680_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, &init_param->spi_init);

	ad9680_spi_read(dev,
			AD9680_REG_CHIP_ID_LOW,
			&chip_id);
	if(chip_id != AD9680_CHIP_ID) {
		printf("AD9680: Invalid CHIP ID (0x%x).\n", chip_id);
		return -1;
	}

	ad9680_spi_write(dev,
			 AD9680_REG_INTERFACE_CONF_A,
			 0x81);	// RESET
	no_os_mdelay(250);

	ad9680_spi_write(dev,
			 AD9680_REG_LINK_CONTROL,
			 0x15);	// disable link, ilas enable
	ad9680_spi_write(dev,
			 AD9680_REG_JESD204B_MF_CTRL,
			 0x1f);	// mf-frame-count
	ad9680_spi_write(dev,
			 AD9680_REG_JESD204B_CSN_CONFIG,
			 0x2d);	// 14-bit
	ad9680_spi_write(dev,
			 AD9680_REG_JESD204B_SUBCLASS_CONFIG,
			 0x2f);	// subclass-1, N'=16
	ad9680_spi_write(dev,
			 AD9680_REG_JESD204B_QUICK_CONFIG,
			 0x88);	// m=2, l=4, f= 1
	if (init_param->lane_rate_kbps < 6250000)
		ad9680_spi_write(dev,
				 AD9680_REG_JESD204B_LANE_RATE_CTRL,
				 0x10);	// low line rate mode must be enabled
	else
		ad9680_spi_write(dev,
				 AD9680_REG_JESD204B_LANE_RATE_CTRL,
				 0x00);	// low line rate mode must be disabled
	ad9680_spi_write(dev,
			 AD9680_REG_LINK_CONTROL,
			 0x14);	// link enable
	no_os_mdelay(250);

	ad9680_spi_read(dev,
			AD9680_REG_JESD204B_PLL_LOCK_STATUS,
			&pll_stat);
	if ((pll_stat & 0x80) != 0x80) {
		printf("AD9680: PLL is NOT locked!\n");
		ret = -1;
	}

	*device = dev;

	return ret;
}

/*******************************************************************************
 * @brief ad9680_setup_jesd_fsm
 *******************************************************************************/
int32_t ad9680_setup_jesd_fsm(struct ad9680_dev **device,
			      const struct ad9680_init_param *init_param)
{
	struct ad9680_jesd204_priv *priv;
	uint8_t chip_id;
	uint8_t pll_stat;
	int32_t ret;
	struct ad9680_dev *dev;

	ret = 0;

	dev = (struct ad9680_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, &init_param->spi_init);

	ad9680_spi_read(dev,
			AD9680_REG_CHIP_ID_LOW,
			&chip_id);
	if(chip_id != AD9680_CHIP_ID) {
		printf("AD9680: Invalid CHIP ID (0x%x).\n", chip_id);
		return -EINVAL;
	}

	ad9680_spi_write(dev,
			 AD9680_REG_INTERFACE_CONF_A,
			 0x81);	// RESET
	no_os_mdelay(250);

	ad9680_spi_write(dev,
			 AD9680_REG_LINK_CONTROL,
			 0x15);	// disable link, ilas enable
	ad9680_spi_write(dev,
			 AD9680_REG_JESD204B_MF_CTRL,
			 0x1f);	// mf-frame-count
	ad9680_spi_write(dev,
			 AD9680_REG_JESD204B_CSN_CONFIG,
			 0x2d);	// 14-bit
	ad9680_spi_write(dev,
			 AD9680_REG_JESD204B_SUBCLASS_CONFIG,
			 0x2f);	// subclass-1, N'=16
	ad9680_spi_write(dev,
			 AD9680_REG_JESD204B_QUICK_CONFIG,
			 0x88);	// m=2, l=4, f= 1
	if (init_param->lane_rate_kbps < 6250000)
		ad9680_spi_write(dev,
				 AD9680_REG_JESD204B_LANE_RATE_CTRL,
				 0x10);	// low line rate mode must be enabled
	else
		ad9680_spi_write(dev,
				 AD9680_REG_JESD204B_LANE_RATE_CTRL,
				 0x00);	// low line rate mode must be disabled
	ad9680_spi_write(dev,
			 AD9680_REG_LINK_CONTROL,
			 0x14);	// link enable
	no_os_mdelay(250);

	ad9680_spi_read(dev,
			AD9680_REG_JESD204B_PLL_LOCK_STATUS,
			&pll_stat);
	if ((pll_stat & 0x80) != 0x80) {
		printf("AD9680: PLL is NOT locked!\n");
		ret = -1;
	}

	dev->sampling_frequency_hz = init_param->sampling_frequency_hz;
	dev->dcm = init_param->dcm;
	dev->sysref_mode = init_param->sysref_mode;

	ret = jesd204_dev_register(&dev->jdev, &jesd204_ad9680_init);
	if (ret)
		return ret;
	priv = jesd204_dev_priv(dev->jdev);
	priv->dev = dev;

	*device = dev;

	return ret;
}


/***************************************************************************//**
 * @brief Free the resources allocated by ad9680_setup().
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad9680_remove(struct ad9680_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_desc);

	no_os_free(dev);

	return ret;
}
