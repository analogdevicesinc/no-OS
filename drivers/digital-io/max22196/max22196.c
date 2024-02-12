/***************************************************************************//**
 *   @file   max22196.c
 *   @brief  Source file of MAX22196 Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
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
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "max22196.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

/**
 * @brief Compute the CRC5 value for an array of bytes when writing to MAX22196
 * @param data - array of data to compute for.
 * @param encode - boolean that tells if the input data is to encode or decode.
 * @return the resulted CRC5
 */
static uint8_t max22196_crc(uint8_t *data, bool encode)
{
	uint8_t crc5_start = 0x1f;
	uint8_t crc5_poly = 0x15;
	uint8_t crc5_result = crc5_start;
	uint8_t extra_byte = 0x00;
	uint8_t data_bit;
	uint8_t result_bit;
	int i;

	for (i = (encode) ? 0 : 2; i < 8; i++) {
		data_bit = (data[0] >> (7 - i)) & 0x01;
		result_bit = (crc5_result & 0x10) >> 4;
		if (data_bit ^ result_bit)
			crc5_result = crc5_poly ^ ((crc5_result << 1) & 0x1f);
		else
			crc5_result = (crc5_result << 1) & 0x1f;
	}

	for (i = 0; i < 8; i++) {
		data_bit = (data[1] >> (7 - i)) & 0x01;
		result_bit = (crc5_result & 0x10) >> 4;
		if (data_bit ^ result_bit)
			crc5_result = crc5_poly ^ ((crc5_result << 1) & 0x1f);
		else
			crc5_result = (crc5_result << 1) & 0x1f;
	}

	for (i = 0; i < 3; i++) {
		data_bit = (extra_byte >> (7 - i)) & 0x01;
		result_bit = (crc5_result & 0x10) >> 4;
		if (data_bit ^ result_bit)
			crc5_result = crc5_poly ^ ((crc5_result << 1) & 0x1f);
		else
			crc5_result = (crc5_result << 1) & 0x1f;
	}

	return crc5_result;
}

