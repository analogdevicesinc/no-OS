/***************************************************************************//**
 *   @file   ad74413r.h
 *   @brief  Header file of AD74413r Driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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
#ifndef _AD74413R_H
#define _AD74413R_H

#include "stdint.h"
#include "stdbool.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"

#define AD74413R_N_CHANNELS             4
#define AD74413R_N_DIAG_CHANNELS	4

#define AD74413R_CH_A                   0
#define AD74413R_CH_B                   1
#define AD74413R_CH_C                   2
#define AD74413R_CH_D                   3

/** The value of the sense resistor in ohms */
#define AD74413R_RSENSE                 100
/** 16 bit ADC */
#define AD74413R_ADC_MAX_VALUE		65535

/** Register map */

#define AD74413R_NOP                            0x00
#define AD74413R_CH_FUNC_SETUP(x)               (0x01 + x)
#define AD74413R_ADC_CONFIG(x)                  (0x05 + x)
#define AD74413R_DIN_CONFIG(x)                  (0x09 + x)
#define AD74413R_GPO_PARALLEL                   0x0D
#define AD74413R_GPO_CONFIG(x)                  (0x0E + x)
#define AD74413R_OUTPUT_CONFIG(x)               (0x12 + x)
#define AD74413R_DAC_CODE(x)                    (0x16 + x)
#define AD74413R_DAC_CLR_CODE(x)                (0x1A + x)
#define AD74413R_DAC_ACTIVE(x)                  (0x1E + x)
#define AD74413R_DIN_THRESH                     0x22
#define AD74413R_ADC_CONV_CTRL                  0x23
#define AD74413R_DIAG_ASSIGN                    0x24
#define AD74413R_DIN_COMP_OUT                   0x25
#define AD74413R_ADC_RESULT(x)                  (0x26 + x)
#define AD74413R_DIAG_RESULT(x)                 (0x2A + x)
#define AD74413R_ALERT_STATUS                   0x2E
#define AD74413R_LIVE_STATUS                    0x2F
#define AD74413R_ALERT_MASK                     0x3C
#define AD74413R_DIN_COUNTER(x)                 (0x3D + x)
#define AD74413R_READ_SELECT                    0x41
#define AD74413R_THERM_RST                      0x43
#define AD74413R_CMD_KEY                        0x44
#define AD74413R_SCRATCH                        0x45
#define AD74413R_SILICON_REV                    0x46
#define AD74413R_ALERT_STATUS_RESET             NO_OS_GENMASK(15, 0)

/** Software reset sequence */
#define AD74413R_CMD_KEY_RESET_1                0x15FA
#define AD74413R_CMD_KEY_RESET_2                0xAF51

/** Command to load code into the DAC from DAC_CODEx */
#define AD74413R_CMD_KEY_LDAC	                0x953A

/** Command to load code into the DAC from DAC_CLR_CODEx */
#define AD74413R_CMD_KEY_DAC_CLEAR		0x73D1

#define AD74413R_SPI_RD_RET_INFO_MASK		NO_OS_BIT(8)
#define AD74413R_ERR_CLR_MASK			NO_OS_GENMASK(15, 0)
#define AD74413R_SPI_CRC_ERR_MASK		NO_OS_BIT(13)
#define AD74413R_CH_FUNC_SETUP_MASK             NO_OS_GENMASK(3, 0)
#define AD74413R_ADC_RANGE_MASK                 NO_OS_GENMASK(7, 5)
#define AD74413R_ADC_REJECTION_MASK             NO_OS_GENMASK(4, 3)
#define AD74413R_DEBOUNCE_TIME_MASK             NO_OS_GENMASK(4, 0)
#define AD74413R_DEBOUNCE_MODE_MASK             NO_OS_BIT(5)
#define AD74413R_DIAG_RESULT_MASK		NO_OS_GENMASK(15, 0)
#define AD74413R_REV_ID				NO_OS_GENMASK(7, 0)
#define AD74413R_CH_200K_TO_GND_MASK		NO_OS_BIT(2)

/** GPO_PARALLEL register */
#define AD74413R_GPO_PAR_DATA_MASK(x)		NO_OS_BIT(x)

