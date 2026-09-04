/***************************************************************************//**
 *   @file   ade9039.c
 *   @brief  Implementation of ADE9039 Driver.
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <stdlib.h>
#include <errno.h>
#include "ade9039.h"
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
int ade9039_read(struct ade9039_dev *dev, uint16_t reg_addr, uint32_t *reg_data)
{
	int ret;
	/* data buffer large enough for 32 bits reg */
	uint8_t buff[8] = { 0 };
	/* register addres */
	uint32_t addr;

	if (!dev)
		return -ENODEV;
	if (!reg_data)
		return -EINVAL;

	addr = (uint16_t) no_os_field_prep(NO_OS_GENMASK(15, 4), reg_addr);
	no_os_put_unaligned_be16(addr, buff);
	buff[1] = buff[1] | ADE9039_SPI_READ;

	/* 16 bits registers */
	if (reg_addr >= ADE9039_REG_RUN && reg_addr <= ADE9039_REG_VERSION) {
		/* for 16 bits reg the buffer is 4 bytes + 2 bytes = 6 bytes if
		CRC of reg data is used */
		ret = no_os_spi_write_and_read(dev->spi_desc, buff, 4);
		if (ret)
			return ret;

		*reg_data = no_os_get_unaligned_be16(&buff[2]);
	} else {
		/* 32 bits registers */
		/* for 32 bits reg the buffer is 6 bytes + 2 bytes = 8 bytes if
		CRC of reg data is used */
		ret = no_os_spi_write_and_read(dev->spi_desc, buff, 6);
		if (ret)
			return ret;

		*reg_data = no_os_get_unaligned_be32(&buff[2]);
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
int ade9039_write(struct ade9039_dev *dev, uint16_t reg_addr, uint32_t reg_data)
{
	/* data buffer */
	uint8_t buff[6] = { 0 };

	if (!dev)
		return -ENODEV;

	buff[0] = reg_addr >> 4;
	buff[1] = reg_addr << 4;

	/* 16 bits registers */
	if (reg_addr >= ADE9039_REG_RUN && reg_addr <= ADE9039_REG_VERSION) {
		no_os_put_unaligned_be16(reg_data, &buff[2]);
		return no_os_spi_write_and_read(dev->spi_desc, buff, 4);
	}

	/* 32 bits registers */
	no_os_put_unaligned_be32(reg_data, &buff[2]);
	return no_os_spi_write_and_read(dev->spi_desc, buff, 6);
}

/**
 * @brief Set power mode
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9039_set_power_mode(struct ade9039_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;
	if (!dev->psm0_desc)
		return -EINVAL;
	if (!dev->psm1_desc)
		return -EINVAL;

	switch (dev->power_mode) {
	case IDLE_MODE:
		ret = no_os_gpio_set_value(dev->psm1_desc,
					   NO_OS_GPIO_HIGH);
		if (ret)
			return ret;
		ret = no_os_gpio_set_value(dev->psm0_desc,
					   NO_OS_GPIO_HIGH);
		break;
	case CURRENT_PEAK_DETECT_MODE:
		ret = no_os_gpio_set_value(dev->psm1_desc,
					   NO_OS_GPIO_HIGH);
		if (ret)
			return ret;
		ret = no_os_gpio_set_value(dev->psm0_desc,
					   NO_OS_GPIO_LOW);
		break;
	case TAMPER_MODE:
		ret = no_os_gpio_set_value(dev->psm1_desc,
					   NO_OS_GPIO_LOW);
		if (ret)
			return ret;
		ret = no_os_gpio_set_value(dev->psm0_desc,
					   NO_OS_GPIO_HIGH);
		break;
	case NORMAL_MODE:
		ret = no_os_gpio_set_value(dev->psm1_desc,
					   NO_OS_GPIO_LOW);
		if (ret)
			return ret;
		ret = no_os_gpio_set_value(dev->psm0_desc,
					   NO_OS_GPIO_LOW);
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
int ade9039_update_bits(struct ade9039_dev *dev, uint16_t reg_addr,
			uint32_t mask, uint32_t reg_data)
{
	int ret;
	/* register value read */
	uint32_t data;

	if (!dev)
		return -ENODEV;

	ret = ade9039_read(dev, reg_addr, &data);
	if (ret)
		return ret;

	data &= ~mask;
	data |= reg_data & mask;

	return ade9039_write(dev, reg_addr, data);
}

