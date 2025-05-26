/***************************************************************************//**
 *   @file   ad5242.c
 *   @brief  Source file for the ad5242 digital potentiometer drivers
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
#include "ad5242.h"
#include "no_os_delay.h"

/* Contains the address that need to be programmed while configuring the two different channels of AD5242*/
static uint8_t ad5242_dpot_cmd_addr[] = {
	[DPOT_CHN_RDAC1] = 0x00,
	[DPOT_CHN_RDAC2] = 0x80

};

int ad5242_dpot_send_cmd_write(struct dpot_dev *desc, struct dpot_command *cmd);
static uint8_t nRdac_value = 0x80;
/* AD5144 dpot command address mapped to channel type */
/**
 * @brief Initialize the AD5144 digipot GPIOs.
 * @param param - AD5144 digipot init parameters.
 * @param dev - AD5144 digipot descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad5242_dpot_gpio_init(struct ad5242_dpot_init_param *param,
				 struct ad5242_dpot_dev *dev)
{
	int ret;
	if (!param || !dev)
		return -EINVAL;

	/* DIS GPIO */
	ret = no_os_gpio_get_optional(&dev->shdn_gpio_desc, param->shdn_gpio_init);
	if (ret)
		return ret;
	return 0;
}

/**
 * @brief Free the memory allocated by ad5144_dpot_gpio_init().
 * @param dev - AD5144 digipot descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad5242_dpot_gpio_remove(struct ad5242_dpot_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	if (dev->shdn_gpio_desc) {
		ret = no_os_gpio_remove(dev->shdn_gpio_desc);
		if (ret)
			return ret;
	}

	return 0;
}
/**
 * @brief Initialize the ad5242 digital potentiometer.
 * @param param - digipot init parameters.
 * @param desc - digipot descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5242_dpot_init(struct dpot_init_param *param, struct dpot_dev **desc)
{
	int ret = -EINVAL;
	struct dpot_dev *dev = NULL;
	struct ad5242_dpot_init_param *ad5242_params;
	struct ad5242_dpot_dev *ad5242_dev;
	struct no_os_i2c_init_param *i2c_init;
	if (!param)
		return -EINVAL;

	/* Allocate memory for digipot device descriptor */
	dev = (struct dpot_dev *)calloc(1, sizeof(struct dpot_dev));
	if (!dev)
		return -ENOMEM;

	/* Allocate memory for ad5242 digipot device descriptor */
	ad5242_dev = (struct ad5242_dpot_dev *)calloc(1, sizeof(*ad5242_dev));
	if (!ad5242_dev) {
		ret = -ENOMEM;
		goto err_ad5242_dev;
	}

	ad5242_params = param->extra;
	ad5242_params->eIntfType = param->intf_type;
	dev->extra = ad5242_dev;
	/* Initialize GPIOs */
	ret = ad5242_dpot_gpio_init(ad5242_params, ad5242_dev);
	if (ret)
		goto err_dpot_gpio;

	/* Initialize the digital interface */
	if (param->intf_type == AD_SPI_INTERFACE) {
		goto err_dpot_gpio;

	} else if (param->intf_type == AD_I2C_INTERFACE) {
		i2c_init = ((struct ad5242_dpot_init_param *)param->extra)->i2c_init;
		ret = no_os_i2c_init(&ad5242_dev->i2c_desc, i2c_init);
		if (ret)
			goto err_dpot_init;
	} else {
		/* Interface not supported */
		ret = -EINVAL;
		goto err_dpot_gpio;
	}

	ad5242_dev->intf_type = param->intf_type;

	*desc = dev;

	return 0;

err_dpot_init:
	if (ad5242_dev->i2c_desc)
		no_os_i2c_remove(ad5242_dev->i2c_desc);
err_dpot_gpio:
	free(ad5242_dev);
err_ad5242_dev:
	free(dev);

	return ret;
}

