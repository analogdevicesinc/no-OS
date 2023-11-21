/***************************************************************************//**
 *   @file   ad5754r.c
 *   @brief  Implementation of AD5754R Driver.
 *   @author Ribhu DasPurkayastha (Ribhu.DasPurkayastha@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
 *
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include "ad5754r.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"

/******************************************************************************/
/******************** Variables and User Defined Data Types *******************/
/******************************************************************************/

/* AD5754R gain values scaled up to avoid float computations */
const unsigned int ad5754r_gain_values_scaled[AD5754R_SPAN_M10V8_TO_10V8 + 1] =
{
	2000,
	4000,
	4320,
	4000,
	8000,
	8640
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/******************************************************************************/

/**
 * @brief Write device register.
 * @param dev- The device structure.
 * @param instr_addr - The instruction address comprising register and
 *				dac channel address.
 * @param reg_val - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5754r_write(struct ad5754r_dev *dev,
		  uint8_t instr_addr,
		  uint16_t reg_val)
{
	uint8_t buff[3];

	if (!dev)
		return -EINVAL;

	/* Instruction frame format - {R/Wb, 0, R2, R1, R0, A2, A1, A0}
	 * R2, R1, R0 : Register address bits
	 * D2, D1, D0 : DAC address bits */
	buff[0] = AD5754R_WRITE | instr_addr;
	buff[1] = no_os_field_get(AD5754R_BYTE_H, reg_val);
	buff[2] = no_os_field_get(AD5754R_BYTE_L, reg_val);

	return no_os_spi_write_and_read(dev->spi_desc, buff, sizeof(buff));
}

/**
 * @brief Read device register.
 * @param dev - The device structure.
 * @param instr_addr - The instruction address comprising register and
 *				dac channel address.
 * @param reg_val - The data read from the register.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5754r_read(struct ad5754r_dev *dev,
		 uint8_t instr_addr,
		 uint16_t *reg_val)
{
	int ret;
	uint8_t buff[3];

	if (!dev || !reg_val)
		return -EINVAL;

	/* Instruction frame format - {R/Wb, 0, R2, R1, R0, A2, A1, A0}
	 * R2, R1, R0 : Register address bits
	 * D2, D1, D0 : DAC address bits */
	buff[0] = AD5754R_READ | instr_addr;
	buff[1] = 0;
	buff[2] = 0;
	ret = no_os_spi_write_and_read(dev->spi_desc, buff, sizeof(buff));
	if (ret)
		return ret;

	/* Perform NOP */
	buff[0] = AD5754R_INSTR_NOP;
	buff[1] = 0;
	buff[2] = 0;
	ret = no_os_spi_write_and_read(dev->spi_desc, buff, sizeof(buff));
	if (ret)
		return ret;

	*reg_val = no_os_get_unaligned_be16(&buff[1]);

	return ret;
}

