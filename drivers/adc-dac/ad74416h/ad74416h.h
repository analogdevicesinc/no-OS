/***************************************************************************//**
 *   @file   ad74416h.h
 *   @brief  Header file of AD74416h Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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
*******************************************************************************/
#ifndef _AD74416H_H
#define _AD74416H_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "stdint.h"
#include "stdbool.h"
#include "no_os_spi.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define AD74416H_N_CHANNELS             4

#define AD74416H_CH_A                   0
#define AD74416H_CH_B                   1
#define AD74416H_CH_C                   2
#define AD74416H_CH_D                   3

/** The value of the sense resistor in ohms */
#define AD74416H_RSENSE                 12
/** 16 bit ADC */
#define AD74416H_ADC_MAX_VALUE		16777215

/** Register map */
#define AD74416H_NOP				0x00
#define AD74416H_CH_FUNC_SETUP(x)               (0x01 + (x * 12))
#define AD74416H_ADC_CONFIG(x)                  (0x02 + (x * 12))
#define AD74416H_DIN_CONFIG0(x)			(0x03 + (x * 12))
#define AD74416H_DIN_CONFIG1(x)			(0x04 + (x * 12))
#define AD74416H_OUTPUT_CONFIG(x)               (0x05 + (x * 12))
#define AD74416H_RTD_CONFIG(x)			(0x06 + (x * 12))
#define AD74416H_FET_LKG_COMP(x)		(0x07 + (x * 12))
#define AD74416H_DO_EXT_CONFIG(x)		(0x08 + (x * 12))
#define AD74416H_I_BURNOUT_CONFIG(x)		(0x09 + (x * 12))
#define AD74416H_DAC_CODE(x)			(0x0A + (x * 12))
#define AD74416H_DAC_ACTIVE(x)			(0x0C + (x * 12))
#define AD74416H_GPIO_CONFIG(x)			(0x32 + x)
#define AD74416H_PWR_OPTIM_CONFIG		0x38
#define AD74416H_ADC_CONV_CTRL			0x39
#define AD74416H_DIAG_ASSIGN			0x3A
#define AD74416H_WTD_CONFIG			0x3B
#define AD74416H_DIN_COMP_OUT			0x3E
#define AD74416H_ALERT_STATUS			0x3F
#define AD74416H_LIVE_STATUS			0x40
#define AD74416H_ADC_RESULT_UPR(x)		(0x41 + (x * 2))
#define AD74416H_ADC_RESULT(x)			(0x42 + (x * 2))
#define AD74416H_ADC_DIAG_RESULT(x)		(0x49 + x)
#define AD74416H_LAST_ADC_RESULT_UPR		0x4D
#define AD74416H_LAST_ADC_RESULT		0x4E
#define AD74416H_DIN_COUNTER(x)			(0x50 + (x * 2))
#define AD74416H_SUPPLY_ALERT_STATUS		0x57
#define AD74416H_CHANNEL_ALERT_STATUS(x)	(0x58 + x)
#define AD74416H_ALERT_MASK			0x5C
#define AD74416H_SUPPLY_ALERT_MASK		0x5D
#define AD74416H_CHANNEL_ALERT_MASK(x)		(0x5E + x)
#define AD74416H_READ_SELECT			0x6E
#define AD74416H_BURST_READ_SEL			0x6F
#define AD74416H_THERM_RST			0x73
#define AD74416H_CMD_KEY			0x74
#define AD74416H_BORADCAST_CMD_KEY		0x75
#define AD74416H_SCRATCH(x)			(0x76 + x)
#define AD74416H_GENERIC_ID			0x7A
#define AD74416H_SILICON_REV			0x7B
#define AD74416H_SILICON_ID0			0x7D
#define AD74416H_SILICON_ID1			0x7E
#define AD74416H_HART_ALERT_STATUS(x)		(0x80 + (x * 16))
#define AD74416H_HART_RX(x)			(0x81 + (x * 16))
#define AD74416H_HART_TX(x)			(0x82 + (x * 16))
#define AD74416H_HART_FCR(x)			(0x83 + (x * 16))
#define AD74416H_HART_MCR(x)			(0x84 + (x * 16))
#define AD74416H_HART_RFC(x)			(0x85 + (x * 16))
#define AD74416H_HART_TFC(x)			(0x86 + (x * 16))
#define AD74416H_HART_ALERT_MASK(x)		(0x87 + (x * 16))
#define AD74416H_HART_CONFIG(x)			(0x88 + (x * 16))
#define AD74416H_HART_TX_PREM(x)		(0x89 + (x * 16))
#define AD74416H_HART_EVDET(x)			(0x8A + (x * 16))
#define AD74416H_HART_TX_GAIN(x)		(0x8B + (x * 16))
#define AD74416H_HART_GPIO_IF_CONFIG		0xC0
#define AD74416H_HART_GPIO_MON_CONFIG(x)	(0xC1 + x)

