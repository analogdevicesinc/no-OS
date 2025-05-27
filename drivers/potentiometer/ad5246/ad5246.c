/***************************************************************************//**
 *   @file   ad5246.c
 *   @brief  Source file for the ad5246 digital potentiometer drivers
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
#include "ad5246.h"
#include "no_os_delay.h"

/* AD5246 dpot command address mapped to channel type */
/**
 * @struct dpot_command.
 * @brief Digital potentiometer command structure.
 */
struct dpot_command_5246 {
	/* Control bits (DB15:DB12) */
	uint8_t nInstruction;
	/* Address bits (DB11:DB8) */
	uint8_t address;
	/* Data bits (DB7:DB0) */
	uint8_t data;
	/* Readback needed (optional response to command) */
	bool is_readback;
	/* Response (optional, if 'is_readback' is set true) */
	uint8_t response;
};
/**
 * @brief Send command word to the ad5246 digital potentiometer.
 * @param desc - digipot descriptor.
 * @param cmd  - digipot command word
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad5246_dpot_send_cmd(struct dpot_dev *desc,
				struct dpot_command *cmd)
{
	int ret;
	uint8_t buf[2];
	struct ad5246_dpot_dev *ad5246_dev;
	if (!desc || !cmd)
		return -EINVAL;

	ad5246_dev = desc->extra;

	/* Form a command word */
	buf[0] = cmd->data;
	/* Send command word based on the interface type selection */
	if (cmd->is_readback == false) {
		ret = no_os_i2c_write(ad5246_dev->i2c_desc, buf, 1, 1);
		if (ret)
			return ret;
	}
	/* Read response (only for readback commands) */
	else  {
		/* Form NOP command word to readback contents */
		buf[0] = 0x0;
		ret = no_os_i2c_read(ad5246_dev->i2c_desc, &buf[0], 1, 1);
		if (ret)
			return ret;
		cmd->response = buf[0];
	}
	return 0;
}
/**
 * @brief Initialize the ad5246 digital potentiometer.
 * @param param - digipot init parameters.
 * @param desc - digipot descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5246_dpot_init(struct dpot_init_param *param, struct dpot_dev **desc)
{
	int ret = -EINVAL;
	struct dpot_dev *dev = NULL;
	struct ad5246_dpot_init_param *ad5246_params;
	struct ad5246_dpot_dev *ad5246_dev;
	struct no_os_i2c_init_param *i2c_init;

	if (!param)
		return -EINVAL;

	/* Allocate memory for digipot device descriptor */
	dev = (struct dpot_dev *)calloc(1, sizeof(struct dpot_dev));
	if (!dev)
		return -ENOMEM;

	/* Allocate memory for ad5246 digipot device descriptor */
	ad5246_dev = (struct ad5246_dpot_dev *)calloc(1, sizeof(*ad5246_dev));
	if (!ad5246_dev) {
		ret = -ENOMEM;
		goto err_ad5246_dev;
	}

	ad5246_params = param->extra;
	ad5246_params->eIntfType = param->intf_type;
	dev->extra = ad5246_dev;

	/* Initialize the digital interface */
	if (param->intf_type == AD_SPI_INTERFACE) {
		goto err_intf_init;
	} else if (param->intf_type == AD_I2C_INTERFACE) {
		i2c_init = ((struct ad5246_dpot_init_param *)param->extra)->i2c_init;
		ret = no_os_i2c_init(&ad5246_dev->i2c_desc, i2c_init);
		if (ret)
			goto err_intf_init;
	} else {
		/* Interface not supported */
		ret = -EINVAL;
		goto err_dpot_gpio;
	}

	ad5246_dev->intf_type = param->intf_type;

	*desc = dev;

	return 0;

err_intf_init:
	if (ad5246_dev->i2c_desc)
		no_os_i2c_remove(ad5246_dev->i2c_desc);
err_dpot_gpio:
	free(ad5246_dev);
err_ad5246_dev:
	free(dev);

	return ret;
}

/**
 * @brief Free the memory allocated by ad5246_dpot_init().
 * @param desc - digipot descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5246_dpot_remove(struct dpot_dev *desc)
{
	int ret;
	struct ad5246_dpot_dev *ad5246_dev;

	if (!desc || !desc->extra)
		return -EINVAL;

	ad5246_dev = desc->extra;

	ret = no_os_i2c_remove((struct no_os_i2c_desc *)ad5246_dev->i2c_desc);
	if (ret)
		return ret;

	free(ad5246_dev);
	free(desc);

	return 0;
}

/**
 * @brief Read the ad5246 digital potentiometer channel.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param data  - Channel data to be read.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5246_dpot_chn_read(struct dpot_dev *desc,
			 enum dpot_chn_type chn, uint8_t *data)
{
	int ret;
	struct dpot_command cmd;

	if (!desc || chn > DPOT_CHN_RDAC1)
		return -EINVAL;

	/* Read back RDAC contents (command #3) */
	cmd.data = 0x00;
	cmd.is_readback = true;

	ret = ad5246_dpot_send_cmd(desc, &cmd);
	if (ret)
		return ret;
	*data = cmd.response;
	return 0;
}

/**
 * @brief Write to the digital potentiometer channel.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param data - Channel data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5246_dpot_chn_write(struct dpot_dev *desc,
			  enum dpot_chn_type chn, uint8_t data)
{
	int ret;
	struct dpot_command cmd;

	if (!desc || chn > DPOT_CHN_RDAC1)
		return -EINVAL;

	/* Write contents of serial register data to RDAC (command #1) */
	cmd.data = data;
	cmd.is_readback = false;

	ret = ad5246_dpot_send_cmd(desc, &cmd);
	if (ret)
		return ret;

	return 0;
}
/* ad5246 digital potentiometer ops structure */
const struct dpot_ops ad5246_dpot_ops = {
	.dpot_init = &ad5246_dpot_init,
	.dpot_reset = NULL,
	.dpot_shutdown = NULL,
	.dpot_set_operating_mode = NULL,
	.dpot_input_reg_read = NULL,
	.dpot_input_reg_write = NULL,
	.dpot_sw_lrdac_update = NULL,
	.dpot_chn_read = &ad5246_dpot_chn_read,
	.dpot_chn_write = &ad5246_dpot_chn_write,
	.dpot_nvm_read = NULL,
	.dpot_nvm_write = NULL,
	.dpot_copy_rdac_to_nvm = NULL,
	.dpot_copy_nvm_to_rdac = NULL,
	.dpot_rdac_linear_update = NULL,
	.dpot_rdac_6db_update = NULL,
	.dpot_remove = &ad5246_dpot_remove,
	.dpot_enable_top_bottom_scale = NULL,
	.dpot_tolerance_read = NULL
};
