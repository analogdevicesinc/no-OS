/***************************************************************************//**
*   @file   ade7953.c
*   @brief  Implementation of ADE7953 Driver.
*   @author REtz (radu.etz@analog.com)
********************************************************************************
* Copyright 2025(c) Analog Devices, Inc.
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

#include "ade7953.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "no_os_util.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_irq.h"
#include "no_os_delay.h"
#include "no_os_units.h"
#include "no_os_alloc.h"
#include "no_os_crc16.h"
#include "no_os_print_log.h"
#include <stdlib.h>
#include <errno.h>
#include <math.h>

/**
 * @brief Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */

int ade7953_init(struct ade7953_dev **device,
		 struct ade7953_init_param init_param)
{
	int ret;
	/* ADE7953 device structure */
	struct ade7953_dev *dev;
	/* value read from register */
	uint32_t reg_val;

	dev = (struct ade7953_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* SPI Initialization */
	ret = no_os_spi_init(&dev->spi_desc,
			     init_param.spi_init);
	if (ret)
		goto error_spi;

	/* select 24 bits or 32 bits regs comm */
	dev->en_24_bit = init_param.en_24_bit;

	/* interrupt */
	dev->irq_ctrl = init_param.irq_ctrl;
	/* reset gpio */
	dev->gpio_reset = init_param.gpio_reset;

	/* Lock the communication interface after autoidentification */
	ret = ade7953_update_bits(dev, ADE7953_REG_CONFIG, ADE7953_COMM_LOCK_MSK,
				  no_os_field_prep(ADE7953_COMM_LOCK_MSK, ENABLE));

	/* Configure for optimum operation */
	/* see datasheet for details */
	ret = ade7953_write(dev, ADE7953_REG_UNLOCK_120, ADE7953_UNLOCK_KEY);
	ret = ade7953_write(dev, ADE7953_REG_RESERVED, ADE7953_OPT_SETT);

	/* Read version product */
	ret = ade7953_version_product(dev, &reg_val);
	if (ret)
		goto error_spi;

	if (reg_val != ADE7953_VERSION) {
		pr_info("ERROR !!!\n");
		goto error_spi;
	}

	pr_info("Product Version OK \n\n");

	*device = dev;

	return 0;

error_spi:
	no_os_spi_remove(dev->spi_desc);
error_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Read device register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data read from 32bit register.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7953_read(struct ade7953_dev *dev, uint16_t reg_addr,
		 int32_t *reg_data)
{
	int ret;
	/* number of bytes - 4 for 8 bits register data */
	uint8_t no_bytes = 4;
	/* offset of data read in the read buffer */
	uint8_t data_byte_offset = 3;
	/* buffer for data read */
	uint8_t buff[8] = { 0 };
	/* index */
	uint8_t i;
	/* register addres */
	uint32_t addr;

	if (!dev)
		return -ENODEV;
	if (!reg_data)
		return -EINVAL;

	no_os_put_unaligned_be16(reg_addr, &buff);
	buff[2] = ADE7953_SPI_READ;

	/* 8 bits registers */
	if (IS_8BITS_REG(reg_addr)) {
		ret = no_os_spi_write_and_read(dev->spi_desc, buff, no_bytes);
		if (ret)
			return ret;

		*reg_data = buff[data_byte_offset];

	} else if (IS_16BITS_REG(reg_addr)) {
		/* 16 bits registers */
		ret = no_os_spi_write_and_read(dev->spi_desc, buff, no_bytes + 1);
		if (ret)
			return ret;

		*reg_data = no_os_sign_extend32(no_os_get_unaligned_be16(
							&buff[data_byte_offset]), 15);
	} else {
		if (dev->en_24_bit) {
			/* 24 bits registers */
			ret = no_os_spi_write_and_read(dev->spi_desc, buff, no_bytes + 2);
			if (ret)
				return ret;

			*reg_data = no_os_sign_extend32(no_os_get_unaligned_be24(
								&buff[data_byte_offset]), 23);
		} else {
			/* 32 bits registers */
			/* offset of the 32 bits registers adresses is 0x100 */
			buff[0]++;
			ret = no_os_spi_write_and_read(dev->spi_desc, buff, no_bytes + 3);
			if (ret)
				return ret;

			*reg_data = no_os_get_unaligned_be32(&buff[data_byte_offset]);
		}
	}

	return 0;
}

/**
 * @brief Write device register.
 * @param dev- The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7953_write(struct ade7953_dev *dev, uint16_t reg_addr,
		  uint32_t reg_data)
{
	int ret;
	/* number of bytes - 4 for 8 bits register data */
	uint8_t no_bytes = 4;
	/* buffer for data write */
	uint8_t buff[8] = {0};
	/* register addres */
	uint16_t addr;
	/* data to be written */
	uint32_t data;
	/* offset of data to be written in the buffer */
	uint8_t data_byte_offset = 3;
	int i;

	if (!dev)
		return -ENODEV;

	no_os_put_unaligned_be16(reg_addr, &buff);
	buff[2] = 0x00;

	/* 8 bits registers */
	if (IS_8BITS_REG(reg_addr)) {
		buff[3] = (uint8_t)reg_data;

		return no_os_spi_write_and_read(dev->spi_desc, buff, no_bytes);
	} else if (IS_16BITS_REG(reg_addr)) {
		/* 16 bits registers */
		no_os_put_unaligned_be16(reg_data, &buff[data_byte_offset]);

		return no_os_spi_write_and_read(dev->spi_desc, buff, no_bytes + 1);
	} else {
		if (dev->en_24_bit) {
			/* 24 bits registers */
			no_os_put_unaligned_be24(reg_data, &buff[data_byte_offset]);

			return no_os_spi_write_and_read(dev->spi_desc, buff, no_bytes + 2);
		} else {
			/* 32 bits registers */
			no_os_put_unaligned_be32(reg_data, &buff[data_byte_offset]);

			return no_os_spi_write_and_read(dev->spi_desc, buff, no_bytes + 3);
		}
	}
}

/**
 * @brief Update specific register bits.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - Specific bits mask.
 * @param reg_data - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ade7953_update_bits(struct ade7953_dev *dev, uint16_t reg_addr,
			       uint32_t mask, uint32_t reg_data)
{
	int ret;
	/* data to be written */
	uint32_t data;

