/***************************************************************************//**
*   @file   max17616.c
*   @brief  Source file of the MAX17616 Driver
*   @author Carlos Jones (carlosjr.jones@analog.com)
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

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include "no_os_units.h"
#include "no_os_util.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_i2c.h"
#include "no_os_crc8.h"

#include "max17616.h"

/* Reference : max17616-max17616a.pdf */
#define MAX17616_PMBUS_REVISION_VALUE	0x33 /* PMBus Part I and II Rev1.3 */
#define MAX17616_MFR_ID_STR		"MAXIM"
#define MAX17616_MFR_ID_SIZE		5
#define MAX17616_MFR_REV_SIZE		2

/* DIRECT format coefficients for MAX17616 data conversion */
struct max17616_direct_coeffs {
	int16_t m;	/* Slope coefficient */
	int16_t b;	/* Offset coefficient */
	int8_t R;	/* Exponent */
};

/* Reference : max17616-max17616a.pdf */
static const struct max17616_direct_coeffs max17616_vin_coeffs = {
	.m = 512, .b = -18, .R = -1
};
static const struct max17616_direct_coeffs max17616_vout_coeffs = {
	.m = 512, .b = -18, .R = -1
};
static const struct max17616_direct_coeffs max17616_iout_coeffs = {
	.m = 5845, .b = 80, .R = -1
};
static const struct max17616_direct_coeffs max17616_temp_coeffs = {
	.m = 71, .b = 19653, .R = -1
};

static const struct max17616_chip_info max17616_info = {
	.mfr_id = MAX17616_MFR_ID_STR,
	.mfr_id_size = MAX17616_MFR_ID_SIZE,
	.mfr_rev = "01",
	.mfr_rev_size = MAX17616_MFR_REV_SIZE,
	.pmbus_rev = MAX17616_PMBUS_REVISION_VALUE,
	.specific_info = {
		[ID_MAX17616] = {
			.ic_dev_id = "MAX17616",
			.ic_dev_id_size = sizeof("MAX17616") - 1,
		},
		[ID_MAX17616A] = {
			.ic_dev_id = "MAX17616A",
			.ic_dev_id_size = sizeof("MAX17616A") - 1,
		}
	},
};

/**
 * @brief Send a PMBus command to the device
 * @param dev - Device structure
 * @param cmd - PMBus command
 * @return 0 in case of success, negative error code otherwise
 */
int max17616_send_byte(struct max17616_dev *dev, uint8_t cmd)
{
	uint8_t tx_buf[2] = {0};

	tx_buf[0] = cmd;

	return no_os_i2c_write(dev->i2c_desc, tx_buf, 1, 1);
}

/**
 * @brief Perform a raw PMBus read byte operation
 * @param dev - Device structure
 * @param cmd - PMBus command
 * @param data - Address of the byte read
 * @return 0 in case of success, negative error code otherwise
 */
int max17616_read_byte(struct max17616_dev *dev, uint8_t cmd, uint8_t *data)
{
	int ret;
	uint8_t rx_buf[2];
	uint8_t tx_buf[2] = {0};
	tx_buf[0] = cmd;

	ret = no_os_i2c_write(dev->i2c_desc, tx_buf, 1, 0);
	if (ret)
		return ret;

	ret = no_os_i2c_read(dev->i2c_desc, rx_buf, 1, 1);
	if (ret)
		return ret;

	*data = rx_buf[0];

	return 0;
}

/**
 * @brief Perform a raw PMBus read word operation
 * @param dev - Device structure
 * @param cmd - PMBus command
 * @param word - Address of the read word
 * @return 0 in case of success, negative error code otherwise
 */
int max17616_read_word(struct max17616_dev *dev, uint8_t cmd, uint16_t *word)
{
	int ret;
	uint8_t rx_buf[3] = {0};
	uint8_t tx_buf[2] = {0};
	tx_buf[0] = cmd;

	ret = no_os_i2c_write(dev->i2c_desc, tx_buf, 1, 0);
	if (ret)
		return ret;

	ret = no_os_i2c_read(dev->i2c_desc, rx_buf, 2, 1);
	if (ret)
		return ret;

	*word = no_os_get_unaligned_le16(rx_buf);

	return 0;
}

/**
 * @brief Perform a PMBus read block operation
 * @param dev - Device structure
 * @param cmd - PMBus command
 * @param data - Address of the read block
 * @param nbytes - Size of the block in bytes
 * @return 0 in case of success, negative error code otherwise
 */