/**
 * @brief MAX22196 register write function
 * @param desc - The device descriptor for MAX22196.
 * @param reg - The register where the information is to be written.
 * @param val - The value to be written to reg.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22196_reg_write(struct max22196_desc *desc, uint32_t reg, uint32_t val)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = desc->buff,
		.bytes_number = MAX22196_FRAME_SIZE,
		.cs_change = 1,
	};

	desc->buff[0] = no_os_field_prep(MAX22196_ADDR_MASK, desc->chip_address) |
			no_os_field_prep(MAX22196_REG_ADDR_MASK, reg) |
			no_os_field_prep(MAX22196_RW_MASK, 1);
	desc->buff[1] = val;

	if (desc->crc_en) {
		xfer.bytes_number++;
		desc->buff[2] = max22196_crc(desc->buff, true);
	}

	return no_os_spi_transfer(desc->comm_desc, &xfer, 1);
}

/**
 * @brief MAX22196 register read function
 * @param desc - The device descriptor for MAX22196.
 * @param reg - The register value where the data is read from.
 * @param val - The value returned from read.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22196_reg_read(struct max22196_desc *desc, uint32_t reg, uint32_t *val)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = desc->buff,
		.rx_buff = desc->buff,
		.bytes_number = MAX22196_FRAME_SIZE,
		.cs_change = 1,
	};
	uint8_t crc;
	int ret;

	if (desc->crc_en)
		xfer.bytes_number++;

	memset(desc->buff, 0, xfer.bytes_number);
	desc->buff[0] = no_os_field_prep(MAX22196_ADDR_MASK, desc->chip_address) |
			no_os_field_prep(MAX22196_REG_ADDR_MASK, reg) |
			no_os_field_prep(MAX22196_RW_MASK, 0);

	if (desc->crc_en)
		desc->buff[2] = max22196_crc(&desc->buff[0], true);

	ret = no_os_spi_transfer(desc->comm_desc, &xfer, 1);
	if (ret)
		return ret;

	if (desc->crc_en) {
		crc = max22196_crc(&desc->buff[0], false);
		if (crc != desc->buff[2])
			return -EINVAL;
	}

	*val = desc->buff[1];

	return 0;
}

/**
 * @brief - MAX22196 register update function
 * @param desc - The device descriptor for MAX22196.
 * @param reg - The register to be updated.
 * @param mask - The specific mask where the value will be updated.
 * @param val - The value that is to be written to the mask.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22196_reg_update(struct max22196_desc *desc, uint32_t reg, uint32_t mask,
			uint32_t val)
{
	int ret;
	uint32_t reg_val = 0;

	ret = max22196_reg_read(desc, reg, &reg_val);
	if (ret)
		return ret;

	reg_val &= ~mask;
	reg_val |= mask & val;

	return max22196_reg_write(desc, reg, reg_val);
}

/**
 * @brief - MAX22196 function that helps setting the mode of the device.
 * @param desc - The device descriptor for MAX22196
 * @param ch - The channel for which the mode will be set.
 * @param mode - The mode to be set.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22196_set_mode(struct max22196_desc *desc, uint32_t ch,
		      enum max22196_mode mode)
{
	if (ch > desc->max_chn_nb - 1)
		return -EINVAL;

	return max22196_reg_update(desc, MAX22196_CFG_REG(ch),
				   MAX22196_SOURCE_MASK,
				   no_os_field_prep(MAX22196_SOURCE_MASK, mode));
}

/**
 * @brief - MAX22196 channel configuration function
 * @param desc - The device descriptor for MAX22196.
 * @param ch - Channel for which the configuration is to be changed.
 * @param hi_thr - Voltage threshold(0 for inactive, 1 for active).
 * @param curr - Channel current to be set.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22196_chan_cfg(struct max22196_desc *desc, uint32_t ch, uint32_t hi_thr,
		      enum max22196_curr curr)
{
	if (ch > desc->max_chn_nb - 1)
		return -EINVAL;

	return max22196_reg_update(desc, MAX22196_CFG_REG(ch),
				   MAX22196_CFG_MASK,
				   no_os_field_prep(MAX22196_HITHR_MASK, hi_thr)
				   | no_os_field_prep(MAX22196_CURR_MASK, curr));
}

/**
 * @brief - MAX22196 filter set function
 * @param desc - MAX22196 device descriptor.
 * @param ch - Channel to which the filter values will be applied to.
 * @param flt_en - Filter enable value(0 for filter disabled,
 * 		   1 for filter enable)
 * @param clr_filtr - Clear filter and set delay value to mid-scale.
 * @param delay - Filter delay to be set.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22196_filter_set(struct max22196_desc *desc, uint32_t ch,
			uint32_t flt_en, uint32_t clr_filtr,
			enum max22196_delay delay)
{
	int ret;

	if (ch > desc->max_chn_nb - 1)
		return -EINVAL;

	ret =  max22196_reg_update(desc, MAX22196_CFG_REG(ch),
				   MAX22196_CFG_MASK,
				   no_os_field_prep(MAX22196_DELAY_MASK, delay) |
				   no_os_field_prep(MAX22196_FLTEN_MASK, flt_en));
	if (ret)
		return ret;

	return max22196_reg_update(desc, MAX22196_GLOBALCFG_REG,
				   MAX22196_FILTER_CLRFLT_MASK,
				   no_os_field_prep(MAX22196_FILTER_CLRFLT_MASK,
						   clr_filtr));
}

/**
 * @brief - MAX22196 filter get function
 * @param desc - MAX22196 device descriptor.
 * @param ch - Channel from which the filter values are read.
 * @param flt_en - Variable through which the fillter's state is read.
 * @param clr_filtr - Clear the filter value and set to mid-scale delay.
 * @param delay - Filter delay value to be read for the MAX22196's specified
 * 		  channel.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22196_filter_get(struct max22196_desc *desc, uint32_t ch,
			uint32_t *flt_en, uint32_t *clr_filtr,
			enum max22196_delay *delay)
{
	int ret;
	uint32_t del_val;

	if (ch > desc->max_chn_nb - 1)
		return -EINVAL;

	ret = max22196_reg_read(desc, MAX22196_CFG_REG(ch), &del_val);
	if (ret)
		return ret;

	*delay = no_os_field_get(MAX22196_DELAY_MASK, del_val);
	*flt_en = no_os_field_get(MAX22196_FLTEN_MASK, del_val);

	ret = max22196_reg_read(desc, MAX22196_GLOBALCFG_REG, &del_val);
	if (ret)
		return ret;

	*clr_filtr = no_os_field_get(MAX22196_FILTER_CLRFLT_MASK, del_val);

	return 0;
}

/**
 * @brief Set fault mask bits in the fault mask registers.
 * @param desc - MAX22196 device descriptor.
 * @param fault_mask - Requested fault to set mask to.
 * @param enabled - Mask state to be set.
 * @return - 0 in case of succes, negative error code otherwise.
*/
int max22196_fault_mask_set(struct max22196_desc *desc,
			    enum max22196_fault_mask fault_mask,
			    bool enabled)
{
	int ret;

	switch (fault_mask) {
	case MAX22196_GLOBAL_REFDISHTCFG:
		return max22196_reg_update(desc, MAX22196_GLOBALCFG_REG,
					   MAX22196_FAULT_MASK(fault_mask),
					   no_os_field_prep(MAX22196_FAULT_MASK(fault_mask),
							   enabled));
	case MAX22196_FAULT1_VMLOW ... MAX22196_FAULT1_FAULT2:
		return max22196_reg_update(desc, MAX22196_F1MASK_REG,
					   MAX22196_FAULT_MASK(fault_mask),
					   no_os_field_prep(MAX22196_FAULT_MASK(fault_mask),
							   enabled));
	case MAX22196_FAULT2_RFDIS ... MAX22196_FAULT2_VAUV:
		ret = max22196_reg_update(desc, MAX22196_F1MASK_REG,
					  MAX22196_FAULT_MASK(fault_mask - MAX22196_FAULT2_RFDIS),
					  no_os_field_prep(MAX22196_FAULT_MASK(fault_mask - MAX22196_FAULT2_RFDIS),
							  enabled));
		if (ret)
			return ret;

		desc->fault2en &= ~MAX22196_FAULT_MASK(fault_mask -
						       MAX22196_FAULT2_RFDIS);
		desc->fault2en |= MAX22196_FAULT_MASK(fault_mask -
						      MAX22196_FAULT2_RFDIS)
				  & no_os_field_prep(MAX22196_FAULT_MASK(fault_mask -
						     MAX22196_FAULT2_RFDIS),
						     enabled);
		desc->fault2en &= MAX22196_FAULT2_MASK;

		return max22196_reg_update(desc, MAX22196_F1MASK_REG,
					   MAX22196_FAULT_MASK(MAX22196_FAULT1_FAULT2),
					   no_os_field_prep(MAX22196_FAULT_MASK(MAX22196_FAULT1_FAULT2),
							   !!desc->fault2en));
	default:
		return -EINVAL;
	}
}


