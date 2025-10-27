/*******************************************************************************
 *   @file   ad5141.c
 *   @brief  Source file for the AD5141 digital potentiometer drivers
********************************************************************************
 Copyright 2025(c) Analog Devices, Inc.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of Analog Devices, Inc. nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include "ad5141.h"
#include "no_os_delay.h"

#define AD5141_NVM_WRITE_DELAY 18

/* AD5141 dpot command address mapped to channel type */
uint8_t ad5141_dpot_cmd_addr[] = {
	[DPOT_CHN_RDAC1] = 0x0,
	[DPOT_CHN_R_AW1] = 0x4,
	[DPOT_CHN_R_WB1] = 0x0,
};
/* AD5142 dpot command address mapped to channel type */
uint8_t ad5141_dpot_cmd_nvm_addr[] = {
	[DPOT_CHN_RDAC1] = 0x0,
	[DPOT_CHN_R_AW1] = 0x1,
	[DPOT_CHN_R_WB1] = 0x0,
};
/**
 * @brief Initialize the AD5141 digipot GPIOs.
 * @param param - AD5141 digipot init parameters.
 * @param dev - AD5141 digipot descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad5141_dpot_gpio_init(struct ad5141_dpot_init_param *param,
				 struct ad5141_dpot_dev *dev)
{
	int ret;
	uint8_t gpio_state;

	if (!param || !dev)
		return -EINVAL;

	/* Reset GPIO */
	ret = no_os_gpio_get_optional(&dev->reset_gpio_desc, param->reset_gpio_init);
	if (ret)
		return ret;

	if (dev->reset_gpio_desc) {
		/* Set high for normal operation */
		ret = no_os_gpio_direction_output(dev->reset_gpio_desc, NO_OS_GPIO_HIGH);
		if (ret)
			return ret;
	}

	/* EEPROM WP GPIO */
	ret = no_os_gpio_get_optional(&dev->wp_gpio_desc, param->wp_gpio_init);
	if (ret)
		return ret;

	if (dev->wp_gpio_desc) {
		/* Set high to allow EEPROM write */
		ret = no_os_gpio_direction_output(dev->wp_gpio_desc, NO_OS_GPIO_HIGH);
		if (ret)
			return ret;
	}

	/* LRDAC GPIO */
	ret = no_os_gpio_get_optional(&dev->lrdac_gpio_desc, param->lrdac_gpio_init);
	if (ret)
		return ret;

	if (dev->lrdac_gpio_desc) {
		ret = no_os_gpio_direction_output(dev->lrdac_gpio_desc, NO_OS_GPIO_HIGH);
		if (ret)
			return ret;
	}

	/* DIS GPIO */
	ret = no_os_gpio_get_optional(&dev->dis_gpio_desc, param->dis_gpio_init);
	if (ret)
		return ret;

	if (dev->dis_gpio_desc) {
		if (param->eintf_type == AD_SPI_INTERFACE)
			gpio_state = NO_OS_GPIO_LOW;
		else
			gpio_state = NO_OS_GPIO_HIGH;

		ret = no_os_gpio_direction_output(dev->dis_gpio_desc, gpio_state);
		if (ret)
			return ret;
	}

	/* INDEP GPIO */
	ret = no_os_gpio_get_optional(&dev->indep_gpio_desc, param->indep_gpio_init);
	if (ret)
		return ret;

	if (dev->indep_gpio_desc) {
		if (param->eoperating_mode == DPOT_LINEAR_GAIN_SETTING_MODE)
			gpio_state = NO_OS_GPIO_HIGH;
		else
			gpio_state = NO_OS_GPIO_LOW;

		ret = no_os_gpio_direction_output(dev->indep_gpio_desc, gpio_state);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Free the memory allocated by ad5141_dpot_gpio_init().
 * @param dev - AD5141 digipot descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad5141_dpot_gpio_remove(struct ad5141_dpot_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	if (dev->reset_gpio_desc) {
		ret = no_os_gpio_remove(dev->reset_gpio_desc);
		if (ret)
			return ret;
	}

	if (dev->wp_gpio_desc) {
		ret = no_os_gpio_remove(dev->wp_gpio_desc);
		if (ret)
			return ret;
	}

	if (dev->lrdac_gpio_desc) {
		ret = no_os_gpio_remove(dev->lrdac_gpio_desc);
		if (ret)
			return ret;
	}

	if (dev->dis_gpio_desc) {
		ret = no_os_gpio_remove(dev->dis_gpio_desc);
		if (ret)
			return ret;
	}

	if (dev->indep_gpio_desc) {
		ret = no_os_gpio_remove(dev->indep_gpio_desc);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Validate the correctness of input channel.
 * @param ad5141_desc - AD5141 digipot descriptor.
 * @param chn  - digipot channel.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad5141_validate_chn(struct ad5141_dpot_dev *ad5141_desc,
			       enum dpot_chn_type chn)
{
	if (ad5141_desc->operating_mode == DPOT_LINEAR_GAIN_SETTING_MODE) {
		if (chn < DPOT_CHN_R_AW1 || chn > DPOT_CHN_R_WB1)
			return -EINVAL;
	} else {
		if (chn > DPOT_CHN_RDAC1)
			return -EINVAL;
	}

	return 0;
}

/**
 * @brief Initialize the AD5141 digital potentiometer.
 * @param param - digipot init parameters.
 * @param desc - digipot descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5141_dpot_init(struct dpot_init_param *param, struct dpot_dev **desc)
{
	int ret, nIndex = 0;
	struct dpot_dev *dev;
	struct ad5141_dpot_init_param *ad5141_params;
	struct ad5141_dpot_dev *ad5141_dev;
	struct no_os_spi_init_param *spi_init;
	struct no_os_i2c_init_param *i2c_init;
	enum dpot_chn_type chn;

	if (!param)
		return -EINVAL;

	/* Allocate memory for digipot device descriptor */
	dev = (struct dpot_dev *)calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* Allocate memory for AD5141 digipot device descriptor */
	ad5141_dev = (struct ad5141_dpot_dev *)calloc(1, sizeof(*ad5141_dev));
	if (!ad5141_dev) {
		ret = -ENOMEM;
		goto err_ad5141_dev;
	}

	ad5141_params = param->extra;
	dev->extra = ad5141_dev;
	ad5141_params->eintf_type = param->intf_type;
	ad5141_params->eoperating_mode =  param->operating_mode;

	/* Initialize GPIOs */
	ret = ad5141_dpot_gpio_init(ad5141_params, ad5141_dev);
	if (ret)
		goto err_dpot_gpio;

	/* Initialize the digital interface */
	if (ad5141_params->eintf_type == AD_SPI_INTERFACE) {
		spi_init = ((struct ad5141_dpot_init_param *)param->extra)->spi_init;
		ret = no_os_spi_init(&ad5141_dev->spi_desc, spi_init);
		if (ret)
			goto err_dpot_init;
	} else if (ad5141_params->eintf_type == AD_I2C_INTERFACE) {
		i2c_init = ((struct ad5141_dpot_init_param *)param->extra)->i2c_init;
		ret = no_os_i2c_init(&ad5141_dev->i2c_desc, i2c_init);
		if (ret)
			goto err_dpot_init;
	} else {
		/* Interface not supported */
		ret = -EINVAL;
		goto err_intf_init;
	}

	ad5141_dev->intf_type = ad5141_params->eintf_type;

	/* Reset dpot */
	ret = ad5141_dpot_reset(dev);
	if (ret)
		goto err_dpot_init;
	no_os_udelay(30 * 5);
	/* Set operating mode */
	ret = ad5141_dpot_set_operating_mode(dev, ad5141_params->eoperating_mode);
	if (ret)
		goto err_dpot_init;

	/* Enable/disable shutdown */
	if (ad5141_params->eoperating_mode == DPOT_LINEAR_GAIN_SETTING_MODE) {
		for (chn = DPOT_CHN_R_AW1; chn <= DPOT_CHN_R_WB1; chn++) {
			ret = ad5141_dpot_shutdown(dev, chn,
						   ad5141_params->shutdown_enable_lg[nIndex++]);
			if (ret)
				goto err_dpot_init;
		}
	} else {
		ret = ad5141_dpot_shutdown(dev, DPOT_CHN_RDAC1,
					   ad5141_params->shutdown_enable_pt[nIndex]);
		if (ret)
			goto err_dpot_init;
	}

	*desc = dev;

	return 0;

err_dpot_init:
	if (ad5141_dev->spi_desc)
		no_os_spi_remove(ad5141_dev->spi_desc);
	if (ad5141_dev->i2c_desc)
		no_os_i2c_remove(ad5141_dev->i2c_desc);
err_intf_init:
	ad5141_dpot_gpio_remove(ad5141_dev);
err_dpot_gpio:
	free(ad5141_dev);
err_ad5141_dev:
	free(dev);

	return ret;
}

/**
 * @brief Free the memory allocated by ad5141_dpot_init().
 * @param desc - digipot descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5141_dpot_remove(struct dpot_dev *desc)
{
	int ret;
	struct ad5141_dpot_dev *ad5141_dev;

	if (!desc || !desc->extra)
		return -EINVAL;

	ad5141_dev = desc->extra;

	ret = ad5141_dpot_gpio_remove(ad5141_dev);
	if (ret)
		return ret;

	if (ad5141_dev->intf_type == AD_SPI_INTERFACE) {
		ret = no_os_spi_remove((struct no_os_spi_desc *)ad5141_dev->spi_desc);
		if (ret)
			return ret;
	} else if (ad5141_dev->intf_type == AD_I2C_INTERFACE) {
		ret = no_os_i2c_remove((struct no_os_i2c_desc *)ad5141_dev->i2c_desc);
		if (ret)
			return ret;
	} else {
		/* Interface not supported */
		return -EINVAL;
	}

	free(ad5141_dev);
	free(desc);

	return 0;
}