/** Software reset sequence */
#define AD74416H_CMD_KEY_RESET_1                0x15FA
#define AD74416H_CMD_KEY_RESET_2                0xAF51

#define AD74416H_SPI_RD_RET_INFO_MSK		NO_OS_BIT(8)
#define AD74416H_ERR_CLR_MSK			NO_OS_GENMASK(15, 0)
#define AD74416H_SPI_CRC_ERR_MSK		NO_OS_BIT(13)

/* AD74416H_CH_FUNC_SETUP */
#define AD74416H_CH_FUNC_SETUP_MSK		NO_OS_GENMASK(3, 0)

/* AD74416H_ADC_CONFIG */
#define AD74416H_ADC_CONV_RATE_MSK		NO_OS_GENMASK(10, 8)
#define AD74416H_ADC_CONV_RANGE_MSK		NO_OS_GENMASK(6, 4)
#define AD74416H_CONV_MUX_MSK			NO_OS_GENMASK(2, 0)

/* AD74416H_DIN_CONFIG0 */
#define AD74416H_COUNT_EN_MSK			NO_OS_BIT(15)
#define AD74416H_DIN_INV_COMP_OUT_MSK		NO_OS_BIT(14)
#define AD74416H_COMPARATOR_EN_MSK		NO_OS_BIT(13)
#define AD74416H_DIN_SINK_RANGE_MSK		NO_OS_BIT(12)
#define AD74416H_DIN_SINK_MSK			NO_OS_GENMASK(11, 7)
#define AD74416H_DEBOUNCE_MODE_MSK		NO_OS_BIT(6)
#define AD74416H_DEBOUNCE_TIME_MSK		NO_OS_GENMASK(4, 0)

/** ADC_CONV_CTRL register */
#define AD74416H_CONV_SEQ_MSK                  NO_OS_GENMASK(9, 8)
#define AD74416H_DIAG_EN_MSK(x)		       (NO_OS_BIT(x) << 4)
#define AD74416H_CH_EN_MSK(x)                  NO_OS_BIT(x)

/** DIAG_ASSIGN register */
#define AD74416H_DIAG_ASSIGN_MSK(x)		(NO_OS_GENMASK(3, 0) << (x))

/** GPIO_CONFIGx register */
#define AD74416H_GPIO_DATA_MSK			NO_OS_BIT(4)
#define AD74416H_GPIO_SELECT_MSK		NO_OS_GENMASK(2, 0)

/** OUTPUT_CONFIGx register */
#define AD74416H_SLEW_EN_MSK			NO_OS_GENMASK(7, 6)
#define AD74416H_SLEW_LIN_STEP_MSK		NO_OS_GENMASK(5, 4)
#define AD74416H_SLEW_LIN_RATE_MSK		NO_OS_GENMASK(3, 2)
#define AD74416H_CLR_EN_MSK			NO_OS_BIT(1)
#define AD74416H_I_LIMIT_MSK			NO_OS_BIT(0)


/** DIN_THRESH */
#define AD74416H_DIN_THRESH_MODE_MASK		NO_OS_BIT(7)
#define AD74416H_COMP_THRESH_MASK		NO_OS_GENMASK(6, 0)

/* AD74416H_ADC_RESULT_UPRn */
#define AD74416H_ADC_RESULT_UPR_MSK		NO_OS_GENMASK(7, 0)

