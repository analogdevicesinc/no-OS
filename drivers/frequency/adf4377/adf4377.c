/***************************************************************************//**
 *   @file   adf4377.c
 *   @brief  Implementation of adf4377 Driver.
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
#include <malloc.h>
#include "adf4377.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * @brief Writes data to ADF4377 over SPI.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param data - Data value to write.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t adf4377_spi_write(struct adf4377_dev *dev, uint8_t reg_addr,
			  uint8_t data)
{
	uint8_t buff[ADF4377_BUFF_SIZE_BYTES];

	if (dev->spi_desc->bit_order) {
		buff[0] = no_os_bit_swap_constant_8(reg_addr);
		buff[1] = no_os_bit_swap_constant_8(ADF4377_SPI_WRITE_CMD);
		buff[2] = no_os_bit_swap_constant_8(data);
	} else {
		buff[0] = ADF4377_SPI_WRITE_CMD;
		buff[1] = reg_addr;
		buff[2] = data;
	}

	return no_os_spi_write_and_read(dev->spi_desc, buff, ADF4377_BUFF_SIZE_BYTES);
}

/**
 * @brief Update ADF4377 register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - Mask for specific register bits to be updated.
 * @param data - Data written to the device (requires prior bit shifting).
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t adf4377_spi_write_mask(struct adf4377_dev *dev, uint8_t reg_addr,
			       uint8_t mask, uint8_t data)
{
	uint8_t read_val;
	int32_t ret;

	ret = adf4377_spi_read(dev, reg_addr, &read_val);
	if (ret < 0)
		return ret;

	read_val &= ~mask;
	read_val |= data;

	return adf4377_spi_write(dev, reg_addr, read_val);
}

/**
 * @brief Reads data from ADF4377 over SPI.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param data - Data read from the device.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t adf4377_spi_read(struct adf4377_dev *dev, uint8_t reg_addr,
			 uint8_t *data)
{
	int32_t ret;
	uint8_t buff[ADF4377_BUFF_SIZE_BYTES];

	if (dev->spi_desc->bit_order) {
		buff[0] = no_os_bit_swap_constant_8(reg_addr);
		buff[1] = no_os_bit_swap_constant_8(ADF4377_SPI_READ_CMD);
		buff[2] = no_os_bit_swap_constant_8(ADF4377_SPI_DUMMY_DATA);
	} else {
		buff[0] = ADF4377_SPI_READ_CMD;
		buff[1] = reg_addr;
		buff[2] = ADF4377_SPI_DUMMY_DATA;
	}

	ret = no_os_spi_write_and_read(dev->spi_desc, buff, ADF4377_BUFF_SIZE_BYTES);
	if(ret < 0)
		return ret;

	*data = buff[2];

	return ret;
}

/**
 * @brief ADF4377 SPI Scratchpad check.
 * @param dev - The device structure.
 * @return Returns 0 in case of success or negative error code.
 */
int32_t adf4377_check_scratchpad(struct adf4377_dev *dev)
{
	int32_t ret;
	uint8_t scratchpad;

	ret = adf4377_spi_write(dev, ADF4377_REG(0x0A), ADF4377_SPI_SCRATCHPAD_TEST_A);
	if (ret < 0)
		return ret;

	ret = adf4377_spi_read(dev, ADF4377_REG(0x0A), &scratchpad);
	if (ret < 0)
		return ret;

	if(scratchpad != ADF4377_SPI_SCRATCHPAD_TEST_A)
		return -1;

	ret = adf4377_spi_write(dev, ADF4377_REG(0x0A), ADF4377_SPI_SCRATCHPAD_TEST_B);
	if (ret < 0)
		return ret;

	ret = adf4377_spi_read(dev, ADF4377_REG(0x0A), &scratchpad);
	if (ret < 0)
		return ret;

	if(scratchpad != ADF4377_SPI_SCRATCHPAD_TEST_B)
		return -1;

	return 0;
}

/**
 * @brief Set default datasheet register values.
 * @param dev - The device structure.
 * @return Returns 0 in case of success or negative error code.
 */
