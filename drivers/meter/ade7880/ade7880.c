/***************************************************************************//**
 *   @file   ade7880.c
 *   @brief  Implementation of ADE7880 Driver.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include <errno.h>
#include "ade7880.h"
#include "no_os_delay.h"
#include "no_os_units.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Read device register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data read from the register.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7880_read(struct ade7880_dev *dev, uint16_t reg_addr, uint32_t *reg_data)
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

	buff[0] = ADE7880_SPI_READ_CMD;
	no_os_put_unaligned_be16(reg_addr, &buff[1]);

	/* 8 bits registers */
	if ((reg_addr >= ADE7880_REG_MMODE && reg_addr <= ADE7880_REG_LAST_RWDATA8) ||
	    (reg_addr >= ADE7880_REG_CONFIG3 && reg_addr <= ADE7880_REG_CONFIG2)) {
		ret = no_os_spi_write_and_read(dev->spi_desc, buff, 4);
		if (ret)
			return ret;

		*reg_data = buff[3];
	}
	/* 16 bits registers */
	else if ((reg_addr >= ADE7880_REG_PHSTATUS && reg_addr <= ADE7880_REG_CONFIG) ||
		 (reg_addr >= ADE7880_REG_HCONFIG && reg_addr <= ADE7880_REG_LAST_RWDATA16) ||
		 (reg_addr == ADE7880_REG_RUN)) {
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
int ade7880_write(struct ade7880_dev *dev, uint16_t reg_addr, uint32_t reg_data)
{
	/* data buffer */
	uint8_t buff[7] = { 0 };
	int i;

	if (!dev)
		return -ENODEV;
	// write operation
	buff[0] = ADE7880_SPI_WRITE_CMD;
	no_os_put_unaligned_be16(reg_addr, &buff[1]);

	/* 8 bits registers */
	if ((reg_addr >= ADE7880_REG_MMODE && reg_addr <= ADE7880_REG_LAST_RWDATA8) ||
	    (reg_addr >= ADE7880_REG_CONFIG3 && reg_addr <= ADE7880_REG_CONFIG2)) {
		buff[3] = (uint8_t)reg_data;

		return no_os_spi_write_and_read(dev->spi_desc, buff, 4);
	}

	/* 16 bits registers */
	if ((reg_addr >= ADE7880_REG_PHSTATUS && reg_addr <= ADE7880_REG_CONFIG) ||
	    (reg_addr >= ADE7880_REG_HCONFIG && reg_addr <= ADE7880_REG_LAST_RWDATA16) ||
	    (reg_addr == ADE7880_REG_RUN)) {
		no_os_put_unaligned_be16(reg_data, &buff[3]);

		return no_os_spi_write_and_read(dev->spi_desc, buff, 5);
	}

	/* 32 bits registers */
	no_os_put_unaligned_be32(reg_data, &buff[3]);

	return no_os_spi_write_and_read(dev->spi_desc, buff, 7);
}

/**
 * @brief Set power mode
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7880_set_power_mode(struct ade7880_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;
	if (!dev->psm0_desc)
		return -EINVAL;
	if (!dev->psm1_desc)
		return -EINVAL;

	switch (dev->power_mode) {
	case SLEEP_MODE:
		ret = no_os_gpio_set_value(dev->psm1_desc,
					   NO_OS_GPIO_HIGH);
		if (ret)
			return ret;
		ret = no_os_gpio_set_value(dev->psm0_desc,
					   NO_OS_GPIO_HIGH);
		break;
	case LOW_POWER_MODE:
		ret = no_os_gpio_set_value(dev->psm1_desc,
					   NO_OS_GPIO_HIGH);
		if (ret)
			return ret;
		ret = no_os_gpio_set_value(dev->psm0_desc,
					   NO_OS_GPIO_LOW);
		break;
	case REDUCED_POWER_MODE:
		ret = no_os_gpio_set_value(dev->psm1_desc,
					   NO_OS_GPIO_LOW);
		if (ret)
			return ret;
		ret = no_os_gpio_set_value(dev->psm0_desc,
					   NO_OS_GPIO_LOW);
		break;
	case NORMAL_MODE:
		ret = no_os_gpio_set_value(dev->psm1_desc,
					   NO_OS_GPIO_LOW);
		if (ret)
			return ret;
		ret = no_os_gpio_set_value(dev->psm0_desc,
					   NO_OS_GPIO_HIGH);
		break;
	default:
		return -EINVAL;
	}

	return ret;
}

/**
 * @brief Update specific register bits.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - Specific bits mask.
 * @param reg_data - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7880_update_bits(struct ade7880_dev *dev, uint16_t reg_addr,
			uint32_t mask, uint32_t reg_data)
{
	int ret;
	/* register value read */
	uint32_t data;

	if (!dev)
		return -ENODEV;

	ret = ade7880_read(dev, reg_addr, &data);
	if (ret)
		return ret;

	data &= ~mask;
	data |= reg_data & mask;

	return ade7880_write(dev, reg_addr, data);
}

