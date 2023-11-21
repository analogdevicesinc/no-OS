/***************************************************************************//**
 *   @file   ad5754r.h
 *   @brief  Header file of AD5754R Driver.
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
#ifndef __AD5754R_H__
#define __AD5754R_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_util.h"
#include "no_os_error.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/* Maximum resolution */
#define AD5754R_MAX_RESOLUTION          	16

/* DAC Channel Count */
#define AD5754R_NUM_CHANNELS			4

/* Register Map */
#define AD5754R_REG_DAC				0
#define AD5754R_REG_OUTPUT_RANGE_SEL    	1
#define AD5754R_REG_PWR_CTRL			2
#define AD5754R_REG_CONTROL			3

/* DAC Channel Address */
#define AD5754R_DAC_CH_A_ADDR			0
#define AD5754R_DAC_CH_B_ADDR			1
#define AD5754R_DAC_CH_C_ADDR			2
#define AD5754R_DAC_CH_D_ADDR			3
#define AD5754R_DAC_CH_ALL_ADDR			4

/* Input Shift Register Masks */
#define AD5754R_ADDR_REG_MASK			NO_OS_GENMASK(5, 3)
#define AD5754R_ADDR_REG(x)			no_os_field_prep(AD5754R_ADDR_REG_MASK, x)
#define AD5754R_ADDR_DAC_CH_MASK		NO_OS_GENMASK(2, 0)
#define AD5754R_ADDR_DAC_CH(x)			no_os_field_prep(AD5754R_ADDR_DAC_CH_MASK, x)
#define AD5754R_PREP_INSTR_ADDR(reg, dac_ch)	(AD5754R_ADDR_REG(reg) | \
						AD5754R_ADDR_DAC_CH(dac_ch))

/* Output Range Select Mask */
#define AD5754R_OUTPUT_RANGE_SEL_MASK		NO_OS_GENMASK(2,0)

/* Control Register bit Definition */
#define AD5754R_CTRL_NOP			0
#define AD5754R_CTRL_TSD_EN(x)			no_os_field_prep(NO_OS_BIT(3), x)
#define AD5754R_CTRL_TSD_EN_MASK		NO_OS_BIT(3)
#define AD5754R_CTRL_CLAMP_EN(X)		no_os_field_prep(NO_OS_BIT(2), x)
#define AD5754R_CTRL_CLAMP_EN_MASK		NO_OS_BIT(2)
#define AD5754R_CTRL_CLR_SEL(x)			no_os_field_prep(NO_OS_BIT(1), x)
#define AD5754R_CTRL_CLR_SEL_MASK		NO_OS_BIT(1)
#define AD5754R_CTRL_SDO_DISABLE(x)		(x)
#define AD5754R_CTRL_SDO_DISABLE_MASK	  	NO_OS_BIT(0)
#define AD5754R_CTRL_CLEAR			0
#define AD5754R_CTRL_LOAD			0

/* Power Control Register bit definition */
#define AD5754R_PWR_UP_DAC_CH_MASK(x)		NO_OS_BIT(x)
#define AD5754R_PWR_UP_INT_REF_MASK		NO_OS_BIT(4)
#define AD5754R_PWR_OC_ALERT_MASK		NO_OS_GENMASK(10,7)
#define AD5754R_PWR_OC_ALERT_CH_MASK(x)		no_os_field_prep(AD5754R_PWR_OC_ALERT_MASK, \
						NO_OS_BIT(x))
#define AD5754R_PWR_TSD_ALERT_MASK		NO_OS_BIT(5)

#define AD5754R_BYTE_H				NO_OS_GENMASK(15, 8)
#define AD5754R_BYTE_L				NO_OS_GENMASK(7, 0)

#define AD5754R_READ				NO_OS_BIT(7)
#define AD5754R_WRITE				0

#define AD5754R_INSTR_NOP			0x18
#define AD5754R_INSTR_CLEAR			0x1C
#define AD5754R_INSTR_LOAD			0x1D

#define AD5754R_GAIN_SCALE      		1000

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum ad5754r_dac_channels
 * @brief DAC Channels
 */
enum ad5754r_dac_channels {
	AD5754R_DAC_CH_A,
	AD5754R_DAC_CH_B,
	AD5754R_DAC_CH_C,
	AD5754R_DAC_CH_D,
};

/**
 * @enum ad5754r_dac_ch_range
 * @brief DAC Channel Output Ranges
 */
enum ad5754r_dac_ch_range {
	AD5754R_SPAN_0V_TO_5V,
	AD5754R_SPAN_0V_TO_10V,
	AD5754R_SPAN_0V_TO_10V8,
	AD5754R_SPAN_M5V_TO_5V,
	AD5754R_SPAN_M10V_TO_10V,
	AD5754R_SPAN_M10V8_TO_10V8,
};

/**
 * @enum ad5754r_dac_addr_for_ctrl_settings
 * @brief DAC Addresses for Control Register Settings
 */
enum ad5754r_dac_addr_for_ctrl_settings {
	AD5754R_NOP_DAC_ADDR = 0,
	AD5754R_TSD_CLAMP_CLR_SDO_DAC_ADDR = 1,
	AD5754R_CLEAR_DAC_ADDR = 4,
	AD5754R_LOAD_DAC_ADDR = 5
};

