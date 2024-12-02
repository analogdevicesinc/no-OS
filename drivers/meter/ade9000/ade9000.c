/***************************************************************************//**
 *   @file   ade9000.c
 *   @brief  Implementation of ADE9000 Driver.
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
#include "ade9000.h"
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
int ade9000_read(struct ade9000_dev *dev, uint16_t reg_addr, uint32_t *reg_data)
{
	int ret;
	/* index */
	int i;
	/* data buffer large enough for 32 bits reg */
	uint8_t buff[6] = { 0 };
	/* register addres */
	uint32_t addr;

	if (!dev)
		return -ENODEV;
	if (!reg_data)
		return -EINVAL;

	addr = (uint16_t) no_os_field_prep(NO_OS_GENMASK(16, 4), reg_addr);
	no_os_put_unaligned_be16(addr, &buff);
	buff[1] = buff[1] | ADE9000_SPI_READ;

	/* 16 bits registers */
	if (reg_addr >= ADE9000_REG_RUN && reg_addr <= ADE9000_REG_VERSION) {
		ret = no_os_spi_write_and_read(dev->spi_desc, buff, 4);
		if (ret)
			return ret;

		*reg_data = no_os_get_unaligned_be16(&buff[2]);
	} else {
		/* 32 bits registers */
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
int ade9000_write(struct ade9000_dev *dev, uint16_t reg_addr, uint32_t reg_data)
{
	/* data buffer */
	uint8_t buff[6] = { 0 };

	if (!dev)
		return -ENODEV;

	buff[0] = reg_addr >> 4;
	buff[1] = reg_addr << 4;

	/* 16 bits registers */
	if (reg_addr >= ADE9000_REG_RUN && reg_addr <= ADE9000_REG_VERSION) {
		no_os_put_unaligned_be16(reg_data, &buff[2]);
		return no_os_spi_write_and_read(dev->spi_desc, buff, 4);
	}

	/* 32 bits registers */
	no_os_put_unaligned_be32(reg_data, &buff[2]);
	return no_os_spi_write_and_read(dev->spi_desc, buff, 6);
}

/**
 * @brief Update specific register bits.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - Specific bits mask.
 * @param reg_data - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9000_update_bits(struct ade9000_dev *dev, uint16_t reg_addr,
			uint32_t mask, uint32_t reg_data)
{
	int ret;
	/* register value read */
	uint32_t data;

	if (!dev)
		return -ENODEV;

	ret = ade9000_read(dev, reg_addr, &data);
	if (ret)
		return ret;

	data &= ~mask;
	data |= reg_data & mask;

	return ade9000_write(dev, reg_addr, data);
}

/**
 * @brief Get interrupt indicator from STATUS0 register.
 * @param dev - The device structure.
 * @param msk - Interrupt mask.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9000_get_int_status0(struct ade9000_dev *dev, uint32_t msk,
			    uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!dev)
		return -ENODEV;
	if (!status)
		return -EINVAL;

	ret = ade9000_read(dev, ADE9000_REG_STATUS0, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(msk), &reg_val);

	return 0;
}

/**
 * @brief Read the temperature.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9000_read_temp(struct ade9000_dev *dev)
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
	/* status of temperature ready */
	uint8_t *status = 0;
	/* timeout for temperature read */
	uint16_t timeout = 0;

	if (!dev)
		return -ENODEV;

	ret = ade9000_update_bits(dev, ADE9000_REG_TEMP_CFG, ADE9000_TEMP_START,
				  no_os_field_prep(ADE9000_TEMP_START, 1));
	if (ret)
		return ret;

	// wait for conversion result
	while (!(status)) {
		ret = ade9000_get_int_status0(dev, ADE9000_MASK0_TEMP_RDY, &status);
		no_os_mdelay(2);
		timeout++;
		if (timeout == 2000) {
			ret = -ENODATA;
			return ret;
		}
	}

	// if conversion succeded compute temperature
	ret = ade9000_read(dev, ADE9000_REG_TEMP_RSLT, &temp_raw);
	if (ret)
		return ret;

	temp_raw = no_os_field_get(ADE9000_TEMP_RESULT, temp_raw);

	ret = ade9000_read(dev, ADE9000_REG_TEMP_TRIM, &temp);
	if (ret)
		return ret;

	gain = no_os_field_get(ADE9000_TEMP_GAIN, temp);
	offset = no_os_field_get(ADE9000_TEMP_OFFSET, temp);

	/* Temperature (°C) = TEMP_RSLT × (−TEMP_GAIN/65536) + (TEMP_OFFSET/32) */
	dev->temp_deg = (offset / (1 << 5)) - (temp_raw * (1000
					       * (int64_t)gain / 65536) / 1000);

	return 0;
}