/** GPO_CONFIGx register */
#define AD74413R_GPO_SELECT_MASK		NO_OS_GENMASK(2, 0)
#define AD74413R_GPO_DATA_MASK			NO_OS_BIT(3)

/** OUTPUT_CONFIGx register */
#define AD74413R_SLEW_EN_MASK			NO_OS_GENMASK(7, 6)
#define AD74413R_SLEW_LIN_STEP_MASK		NO_OS_GENMASK(5, 4)
#define AD74413R_SLEW_LIN_RATE_MASK		NO_OS_GENMASK(3, 2)
#define AD74413R_CLR_EN_MASK			NO_OS_BIT(1)
#define AD74413R_I_LIMIT_MASK			NO_OS_BIT(0)

/** DAC_CODEx register */
#define AD74413R_DAC_CODE_MASK			NO_OS_GENMASK(12, 0)

/** DAC_CLR_CODEx register */
#define AD74413R_CLR_CODE_MASK			NO_OS_GENMASK(12, 0)

/** DAC_ACTIVEx register */
#define AD74413R_DAC_ACTIVE_CODE_MASK		NO_OS_GENMASK(12, 0)

/** DIN_THRESH */
#define AD74413R_COMP_THRESH_MASK		NO_OS_GENMASK(5, 1)
#define AD74413R_DIN_THRESH_MODE_MASK		NO_OS_BIT(0)

/** DIN_COMP_OUT register */
#define AD74413R_DIN_COMP_CH(x)			NO_OS_BIT(x)

/** ADC_CONV_CTRL register */
#define AD74413R_EN_REJ_DIAG_MASK		NO_OS_BIT(10)
#define AD74413R_CONV_SEQ_MASK                  NO_OS_GENMASK(9, 8)
#define AD74413R_DIAG_EN_MASK(x)		(NO_OS_BIT(x) << 4)
#define AD74413R_CH_EN_MASK(x)                  NO_OS_BIT(x)

/** DIAG_ASSIGN register */
#define AD74413R_DIAG_ASSIGN_MASK(x)		(NO_OS_GENMASK(3, 0) << (x * 4))

/** The maximum voltage output of the DAC is 11V */
#define AD74413R_DAC_RANGE			11000
/** 13 bit DAC */
#define AD74413R_DAC_RESOLUTION			13
#define AD74413R_DAC_CODE_MAX			8191
#define AD74413R_ADC_RESOLUTION			16
#define AD74413R_ADC_CODE_MAX			65536

/** The number of possible DAC values */
#define AD74413R_THRESHOLD_DAC_RANGE		29
/** The comparator's value can be set betwen 0 - 16 V*/
#define AD74413R_THRESHOLD_RANGE		16000

#define AD74413R_TEMP_OFFSET			-2392
#define AD74413R_TEMP_SCALE			8950
#define AD74413R_TEMP_SCALE_DIV			1000

#define AD74413R_RANGE_10V_SCALE		15259ULL
#define AD74413R_RANGE_10V_SCALE_DIV		100000ULL
#define AD74413R_RANGE_2V5_SCALE		38147ULL
#define AD74413R_RANGE_2V5_SCALE_DIV		1000000ULL
#define AD74413R_RANGE_5V_SCALE			76294ULL
#define AD74413R_RANGE_5V_SCALE_DIV		1000000ULL
#define AD74413R_RANGE_5V_OFFSET		-(AD74413R_ADC_MAX_VALUE / 2)
#define AD74413R_RTD_PULL_UP			2100000ULL
#define AD74413R_SENSE_RESISTOR_OHMS		100

/**
 * @brief The chips supported by this driver.
 */
enum ad74413r_chip_id {
	AD74413R,
	AD74412R
};

/**
 * @brief Rejection config values. The HART variants are not supported by the
 * AD74412R device.
 */
enum ad74413r_rejection {
	AD74413R_REJECTION_50_60,
	AD74413R_REJECTION_NONE,
	AD74413R_REJECTION_50_60_HART,
	AD74413R_REJECTION_HART
};

/**
 * @brief Operation modes of the device.
 */