/**
 * @enum ad5754r_ctrl_current_clamp_en
 * @brief Current Clamp EN/DIS Modes
 */
enum ad5754r_ctrl_current_clamp_en {
	AD5754R_CTRL_CLAMP_DIS,
	AD5754R_CTRL_CLAMP_EN
};

/**
 * @enum ad5754r_ctrl_tsd_en
 * @brief TSD EN/DIS Modes
 */
enum ad5754r_ctrl_tsd_en {
	AD5754R_CTRL_TSD_DIS,
	AD5754R_CTRL_TSD_EN
};

/**
 * @enum ad5754r_ctrl_clear_sel
 * @brief Clear Mode Options
 */
enum ad5754r_ctrl_clear_sel {
	AD5754R_CTRL_CLEAR_0V,
	AD5754R_CTRL_CLEAR_MIDSCALE_CODE
};

/**
 * @enum ad5754r_ctrl_sdo_dis
 * @brief SDO EN/DIS Modes
 */
enum ad5754r_ctrl_sdo_dis {
	AD5754R_CTRL_SDO_EN,
	AD5754R_CTRL_SDO_DIS
};

/**
 * @enum ad5754r_pwr_dac_ch_pwrup
 * @brief DAC Channel Powerup Modes
 */
enum ad5754r_pwr_dac_ch_pwrup {
	AD5754R_PWR_DAC_CH_POWERDOWN,
	AD5754R_PWR_DAC_CH_POWERUP
};

/**
 * @enum ad5754r_pwr_int_ref_pwrup
 * @brief Internal Reference Powerup Modes
 */
enum ad5754r_pwr_int_ref_pwrup {
	AD5754R_PWR_INT_REF_POWERDOWN,
	AD5754R_PWR_INT_REF_POWERUP
};

/**
 * @enum ad5754r_pwr_tsd_alert
 * @brief Thermal Shutdown Alert State
 */
enum ad5754r_pwr_tsd_alert {
	AD5754R_PWR_TSD_NOT_DETECTED,
	AD5754R_PWR_TSD_DETECTED
};

/**
 * @enum ad5754r_pwr_oc_ch_alert
 * @brief DAC Channel Overcurrent Alert State
 */
enum ad5754r_pwr_oc_ch_alert {
	AD5754R_PWR_OC_CH_NOT_DETECTED,
	AD5754R_PWR_OC_CH_DETECTED
};

/**
 * @enum ad5754r_encoding_scheme
 * @brief DAC encoding scheme: Binary/2sComplement
 */
enum ad5754r_encoding_scheme {
	AD5754R_ENCODING_TWOSCOMPLEMENT,
	AD5754R_ENCODING_BINARY
};

/**
 * @struct ad5754r_init_param
 * @brief ad5754r Device initialization parameters.
 */
struct ad5754r_init_param {
	/* SPI Init Parameters */
	struct no_os_spi_init_param	*spi_init;
	/* Clear GPIO Init Params*/
	struct no_os_gpio_init_param *gpio_clear_init;
	/* Load DAC Init Params*/
	struct no_os_gpio_init_param *gpio_ldac_init;
	/* Output Range Select */
	enum ad5754r_dac_ch_range dac_ch_range[AD5754R_NUM_CHANNELS];
	/* Current Clamp Enable */
	enum ad5754r_ctrl_current_clamp_en clamp_en;
	/* TSD Enable */
	enum ad5754r_ctrl_tsd_en tsd_en;
	/* Clear Setting */
	enum ad5754r_ctrl_clear_sel clear_sel;
	/* SDO Disable */
	enum ad5754r_ctrl_sdo_dis sdo_dis;
	/* Powerup States of DAC Channels */
	enum ad5754r_pwr_dac_ch_pwrup dac_ch_pwr_states[AD5754R_NUM_CHANNELS];
	/* Powerup State of Internal Reference */
	enum ad5754r_pwr_int_ref_pwrup int_ref_pwrup;
	/* Encoding scheme */
	enum ad5754r_encoding_scheme encoding;
	/* Reference voltage in millivolts */
	uint16_t vref_mv;
};

/**
 * @struct ad5754r_dev
 * @brief ad5754r Device structure.
 */
struct ad5754r_dev {
	/* SPI Descriptor */
	struct no_os_spi_desc	*spi_desc;
	/* Clear GPIO */
	struct no_os_gpio_desc *gpio_clear;
	/* Load DAC */
	struct no_os_gpio_desc *gpio_ldac;
	/* Output Range Select */
	enum ad5754r_dac_ch_range dac_ch_range[AD5754R_NUM_CHANNELS];
	/* Current Clamp Enable */
	enum ad5754r_ctrl_current_clamp_en clamp_en;
	/* TSD Enable */
	enum ad5754r_ctrl_tsd_en tsd_en;
	/* Clear Setting */
	enum ad5754r_ctrl_clear_sel clear_sel;
	/* SDO Disable */
	enum ad5754r_ctrl_sdo_dis sdo_dis;
	/* Powerup States of DAC Channels */
	enum ad5754r_pwr_dac_ch_pwrup dac_ch_pwr_states[AD5754R_NUM_CHANNELS];
	/* Powerup State of Internal Reference */
	enum ad5754r_pwr_int_ref_pwrup int_ref_pwrup;
	/* Encoding scheme */
	enum ad5754r_encoding_scheme encoding;
	/* Reference voltage in millivolts */
	uint16_t vref_mv;
};