/**
 * @brief Read the power/energy for specific phase.
 * @param dev - The device structure.
 * @param phase - ADE9000 Phase.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9000_read_data_ph(struct ade9000_dev *dev, enum ade9000_phase phase)
{
	int ret;
	/* register value */
	uint32_t temp;
	/* i rms phase register addr */
	uint16_t irms_reg;
	/* v rms phase register addr */
	uint16_t vrms_reg;
	/* power phase register addr */
	uint16_t watt_reg;
	/* intermediate values used for computation */
	uint64_t val_int;

	if (!dev)
		return -ENODEV;

	/* select the phase for values read */
	switch (phase) {
	case ADE9000_PHASE_A:
		irms_reg = ADE9000_REG_AIRMS;
		vrms_reg = ADE9000_REG_AVRMS;
		watt_reg = ADE9000_REG_AWATT;
		break;
	case ADE9000_PHASE_B:
		irms_reg = ADE9000_REG_BIRMS;
		vrms_reg = ADE9000_REG_BVRMS;
		watt_reg = ADE9000_REG_BWATT;
		break;
	case ADE9000_PHASE_C:
		irms_reg = ADE9000_REG_CIRMS;
		vrms_reg = ADE9000_REG_CVRMS;
		watt_reg = ADE9000_REG_CWATT;
		break;
	default:
		return -EINVAL;
	}

	ret = ade9000_read(dev, irms_reg, &temp);
	if (ret)
		return ret;

	// Value in mA
	dev->irms_val = (uint32_t)((uint64_t)(((uint64_t) temp * ADE9000_FS_VOLTAGE
					       * ADE9000_CURRENT_TR_FCN) / (uint64_t)ADE9000_RMS_FS_CODES));

	ret = ade9000_read(dev, vrms_reg, &temp);
	if (ret)
		return ret;

	// Value in mV
	dev->vrms_val = (uint32_t)((uint64_t)(((uint64_t) temp * ADE9000_FS_VOLTAGE
					       * ADE9000_VOLTAGE_TR_FCN) / (uint64_t)ADE9000_RMS_FS_CODES));

	ret = ade9000_read(dev, watt_reg, &temp);
	if (ret)
		return ret;

	val_int = ADE9000_FS_VOLTAGE * (ADE9000_CURRENT_TR_FCN / 100)
		  * (ADE9000_FS_VOLTAGE) * (ADE9000_VOLTAGE_TR_FCN / 10);
	val_int = ((uint64_t)((uint64_t)temp * (uint64_t)val_int)) /
		  (uint64_t)ADE9000_WATT_FS_CODES;
	// Value in mW
	dev->watt_val = (uint32_t)val_int;

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
int ade9000_set_egy_model(struct ade9000_dev *dev, enum ade9000_egy_model model,
			  uint16_t value)
{
	int ret;
	uint16_t egy_ld_accum, egy_tmr_mode, rd_rst_en, egy_pwr_en;

	if (!dev)
		return -ENODEV;

	switch (model) {
	case ADE9000_EGY_WITH_RESET:
		/* For reading energy with reset the value should be 1 */
		if (value != 1)
			return -EINVAL;
		egy_ld_accum = 0;
		egy_tmr_mode = 0;
		rd_rst_en = 1;
		egy_pwr_en = 1;
		break;
	case ADE9000_EGY_HALF_LINE_CYCLES:
		egy_ld_accum = 1;
		egy_tmr_mode = 1;
		rd_rst_en = 0;
		egy_pwr_en = 1;
		break;
	case ADE9000_EGY_NR_SAMPLES:
		egy_ld_accum = 1;
		egy_tmr_mode = 0;
		rd_rst_en = 0;
		egy_pwr_en = 1;
		break;
	default:
		return -EINVAL;
	}

	ret = ade9000_write(dev, ADE9000_REG_RUN, 0);
	if (ret)
		return ret;

	ret = ade9000_write(dev, ADE9000_REG_EP_CFG,
			    no_os_field_prep(ADE9000_EGY_LD_ACCUM, egy_ld_accum) |
			    no_os_field_prep(ADE9000_EGY_TMR_MODE, egy_tmr_mode) |
			    no_os_field_prep(ADE9000_RD_RST_EN, rd_rst_en) |
			    no_os_field_prep(ADE9000_EGY_PWR_EN, egy_pwr_en));
	if (ret)
		return ret;

	ret = ade9000_write(dev, ADE9000_REG_EGY_TIME, value);
	if (ret)
		return ret;

	return ade9000_write(dev, ADE9000_REG_RUN, 1);
}