/* AD74416H_ADC_RESULT_UPRn */
#define AD74416H_ADC_RESULT_MSK			NO_OS_GENMASK(15, 0)

/* AD74416H_ADC_DIAG_RESULTn */
#define AD74416H_ADC_DIAG_RESULT_MSK		NO_OS_GENMASK(15, 0)

#define AD74416H_TEMP_OFFSET			-2392
#define AD74416H_TEMP_SCALE			8950
#define AD74416H_TEMP_SCALE_DIV			1000

#define AD74416H_FRAME_SIZE 			5
#define AD74416H_THRESHOLD_DAC_RANGE		98
#define AD74416H_THRESHOLD_RANGE		30000
#define AD74416H_DAC_RANGE			12000
#define AD74416H_DAC_CURRENT_RANGE		25000
#define AD74416H_DAC_RESOLUTION			16
#define AD74116H_CONV_TIME_US			1000000

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
/**
 * @brief DAC output ranges in Vout mode
 */
enum ad74416h_vout_range {
	AD74416H_VOUT_RANGE_RNG_0_12V,
	AD74416H_VOUT_RANGE_RNG_NEG12_12V,
};

/**
 * #brief DAC Current limits in Vout mode
 */
enum ad74416h_i_limit {
	AD74416H_I_LIMIT0,
	AD74416H_I_LIMIT1,
};

/**
 * @brief Operation modes of the device.
 */
enum ad74416h_op_mode {
	AD74416H_HIGH_Z,
	AD74416H_VOLTAGE_OUT,
	AD74416H_CURRENT_OUT,
	AD74416H_VOLTAGE_IN,
	AD74416H_CURRENT_IN_EXT,
	AD74416H_CURRENT_IN_LOOP,
	AD74416H_RESISTANCE,
	AD74416H_DIGITAL_INPUT,
	AD74416H_DIGITAL_INPUT_LOOP,
	AD74416H_CURRENT_OUT_HART,
	AD74416H_CURRENT_IN_EXT_HART,
	AD74416H_CURRENT_IN_LOOP_HART,
};

/**
 * @brief GPO operation modes.
 */
enum ad74416h_gpio_select {
	AD74416H_GPIO_CONFIG_HIGH_Z,
	AD74416H_GPIO_CONFIG_DATA,
	AD74416H_GPIO_CONFIG_IN,
	AD74416H_GPIO_CONFIG_COMP,
	AD74416H_GPIO_CONFIG_DO
};

/**
 * @brief Conversion configuration values.
 */
enum ad74416h_adc_rate {
	AD74416H_10SPS_50_60HZ_HART_REJECTION,
	AD74416H_20SPS_50_60HZ_REJECTION,
	AD74416H_1K2SPS,
	AD74416H_4K8SPS,
	AD74416H_9K6SPS,
	AD74416H_19K2SPS,
	AD74416H_200SPS_HART_REJECTION,
};

/**
 * @brief Debounce modes for the IOx inputs when using the digital input op mode.
 */
enum ad74416h_debounce_mode {
	AD74416H_DEBOUNCE_MODE_0,
	AD74416H_DEBOUNCE_MODE_1
};

/**
 * @brief ADC conversion sequence commands.
 */
enum ad74416h_conv_seq {
	AD74416H_STOP_PWR_UP,
	AD74416H_START_SINGLE,
	AD74416H_START_CONT,
	AD74416H_STOP_PWR_DOWN,
};

/**74416h_get_live
 * @brief Possible values to be loaded in the DIAG_RESULT register
 */
enum ad74416h_diag_mode {
	AD74416H_DIAG_AGND,
	AD74416H_DIAG_TEMP,
	AD74416H_DIAG_DVCC,
	AD74416H_DIAG_AVCC,
	AD74416H_DIAG_LDO1V8,
	AD74416H_DIAG_AVDD_HI,
	AD74416H_DIAG_AVDD_LO,
	AD74416H_DIAG_AVSS,
	AD74416H_DIAG_LVIN,
	AD74416H_DIAG_DO_VDD,
	AD74416H_VSENSEP_C,
	AD74416H_VSENSEN_C,
	AD74416H_DO_C,
	AD74416H_AVDD_C,
};

