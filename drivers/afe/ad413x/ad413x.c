/***************************************************************************//**
 *   @file   ad413x.c
 *   @brief  Implementation of AD413X Driver.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
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
#include <stdio.h>
#include <stdlib.h>
#include "ad413x.h"
#include "no-os/error.h"
#include "no-os/irq.h"
#include "no-os/print_log.h"
#include "no-os/delay.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/
/***************************************************************************//**
 * Compute CRC8 checksum.
 *
 * @param data      - The data buffer.
 * @param data_size - The size of the data buffer.
 *
 * @return CRC8 checksum.
*******************************************************************************/
static uint8_t ad413x_compute_crc8(uint8_t *data, uint8_t data_size)
{
	uint8_t i;
	uint8_t crc = 0;

	while (data_size) {
		for (i = 0x80; i != 0; i >>= 1) {
			if (((crc & 0x80) != 0) != ((*data & i) != 0)) {
				crc <<= 1;
				crc ^= AD413X_CRC8_POLY;
			} else
				crc <<= 1;
		}
		data++;
		data_size--;
	}
	return crc;
}

/***************************************************************************//**
 * SPI DATA register continuous read from device.
 *
 * @param dev      - The device structure.
 * @param reg_data - The register data.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
static int32_t ad413x_spi_data_cont_read(struct ad413x_dev *dev,
		uint32_t *reg_data)
{
	uint8_t ret;
	uint8_t buf[3];
	uint32_t data;

	memset(buf, 0xAA, 3); // dummy data bytes

	ret = spi_write_and_read(dev->spi_dev, buf, 3);
	if (ret)
		return ret;

	data = (buf[0] << 16) | (buf[1] << 8) | buf[2];

	*reg_data = data;

	return SUCCESS;
}

/***************************************************************************//**
 * IRQ handler for single ADC read.
*******************************************************************************/
static void irq_adc_single_read(struct ad413x_callback_ctx *ctx)
{
	struct ad413x_dev *dev = ctx->dev;
	if(ctx->buffer_size > 0) {
		if(ad413x_spi_int_reg_read(ctx->dev, AD413X_REG_DATA, ctx->buffer))
			pr_err("DATA reg could not be read \n");
		ctx->buffer_size--;
		ctx->buffer++;
		if(irq_enable(dev->irq_desc, dev->rdy_pin))
			pr_err("IRQ_enable error \n");
	} else if(irq_disable(dev->irq_desc, dev->rdy_pin)) {
		pr_err("IRQ_disable error \n");
	}
}

/***************************************************************************//**
 * IRQ handler for continuously ADC read.
*******************************************************************************/
static void irq_adc_cont_read(struct ad413x_callback_ctx *ctx)
{
	struct ad413x_dev *dev = ctx->dev;
	if(ctx->buffer_size > 0) {
		if(ad413x_spi_data_cont_read(ctx->dev, ctx->buffer))
			pr_err("DATA reg could not be read \n");
		ctx->buffer_size--;
		ctx->buffer++;
		if(irq_enable(dev->irq_desc, dev->rdy_pin))
			pr_err("IRQ_enable error \n");
	} else if(irq_disable(dev->irq_desc, dev->rdy_pin)) {
		pr_err("IRQ_disable error \n");
	}
}

