/***************************************************************************//**
 *   @file   ad5259.c
 *   @brief  Source file for the ad5259 digital potentiometer drivers
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
#include "ad5259.h"
#include "no_os_delay.h"

static int ad5259_dpot_send_cmd_write(struct dpot_dev *desc,
				      struct dpot_command *cmd);
static int ad5259_dpot_send_cmd(struct dpot_dev *desc,
				struct dpot_command *cmd);

#define  AD5259_RDAC_READ_INSTRUCTION            0X00

#define  AD5259_EEPROM_READ_INSTRUCTION          0x20

#define  AD5259_RDAC_WRITE_INSTRUCTION           0X00

#define  AD5259_EEPROM_WRITE_INSTRUCTION         0x20

#define  AD5259_COPY_EEPROM_TO_RDAC_INSTRUCTION  0XA0

#define  AD5259_COPY_RDAC_TO_EEPROM_INSTRUCTION  0xC0

#define  AD5259_READ_TOLERANCE_INT_INSTRUCTION   0x3E

#define  AD5259_READ_TOLERANCE_DEC_INSTRUCTION   0x3F

/* AD5229 dpot command address mapped to channel type */
/**
 * @struct dpot_command.
 * @brief Digital potentiometer command structure.
 */
struct dpot_command_525x {
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
 * @brief Initialize the ad5259 digital potentiometer.
 * @param param - digipot init parameters.
 * @param desc - digipot descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5259_dpot_init(struct dpot_init_param *param, struct dpot_dev **desc)
{
	int ret = -EINVAL;
	struct dpot_dev *dev = NULL;
	struct ad5259_dpot_init_param *ad5259_params;
	struct ad5259_dpot_dev *ad5259_dev;
	struct no_os_i2c_init_param *i2c_init;

	if (!param)
		return -EINVAL;

	/* Allocate memory for digipot device descriptor */
	dev = (struct dpot_dev *)calloc(1, sizeof(struct dpot_dev));
	if (!dev)
		return -ENOMEM;

	/* Allocate memory for ad5259 digipot device descriptor */
	ad5259_dev = (struct ad5259_dpot_dev *)calloc(1, sizeof(*ad5259_dev));
	if (!ad5259_dev) {
		ret = -ENOMEM;
		goto err_ad5259_dev;
	}

	ad5259_params = param->extra;
	ad5259_params->eIntfType = param->intf_type;
	dev->extra = ad5259_dev;

	/* Initialize the digital interface */
	if (param->intf_type == AD_I2C_INTERFACE) {
		i2c_init = ((struct ad5259_dpot_init_param *)param->extra)->i2c_init;
		ret = no_os_i2c_init(&ad5259_dev->i2c_desc, i2c_init);
		if (ret)
			goto err_intf_init;
	} else {
		/* Interface not supported */
		ret = -EINVAL;
		goto err_dpot_gpio;
	}

	ad5259_dev->intf_type = param->intf_type;

	*desc = dev;

	return 0;

err_intf_init:
	if (ad5259_dev->i2c_desc)
		no_os_i2c_remove(ad5259_dev->i2c_desc);
err_dpot_gpio:
	free(ad5259_dev);
err_ad5259_dev:
	free(dev);

	return ret;
}

/**
 * @brief Free the memory allocated by ad5259_dpot_init().
 * @param desc - digipot descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5259_dpot_remove(struct dpot_dev *desc)
{
	int ret;
	struct ad5259_dpot_dev *ad5259_dev;

	if (!desc || !desc->extra)
		return -EINVAL;

	ad5259_dev = desc->extra;

	ret = no_os_i2c_remove((struct no_os_i2c_desc *)ad5259_dev->i2c_desc);
	if (ret)
		return ret;

	free(ad5259_dev);
	free(desc);

	return 0;
}

/**
 * @brief Read the ad5259 digital potentiometer channel.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param data  - Channel data to be read.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5259_dpot_chn_read(struct dpot_dev *desc,
			 enum dpot_chn_type chn, uint8_t *data)
{
	int ret;
	struct dpot_command cmd;

	if (!desc || chn > DPOT_CHN_RDAC1)
		return -EINVAL;

	/* Read back RDAC contents (command #3) */
	cmd.control =  AD5259_RDAC_READ_INSTRUCTION;
	cmd.data = 0x00;
	cmd.is_readback = true;

