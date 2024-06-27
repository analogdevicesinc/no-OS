/***************************************************************************//**
*   @file   ade9113.c
*   @brief  Implementation of ADE9113 Driver.
*   @author George Mois (george.mois@analog.com)
********************************************************************************
* Copyright 2023(c) Analog Devices, Inc.
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
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include "ade9113.h"
#include "no_os_delay.h"
#include "no_os_units.h"
#include "no_os_alloc.h"
#include "no_os_crc8.h"
#include "no_os_crc16.h"
#include "no_os_print_log.h"

NO_OS_DECLARE_CRC8_TABLE(ade9113_crc8);
NO_OS_DECLARE_CRC16_TABLE(ade9113_crc16);

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Read device register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data read from the register.
 * @param op_mode - Long/short write operation.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_read(struct ade9113_dev *dev, uint8_t reg_addr, uint8_t *reg_data,
		 enum ade9113_operation_e op_mode)
{
	int ret;
	/* CRC computed for sent commands */
	uint8_t crc8;
	/* CRC computed for read response messages */
	uint16_t crc16;
	/* CRC received with read response messages */
	uint16_t recv_crc;
	/* no of read bytes, depends on long/short operation and on CRC enable state */
	uint8_t no_of_read_bytes = 4;
	/* offset of data read in the read buffer */
	uint8_t data_byte_offset = 3;
	/* position of byte in guffer to start read to */
	uint8_t position = 12;
	/* buffer for data read (large enough for a long read) */
	uint8_t buff[16] = { 0 };
	/* index */
	uint8_t i;

	/* set read bit */
	buff[12] = ADE9113_SPI_READ | 0x00;
	/* set long operation bit */
	if (op_mode == ADE9113_L_OP)
		buff[12] = ADE9113_OP_MODE_LONG | buff[12];
	/* set address to read from */
	buff[13] = reg_addr;

	/* compute CRC and add it to command */
	crc8 = no_os_crc8(ade9113_crc8, &buff[12], 3, 0);
	crc8 ^= 0x55;
	buff[15] = crc8;
	no_of_read_bytes = 6;
	position = 10;

	/* set message structure for long operations */
	if (op_mode == ADE9113_L_OP) {
		no_of_read_bytes = 16;
		position = 0;
		data_byte_offset = 13;
	}

	/* send read command */
	ret = no_os_spi_write_and_read(dev->spi_desc, &buff[position],
				       no_of_read_bytes);
	if (ret)
		return ret;

	/* reset command buffer */
	for (i = 0; i < 16; i++)
		buff[i] = 0;

	/* set long operation bit */
	if (op_mode == ADE9113_L_OP)
		buff[12] = ADE9113_OP_MODE_LONG | buff[12];

	/* compute CRC and add it to command if CRC enabled */
	crc8 = no_os_crc8(ade9113_crc8, &buff[12], 3, 0);
	crc8 ^= 0x55;
	buff[15] = crc8;

	/* send read command */
	ret = no_os_spi_write_and_read(dev->spi_desc, &buff[position],
				       no_of_read_bytes);
	if (ret)
		return ret;

	/* check received CRC, if enabled */
	if (dev->crc_en) {
		crc16 = no_os_crc16(ade9113_crc16, &buff[position], no_of_read_bytes - 2,
				    ADE9113_CRC16_INIT_VAL);

		recv_crc = no_os_get_unaligned_le16(&buff[position + no_of_read_bytes - 2]);

		if (recv_crc != crc16) {
			/* if we read 0s on SPI then there is no communication */
			if (!recv_crc)
				return -ENODEV;
			/* the application should handle this result */
			return -EPROTO;
		}
	}

	if (op_mode == ADE9113_L_OP) {
		dev->i_wav = no_os_sign_extend32(no_os_get_unaligned_le24(&buff[1]), 23);
		dev->v1_wav = no_os_sign_extend32(no_os_get_unaligned_le24(&buff[5]), 23);
		dev->v2_wav = no_os_sign_extend32(no_os_get_unaligned_le24(&buff[9]), 23);
	}

	/* set read data */
	*reg_data = buff[position + data_byte_offset];

	return 0;
}

/**
 * @brief Write device register.
 * @param dev- The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data to be written.
 * @param op_mode - Long/short write operation.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_write(struct ade9113_dev *dev, uint8_t reg_addr, uint8_t reg_data,
		  enum ade9113_operation_e op_mode)
{
	uint8_t crc;
	uint8_t buff[4] = {0};

	if (op_mode == ADE9113_L_OP)
		buff[0] |= ADE9113_OP_MODE_LONG;
	buff[1] = reg_addr;
	buff[2] = reg_data;

	crc = no_os_crc8(ade9113_crc8, buff, 3, 0);
	crc ^= 0x55;
	buff[3] = crc;

	return no_os_spi_write_and_read(dev->spi_desc, buff, 4);
}

/**
 * @brief Update specific register bits.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - Specific bits mask.
 * @param reg_data - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ade9113_update_bits(struct ade9113_dev *dev, uint8_t reg_addr,
			       uint8_t mask, uint8_t reg_data)
{
	int ret;
	uint8_t data;

	ret = ade9113_read(dev, reg_addr, &data, ADE9113_S_OP);
	if (ret)
		return ret;

	data &= ~mask;
	data |= reg_data & mask;

	return ade9113_write(dev, reg_addr, data, ADE9113_S_OP);
}

/**
 * @brief GPIO interrupt handler for data ready.
 * @param dev - The device structure.
 */
static void ade9113_irq_handler(void *dev)
{
	struct ade9113_dev *desc = dev;
	uint8_t reg_val;
	int ret;

	/* Disable interrupt while reading data. */
	ret = no_os_irq_disable(desc->irq_ctrl, desc->gpio_rdy->number);
	if (ret)
		return;

	/* READ the data and place it in device structure */
	ret = ade9113_read(dev, ADE9113_REG_CONFIG0, &reg_val, ADE9113_L_OP);
	if (ret)
		return;

	/* Reenable interrupt */
	ret = no_os_irq_enable(desc->irq_ctrl,
			       desc->gpio_rdy->number);

	return;
}