/**
 * @brief Get fault mask bits from the fault mask registers.
 * @param desc - MAX22196 device descriptor.
 * @param fault_mask - Requested fault to get mask from.
 * @param enabled - Mask state to be read.
 * @return - 0 in case of succes, negative error code otherwise.
*/
int max22196_fault_mask_get(struct max22196_desc *desc,
			    enum max22196_fault_mask fault_mask,
			    bool *enabled)
{
	int ret;
	uint32_t reg_val;

	switch (fault_mask) {
	case MAX22196_GLOBAL_REFDISHTCFG:
		ret = max22196_reg_read(desc, MAX22196_GLOBALCFG_REG, &reg_val);
		if (ret)
			return ret;
		*enabled = no_os_field_get(MAX22196_FAULT_MASK(fault_mask), reg_val);

		break;
	case MAX22196_FAULT1_VMLOW ... MAX22196_FAULT1_FAULT2:
		ret =  max22196_reg_read(desc, MAX22196_F1MASK_REG, &reg_val);
		if (ret)
			return ret;
		*enabled = no_os_field_get(MAX22196_FAULT_MASK(fault_mask),
					   reg_val);
		break;
	case MAX22196_FAULT2_RFDIS ... MAX22196_FAULT2_VAUV:
		ret =  max22196_reg_read(desc, MAX22196_F2MASK_REG, &reg_val);
		if (ret)
			return ret;
		*enabled = no_os_field_get(MAX22196_FAULT_MASK(fault_mask -
					   MAX22196_FAULT2_RFDIS),
					   reg_val);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Set global configuration.
 * @param desc - MAX22196 device descriptor.
 * @param global_cfg - Specifc configuration to be set.
 * @param enabled - State of the specific configuration.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22196_global_cfg(struct max22196_desc *desc,
			enum max22196_global_cfg global_cfg, bool enabled)
{
	switch (global_cfg) {
	case MAX22196_GLOBAL_FSPICLR ... MAX22196_GLOBAL_GPO:
		return max22196_reg_update(desc, MAX22196_GLOBALCFG_REG,
					   MAX22196_GLOBAL_MASK(global_cfg),
					   no_os_field_prep(MAX22196_GLOBAL_MASK(global_cfg),
							   enabled));
	default:
		return -EINVAL;
	}
}

/**
 * @brief - MAX22196 channel counter set function
 * @param desc - MAX22196 device descriptor.
 * @param ch - MAX22196 channel to which the counter is set.
 * @param cnt_msb_lsb_bytes - 8MSBs represent the MSB byte to be written to the
 *			      specifc chanel's counter, 8LSBs represent the LSB
 *			      byte.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22196_set_chan_cnt(struct max22196_desc *desc, uint32_t ch,
			  uint16_t cnt_msb_lsb_bytes)
{
	int ret;
	uint8_t msb_byte, lsb_byte;

	if (desc->chip_id == ID_MAX22194)
		return -EINVAL;

	if (ch > desc->max_chn_nb - 1)
		return -EINVAL;

	ret = max22196_reg_update(desc, MAX22196_START_STOP_REG,
				  MAX22196_CNT_MASK(ch),
				  no_os_field_prep(MAX22196_CNT_MASK(ch), 0));
	if (ret)
		return ret;

	msb_byte = no_os_field_get(MAX22196_MSB_MASK, cnt_msb_lsb_bytes);
	msb_byte = no_os_field_prep(MAX22196_CNT_BYTE_MASK, msb_byte);

	ret = max22196_reg_write(desc, MAX22196_CNT_MSB_REG(ch),
				 msb_byte);
	if (ret)
		return ret;

	lsb_byte = no_os_field_get(MAX22196_LSB_MASK, cnt_msb_lsb_bytes);
	lsb_byte = no_os_field_prep(MAX22196_CNT_BYTE_MASK, lsb_byte);

	ret = max22196_reg_write(desc, MAX22196_CNT_LSB_REG(ch),
				 lsb_byte);
	if (ret)
		return ret;

	return max22196_reg_update(desc, MAX22196_START_STOP_REG,
				   MAX22196_CNT_MASK(ch),
				   no_os_field_prep(MAX22196_CNT_MASK(ch), 1));
}

/**
 * @brief MAX22196 channel counter get function
 * @param desc - MAX22196 device descriptor.
 * @param ch - Channel for which the channel counter is read.
 * @param cnt_msb_lsb_bytes - 8MSBs represent the MSB byte read from the specifc
 *			      chanel's counter, 8LSBs represent the LSB byte.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22196_get_chan_cnt(struct max22196_desc *desc, uint32_t ch,
			  uint16_t *cnt_msb_lsb_bytes)
{
	int ret;
	uint32_t reg_val;

	if (desc->chip_id == ID_MAX22194)
		return -EINVAL;

	if (ch > desc->max_chn_nb - 1)
		return -EINVAL;

	ret = max22196_reg_update(desc, MAX22196_START_STOP_REG,
				  MAX22196_CNT_MASK(ch),
				  no_os_field_prep(MAX22196_CNT_MASK(ch), 0));
	if (ret)
		return ret;

	ret = max22196_reg_read(desc, MAX22196_CNT_LSB_REG(ch), &reg_val);
	if (ret)
		return ret;

	*cnt_msb_lsb_bytes = no_os_field_prep(MAX22196_LSB_MASK, reg_val);

	ret = max22196_reg_read(desc, MAX22196_CNT_MSB_REG(ch), &reg_val);
	if (ret)
		return ret;

	*cnt_msb_lsb_bytes |= no_os_field_prep(MAX22196_MSB_MASK, reg_val);

	return max22196_reg_update(desc, MAX22196_START_STOP_REG,
				   MAX22196_CNT_MASK(ch),
				   no_os_field_prep(MAX22196_CNT_MASK(ch), 1));
}

/**
 * @brief Initialize and configure the MAX22196 device.
 * @param desc - device descriptor for the MAX22196 that will be initialized.
 * @param param - initialization parameter for the device.
 * @return 0 in case of success, negative error code otherwise.
 */
int max22196_init(struct max22196_desc **desc,
		  struct max22196_init_param *param)
{
	struct max22196_desc *descriptor;
	uint32_t reg_val;
	int ret;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;


	ret = no_os_spi_init(&descriptor->comm_desc, param->comm_param);
	if (ret)
		goto error;

	if (param->crc_en) {
		ret = no_os_gpio_get(&descriptor->crc_desc, param->crc_param);
		if (ret)
			goto error;

		ret = no_os_gpio_direction_output(descriptor->crc_desc,
						  NO_OS_GPIO_HIGH);
		if (ret)
			goto error;

		descriptor->crc_en = true;
	}

	descriptor->chip_id = param->chip_id;
	descriptor->max_chn_nb = param->chip_id ? MAX22196_CHANNELS : MAX22194_CHANNELS;

	/* Clear the latched faults generated at power-up of MAX22196. */
	ret = max22196_reg_read(descriptor, MAX22196_FAULT1_REG, &reg_val);
	if (ret)
		goto error;

	ret = max22196_reg_read(descriptor, MAX22196_FAULT2_REG, &reg_val);
	if (ret)
		goto error;

	*desc = descriptor;

	return 0;

error:
	max22196_remove(descriptor);

	return ret;
}

/**
 * @brief Free the resources allocated during init.
 * @param desc - device descriptor for the MAX14916 that will be initialized.
 * @return 0 in case of success, negative error code otherwise.
 */
int max22196_remove(struct max22196_desc *desc)
{
	int ret, i;

	if (!desc)
		return -ENODEV;

	if (desc->comm_desc) {
		for (i = 0; i < desc->max_chn_nb; i++) {
			ret = max22196_set_chan_cnt(desc, i, MAX22196_CHN_CNT_RESET);
			if (ret)
				return ret;
		}
	}

	no_os_spi_remove(desc->comm_desc);
	no_os_gpio_remove(desc->crc_desc);
	no_os_free(desc);

	return 0;
}
