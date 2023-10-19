/***************************************************************************//**
 *   @file   max22200.c
 *   @brief  Source file of MAX22200 Driver.
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
#include <no_os_error.h>
#include <stdlib.h>
#include <string.h>
#include "max22200.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"

/**
 * @brief Read data from desired register for MAX22200
 * @param desc - MAX22200 device descriptor.
 * @param reg - Register address to read data from.
 * @param val - Value read from the register.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22200_reg_read(struct max22200_desc *desc, uint32_t reg, uint32_t *val)
{
	int ret;
	struct no_os_spi_msg xfer = {
		.tx_buff = desc->buff,
		.rx_buff = desc->buff,
		.bytes_number = 1,
		.cs_change = 1,
	};

	desc->buff[0] = no_os_field_prep(MAX22200_RW_MASK, 0) |
			no_os_field_prep(MAX22200_ADDR_MASK, reg);

	ret = no_os_gpio_set_value(desc->cmd_desc, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	ret = no_os_spi_transfer(desc->comm_desc, &xfer, 1);
	if (ret)
		return ret;

	if (!no_os_field_get(reg, MAX22200_R1B))
		xfer.bytes_number = MAX22200_FRAME_SIZE;

	ret = no_os_gpio_set_value(desc->cmd_desc, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	ret = no_os_spi_transfer(desc->comm_desc, &xfer, 1);
	if (ret)
		return ret;

	if (no_os_field_get(reg, MAX22200_R1B))
		*val = desc->buff[0];
	else
		*val = no_os_get_unaligned_be32(desc->buff);

	return 0;
}

/**
 * @brief Write data to desired register for MAX22200
 * @param desc - MAXX22200 device descriptor.
 * @param reg - Register address to write data to.
 * @param val - Value to be written to the register.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22200_reg_write(struct max22200_desc *desc, uint32_t reg, uint32_t val)
{
	int ret;
	struct no_os_spi_msg xfer = {
		.tx_buff = desc->buff,
		.rx_buff = desc->buff,
		.bytes_number = 1,
		.cs_change = 1,
	};

	if (no_os_field_get(reg, MAX22200_R1B) && val > 0xFF)
		return -EINVAL;

	desc->buff[0] = no_os_field_prep(MAX22200_RW_MASK, 1) |
			no_os_field_prep(MAX22200_ADDR_MASK, reg);

	ret = no_os_gpio_set_value(desc->cmd_desc, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	ret = no_os_spi_transfer(desc->comm_desc, &xfer, 1);
	if (ret)
		return ret;

	if (!no_os_field_get(reg, MAX22200_R1B)) {
		xfer.bytes_number = MAX22200_FRAME_SIZE;
		no_os_put_unaligned_le32(val, desc->buff);
	} else {
		desc->buff[0] = val;
	}

	ret = no_os_gpio_set_value(desc->cmd_desc, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	return no_os_spi_transfer(desc->comm_desc, &xfer, 1);
}

/**
 * @brief Update data in the desired register.
 * @param desc - MAX22200 device descriptor.
 * @param reg - Register address to update data to.
 * @param mask - Bit mask of the field to be updated.
 * @param val - Value to be updated in the desired register.
 * 		Should be bit shifted by using no_os_field_prep(mask, val)
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22200_reg_update(struct max22200_desc *desc, uint32_t reg, uint32_t mask,
			uint32_t val)
{
	int ret;
	uint32_t reg_val = 0;

	ret = max22200_reg_read(desc, reg, &reg_val);
	if (ret)
		return ret;

	reg_val &= ~mask;
	reg_val |= mask & val;

	return max22200_reg_write(desc, reg, reg_val);
}

/**
 * @brief Set external trigger's state of the MAX22200
 * @param desc - MAX22200 device descriptor.
 * @param trig_state - State to change to.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22200_set_trig_state(struct max22200_desc *desc, bool trig_state)
{
	if (!desc->trig_desc)
		return -EINVAL;

	return no_os_gpio_set_value(desc->trig_desc,
				    trig_state ? NO_OS_GPIO_HIGH : NO_OS_GPIO_LOW);
}

/**
 * @brief Set channel state for specific channel
 * @param desc - MAX22200 device descriptor.
 * @param ch - Channel number.
 * @param ch_state - Channel's state to be changed into.
 * @return 0 in case of succces, negative error code otherwise.
*/
int max22200_set_ch_state(struct max22200_desc *desc, uint32_t ch,
			  bool ch_state)
{
	if (ch > MAX22200_MAX_CHN_IDX)
		return -EINVAL;

	return max22200_reg_update(desc, MAX22200_STATUS_REG, MAX22200_ONCH_MASK(ch),
				   no_os_field_prep(MAX22200_ONCH_MASK(ch), ch_state));
}