int max17616_read_block_data(struct max17616_dev *dev, uint8_t cmd,
			     uint8_t *data, size_t nbytes)
{
	int ret;
	uint8_t rxbuf[nbytes + 2];

	ret = no_os_i2c_write(dev->i2c_desc, &cmd, 1, 0);
	if (ret)
		return ret;

	ret = no_os_i2c_read(dev->i2c_desc, rxbuf, nbytes + 1, 1);
	if (ret)
		return ret;

	if ((size_t)rxbuf[0] > nbytes)
		return -EMSGSIZE;

	memcpy(data, &rxbuf[1], nbytes);

	return 0;
}

/**
 * @brief Perform a raw PMBus write byte operation
 * @param dev - Device structure
 * @param cmd - PMBus command
 * @param value - Byte to be written
 * @return 0 in case of success, negative error code otherwise
 */
int max17616_write_byte(struct max17616_dev *dev, uint8_t cmd, uint8_t value)
{
	uint8_t tx_buf[3] = {0};

	tx_buf[0] = cmd;
	tx_buf[1] = value;

	return no_os_i2c_write(dev->i2c_desc, tx_buf, 2, 1);
}

/**
 * @brief Convert DIRECT format raw value to integer
 * @param raw_value - Raw 16-bit value from device
 * @param coeffs - DIRECT format coefficients
 * @return Converted integer value
 */
static int max17616_direct_to_int(uint16_t raw_value,
				  const struct max17616_direct_coeffs *coeffs)
{
	int32_t y = (int16_t)raw_value;  /* Convert to signed */
	int32_t result;

	/* X = (1/m) × (Y × 10^(-R) - b) */
	/* First: Y × 10^(-R) */
	int32_t y_scaled;
	/* Negative R: Y × 10^(-R) = Y × 10^|R| */
	int32_t power = 1;
	for (int i = 0; i < -coeffs->R; i++)
		power *= 10;

	y_scaled = y * power;

	/* Then: (Y × 10^(-R) - b) / m */
	result = (y_scaled - coeffs->b) / coeffs->m;

	return (int)result;
}

/**
 * @brief Read STATUS_BYTE register
 * @param dev - Device structure
 * @param status_byte - Pointer to store status byte
 * @return 0 on success, negative error code otherwise
 */
int max17616_read_status_byte(struct max17616_dev *dev, uint8_t *status_byte)
{
	return max17616_read_byte(dev, MAX17616_CMD(MAX17616_STATUS_BYTE),
				  status_byte);
}

/**
 * @brief Read STATUS_VOUT register
 * @param dev - Device structure
 * @param status_vout - Pointer to store status VOUT byte
 * @return 0 on success, negative error code otherwise
 */
int max17616_read_status_vout(struct max17616_dev *dev, uint8_t *status_vout)
{
	if (!dev || !status_vout)
		return -EINVAL;

	return max17616_read_byte(dev, MAX17616_CMD(MAX17616_STATUS_VOUT),
				  status_vout);
}

/**
 * @brief Read STATUS_IOUT register
 * @param dev - Device structure
 * @param status_iout - Pointer to store status IOUT byte
 * @return 0 on success, negative error code otherwise
 */
int max17616_read_status_iout(struct max17616_dev *dev, uint8_t *status_iout)
{
	if (!dev || !status_iout)
		return -EINVAL;

	return max17616_read_byte(dev, MAX17616_CMD(MAX17616_STATUS_IOUT),
				  status_iout);
}

/**
 * @brief Read STATUS_INPUT register
 * @param dev - Device structure
 * @param status_input - Pointer to store status INPUT byte
 * @return 0 on success, negative error code otherwise
 */
int max17616_read_status_input(struct max17616_dev *dev, uint8_t *status_input)
{
	if (!dev || !status_input)
		return -EINVAL;

	return max17616_read_byte(dev, MAX17616_CMD(MAX17616_STATUS_INPUT),
				  status_input);
}

/**
 * @brief Read STATUS_TEMPERATURE register
 * @param dev - Device structure
 * @param status_temperature - Pointer to store status TEMPERATURE byte
 * @return 0 on success, negative error code otherwise
 */
int max17616_read_status_temperature(struct max17616_dev *dev,
				     uint8_t *status_temperature)
{
	if (!dev || !status_temperature)
		return -EINVAL;

	return max17616_read_byte(dev, MAX17616_CMD(MAX17616_STATUS_TEMPERATURE),
				  status_temperature);
}

/**
 * @brief Read STATUS_CML register
 * @param dev - Device structure
 * @param status_cml - Pointer to store status CML byte
 * @return 0 on success, negative error code otherwise
 */