/**
 * @brief Reset the AD5141 digital potentiometer.
 * @param desc - digipot descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5141_dpot_reset(struct dpot_dev *desc)
{
	int ret;
	struct dpot_command cmd;
	struct ad5141_dpot_dev *ad5141_desc;

	if (!desc)
		return -EINVAL;

	ad5141_desc = desc->extra;

	if (ad5141_desc->reset_gpio_desc) {
		/* Hardware reset */
		ret = no_os_gpio_set_value(ad5141_desc->reset_gpio_desc, NO_OS_GPIO_LOW);
		if (ret)
			return ret;

		/* Min reset low time as per datasheet spec is 0.1usec */
		no_os_udelay(1);

		ret = no_os_gpio_set_value(ad5141_desc->reset_gpio_desc, NO_OS_GPIO_HIGH);
		if (ret)
			return ret;
	} else {
		/* Software reset (command #14) */
		cmd.control = EXTRACT_CMD_BITS(AD51XX_CMD_SW_RESET);
		cmd.address = EXTRACT_ADS_BITS(AD51XX_CMD_SW_RESET);
		cmd.data    = EXTRACT_DATA_BITS(AD51XX_CMD_SW_RESET);
		cmd.is_readback = false;

		ret = ad5141_dpot_send_cmd(desc, &cmd);
		if (ret)
			return ret;
	}

	/* Reset EEPROM restore time as per datasheet spec.
	 * This delay ensures RDAC contents are loaded into EEPROM
	 * after device reset */
	no_os_udelay(30);

	return 0;
}