/**
 * @brief Get interrupt indicator from STATUS0 register.
 * @param dev - The device structure.
 * @param msk - Interrupt mask.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7880_get_int_status0(struct ade7880_dev *dev, uint32_t msk,
			    uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	ret = ade7880_read(dev, ADE7880_REG_STATUS0, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(msk), &reg_val);

	return 0;
}

/**
 * @brief Read the power/energy for specific phase.
 * @param dev - The device structure.
 * @param phase - ADE7880 Phase.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7880_read_data_ph(struct ade7880_dev *dev, enum ade7880_phase phase)
{
	int ret;
	/* register value */
	uint32_t temp;
	/* i rms phase register addr */
	uint16_t irms_reg;
	/* v rms phase register addr */
	uint16_t vrms_reg;
	/* intermediate values used for computation */
	uint64_t val_int;

	if (!dev)
		return -ENODEV;

	/* select the phase for values read */
	switch (phase) {
	case ADE7880_PHASE_A:
		irms_reg = ADE7880_REG_AIRMS;
		vrms_reg = ADE7880_REG_AVRMS;
		break;
	case ADE7880_PHASE_B:
		irms_reg = ADE7880_REG_BIRMS;
		vrms_reg = ADE7880_REG_BVRMS;
		break;
	case ADE7880_PHASE_C:
		irms_reg = ADE7880_REG_CIRMS;
		vrms_reg = ADE7880_REG_CVRMS;
		break;
	default:
		return -EINVAL;
	}

	ret = ade7880_read(dev, irms_reg, &temp);
	if (ret)
		return ret;

	// Value in mA
	dev->irms_val = (uint32_t)(uint64_t)(((uint64_t) temp *
					      (uint64_t) ADE7880_FS_VOLTAGE
					      * (uint64_t) ADE7880_CURRENT_TR_FCN) / ((uint64_t)ADE7880_RMS_FS_CODES * 10));

	ret = ade7880_read(dev, vrms_reg, &temp);
	if (ret)
		return ret;

	// Value in mV
	dev->vrms_val = (uint32_t)(uint64_t)(((uint64_t) temp *
					      (uint64_t) ADE7880_FS_VOLTAGE
					      * (uint64_t) ADE7880_VOLTAGE_TR_FCN) / ((uint64_t)ADE7880_RMS_FS_CODES * 10));

	return 0;
}

