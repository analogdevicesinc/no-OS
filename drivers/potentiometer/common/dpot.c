/*******************************************************************************
 *   @file   dpot.c
 *   @brief  Source file for the digital potentiometer drivers
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
#include "dpot.h"

/**
 * @brief Initialize the digital potentiometer.
 * @param desc - digipot descriptor.
 * @param pdpot_init_param - digipot init parameters. 
 * @return 0 in case of success, negative error code otherwise.
 */
int dpot_init(struct dpot_dev **desc, struct dpot_init_param *pdpot_init_param)
{
	int ret;
	if (!pdpot_init_param || !pdpot_init_param->dpot_ops) {
		return -EINVAL;
	}
	if (!pdpot_init_param->dpot_ops->dpot_init) {
		return -ENOSYS;
	}

	ret = pdpot_init_param->dpot_ops->dpot_init(pdpot_init_param, desc);
	if (ret) {
		return ret;
	}
	(*desc)->device_id = pdpot_init_param->device_id;
	(*desc)->dpot_ops = (struct dpot_ops *)pdpot_init_param->dpot_ops;

	return 0;
}

/**
 * @brief Free the memory allocated by dpot_init().
 * @param desc - digipot descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int dpot_remove(struct dpot_dev *desc)
{
	if (!desc || !desc->dpot_ops) {
		return -EINVAL;
	}
	if (!desc->dpot_ops->dpot_remove) {
		return -ENOSYS;
	}
	return desc->dpot_ops->dpot_remove(desc);
}

/**
 * @brief Reset the digital potentiometer.
 * @param desc - digipot descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int dpot_reset(struct dpot_dev *desc)
{
	if (!desc || !desc->dpot_ops) {
		return -EINVAL;
	}
	if (!desc->dpot_ops->dpot_reset) {
		return -ENOTSUP;
	}
	return desc->dpot_ops->dpot_reset(desc);
}

/**
 * @brief Shutdown the digital potentiometer channel.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param shutdown_enable - Shutdown enable status flag.
 * @return 0 in case of success, negative error code otherwise.
 */
int dpot_shutdown(struct dpot_dev *desc, enum dpot_chn_type chn,
		  bool shutdown_enable)
{
	if (!desc || !desc->dpot_ops) {
		return -EINVAL;
	}
	if (!desc->dpot_ops->dpot_shutdown) {
		return -ENOTSUP;
	}
	return desc->dpot_ops->dpot_shutdown(desc, chn, shutdown_enable);
}

/**
 * @brief Set the digital potentiometer operating mode.
 * @param desc - digipot descriptor.
 * @param operating_mode - digipot operating mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int dpot_set_operating_mode(struct dpot_dev *desc,
			    enum dpot_operating_mode operating_mode)
{
	if (!desc || !desc->dpot_ops) {
		return -EINVAL;
	}
	if (!desc->dpot_ops->dpot_set_operating_mode) {
		return -ENOTSUP;
	}
	return desc->dpot_ops->dpot_set_operating_mode(desc, operating_mode);
}

/**
 * @brief Read the digital potentiometer input register.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param data  - input register data to be read.
 * @return 0 in case of success, negative error code otherwise.
 */
int dpot_input_reg_read(struct dpot_dev *desc, enum dpot_chn_type chn,
			uint8_t *data)
{
	if (!desc || !desc->dpot_ops) {
		return -EINVAL;
	}
	if (!desc->dpot_ops->dpot_input_reg_read) {
		return -ENOSYS;
	}

	return desc->dpot_ops->dpot_input_reg_read(desc, chn, data);
}

/**
 * @brief Write to the digital potentiometer input register.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param data - input register data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int dpot_input_reg_write(struct dpot_dev *desc, enum dpot_chn_type chn,
			 uint8_t data)
{
	if (!desc || !desc->dpot_ops) {
		return -EINVAL;
	}

	if (!desc->dpot_ops->dpot_input_reg_write) {
		return -ENOSYS;
	}
	return desc->dpot_ops->dpot_input_reg_write(desc, chn, data);
}

/**
 * @brief Copy input register to RDAC (software LRDAC).
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @return 0 in case of success, negative error code otherwise.
 */
