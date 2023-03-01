/***************************************************************************//**
 *   @file   ad5766.c
 *   @brief  Implementation of AD5766 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2016(c) Analog Devices, Inc.
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
#include <stdio.h>
#include <stdlib.h>
#include "ad5766.h"
#include "no_os_error.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/
/**
 * SPI command write to device.
 * @param dev - The device structure.
 * @param cmd - The command.
 * @param data - The data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5766_spi_cmd_write(struct ad5766_dev *dev,
			     uint8_t cmd,
			     uint16_t data)
{
	uint8_t buf[3];

	int32_t ret;

	buf[0] = cmd;
	buf[1] = (data & 0xFF00) >> 8;
	buf[2] = (data & 0x00FF) >> 0;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 3);

	return ret;
}

/**
 * SPI readback register from device.
 * @param dev - The device structure.
 * @param dac - The dac address.
 * @param data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5766_spi_readback_reg(struct ad5766_dev *dev,
				enum ad5766_dac dac,
				uint32_t *data)
{
	uint8_t buf[3] = {0, 0, 0};
	int32_t ret;

	if (dev->daisy_chain_en == AD5766_ENABLE) {
		printf("%s: This feature is not available in Daisy-Chain mode.\n",
		       __func__);
		return -1;
	}

	ad5766_spi_cmd_write(dev, AD5766_CMD_READBACK_REG(dac), 0x0000);

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 3);

	*data = (buf[0] << 16) | ((buf[1] << 8)) | (buf[2] << 0);

	return ret;
}

/**
 * Set software LDAC for the selected channels.
 * @param dev - The device structure.
 * @param setting - The setting.
 *		    Accepted values: AD5766_LDAC(x) | AD5766_LDAC(y) | ...
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5766_set_sw_ldac(struct ad5766_dev *dev,
			   uint16_t setting)
{
	return ad5766_spi_cmd_write(dev,
				    AD5766_CMD_SW_LDAC,
				    setting);
}

/**
 * Set clear code and span settings.
 * @param dev - The device structure.
 * @param clr - The clear code setting.
 *		Accepted values: AD5766_ZERO
 *				 AD5766_MID
 *				 AD5766_FULL
 * @param span - The span setting.
 *		 Accepted values: AD5766_M_20V_TO_0V
 *				  AD5766_M_16V_TO_0V
 *				  AD5766_M_10V_TO_0V
 *				  AD5766_M_12V_TO_P_14V
 *				  AD5766_M_16V_TO_P_10V
 *				  AD5766_M_5V_TO_P_6V
 *				  AD5766_M_10V_TO_P_10V
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5766_set_clr_span(struct ad5766_dev *dev,
			    enum ad5766_clr clr,
			    enum ad5766_span span)
{
	return ad5766_spi_cmd_write(dev,
				    AD5766_CMD_SPAN_REG,
				    AD5766_CFG_CLR(clr) | AD5766_SPAN(span));
}

/**
 * Power down the selected channels.
 * @param dev - The device structure.
 * @param setting - The setting.
 *		    Accepted values: AD5766_PWDN(x) | AD5766_PWDN(y) | ...
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5766_set_pwr_dac(struct ad5766_dev *dev,
			   uint16_t setting)
{
	return ad5766_spi_cmd_write(dev,
				    AD5766_CMD_WR_PWR_DAC,
				    setting);
}

/**
 * Power down the dither block for the selected channels.
 * @param dev - The device structure.
 * @param setting - The setting.
 *		    Accepted values: AD5766_PWDN(x) | AD5766_PWDN(y) | ...
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5766_set_pwr_dither(struct ad5766_dev *dev,
			      uint16_t setting)
{
	return ad5766_spi_cmd_write(dev,
				    AD5766_CMD_WR_PWR_DITHER,
				    setting);
}

/**
 * Enable the dither signal for the selected channels.
 * @param dev - The device structure.
 * @param setting - The setting.
 *		    Accepted values: AD5766_N0(x) | AD5766_N1(y) | ...
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5766_set_dither_signal(struct ad5766_dev *dev,
				 uint32_t setting)
{
	int32_t ret;

	ret = ad5766_spi_cmd_write(dev, AD5766_CMD_DITHER_SIG_1,
				   (setting & 0xFFFF) >> 0);
	ret |= ad5766_spi_cmd_write(dev, AD5766_CMD_DITHER_SIG_2,
				    (setting & 0xFFFF0000) >> 16);

	return ret;
}

/**
 * Invert the dither signal for the selected channels.
 * @param dev - The device structure.
 * @param setting - The setting.
 *		    Accepted values: AD5766_INV_D(x) | AD5766_INV_D(y) | ...
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5766_set_inv_dither(struct ad5766_dev *dev,
			      uint16_t setting)
{
	return ad5766_spi_cmd_write(dev, AD5766_CMD_INV_DITHER,
				    setting);
}

/**
 * Enable the dither scaling for the selected channels.
 * @param dev - The device structure.
 * @param setting - The setting.
 *		    Accepted values: AD5766_75(x) | AD5766_50(y) |
 *				     AD5766_25(z) | ...
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5766_set_dither_scale(struct ad5766_dev *dev,
				uint32_t setting)
{
	int32_t ret;

	ret = ad5766_spi_cmd_write(dev, AD5766_CMD_DITHER_SCALE_1,
				   (setting & 0xFFFF) >> 0);
	ret |= ad5766_spi_cmd_write(dev, AD5766_CMD_DITHER_SCALE_2,
				    (setting & 0xFFFF0000) >> 16);

	return ret;
}

/**
 * Do a software reset.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5766_do_soft_reset(struct ad5766_dev *dev)
{
	return ad5766_spi_cmd_write(dev, AD5766_CMD_SW_FULL_RESET,
				    AD5766_RESET);
}

/**
 * Set the input register for the selected channel.
 * @param dev - The device structure.
 * @param dac - The selected channel.
 * @param data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5766_set_in_reg(struct ad5766_dev *dev,
			  enum ad5766_dac dac,
			  uint16_t data)
{
	return ad5766_spi_cmd_write(dev, AD5766_CMD_WR_IN_REG(dac),
				    data);
}

/**
 * Set the DAC register for the selected channel.
 * @param dev - The device structure.
 * @param dac - The selected channel.
 * @param data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5766_set_dac_reg(struct ad5766_dev *dev,
			   enum ad5766_dac dac,
			   uint16_t data)
{
	return ad5766_spi_cmd_write(dev, AD5766_CMD_WR_DAC_REG(dac),
				    data);
}

/**
 * Set the DAC register for all channels.
 * @param dev - The device structure.
 * @param data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5766_set_dac_reg_all(struct ad5766_dev *dev,
			       uint16_t data)
{
	return ad5766_spi_cmd_write(dev, AD5766_CMD_WR_DAC_REG_ALL,
				    data);
}

/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 *		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad5766_init(struct ad5766_dev **device,
		    struct ad5766_init_param init_param)
{
	struct ad5766_dev *dev;
	int32_t ret;

	dev = (struct ad5766_dev *)no_os_malloc(sizeof(*dev));
	if (!dev) {
		return -1;
	}

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);

	/* GPIO */
	ret |= no_os_gpio_get(&dev->gpio_reset, &init_param.gpio_reset);
	ret |= no_os_gpio_direction_output(dev->gpio_reset, NO_OS_GPIO_LOW);
	no_os_mdelay(10);
	ret |= no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_HIGH);
	no_os_mdelay(10);

	/* Device Settings */
	dev->daisy_chain_en = init_param.daisy_chain_en;
	ret |= ad5766_spi_cmd_write(dev, AD5766_CMD_SDO_CNTRL,
				    dev->daisy_chain_en ? AD5766_SDO_EN : 0);

	ret |= ad5766_set_clr_span(dev, init_param.clr, init_param.span);
	ret |= ad5766_set_pwr_dac(dev, init_param.pwr_dac_setting);
	ret |= ad5766_set_pwr_dither(dev, init_param.pwr_dither_setting);
	ret |= ad5766_set_dither_signal(dev, init_param.dither_signal_setting);
	ret |= ad5766_set_inv_dither(dev, init_param.inv_dither_setting);
	ret |= ad5766_set_dither_scale(dev, init_param.dither_scale_setting);

	*device = dev;

	if (!ret)
		printf("AD5766 successfully initialized\n");
	else
		printf("AD5766 initialization error (%d)\n", ret);

	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad5766_init().
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad5766_remove(struct ad5766_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_desc);

	ret |= no_os_gpio_remove(dev->gpio_reset);

	no_os_free(dev);

	return ret;
}
