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
#include "stdio.h"
#include "stdlib.h"
#include "ad469x.h"
#include "spi_engine.h"
#include "delay.h"
#include "error.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD469x_TEST_DATA 0xEA

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
	*reg_data = buf[1];

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

	*data = (reg_data[1] & mask);

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

		ret = ad469x_spi_write_mask(dev,
					    AD469x_REG_SEQ_CTRL,
					    AD469x_SEQ_CTRL_NUM_SLOTS_AS_MASK,
					    AD469x_SEQ_CTRL_NUM_SLOTS_AS(1));
		if (ret != SUCCESS)
			return ret;

		break;

	default:
		return FAILURE;
		break;
	}

	return ret;
}

/**
 * @brief Configure advanced sequencer number of slots
 * @param [in] dev - ad469x_dev device handler.
 * @param [in] num_slots - Number of slots, max value = 0x7f
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t ad469x_adv_sequence_set_num_slots(struct ad469x_dev *dev,
		uint8_t num_slots)
{
	return ad469x_spi_reg_write(dev,
				    AD469x_REG_SEQ_CTRL,
				    AD469x_SEQ_CTRL_NUM_SLOTS_AS(num_slots));
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
	return ad469x_spi_reg_write(dev,
				    AD469x_REG_AS_SLOT(slot),
				    AD469x_REG_AS_SLOT_INX(channel));
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
	return ad469x_spi_write_mask(dev,
				     AD469x_REG_TEMP_CTRL,
				     AD469x_REG_TEMP_CTRL_TEMP_EN_MASK,
				     AD469x_REG_TEMP_CTRL_TEMP_EN(1));
}

/**
 * @brief Disable temperature read at the end of the sequence, for standard and
 * advanced sequencer
 * @param [in] dev - ad469x_dev device handler.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t ad469x_sequence_disable_temp(struct ad469x_dev *dev)
{
	return ad469x_spi_write_mask(dev,
				     AD469x_REG_TEMP_CTRL,
				     AD469x_REG_TEMP_CTRL_TEMP_EN_MASK,
				     AD469x_REG_TEMP_CTRL_TEMP_EN(0));
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
 * @brief Read from device when converter has the channel sequencer activated.
 *        Enter register mode to read/write registers
 * @param [in] dev - ad469x_dev device handler.
 * @param [out] buf - data buffer.
 * @param [in] samples - sample number.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t ad469x_seq_read_data(struct ad469x_dev *dev,
			     uint32_t *buf,
			     uint16_t samples)
{
	return ad469x_read_data(dev, 0, buf, samples);
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

	commands_data[0] = AD469x_CMD_CONFIG_CH_SEL(channel) << 8;

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
	ad469x_remove(dev);

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
