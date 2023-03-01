/***************************************************************************//**
 *   @file   adaq8092.c
 *   @brief  Implementation of ADAQ8092 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#include <errno.h>
#include "adaq8092.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/******************************************************************************/

/**
 * @brief Read device register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data read from the register.
 * @return 0 in case of success, negative error code otherwise.
 */
int adaq8092_read(struct adaq8092_dev *dev, uint8_t reg_addr, uint8_t *reg_data)
{
	int ret;
	uint8_t buff[2] = {0};

	buff[0] = ADAQ8092_SPI_READ | reg_addr;

	ret = no_os_spi_write_and_read(dev->spi_desc, buff, 2);
	if (ret)
		return ret;

	*reg_data = buff[1];

	return 0;
}

/**
 * @brief Write device register.
 * @param dev- The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int adaq8092_write(struct adaq8092_dev *dev, uint8_t reg_addr, uint8_t reg_data)
{
	uint8_t buff[2] = {0};

	buff[0] = reg_addr;
	buff[1] = reg_data;

	return no_os_spi_write_and_read(dev->spi_desc, buff, 2);
}

/**
 * @brief Update specific register bits.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - Specific bits mask.
 * @param reg_data - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int adaq8092_update_bits(struct adaq8092_dev *dev, uint8_t reg_addr,
			 uint8_t mask, uint8_t reg_data)
{
	int ret;
	uint8_t data;

	ret = adaq8092_read(dev, reg_addr, &data);
	if (ret)
		return ret;

	data &= ~mask;
	data |= reg_data;

	return adaq8092_write(dev, reg_addr, data);
}

/**
 * @brief Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int adaq8092_init(struct adaq8092_dev **device,
		  struct adaq8092_init_param init_param)
{
	struct adaq8092_dev *dev;
	int ret;

	dev = (struct adaq8092_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* SPI Initialization*/
	ret = no_os_spi_init(&dev->spi_desc, init_param.spi_init);
	if (ret)
		goto error_dev;

	/* GPIO Initialization */
	ret = no_os_gpio_get(&dev->gpio_adc_pd1, init_param.gpio_adc_pd1_param);
	if (ret)
		goto error_spi;

	ret = no_os_gpio_get(&dev->gpio_adc_pd2, init_param.gpio_adc_pd2_param);
	if (ret)
		goto error_adc_pd1;

	ret = no_os_gpio_get(&dev->gpio_en_1p8, init_param.gpio_en_1p8_param);
	if (ret)
		goto error_adc_pd2;

	ret = no_os_gpio_get(&dev->gpio_par_ser, init_param.gpio_par_ser_param);
	if (ret)
		goto error_en_1p8;

	/* Powerup Sequence */
	ret = no_os_gpio_direction_output(dev->gpio_adc_pd1, NO_OS_GPIO_LOW);
	if (ret)
		goto error_par_ser;

	ret = no_os_gpio_direction_output(dev->gpio_adc_pd2, NO_OS_GPIO_LOW);
	if (ret)
		goto error_par_ser;

	ret = no_os_gpio_direction_output(dev->gpio_en_1p8, NO_OS_GPIO_LOW);
	if (ret)
		goto error_par_ser;

	ret = no_os_gpio_direction_output(dev->gpio_par_ser, NO_OS_GPIO_LOW);
	if (ret)
		goto error_par_ser;

	no_os_mdelay(1000);

	ret = no_os_gpio_set_value(dev->gpio_en_1p8, NO_OS_GPIO_HIGH);
	if (ret)
		goto error_par_ser;

	no_os_mdelay(500);

	ret = no_os_gpio_set_value(dev->gpio_adc_pd1, NO_OS_GPIO_HIGH);
	if (ret)
		goto error_par_ser;

	no_os_mdelay(1);

	ret = no_os_gpio_set_value(dev->gpio_adc_pd2, NO_OS_GPIO_HIGH);
	if (ret)
		goto error_par_ser;

	/* Software Reset */
	ret = adaq8092_write(dev,  ADAQ8092_REG_RESET, no_os_field_prep(ADAQ8092_RESET,
			     1));
	if (ret)
		goto error_par_ser;

	no_os_mdelay(100);

	/* Device Initialization */
	ret = adaq8092_set_pd_mode(dev, init_param.pd_mode);
	if (ret)
		goto error_par_ser;

	ret = adaq8092_set_clk_pol_mode(dev, init_param.clk_pol_mode);
	if (ret)
		goto error_par_ser;

	ret = adaq8092_set_clk_phase_mode(dev, init_param.clk_phase_mode);
	if (ret)
		goto error_par_ser;

	ret = adaq8092_set_clk_dc_mode(dev, init_param.clk_dc_mode);
	if (ret)
		goto error_par_ser;

	ret = adaq8092_set_lvds_cur_mode(dev, init_param.lvds_cur_mode);
	if (ret)
		goto error_par_ser;

	ret = adaq8092_set_lvds_term_mode(dev, init_param.lvds_term_mode);
	if (ret)
		goto error_par_ser;

	ret = adaq8092_set_dout_en(dev, init_param.dout_en);
	if (ret)
		goto error_par_ser;

	ret = adaq8092_set_dout_mode(dev, init_param.dout_mode);
	if (ret)
		goto error_par_ser;

	ret = adaq8092_set_test_mode(dev, init_param.test_mode);
	if (ret)
		goto error_par_ser;

	ret = adaq8092_set_alt_pol_en(dev, init_param.alt_bit_pol_en);
	if (ret)
		goto error_par_ser;

	ret = adaq8092_set_data_rand_en(dev, init_param.data_rand_en);
	if (ret)
		goto error_par_ser;

	ret = adaq8092_set_twos_comp(dev, init_param.twos_comp);
	if (ret)
		goto error_par_ser;

	*device = dev;

	return 0;