enum ad74413r_op_mode {
	AD74413R_HIGH_Z,
	AD74413R_VOLTAGE_OUT,
	AD74413R_CURRENT_OUT,
	AD74413R_VOLTAGE_IN,
	AD74413R_CURRENT_IN_EXT,
	AD74413R_CURRENT_IN_LOOP,
	AD74413R_RESISTANCE,
	AD74413R_DIGITAL_INPUT,
	AD74413R_DIGITAL_INPUT_LOOP,
	AD74413R_CURRENT_IN_EXT_HART,
	AD74413R_CURRENT_IN_LOOP_HART,
};

/**
 * @brief ADC ranges configurations. These are dependent on the operation mode.
 */
enum ad74413r_adc_range {
	AD74413R_ADC_RANGE_10V,
	AD74413R_ADC_RANGE_2P5V_EXT_POW,
	AD74413R_ADC_RANGE_2P5V_INT_POW,
	AD74413R_ADC_RANGE_5V_BI_DIR
};

/**
 * @brief ADC conversion sequence commands.
 */
enum ad74413r_conv_seq {
	AD74413R_STOP_PWR_UP,
	AD74413R_START_SINGLE,
	AD74413R_START_CONT,
	AD74413R_STOP_PWR_DOWN,
};

/**
 * @brief GPO operation modes.
 */
enum ad74413r_gpo_select {
	AD74413R_GPO_CONFIG_100K_PD,
	AD74413R_GPO_CONFIG_DATA,
	AD74413R_GPO_CONFIG_PAR_DATA,
	AD74413R_GPO_CONFIG_COMP,
	AD74413R_GPO_CONFIG_HIGH_Z
};

/**
 * @brief Possible values to be loaded in the DIAG_RESULT register
 */
enum ad74413r_diag_mode {
	AD74413R_DIAG_AGND,
	AD74413R_DIAG_TEMP,
	AD74413R_DIAG_AVDD,
	AD74413R_DIAG_AVSS,
	AD74413R_DIAG_REFOUT,
	AD74413R_DIAG_ALDO_5V,
	AD74413R_DIAG_ALDO_1V8,
	AD74413R_DIAG_DLDO_1V8,
	AD74413R_DIAG_DVCC,
	AD74413R_DIAG_IOVDD,
	AD74413R_SENSEL_A,
	AD74413R_SENSEL_B,
	AD74413R_SENSEL_C,
	AD74413R_SENSEL_D
};

/**
 * @brief Debounce modes for the IOx inputs when using the digital input op mode.
 */
enum ad74413r_debounce_mode {
	AD74413R_DEBOUNCE_MODE_0,
	AD74413R_DEBOUNCE_MODE_1
};

/**
 * @brief The number of increments per step a DAC does when slew control is enabled.
 */
enum ad74413r_slew_lin_step {
	AD74413R_STEP_64,
	AD74413R_STEP_120,
	AD74413R_STEP_500,
	AD74413R_STEP_1820,
};

/**
 * @brief Possible update rates for a DAC when slew control is enabled
 */
enum ad74413r_lin_rate {
	AD74413R_LIN_RATE_4KHZ,
	AD74413R_LIN_RATE_64KHZ,
	AD74413R_LIN_RATE_150KHZ,
	AD74413R_LIN_RATE_240KHZ,
};

enum ad74413r_adc_sample {
	AD74413R_ADC_SAMPLE_20HZ = 20,
	AD74413R_ADC_SAMPLE_4800HZ = 4800,
	AD74413R_ADC_SAMPLE_10HZ = 10,
	AD74413R_ADC_SAMPLE_1200HZ = 1200,
};

/**
 * @brief Initialization parameter for the device descriptor.
 */
struct ad74413r_init_param {
	enum ad74413r_chip_id chip_id;
	struct no_os_spi_init_param comm_param;
	struct no_os_gpio_init_param *reset_gpio_param;
};

/**
 * @brief ADC value format
 */
struct ad74413r_decimal {
	int64_t integer;
	int32_t decimal;
};

/**
 * @brief Device channel state.
 */
struct ad74413r_channel_config {
	bool enabled;
	enum ad74413r_op_mode function;
};

/**
 * @brief Bitfield struct which maps on the LIVE_STATUS register
 */