/**
 * @brief Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9000_init(struct ade9000_dev **device,
		 struct ade9000_init_param init_param)
{
	int ret;
	/* device structure */
	struct ade9000_dev *dev;
	/* chip id read value */
	uint32_t chip_id;

	dev = (struct ade9000_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* SPI Initialization*/
	ret = no_os_spi_init(&dev->spi_desc, init_param.spi_init);
	if (ret)
		goto error_dev;

	ret = ade9000_update_bits(dev, ADE9000_REG_CONFIG1, ADE9000_SWRST,
				  no_os_field_prep(ADE9000_SWRST, 1));
	if (ret)
		goto error_spi;

	// wait for device to initialize after software reset
	// > 46 ms see datasheet.
	no_os_mdelay(50);

	/* Use a valid register with default value different from 0*/
	ret = ade9000_read(dev, ADE9000_REG_CONFIG5, &chip_id);
	if (ret)
		goto error_spi;

	if (chip_id != ADE9000_CHIP_ID)
		goto error_spi;

	/* Enable Temperature Sensor */
	ret = ade9000_update_bits(dev, ADE9000_REG_TEMP_CFG, ADE9000_TEMP_EN,
				  no_os_field_prep(ADE9000_TEMP_EN, init_param.temp_en));
	if (ret)
		goto error_spi;

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
int ade9000_setup(struct ade9000_dev *dev)
{
	int ret;

	if (!dev)
		return -ENOMEM;

	/* set channels gains */
	ret = ade9000_write(dev, ADE9000_REG_PGA_GAIN, ADE9000_PGA_GAIN);
	if (ret)
		return ret;
	/* Configure the device based on the values in ade9000.h */
	ret = ade9000_write(dev, ADE9000_REG_CONFIG0, ADE9000_CONFIG0);
	if (ret)
		return ret;
	ret = ade9000_write(dev, ADE9000_REG_CONFIG1, ADE9000_CONFIG1);
	if (ret)
		return ret;
	ret = ade9000_write(dev, ADE9000_REG_CONFIG2, ADE9000_CONFIG2);
	if (ret)
		return ret;
	ret = ade9000_write(dev, ADE9000_REG_CONFIG3, ADE9000_CONFIG3);
	if (ret)
		return ret;
	ret = ade9000_write(dev, ADE9000_REG_ACCMODE, ADE9000_ACCMODE);
	if (ret)
		return ret;
	ret = ade9000_write(dev, ADE9000_REG_ZX_LP_SEL, ADE9000_ZX_LP_SEL);
	if (ret)
		return ret;
	/* Enable temperature ready interrupt */
	ret = ade9000_update_bits(dev, ADE9000_REG_MASK0, ADE9000_MASK0_TEMP_RDY,
				  no_os_field_prep(ADE9000_MASK0_TEMP_RDY, 1));
	if (ret)
		return ret;
	ret = ade9000_write(dev, ADE9000_REG_MASK1, ADE9000_MASK1);
	if (ret)
		return ret;
	ret = ade9000_write(dev, ADE9000_REG_EVENT_MASK, ADE9000_EVENT_MASK);
	if (ret)
		return ret;
	ret = ade9000_write(dev, ADE9000_REG_WFB_CFG, ADE9000_WFB_CFG);
	if (ret)
		return ret;
	ret = ade9000_write(dev, ADE9000_REG_VLEVEL, ADE9000_VLEVEL);
	if (ret)
		return ret;
	ret = ade9000_write(dev, ADE9000_REG_DICOEFF, ADE9000_DICOEFF);
	if (ret)
		return ret;
	ret = ade9000_write(dev, ADE9000_REG_EGY_TIME, ADE9000_EGY_TIME);
	if (ret)
		return ret;
	ret = ade9000_write(dev, ADE9000_REG_EP_CFG, ADE9000_EP_CFG);
	if (ret)
		return ret;
	/* Enable the DSP */
	return ade9000_write(dev, ADE9000_REG_RUN, ADE9000_RUN_ON);
}

/**
 * @brief Remove the device and release resources.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9000_remove(struct ade9000_dev *dev)
{
	int ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}
