/***************************************************************************//**
 *   @file   ad469x.c
 *   @brief  Implementation of ad69x Driver.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
#include <string.h>
#include "stdio.h"
#include "stdlib.h"
#include "ad469x.h"
#include "spi_engine.h"
#include "delay.h"
#include "error.h"
#include "util.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD469x_TEST_DATA 0xEA

/**
 * @brief Device resolution
 */
const uint8_t ad469x_device_resol[] = {
	[AD469x_OSR_1] = 16,
	[AD469x_OSR_4] = 17,
	[AD469x_OSR_16] = 18,
	[AD469x_OSR_64] = 19
};

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * Read from device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ad469x_spi_reg_read(struct ad469x_dev *dev,
			    uint16_t reg_addr,
			    uint8_t *reg_data)
{
	int32_t ret;
	uint8_t buf[3];

	ret = spi_engine_set_transfer_width(dev->spi_desc, dev->reg_data_width);
	if (ret != SUCCESS)
		return ret;

	spi_engine_set_speed(dev->spi_desc, dev->reg_access_speed);

	buf[0] = (1 << 7) | ((reg_addr >> 8) & 0x7F);
	buf[1] = 0xFF & reg_addr;
	buf[2] = 0xFF;

	ret = spi_write_and_read(dev->spi_desc, buf, 3);
	*reg_data = buf[2];

	ret = spi_engine_set_transfer_width(dev->spi_desc,
					    dev->capture_data_width);
	if (ret != SUCCESS)
		return ret;

	spi_engine_set_speed(dev->spi_desc, dev->spi_desc->max_speed_hz);

	return ret;
}

/**
 * Write to device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @@eturn SUCCESS in case of success, negative error code otherwise.
 */
int32_t ad469x_spi_reg_write(struct ad469x_dev *dev,
			     uint16_t reg_addr,
			     uint8_t reg_data)
{
	int32_t ret;
	uint8_t buf[3];

	ret = spi_engine_set_transfer_width(dev->spi_desc, dev->reg_data_width);
	if (ret != SUCCESS)
		return ret;

	spi_engine_set_speed(dev->spi_desc, dev->reg_access_speed);

	buf[0] = ((reg_addr >> 8) & 0x7F);
	buf[1] = 0xFF & reg_addr;
	buf[2] = reg_data;

	ret = spi_write_and_read(dev->spi_desc, buf, 3);

	ret = spi_engine_set_transfer_width(dev->spi_desc,
					    dev->capture_data_width);
	if (ret != SUCCESS)
		return ret;

	spi_engine_set_speed(dev->spi_desc, dev->spi_desc->max_speed_hz);

	return ret;
}

/**
 * SPI read from device using a mask.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - The mask.
 * @param data - The register data.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ad469x_spi_read_mask(struct ad469x_dev *dev,
			     uint16_t reg_addr,
			     uint8_t mask,
			     uint8_t *data)
{
	uint8_t reg_data[3];
	int32_t ret;

	ret = ad469x_spi_reg_read(dev, reg_addr, reg_data);
	if (ret != SUCCESS)
		return ret;

	*data = (reg_data[0] & mask);

	return ret;
}

/**
 * SPI write to device using a mask.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - The mask.
 * @param data - The register data.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ad469x_spi_write_mask(struct ad469x_dev *dev,
			      uint16_t reg_addr,
			      uint8_t mask,
			      uint8_t data)
{
	uint8_t reg_data;
	int32_t ret;

	ret = ad469x_spi_reg_read(dev, reg_addr, &reg_data);
	if (ret != SUCCESS)
		return ret;

	reg_data &= ~mask;
	reg_data |= data;

	return ad469x_spi_reg_write(dev, reg_addr, reg_data);
}

/**
 * @brief Configure register access mode
 * @param [in] dev - ad469x_dev device handler.
 * @param [in] access - Access mode
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t ad469x_set_reg_access_mode(struct ad469x_dev *dev,
				   enum ad469x_reg_access access)
{
	return ad469x_spi_write_mask(dev,
				     AD469x_REG_IF_CONFIG_C,
				     AD469x_REG_IF_CONFIG_C_MB_STRICT_MASK,
				     AD469x_REG_IF_CONFIG_C_MB_STRICT(access));
}

/**
 * @brief Initialize GPIO driver handlers for the GPIOs in the system.
 *        ad469x_init() helper function.
 * @param [out] dev - ad469x_dev device handler.
 * @param [in] init_param - Pointer to the initialization structure.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
static int32_t ad469x_init_gpio(struct ad469x_dev *dev,
				struct ad469x_init_param *init_param)
{
	int32_t ret;

	ret = gpio_get_optional(&dev->gpio_resetn, init_param->gpio_resetn);
	if (ret != SUCCESS)
		return ret;

	/** Reset to configure pins */
	if (init_param->gpio_resetn) {
		ret = gpio_direction_output(dev->gpio_resetn, GPIO_LOW);
		if (ret != SUCCESS)
			return ret;

		mdelay(100);
		ret = gpio_set_value(dev->gpio_resetn, GPIO_HIGH);
		if (ret != SUCCESS)
			return ret;

		mdelay(100);
	}

	return SUCCESS;
}