/**
 * @brief Set fault mask bits in the status register.
 * @param desc - MAX22200 device descriptor.
 * @param fault_mask - Desired fault mask bit to set.
 * @param enabled - enabled/disabled state of the mask.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22200_fault_mask_set(struct max22200_desc *desc,
			    enum max22200_fault_mask fault_mask, bool enabled)
{
	if (fault_mask == MAX22200_M_HHF && desc->chan_drive == MAX22200_VOLTAGE_DRIVE)
		return -EINVAL;

	return max22200_reg_update(desc, MAX22200_STATUS_REG,
				   MAX22200_STATUS_FAULT_MASK(fault_mask),
				   no_os_field_prep(MAX22200_STATUS_FAULT_MASK(fault_mask), enabled));
}

/**
 * @brief Set chopping frequency value.
 * @param desc - MAX22200 device descriptor.
 * @param chopping_freq - Chopping frequency value.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22200_set_chop_freq(struct max22200_desc *desc,
			   enum max22200_chopping_freq chopping_freq)
{
	return max22200_reg_update(desc, MAX22200_STATUS_REG, MAX22200_STATUS_FREQ_MASK,
				   no_os_field_prep(MAX22200_STATUS_FREQ_MASK, chopping_freq));
}

/**
 * @brief Set channel HIT time and HIT current.
 * @param desc - MAX22200 device descriptor.
 * @param ch - Channel for which HIT time is to be set.(0 based)
 * @param hit_current - HIT current value, it shall not be greater than 127
 * 			since the value is stored only on 7 bits in the
 * 			channel's configuration register.
 * @param hit_time - HIT time value to be set.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22200_set_ch_hit(struct max22200_desc *desc, uint32_t ch,
			uint8_t hit_current, uint8_t hit_time)
{
	int ret;

	if (ch > MAX22200_MAX_CHN_IDX)
		return -EINVAL;

	if (hit_current > MAX22200_HIT_MAX_VAL)
		return -EINVAL;

	ret = max22200_reg_update(desc, MAX22200_CFG_CH(ch), MAX22200_HIT_MASK,
				  no_os_field_prep(MAX22200_HIT_MASK, (uint32_t)hit_current));
	if (ret)
		return ret;

	return max22200_reg_update(desc, MAX22200_CFG_CH(ch), MAX22200_HIT_T_MASK,
				   no_os_field_prep(MAX22200_HIT_T_MASK, (uint32_t)hit_time));
}

/**
 * @brief Set channel hold current
 * @param desc - MAX22200 device descriptor.
 * @param ch - Channel's number (0 based).
 * @param hold_current - Hold current value to be set. It shall not be greater
 * 			 than 127 since the value it's stored on 7 bits in
 * 			 the channel's configuration register.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22200_set_ch_hold(struct max22200_desc *desc, uint32_t ch,
			 uint8_t hold_current)
{
	if (ch > MAX22200_MAX_CHN_IDX)
		return -EINVAL;

	if (hold_current > MAX22200_HIT_MAX_VAL)
		return -EINVAL;

	return max22200_reg_update(desc, MAX22200_CFG_CH(ch), MAX22200_HOLD_MASK,
				   no_os_field_prep(MAX22200_HOLD_MASK, (uint32_t)hold_current));
}

/**
 * @brief Set channel's scale to fullscale or half fullscale.
 * @param desc - MAX22200 device dscriptor.
 * @param ch - Channel's number (0 based).
 * @param scale - Scale value.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22200_set_ch_scale(struct max22200_desc *desc, uint32_t ch,
			  enum max22200_scale scale)
{
	if (ch > MAX22200_MAX_CHN_IDX)
		return -EINVAL;

	if (scale == MAX22200_HALF_FULL_SCALE && desc->chan_side == MAX22200_HIGH_SIDE)
		return -EINVAL;

	return max22200_reg_update(desc, MAX22200_CFG_CH(ch), MAX22200_HFS_MASK,
				   no_os_field_prep(MAX22200_HFS_MASK, scale));
}

/**
 * @brief Set channel's trigger to be either SPI or external trigger.
 * @param desc - MAX22200 device descriptor.
 * @param ch - Channel's number (0 based).
 * @param trig - Trigger to be set. If external trigger is requested but no
 * 		 external's trigger descriptor was initialized, will return
 * 		 negative error code.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22200_set_ch_trig(struct max22200_desc *desc, uint32_t ch,
			 enum max22200_trig trig)
{
	if (ch > MAX22200_MAX_CHN_IDX)
		return -EINVAL;

	if (trig == MAX22200_TRIG && !desc->trig_desc)
		return -EINVAL;

	return max22200_reg_update(desc, MAX22200_CFG_CH(ch), MAX22200_TRGNSP_IO_MASK,
				   no_os_field_prep(MAX22200_TRGNSP_IO_MASK, trig));
}

/**
 * @brief Set chanmel operation mode, high-side/low-side and drive.
 * @param desc - MAX22200 device descriptor.
 * @param ch - Channel number (0 based).
 * @param ch_drive - Voltage/Current drive.
 * @param ch_side - High-Side/Low-Side.
 * @param ch_op_mode - Independent/Half-Bridge/Full-Bridge mode.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22200_set_ch_mode(struct max22200_desc *desc, uint32_t ch,
			 enum max22200_ch_drive ch_drive,
			 enum max22200_ch_side ch_side,
			 enum max22200_ch_op_mode ch_op_mode)
{
	int ret;
	uint32_t mask, val;

	if (ch > MAX22200_MAX_CHN_IDX)
		return -EINVAL;

	if (ch_side == MAX22200_HIGH_SIDE && ch_drive == MAX22200_CURRENT_DRIVE)
		return -EINVAL;

	mask = MAX22200_CH_MODE_MASK(ch);
	val = no_os_field_prep(MAX22200_CH_MODE_MASK(ch), (uint32_t)ch_op_mode);

	ret = max22200_reg_update(desc, MAX22200_STATUS_REG, mask, val);
	if (ret)
		return ret;

	/* Channels are configured 2 by 2, either they are configured as
	   Independent, Half-Bridge or Full-Bridge, thereby ch_config[0] is
	   the configuration for channels 0 and 1, ch_config[1] is the
	   configuration for channels 1 and 2, and so on. */
	desc->ch_config[ch / 2] = ch_op_mode;

	mask = MAX22200_VDRNCDR_MASK | MAX22200_HSNLS_MASK;
	val = no_os_field_prep(MAX22200_VDRNCDR_MASK, (uint32_t)ch_drive) |
	      no_os_field_prep(MAX22200_HSNLS_MASK, (uint32_t)ch_side);

	ret = max22200_reg_update(desc, MAX22200_CFG_CH(ch) | MAX22200_R1B,
				  mask, val);
	if (ret)
		return ret;

	desc->chan_side = ch_side;

	return 0;
}