/**
 * @brief Update specific register bits.
 * @param dev - The device structure.
 * @param instr_addr - The instruction address comprising register and
 *				dac channel address.
 * @param mask - Specific bits mask.
 * @param reg_val - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5754r_update_bits(struct ad5754r_dev *dev,
			uint8_t instr_addr,
			uint16_t mask,
			uint16_t reg_val)
{
	int ret;
	uint16_t reg_data;

	ret = ad5754r_read(dev, instr_addr, &reg_data);
	if (ret)
		return ret;

	reg_data &= ~mask;
	reg_data |= no_os_field_prep(mask, reg_val);

	return ad5754r_write(dev, instr_addr, reg_data);
}

/**
 * @brief Update DAC register value for specific channel.
 * @param dev - The device structure.
 * @param chn - The index of channel being addressed.
 * @param value - The data to be written to the dac channel.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5754r_update_dac_ch_register(struct ad5754r_dev *dev,
				   uint8_t chn,
				   uint16_t value)
{
	int ret;

	if (!dev || chn > AD5754R_NUM_CHANNELS)
		return -EINVAL;

	ret = no_os_gpio_set_value(dev->gpio_ldac, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	ret = ad5754r_write(dev, AD5754R_PREP_INSTR_ADDR(AD5754R_REG_DAC, chn),
			    value);
	if (ret)
		return ret;

	return no_os_gpio_set_value(dev->gpio_ldac, NO_OS_GPIO_HIGH);
}

/**
 * @brief Update DAC register value for all channels.
 * @param dev - The device structure.
 * @param value - The data to be written to the dac channel registers.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5754r_update_dac_all_ch_registers(struct ad5754r_dev *dev,
					uint16_t value)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = no_os_gpio_set_value(dev->gpio_ldac, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	ret = ad5754r_write(dev,
			    AD5754R_PREP_INSTR_ADDR(AD5754R_REG_DAC,
					    AD5754R_DAC_CH_ALL_ADDR),
			    value);
	if (ret)
		return ret;

	return no_os_gpio_set_value(dev->gpio_ldac, NO_OS_GPIO_HIGH);
}

/**
 * @brief Trigger LDAC.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5754r_ldac_trigger(struct ad5754r_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	if (dev->gpio_ldac) {
		ret = no_os_gpio_set_value(dev->gpio_ldac, NO_OS_GPIO_LOW);
		if (ret)
			return ret;

		/* Delay must be greater than 20ns, per the datasheet. */
		no_os_udelay(1);

		return no_os_gpio_set_value(dev->gpio_ldac, NO_OS_GPIO_HIGH);
	}

	/* If no gpio is assigned use SW LOAD */
	return ad5754r_write(dev, AD5754R_INSTR_LOAD, 0x0000);
}

/**
 * @brief Clear DAC output for all channels.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5754r_clear_async(struct ad5754r_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	if (dev->gpio_clear) {
		ret = no_os_gpio_set_value(dev->gpio_clear, NO_OS_GPIO_LOW);
		if (ret)
			return ret;

		return no_os_gpio_set_value(dev->gpio_clear, NO_OS_GPIO_HIGH);
	}

	/* If no gpio is assigned use SW CLEAR */
	return ad5754r_write(dev, AD5754R_INSTR_CLEAR, 0x0000);
}

/**
 * @brief Read DAC register value for specific channel.
 * @param dev - The device structure.
 * @param chn - The index of channel being addressed.
 * @param value - The data read from dac channel register.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5754r_read_dac_ch_register(struct ad5754r_dev *dev,
				 uint8_t chn,
				 uint16_t *value)
{
	if (!value || (chn >= AD5754R_NUM_CHANNELS))
		return -EINVAL;

	return ad5754r_read(dev,
			    AD5754R_PREP_INSTR_ADDR(AD5754R_REG_DAC, chn),
			    (uint16_t *)value);
}

/**
 * @brief Convert DAC code to millivolts for specific channel.
 * @param dev - The device structure.
 * @param chn - The index of channel being addressed.
 * @param code - DAC code
 * @param value - Millivolts output calculated using DAC code.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5754r_dac_code_to_mvolts(struct ad5754r_dev *dev, uint8_t chn,
			       uint16_t code, uint16_t *value)
{
	if (!dev || !value || (chn >= AD5754R_NUM_CHANNELS))
		return -EINVAL;

	/* Based on encoding, calculate millivolts */
	if (dev->dac_ch_range[chn] < AD5754R_SPAN_M5V_TO_5V) {
		/* For unipolar output range, encoding is straight binary */
		*value = (ad5754r_gain_values_scaled[dev->dac_ch_range[chn]] *
			  dev->vref_mv * code) / (1 << AD5754R_MAX_RESOLUTION);

		/* Compensate for the scaled-up gain value */
		*value /= AD5754R_GAIN_SCALE;
	} else {
		if (dev->encoding == AD5754R_ENCODING_BINARY) {
			code -= 1 << (AD5754R_MAX_RESOLUTION - 1);
		} else {
			if (code >= (1 << (AD5754R_MAX_RESOLUTION - 1))) {
				code = (1 << AD5754R_MAX_RESOLUTION) - code;
			}
		}

		*value = (ad5754r_gain_values_scaled[dev->dac_ch_range[chn]] *
			  dev->vref_mv * code) / (1 << AD5754R_MAX_RESOLUTION);

		/* Compensate for the scaled-up gain value */
		*value /= AD5754R_GAIN_SCALE;
	}

	return 0;
}