/**
 * @brief Shutdown the AD5141 digital potentiometer channel.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param shutdown_enable - Shutdown enable status flag.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5141_dpot_shutdown(struct dpot_dev *desc,
			 enum dpot_chn_type chn, bool shutdown_enable)
{
	int ret;
	struct dpot_command cmd;
	struct ad5141_dpot_dev *ad5141_desc;
	if (!desc)
		return -EINVAL;

	ad5141_desc = desc->extra;

	ret = ad5141_validate_chn(ad5141_desc, chn);
	if (ret)
		return ret;

	/* Software shutdown (command #15) */
	cmd.control = EXTRACT_CMD_BITS(AD51XX_CMD_ENTER_SHUTDOWN);
	cmd.address = ad5141_dpot_cmd_addr[chn];
	if (shutdown_enable)
		cmd.data    = EXTRACT_DATA_BITS(AD51XX_CMD_ENTER_SHUTDOWN);
	else
		cmd.data    = EXTRACT_DATA_BITS(AD51XX_CMD_ENTER_NORMAL_MODE);
	cmd.is_readback = false;

	ret = ad5141_dpot_send_cmd(desc, &cmd);
	if (ret)
		return ret;

	return 0;
}
/**
 * @brief Set the AD5141 digital potentiometer operating mode.
 * @param desc - digipot descriptor.
 * @param operating_mode - digipot operating mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5141_dpot_set_operating_mode(struct dpot_dev *desc,
				   enum dpot_operating_mode operating_mode)
{
	int ret;
	struct dpot_command cmd;
	struct ad5141_dpot_dev *ad5141_desc;
	uint8_t control_mode;

	if (!desc)
		return -EINVAL;

	ad5141_desc = desc->extra;

	/* Readback control register */
	cmd.control = EXTRACT_CMD_BITS(AD51XX_CMD_READ_CTL_REG);
	cmd.address = EXTRACT_ADS_BITS(AD51XX_CMD_READ_CTL_REG);
	cmd.data    = EXTRACT_DATA_BITS(AD51XX_CMD_READ_CTL_REG);
	cmd.is_readback = true;

	ret = ad5141_dpot_send_cmd(desc, &cmd);
	if (ret)
		return ret;

	control_mode = cmd.response;
	cmd.data = control_mode & OPERATING_MODE_BIT_MASK;
	/* Set operating mode (potentiometer/linear gain setting)
	 * (command #16) */
	cmd.control = EXTRACT_CMD_BITS(AD51XX_CMD_WRITE_SERIAL_REG_TO_CTL);
	cmd.address = EXTRACT_ADS_BITS(AD51XX_CMD_WRITE_SERIAL_REG_TO_CTL);;
	if (operating_mode == DPOT_LINEAR_GAIN_SETTING_MODE)
		cmd.data = control_mode | (1 << OPERATING_MODE_BIT_POSITION);
	cmd.is_readback = false;

	ret = ad5141_dpot_send_cmd(desc, &cmd);
	if (ret)
		return ret;

	ad5141_desc->operating_mode = operating_mode;

	return 0;
}