/***************************************************************************//**
 * SPI internal register write to device using a mask.
 *
 * @param dev      - The device structure.
 * @param reg_addr - The register address.
 * @param data     - The register data.
 * @param mask     - The mask.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_spi_int_reg_write_msk(struct ad413x_dev *dev,
				     uint32_t reg_addr,
				     uint32_t data,
				     uint32_t mask)
{
	int32_t ret;
	uint32_t reg_data;

	ret = ad413x_spi_int_reg_read(dev, reg_addr, &reg_data);
	if (ret)
		return ret;
	reg_data &= ~mask;
	reg_data |= data;
	return ad413x_spi_int_reg_write(dev, reg_addr, reg_data);
}

/***************************************************************************//**
 * Set the mode of the ADC.
 *
 * @param dev       - The device structure.
 * @param mode      - The ADC mode
 *		      Accepted values: AD4110_CONTINOUS_CONV_MODE
 *				       AD4110_SINGLE_CONV_MODE
 *				       AD4110_STANDBY_MODE
 *				       AD4110_PW_DOWN_MODE
 *				       AD4110_SYS_OFFSET_CAL
 *				       AD4110_SYS_GAIN_CAL
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_set_adc_mode(struct ad413x_dev *dev, enum ad413x_adc_mode mode)
{
	int32_t ret;
	ret = ad413x_spi_int_reg_write_msk(dev,
					   AD413X_REG_ADC_CTRL,
					   AD413X_ADC_MODE(mode),
					   AD413X_ADC_MODE(0xF));
	if (ret)
		return ret;

	dev->op_mode = mode;

	return ret;
}

/***************************************************************************//**
 * Set the gain from configuration register.
 *
 * @param dev  - The device structure.
 * @param gain - The gain value.
 * 		 Accepted values: AD413X_GAIN_1
 * 				  AD413X_GAIN_2
 * 				  AD413X_GAIN_4
 * 				  AD413X_GAIN_8
 * 				  AD413X_GAIN_16
 * 				  AD413X_GAIN_32
 * 				  AD413X_GAIN_64
 *				  AD413X_GAIN_128
 * @param reg_nb - Number of Configuration Register
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_set_gain(struct ad413x_dev *dev, enum ad413x_gain gain,
			uint8_t reg_nb)
{
	int32_t ret;

	ret = ad413x_spi_int_reg_write_msk(dev,
					   AD413X_REG_CONFIG(reg_nb),
					   AD413X_PGA_N(gain),
					   AD413X_PGA_N(0xF));
	if (ret)
		return ret;

	dev->preset[reg_nb].gain = gain;

	return SUCCESS;
}

/***************************************************************************//**
 * Select reference from configuration register.
 *
 * @param dev  - The device structure.
 * @param ref - The gain value.
 * 		 Accepted values: AD413X_REFIN1
 *					AD413X_REFIN2
 *					AD413X_REFOUT_AVSS
 *					AD413X_AVDD_AVSS
 * @param reg_nb - Number of Configuration Register
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_set_ref(struct ad413x_dev *dev, enum ad413x_ref_sel ref,
		       uint8_t reg_nb)
{
	int32_t ret;

	ret = ad413x_spi_int_reg_write_msk(dev,
					   AD413X_REG_CONFIG(reg_nb),
					   AD413X_REF_SEL_N(ref),
					   AD413X_REF_SEL_N(0xF));
	if (ret)
		return ret;

	dev->preset[reg_nb].ref_sel = ref;

	return SUCCESS;
}

/***************************************************************************//**
 * Select filter from filter register.
 *
 * @param dev  	 - The device structure.
 * @param filter - The gain value.
 * 		 Accepted values: AD413X_SYNC4_STANDALONE
 *					AD413X_SYNC4_SYNC1
 *					AD413X_SYNC3_STANDALONE
 *					AD413X_SYNC3_REJ60
 *					AD413X_SYNC3_SYNC1
 *					AD413X_SYNC3_PF1
 *					AD413X_SYNC3_PF2
 *					AD413X_SYNC3_PF3
 *					AD413X_SYNC3_PF4
 * @param reg_nb - Number of Configuration Register
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_set_filter(struct ad413x_dev *dev, enum ad413x_filter filter,
			  uint8_t reg_nb)
{
	int32_t ret;

	ret = ad413x_spi_int_reg_write_msk(dev,
					   AD413X_REG_FILTER(reg_nb),
					   AD413X_FILTER_MODE_N(filter),
					   AD413X_FILTER_MODE_N(0xF));
	if (ret)
		return ret;

	dev->preset[reg_nb].filter = filter;

	return SUCCESS;
}

/***************************************************************************//**
 * Select preset for adc channel.
 *
 * @param dev  		- The device structure.
 * @param ch_nb  	- The channel number.
 * @param preset_nb - The preset number.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_set_ch_preset(struct ad413x_dev *dev, uint8_t ch_nb,
			     uint8_t preset_nb)
{
	int32_t ret;

	ret = ad413x_spi_int_reg_write_msk(dev,
					   AD413X_REG_CHN(ch_nb),
					   AD413X_SETUP_M(preset_nb),
					   AD413X_SETUP_M(0xF));
	if (ret)
		return ret;

	dev->ch[ch_nb].preset = preset_nb;

	return SUCCESS;
}

/***************************************************************************//**
 * Enable/disable channel.
 *
 * @param dev  		- The device structure.
 * @param ch_nb  	- The channel number.
 * @param enable 	- Channel status.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_ch_en(struct ad413x_dev *dev, uint8_t ch_nb, uint8_t enable)
{
	int32_t ret;

	if (enable) {
		ret = ad413x_spi_int_reg_write_msk(dev,
						   AD413X_REG_CHN(ch_nb),
						   AD413X_ENABLE_M,
						   AD413X_ENABLE_M);
		if (ret)
			return ret;
	} else {
		ret = ad413x_spi_int_reg_write_msk(dev,
						   AD413X_REG_CHN(ch_nb),
						   0x0U,
						   AD413X_ENABLE_M);
		if (ret)
			return ret;
	}

	dev->ch[ch_nb].enable = enable;

	return SUCCESS;
}

/***************************************************************************//**
 * Enable/disable bipolar data coding.
 *
 * @param dev  		- The device structure.
 * @param enable 	- 1 - Bipolar
 * 					  0 - Unipolar
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_adc_bipolar(struct ad413x_dev *dev, uint8_t enable)
{
	int32_t ret;

	if (enable) {
		ret = ad413x_spi_int_reg_write_msk(dev,
						   AD413X_REG_ADC_CTRL,
						   AD413X_ADC_BIPOLAR,
						   AD413X_ADC_BIPOLAR);
		if (ret)
			return ret;
	} else {
		ret = ad413x_spi_int_reg_write_msk(dev,
						   AD413X_REG_ADC_CTRL,
						   0x0U,
						   AD413X_ADC_BIPOLAR);
		if (ret)
			return ret;
	}

	dev->bipolar = enable;

	return SUCCESS;
}

/***************************************************************************//**
 * Set ADC master clock mode.
 *
 * @param dev  - The device structure.
 * @param clk - The clock mode.
 * 		 Accepted values: AD413X_INT_76_8_KHZ_OUT_OFF
 * 				  AD413X_INT_76_8_KHZ_OUT_ON
 * 				  AD413X_EXT_76_8KHZ
 * 				  AD413X_EXT_153_6_KHZ_DIV_2
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_set_mclk(struct ad413x_dev *dev, enum ad413x_mclk_sel clk)
{
	int32_t ret;
	ret = ad413x_spi_int_reg_write_msk(dev,
					   AD413X_REG_ADC_CTRL,
					   AD413X_ADC_CNTRL_MCLK(clk),
					   AD413X_ADC_CNTRL_MCLK(0xF));
	if(ret)
		return ret;

	dev->mclk = clk;

	return ret;
}

/***************************************************************************//**
 * Do a SPI software reset.
 *
 * @param dev - The device structure.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_spi_do_soft_reset(struct ad413x_dev *dev)
{
	int32_t ret;
	uint8_t buf [ 8 ] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

	/* The AD413X can be reset by writing a series of 64 1�s to the DIN
	 * input */
	ret = spi_write_and_read(dev->spi_dev, buf, 8);
	if(ret)
		return ret;

	mdelay(1); // TBD

	return SUCCESS;
}