struct _ad74413r_live_status {
	uint8_t VI_ERR_A: 1;
	uint8_t VI_ERR_B: 1;
	uint8_t VI_ERR_C: 1;
	uint8_t VI_ERR_D: 1;
	uint8_t HI_TEMP_ERR: 1;
	uint8_t CHARGE_PUMP_ERR: 1;
	uint8_t ALDO5V_ERR: 1;
	uint8_t AVDD_ERR: 1;
	uint8_t DVCC_ERR: 1;
	uint8_t ALDO1V8_ERR: 1;
	uint8_t ADC_CH_CURR: 3;
	uint8_t ADC_BUSY: 1;
	uint8_t ADC_DATA_RDY: 1;
	uint8_t _RESERVED: 1;
};

/**
 * @brief Used to store the live status bit fields.
 */
union ad74413r_live_status {
	struct _ad74413r_live_status status_bits;
	uint16_t value;
};

/**
 * @brief AD74413r device descriptor.
 */
struct ad74413r_desc {
	enum ad74413r_chip_id chip_id;
	struct no_os_spi_desc *comm_desc;
	uint8_t comm_buff[4];
	struct ad74413r_channel_config channel_configs[AD74413R_N_CHANNELS];
	struct no_os_gpio_desc *reset_gpio;
};

/** Converts a millivolt value in the corresponding DAC 13 bit code */
int ad74413r_dac_voltage_to_code(uint32_t, uint32_t *);

/** Convert a voltage range from an enum representation to a millivolt value */
int ad74413r_range_to_voltage_range(enum ad74413r_adc_range, uint32_t *);

int ad74413r_range_to_voltage_offset(enum ad74413r_adc_range, int32_t *);

/** Write a register's value */
int ad74413r_reg_write(struct ad74413r_desc *, uint32_t, uint16_t);

/** Read a raw communication frame */
int ad74413r_reg_read_raw(struct ad74413r_desc *, uint32_t, uint8_t *);

/** Read a register's value */
int ad74413r_reg_read(struct ad74413r_desc *, uint32_t, uint16_t *);

/** Update a register's field */
int ad74413r_reg_update(struct ad74413r_desc *, uint32_t, uint16_t,
			uint16_t);

/** Get the number of active channels */
int ad74413r_nb_active_channels(struct ad74413r_desc *, uint8_t *);

/** Clear the ALERT_STATUS register */
int ad74413r_clear_errors(struct ad74413r_desc *);

/**
 * Select which information the device will respond with (in the readback field)
 * when a read operation is performed
 */
int ad74413r_set_info(struct ad74413r_desc *desc, uint16_t mode);

/** Perform either a software or hardware reset and wait for device reset time. */
int ad74413r_reset(struct ad74413r_desc *);

/** Set the operation mode for a specific channel */
int ad74413r_set_channel_function(struct ad74413r_desc *,
				  uint32_t, enum ad74413r_op_mode);

/** Read the raw ADC raw conversion value */
int ad74413r_get_raw_adc_result(struct ad74413r_desc *, uint32_t,
				uint16_t *);

/** Enable/disable a specific ADC channel */
int ad74413r_set_adc_channel_enable(struct ad74413r_desc *, uint32_t,
				    bool);

/** Enable conversions on a diagnostic register */
int ad74413r_set_diag_channel_enable(struct ad74413r_desc *, uint32_t, bool);

/** Get the ADC measurement range for a specific channel */
int ad74413r_get_adc_range(struct ad74413r_desc *, uint32_t, uint16_t *);

/** Get the rejection setting for a specific channel */
int ad74413r_get_adc_rejection(struct ad74413r_desc *, uint32_t,
			       enum ad74413r_rejection *);

/** Get the rejection setting for any diagnostics channel */
int ad74413r_get_adc_diag_rejection(struct ad74413r_desc *,
				    enum ad74413r_rejection *);

/** Set the rejection setting for a specific channel */
int ad74413r_set_adc_rejection(struct ad74413r_desc *, uint32_t,
			       enum ad74413r_rejection);

/** Get the ADC sample rate. */
int ad74413r_get_adc_rate(struct ad74413r_desc *, uint32_t,
			  enum ad74413r_adc_sample *);