	ret = ade7953_read(dev, reg_addr, &data);
	if (ret)
		return ret;

	data &= ~mask;
	data |= reg_data & mask;

	return ade7953_write(dev, reg_addr, data);
}

/**
 * @brief Remove the device and release resources.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7953_remove(struct ade7953_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_reset);
	if (ret)
		return ret;

	ret = no_os_irq_ctrl_remove(dev->irq_ctrl);
	if (ret)
		return ret;

	ret = no_os_irq_ctrl_remove(dev->zx_ctrl);
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
int ade7953_sw_reset(struct ade7953_dev *dev)
{
	int ret;

	ret = ade7953_update_bits(dev, ADE7953_REG_CONFIG, ADE7953_SWRST_MSK,
				  no_os_field_prep(ADE7953_SWRST_MSK, ENABLE));
	if (ret)
		return ret;

	/* Wait for device to initialize */
	no_os_mdelay(ADE7953_RESET_DEL);

	return 0;
}

/**
 * @brief Reset the device using HW reset.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7953_hw_reset(struct ade7953_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	if (dev->gpio_reset) {
		ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_LOW);
		if (ret)
			return ret;
	}
	no_os_mdelay(1);

	if (dev->gpio_reset)
		ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Lock write to registers.
 * @param dev - The device structure.
 * @param regs_select - Selects the registers to be write protected.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7953_wr_lock_8bit(struct ade7953_dev *dev,
			 enum ade7953_write_protect_e regs_select)
{
	int ret;

	switch (regs_select) {
	case ADE7953_8BITS_REGS :
		ret = ade7953_update_bits(dev, ADE7953_REG_WRITE_PROTECT,
					  ADE7953_8_BITS_PROTECT,
					  no_os_field_prep(ADE7953_8_BITS_PROTECT, ENABLE));
		break;
	case ADE7953_16BITS_REGS :
		ret = ade7953_update_bits(dev, ADE7953_REG_WRITE_PROTECT,
					  ADE7953_16_BITS_PROTECT,
					  no_os_field_prep(ADE7953_16_BITS_PROTECT, ENABLE));
		break;
	case ADE7953_24_32BITS_REGS :
		ret = ade7953_update_bits(dev, ADE7953_REG_WRITE_PROTECT,
					  ADE7953_24_32_BITS_PROTECT,
					  no_os_field_prep(ADE7953_24_32_BITS_PROTECT, ENABLE));
		break;
	default :
		ret = -EINVAL;
	}
	return ret;
}

/**
 * @brief Version product
 * @param dev - The device structure.
 * @param data_read - The version product read value
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7953_version_product(struct ade7953_dev *dev, uint32_t *data_read)
{
	int ret;
	/* version product */
	uint32_t version;

	if (!data_read)
		return -EINVAL;

	ret = ade7953_read(dev, ADE7953_REG_VERSION_PRODUCT, &version);
	if (ret)
		return ret;

	*data_read = version;

	return 0;
}

