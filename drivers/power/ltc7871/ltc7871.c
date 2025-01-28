/***************************************************************************//**
 *   @file   ltc7871.c
 *   @brief  Source file for the LTC7871 Driver
 *   @author Aldrin Abacan (aldrin.abacan@analog.com)
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
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_print_log.h"
#include "ltc7871.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Compute the PEC.
 * @param data - Data buffer.
 * @param len - Buffer length.
 * @return 8-bit CRC value
 */
uint8_t ltc7871_get_pec_byte(uint8_t *data, uint8_t len)
{
	uint8_t pec = 0x41; // Initial PEC value

	for (int8_t idx = 0 ; idx < len; idx++) {
		uint8_t byte = data[idx];

		for (int bit = 7; bit >= 0; bit--) {
			uint8_t din = (byte >> bit) & 0x01;
			uint8_t in0 = din ^ ((pec >> 7) & 0x01);
			uint8_t in1 = ((pec >> 0) & 0x01) ^ in0;
			uint8_t in2 = ((pec >> 1) & 0x01) ^ in0;

			pec = (pec << 1) & 0xF8; // Shift left and clear LSB
			if (in2) pec |= 0x04; // Set bit 2
			if (in1) pec |= 0x02; // Set bit 1
			if (in0) pec |= 0x01; // Set bit 0
		}
	}

	return pec;
}
/**
 * @brief Register Read
 * @param dev - The device structure
 * @param reg - The register value
 * @param data - The data read from the device.
 * @return 0 in case of success, negative code otherwise
 */
int ltc7871_reg_read(struct ltc7871_dev *dev, uint8_t reg, uint8_t *data)
{
	int ret;
	uint8_t r_buf[3] = {0};
	uint8_t r_buf_check[2] = {0};

	if (!dev || !data)
		return -EINVAL;

	r_buf[0] = r_buf_check[0] = reg << 1 | LTC7871_SPI_READ;

	ret = no_os_spi_write_and_read(dev->spi, r_buf, 3);
	if (ret)
		return ret;

	r_buf_check[1] = r_buf[1];
	uint8_t pec = ltc7871_get_pec_byte(r_buf_check, 2);

	if (r_buf[2] != pec)
		return -EBADMSG;

	*data = r_buf[1];
	return 0;
}

/**
 * @brief Register Write
 * @param dev - The device structure
 * @param reg - The register value
 * @param data - The data written to the device
 * @return 0 in case of success, negative code otherwise
 */
int ltc7871_reg_write(struct ltc7871_dev *dev, uint8_t reg, uint8_t data)
{
	uint8_t w_buf[3] = {0};

	if (!dev)
		return -EINVAL;

	w_buf[0] = reg << 1 | LTC7871_SPI_WRITE;
	w_buf[1] = data;
	w_buf[2] = ltc7871_get_pec_byte(w_buf, 2);

	return no_os_spi_write_and_read(dev->spi, w_buf, 3);

}

/**
 * @brief Write to LTC7871 device register with mask.
 * @param dev - LTC7871 device descriptor
 * @param address - Register address.
 * @param mask - Mask to be applied.
 * @param data - Data to be written.
 * @return 0 in case of succes, negative error code otherwise
*/
int ltc7871_reg_write_mask(struct ltc7871_dev *dev, uint8_t address,
			   uint8_t mask, uint8_t data)
{
	int ret;
	uint8_t reg_data;

	if (!dev)
		return -EINVAL;

	ret = ltc7871_reg_read(dev, address, &reg_data);
	if (ret)
		return ret;

	reg_data &= ~mask;
	reg_data |= no_os_field_prep(mask, data);

	return ltc7871_reg_write(dev, address, reg_data);
}

/**
 * @brief gets mfr fault bit.
 * @param dev - LTC7871 device descriptor
 * @param status - m askfault status to get.
 * @param value - status value to be returned.
 * @return 0 in case of succes, negative error code otherwise
*/
int ltc7871_get_mfr_fault(struct ltc7871_dev *dev, uint8_t status, bool *value)
{
	int ret;
	uint8_t data;

	if (!dev || !value)
		return -EINVAL;

	ret = ltc7871_reg_read(dev, LTC7871_MFR_FAULT, &data);
	if (ret)
		return ret;

	*value = no_os_field_get(status, data);

	return 0;
}

/**
 * @brief gets status for overcurrent condition status.
 * @param dev - LTC7871 device descriptor
 * @param status - mask fault status to get. (0-5)
 * @param value - status value to be returned.
 * @return 0 in case of succes, negative error code otherwise
*/
int ltc7871_get_mfr_oc_fault(struct ltc7871_dev *dev, uint8_t status,
			     bool *value)
{
	int ret;
	uint8_t data;

	if (!dev || !value)
		return -EINVAL;

	ret = ltc7871_reg_read(dev, LTC7871_MFR_OC_FAULT, &data);
	if (ret)
		return ret;

	*value = no_os_field_get(status, data);

	return 0;
}

