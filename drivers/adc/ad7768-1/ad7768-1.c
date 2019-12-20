/***************************************************************************//**
 *   @file   ad77681.c
 *   @brief  Implementation of AD7768-1 Driver.
 *   @author SPopa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2017(c) Analog Devices, Inc.
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
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include <string.h>
#include "ad7768-1.h"

/******************************************************************************/
/********************************* Macros *************************************/
/******************************************************************************/

#define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/
/**
 * Compute CRC8 checksum.
 * @param data - The data buffer.
 * @param data_size - The size of the data buffer.
 * @return CRC8 checksum.
 */
uint8_t ad77681_compute_crc8(uint8_t *data,
			     uint8_t data_size)
{
	uint8_t i;
	uint8_t crc = 0;

	while (data_size) {
		for (i = 0x80; i != 0; i >>= 1) {
			if (((crc & 0x80) != 0) != ((*data & i) != 0)) {
				crc <<= 1;
				crc ^= AD77681_CRC8_POLY;
			} else
				crc <<= 1;
		}
		data++;
		data_size--;
	}
	return crc;
}

/**
 * Compute XOR checksum.
 * @param data - The data buffer.
 * @param data_size - The size of the data buffer.
 * @return XOR checksum.
 */
uint8_t ad77681_compute_xor(uint8_t *data,
			    uint8_t data_size)
{
	uint8_t crc = 0;
	uint8_t buf[3];
	uint8_t i;

	for (i = 0; i < data_size; i++) {
		buf[i] = *data;
		crc ^= buf[i];
		data++;
	}
	return crc;
}

/**
 * Read from device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_spi_reg_read(struct ad77681_dev *dev,
			     uint8_t reg_addr,
			     uint8_t *reg_data)
{
	int32_t ret;
	uint8_t buf[3];
	uint8_t word_len = (dev->crc_sel == AD77681_NO_CRC) ? 16 : 24;

	//spi_eng_set_transfer_length(dev->spi_desc,word_len);

	buf[0] = AD77681_REG_READ(reg_addr);
	buf[1] = 0x00;

	ret = spi_write_and_read(dev->spi_desc, buf, word_len / 8);
	if (ret < 0)
		return ret;

	memcpy(reg_data, buf, ARRAY_SIZE(buf));

	return 0;
}

/**
 * Write to device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_spi_reg_write(struct ad77681_dev *dev,
			      uint8_t reg_addr,
			      uint8_t reg_data)
{
	uint8_t buf[2];

	buf[0] = AD77681_REG_WRITE(reg_addr);
	buf[1] = reg_data;
	//spi_eng_set_transfer_length(dev->spi_desc,16);
	return spi_write_and_read(dev->spi_desc, buf, ARRAY_SIZE(buf));
}

/**
 * SPI read from device using a mask.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - The mask.
 * @param data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_spi_read_mask(struct ad77681_dev *dev,
			      uint8_t reg_addr,
			      uint8_t mask,
			      uint8_t *data)
{
	uint8_t reg_data[3];
	int32_t ret;

	ret = ad77681_spi_reg_read(dev, reg_addr, reg_data);
	*data = (reg_data[1] & mask);

	return ret;
}

/**
 * SPI write to device using a mask.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - The mask.
 * @param data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_spi_write_mask(struct ad77681_dev *dev,
			       uint8_t reg_addr,
			       uint8_t mask,
			       uint8_t data)
{
	uint8_t reg_data[3];
	int32_t ret;

	ret = ad77681_spi_reg_read(dev, reg_addr, reg_data);
	reg_data[0] &= ~mask;
	reg_data[0] |= data;
	ret |= ad77681_spi_reg_write(dev, reg_addr, reg_data[0]);

	return ret;
}

/**
 * Helper function to get the number of rx bytes
 * @param dev - The device structure.
 * @return rx_buf_len - the number of rx bytes
 */
uint8_t ad77681_get_rx_buf_len(struct ad77681_dev *dev)
{
	uint8_t rx_buf_len = 0;
	uint8_t data_len = 0;
	uint8_t crc = 0;
	uint8_t status_bit = 0;

	data_len = 3;
	crc = (dev->crc_sel == AD77681_NO_CRC) ? 0 : 1; // 1 byte for crc
	status_bit = dev->status_bit; // one byte for status

	rx_buf_len = data_len + crc + status_bit;

	return rx_buf_len;
}