/**
 * @brief Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7880_init(struct ade7880_dev **device,
		 struct ade7880_init_param init_param)
{
	int ret;
	/* device structure */
	struct ade7880_dev *dev;
	/* chip id read value */
	uint32_t chip_id;
	/* part id read value */
	uint32_t part_id;

	dev = (struct ade7880_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* Set power mode */
	dev->power_mode = init_param.power_mode;
	if (!init_param.psm0_desc)
		goto error_dev;
	dev->psm0_desc = init_param.psm0_desc;
	if (!init_param.psm1_desc)
		goto error_dev;
	dev->psm1_desc = init_param.psm1_desc;

	ret = ade7880_set_power_mode(dev);
	if (ret)
		goto error_dev;

	/* Hard reset the device */
	if (!init_param.reset_desc)
		goto error_dev;
	dev->reset_desc = init_param.reset_desc;
	ret = no_os_gpio_set_value(dev->reset_desc,
				   NO_OS_GPIO_LOW);
	if (ret)
		goto error_dev;
	// delay reset
	no_os_mdelay(1);
	ret = no_os_gpio_set_value(dev->reset_desc,
				   NO_OS_GPIO_HIGH);
	if (ret)
		goto error_dev;

	// wait for device to initialize after hardware reset
	// >= 100 ms see datasheet.
	no_os_mdelay(ADE7880_RESET_RECOVER);

	/* SPI Initialization */
	ret = no_os_spi_init(&dev->spi_desc, init_param.spi_init);
	if (ret)
		goto error_dev;

	// Select the SPI as the serial interface
	//Execute three SPI writes to an unallocated address space -> see datasheet.
	for (int i = 0; i < 3; i++) {
		ret = ade7880_write(dev, ADE7880_SET_SPI_ADDR, ADE7880_DUMB_VAL);
	}

	// Lock the SPI port as active
	ret = ade7880_update_bits(dev, ADE7880_REG_CONFIG2, ADE7880_I2C_LOCK, ENABLE);

	/* Use a valid register with default value different from 0 */
	ret = ade7880_read(dev, ADE7880_REG_CFMODE, &chip_id);
	if (ret)
		goto error_spi;

	if (chip_id != ADE7880_CHIP_ID) {
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
int ade7880_setup(struct ade7880_dev *dev)
{
	int ret;

	if (!dev)
		return -ENOMEM;

	/* set channels gains */
	ret = ade7880_update_bits(dev, ADE7880_REG_GAIN, ADE7880_PGA1,
				  ADE7880_PGAGAIN_1);
	if (ret)
		return ret;
	ret = ade7880_update_bits(dev, ADE7880_REG_GAIN, ADE7880_PGA2,
				  ADE7880_PGAGAIN_1);
	if (ret)
		return ret;
	ret = ade7880_update_bits(dev, ADE7880_REG_GAIN, ADE7880_PGA3,
				  ADE7880_PGAGAIN_1);
	if (ret)
		return ret;

	ret = ade7880_write(dev, ADE7880_REG_VLEVEL, ADE7880_VLEVEL_VAL);
	if (ret)
		return ret;

	/* Select frequency 50 Hz */
	ret = ade7880_update_bits(dev, ADE7880_REG_COMPMODE, ADE7880_SELFREQ,
				  ADE7880_SELFREQ_50);
	if (ret)
		return ret;

	/* Initialize here all the other data memory RAM registers. Write
	the last register in the queue three times to ensure that its
	value is written into the RAM*/
	for (int i = 0; i < 3; i++) {
		ret = ade7880_write(dev, ADE7880_REG_DICOEFF, ADE7880_DICOEFF);
		if (ret)
			return ret;
	}

	/* Enable DSP RAM protection */
	ret = ade7880_write(dev, ADE7880_RAM_PROTECTION1, ADE7880_RAM_PROT_VAL1);
	if (ret)
		return ret;
	ret = ade7880_write(dev, ADE7880_RAM_PROTECTION2, ADE7880_RAM_PROT_VAL2);
	if (ret)
		return ret;

	/* Enable the DSP */
	return ade7880_write(dev, ADE7880_REG_RUN, ADE7880_RUN_ON);
}

/**
 * @brief Remove the device and release resources.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7880_remove(struct ade7880_dev *dev)
{
	int ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}