/**
 * @brief gets status for negative overcurrent fault condition status.
 * @param dev - LTC7871 device descriptor
 * @param status - mask fault status to get. (0-5)
 * @param value - status value to be returned.
 * @return 0 in case of succes, negative error code otherwise
*/
int ltc7871_get_mfr_noc_fault(struct ltc7871_dev *dev, uint8_t status,
			      bool *value)
{
	int ret;
	uint8_t data;

	if (!dev || !value)
		return -EINVAL;

	ret = ltc7871_reg_read(dev, LTC7871_MFR_NOC_FAULT, &data);
	if (ret)
		return ret;

	*value = no_os_field_get(status, data);

	return 0;
}

/**
 * @brief gets configuration of the controller programmed by the pins.
 * @param dev - LTC7871 device descriptor
 * @param config - configuration to get.
 * @param value - status value to be returned.
 * @return 0 in case of succes, negative error code otherwise
*/
int ltc7871_get_mfr_config1_setting(struct ltc7871_dev *dev, uint8_t config,
				    uint8_t *value)
{
	int ret;
	uint8_t data;

	if (!dev || !value)
		return -EINVAL;

	ret = ltc7871_reg_read(dev, LTC7871_MFR_CONFIG1, &data);
	if (ret)
		return ret;

	*value = no_os_field_get(config, data);

	return 0;
}

/**
 * @brief gets configuration of the controller programmed by the pins.
 * @param dev - LTC7871 device descriptor
 * @param config - configuration to get.
 * @param value - status value to be returned.
 * @return 0 in case of succes, negative error code otherwise
*/
int ltc7871_get_mfr_config2_setting(struct ltc7871_dev *dev, uint8_t config,
				    uint8_t *value)
{
	int ret;
	uint8_t data;

	if (!dev || !value)
		return -EINVAL;

	ret = ltc7871_reg_read(dev, LTC7871_MFR_CONFIG2, &data);
	if (ret)
		return ret;

	*value = no_os_field_get(config, data);

	return 0;
}

/**
 * @brief gets status.
 * @param dev - LTC7871 device descriptor
 * @param status_mask - status mask  to be acquire.
 * @param status - status value.
 * @return 0 in case of succes, negative error code otherwise
*/
int ltc7871_get_mfr_status(struct ltc7871_dev *dev, uint8_t status_mask,
			   bool *status)
{
	int ret;
	uint8_t data;

	if (!dev || !status)
		return -EINVAL;

	ret = ltc7871_reg_read(dev, LTC7871_MFR_STATUS, &data);
	if (ret)
		return ret;

	*status = no_os_field_get(status_mask, data);

	return 0;
}

/**
 * @brief Reset all R/W registers.
 * @param dev - LTC7871 device descriptor
 * @return 0 in case of succes, negative error code otherwise
 */
