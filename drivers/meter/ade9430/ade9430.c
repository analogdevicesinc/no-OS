/***************************************************************************//**
 *   @file   ade9430.c
 *   @brief  Implementation of ADE9430 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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
#include "ade9430.h"
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
int ade9430_read(struct ade9430_dev *dev, uint16_t reg_addr, uint32_t *reg_data)
{
	int ret;
	uint8_t buff[6] = {0};

	buff[0] = reg_addr >> 4;
	buff[1] = ADE9430_SPI_READ | reg_addr << 4;

	if (reg_addr >= ADE9430_REG_RUN && reg_addr <= ADE9430_REG_VERSION) {
		ret = no_os_spi_write_and_read(dev->spi_desc, buff, 4);
		if (ret)
			return ret;

		*reg_data = no_os_get_unaligned_be16(&buff[2]);
	} else {
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
int ade9430_write(struct ade9430_dev *dev, uint16_t reg_addr, uint32_t reg_data)
{
	uint8_t buff[6] = {0};

	buff[0] = reg_addr >> 4;
	buff[1] = reg_addr << 4;

	if (reg_addr >= ADE9430_REG_RUN && reg_addr <= ADE9430_REG_VERSION) {
		no_os_put_unaligned_be16(reg_data, &buff[2]);
		return no_os_spi_write_and_read(dev->spi_desc, buff, 4);
	}

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
int ade9430_update_bits(struct ade9430_dev *dev, uint16_t reg_addr,
			uint32_t mask, uint32_t reg_data)
{
	int ret;
	uint32_t data;

	ret = ade9430_read(dev, reg_addr, &data);
	if (ret)
		return ret;

	data &= ~mask;
	data |= reg_data & mask;

	return ade9430_write(dev, reg_addr, data);
}

/**
 * @brief Read the temperature.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9430_read_temp(struct ade9430_dev *dev)
{
	int ret;
	uint32_t temp_raw, temp, gain, offset;

	ret = ade9430_update_bits(dev, ADE9430_REG_TEMP_CFG, ADE9430_TEMP_START,
				  no_os_field_prep(ADE9430_TEMP_START, 1));
	if (ret)
		return ret;

	/* New temperature reading available after 1.25ms */
	no_os_mdelay(2);

	ret = ade9430_read(dev, ADE9430_REG_TEMP_RSLT, &temp_raw);
	if (ret)
		return ret;

	temp_raw = no_os_field_get(ADE9430_TEMP_RESULT, temp_raw);

	ret = ade9430_read(dev, ADE9430_REG_TEMP_TRIM, &temp);
	if (ret)
		return ret;

	gain = no_os_field_get(ADE9430_TEMP_GAIN, temp);
	offset = no_os_field_get(ADE9430_TEMP_OFFSET, temp);

	dev->temp_deg = temp_raw * ((-1000) * (int64_t)gain / 65536) /
			1000 + (offset / 32);

	return 0;
}