/**
 * @brief Free the memory allocated by ad5242_dpot_init().
 * @param desc - digipot descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5242_dpot_remove(struct dpot_dev *desc)
{
	int ret;
	struct ad5242_dpot_dev *ad5242_dev;

	if (!desc)
		return -EINVAL;

	if (!desc->extra)
		return -EINVAL;

	ad5242_dev = desc->extra;
	ret = ad5242_dpot_gpio_remove(ad5242_dev);
	if (ret)
		return ret;
	if (ad5242_dev->intf_type == AD_I2C_INTERFACE) {
		ret = no_os_i2c_remove((struct no_os_i2c_desc *)ad5242_dev->i2c_desc);
		if (ret)
			return ret;
	}
	free(ad5242_dev);
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
int ad5242_dpot_chn_write(struct dpot_dev *desc,
			  enum dpot_chn_type chn,
			  uint8_t data)
{
	int ret;
	struct dpot_command cmd;

	if (!desc)
		return -EINVAL;

	if (!desc->extra)
		return -EINVAL;

	/* Write contents of serial register data to RDAC (command #1) */
	cmd.control =  ad5242_dpot_cmd_addr[chn];
	cmd.data = data;
	cmd.is_readback = true;

	ret = ad5242_dpot_send_cmd_write(desc, &cmd);
	if (ret)
		return ret;
	nRdac_value = data;
	return 0;
}
/**
 * @brief Read the ad5242 digital potentiometer channel.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param data  - Channel data to be read.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5242_dpot_chn_read(struct dpot_dev *desc,
			 enum dpot_chn_type chn, uint8_t *data)
{
	int ret;
	struct dpot_command cmd;
	if (!desc)
		return -EINVAL;

	if (!desc->extra)
		return -EINVAL;

	if (chn > DPOT_CHN_RDAC2)
		return -EINVAL;

	/* Use the bit "0" (which is don't care) to indicate no data to be written.  */
	cmd.control = 0x00 | ad5242_dpot_cmd_addr[chn] | AD5242_READ_ENABLE;
	cmd.data = 0x00;
	cmd.is_readback = true;
	ret = ad5242_dpot_send_cmd_write(desc, &cmd);
	if (ret)
		return ret;
	*data = cmd.response;
	return 0;
}
/**
 * @brief Shutdown the AD5144 digital potentiometer channel.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param shutdown_enable - Shutdown enable status flag.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5242_dpot_shutdown(struct dpot_dev *desc,
			 enum dpot_chn_type chn,
			 bool shutdown_enable)
{
	int ret;
	struct dpot_command cmd;

	if (!desc)
		return -EINVAL;

	if (!desc->extra)
		return -EINVAL;

	if (chn > DPOT_CHN_RDAC2)
		return -EINVAL;


	/* Software shutdown (command #20) */
	cmd.address = 0x00;
	if (shutdown_enable) {
		cmd.control = AD5242_SHUT_DOWN_ENABLE | ad5242_dpot_cmd_addr[chn] | 0x01;
		cmd.data = 0x00;
	} else {
		/* Software shutdown (command #40) */
		cmd.control = ad5242_dpot_cmd_addr[chn] | 0x01;
		cmd.data = 0x00;
	}
	cmd.is_readback = true;
	ret = ad5242_dpot_send_cmd_write(desc, &cmd);
	if (ret)
		return ret;
	return 0;
}
/**
 * @brief Shutdown the AD5144 digital potentiometer channel.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param bMidScaleEnable - boolean variable to indiate  .
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5242_dpot_set_mid_scale(struct dpot_dev *desc,
			      enum dpot_chn_type chn,
			      bool bMidScaleEnable)
{
	int ret = -EINVAL;
	struct dpot_command cmd;
	if (!desc)
		return -EINVAL;

	if (!desc->extra)
		return -EINVAL;

	if (chn > DPOT_CHN_RDAC2)
		return -EINVAL;

	/* Software shutdown (command #20) */
	cmd.data = 0x00;
	if (bMidScaleEnable == true) {
		cmd.control = AD5242_CTL_FOR_MID_SCALE_POSITION | ad5242_dpot_cmd_addr[chn] |
			      0x01; ;
		cmd.is_readback = false;
		ret = ad5242_dpot_send_cmd_write(desc, &cmd);
		if (ret)
			return ret;

	}

	return ret;
}
/**
 * @brief Send command word to the ad5242 digital potentiometer.
 * @param desc - digipot descriptor.
 * @param cmd  - digipot command word
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5242_dpot_send_cmd_write(struct dpot_dev *desc, struct dpot_command *cmd)
{
	int ret = -EINVAL;
	uint8_t buf[2];
	uint8_t nSize = 2;

	struct ad5242_dpot_dev *ad5242_dev;
	if (!desc)
		return -EINVAL;

	if (!desc->extra)
		return -EINVAL;

	if (!cmd)
		return -EINVAL;

	ad5242_dev = desc->extra;
	if ((cmd->control & 0x01) != 0) {
		nSize = 1;
	}
	/* Form a command word */
	buf[0] = cmd->control;
	buf[1] = cmd->data;
	/* Send command word based on the interface type selection */

	if (ad5242_dev->intf_type == AD_SPI_INTERFACE) {
		return ret;
	} else if (ad5242_dev->intf_type == AD_I2C_INTERFACE) {
		ret = no_os_i2c_write(ad5242_dev->i2c_desc, buf, nSize, 1);
		if (ret)
			return ret;
		/* Send command word based on the interface type selection */
		/* Read response (only for readback commands) */
		if (cmd->is_readback) {
			/* Form NOP command word to readback contents */
			buf[0] = 0x0;
			ret = no_os_i2c_read(ad5242_dev->i2c_desc, &buf[0], 1, 1);
			if (ret)
				return ret;
			cmd->response = buf[0];
		}
	} else {
		return -EINVAL;
	}

	return ret;
}

/* ad5242 digital potentiometer ops structure */
const struct dpot_ops ad5242_dpot_ops = {
	.dpot_init = &ad5242_dpot_init,
	.dpot_reset = NULL,
	.dpot_shutdown = &ad5242_dpot_shutdown,
	.dpot_set_operating_mode = NULL,
	.dpot_input_reg_read = NULL,
	.dpot_input_reg_write = NULL,
	.dpot_sw_lrdac_update = NULL,
	.dpot_chn_read = &ad5242_dpot_chn_read,
	.dpot_chn_write = &ad5242_dpot_chn_write,
	.dpot_nvm_read = NULL,
	.dpot_nvm_write = NULL,
	.dpot_copy_rdac_to_nvm = NULL,
	.dpot_copy_nvm_to_rdac = NULL,
	.dpot_rdac_linear_update = NULL,
	.dpot_rdac_6db_update = NULL,
	//.dpot_send_cmd = &ad5242_dpot_send_cmd,
	.dpot_remove = &ad5242_dpot_remove,
	.dpot_enable_top_bottom_scale = NULL,
	.dpot_tolerance_read = NULL,
	.dpot_set_mid_scale = &ad5242_dpot_set_mid_scale
};
