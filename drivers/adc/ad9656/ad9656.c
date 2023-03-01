/***************************************************************************//**
 * @file ad9656.c
 * @brief Implementation of AD9656 Driver.
 * @author DHotolea (dan.hotoleanu@analog.com)
 ********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "no_os_error.h"
#include "ad9656.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"

/**
 * @brief Reads from the ad9656 that is contected to the SPI
 * @param dev - The ad9656 device handler
 * @param reg_addr - The address of the internal register of the ad9656 chip
 * @param reg_data - The value read from the internal register
 * @return 0 if the value was successfully read, -1 otherwise
 */
int32_t ad9656_reg_read(struct ad9656_dev *dev,
			uint16_t reg_addr,
			uint8_t *reg_data)
{
	uint8_t buf[3];

	int32_t ret;

	/*
	 * the MSB of byte 0 indicates a r/w operation, following 7 bits are the
	 * bits 14-8 of the address of the register that is accessed. Byte 1
	 * contains the bits 7-0 of the address of the register.
	 */
	buf[0] = 0x80 | (reg_addr >> 8);
	buf[1] = reg_addr & 0xFF;
	buf[2] = 0x00;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 3);
	if (ret < 0)
		return ret;

	*reg_data = buf[2];

	return ret;
}

/**
 * @brief Write to the ad9656 that is conected to the SPI
 * @param dev - The device handler for the ad9656 chip
 * @param reg_addr - Address of the internal register of the ad9656 chip
 * @param reg_data - Value to be written to the register
 * @return 0 if the value was written successfully, -1 otherwise
 */
int32_t ad9656_reg_write(struct ad9656_dev *dev,
			 uint16_t reg_addr,
			 uint8_t reg_data)
{
	uint8_t buf[3];

	/*
	 * the MSB of byte 0 indicates a r/w operation, following 7 bits are the
	 * bits 14-8 of the address of the register that is accessed. Byte 1
	 * contains the bits 7-0 of the address of the register. Byte 2 contains
	 * the data to be written.
	 */
	buf[0] = reg_addr >> 8;
	buf[1] = reg_addr & 0xFF;
	buf[2] = reg_data;

	return no_os_spi_write_and_read(dev->spi_desc, buf, 3);
}

/**
 * @brief Launch the JESD204 test functionality for the receive path for the ad9656 chip
 * @param dev - The device handler for the ad9656 chip
 * @param test_mode - The type of test that is to be performed or OFF if the testing
 * 					  process is to be stopped
 * @return 0 if the ad9656 chip could be successfully set for JESD204 link testing,
 * 		   -1 otherwise
 */
int32_t ad9656_JESD204_test(struct ad9656_dev *dev,
			    uint32_t test_mode)
{
	uint8_t format;
	int32_t ret;

	ret = ad9656_reg_write(dev, AD9656_REG_ADC_TEST_MODE, test_mode);
	if (ret != 0)
		return ret;

	if (test_mode == AD9656_TEST_OFF)
		format = AD9656_FORMAT_OFFSET_BINARY;
	else
		format = AD9656_FORMAT_2S_COMPLEMENT;

	return ad9656_reg_write(dev, AD9656_REG_OUTPUT_MODE, format);
}

/**
 * @brief Launch the user input test functionality for the receive path for the ad9656 chip.
 * 	      User input data is supplied on the outputs instead of real data captured on the ADC.
 * @param dev - The device handler for the ad9656 chip
 * @param test_mode - The type of test that is to be performed or OFF if the testing
 * 					  process is to be stopped
 * @param user_input_test_pattern - User input test pattern that is sent on the output instead of
 * 					the ADC	data
 * @return 0 if the ad9656 chip could be successfully set for user input testing, FAILURE
 * 		   otherwise
 */
int32_t ad9656_user_input_test(struct ad9656_dev *dev, uint32_t test_mode,
			       struct ad9656_user_input_test_pattern user_input_test_pattern)
{
	int32_t ret;

	/* write user input test pattern in the registers */
	ret = ad9656_reg_write(dev, AD9656_REG_USER_TEST_PATTERN_1_LSB,
			       user_input_test_pattern.user_test_pattern1 & 0xFF);
	if (ret != 0)
		return ret;