/**
 * @brief Read the AD5141 digital potentiometer input register.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param data  - input register data to be read.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5141_dpot_input_reg_read(struct dpot_dev *desc,
			       enum dpot_chn_type chn, uint8_t *data)
{
	int ret;
	struct dpot_command cmd;

	if (!desc || !data)
		return -EINVAL;

	ret = ad5141_validate_chn(desc->extra, chn);
	if (ret)
		return ret;

	/* Read back input register contents (command #3) */
	cmd.control = EXTRACT_CMD_BITS(AD51XX_CMD_READ_INP_REG);
	cmd.data    = EXTRACT_DATA_BITS(AD51XX_CMD_READ_INP_REG);
	cmd.address = ad5141_dpot_cmd_addr[chn];
	cmd.is_readback = true;

	ret = ad5141_dpot_send_cmd(desc, &cmd);
	if (ret)
		return ret;

	*data = cmd.response;

	return 0;
}

/**
 * @brief Write to the digital potentiometer input register.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param data - input register data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5141_dpot_input_reg_write(struct dpot_dev *desc,
				enum dpot_chn_type chn, uint8_t data)
{
	int ret;
	struct dpot_command cmd;

	if (!desc)
		return -EINVAL;

	ret = ad5141_validate_chn(desc->extra, chn);
	if (ret)
		return ret;

	/* Write contents of serial register data to
	 * input register (command #2) */
	cmd.control = EXTRACT_CMD_BITS(AD51XX_CMD_WRITE_INP_REG);
	cmd.address = ad5141_dpot_cmd_addr[chn];
	cmd.data = data;
	cmd.is_readback = false;

	ret = ad5141_dpot_send_cmd(desc, &cmd);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Copy input register to RDAC (software LRDAC).
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5141_dpot_sw_lrdac_update(struct dpot_dev *desc, enum dpot_chn_type chn)
{
	int ret;
	struct dpot_command cmd;

	if (!desc)
		return -EINVAL;

	ret = ad5141_validate_chn(desc->extra, chn);
	if (ret)
		return ret;

	/* Copy input register to RDAC (software LRDAC) (command #8) */
	cmd.control = EXTRACT_CMD_BITS(AD51XX_CMD_COPY_INP_REG_TO_RDAC);
	cmd.data    = EXTRACT_DATA_BITS(AD51XX_CMD_COPY_INP_REG_TO_RDAC);
	cmd.address = ad5141_dpot_cmd_addr[chn];
	cmd.is_readback = false;

	ret = ad5141_dpot_send_cmd(desc, &cmd);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Read the AD5141 digital potentiometer channel.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param data  - Channel data to be read.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5141_dpot_chn_read(struct dpot_dev *desc,
			 enum dpot_chn_type chn, uint8_t *data)
{
	int ret;
	struct dpot_command cmd;

	if (!desc || !data)
		return -EINVAL;

	ret = ad5141_validate_chn(desc->extra, chn);
	if (ret)
		return ret;

	/* Read back RDAC contents (command #3) */
	cmd.control = EXTRACT_CMD_BITS(AD51XX_CMD_READ_RDAC_REG);
	cmd.data    = EXTRACT_DATA_BITS(AD51XX_CMD_READ_RDAC_REG);
	cmd.address = ad5141_dpot_cmd_addr[chn];
	cmd.is_readback = true;

	ret = ad5141_dpot_send_cmd(desc, &cmd);
	if (ret)
		return ret;

	*data = cmd.response;

	return 0;
}

/**
 * @brief Enter/Exit the top scale or bottom scale .
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param is_top_scale  - boolean flag to indicate top scale or bottom scale.
 * @param enable - Enter or exit the TS.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5141_dpot_top_bottom_scale_enable(struct dpot_dev *desc,
					enum dpot_chn_type chn,
					bool is_top_scale,
					uint8_t enable)
{
	int ret;
	struct dpot_command cmd;

	if (!desc)
		return -EINVAL;

	ret = ad5141_validate_chn(desc->extra, chn);
	if (ret)
		return ret;

	/* Enter/Exit the top scale or bottom scale (command #12 or command #13) */
	cmd.control = EXTRACT_CMD_BITS(AD51XX_CMD_TOP_SCALE_EXIT);
	cmd.address = ad5141_dpot_cmd_addr[chn];
	cmd.data =  enable;
	if (is_top_scale == true)
		cmd.data |= EXTRACT_DATA_BITS(AD51XX_CMD_TOP_SCALE_EXIT);

	cmd.is_readback = false;

	return ad5141_dpot_send_cmd(desc, &cmd);
}

