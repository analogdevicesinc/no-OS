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
#include "no_os_error.h"
#include "no_os_irq.h"
#include "no_os_print_log.h"
#include "no_os_delay.h"
#include "no_os_crc8.h"
#include "no_os_spi.h"
#include "no_os_alloc.h"

NO_OS_DECLARE_CRC8_TABLE(ad413x_crc8);
uint32_t timeout = 0xFFFFFF;

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/***************************************************************************//**
 * SPI internal register write to device using a mask.
 *
 * @param dev      - The device structure.
 * @param reg_addr - The register address.
 * @param data     - The register data.
 * @param mask     - The mask.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_reg_write_msk(struct ad413x_dev *dev,
			     uint32_t reg_addr,
			     uint32_t data,
			     uint32_t mask)
{
	int32_t ret;
	uint32_t reg_data;

	ret = ad413x_reg_read(dev, reg_addr, &reg_data);
	if (ret)
		return ret;
	reg_data &= ~mask;
	reg_data |= data;
	return ad413x_reg_write(dev, reg_addr, reg_data);
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
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_set_adc_mode(struct ad413x_dev *dev, enum ad413x_adc_mode mode)
{
	int32_t ret;
	ret = ad413x_reg_write_msk(dev,
				   AD413X_REG_ADC_CTRL,
				   AD413X_ADC_MODE(mode),
				   AD413X_ADC_MODE(0xF));
	if (ret)
		return ret;

	dev->op_mode = mode;

	return 0;
}

/***************************************************************************//**
 * Set the internal reference.
 *
 * @param dev       - The device structure.
 * @param int_ref   - The internal reference option.
 *		      Accepted values: AD413X_INTREF_DISABLED
 *					   AD413X_INTREF_2_5V,
 *					   AD413X_INTREF_1_25V
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_set_int_ref(struct ad413x_dev *dev, enum ad413x_int_ref int_ref)
{
	int32_t ret;
	switch (int_ref) {
	case AD413X_INTREF_DISABLED:
		ret = ad413x_reg_write_msk(dev,
					   AD413X_REG_ADC_CTRL,
					   0U,
					   AD413X_ADC_REF_EN);
		break;

	case AD413X_INTREF_2_5V:
		ret = ad413x_reg_write_msk(dev,
					   AD413X_REG_ADC_CTRL,
					   AD413X_ADC_REF_EN,
					   AD413X_ADC_REF_EN);
		if (ret)
			return ret;
		ret = ad413x_reg_write_msk(dev,
					   AD413X_REG_ADC_CTRL,
					   0U,
					   AD413X_ADC_REF_VAL);
		break;

	case AD413X_INTREF_1_25V:
		ret = ad413x_reg_write_msk(dev,
					   AD413X_REG_ADC_CTRL,
					   AD413X_ADC_REF_EN,
					   AD413X_ADC_REF_EN);
		if (ret)
			return ret;
		ret = ad413x_reg_write_msk(dev,
					   AD413X_REG_ADC_CTRL,
					   AD413X_ADC_REF_VAL,
					   AD413X_ADC_REF_VAL);
		break;

	default:
		return -EINVAL;
	}

	if (ret)
		return ret;

	dev->int_ref = int_ref;

	return 0;
}

/***************************************************************************//**
 * Enable/disable DATA_STAT field.
 *
 * @param dev       - The device structure.
 * @param enable	- 0 DISABLE
 * 					  1 ENABLE
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_data_stat_en(struct ad413x_dev *dev, uint8_t enable)
{
	int32_t ret;
	ret = ad413x_reg_write_msk(dev,
				   AD413X_REG_ADC_CTRL,
				   enable ? AD413X_ADC_DATA_STATUS : 0x0U,
				   AD413X_ADC_DATA_STATUS);
	if (ret)
		return ret;

	dev->data_stat = enable;

	return 0;
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
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_set_gain(struct ad413x_dev *dev, enum ad413x_gain gain,
			enum ad413x_preset_nb reg_nb)
{
	int32_t ret;

	ret = ad413x_reg_write_msk(dev,
				   AD413X_REG_CONFIG(reg_nb),
				   AD413X_PGA_N(gain),
				   AD413X_PGA_N(0xF));
	if (ret)
		return ret;

	dev->preset[reg_nb].gain = gain;

	return 0;
}

/***************************************************************************//**
 * Select reference from configuration register.
 *
 * @param dev  - The device structure.
 * @param ref - The reference value.
 * 		 Accepted values: AD413X_REFIN1
 *					AD413X_REFIN2
 *					AD413X_REFOUT_AVSS
 *					AD413X_AVDD_AVSS
 * @param reg_nb - Number of Configuration Register
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_set_ref(struct ad413x_dev *dev, enum ad413x_ref_sel ref,
		       enum ad413x_preset_nb reg_nb)
{
	int32_t ret;

	ret = ad413x_reg_write_msk(dev,
				   AD413X_REG_CONFIG(reg_nb),
				   AD413X_REF_SEL_N(ref),
				   AD413X_REF_SEL_N(0xF));
	if (ret)
		return ret;

	dev->preset[reg_nb].ref_sel = ref;

	return 0;
}

/***************************************************************************//**
 * Select the reference buffers.
 *
 * @param dev  - The device structure.
 * @param ref_buf - The reference buffer status.
 * @param reg_nb - Number of Configuration Register
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_set_ref_buf(struct ad413x_dev *dev,
			   struct ad413x_ref_buf ref_buf,
			   enum ad413x_preset_nb reg_nb)
{
	int32_t ret;
	uint32_t reg_val;

	reg_val = no_os_field_prep(AD413X_REF_BUFP_N, ref_buf.ref_buf_p_en);
	reg_val |= no_os_field_prep(AD413X_REF_BUFM_N, ref_buf.ref_buf_m_en);

	ret = ad413x_reg_write_msk(dev,
				   AD413X_REG_CONFIG(reg_nb),
				   reg_val,
				   AD413X_REF_BUF_MSK);
	if (ret)
		return ret;

	dev->preset[reg_nb].ref_buf = ref_buf;

	return 0;
}

/***************************************************************************//**
 * Select filter from filter register.
 *
 * @param dev  	 - The device structure.
 * @param filter - The filter type.
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
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_set_filter(struct ad413x_dev *dev, enum ad413x_filter filter,
			  enum ad413x_preset_nb reg_nb)
{
	int32_t ret;

	ret = ad413x_reg_write_msk(dev,
				   AD413X_REG_FILTER(reg_nb),
				   AD413X_FILTER_MODE_N(filter),
				   AD413X_FILTER_MODE_N(0xF));
	if (ret)
		return ret;

	dev->preset[reg_nb].filter = filter;

	return 0;
}

/***************************************************************************//**
 * Select settle time from filter register.
 *
 * @param dev  	 - The device structure.
 * @param s_time - The settle time value.
 * 		 Accepted values: AD413X_32_MCLK
 *					AD413X_64_MCLK
 *					AD413X_128_MCLK
 *					AD413X_256_MCLK
 *					AD413X_512_MCLK
 *					AD413X_1024_MCLK
 *					AD413X_2048_MCLK
 *					AD413X_4096_MCLK
 * @param reg_nb - Number of Configuration Register
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_set_settle_time(struct ad413x_dev *dev,
			       enum ad413x_settle_time s_time,
			       enum ad413x_preset_nb reg_nb)
{
	int32_t ret;

	ret = ad413x_reg_write_msk(dev,
				   AD413X_REG_FILTER(reg_nb),
				   AD413X_SETTLE_N(s_time),
				   AD413X_SETTLE_N(0xFF));
	if (ret)
		return ret;

	dev->preset[reg_nb].s_time = s_time;

	return 0;
}

/***************************************************************************//**
 * Select excitation current value from config register.
 *
 * @param dev  	 - The device structure.
 * @param iout0_exc - The Iout0 excitation current value.
 * @param iout1_exc - The Iout1 excitation current value.
 * 		 Accepted values: AD413X_EXC_OFF
 *					AD413X_EXC_10UA
 *					AD413X_EXC_20UA
 *					AD413X_EXC_50UA
 *					AD413X_EXC_100UA
 *					AD413X_EXC_150UA
 *					AD413X_EXC_200UA
 *					AD413X_EXC_100NA
 * @param reg_nb - Number of Configuration Register
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_set_exc_current(struct ad413x_dev *dev,
			       enum ad413x_exc_current iout0_exc,
			       enum ad413x_exc_current iout1_exc,
			       enum ad413x_preset_nb reg_nb)
{
	int32_t ret;

	ret = ad413x_reg_write_msk(dev,
				   AD413X_REG_CONFIG(reg_nb),
				   AD413X_I_OUT0_N(iout0_exc) | AD413X_I_OUT1_N(iout1_exc),
				   AD413X_I_OUT_MSK);
	if (ret)
		return ret;

	dev->preset[reg_nb].iout0_exc_current = iout0_exc;
	dev->preset[reg_nb].iout1_exc_current = iout1_exc;

	return 0;
}

/***************************************************************************//**
 * Select preset for adc channel.
 *
 * @param dev  		- The device structure.
 * @param ch_nb  	- The channel number.
 * @param preset_nb - The preset number.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_set_ch_preset(struct ad413x_dev *dev, uint8_t ch_nb,
			     enum ad413x_preset_nb preset_nb)
{
	int32_t ret;

	ret = ad413x_reg_write_msk(dev,
				   AD413X_REG_CHN(ch_nb),
				   AD413X_SETUP_M(preset_nb),
				   AD413X_SETUP_M(0xF));
	if (ret)
		return ret;

	dev->ch[ch_nb].preset = preset_nb;

	return 0;
}

/***************************************************************************//**
 * Select the excitation source pins.
 *
 * @param dev  		- The device structure.
 * @param ch_nb  	- The channel number.
 * @param iout0_exc_inp 	- IOUT0 excitation input pin.
 * @param iout1_exc_inp 	- IOUT1 excitation input pin.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_ch_exc_input(struct ad413x_dev *dev, uint8_t ch_nb,
			    enum ad413x_input iout0_exc_inp,
			    enum ad413x_input iout1_exc_inp)
{
	int32_t ret;

	ret = ad413x_reg_write_msk(dev,
				   AD413X_REG_CHN(ch_nb),
				   AD413X_I_OUT0_CH_M(iout0_exc_inp) |
				   AD413X_I_OUT1_CH_M(iout1_exc_inp),
				   AD413X_I_OUT_CH_MSK);
	if (ret)
		return ret;

	dev->ch[ch_nb].iout0_exc_input = iout0_exc_inp;
	dev->ch[ch_nb].iout1_exc_input = iout1_exc_inp;

	return 0;
}

/***************************************************************************//**
 * Enable/disable channel.
 *
 * @param dev  		- The device structure.
 * @param ch_nb  	- The channel number.
 * @param enable 	- Channel status.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_ch_en(struct ad413x_dev *dev, uint8_t ch_nb, uint8_t enable)
{
	int32_t ret;

	ret = ad413x_reg_write_msk(dev,
				   AD413X_REG_CHN(ch_nb),
				   enable ? AD413X_ENABLE_M : 0x0U,
				   AD413X_ENABLE_M);
	if (ret)
		return ret;

	dev->ch[ch_nb].enable = enable;

	return 0;
}

/***************************************************************************//**
 * Enable/disable Power-Down Switch (PDSW).
 *
 * @param dev  		- The device structure.
 * @param ch_nb  	- The channel number.
 * @param pdsw_en 	- PDSW status.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_pdsw_en(struct ad413x_dev *dev, uint8_t ch_nb, bool pdsw_en)
{
	int32_t ret;

	ret = ad413x_reg_write_msk(dev,
				   AD413X_REG_CHN(ch_nb),
				   pdsw_en ? AD413X_PDSW_M : 0x0U,
				   AD413X_PDSW_M);
	if (ret)
		return ret;

	dev->ch[ch_nb].pdsw_en = pdsw_en;

	return 0;
}

/***************************************************************************//**
 * Enable/disable bipolar data coding.
 *
 * @param dev  		- The device structure.
 * @param enable 	- 1 - Bipolar
 * 					  0 - Unipolar
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_adc_bipolar(struct ad413x_dev *dev, uint8_t enable)
{
	int32_t ret;

	ret = ad413x_reg_write_msk(dev,
				   AD413X_REG_ADC_CTRL,
				   enable ? AD413X_ADC_BIPOLAR : 0x0U,
				   AD413X_ADC_BIPOLAR);
	if (ret)
		return ret;

	dev->bipolar = enable;

	return 0;
}

/***************************************************************************//**
 * Set output VBIAS on analog inputs.
 *
 * @param dev  		- The device structure.
 * @param v_bias_val 	- V_BIAS control register value
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_set_v_bias(struct ad413x_dev *dev, uint16_t v_bias_val)
{
	int32_t ret;

	ret = ad413x_reg_write(dev,
			       AD413X_REG_VBIAS_CTRL,
			       v_bias_val);
	if (ret)
		return ret;

	dev->v_bias = v_bias_val;

	return 0;
}

/***************************************************************************//**
 * Set standby control flags.
 *
 * @param dev  		- The device structure.
 * @param standby_ctrl 	- Standby control value
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_set_standby_ctrl(struct ad413x_dev *dev,
				struct ad413x_standby_ctrl standby_ctrl)
{
	int32_t ret;
	uint32_t reg_val;

	reg_val = no_os_field_prep(AD413X_STBY_INTREF_EN,
				   standby_ctrl.standby_int_ref_en);
	reg_val |= no_os_field_prep(AD413X_STBY_REFHOL_EN,
				    standby_ctrl.standby_ref_holder_en);
	reg_val |= no_os_field_prep(AD413X_STBY_IEXC_EN, standby_ctrl.standby_iexc_en);
	reg_val |= no_os_field_prep(AD413X_STBY_VBIAS_EN,
				    standby_ctrl.standby_vbias_en);
	reg_val |= no_os_field_prep(AD413X_STBY_BURNOUT_EN,
				    standby_ctrl.standby_burnout_en);
	reg_val |= no_os_field_prep(AD413X_STBY_PDSW_EN, standby_ctrl.standby_pdsw_en);
	reg_val |= no_os_field_prep(AD413X_STBY_GPO_EN, standby_ctrl.standby_gpio_en);
	reg_val |= no_os_field_prep(AD413X_STBY_DIAGNOSTICS_EN,
				    standby_ctrl.standby_diagn_en);
	reg_val |= no_os_field_prep(AD413X_STBY_OUT_EN, standby_ctrl.standby_output_en);

	ret = ad413x_reg_write_msk(dev,
				   AD413X_REG_MISC,
				   reg_val,
				   AD413X_STBY_CTRL_MSK);
	if (ret)
		return ret;

	dev->standby_ctrl = standby_ctrl;

	return 0;
}

/***************************************************************************//**
 * Set ADC master clock mode.
 *
 * @param dev  - The device structure.
 * @param clk  - The clock mode.
 * 		 Accepted values: AD413X_INT_76_8_KHZ_OUT_OFF
 * 				  AD413X_INT_76_8_KHZ_OUT_ON
 * 				  AD413X_EXT_76_8KHZ
 * 				  AD413X_EXT_153_6_KHZ_DIV_2
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_set_mclk(struct ad413x_dev *dev, enum ad413x_mclk_sel clk)
{
	int32_t ret;
	ret = ad413x_reg_write_msk(dev,
				   AD413X_REG_ADC_CTRL,
				   AD413X_ADC_CNTRL_MCLK(clk),
				   AD413X_ADC_CNTRL_MCLK(0xF));
	if(ret)
		return ret;

	dev->mclk = clk;

	return 0;
}

/***************************************************************************//**
 * Do a SPI software reset.
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_do_soft_reset(struct ad413x_dev *dev)
{
	int32_t ret;
	uint8_t buf [ 8 ] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

	/* The AD413X can be reset by writing a series of 64 consecutive 1s
	 * to the DIN input */
	ret = no_os_spi_write_and_read(dev->spi_dev, buf, 8);
	if(ret)
		return ret;

	no_os_mdelay(5); // TBD

	return 0;
}

