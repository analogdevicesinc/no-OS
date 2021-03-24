/***************************************************************************//**
 *   @file   ada4250.c
 *   @brief  Implementation of ADA4250 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
#include <malloc.h>
#include "ada4250.h"
#include "error.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/
/**
 * @brief Writes data to ada4250 over SPI.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param data - Data value to write.
 * @return Returns SUCCESS in case of success or negative error code otherwise.
 */

int32_t ada4250_write(struct ada4250_dev *dev, uint8_t reg_addr,
		      uint8_t data)
{
	uint8_t buff[ADA4250_BUFF_SIZE_BYTES];

	buff[0] = ADA4250_SPI_WRITE_CMD | reg_addr;
	buff[1] = data;

	return spi_write_and_read(dev->spi_desc, buff, ADA4250_BUFF_SIZE_BYTES);
}

/**
 * @brief Reads data from ada4250 over SPI.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param data - Data read from the device.
 * @return Returns SUCCESS in case of success or negative error code otherwise.
 */

int32_t ada4250_read(struct ada4250_dev *dev, uint8_t reg_addr,
		     uint8_t *data)
{
	int32_t ret;
	uint8_t buff[ADA4250_BUFF_SIZE_BYTES];

	buff[0] = ADA4250_SPI_READ_CMD | reg_addr;
	buff[1] = ADA4250_SPI_DUMMY_DATA;

	ret = spi_write_and_read(dev->spi_desc, buff, ADA4250_BUFF_SIZE_BYTES);
	if(ret != SUCCESS)
		return ret;

	*data = buff[1];

	return ret;
}

/**
 * @brief Update ADA4250 register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - Mask for specific register bits to be updated.
 * @param data - Data read from the device.
 * @return Returns SUCCESS in case of success or negative error code otherwise.
 */
int32_t ada4250_update(struct ada4250_dev *dev, uint8_t reg_addr,
		       uint8_t mask, uint8_t data)
{
	uint8_t read_val;
	int32_t ret;

	ret = ada4250_read(dev, reg_addr, &read_val);
	if (ret != SUCCESS)
		return ret;

	read_val &= ~mask;
	read_val |= data;

	ret = ada4250_write(dev, reg_addr, read_val);

	return ret;
}

/**
 * @brief Software reset.
 * @param dev - The device structure.
 * @return Returns SUCCESS in case of success or negative error code.
 */
int32_t ada4250_soft_reset(struct ada4250_dev *dev)
{
	int32_t ret;
	uint16_t timeout = 0xFFFF;
	uint8_t data;

	ret = ada4250_update(dev, ADA4250_REG_RESET, ADA4250_RESET_MSK,
			     ADA4250_RESET(ADA4250_RESET_ENABLE));

	while(timeout--) {
		ret = ada4250_read(dev, ADA4250_REG_RESET, &data);
		if (ret != SUCCESS)
			return ret;

		if(!(data & ADA4250_RESET(ADA4250_RESET_ENABLE)))
			return SUCCESS;
	}

	return FAILURE;
}

/**
 * @brief Enable/Disable Reference Buffer.
 * @param dev - The device structure.
 * @param refbuf - REFBUF enable/disable.
 * @return Returns SUCCESS in case of success or negative error code.
 */
int32_t ada4250_en_refbuf(struct ada4250_dev *dev, bool refbuf)
{
	return ada4250_update(dev, ADA4250_REG_REFBUF_EN, ADA4250_REFBUF_MSK,
			      ADA4250_REFBUF(refbuf));
}

/**
 * @brief Set current bias for ADA4250.
 * @param dev - The device structure.
 * @param bias - Current bias option.
 * @return Returns SUCCESS in case of success or negative error code.
 */
int32_t ada4250_set_bias(struct ada4250_dev *dev, enum ada4250_bias bias)
{
	return ada4250_update(dev, ADA4250_REG_SNSR_CAL_CNFG, ADA4250_BIAS_SET_MSK,
			      ADA4250_BIAS_SET(bias));
}