/**
 * @brief Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_init(struct ade9113_dev **device,
		 struct ade9113_init_param init_param)
{
	struct ade9113_dev *dev;
	uint8_t reg_val;
	int ret;
	int timeout = 0;

	if (!init_param.irq_ctrl)
		return -EINVAL;

	dev = (struct ade9113_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	struct no_os_callback_desc irq_cb = {
		.callback = ade9113_irq_handler,
		.ctx = dev,
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ
	};

	if (init_param.drdy_callback)
		irq_cb.callback = init_param.drdy_callback;

	/* SPI Initialization */
	ret = no_os_spi_init(&dev->spi_desc, init_param.spi_init);
	if (ret)
		goto error_dev;

	ret = no_os_gpio_get_optional(&dev->gpio_rdy, init_param.gpio_rdy);
	if (ret)
		goto error_spi;

	if (dev->gpio_rdy) {
		ret = no_os_gpio_direction_input(dev->gpio_rdy);
		if (ret)
			goto error_gpio;
	}

	ret = no_os_irq_register_callback(init_param.irq_ctrl,
					  dev->gpio_rdy->number, &irq_cb);
	if (ret)
		goto error_gpio;

	dev->irq_cb = irq_cb;

	ret = no_os_irq_trigger_level_set(init_param.irq_ctrl,
					  dev->gpio_rdy->number, NO_OS_IRQ_EDGE_FALLING);
	if (ret)
		goto error_irq;

	ret = no_os_irq_set_priority(init_param.irq_ctrl, dev->gpio_rdy->number, 3);
	if (ret)
		goto error_irq;

	ret = no_os_irq_disable(init_param.irq_ctrl,
				dev->gpio_rdy->number);
	if (ret)
		goto error_irq;

	dev->irq_ctrl = init_param.irq_ctrl;

	ret = no_os_gpio_get_optional(&dev->gpio_reset, init_param.gpio_reset);
	if (ret)
		goto error_spi;

	if (dev->gpio_reset) {
		ret = no_os_gpio_direction_output(dev->gpio_reset, NO_OS_GPIO_HIGH);
		if (ret)
			goto error_gpio;
	}

	/* Create the CRC-8 lookup table for polynomial ADE9113_CRC8_POLY */
	no_os_crc8_populate_msb(ade9113_crc8, ADE9113_CRC8_POLY);

	/* Create the CRC-16 lookup table for polynomial ADE9113_CRC8_POLY */
	no_os_crc16_populate_msb(ade9113_crc16, ADE9113_CRC16_POLY);

	/* CRC enabled by default */
	dev->crc_en = 1;

	/* Reset device */
	ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_LOW);
	if (ret)
		goto error_gpio;

	no_os_mdelay(10);

	ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_HIGH);
	if (ret)
		goto error_gpio;

	no_os_mdelay(50);

	do {
		ret = ade9113_get_com_up(dev, &reg_val);
		if (ret)
			goto error_gpio;

		no_os_mdelay(5);
		timeout++;
		if (timeout == 20) {
			ret = -ENOTCONN;
			goto error_gpio;
		}
	} while (!reg_val);

	/* Read version product */
	ret = ade9113_get_version_product(dev, &reg_val);
	if (ret)
		goto error_gpio;

	dev->ver_product = reg_val;

	*device = dev;

	return 0;

error_irq:
	no_os_irq_unregister_callback(init_param.irq_ctrl, dev->gpio_rdy->number,
				      &irq_cb);
error_gpio:
	no_os_gpio_remove(dev->gpio_rdy);
error_spi:
	no_os_spi_remove(dev->spi_desc);
error_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Remove the device and release resources.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_remove(struct ade9113_dev *dev)
{
	int ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_rdy);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_reset);
	if (ret)
		return ret;

	ret = no_os_irq_unregister_callback(dev->irq_ctrl, dev->gpio_rdy->number,
					    &dev->irq_cb);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}

/**
 * @brief Reset the device using SW reset.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_sw_reset(struct ade9113_dev *dev)
{
	int ret;
	uint8_t ver_product;

	ret = ade9113_write(dev, ADE9113_REG_SWRST, ADE9113_SWRST_CMD, ADE9113_S_OP);
	if (ret)
		return ret;

	dev->crc_en = 1;

	/* Wait for device to initialize */ // ToDo - check required timing
	no_os_mdelay(100);

	/* Read version product */
	ret = ade9113_get_version_product(dev, &ver_product);
	if (ret)
		return ret;

	dev->ver_product = ver_product;

	return 0;
}

/**
 * @brief Reset the device using SW reset.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_hw_reset(struct ade9113_dev *dev)
{
	int ret;
	uint8_t ver_product;

	ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	no_os_mdelay(10);

	ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	dev->crc_en = 1;

	/* Wait for device to initialize */ // ToDo - check required timing
	no_os_mdelay(100);

	/* Read version product */
	ret = ade9113_get_version_product(dev, &ver_product);
	if (ret)
		return ret;

	dev->ver_product = ver_product;

	return 0;
}

/**
 * @brief Convert a 24-bit raw sample to millivolts.
 * @param dev - The device structure.
 * @param ch - Device channel.
 * @param mv_val - Value in millivolts.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_convert_to_millivolts(struct ade9113_dev *dev,
				  enum ade9113_wav_e ch, int32_t *mv_val)
{
	int64_t value = 0;

	if (!dev)
		return	-ENODEV;

	if (ch > ADE9113_V2_WAV)
		return	-EINVAL;

	switch (ch) {
	case ADE9113_I_WAV:
		/* times 2, two's complement data */
		value = (int64_t)dev->i_wav;
		break;
	case ADE9113_V1_WAV:
		value = (int64_t)dev->v1_wav;
		break;
	default:
		value = (int64_t)dev->v2_wav;
		break;
	}

	*mv_val = (int32_t)value;

	return 0;
}

