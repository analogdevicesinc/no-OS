/***************************************************************************//**
 *   @file   max22190.c
 *   @brief  Source file of MAX22190 Driver.
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
#include "max22190.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

/**
 * @brief Compute the CRC5 value for MAX22190
 * @param data - Data array to calculate CRC for.
 * @return CRC result.
*/
static uint8_t max22190_crc(uint8_t *data)
{
	int length = 19;
	uint8_t crc_step, tmp;
	uint8_t crc_init = 0x7;
	uint8_t crc_poly = 0x35;
	int i;

	/**
	 * This is the C custom implementation of CRC function for MAX22190, and
	 * can be found here:
	 * https://www.analog.com/en/design-notes/guidelines-to-implement-crc-algorithm.html
	*/
	uint32_t datainput = (uint32_t)((data[0] << 16) + (data[1] << 8) + data[2]);

	datainput = (datainput & 0xFFFFE0) + crc_init;

	tmp = (uint8_t)((datainput & 0xFC0000) >> 18);

	if ((tmp & 0x20) == 0x20)
		crc_step = (uint8_t)(tmp ^ crc_poly);
	else
		crc_step = tmp;

	for (i = 0; i < length - 1; i++) {
		tmp = (uint8_t)(((crc_step & 0x1F) << 1) +
				((datainput >> (length -2 - i)) & 0x01));

		if ((tmp & 0x20) == 0x20)
			crc_step = (uint8_t)(tmp ^ crc_poly);
		else
			crc_step = tmp;
	}

	return (uint8_t)(crc_step & 0x1F);
}

/**
 * @brief Set filter delay, wite break detection and if the filter is used or
 * 	  bypassed.
 * @param desc - MAX22190 device descriptor.
 * @param ch - The channel to which the filter corresponds(0 based).
 * @param clrf - Filter operation  (0 - normal operation, 1 - filters fixed at
 * 				    mid -scale value for the chosen delay).
 * @param fbp - Filter usage(used - 0, bypassed - 1).
 * @param delay - Filter delay.
 * @return 0 in case of success, negative error code otherwise.
*/
int max22190_filter_set(struct max22190_desc *desc, uint32_t ch, uint32_t clrf,
			uint32_t fbp, enum max22190_delay delay)
{
	int ret;

	if (ch > MAX22190_CHANNELS - 1)
		return -EINVAL;

	ret = max22190_reg_update(desc, MAX22190_FILTER_IN_REG(ch),
				  MAX22190_FBP_MASK | MAX22190_DELAY_MASK,
				  no_os_field_prep(MAX22190_FBP_MASK, fbp) |
				  no_os_field_prep(MAX22190_DELAY_MASK, delay));
	if (ret)
		return ret;

	return max22190_reg_update(desc, MAX22190_CONFIG_REG,
				   MAX22190_CFG_CLRF_MASK,
				   no_os_field_prep(MAX22190_CFG_CLRF_MASK,
						   clrf));
}

/**
 * @brief Get filter delay, wite break detection and if the filter is used or
 * 	  bypassed.
 * @param desc - MAX22190 device descriptor.
 * @param ch - The channel to which the filter corresponds(0 based).
 * @param clrf - Filter operation  (0 - normal operation, 1 - filters fixed at
 * 				    mid -scale value for the chosen delay).
 * @param fbp - Filter usage(used - 0, bypassed - 1).
 * @param delay - Filter delay.
 * @return 0 in case of success, negative error code otherwise.
*/
int max22190_filter_get(struct max22190_desc *desc, uint32_t ch, uint32_t *clrf,
			uint32_t *fbp, enum max22190_delay *delay)
{
	int ret;
	uint32_t read_byte;

	if (ch > MAX22190_CHANNELS - 1)
		return -EINVAL;

	ret = max22190_reg_read(desc, MAX22190_FILTER_IN_REG(ch), &read_byte);
	if (ret)
		return ret;

	*fbp = no_os_field_get(MAX22190_FBP_MASK, read_byte);
	*delay = no_os_field_get(MAX22190_DELAY_MASK, read_byte);

	ret = max22190_reg_read(desc, MAX22190_CONFIG_REG, &read_byte);
	if (ret)
		return ret;

	*clrf = no_os_field_get(MAX22190_CFG_CLRF_MASK, read_byte);

	return 0;
}