static int32_t adf4377_set_default(struct adf4377_dev *dev)
{
	int32_t ret;

	ret = adf4377_spi_write(dev, ADF4377_REG(0x0f), ADF4377_R00F_RSV1);
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write_mask(dev, ADF4377_REG(0x1c), ADF4377_R01C_RSV1_MSK,
				     ADF4377_R01C_RSV1(0x1));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write_mask(dev, ADF4377_REG(0x1f), ADF4377_R01F_RSV1_MSK,
				     ADF4377_R01F_RSV1(0x7));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write_mask(dev, ADF4377_REG(0x20), ADF4377_R020_RSV1_MSK,
				     ADF4377_R020_RSV1(0x1));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write(dev, ADF4377_REG(0x21), ADF4377_R021_RSV1);
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write(dev, ADF4377_REG(0x22), ADF4377_R022_RSV1);
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write(dev, ADF4377_REG(0x23), ADF4377_R023_RSV1);
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write_mask(dev, ADF4377_REG(0x25), ADF4377_R025_RSV1_MSK,
				     ADF4377_R025_RSV1(0xB));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write(dev, ADF4377_REG(0x2C), ADF4377_R02C_RSV1);
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write(dev, ADF4377_REG(0x31), ADF4377_R031_RSV1);
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write_mask(dev, ADF4377_REG(0x32), ADF4377_R032_RSV1_MSK,
				     ADF4377_R032_RSV1(0x9));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write(dev, ADF4377_REG(0x33), ADF4377_R033_RSV1);
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write(dev, ADF4377_REG(0x34), ADF4377_R034_RSV1);
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write(dev, ADF4377_REG(0x3A), ADF4377_R03A_RSV1);
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write(dev, ADF4377_REG(0x3B), ADF4377_R03B_RSV1);
	if (ret < 0)
		return ret;

	return adf4377_spi_write(dev, ADF4377_REG(0x42), ADF4377_R042_RSV1);
}

/**
 * @brief Software reset.
 * @param dev - The device structure.
 * @return Returns 0 in case of success or negative error code.
 */
int32_t adf4377_soft_reset(struct adf4377_dev *dev)
{
	int32_t ret;
	uint16_t timeout = 0xFFFF;
	uint8_t data;

	ret = adf4377_spi_write_mask(dev, ADF4377_REG(0x00), ADF4377_SOFT_RESET_MSK |
				     ADF4377_SOFT_RESET_R_MSK, ADF4377_SOFT_RESET(ADF4377_SOFT_RESET_EN) |
				     ADF4377_SOFT_RESET_R(ADF4377_SOFT_RESET_EN));
	if (ret < 0)
		return ret;

	while(timeout--) {
		ret = adf4377_spi_read(dev, ADF4377_REG(0x00), &data);
		if (ret < 0)
			return ret;

		if(!(data & ADF4377_SOFT_RESET(ADF4377_SOFT_RESET_EN)))
			return adf4377_set_default(dev);
	}

	return -1;
}

/**
 * Set the output frequency.
 * @param dev - The device structure.
 * @param freq - The output frequency.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t adf4377_set_freq(struct adf4377_dev *dev, uint64_t freq)
{
	int32_t ret;
	uint32_t timeout = 0xFFFFFFFF;
	uint8_t data;

	dev->clkout_div_sel = 0;

	if(ADF4377_CHECK_RANGE(dev->f_clk, CLKPN_FREQ))
		return -1;

	dev->f_vco = freq;

	while (dev->f_vco < ADF4377_MIN_VCO_FREQ) {
		dev->f_vco <<= 1;
		dev->clkout_div_sel++;
	}

	dev->n_int = freq / dev->f_pfd;

	ret = adf4377_spi_write_mask(dev, ADF4377_REG(0x11),
				     ADF4377_EN_RDBLR_MSK | ADF4377_N_INT_MSB_MSK,
				     ADF4377_EN_RDBLR(dev->ref_doubler_en) | ADF4377_N_INT_MSB(dev->n_int >> 8));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write_mask(dev, ADF4377_REG(0x12),
				     ADF4377_R_DIV_MSK | ADF4377_CLKOUT_DIV_MSK,
				     ADF4377_CLKOUT_DIV(dev->clkout_div_sel) | ADF4377_R_DIV(dev->ref_div_factor));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write(dev, ADF4377_REG(0x10), ADF4377_N_INT_LSB(dev->n_int));
	if (ret < 0)
		return ret;

	/** Wait for VCO calibration to finish */
	do {
		ret = adf4377_spi_read(dev, ADF4377_REG(0x49), &data);
		if (ret < 0)
			return ret;

	} while (((ADF4377_ADC_BUSY(1) | ADF4377_FSM_BUSY(1)) & data) && timeout--);

	if (!timeout)
		ret = -ETIMEDOUT;

	dev->f_clk = freq;

	return ret;

}

