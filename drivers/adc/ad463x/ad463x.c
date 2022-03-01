/***************************************************************************//**
 *   @file   ad463x.c
 *   @brief  Implementation of AD463x Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
#include <stdlib.h>
#include <stdio.h>
#include "no_os_error.h"
#include "no_os_delay.h"
#include "ad463x.h"
#include "no_os_print_log.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD463x_TEST_DATA 0xAA

/******************************************************************************/
/************************* Functions Definitions ******************************/
/******************************************************************************/

/**
 * @brief Read device register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data read from the register.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ad463x_spi_reg_read(struct ad463x_dev *dev,
			    uint16_t reg_addr,
			    uint8_t *reg_data)
{
	int32_t ret;
	uint8_t buf[3];

	buf[0] = AD463X_REG_READ| ((reg_addr >> 8) & 0x7F);
	buf[1] = (uint8_t)reg_addr;
	buf[2] = AD463X_REG_READ_DUMMY;

	ret = spi_write_and_read(dev->spi_desc, buf, 3);
	*reg_data = buf[2];

	return ret;
}

/**
 * @brief Write device register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ad463x_spi_reg_write(struct ad463x_dev *dev,
			     uint16_t reg_addr,
			     uint8_t reg_data)
{
	uint8_t buf[3];

	buf[0] = ((reg_addr >> 8) & 0x7F);
	buf[1] = (uint8_t)reg_addr;
	buf[2] = reg_data;

	return spi_write_and_read(dev->spi_desc, buf, 3);
}

/**
 * @brief SPI write device register using a mask.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - The mask.
 * @param data - The register data.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ad463x_spi_reg_write_masked(struct ad463x_dev *dev,
				    uint16_t reg_addr,
				    uint8_t mask,
				    uint8_t data)
{
	uint8_t reg_data;
	int32_t ret;

	ret = ad463x_spi_reg_read(dev, reg_addr, &reg_data);
	if (ret != SUCCESS)
		return ret;

	reg_data &= ~mask;
	reg_data |= data;

	return ad463x_spi_reg_write(dev, reg_addr, reg_data);
}

/**
 * @brief Set power mode.
 * @param dev - The device structure.
 * @param mode - The power mode.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ad463x_set_pwr_mode(struct ad463x_dev *dev, uint8_t mode)
{
	if ((mode != AD463X_NORMAL_MODE) && (mode != AD463X_LOW_POWER_MODE))
		return FAILURE;

	return ad463x_spi_reg_write(dev, AD463X_REG_DEVICE_CONFIG, mode);
}

/**
 * @brief Set average frame length.
 * @param dev - The device structure.
 * @param mode - Average filter frame length mode.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ad463x_set_avg_frame_len(struct ad463x_dev *dev, uint8_t mode)
{
	int32_t ret;

	if (mode == AD463X_NORMAL_MODE) {
		ret = ad463x_spi_reg_write_masked(dev,
						  AD463X_REG_MODES,
						  AD463X_OUT_DATA_MODE_MSK,
						  AD463X_24_DIFF_8_COM);
		if (ret != SUCCESS)
			return ret;
		ret = ad463x_spi_reg_write(dev,
					   AD463X_REG_AVG,
					   AD463X_AVG_FILTER_RESET);
	} else {
		if(mode < 1 || mode > 16)
			return -EINVAL;
		ret = ad463x_spi_reg_write_masked(dev,
						  AD463X_REG_MODES,
						  AD463X_OUT_DATA_MODE_MSK,
						  AD463X_30_AVERAGED_DIFF);
		if (ret != SUCCESS)
			return ret;
		ret = ad463x_spi_reg_write(dev,
					   AD463X_REG_AVG,
					   mode);
	}

	return ret;
}

/**
 * @brief Set drive strength.
 * @param dev - The device structure.
 * @param mode - The register data.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ad463x_set_drive_strength(struct ad463x_dev *dev, uint8_t mode)
{
	if((mode != AD463X_NORMAL_OUTPUT_STRENGTH)
	    && (mode != AD463X_DOUBLE_OUTPUT_STRENGTH))
		return -EINVAL;

	return ad463x_spi_reg_write_masked(dev, AD463X_REG_IO,
					   AD463X_DRIVER_STRENGTH_MASK, mode);
}

/**
 * @brief Exit register configuration mode.
 * @param dev - The device structure.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ad463x_exit_reg_cfg_mode(struct ad463x_dev *dev)
{
	int32_t ret;

	ret = ad463x_spi_reg_write(dev, AD463X_REG_EXIT_CFG_MODE,
				   AD463X_EXIT_CFG_MODE);
	if (ret != SUCCESS)
		return ret;

	ret = spi_engine_set_transfer_width(dev->spi_desc,
					    dev->capture_data_width);
	if (ret != SUCCESS)
		return ret;

	spi_engine_set_speed(dev->spi_desc, dev->spi_desc->max_speed_hz);

	return ret;
}

/**
 * @brief Set channel gain
 * @param dev - The device structure.
 * @param ch_idx - The channel index.
 * @param gain - The gain value scaled by 10000.
 * 			Example: to set gain 1.5, use 150000
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ad463x_set_ch_gain(struct ad463x_dev *dev, uint8_t ch_idx,
			   uint64_t gain)
{
	int32_t ret;
	uint32_t g;

	if (gain < 0 || gain > AD463X_GAIN_MAX_VAL_SCALED)
		return -EINVAL;

	g = ((gain * 0xFFFF) / AD463X_GAIN_MAX_VAL_SCALED);

	ret = ad463x_spi_reg_write(dev,
				   AD463X_REG_CHAN_GAIN(ch_idx, AD463X_GAIN_LSB),
				   g);
	if (ret != SUCCESS)
		return ret;

	return ad463x_spi_reg_write(dev,
				    AD463X_REG_CHAN_GAIN(ch_idx, AD463X_GAIN_MSB),
				    g >> 8);
}

/**
 * @brief Set channel offset
 * @param dev - The device structure.
 * @param ch_idx - The channel index.
 * @param offset - The channel offset.
 * @return SUCCESS in case of success, negative error code otherwise.
 */