/**
 * @brief Write to the digital potentiometer channel.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param data - Channel data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5141_dpot_chn_write(struct dpot_dev *desc,
			  enum dpot_chn_type chn, uint8_t data)
{
	int ret;
	struct dpot_command cmd;

	if (!desc)
		return -EINVAL;

	ret = ad5141_validate_chn(desc->extra, chn);
	if (ret)
		return ret;

	/* Write contents of serial register data to RDAC (command #1) */
	cmd.control = EXTRACT_CMD_BITS(AD51XX_CMD_WRITE_RDAC_REG);
	cmd.address = ad5141_dpot_cmd_addr[chn];
	cmd.data = data;
	cmd.is_readback = false;

	ret = ad5141_dpot_send_cmd(desc, &cmd);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Read the NVM (non-volatile) memory data.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param data  - NVM data to be read.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5141_dpot_nvm_read(struct dpot_dev *desc,
			 enum dpot_chn_type chn, uint8_t *data)
{
	int ret;
	struct dpot_command cmd;

	if (!desc || !data)
		return -EINVAL;

	ret = ad5141_validate_chn(desc->extra, chn);
	if (ret)
		return ret;

	/* Read back EEPROM contents (command #3) */
	cmd.control = EXTRACT_CMD_BITS(AD51XX_CMD_READ_EEPROM);
	cmd.data    = EXTRACT_DATA_BITS(AD51XX_CMD_READ_EEPROM);
	cmd.address = ad5141_dpot_cmd_nvm_addr[chn];
	cmd.is_readback = true;

	ret = ad5141_dpot_send_cmd(desc, &cmd);
	if (ret)
		return ret;

	*data = cmd.response;

	return 0;
}

