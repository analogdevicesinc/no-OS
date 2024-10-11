/***************************************************************************//**
 *   @file   max22017.c
 *   @brief  Source file of MAX22017 Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
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
#include <string.h>
#include "max22017.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_crc8.h"

NO_OS_DECLARE_CRC8_TABLE(table);

/**
 * @brief Obtain the GPIO decriptor.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO initialization parameters
 * @return 0 in case of success, errno error codes otherwise.
 */
int32_t max22017_gpio_get(struct no_os_gpio_desc **desc,
			  const struct no_os_gpio_init_param *param)
{
	struct no_os_gpio_desc *descriptor;
	struct max22017_gpio_init_param *gpio_extra;
	int ret;

	if (!param || !param->extra || param->number)
		return -EINVAL;

	gpio_extra = param->extra;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = max22017_reg_update(gpio_extra->max22017_desc, MAX22017_GEN_GPIO_CTRL,
				  MAX22017_GPIO_MASK(param->number),
				  no_os_field_prep(MAX22017_GPIO_MASK(param->number), 1));
	if (ret)
		goto free_descriptor;

	descriptor->port = 0;
	descriptor->number = param->number;
	descriptor->pull = NO_OS_PULL_NONE;
	descriptor->platform_ops = param->platform_ops;
	descriptor->extra = param->extra;

	*desc = descriptor;

	return 0;

free_descriptor:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free the resources allocated by no_os_gpio_get().
 * @param desc - The GPIO descriptor.
 * @return 0 in case of success, errno error codes otherwise.
 */
int32_t max22017_gpio_remove(struct no_os_gpio_desc *desc)
{
	int ret;
	struct max22017_gpio_init_param *gpio_extra;

	if (!desc || !desc->extra)
		return -EINVAL;

	gpio_extra = desc->extra;

	ret = max22017_reg_update(gpio_extra->max22017_desc, MAX22017_GEN_GPIO_CTRL,
				  MAX22017_GPIO_MASK(desc->number),
				  no_os_field_prep(MAX22017_GPIO_MASK(desc->number), 0));
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}

/**
 * @brief Enable the input direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @return 0 in case of success, errno error codes otherwise.
 */
int32_t max22017_gpio_direction_input(struct no_os_gpio_desc *desc)
{
	struct max22017_gpio_init_param *gpio_extra;

	if (!desc || !desc->extra)
		return -EINVAL;

	gpio_extra = desc->extra;

	return max22017_reg_update(gpio_extra->max22017_desc, MAX22017_GEN_GPIO_CTRL,
				   MAX22017_GPIO_DIR_MASK(desc->number),
				   no_os_field_prep(MAX22017_GPIO_DIR_MASK(desc->number), 0));
}

/**
 * @brief Enable the output direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: NO_OS_GPIO_HIGH
 *                         NO_OS_GPIO_LOW
 * @return 0 in case of success, errno error codes otherwise.
 */
int32_t max22017_gpio_direction_output(struct no_os_gpio_desc *desc,
				       uint8_t value)
{
	int ret;
	struct max22017_gpio_init_param *gpio_extra;

	if (!desc || !desc->extra)
		return -EINVAL;

	gpio_extra = desc->extra;

	ret = max22017_reg_update(gpio_extra->max22017_desc, MAX22017_GEN_GPIO_CTRL,
				  MAX22017_GPIO_DIR_MASK(desc->number),
				  no_os_field_prep(MAX22017_GPIO_DIR_MASK(desc->number), 1));
	if (ret)
		return ret;

	return max22017_reg_update(gpio_extra->max22017_desc, MAX22017_GEN_GPIO_DATA,
				   MAX22017_GPIO_OUT_VAL_MASK(desc->number),
				   no_os_field_prep(MAX22017_GPIO_OUT_VAL_MASK(desc->number), value ? 1 : 0));
}

/**
 * @brief Get the direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param direction - The direction.
 *                    Example: NO_OS_GPIO_OUT
 *                             NO_OS_GPIO_IN
 * @return 0 in case of success, errno error codes otherwise.
 */
int32_t max22017_gpio_get_direction(struct no_os_gpio_desc *desc,
				    uint8_t *direction)
{
	int ret;
	uint32_t reg_val;
	struct max22017_gpio_init_param *gpio_extra;

	if (!desc || !desc->extra)
		return -EINVAL;

	gpio_extra = desc->extra;

	ret = max22017_reg_read(gpio_extra->max22017_desc, MAX22017_GEN_GPIO_CTRL,
				&reg_val);
	if (ret)
		return ret;

	*direction = no_os_field_get(MAX22017_GPIO_DIR_MASK(desc->number), reg_val);

	return 0;
}

/**
 * @brief Set the value of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: NO_OS_GPIO_HIGH
 *                         NO_OS_GPIO_LOW
 * @return 0 in case of success, errno error codes otherwise.
 */
int32_t max22017_gpio_set_value(struct no_os_gpio_desc *desc, uint8_t value)
{
	struct max22017_gpio_init_param *gpio_extra;

	if (!desc || !desc->extra)
		return -EINVAL;

	gpio_extra = desc->extra;

	return max22017_reg_update(gpio_extra->max22017_desc, MAX22017_GEN_GPIO_DATA,
				   MAX22017_GPIO_OUT_VAL_MASK(desc->number), value ? 1 : 0);
}

/**
 * @brief Get the value of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: NO_OS_GPIO_HIGH
 *                         NO_OS_GPIO_LOW
 * @return 0 in case of success, errno error codes otherwise.
 */
int32_t max22017_gpio_get_value(struct no_os_gpio_desc *desc, uint8_t *value)
{
	int ret;
	uint32_t reg_val;
	struct max22017_gpio_init_param *gpio_extra;

	if (!desc || !desc->extra)
		return -EINVAL;

	gpio_extra = desc->extra;

	ret = max22017_reg_read(gpio_extra->max22017_desc, MAX22017_GEN_GPIO_DATA,
				&reg_val);
	if (ret)
		return ret;

	*value = no_os_field_get(MAX22017_GPIO_IN_VAL_MASK(desc->number), reg_val);

	return 0;
}

/**
 * @brief Set timeout settings for MAX22017 device
 * @param desc - MAX22017 device descriptor
 * @param timeout_cnfg - Timeout configuration :
 * 			 false - A timeout will only assert TMOUT_INT bit in
 * 			 Interrupts Register
 * 			 true - A timeout will also reset AO_EN, AI_EN and
 * 			 GPIO_EN pair of bits.
 * @param timeout_en - Enable/disable timeout
 * @param timeout - Timeout time value
 * @return 0 in case of succes, negative error code otherwise
*/
int max22017_set_timeout(struct max22017_desc *desc, bool timeout_cnfg,
			 bool timeout_en, enum max22017_timeout_sel timeout)
{
	uint32_t reg_val;

	reg_val = no_os_field_prep(MAX22017_TIMEOUT_SEL_MASK, timeout) |
		  no_os_field_get(MAX22017_TIMEOUT_CNFG_MASK, timeout_cnfg) |
		  no_os_field_get(MAX22017_TIMEOUT_EN_MASK, timeout_en);

	return max22017_reg_update(desc, MAX22017_GEN_CNFG, MAX22017_TIMEOUT_MASK,
				   reg_val);
}

/**
 * @brief Set data of the selected channel
 * @param desc - MAX22017 device descriptor
 * @param ch - Selected channel
 * @param data - Data to be set.
 * @return 0 in case of succes, negative error code otherwise
*/
int max22017_set_data(struct max22017_desc *desc, enum max22017_channel ch,
		      int32_t data)
{
	if (desc->uni && data < 0)
		return -EINVAL;

	if (!desc->uni)
		data = no_os_sign_extend32(data, 18);

	return max22017_reg_write(desc,
				  ch ? MAX22017_AO_DATA_CH2 : MAX22017_AO_DATA_CH1, (uint32_t)data);
}

/**
 * @brief Get data from requestec channel
 * @param desc - MAX22017 device descriptor
 * @param ch - Selected channel
 * @param data - Received data from channel
 * @return 0 in case of succes, negative error code otherwise
*/
int max22017_get_data(struct max22017_desc *desc, enum max22017_channel ch,
		      int32_t *data)
{
	int ret;

	ret = max22017_reg_read(desc, ch ? MAX22017_AO_DATA_CH2 : MAX22017_AO_DATA_CH1,
				(uint32_t *)data);
	if (ret)
		return ret;

	if (!desc->uni)
		*data = no_os_sign_extend32(*data, 18);

	return 0;
}

/**
 * @brief Set slew rate settings for requested channels
 * @param desc - MAX22017 device descriptor
 * @param ch - Selected channel
 * @param step_size - Step size value
 * @param update_rate - Update rate value
 * @param enable - Enable/disable slew rate control
 * @param hart - true - Slew rate times for HART applications
 * 		 false - Slew rate times for inductive load applications
 * @return 0 in case of succes, negative error code otherwise
*/
int max22017_slew_rate(struct max22017_desc *desc, enum max22017_channel ch,
		       enum max22017_sr_step_size step_size,
		       enum max22017_sr_update_rate update_rate,
		       bool enable, bool hart)
{
	uint32_t reg_val;

	reg_val = no_os_field_prep(MAX22017_SR_EN_MASK, enable) |
		  no_os_field_prep(MAX22017_SR_SEL_MASK, hart) |
		  no_os_field_prep(MAX22017_SR_STEP_SIZE_MASK, step_size) |
		  no_os_field_prep(MAX22017_SR_UPDATE_RATE_MASK, update_rate);

	return max22017_reg_write(desc,
				  ch ? MAX22017_AO_SLEW_RATE_CH2 : MAX22017_AO_SLEW_RATE_CH1, reg_val);
}

/**
 * @brief Set offset value for output calibration
 * @param desc - MAX22017 device descriptor
 * @param ch - Selected channel
 * @param offset - Offset value
 * @return 0 in case of succes, negative error code otherwise
*/
int max22017_offset_calib(struct max22017_desc *desc, enum max22017_channel ch,
			  int16_t offset)
{
	if (desc->uni && offset < 0)
		return -EINVAL;

	return max22017_reg_write(desc,
				  ch ? MAX22017_AO_OFFSET_CORR_CH2 : MAX22017_AO_OFFSET_CORR_CH1,
				  (uint32_t)offset);
}

/**
 * @brief Set gain value for calibration of requested channel
 * @param desc - MAX22017 device descriptor
 * @param ch - Selected channel
 * @param gain - Gain value
 * @return 0 in case of succes, negative error code otherwise
*/
int max22017_gain_calib(struct max22017_desc *desc, enum max22017_channel ch,
			uint16_t gain)
{
	return max22017_reg_write(desc,
				  ch ? MAX22017_AO_GAIN_CORR_CH2 : MAX22017_AO_GAIN_CORR_CH1, (uint32_t)gain);
}

/**
 * @brief Set operation mode for requested channel
 * @param desc - MAX22017 device descriptor
 * @param ch - Selected channel
 * @param op_mode - Operation mode
 * @return 0 in case of succes, negative error code otherwise
*/
int max22017_op_mode(struct max22017_desc *desc, enum max22017_channel ch,
		     enum max22017_op_mode op_mode)
{
	uint32_t reg_val;

	reg_val = no_os_field_prep(MAX22017_AO_MODE_MASK(ch),
				   no_os_field_get(MAX22017_AO_MODE_MODE_MASK, op_mode)) |
		  no_os_field_prep(MAX22017_AI_EN_MASK(ch),
				   no_os_field_get(MAX22017_AI_EN_MODE_MASK, op_mode)) |
		  no_os_field_prep(MAX22017_AO_EN_MASK(ch),
				   no_os_field_get(MAX22017_AO_EN_MODE_MASK, op_mode));

	return max22017_reg_update(desc, MAX22017_AO_CNFG, MAX22017_OP_MODE_MASK(ch),
				   reg_val);
}

/**
 * @brief Set AO configuration
 * @param desc - MAX22017 device descriptor
 * @param ch - Selected channel
 * @param uni - Polarity type :
 * 		false - bipolar
 * 		true - unipolar
 * @param cm_sense - CM sense resistor value, affects OW value as well
 * @param ldac - Enable/Disable LDAC function, takes effect only if LDAC GPIO
 * 		 is set with load_control API.
 * @return 0 in case of succes, negative error code otherwise
*/
int max22017_config(struct max22017_desc *desc, enum max22017_channel ch,
		    enum max22017_out_mode uni, enum max22017_cm_sense cm_sense,
		    bool ldac)
{
	uint32_t reg_val;
	int ret;

	if (desc->ldac_desc) {
		ret = no_os_gpio_set_value(desc->ldac_desc,
					   ldac ? NO_OS_GPIO_LOW : NO_OS_GPIO_HIGH);
		if (ret)
			return ret;

		reg_val = no_os_field_prep(MAX22017_LD_CNFG_MASK(ch), !ldac);
	} else
		reg_val = no_os_field_prep(MAX22017_LD_CNFG_MASK(ch), 1);

	reg_val |= no_os_field_prep(MAX22017_CM_SENSE_MASK(ch), cm_sense) |
		   no_os_field_prep(MAX22017_UNI_MASK(ch), uni) |
		   no_os_field_prep(MAX22017_OW_MASK(ch), !cm_sense);

	ret = max22017_reg_update(desc, MAX22017_AO_CNFG, MAX22017_CNFG_MASK(ch),
				  reg_val);
	if (ret)
		return ret;

	desc->uni = uni;

	return 0;
}

/**
 * @brief Soft reset requested channel
 * @param desc - MAX22017 device descriptor
 * @param ch - Selected channel
 * @param coeff_only - Coefficient only reset enable/disable
 * @return 0 in case of succes, negative error code otherwise
*/
int max22017_soft_reset(struct max22017_desc *desc, enum max22017_channel ch,
			bool coeff_only)
{
	int ret;

	if (coeff_only)
		return max22017_reg_update(desc, MAX22017_GEN_RST_CTRL,
					   MAX22017_COEFF_ONLY_MASK, no_os_field_prep(MAX22017_COEFF_ONLY_MASK,
							   MAX22017_COEFF_ONLY_VAL));

	ret = max22017_reg_update(desc, MAX22017_GEN_RST_CTRL, MAX22017_RST_MASK(ch),
				  no_os_field_prep(MAX22017_RST_MASK(ch), 1));
	if (ret)
		return ret;

	return max22017_reg_update(desc, MAX22017_GEN_RST_CTRL, MAX22017_RST_MASK(ch),
				   no_os_field_prep(MAX22017_RST_MASK(ch), 0));
}

/**
 * @brief Read the value of a device register
 * @param desc - device descriptor for the MAX22017
 * @param addr - address of the register
 * @param val - value of the register
 * @return 0 in case of success, negative error code otherwise
 */
int max22017_reg_read(struct max22017_desc *desc, uint32_t addr, uint32_t *val)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = desc->buff,
		.rx_buff = desc->buff,
		.bytes_number = MAX22017_FRAME_SIZE,
		.cs_change = 1,
	};
	uint8_t crc;
	int ret;

	if (desc->crc_en)
		xfer.bytes_number++;

	memset(desc->buff, 0, xfer.bytes_number);
	desc->buff[0] = no_os_field_prep(MAX22017_ADRR_MASK, addr) |
			no_os_field_prep(MAX22017_RW_MASK, 1);

	if (desc->crc_en)
		desc->buff[3] = no_os_crc8(table, desc->buff, xfer.bytes_number,
					   0);

	ret = no_os_spi_transfer(desc->comm_desc, &xfer, 1);
	if (ret)
		return ret;

	if (desc->crc_en) {
		crc = no_os_crc8(table, desc->buff, xfer.bytes_number, 0);
		if (crc != desc->buff[3])
			return -EINVAL;
	}

	*val = no_os_field_prep(MAX22017_PAYLOAD_MSB_MASK, desc->buff[1]) |
	       no_os_field_prep(MAX22017_PAYLOAD_LSB_MASK, desc->buff[2]);

	return 0;
}