/**
 * @brief reset IApeak val
 * @param dev - The device structure.
 * @param val - Iapeak val
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7953_reset_iapk_val(struct ade7953_dev *dev, uint32_t *val)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!val)
		return -EINVAL;

	ret = ade7953_read(dev, ADE7953_REG_RSTIAPEAK, &reg_val);
	if (ret)
		return ret;
	*val = reg_val;

	return 0;
}

/**
 * @brief reset IBpeak val
 * @param dev - The device structure.
 * @param val - Ibpeak val
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7953_reset_ibpk_val(struct ade7953_dev *dev, uint32_t *val)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!val)
		return -EINVAL;

	ret = ade7953_read(dev, ADE7953_REG_RSTIBPEAK, &reg_val);
	if (ret)
		return ret;
	*val = reg_val;

	return 0;
}

/**
 * @brief reset Vpeak val
 * @param dev - The device structure.
 * @param val - Vpeak val
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7953_reset_vpk_val(struct ade7953_dev *dev, uint32_t *val)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!val)
		return -EINVAL;

	ret = ade7953_read(dev, ADE7953_REG_RSTVPEAK, &reg_val);
	if (ret)
		return ret;
	*val = reg_val;

	return 0;
}

/**
 * @brief Get interrupt indicator from STATA register.
 * @param dev - The device structure.
 * @param msk - Interrupt mask.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7953_get_int_stata(struct ade7953_dev *dev, uint32_t msk,
			  uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!status)
		return -EINVAL;

	ret = ade7953_read(dev, ADE7953_REG_IRQSTATA, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(msk), &reg_val);

	return 0;
}

/**
 * @brief Get interrupt indicator from STATB register.
 * @param dev - The device structure.
 * @param msk - Interrupt mask.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7953_get_int_statb(struct ade7953_dev *dev, uint32_t msk,
			  uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!status)
		return -EINVAL;

	ret = ade7953_read(dev, ADE7953_REG_IRQSTATB, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(msk), &reg_val);

	return 0;
}

/**
 * @brief Clear irq stata flags.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7953_clear_irq_stata(struct ade7953_dev *dev)
{
	uint32_t data;

	return ade7953_read(dev, ADE7953_REG_RSTIRQSTATA, &data);

}

/**
 * @brief Clear irq statb flags.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7953_clear_irq_statb(struct ade7953_dev *dev)
{
	uint32_t data;

	return ade7953_read(dev, ADE7953_REG_RSTIRQSTATB, &data);

}

/**
 * @brief Enable interrupt voltage ch and current ch A.
 * @param dev - The device structure.
 * @param msk - Interrupt mask.
 * @param en -Enable/Disable
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7953_enable_int_a(struct ade7953_dev *dev, uint32_t msk, uint8_t en)
{

	return ade7953_update_bits(dev, ADE7953_REG_IRQENA, msk,
				   no_os_field_prep(msk, en));

}

/**
 * @brief Enable/disable interrupt voltage ch and current ch B.
 * @param dev - The device structure.
 * @param msk - Interrupt mask.
 * @param en -Enable/Disable
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7953_enable_int_b(struct ade7953_dev *dev, uint32_t msk, uint8_t en)
{

	return ade7953_update_bits(dev, ADE7953_REG_IRQENB, msk,
				   no_os_field_prep(msk, en));

}

/**
 * @brief Zero-crossing interrupt edge selection
 * @param dev - The device structure.
 * @param sel - Edge selection
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7953_zx_int_edge_set(struct ade7953_dev *dev,
			    enum ade7953_zx_edge_e sel)
{
	int ret;

	switch (sel) {
	case ADE7953_ZX_BOTH_2:
		ret = ade7953_update_bits(dev, ADE7953_REG_CONFIG, ADE7953_ZX_EDGE_MSK,
					  no_os_field_prep(ADE7953_ZX_EDGE_MSK, ADE7953_ZX_BOTH_2));
		break;
	case ADE7953_ZX_POS:
		ret = ade7953_update_bits(dev, ADE7953_REG_CONFIG, ADE7953_ZX_EDGE_MSK,
					  no_os_field_prep(ADE7953_ZX_EDGE_MSK, ADE7953_ZX_POS));
		break;
	case ADE7953_ZX_NEG:
		ret = ade7953_update_bits(dev, ADE7953_REG_CONFIG, ADE7953_ZX_EDGE_MSK,
					  no_os_field_prep(ADE7953_ZX_EDGE_MSK, ADE7953_ZX_NEG));
		break;
	default:
		ret = ade7953_update_bits(dev, ADE7953_REG_CONFIG, ADE7953_ZX_EDGE_MSK,
					  no_os_field_prep(ADE7953_ZX_EDGE_MSK, ADE7953_ZX_BOTH_1));
		break;
	}

	return ret;
}

/**
 * @brief Configure output signal on CF1/CF2 pin
 * @param dev - The device structure.
 * @param sel - CF signal
 * @param cf_pin - 0 for CF1 pin, 1 for CF2 pin;
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7953_cf_output_set(struct ade7953_dev *dev,
			  enum ade7953_cfsel_e sel, enum ade7953_cf_pin_e cf_pin)
{
	int ret;
	/* CFx sel mask*/
	uint16_t mask;

	if (cf_pin == ADE7953_CF1_PIN)
		mask = ADE7953_CF1SEL_MSK;
	else if (cf_pin == ADE7953_CF2_PIN)
		mask = ADE7953_CF2SEL_MSK;
	else
		return -EINVAL;

	switch (sel) {
	case CF_APA_APB:
		ret = ade7953_update_bits(dev, ADE7953_REG_CFMODE, mask,
					  no_os_field_prep(mask, CF_APA_APB));
		break;
	case CF_IRMSA_IRMSB:
		ret = ade7953_update_bits(dev, ADE7953_REG_CFMODE, mask,
					  no_os_field_prep(mask, CF_IRMSA_IRMSB));
		break;
	case CF_IRMSB:
		ret = ade7953_update_bits(dev, ADE7953_REG_CFMODE, mask,
					  no_os_field_prep(mask, CF_IRMSB));
		break;
	case CF_APPB:
		ret = ade7953_update_bits(dev, ADE7953_REG_CFMODE, mask,
					  no_os_field_prep(mask, CF_APPB));
		break;
	case CF_RPB:
		ret = ade7953_update_bits(dev, ADE7953_REG_CFMODE, mask,
					  no_os_field_prep(mask, CF_RPB));
		break;
	case CF_BPB:
		ret = ade7953_update_bits(dev, ADE7953_REG_CFMODE, mask,
					  no_os_field_prep(mask, CF_BPB));
		break;
	case CF_IRMSA:
		ret = ade7953_update_bits(dev, ADE7953_REG_CFMODE, mask,
					  no_os_field_prep(mask, CF_IRMSA));
		break;
	case CF_APPA:
		ret = ade7953_update_bits(dev, ADE7953_REG_CFMODE, mask,
					  no_os_field_prep(mask, CF_APPA));
		break;
	case CF_RPA:
		ret = ade7953_update_bits(dev, ADE7953_REG_CFMODE, mask,
					  no_os_field_prep(mask, CF_RPA));
		break;
	default:
		ret = ade7953_update_bits(dev, ADE7953_REG_CFMODE, mask,
					  no_os_field_prep(mask, CF_APA));
		break;
	}

	return ret;
}