/**
 * @brief Configure over sampling ratio in advanced sequencer mode
 * @param [in] dev - ad469x_dev device handler.
 * @param [in] ch - Channel to configure.
 * @param [in] ratio - OSR ratio.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t ad469x_adv_seq_osr(struct ad469x_dev *dev, uint16_t ch,
			   enum ad469x_osr_ratios ratio)
{
	int32_t ret;

	if (dev->ch_sequence == AD469x_single_cycle ||
	    dev->ch_sequence == AD469x_two_cycle)
		return FAILURE;

	if (ch >= AD469x_CHANNEL_NO)
		return FAILURE;

	ret = ad469x_spi_write_mask(dev,
				    AD469x_REG_CONFIG_IN(ch),
				    AD469x_REG_CONFIG_IN_OSR_MASK,
				    AD469x_REG_CONFIG_IN_OSR(ratio));
	if (ret != SUCCESS)
		return ret;

	dev->adv_seq_osr_resol[ch] = ad469x_device_resol[ratio];
	/* Set storage to maximum data width */
	dev->capture_data_width = ad469x_device_resol[AD469x_OSR_64];

	return SUCCESS;
}

/**
 * @brief Configure over sampling ratio to 1 in single and two cycle modes.
 * @param [in] dev - ad469x_dev device handler.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
static int32_t ad469x_seq_osr_clear(struct ad469x_dev *dev)
{
	int32_t ret;
	uint8_t i = 0;

	for (i = 0; i < AD469x_CHANNEL_NO; i++) {
		ret = ad469x_spi_write_mask(dev,
					    AD469x_REG_CONFIG_IN(i),
					    AD469x_REG_CONFIG_IN_OSR_MASK,
					    AD469x_REG_CONFIG_IN_OSR(AD469x_OSR_1));
		if (ret != SUCCESS)
			return ret;
		dev->adv_seq_osr_resol[i] = ad469x_device_resol[AD469x_OSR_1];
	}
	/* Set storage to minimum data width */
	dev->capture_data_width = ad469x_device_resol[AD469x_OSR_1];

	return SUCCESS;
}