/**
 * @brief Set channel's frequency.
 * @param desc - MAX22200 device descriptor
 * @param ch - Channel's number (0 based).
 * @param ch_freq - 80KHz/100KHz frequency.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22200_set_ch_freq(struct max22200_desc *desc, uint32_t ch,
			 enum max22200_ch_freq ch_freq)
{
	if (ch > MAX22200_MAX_CHN_IDX)
		return -EINVAL;

	return max22200_reg_update(desc, MAX22200_CFG_CH(ch) | MAX22200_R1B,
				   MAX22200_FREQ_CFG_MASK,
				   no_os_field_prep(MAX22200_FREQ_CFG_MASK, ch_freq));
}

/**
 * @brief Set channel's enables for different checks, detection and functions.
 * @param desc - MAX22200 device descriptor.
 * @param ch - Channel's number (0 based).
 * @param ch_enable - Channel enable to be set.
 * @param enabled - Disabled/Enabled state.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22200_set_ch_enable(struct max22200_desc *desc, uint32_t ch,
			   enum max22200_ch_enable ch_enable, bool enabled)
{
	if (ch > MAX22200_MAX_CHN_IDX)
		return -EINVAL;

	return max22200_reg_update(desc, MAX22200_CFG_CH(ch) | MAX22200_R1B,
				   MAX22200_CH_ENABLE_MASK(ch_enable),
				   no_os_field_prep(MAX22200_CH_ENABLE_MASK(ch_enable), enabled));
}

/**
 * @brief Set configuration DPM.
 * @param desc - MAX22200 device descriptor.
 * @param dpm_istart_byte - Detection of plunger movement starting current
 * @param dpm_tdeb_ipth_byte - Detection of plunger movement debounce time.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22200_set_cfg_dpm(struct max22200_desc *desc, uint8_t dpm_istart_byte,
			 uint8_t dpm_tdeb_ipth_byte)
{
	uint32_t val, mask;

	if (dpm_istart_byte > MAX22200_HIT_MAX_VAL)
		return -EINVAL;

	mask = MAX22200_DPM_ISTART_MASK | MAX22200_DPM_TDEB_MASK |
	       MAX22200_DPM_IPTH_MASK;
	val = no_os_field_prep(MAX22200_DPM_ISTART_MASK, dpm_istart_byte) |
	      no_os_field_prep(MAX22200_DPM_TDEB_MASK | MAX22200_DPM_IPTH_MASK,
			       dpm_tdeb_ipth_byte);

	return max22200_reg_update(desc, MAX22200_CFG_DPM_REG, mask, val);
}

/**
 * @brief Fault mask bit get function
 * @param desc - MAX22200 device descriptor.
 * @param fault_mask - Fault mask specific bit to read the state for.
 * @param enabled - State of the bit.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22200_fault_mask_get(struct max22200_desc * desc,
			    enum max22200_fault_mask fault_mask, bool *enabled)
{
	int ret;
	uint32_t reg_val;

	ret = max22200_reg_read(desc, MAX22200_STATUS_REG, &reg_val);
	if (ret)
		return ret;

	*enabled = no_os_field_get(MAX22200_STATUS_FAULT_MASK(fault_mask),
				   reg_val);

	return 0;
}

/**
 * @brief Read the state of a channel
 * @param desc - MAX22200 device descriptor
 * @param ch - Channel number (0 based).
 * @param ch_state - State of the channel.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22200_get_ch_state(struct max22200_desc *desc, uint32_t ch,
			  bool *ch_state)
{
	int ret;
	uint32_t reg_val;

	if (ch < MAX22200_CHANNELS - 1)
		return -EINVAL;

	ret = max22200_reg_read(desc, MAX22200_STATUS_REG, &reg_val);
	if (ret)
		return ret;

	*ch_state = no_os_field_get(MAX22200_ONCH_MASK(ch), reg_val);

	return 0;
}

/**
 * @brief Read chopping frequency
 * @param desc - MAX22200 device descriptor.
 * @param chopping_freq - Pointer of the variable holding the value of the
 * 			  chopping frequency.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22200_get_chop_freq(struct max22200_desc *desc,
			   enum max22200_chopping_freq *chopping_freq)
{
	int ret;
	uint32_t reg_val;

	ret = max22200_reg_read(desc, MAX22200_STATUS_REG, &reg_val);
	if (ret)
		return ret;

	*chopping_freq = no_os_field_get(MAX22200_STATUS_FREQ_MASK, reg_val);

	return 0;
}

/**
 * @brief Read channel HIT configuration.
 * @param desc - MAX22200 device descriptor.
 * @param ch - Channel's number (0 based).
 * @param hit_current - HIT current value read from the device.
 * @param hit_time - HIT time value read from the device.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22200_get_ch_hit(struct max22200_desc *desc, uint32_t ch,
			uint8_t *hit_current, uint8_t *hit_time)
{
	int ret;
	uint32_t reg_val;

	if (ch > MAX22200_MAX_CHN_IDX)
		return -EINVAL;

	ret = max22200_reg_read(desc, MAX22200_CFG_CH(ch), &reg_val);
	if (ret)
		return ret;

	*hit_current = no_os_field_get(MAX22200_HIT_MASK, reg_val);
	*hit_time = no_os_field_get(MAX22200_HIT_T_MASK, reg_val);

	return 0;
}

/**
 * @brief Read channel HOLD configuration.
 * @param desc - MAX22200 device descriptor.
 * @param ch - Channel's number (0 based).
 * @param hold_current - HOLD current value read from the device.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22200_get_ch_hold(struct max22200_desc *desc, uint32_t ch,
			 uint8_t *hold_current)
{
	int ret;
	uint32_t reg_val;

	if (ch > MAX22200_MAX_CHN_IDX)
		return -EINVAL;

	ret = max22200_reg_read(desc, MAX22200_CFG_CH(ch), &reg_val);
	if (ret)
		return ret;

	*hold_current = no_os_field_get(MAX22200_HOLD_MASK, reg_val);

	return 0;
}

/**
 * @brief Read channel's scale.
 * @param desc - MAX22200 device descriptor.
 * @param ch - Channel's number (0 based).
 * @param scale - Scale read from device's channel.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22200_get_ch_scale(struct max22200_desc *desc, uint32_t ch,
			  enum max22200_scale *scale)
{
	int ret;
	uint32_t reg_val;

	if (ch > MAX22200_MAX_CHN_IDX)
		return -EINVAL;

	ret = max22200_reg_read(desc,MAX22200_CFG_CH(ch), &reg_val);
	if (ret)
		return ret;

	*scale = no_os_field_get(MAX22200_HFS_MASK, reg_val);

	return 0;
}

/**
 * @brief Read channel's selected trigger for specific channel.
 * @param desc - MAX22200 device descriptor.
 * @param ch - Channel's number (0 based).
 * @param trig - Selected trigger for the channel requested read from the
 * 		 device.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22200_get_ch_trig(struct max22200_desc *desc, uint32_t ch,
			 enum max22200_trig *trig)
{
	int ret;
	uint32_t reg_val;

	if (ch > MAX22200_MAX_CHN_IDX)
		return -EINVAL;

	ret = max22200_reg_read(desc, MAX22200_CFG_CH(ch), &reg_val);
	if (ret)
		return ret;

	*trig = no_os_field_get(MAX22200_TRGNSP_IO_MASK, reg_val);

	return 0;
}

/**
 * @brief Read channel's mode configuration
 * @param desc - MAX22200 device descriptor.
 * @param ch - Channel's number (0 based).
 * @param ch_drive - Channel's drive read from the device.
 * @param ch_side - Channel's high-side/low-side configuraton read from the
 * 		    device.
 * @param ch_op_mode - Channel's operation mode read from the device.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22200_get_ch_mode(struct max22200_desc *desc, uint32_t ch,
			 enum max22200_ch_drive *ch_drive,
			 enum max22200_ch_side *ch_side,
			 enum max22200_ch_op_mode *ch_op_mode)
{
	int ret;
	uint32_t reg_val;

	if (ch > MAX22200_MAX_CHN_IDX)
		return -EINVAL;

	ret = max22200_reg_read(desc, MAX22200_CFG_CH(ch) | MAX22200_R1B,
				&reg_val);
	if (ret)
		return ret;

	*ch_drive = no_os_field_get(MAX22200_VDRNCDR_MASK, reg_val);
	*ch_side = no_os_field_get(MAX22200_HSNLS_MASK, reg_val);

	ret = max22200_reg_read(desc, MAX22200_STATUS_REG, &reg_val);
	if (ret)
		return ret;

	*ch_op_mode = no_os_field_get(MAX22200_CH_MODE_MASK(ch), reg_val);

	return 0;
}

/**
 * @brief Read channel's frequency
 * @param desc - MAX22200 device descriptor.
 * @param ch - Channel's number (0 based).
 * @param ch_freq - Channel's frequency value read from the device.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22200_get_ch_freq(struct max22200_desc *desc, uint32_t ch,
			 enum max22200_ch_freq *ch_freq)
{
	int ret;
	uint32_t reg_val;

	if (ch > MAX22200_MAX_CHN_IDX)
		return -EINVAL;

	ret = max22200_reg_read(desc, MAX22200_CFG_CH(ch), &reg_val);
	if (ret)
		return ret;

	*ch_freq = no_os_field_get(MAX22200_FREQ_CFG_MASK, reg_val);

	return 0;
}

/**
 * @brief Read channel's enable bits state
 * @param desc - MAX22200 device descriptor.
 * @param ch - Channel's number (0 based).
 * @param ch_enable - Requested channel enable bit to read the state for.
 * @param enabled - Requested enabled bit state.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22200_get_ch_enable(struct max22200_desc *desc, uint32_t ch,
			   enum max22200_ch_enable ch_enable, bool *enabled)
{
	int ret;
	uint32_t reg_val;

	if (ch > MAX22200_MAX_CHN_IDX)
		return -EINVAL;

	ret = max22200_reg_read(desc, MAX22200_CFG_CH(ch) | MAX22200_R1B,
				&reg_val);
	if (ret)
		return ret;

	*enabled = no_os_field_get(MAX22200_CH_ENABLE_MASK(ch_enable),
				   reg_val);

	return 0;
}

/**
 * @brief Read configuration for the detection of the plunger movement of the
 * 	  device
 * @param desc - MAX22200 device descriptor.
 * @param dpm_istart_byte - Detection of plunger movement starting current
 * 			    value.
 * @param dpm_tdeb_ipth_byte - Detection of plunger movement debounce time
 * 			       value.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22200_get_cfg_dpm(struct max22200_desc *desc, uint8_t *dpm_istart_byte,
			 uint8_t *dpm_tdeb_ipth_byte)
{
	int ret;
	uint32_t reg_val;

	ret = max22200_reg_read(desc, MAX22200_CFG_DPM_REG, &reg_val);
	if (ret)
		return ret;

	*dpm_istart_byte = no_os_field_get(MAX22200_DPM_ISTART_MASK, reg_val);
	*dpm_tdeb_ipth_byte = no_os_field_get(MAX22200_DPM_TDEB_MASK, reg_val) |
			      no_os_field_get(MAX22200_DPM_IPTH_MASK, reg_val);

	return 0;
}

/**
 * @brief MAX22200 descriptor initialization function.
 * @param desc - MAX22200 device descriptor.
 * @param init_param - Initialization parameter containing data about the device
 * 		       descriptor to be initialized.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22200_init(struct max22200_desc **desc,
		  struct max22200_init_param *init_param)
{
	struct max22200_desc *descriptor;
	uint32_t reg_val;
	uint32_t status_reg = 0;
	int ret, i;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_spi_init(&descriptor->comm_desc, init_param->comm_param);
	if (ret)
		goto err;

	/* Time between enable pin set and device power-up. */
	no_os_udelay(500);

	ret = no_os_gpio_get(&descriptor->cmd_desc,
			     init_param->cmd_param);
	if (ret)
		goto spi_err;

	ret = no_os_gpio_direction_output(descriptor->cmd_desc, NO_OS_GPIO_LOW);
	if (ret)
		goto cmd_err;

	/* Enable pin is optional to be connected to master through GPIO and be
	   controlled in this way, but mandatory to be logic high (3.3V) in
	   order for the device to be enabled. */
	ret = no_os_gpio_get_optional(&descriptor->enable_desc,
				      init_param->enable_param);
	if (ret)
		goto cmd_err;

	if (descriptor->enable_desc) {
		ret = no_os_gpio_direction_output(descriptor->enable_desc,
						  NO_OS_GPIO_HIGH);
		if (ret)
			goto enable_err;
	}

	ret = no_os_gpio_get_optional(&descriptor->trig_desc,
				      init_param->trig_param);
	if (ret)
		goto enable_err;

	/* External trigger if used, is set logic LOW at initialization.
	   Can be set logic HIGH in case of choosing to use the external
	   trigger for Full-Bridge Mode. */
	if (descriptor->trig_desc) {
		ret = no_os_gpio_direction_output(descriptor->trig_desc,
						  NO_OS_GPIO_LOW);
		if (ret)
			goto trig_err;
	}

	ret = no_os_gpio_get_optional(&descriptor->fault_desc,
				      init_param->fault_param);
	if (ret)
		goto trig_err;

	if (descriptor->fault_desc) {
		ret = no_os_gpio_direction_input(descriptor->fault_desc);
		if (ret)
			goto fault_err;
	}

	ret = max22200_reg_read(descriptor, MAX22200_STATUS_REG, &reg_val);
	if (ret)
		goto fault_err;

	/* Getting channel mode info from init_param */
	for (i = 0; i < MAX22200_CHANNELS_CONFIG; i++)
		status_reg |= no_os_field_prep(MAX22200_CH_MODE_MASK(i),
					       *(init_param->ch_config + i));

	/* Writing to the status register the mode of the channels and
	   setting active bit to 1. */
	status_reg |= no_os_field_prep(MAX22200_ACTIVE_MASK, 1);
	ret = max22200_reg_write(descriptor, MAX22200_STATUS_REG,
				 status_reg);
	if (ret)
		goto fault_err;

	/* Configuring channels at startup. */
	for (i = 0; i < MAX22200_CHANNELS; i++) {
		ret = max22200_set_ch_scale(descriptor, i, MAX22200_FULLSCALE);
		if (ret)
			goto fault_err;

		ret = max22200_set_ch_hold(descriptor, i, MAX22200_HIT_MAX_VAL);
		if (ret)
			goto fault_err;

		ret = max22200_set_ch_trig(descriptor, i, MAX22200_ONCH_SPI);
		if (ret)
			goto fault_err;

		ret = max22200_set_ch_hit(descriptor, i, MAX22200_HIT_MAX_VAL,
					  MAX22200_HIT_NO_TIME);
		if (ret)
			goto fault_err;
	}

	/* Delay needed from setting active bit to 1 and configuring channels
	   to normal operation. */
	no_os_udelay(2500);

	/* Reading from the status register again to clear faults. */
	ret = max22200_reg_read(descriptor, MAX22200_STATUS_REG, &reg_val);
	if (ret)
		goto fault_err;

	descriptor->chan_side = MAX22200_LOW_SIDE;
	descriptor->chan_drive = MAX22200_CURRENT_DRIVE;

	*desc = descriptor;

	return 0;

