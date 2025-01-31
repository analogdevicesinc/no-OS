/***************************************************************************//**
 *   @file   ltc7841.h
 *   @brief  Header file of LTC7841 Driver.
 *   @author Marvin Cabuenas (marvinneil.cabuenas@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef __LTC7841_H__
#define __LTC7841_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "no_os_i2c.h"
#include "no_os_irq.h"
#include "no_os_print_log.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_units.h"
#include "no_os_alloc.h"

/** X is set based on the pull configuration of the ADDR pin */
#define TWO_BYTES_LENGTH                       2
#define REPEATED_START_MODE                    1
#define I2C_WRITE_LENGTH_FOR_READ              1
#define I2C_DEFAULT_ARRAY_SIZE                 2
#define STOP_MODE                              0
#define LTC7841_DEFAULT_ADDRESS_0              0x5A
#define LTC7841_DEFAULT_ADDRESS_1              0x5B
#define LTC7841_TOTAL_REGISTERS                0x16
#define LTC7841_BYTE_LENGTH                    1
#define LTC7841_WORD_LENGTH                    2
#define LTC7841_I2C_WR_FRAME_SIZE              3
/* This denotes the max voltage output of LTC7841 in millivolts*/
#define LTC7841_MAX_VOUT                       48000
#define LTC7841_7_BIT_ADDRESS_MASK             0x7F
#define LTC7841_OPERATION_OFF                  0x00
#define LTC7841_OPERATION_TURN_ON              0x80
#define LTC7841_OPERATION_MARGIN_LOW           0x98
#define LTC7841_OPERATION_MARGIN_HIGH          0xA8
/* Slew rate multiplier*/
#define LTC7841_NOMINAL_SLEW_RATE              0b00
#define LTC7841_SLOW_SLEW_RATE                 0b01
#define LTC7841_FAST_SLEW_RATE                 0b11
/* READ TEMPERATURE 1*/
#define LTC7841_INTERNAL_DIE_TEMPERATURE       0b0
#define LTC7841_TSNS_PIN_VOLTAGE               0b1
#define READ_TEMPERATURE_1_BIT                 2
/* MFR MARGIN MAX VALUE*/
#define MFR_MAX_VALUE                          0x1FF
/* MFR VOUT INITIALIZE TO HALF */
#define MFR_VOUT_INIT                          0x0FF
#define WRITE_ONLY_REGISTERS_NUMBER            2
#define ECOMM                                  9

extern const uint8_t ltc7841_write_only_registers[WRITE_ONLY_REGISTERS_NUMBER];

/* LTC7841 registers list */
#define  LTC7841_OPERATION                     1
#define  LTC7841_VOUT_MODE                     0x20
#define  LTC7841_STATUS_WORD                   0x79
#define  LTC7841_READ_VIN                      0x88
#define  LTC7841_READ_IIN                      0x89
#define  LTC7841_READ_VOUT                     0x8B
#define  LTC7841_READ_IOUT                     0x8C
#define  LTC7841_READ_TEMPERATURE_1            0x8D
#define  LTC7841_PMBUS_REVISION                0x98
#define  LTC7841_MFR_IOUT_PEAK                 0xD7
#define  LTC7841_MFR_VOUT_PEAK                 0xDD
#define  LTC7841_MFR_VIN_PEAK                  0xDE
#define  LTC7841_MFR_TEMEPRATURE1_PEAK         0xDF
#define  LTC7841_MFR_IIN_PEAK                  0xE1
#define  LTC7841_MFR_CLEAR_PEAKS               0xE3
#define  LTC7841_MFR_VOUT_MARGIN_HIGH          0xE5
#define  LTC7841_MFR_SPECIAL_ID                0xE7
#define  LTC7841_MFR_VOUT_COMMAND              0xE8
#define  LTC7841_MFR_CONFIG                    0xE9
#define  LTC7841_MFR_VOUT_MARGIN_LOW           0xED
#define  LTC7841_MFR_RAIL_ADDRESS              0xFA
#define  LTC7841_MFR_RESET                     0xFD

