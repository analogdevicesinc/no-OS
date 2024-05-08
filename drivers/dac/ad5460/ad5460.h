/***************************************************************************//**
 *   @file   ad5460.h
 *   @brief  Header file of AD5460 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#ifndef _AD5460_H
#define _AD5460_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "stdint.h"
#include "stdbool.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define AD5460_NOP				0x00
#define AD5460_CH_FUNC_SETUP(x)               (0x01 + (x * 6))
#define AD5460_OUTPUT_CONFIG(x)               (0x02 + (x * 6))
#define AD5460_DAC_SLEW_CONFIG(x)		(0x03 + (x * 6))
#define AD5460_DAC_CODE(x)			(0x04 + (x * 6))
#define AD5460_DAC_CLR_CODE(x)                (0x05 + (x * 6))
#define AD5460_DAC_ACTIVE(x)			(0x06 + (x * 6))
#define AD5460_GPIO_CONFIG(x)			(0x19 + x)
#define AD5460_WTD_CONFIG			0x1D
#define AD5460_DIAG_CONFIG			0x1E
#define AD5460_PWR_CONFIG			0x1F
#define AD5460_LIVE_STATUS			0x20
#define AD5460_DIGITAL_ALERT_STATUS		0x21
#define AD5460_ANALOG_ALERT_STATUS		0x22
#define AD5460_DIGITAL_ALERT_MSK		0x23
#define AD5460_ANALOG_ALERT_MSK		0x24
#define AD5460_READ_SELECT			0x73
#define AD5460_THERM_RST			0x74
#define AD5460_CMD_KEY			0x75
#define AD5460_BROADCAST_CMD_KEY		0x76
#define AD5460_SCRATCH(x)			(0x77 + x)
#define AD5460_GENERIC_ID			0x7B
#define AD5460_SILICON_REV			0x7C
#define AD5460_SILICON_ID0			0x7D
#define AD5460_SILICON_ID1			0x7E
#define AD5460_SILICON_ID2			0x7F

/** Software reset sequence */
#define AD5460_CMD_KEY_RESET_1                0x9A17
#define AD5460_CMD_KEY_RESET_2                0xE9C1

/* AD5460_CH_FUNC_SETUP */
#define AD5460_CH_FUNC_SETUP_MSK		NO_OS_GENMASK(3, 0)

/** OUTPUT_CONFIGx Register */
#define AD5460_AVDD_SELECT_MSK		NO_OS_GENMASK(13, 12)
#define AD5460_CLR_EN_MSK			NO_OS_BIT(11)
#define AD5460_WAIT_LDAC_CMD_MSK		NO_OS_BIT(10)
#define AD5460_IOUT_RANGE_MSK			NO_OS_GENMASK(9, 8)
#define AD5460_I_LIMIT_MSK			NO_OS_BIT(7)
#define AD5460_ALARM_DEG_PERIOD_MSK		NO_OS_BIT(6)
#define AD5460_VOUT_RANGE_MSK			NO_OS_BIT(5)
#define AD5460_VIOUT_DRV_EN_DLY_MSK		NO_OS_GENMASK(4, 3)
#define AD5460_VOUT_4W_EN_MSK			NO_OS_BIT(2)
#define AD5460_VIOUT_GLITCH_MSK		NO_OS_GENMASK(1, 0)

/** DAC_SLEW_CONFIG Register */
#define AD5460_SLEW_EN_MSK			NO_OS_GENMASK(5, 4)
#define AD5460_SLEW_LIN_STEP_MSK		NO_OS_GENMASK(3, 2)
#define AD5460_SLEW_LIN_RATE_MSK		NO_OS_GENMASK(1, 0)

/** DAC_CODE Register*/
#define AD5460_DAC_CODE_MSK			NO_OS_GENMASK(15, 0)

/** DAC_CLR_CODE Register*/
#define AD5460_DAC_CLR_CODE_MSK		NO_OS_GENMASK(15, 0)

/** DAC_ACTIVE Register*/
#define AD5460_DAC_ACTIVE_MSK			NO_OS_GENMASK(15, 0)

/** GPIO_CONFIG Register */
#define AD5460_GPI_DATA_MSK			NO_OS_BIT(5)
#define AD5460_GPO_DATA_MSK			NO_OS_BIT(4)
#define AD5460_GP_WK_PD_EN_MSK		NO_OS_BIT(3)
#define AD5460_GPIO_SELECT_MSK		NO_OS_GENMASK(2, 0)

/** DIAG_SELECT Register */
#define AD5460_DIAG_SELECT_MSK		NO_OS_GENMASK(4, 0)