/**
 * @brief Write the value of a device register
 * @param desc - device descriptor for the MAX22017
 * @param addr - address of the register
 * @param val - value of the register
 * @return 0 in case of success, negative error code otherwise
 */
int max22017_reg_write(struct max22017_desc *desc, uint32_t addr, uint32_t val)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = desc->buff,
		.bytes_number = MAX22017_FRAME_SIZE,
		.cs_change = 1,
	};

	desc->buff[0] = no_os_field_prep(MAX22017_ADRR_MASK, addr) |
			no_os_field_prep(MAX22017_RW_MASK, 0);
	desc->buff[1] = no_os_field_get(MAX22017_PAYLOAD_MSB_MASK, val);
	desc->buff[2] = no_os_field_get(MAX22017_PAYLOAD_LSB_MASK, val);

	if (desc->crc_en) {
		xfer.bytes_number++;
		desc->buff[3] = no_os_crc8(table, desc->buff, xfer.bytes_number,
					   0);
	}

	return no_os_spi_transfer(desc->comm_desc, &xfer, 1);
}

/**
 * @brief Update the value of a device register (read/write sequence).
 * @param desc - device descriptor for the MAX22017
 * @param addr - address of the register
 * @param mask - bit mask of the field to be updated
 * @param val - value of the masked field. Should be bit shifted by using
 * 		 no_os_field_prep(mask, val)
 * @return 0 in case of success, negative error code otherwise
 */