/**
 * @brief The voltage step size of Full Scale DAC Voltage.
 */
enum ad74416h_slew_lin_step {
	AD74416H_STEP_0_8_PERCENT,
	AD74416H_STEP_1_5_PERCENT,
	AD74416H_STEP_6_1_PERCENT,
	AD74416H_STEP_22_2_PERCENT,
};

/**
 * @brief Possible update rates for a DAC when slew control is enabled
 */
enum ad74416h_lin_rate {
	AD74416H_LIN_RATE_4KHZ8,
	AD74416H_LIN_RATE_76KHZ8,
	AD74416H_LIN_RATE_153KHZ6,
	AD74416H_LIN_RATE_230KHZ4,
};

/**
 * @brief Bitfield struct which maps on the LIVE_STATUS register
 */
struct _ad74416h_live_status {
	uint8_t SUPPLY_STATUS: 1;
	uint8_t ADC_BUSY: 1;
	uint8_t ADC_DATA_RDY: 1;
	uint8_t TEMP_ALERT_STATUS: 1;
	uint8_t DIN_STATUS_A: 1;
	uint8_t DIN_STATUS_B: 1;
	uint8_t DIN_STATUS_C: 1;
	uint8_t DIN_STATUD_D: 1;
	uint8_t DO_STATUS_A: 1;
	uint8_t DO_STATUS_B: 1;
	uint8_t DO_STATUS_C: 3;
	uint8_t DO_STATUS_D: 1;
	uint8_t ANALOG_IO_STATUS_A: 1;
	uint8_t ANALOG_IO_STATUS_B: 1;
	uint8_t ANALOG_IO_STATUS_C: 1;
	uint8_t ANALOG_IO_STATUS_D: 1;
};

/**
 * @brief Used to store the live status bit fields.
 */
union ad74416h_live_status {
	struct _ad74416h_live_status status_bits;
	uint16_t value;
};

struct ad74416h_init_param {
	uint8_t dev_addr;
	struct no_os_spi_init_param spi_ip;
};

/**
 * @brief Device channel state.
 */

struct ad74416h_channel_config {
	bool enabled;
	enum ad74416h_op_mode function;
	enum ad74416h_vout_range vout_range;
	enum ad74416h_i_limit i_limit;
};

/**
 * @brief AD74416h device descriptor.
 */