int dpot_sw_lrdac_update(struct dpot_dev *desc, enum dpot_chn_type chn)
{
	if (!desc || !desc->dpot_ops) {
		return -EINVAL;
	}
	if (!desc->dpot_ops->dpot_sw_lrdac_update) {
		return -ENOSYS;
	}
	return desc->dpot_ops->dpot_sw_lrdac_update(desc, chn);
}

/**
 * @brief Read the digital potentiometer channel.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param data  - Channel data to be read.
 * @return 0 in case of success, negative error code otherwise.
 */
int dpot_chn_read(struct dpot_dev *desc, enum dpot_chn_type chn, uint8_t *data)
{
	if (!desc || !desc->dpot_ops) {
		return -EINVAL;
	}
	if (!desc->dpot_ops->dpot_chn_read) {
		return -ENOSYS;
	}
	return desc->dpot_ops->dpot_chn_read(desc, chn, data);
}

/**
 * @brief Write to the digital potentiometer channel.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param data - Channel data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int dpot_chn_write(struct dpot_dev *desc, enum dpot_chn_type chn, uint8_t data)
{
	if (!desc || !desc->dpot_ops) {
		return -EINVAL;
	}

	if (!desc->dpot_ops->dpot_chn_write) {
		return -ENOSYS;
	}

	return desc->dpot_ops->dpot_chn_write(desc, chn, data);
}

/**
 * @brief Read the NVM (non-volatile) memory data.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param data  - NVM data to be read.
 * @return 0 in case of success, negative error code otherwise.
 */
int dpot_nvm_read(struct dpot_dev *desc, enum dpot_chn_type chn, uint8_t *data)
{
	if (!desc || !desc->dpot_ops) {
		return -EINVAL;
	}

	if (!desc->dpot_ops->dpot_nvm_read) {
		return -ENOTSUP;
	}

	return desc->dpot_ops->dpot_nvm_read(desc, chn, data);
}

/**
 * @brief Write data to NVM (non-volatile) memory.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param data  - NVM data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int dpot_nvm_write(struct dpot_dev *desc, enum dpot_chn_type chn, uint8_t data)
{
	if (!desc || !desc->dpot_ops) {
		return -EINVAL;
	}

	if (!desc->dpot_ops->dpot_nvm_write) {
		return -ENOTSUP;
	}

	return desc->dpot_ops->dpot_nvm_write(desc, chn, data);
}

/**
 * @brief Copy RDAC register to NVM/EEPROM.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @return 0 in case of success, negative error code otherwise.
 */
int dpot_copy_rdac_to_nvm(struct dpot_dev *desc, enum dpot_chn_type chn)
{
	if (!desc || !desc->dpot_ops) {
		return -EINVAL;
	}

	if (!desc->dpot_ops->dpot_copy_rdac_to_nvm) {
		return -ENOTSUP;
	}

	return desc->dpot_ops->dpot_copy_rdac_to_nvm(desc, chn);
}

/**
 * @brief Copy NVM/EEPROM data to RDAC.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @return 0 in case of success, negative error code otherwise.
 */
int dpot_copy_nvm_to_rdac(struct dpot_dev *desc, enum dpot_chn_type chn)
{
	if (!desc || !desc->dpot_ops) {
		return -EINVAL;
	}
	if (!desc->dpot_ops->dpot_copy_nvm_to_rdac) {
		return -ENOTSUP;
	}
	return desc->dpot_ops->dpot_copy_nvm_to_rdac(desc, chn);
}

/**
 * @brief Perform RDAC linear increment/decrement.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param status - linear increment/decrement status.
 * @return 0 in case of success, negative error code otherwise.
 */