int max22017_reg_update(struct max22017_desc *desc, uint32_t addr,
			uint32_t mask, uint32_t val)
{
	int ret;
	uint32_t reg_val = 0;

	ret = max22017_reg_read(desc, addr, &reg_val);
	if (ret)
		return ret;

	reg_val &= ~mask;
	reg_val |= mask & val;

	return max22017_reg_write(desc, addr, reg_val);
}

/**
 * @brief Initialize and configure the MAX22017 device
 * @param desc - device descriptor for the MAX22017 that will be initialized.
 * @param param - initialization parameter for the device.
 * @return 0 in case of success, negative error code otherwise
 */
int max22017_init(struct max22017_desc **desc,
		  struct max22017_init_param *param)
{
	struct max22017_desc *descriptor;
	int ret;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_spi_init(&descriptor->comm_desc, param->comm_param);
	if (ret)
		goto error;

	ret = no_os_gpio_get(&descriptor->rstb_desc, param->rstb_param);
	if (ret)
		goto error;

	ret = no_os_gpio_direction_output(descriptor->rstb_desc, NO_OS_GPIO_HIGH);
	if (ret)
		goto error;

	ret = no_os_gpio_get_optional(&descriptor->fault_desc, param->fault_param);
	if (ret)
		goto error;

	if (descriptor->fault_desc) {
		ret = no_os_gpio_direction_input(descriptor->fault_desc);
		if (ret)
			goto error;
	}

