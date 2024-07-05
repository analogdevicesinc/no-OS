/***************************************************************************//**
 *   @file   ad469x.c
 *   @brief  Implementation of ad69x Driver.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2020-22(c) Analog Devices, Inc.
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
#if !defined(USE_STANDARD_SPI)
#include "spi_engine.h"
#endif
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

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

struct ad469x_device_info {
	uint8_t max_data_ch;
	uint32_t max_rate_ksps;
};

struct ad469x_device_info dev_info[] = {
	[ID_AD4695] = {.max_data_ch = 16, .max_rate_ksps = 500},
	[ID_AD4696] = {.max_data_ch = 16, .max_rate_ksps = 1000},
	[ID_AD4697] = {.max_data_ch = 8, .max_rate_ksps = 500},
	[ID_AD4698] = {.max_data_ch = 8, .max_rate_ksps = 1000},
};

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * Read from device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad469x_spi_reg_read(struct ad469x_dev *dev,
			    uint16_t reg_addr,
			    uint8_t *reg_data)
{
	int32_t ret;
	uint8_t buf[3];

#if !defined(USE_STANDARD_SPI)
	ret = spi_engine_set_transfer_width(dev->spi_desc, dev->reg_data_width);
	if (ret != 0)
		return ret;

	spi_engine_set_speed(dev->spi_desc, dev->reg_access_speed);
#endif

	buf[0] = (1 << 7) | ((reg_addr >> 8) & 0x7F);
	buf[1] = 0xFF & reg_addr;
	buf[2] = 0xFF;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 3);
	*reg_data = buf[2];

#if !defined(USE_STANDARD_SPI)
	ret = spi_engine_set_transfer_width(dev->spi_desc,
					    dev->capture_data_width);
	if (ret != 0)
		return ret;

	spi_engine_set_speed(dev->spi_desc, dev->spi_desc->max_speed_hz);
#endif

	return ret;
}

/**
 * Write to device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @@eturn 0 in case of success, negative error code otherwise.
 */
int32_t ad469x_spi_reg_write(struct ad469x_dev *dev,
			     uint16_t reg_addr,
			     uint8_t reg_data)
{
	int32_t ret;
	uint8_t buf[3];

#if !defined(USE_STANDARD_SPI)
	ret = spi_engine_set_transfer_width(dev->spi_desc, dev->reg_data_width);
	if (ret != 0)
		return ret;

	spi_engine_set_speed(dev->spi_desc, dev->reg_access_speed);
#endif

	buf[0] = ((reg_addr >> 8) & 0x7F);
	buf[1] = 0xFF & reg_addr;
	buf[2] = reg_data;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 3);

#if !defined(USE_STANDARD_SPI)
	ret = spi_engine_set_transfer_width(dev->spi_desc,
					    dev->capture_data_width);
	if (ret != 0)
		return ret;

	spi_engine_set_speed(dev->spi_desc, dev->spi_desc->max_speed_hz);
#endif

	return ret;
}