/**
 * @brief Configure of ZX pin (Pin1)
 * @param dev - The device structure.
 * @param sel - ZX pin configuration
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7953_zx_config_pin(struct ade7953_dev *dev,
			  enum ade7953_zx_alt_e sel)
{
	int ret;

	switch (sel) {
	case ZX_ALT_REVP:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_ZX_ALT_MSK,
					  no_os_field_prep(ADE7953_ZX_ALT_MSK, ZX_ALT_REVP));
		break;
	case ZX_ALT_ZX_I:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_ZX_ALT_MSK,
					  no_os_field_prep(ADE7953_ZX_ALT_MSK, ZX_ALT_ZX_I));
		break;
	case ZX_ALT_IRQ:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_ZX_ALT_MSK,
					  no_os_field_prep(ADE7953_ZX_ALT_MSK, ZX_ALT_IRQ));
		break;
	case ZX_ALT_WSMP:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_ZX_ALT_MSK,
					  no_os_field_prep(ADE7953_ZX_ALT_MSK, ZX_ALT_WSMP));
		break;
	case ZX_ALT_VARNLOAD_B:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_ZX_ALT_MSK,
					  no_os_field_prep(ADE7953_ZX_ALT_MSK, ZX_ALT_VARNLOAD_B));
		break;
	case ZX_ALT_VARNLOAD_A:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_ZX_ALT_MSK,
					  no_os_field_prep(ADE7953_ZX_ALT_MSK, ZX_ALT_VARNLOAD_A));
		break;
	case ZX_ALT_APNLOAD_B:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_ZX_ALT_MSK,
					  no_os_field_prep(ADE7953_ZX_ALT_MSK, ZX_ALT_APNLOAD_B));
		break;
	case ZX_ALT_APNLOAD_A:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_ZX_ALT_MSK,
					  no_os_field_prep(ADE7953_ZX_ALT_MSK, ZX_ALT_APNLOAD_A));
		break;
	case ZX_ALT_SAG:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_ZX_ALT_MSK,
					  no_os_field_prep(ADE7953_ZX_ALT_MSK, CF_RPA));
		break;
	default:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_ZX_ALT_MSK,
					  no_os_field_prep(ADE7953_ZX_ALT_MSK, ZX_ALT_ZX));
		break;
	}

	return ret;
}

/**
 * @brief Configure of ZXI pin (Pin21)
 * @param dev - The device structure.
 * @param sel - ZXI pin configuration
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7953_zxi_config_pin(struct ade7953_dev *dev,
			   enum ade7953_zxi_alt_e sel)
{
	int ret;

	switch (sel) {
	case ZXI_ALT_REVP:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_REVP_ALT_MSK,
					  no_os_field_prep(ADE7953_REVP_ALT_MSK, ZXI_ALT_REVP));
		break;
	case ZXI_ALT_ZX:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_REVP_ALT_MSK,
					  no_os_field_prep(ADE7953_REVP_ALT_MSK, ZXI_ALT_ZX));
		break;
	case ZXI_ALT_IRQ:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_REVP_ALT_MSK,
					  no_os_field_prep(ADE7953_REVP_ALT_MSK, ZXI_ALT_IRQ));
		break;
	case ZXI_ALT_WSMP:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_REVP_ALT_MSK,
					  no_os_field_prep(ADE7953_REVP_ALT_MSK, ZXI_ALT_WSMP));
		break;
	case ZXI_ALT_VARNLOAD_B:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_REVP_ALT_MSK,
					  no_os_field_prep(ADE7953_REVP_ALT_MSK, ZXI_ALT_VARNLOAD_B));
		break;
	case ZXI_ALT_VARNLOAD_A:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_REVP_ALT_MSK,
					  no_os_field_prep(ADE7953_REVP_ALT_MSK, ZXI_ALT_VARNLOAD_A));
		break;
	case ZXI_ALT_APNLOAD_B:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_REVP_ALT_MSK,
					  no_os_field_prep(ADE7953_REVP_ALT_MSK, ZXI_ALT_APNLOAD_B));
		break;
	case ZXI_ALT_APNLOAD_A:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_REVP_ALT_MSK,
					  no_os_field_prep(ADE7953_REVP_ALT_MSK, ZXI_ALT_APNLOAD_A));
		break;
	case ZXI_ALT_SAG:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_REVP_ALT_MSK,
					  no_os_field_prep(ADE7953_REVP_ALT_MSK, ZXI_ALT_SAG));
		break;
	default:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_REVP_ALT_MSK,
					  no_os_field_prep(ADE7953_REVP_ALT_MSK, ZXI_ALT_ZX_I));
		break;
	}

	return ret;
}

/**
 * @brief Configure of REVP pin (Pin20)
 * @param dev - The device structure.
 * @param sel - REVP pin configuration
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7953_revp_pin_config(struct ade7953_dev *dev,
			    enum ade7953_revp_alt_e sel)
{
	int ret;

	switch (sel) {
	case REVP_ALT_ZX_I:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_REVP_ALT_MSK,
					  no_os_field_prep(ADE7953_REVP_ALT_MSK, REVP_ALT_ZX_I));
		break;
	case REVP_ALT_ZX:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_REVP_ALT_MSK,
					  no_os_field_prep(ADE7953_REVP_ALT_MSK, REVP_ALT_ZX));
		break;
	case REVP_ALT_IRQ:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_REVP_ALT_MSK,
					  no_os_field_prep(ADE7953_REVP_ALT_MSK, REVP_ALT_IRQ));
		break;
	case REVP_ALT_WSMP:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_REVP_ALT_MSK,
					  no_os_field_prep(ADE7953_REVP_ALT_MSK, REVP_ALT_WSMP));
		break;
	case REVP_ALT_VARNLOAD_B:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_REVP_ALT_MSK,
					  no_os_field_prep(ADE7953_REVP_ALT_MSK, REVP_ALT_VARNLOAD_B));
		break;
	case REVP_ALT_VARNLOAD_A:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_REVP_ALT_MSK,
					  no_os_field_prep(ADE7953_REVP_ALT_MSK, REVP_ALT_VARNLOAD_A));
		break;
	case REVP_ALT_APNLOAD_B:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_REVP_ALT_MSK,
					  no_os_field_prep(ADE7953_REVP_ALT_MSK, REVP_ALT_APNLOAD_B));
		break;
	case REVP_ALT_APNLOAD_A:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_REVP_ALT_MSK,
					  no_os_field_prep(ADE7953_REVP_ALT_MSK, REVP_ALT_APNLOAD_A));
		break;
	case REVP_ALT_SAG:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_REVP_ALT_MSK,
					  no_os_field_prep(ADE7953_REVP_ALT_MSK, REVP_ALT_SAG));
		break;
	default:
		ret = ade7953_update_bits(dev, ADE7953_REG_ALT_OUTPUT, ADE7953_REVP_ALT_MSK,
					  no_os_field_prep(ADE7953_REVP_ALT_MSK, REVP_ALT_REVP));
		break;
	}

	return ret;
}

/**
 * @brief ACC mode selection for active energy
 * @param dev - The device structure.
 * @param mode - mode selection
 * @param channel - current channel selector
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7953_acc_active_engy_mode(struct ade7953_dev *dev,
				 enum ade7953_awattacc_e mode, enum ade7953_i_ch_e channel)
{
	int ret;
	/* current channel */
	uint16_t ch;

	if (channel == ADE7953_I_CH1)
		ch = ADE7953_AWATTACC_MSK;
	else if (channel == ADE7953_I_CH2)
		ch = ADE7953_BWATTACC_MSK;
	else
		return -EINVAL;

	switch (mode) {
	case ADE7953_ABSOLUTE_ACC_MODE_AWATT:
		ret = ade7953_update_bits(dev, ADE7953_REG_ACCMODE, ch,
					  no_os_field_prep(ch, ADE7953_ABSOLUTE_ACC_MODE_AWATT));
		break;
	case ADE7953_POSITIVE_ACC_MODE:
		ret = ade7953_update_bits(dev, ADE7953_REG_ACCMODE, ch,
					  no_os_field_prep(ch, ADE7953_POSITIVE_ACC_MODE));
		break;
	default:
		ret = ade7953_update_bits(dev, ADE7953_REG_ACCMODE, ch,
					  no_os_field_prep(ch, ADE7953_NORMAL_ACC_MODE_AWATT));
		break;
	}

	return ret;
}