	ret = no_os_gpio_get_optional(&descriptor->busy_desc, param->busy_param);
	if (ret)
		goto error;

	if (descriptor->busy_desc) {
		ret = no_os_gpio_direction_output(descriptor->busy_desc, NO_OS_GPIO_HIGH);
		if (ret)
			goto error;
	}

	ret = no_os_gpio_get_optional(&descriptor->ldac_desc,
				      param->ldac_param);
	if (ret)
		goto error;

	if (descriptor->ldac_desc) {
		ret = no_os_gpio_direction_output(descriptor->ldac_desc,
						  NO_OS_GPIO_LOW);
		if (ret)
			goto error;
	}

	if (param->crc_en)
		no_os_crc8_populate_msb(table, 0x31);

	ret = max22017_reg_update(descriptor, MAX22017_GEN_CNFG, MAX22017_CRC_MASK,
				  no_os_field_prep(MAX22017_CRC_MASK, param->crc_en));
	if (ret)
		goto error;

	descriptor->crc_en = param->crc_en;

	ret = max22017_reg_update(descriptor, MAX22017_GEN_CNFG,
				  MAX22017_EXT_DAC_REF_MASK, no_os_field_prep(MAX22017_EXT_DAC_REF_MASK,
						  param->ext_dac_ref));
	if (ret)
		goto error;

