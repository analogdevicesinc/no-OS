/***************************************************************************//**
 *   @file   ad5161.c
 *   @brief  Source file for the ad5161 digital potentiometer drivers
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include "ad5161.h"
#include "no_os_delay.h"
uint8_t ad5161_dpot_cmd_addr[] = {
	[DPOT_CHN_RDAC1] = 0x0,

};
static int ad5161_dpot_send_cmd(struct dpot_dev* desc,
				struct dpot_command* cmd);
static int ad5161_dpot_send_cmd_write(struct dpot_dev *desc,
				      struct dpot_command *cmd);

static uint8_t nRdac_value = 0x80;
/* AD5144 dpot command address mapped to channel type */
/**
 * @brief Initialize the AD5144 digipot GPIOs.
 * @param param - AD5144 digipot init parameters.
 * @param dev - AD5144 digipot descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad5161_dpot_gpio_init(struct ad516x_dpot_init_param *param,
				 struct ad5161_dpot_dev *dev)
{
	int ret;
	uint8_t gpio_state;

	if (!param || !dev)
		return -EINVAL;

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


	return 0;
}

/**
 * @brief Free the memory allocated by ad5144_dpot_gpio_init().
 * @param dev - AD5144 digipot descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad5161_dpot_gpio_remove(struct ad5161_dpot_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	if (dev->dis_gpio_desc) {
		ret = no_os_gpio_remove(dev->dis_gpio_desc);
		if (ret)
			return ret;
	}

	return 0;
}
/**
 * @brief Initialize the ad5161 digital potentiometer.
 * @param param - digipot init parameters.
 * @param desc - digipot descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5161_dpot_init(struct dpot_init_param *param, struct dpot_dev **desc)
{
	int ret=-EINVAL;
	struct dpot_dev *dev=NULL;
	struct ad516x_dpot_init_param *ad5161_params;
	struct ad5161_dpot_dev *ad5161_dev;
	struct no_os_i2c_init_param *i2c_init;
	struct no_os_spi_init_param *spi_init;
	if (!param)
		return -EINVAL;

	/* Allocate memory for digipot device descriptor */
	dev = (struct dpot_dev *)calloc(1, sizeof(struct dpot_dev));
	if (!dev)
		return -ENOMEM;

	/* Allocate memory for ad5161 digipot device descriptor */
	ad5161_dev = (struct ad5161_dpot_dev *)calloc(1, sizeof(*ad5161_dev));
	if (!ad5161_dev) {
		ret = -ENOMEM;
		goto err_ad5161_dev;
	}

	ad5161_params = param->extra;
	ad5161_params->eintf_type = param->intf_type;
	dev->extra = ad5161_dev;
	/* Initialize GPIOs */
	ret = ad5161_dpot_gpio_init(ad5161_params, ad5161_dev);
	if (ret)
		goto err_dpot_gpio;

	/* Initialize the digital interface */
	if (param->intf_type == AD_SPI_INTERFACE) {
		spi_init = ((struct ad516x_dpot_init_param *)param->extra)->spi_init;
		ret = no_os_spi_init(&ad5161_dev->spi_desc, spi_init);
		if (ret)
			goto err_intf_init;

	} else if (param->intf_type == AD_I2C_INTERFACE) {
		i2c_init = ((struct ad516x_dpot_init_param *)param->extra)->i2c_init;
		ret = no_os_i2c_init(&ad5161_dev->i2c_desc, i2c_init);
		if (ret)
			goto err_intf_init;
	} else {
		/* Interface not supported */
		ret = -EINVAL;
		goto err_intf_init;
	}

	ad5161_dev->intf_type = param->intf_type;

	*desc = dev;

	return 0;

err_intf_init:
	if (ad5161_dev->i2c_desc)
		no_os_i2c_remove(ad5161_dev->i2c_desc);
	if (ad5161_dev->spi_desc)
		no_os_spi_remove(ad5161_dev->spi_desc);

	ad5161_dpot_gpio_remove(ad5161_dev);

err_dpot_gpio:
	free(ad5161_dev);
err_ad5161_dev:
	free(dev);

	return ret;
}