/**
 * @brief ACC mode selection for reactive energy
 * @param dev - The device structure.
 * @param mode - mode selection
 * @param channel - current channel selector
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7953_acc_reactive_engy_mode(struct ade7953_dev *dev,
				   enum ade7953_avaracc_e mode, enum ade7953_i_ch_e channel)
{
	int ret;
	/* current channel */
	uint16_t ch;

	if (channel == ADE7953_I_CH1)
		ch = ADE7953_AVARACC_MSK;
	else if (channel == ADE7953_I_CH2)
		ch = ADE7953_BVARACC_MSK;
	else
		return -EINVAL;

	switch (mode) {
	case ADE7953_ABSOLUTE_ACC_MODE_AVAR:
		ret = ade7953_update_bits(dev, ADE7953_REG_ACCMODE, ch,
					  no_os_field_prep(ch, ADE7953_ABSOLUTE_ACC_MODE_AVAR));
		break;
	case ADE7953_ANTITAMP_ACC_MODE:
		ret = ade7953_update_bits(dev, ADE7953_REG_ACCMODE, ch,
					  no_os_field_prep(ch, ADE7953_ANTITAMP_ACC_MODE));
		break;
	default:
		ret = ade7953_update_bits(dev, ADE7953_REG_ACCMODE, ch,
					  no_os_field_prep(ch, ADE7953_NORMAL_ACC_MODE_AVAR));
		break;
	}

	return ret;
}