/**
 * @brief Switch selected channel's state to disabled or enabled.
 * @param desc - MAX22190 device descriptor.
 * @param ch - Channel to disable/enable(0 based).
 * @param state - The state requested.
 * @return 0 in case of success, negative error code otherwise.
*/
int max22190_chan_state(struct max22190_desc *desc, uint32_t ch,
			enum max22190_ch_state state)
{
	int ret;

	if (ch > MAX22190_CHANNELS - 1)
		return -EINVAL;

	ret = max22190_reg_update(desc, MAX22190_INPUT_EN_REG,
				  MAX22190_CH_STATE_MASK(ch), state);
	if (ret)
		return ret;

	desc->channels[ch] = state;

	return 0;
}

/**
 * @brief Get Wire Break detection for requested channel
 * @param desc - MAX22190 device descriptor.
 * @param ch - Channel to read the WBE from.
 * @param enabled - State of the WBE for the specific channel returned after
 * 		    reading the register.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22190_wbe_get(struct max22190_desc *desc, uint32_t ch, bool *enabled)
{
	int ret;
	uint32_t reg_val;

	if (ch > MAX22190_CHANNELS - 1)
		return -EINVAL;

	ret = max22190_reg_read(desc, MAX22190_FILTER_IN_REG(ch), &reg_val);
	if (ret)
		return ret;

	*enabled = no_os_field_get(MAX22190_FAULT2_WBE_MASK, reg_val);

	return 0;
}

/**
 * @brief Set Wire Break detection for requested channel
 * @param desc - MAX22190 device descriptor.
 * @param ch - Channel to set the WBE to.
 * @param enabled - State of the WBE for the specific channel to be set.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22190_wbe_set(struct max22190_desc *desc, uint32_t ch, bool enabled)
{
	if (ch > MAX22190_CHANNELS - 1)
		return -EINVAL;

	return max22190_reg_update(desc, MAX22190_FILTER_IN_REG(ch),
				   MAX22190_FAULT2_WBE_MASK,
				   no_os_field_prep(MAX22190_FAULT2_WBE_MASK,
						   enabled));
}

/**
 * @brief Get fault enable from the fault registers
 * @param desc - MAX22190 device descriptor
 * @param fault_enable - Specific fault enable to read
 * @param enabled - State of the selected enable (0 for disabled, 1 for enabled)
 * @return 0 in case of succes, negative error code otherwise
*/
int max22190_fault_enable_get(struct max22190_desc *desc,
			      enum max22190_fault_enable fault_enable,
			      bool *enabled)
{
	int ret;
	uint32_t reg_val;

	switch(fault_enable) {
	case MAX22190_FAULT1_WBGE ... MAX22190_FAULT1_FAULT2E:
		ret = max22190_reg_read(desc, MAX22190_FAULT1_EN_REG, &reg_val);
		if (ret)
			return ret;
		*enabled = no_os_field_get(MAX22190_FAULT_MASK(fault_enable),
					   reg_val);
		break;
	case MAX22190_FAULT2_RFWBSE ... MAX22190_FAULT2_FAULT8CKE:
		ret = max22190_reg_read(desc, MAX22190_FAULT2_EN_REG, &reg_val);
		if (ret)
			return ret;
		*enabled = no_os_field_get(MAX22190_FAULT_MASK(fault_enable -
					   MAX22190_FAULT2_RFWBSE), reg_val);
		break;
	case MAX22190_CFG_24VF:
		ret = max22190_reg_read(desc, MAX22190_CONFIG_REG, &reg_val);
		if (ret)
			return ret;
		*enabled = no_os_field_get(MAX22190_CFG_24VF_MASK, reg_val);
		break;
	case MAX22190_CFG_REFDI_SH_ENA:
		ret = max22190_reg_read(desc, MAX22190_CONFIG_REG, &reg_val);
		if (ret)
			return ret;
		*enabled = no_os_field_get(MAX22190_CFG_REFDI_MASK, reg_val);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Set fault enable in the fault registers
 * @param desc - MAX22190 device descriptor
 * @param fault_enable - Specific fault enable to write
 * @param enabled - State of the selected enable (0 for disabled, 1 for enabled)
 * @return 0 in case of succes, negative error code otherwise
*/
int max22190_fault_enable_set(struct max22190_desc *desc,
			      enum max22190_fault_enable fault_enable,
			      bool enabled)
{
	int ret;

	switch(fault_enable) {
	case MAX22190_FAULT1_WBGE ... MAX22190_FAULT1_FAULT2E:
		return max22190_reg_update(desc, MAX22190_FAULT1_EN_REG,
					   MAX22190_FAULT_MASK(fault_enable),
					   no_os_field_prep(MAX22190_FAULT_MASK(fault_enable),
							   enabled));
	case MAX22190_FAULT2_RFWBSE ... MAX22190_FAULT2_FAULT8CKE:
		ret = max22190_reg_update(desc, MAX22190_FAULT2_EN_REG,
					  MAX22190_FAULT_MASK(fault_enable -
							  MAX22190_FAULT2_RFWBSE),
					  no_os_field_prep(MAX22190_FAULT_MASK(fault_enable -
							  MAX22190_FAULT2_RFWBSE),
							  enabled));
		if (ret)
			return ret;

		/* Setting the bit corresponding to the specific fault enable
		   in the fault2en variable of the descriptor. */
		desc->fault2en &= ~MAX22190_FAULT_MASK(fault_enable -
						       MAX22190_FAULT2_RFWBSE);
		desc->fault2en |= MAX22190_FAULT_MASK(fault_enable -
						      MAX22190_FAULT2_RFWBSE)
				  & no_os_field_prep(MAX22190_FAULT_MASK(fault_enable -
						     MAX22190_FAULT2_RFWBSE),
						     enabled);
		desc->fault2en &= MAX22190_FAULT2_EN_MASK;

		return max22190_reg_update(desc, MAX22190_FAULT1_EN_REG,
					   MAX22190_FAULT_MASK(MAX22190_FAULT1_FAULT2E),
					   no_os_field_prep(MAX22190_FAULT_MASK(MAX22190_FAULT1_FAULT2E),
							   !!desc->fault2en));
	case MAX22190_CFG_24VF:
		return max22190_reg_update(desc, MAX22190_CONFIG_REG,
					   MAX22190_CFG_24VF_MASK,
					   no_os_field_prep(MAX22190_CFG_24VF_MASK,
							   enabled));
	case MAX22190_CFG_REFDI_SH_ENA:
		return max22190_reg_update(desc, MAX22190_CONFIG_REG,
					   MAX22190_CFG_REFDI_MASK,
					   no_os_field_prep(MAX22190_CFG_REFDI_MASK,
							   enabled));
	default:
		return -EINVAL;
	}
}

/**
 * @brief Register read function for MAX22190
 * @param desc - MAX22190 device descriptor.
 * @param addr - Register address from which data is read.
 * @param val - The value received from the read operation.
 * @return 0 in case of success, negative error code otherwise.
*/
int max22190_reg_read(struct max22190_desc *desc, uint32_t addr, uint32_t *val)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = desc->buff,
		.rx_buff = desc->buff,
		.bytes_number = MAX22190_FRAME_SIZE,
		.cs_change = 1,
	};
	uint8_t crc;
	int ret;

	if (desc->crc_en)
		xfer.bytes_number++;

	memset(desc->buff, 0, xfer.bytes_number);
	desc->buff[0] = no_os_field_prep(MAX22190_ADDR_MASK, addr) |
			no_os_field_prep(MAX22190_RW_MASK, 0);

	if (desc->crc_en)
		desc->buff[2] = max22190_crc(&desc->buff[0]);

	ret = no_os_spi_transfer(desc->comm_desc, &xfer, 1);
	if (ret)
		return ret;

	if (desc->crc_en) {
		crc = max22190_crc(&desc->buff[0]);
		if (crc != (desc->buff[2] & 0x1F))
			return -EINVAL;
	}

	*val = desc->buff[1];

	return 0;
}