/**
 * @brief Configure over sampling ratio in standard sequencer mode
 * @param [in] dev - ad469x_dev device handler.
 * @param [in] ratio - OSR ratio.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t ad469x_std_seq_osr(struct ad469x_dev *dev, enum ad469x_osr_ratios ratio)
{
	int ret;

	if (dev->ch_sequence == AD469x_single_cycle ||
	    dev->ch_sequence == AD469x_two_cycle)
		return FAILURE;

	ret = ad469x_spi_write_mask(dev,
				    AD469x_REG_CONFIG_IN(0),
				    AD469x_REG_CONFIG_IN_OSR_MASK,
				    AD469x_REG_CONFIG_IN_OSR(ratio));
	if (ret != SUCCESS)
		return ret;

	dev->capture_data_width = ad469x_device_resol[ratio];

	return ret;
}

/**
 * @brief Set channel sequence.
 * @param [in] dev - ad469x_dev device handler.
 * @param [in] seq - Channel sequence.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t ad469x_set_channel_sequence(struct ad469x_dev *dev,
				    enum ad469x_channel_sequencing seq)
{
	int32_t ret;

	switch (seq) {
	case AD469x_single_cycle:
		ret = ad469x_spi_write_mask(dev,
					    AD469x_REG_SEQ_CTRL,
					    AD469x_SEQ_CTRL_STD_SEQ_EN_MASK,
					    AD469x_SEQ_CTRL_STD_SEQ_EN(0));
		if (ret != SUCCESS)
			return ret;

		ret = ad469x_spi_write_mask(dev,
					    AD469x_REG_SEQ_CTRL,
					    AD469x_SEQ_CTRL_NUM_SLOTS_AS_MASK,
					    AD469x_SEQ_CTRL_NUM_SLOTS_AS(0));
		if (ret != SUCCESS)
			return ret;

		ret = ad469x_spi_write_mask(dev,
					    AD469x_REG_SETUP,
					    AD469x_SETUP_CYC_CTRL_MASK,
					    AD469x_SETUP_CYC_CTRL_SINGLE(0));
		if (ret != SUCCESS)
			return ret;

		ret = ad469x_seq_osr_clear(dev);
		if (ret != SUCCESS)
			return ret;

		break;

	case AD469x_two_cycle:
		ret = ad469x_spi_write_mask(dev,
					    AD469x_REG_SEQ_CTRL,
					    AD469x_SEQ_CTRL_STD_SEQ_EN_MASK,
					    AD469x_SEQ_CTRL_STD_SEQ_EN(0));
		if (ret != SUCCESS)
			return ret;

		ret = ad469x_spi_write_mask(dev,
					    AD469x_REG_SEQ_CTRL,
					    AD469x_SEQ_CTRL_NUM_SLOTS_AS_MASK,
					    AD469x_SEQ_CTRL_NUM_SLOTS_AS(0));
		if (ret != SUCCESS)
			return ret;

		ret = ad469x_spi_write_mask(dev,
					    AD469x_REG_SETUP,
					    AD469x_SETUP_CYC_CTRL_MASK,
					    AD469x_SETUP_CYC_CTRL_SINGLE(1));
		if (ret != SUCCESS)
			return ret;

		ret = ad469x_seq_osr_clear(dev);
		if (ret != SUCCESS)
			return ret;

		break;

	case AD469x_standard_seq:
		ret = ad469x_spi_write_mask(dev,
					    AD469x_REG_SEQ_CTRL,
					    AD469x_SEQ_CTRL_STD_SEQ_EN_MASK,
					    AD469x_SEQ_CTRL_STD_SEQ_EN(1));
		if (ret != SUCCESS)
			return ret;

		break;

	case AD469x_advanced_seq:
		ret = ad469x_spi_write_mask(dev,
					    AD469x_REG_SEQ_CTRL,
					    AD469x_SEQ_CTRL_STD_SEQ_EN_MASK,
					    AD469x_SEQ_CTRL_STD_SEQ_EN(0));
		if (ret != SUCCESS)
			return ret;

		break;

	default:
		return FAILURE;
		break;
	}

	dev->ch_sequence = seq;

	return ret;
}

/**
 * @brief Configure advanced sequencer number of slots, temp channel not
 * included
 * @param [in] dev - ad469x_dev device handler.
 * @param [in] num_slots - Number of slots, max value = 0x7f
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t ad469x_adv_sequence_set_num_slots(struct ad469x_dev *dev,
		uint8_t num_slots)
{
	int32_t ret;
	uint8_t write_num_slots = 0;

	if (num_slots)
		write_num_slots = num_slots - 1;

	ret = ad469x_spi_write_mask(dev,
				    AD469x_REG_SEQ_CTRL,
				    AD469x_SEQ_CTRL_NUM_SLOTS_AS_MASK,
				    AD469x_SEQ_CTRL_NUM_SLOTS_AS(write_num_slots));
	if (ret != SUCCESS)
		return ret;

	dev->num_slots = num_slots;

	return SUCCESS;
}

/**
 * @brief Advanced sequencer, assign channel to a slot
 * @param [in] dev - ad469x_dev device handler.
 * @param [in] slot - Slot number [0x00, 0x7f]
 * @param [in] channel - Assigned channel [0x00, 0x0f].
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t ad469x_adv_sequence_set_slot(struct ad469x_dev *dev,
				     uint8_t slot,
				     uint8_t channel)
{
	int32_t ret;
	ret = ad469x_spi_reg_write(dev,
				   AD469x_REG_AS_SLOT(slot),
				   AD469x_REG_AS_SLOT_INX(channel));
	if (ret != SUCCESS)
		return ret;

	dev->ch_slots[slot] = channel;

	return SUCCESS;
}

/**
 * @brief Configure standard sequencer channels
 * @param [in] dev - ad469x_dev device handler.
 * @param [in] ch_mask - Extra channels to activate.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t ad469x_std_sequence_ch(struct ad469x_dev *dev, uint16_t ch_mask)
{
	int32_t ret;

	ret = ad469x_spi_reg_write(dev,
				   AD469x_REG_STD_SEQ_CONFIG,
				   0x0f & ch_mask);
	if (ret != SUCCESS)
		return ret;

	ret = ad469x_spi_reg_write(dev,
				   AD469x_REG_STD_SEQ_CONFIG + 1,
				   ch_mask >> 8);
	if (ret != SUCCESS)
		return ret;

	dev->num_slots = hweight8(ch_mask);

	return ret;
}

/**
 * @brief Enable temperature read at the end of the sequence, for standard and
 * advanced sequencer
 * @param [in] dev - ad469x_dev device handler.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t ad469x_sequence_enable_temp(struct ad469x_dev *dev)
{
	int32_t ret;

	ret = ad469x_spi_write_mask(dev,
				    AD469x_REG_TEMP_CTRL,
				    AD469x_REG_TEMP_CTRL_TEMP_EN_MASK,
				    AD469x_REG_TEMP_CTRL_TEMP_EN(1));
	if (ret != SUCCESS)
		return ret;

	dev->temp_enabled = true;

	return ret;
}

/**
 * @brief Disable temperature read at the end of the sequence, for standard and
 * advanced sequencer
 * @param [in] dev - ad469x_dev device handler.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t ad469x_sequence_disable_temp(struct ad469x_dev *dev)
{
	int32_t ret;

	ret = ad469x_spi_write_mask(dev,
				    AD469x_REG_TEMP_CTRL,
				    AD469x_REG_TEMP_CTRL_TEMP_EN_MASK,
				    AD469x_REG_TEMP_CTRL_TEMP_EN(0));
	if (ret != SUCCESS)
		return ret;

	dev->temp_enabled = false;

	return ret;
}

/**
 * @brief Configure converter busy indicator to the output of the specified port
 * @param [in] dev - ad469x_dev device handler.
 * @param [in] gp_sel - Port.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t ad469x_set_busy(struct ad469x_dev *dev,
			enum ad469x_busy_gp_sel gp_sel)
{
	int32_t ret;

	ret = ad469x_spi_write_mask(dev,
				    AD469x_REG_GP_MODE,
				    AD469x_GP_MODE_BUSY_GP_EN_MASK,
				    AD469x_GP_MODE_BUSY_GP_EN(1));
	if (ret != SUCCESS)
		return ret;


	ret = ad469x_spi_write_mask(dev,
				    AD469x_REG_GP_MODE,
				    AD469x_GP_MODE_BUSY_GP_SEL_MASK,
				    AD469x_GP_MODE_BUSY_GP_SEL(gp_sel));
	if (ret != SUCCESS)
		return ret;


	return ret;
}

/**
 * @brief Enter conversion mode.
 *        To exit conversion mode send a 5 bit conversion mode command
 *        AD469x_CMD_REG_CONFIG_MODE
 * @param [in] dev - ad469x_dev device handler.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t ad469x_enter_conversion_mode(struct ad469x_dev *dev)
{
	return ad469x_spi_write_mask(dev,
				     AD469x_REG_SETUP,
				     AD469x_SETUP_IF_MODE_MASK,
				     AD469x_SETUP_IF_MODE_CONV);
}

/**
 * @brief Exit conversion mode.
 *        Enter register mode to read/write registers
 * @param [in] dev - ad469x_dev device handler.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t ad469x_exit_conversion_mode(struct ad469x_dev *dev)
{
	uint32_t commands_data[1], buf;
	struct spi_engine_offload_message msg;
	uint32_t spi_eng_msg_cmds[3] = {
		CS_LOW,
		WRITE_READ(1),
		CS_HIGH
	};
	int32_t ret;

	pwm_enable(dev->trigger_pwm_desc);

	commands_data[0] = AD469x_CMD_REG_CONFIG_MODE << 8;

	ret = spi_engine_offload_init(dev->spi_desc, dev->offload_init_param);
	if (ret != SUCCESS)
		return ret;

	msg.commands = spi_eng_msg_cmds;
	msg.no_commands = ARRAY_SIZE(spi_eng_msg_cmds);
	msg.rx_addr = (uint32_t)&buf;
	msg.commands_data = commands_data;

	ret = spi_engine_offload_transfer(dev->spi_desc, msg, 1);
	if (ret != SUCCESS)
		return ret;

	pwm_disable(dev->trigger_pwm_desc);
	if (ret != SUCCESS)
		return ret;

	return SUCCESS;
}

/**
 * @brief Advanced sequencer, get util data bits in a sample
 * @param [in] dev - ad469x_dev device handler.
 * @param [in] cur_sample - Current sample number
 * @param [in] sample - Sample data
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
static int32_t ad469x_adv_seq_osr_get_util_data(struct ad469x_dev *dev,
		uint16_t cur_sample, uint32_t *sample)
{
	uint8_t cur_slot, cur_ch;

	cur_slot = cur_sample % (dev->num_slots + dev->temp_enabled);
	cur_ch = dev->ch_slots[cur_slot];

	/* Temperature channel sample */
	if (dev->temp_enabled && cur_slot == dev->num_slots)
		return SUCCESS;

	*sample = (*sample) >> (dev->capture_data_width -
				dev->adv_seq_osr_resol[cur_ch]);

	return SUCCESS;
}