/**
 * Read conversion result from device.
 * @param dev - The device structure.
 * @param adc_data - The conversion result data
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_spi_read_adc_data(struct ad77681_dev *dev,
				  uint8_t *adc_data)
{
	uint8_t crc_calc_buf[4], buf[4], crc;
	uint8_t rx_tx_buf_len = ad77681_get_rx_buf_len(dev) + 1;
	int32_t ret = 0;

	buf[0] = AD77681_REG_READ(AD77681_REG_ADC_DATA);
	buf[1] = 0x00;
	buf[2] = 0x00;
	buf[3] = 0x00;


	ad77681_spi_write_mask(dev,
			       AD77681_REG_CONVERSION,
			       AD77681_CONVERSION_MODE_MSK,
			       AD77681_CONVERSION_MODE(AD77681_CONV_SINGLE));
	//spi_eng_set_transfer_length(dev->spi_desc,32);
	ret |= spi_write_and_read(dev->spi_desc, buf, rx_tx_buf_len);
	ad77681_spi_write_mask(dev,
			       AD77681_REG_CONVERSION,
			       AD77681_CONVERSION_MODE_MSK,
			       AD77681_CONVERSION_MODE(AD77681_CONV_CONTINUOUS));
	if (ret < 0)
		return ret;

	if (dev->crc_sel == AD77681_CRC) {
		memcpy(crc_calc_buf, buf, ARRAY_SIZE(buf));
		crc = ad77681_compute_crc8(crc_calc_buf, 4);
		if (crc != buf[3]) {
			printf("%s: CRC Error.\n", __func__);
			ret = -1;
		}
	}

	/* Fill the adc_data buffer */
	memcpy(adc_data, buf, ARRAY_SIZE(buf));

	return 0;
}