/**
 * SPI read from device using a mask.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - The mask.
 * @param data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad469x_spi_read_mask(struct ad469x_dev *dev,
			     uint16_t reg_addr,
			     uint8_t mask,
			     uint8_t *data)
{
	uint8_t reg_data[3];
	int32_t ret;

	ret = ad469x_spi_reg_read(dev, reg_addr, reg_data);
	if (ret != 0)
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
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad469x_spi_write_mask(struct ad469x_dev *dev,
			      uint16_t reg_addr,
			      uint8_t mask,
			      uint8_t data)
{
	uint8_t reg_data;
	int32_t ret;

	ret = ad469x_spi_reg_read(dev, reg_addr, &reg_data);
	if (ret != 0)
		return ret;

	reg_data &= ~mask;
	reg_data |= data;

	return ad469x_spi_reg_write(dev, reg_addr, reg_data);
}

/**
 * @brief Configure register access mode
 * @param [in] dev - ad469x_dev device handler.
 * @param [in] access - Access mode
 * @return 0 in case of success, -1 otherwise.
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
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t ad469x_init_gpio(struct ad469x_dev *dev,
				struct ad469x_init_param *init_param)
{
	int32_t ret;

	ret = no_os_gpio_get_optional(&dev->gpio_resetn, init_param->gpio_resetn);
	if (ret != 0)
		return ret;

	ret = no_os_gpio_direction_output(dev->gpio_resetn, NO_OS_GPIO_HIGH);
	if (ret != 0)
		return ret;

	ret = no_os_gpio_get_optional(&dev->gpio_busy, init_param->gpio_busy);
	if (ret != 0)
		return ret;

	ret = no_os_gpio_direction_input(dev->gpio_busy);
	if (ret != 0)
		return ret;

	ret = no_os_gpio_get_optional(&dev->gpio_convst, init_param->gpio_convst);
	if (ret != 0)
		return ret;

	ret = no_os_gpio_direction_output(dev->gpio_convst,  NO_OS_GPIO_LOW);
	if (ret != 0)
		return ret;

	return 0;
}

int32_t ad469x_get_num_channels(struct ad469x_dev *dev,
				uint8_t *num_channels)
{
	if (!dev)
		return -EINVAL;

	*num_channels = dev->num_data_ch;
	if (dev->temp_enabled)
		*num_channels = dev->num_data_ch + 1;

	return 0;
}

bool ad469x_is_temp_channel(struct ad469x_dev *dev,
			    uint8_t channel)
{
	if (!dev) {
		printf("Error: %s: null dev structure\n", __func__);
		return false;
	}

	if ((dev->temp_enabled) && (channel == dev->num_data_ch))
		return true;

	return false;
}

/**
 * @brief Configure over sampling ratio in advanced sequencer mode
 * @param [in] dev - ad469x_dev device handler.
 * @param [in] ch - Channel to configure.
 * @param [in] ratio - OSR ratio.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ad469x_adv_seq_osr(struct ad469x_dev *dev, uint16_t ch,
			   enum ad469x_osr_ratios ratio)
{
	int32_t ret;

	if (dev->ch_sequence == AD469x_single_cycle ||
	    dev->ch_sequence == AD469x_two_cycle)
		return -1;

	if (ch >= dev->num_data_ch)
		return -1;

	ret = ad469x_spi_write_mask(dev,
				    AD469x_REG_CONFIG_IN(ch),
				    AD469x_REG_CONFIG_IN_OSR_MASK,
				    AD469x_REG_CONFIG_IN_OSR(ratio));
	if (ret != 0)
		return ret;

	dev->adv_seq_osr_resol[ch] = ad469x_device_resol[ratio];
	/* Set storage to maximum data width */
	dev->capture_data_width = ad469x_device_resol[AD469x_OSR_64];

	return 0;
}

/**
 * @brief Configure over sampling ratio to 1 in single and two cycle modes.
 * @param [in] dev - ad469x_dev device handler.
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t ad469x_seq_osr_clear(struct ad469x_dev *dev)
{
	int32_t ret;
	uint8_t i = 0;

	for (i = 0; i < dev->num_data_ch; i++) {
		ret = ad469x_spi_write_mask(dev,
					    AD469x_REG_CONFIG_IN(i),
					    AD469x_REG_CONFIG_IN_OSR_MASK,
					    AD469x_REG_CONFIG_IN_OSR(AD469x_OSR_1));
		if (ret != 0)
			return ret;
		dev->adv_seq_osr_resol[i] = ad469x_device_resol[AD469x_OSR_1];
	}
	/* Set storage to minimum data width */
	dev->capture_data_width = ad469x_device_resol[AD469x_OSR_1];

	return 0;
}

/**
 * @brief Configure over sampling ratio in standard sequencer mode
 * @param [in] dev - ad469x_dev device handler.
 * @param [in] ratio - OSR ratio.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ad469x_std_seq_osr(struct ad469x_dev *dev, enum ad469x_osr_ratios ratio)
{
	int ret;

	if (dev->ch_sequence == AD469x_single_cycle ||
	    dev->ch_sequence == AD469x_two_cycle)
		return -1;

	ret = ad469x_spi_write_mask(dev,
				    AD469x_REG_CONFIG_IN(0),
				    AD469x_REG_CONFIG_IN_OSR_MASK,
				    AD469x_REG_CONFIG_IN_OSR(ratio));
	if (ret != 0)
		return ret;

	dev->std_seq_osr = ratio;
	dev->capture_data_width = ad469x_device_resol[ratio];

	return ret;
}

/**
 * @brief Configure the pin pairing option in standard sequencer mode.
 * @param [in] dev - ad469x_dev device handler.
 * @param [in] pin_pair - Pin pairing selection.
 * @return 0 in case of success, negative error code otherwise.
 * @note In standard sequencer the pin pair option is common for all channels.
 */