int ltc7871_reset(struct ltc7871_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ltc7871_reg_write_mask(dev, LTC7871_CHIP_CTRL,
				     LTC7871_RESET_MASK, SET);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Clear PEC fault bit.
 * @param dev - LTC7871 device descriptor
 * @return 0 in case of success, negative error code otherwise
 */
int ltc7871_clear_pec_fault(struct ltc7871_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ltc7871_reg_write_mask(dev, LTC7871_CHIP_CTRL,
				     LTC7871_CML_MASK, SET);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Read PEC fault bit.
 * @param dev - LTC7871 device descriptor
 * @param value - fault bit value.
 * @return 0 in case of success, negative error code otherwise
 */
int ltc7871_read_pec_fault(struct ltc7871_dev *dev, bool *value)
{
	int ret;
	uint8_t data;

	if (!dev)
		return -EINVAL;

	ret = ltc7871_reg_read(dev, LTC7871_CHIP_CTRL, &data);
	if (ret)
		return ret;

	*value = no_os_field_get(LTC7871_CML_MASK, data);

	return 0;
}

/**
 * @brief set write protect bit.
 * @param dev - LTC7871 device descriptor
 * @param value - 1 if enable wp, 0 if disable
 * @return 0 in case of success, negative error code otherwise
 */
int ltc7871_set_write_protect(struct ltc7871_dev *dev, bool value)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ltc7871_reg_write_mask(dev, LTC7871_CHIP_CTRL,
				     LTC7871_WP_MASK, value);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Read Write Protect bit for IDAC registers, and MFR_SSFM register.
 * @param dev - LTC7871 device descriptor
 * @param value - value of write protect bit
 * @return 0 in case of success, negative error code otherwise
 */
int ltc7871_is_write_protected(struct ltc7871_dev *dev, bool *value)
{
	int ret;
	uint8_t data;

	if (!dev)
		return -EINVAL;

	ret = ltc7871_reg_read(dev, LTC7871_CHIP_CTRL, &data);
	if (ret)
		return ret;

	*value = no_os_field_get(LTC7871_WP_MASK, data);

	return 0;
}

/**
 * @brief get the current DAC value to program the VLOW voltage.
 * @param dev - LTC7871 device descriptor
 * @param value - DAC Value
 * @return 0 in case of succes, negative error code otherwise
 */
int ltc7871_get_mfr_idac_vlow(struct ltc7871_dev *dev, int8_t *value)
{
	int ret;
	int8_t data;

	if (!dev || !value)
		return -EINVAL;

	ret = ltc7871_reg_read(dev, LTC7871_IDAC_VLOW, &data);
	if (ret)
		return ret;

	*value = no_os_field_get(LTC7871_MFR_IDAC_VLOW_MASK, data);

	return 0;
}

/**
 * @brief stores the current DAC value to program the VLOW voltage.
 * @param dev - LTC7871 device descriptor
 * @param value - DAC value
 * @return 0 in case of succes, negative error code otherwise
 */
int ltc7871_set_mfr_idac_vlow(struct ltc7871_dev *dev, int8_t value)
{
	int ret;

	if (!dev)
		return -EINVAL;

	bool wp;
	ret = ltc7871_is_write_protected(dev, &wp);
	if (ret)
		return ret;

	if (!wp) {
		ret = ltc7871_reg_write_mask(dev, LTC7871_IDAC_VLOW,
					     LTC7871_MFR_IDAC_VLOW_MASK, value);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief get the current DAC value to program the VHIGH voltage.
 * @param dev - LTC7871 device descriptor
 * @param value - DAC Value
 * @return 0 in case of succes, negative error code otherwise
 */
int ltc7871_get_mfr_idac_vhigh(struct ltc7871_dev *dev, int8_t *value)
{
	int ret;
	int8_t data;

	if (!dev || !value)
		return -EINVAL;

	ret = ltc7871_reg_read(dev, LTC7871_IDAC_VHIGH, &data);
	if (ret)
		return ret;

	*value = no_os_field_get(LTC7871_MFR_IDAC_VHIGH_MASK, data);

	return 0;
}

/**
 * @brief stores the current DAC value to program the VHIGH voltage.
 * @param dev - LTC7871 device descriptor
 * @param value - DAC value
 * @return 0 in case of succes, negative error code otherwise
 */
int ltc7871_set_mfr_idac_vhigh(struct ltc7871_dev *dev, int8_t value)
{
	int ret;

	if (!dev)
		return -EINVAL;

	bool wp;
	ret = ltc7871_is_write_protected(dev, &wp);
	if (ret)
		return ret;

	if (!wp) {
		ret = ltc7871_reg_write_mask(dev, LTC7871_IDAC_VHIGH,
					     LTC7871_MFR_IDAC_VHIGH_MASK, value);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief  get the current DAC value to program the sourcing current of the SETCUR pin.
 * @param dev - LTC7871 device descriptor
 * @param value - DAC Value
 * @return 0 in case of succes, negative error code otherwise
 */
int ltc7871_get_mfr_idac_setcur(struct ltc7871_dev *dev, int8_t *value)
{
	int ret;
	int8_t data;

	if (!dev || !value)
		return -EINVAL;

	ret = ltc7871_reg_read(dev, LTC7871_IDAC_SETCUR, &data);
	if (ret)
		return ret;

	*value = no_os_field_get(LTC7871_MFR_IDAC_SETCUR_MASK, data);

	return 0;
}

/**
 * @brief stores the current DAC value to program the sourcing current of the SETCUR pin.
 * @param dev - LTC7871 device descriptor
 * @param value - DAC value
 * @return 0 in case of succes, negative error code otherwise
 */
int ltc7871_set_mfr_idac_setcur(struct ltc7871_dev *dev, int8_t value)
{
	int ret;

	if (!dev)
		return -EINVAL;

	bool wp;
	ret = ltc7871_is_write_protected(dev, &wp);
	if (ret)
		return ret;

	if (!wp) {
		ret = ltc7871_reg_write_mask(dev, LTC7871_IDAC_SETCUR,
					     LTC7871_MFR_IDAC_SETCUR_MASK, value);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief  Get Frequency Spread Range.
 * @param dev - LTC7871 device descriptor
 * @param value - Frequencey Spread range
 * @return 0 in case of succes, negative error code otherwise
 */
int ltc7871_get_freq_spread_range(struct ltc7871_dev *dev, uint8_t *value)
{
	int ret;
	uint8_t data;

	if (!dev || !value)
		return -EINVAL;

	ret = ltc7871_reg_read(dev, LTC7871_MFR_SSFM, &data);
	if (ret)
		return ret;

	*value = no_os_field_get(LTC7871_MFR_SSFM_FSR_MASK, data);

	return 0;
}

/**
 * @brief Set Frequency Spread Range.
 * @param dev - LTC7871 device descriptor
 * @param value - Frequency Spread Range value
 * @return 0 in case of succes, negative error code otherwise
 */
int ltc7871_set_freq_spread_range(struct ltc7871_dev *dev,
				  enum ltc7871_ssfm_fsr value)
{
	int ret;

	if (!dev)
		return -EINVAL;

	bool wp;
	ret = ltc7871_is_write_protected(dev, &wp);
	if (ret)
		return ret;

	if (!wp) {
		ret = ltc7871_reg_write_mask(dev, LTC7871_MFR_SSFM,
					     LTC7871_MFR_SSFM_FSR_MASK, value);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief  Get Modulation Signal Frequency.
 * @param dev - LTC7871 device descriptor
 * @param value - Modulation signal frequency value
 * @return 0 in case of succes, negative error code otherwise
 */
int ltc7871_get_mod_freq(struct ltc7871_dev *dev, uint8_t *value)
{
	int ret;
	uint8_t data;

	if (!dev || !value)
		return -EINVAL;

	ret = ltc7871_reg_read(dev, LTC7871_MFR_SSFM, &data);
	if (ret)
		return ret;

	*value = no_os_field_get(LTC7871_MFR_SSFM_MSF_MASK, data);

	return 0;
}

/**
 * @brief Set Modulation Signal Frequency.
 * @param dev - LTC7871 device descriptor
 * @param value - Modulation signal frequency value
 * @return 0 in case of succes, negative error code otherwise
 */
int ltc7871_set_mod_freq(struct ltc7871_dev *dev, enum ltc7871_ssfm_msf value)
{
	int ret;

	if (!dev)
		return -EINVAL;

	bool wp;
	ret = ltc7871_is_write_protected(dev, &wp);
	if (ret)
		return ret;

	if (!wp) {
		ret = ltc7871_reg_write_mask(dev, LTC7871_MFR_SSFM,
					     LTC7871_MFR_SSFM_MSF_MASK, value);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Set PWMEN pin of LTC7871 device.
 * @param dev - LTC7871 device descriptor
 * @param value Set SWEN pin of LTC7871 device.
 * @return- 0 in case of succes, negative error code otherwise
*/
int ltc7871_set_pwmen_pin(struct ltc7871_dev *dev, uint8_t value)
{
	if (!dev)
		return -EINVAL;

	return no_os_gpio_set_value(dev->gpio_pwmen, value);
}

/**
 * @brief Get EN pin of LTC7871 device.
 * @param dev - LTC7871 device descriptor
 * @param value - EN pin value.
 * @return 0 in case of succes, negative error code otherwise
 */
int ltc7871_get_pwmen_pin(struct ltc7871_dev *dev, uint8_t *value)
{
	if (!dev || !value)
		return -EINVAL;

	return no_os_gpio_get_value(dev->gpio_pwmen, value);
}

/**
 * @brief Initialize the LTC7871 device.
 * @param device - LTC7871 device descriptor
 * @param init_param - Initialization parameter containing information about the
 * 		 LTC7871 device to be initialized.
 * @return 0 in case of succes, negative error code otherwise
*/
int ltc7871_init(struct ltc7871_dev **device,
		 struct ltc7871_init_param *init_param)
{
	struct ltc7871_dev *dev;
	int ret;

	if (!device || !init_param)
		return -EINVAL;

	dev = (struct ltc7871_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_spi_init(&dev->spi, init_param->spi);
	if (ret) {
		goto free_desc;
	}

	ret = no_os_gpio_get_optional(&dev->gpio_pwmen, init_param->gpio_pwmen);
	if (ret)
		goto free_desc;

	ret = no_os_gpio_direction_output(dev->gpio_pwmen, NO_OS_GPIO_HIGH);
	if (ret)
		goto free_desc;

	ret = ltc7871_reset(dev);

	*device = dev;

	return 0;


free_desc:
	ltc7871_remove(dev);

	return ret;
}

/**
 * @brief Free the resources allocated by the LT7871_init()
 * @param dev - LTC7871 device descriptor
 * @return- 0 in case of succes, negative error code otherwise
*/
int ltc7871_remove(struct ltc7871_dev *dev)
{
	if (!dev)
		return -ENODEV;

	no_os_gpio_remove(dev->gpio_pwmen);
	no_os_spi_remove(dev->spi);
	no_os_free(dev);

	return 0;
}