/**
 * @brief Write data to NVM (non-volatile) memory.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param data  - NVM data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5141_dpot_nvm_write(struct dpot_dev *desc,
			  enum dpot_chn_type chn, uint8_t data)
{
	int ret;
	struct dpot_command cmd;

	if (!desc)
		return -EINVAL;

	ret = ad5141_validate_chn(desc->extra, chn);
	if (ret)
		return ret;

	/* Write contents of serial register data to EEPROM (command #11) */
	cmd.control = EXTRACT_CMD_BITS(AD51XX_CMD_WRITE_TO_EEPROM);
	cmd.address = ad5141_dpot_cmd_nvm_addr[chn];
	cmd.data = data;
	cmd.is_readback = false;

	ret = ad5141_dpot_send_cmd(desc, &cmd);
	if (ret)
		return ret;

	/* EEPROM write delay as per device specifications */
	no_os_mdelay(AD5141_NVM_WRITE_DELAY);

	return 0;
}

/**
 * @brief Copy RDAC register to NVM/EEPROM.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5141_dpot_copy_rdac_to_nvm(struct dpot_dev *desc, enum dpot_chn_type chn)
{
	int ret;
	struct dpot_command cmd;

	if (!desc)
		return -EINVAL;

	ret = ad5141_validate_chn(desc->extra, chn);
	if (ret)
		return ret;

	/* Copy RDAC register to EEPROM (command #9) */
	cmd.control = EXTRACT_CMD_BITS(AD51XX_CMD_COPY_RDAC_TO_EEPROM);
	cmd.data    = EXTRACT_DATA_BITS(AD51XX_CMD_COPY_RDAC_TO_EEPROM);
	cmd.address = ad5141_dpot_cmd_addr[chn];
	cmd.is_readback = false;

	ret = ad5141_dpot_send_cmd(desc, &cmd);
	if (ret)
		return ret;

	/* EEPROM write delay as per device specifications */
	no_os_mdelay(18);

	return 0;
}

/**
 * @brief Copy NVM/EEPROM data to RDAC.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5141_dpot_copy_nvm_to_rdac(struct dpot_dev *desc, enum dpot_chn_type chn)
{
	int ret;
	struct dpot_command cmd;

	if (!desc)
		return -EINVAL;

	ret = ad5141_validate_chn(desc->extra, chn);
	if (ret)
		return ret;

	/* Copy EEPROM into RDAC (command #10) */
	cmd.control = EXTRACT_CMD_BITS(AD51XX_CMD_COPY_EEPROM_TO_RDAC);
	cmd.data    = EXTRACT_DATA_BITS(AD51XX_CMD_COPY_EEPROM_TO_RDAC);
	cmd.address = ad5141_dpot_cmd_addr[chn];
	cmd.is_readback = false;

	ret = ad5141_dpot_send_cmd(desc, &cmd);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Perform RDAC linear increment/decrement.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param status - linear increment/decrement status.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5141_dpot_rdac_linear_update(struct dpot_dev *desc,
				   enum dpot_chn_type chn,
				   enum dpot_rdac_linear_status status)
{
	int ret;
	struct dpot_command cmd;

	if (!desc)
		return -EINVAL;

	ret = ad5141_validate_chn(desc->extra, chn);
	if (ret)
		return ret;

	/* RDAC linear increment/decrement (command #4 and #5) */
	cmd.data = EXTRACT_DATA_BITS(AD51XX_CMD_LRDAC_DEC);
	cmd.control = EXTRACT_CMD_BITS(AD51XX_CMD_LRDAC_INC);
	cmd.address = ad5141_dpot_cmd_addr[chn];
	if (status == DPOT_RDAC_LINEAR_INCREMENT)
		cmd.data = EXTRACT_DATA_BITS(AD51XX_CMD_LRDAC_INC);;
	cmd.is_readback = false;

	ret = ad5141_dpot_send_cmd(desc, &cmd);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Perform RDAC 6dB increment/decrement.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param status - 6dB increment/decrement status.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5141_dpot_rdac_6db_update(struct dpot_dev *desc, enum dpot_chn_type chn,
				enum dpot_rdac_6db_status status)
{
	int ret;
	struct dpot_command cmd;

	if (!desc)
		return -EINVAL;