	descriptor->uni = false;

	*desc = descriptor;

	return 0;

error:
	max22017_remove(descriptor);
	return ret;
}

/**
 * @brief Free the resources allocated during init and perform a soft reset.
 * @param desc - device descriptor for the MAX22017 that will be initialized.
 * @return 0 in case of success, negative error code otherwise
 */
int max22017_remove(struct max22017_desc *desc)
{
	int ret;

	if (!desc)
		return -ENODEV;

	ret = max22017_soft_reset(desc, MAX22017_CH1, false);
	if (ret)
		return ret;

	ret = max22017_soft_reset(desc, MAX22017_CH2, false);
	if (ret)
		return ret;

	no_os_spi_remove(desc->comm_desc);
	no_os_gpio_remove(desc->ldac_desc);
	no_os_free(desc);

	return 0;
}

const struct no_os_gpio_platform_ops max22017_gpio_ops = {
	.gpio_ops_get = &max22017_gpio_get,
	.gpio_ops_remove = &max22017_gpio_remove,
	.gpio_ops_direction_input = &max22017_gpio_direction_input,
	.gpio_ops_direction_output = &max22017_gpio_direction_output,
	.gpio_ops_get_direction = &max22017_gpio_get_direction,
	.gpio_ops_set_value = &max22017_gpio_set_value,
	.gpio_ops_get_value = &max22017_gpio_get_value
};