/** PWR_CONFIG Register */
#define AD5460_REF_EN_MSK			NO_OS_BIT(0)

/** LIVE_STATUS Register */
#define AD5460_TEMP_ALERT_STATUS_MSK		NO_OS_BIT(9)
#define AD5460_SUPPLY_STATUS_MSK		NO_OS_BIT(8)
#define AD5460_ANALOG_IO_SC_STATUS_D_MSK	NO_OS_BIT(7)
#define AD5460_ANALOG_IO_SC_STATUS_C_MSK	NO_OS_BIT(6)
#define AD5460_ANALOG_IO_SC_STATUS_B_MSK	NO_OS_BIT(5)
#define AD5460_ANALOG_IO_SC_STATUS_A_MSK	NO_OS_BIT(4)
#define AD5460_ANALOG_IO_OC_STATUS_D_MSK	NO_OS_BIT(3)
#define AD5460_ANALOG_IO_OC_STATUS_C_MSK	NO_OS_BIT(2)
#define AD5460_ANALOG_IO_OC_STATUS_B_MSK	NO_OS_BIT(1)
#define AD5460_ANALOG_IO_OC_STATUS_A_MSK	NO_OS_BIT(0)

/** DIGITAL_ALERT_STATUS Register */
#define AD5460_TEMP_ALERT_MSK			NO_OS_BIT(4)
#define AD5460_CAL_MEM_ERR_MSK		NO_OS_BIT(3)
#define AD5460_SPI_ERR_MSK			NO_OS_BIT(2)
#define AD5460_WDT_RESET_OCCURRED_MSK		NO_OS_BIT(1)
#define AD5460_RESET_OCCURRED_MSK		NO_OS_BIT(0)

/* ANALOG_ALERT_STATUS Register */
#define AD5460_AVDD_HI_ERR_MSK			NO_OS_BIT(12)
#define AD5460_AVDD_LO_ERR_MSK			NO_OS_BIT(11)
#define AD5460_AVCC_ERR_MSK			NO_OS_BIT(10)
#define AD5460_IODVCC_ERR_MSK			NO_OS_BIT(9)
#define AD5460_AVSS_ERR_MSK			NO_OS_BIT(8)
#define AD5460_ANALOG_IO_SC_D_MSK		NO_OS_BIT(7)
#define AD5460_ANALOG_IO_SC_C_MSK		NO_OS_BIT(6)
#define AD5460_ANALOG_IO_SC_B_MSK		NO_OS_BIT(5)
#define AD5460_ANALOG_IO_SC_A_MSK		NO_OS_BIT(4)
#define AD5460_ANALOG_IO_OC_D_MSK		NO_OS_BIT(3)
#define AD5460_ANALOG_IO_OC_C_MSK		NO_OS_BIT(2)
#define AD5460_ANALOG_IO_OC_B_MSK		NO_OS_BIT(1)
#define AD5460_ANALOG_IO_OC_A_MSK		NO_OS_BIT(0)

/* AD5460 READ_SELECT Register */
#define AD5460_READBACK_SELECT_MODE_MSK	NO_OS_GENMASK(8, 0)
#define AD5460_READBACK_ADDR_MSK		NO_OS_GENMASK(8, 0)

/* THERM_RST Register */
#define AD5460_THERM_RST_MSK			NO_OS_GENMASK(8, 0)

/* CMD_KEY Register */
#define AD5460_CMD_KEY_MSK			NO_OS_GENMASK(15, 0)

/* BROADCAST_CMD_KEY Register */
#define AD5460_BROADCAST_CMD_KEY_MSK		NO_OS_GENMASK(15, 0)

/* SCRATCH Register */
#define AD5460_SCRATCH_BITS_MSK		NO_OS_GENMASK(15, 0)

/* GENERIC_ID Register */
#define AD5460_GENERIC_ID_MSK			NO_OS_GENMASK(2, 0)

/* SILICON_REV Register */
#define AD5460_SILICON_REV_ID_MSK		NO_OS_GENMASK(7, 0)

/* SILICON_ID0 Register */
#define AD5460_LOT_MSK			NO_OS_GENMASK(6, 0)

/* SILICON_ID1 Register */
#define AD5460_Y_COORDS_MSK			NO_OS_GENMASK(13, 7)
#define AD5460_X_COORDS_MSK			NO_OS_GENMASK(6, 0)

/* SILICON_ID2 Register */
#define AD5460_WAFER_NUM_MSK			NO_OS_GENMASK(4, 0)