/** Set the ADC sample rate. */
int ad74413r_set_adc_rate(struct ad74413r_desc *, uint32_t,
			  enum ad74413r_adc_sample);

/** Get the ADC sample rate for the diagnostics channels. */
int ad74413r_get_adc_diag_rate(struct ad74413r_desc *, uint32_t,
			       enum ad74413r_adc_sample *);

/** Set the ADC sample rate for the diagnostics channels. */
int ad74413r_set_adc_diag_rate(struct ad74413r_desc *, uint32_t,
			       enum ad74413r_adc_sample);

/** Start or stop ADC conversions */
int ad74413r_set_adc_conv_seq(struct ad74413r_desc *, enum ad74413r_conv_seq);

/** Get a single ADC raw value for a specific channel, then power down the ADC */
int ad74413r_get_adc_single(struct ad74413r_desc *, uint32_t, uint16_t *, bool);

/** Get the ADC real value, according to the operation mode */
int ad74413r_adc_get_value(struct ad74413r_desc *, uint32_t,
			   struct ad74413r_decimal *);

/** Read the die's temperature from the diagnostic register */
int ad74413r_get_temp(struct ad74413r_desc *, uint32_t, uint16_t *);

/** Set and load a code for the DAC on a specific channel */
int ad74413r_set_channel_dac_code(struct ad74413r_desc *, uint32_t, uint16_t);

/** Set which diagnostic value to be loaded in the DIAG_RESULT register */
int ad74413r_set_diag(struct ad74413r_desc *, uint32_t,
		      enum ad74413r_diag_mode);

/** Get the diagnostic value for a specific channel */
int ad74413r_get_diag(struct ad74413r_desc *, uint32_t, uint16_t *);

/**
 * Set the debounce mode for the IOx inputs when the ADC is running in digital
 * input mode.
 */
int ad74413r_set_debounce_mode(struct ad74413r_desc *, uint32_t,
			       enum ad74413r_debounce_mode);

/**
 * Set the debounce settle time for the IOx inputs when the ADC is
 * running in digital input mode
 */
int ad74413r_set_debounce_time(struct ad74413r_desc *, uint32_t, uint32_t);

/** Get the GPO value for a specific channel */
int ad74413r_gpo_get(struct ad74413r_desc *, uint32_t, uint8_t *);

/** Set the GPO operation mode */
int ad74413r_set_gpo_config(struct ad74413r_desc *, uint32_t,
			    enum ad74413r_gpo_select);

/**
 * Set the threshold, for which a signal would be considered high,
 * when the ADC is running in digital input mode
 */
int ad74413r_set_threshold(struct ad74413r_desc *, uint32_t, uint32_t);

/** Set the logic value of a GPO pin */
int ad74413r_gpo_set(struct ad74413r_desc *, uint32_t, uint8_t);

/** Set multiple GPO values at once */
int ad74413r_gpo_set_multiple(struct ad74413r_desc *, uint32_t);

/** Read the live status bits */
int ad74413r_get_live(struct ad74413r_desc *,
		      union ad74413r_live_status *);

/**
 * The code value will be loaded into the DACs when the CLR_EN bit in the
 * OUTPUT_CONFIGx registers is asserted and the DAC clear key is written
 */
int ad74413r_set_dac_clear_code(struct ad74413r_desc *, uint32_t, uint16_t);

/** Clear the DAC (to the code in DAC_CLR_CODE register) */
int ad74413r_clear_dac(struct ad74413r_desc *, uint32_t);

/** Configure and enable slew rate control for a DAC on a specific channel */
int ad74413r_dac_slew_enable(struct ad74413r_desc *, uint32_t,
			     enum ad74413r_slew_lin_step,
			     enum ad74413r_lin_rate);

/** Disable the slew rate control */
int ad74413r_dac_slew_disable(struct ad74413r_desc *, uint32_t);

/** Enable or disable the higher thermal reset */
int ad74413r_set_therm_rst(struct ad74413r_desc *, bool);

/** Initialize the device structure */
int ad74413r_init(struct ad74413r_desc **, struct ad74413r_init_param *);

/** Free the device descriptor */
int ad74413r_remove(struct ad74413r_desc *desc);

#endif // _AD74413R_H