struct ad74416h_desc {
	uint8_t dev_addr;
	struct no_os_spi_desc *spi_desc;
	uint8_t comm_buff[AD74416H_FRAME_SIZE];
	struct ad74416h_channel_config channel_configs[AD74416H_N_CHANNELS];
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/** Converts a millivolt value in the corresponding DAC 16 bit code */
int ad74416h_dac_voltage_to_code(uint32_t, uint32_t *);

/** Converts a microamp value in the corresponding DAC 16 bit code */
int ad74416h_dac_current_to_code(uint32_t, uint16_t *);

/** Write a register's value */
int ad74416h_reg_write(struct ad74416h_desc *, uint32_t, uint16_t);

/** Read a raw communication frame */
int ad74416h_reg_read_raw(struct ad74416h_desc *, uint32_t, uint8_t *);

/** Read a register's value */
int ad74416h_reg_read(struct ad74416h_desc *, uint32_t, uint16_t *);

/** Update a register's field */
int ad74416h_reg_update(struct ad74416h_desc *, uint32_t, uint16_t,
			uint16_t);

/** Get the number of active channels */
int ad74416h_nb_active_channels(struct ad74416h_desc *, uint8_t *);

/**
 * Select which information the device will respond with (in the readback field)
 * when a read operation is performed
 */
int ad74416h_set_info(struct ad74416h_desc *desc, uint16_t mode);

/** Set the operation mode for a specific channel */
int ad74416h_set_channel_function(struct ad74416h_desc *,
				  uint32_t, enum ad74416h_op_mode);

/** Set the voltage range for a specific channel DAC */
int ad74416h_set_channel_vout_range(struct ad74416h_desc *,
                                    uint32_t, enum ad74416h_vout_range);

/** Set the current limit for vout mode for a specific channel DAC */
int ad74416h_set_channel_i_limit(struct ad74416h_desc *,
				 uint32_t, enum ad74416h_i_limit);

/** Read the raw ADC raw conversion value */
int ad74416h_get_raw_adc_result(struct ad74416h_desc *, uint32_t,
				uint32_t *);

/** Enable/disable a specific ADC channel */
int ad74416h_set_adc_channel_enable(struct ad74416h_desc *, uint32_t,
				    bool);

/** Enable conversions on a diagnostic register */
int ad74416h_set_diag_channel_enable(struct ad74416h_desc *, uint32_t, bool);

/** Get the ADC measurement range for a specific channel */
int ad74416h_get_adc_range(struct ad74416h_desc *, uint32_t, uint16_t *);

/** Get the ADC sample rate. */
int ad74416h_get_adc_rate(struct ad74416h_desc *, uint32_t,
			  enum ad74416h_adc_rate *);

/** Set the ADC sample rate. */
int ad74416h_set_adc_rate(struct ad74416h_desc *, uint32_t,
			  enum ad74416h_adc_rate);

/** Start or stop ADC conversions */
int ad74416h_set_adc_conv_seq(struct ad74416h_desc *, enum ad74416h_conv_seq);

/** Get a single ADC raw value for a specific channel, then power down the ADC */
int ad74416h_get_adc_single(struct ad74416h_desc *, uint32_t, uint16_t *);

/** Read the die's temperature from the diagnostic register */
int ad74416h_get_temp(struct ad74416h_desc *, uint32_t, uint16_t *);

/** Set and load a code for the DAC on a specific channel */
int ad74416h_set_channel_dac_code(struct ad74416h_desc *, uint32_t, uint16_t);

/** Set which diagnostic value to be loaded in the DIAG_RESULT register */
int ad74416h_set_diag(struct ad74416h_desc *, uint32_t,
		      enum ad74416h_diag_mode);

/** Get the diagnostic value for a specific channel */
int ad74416h_get_diag(struct ad74416h_desc *, uint32_t, uint16_t *);

/**
 * Set the debounce mode for the IOx inputs when the ADC is running in digital
 * input mode.
 */
int ad74416h_set_debounce_mode(struct ad74416h_desc *, uint32_t,
			       enum ad74416h_debounce_mode);

/**
 * Set the debounce settle time for the IOx inputs when the ADC is
 * running in digital input mode
 */
int ad74416h_set_debounce_time(struct ad74416h_desc *, uint32_t, uint32_t);

/** Get the GPIO value for a specific channel */
int ad74416h_gpio_get(struct ad74416h_desc *, uint32_t, uint8_t *);

/** Set the GPIO operation mode */
int ad74416h_set_gpio_config(struct ad74416h_desc *, uint32_t,
			     enum ad74416h_gpio_select);

/**
 * Set the threshold, for which a signal would be considered high,
 * when the ADC is running in digital input mode
 */
int ad74416h_set_threshold(struct ad74416h_desc *, uint32_t, uint32_t);

/** Set the logic value of a GPO pin */
int ad74416h_gpio_set(struct ad74416h_desc *, uint32_t, uint8_t);

/** Read the live status bits */
int ad74416h_get_live(struct ad74416h_desc *,
		      union ad74416h_live_status *);

/** Configure and enable slew rate control for a DAC on a specific channel */
int ad74416h_dac_slew_enable(struct ad74416h_desc *, uint32_t,
			     enum ad74416h_slew_lin_step,
			     enum ad74416h_lin_rate);

/** Disable the slew rate control */
int ad74416h_dac_slew_disable(struct ad74416h_desc *, uint32_t);

/** Enable or disable the higher thermal reset */
int ad74416h_set_therm_rst(struct ad74416h_desc *, bool);

/** Perform a soft reset */
int ad74416h_reset(struct ad74416h_desc *);

/** Initialize the device structure */
int ad74416h_init(struct ad74416h_desc **, struct ad74416h_init_param *);

/** Free the device descriptor */
int ad74416h_remove(struct ad74416h_desc *desc);

#endif // _AD74416H_H