/**
 * @brief Convert DAC millivolts to code for specific channel.
 * @param dev - The device structure.
 * @param chn - The index of channel being addressed.
 * @param mvolts - Desired millivolts ouput for specific channel
 * @param code - DAC code for desired output voltage
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5754r_dac_mvolts_to_code(struct ad5754r_dev *dev,
			       uint8_t chn,
			       uint16_t mvolts,
			       uint16_t *code)
{
	if (!dev || !code || (chn >= AD5754R_NUM_CHANNELS))
		return -EINVAL;

	/* Based on encoding, calculate code and compensate for the scaled gain */
	if (dev->dac_ch_range[chn] < AD5754R_SPAN_M5V_TO_5V) {
		/* For unipolar output range, encoding is straight binary */
		*code = ((1 << AD5754R_MAX_RESOLUTION) * mvolts * AD5754R_GAIN_SCALE) /
			(dev->vref_mv * ad5754r_gain_values_scaled[dev->dac_ch_range[chn]]);
	} else {
		*code = ((1 << AD5754R_MAX_RESOLUTION) * mvolts * AD5754R_GAIN_SCALE) /
			(ad5754r_gain_values_scaled[dev->dac_ch_range[chn]] * dev->vref_mv);

		if (dev->encoding == AD5754R_ENCODING_BINARY) {
			*code += 1 << (AD5754R_MAX_RESOLUTION - 1);
		} else {
			if (*code >= (1 << (AD5754R_MAX_RESOLUTION - 1))) {
				*code -= 1 << AD5754R_MAX_RESOLUTION ;
			}
		}
	}

	return 0;
}