/**
 * @brief Get interrupt indicator from STATUS0 register.
 * @param dev - The device structure.
 * @param msk - Interrupt mask.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9039_get_int_status0(struct ade9039_dev *dev, uint32_t msk,
			    uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	ret = ade9039_read(dev, ADE9039_REG_STATUS0, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(msk), &reg_val);

	return 0;
}

/**
 * @brief Read the rms values for a specific phase.
 * @param dev - The device structure.
 * @param phase - ADE9039 Phase.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9039_read_rms_ph(struct ade9039_dev *dev, enum ade9039_phase phase)
{
	int ret;
	/* register value */
	uint32_t temp;

	if (!dev)
		return -ENODEV;

	ret = ade9039_read(dev, irms_regs[phase], &temp);
	if (ret)
		return ret;
	dev->irms_val = temp;

	ret = ade9039_read(dev, vrms_regs[phase], &temp);
	if (ret)
		return ret;
	dev->vrms_val = temp;

	return 0;
}

/**
 * @brief Read the power values for a specific phase.
 * @param dev - The device structure.
 * @param phase - ADE9039 Phase.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9039_read_power_ph(struct ade9039_dev *dev, enum ade9039_phase phase)
{
	int ret;
	/* register value */
	uint32_t temp;

	if (!dev)
		return -ENODEV;
	ret = ade9039_read(dev, watt_regs[phase], &temp);
	if (ret)
		return ret;
	dev->watt_val = temp;

	ret = ade9039_read(dev, va_regs[phase], &temp);
	if (ret)
		return ret;
	dev->va_val = temp;

	ret = ade9039_read(dev, fwatt_regs[phase], &temp);
	if (ret)
		return ret;
	dev->fwatt_val = temp;

	ret = ade9039_read(dev, fvar_regs[phase], &temp);
	if (ret)
		return ret;
	dev->fvar_val = temp;

	ret = ade9039_read(dev, fva_regs[phase], &temp);
	if (ret)
		return ret;
	dev->fva_val = temp;

	return 0;
}

/**
 * @brief Read the energy values for a specific phase.
 * @param dev - The device structure.
 * @param phase - ADE9039 Phase.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9039_read_energy_ph(struct ade9039_dev *dev, enum ade9039_phase phase)
{
	int ret;
	/* register value */
	uint32_t temp;

	if (!dev)
		return -ENODEV;

	ret = ade9039_read(dev, watthr_hi_regs[phase], &temp);
	if (ret)
		return ret;
	dev->watthr_val = temp;

	ret = ade9039_read(dev, varhr_hi_regs[phase], &temp);
	if (ret)
		return ret;
	dev->varhr_val = temp;

	ret = ade9039_read(dev, fwatthr_hi_regs[phase], &temp);
	if (ret)
		return ret;
	dev->fwatthr_val = temp;

	ret = ade9039_read(dev, fvarhr_hi_regs[phase], &temp);
	if (ret)
		return ret;
	dev->fvarhr_val = temp;

	ret = ade9039_read(dev, fvarahr_hi_regs[phase], &temp);
	if (ret)
		return ret;
	dev->fvarahr_val = temp;

	return 0;
}

/**
 * @brief Read the power factor values for a specific phase.
 * @param dev - The device structure.
 * @param phase - ADE9039 Phase.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9039_read_power_factor_ph(struct ade9039_dev *dev,
				 enum ade9039_phase phase)
{
	int ret;
	/* register value */
	uint32_t temp;

	if (!dev)
		return -ENODEV;
	ret = ade9039_read(dev, pf_regs[phase], &temp);
	if (ret)
		return ret;
	dev->pf_val = temp;

	ret = ade9039_read(dev, fpf_regs[phase], &temp);
	if (ret)
		return ret;
	dev->fpf_val = temp;

	return 0;
}