#define AD5460_FRAME_SIZE			5
#define AD5460_N_CHANNELS			4
#define AD5460_CH_A				0
#define AD5460_CH_B				1
#define AD5460_CH_C				2
#define AD5460_CH_D				3
#define AD5460_DAC_RANGE			12000
#define AD5460_DAC_CURRENT_RANGE_25MA		25000
#define AD5460_DAC_CURRENT_RANGE_20MA		20000
#define AD5460_DAC_CURRENT_RANGE_4MA		4000
#define AD5460_DAC_RESOLUTION			16

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @brief Operation modes of the device.
 */
enum ad5460_op_mode {
	AD5460_HIGH_Z = 0x0,
	AD5460_VOLTAGE_OUT = 0x1,
	AD5460_CURRENT_OUT = 0x2,
	AD5460_CURRENT_OUT_HART = 0x10,

};

/**
 * @brief Possible voltage output ranges for the DAC
 */
enum ad5460_vout_range {
	AD5460_VOUT_RANGE_0_12V,
	AD5460_VOUT_RANGE_NEG12_12V,
};

/**
 * @brief Possible current output ranges for the DAC
 */
enum ad5460_iout_range {
	AD5460_IOUT_RANGE_0_25MA,
	AD5460_IOUT_RANGE_0_20MA,
	AD5460_IOUT_RANGE_4_20MA,
};

/**
 * @brief DAC Current limits in Vout mode
 */
enum ad5460_i_limit {
	AD5460_I_LIMIT0,
	AD5460_I_LIMIT1,
};

/**
 * @brief The voltage step size of Full Scale DAC Voltage.
 */
enum ad5460_slew_lin_step {
	AD5460_STEP_0_8_PERCENT,
	AD5460_STEP_1_5_PERCENT,
	AD5460_STEP_6_1_PERCENT,
	AD5460_STEP_22_2_PERCENT,
};

/**
 * @brief Possible update rates for a DAC when slew control is enabled
 */
enum ad5460_lin_rate {
	AD5460_LIN_RATE_4KHZ8,
	AD5460_LIN_RATE_76KHZ8,
	AD5460_LIN_RATE_153KHZ6,
	AD5460_LIN_RATE_230KHZ4,
};

/**
 * @brief GPO operation modes.
 */
enum ad5460_gpio_select {
	AD5460_GPIO_SEL_HIGH_Z,
	AD5460_GPIO_SEL_GPIO,
	AD5460_GPIO_SEL_GPI,
};

/**
 * @brief Possible values to be loaded in the DIAG_RESULT register
 */
enum ad5460_diag_mode {
	AD5460_DIAG_NO_DIAG,
	AD5460_DIAG_PTAT,
	AD5460_DIAG_REFIN,
	AD5460_DIAG_REFOUT,
	AD5460_DIAG_INT_BG_V,
	AD5460_DIAG_SENSE_P_CH_A,
	AD5460_DIAG_SENSE_CH_B,
	AD5460_DIAG_SENSE_P_CH_C,
	AD5460_DIAG_SENSE_P_CH_D,
	AD5460_DIAG_SENSE_N_CH_A,
	AD5460_DIAG_SENSE_N_CH_B,
	AD5460_DIAG_SENSE_N_CH_C,
	AD5460_DIAG_SENSE_N_CH_D,
	AD5460_DIAG_SENSE_AVCC,
	AD5460_DIAG_SENSE_LDO,
	AD5460_DIAG_SENSE_DVCC,
	AD5460_DIAG_SENSE_AGND1,
	AD5460_DIAG_SENSE_AGND2,
	AD5460_DIAG_SENSE_AGND3,
	AD5460_DIAG_SENSE_DGND,
	AD5460_DIAG_SENSE_AVDD_HI,
	AD5460_DIAG_SENSE_AVDD_LO,
	AD5460_DIAG_SENSE_AVSS,
	AD5460_DIAG_SENSE_AVDD_CH_A,
	AD5460_DIAG_SENSE_AVDD_CH_B,
	AD5460_DIAG_SENSE_AVDD_CH_C,
	AD5460_DIAG_SENSE_AVDD_CH_D,
};

/**
 * @brief Bitfield struct which maps on the LIVE_STATUS register
 */
struct _ad5460_live_status {
	uint8_t ANALOG_IO_OC_STATUS_A: 1;
	uint8_t ANALOG_IO_OC_STATUS_B: 1;
	uint8_t ANALOG_IO_OC_STATUS_C: 1;
	uint8_t ANALOG_IO_OC_STATUS_D: 1;
	uint8_t ANALOG_IO_SC_STATUS_A: 1;
	uint8_t ANALOG_IO_SC_STATUS_B: 1;
	uint8_t ANALOG_IO_SC_STATUS_C: 1;
	uint8_t ANALOG_IO_SC_STATUS_D: 1;
	uint8_t SUPPLY_STATUS: 1;
	uint8_t TEMP_ALERT_STATUS: 1;
};