	ret = ad5141_validate_chn(desc->extra, chn);
	if (ret)
		return ret;

	/* RDAC 6dB increment/decrement (command #6 and #7) */
	cmd.data = EXTRACT_DATA_BITS(AD51XX_CMD_LRDAC_6DB_DEC);
	cmd.control = EXTRACT_CMD_BITS(AD51XX_CMD_LRDAC_6DB_INC);
	cmd.address = ad5141_dpot_cmd_addr[chn];
	if (status == DPOT_RDAC_6DB_INCREMENT)
		cmd.data = EXTRACT_DATA_BITS(AD51XX_CMD_LRDAC_6DB_INC);
	cmd.is_readback = false;

	ret = ad5141_dpot_send_cmd(desc, &cmd);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Send command word to the AD5141 digital potentiometer.
 * @param desc - digipot descriptor.
 * @param cmd  - digipot command word
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5141_dpot_send_cmd(struct dpot_dev *desc,
			 struct dpot_command *cmd)
{
	int ret;
	uint8_t buf[2];
	struct ad5141_dpot_dev *ad5141_dev;

	if (!desc || !cmd)
		return -EINVAL;

	ad5141_dev = desc->extra;

	/* Form a command word */
	buf[0] = ((cmd->control & 0x0f) << 4) | (cmd->address & 0x0f);
	buf[1] = cmd->data;

	/* Send command word based on the interface type selection */
	if (ad5141_dev->intf_type == AD_SPI_INTERFACE) {
		ret = no_os_spi_write_and_read(ad5141_dev->spi_desc, buf, sizeof(buf));
		if (ret)
			return ret;
	} else if (ad5141_dev->intf_type == AD_I2C_INTERFACE) {
		ret = no_os_i2c_write(ad5141_dev->i2c_desc, buf, sizeof(buf), 1);
		if (ret)
			return ret;
	} else {
		return -EINVAL;
	}

	/* Read response (only for readback commands) */
	if (cmd->is_readback) {
		/* Form NOP command word to readback contents */
		buf[0] = 0x0;
		buf[1] = 0x0;

		/* Send command word based on the interface type selection */
		if (ad5141_dev->intf_type == AD_SPI_INTERFACE) {
			ret = no_os_spi_write_and_read(ad5141_dev->spi_desc, buf, sizeof(buf));
			if (ret)
				return ret;
		} else if (ad5141_dev->intf_type == AD_I2C_INTERFACE) {
			ret = no_os_i2c_read(ad5141_dev->i2c_desc, &buf[1], 1, 1);
			if (ret)
				return ret;
		} else {
			return -EINVAL;
		}

		cmd->response = buf[1];
	}

	return 0;
}

/* AD5141 digital potentiometer ops structure */
const struct dpot_ops ad5141_dpot_ops = {
	.dpot_init = &ad5141_dpot_init,
	.dpot_reset = &ad5141_dpot_reset,
	.dpot_shutdown = &ad5141_dpot_shutdown,
	.dpot_set_operating_mode = &ad5141_dpot_set_operating_mode,
	.dpot_input_reg_read = &ad5141_dpot_input_reg_read,
	.dpot_input_reg_write = &ad5141_dpot_input_reg_write,
	.dpot_sw_lrdac_update = &ad5141_dpot_sw_lrdac_update,
	.dpot_chn_read = &ad5141_dpot_chn_read,
	.dpot_chn_write = &ad5141_dpot_chn_write,
	.dpot_nvm_read = &ad5141_dpot_nvm_read,
	.dpot_nvm_write = &ad5141_dpot_nvm_write,
	.dpot_copy_rdac_to_nvm = &ad5141_dpot_copy_rdac_to_nvm,
	.dpot_copy_nvm_to_rdac = &ad5141_dpot_copy_nvm_to_rdac,
	.dpot_rdac_linear_update = &ad5141_dpot_rdac_linear_update,
	.dpot_rdac_6db_update = &ad5141_dpot_rdac_6db_update,
//	.dpot_send_cmd = &ad5141_dpot_send_cmd,
	.dpot_remove = &ad5141_dpot_remove,
	.dpot_enable_top_bottom_scale = &ad5141_dpot_top_bottom_scale_enable
};