/**
 * @brief Set output range for specific channel.
 * @param dev - The device structure.
 * @param chn - The index of channel being addressed.
 * @param ch_range - Range option to be set for specified channel.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5754r_set_ch_range(struct ad5754r_dev *dev, uint8_t chn,
			 enum ad5754r_dac_ch_range ch_range)
{
	int ret;

	if (chn > AD5754R_NUM_CHANNELS || (ch_range > AD5754R_SPAN_M10V8_TO_10V8))
		return -EINVAL;

	ret = ad5754r_write(dev,
			    AD5754R_PREP_INSTR_ADDR(AD5754R_REG_OUTPUT_RANGE_SEL, chn),
			    ch_range);
	if (ret)
		return ret;

	dev->dac_ch_range[chn] = ch_range;

	return 0;
}

/**
 * @brief Set power-up state for specific channel.
 * @param dev - The device structure.
 * @param chn - The index of channel being addressed.
 * @param ch_pwrup - Powerup setting for specified channel.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5754r_set_ch_pwrup(struct ad5754r_dev *dev, uint8_t chn,
			 enum ad5754r_pwr_dac_ch_pwrup ch_pwrup)
{
	int ret;

	if ((chn >= AD5754R_NUM_CHANNELS) ||
	    (ch_pwrup > AD5754R_PWR_DAC_CH_POWERUP))
		return -EINVAL;

	// TODO Confirm whether the DAC channel address
	// is don't care, or 0

	ret = ad5754r_update_bits(dev,
				  AD5754R_PREP_INSTR_ADDR(AD5754R_REG_PWR_CTRL, 0),
				  AD5754R_PWR_UP_DAC_CH_MASK(chn),
				  ch_pwrup);
	if (ret)
		return ret;

	dev->dac_ch_pwr_states[chn] = ch_pwrup;

	return 0;
}

/**
 * @brief Apply current clamp setting for device.
 * @param dev - The device structure.
 * @param clamp_en - Clamp setting for device.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5754r_set_current_clamp_en(struct ad5754r_dev *dev,
				 enum ad5754r_ctrl_current_clamp_en clamp_en)
{
	int ret;

	if (clamp_en > AD5754R_CTRL_CLAMP_EN)
		return -EINVAL;

	ret = ad5754r_update_bits(dev,
				  AD5754R_PREP_INSTR_ADDR(AD5754R_REG_CONTROL,
						  AD5754R_TSD_CLAMP_CLR_SDO_DAC_ADDR),
				  AD5754R_CTRL_CLAMP_EN_MASK,
				  clamp_en);
	if (ret)
		return ret;

	dev->clamp_en = clamp_en;

	return 0;
}

/**
 * @brief Apply TSD setting for device.
 * @param dev - The device structure.
 * @param tsd_en - TSD setting for device.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5754r_set_tsd_en(struct ad5754r_dev *dev,
		       enum ad5754r_ctrl_tsd_en tsd_en)
{
	int ret;

	if (tsd_en > AD5754R_CTRL_TSD_EN)
		return -EINVAL;

	ret = ad5754r_update_bits(dev,
				  AD5754R_PREP_INSTR_ADDR(AD5754R_REG_CONTROL,
						  AD5754R_TSD_CLAMP_CLR_SDO_DAC_ADDR),
				  AD5754R_CTRL_TSD_EN_MASK,
				  tsd_en);
	if (ret)
		return ret;

	dev->tsd_en = tsd_en;

	return 0;
}

/**
 * @brief Set clear mode setting for device.
 * @param dev - The device structure.
 * @param clear_sel - Clear mode setting for device.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5754r_set_clear_mode(struct ad5754r_dev *dev,
			   enum ad5754r_ctrl_clear_sel clear_sel)
{
	int ret;

	if (clear_sel > AD5754R_CTRL_CLEAR_MIDSCALE_CODE)
		return -EINVAL;

	ret = ad5754r_update_bits(dev,
				  AD5754R_PREP_INSTR_ADDR(AD5754R_REG_CONTROL,
						  AD5754R_TSD_CLAMP_CLR_SDO_DAC_ADDR),
				  AD5754R_CTRL_CLR_SEL_MASK,
				  clear_sel);
	if (ret)
		return ret;

	dev->clear_sel = clear_sel;

	return 0;
}

/**
 * @brief Set SDO Enable/Disable state for device.
 * @param dev - The device structure.
 * @param sdo_dis - SDO Enable/Disable state for device.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5754r_set_sdo_disable(struct ad5754r_dev *dev,
			    enum ad5754r_ctrl_sdo_dis sdo_dis)
{
	int ret;

	if (sdo_dis > AD5754R_CTRL_SDO_DIS)
		return -EINVAL;

	ret = ad5754r_update_bits(dev,
				  AD5754R_PREP_INSTR_ADDR(AD5754R_REG_CONTROL,
						  AD5754R_TSD_CLAMP_CLR_SDO_DAC_ADDR),
				  AD5754R_CTRL_SDO_DISABLE_MASK,
				  sdo_dis);
	if (ret)
		return ret;

	dev->sdo_dis = sdo_dis;

	return 0;
}

/**
 * @brief Set Internal Reference Enable/Disable state for device.
 * @param dev - The device structure.
 * @param int_ref_pwrup - Internal Reference Enable/Disable state for device.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5754r_set_int_ref_pwrup(struct ad5754r_dev *dev,
			      enum ad5754r_pwr_int_ref_pwrup int_ref_pwrup)
{
	int ret;

	if (int_ref_pwrup > AD5754R_PWR_INT_REF_POWERUP)
		return -EINVAL;

	// TODO Confirm with apps on whether the DAC channel address
	// is don't care, or 0

	ret = ad5754r_update_bits(dev,
				  AD5754R_PREP_INSTR_ADDR(AD5754R_REG_PWR_CTRL, 0),
				  AD5754R_PWR_UP_INT_REF_MASK,
				  int_ref_pwrup);
	if (ret)
		return ret;

	dev->int_ref_pwrup = int_ref_pwrup;

	return 0;
}

/**
 * @brief Get Over-current status bit for specific channel.
 * @param dev - The device structure.
 * @param chn - The index of channel being addressed.
 * @param oc_ch_alert - Store channel overcurrent alert status.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5754r_get_oc_ch_alert(struct ad5754r_dev *dev, uint8_t chn,
			    enum ad5754r_pwr_oc_ch_alert *oc_ch_alert)
{
	int ret;
	uint16_t reg_val;

	if (!oc_ch_alert || (chn >= AD5754R_NUM_CHANNELS))
		return -EINVAL;

	// TODO Confirm whether the DAC channel address
	// is don't care, or 0

	ret = ad5754r_read(dev, AD5754R_PREP_INSTR_ADDR(AD5754R_REG_PWR_CTRL, 0),
			   &reg_val);
	if (ret)
		return ret;

	*oc_ch_alert = no_os_field_get(AD5754R_PWR_OC_ALERT_CH_MASK(chn), reg_val);

	return 0;
}

/**
 * @brief Get Thermal Shutdown status bit.
 * @param dev - The device structure.
 * @param tsd_alert - Store TSD alert status.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5754r_get_tsd_alert(struct ad5754r_dev *dev,
			  enum ad5754r_pwr_tsd_alert *tsd_alert)
{
	int ret;
	uint16_t reg_val;

	if (!tsd_alert)
		return -EINVAL;

	// TODO Confirm whether the DAC channel address
	// is don't care, or 0

	ret = ad5754r_read(dev,
			   AD5754R_PREP_INSTR_ADDR(AD5754R_REG_PWR_CTRL, 0),
			   &reg_val);
	if (ret)
		return ret;

	*tsd_alert = no_os_field_get(AD5754R_PWR_TSD_ALERT_MASK, reg_val);

	return 0;
}

/**
 * @brief Write to and read back a register to verify SPI.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5754r_spi_verify(struct ad5754r_dev *dev)
{
	int ret;
	uint16_t reg_data;
	uint8_t instr_addr;

	if (!dev)
		return -EINVAL;

	/* Use Output Range Select register for DAC Channel A, to verify SPI */
	instr_addr = AD5754R_PREP_INSTR_ADDR(AD5754R_REG_OUTPUT_RANGE_SEL,
					     AD5754R_DAC_CH_A_ADDR);

	/* Change Channel A output range to 0-10V */
	ret = ad5754r_set_ch_range(dev, AD5754R_DAC_CH_A, AD5754R_SPAN_0V_TO_10V);
	if (ret)
		return ret;

	/* Read channel A output range */
	ret = ad5754r_read(dev, instr_addr, &reg_data);
	if (ret)
		return ret;

	/* Confirm if readback value matches what was written */
	if (no_os_field_get(AD5754R_OUTPUT_RANGE_SEL_MASK,
			    reg_data) != AD5754R_SPAN_0V_TO_10V)
		return -EBADMSG;

	/* Restore Channel A output range to 0-5V */
	return ad5754r_set_ch_range(dev, AD5754R_DAC_CH_A, AD5754R_SPAN_0V_TO_5V);
}