fault_err:
	no_os_gpio_remove(descriptor->fault_desc);
trig_err:
	no_os_gpio_remove(descriptor->trig_desc);
enable_err:
	no_os_gpio_remove(descriptor->enable_desc);
cmd_err:
	no_os_gpio_remove(descriptor->cmd_desc);
spi_err:
	no_os_spi_remove(descriptor->comm_desc);
err:
	no_os_free(descriptor);
	return ret;
}

/**
 * @brief Deallocates all the resources used at initialization.
 * @param desc - MAX22200 device descriptor.
 * @return 0 in case of succes, negative error code otherwise.
*/
int max22200_remove(struct max22200_desc *desc)
{
	int ret, i;

	if (!desc)
		return -ENODEV;

	for (i = 0; i < MAX22200_CHANNELS; i++) {
		ret = max22200_set_ch_state(desc, i, false);
		if (ret)
			return ret;
	}

	ret = max22200_reg_update(desc, MAX22200_STATUS_REG | MAX22200_R1B,
				  MAX22200_ACTIVE_MASK, 0);
	if (ret)
		return ret;

	ret = no_os_gpio_set_value(desc->enable_desc, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	no_os_spi_remove(desc->comm_desc);
	no_os_gpio_remove(desc->enable_desc);
	no_os_gpio_remove(desc->cmd_desc);
	no_os_gpio_remove(desc->trig_desc);
	no_os_gpio_remove(desc->fault_desc);

	no_os_free(desc);

	return 0;
}