/**
 * @brief Get STREAM_DBG mode.
 * @param dev - The device structure.
 * @param stream_dbg - Read debug mode setting read.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_stream_dbg_mode(struct ade9113_dev *dev,
				enum ade9113_stream_debug_e *stream_dbg)
{
	int ret;
	uint8_t reg_val;

	if (!stream_dbg)
		return -EINVAL;

	ret = ade9113_read(dev, ADE9113_REG_CONFIG0, &reg_val, ADE9113_S_OP);
	if (ret)
		return ret;

	switch (reg_val & ADE9113_STREAM_DBG_MSK) {
	case 0:
		*stream_dbg = ADE9113_STREAM_NORMAL_MODE;
		break;
	case 1:
		*stream_dbg = ADE9113_STREAM_STATIC_MODE;
		break;
	case 2:
		*stream_dbg = ADE9113_STREAM_INCREMENTS_MODE;
		break;
	default:
		*stream_dbg = ADE9113_STREAM_FUNCTIONAL_MODE;
		break;
	}

	return 0;
}

/**
 * @brief Set STREAM_DBG mode.
 * @param dev - The device structure.
 * @param stream_dbg - Stream debug mode setting.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_set_stream_dbg_mode(struct ade9113_dev *dev,
				enum ade9113_stream_debug_e stream_dbg)
{
	if (stream_dbg > ADE9113_STREAM_FUNCTIONAL_MODE)
		return -EINVAL;

	return ade9113_update_bits(dev, ADE9113_REG_CONFIG0, ADE9113_STREAM_DBG_MSK,
				   no_os_field_prep(ADE9113_STREAM_DBG_MSK, stream_dbg));
}

/**
 * @brief Get CRC enable on SPI write setting.
 * @param dev - The device structure.
 * @param crc_en_state - Read CRC setting.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_crc_en_state(struct ade9113_dev *dev,
			     uint8_t *crc_en_state)
{
	int ret;
	uint8_t reg_val;

	if (!crc_en_state)
		return -EINVAL;

	*crc_en_state = dev->crc_en;

	ret = ade9113_read(dev, ADE9113_REG_CONFIG0, &reg_val, ADE9113_S_OP);
	if (ret)
		return ret;

	if (reg_val & ADE9113_CRC_EN_SPI_WRITE_MSK)
		*crc_en_state = 1;

	dev->crc_en = *crc_en_state;

	return 0;
}

/**
 * @brief Set CRC enable on SPI write setting.
 * @param dev - The device structure.
 * @param crc_en_state - Read CRC setting.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_set_crc_en_state(struct ade9113_dev *dev,
			     uint8_t crc_en_state)
{
	int ret;

	if (crc_en_state > 1)
		return -EINVAL;

	ret = ade9113_update_bits(dev, ADE9113_REG_CONFIG0,
				  ADE9113_CRC_EN_SPI_WRITE_MSK,
				  no_os_field_prep(ADE9113_CRC_EN_SPI_WRITE_MSK, crc_en_state));

	if (ret)
		return ret;

	dev->crc_en = crc_en_state;

	return 0;
}

/**
 * @brief Lock device.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_wr_lock(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_write(dev, ADE9113_REG_WR_LOCK, ADE9113_LOCK_KEY, ADE9113_S_OP);
}

/**
 * @brief Unlock device.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_wr_unlock(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_write(dev, ADE9113_REG_WR_LOCK, ADE9113_UNLOCK_KEY,
			     ADE9113_S_OP);
}

/**
 * @brief Write value in the scratchpad register.
 * @param dev - The device structure.
 * @param val - The value to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_write_scratchpad(struct ade9113_dev *dev,
			     uint8_t val)
{
	if (!dev)
		return -ENODEV;

	return ade9113_write(dev, ADE9113_REG_SCRATCH, val, ADE9113_S_OP);
}

/**
 * @brief Get the value stired in the scratchpad register.
 * @param dev - The device structure.
 * @param val - The read value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_read_scratchpad(struct ade9113_dev *dev,
			    uint8_t *val)
{
	int ret;
	uint8_t reg_val;

	if (!val)
		return -EINVAL;

	ret = ade9113_read(dev, ADE9113_REG_SCRATCH, &reg_val, ADE9113_S_OP);
	if (ret)
		return ret;

	*val = reg_val;

	return 0;
}

/**
 * @brief Set normal mode of operation.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_set_normal_mode(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_set_stream_dbg_mode(dev, ADE9113_STREAM_NORMAL_MODE);
}

/**
 * @brief Set static mode of operation.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_set_static_mode(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_set_stream_dbg_mode(dev, ADE9113_STREAM_STATIC_MODE);
}

/**
 * @brief Set data increments mode of operation.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_set_data_increments_mode(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_set_stream_dbg_mode(dev, ADE9113_STREAM_INCREMENTS_MODE);
}

/**
 * @brief Get ECC or PHY Error Count on ISO to NONISO Communications.
 * @param dev - The device structure.
 * @param err_count - Read erro count.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_err_count(struct ade9113_dev *dev,
			  uint8_t *err_count)
{
	if (!err_count)
		return -EINVAL;

	return ade9113_read(dev, ADE9113_REG_COM_FLT_COUNT, err_count, ADE9113_S_OP);
}

/**
 * @brief Invert V2 channel inputs.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_invert_v2_inputs(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_update_bits(dev, ADE9113_REG_CONFIG_FILT,
				   ADE9113_V2_ADC_INVERT_MSK, no_os_field_prep(ADE9113_V2_ADC_INVERT_MSK, ENABLE));
}

/**
 * @brief Invert V1 channel inputs.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_invert_v1_inputs(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_update_bits(dev, ADE9113_REG_CONFIG_FILT,
				   ADE9113_V1_ADC_INVERT_MSK, no_os_field_prep(ADE9113_V1_ADC_INVERT_MSK, ENABLE));
}

/**
 * @brief Invert I channel inputs.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_invert_i_inputs(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_update_bits(dev, ADE9113_REG_CONFIG_FILT,
				   ADE9113_I_ADC_INVERT_MSK, no_os_field_prep(ADE9113_I_ADC_INVERT_MSK, ENABLE));
}

/**
 * @brief Disable invert V2 channel inputs.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_invert_v2_inputs_disable(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_update_bits(dev, ADE9113_REG_CONFIG_FILT,
				   ADE9113_V2_ADC_INVERT_MSK, no_os_field_prep(ADE9113_V2_ADC_INVERT_MSK,
						   DISABLE));
}

/**
 * @brief Disable invert V1 channel inputs.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_invert_v1_inputs_disable(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_update_bits(dev, ADE9113_REG_CONFIG_FILT,
				   ADE9113_V1_ADC_INVERT_MSK, no_os_field_prep(ADE9113_V1_ADC_INVERT_MSK,
						   DISABLE));
}

/**
 * @brief Disable invert I channel inputs.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_invert_i_inputs_disable(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_update_bits(dev, ADE9113_REG_CONFIG_FILT,
				   ADE9113_I_ADC_INVERT_MSK, no_os_field_prep(ADE9113_I_ADC_INVERT_MSK, DISABLE));
}

/**
 * @brief Set filter bandwidth to 2.7 kHz at 8ksps output data rate.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_set_lpf_bw_2_7(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_update_bits(dev, ADE9113_REG_CONFIG_FILT,
				   ADE9113_LPF_BW_MSK, no_os_field_prep(ADE9113_LPF_BW_MSK, DISABLE));
}

/**
 * @brief Set filter bandwidth to 3.3 kHz at 8ksps output data rate.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_set_lpf_bw_3_3(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_update_bits(dev, ADE9113_REG_CONFIG_FILT,
				   ADE9113_LPF_BW_MSK, no_os_field_prep(ADE9113_LPF_BW_MSK, ENABLE));
}

/**
 * @brief Set digital signal processing configuration.
 * @param dev - The device structure.
 * @param config - The ADE9113 DSP configuration.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_set_dsp_config(struct ade9113_dev *dev,
			   enum ade9113_datapath_config_e config)
{
	if (config > ADE9113_SINC3_LPF_EN_1_KHZ_SAMPLING)
		return -EINVAL;

	if (config > ADE9113_SINC3_LPF_EN_1_KHZ_SAMPLING)
		return -EINVAL;

	return ade9113_update_bits(dev, ADE9113_REG_CONFIG_FILT,
				   ADE9113_DATAPATH_CONFIG_MSK, no_os_field_prep(ADE9113_DATAPATH_CONFIG_MSK,
						   config));
}

/**
 * @brief Enable write access to DC_OFFSET_MODE register.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_enable_wa_dc_offset_mode(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_update_bits(dev, ADE9113_REG_CONFIG_ISO_ACC,
				   ADE9113_ISO_WR_ACC_EN_MSK, no_os_field_prep(ADE9113_ISO_WR_ACC_EN_MSK, ENABLE));
}

/**
 * @brief Disable write access to DC_OFFSET_MODE register.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_disable_wa_dc_offset_mode(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_update_bits(dev, ADE9113_REG_CONFIG_ISO_ACC,
				   ADE9113_ISO_WR_ACC_EN_MSK, no_os_field_prep(ADE9113_ISO_WR_ACC_EN_MSK,
						   DISABLE));
}

/**
 * @brief Get register map CRC.
 * @param dev - The device structure.
 * @param crc - CRC value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_crc(struct ade9113_dev *dev, uint16_t *crc)
{
	int ret;
	uint8_t reg_val[2];

	if (!crc)
		return -EINVAL;

	ret = ade9113_read(dev, ADE9113_REG_CRC_RESULT_HI, &reg_val[1], ADE9113_S_OP);
	if (ret)
		return ret;
	ret = ade9113_read(dev, ADE9113_REG_CRC_RESULT_LO, &reg_val[0], ADE9113_S_OP);
	if (ret)
		return ret;

	*crc = no_os_get_unaligned_le16(reg_val);

	return 0;
}

/**
 * @brief Refresh EFuse Memory.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_efuse_refresh(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_update_bits(dev, ADE9113_REG_EFUSE_REFRESH,
				   ADE9113_EFUSE_REFRESH_MSK, no_os_field_prep(ADE9113_EFUSE_REFRESH_MSK, ENABLE));
}

/**
 * @brief Select EMI frequency hopping.
 * @param dev - The device structure.
 * @param config - EMI frequency hopping selection.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_set_emi_config(struct ade9113_dev *dev,
			   enum ade9113_emi_config_e config)
{
	if (config > ADE9113_RANDOM_HOPPING_FREQUENCY)
		return -EINVAL;

	return ade9113_update_bits(dev, ADE9113_REG_EMI_CONFIG,
				   ADE9113_EMI_CONFIG_MSK, no_os_field_prep(ADE9113_EMI_CONFIG_MSK, config));
}

/**
 * @brief Get EMI HI mask.
 * @param dev - The device structure.
 * @param msk - EMI HI mask.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_emi_hi_mask(struct ade9113_dev *dev, uint8_t *msk)
{
	if (!msk)
		return -EINVAL;

	return ade9113_read(dev, ADE9113_REG_EMI_HI_MASK, msk, ADE9113_S_OP);
}

/**
 * @brief Get EMI LO mask.
 * @param dev - The device structure.
 * @param msk - EMI LO mask.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_emi_lo_mask(struct ade9113_dev *dev, uint8_t *msk)
{
	if (!msk)
		return -EINVAL;

	return ade9113_read(dev, ADE9113_REG_EMI_LO_MASK, msk, ADE9113_S_OP);
}

/**
 * @brief Set EMI HI mask.
 * @param dev - The device structure.
 * @param msk - EMI HI mask set.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_set_emi_hi_mask(struct ade9113_dev *dev, uint8_t msk)
{
	if (!dev)
		return -ENODEV;

	return ade9113_write(dev, ADE9113_REG_EMI_HI_MASK, msk, ADE9113_S_OP);
}

/**
 * @brief Set EMI LO mask.
 * @param dev - The device structure.
 * @param msk - EMI LO mask set.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_set_emi_lo_mask(struct ade9113_dev *dev, uint8_t msk)
{
	if (!dev)
		return -ENODEV;

	return ade9113_write(dev, ADE9113_REG_EMI_LO_MASK, msk, ADE9113_S_OP);
}

/**
 * @brief Get EMI HI limit.
 * @param dev - The device structure.
 * @param limit - Read EMI HI limit.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_emi_hi_limit(struct ade9113_dev *dev, uint8_t *limit)
{
	if (!limit)
		return -EINVAL;

	return ade9113_read(dev, ADE9113_REG_EMI_HI_LIMIT, limit, ADE9113_S_OP);
}

/**
 * @brief Get EMI MID limit.
 * @param dev - The device structure.
 * @param limit - Read EMI MID limit.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_emi_mid_limit(struct ade9113_dev *dev, uint8_t *limit)
{
	if (!limit)
		return -EINVAL;

	return ade9113_read(dev, ADE9113_REG_EMI_MID_LIMIT, limit, ADE9113_S_OP);
}

/**
 * @brief Get EMI LO limit.
 * @param dev - The device structure.
 * @param limit - Read EMI LO limit.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_emi_lo_limit(struct ade9113_dev *dev, uint8_t *limit)
{
	if (!limit)
		return -EINVAL;

	return ade9113_read(dev, ADE9113_REG_EMI_LO_LIMIT, limit, ADE9113_S_OP);
}

/**
 * @brief Enable/disable interrupt.
 * @param dev - The device structure.
 * @param reg_addr - MASK register address.
 * @param int_msk - Interrupt mask.
 * @param en - Enable/Disable.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_control_interrupt(struct ade9113_dev *dev, uint8_t reg_addr,
			      uint8_t int_msk, uint8_t en)
{
	if (!dev)
		return -ENODEV;

	return ade9113_update_bits(dev, reg_addr,
				   int_msk, no_os_field_prep(int_msk, en));
}

/**
 * @brief Enable STATUS1X interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_enable_status1x_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK0,
					 ADE9113_STATUS1X_MSK, ENABLE);
}

/**
 * @brief Disable STATUS1X interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_disable_status1x_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK0,
					 ADE9113_STATUS1X_MSK, DISABLE);
}

/**
 * @brief Enable STATUS2X interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_enable_status2x_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK0,
					 ADE9113_STATUS2X_MSK, ENABLE);
}

/**
 * @brief Disable STATUS2X interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_disable_status2x_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK0,
					 ADE9113_STATUS2X_MSK, DISABLE);
}

/**
 * @brief Enable COM_UP interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_enable_com_up_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK0,
					 ADE9113_COM_UP_MSK, ENABLE);
}

/**
 * @brief Disable COM_UP interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_disable_com_up_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK0,
					 ADE9113_COM_UP_MSK, DISABLE);
}

/**
 * @brief Enable CRC_CHG interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_enable_crc_chg_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK0,
					 ADE9113_CRC_CHG_MSK, ENABLE);
}

/**
 * @brief Disable CRC_CHG interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_disable_crc_chg_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK0,
					 ADE9113_CRC_CHG_MSK, DISABLE);
}

/**
 * @brief Enable SPI_CRC_ERR interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_enable_spi_crc_err_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK0,
					 ADE9113_SPI_CRC_ERR_MSK, ENABLE);
}

/**
 * @brief Disable SPI_CRC_ERR interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_disable_spi_crc_err_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK0,
					 ADE9113_SPI_CRC_ERR_MSK, DISABLE);
}

/**
 * @brief Enable COMFLT_ERR interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_enable_comflt_err_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK0,
					 ADE9113_COMFLT_ERR_MSK, ENABLE);
}

/**
 * @brief Disable COMFLT_ERR interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_disable_comflt_err_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK0,
					 ADE9113_COMFLT_ERR_MSK, DISABLE);
}

/**
 * @brief Enable V2_WAV_OVRNG interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_enable_v2_wav_ovrng_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK1,
					 ADE9113_V2_WAV_OVRNG_MSK, ENABLE);
}

/**
 * @brief Disable V2_WAV_OVRNG interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_disable_v2_wav_ovrng_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK1,
					 ADE9113_V2_WAV_OVRNG_MSK, DISABLE);
}

/**
 * @brief Enable V1_WAV_OVRNG interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_enable_v1_wav_ovrng_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK1,
					 ADE9113_V1_WAV_OVRNG_MSK, ENABLE);
}

/**
 * @brief Disable V1_WAV_OVRNG interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_disable_v1_wav_ovrng_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK1,
					 ADE9113_V1_WAV_OVRNG_MSK, DISABLE);
}

/**
 * @brief Enable I_WAV_OVRNG interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_enable_i_wav_ovrng_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK1,
					 ADE9113_I_WAV_OVRNG_MSK, ENABLE);
}

/**
 * @brief Disable I_WAV_OVRNG interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_disable_i_wav_ovrng_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK1,
					 ADE9113_I_WAV_OVRNG_MSK, DISABLE);
}

/**
 * @brief Enable ADC_SYNC_DONE interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_enable_adc_sync_done_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK1,
					 ADE9113_ADC_SYNC_DONE_MSK, ENABLE);
}

/**
 * @brief Disable ADC_SYNC_DONE interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_disable_adc_sync_done_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK1,
					 ADE9113_ADC_SYNC_DONE_MSK, DISABLE);
}

/**
 * @brief Enable ISO_CLK_STBL_ERR interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_enable_iso_clk_stbl_err_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK2,
					 ADE9113_ISO_CLK_STBL_ERR_MSK, ENABLE);
}

/**
 * @brief Disable ISO_CLK_STBL_ERR interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_disable_iso_clk_stbl_err_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK2,
					 ADE9113_ISO_CLK_STBL_ERR_MSK, DISABLE);
}

/**
 * @brief Enable ISO_PHY_CRC_ERR interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_enable_iso_phy_crc_err_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK2,
					 ADE9113_ISO_PHY_CRC_ERR_MSK, ENABLE);
}

/**
 * @brief Disable ISO_PHY_CRC_ERR interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_disable_iso_phy_crc_err_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK2,
					 ADE9113_ISO_PHY_CRC_ERR_MSK, DISABLE);
}

/**
 * @brief Enable ISO_EFUSE_MEM_ERR interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_enable_iso_efuse_mem_err_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK2,
					 ADE9113_ISO_EFUSE_MEM_ERR_MSK, ENABLE);
}

/**
 * @brief Disable ISO_EFUSE_MEM_ERR interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_disable_iso_efuse_mem_err_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK2,
					 ADE9113_ISO_EFUSE_MEM_ERR_MSK, DISABLE);
}

/**
 * @brief Enable ISO_DIG_MOD_V2_OVF interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_enable_iso_dig_mod_v2_ovf_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK2,
					 ADE9113_ISO_DIG_MOD_V2_OVF_MSK, ENABLE);
}

/**
 * @brief Disable ISO_DIG_MOD_V2_OVF interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_disable_iso_dig_mod_v2_ovf_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK2,
					 ADE9113_ISO_DIG_MOD_V2_OVF_MSK, DISABLE);
}

/**
 * @brief Enable ISO_DIG_MOD_V1_OVF interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_enable_iso_dig_mod_v1_ovf_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK2,
					 ADE9113_ISO_DIG_MOD_V1_OVF_MSK, ENABLE);
}

/**
 * @brief Disable ISO_DIG_MOD_V1_OVF interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_disable_iso_dig_mod_v1_ovf_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK2,
					 ADE9113_ISO_DIG_MOD_V1_OVF_MSK, DISABLE);
}

/**
 * @brief Enable ISO_DIG_MOD_I_OVF interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_enable_iso_dig_mod_i_ovf_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK2,
					 ADE9113_ISO_DIG_MOD_I_OVF_MSK, ENABLE);
}

/**
 * @brief Disable ISO_DIG_MOD_I_OVF interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_disable_iso_dig_mod_i_ovf_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK2,
					 ADE9113_ISO_DIG_MOD_I_OVF_MSK, DISABLE);
}

/**
 * @brief Enable ISO_TEST_MMR_ERR interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_enable_iso_test_mmr_err_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK2,
					 ADE9113_ISO_TEST_MMR_ERR_MSK, ENABLE);
}

/**
 * @brief Disable ISO_TEST_MMR_ERR interrupt.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_disable_iso_test_mmr_err_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_control_interrupt(dev, ADE9113_REG_MASK2,
					 ADE9113_ISO_TEST_MMR_ERR_MSK, DISABLE);
}

/**
 * @brief Select zero crossing edge.
 * @param dev - The device structure.
 * @param sel - Zero crossing edge selection.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_select_zero_crossing_edge(struct ade9113_dev *dev,
				      enum ade9113_zx_edge_sel_e sel)
{
	if (!dev)
		return -ENODEV;

	if (sel > ADE9113_ZX_DETECT_BOTH_SLOPES)
		return -EINVAL;

	return ade9113_update_bits(dev, ADE9113_REG_CONFIG_ZX,
				   ADE9113_ZX_EDGE_SEL_MSK, no_os_field_prep(ADE9113_ZX_EDGE_SEL_MSK, sel));
}

/**
 * @brief Select zero crossing channel.
 * @param dev - The device structure.
 * @param cfg - Zero crossing channel selection.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_select_zero_crossing_channel(struct ade9113_dev *dev,
		enum ade9113_zx_channel_cfg_e cfg)
{
	if (!dev)
		return -ENODEV;

	if (cfg > ADE9113_ZX_V2_SEL)
		return -EINVAL;

	return ade9113_update_bits(dev, ADE9113_REG_CONFIG_ZX,
				   ADE9113_ZX_CHANNEL_CONFIG_MSK, no_os_field_prep(ADE9113_ZX_CHANNEL_CONFIG_MSK,
						   cfg));
}

/**
 * @brief ADC prepare broadcast.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_adc_prepare_broadcast(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_update_bits(dev, ADE9113_REG_SYNC_SNAP,
				   ADE9113_PREP_BROADCAST_MSK, no_os_field_prep(ADE9113_PREP_BROADCAST_MSK,
						   ENABLE));
}

/**
 * @brief ADC align.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_adc_align(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_update_bits(dev, ADE9113_REG_SYNC_SNAP,
				   ADE9113_ALIGN_MSK, no_os_field_prep(ADE9113_ALIGN_MSK,
						   ENABLE));
}

/**
 * @brief ADC snapshot.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_adc_snapshot(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_update_bits(dev, ADE9113_REG_SYNC_SNAP,
				   ADE9113_SNAPSHOT_MSK, no_os_field_prep(ADE9113_SNAPSHOT_MSK,
						   ENABLE));
}

/**
 * @brief Get interrupt indicator from STATUS register.
 * @param dev - The device structure.
 * @param addr - Register address.
 * @param msk - Interrupt mask.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_int_status(struct ade9113_dev *dev, uint8_t addr, uint8_t msk,
			   uint8_t *status)
{
	uint8_t reg_val;
	int ret;

	if (!status)
		return -EINVAL;

	ret = ade9113_read(dev, addr, &reg_val, ADE9113_S_OP);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(msk), &reg_val);

	return 0;
}

/**
 * @brief Get STATUSx register value.
 * @param dev - The device structure.
 * @param addr - Register address.
 * @param status - 8-bit register value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_statusx_val(struct ade9113_dev *dev, uint8_t addr,
			    uint8_t *status)
{
	uint8_t reg_val;
	int ret;

	if (!status)
		return -EINVAL;

	ret = ade9113_read(dev, addr, &reg_val, ADE9113_S_OP);
	if (ret)
		return ret;

	*status = reg_val;

	return 0;
}

/**
 * @brief Get STATUS1 indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_status1x(struct ade9113_dev *dev, uint8_t *status)
{
	return ade9113_get_int_status(dev, ADE9113_REG_STATUS0, ADE9113_STATUS1X_MSK,
				      status);
}

/**
 * @brief Get STATUS2 indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_status2x(struct ade9113_dev *dev, uint8_t *status)
{
	return ade9113_get_int_status(dev, ADE9113_REG_STATUS0, ADE9113_STATUS2X_MSK,
				      status);
}

/**
 * @brief Get RESET_DONE indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_reset_done(struct ade9113_dev *dev, uint8_t *status)
{
	return ade9113_get_int_status(dev, ADE9113_REG_STATUS0, ADE9113_RESET_DONE_MSK,
				      status);
}

/**
 * @brief Get COM_UP indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_com_up(struct ade9113_dev *dev, uint8_t *status)
{
	return ade9113_get_int_status(dev, ADE9113_REG_STATUS0, ADE9113_COM_UP_MSK,
				      status);
}

/**
 * @brief Get CRC_CHG indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_crc_chg(struct ade9113_dev *dev, uint8_t *status)
{
	return ade9113_get_int_status(dev, ADE9113_REG_STATUS0, ADE9113_CRC_CHG_MSK,
				      status);
}

/**
 * @brief Get EFUSE_MEM_ERR indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_efuse_mem_err(struct ade9113_dev *dev, uint8_t *status)
{
	return ade9113_get_int_status(dev, ADE9113_REG_STATUS0,
				      ADE9113_EFUSE_MEM_ERR_MSK,
				      status);
}

/**
 * @brief Get SPI_CRC_ERR indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_spi_crc_err(struct ade9113_dev *dev, uint8_t *status)
{
	return ade9113_get_int_status(dev, ADE9113_REG_STATUS0, ADE9113_SPI_CRC_ERR_MSK,
				      status);
}

/**
 * @brief Get COMFLT_ERR indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_comflt_err(struct ade9113_dev *dev, uint8_t *status)
{
	return ade9113_get_int_status(dev, ADE9113_REG_STATUS0, ADE9113_COMFLT_ERR_MSK,
				      status);
}

/**
 * @brief Clear the RESET_DONE int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_clear_reset_done_int(struct ade9113_dev *dev)
{
	return ade9113_write(dev, ADE9113_REG_STATUS0, ADE9113_RESET_DONE_MSK,
			     ADE9113_S_OP);
}

/**
 * @brief Clear the COM_UP int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_clear_com_up_int(struct ade9113_dev *dev)
{
	return ade9113_write(dev, ADE9113_REG_STATUS0, ADE9113_COM_UP_MSK,
			     ADE9113_S_OP);
}

/**
 * @brief Clear the CRC_CHG int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_clear_crc_chg_int(struct ade9113_dev *dev)
{
	return ade9113_write(dev, ADE9113_REG_STATUS0, ADE9113_CRC_CHG_MSK,
			     ADE9113_S_OP);
}

/**
 * @brief Clear the SPI_CRC_ERR int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_clear_spi_crc_err_int(struct ade9113_dev *dev)
{
	return ade9113_write(dev, ADE9113_REG_STATUS0, ADE9113_SPI_CRC_ERR_MSK,
			     ADE9113_S_OP);
}

/**
 * @brief Clear the COMFLT_ERR int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_clear_comflt_err_int(struct ade9113_dev *dev)
{
	return ade9113_write(dev, ADE9113_REG_STATUS0, ADE9113_COM_UP_MSK,
			     ADE9113_S_OP);
}

/**
 * @brief Get V2_WAV_OVRNG indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_v2_wav_ovrng(struct ade9113_dev *dev, uint8_t *status)
{
	return ade9113_get_int_status(dev, ADE9113_REG_STATUS1,
				      ADE9113_V2_WAV_OVRNG_MSK,
				      status);
}

/**
 * @brief Clear the V2_WAV_OVRNG int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_clear_v2_wav_ovrng_int(struct ade9113_dev *dev)
{
	return ade9113_write(dev, ADE9113_REG_STATUS1, ADE9113_V2_WAV_OVRNG_MSK,
			     ADE9113_S_OP);
}

/**
 * @brief Get V1_WAV_OVRNG indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_v1_wav_ovrng(struct ade9113_dev *dev, uint8_t *status)
{
	return ade9113_get_int_status(dev, ADE9113_REG_STATUS1,
				      ADE9113_V1_WAV_OVRNG_MSK,
				      status);
}

/**
 * @brief Clear the V1_WAV_OVRNG int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_clear_v1_wav_ovrng_int(struct ade9113_dev *dev)
{
	return ade9113_write(dev, ADE9113_REG_STATUS1, ADE9113_V1_WAV_OVRNG_MSK,
			     ADE9113_S_OP);
}

/**
 * @brief Get I_WAV_OVRNG indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_i_wav_ovrng(struct ade9113_dev *dev, uint8_t *status)
{
	return ade9113_get_int_status(dev, ADE9113_REG_STATUS1, ADE9113_I_WAV_OVRNG_MSK,
				      status);
}

/**
 * @brief Clear the I_WAV_OVRNG int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_clear_i_wav_ovrng_int(struct ade9113_dev *dev)
{
	return ade9113_write(dev, ADE9113_REG_STATUS1, ADE9113_I_WAV_OVRNG_MSK,
			     ADE9113_S_OP);
}

/**
 * @brief Get ADC_SYNC_DONE indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_adc_sync_done(struct ade9113_dev *dev, uint8_t *status)
{
	return ade9113_get_int_status(dev, ADE9113_REG_STATUS1,
				      ADE9113_ADC_SYNC_DONE_MSK,
				      status);
}

/**
 * @brief Clear the ADC_SYNC_DONE int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_clear_adc_sync_done_int(struct ade9113_dev *dev)
{
	return ade9113_write(dev, ADE9113_REG_STATUS1, ADE9113_ADC_SYNC_DONE_MSK,
			     ADE9113_S_OP);
}

/**
 * @brief Get ISO_CLK_STBL_ERR indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_iso_clk_stbl_err(struct ade9113_dev *dev, uint8_t *status)
{
	return ade9113_get_int_status(dev, ADE9113_REG_STATUS2,
				      ADE9113_ISO_CLK_STBL_ERR_MSK,
				      status);
}

/**
 * @brief Clear the ISO_CLK_STBL_ERR int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_clear_iso_clk_stbl_err_int(struct ade9113_dev *dev)
{
	return ade9113_write(dev, ADE9113_REG_STATUS2, ADE9113_ISO_CLK_STBL_ERR_MSK,
			     ADE9113_S_OP);
}

/**
 * @brief Get ISO_PHY_CRC_ERR indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_iso_phy_crc_err(struct ade9113_dev *dev, uint8_t *status)
{
	return ade9113_get_int_status(dev, ADE9113_REG_STATUS2,
				      ADE9113_ISO_PHY_CRC_ERR_MSK,
				      status);
}

/**
 * @brief Clear the ISO_PHY_CRC_ERR int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_clear_iso_phy_crc_err_int(struct ade9113_dev *dev)
{
	return ade9113_write(dev, ADE9113_REG_STATUS2, ADE9113_ISO_PHY_CRC_ERR_MSK,
			     ADE9113_S_OP);
}

/**
 * @brief Get ISO_EFUSE_MEM_ERR indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_iso_efuse_mem_err_err(struct ade9113_dev *dev, uint8_t *status)
{
	return ade9113_get_int_status(dev, ADE9113_REG_STATUS2,
				      ADE9113_ISO_EFUSE_MEM_ERR_MSK,
				      status);
}

/**
 * @brief Clear the ISO_EFUSE_MEM_ERR int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_clear_iso_efuse_mem_err_int(struct ade9113_dev *dev)
{
	return ade9113_write(dev, ADE9113_REG_STATUS2, ADE9113_ISO_EFUSE_MEM_ERR_MSK,
			     ADE9113_S_OP);
}

/**
 * @brief Get ISO_DIG_MOD_V2_OVF indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_iso_dig_mod_v2_ovf(struct ade9113_dev *dev, uint8_t *status)
{
	return ade9113_get_int_status(dev, ADE9113_REG_STATUS2,
				      ADE9113_ISO_DIG_MOD_V2_OVF_MSK,
				      status);
}

/**
 * @brief Clear the ISO_DIG_MOD_V2_OVF int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_clear_iso_dig_mod_v2_ovf_int(struct ade9113_dev *dev)
{
	return ade9113_write(dev, ADE9113_REG_STATUS2, ADE9113_ISO_DIG_MOD_V2_OVF_MSK,
			     ADE9113_S_OP);
}

/**
 * @brief Get ISO_DIG_MOD_V1_OVF indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_iso_dig_mod_v1_ovf(struct ade9113_dev *dev, uint8_t *status)
{
	return ade9113_get_int_status(dev, ADE9113_REG_STATUS2,
				      ADE9113_ISO_DIG_MOD_V1_OVF_MSK,
				      status);
}

/**
 * @brief Clear the ISO_DIG_MOD_V1_OVF int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_clear_iso_dig_mod_v1_ovf_int(struct ade9113_dev *dev)
{
	return ade9113_write(dev, ADE9113_REG_STATUS2, ADE9113_ISO_DIG_MOD_V1_OVF_MSK,
			     ADE9113_S_OP);
}

/**
 * @brief Get ISO_DIG_MOD_I_OVF indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_iso_dig_mod_i_ovf(struct ade9113_dev *dev, uint8_t *status)
{
	return ade9113_get_int_status(dev, ADE9113_REG_STATUS2,
				      ADE9113_ISO_DIG_MOD_I_OVF_MSK,
				      status);
}

/**
 * @brief Clear the ISO_DIG_MOD_I_OVF int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_clear_iso_dig_mod_i_ovf_int(struct ade9113_dev *dev)
{
	return ade9113_write(dev, ADE9113_REG_STATUS2, ADE9113_ISO_DIG_MOD_I_OVF_MSK,
			     ADE9113_S_OP);
}

/**
 * @brief Get ISO_TEST_MMR_ERR indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_iso_test_mmr_err(struct ade9113_dev *dev, uint8_t *status)
{
	return ade9113_get_int_status(dev, ADE9113_REG_STATUS2,
				      ADE9113_ISO_TEST_MMR_ERR_MSK,
				      status);
}

/**
 * @brief Clear the ISO_TEST_MMR_ERR int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_clear_iso_test_mmr_err_int(struct ade9113_dev *dev)
{
	return ade9113_write(dev, ADE9113_REG_STATUS2, ADE9113_ISO_TEST_MMR_ERR_MSK,
			     ADE9113_S_OP);
}

/**
 * @brief Get ISO_STATUS_RD_ECC_ERR indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_iso_status_rd_ecc_err(struct ade9113_dev *dev, uint8_t *status)
{
	return ade9113_get_int_status(dev, ADE9113_REG_COM_FLT_TYPE,
				      ADE9113_ISO_STATUS_RD_ECC_ERR_MSK,
				      status);
}

/**
 * @brief Get ISO_PHY_ERR indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_iso_phy_err(struct ade9113_dev *dev, uint8_t *status)
{
	return ade9113_get_int_status(dev, ADE9113_REG_COM_FLT_TYPE,
				      ADE9113_ISO_PHY_ERR_MSK,
				      status);
}

/**
 * @brief Get ISO_ECC_ERR indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_iso_ecc_err(struct ade9113_dev *dev, uint8_t *status)
{
	return ade9113_get_int_status(dev, ADE9113_REG_COM_FLT_TYPE,
				      ADE9113_ISO_ECC_ERR_MSK,
				      status);
}

/**
 * @brief Get CRC_DONE indicator.
 * @param dev - The device structure.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_crc_done_flag(struct ade9113_dev *dev, uint8_t *status)
{
	return ade9113_get_int_status(dev, ADE9113_REG_CONFIG_CRC,
				      ADE9113_CRC_DONE_MSK,
				      status);
}

/**
 * @brief Clear the CRC_DONE int mask.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_clear_crc_done_int(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_write(dev, ADE9113_REG_CONFIG_CRC, ADE9113_CRC_DONE_MSK,
			     ADE9113_S_OP);
}

/**
 * @brief Force background register map CRC recalculation.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_force_crc_recalculation(struct ade9113_dev *dev)
{
	if (!dev)
		return -ENODEV;

	return ade9113_write(dev, ADE9113_REG_CONFIG_CRC, ADE9113_CRC_FORCE_MSK,
			     ADE9113_S_OP);
}

/**
 * @brief Get SILICON_REVISION value.
 * @param dev - The device structure.
 * @param silicon_rev - Read silicon revision value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_silicon_revision(struct ade9113_dev *dev, uint8_t *silicon_rev)
{
	if (!silicon_rev)
		return -EINVAL;

	return ade9113_read(dev, ADE9113_REG_SILICON_REVISION, silicon_rev,
			    ADE9113_S_OP);
}

/**
 * @brief Get VERSION_PRODUCT value.
 * @param dev - The device structure.
 * @param ver_product - VErsion product value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_version_product(struct ade9113_dev *dev, uint8_t *ver_product)
{
	int ret;
	uint8_t reg_val;

	if (!ver_product)
		return -EINVAL;

	ret = ade9113_read(dev, ADE9113_REG_VERSION_PRODUCT, &reg_val,
			   ADE9113_S_OP);

	if (ret)
		return ret;

	switch (reg_val) {
	case 0:
		*ver_product = ADE9113_3_CHANNEL_ADE9113;
		break;
	case 1:
		*ver_product = ADE9113_2_CHANNEL_ADE9112;
		break;
	case 3:
		*ver_product = ADE9113_NONISOLATED_ADE9103;
		break;
	default:
		ret = -ENODEV;
		break;
	}

	return ret;
}

/**
 * @brief Get wave value.
 * @param dev - The device structure.
 * @param selection - Wave selection.
 * @param val - Read I_WAV value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_get_wav(struct ade9113_dev *dev, enum ade9113_wav_e selection,
		    uint32_t *val)
{
	int ret;
	uint8_t reg_val;
	uint32_t value;

	if (!val)
		return -EINVAL;

	if (selection > ADE9113_V2_WAV)
		return -EINVAL;

	ret = ade9113_read(dev, ADE9113_REG_I_WAV_HI + 3 * selection, &reg_val,
			   ADE9113_S_OP);
	if (ret)
		return ret;

	value = reg_val;
	value <<= 8;

	ret = ade9113_read(dev, ADE9113_REG_I_WAV_MD + 3 * selection, &reg_val,
			   ADE9113_S_OP);
	if (ret)
		return ret;

	value |= reg_val;
	value <<= 8;

	ret = ade9113_read(dev, ADE9113_REG_I_WAV_LO + 3 * selection, &reg_val,
			   ADE9113_S_OP);
	if (ret)
		return ret;

	value |= reg_val;

	*val = value;

	return 0;
}

/**
 * @brief DRDY inerrupt enable.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_drdy_int_enable(struct ade9113_dev *dev)
{
	return no_os_irq_enable(dev->irq_ctrl, dev->gpio_rdy->number);
}

/**
 * @brief DRDY inerrupt disable.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9113_drdy_int_disable(struct ade9113_dev *dev)
{
	return no_os_irq_disable(dev->irq_ctrl, dev->gpio_rdy->number);
}