/**
 * @brief Set User Energy use model.
 * @param dev - The device structure.
 * @param model - The User Energy use model to be set.
 * @param value - value specific to the user energy use model, it can be either
 * 		1 for reading with reset, number of half line cycles or number
 * 		of samples.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9039_set_egy_model(struct ade9039_dev *dev, enum ade9039_egy_model model,
			  uint16_t value)
{
	int ret;
	uint16_t egy_ld_accum, egy_tmr_mode, egy_pwr_en;
	uint16_t reg_val;

	if (!dev)
		return -ENODEV;

	switch (model) {
	case ADE9039_EGY_HALF_LINE_CYCLES:
		egy_ld_accum = 1;
		egy_tmr_mode = 1;
		egy_pwr_en = 1;
		break;
	case ADE9039_EGY_NR_SAMPLES:
		egy_ld_accum = 1;
		egy_tmr_mode = 0;
		egy_pwr_en = 1;
		break;
	default:
		return -EINVAL;
	}

	/* stop the DSP if enabled */
	ret = ade9039_read(dev, ADE9039_REG_RUN, &reg_val);
	if (ret)
		return ret;
	if (reg_val) {
		ret = ade9039_write(dev, ADE9039_REG_RUN, 0);
		if (ret)
			return ret;
	}

	ret = ade9039_write(dev, ADE9039_REG_EP_CFG,
			    no_os_field_prep(ADE9039_EGY_LD_ACCUM, egy_ld_accum) |
			    no_os_field_prep(ADE9039_EGY_TMR_MODE, egy_tmr_mode));
	if (ret)
		return ret;

	ret = ade9039_write(dev, ADE9039_REG_EGY_TIME, value);
	if (ret)
		return ret;

	ret = ade9039_write(dev, ADE9039_REG_RUN, ADE9039_RUN_ON);
	if (ret)
		return ret;

	return ade9039_update_bits(dev, ADE9039_REG_EP_CFG, ADE9039_EGY_PWR_EN,
				   no_os_field_prep(ADE9039_EGY_PWR_EN, egy_pwr_en));
}

