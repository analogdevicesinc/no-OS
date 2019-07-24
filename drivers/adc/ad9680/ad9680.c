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

	ret = spi_write_and_read(dev->spi_desc,
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

	ret = spi_write_and_read(dev->spi_desc,
				 buf,
				 3);

	return ret;
}

/***************************************************************************//**
 * @brief ad9680_setup
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

	dev = (struct ad9680_dev *)malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* SPI */
	ret = spi_init(&dev->spi_desc, &init_param->spi_init);

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
	mdelay(250);

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
	mdelay(250);

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

/***************************************************************************//**
 * @brief Free the resources allocated by ad9680_setup().
 *
 * @param dev - The device structure.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad9680_remove(struct ad9680_dev *dev)
{
	int32_t ret;

	ret = spi_remove(dev->spi_desc);

	free(dev);

	return ret;
}