int max17616_read_status_cml(struct max17616_dev *dev, uint8_t *status_cml)
{
	if (!dev || !status_cml)
		return -EINVAL;

	return max17616_read_byte(dev, MAX17616_CMD(MAX17616_STATUS_CML),
				  status_cml);
}

/**
 * @brief Read STATUS_MFR_SPECIFIC register
 * @param dev - Device structure
 * @param status_mfr - Pointer to store status MFR SPECIFIC byte
 * @return 0 on success, negative error code otherwise
 */
int max17616_read_status_mfr_specific(struct max17616_dev *dev,
				      uint8_t *status_mfr)
{
	if (!dev || !status_mfr)
		return -EINVAL;

	return max17616_read_byte(dev, MAX17616_CMD(MAX17616_STATUS_MFR_SPECIFIC),
				  status_mfr);
}

/**
 * @brief Read device capability register
 * @param dev - Device structure
 * @param capability - Pointer to store capability byte
 * @return 0 on success, negative error code otherwise
 */
int max17616_read_capability(struct max17616_dev *dev, uint8_t *capability)
{
	if (!dev || !capability)
		return -EINVAL;

	return max17616_read_byte(dev, MAX17616_CMD(MAX17616_CAPABILITY),
				  capability);
}

/**
 * @brief Read and verify manufacturer ID
 * @param dev - Device structure
 * @return 0 on success, negative error code otherwise
 */
int max17616_verify_manufacturer_id(struct max17616_dev *dev)
{
	uint8_t mfr_id[8];
	int ret;

	ret = max17616_read_block_data(dev, MAX17616_CMD(MAX17616_MFR_ID),
				       mfr_id,
				       MAX17616_DATA_SIZE(MAX17616_MFR_ID));
	if (ret)
		return ret;

	if (strncmp((char*)mfr_id,
		    MAX17616_MFR_ID_STR, MAX17616_MFR_ID_SIZE) != 0)
		return -ENODEV;

	return 0;
}

/**
 * @brief Read device ID and identify chip variant
 * @param dev - Device structure
 * @return 0 on success, negative error code otherwise
 */
int max17616_identify_chip_variant(struct max17616_dev *dev)
{
	uint8_t device_id[16];
	int ret;

	ret = max17616_read_block_data(dev, MAX17616_CMD(MAX17616_IC_DEVICE_ID),
				       device_id,
				       MAX17616_DATA_SIZE(MAX17616_IC_DEVICE_ID));
	if (ret)
		return ret;

	for (int i = 0; i < ID_MAX17616_CHIP_COUNT; i++) {
		if (strncmp((char*)device_id, dev->chip_info->specific_info[i].ic_dev_id,
			    dev->chip_info->specific_info[i].ic_dev_id_size) == 0) {
			dev->chip_id = i;
			return 0;
		}
	}

	return -ENODEV;
}

/**
 * @brief Verify PMBus revision
 * @param dev - Device structure
 * @return 0 on success, negative error code otherwise
 */
int max17616_verify_pmbus_revision(struct max17616_dev *dev)
{
	uint16_t pmbus_rev;
	int ret;

	ret = max17616_read_word(dev, MAX17616_CMD(MAX17616_PMBUS_REVISION),
				 &pmbus_rev);
	if (ret)
		return ret;

	if (pmbus_rev != MAX17616_PMBUS_REVISION_VALUE)
		return -ENODEV;

	return 0;
}

/**
 * @brief Identify MAX17616 device and verify compatibility
 * @param dev - Device structure
 * @return 0 on success, negative error code otherwise
 */