/**
 * @brief Read energy values
 * @param dev - The device structure.
 * @param data - Structure to store energy values
 * @param channel - current channel selector
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7953_energy_vals(struct ade7953_dev *dev,
			struct ade7953_energy_values *data, enum ade7953_i_ch_e channel)
{
	int ret;
	/* temporary value read from register */
	uint32_t temp_val;
	/* energy registers addresses */
	uint16_t active_energy_addr, reactive_energy_addr, apparent_energy_addr;
	if (!data)
		return -EINVAL;

	if (channel == ADE7953_I_CH1) {
		active_energy_addr = ADE7953_REG_AENERGYA;
		reactive_energy_addr = ADE7953_REG_RENERGYA;
		apparent_energy_addr = ADE7953_REG_APENERGYA;
	} else if (channel == ADE7953_I_CH2) {
		active_energy_addr = ADE7953_REG_AENERGYB;
		reactive_energy_addr = ADE7953_REG_RENERGYB;
		apparent_energy_addr = ADE7953_REG_APENERGYB;
	} else
		return -EINVAL;

	/* read accumulated total active energy MSBs */
	ret = ade7953_read(dev, active_energy_addr, &temp_val);
	if (ret)
		return ret;
	data->active_energy_reg_val = (int32_t)temp_val;

	/* read accumulated fundamental reactive energy MSBs */
	ret = ade7953_read(dev, reactive_energy_addr, &temp_val);
	if (ret)
		return ret;
	data->fundamental_reactive_energy_reg_val = (int32_t)temp_val;

	/* read accumulated total apparent energy MSBs */
	ret = ade7953_read(dev, apparent_energy_addr, &temp_val);
	if (ret)
		return ret;
	data->apparent_energy_reg_val = (int32_t)temp_val;

	return 0;
}