int dpot_rdac_linear_update(struct dpot_dev *desc, enum dpot_chn_type chn,
			    enum dpot_rdac_linear_status status)
{
	if (!desc || !desc->dpot_ops) {
		return -EINVAL;
	}
	if (!desc->dpot_ops->dpot_rdac_linear_update) {
		return -ENOTSUP;
	}
	return desc->dpot_ops->dpot_rdac_linear_update(desc, chn, status);
}

/**
 * @brief Perform Enter/exit top scale.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param nEnter - Enter or exit the TS.
 * @return 0 in case of success, negative error code otherwise.
 */
int dpot_enable_top_scale(struct dpot_dev *desc,
			  enum dpot_chn_type chn,
			  uint8_t nEnter)
{
	if (!desc || !desc->dpot_ops) {
		return -EINVAL;
	}
	return desc->dpot_ops->dpot_enable_top_bottom_scale(desc, chn, true, nEnter);
}

/**
 * @brief Perform Enter/exit top scale.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param nEnter - Enter or exit the Bottom scake.
 * @return 0 in case of success, negative error code otherwise.
 */
int dpot_enable_bottom_scale(struct dpot_dev *desc,
			     enum dpot_chn_type chn,
			     uint8_t nEnter)
{
	if (!desc || !desc->dpot_ops) {
		return -EINVAL;
	}
	return desc->dpot_ops->dpot_enable_top_bottom_scale(desc, chn, false, nEnter);
}

/**
 * @brief Perform RDAC 6dB increment/decrement.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param status - 6dB increment/decrement status.
 * @return 0 in case of success, negative error code otherwise.
 */
int dpot_rdac_6db_update(struct dpot_dev *desc, enum dpot_chn_type chn,
			 enum dpot_rdac_6db_status status)
{
	if (!desc || !desc->dpot_ops) {
		return -EINVAL;
	}
	if (!desc->dpot_ops->dpot_rdac_6db_update) {
		return -ENOTSUP;
	}

	return desc->dpot_ops->dpot_rdac_6db_update(desc, chn, status);
}

/**
 * @brief Send command word to the digital potentiometer.
 * @param desc - digipot descriptor.
 * @param cmd  - digipot command word
 * @return 0 in case of success, negative error code otherwise.
 */
int dpot_send_cmd(struct dpot_dev *desc, struct dpot_command *cmd)
{
	if (!desc || !desc->dpot_ops || !cmd) {
		return -EINVAL;
	}

	if (!desc->dpot_ops->dpot_send_cmd) {
		return -ENOTSUP;
	}

	return desc->dpot_ops->dpot_send_cmd(desc, cmd);
}
/**
 * @brief Send command word to the digital potentiometer.
 * @param desc - digipot descriptor.
 * @param chn  - Channel type 
 * @param data  - Pointer to location where tolerance should be written.  
  * @return 0 in case of success, negative error code otherwise.
 */
int dpot_tolerance_read(struct dpot_dev *desc, enum dpot_chn_type chn,
			uint8_t *data)
{
	if (!desc || !desc->dpot_ops) {
		return -EINVAL;
	}
	if (!desc->dpot_ops->dpot_tolerance_read) {
		return -ENOTSUP;
	}
	return desc->dpot_ops->dpot_tolerance_read(desc, chn, data);
}

/**
 * @brief Shutdown the digital potentiometer channel.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param shutdown_enable - To set bid scaleenable mid scale .
 * @return 0 in case of success, negative error code otherwise.
 */
int dpot_set_mid_scale(struct dpot_dev *desc, enum dpot_chn_type chn,
		       bool shutdown_enable)
{
	if (!desc || !desc->dpot_ops) {
		return -EINVAL;
	}
	if (!desc->dpot_ops->dpot_set_mid_scale) {
		return -ENOTSUP;
	}
	return desc->dpot_ops->dpot_set_mid_scale(desc, chn, shutdown_enable);
}