/**
 * Setup the device.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t adf4377_setup(struct adf4377_dev *dev)
{
	int32_t ret;
	uint32_t f_div_rclk;
	uint8_t dclk_div1, dclk_div2;
	bool dclk_mode_en;
	uint16_t synth_lock_timeout, vco_alc_timeout, adc_clk_div, vco_band_div;

	dev->ref_div_factor = 0;

	/* Set Default Registers */
	ret = adf4377_set_default(dev);
	if (ret < 0)
		return ret;

	/* Update Charge Pump Current Value */
	ret = adf4377_spi_write_mask(dev, ADF4377_REG(0x15), ADF4377_CP_I_MSK,
				     ADF4377_CP_I(dev->cp_i));
	if (ret < 0)
		return ret;

	/*Compute PFD */
	if (!(dev->ref_doubler_en))
		do {
			dev->ref_div_factor++;
			dev->f_pfd = dev->clkin_freq / dev->ref_div_factor;
		} while (dev->f_pfd > ADF4377_MAX_FREQ_PFD);
	else
		dev->f_pfd = dev->clkin_freq * (1 + dev->ref_doubler_en);

	if(ADF4377_CHECK_RANGE(dev->f_pfd, FREQ_PFD))
		return -1;

	f_div_rclk = dev->f_pfd;

	if (dev->f_pfd <= ADF4377_FREQ_PFD_80MHZ) {
		dclk_div1 = ADF4377_DCLK_DIV1_1;
		dclk_div2 = ADF4377_DCLK_DIV2_1;
		dclk_mode_en = false;
	} else if (dev->f_pfd <= ADF4377_FREQ_PFD_125MHZ) {
		dclk_div1 = ADF4377_DCLK_DIV1_1;
		dclk_div2 = ADF4377_DCLK_DIV2_1;
		dclk_mode_en = true;
	} else if (dev->f_pfd <= ADF4377_FREQ_PFD_160MHZ) {
		dclk_div1 = ADF4377_DCLK_DIV1_2;
		dclk_div2 = ADF4377_DCLK_DIV2_1;
		dclk_mode_en = false;
		f_div_rclk /= 2;
	} else if (dev->f_pfd <= ADF4377_FREQ_PFD_250MHZ) {
		dclk_div1 = ADF4377_DCLK_DIV1_2;
		dclk_div2 = ADF4377_DCLK_DIV2_1;
		dclk_mode_en = true;
		f_div_rclk /= 2;
	} else if (dev->f_pfd <= ADF4377_FREQ_PFD_320MHZ) {
		dclk_div1 = ADF4377_DCLK_DIV1_2;
		dclk_div2 = ADF4377_DCLK_DIV2_2;
		dclk_mode_en = false;
		f_div_rclk /= 4;
	} else {
		dclk_div1 = ADF4377_DCLK_DIV1_2;
		dclk_div2 = ADF4377_DCLK_DIV2_2;
		dclk_mode_en = true;
		f_div_rclk /= 4;
	}

	synth_lock_timeout = NO_OS_DIV_ROUND_UP(f_div_rclk, 50000);
	vco_alc_timeout = NO_OS_DIV_ROUND_UP(f_div_rclk, 20000);
	vco_band_div = NO_OS_DIV_ROUND_UP(f_div_rclk,
					  150000 * 16 * (1 << dclk_mode_en));
	adc_clk_div = NO_OS_DIV_ROUND_UP((f_div_rclk / 400000 - 2), 4);

	ret = adf4377_spi_write_mask(dev, ADF4377_REG(0x1C),
				     ADF4377_EN_DNCLK_MSK | ADF4377_EN_DRCLK_MSK,
				     ADF4377_EN_DNCLK(ADF4377_EN_DNCLK_ON) | ADF4377_EN_DRCLK(
					     ADF4377_EN_DRCLK_ON));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write_mask(dev, ADF4377_REG(0x1D), ADF4377_MUXOUT_MSK,
				     ADF4377_MUXOUT(dev->muxout_default));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write_mask(dev, ADF4377_REG(0x11),
				     ADF4377_EN_AUTOCAL_MSK | ADF4377_DCLK_DIV2_MSK,
				     ADF4377_EN_AUTOCAL(ADF4377_VCO_CALIB_EN) | ADF4377_DCLK_DIV2(dclk_div2));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write_mask(dev, ADF4377_REG(0x2E),
				     ADF4377_EN_ADC_CNV_MSK | ADF4377_EN_ADC_MSK | ADF4377_ADC_A_CONV_MSK,
				     ADF4377_EN_ADC_CNV(true) | ADF4377_EN_ADC(true) |
				     ADF4377_ADC_A_CONV(ADF4377_ADC_A_CONV_VCO_CALIB));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write_mask(dev, ADF4377_REG(0x20), ADF4377_EN_ADC_CLK_MSK,
				     ADF4377_EN_ADC_CLK(true));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write_mask(dev, ADF4377_REG(0x2F), ADF4377_DCLK_DIV1_MSK,
				     ADF4377_DCLK_DIV1(dclk_div1));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write_mask(dev, ADF4377_REG(0x24), ADF4377_DCLK_MODE_MSK,
				     ADF4377_DCLK_MODE(dclk_mode_en));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write(dev, ADF4377_REG(0x27),
				ADF4377_SYNTH_LOCK_TO_LSB(synth_lock_timeout));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write_mask(dev, ADF4377_REG(0x28),
				     ADF4377_SYNTH_LOCK_TO_MSB_MSK,
				     ADF4377_SYNTH_LOCK_TO_MSB(synth_lock_timeout >> 8));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write(dev,ADF4377_REG(0x29),
				ADF4377_VCO_ALC_TO_LSB(vco_alc_timeout));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write_mask(dev, ADF4377_REG(0x2A), ADF4377_VCO_ALC_TO_MSB_MSK,
				     ADF4377_VCO_ALC_TO_MSB(vco_alc_timeout >> 8));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write(dev, ADF4377_REG(0x26),
				ADF4377_VCO_BAND_DIV(vco_band_div));
	if (ret < 0)
		return ret;

	ret = adf4377_spi_write(dev, ADF4377_REG(0x2D),
				ADF4377_ADC_CLK_DIV(adc_clk_div));
	if (ret < 0)
		return ret;

	/* Power Up */
	ret = adf4377_spi_write(dev, ADF4377_REG(0x1a),
				ADF4377_PD_ALL(ADF4377_PD_ALL_N_OP) |
				ADF4377_PD_RDIV(ADF4377_PD_RDIV_N_OP) | ADF4377_PD_NDIV(ADF4377_PD_NDIV_N_OP) |
				ADF4377_PD_VCO(ADF4377_PD_VCO_N_OP) | ADF4377_PD_LD(ADF4377_PD_LD_N_OP) |
				ADF4377_PD_PFDCP(ADF4377_PD_PFDCP_N_OP) | ADF4377_PD_CLKOUT1(
					ADF4377_PD_CLKOUT1_N_OP) |
				ADF4377_PD_CLKOUT2(ADF4377_PD_CLKOUT2_N_OP));

	ret = adf4377_set_freq(dev, dev->f_clk);
	if (ret < 0)
		return ret;

	/* Disable EN_DNCLK, EN_DRCLK */
	ret = adf4377_spi_write_mask(dev, ADF4377_REG(0x1C),
				     ADF4377_EN_DNCLK_MSK | ADF4377_EN_DRCLK_MSK,
				     ADF4377_EN_DNCLK(ADF4377_EN_DNCLK_OFF) | ADF4377_EN_DRCLK(
					     ADF4377_EN_DRCLK_OFF));
	if (ret < 0)
		return ret;

	/* Disable EN_ADC_CLK */
	ret = adf4377_spi_write_mask(dev, ADF4377_REG(0x20), ADF4377_EN_ADC_CLK_MSK,
				     ADF4377_EN_ADC_CLK(false));

	/* Set output Amplitude */
	ret = adf4377_spi_write_mask(dev, ADF4377_REG(0x19),
				     ADF4377_CLKOUT2_OP_MSK | ADF4377_CLKOUT1_OP_MSK,
				     ADF4377_CLKOUT1_OP(dev->clkout_op) | ADF4377_CLKOUT2_OP(dev->clkout_op));

	return ret;
}