/**
 * @brief Read power values
 * @param dev - The device structure.
 * @param data - Structure to store power values
 * @param channel - current channel selector
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7953_power_vals(struct ade7953_dev *dev,
		       struct ade7953_power_values *data, enum ade7953_i_ch_e channel)
{
	int ret;
	/* temporary value read from register */
	uint32_t temp_val;
	/* power registers addresses */
	uint16_t active_power_addr, reactive_power_addr, apparent_power_addr;

	if (!data)
		return -EINVAL;

	if (channel == ADE7953_I_CH1) {
		active_power_addr = ADE7953_REG_AWATT;
		reactive_power_addr = ADE7953_REG_AVAR;
		apparent_power_addr = ADE7953_REG_AVA;
	} else if (channel == ADE7953_I_CH2) {
		active_power_addr = ADE7953_REG_BWATT;
		reactive_power_addr = ADE7953_REG_BVAR;
		apparent_power_addr = ADE7953_REG_BVA;
	} else
		return -EINVAL;

	/* read total active power */
	ret = ade7953_read(dev, active_power_addr, &temp_val);
	if (ret)
		return ret;
	data->active_power_reg_val = (int32_t)temp_val;

	/* read fundamental reactive power */
	ret = ade7953_read(dev, reactive_power_addr, &temp_val);
	if (ret)
		return ret;
	data->reactive_power_reg_val = (int32_t)temp_val;

	/* read total apparent power */
	ret = ade7953_read(dev, apparent_power_addr, &temp_val);
	if (ret)
		return ret;
	data->apparent_power_reg_val = temp_val;

	return 0;
}