/***************************************************************************//**
 * SPI internal register write to device.
 *
 * @param dev      - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_reg_write(struct ad413x_dev *dev,
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

	if (dev->spi_crc_en)
		buf[data_size] = no_os_crc8(ad413x_crc8, buf, ++data_size, 0);

	return no_os_spi_write_and_read(dev->spi_dev, buf, data_size + 1);
}

/***************************************************************************//**
 * SPI internal register read from device.
 *
 * @param dev      - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_reg_read(struct ad413x_dev *dev,
			uint32_t reg_addr,
			uint32_t *reg_data)
{
	uint8_t ret;
	uint8_t buf[] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
	uint32_t data;
	uint8_t crc;

	uint8_t data_size = AD413X_TRANSF_LEN(reg_addr);

	buf[0] = AD413X_CMD_RD_COM_REG(reg_addr);

	if ((reg_addr == AD413X_REG_DATA) && (dev->data_stat))
		data_size++;

	if (dev->spi_crc_en)
		data_size++;

	ret = no_os_spi_write_and_read(dev->spi_dev, buf, data_size + 1);
	if (ret)
		return ret;

	if (dev->spi_crc_en) {
		buf[0] = AD413X_CMD_RD_COM_REG(reg_addr);
		crc = no_os_crc8(ad413x_crc8, buf, data_size, 0);
		if (buf[data_size] != crc)
			return -EBADMSG;
		data_size--;
	}

	if ((reg_addr == AD413X_REG_DATA) && (dev->data_stat))
		data_size--;

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

	case 4:
		data = (buf[1] << 24) | (buf[2] << 16) | (buf[3] << 8) | buf[4];
		break;

	default:
		return -EINVAL;
	}

	*reg_data = data;

	return 0;
}

/***************************************************************************//**
 * Single conversion of each adc active channel.
 *
 * @param dev	     - The device structure.
 * @param buffer	 - Buffer to store read data. Buffer size needs to be at
 * 					least equal to the number of active channels. Results will
 * 					be stored in consecutive order of the active channels.
 * @param ch_nb		 - Number of active channels.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_single_conv(struct ad413x_dev *dev, uint32_t *buffer,
			   uint8_t ch_nb)
{
	int32_t ret;
	uint8_t pin_val;

	struct ad413x_callback_ctx ctx = {
		.dev = dev,
		.buffer = buffer,
		.buffer_size = ch_nb
	};

	ret = ad413x_set_adc_mode(dev, AD413X_SINGLE_CONV_MODE);
	if (ret)
		return ret;

	while(ctx.buffer_size) {
		timeout = 0xFFFFFF;
		do {
			ret = no_os_gpio_get_value(dev->rdy_pin_desc, &pin_val);;
			if (ret)
				timeout = 0;
			timeout--;
		} while (pin_val && timeout);
		if (timeout) {
			ret = ad413x_reg_read(dev, AD413X_REG_DATA, ctx.buffer);
			if (ret)
				return ret;
			ctx.buffer_size--;
			ctx.buffer++;
		}
	}

	if (!timeout)
		return -ETIMEDOUT;

	return 0;
}

/***************************************************************************//**
 * Continuous conversion of each adc active channel.
 *
 * @param dev        - The device structure.
 * @param buffer	 - Buffer to store read data. Buffer size needs to be at
 * 					least equal to the number of active channels * samples number.
 * 					Results will be stored in consecutive order of the active
 * 					channels.
 * @param ch_nb		 - Number of active channels.
 * @param sample_nb  - Samples number.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_continuous_conv(struct ad413x_dev *dev, uint32_t *buffer,
			       uint8_t ch_nb, uint32_t sample_nb)
{
	int ret;
	uint8_t pin_val;

	struct ad413x_callback_ctx ctx = {
		.dev = dev,
		.buffer = buffer,
		.buffer_size = ch_nb * sample_nb
	};

	ret = ad413x_set_adc_mode(dev, AD413X_CONTINOUS_CONV_MODE);
	if (ret)
		return ret;

	while(ctx.buffer_size > 0) {
		timeout = 0xFFFFFF;
		do {
			ret = no_os_gpio_get_value(dev->rdy_pin_desc, &pin_val);
			if (ret)
				timeout = 0;
			timeout--;
		} while (pin_val);// && timeout);
		if (timeout) {
			ret = ad413x_reg_read(dev, AD413X_REG_DATA, ctx.buffer);
			if (ret)
				return ret;
			ctx.buffer_size--;
			ctx.buffer++;
		}
	}

	ret = ad413x_set_adc_mode(dev, AD413X_STANDBY_MODE);
	if (ret)
		return ret;

	if (!timeout)
		return -ETIMEDOUT;

	return 0;
}

/***************************************************************************//**
 * Store adc channel presets.
 *
 * @param dev        - The device structure.
 * @param preset 	 - The structure to be saved as preset.
 * @param preset_nb	 - Preset's number.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_preset_store(struct ad413x_dev *dev, struct ad413x_preset preset,
			    enum ad413x_preset_nb preset_nb)
{
	int32_t ret;

	ret = ad413x_set_gain(dev, preset.gain, preset_nb);
	if (ret)
		return ret;

	ret = ad413x_set_ref(dev, preset.ref_sel, preset_nb);
	if (ret)
		return ret;

	ret = ad413x_set_ref_buf(dev, preset.ref_buf, preset_nb);
	if (ret)
		return ret;

	ret = ad413x_set_settle_time(dev, preset.s_time, preset_nb);
	if (ret)
		return ret;

	ret = ad413x_set_exc_current(dev, preset.iout0_exc_current,
				     preset.iout1_exc_current, preset_nb);
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
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_init(struct ad413x_dev **device,
		    struct ad413x_init_param init_param)
{
	struct ad413x_dev *dev;
	uint32_t reg_data;
	int32_t ret;
	int32_t i;

	no_os_crc8_populate_msb(ad413x_crc8, AD413X_CRC8_POLY);

	dev = (struct ad413x_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	dev->chip_id = init_param.chip_id;
	dev->spi_crc_en = 0;

	/* SPI */
	ret = no_os_spi_init(&dev->spi_dev, init_param.spi_init);
	if (ret)
		goto err_dev;

	/* RDY pin */
	ret = no_os_gpio_get(&dev->rdy_pin_desc, init_param.rdy_pin_init);
	if (ret)
		goto err_dev;

	/* Enable the input direction of the specified GPIO. */
	ret = no_os_gpio_direction_input(dev->rdy_pin_desc);
	if (ret)
		goto err_dev;

	/* Device Settings */
	ret = ad413x_do_soft_reset(dev);
	if (ret)
		goto err_spi;

	/* Reset POR flag */
	ret = ad413x_reg_read(dev, AD413X_REG_STATUS, &reg_data);
	if (ret)
		return -1;

	/* Change SPI to 4 wire*/
	ret = ad413x_reg_write_msk(dev,
				   AD413X_REG_ADC_CTRL,
				   AD413X_ADC_CSB_EN,
				   AD413X_ADC_CSB_EN);
	if (ret)
		goto err_spi;

	ret = ad413x_reg_read(dev, AD413X_REG_ID, &reg_data);
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

	if (init_param.spi_crc_en) {
		ret = ad413x_reg_write_msk(dev,
					   AD413X_REG_ERROR_EN,
					   AD413X_SPI_CRC_ERR_EN,
					   AD413X_SPI_CRC_ERR_EN);
		if (ret)
			goto err_spi;
		dev->spi_crc_en = init_param.spi_crc_en;
	}

	/* Preset configured and saved in dev */
	for (i = 0; i < 8; i++) {
		ret = ad413x_preset_store(dev, init_param.preset[i], i);
		if (ret)
			goto err_spi;
	}

	/* Channel setup */
	for (i = 0; i < 16; i++) {
		ret = ad413x_set_ch_preset(dev, i, init_param.ch[i].preset);
		if (ret)
			goto err_spi;

		ret = ad413x_reg_write_msk(dev,
					   AD413X_REG_CHN(i),
					   AD413X_AINP_M(init_param.ch[i].ain_p),
					   AD413X_AINP_M(0xFF));
		if (ret)
			goto err_spi;
		dev->ch[i].ain_p = init_param.ch[i].ain_p;

		ret = ad413x_reg_write_msk(dev,
					   AD413X_REG_CHN(i),
					   AD413X_AINM_M(init_param.ch[i].ain_m),
					   AD413X_AINM_M(0xFF));
		if (ret)
			goto err_spi;
		dev->ch[i].ain_m = init_param.ch[i].ain_m;

		ret = ad413x_ch_exc_input(dev, i, init_param.ch[i].iout0_exc_input,
					  init_param.ch[i].iout1_exc_input);
		if (ret)
			goto err_spi;

		ret = ad413x_pdsw_en(dev, i, init_param.ch[i].pdsw_en);
		if (ret)
			goto err_spi;

		ret = ad413x_ch_en(dev, i, init_param.ch[i].enable);
		if (ret)
			goto err_spi;
	}

	ret = ad413x_set_int_ref(dev, init_param.int_ref);
	if (ret)
		goto err_spi;

	ret = ad413x_adc_bipolar(dev, init_param.bipolar);
	if (ret)
		goto err_spi;

	ret = ad413x_set_v_bias(dev, init_param.v_bias);
	if (ret)
		goto err_spi;

	ret = ad413x_set_standby_ctrl(dev, init_param.standby_ctrl);
	if (ret)
		goto err_spi;

	ret = ad413x_set_mclk(dev, init_param.mclk);
	if (ret)
		goto err_spi;

	ret = ad413x_data_stat_en(dev, init_param.data_stat);
	if (ret)
		goto err_spi;

	*device = dev;

	pr_info("AD413X successfully initialized\n");
	return 0;

err_spi:
	no_os_spi_remove(dev->spi_dev);
err_dev:
	no_os_free(dev);
	pr_err("AD413X initialization error (%d)\n", ret);
	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad413x_init().
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad413x_remove(struct ad413x_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_dev);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}