static int max17616_identify(struct max17616_dev *dev)
{
	int ret;

	ret = max17616_verify_manufacturer_id(dev);
	if (ret)
		return ret;

	ret = max17616_identify_chip_variant(dev);
	if (ret)
		return ret;

	ret = max17616_verify_pmbus_revision(dev);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Clear all asserted faults
 * @param dev - Device structure
 * @return 0 in case of success, negative error code otherwise
 */
int max17616_clear_faults(struct max17616_dev *dev)
{
	return max17616_send_byte(dev, MAX17616_CMD(MAX17616_CLEAR_FAULTS));
}

/**
 * @brief Read a specific value with automatic data conversion
 * @param dev - Device structure
 * @param value_type - Type of value to read
 * @param value - Pointer to store the converted value
 * @return 0 on success, negative error code otherwise
 */
int max17616_read_value(struct max17616_dev *dev,
			enum max17616_value_type value_type, int *value)
{
	uint16_t raw_value;
	int ret;
	int vin, iout;

	if (!dev || !value)
		return -EINVAL;

	switch (value_type) {
	case MAX17616_VIN:
		ret = max17616_read_word(dev, MAX17616_CMD(MAX17616_READ_VIN),
					 &raw_value);
		if (ret)
			return ret;

		*value = max17616_direct_to_int(raw_value, &max17616_vin_coeffs);
		break;

	case MAX17616_VOUT:
		ret = max17616_read_word(dev, MAX17616_CMD(MAX17616_READ_VOUT),
					 &raw_value);
		if (ret)
			return ret;

		*value = max17616_direct_to_int(raw_value, &max17616_vout_coeffs);
		break;

	case MAX17616_IOUT:
		ret = max17616_read_word(dev, MAX17616_CMD(MAX17616_READ_IOUT),
					 &raw_value);
		if (ret)
			return ret;

		*value = max17616_direct_to_int(raw_value, &max17616_iout_coeffs);
		break;

	case MAX17616_TEMP:
		ret = max17616_read_word(dev,
					 MAX17616_CMD(MAX17616_READ_TEMPERATURE_1),
					 &raw_value);
		if (ret)
			return ret;

		*value = max17616_direct_to_int(raw_value, &max17616_temp_coeffs);
		break;

	case MAX17616_POWER:
		/* Calculate power from voltage and current */
		ret = max17616_read_value(dev, MAX17616_VOUT, &vin);
		if (ret)
			return ret;
		ret = max17616_read_value(dev, MAX17616_IOUT, &iout);
		if (ret)
			return ret;

		*value = vin * iout;
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Set device operation state
 * @param dev - Device structure
 * @param enable - true to enable, false to disable
 * @return 0 on success, negative error code otherwise
 */
int max17616_set_operation_state(struct max17616_dev *dev, bool enable)
{
	uint8_t byte_value = enable ? 0x80 : 0x00;
	return max17616_write_byte(dev, MAX17616_CMD(MAX17616_OPERATION),
				   byte_value);
}

/**
 * @brief Get device operation state
 * @param dev - Device structure
 * @param enabled - Pointer to store operation state
 * @return 0 on success, negative error code otherwise
 */
int max17616_get_operation_state(struct max17616_dev *dev, bool *enabled)
{
	uint8_t operation;

	if (!dev || !enabled)
		return -EINVAL;

	int ret = max17616_read_byte(dev, MAX17616_CMD(MAX17616_OPERATION),
				     &operation);

	if (ret == 0)
		*enabled = (operation & 0x80) ? true : false;

	return ret;
}

/**
 * @brief Get current limit mode
 * @param dev - Device structure
 * @param clmode - Pointer to the current limit mode setting
 * @return 0 on success, negative error code otherwise
 */
int max17616_get_current_limit_mode(struct max17616_dev *dev,
				    enum max17616_current_limit_mode *clmode)
{
	uint8_t raw_value;
	int ret;

	if (!dev || !clmode)
		return -EINVAL;

	ret = max17616_read_byte(dev, MAX17616_CMD(MAX17616_SET_CLMODE),
				 &raw_value);
	if (ret)
		return ret;

	/* Extract bits 7:6 and convert to enum */
	switch (raw_value & MAX17616_CLMODE_BITS_MASK) {
	case MAX17616_CLMODE_LATCH_OFF_BITS:
		*clmode = MAX17616_CLMODE_LATCH_OFF;
		break;
	case MAX17616_CLMODE_CONTINUOUS_BITS:
		*clmode = MAX17616_CLMODE_CONTINUOUS;
		break;
	case MAX17616_CLMODE_AUTO_RETRY_BITS:
		*clmode = MAX17616_CLMODE_AUTO_RETRY;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Set current limit mode
 * @param dev - Device structure
 * @param clmode - Current limit mode setting
 * @return 0 on success, negative error code otherwise
 */
int max17616_set_current_limit_mode(struct max17616_dev *dev,
				    enum max17616_current_limit_mode clmode)
{
	if (!dev)
		return -EINVAL;

	return max17616_write_byte(dev, MAX17616_CMD(MAX17616_SET_CLMODE),
				   (uint8_t)clmode);
}

/**
 * @brief Get current start ratio
 * @param dev - Device structure
 * @param istart_ratio - Pointer to the current start ratio setting
 * @return 0 on success, negative error code otherwise
 */
int max17616_get_istart_ratio(struct max17616_dev *dev,
			      enum max17616_istart_ratio *istart_ratio)
{
	uint8_t raw_value;
	int ret;

	if (!dev || !istart_ratio)
		return -EINVAL;

	ret = max17616_read_byte(dev, MAX17616_CMD(MAX17616_SET_ISTART_RATIO),
				 &raw_value);
	if (ret)
		return ret;

	/* Extract bits 3:0 and convert to enum */
	switch (raw_value & MAX17616_ISTART_BITS_MASK) {
	case MAX17616_ISTART_FULL_BITS:
		*istart_ratio = MAX17616_ISTART_FULL;
		break;
	case MAX17616_ISTART_HALF_BITS:
		*istart_ratio = MAX17616_ISTART_HALF;
		break;
	case MAX17616_ISTART_QUARTER_BITS:
		*istart_ratio = MAX17616_ISTART_QUARTER;
		break;
	case MAX17616_ISTART_EIGHTH_BITS:
		*istart_ratio = MAX17616_ISTART_EIGHTH;
		break;
	case MAX17616_ISTART_SIXTEENTH_BITS:
		*istart_ratio = MAX17616_ISTART_SIXTEENTH;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Set current start ratio
 * @param dev - Device structure
 * @param istart_ratio - Current start ratio setting
 * @return 0 on success, negative error code otherwise
 */
int max17616_set_istart_ratio(struct max17616_dev *dev,
			      enum max17616_istart_ratio istart_ratio)
{
	if (!dev)
		return -EINVAL;

	return max17616_write_byte(dev, MAX17616_CMD(MAX17616_SET_ISTART_RATIO),
				   (uint8_t)istart_ratio);
}

/**
 * @brief Get short-term overcurrent duration
 * @param dev - Device structure
 * @param timeout - Pointer to store short-term overcurrent duration setting
 * @return 0 on success, negative error code otherwise
 */
int max17616_get_overcurrent_timeout(struct max17616_dev *dev,
				     enum max17616_overcurrent_timeout *timeout)
{
	uint8_t raw_value;
	int ret;

	if (!dev || !timeout)
		return -EINVAL;

	ret = max17616_read_byte(dev, MAX17616_CMD(MAX17616_SET_TSTOC),
				 &raw_value);
	if (ret)
		return ret;

	switch (raw_value & MAX17616_TIMEOUT_BITS_MASK) {
	case MAX17616_TIMEOUT_400US_BITS:
		*timeout = MAX17616_TIMEOUT_400US;
		break;
	case MAX17616_TIMEOUT_1MS_BITS:
		*timeout = MAX17616_TIMEOUT_1MS;
		break;
	case MAX17616_TIMEOUT_4MS_BITS:
		*timeout = MAX17616_TIMEOUT_4MS;
		break;
	case MAX17616_TIMEOUT_24MS_BITS:
		*timeout = MAX17616_TIMEOUT_24MS;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Set short-term overcurrent duration
 * @param dev - Device structure
 * @param timeout - Short-term overcurrent duration setting
 * @return 0 on success, negative error code otherwise
 */
int max17616_set_overcurrent_timeout(struct max17616_dev *dev,
				     enum max17616_overcurrent_timeout timeout)
{
	if (!dev)
		return -EINVAL;

	return max17616_write_byte(dev, MAX17616_CMD(MAX17616_SET_TSTOC),
				   (uint8_t)timeout);
}

/**
 * @brief Get the short-term overcurrent limit ratio
 * @param dev - Device structure
 * @param istlim - Pointer to store ISTLIM setting
 * @return 0 on success, negative error code otherwise
 */
int max17616_get_overcurrent_limit(struct max17616_dev *dev,
				   enum max17616_overcurrent_limit *istlim)
{
	uint8_t raw_value;
	int ret;

	if (!dev || !istlim)
		return -EINVAL;

	ret = max17616_read_byte(dev, MAX17616_CMD(MAX17616_SET_ISTLIM),
				 &raw_value);
	if (ret)
		return ret;

	switch (raw_value & MAX17616_OC_LIMIT_BITS_MASK) {
	case MAX17616_OC_LIMIT_1_25_BITS:
		*istlim = MAX17616_OC_LIMIT_1_25;
		break;
	case MAX17616_OC_LIMIT_1_50_BITS:
		*istlim = MAX17616_OC_LIMIT_1_50;
		break;
	case MAX17616_OC_LIMIT_1_75_BITS:
		*istlim = MAX17616_OC_LIMIT_1_75;
		break;
	case MAX17616_OC_LIMIT_2_00_BITS:
		*istlim = MAX17616_OC_LIMIT_2_00;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Set the short-term overcurrent limit ratio
 * @param dev - Device structure
 * @param istlimit - Short-term overcurrent limit ratio setting
 * @return 0 on success, negative error code otherwise
 */
int max17616_set_overcurrent_limit(struct max17616_dev *dev,
				   enum max17616_overcurrent_limit istlimit)
{
	if (!dev)
		return -EINVAL;

	return max17616_write_byte(dev, MAX17616_CMD(MAX17616_SET_ISTLIM),
				   (uint8_t)istlimit);
}

/**
 * @brief Set VOUT UV fault limit configuration using enums
 * @param dev - Device structure
 * @param voltage - Nominal voltage selection (bits 4:2)
 * @param threshold - PGOOD rising threshold (bits 1:0)
 * @return 0 on success, negative error code otherwise
 */
int max17616_set_vout_uv_fault_limit_config(struct max17616_dev *dev,
		enum max17616_nominal_voltage voltage,
		enum max17616_pgood_threshold threshold)
{
	uint8_t reg_value;

	if (!dev)
		return -EINVAL;

	/* Validate enum values */
	if (voltage > MAX17616_NOMINAL_72V
	    || threshold > MAX17616_PGOOD_MINUS_30_PERCENT)
		return -EINVAL;

	/* Combine voltage selection (bits 4:2) and PGOOD threshold (bits 1:0) */
	reg_value = ((uint8_t)voltage << 2) | (uint8_t)threshold;

	return max17616_write_byte(dev, MAX17616_CMD(MAX17616_VOUT_UV_FAULT_LIMIT),
				   reg_value);
}

/**
 * @brief Get VOUT UV fault limit configuration using enums
 * @param dev - Device structure
 * @param voltage - Pointer to store nominal voltage selection
 * @param threshold - Pointer to store PGOOD rising threshold
 * @return 0 on success, negative error code otherwise
 */
int max17616_get_vout_uv_fault_limit_config(struct max17616_dev *dev,
		enum max17616_nominal_voltage *voltage,
		enum max17616_pgood_threshold *threshold)
{
	uint8_t raw_value;
	int ret;

	if (!dev || !voltage || !threshold)
		return -EINVAL;

	ret = max17616_read_byte(dev, MAX17616_CMD(MAX17616_VOUT_UV_FAULT_LIMIT),
				 &raw_value);
	if (ret)
		return ret;

	/* Extract voltage selection (bits 4:2) */
	uint8_t voltage_bits = (raw_value >> 2) & MAX17616_NOMINAL_VOLTAGE_BITS_MASK;
	switch (voltage_bits) {
	case MAX17616_NOMINAL_5V_BITS:
		*voltage = MAX17616_NOMINAL_5V;
		break;
	case MAX17616_NOMINAL_9V_BITS:
		*voltage = MAX17616_NOMINAL_9V;
		break;
	case MAX17616_NOMINAL_12V_BITS:
		*voltage = MAX17616_NOMINAL_12V;
		break;
	case MAX17616_NOMINAL_24V_BITS:
		*voltage = MAX17616_NOMINAL_24V;
		break;
	case MAX17616_NOMINAL_36V_BITS:
		*voltage = MAX17616_NOMINAL_36V;
		break;
	case MAX17616_NOMINAL_48V_BITS:
		*voltage = MAX17616_NOMINAL_48V;
		break;
	case MAX17616_NOMINAL_60V_BITS:
		*voltage = MAX17616_NOMINAL_60V;
		break;
	case MAX17616_NOMINAL_72V_BITS:
		*voltage = MAX17616_NOMINAL_72V;
		break;
	default:
		return -EINVAL;
	}

	/* Extract PGOOD threshold (bits 1:0) */
	uint8_t threshold_bits = raw_value & MAX17616_PGOOD_THRESHOLD_BITS_MASK;
	switch (threshold_bits) {
	case MAX17616_PGOOD_MINUS_10_PERCENT_BITS:
		*threshold = MAX17616_PGOOD_MINUS_10_PERCENT;
		break;
	case MAX17616_PGOOD_MINUS_20_PERCENT_BITS:
		*threshold = MAX17616_PGOOD_MINUS_20_PERCENT;
		break;
	case MAX17616_PGOOD_MINUS_30_PERCENT_BITS:
		*threshold = MAX17616_PGOOD_MINUS_30_PERCENT;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Read MAX17616 device status
 * @param dev - Device structure
 * @param status - Pointer to store status
 * @return 0 on success, negative error code otherwise
 */
int max17616_read_status(struct max17616_dev *dev,
			 struct max17616_status *status)
{
	int ret;

	if (!dev || !status)
		return -EINVAL;

	memset(status, 0, sizeof(*status));

	/* Read STATUS_WORD first - it contains bit flags indicating which
	 * specific status registers have faults */
	ret = max17616_read_word(dev, MAX17616_CMD(MAX17616_STATUS_WORD),
				 &status->word);
	if (ret)
		return ret;

	/* STATUS_BYTE is the lower byte of STATUS_WORD
	 * Note: For compliance, use max17616_read_status_byte() to read STATUS_BYTE directly */
	status->byte = (uint8_t)(status->word & 0xFF);

	/* Only read individual status registers if their corresponding
	 * STATUS_WORD bits are asserted */

	/* Check CML fault bit (bit 1 of STATUS_BYTE) */
	if (status->word & (1 << MAX17616_STATUS_BIT_CML)) {
		ret = max17616_read_status_cml(dev, &status->cml);
		if (ret)
			return ret;
	}

	/* Check Temperature fault bit (bit 2 of STATUS_BYTE) */
	if (status->word & (1 << MAX17616_STATUS_BIT_TEMPERATURE)) {
		ret = max17616_read_status_temperature(dev, &status->temperature);
		if (ret)
			return ret;
	}

	/* Check Input fault bit (bit 5 of STATUS_WORD high byte) */
	if (status->word & (1 << (8 + MAX17616_STATUS_BIT_INPUT))) {
		ret = max17616_read_status_input(dev, &status->input);
		if (ret)
			return ret;
	}

	/* Check IOUT/POUT fault bit (bit 6 of STATUS_WORD high byte) */
	if (status->word & (1 << (8 + MAX17616_STATUS_BIT_IOUT_POUT))) {
		ret = max17616_read_status_iout(dev, &status->iout);
		if (ret)
			return ret;
	}

	/* Check VOUT fault bit (bit 7 of STATUS_WORD high byte) */
	if (status->word & (1 << (8 + MAX17616_STATUS_BIT_VOUT))) {
		ret = max17616_read_status_vout(dev, &status->vout);
		if (ret)
			return ret;
	}

	/* Check Manufacturer Specific fault bit (bit 4 of STATUS_WORD high byte) */
	if (status->word & (1 << (8 + MAX17616_STATUS_BIT_MFR))) {
		ret = max17616_read_status_mfr_specific(dev, &status->mfr_specific);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Read comprehensive telemetry data
 * @param dev - Device structure
 * @param telemetry - Telemetry structure to populate
 * @return 0 on success, negative error code otherwise
 */
int max17616_read_telemetry_all(struct max17616_dev *dev,
				struct max17616_telemetry *telemetry)
{
	int ret;

	if (!dev || !telemetry)
		return -EINVAL;

	/* Initialize telemetry structure */
	memset(telemetry, 0, sizeof(struct max17616_telemetry));

	ret = max17616_read_value(dev, MAX17616_VIN, &telemetry->vin);
	if (ret == 0)
		telemetry->valid_mask |= NO_OS_BIT(0);

	ret = max17616_read_value(dev, MAX17616_VOUT, &telemetry->vout);
	if (ret == 0)
		telemetry->valid_mask |= NO_OS_BIT(1);

	ret = max17616_read_value(dev, MAX17616_IOUT, &telemetry->iout);
	if (ret == 0)
		telemetry->valid_mask |= NO_OS_BIT(3);

	ret = max17616_read_value(dev, MAX17616_TEMP, &telemetry->temp1);
	if (ret == 0)
		telemetry->valid_mask |= NO_OS_BIT(4);

	/* Calculate power (P = V * I) */
	if ((telemetry->valid_mask & NO_OS_BIT(1)) &&
	    (telemetry->valid_mask & NO_OS_BIT(3))) {
		telemetry->pout = (telemetry->vout * telemetry->iout);
		telemetry->valid_mask |= NO_OS_BIT(5);
	}

	return 0;
}

/**
 * @brief Initialize the device structure
 * @param device - Device structure
 * @param init_param - Initialization parameters
 * @return 0 in case of success, negative error code otherwise
 */
int max17616_init(struct max17616_dev **device,
		  struct max17616_init_param *init_param)
{
	struct max17616_dev *dev;
	int ret = -EINVAL;

	dev = (struct max17616_dev *)no_os_calloc(1, sizeof(struct max17616_dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_i2c_init(&dev->i2c_desc, init_param->i2c_init);
	if (ret)
		goto i2c_err;

	dev->chip_info = (struct max17616_chip_info *)&max17616_info;
	dev->chip_id = init_param->chip_id;

	ret = max17616_identify(dev);
	if (ret)
		goto dev_err;

	ret = max17616_clear_faults(dev);
	if (ret)
		goto dev_err;

	ret = max17616_set_operation_state(dev, true);
	if (ret)
		goto dev_err;

	*device = dev;

	return 0;

dev_err:
	no_os_i2c_remove(dev->i2c_desc);
i2c_err:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Free or remove device instance
 * @param dev - The device structure
 * @return 0 in case of success, negative error code otherwise
 */
int max17616_remove(struct max17616_dev *dev)
{
	int ret;

	ret = no_os_i2c_remove(dev->i2c_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return ret;
}

/* Fault information lookup table */
static const struct max17616_fault_info fault_descriptions[] = {
	/* CML Faults */
	{
		MAX17616_FAULT_GRP_CML, MAX17616_CML_FAULT_OTHER,
		"Communications other error", true
	},
	{
		MAX17616_FAULT_GRP_CML, MAX17616_CML_FAULT_MEM_ERROR,
		"Memory error detected", true
	},
	{
		MAX17616_FAULT_GRP_CML, MAX17616_CML_FAULT_PEC_FAIL,
		"Packet error checking failed", true
	},
	{
		MAX17616_FAULT_GRP_CML, MAX17616_CML_FAULT_DATA,
		"Invalid or unsupported data received", true
	},
	{
		MAX17616_FAULT_GRP_CML, MAX17616_CML_FAULT_CMD,
		"Invalid or unsupported command received", true
	},

	/* Temperature Faults */
	{
		MAX17616_FAULT_GRP_TEMPERATURE, MAX17616_TEMPERATURE_FAULT_OT_FAULT,
		"Overtemperature fault occurred", true
	},

	/* Input Faults */
	{
		MAX17616_FAULT_GRP_INPUT, MAX17616_INPUT_FAULT_VIN_UV_FAULT,
		"Input undervoltage fault occurred", true
	},
	{
		MAX17616_FAULT_GRP_INPUT, MAX17616_INPUT_FAULT_VIN_OV_FAULT,
		"Input overvoltage fault occurred", true
	},

	/* Output Current Faults */
	{
		MAX17616_FAULT_GRP_IOUT_POUT, MAX17616_IOUT_FAULT_OUT_OC_UV_LO,
		"Output overcurrent and low voltage fault occurred", true
	},
	{
		MAX17616_FAULT_GRP_IOUT_POUT, MAX17616_IOUT_FAULT_OUT_OC_FAULT,
		"Output overcurrent fault occurred", true
	},

	/* Output Voltage Faults */
	{
		MAX17616_FAULT_GRP_VOUT, MAX17616_VOUT_FAULT_VOUT_UV_WARNING,
		"Output has gone below the output undervoltage threshold", true
	},
	{
		MAX17616_FAULT_GRP_VOUT, MAX17616_VOUT_FAULT_VOUT_OV_WARNING,
		"Output regulation event has occurred", true
	},

	/* Manufacturer Specific Faults */
	{
		MAX17616_FAULT_GRP_MFR_SPECIFIC, MAX17616_MFR_FAULT_REVERSE_CURRENT,
		"Reverse current fault occurred", true
	},
	{
		MAX17616_FAULT_GRP_MFR_SPECIFIC, MAX17616_MFR_FAULT_OUTPUT_SHORT_CIRCUIT,
		"Output short circuit fault occurred", true
	},
	{
		MAX17616_FAULT_GRP_MFR_SPECIFIC, MAX17616_MFR_FAULT_SOFT_START,
		"Soft start failed", true
	},
	{
		MAX17616_FAULT_GRP_MFR_SPECIFIC, MAX17616_MFR_FAULT_IMON_PIN,
		"IMON pin fault", true
	},
	{
		MAX17616_FAULT_GRP_MFR_SPECIFIC, MAX17616_MFR_FAULT_SETI_PIN,
		"SETI pin fault", true
	},
};

/**
 * @brief Get human-readable description for a specific fault
 * @param group - Fault group identifier
 * @param bit - Fault bit position
 * @return Pointer to fault description string or NULL if not found
 */
const char *max17616_get_fault_description(uint16_t group, uint8_t bit)
{
	size_t num_faults = sizeof(fault_descriptions) / sizeof(fault_descriptions[0]);

	for (size_t i = 0; i < num_faults; i++) {
		if (fault_descriptions[i].group == group &&
		    fault_descriptions[i].bit == bit &&
		    fault_descriptions[i].is_supported) {
			return fault_descriptions[i].description;
		}
	}

	return NULL;  /* Return NULL for unknown faults */
}
