/***************************************************************************//**
 *   @file   ade7978.c
 *   @brief  Implementation of ADE7978 Driver.
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#include <stdlib.h>
#include <errno.h>
#include "ade7978.h"
#include "no_os_delay.h"
#include "no_os_units.h"
#include "no_os_alloc.h"

/**
 * @brief Read device register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data read from the register.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7978_read(struct ade7978_dev *dev, uint16_t reg_addr, uint32_t *reg_data)
{
	int ret;
	/* data buffer large enough for 32 bits reg */
	uint8_t buff[7] = { 0 };
	/* register addres */
	uint32_t addr;

	if (!dev)
		return -ENODEV;
	if (!reg_data)
		return -EINVAL;

	buff[0] = ADE7978_SPI_READ_CMD;
	no_os_put_unaligned_be16(reg_addr, &buff[1]);

	/* 8 bits registers */
	if (IS_8BITS_REG(reg_addr)) {
		ret = no_os_spi_write_and_read(dev->spi_desc, buff, 4);
		if (ret)
			return ret;

		*reg_data = buff[3];

	} else if (IS_16BITS_REG(reg_addr)) {
		/* 16 bits registers */
		ret = no_os_spi_write_and_read(dev->spi_desc, buff, 5);
		if (ret)
			return ret;

		*reg_data = no_os_get_unaligned_be16(&buff[3]);
	} else {
		/* 32 bits registers */
		ret = no_os_spi_write_and_read(dev->spi_desc, buff, 7);
		if (ret)
			return ret;

		*reg_data = no_os_get_unaligned_be32(&buff[3]);
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
int ade7978_write(struct ade7978_dev *dev, uint16_t reg_addr, uint32_t reg_data)
{
	/* data buffer */
	uint8_t buff[7] = { 0 };
	int i;

	if (!dev)
		return -ENODEV;
	/* write operation */
	buff[0] = ADE7978_SPI_WRITE_CMD;
	no_os_put_unaligned_be16(reg_addr, &buff[1]);

	/* 8 bits registers */
	if (IS_8BITS_REG(reg_addr)) {
		buff[3] = (uint8_t)reg_data;

		return no_os_spi_write_and_read(dev->spi_desc, buff, 4);
	}

	/* 16 bits registers */
	if (IS_16BITS_REG(reg_addr)) {
		no_os_put_unaligned_be16(reg_data, &buff[3]);

		return no_os_spi_write_and_read(dev->spi_desc, buff, 5);
	}

	/* 32 bits registers */
	no_os_put_unaligned_be32(reg_data, &buff[3]);

	return no_os_spi_write_and_read(dev->spi_desc, buff, 7);
}

/**
 * @brief Update specific register bits.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - Specific bits mask.
 * @param reg_data - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7978_update_bits(struct ade7978_dev *dev, uint16_t reg_addr,
			uint32_t mask, uint32_t reg_data)
{
	int ret;
	/* register value read */
	uint32_t data;

	ret = ade7978_read(dev, reg_addr, &data);
	if (ret)
		return ret;

	data &= ~mask;
	data |= reg_data & mask;

	return ade7978_write(dev, reg_addr, data);
}

/**
 * @brief Get interrupt indicator from STATUS0 register.
 * @param dev - The device structure.
 * @param msk - Interrupt mask.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7978_get_int_status0(struct ade7978_dev *dev, uint32_t msk,
			    uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!status)
		return -EINVAL;

	ret = ade7978_read(dev, ADE7978_REG_STATUS0, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(msk), &reg_val);

	return 0;
}

/**
 * @brief Get interrupt indicator from STATUS1 register.
 * @param dev - The device structure.
 * @param msk - Interrupt mask.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7978_get_int_status1(struct ade7978_dev *dev, uint32_t msk,
			    uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!status)
		return -EINVAL;

	ret = ade7978_read(dev, ADE7978_REG_STATUS1, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(msk), &reg_val);

	return 0;
}

/**
 * @brief Read the measurements for specific phase.
 * @param dev - The device structure.
 * @param phase - ADE7978 Phase.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7978_read_data_ph(struct ade7978_dev *dev, enum ade7978_phase phase)
{
	int ret;
	/* register value */
	uint32_t temp;
	/* i rms phase register addr */
	uint32_t irms_reg;
	/* v rms phase register addr */
	uint32_t vrms_reg;
	/* v2 rms phase register addr */
	uint32_t v2rms_reg;
	/* temperature value for phase */
	uint32_t temperature;
	/* intermediate values used for computation */
	uint64_t val_int;

	if (!dev)
		return -ENODEV;

	/* select the phase for values read */
	switch (phase) {
	case ADE7978_PHASE_A:
		irms_reg = ADE7978_REG_AIRMS;
		vrms_reg = ADE7978_REG_AVRMS;
		if (dev->temp_en)
			temperature = ADE7978_REG_ATEMP;
		else
			v2rms_reg = ADE7978_REG_AV2RMS;
		break;
	case ADE7978_PHASE_B:
		irms_reg = ADE7978_REG_BIRMS;
		vrms_reg = ADE7978_REG_BVRMS;
		if (dev->temp_en)
			temperature = ADE7978_REG_BTEMP;
		else
			v2rms_reg = ADE7978_REG_BV2RMS;
		break;
	case ADE7978_PHASE_C:
		irms_reg = ADE7978_REG_CIRMS;
		vrms_reg = ADE7978_REG_CVRMS;
		if (dev->temp_en)
			temperature = ADE7978_REG_CTEMP;
		else
			v2rms_reg = ADE7978_REG_CV2RMS;
		break;
	default:
		return -EINVAL;
	}

	ret = ade7978_read(dev, irms_reg, &temp);
	if (ret)
		return ret;
	/* Current rms value in ADC code */
	dev->irms_val = temp;

	ret = ade7978_read(dev, vrms_reg, &temp);
	if (ret)
		return ret;
	/* Voltage rms value in ADC code */
	dev->vrms_val = temp;

	if (dev->temp_en) {
		ret = ade7978_read(dev, temperature, &temp);
		if (ret)
			return ret;
		/* Temperature measurement in ADC code */
		dev->temperature = temp;
	} else {
		ret = ade7978_read(dev, v2rms_reg, &temp);
		if (ret)
			return ret;
		/* Voltage 2 rms value in ADC code */
		dev->v2rms_val = temp;
	}

	return 0;
}