/**
 * @brief Read rms values
 * @param dev - The device structure.
 * @param data - Structure to store rms values
 * @param channel - current channel selector
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7953_rms_vals(struct ade7953_dev *dev,
		     struct ade7953_rms_values *data, enum ade7953_i_ch_e channel)
{
	int ret;
	/* temporary value read from register */
	uint32_t temp_val;
	/* current channel */
	uint16_t ch;

	if (!data)
		return -EINVAL;

	/* read voltage rms value */
	ret = ade7953_read(dev, ADE7953_REG_VRMS, &temp_val);
	if (ret)
		return ret;
	data->voltage_rms_reg_val = (int32_t)temp_val;

	/* select current channel to read */
	if (channel == ADE7953_I_CH1)
		ch = ADE7953_REG_IRMSA;
	else if (channel == ADE7953_I_CH2)
		ch = ADE7953_REG_IRMSB;
	else
		return -EINVAL;
	/* read current rms value */
	ret = ade7953_read(dev, ch, &temp_val);
	if (ret)
		return ret;
	if (channel == ADE7953_I_CH1)
		data->current_chA_rms_reg_val = (int32_t)temp_val;
	else
		data->current_chB_rms_reg_val = (int32_t)temp_val;

	return 0;
}

/**
 * @brief Read power quaility values
 * @param dev - The device structure.
 * @param data - Structure to store power quality values
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7953_power_quality_vals(struct ade7953_dev *dev,
			       struct ade7953_pq_values *data)
{
	int ret;
	/* temporary value read from register */
	int32_t temp_val;

	if (!data)
		return -EINVAL;

	/* read power factor value */
	ret = ade7953_read(dev, ADE7953_REG_PFB, &temp_val);
	if (ret)
		return ret;
	data->power_factor_reg_val = (int32_t)temp_val;

	/* read voltage rms value */
	ret = ade7953_read(dev, ADE7953_REG_PERIOD, &temp_val);
	if (ret)
		return ret;
	data->period_reg_val = (int32_t)temp_val;

	return 0;
}