error_par_ser:
	no_os_gpio_remove(dev->gpio_par_ser);
error_en_1p8:
	no_os_gpio_remove(dev->gpio_en_1p8);
error_adc_pd2:
	no_os_gpio_remove(dev->gpio_adc_pd2);
error_adc_pd1:
	no_os_gpio_remove(dev->gpio_adc_pd1);
error_spi:
	no_os_spi_remove(dev->spi_desc);
error_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Remove the device and release resources.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int adaq8092_remove(struct adaq8092_dev *dev)
{
	int ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_adc_pd1);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_adc_pd2);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_en_1p8);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_par_ser);
	if (ret)
		return ret;

	no_os_free(dev);

	return ret;
}

/**
 * @brief Set the device powerodown mode.
 * @param dev - The device structure.
 * @param mode - The device mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int adaq8092_set_pd_mode(struct adaq8092_dev *dev,
			 enum adaq8092_powerdown_modes mode)
{
	int ret;

	ret = adaq8092_update_bits(dev, ADAQ8092_REG_POWERDOWN,
				   ADAQ8092_POWERDOWN_MODE,
				   no_os_field_prep(ADAQ8092_POWERDOWN_MODE, mode));
	if (ret)
		return ret;

	dev->pd_mode = mode;

	return 0;
}

/**
 * @brief Get the device powerdown mode.
 * @param dev - The device structure.
 * @return powerdown mode.
 */
enum adaq8092_powerdown_modes adaq8092_get_pd_mode(struct adaq8092_dev *dev)
{
	return dev->pd_mode;
}

/**
 * @brief Set the clock polarity mode.
 * @param dev - The device structure.
 * @param mode - The device mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int adaq8092_set_clk_pol_mode(struct adaq8092_dev *dev,
			      enum adaq8092_clk_invert mode)
{
	int ret;

	ret = adaq8092_update_bits(dev, ADAQ8092_REG_TIMING,
				   ADAQ8092_CLK_INVERT,
				   no_os_field_prep(ADAQ8092_CLK_INVERT, mode));
	if (ret)
		return ret;

	dev->clk_pol_mode = mode;

	return 0;
}

/**
 * @brief Get the clock polarity mode.
 * @param dev - The device structure.
 * @return clock polarity mode.
 */
enum adaq8092_clk_invert adaq8092_get_clk_pol_mode(struct adaq8092_dev *dev)
{
	return dev->clk_pol_mode;
}