	ret = ad9656_reg_write(dev, AD9656_REG_USER_TEST_PATTERN_1_MSB,
			       user_input_test_pattern.user_test_pattern1 >> 8);
	if (ret != 0)
		return ret;

	ret = ad9656_reg_write(dev, AD9656_REG_USER_TEST_PATTERN_2_LSB,
			       user_input_test_pattern.user_test_pattern2 & 0xFF);
	if (ret != 0)
		return ret;

	ret = ad9656_reg_write(dev, AD9656_REG_USER_TEST_PATTERN_2_MSB,
			       user_input_test_pattern.user_test_pattern2 >> 8);
	if (ret != 0)
		return ret;

	/* determine the chip to output the user test pattern */
	return ad9656_reg_write(dev, AD9656_REG_ADC_TEST_MODE, test_mode);
}

/**
 * @brief Setup the working parameters of the ad9656 chip
 * @param device - The device handler of the ad9656 chip
 * @param init_param - Values for the working parameters of ad9656
 * @return 0 if device is ready for use, -1 otherwise
 */
int32_t ad9656_setup(struct ad9656_dev **device,
		     const struct ad9656_init_param *init_param)
{
	uint8_t chip_id;
	uint8_t pll_stat;
	int32_t ret;
	struct ad9656_dev *dev;
	uint8_t tmp;

	dev = (struct ad9656_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -1;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, &init_param->spi_init);
	if (ret != 0)
		return ret;

	ret = ad9656_reg_write(dev, AD9656_SPI_CONFIG, 0x3C);	/* RESET */
	if (ret != 0)
		return ret;

	no_os_mdelay(250);

	ret = ad9656_reg_read(dev, AD9656_REG_CHIP_ID, &chip_id);
	if (ret != 0)
		return ret;

	if (chip_id != AD9656_CHIP_ID) {
		printf("AD9656: Invalid CHIP ID (0x%x).\n", chip_id);
		return -1;
	}

	ret = ad9656_reg_write(dev, AD9656_REG_LINK_CONTROL,
			       0x15);	/* disable link, ilas enable */
	if (ret != 0)
		return ret;

	ret = ad9656_reg_write(dev, AD9656_REG_JESD204B_MF_CTRL,
			       0x1f);	/* 32 frames per multiframe */
	if (ret != 0)
		return ret;

	ret = ad9656_reg_write(dev, AD9656_REG_JESD204B_M_CTRL,
			       0x03);	/* 4 converters */

	if (ret != 0)
		return ret;

	ret = ad9656_reg_write(dev, AD9656_REG_JESD204B_CSN_CONFIG,
			       0x0d);	/* converter resolution of 14-bit */
	if (ret != 0)
		return ret;

	ret = ad9656_reg_write(dev, AD9656_REG_JESD204B_SUBCLASS_CONFIG,
			       0x2f);	/* subclass-1, N'=16 */
	if (ret != 0)
		return ret;

	ret = ad9656_reg_write(dev, AD9656_REG_JESD204B_QUICK_CONFIG,
			       0x44);	/* m=4, l=4 */
	if (ret != 0)
		return ret;

	ret = ad9656_reg_write(dev, AD9656_REG_JESD204B_SCR_L,
			       0x83);	/* enable scrambling, l=4 */
	if (ret != 0)
		return ret;

	if (init_param->lane_rate_kbps < 2000000)
		tmp = 0x08;  /* low line rate mode must be enabled */
	else
		tmp = 0x00;  /* low line rate mode must be disabled */

	ret = ad9656_reg_write(dev, AD9656_REG_JESD204B_LANE_RATE_CTRL,
			       tmp);
	if (ret != 0)
		return ret;

	ret = ad9656_reg_write(dev, AD9656_REG_LINK_CONTROL, 0x14);	/* link enable */
	if (ret != 0)
		return ret;

	no_os_mdelay(250);

	ret = ad9656_reg_read(dev, AD9656_REG_JESD204B_PLL_LOCK_STATUS, &pll_stat);
	if (ret != 0)
		return ret;

	if ((pll_stat & 0x80) != 0x80) {
		printf("AD9656: PLL is NOT locked!\n");
		ret = -1;
	}

	*device = dev;

	return ret;
}

/**
 * @brief Free the resources allocated by ad9656_setup().
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad9656_remove(struct ad9656_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_desc);

	if (ret == 0)
		no_os_free(dev);

	return ret;
}