int32_t ad469x_std_pin_pairing(struct ad469x_dev *dev,
			       enum ad469x_pin_pairing pin_pair)
{
	dev->std_seq_pin_pairing = pin_pair;

	return ad469x_spi_write_mask(dev,
				     AD469x_REG_CONFIG_IN(0),
				     AD469x_REG_CONFIG_IN_MODE_MASK,
				     AD469x_REG_CONFIG_IN_MODE(pin_pair));
}

/**
 * @brief Set channel sequence.
 * @param [in] dev - ad469x_dev device handler.
 * @param [in] seq - Channel sequence.
 * @return 0 in case of success, -1 otherwise.
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
		if (ret != 0)
			return ret;

		ret = ad469x_spi_write_mask(dev,
					    AD469x_REG_SEQ_CTRL,
					    AD469x_SEQ_CTRL_NUM_SLOTS_AS_MASK,
					    AD469x_SEQ_CTRL_NUM_SLOTS_AS(0));
		if (ret != 0)
			return ret;

		ret = ad469x_spi_write_mask(dev,
					    AD469x_REG_SETUP,
					    AD469x_SETUP_CYC_CTRL_MASK,
					    AD469x_SETUP_CYC_CTRL_SINGLE(1));
		if (ret != 0)
			return ret;

		ret = ad469x_seq_osr_clear(dev);
		if (ret != 0)
			return ret;

		break;

	case AD469x_two_cycle:
		ret = ad469x_spi_write_mask(dev,
					    AD469x_REG_SEQ_CTRL,
					    AD469x_SEQ_CTRL_STD_SEQ_EN_MASK,
					    AD469x_SEQ_CTRL_STD_SEQ_EN(0));
		if (ret != 0)
			return ret;

		ret = ad469x_spi_write_mask(dev,
					    AD469x_REG_SEQ_CTRL,
					    AD469x_SEQ_CTRL_NUM_SLOTS_AS_MASK,
					    AD469x_SEQ_CTRL_NUM_SLOTS_AS(0));
		if (ret != 0)
			return ret;

		ret = ad469x_spi_write_mask(dev,
					    AD469x_REG_SETUP,
					    AD469x_SETUP_CYC_CTRL_MASK,
					    AD469x_SETUP_CYC_CTRL_SINGLE(0));
		if (ret != 0)
			return ret;

		ret = ad469x_seq_osr_clear(dev);
		if (ret != 0)
			return ret;

		break;

	case AD469x_standard_seq:
		ret = ad469x_spi_write_mask(dev,
					    AD469x_REG_SETUP,
					    AD469x_SETUP_CYC_CTRL_MASK,
					    AD469x_SETUP_CYC_CTRL_SINGLE(0));
		if (ret != 0)
			return ret;

		ret = ad469x_spi_write_mask(dev,
					    AD469x_REG_SEQ_CTRL,
					    AD469x_SEQ_CTRL_STD_SEQ_EN_MASK,
					    AD469x_SEQ_CTRL_STD_SEQ_EN(1));
		if (ret != 0)
			return ret;

		break;

	case AD469x_advanced_seq:
		ret = ad469x_spi_write_mask(dev,
					    AD469x_REG_SEQ_CTRL,
					    AD469x_SEQ_CTRL_STD_SEQ_EN_MASK,
					    AD469x_SEQ_CTRL_STD_SEQ_EN(0));
		if (ret != 0)
			return ret;

		break;

	default:
		return -1;
	}

	dev->ch_sequence = seq;

	return ret;
}

/**
 * @brief Configure advanced sequencer number of slots, temp channel not
 * included
 * @param [in] dev - ad469x_dev device handler.
 * @param [in] num_slots - Number of slots, max value = 0x7f
 * @return 0 in case of success, -1 otherwise.
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
	if (ret != 0)
		return ret;

	dev->num_slots = num_slots;

	return 0;
}

/**
 * @brief Advanced sequencer, assign channel to a slot
 * @param [in] dev - ad469x_dev device handler.
 * @param [in] slot - Slot number [0x00, 0x7f]
 * @param [in] channel - Assigned channel [0x00, 0x0f].
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ad469x_adv_sequence_set_slot(struct ad469x_dev *dev,
				     uint8_t slot,
				     uint8_t channel)
{
	int32_t ret;
	ret = ad469x_spi_reg_write(dev,
				   AD469x_REG_AS_SLOT(slot),
				   AD469x_REG_AS_SLOT_INX(channel));
	if (ret != 0)
		return ret;

	dev->ch_slots[slot] = channel;

	return 0;
}

/**
 * @brief Configure standard sequencer channels
 * @param [in] dev - ad469x_dev device handler.
 * @param [in] ch_mask - Extra channels to activate.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ad469x_std_sequence_ch(struct ad469x_dev *dev, uint16_t ch_mask)
{
	int32_t ret;

	ret = ad469x_spi_reg_write(dev,
				   AD469x_REG_STD_SEQ_CONFIG,
				   0xff & ch_mask);
	if (ret != 0)
		return ret;

	ret = ad469x_spi_reg_write(dev,
				   AD469x_REG_STD_SEQ_CONFIG + 1,
				   ch_mask >> 8);
	if (ret != 0)
		return ret;

	dev->num_slots = no_os_hweight16(ch_mask);

	return ret;
}

/**
 * @brief Enable temperature read at the end of the sequence, for standard and
 * advanced sequencer
 * @param [in] dev - ad469x_dev device handler.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ad469x_sequence_enable_temp(struct ad469x_dev *dev)
{
	int32_t ret;

	ret = ad469x_spi_write_mask(dev,
				    AD469x_REG_TEMP_CTRL,
				    AD469x_REG_TEMP_CTRL_TEMP_EN_MASK,
				    AD469x_REG_TEMP_CTRL_TEMP_EN(1));
	if (ret != 0)
		return ret;

	dev->temp_enabled = true;

	return ret;
}

/**
 * @brief Disable temperature read at the end of the sequence, for standard and
 * advanced sequencer
 * @param [in] dev - ad469x_dev device handler.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ad469x_sequence_disable_temp(struct ad469x_dev *dev)
{
	int32_t ret;

	ret = ad469x_spi_write_mask(dev,
				    AD469x_REG_TEMP_CTRL,
				    AD469x_REG_TEMP_CTRL_TEMP_EN_MASK,
				    AD469x_REG_TEMP_CTRL_TEMP_EN(0));
	if (ret != 0)
		return ret;

	dev->temp_enabled = false;

	return ret;
}

/**
 * @brief Configure converter busy indicator to the output of the specified port
 * @param [in] dev - ad469x_dev device handler.
 * @param [in] gp_sel - Port.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ad469x_set_busy(struct ad469x_dev *dev,
			enum ad469x_busy_gp_sel gp_sel)
{
	int32_t ret;

	ret = ad469x_spi_write_mask(dev,
				    AD469x_REG_GP_MODE,
				    AD469x_GP_MODE_BUSY_GP_EN_MASK,
				    AD469x_GP_MODE_BUSY_GP_EN(1));
	if (ret != 0)
		return ret;


	ret = ad469x_spi_write_mask(dev,
				    AD469x_REG_GP_MODE,
				    AD469x_GP_MODE_BUSY_GP_SEL_MASK,
				    AD469x_GP_MODE_BUSY_GP_SEL(gp_sel));
	if (ret != 0)
		return ret;


	return ret;
}

/**
 * @brief Enter conversion mode.
 *        To exit conversion mode send a 5 bit conversion mode command
 *        AD469x_CMD_REG_CONFIG_MODE
 * @param [in] dev - ad469x_dev device handler.
 * @return 0 in case of success, -1 otherwise.
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
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ad469x_exit_conversion_mode(struct ad469x_dev *dev)
{
	int32_t ret;

#if !defined(USE_STANDARD_SPI)
	uint32_t commands_data[1], buf;
	struct spi_engine_offload_message msg;
	uint32_t spi_eng_msg_cmds[3] = {
		CS_LOW,
		WRITE_READ(1),
		CS_HIGH
	};

	no_os_pwm_enable(dev->trigger_pwm_desc);

	commands_data[0] = AD469x_CMD_REG_CONFIG_MODE << 8;

	ret = spi_engine_offload_init(dev->spi_desc, dev->offload_init_param);
	if (ret != 0)
		return ret;

	msg.commands = spi_eng_msg_cmds;
	msg.no_commands = NO_OS_ARRAY_SIZE(spi_eng_msg_cmds);
	msg.rx_addr = (uint32_t)&buf;
	msg.commands_data = commands_data;

	ret = spi_engine_offload_transfer(dev->spi_desc, msg, 1);
	if (ret != 0)
		return ret;

	no_os_pwm_disable(dev->trigger_pwm_desc);
	if (ret != 0)
		return ret;
#else
	/* The subsequent rising edge on CS places the AD4695/AD4696
	 * in register configuration mode. Hence, the digital host must
	* provide 19 SCK pulses to elapse between the fifth SCK rising edge
	* and the CS rising edge.
	*/
	uint8_t cmd[3] = { 0 };
	cmd[0] = AD469x_CMD_REG_CONFIG_MODE;

	ret = no_os_spi_write_and_read(dev->spi_desc, cmd, 3);
	if (ret != 0)
		return ret;