/**
 * @brief Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7978_init(struct ade7978_dev **device,
		 struct ade7978_init_param init_param)
{
	int ret;
	/* device structure */
	struct ade7978_dev *dev;
	/* chip id read value */
	uint32_t chip_id;
	/* data read */
	uint32_t data;

	dev = (struct ade7978_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* Hard reset the device */
	if (!init_param.reset_desc) {
		ret = - EINVAL;
		goto error_dev;
	}
	dev->reset_desc = init_param.reset_desc;
	ret = no_os_gpio_set_value(dev->reset_desc,
				   NO_OS_GPIO_LOW);
	if (ret)
		goto error_dev;

	/* delay reset */
	no_os_mdelay(1);
	ret = no_os_gpio_set_value(dev->reset_desc,
				   NO_OS_GPIO_HIGH);
	if (ret)
		goto error_dev;

	/* wait for device to initialize after hardware reset */
	/* >= 100 ms see datasheet. */
	no_os_mdelay(ADE7978_RESET_RECOVER);

	/* SPI Initialization */
	ret = no_os_spi_init(&dev->spi_desc, init_param.spi_init);
	if (ret)
		goto error_dev;

	/* Select the SPI as the serial interface */
	/* Execute three SPI writes to an unallocated address space -> see datasheet. */
	for (int i = 0; i < 3; i++) {
		ret = ade7978_write(dev, ADE7978_SET_SPI_ADDR, ADE7978_DUMB_VAL);
		if (ret)
			goto error_spi;
	}

	/* Check the rstdone bit */
	do {
		ret = ade7978_get_int_status1(dev, ADE7978_STATUS1_RSTDONE, &data);
		if (ret)
			goto error_spi;
	} while (!data);

	/* Temperature read enable/disable */
	dev->temp_en = init_param.temp_en;

	/* Reset status bits */
	ret = ade7978_write(dev, ADE7978_REG_STATUS1, 0xFFFF);
	if (ret)
		goto error_spi;
	ret = ade7978_write(dev, ADE7978_REG_STATUS1, 0xFFFF);
	if (ret)
		goto error_spi;

	/* Lock the SPI port as active */
	ret = ade7978_update_bits(dev, ADE7978_REG_CONFIG2, ADE7978_I2C_LOCK,
				  no_os_field_prep(ADE7978_I2C_LOCK, ENABLE));
	if (ret)
		goto error_spi;

	/* Use a valid register with default value different from 0 */
	ret = ade7978_read(dev, ADE7978_REG_CFMODE, &chip_id);
	if (ret)
		goto error_spi;

	if (chip_id != ADE7978_CHIP_ID) {
		pr_err("Device ID error \n");
		goto error_spi;
	}

	*device = dev;

	return 0;

error_spi:
	no_os_spi_remove(dev->spi_desc);
error_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Setup the device.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7978_setup(struct ade7978_dev *dev)
{
	int ret;
	/* data read */
	uint32_t data;

	/* Initialize the current gain registers */
	ret = ade7978_write(dev, ADE7978_REG_AIGAIN, 0x0000);
	if (ret)
		return ret;
	ret = ade7978_write(dev, ADE7978_REG_BIGAIN, 0x0000);
	if (ret)
		return ret;
	ret = ade7978_write(dev, ADE7978_REG_CIGAIN, 0x0000);
	if (ret)
		return ret;
	ret = ade7978_write(dev, ADE7978_REG_NIGAIN, 0x0000);
	if (ret)
		return ret;
	/* Initialize here all the other data memory RAM registers. Write
	the last register in the queue three times to ensure that its
	value is written into the RAM*/
	for (int i = 0; i < 3; i++) {
		ret = ade7978_write(dev, ADE7978_REG_NIGAIN, 0x0000);
		if (ret)
			return ret;
	}

	/* Select frequency 50 Hz */
	ret = ade7978_update_bits(dev, ADE7978_REG_COMPMODE, ADE7978_SELFREQ,
				  no_os_field_prep(ADE7978_SELFREQ, ADE7978_SELFREQ_50));
	if (ret)
		return ret;

	/* Enable DSP RAM protection */
	ret = ade7978_write(dev, ADE7978_RAM_PROTECTION1, ADE7978_RAM_PROT_VAL1);
	if (ret)
		return ret;
	ret = ade7978_write(dev, ADE7978_RAM_PROTECTION2, ADE7978_RAM_PROT_VAL2);
	if (ret)
		return ret;

	/* Select the second voltage channel for temperature measurement
	if temp_en active */
	if (dev->temp_en) {
		ret = ade7978_update_bits(dev, ADE7978_REG_CONFIG3, ADE7978_VA2_EN,
					  no_os_field_prep(ADE7978_VA2_EN, DISABLE));
		if (ret)
			return ret;
		ret = ade7978_update_bits(dev, ADE7978_REG_CONFIG3, ADE7978_VB2_EN,
					  no_os_field_prep(ADE7978_VB2_EN, DISABLE));
		if (ret)
			return ret;
		ret = ade7978_update_bits(dev, ADE7978_REG_CONFIG3, ADE7978_VC2_EN,
					  no_os_field_prep(ADE7978_VC2_EN, DISABLE));
		if (ret)
			return ret;
		ret = ade7978_update_bits(dev, ADE7978_REG_CONFIG3, ADE7978_VN2_EN,
					  no_os_field_prep(ADE7978_VN2_EN, DISABLE));
		if (ret)
			return ret;
	}

	/* Dumb read of energy registers to erase their contents */
	ret = ade7978_read(dev, ADE7978_REG_AWATTHR, &data);
	if (ret)
		return ret;
	ret = ade7978_read(dev, ADE7978_REG_BWATTHR, &data);
	if (ret)
		return ret;
	ret = ade7978_read(dev, ADE7978_REG_CWATTHR, &data);
	if (ret)
		return ret;
	ret = ade7978_read(dev, ADE7978_REG_AVAHR, &data);
	if (ret)
		return ret;
	ret = ade7978_read(dev, ADE7978_REG_BVAHR, &data);
	if (ret)
		return ret;
	ret = ade7978_read(dev, ADE7978_REG_CVAHR, &data);
	if (ret)
		return ret;
	ret = ade7978_read(dev, ADE7978_REG_AFWATTHR, &data);
	if (ret)
		return ret;
	ret = ade7978_read(dev, ADE7978_REG_BFWATTHR, &data);
	if (ret)
		return ret;
	ret = ade7978_read(dev, ADE7978_REG_CFWATTHR, &data);
	if (ret)
		return ret;
	ret = ade7978_read(dev, ADE7978_REG_AFVARHR, &data);
	if (ret)
		return ret;
	ret = ade7978_read(dev, ADE7978_REG_BFVARHR, &data);
	if (ret)
		return ret;
	ret = ade7978_read(dev, ADE7978_REG_CFVARHR, &data);
	if (ret)
		return ret;
	ret = ade7978_read(dev, ADE7978_REG_AVAHR, &data);
	if (ret)
		return ret;
	ret = ade7978_read(dev, ADE7978_REG_BVAHR, &data);
	if (ret)
		return ret;
	ret = ade7978_read(dev, ADE7978_REG_CVAHR, &data);
	if (ret)
		return ret;

	/* Enable pulses at CF1, CF2, CF3 */
	ret = ade7978_update_bits(dev, ADE7978_REG_CFMODE, ADE7978_CF1DIS,
				  no_os_field_prep(ADE7978_CF1DIS, DISABLE));
	if (ret)
		return ret;
	ret = ade7978_update_bits(dev, ADE7978_REG_CFMODE, ADE7978_CF2DIS,
				  no_os_field_prep(ADE7978_CF2DIS, DISABLE));
	if (ret)
		return ret;
	ret = ade7978_update_bits(dev, ADE7978_REG_CFMODE, ADE7978_CF3DIS,
				  no_os_field_prep(ADE7978_CF3DIS, DISABLE));
	if (ret)
		return ret;

	/* Enable the DSP */
	ret = ade7978_write(dev, ADE7978_REG_RUN, ADE7978_RUN_ON);
	if (ret)
		return ret;

	/* Clear reset done flag (int0 output becomes high) */
	ade7978_update_bits(dev, ADE7978_REG_STATUS1, ADE7978_STATUS1_RSTDONE,
			    no_os_field_prep(ADE7978_STATUS1_RSTDONE, ENABLE));
	if (ret)
		return ret;
	/* Enable dready interrupt */
	return ade7978_update_bits(dev, ADE7978_REG_MASK0, ADE7978_STATUS0_DREADY,
				   no_os_field_prep(ADE7978_STATUS0_DREADY, ENABLE));
}

/**
 * @brief Remove the device and release resources.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7978_remove(struct ade7978_dev *dev)
{
	int ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}
