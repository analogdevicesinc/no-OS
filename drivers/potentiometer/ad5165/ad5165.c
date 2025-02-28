/***************************************************************************//**
 *   @file   ad5165.c
 *   @brief  Source file for the ad5165 digital potentiometer drivers
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
#include "ad5165.h"
#include "no_os_delay.h"

static int ad5165_dpot_send_cmd_write(struct dpot_dev *desc, struct dpot_command *cmd);
static uint8_t nRdac_value = 0x80;
/**
 * @brief Initialize the ad5165 digital potentiometer.
 * @param param - digipot init parameters.
 * @param desc - digipot descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5165_dpot_init(struct dpot_init_param *param, struct dpot_dev **desc)
{
	int ret=-EINVAL;
	struct dpot_dev *dev=NULL;
	struct ad5165_dpot_init_param *ad5165_params;
	struct ad5165_dpot_dev *ad5165_dev;
	struct no_os_spi_init_param *spi_init;
	if (!param)
		return -EINVAL;

	/* Allocate memory for digipot device descriptor */
	dev = (struct dpot_dev *)calloc(1, sizeof(struct dpot_dev));
	if (!dev)
		return -ENOMEM;

	/* Allocate memory for ad5165 digipot device descriptor */
	ad5165_dev = (struct ad5165_dpot_dev *)calloc(1, sizeof(*ad5165_dev));
	if (!ad5165_dev) {
		ret = -ENOMEM;
		goto err_ad5165_dev;
	}

	ad5165_params = param->extra;
	ad5165_params->eIntfType = param->intf_type;
	dev->extra = ad5165_dev;

	/* Initialize the digital interface */
	if (param->intf_type == AD_SPI_INTERFACE) {
		spi_init = ((struct ad5165_dpot_init_param *)param->extra)->spi_init;
		ret = no_os_spi_init(&ad5165_dev->spi_desc, spi_init);
		if (ret)
			goto err_intf_init;

	} else {
		/* Interface not supported */
		ret = -EINVAL;
		goto err_intf_init;
	}

	ad5165_dev->intf_type = param->intf_type;

	*desc = dev;

	return 0;

err_intf_init:
	free(ad5165_dev);
err_ad5165_dev:
	free(dev);

	return ret;
}

/**
 * @brief Free the memory allocated by ad5165_dpot_init().
 * @param desc - digipot descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5165_dpot_remove(struct dpot_dev *desc)
{
	int ret;
	struct ad5165_dpot_dev *ad5165_dev;

	if (!desc || !desc->extra)
		return -EINVAL;

	ad5165_dev = desc->extra;

	if (ad5165_dev->intf_type == AD_SPI_INTERFACE) {
		ret = no_os_spi_remove((struct no_os_spi_desc *)ad5165_dev->spi_desc);
		if (ret)
			return ret;
	}
	free(ad5165_dev);
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
int ad5165_dpot_chn_write(struct dpot_dev *desc,
			  enum dpot_chn_type chn,
			  uint8_t data)
{
	int ret;
	struct dpot_command cmd;

	if (!desc)
		return -EINVAL;

	/* Write contents of serial register data to RDAC (command #1) */
	cmd.data = data;
	cmd.is_readback = true;

	ret = ad5165_dpot_send_cmd_write(desc, &cmd);
	if (ret)
		return ret;
	nRdac_value = data;
	return 0;
}
/**
 * @brief Read the ad5165 digital potentiometer channel.
 * @param desc - digipot descriptor.
 * @param chn  - digipot channel.
 * @param data  - Channel data to be read.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5165_dpot_chn_read(struct dpot_dev *desc,
			 enum dpot_chn_type chn, uint8_t *data)
{
	if (!desc || chn > DPOT_CHN_RDAC1 )
		return -EINVAL;

	*data = nRdac_value;

	return 0;
}

/**
 * @brief Send command word to the ad5165 digital potentiometer.
 * @param desc - digipot descriptor.
 * @param cmd  - digipot command word
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5165_dpot_send_cmd_write(struct dpot_dev *desc,
			       struct dpot_command *cmd)
{
	int ret;
	uint8_t buf[2];

	struct ad5165_dpot_dev *ad5165_dev;
	if (!desc || !cmd)
		return -EINVAL;

	ad5165_dev = desc->extra;

	/* copy the word to be programmed to RDAC register */
	buf[0] = 0;
	buf[1] = cmd->data;
	/* Send command word based on the interface type selection */

	if (ad5165_dev->intf_type == AD_SPI_INTERFACE) {
		ret = no_os_spi_write_and_read(ad5165_dev->spi_desc, &buf[0], sizeof(buf));
		if (ret)
			return ret;
	} else {
		return -EINVAL;
	}

	return 0;
}

/* ad5165 digital potentiometer ops structure */
const struct dpot_ops ad5165_dpot_ops = {
	.dpot_init = &ad5165_dpot_init,
	.dpot_reset = NULL,
	.dpot_shutdown = NULL,
	.dpot_set_operating_mode = NULL,
	.dpot_input_reg_read = NULL,
	.dpot_input_reg_write = NULL,
	.dpot_sw_lrdac_update = NULL,
	.dpot_chn_read = &ad5165_dpot_chn_read,
	.dpot_chn_write = &ad5165_dpot_chn_write,
	.dpot_nvm_read = NULL,
	.dpot_nvm_write = NULL,
	.dpot_copy_rdac_to_nvm = NULL,
	.dpot_copy_nvm_to_rdac = NULL,
	.dpot_rdac_linear_update = NULL,
	.dpot_rdac_6db_update = NULL,
	.dpot_remove = &ad5165_dpot_remove,
	.dpot_enable_top_bottom_scale = NULL,
	.dpot_tolerance_read = NULL,
	.dpot_set_mid_scale = NULL
};