/******************************************************************************/
/********************** Variables and User Defined Data Types *****************/
/******************************************************************************/

/* AD5754R gain values for different output ranges */
extern const unsigned int ad5754r_gain_values_scaled[AD5754R_SPAN_M10V8_TO_10V8
		+ 1];

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/** Writes data into a register.  */
int ad5754r_write(struct ad5754r_dev *dev, uint8_t instr_addr,
		  uint16_t reg_val);

/** Reads data from a register. */
int ad5754r_read(struct ad5754r_dev *dev, uint8_t instr_addr,
		 uint16_t *reg_val);

/** Update specific register bits */
int ad5754r_update_bits(struct ad5754r_dev *dev,
			uint8_t instr_addr,
			uint16_t mask,
			uint16_t reg_val);

/** Update DAC register value for specific channel. */
int ad5754r_update_dac_ch_register(struct ad5754r_dev *dev, uint8_t chn,
				   uint16_t value);

/** Update DAC register value for all channels.  */
int ad5754r_update_dac_all_ch_registers(struct ad5754r_dev *dev,
					uint16_t value);

/** @brief Trigger LDAC. */
int ad5754r_ldac_trigger(struct ad5754r_dev *dev);

/** Clear DAC output for all channels. */
int ad5754r_clear_async(struct ad5754r_dev *dev);

/** Read DAC register value for specific channel. */
int ad5754r_read_dac_ch_register(struct ad5754r_dev *dev, uint8_t chn,
				 uint16_t *value);

/** Convert DAC code to millivolts for specific channel. */
int ad5754r_dac_code_to_mvolts(struct ad5754r_dev *dev, uint8_t chn,
			       uint16_t code,
			       uint16_t *value);

/** Convert DAC millivolts to code for specific channel. */
int ad5754r_dac_mvolts_to_code(struct ad5754r_dev *dev, uint8_t chn,
			       uint16_t mvolts, uint16_t *code);

/** Set output range for specific channel. */
int ad5754r_set_ch_range(struct ad5754r_dev *dev, uint8_t chn,
			 enum ad5754r_dac_ch_range ch_range);

/** Set power-up state for specific channel. */
int ad5754r_set_ch_pwrup(struct ad5754r_dev *dev, uint8_t chn,
			 enum ad5754r_pwr_dac_ch_pwrup ch_pwrup);

/** Apply current clamp setting for device. */
int ad5754r_set_current_clamp_en(struct ad5754r_dev *dev,
				 enum ad5754r_ctrl_current_clamp_en clamp_en);

/** Apply tsd setting for device. */
int ad5754r_set_tsd_en(struct ad5754r_dev *dev,
		       enum ad5754r_ctrl_tsd_en tsd_en);

/** Set clear mode setting for device. */
int ad5754r_set_clear_mode(struct ad5754r_dev *dev,
			   enum ad5754r_ctrl_clear_sel clear_sel);

/** Set SDO Enable/Disable state for device. */
int ad5754r_set_sdo_disable(struct ad5754r_dev *dev,
			    enum ad5754r_ctrl_sdo_dis sdo_dis);

/** Set Internal Reference Enable/Disable state for device. */
int ad5754r_set_int_ref_pwrup(struct ad5754r_dev *dev,
			      enum ad5754r_pwr_int_ref_pwrup int_ref_pwrup);

/** Get Over-current status bit for specific channel, */
int ad5754r_get_oc_ch_alert(struct ad5754r_dev *dev, uint8_t chn,
			    enum ad5754r_pwr_oc_ch_alert *oc_ch_alert);

/** Get Thermal Shutdown status bit. */
int ad5754r_get_tsd_alert(struct ad5754r_dev *dev,
			  enum ad5754r_pwr_tsd_alert *tsd_alert);

/** Update and readback a register to verify SPI. */
int ad5754r_spi_verify(struct ad5754r_dev* dev);

/** Do GPIO initialization and config for AD5754R */
int ad5754r_gpio_init(struct ad5754r_dev* dev,
		      struct ad5754r_init_param *init_param);

/** Initialize registers based on init parameters */
int ad5754r_reg_init(struct ad5754r_dev* dev,
		     struct ad5754r_init_param *init_param);

/** Initializes the communication with the device. */
int ad5754r_init(struct ad5754r_dev **device,
		 struct ad5754r_init_param *init_param);

/** Remove GPIOs and free resources. */
int ad5754r_remove_gpios(struct ad5754r_dev *dev);

/** Free the resources allocated by ad5754r_init(). */
int ad5754r_remove(struct ad5754r_dev *dev);

#endif /* __AD5754R_H__ */
