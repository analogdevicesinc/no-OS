/***************************************************************************//**
 * @file ad9553.c
 * @brief Implementation of AD9553 Driver.
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
#include "ad9553.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"

/**
 * @brief Reads from the ad9553 that is contected to the SPI
 * @param dev - The ad9553 device handler
 * @param reg_addr - The address of the internal register of the ad9553 chip
 * @param reg_data - The value read from the internal register
 * @return 0 if the value was successfully read, -1 otherwise
 */
int32_t ad9553_reg_read(struct ad9553_dev *dev,
			uint16_t reg_addr,
			uint8_t *reg_data)
{
	uint8_t buf[3];

	int32_t ret;

	// the MSB of byte 0 indicates a r/w operation, following 7 bits are the
	// bits 14-8 of the address of the register that is accessed. Byte 1
	// contains the bits 7-0 of the address of the register.
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
 * @brief Write to the ad9553 that is conected to the SPI
 * @param dev - The device handler for the ad9553 chip
 * @param reg_addr - Address of the internal register of the ad9553 chip
 * @param reg_data - Value to be written to the register
 * @return 0 if the value was written successfully, -1 otherwise
 */
int32_t ad9553_reg_write(struct ad9553_dev *dev,
			 uint16_t reg_addr,
			 uint8_t reg_data)
{
	uint8_t buf[3];

	// the MSB of byte 0 indicates a r/w operation, following 7 bits are the
	// bits 14-8 of the address of the register that is accessed. Byte 1
	// contains the bits 7-0 of the address of the register. Byte 2 contains
	// the data to be written.
	buf[0] = reg_addr >> 8;
	buf[1] = reg_addr & 0xFF;
	buf[2] = reg_data;

	return no_os_spi_write_and_read(dev->spi_desc, buf, 3);
}

/**
 * @brief Setup the working parameters of the ad9553 chip
 * @param device - The device handler of the ad9553 chip
 * @param init_param - Values for the working parameters of ad9553
 * @return 0 if device is ready for use, -1 otherwise
 */
int32_t ad9553_setup(struct ad9553_dev **device,
		     const struct ad9553_init_param *init_param)
{
	int32_t ret;

	struct ad9553_dev *dev;

	dev = (struct ad9553_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, &init_param->spi_init);
	if (ret != 0)
		return ret;

	// reset
	ret = ad9553_reg_write(dev, AD9553_SPI_CONFIG, 0x3C);
	if (ret != 0)
		return ret;

	no_os_mdelay(250);

	// enable SPI control of charge pump
	ret = ad9553_reg_write(dev, AD9553_PLL_CHARGE_PUMP_PFD_CTRL, 0xB0);
	if (ret != 0)
		return ret;

	// lock detector activated
	ret = ad9553_reg_write(dev, AD9553_PLL_CTRL, 0x00);
	if (ret != 0)
		return ret;

	// P1 = 4
	// P1[9:2]
	ret = ad9553_reg_write(dev, AD9553_P1_DIV_HIGH, 0x01);
	if (ret != 0)
		return ret;

	// P1[1:0], P2[9:4]
	ret = ad9553_reg_write(dev, AD9553_P1_DIV_LOW_P2_DIV_HIGH, 0x00);
	if (ret != 0)
		return ret;

	// P2[3:0]
	ret = ad9553_reg_write(dev, AD9553_P2_DIV_LOW, 0x00);
	if (ret != 0)
		return ret;

	// P0 = 7
	ret = ad9553_reg_write(dev, AD9553_P0_DIV, 0x60);
	if (ret != 0)
		return ret;

	// N = 700
	// N[19:12]
	ret = ad9553_reg_write(dev, AD9553_N_DIV_HIGH, 0x00);
	if (ret != 0)
		return ret;

	// N[11:4]
	ret = ad9553_reg_write(dev, AD9553_N_DIV_MEDIUM, 0x2B);
	if (ret != 0)
		return ret;

	// N[3:0], take the N value from the feedback divide register,
	// take the output divider values from the registers, reset
	// counters and logic of the PLL
	ret = ad9553_reg_write(dev, AD9553_N_DIV_LOW, 0xCC);
	if (ret != 0)
		return ret;

	no_os_mdelay(250);

	// RefA = 10
	// RefA[13:6] divider
	ret = ad9553_reg_write(dev, AD9553_REFA_DIV_HIGH, 0x00);
	if (ret != 0)
		return ret;

	// RefA[5:0], use the value stored in RefA register for RefA divider value
	ret = ad9553_reg_write(dev, AD9553_REFA_DIV_LOW, 0x2A);
	if (ret != 0)
		return ret;

	// k = 2/5
	// enable SPI control for x2 for RefA, select x2 for RefA,
	// enable SPI control for :5 for RefA, select :5 for RefA
	ret = ad9553_reg_write(dev, AD9553_K_VALUE, 0xF0);
	if (ret != 0)
		return ret;

	// RefA is configured as a differential input so RefDiff = 1
	ret = ad9553_reg_write(dev, AD9553_REFA_DIFF, 0xA0);
	if (ret != 0)
		return ret;

	// Out1 drive strength diven by SPI configuration, Out1 driver mode
	// selection LVDS , the rest of the settings for this register
	// remain default
	ret = ad9553_reg_write(dev, AD9553_OUT1_DRIVER_CTRL, 0xA1);
	if (ret != 0)
		return ret;

	// Out2 powered down
	ret = ad9553_reg_write(dev, AD9553_OUT2_DRIVER_CTRL, 0xE8);
	if (ret != 0)
		return ret;

	// update contents of the registers with the new values
	ret = ad9553_reg_write(dev, AD9553_IO_UPDATE, 0x01);
	if (ret != 0)
		return ret;

	return 0;
}

/**
 * @brief Free the resources allocated by ad9553_setup().
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad9553_remove(struct ad9553_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret != 0)
		return ret;

	no_os_free(dev);

	return ret;
}