enum ltc7841_status_word_bit_placement {
	LTC7841_COMMUNICATION_FAILURE =    1,
	LTC7841_TEMPERATURE_FAULT =        2,
	LTC7841_OUTPUT_OVERVOLTAGE_FAULT = 5,
	LTC7841_OFF =                      6,
	LTC7841_PGOOD =                    11,
	LTC7841_OUTPUT_VOLTAGE_FAULT =     15
};

/**
 * @brief Initialization parameter for the device descriptor
 */
struct ltc7841_init_param {
	/* I2C */
	struct no_os_i2c_init_param *comm_param;
	/* IRQ controller */
	struct no_os_irq_init_param irq_param;
};

/**
 * @brief ltc7841 device descriptor
 */
struct ltc7841_desc {
	/* I2C */
	struct no_os_i2c_desc *comm_desc;
	/* IRQ controller */
	struct no_os_irq_ctrl_desc *irq_desc;
};
/** Read a register value */
int ltc7841_reg_read(struct ltc7841_desc *desc, uint8_t cmd, uint8_t * data);

/** Write a register value */
int ltc7841_reg_write(struct ltc7841_desc *desc, uint8_t cmd, uint16_t value);

/** Initialize the device structure */
int ltc7841_init(struct ltc7841_desc ** device,
		 struct ltc7841_init_param * init_param);

/** Free the device descriptor */
int ltc7841_remove(struct ltc7841_desc *);
/** Reset the LTC7841 device */
int ltc7841_reset(struct ltc7841_desc *desc);
/** Perform change of rail address of LTC7841 */
int ltc7841_change_rail_address(struct ltc7841_desc *desc, uint8_t addr);
/** Turn off LTC7841 */
int ltc7841_turn_off(struct ltc7841_desc *desc);
/** Turn on LTC7841 */
int ltc7841_turn_on(struct ltc7841_desc *desc);
/** Margin Low Operation for LTC7841 */
int ltc7841_operation_margin_low(struct ltc7841_desc *desc);
/** Margin High Operation for LTC7841 */
int ltc7841_operation_margin_high(struct ltc7841_desc *desc);
/** Change to Nominal Slew Rate for LTC7841 */
int ltc7841_change_to_nominal_slew_rate(struct ltc7841_desc *desc);
/** Change to Slow Slew Rate for LTC7841 */
int ltc7841_change_to_slow_slew_rate(struct ltc7841_desc *desc);
/** Change to Fast Slew Rate for LTC7841 */
int ltc7841_change_to_fast_slew_rate(struct ltc7841_desc *desc);
/** Change to Internal Die Temeperature Reading for LTC7841*/
int ltc7841_change_to_internal_die_temperature(struct ltc7841_desc *desc);
/** Change to TSNS PIN VOLTAGE for LTC7841*/
int ltc7841_change_to_tsns_pin_voltage(struct ltc7841_desc *desc);
/** Change Vout command for LTC7841*/
int ltc7841_change_vout_command(struct ltc7841_desc *desc, uint16_t vout_level);
/** Change MFR MARGIN LOW command for LTC7841*/
int ltc7841_change_margin_low_command(struct ltc7841_desc *desc,
				      uint16_t vout_level);
/** Change MFR MARGIN HIGH command for LTC7841*/
int ltc7841_change_margin_high_command(struct ltc7841_desc *desc,
				       uint16_t vout_level);
/** Clears the mfr peak data values of LTC7841 */
int ltc7841_mfr_clear_peaks(struct ltc7841_desc *desc);
/** Clears the STATUS_WORD BITS that are clearable of LTC7841 */
int ltc7841_clear_status_word_bits(struct ltc7841_desc *desc);
#endif /* LTC7841_H */