/**
 * @brief Initializes the ADF4377.
 * @param device - The device structure.
 * @param init_param - The structure containing the device initial parameters.
 * @return Returns 0 in case of success or negative error code.
 */
int32_t adf4377_init(struct adf4377_dev **device,
		     struct adf4377_init_param *init_param)
{
	int32_t ret;
	uint8_t device_id;
	uint8_t chip_type;
	struct adf4377_dev *dev;

	dev = (struct adf4377_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->dev_id = init_param->dev_id;
	dev->spi4wire = init_param->spi4wire;
	dev->clkin_freq = init_param->clkin_freq;
	dev->cp_i = init_param->cp_i;
	dev->muxout_default = init_param->muxout_select;
	dev->ref_doubler_en = init_param->ref_doubler_en;
	dev->f_clk = init_param->f_clk;
	dev->clkout_op = init_param->clkout_op;

	/* GPIO Chip Enable */
	ret = no_os_gpio_get_optional(&dev->gpio_ce, init_param->gpio_ce_param);
	if (ret < 0)
		goto error_dev;

	if(dev->gpio_ce) {
		ret = no_os_gpio_direction_output(dev->gpio_ce, NO_OS_GPIO_HIGH);
		if (ret < 0)
			goto error_gpio_ce;
	}

	/* SPI */
	ret = no_os_spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret < 0)
		goto error_gpio_ce;