/**
 * Set the power consumption mode of the ADC core.
 * @param dev - The device structure.
 * @param mode - The power mode.
 * 	Accepted values: AD77681_ECO
 *			 AD77681_MEDIAN
 *			 AD77681_FAST
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_set_power_mode(struct ad77681_dev *dev,
			       enum ad77681_power_mode mode)
{
	int32_t ret = 0;

	ret |= ad77681_spi_write_mask(dev,
				      AD77681_REG_POWER_CLOCK,
				      AD77681_POWER_CLK_PWRMODE_MSK,
				      AD77681_POWER_CLK_PWRMODE(mode));

	return ret;
}

/**
 * Set the MCLK divider.
 * @param dev - The device structure.
 * @param clk_div - The MCLK divider.
 * 	Accepted values: AD77681_MCLK_DIV_16
 *			 AD77681_MCLK_DIV_8
 *			 AD77681_MCLK_DIV_4
 *			 AD77681_MCLK_DIV_2
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_set_mclk_div(struct ad77681_dev *dev,
			     enum ad77681_mclk_div clk_div)
{
	ad77681_spi_write_mask(dev,
			       AD77681_REG_POWER_CLOCK,
			       AD77681_POWER_CLK_MCLK_DIV_MSK,
			       AD77681_POWER_CLK_MCLK_DIV(clk_div));

	dev->mclk_div = clk_div;

	return 0;
}

/**
 * Conversion mode and source select
 * @param dev - The device structure.
 * @param conv_mode - Sets the conversion mode of the ADC
 * 	Accepted values: AD77681_CONV_CONTINUOUS
 *			 AD77681_CONV_ONE_SHOT
 *			 AD77681_CONV_SINGLE
 *			 AD77681_CONV_PERIODIC
 * @param diag_mux_sel - Selects which signal to route through diagnostic mux
 * 	Accepted values: AD77681_TEMP_SENSOR
 *			 AD77681_AIN_SHORT
 *			 AD77681_POSITIVE_FS
 *			 AD77681_NEGATIVE_FS
 * @param conv_diag_sel - Select the input for conversion as AIN or diagnostic mux
 * 	Accepted values: true
 *			 false
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_set_conv_mode(struct ad77681_dev *dev,
			      enum ad77681_conv_mode conv_mode,
			      enum ad77681_conv_diag_mux diag_mux_sel,
			      bool conv_diag_sel)
{
	ad77681_spi_write_mask(dev,
			       AD77681_REG_CONVERSION,
			       AD77681_CONVERSION_MODE_MSK,
			       AD77681_CONVERSION_MODE(conv_mode));

	ad77681_spi_write_mask(dev,
			       AD77681_REG_CONVERSION,
			       AD77681_CONVERSION_DIAG_MUX_MSK,
			       AD77681_CONVERSION_DIAG_MUX_SEL(diag_mux_sel));

	ad77681_spi_write_mask(dev,
			       AD77681_REG_CONVERSION,
			       AD77681_CONVERSION_DIAG_SEL_MSK,
			       AD77681_CONVERSION_DIAG_SEL(conv_diag_sel));

	dev->conv_mode = conv_mode;
	dev->diag_mux_sel = diag_mux_sel;
	dev->conv_diag_sel = conv_diag_sel;

	return 0;
}

/**
 * Set the Conversion Result Output Length.
 * @param dev - The device structure.
 * @param conv_len - The MCLK divider.
 * 	Accepted values: AD77681_CONV_24BIT
 *			 AD77681_CONV_16BIT
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_set_convlen(struct ad77681_dev *dev,
			    enum ad77681_conv_len conv_len)
{
	int32_t ret = 0;

	ret |= 	ad77681_spi_write_mask(dev,
				       AD77681_REG_INTERFACE_FORMAT,
				       AD77681_INTERFACE_CONVLEN_MSK,
				       AD77681_INTERFACE_CONVLEN(conv_len));


	dev->conv_len = conv_len;

	return ret;
}

/**
 * Activates CRC on all SPI transactions and
 * Selects CRC method as XOR or 8-bit polynomial
 * @param dev - The device structure.
 * @param crc_sel - The CRC type.
 * 	Accepted values: AD77681_CRC
 *			 AD77681_XOR
 *			 AD77681_NO_CRC
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_set_crc_sel(struct ad77681_dev *dev,
			    enum ad77681_crc_sel crc_sel)
{
	int32_t ret = 0;

	if (crc_sel == AD77681_NO_CRC) {
		ret |= 	ad77681_spi_write_mask(dev,
					       AD77681_REG_INTERFACE_FORMAT,
					       AD77681_INTERFACE_CRC_EN_MSK,
					       AD77681_INTERFACE_CRC_EN(0));
	} else {
		ret |= 	ad77681_spi_write_mask(dev,
					       AD77681_REG_INTERFACE_FORMAT,
					       AD77681_INTERFACE_CRC_EN_MSK,
					       AD77681_INTERFACE_CRC_EN(1));

		ret |= 	ad77681_spi_write_mask(dev,
					       AD77681_REG_INTERFACE_FORMAT,
					       AD77681_INTERFACE_CRC_TYPE_MSK,
					       AD77681_INTERFACE_CRC_TYPE(crc_sel));
	}

	dev->crc_sel = crc_sel;

	return ret;
}

/**
 * Enables Status bits output
 * @param dev - The device structure.
 * @param status_bit - enable or disable status bit
 * 	Accepted values: true
 *			 false
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_set_status_bit(struct ad77681_dev *dev,
			       bool status_bit)
{
	int32_t ret = 0;

	// Set status bit
	ret |= 	ad77681_spi_write_mask(dev,
				       AD77681_REG_INTERFACE_FORMAT,
				       AD77681_INTERFACE_STATUS_EN_MSK,
				       AD77681_INTERFACE_STATUS_EN(status_bit));

	dev->status_bit = status_bit;

	return ret;
}

/**
 * Device reset over SPI.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_soft_reset(struct ad77681_dev *dev)
{
	int32_t ret = 0;

	// Two writes are required to initialize the reset
	ret |= 	ad77681_spi_write_mask(dev,
				       AD77681_REG_SYNC_RESET,
				       AD77681_SYNC_RST_SPI_RESET_MSK,
				       AD77681_SYNC_RST_SPI_RESET(0x3));

	ret |= 	ad77681_spi_write_mask(dev,
				       AD77681_REG_SYNC_RESET,
				       AD77681_SYNC_RST_SPI_RESET_MSK,
				       AD77681_SYNC_RST_SPI_RESET(0x2));

	return ret;
}

/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad77681_setup(struct ad77681_dev **device,
		      struct ad77681_init_param init_param)
{
	struct ad77681_dev *dev;
	int32_t ret = 0;

	dev = (struct ad77681_dev *)malloc(sizeof(*dev));
	if (!dev) {
		return -1;
	}

	dev->power_mode = init_param.power_mode;
	dev->mclk_div = init_param.mclk_div;
	dev->conv_diag_sel = init_param.conv_diag_sel;
	dev->conv_mode = init_param.conv_mode;
	dev->diag_mux_sel = init_param.diag_mux_sel;
	dev->conv_len = init_param.conv_len;
	dev->crc_sel = init_param.crc_sel;
	dev->status_bit = init_param.status_bit;

	spi_init(&dev->spi_desc, init_param.spi_init_param);
	ret |= ad77681_soft_reset(dev);
	ret |= ad77681_set_power_mode(dev, dev->power_mode);
	ret |= ad77681_set_mclk_div(dev, dev->mclk_div);
	ret |= ad77681_set_conv_mode(dev,
				     dev->conv_mode,
				     dev->diag_mux_sel,
				     dev->conv_diag_sel);
	ret |= ad77681_set_convlen(dev, dev->conv_len);
	ret |= ad77681_set_status_bit(dev, dev->status_bit);
	ret |= ad77681_set_crc_sel(dev, dev->crc_sel);

	*device = dev;

	if (!ret)
		printf("ad77681 successfully initialized\n");

	return ret;
}