/**
 * @brief Do GPIO initialization and config for AD5754R.
 * @param dev - The device structure.
 * @param init_param - Pointer to structure containing the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5754r_gpio_init(struct ad5754r_dev* dev,
		      struct ad5754r_init_param *init_param)
{
	int ret;

	if (!dev || !init_param)
		return -EINVAL;

	/* GPIO Clear Initialization */
	ret = no_os_gpio_get_optional(&dev->gpio_clear,
				      init_param->gpio_clear_init);
	if (ret)
		return ret;

	if (dev->gpio_clear) {
		ret = no_os_gpio_direction_output(dev->gpio_clear,
						  NO_OS_GPIO_HIGH);
		if (ret)
			return ret;
	}

	/* GPIO LDAC Initialization */
	ret = no_os_gpio_get_optional(&dev->gpio_ldac, init_param->gpio_ldac_init);
	if (ret)
		return ret;

	if (dev->gpio_ldac)
		return no_os_gpio_direction_output(dev->gpio_ldac,
						   NO_OS_GPIO_HIGH);

	return 0;
}

/**
 * @brief Initialize registers based on init parameters.
 * @param dev - The device structure.
 * @param init_param - Pointer to structure containing the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5754r_reg_init(struct ad5754r_dev* dev,
		     struct ad5754r_init_param *init_param)
{
	int ret;
	uint8_t chn;

	if (!init_param)
		return -EINVAL;

	for (chn = 0; chn < AD5754R_NUM_CHANNELS; chn++) {
		/* DAC Channel Range Select */
		ret = ad5754r_set_ch_range(dev, chn, init_param->dac_ch_range[chn]);
		if (ret)
			return ret;

		/* DAC Channel Powerup */
		ret = ad5754r_set_ch_pwrup(dev, chn,
					   init_param->dac_ch_pwr_states[chn]);
		if (ret)
			return ret;
	}

	/* Current Clamp Enable */
	ret = ad5754r_set_current_clamp_en(dev, init_param->clamp_en);
	if (ret)
		return ret;

	/* TSD Enable */
	ret = ad5754r_set_tsd_en(dev, init_param->tsd_en);
	if (ret)
		return ret;

	/* Clear Mode Setting */
	ret = ad5754r_set_clear_mode(dev, init_param->clear_sel);
	if (ret)
		return ret;

	/* SDO Disable */
	ret = ad5754r_set_sdo_disable(dev, init_param->sdo_dis);
	if (ret)
		return ret;

	/* Internal Reference Powerup */  // TODO figure out where to do this
	return ad5754r_set_int_ref_pwrup(dev, init_param->int_ref_pwrup);
}