/**
 * @brief Register write function for MAX22190
 * @param desc - MAX22190 device descriptor.
 * @param addr - Register value to which data is written.
 * @param val - Value which is to be written to requested register.
 * @return 0 in case of success, negative error code otherwise.
*/
int max22190_reg_write(struct max22190_desc *desc, uint32_t addr, uint32_t val)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = desc->buff,
		.bytes_number = MAX22190_FRAME_SIZE,
		.cs_change = 1,
	};

	if (desc->crc_en)
		xfer.bytes_number++;

	memset(desc->buff, 0, xfer.bytes_number);
	desc->buff[0] = no_os_field_prep(MAX22190_ADDR_MASK, addr) |
			no_os_field_prep(MAX22190_RW_MASK, 1);
	desc->buff[1] = val;

	if (desc->crc_en)
		desc->buff[2] = max22190_crc(desc->buff);

	return no_os_spi_transfer(desc->comm_desc, &xfer, 1);
}

/**
 * @brief Register update function for MAX22190
 * @param desc - MAX22190 device descriptor.
 * @param addr - Register valueto wich data is updated.
 * @param mask - Corresponding mask to the data that will be updated.
 * @param val - Updated value to be written in the register at update.
 * @return 0 in case of success, negative error code otherwise.
*/
int max22190_reg_update(struct max22190_desc *desc, uint32_t addr,
			uint32_t mask, uint32_t val)
{
	int ret;
	uint32_t reg_val = 0;

	ret = max22190_reg_read(desc, addr, &reg_val);
	if (ret)
		return ret;

	reg_val &= ~mask;
	reg_val |= mask & val;

	return max22190_reg_write(desc, addr, reg_val);
}