/**
 * @brief Used to store the live status bit fields.
 */
union ad5460_live_status {
	struct _ad5460_live_status status_bits;
	uint16_t value;
};

struct ad5460_init_param {
	uint8_t dev_addr;
	struct no_os_spi_init_param spi_ip;
	struct no_os_gpio_init_param *reset_gpio_param;
};

/**
 * @brief Device channel state.
 */

struct ad5460_channel_config {
	bool enabled;
	enum ad5460_op_mode function;
	enum ad5460_vout_range vout_range;
	enum ad5460_iout_range iout_range;
	enum ad5460_i_limit i_limit;
};

/**
 * @brief AD5460 device descriptor.
 */
struct ad5460_desc {
	uint8_t dev_addr;
	struct no_os_spi_desc *spi_desc;
	uint8_t comm_buff[AD5460_FRAME_SIZE];
	struct ad5460_channel_config channel_configs[AD5460_N_CHANNELS];
	struct no_os_gpio_desc *reset_gpio;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/** Converts a millivolt value in the corresponding DAC 13 bit code */
int ad5460_dac_voltage_to_code(struct ad5460_desc *, int32_t,
			       uint16_t *, uint32_t);

/** Converts a microamp value in the corresponding DAC 16 bit code */
int ad5460_dac_current_to_code(struct ad5460_desc *, uint32_t, uint16_t *,
			       uint32_t);

/** Write a register's value */
int ad5460_reg_write(struct ad5460_desc *, uint32_t, uint16_t);

/** Read a raw communication frame */
int ad5460_reg_read_raw(struct ad5460_desc *, uint32_t, uint8_t *);

/** Read a register's value */
int ad5460_reg_read(struct ad5460_desc *, uint32_t, uint16_t *);

/** Update a register's field */
int ad5460_reg_update(struct ad5460_desc *, uint32_t, uint16_t,
		      uint16_t);

/** Set the operation mode for a specific channel */
int ad5460_set_channel_function(struct ad5460_desc *,
				uint32_t, enum ad5460_op_mode);

/** Set the voltage range for a specific channel */
int ad5460_set_channel_vout_range(struct ad5460_desc *, uint32_t,
				  enum ad5460_vout_range);

/** Set the current range for a specific channel */
int ad5460_set_channel_iout_range(struct ad5460_desc *, uint32_t,
				  enum ad5460_iout_range);

/** Set the current limit for a specific DAC channel in vout mode */
int ad5460_set_channel_i_limit(struct ad5460_desc *, uint32_t,
			       enum ad5460_i_limit);

/** Set and load a code for the DAC on a specific channel */
int ad5460_set_channel_dac_code(struct ad5460_desc *, uint32_t, uint16_t);

/** Set which diagnostic value to be loaded in the DIAG_RESULT register */
int ad5460_set_diag(struct ad5460_desc *, uint32_t,
		    enum ad5460_diag_mode);

/** Get the GPIO value for a specific channel */
int ad5460_gpio_get(struct ad5460_desc *, uint32_t, uint8_t *);

/** Set the GPIO operation mode */
int ad5460_set_gpio_config(struct ad5460_desc *, uint32_t,
			   enum ad5460_gpio_select);

/** Set the logic value of a GPO pin */
int ad5460_gpio_set(struct ad5460_desc *, uint32_t, uint8_t);

/** Read the live status bits */
int ad5460_get_live(struct ad5460_desc *,
		    union ad5460_live_status *);

/** Configure and enable slew rate control for a DAC on a specific channel */
int ad5460_dac_slew_enable(struct ad5460_desc *, uint32_t,
			   enum ad5460_slew_lin_step,
			   enum ad5460_lin_rate);

/** Disable the slew rate control */
int ad5460_dac_slew_disable(struct ad5460_desc *, uint32_t);

/** Enable or disable the higher thermal reset */
int ad5460_set_therm_rst(struct ad5460_desc *, bool);

/** Perform a soft reset */
int ad5460_reset(struct ad5460_desc *);

/** Initialize the device structure */
int ad5460_init(struct ad5460_desc **, struct ad5460_init_param *);

/** Free the device descriptor */
int ad5460_remove(struct ad5460_desc *desc);

#endif // _AD5460_H