#endif

	return 0;
}

/**
 * @brief Advanced sequencer, get util data bits in a sample
 * @param [in] dev - ad469x_dev device handler.
 * @param [in] cur_sample - Current sample number
 * @param [in] sample - Sample data
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t ad469x_adv_seq_osr_get_util_data(struct ad469x_dev *dev,
		uint32_t cur_sample, uint32_t *sample)
{
	uint8_t cur_slot, cur_ch;

	cur_slot = cur_sample % (dev->num_slots + dev->temp_enabled);
	cur_ch = dev->ch_slots[cur_slot];

	/* Temperature channel sample */
	if (dev->temp_enabled && cur_slot == dev->num_slots)
		return 0;

	*sample = (*sample) >> (dev->capture_data_width -
				dev->adv_seq_osr_resol[cur_ch]);

	return 0;
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
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ad469x_seq_read_data(struct ad469x_dev *dev,
			     uint32_t *buf,
			     uint32_t samples)
{
	int32_t ret;
	uint32_t i;
	uint32_t total_samples;

	total_samples = samples * (dev->num_slots + dev->temp_enabled);
	ret = ad469x_read_data(dev, 0, buf, total_samples);
	if (ret != 0)
		return ret;

	if (dev->ch_sequence != AD469x_advanced_seq)
		return 0;

	for (i = 0; i < total_samples; i++) {
		ret = ad469x_adv_seq_osr_get_util_data(dev, i, &buf[i]);
		if (ret != 0)
			return ret;
	}

	return 0;
}