/**
 * @brief Read the power/energy for specific phase.
 * @param dev - The device structure.
 * @param phase - ADE9430 Phase.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9430_read_data_ph(struct ade9430_dev *dev, enum ade9430_phase phase)
{
	int ret;
	uint32_t temp;
	uint16_t irms_reg, vrms_reg, watt_reg;

	switch (phase) {
	case ADE9430_PHASE_A:
		irms_reg = ADE9430_REG_AIRMS;
		vrms_reg = ADE9430_REG_AVRMS;
		watt_reg = ADE9430_REG_AWATT;
		break;
	case ADE9430_PHASE_B:
		irms_reg = ADE9430_REG_BIRMS;
		vrms_reg = ADE9430_REG_BVRMS;
		watt_reg = ADE9430_REG_BWATT;
		break;
	case ADE9430_PHASE_C:
		irms_reg = ADE9430_REG_CIRMS;
		vrms_reg = ADE9430_REG_CVRMS;
		watt_reg = ADE9430_REG_CWATT;
		break;
	default:
		return -EINVAL;
	}

	ret = ade9430_read(dev, irms_reg, &temp);
	if (ret)
		return ret;

	dev->irms_val = temp * ADE9430_I_RES_NA / NANOAMPER_PER_AMPER;

	ret = ade9430_read(dev, vrms_reg, &temp);
	if (ret)
		return ret;

	dev->vrms_val = temp * ADE9430_V_RES_NV / NANOVOLT_PER_VOLT;

	ret = ade9430_read(dev, watt_reg, &temp);
	if (ret)
		return ret;

	dev->watt_val = temp * ADE9430_W_RES_UW / MICROWATT_PER_WATT;

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
int ade9430_set_egy_model(struct ade9430_dev *dev, enum ade9430_egy_model model,
			  uint16_t value)
{
	int ret;
	uint16_t egy_ld_accum, egy_tmr_mode, rd_rst_en, egy_pwr_en;

	switch (model) {
	case ADE9430_EGY_WITH_RESET:
		/* For reading energy with reset the value should be 1 */
		if (value != 1)
			return -EINVAL;
		egy_ld_accum = 0;
		egy_tmr_mode = 0;
		rd_rst_en = 1;
		egy_pwr_en = 1;
		break;
	case ADE9430_EGY_HALF_LINE_CYCLES:
		egy_ld_accum = 1;
		egy_tmr_mode = 1;
		rd_rst_en = 0;
		egy_pwr_en = 1;
		break;
	case ADE9430_EGY_NR_SAMPLES:
		egy_ld_accum = 1;
		egy_tmr_mode = 0;
		rd_rst_en = 0;
		egy_pwr_en = 1;
		break;
	default:
		return -EINVAL;
	}

	ret = ade9430_write(dev, ADE9430_REG_RUN, 0);
	if (ret)
		return ret;

	ret = ade9430_write(dev, ADE9430_REG_EP_CFG,
			    no_os_field_prep(ADE9430_EGY_LD_ACCUM, egy_ld_accum) |
			    no_os_field_prep(ADE9430_EGY_TMR_MODE, egy_tmr_mode) |
			    no_os_field_prep(ADE9430_RD_RST_EN, rd_rst_en) |
			    no_os_field_prep(ADE9430_EGY_PWR_EN, egy_pwr_en));
	if (ret)
		return ret;

	ret = ade9430_write(dev, ADE9430_REG_EGY_TIME, value);
	if (ret)
		return ret;

	return ade9430_write(dev, ADE9430_REG_RUN, 1);
}

/**
 * @brief Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9430_init(struct ade9430_dev **device,
		 struct ade9430_init_param init_param)
{
	struct ade9430_dev *dev;
	uint32_t chip_id;
	int ret;

	dev = (struct ade9430_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* SPI Initialization*/
	ret = no_os_spi_init(&dev->spi_desc, init_param.spi_init);
	if (ret)
		goto error_dev;

	ret = ade9430_update_bits(dev, ADE9430_REG_CONFIG1, ADE9430_SWRST,
				  no_os_field_prep(ADE9430_SWRST, 1));
	if (ret)
		goto error_spi;

	no_os_mdelay(100);

	ret = ade9430_update_bits(dev, ADE9430_REG_CONFIG1, ADE9430_SWRST,
				  no_os_field_prep(ADE9430_SWRST, 0));
	if (ret)
		goto error_spi;

	/** Use a valid register with default value different from 0*/
	ret = ade9430_read(dev, ADE9430_REG_CONFIG5, &chip_id);
	if (ret)
		goto error_spi;

	if (chip_id != ADE9430_CHIP_ID)
		goto error_spi;

	/* Enable Temperature Sensor */
	ret = ade9430_update_bits(dev, ADE9430_REG_TEMP_CFG, ADE9430_TEMP_EN,
				  no_os_field_prep(ADE9430_TEMP_EN, init_param.temp_en));
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
 * @brief Remove the device and release resources.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade9430_remove(struct ade9430_dev *dev)
{
	int ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}