/**
 * @brief Free the memory allocated by ad5161_dpot_init().
 * @param desc - digipot descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5161_dpot_remove(struct dpot_dev *desc)
{
	int ret;
	struct ad5161_dpot_dev *ad5161_dev;

	if (!desc || !desc->extra)
		return -EINVAL;

	ad5161_dev = desc->extra;
	ret = ad5161_dpot_gpio_remove(ad5161_dev);
	if (ret)
		return ret;
	if (ad5161_dev->intf_type == AD_I2C_INTERFACE) {
		ret = no_os_i2c_remove((struct no_os_i2c_desc *)ad5161_dev->i2c_desc);
		if (ret)
			return ret;
	}

	if (ad5161_dev->intf_type == AD_SPI_INTERFACE) {
		ret = no_os_spi_remove((struct no_os_spi_desc *)ad5161_dev->spi_desc);
		if (ret)
			return ret;
	}

	free(ad5161_dev);
	free(desc);

	return 0;
}
/**
 * @brief Write to the digital potentiometer channel.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param data - Channel data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5161_dpot_chn_write(struct dpot_dev *desc,
			  enum dpot_chn_type chn,
			  uint8_t data)
{
	int ret;
	struct dpot_command cmd;

	if (!desc)
		return -EINVAL;

	/* Write contents of serial register data to RDAC (command #1) */
	cmd.control = AD5161_CTL_FOR_READ_WRITE;
	cmd.address = 0x00;
	cmd.data = data;
	cmd.is_readback = true;

	ret = ad5161_dpot_send_cmd_write(desc, &cmd);
	if (ret)
		return ret;
	nRdac_value = data;
	return 0;
}
/**
 * @brief Read the ad5161 digital potentiometer channel.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param data  - Channel data to be read.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5161_dpot_chn_read(struct dpot_dev *desc,
			 enum dpot_chn_type chn, uint8_t *data)
{
	int ret;
	struct dpot_command cmd;
	struct ad5161_dpot_dev *ad5161_dev;
	ad5161_dev = desc->extra;
	if (!desc || chn > DPOT_CHN_RDAC1 )
		return -EINVAL;

	if (ad5161_dev->intf_type == AD_I2C_INTERFACE) {
		/* Read back RDAC contents (command #3) */
		cmd.control = AD5161_CTL_FOR_READ_WRITE;
		cmd.data = 0x00;
		cmd.is_readback = true;

		ret = ad5161_dpot_send_cmd(desc, &cmd);
		if (ret)
			return ret;
	} else {
		cmd.response = nRdac_value;
	}
	*data = cmd.response;

	return 0;
}
/**
 * @brief Shutdown the AD5161 digital potentiometer channel.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param shutdown_enable - Shutdown enable status flag.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5161_dpot_shutdown(struct dpot_dev *desc,
			 enum dpot_chn_type chn,
			 bool shutdown_enable)
{
	int ret;
	struct dpot_command cmd;

	if (!desc)
		return -EINVAL;

	if (!desc || chn > DPOT_CHN_RDAC1)
		return -EINVAL;

	/* Software shutdown (command #20) */
	cmd.address = 0x00;
	cmd.data = 0x00;
	if (shutdown_enable) {
		cmd.control = AD5161_CTL_FOR_EN_SHUTWOWN;
	} else {
		/* Software shutdown (command #80) */
		cmd.control = AD5161_CTL_FOR_DIS_SHUTWOWN;
	}
	cmd.is_readback = true;
	ret = ad5161_dpot_send_cmd_write(desc, &cmd);
	if (ret)
		return ret;
	return 0;
}
/**
 * @brief To set the  the AD5161 to mid scale. 
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param midscale_en  - To enable the set the device to mid scale.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5161_dpot_set_mid_scale(struct dpot_dev *desc,
			      enum dpot_chn_type chn,
			      bool midscale_en )
{
	int ret= -EINVAL;
	struct dpot_command cmd;
	struct ad5161_dpot_dev *ad5161_desc;

	if (!desc)
		return -EINVAL;

	ad5161_desc = desc->extra;

	if (!desc || chn > DPOT_CHN_RDAC1)
		return -EINVAL;


	/* Software shutdown (command #20) */
	cmd.address = 0x00;
	cmd.data = 0x00;
	if (midscale_en  == true) {
		cmd.control = AD5161_CTL_FOR_RESET_POSITION;
		cmd.is_readback = false;
		ret = ad5161_dpot_send_cmd_write(desc, &cmd);

		if (ret)
			return ret;
		if (ad5161_desc->intf_type == AD_SPI_INTERFACE) {
			nRdac_value = 0x80;
		}
	}
	return ret;
}
/**
 * @brief Send command word to read from ad5161 digital potentiometer.
 *  Reading is  possible only if interface is I2C.
 * @param desc - digipot descriptor.
 * @param cmd  - digipot command word
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5161_dpot_send_cmd(struct dpot_dev *desc,
			 struct dpot_command *cmd)
{
	int ret;
	uint8_t buf[2];
	struct ad5161_dpot_dev *ad5161_dev;
	if (!desc || !cmd)
		return -EINVAL;

	ad5161_dev = desc->extra;

	/* Form a command word */
	buf[0] = cmd->control;
	buf[1] = 0x00;
	/* Form NOP command word to readback contents */
	buf[0] = 0x0;
	ret = no_os_i2c_read(ad5161_dev->i2c_desc,&buf[0], 1, 1);
	if (ret){
		return ret;
	}
	cmd->response = buf[0];
	return 0;
}
/**
 * @brief Send command for writing to ad5161 digital potentiometer.
 * @param desc - digipot descriptor.
 * @param cmd  - digipot command word
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5161_dpot_send_cmd_write(struct dpot_dev *desc,
			       struct dpot_command *cmd)
{
	int ret;
	uint8_t buf[2];
	uint8_t nSize = 2;

	struct ad5161_dpot_dev *ad5161_dev;
	if (!desc || !cmd)
		return -EINVAL;

	ad5161_dev = desc->extra;
	if (cmd->control != 0) {
		nSize = 1;
	}
	/* Form a command word */
	buf[0] = cmd->control;
	buf[1] = cmd->data;
	/* Send command word based on the interface type selection */

	if (ad5161_dev->intf_type == AD_SPI_INTERFACE) {
		ret = no_os_spi_write_and_read(ad5161_dev->spi_desc, &buf[0], nSize);
		if (ret)
			return ret;
	} else if (ad5161_dev->intf_type == AD_I2C_INTERFACE) {
		ret = no_os_i2c_write(ad5161_dev->i2c_desc, buf, nSize, 1);
		/* Send command word based on the interface type selection */
		/* Read response (only for readback commands) */
		if (cmd->is_readback) {
			/* Form NOP command word to readback contents */
			buf[0] = 0x0;
			ret = no_os_i2c_read(ad5161_dev->i2c_desc, &buf[0], 1, 1);
			if (ret)
				return ret;
			cmd->response = buf[0];
		}
	} else {
		return -EINVAL;
	}

	return 0;
}

/* ad5161 digital potentiometer ops structure */
const struct dpot_ops ad5161_dpot_ops = {
	.dpot_init = &ad5161_dpot_init,
	.dpot_reset = NULL,
	.dpot_shutdown = &ad5161_dpot_shutdown,
	.dpot_set_operating_mode = NULL,
	.dpot_input_reg_read = NULL,
	.dpot_input_reg_write = NULL,
	.dpot_sw_lrdac_update = NULL,
	.dpot_chn_read = &ad5161_dpot_chn_read,
	.dpot_chn_write = &ad5161_dpot_chn_write,
	.dpot_nvm_read = NULL,
	.dpot_nvm_write = NULL,
	.dpot_copy_rdac_to_nvm = NULL,
	.dpot_copy_nvm_to_rdac = NULL,
	.dpot_rdac_linear_update = NULL,
	.dpot_rdac_6db_update = NULL,
	.dpot_send_cmd = NULL,
	.dpot_remove = &ad5161_dpot_remove,
	.dpot_enable_top_bottom_scale = NULL,
	.dpot_tolerance_read = NULL,
	.dpot_set_mid_scale = &ad5161_dpot_set_mid_scale
};