/**
 * @brief MAX22190 Initialization function
 * @param desc - MAX22190 device descriptor.
 * @param param - MAX22190 initalization parameter.
 * @return 0 in case of success, negative error code otherwise.
*/
int max22190_init(struct max22190_desc **desc,
		  struct max22190_init_param *param)
{
	struct max22190_desc *descriptor;
	int ret;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_spi_init(&descriptor->comm_desc, param->comm_param);
	if (ret)
		goto err;

	descriptor->crc_en = param->crc_en;

	ret = no_os_gpio_get_optional(&descriptor->en_gpio,
				      param->en_gpio_param);
	if (ret)
		goto spi_err;

	if (descriptor->en_gpio) {
		ret = no_os_gpio_set_value(descriptor->en_gpio,
					   NO_OS_GPIO_HIGH);
		if (ret)
			goto gpio_err;
	}

	/* Clearing latched faults generated on power-up and setting POR bit to
	   0. */
	ret = max22190_reg_update(descriptor, MAX22190_FAULT1_REG,
				  MAX22190_POR_MASK,
				  no_os_field_prep(MAX22190_POR_MASK, 0));
	if (ret)
		goto gpio_err;

	/* Fault 2 Enable Register is 0 at power-up except RFWBO pin. */
	descriptor->fault2en = 0;

	*desc = descriptor;

	return 0;

gpio_err:
	no_os_gpio_remove(descriptor->en_gpio);
spi_err:
	no_os_spi_remove(descriptor->comm_desc);
err:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free the resources allocated by initialization
 * @param desc - MAX22190 device descriptor.
 * @return 0 in case of success, negative error code otherwise.
*/
int max22190_remove(struct max22190_desc *desc)
{
	int ret;
	int i;

	if (!desc)
		return -ENODEV;

	for(i = 0; i < MAX22190_CHANNELS; i++) {
		ret = max22190_chan_state(desc, i, MAX22190_CH_OFF);
		if (ret)
			return ret;
	}

	no_os_spi_remove(desc->comm_desc);
	no_os_gpio_remove(desc->en_gpio);
	no_os_free(desc);

	return 0;
}