	ret = ad5259_dpot_send_cmd(desc, &cmd);
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
int ad5259_dpot_chn_write(struct dpot_dev *desc,
			  enum dpot_chn_type chn, uint8_t data)
{
	int ret;
	struct dpot_command cmd;

	if (!desc || chn > DPOT_CHN_RDAC1)
		return -EINVAL;

	/* Write contents of serial register data to RDAC (command #1) */
	cmd.control =  AD5259_RDAC_WRITE_INSTRUCTION;
	cmd.data = data;
	cmd.is_readback = false;

	ret = ad5259_dpot_send_cmd_write(desc, &cmd);
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
int ad5259_dpot_nvm_read(struct dpot_dev *desc,
			 enum dpot_chn_type chn, uint8_t *data)
{
	int ret;
	struct dpot_command cmd;

	if (!desc || chn > DPOT_CHN_RDAC1)
		return -EINVAL;

	/* Read back EEPROM contents (command #3) */
	cmd.control =  AD5259_EEPROM_READ_INSTRUCTION;
	cmd.data = 0x00;
	cmd.is_readback = true;

	ret = ad5259_dpot_send_cmd(desc, &cmd);
	if (ret)
		return ret;

	*data = cmd.response;

	return 0;
}

/**
 * @brief Read the NVM (non-volatile) memory data.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param data  - NVM data to be read.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5259_dpot_tolerance_read(struct dpot_dev *desc,
			       enum dpot_chn_type chn,
			       uint8_t *data)
{
	int ret;
	struct dpot_command cmd;

	if (!desc || chn > DPOT_CHN_RDAC1)
		return -EINVAL;

	/* Read back EEPROM contents (command #3) */
	cmd.control = AD5259_READ_TOLERANCE_INT_INSTRUCTION;
	cmd.data = 0x00;
	cmd.is_readback = true;

	ret = ad5259_dpot_send_cmd(desc, &cmd);
	if (ret)
		return ret;

	data[0] = cmd.response;

	cmd.control = AD5259_READ_TOLERANCE_DEC_INSTRUCTION;
	cmd.data = 0x00;
	cmd.is_readback = true;

	ret = ad5259_dpot_send_cmd(desc, &cmd);
	if (ret)
		return ret;
	data[1] = cmd.response;
	return 0;
}
/**
 * @brief Write data to NVM (non-volatile) memory.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param data  - NVM data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5259_dpot_nvm_write(struct dpot_dev *desc,
			  enum dpot_chn_type chn, uint8_t data)
{
	int ret;
	struct dpot_command cmd;

	if (!desc || chn > DPOT_CHN_RDAC1)
		return -EINVAL;

	/* Write contents of serial register data to EEPROM (command #11) */
	cmd.control =  AD5259_EEPROM_WRITE_INSTRUCTION;
	cmd.data = data;
	cmd.is_readback = false;

	ret = ad5259_dpot_send_cmd_write(desc, &cmd);
	if (ret)
		return ret;

	/* EEPROM write delay as per device specifications */
	no_os_mdelay(180);
	return 0;
}