/**
 * @brief Read from device.
 *        Enter register mode to read/write registers
 * @param [in] dev - ad469x_dev device handler.
 * @param [in] channel - ad469x selected channel.
 * @param [out] buf - data buffer.
 * @param [in] samples - sample number.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ad469x_read_data(struct ad469x_dev *dev,
			 uint8_t channel,
			 uint32_t *buf,
			 uint16_t samples)
{
	int32_t ret;

#if !defined(USE_STANDARD_SPI)
	uint32_t commands_data[1];
	struct spi_engine_offload_message msg;
	uint32_t spi_eng_msg_cmds[3] = {
		CS_LOW,
		WRITE_READ(1),
		CS_HIGH
	};
	if (channel < dev->num_data_ch)
		commands_data[0] = AD469x_CMD_CONFIG_CH_SEL(channel) << 8;
	else if (channel == dev->num_data_ch)
		commands_data[0] = AD469x_CMD_SEL_TEMP_SNSOR_CH << 8;
	else
		return -1;

	no_os_pwm_enable(dev->trigger_pwm_desc);

	ret = spi_engine_offload_init(dev->spi_desc, dev->offload_init_param);
	if (ret != 0)
		return ret;

	msg.commands = spi_eng_msg_cmds;
	msg.no_commands = NO_OS_ARRAY_SIZE(spi_eng_msg_cmds);
	msg.rx_addr = (uint32_t)buf;
	msg.commands_data = commands_data;

	ret = spi_engine_offload_transfer(dev->spi_desc, msg, samples);
	if (ret != 0)
		return ret;

	if (dev->dcache_invalidate_range)
		dev->dcache_invalidate_range(msg.rx_addr, samples * 4);
#else
	ret = no_os_spi_write_and_read(dev->spi_desc, buf, (samples * 2));
	if (ret != 0)
		return ret;
#endif

	return ret;
}

/**
 * @brief Resets the ad469x device
 * @param [in] dev - ad469x_dev device handler.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad469x_reset_dev(struct ad469x_dev *dev)
{
	int32_t ret;
	uint8_t reset_status = 0;

	ret = no_os_gpio_set_value(dev->gpio_resetn, NO_OS_GPIO_LOW);
	if (ret != 0)
		return ret;

	no_os_mdelay(10);

	ret = no_os_gpio_set_value(dev->gpio_resetn, NO_OS_GPIO_HIGH);
	if (ret != 0)
		return ret;

	no_os_mdelay(10);

	ret = ad469x_spi_reg_read(dev, AD469x_REG_STATUS, &reset_status);
	if (ret != 0)
		return ret;

	if(!((reset_status & AD469x_REG_STATUS_RESET_MASK) >> 5))
		return -1;

	return 0;
}

/**
 * @brief Get the value of reference
 * @param device AD469x Device instance.
 * @param ref_set Value of VREF_SET
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad469x_get_reference(struct ad469x_dev *device,
			     enum ad469x_ref_set *ref_set)
{
	int32_t ret;
	uint8_t reg_data;

	if (!device)
		return -EINVAL;

	ret = ad469x_spi_reg_read(device, AD469x_REG_REF_CTRL, &reg_data);
	if (ret)
		return ret;

	*ref_set = no_os_field_get(AD469x_REG_REF_VREF_SET_MASK, reg_data);

	return 0;
}

/**
 * @brief Set the value of reference
 * @param device AD469x Device instance.
 * @param ref_set Value of VREF_SET
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad469x_set_reference(struct ad469x_dev *device,
			     enum ad469x_ref_set ref_set)
{
	return ad469x_spi_write_mask(device,
				     AD469x_REG_REF_CTRL,
				     AD469x_REG_REF_VREF_SET_MASK,
				     no_os_field_prep(AD469x_REG_REF_VREF_SET_MASK, ref_set));
}

/**
 * Configure the device with initial parameters.
 * @param [in, out] dev - The device structure.
 * @param [in] config_desc - Pointer to structure containing configuration
 *                           parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad469x_config(struct ad469x_dev *dev, struct
		      ad469x_init_param *config_desc)
{
	int32_t ret;

	ret = ad469x_set_reg_access_mode(dev, AD469x_BYTE_ACCESS);
	if (ret != 0)
		return ret;

	ret = ad469x_set_busy(dev, AD469x_busy_gp0);
	if (ret != 0)
		return ret;

	ret = ad469x_std_seq_osr(dev, config_desc->std_seq_osr);
	if (ret != 0)
		return ret;

	ret = ad469x_std_pin_pairing(dev, config_desc->std_seq_pin_pairing);
	if (ret != 0)
		return ret;

	if (config_desc->temp_enabled) {
		ret = ad469x_sequence_enable_temp(dev);
		if (ret != 0)
			return ret;
	}

	ret = ad469x_set_channel_sequence(dev, config_desc->ch_sequence);
	if (ret != 0)
		return ret;

	return 0;
}

/**
 * Configure the device with default parameters and enter conversion mode
 * this adds default ch/slot assigments based on sequece mode.
 * @param [in, out] dev - The device structure.
 * @param [in] config_desc - Pointer to structure containing configuration
 *                           parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad469x_config_extended(struct ad469x_dev *dev, struct
			       ad469x_init_param *config_desc)
{
	int32_t ret, i;
	uint32_t ch_mask = NO_OS_GENMASK(config_desc->num_data_ch - 1, 0);

	ret = ad469x_set_reg_access_mode(dev, AD469x_BYTE_ACCESS);
	if (ret != 0)
		return ret;

	ret = ad469x_set_busy(dev, AD469x_busy_gp0);
	if (ret != 0)
		return ret;

	if (config_desc->ch_sequence == AD469x_advanced_seq) {
		ret = ad469x_adv_sequence_set_num_slots(dev, dev->num_data_ch);
		if (ret)
			return ret;

		for (i = 0; i < config_desc->num_data_ch; i++) {
			ret = ad469x_adv_sequence_set_slot(dev, i, i);
			if (ret)
				return ret;

			ret = ad469x_adv_seq_osr(dev, i, config_desc->adv_seq_osr_resol[i]);
			if (ret)
				return ret;

		}
	} else if(config_desc->ch_sequence == AD469x_standard_seq)  {
		ret = ad469x_std_sequence_ch(dev, ch_mask);
		if (ret)
			return ret;

		ret = ad469x_std_seq_osr(dev, config_desc->std_seq_osr);
		if (ret != 0)
			return ret;
	}

	ret = ad469x_std_pin_pairing(dev, config_desc->std_seq_pin_pairing);
	if (ret != 0)
		return ret;

	if (config_desc->temp_enabled) {
		ret = ad469x_sequence_enable_temp(dev);
		if (ret != 0)
			return ret;
	}

	ret = ad469x_set_channel_sequence(dev, config_desc->ch_sequence);
	if (ret != 0)
		return ret;

	return ad469x_enter_conversion_mode(dev);
}

/**
 * @brief Configure analog input high Z mode
 * @param dev The device structure
 * @param ch Channel ID
 * @param status Status of analog input high Z bit
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad469x_configure_ain_high_z(struct ad469x_dev *dev, uint8_t ch,
				    enum ad469x_ain_high_z status)
{
	return ad469x_spi_write_mask(dev,
				     AD469x_REG_CONFIG_IN(dev->ch_sequence == AD469x_standard_seq ? 0 : ch),
				     AD469x_REG_CONFIG_IN_HIZ_EN_MASK,
				     AD469x_REG_CONFIG_IN_HIZ_EN(status));
}

/**
 * @brief Get the status of analog input high Z mode
 * @param dev The device structure
 * @param ch Channel ID
 * @param status Status of analog input high Z bit
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad469x_get_ain_high_z_status(struct ad469x_dev *dev,
				     uint8_t ch,
				     enum ad469x_ain_high_z *status)
{
	uint8_t reg_data;
	int32_t ret;

	ret = ad469x_spi_reg_read(dev,
				  AD469x_REG_CONFIG_IN(dev->ch_sequence == AD469x_standard_seq ? 0 : ch),
				  &reg_data);
	if (ret)
		return ret;

	*status = no_os_field_get(AD469x_REG_CONFIG_IN_HIZ_EN_MASK, reg_data);

	return 0;
}

/**
 * Initialize the device.
 * @param [out] device - The device structure.
 * @param [in] init_param - The structure that contains the device initial
 * 		parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t ad469x_init(struct ad469x_dev **device,
		    struct ad469x_init_param *init_param)
{
	struct ad469x_dev *dev;
	int32_t ret;
	uint8_t data = 0;
	uint8_t max_data_ch;
	uint32_t sample_frequncy_ksps;

	dev = (struct ad469x_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

#if !defined(USE_STANDARD_SPI)
	ret = axi_clkgen_init(&dev->clkgen, init_param->clkgen_init);
	if (ret != 0) {
		printf("error: %s: axi_clkgen_init() failed\n",
		       init_param->clkgen_init->name);
		goto error_dev;
	}

	ret = axi_clkgen_set_rate(dev->clkgen, init_param->axi_clkgen_rate);
	if (ret != 0) {
		printf("error: %s: axi_clkgen_set_rate() failed\n",
		       init_param->clkgen_init->name);
		goto error_clkgen;
	}
#endif

	ret = ad469x_init_gpio(dev, init_param);
	if (ret != 0)
		goto error_clkgen;

	ret = no_os_spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret != 0)
		goto error_gpio;

#if !defined(USE_STANDARD_SPI)
	dev->offload_init_param = init_param->offload_init_param;
	dev->reg_access_speed = init_param->reg_access_speed;
	dev->reg_data_width = init_param->reg_data_width;
	dev->capture_data_width = init_param->capture_data_width;
#endif
	dev->dev_id = init_param->dev_id;

	max_data_ch = dev_info[dev->dev_id].max_data_ch;
	if (init_param->num_data_ch > max_data_ch) {
		printf("Error: too many channels(%d). Max (%d)\n",
		       init_param->num_data_ch, max_data_ch);
		ret = -EINVAL;
		goto error_spi;
	} else if(init_param->num_data_ch == 0) {
		dev->num_data_ch = max_data_ch;
	} else {
		dev->num_data_ch = init_param->num_data_ch;
	}

	dev->dcache_invalidate_range = init_param->dcache_invalidate_range;
	dev->std_seq_osr = init_param->std_seq_osr;
	dev->std_seq_pin_pairing = init_param->std_seq_pin_pairing;
	dev->ch_sequence = init_param->ch_sequence;
	dev->num_slots = 0;
	dev->temp_enabled = false;
	memset(dev->ch_slots, 0, sizeof(dev->ch_slots));

	ret = ad469x_reset_dev(dev);
	if (ret != 0)
		goto error_spi;

	ret = ad469x_spi_reg_write(dev, AD469x_REG_SCRATCH_PAD, AD469x_TEST_DATA);
	if (ret != 0)
		goto error_spi;

	ret = ad469x_spi_reg_read(dev, AD469x_REG_SCRATCH_PAD, &data);
	if (ret != 0)
		goto error_spi;

	if (data != AD469x_TEST_DATA)
		goto error_spi;

	if (init_param->enable_extended_init) {
		ret = ad469x_config_extended(dev, init_param);
		if (ret != 0)
			goto error_spi;
	} else {
		ret = ad469x_config(dev, init_param);
		if (ret != 0)
			goto error_spi;
	}

#if !defined(USE_STANDARD_SPI)
	sample_frequncy_ksps = NO_OS_DIV_ROUND_UP(1000000,
			       init_param->trigger_pwm_init->period_ns);
	if (sample_frequncy_ksps > dev_info[dev->dev_id].max_rate_ksps) {
		printf("Error: Sample frequency too high (%ld)\n", sample_frequncy_ksps);
		ret = -EINVAL;
		goto error_spi;
	}

	ret = no_os_pwm_init(&dev->trigger_pwm_desc, init_param->trigger_pwm_init);
	if (ret != 0)
		goto error_spi;
#endif

	*device = dev;

	return ret;

error_spi:
	no_os_spi_remove(dev->spi_desc);
error_gpio:
	no_os_gpio_remove(dev->gpio_resetn);
	no_os_gpio_remove(dev->gpio_busy);
	no_os_gpio_remove(dev->gpio_convst);
error_clkgen:
#if !defined(USE_STANDARD_SPI)
	axi_clkgen_remove(dev->clkgen);
#endif
error_dev:
	no_os_free(dev);

	return -1;
}

/**
 * @brief Free the memory allocated by ad469x_init().
 * @param [in] dev - Pointer to the device handler.
 * @return 0 in case of success, -1 otherwise
 */
int32_t ad469x_remove(struct ad469x_dev *dev)
{
	int32_t ret;

	if (!dev)
		return -1;

#if !defined(USE_STANDARD_SPI)
	ret = no_os_pwm_remove(dev->trigger_pwm_desc);
	if (ret != 0)
		return ret;
#endif

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret != 0)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_resetn);
	if (ret != 0)
		return ret;

	no_os_gpio_remove(dev->gpio_busy);
	if (ret != 0)
		return ret;

	no_os_gpio_remove(dev->gpio_convst);
	if (ret != 0)
		return ret;

#if !defined(USE_STANDARD_SPI)
	ret = axi_clkgen_remove(dev->clkgen);
	if (ret != 0)
		return ret;
#endif

	no_os_free(dev);

	return ret;
}