/**
 * @brief Read from device when converter has the channel sequencer activated.
 *        Enter register mode to read/write registers
 * @param [in] dev - ad469x_dev device handler.
 * @param [out] buf - data buffer.
 * @param [in] samples - Number of samples per channel. For example, if  with
 * ad469x_std_sequence_ch 2 channel where activated, buf will be filled with
 * 10 samples for each of them. If temp is enable, the there will be an other 10
 * samples for temperature
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t ad469x_seq_read_data(struct ad469x_dev *dev,
			     uint32_t *buf,
			     uint16_t samples)
{
	int32_t ret;
	uint16_t i;
	uint32_t total_samples;

	total_samples = samples * (dev->num_slots + dev->temp_enabled);
	ret = ad469x_read_data(dev, 0, buf, total_samples);
	if (ret != SUCCESS)
		return ret;

	if (dev->ch_sequence != AD469x_advanced_seq)
		return SUCCESS;

	for (i = 0; i < total_samples; i++) {
		ret = ad469x_adv_seq_osr_get_util_data(dev, i, &buf[i]);
		if (ret != SUCCESS)
			return ret;
	}

	return SUCCESS;
}

/**
 * @brief Read from device.
 *        Enter register mode to read/write registers
 * @param [in] dev - ad469x_dev device handler.
 * @param [in] channel - ad469x selected channel.
 * @param [out] buf - data buffer.
 * @param [in] samples - sample number.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t ad469x_read_data(struct ad469x_dev *dev,
			 uint8_t channel,
			 uint32_t *buf,
			 uint16_t samples)
{
	int32_t ret;
	uint32_t commands_data[1];
	struct spi_engine_offload_message msg;
	uint32_t spi_eng_msg_cmds[3] = {
		CS_LOW,
		WRITE_READ(1),
		CS_HIGH
	};
	if (channel < AD469x_CHANNEL_NO)
		commands_data[0] = AD469x_CMD_CONFIG_CH_SEL(channel) << 8;
	else if (channel == AD469x_CHANNEL_TEMP)
		commands_data[0] = AD469x_CMD_SEL_TEMP_SNSOR_CH << 8;
	else
		return FAILURE;

	pwm_enable(dev->trigger_pwm_desc);

	ret = spi_engine_offload_init(dev->spi_desc, dev->offload_init_param);
	if (ret != SUCCESS)
		return ret;

	msg.commands = spi_eng_msg_cmds;
	msg.no_commands = ARRAY_SIZE(spi_eng_msg_cmds);
	msg.rx_addr = (uint32_t)buf;
	msg.commands_data = commands_data;

	ret = spi_engine_offload_transfer(dev->spi_desc, msg, samples * 2);
	if (ret != SUCCESS)
		return ret;

	if (dev->dcache_invalidate_range)
		dev->dcache_invalidate_range(msg.rx_addr, samples * 4);

	return ret;
}

/**
 * Initialize the device.
 * @param [out] device - The device structure.
 * @param [in] init_param - The structure that contains the device initial
 * 		parameters.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t ad469x_init(struct ad469x_dev **device,
		    struct ad469x_init_param *init_param)
{
	struct ad469x_dev *dev;
	int32_t ret;
	uint8_t data = 0;

	dev = (struct ad469x_dev *)malloc(sizeof(*dev));
	if (!dev)
		return FAILURE;

	ret = axi_clkgen_init(&dev->clkgen, init_param->clkgen_init);
	if (ret != SUCCESS) {
		printf("error: %s: axi_clkgen_init() failed\n",
		       init_param->clkgen_init->name);
		goto error_dev;
	}

	ret = axi_clkgen_set_rate(dev->clkgen, init_param->axi_clkgen_rate);
	if (ret != SUCCESS) {
		printf("error: %s: axi_clkgen_set_rate() failed\n",
		       init_param->clkgen_init->name);
		goto error_clkgen;
	}

	ret = ad469x_init_gpio(dev, init_param);
	if (ret != SUCCESS)
		goto error_clkgen;

	ret = spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret != SUCCESS)
		goto error_gpio;

	dev->offload_init_param = init_param->offload_init_param;

	dev->reg_access_speed = init_param->reg_access_speed;
	dev->reg_data_width = init_param->reg_data_width;
	dev->capture_data_width = init_param->capture_data_width;
	dev->dev_id = init_param->dev_id;
	dev->dcache_invalidate_range = init_param->dcache_invalidate_range;
	dev->ch_sequence = AD469x_standard_seq;
	dev->num_slots = 0;
	dev->temp_enabled = false;
	memset(dev->ch_slots, 0, sizeof(dev->ch_slots));

	ret = ad469x_spi_reg_write(dev, AD469x_REG_SCRATCH_PAD, AD469x_TEST_DATA);
	if (ret != SUCCESS)
		goto error_spi;

	ret = ad469x_spi_reg_read(dev, AD469x_REG_SCRATCH_PAD, &data);
	if (ret != SUCCESS)
		goto error_spi;

	if (data != AD469x_TEST_DATA)
		goto error_spi;

	ret = ad469x_set_reg_access_mode(dev, AD469x_BYTE_ACCESS);
	if (ret != SUCCESS)
		goto error_spi;

	ret = ad469x_set_busy(dev, AD469x_busy_gp0);
	if (ret != SUCCESS)
		goto error_spi;

	ret = ad469x_seq_osr_clear(dev);
	if (ret != SUCCESS)
		goto error_spi;

	ret = pwm_init(&dev->trigger_pwm_desc, init_param->trigger_pwm_init);
	if (ret != SUCCESS)
		goto error_spi;

	*device = dev;

	return ret;

error_spi:
	spi_remove(dev->spi_desc);
error_gpio:
	gpio_remove(dev->gpio_resetn);
error_clkgen:
	axi_clkgen_remove(dev->clkgen);
error_dev:
	free(dev);

	return FAILURE;
}

/**
 * @brief Free the memory allocated by ad469x_init().
 * @param [in] dev - Pointer to the device handler.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise
 */
int32_t ad469x_remove(struct ad469x_dev *dev)
{
	int32_t ret;

	if (!dev)
		return FAILURE;

	ret = pwm_remove(dev->trigger_pwm_desc);
	if (ret != SUCCESS)
		return ret;

	ret = spi_remove(dev->spi_desc);
	if (ret != SUCCESS)
		return ret;

	ret = gpio_remove(dev->gpio_resetn);
	if (ret != SUCCESS)
		return ret;

	ret = axi_clkgen_remove(dev->clkgen);
	if (ret != SUCCESS)
		return ret;

	free(dev);

	return ret;
}