/**
 * @brief Set the clock phase delay mode.
 * @param dev - The device structure.
 * @param mode - The device mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int adaq8092_set_clk_phase_mode(struct adaq8092_dev *dev,
				enum adaq8092_clk_phase_delay mode)
{
	int ret;

	ret = adaq8092_update_bits(dev, ADAQ8092_REG_TIMING,
				   ADAQ8092_CLK_PHASE,
				   no_os_field_prep(ADAQ8092_CLK_PHASE, mode));
	if (ret)
		return ret;

	dev->clk_phase_mode = mode;

	return 0;
}

/**
 * @brief Get the clock phase delay mode.
 * @param dev - The device structure.
 * @return clock phase delay mode.
 */
enum adaq8092_clk_phase_delay adaq8092_get_clk_phase_mode(
	struct adaq8092_dev *dev)
{
	return dev->clk_phase_mode;
}

/**
 * @brief Set the clock duty cycle stabilizer mode.
 * @param dev - The device structure.
 * @param mode - The device mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int adaq8092_set_clk_dc_mode(struct adaq8092_dev *dev,
			     enum adaq8092_clk_dutycycle mode)
{
	int ret;

	ret = adaq8092_update_bits(dev, ADAQ8092_REG_TIMING,
				   ADAQ8092_CLK_DUTYCYCLE,
				   no_os_field_prep(ADAQ8092_CLK_DUTYCYCLE, mode));
	if (ret)
		return ret;

	dev->clk_dc_mode = mode;

	return 0;
}

/**
 * @brief Get the clock duty cycle stabilizer mode.
 * @param dev - The device structure.
 * @return clock duty cycle stabilizer mode.
 */
enum adaq8092_clk_dutycycle adaq8092_get_clk_dc_mode(struct adaq8092_dev *dev)
{
	return dev->clk_dc_mode;
}

/**
 * @brief Set the LVDS output current mode.
 * @param dev - The device structure.
 * @param mode - The device mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int adaq8092_set_lvds_cur_mode(struct adaq8092_dev *dev,
			       enum adaq8092_lvds_out_current mode)
{
	int ret;

	ret = adaq8092_update_bits(dev, ADAQ8092_REG_OUTPUT_MODE,
				   ADAQ8092_ILVDS,
				   no_os_field_prep(ADAQ8092_ILVDS, mode));
	if (ret)
		return ret;

	dev->lvds_cur_mode = mode;

	return 0;
}

/**
 * @brief Get the LVDS output current mode.
 * @param dev - The device structure.
 * @return LVDS output current mode.
 */
enum adaq8092_lvds_out_current adaq8092_get_lvds_cur_mode(
	struct adaq8092_dev *dev)
{
	return dev->lvds_cur_mode;
}

/**
 * @brief Set the LVDS internal temination mode.
 * @param dev - The device structure.
 * @param mode - The device mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int adaq8092_set_lvds_term_mode(struct adaq8092_dev *dev,
				enum adaq8092_internal_term mode)
{
	int ret;

	ret = adaq8092_update_bits(dev, ADAQ8092_REG_OUTPUT_MODE,
				   ADAQ8092_TERMON,
				   no_os_field_prep(ADAQ8092_TERMON, mode));
	if (ret)
		return ret;

	dev->lvds_term_mode = mode;

	return 0;
}

/**
 * @brief Get the LVDS internal temination device mode.
 * @param dev - The device structure.
 * @return LVDS internal temination device mode.
 */
enum adaq8092_internal_term adaq8092_get_lvds_term_mode(
	struct adaq8092_dev *dev)
{
	return dev->lvds_term_mode;
}

/**
 * @brief Set digital outputs.
 * @param dev - The device structure.
 * @param mode - The device mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int adaq8092_set_dout_en(struct adaq8092_dev *dev,
			 enum adaq8092_dout_enable mode)
{
	int ret;

	ret = adaq8092_update_bits(dev, ADAQ8092_REG_OUTPUT_MODE,
				   ADAQ8092_OUTOFF,
				   no_os_field_prep(ADAQ8092_OUTOFF, mode));
	if (ret)
		return ret;

	dev->dout_en = mode;

	return 0;
}

/**
 * @brief Get digital outputs.
 * @param dev - The device structure.
 * @return digital outputs.
 */
enum adaq8092_dout_enable adaq8092_get_dout_en(struct adaq8092_dev *dev)
{
	return dev->dout_en;
}