/**
 * @brief Copy RDAC register to NVM/EEPROM.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5259_dpot_copy_rdac_to_nvm(struct dpot_dev *desc, enum dpot_chn_type chn)
{
	int ret;
	struct dpot_command cmd;

	if (!desc)
		return -EINVAL;
	if (!desc || chn > DPOT_CHN_RDAC1)
		return -EINVAL;

	/* Copy RDAC register to EEPROM (command #9) */
	cmd.control = AD5259_COPY_RDAC_TO_EEPROM_INSTRUCTION;
	cmd.data = 0x00;
	cmd.is_readback = false;

	ret = ad5259_dpot_send_cmd(desc, &cmd);
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
int ad5259_dpot_copy_nvm_to_rdac(struct dpot_dev *desc, enum dpot_chn_type chn)
{
	struct dpot_command cmd;

	if (!desc || chn > DPOT_CHN_RDAC1)
		return -EINVAL;

	/* Copy EEPROM into RDAC (command #10) */
	cmd.control =  AD5259_COPY_EEPROM_TO_RDAC_INSTRUCTION;
	cmd.data = 0x0;
	cmd.is_readback = false;
	return (ad5259_dpot_send_cmd(desc, &cmd));
}

/**
 * @brief Send command word to write to thethe ad5259 digital potentiometer.
 * @param desc - digipot descriptor.
 * @param cmd  - digipot command word
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad5259_dpot_send_cmd_write(struct dpot_dev *desc,
				      struct dpot_command *cmd)
{
	uint8_t buf[2];
	struct ad5259_dpot_dev *ad5259_dev;
	if (!desc || !cmd)
		return -EINVAL;

	ad5259_dev = desc->extra;

	/* Form a command word */
	buf[0] = cmd->control;
	buf[1] = cmd->data;
	/* Send command word based on the interface type selection */
	return (no_os_i2c_write(ad5259_dev->i2c_desc, buf, sizeof(buf), 1));

}
/**
 * @brief Send command word to the ad5259 digital potentiometer.
 * @param desc - digipot descriptor.
 * @param cmd  - digipot command word
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad5259_dpot_send_cmd(struct dpot_dev *desc,
				struct dpot_command *cmd)
{
	int ret;
	uint8_t buf[2];
	struct ad5259_dpot_dev *ad5259_dev;
	if (!desc || !cmd)
		return -EINVAL;

	ad5259_dev = desc->extra;

	/* Form a command word */
	buf[0] = cmd->control;
	/* Send command word via I2C interface */

	ret = no_os_i2c_write(ad5259_dev->i2c_desc, buf, 1, 1);
	if (ret)
		return ret;

	/* Read response (only for readback commands) */
	if (cmd->is_readback) {
		/* Form NOP command word to readback contents */
		buf[0] = 0x0;
		ret = no_os_i2c_read(ad5259_dev->i2c_desc, &buf[0], 1, 1);
		if (ret)
			return ret;
		cmd->response = buf[0];
	}

	return 0;
}

/* ad5259 digital potentiometer ops structure */
const struct dpot_ops ad5259_dpot_ops = {
	.dpot_init = &ad5259_dpot_init,
	.dpot_reset = NULL,
	.dpot_shutdown = NULL,
	.dpot_set_operating_mode = NULL,
	.dpot_input_reg_read = NULL,
	.dpot_input_reg_write = NULL,
	.dpot_sw_lrdac_update = NULL,
	.dpot_chn_read = &ad5259_dpot_chn_read,
	.dpot_chn_write = &ad5259_dpot_chn_write,
	.dpot_nvm_read = &ad5259_dpot_nvm_read,
	.dpot_nvm_write = &ad5259_dpot_nvm_write,
	.dpot_copy_rdac_to_nvm = &ad5259_dpot_copy_rdac_to_nvm,
	.dpot_copy_nvm_to_rdac = &ad5259_dpot_copy_nvm_to_rdac,
	.dpot_rdac_linear_update = NULL,
	.dpot_rdac_6db_update = NULL,
	.dpot_remove = &ad5259_dpot_remove,
	.dpot_enable_top_bottom_scale = NULL,
	.dpot_tolerance_read = &ad5259_dpot_tolerance_read
};
