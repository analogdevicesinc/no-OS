/***************************************************************************//**
 *   @file   adgs1408.c
 *   @brief  Implementation of ADGS1408 Driver.
 *   @author Mircea Caprioru (mircea.caprioru@analog.com)
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
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "adgs1408.h"
#include "no_os_error.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/
/**
 * Compute CRC8 checksum.
 * @param data - The data buffer.
 * @param data_size - The size of the data buffer.
 * @return CRC8 checksum.
 */
uint8_t adgs1408_compute_crc8(uint8_t *data,
			      uint8_t data_size)
{
	uint8_t i;
	uint8_t crc = 0;

	while (data_size) {
		for (i = 0x80; i != 0; i >>= 1) {
			if (((crc & 0x80) != 0) != ((*data & i) != 0)) {
				crc <<= 1;
				crc ^= ADGS1408_CRC8_POLY;
			} else {
				crc <<= 1;
			}
		}
		data++;
		data_size--;
	}

	return crc;
}

/**
 * SPI register read from device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adgs1408_spi_reg_read(struct adgs1408_dev *dev,
			      uint8_t reg_addr,
			      uint8_t *reg_data)
{
	uint8_t buf[3];
	uint8_t buf_size = 2;
	uint8_t crc;
	int32_t ret;

	if (dev->daisy_chain_en == ADGS1408_ENABLE) {
		printf("%s: This feature is not available in Daisy-Chain mode.\n",
		       __func__);

		return -1;
	}

	buf[0] = 0x80 | (reg_addr & 0x7F);
	buf[1] = 0x00;
	buf[2] = 0x00;
	if (dev->crc_en == ADGS1408_ENABLE)
		buf_size = 3;
	ret = no_os_spi_write_and_read(dev->spi_desc, buf, buf_size);

	if(ret < 0)
		return -1;

	if (buf[0] != ADGS1408_ALIGNMENT) {
		printf("%s: Alignment Error: 0x%x.\n", __func__, buf[0]);

		return -1;
	}
	*reg_data = buf[1];
	if (dev->crc_en == ADGS1408_ENABLE) {
		buf[0] = 0x80 | (reg_addr & 0x7F);
		crc = adgs1408_compute_crc8(&buf[0], 2);
		if (crc != buf[2]) {
			printf("%s: CRC Error.\n", __func__);

			return -1;
		}
	}

	return 0;
}

/**
 * SPI register write to device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adgs1408_spi_reg_write(struct adgs1408_dev *dev,
			       uint8_t reg_addr,
			       uint8_t reg_data)
{
	uint8_t buf[3];
	uint8_t buf_size = 2;

	if (dev->daisy_chain_en == ADGS1408_ENABLE) {
		printf("%s: This feature is not available in Daisy-Chain mode.\n",
		       __func__);

		return -1;
	}

	buf[0] = 0x00 | (reg_addr & 0x7F);
	buf[1] = reg_data;
	if (dev->crc_en == ADGS1408_ENABLE) {
		buf[2] = adgs1408_compute_crc8(&buf[0], 2);
		buf_size = 3;
	}
	no_os_spi_write_and_read(dev->spi_desc, buf, buf_size);
	if (buf[0] != ADGS1408_ALIGNMENT) {
		printf("%s: Alignment Error: 0x%x.\n", __func__, buf[0]);

		return -1;
	}

	return 0;
}

/**
 * SPI internal register write to device using a mask.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - The mask.
 * @param data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adgs1408_spi_reg_write_mask(struct adgs1408_dev *dev,
				    uint8_t reg_addr,
				    uint8_t mask,
				    uint8_t data)
{
	uint8_t reg_data;
	int32_t ret;

	if (dev->daisy_chain_en == ADGS1408_ENABLE) {
		printf("%s: This feature is not available in Daisy-Chain mode.\n",
		       __func__);

		return -1;
	}

	ret = adgs1408_spi_reg_read(dev, reg_addr, &reg_data);
	reg_data &= ~mask;
	reg_data |= data;
	ret |= adgs1408_spi_reg_write(dev, reg_addr, reg_data);

	return ret;
}

/**
 * Do a software reset.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adgs1408_do_soft_reset(struct adgs1408_dev *dev)
{
	int32_t ret;

	if (dev->daisy_chain_en == ADGS1408_ENABLE) {
		printf("%s: This feature is not available in Daisy-Chain mode.\n",
		       __func__);

		return -1;
	}

	ret = adgs1408_spi_reg_write(dev,
				     ADGS1408_REG_SOFT_RESETB,
				     ADGS1408_RESET_1);
	ret |= adgs1408_spi_reg_write(dev,
				      ADGS1408_REG_SOFT_RESETB,
				      ADGS1408_RESET_2);

	return ret;
}

/**
 * Clear the Error Flags Register.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adgs1408_clear_err_flags(struct adgs1408_dev *dev)
{
	uint8_t buf[3];
	uint8_t buf_size = 2;

	if (dev->daisy_chain_en == ADGS1408_ENABLE) {
		printf("%s: This feature is not available in Daisy-Chain mode.\n",
		       __func__);

		return -1;
	}

	buf[0] = ADGS1408_CLR_1;
	buf[1] = ADGS1408_CLR_2;
	if (dev->crc_en == ADGS1408_ENABLE) {
		buf[2] = adgs1408_compute_crc8(&buf[0], 2);
		buf_size = 3;
	}

	return no_os_spi_write_and_read(dev->spi_desc, buf, buf_size);
}

/**
 * Enter Daisy-Chain Mode.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adgs1408_enter_daisy_chain(struct adgs1408_dev *dev)
{
	uint8_t buf[2];

	if (dev->daisy_chain_en == ADGS1408_ENABLE) {
		printf("%s: This feature is not available in Daisy-Chain mode.\n",
		       __func__);

		return -1;
	}

	buf[0] = ADGS1408_DAISY_CHAIN_1;
	buf[1] = ADGS1408_DAISY_CHAIN_2;

	return no_os_spi_write_and_read(dev->spi_desc, buf, 2);
}

/**
 * Send Daisy-Chain commands.
 * @param dev - The device structure.
 * @param cmds - The commands to be sent.
 * @param cmds_size - The number of commands.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adgs1408_send_daisy_chain_cmds(struct adgs1408_dev *dev,
				       uint8_t *cmds,
				       uint8_t cmds_size)
{

	if (dev->daisy_chain_en == ADGS1408_DISABLE) {
		printf("%s: This feature is available in Daisy-Chain mode only.\n",
		       __func__);

		return -1;
	}

	return no_os_spi_write_and_read(dev->spi_desc, cmds, cmds_size);
}

/**
 * Enter Round Robin Mode.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adgs1408_enter_round_robin(struct adgs1408_dev *dev)
{
	uint8_t buf[2];

	if (dev->round_robin_en == ADGS1408_ENABLE) {
		printf("%s: This feature is not available in Round Robin mode.\n",
		       __func__);

		return -1;
	}

	buf[0] = ADGS1408_REG_ROUND_ROBIN_EN;
	buf[1] = ADGS1408_ROUND_ROBIN_EN;

	return no_os_spi_write_and_read(dev->spi_desc, buf, 2);
}

/**
 * Configure Round Robin Mode.
 * @param dev - The device structure.
 * @param cnv_polarity - The CNV pin polarity.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adgs1408_configure_round_robin(struct adgs1408_dev *dev,
				       uint8_t cnv_polarity)
{
	uint8_t reg_config;
	uint8_t buf[2];
	int32_t ret;

	if(dev->act_device == ID_ADGS1408) {
		reg_config = ADGS1408_RROBIN_SW1(dev->rrobin_sw_config.rrobin_sw1) |
			     ADGS1408_RROBIN_SW2(dev->rrobin_sw_config.rrobin_sw2) |
			     ADGS1408_RROBIN_SW3(dev->rrobin_sw_config.rrobin_sw3) |
			     ADGS1408_RROBIN_SW4(dev->rrobin_sw_config.rrobin_sw4) |
			     ADGS1408_RROBIN_SW5(dev->rrobin_sw_config.rrobin_sw5) |
			     ADGS1408_RROBIN_SW6(dev->rrobin_sw_config.rrobin_sw6) |
			     ADGS1408_RROBIN_SW7(dev->rrobin_sw_config.rrobin_sw7) |
			     ADGS1408_RROBIN_SW8(dev->rrobin_sw_config.rrobin_sw8);
	} else {
		reg_config = ADGS1408_RROBIN_SW1(dev->rrobin_sw_config.rrobin_sw1) |
			     ADGS1408_RROBIN_SW2(dev->rrobin_sw_config.rrobin_sw2) |
			     ADGS1408_RROBIN_SW3(dev->rrobin_sw_config.rrobin_sw3) |
			     ADGS1408_RROBIN_SW4(dev->rrobin_sw_config.rrobin_sw4);
	}

	buf[0] = ADGS1408_REG_RROBIN_CHANNEL_CONFIG;
	buf[1] = reg_config;
	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 2);

	if(ret != -1) {
		buf[0] = ADGS1408_REG_CNV_EDGE_SEL;
		buf[1] = cnv_polarity;
		ret |= no_os_spi_write_and_read(dev->spi_desc, buf, 2);
	}

	return ret;
}

/**
 * Exit Round Robin Mode.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adgs1408_exit_round_robin(struct adgs1408_dev *dev)
{
	uint8_t buf[2];
	int32_t ret;

	buf[0] = ADGS1408_RROBIN_EXIT_1;
	buf[1] = ADGS1408_RROBIN_EXIT_2;
	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 2);

	if(ret != -1) {
		buf[0] = ADGS1408_RROBIN_EXIT_3;
		buf[1] = ADGS1408_RROBIN_EXIT_4;
		ret |= no_os_spi_write_and_read(dev->spi_desc, buf, 2);
	}

	return ret;
}

/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adgs1408_init(struct adgs1408_dev **device,
		      struct adgs1408_init_param init_param)
{
	struct adgs1408_dev *dev;
	int32_t ret;

	dev = (struct adgs1408_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);
	ret |= adgs1408_do_soft_reset(dev);

	/* Device Settings */
	dev->crc_en = ADGS1408_DISABLE;
	dev->daisy_chain_en = ADGS1408_DISABLE;
	dev->round_robin_en = ADGS1408_DISABLE;
	ret |= adgs1408_do_soft_reset(dev);

	if (init_param.crc_en == ADGS1408_ENABLE) {
		ret |= adgs1408_spi_reg_write_mask(dev,
						   ADGS1408_REG_ERR_CONFIG,
						   ADGS1408_CRC_ERR_EN,
						   ADGS1408_CRC_ERR_EN);
		dev->crc_en = ADGS1408_ENABLE;
	}

	dev->burst_mode_en = init_param.burst_mode_en;
	if (dev->burst_mode_en == ADGS1408_ENABLE)
		ret |= adgs1408_spi_reg_write_mask(dev,
						   ADGS1408_REG_BURST_EN,
						   ADGS1408_BURST_MODE_EN,
						   ADGS1408_BURST_MODE_EN);

	if (init_param.daisy_chain_en == ADGS1408_ENABLE) {
		ret |= adgs1408_enter_daisy_chain(dev);
		dev->daisy_chain_en = ADGS1408_ENABLE;
	}

	if (init_param.round_robin_en == ADGS1408_ENABLE) {
		dev->rrobin_sw_config = init_param.rrobin_sw_config;
		ret |= adgs1408_configure_round_robin(dev, 0);
		ret |= adgs1408_enter_round_robin(dev);
		dev->round_robin_en = ADGS1408_ENABLE;
	}

	*device = dev;

	if (!ret)
		printf("ADGS1408 successfully initialized\n");
	else
		printf("ADGS1408 initialization error (%d)\n", ret);

	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by adgs1408_init().
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t adgs1408_remove(struct adgs1408_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_desc);

	no_os_free(dev);

	return ret;
}