/**
 * @brief Set the digital output mode.
 * @param dev - The device structure.
 * @param mode - The device mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int adaq8092_set_dout_mode(struct adaq8092_dev *dev,
			   enum adaq8092_dout_modes mode)
{
	int ret;

	ret = adaq8092_update_bits(dev, ADAQ8092_REG_OUTPUT_MODE,
				   ADAQ8092_OUTMODE,
				   no_os_field_prep(ADAQ8092_OUTMODE, mode));
	if (ret)
		return ret;

	dev->dout_mode = mode;

	return 0;
}

/**
 * @brief Get the digital output mode.
 * @param dev - The device structure.
 * @return digital output mode.
 */
enum adaq8092_dout_modes adaq8092_get_dout_mode(struct adaq8092_dev *dev)
{
	return dev->dout_mode;
}

/**
 * @brief Set digital output test pattern mode.
 * @param dev - The device structure.
 * @param mode - The device mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int adaq8092_set_test_mode(struct adaq8092_dev *dev,
			   enum adaq8092_out_test_modes mode)
{
	int ret;

	ret = adaq8092_update_bits(dev, ADAQ8092_REG_DATA_FORMAT,
				   ADAQ8092_OUTTEST,
				   no_os_field_prep(ADAQ8092_OUTTEST, mode));
	if (ret)
		return ret;

	dev->test_mode = mode;

	return 0;
}

/**
 * @brief Get digital output test pattern mode.
 * @param dev - The device structure.
 * @return digital output test pattern mode.
 */
enum adaq8092_out_test_modes adaq8092_get_test_mode(struct adaq8092_dev *dev)
{
	return dev->test_mode;
}

/**
 * @brief Set the alternate bit polarity mode.
 * @param dev - The device structure.
 * @param mode - The device mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int adaq8092_set_alt_pol_en(struct adaq8092_dev *dev,
			    enum adaq8092_alt_bit_pol mode)
{
	int ret;

	ret = adaq8092_update_bits(dev, ADAQ8092_REG_DATA_FORMAT,
				   ADAQ8092_ABP,
				   no_os_field_prep(ADAQ8092_ABP, mode));
	if (ret)
		return ret;

	dev->alt_bit_pol_en = mode;

	return 0;
}

/**
 * @brief Get the alternate bit polarity mode.
 * @param dev - The device structure.
 * @return alternate bit polarity mode.
 */
enum adaq8092_alt_bit_pol adaq8092_get_alt_pol_en(struct adaq8092_dev *dev)
{
	return dev->alt_bit_pol_en;
}

/**
 * @brief Set the data output randomizer mode.
 * @param dev - The device structure.
 * @param mode - The device mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int adaq8092_set_data_rand_en(struct adaq8092_dev *dev,
			      enum adaq8092_data_rand mode)
{
	int ret;

	ret = adaq8092_update_bits(dev, ADAQ8092_REG_DATA_FORMAT,
				   ADAQ8092_RAND,
				   no_os_field_prep(ADAQ8092_RAND, mode));
	if (ret)
		return ret;

	dev->data_rand_en = mode;

	return 0;
}

/**
 * @brief Get the data output randomizer mode.
 * @param dev - The device structure.
 * @return data output randomizer mode.
 */
enum adaq8092_data_rand adaq8092_get_data_rand_en(struct adaq8092_dev *dev)
{
	return dev->data_rand_en;
}

/**
 * @brief Set the Tows Complement mode.
 * @param dev - The device structure.
 * @param mode - The device mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int adaq8092_set_twos_comp(struct adaq8092_dev *dev,
			   enum adaq8092_twoscomp mode)
{
	int ret;

	ret = adaq8092_update_bits(dev, ADAQ8092_REG_DATA_FORMAT,
				   ADAQ8092_TWOSCOMP,
				   no_os_field_prep(ADAQ8092_TWOSCOMP, mode));
	if (ret)
		return ret;

	dev->twos_comp = mode;

	return 0;
}

/**
 * @brief Get the Tows Complement mode.
 * @param dev - The device structure.
 * @return Tows Complement mode.
 */
enum adaq8092_twoscomp adaq8092_get_twos_comp(struct adaq8092_dev *dev)
{
	return dev->twos_comp;
}