/**
 * @brief Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9039_init(struct ade9039_dev **device,
		 struct ade9039_init_param init_param)
{
	int ret;
	/* device structure */
	struct ade9039_dev *dev;
	/* chip id read value */
	uint32_t chip_id;
	/* part id read value */
	uint32_t part_id;

	dev = (struct ade9039_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* SPI Initialization */
	ret = no_os_spi_init(&dev->spi_desc, init_param.spi_init);
	if (ret)
		goto error_dev;

	/* Set power mode */
	dev->power_mode = init_param.power_mode;
	if (!init_param.psm0_desc) {
		ret = -EINVAL;
		goto error_spi;
	}
	dev->psm0_desc = init_param.psm0_desc;
	if (!init_param.psm1_desc) {
		ret = -EINVAL;
		goto error_spi;
	}
	dev->psm1_desc = init_param.psm1_desc;

	ret = ade9039_set_power_mode(dev);
	if (ret)
		goto error_spi;

	/* Hard reset the device */
	if (!init_param.reset_desc) {
		ret = -EINVAL;
		goto error_spi;
	}
	dev->reset_desc = init_param.reset_desc;
	ret = no_os_gpio_set_value(dev->reset_desc,
				   NO_OS_GPIO_LOW);
	if (ret)
		goto error_spi;
	// delay reset
	no_os_mdelay(1);
	ret = no_os_gpio_set_value(dev->reset_desc,
				   NO_OS_GPIO_HIGH);
	if (ret)
		goto error_spi;

	// wait for device to initialize after hardware reset
	// >= 100 ms see datasheet.
	no_os_mdelay(ADE9039_RESET_RECOVER);

	ret = ade9039_read(dev, ADE9039_REG_VERSION, &chip_id);
	if (ret)
		goto error_spi;

	ret = ade9039_read(dev, ADE9039_REG_PART_ID, &part_id);
	if (ret)
		goto error_spi;

	part_id = no_os_test_bit(no_os_find_first_set_bit(ADE9039_ADE9039_ID),
				 &part_id);

	if ((chip_id != ADE9039_CHIP_ID) || (part_id != ADE9039_PART_ID)) {
		ret = -EINVAL;
		pr_err("Device ID error \n");
		goto error_spi;
	}

	/* Enable Temperature Sensor */
	ret = ade9039_update_bits(dev, ADE9039_REG_TEMP_CFG, ADE9039_TEMP_EN,
				  no_os_field_prep(ADE9039_TEMP_EN, init_param.temp_en));
	if (ret)
		goto error_spi;

	dev->temp_en = init_param.temp_en;
	dev->freq = init_param.freq;

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
int ade9039_setup(struct ade9039_dev *dev)
{
	int ret;
	uint32_t config = 0;
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;

	/* stop the DSP if enabled */
	ret = ade9039_read(dev, ADE9039_REG_RUN, &reg_val);
	if (ret)
		return ret;
	if (reg_val) {
		ret = ade9039_write(dev, ADE9039_REG_RUN, 0);
		if (ret)
			return ret;
	}
	/* set channels gains */
	ret = ade9039_write(dev, ADE9039_REG_PGA_GAIN, ADE9039_PGA_GAIN);
	if (ret)
		return ret;
	/* Configure the device based on the values in ade9039.h */
	ret = ade9039_write(dev, ADE9039_REG_CONFIG0, ADE9039_CONFIG0);
	if (ret)
		return ret;
	ret = ade9039_write(dev, ADE9039_REG_CONFIG1, ADE9039_CONFIG1);
	if (ret)
		return ret;
	ret = ade9039_write(dev, ADE9039_REG_CONFIG2, ADE9039_CONFIG2);
	if (ret)
		return ret;
	ret = ade9039_write(dev, ADE9039_REG_CONFIG3, ADE9039_CONFIG3);
	if (ret)
		return ret;
	ret = ade9039_write(dev, ADE9039_REG_ACCMODE, ADE9039_ACCMODE);
	if (ret)
		return ret;
	ret = ade9039_write(dev, ADE9039_REG_ZX_LP_SEL, ADE9039_ZX_LP_SEL);
	if (ret)
		return ret;
	ret = ade9039_write(dev, ADE9039_REG_MASK1, ADE9039_MASK1);
	if (ret)
		return ret;
	ret = ade9039_write(dev, ADE9039_REG_EVENT_MASK, ADE9039_EVENT_MASK);
	if (ret)
		return ret;
	ret = ade9039_write(dev, ADE9039_REG_WFB_CFG, ADE9039_WFB_CFG);
	if (ret)
		return ret;
	ret = ade9039_write(dev, ADE9039_REG_DICOEFF, ADE9039_DICOEFF);
	if (ret)
		return ret;
	/* Configure VLEVEL for headroom above nominal voltage */
	config = 2 * ADE9039_VLEVEL;
	ret = ade9039_write(dev, ADE9039_REG_VLEVEL, config);
	if (ret)
		return ret;
	/* Enable energy comparison for all 3 phases */
	ret = ade9039_write(dev, ADE9039_REG_COMPMODE, ADE9039_COMPMODE);
	if (ret)
		return ret;
	/* Energy accumulation time: 100 half-cycles at 50Hz, 120 at 60Hz = ~1 second */
	config = (dev->freq == 50) ? 99 : 119;
	/* Energy/power accumulation enable */
	return ade9039_set_egy_model(dev, ADE9039_EGY_HALF_LINE_CYCLES, config);
}

/**
 * @brief Read the ADE9039 temperature using the conversion ready status.
 * @param dev - The ADE9039 device structure.
 * @return 0 in case of success, negative error code otherwise.
*/
int ade9039_read_temp(struct ade9039_dev *dev)
{
	int ret;
	/* raw temperature value */
	uint32_t temp_raw;
	/* register value */
	uint32_t temp;
	/* temperature gain */
	uint32_t gain;
	/* temperature offset */
	uint32_t offset;

	if (!dev)
		return -ENODEV;

	/* compute the temperature */
	ret = ade9039_read(dev, ADE9039_REG_TEMP_RSLT, &temp_raw);
	if (ret)
		return ret;

	temp_raw = no_os_field_get(ADE9039_TEMP_RSLT, temp_raw);

	ret = ade9039_read(dev, ADE9039_REG_TEMP_TRIM, &temp);
	if (ret)
		return ret;

	gain = no_os_field_get(ADE9039_TEMP_GAIN, temp);
	offset = no_os_field_get(ADE9039_TEMP_OFFSET, temp);

	dev->temp_raw = temp_raw;
	dev->temp_offset = offset;
	dev->temp_gain = gain;

	return 0;
}

/**
 * @brief Remove the device and release resources.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9039_remove(struct ade9039_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;
	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}

/**
 * @brief Check bit status of a register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param bit_mask - The bit mask to check.
 * @param status - The status of the bit (1 if set, 0 if not).
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9039_check_bit_status(struct ade9039_dev *dev, uint16_t reg_addr,
			     uint32_t bit_mask, uint8_t *status)
{
	int ret;
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	ret = ade9039_read(dev, reg_addr, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(bit_mask), &reg_val);

	return 0;
}