int32_t ad463x_set_ch_offset(struct ad463x_dev *dev, uint8_t ch_idx,
			     uint32_t offset)
{
	int32_t ret;

	ret = ad463x_spi_reg_write(dev,
				   AD463X_REG_CHAN_OFFSET(ch_idx, AD463X_OFFSET_0),
				   offset);
	if (ret < 0)
		return ret;

	ret = ad463x_spi_reg_write(dev,
				   AD463X_REG_CHAN_OFFSET(ch_idx, AD463X_OFFSET_1),
				   offset >> 8);
	if (ret < 0)
		return ret;

	return ad463x_spi_reg_write(dev,
				    AD463X_REG_CHAN_OFFSET(ch_idx, AD463X_OFFSET_2),
				    offset >> 16);
}


/**
 * @brief Initialize GPIO driver handlers for the GPIOs in the system.
 *        ad463x_init() helper function.
 * @param [out] dev - ad463x_dev device handler.
 * @param [in] init_param - Pointer to the initialization structure.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
static int32_t ad463x_init_gpio(struct ad463x_dev *dev,
				struct ad463x_init_param *init_param)
{
	int32_t ret;

	ret = gpio_get_optional(&dev->gpio_resetn, init_param->gpio_resetn);
	if (ret != SUCCESS)
		return ret;

	/** Reset to configure pins */
	if (dev->gpio_resetn) {
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
 * @brief Read from device.
 *        Enter register mode to read/write registers
 * @param [in] dev - ad469x_dev device handler.
 * @param [out] buf - data buffer.
 * @param [in] samples - sample number.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t ad463x_read_data(struct ad463x_dev *dev,
			 uint32_t *buf,
			 uint16_t samples)
{
	int32_t ret;
	uint32_t commands_data[1] = {0};
	struct spi_engine_offload_message msg;
	uint32_t spi_eng_msg_cmds[3] = {
		CS_LOW,
		READ(dev->read_bytes_no),
		CS_HIGH
	};

	ret = pwm_enable(dev->trigger_pwm_desc);
	if (ret != SUCCESS)
		return ret;

	ret = spi_engine_offload_init(dev->spi_desc, dev->offload_init_param);
	if (ret != SUCCESS)
		return ret;

	msg.commands = spi_eng_msg_cmds;
	msg.no_commands = ARRAY_SIZE(spi_eng_msg_cmds);
	msg.rx_addr = (uint32_t)buf;
	msg.commands_data = commands_data;

	ret = spi_engine_offload_transfer(dev->spi_desc, msg, samples);
	if (ret != SUCCESS)
		return ret;

	if (dev->dcache_invalidate_range)
		dev->dcache_invalidate_range(msg.rx_addr, samples * 2);

	return ret;
}

/**
 * @brief Initialize the device.
 * @param [out] device - The device structure.
 * @param [in] init_param - The structure that contains the device initial
 * 		parameters.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t ad463x_init(struct ad463x_dev **device,
		    struct ad463x_init_param *init_param)
{
	struct ad463x_dev *dev;
	int32_t ret;
	uint8_t data = 0;
	uint8_t sample_width;

	if (!init_param || !device)
		return FAILURE;

	if (init_param->clock_mode == AD463X_SPI_COMPATIBLE_MODE &&
	    init_param->data_rate == AD463X_DDR_MODE) {
		pr_err("DDR_MODE not available when clock mode is SPI_COMPATIBLE\n");
		return FAILURE;
	}

	dev = (struct ad463x_dev *)malloc(sizeof(*dev));
	if (!dev)
		return FAILURE;

	/** Perform Hardware Reset */
	ret = ad463x_init_gpio(dev, init_param);
	if (ret != SUCCESS)
		goto error_dev;

	ret = axi_clkgen_init(&dev->clkgen, init_param->clkgen_init);
	if (ret != SUCCESS) {
		pr_err("error: %s: axi_clkgen_init() failed\n",
		       init_param->clkgen_init->name);
		goto error_gpio;
	}

	ret = axi_clkgen_set_rate(dev->clkgen, init_param->axi_clkgen_rate);
	if (ret != SUCCESS) {
		pr_err("error: %s: axi_clkgen_set_rate() failed\n",
		       init_param->clkgen_init->name);
		goto error_clkgen;
	}

	ret = spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret != SUCCESS)
		goto error_clkgen;

	dev->offload_init_param = init_param->offload_init_param;
	dev->reg_access_speed = init_param->reg_access_speed;
	dev->reg_data_width = init_param->reg_data_width;
	dev->output_mode = init_param->output_mode;
	dev->lane_mode = init_param->lane_mode;
	dev->clock_mode = init_param->clock_mode;
	dev->data_rate = init_param->data_rate;
	dev->device_id = init_param->device_id;
	dev->dcache_invalidate_range = init_param->dcache_invalidate_range;

	if (dev->output_mode > AD463X_16_DIFF_8_COM)
		sample_width = 32;
	else
		sample_width = 24;

	switch (dev->lane_mode) {
	case AD463X_ONE_LANE_PER_CH:
		dev->capture_data_width = sample_width;
		break;

	case AD463X_TWO_LANES_PER_CH:
		dev->capture_data_width = sample_width / 2;
		break;

	case AD463X_FOUR_LANES_PER_CH:
		dev->capture_data_width = sample_width / 4;
		break;

	case AD463X_SHARED_TWO_CH:
		dev->capture_data_width = sample_width * 2;
		break;
	default:
		goto error_spi;
		break;
	}

	if (dev->data_rate == AD463X_DDR_MODE)
		dev->capture_data_width /= 2;

	dev->read_bytes_no = dev->capture_data_width / 8;

	ret = spi_engine_set_transfer_width(dev->spi_desc, dev->reg_data_width);
	if (ret != SUCCESS)
		goto error_spi;

	spi_engine_set_speed(dev->spi_desc, dev->reg_access_speed);

	ret = ad463x_spi_reg_read(dev, AD463X_CONFIG_TIMING, &data);
	if (ret != SUCCESS)
		goto error_spi;

	ret = ad463x_spi_reg_write(dev, AD463X_REG_SCRATCH_PAD, AD463x_TEST_DATA);
	if (ret != SUCCESS)
		goto error_spi;

	ret = ad463x_spi_reg_read(dev, AD463X_REG_SCRATCH_PAD, &data);
	if (ret != SUCCESS)
		goto error_spi;

	if (data != AD463x_TEST_DATA) {
		pr_err("Test Data read failed.\n");
		goto error_spi;
	}

	ret = ad463x_spi_reg_write_masked(dev, AD463X_REG_MODES, AD463X_LANE_MODE_MSK,
					  dev->lane_mode);
	if (ret != SUCCESS)
		return ret;

	ret = ad463x_spi_reg_write_masked(dev, AD463X_REG_MODES, AD463X_CLK_MODE_MSK,
					  dev->clock_mode);
	if (ret != SUCCESS)
		return ret;

	ret = ad463x_spi_reg_write_masked(dev, AD463X_REG_MODES, AD463X_DDR_MODE_MSK,
					  dev->data_rate);
	if (ret != SUCCESS)
		return ret;

	ret = ad463x_spi_reg_write_masked(dev, AD463X_REG_MODES,
					  AD463X_OUT_DATA_MODE_MSK, dev->output_mode);
	if (ret != SUCCESS)
		return ret;

	ret = pwm_init(&dev->trigger_pwm_desc, init_param->trigger_pwm_init);
	if (ret != SUCCESS)
		goto error_spi;

	*device = dev;

	return ret;

error_spi:
	spi_remove(dev->spi_desc);
error_clkgen:
	axi_clkgen_remove(dev->clkgen);
error_gpio:
	gpio_remove(dev->gpio_resetn);
error_dev:
	free(dev);

	return FAILURE;
}

/**
 * @brief Free the memory allocated by ad463x_init().
 * @param [in] dev - Pointer to the device handler.
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise
 */
int32_t ad463x_remove(struct ad463x_dev *dev)
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