/***************************************************************************//**
 * SPI internal register write to device.
 *
 * @param dev      - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_spi_int_reg_write(struct ad413x_dev *dev,
				 uint32_t reg_addr,
				 uint32_t reg_data)
{
	uint8_t buf[5];

	uint8_t data_size = AD413X_TRANSF_LEN(reg_addr);

	buf[0] = AD413X_CMD_WR_COM_REG(reg_addr);

	switch(data_size) {
	case 1:
		buf[1] = (reg_data & 0xFF);
		break;

	case 2:
		buf[1] = (reg_data & 0xFF00) >> 8;
		buf[2] = (reg_data & 0xFF);
		break;

	case 3:
		buf[1] = (reg_data & 0xFF0000) >> 16;
		buf[2] = (reg_data & 0xFF00) >> 8;
		buf[3] = (reg_data & 0xFF);
		break;

	default:
		return -EINVAL;
	}

	return spi_write_and_read(dev->spi_dev, buf, data_size + 1);
}

/***************************************************************************//**
 * SPI internal register read from device.
 *
 * @param dev      - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_spi_int_reg_read(struct ad413x_dev *dev,
				uint32_t reg_addr,
				uint32_t *reg_data)
{
	uint8_t ret;
	uint8_t buf[5];
	uint32_t data;

	uint8_t data_size = AD413X_TRANSF_LEN(reg_addr);

	buf[0] = AD413X_CMD_RD_COM_REG(reg_addr);

	memset(buf + 1, 0xAA, 4); // dummy data bytes

	ret = spi_write_and_read(dev->spi_dev, buf, data_size + 1);
	if (ret)
		return ret;

	switch (data_size) {
	case 1:
		data = buf[1];
		break;

	case 2:
		data = (buf[1] << 8) | buf[2];
		break;

	case 3:
		data = (buf[1] << 16) | (buf[2] << 8) | buf[3];
		break;

	default:
		return -EINVAL;
	}

	*reg_data = data;

	return SUCCESS;
}

/***************************************************************************//**
 * Single read of each adc active channel's data.
 *
 * @param device     - The device structure.
 * @param buffer	 - Buffer to store read data.
 * @param ch_nb		 - Number of active channels.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_single_read(struct ad413x_dev *dev, int32_t *buffer,
			   int32_t ch_nb)
{
	int32_t ret;

	struct ad413x_callback_ctx ctx = {
		.dev = dev,
		.buffer = buffer,
		.buffer_size = ch_nb,
	};

	struct callback_desc irq_callback = {
		.callback = &irq_adc_single_read,
		.ctx = &ctx
	};

	ret = irq_trigger_level_set(dev->irq_desc, dev->rdy_pin, IRQ_LEVEL_LOW);
	if (ret)
		return ret;
	ret = irq_register_callback(dev->irq_desc, dev->rdy_pin, &irq_callback);
	if (ret)
		return ret;

	ret = ad413x_set_adc_mode(dev, AD413X_SINGLE_CONV_MODE);
	if (ret)
		return ret;

	// make sure adc is fully initialized before irq enabling
	mdelay(1U);

	ret = irq_enable(dev->irq_desc, dev->rdy_pin);
	if (ret)
		return ret;

	while(ctx.buffer_size != 0U) ;

	return SUCCESS;
}

/***************************************************************************//**
 * Continuous read of each adc active channel's data.
 *
 * @param dev        - The device structure.
 * @param buffer	 - Buffer to store read data.
 * @param ch_nb		 - Number of active channels.
 * @param sample_nb  - Samples number.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_continuous_read(struct ad413x_dev *dev, int32_t *buffer,
			       int32_t ch_nb, int32_t sample_nb)
{
	int32_t ret;

	struct ad413x_callback_ctx ctx = {
		.dev = dev,
		.buffer = buffer,
		.buffer_size = ch_nb * sample_nb,
	};

	struct callback_desc irq_callback = {
		.callback = &irq_adc_cont_read,
		.ctx = &ctx
	};

	ret = irq_trigger_level_set(dev->irq_desc, dev->rdy_pin, IRQ_LEVEL_LOW);
	if (ret)
		return ret;
	ret = irq_register_callback(dev->irq_desc, dev->rdy_pin, &irq_callback);
	if (ret)
		return ret;

	ret = ad413x_spi_int_reg_write_msk(dev,
					   AD413X_REG_ADC_CTRL,
					   AD413X_ADC_CONT_READ,
					   AD413X_ADC_CONT_READ);
	if (ret)
		return ret;

	ret = ad413x_set_adc_mode(dev, AD413X_CONTINOUS_CONV_MODE);
	if (ret)
		return ret;

	// make sure adc is fully initialized before irq enabling
	mdelay(1U);

	ret = irq_enable(dev->irq_desc, dev->rdy_pin);
	if (ret)
		return ret;

	while(ctx.buffer_size != 0U) ;

	return ad413x_spi_int_reg_write_msk(dev,
					    AD413X_REG_ADC_CTRL,
					    0x00,
					    AD413X_ADC_CONT_READ);
}

/***************************************************************************//**
 * Store adc channel presets.
 *
 * @param dev        - The device structure.
 * @param preset 	 - The structure to be saved as preset.
 * @param preset_nb	 - Preset's number.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_preset_store(struct ad413x_dev *dev, struct ad413x_preset preset,
			    uint8_t preset_nb)
{
	int32_t ret;

	ret = ad413x_set_gain(dev, preset.gain, preset_nb);
	if (ret)
		return ret;

	ret = ad413x_set_ref(dev, preset.ref_sel, preset_nb);
	if (ret)
		return ret;

	return ad413x_set_filter(dev, preset.filter, preset_nb);
}

/***************************************************************************//**
 * Initialize the device.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_init(struct ad413x_dev **device,
		    struct ad413x_init_param init_param)
{
	struct ad413x_dev *dev;
	uint32_t reg_data;
	int32_t ret;
	int32_t i;

	dev = (struct ad413x_dev *)malloc(sizeof(*dev));
	if (!dev)
		return FAILURE;

	dev->chip_id = init_param.chip_id;
	dev->int_ref = init_param.int_ref;

	/* SPI */
	ret = spi_init(&dev->spi_dev, init_param.spi_init);
	if (ret)
		goto err_dev;

	/* Device Settings */
	ret = ad413x_spi_do_soft_reset(dev);
	if (ret)
		goto err_spi;

	/* Change SPI to 4 wire*/
	ret = ad413x_spi_int_reg_write_msk(dev,
					   AD413X_REG_ADC_CTRL,
					   AD413X_ADC_CSB_EN,
					   AD413X_ADC_CSB_EN);
	if (ret)
		goto err_spi;

	/* TBD - chip ID is 0x00 for now */
	ret = ad413x_spi_int_reg_read(dev, AD413X_REG_ID, &reg_data);
	if (ret)
		goto err_spi;

	switch(dev->chip_id) {
	case AD4130_8:
		if(reg_data != AD4130_8) {
			goto err_spi;
		}
		break;
	default:
		goto err_spi;
	}

	ret = ad413x_set_adc_mode(dev, AD413X_STANDBY_MODE);
	if (ret)
		goto err_spi;

	/* TDB */
	mdelay(1);

	/* Preset configured and saved in dev */
	for (i = 0; i < 8; i++) {
		ret = ad413x_set_gain(dev, init_param.preset[i].gain, i);
		if (ret)
			goto err_spi;
		ret = ad413x_set_ref(dev, init_param.preset[i].ref_sel, i);
		if (ret)
			goto err_spi;
		ret = ad413x_set_filter(dev, init_param.preset[i].filter, i);
		if (ret)
			goto err_spi;
	}

	/* Channel setup */
	for (i = 0; i < 16; i++) {
		ret = ad413x_set_ch_preset(dev, i, init_param.ch[i].preset);
		if (ret)
			goto err_spi;
		ret = ad413x_ch_en(dev, i, init_param.ch[i].enable);
		if (ret)
			goto err_spi;

		ret = ad413x_spi_int_reg_write_msk(dev,
						   AD413X_REG_CHN(i),
						   AD413X_AINP_M(init_param.ch[i].ain_p),
						   AD413X_AINP_M(0xFF));
		if (ret)
			goto err_spi;
		dev->ch[i].ain_p = init_param.ch[i].ain_p;

		ret = ad413x_spi_int_reg_write_msk(dev,
						   AD413X_REG_CHN(i),
						   AD413X_AINM_M(init_param.ch[i].ain_m),
						   AD413X_AINM_M(0xFF));
		if (ret)
			goto err_spi;
		dev->ch[i].ain_m = init_param.ch[i].ain_m;
	}

	/* EN/DIS internal reference */
	if (dev->int_ref) {
		ret = ad413x_spi_int_reg_write_msk(dev,
						   AD413X_REG_ADC_CTRL,
						   AD413X_ADC_REF_EN,
						   AD413X_ADC_REF_EN);
	}

	ret = ad413x_adc_bipolar(dev, init_param.bipolar);
	if (ret)
		goto err_spi;

	ret = ad413x_set_mclk(dev, init_param.mclk);
	if (ret)
		goto err_spi;

	*device = dev;

	pr_info("AD413X successfully initialized\n");
	return SUCCESS;

err_spi:
	spi_remove(dev->spi_dev);
err_dev:
	free(dev);
	pr_err("AD413X initialization error (%d)\n", ret);
	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad413x_init().
 *
 * @param dev - The device structure.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_remove(struct ad413x_dev *dev)
{
	int32_t ret;

	ret = spi_remove(dev->spi_dev);
	if (ret)
		return ret;

	free(dev);

	return SUCCESS;
}