	/* Software Reset */
	ret = adf4377_soft_reset(dev);
	if (ret < 0)
		goto error_spi;

	ret = adf4377_spi_write(dev, ADF4377_REG(0x00),
				ADF4377_LSB_FIRST_R(dev->spi_desc->bit_order) |
				ADF4377_LSB_FIRST(dev->spi_desc->bit_order) |
				ADF4377_SDO_ACTIVE_R(dev->spi4wire) |
				ADF4377_SDO_ACTIVE(dev->spi4wire) |
				ADF4377_ADDRESS_ASC_R(ADF4377_ADDR_ASC_AUTO_DECR) |
				ADF4377_ADDRESS_ASC(ADF4377_ADDR_ASC_AUTO_DECR));
	if (ret < 0)
		goto error_spi;

	/* Read Chip Type */
	ret = adf4377_spi_read(dev, ADF4377_REG(0x03), &chip_type);
	if (ret < 0)
		goto error_spi;

	if (chip_type != ADF4377_CHIP_TYPE)
		goto error_spi;

	/* Scratchpad Check */
	ret = adf4377_check_scratchpad(dev);
	if (ret < 0)
		goto error_spi;

	ret = adf4377_spi_read(dev, ADF4377_REG(0x04), &device_id);
	if (ret < 0)
		goto error_spi;

	if (dev->dev_id == device_id) {
		ret = no_os_gpio_get_optional(&dev->gpio_enclk1, init_param->gpio_enclk1_param);
		if (ret < 0)
			goto error_spi;

		if(dev->gpio_enclk1) {
			ret = no_os_gpio_direction_output(dev->gpio_enclk1, NO_OS_GPIO_HIGH);
			if (ret < 0)
				goto error_gpio_enclk1;
		}

		if (dev->dev_id == ADF4377) {
			ret = no_os_gpio_get_optional(&dev->gpio_enclk2, init_param->gpio_enclk2_param);
			if (ret < 0)
				goto error_gpio_enclk1;

			if(dev->gpio_enclk2) {
				ret = no_os_gpio_direction_output(dev->gpio_enclk2, NO_OS_GPIO_HIGH);
				if (ret < 0)
					goto error_gpio_enclk2;
			}
		}
	} else {
		ret = -ENODEV;
		goto error_spi;
	}

	ret = adf4377_setup(dev);
	if (ret < 0) {
		if (dev->dev_id == ADF4378)
			goto error_gpio_enclk1;
		else
			goto error_gpio_enclk2;
	}

	*device = dev;

	return ret;

error_gpio_enclk2:
	no_os_gpio_remove(dev->gpio_enclk2);

error_gpio_enclk1:
	no_os_gpio_remove(dev->gpio_enclk1);

error_spi:
	no_os_spi_remove(dev->spi_desc);

error_gpio_ce:
	no_os_gpio_remove(dev->gpio_ce);

error_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Free resoulces allocated for ADF4377
 * @param dev - The device structure.
 * @return Returns 0 in case of success or negative error code.
 */
int32_t adf4377_remove(struct adf4377_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret < 0)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_ce);
	if (ret < 0)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_enclk1);
	if (ret < 0)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_enclk2);
	if (ret < 0)
		return ret;

	no_os_free(dev);

	return ret;
}