/**
 * @brief Set offset trim range for ADA4250.
 * @param dev - The device structure.
 * @param range - Offset range option.
 * @return Returns SUCCESS in case of success or negative error code.
 */
int32_t ada4250_set_range(struct ada4250_dev *dev,
			  enum ada4250_offset_range range)
{
	return ada4250_update(dev, ADA4250_REG_SNSR_CAL_CNFG, ADA4250_RANGE_SET_MSK,
			      ADA4250_RANGE_SET(ADA4250_RANGE(range)));
}

/**
 * @brief Set gain for ADA4250.
 * @param dev - The device structure.
 * @param gain - Gain Value.
 * @return Returns SUCCESS in case of success or negative error code.
 */
int32_t ada4250_set_gain(struct ada4250_dev *dev, uint8_t gain)
{
	return ada4250_update(dev, ADA4250_REG_GAIN_MUX, ADA4250_GAIN_MUX_MSK,
			      ADA4250_GAIN_MUX(ADA4250_GAIN(gain)));
}

/**
 * @brief Set offset value for ADA4250.
 * @param dev - The device structure.
 * @param offset - Offset Value.
 * @return Returns SUCCESS in case of success or negative error code.
 */
int32_t ada4250_set_offset(struct ada4250_dev *dev, int8_t offset)
{
	if (offset < 0)
		return ada4250_write(dev, ADA4250_REG_SNSR_CAL_VAL, (1 << 8 | abs(offset)));
	else
		return ada4250_write(dev, ADA4250_REG_SNSR_CAL_VAL, offset);
}

/**
 * @brief Initialize the ADA4250 device.
 * @param device - The device structure.
 * @param init_param - The structure containing the device initial parameters.
 * @return Returns SUCCESS in case of success or negative error code.
 */
int32_t ada4250_init(struct ada4250_dev **device,
		     struct ada4250_init_param *init_param)
{
	int32_t ret;
	uint8_t data;
	uint16_t chip_id;
	struct ada4250_dev *dev;

	dev = (struct ada4250_dev *)calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->refbuf_en = init_param->refbuf_en;
	dev->gain = init_param->gain;
	dev->bias = init_param->bias;
	dev->offset_range = init_param->offset_range;
	dev->offset_val = init_param->offset_val;

	/* SPI */
	ret = spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret != SUCCESS)
		goto error_dev;

	ret = ada4250_read(dev, ADA4250_REG_CHIP_ID2, &data);
	if (ret != SUCCESS)
		goto error_spi;

	chip_id = data;

	ret = ada4250_read(dev, ADA4250_REG_CHIP_ID1, &data);
	if (ret != SUCCESS)
		goto error_spi;

	chip_id = (chip_id << 8 | data);

	if (chip_id != ADA4250_CHIP_ID)
		goto error_spi;

	ret = ada4250_soft_reset(dev);
	if(ret != SUCCESS)
		return FAILURE;

	ret = ada4250_set_gain(dev, dev->gain);
	if(ret != SUCCESS)
		return FAILURE;

	ret = ada4250_en_refbuf(dev, dev->refbuf_en);
	if(ret != SUCCESS)
		return FAILURE;

	ret = ada4250_set_bias(dev, dev->bias);
	if(ret != SUCCESS)
		return FAILURE;

	ret = ada4250_set_range(dev, dev->offset_range);
	if(ret != SUCCESS)
		return FAILURE;

	ret = ada4250_set_offset(dev, dev->offset_val);
	if(ret != SUCCESS)
		return FAILURE;

	*device = dev;

	return ret;

error_spi:
	spi_remove(dev->spi_desc);
error_dev:
	free(dev);

	return ret;
}
/**
 * @brief Free resoulces allocated for ADA4250
 * @param dev - The device structure.
 * @return Returns SUCCESS in case of success or negative error code.
 */
int32_t ada4250_remove(struct ada4250_dev *dev)
{
	int32_t ret;

	ret = spi_remove(dev->spi_desc);
	if (ret != SUCCESS)
		return ret;

	free(dev);

	return ret;
}