/**
 * @brief Initialize the device.
 * @param device - The device structure.
 * @param init_param - Pointer to structure containing the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5754r_init(struct ad5754r_dev **device,
		 struct ad5754r_init_param *init_param)
{
	struct ad5754r_dev *dev;
	int ret;

	if (!device || !init_param)
		return -EINVAL;

	dev = (struct ad5754r_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* GPIO Initialization */
	ret = ad5754r_gpio_init(dev, init_param);
	if (ret)
		goto error_dev;

	/* SPI Initialization */
	ret = no_os_spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret)
		goto error_gpio;

	/* Verify whether SPI is working by writing to a register
	 * and reading it back */
	ret = ad5754r_spi_verify(dev);
	if (ret)
		goto error_spi;

	/* Initialize device registers */
	ret = ad5754r_reg_init(dev, init_param);
	if (ret)
		goto error_spi;

	/* Encoding */
	dev->encoding = init_param->encoding;

	/* Reference voltage */
	dev->vref_mv = init_param->vref_mv;

	*device = dev;

	return 0;

error_spi:
	no_os_spi_remove(dev->spi_desc);
error_gpio:
	ad5754r_remove_gpios(dev);
error_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Remove GPIOs and free resources.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5754r_remove_gpios(struct ad5754r_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	if (dev->gpio_clear) {
		ret = no_os_gpio_remove(dev->gpio_clear);
		if (ret)
			return ret;
	}

	if (dev->gpio_ldac) {
		ret = no_os_gpio_remove(dev->gpio_ldac);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Remove the device and release resources.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5754r_remove(struct ad5754r_dev *dev)
{
	int ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	ret = ad5754r_remove_gpios(dev);
	if (ret)
		return ret;

	no_os_free(dev);

	return ret;
}
