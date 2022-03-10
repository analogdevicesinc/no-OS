/***************************************************************************//**
 *   @file   ad5758.h
 *   @brief  Header file for ad5758 Driver.
 *   @author SPopa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
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
#ifndef AD5758_H_
#define AD5758_H_

#include "no_os_gpio.h"
#include "no_os_spi.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
/*
 * Create a contiguous bitmask starting at bit position @l and ending at
 * position @h.
 */
#define NO_OS_GENMASK(h, l) \
		(((~0UL) - (1UL << (l)) + 1) & (~0UL >> (31 - (h))))
#define NO_OS_BIT(x)	(1UL << (x))

/* AD5758 registers definition */
#define AD5758_REG_NOP						0x00
#define AD5758_REG_DAC_INPUT					0x01
#define AD5758_REG_DAC_OUTPUT					0x02
#define AD5758_REG_CLEAR_CODE					0x03
#define AD5758_REG_USER_GAIN					0x04
#define AD5758_REG_USER_OFFSET					0x05
#define AD5758_REG_DAC_CONFIG					0x06
#define AD5758_REG_SW_LDAC					0x07
#define AD5758_REG_KEY						0x08
#define AD5758_REG_GP_CONFIG1					0x09
#define AD5758_REG_GP_CONFIG2					0x0A
#define AD5758_REG_DCDC_CONFIG1					0x0B
#define AD5758_REG_DCDC_CONFIG2					0x0C
#define AD5758_REG_WDT_CONFIG					0x0F
#define AD5758_REG_DIGITAL_DIAG_CONFIG				0x10
#define AD5758_REG_ADC_CONFIG					0x11
#define AD5758_REG_FAULT_PIN_CONFIG				0x12
#define AD5758_REG_TWO_STAGE_READBACK_SELECT			0x13
#define AD5758_REG_DIGITAL_DIAG_RESULTS				0x14
#define AD5758_REG_ANALOG_DIAG_RESULTS				0x15
#define AD5758_REG_STATUS					0x16
#define AD5758_REG_CHIP_ID					0x17
#define AD5758_REG_FREQ_MONITOR					0x18
#define AD5758_REG_DEVICE_ID_0					0x19
#define AD5758_REG_DEVICE_ID_1					0x1A
#define AD5758_REG_DEVICE_ID_2					0x1B
#define AD5758_REG_DEVICE_ID_3					0x1C

/* AD5758_REG_DAC_CONFIG */
#define AD5758_DAC_CONFIG_RANGE_MSK			NO_OS_GENMASK(3, 0)
#define AD5758_DAC_CONFIG_RANGE_MODE(x)			(((x) & 0xF) << 0)
#define AD5758_DAC_CONFIG_OVRNG_EN_MSK			NO_OS_BIT(4)
#define AD5758_DAC_CONFIG_OVRNG_EN_MODE(x)		(((x) & 0x1) << 4)
#define AD5758_DAC_CONFIG_INT_EN_MSK			NO_OS_BIT(5)
#define AD5758_DAC_CONFIG_INT_EN_MODE(x)		(((x) & 0x1) << 5)
#define AD5758_DAC_CONFIG_OUT_EN_MSK			NO_OS_BIT(6)
#define AD5758_DAC_CONFIG_OUT_EN_MODE(x)		(((x) & 0x1) << 6)
#define AD5758_DAC_CONFIG_RSET_EXT_EN_MSK		NO_OS_BIT(7)
#define AD5758_DAC_CONFIG_RSET_EXT_EN_MODE(x)		(((x) & 0x1) << 7)
#define AD5758_DAC_CONFIG_SR_EN_MSK			NO_OS_BIT(8)
#define AD5758_DAC_CONFIG_SR_EN_MODE(x)			(((x) & 0x1) << 8)
#define AD5758_DAC_CONFIG_SR_CLOCK_MSK			NO_OS_GENMASK(12, 9)
#define AD5758_DAC_CONFIG_SR_CLOCK_MODE(x)		(((x) & 0xF) << 9)
#define AD5758_DAC_CONFIG_SR_STEP_MSK			NO_OS_GENMASK(15, 13)
#define AD5758_DAC_CONFIG_SR_STEP_MODE(x)		(((x) & 0x7) << 13)

/* AD5758_REG_SW_LDAC */
#define AD5758_SW_LDAC_COMMAND				0x1DAC

/* AD5758_REG_KEY */
#define AD5758_KEY_CODE_RESET_1				0x15FA
#define AD5758_KEY_CODE_RESET_2				0xAF51
#define AD5758_KEY_CODE_SINGLE_ADC_CONV			0x1ADC
#define AD5758_KEY_CODE_RESET_WDT			0x0D06
#define AD5758_KEY_CODE_CALIB_MEM_REFRESH		0xFCBA

/* AD5758_REG_GP_CONFIG1 */
#define AD5758_GP_CONFIG1_OSC_STOP_DETECT_EN_MSK	NO_OS_BIT(2)
#define AD5758_GP_CONFIG1_OSC_STOP_DETECT_EN_MODE(x)	(((x) & 0x1) << 2)
#define AD5758_GP_CONFIG1_SPI_DIAG_QUIET_EN_MSK		NO_OS_BIT(3)
#define AD5758_GP_CONFIG1_SPI_DIAG_QUIET_EN_MODE(x)	(((x) & 0x1) << 3)
#define AD5758_GP_CONFIG1_CLEAR_NOW_EN_MSK		NO_OS_BIT(4)
#define AD5758_GP_CONFIG1_CLEAR_NOW_EN_MODE(x)		(((x) & 0x1) << 4)
#define AD5758_GP_CONFIG1_NEG_OFFSET_EN_MSK		NO_OS_BIT(5)
#define AD5758_GP_CONFIG1_NEG_OFFSET_EN_MODE(x)		(((x) & 0x1) << 5)
#define AD5758_GP_CONFIG1_HART_EN_MSK			NO_OS_BIT(6)
#define AD5758_GP_CONFIG1_HART_EN_MODE(x)		(((x) & 0x1) << 6)
#define AD5758_GP_CONFIG1_CLKOUT_FREQ_MSK		NO_OS_GENMASK(9, 7)
#define AD5758_GP_CONFIG1_CLKOUT_FREQ_MODE(x)		(((x) & 0x7) << 7)
#define AD5758_GP_CONFIG1_CLKOUT_CONFIG_MSK		NO_OS_GENMASK(11, 10)
#define AD5758_GP_CONFIG1_CLKOUT_CONFIG_MODE(x)		(((x) & 0x3) << 10)
#define AD5758_GP_CONFIG1_SET_TEMP_THRESHOLD_MSK	NO_OS_GENMASK(13, 12)
#define AD5758_GP_CONFIG1_SET_TEMP_THRESHOLD_MODE(x)	(((x) & 0x3) << 12)

/* AD5758_REG_GP_CONFIG2 */
#define AD5758_GP_CONFIG2_FAULT_TIMEOUT_MSK		NO_OS_BIT(9)
#define AD5758_GP_CONFIG2_FAULT_TIMEOUT_MODE(x)		(((x) & 0x1) << 9)
#define AD5758_GP_CONFIG2_GLOBAL_SW_LDAC_MSK		NO_OS_BIT(10)
#define AD5758_GP_CONFIG2_GLOBAL_SW_LDAC_MODE(x)	(((x) & 0x1) << 10)
#define AD5758_GP_CONFIG2_INT_I_MONITOR_EN_MSK		NO_OS_BIT(11)
#define AD5758_GP_CONFIG2_INT_I_MONITOR_EN_MODE(x)	(((x) & 0x1) << 11)
#define AD5758_GP_CONFIG2_COMPARATOR_CONFIG_MSK		NO_OS_GENMASK(14, 13)
#define AD5758_GP_CONFIG2_COMPARATOR_CONFIG_MODE(x)	(((x) & 0x3) << 13)

/* AD5758_REG_DCDC_CONFIG1 */
#define AD5758_DCDC_CONFIG1_DCDC_VPROG_MSK		NO_OS_GENMASK(4, 0)
#define AD5758_DCDC_CONFIG1_DCDC_VPROG_MODE(x)		(((x) & 0x1F) << 0)
#define AD5758_DCDC_CONFIG1_DCDC_MODE_MSK		NO_OS_GENMASK(6, 5)
#define AD5758_DCDC_CONFIG1_DCDC_MODE_MODE(x)		(((x) & 0x3) << 5)

/* AD5758_REG_DCDC_CONFIG2 */
#define AD5758_DCDC_CONFIG2_ILIMIT_MSK			NO_OS_GENMASK(3, 1)
#define AD5758_DCDC_CONFIG2_ILIMIT_MODE(x)		(((x) & 0x7) << 1)
#define AD5758_DCDC_CONFIG2_ADC_CONTROL_DIAG_MSK	NO_OS_GENMASK(5, 4)
#define AD5758_DCDC_CONFIG2_ADC_CONTROL_DIAG_MODE(x)	(((x) & 0x3) << 4)
#define AD5758_DCDC_CONFIG2_VIOUT_PULLDOWN_EN_MSK	NO_OS_BIT(6)
#define AD5758_DCDC_CONFIG2_VIOUT_PULLDOWN_EN_MODE(x)	(((x) & 0x1) << 6)
#define AD5758_DCDC_CONFIG2_SHORT_DEGLITCH_MSK		NO_OS_BIT(7)
#define AD5758_DCDC_CONFIG2_SHORT_DEGLITCH_MODE(x)	(((x) & 0x1) << 7)
#define AD5758_DCDC_CONFIG2_READ_COMP_DIS_MSK		NO_OS_BIT(10)
#define AD5758_DCDC_CONFIG2_READ_COMP_DIS_MODE(x)	(((x) & 0x1) << 10)
#define AD5758_DCDC_CONFIG2_INTR_SAT_3WI_MSK		NO_OS_BIT(11)
#define AD5758_DCDC_CONFIG2_BUSY_3WI_MSK		NO_OS_BIT(12)

/* AD5758_REG_WDT_CONFIG */
#define AD5758_WDT_CONFIG_WDT_TIMEOUT_MSK			NO_OS_GENMASK(3, 0)
#define AD5758_WDT_CONFIG_WDT_TIMEOUT_MODE(x)			(((x) & 0xF) << 0)
#define AD5758_WDT_CONFIG_WDT_EN_MSK				NO_OS_BIT(6)
#define AD5758_WDT_CONFIG_WDT_EN_MODE(x)			(((x) & 0x1) << 6)
#define AD5758_WDT_CONFIG_KICK_ON_VALID_WRITE_MSK		NO_OS_BIT(8)
#define AD5758_WDT_CONFIG_KICK_ON_VALID_WRITE_MODE(x)		(((x) & 0x1) << 8)
#define AD5758_WDT_CONFIG_RESET_ON_WDT_FAIL_MSK			NO_OS_BIT(9)
#define AD5758_WDT_CONFIG_RESET_ON_WDT_FAIL_MODE(x)		(((x) & 0x1) << 9)
#define AD5758_WDT_CONFIG_CLEAR_ON_WDT_FAIL_MSK			NO_OS_BIT(10)
#define AD5758_WDT_CONFIG_CLEAR_ON_WDT_FAIL_MODE(x)		(((x) & 0x1) << 10)

/* AD5758_REG_DIGITAL_DIAG_CONFIG */
#define AD5758_DIG_DIAG_CONFIG_SPI_CRC_EN_MSK			NO_OS_BIT(0)
#define AD5758_DIG_DIAG_CONFIG_SPI_CRC_EN_MODE(x)		(((x) & 0x1) << 0)
#define AD5758_DIG_DIAG_CONFIG_FREQ_MON_EN_MSK			NO_OS_BIT(2)
#define AD5758_DIG_DIAG_CONFIG_FREQ_MON_EN_MODE(x)		(((x) & 0x1) << 2)
#define AD5758_DIG_DIAG_CONFIG_CAL_MEM_CRC_EN_MSK		NO_OS_BIT(3)
#define AD5758_DIG_DIAG_CONFIG_CAL_MEM_CRC_EN_MODE(x)		(((x) & 0x1) << 3)
#define AD5758_DIG_DIAG_CONFIG_INV_DAC_CHECK_EN_MSK		NO_OS_BIT(4)
#define AD5758_DIG_DIAG_CONFIG_INV_DAC_CHECK_EN_MODE(x)		(((x) & 0x1) << 4)
#define AD5758_DIG_DIAG_CONFIG_DAC_LATCH_MON_EN_MSK		NO_OS_BIT(6)
#define AD5758_DIG_DIAG_CONFIG_DAC_LATCH_MON_EN_MODE(x)		(((x) & 0x1) << 6)

/* AD5758_REG_ADC_CONFIG */
#define AD5758_ADC_CONFIG_ADC_IP_SELECT_MSK			NO_OS_GENMASK(4, 0)
#define AD5758_ADC_CONFIG_ADC_IP_SELECT_MODE(x)			(((x) & 0x1F) << 0)
#define AD5758_ADC_CONFIG_SEQUENCE_DATA_MSK			NO_OS_GENMASK(7, 5)
#define AD5758_ADC_CONFIG_SEQUENCE_DATA_MODE(x)			(((x) & 0x7) << 5)
#define AD5758_ADC_CONFIG_SEQUENCE_COMMAND_MSK			NO_OS_GENMASK(10, 8)
#define AD5758_ADC_CONFIG_SEQUENCE_COMMAND_MODE(x)		(((x) & 0x7) << 8)
#define AD5758_ADC_CONFIG_PPC_BUF_MSK				NO_OS_BIT(11)
#define AD5758_ADC_CONFIG_PPC_BUF_EN(x)				(((x) & 0x1) << 11)

/* AD5758_REG_FAULT_PIN_CONFIG */
#define AD5758_FAULT_PIN_CONFIG_MAIN_DIE_TEMP_ERR_MSK		NO_OS_BIT(0)
#define AD5758_FAULT_PIN_CONFIG_MAIN_DIE_TEMP_ERR_MODE(x) 	(((x) & 0x1) << 0)
#define AD5758_FAULT_PIN_CONFIG_DCDC_DIE_TEMP_ERR_MSK		NO_OS_BIT(1)
#define AD5758_FAULT_PIN_CONFIG_DCDC_DIE_TEMP_ERR_MODE(x)	(((x) & 0x1) << 1)
#define AD5758_FAULT_PIN_CONFIG_VOUT_SC_ERR_MSK			NO_OS_BIT(2)
#define AD5758_FAULT_PIN_CONFIG_VOUT_SC_ERR_MODE(x)		(((x) & 0x1) << 2)
#define AD5758_FAULT_PIN_CONFIG_IOUT_OC_ERR_MSK			NO_OS_BIT(3)
#define AD5758_FAULT_PIN_CONFIG_IOUT_OC_ERR_MODE(x)		(((x) & 0x1) << 3)
#define AD5758_FAULT_PIN_CONFIG_DCDC_P_SC_ERR_MSK		NO_OS_BIT(4)
#define AD5758_FAULT_PIN_CONFIG_DCDC_P_SC_ERR_MODE(x)		(((x) & 0x1) << 4)
#define AD5758_FAULT_PIN_CONFIG_SPI_CRC_ERR_MSK			NO_OS_BIT(6)
#define AD5758_FAULT_PIN_CONFIG_SPI_CRC_ERR_MODE(x)		(((x) & 0x1) << 6)
#define AD5758_FAULT_PIN_CONFIG_SLIPBIT_ERR_MSK			NO_OS_BIT(7)
#define AD5758_FAULT_PIN_CONFIG_SLIPBIT_ERR_MODE(x)		(((x) & 0x1) << 7)
#define AD5758_FAULT_PIN_CONFIG_WDT_ERR_MSK			NO_OS_BIT(8)
#define AD5758_FAULT_PIN_CONFIG_WDT_ERR_MODE(x)			(((x) & 0x1) << 8)
#define AD5758_FAULT_PIN_CONFIG_DAC_LATCH_MON_ERR_MSK		NO_OS_BIT(9)
#define AD5758_FAULT_PIN_CONFIG_DAC_LATCH_MON_ERR_MODE(x) 	(((x) & 0x1) << 9)
#define AD5758_FAULT_PIN_CONFIG_OSC_STOP_DETECT_MSK		NO_OS_BIT(10)
#define AD5758_FAULT_PIN_CONFIG_OSC_STOP_DETECT_MODE(x)		(((x) & 0x1) << 10)
#define AD5758_FAULT_PIN_CONFIG_INV_DAC_CHECK_ERR_MSK		NO_OS_BIT(12)
#define AD5758_FAULT_PIN_CONFIG_INV_DAC_CHECK_ERR_MODE(x) 	(((x) & 0x1) << 12)
#define AD5758_FAULT_PIN_CONFIG_PROT_SW_ERR_MSK			NO_OS_BIT(14)
#define AD5758_FAULT_PIN_CONFIG_PROT_SW_ERR_MODE(x)		(((x) & 0x1) << 14)
#define AD5758_FAULT_PIN_CONFIG_SPI_ACC_ERR_MSK			NO_OS_BIT(15)
#define AD5758_FAULT_PIN_CONFIG_SPI_ACC_ERR_MODE(x)		(((x) & 0x1) << 15)

/* AD5758_REG_TWO_STAGE_READBACK_SELECT */
#define AD5758_TWO_STAGE_READBACK_SELECT_MSK			NO_OS_GENMASK(4, 0)
#define AD5758_TWO_STAGE_READBACK_SELECT_MODE(x)		(((x) & 0x1F) << 0)
#define AD5758_TWO_STAGE_READBACK_SELECT_MODE_MSK		NO_OS_GENMASK(6, 5)
#define AD5758_TWO_STAGE_READBACK_SELECT_MODE_MODE(x)		(((x) & 0x3) << 5)

/* AD5758_REG_DIGITAL_DIAG_RESULTS */
#define AD5758_DIG_DIAG_RESULTS_SPI_CRC_ERR			NO_OS_BIT(0)
#define AD5758_DIG_DIAG_RESULTS_SLIPBIT_ERR_MSK			NO_OS_BIT(1)
#define AD5758_DIG_DIAG_RESULTS_SCLK_COUNT_ERR_MSK		NO_OS_BIT(2)
#define AD5758_DIG_DIAG_RESULTS_INVALID_SPI_ACC_ERR_MSK		NO_OS_BIT(4)
#define AD5758_DIG_DIAG_RESULTS_CAL_MEM_CRC_ERR_MSK		NO_OS_BIT(5)
#define AD5758_DIG_DIAG_RESULTS_INV_DAC_CHECK_ERR_MSK		NO_OS_BIT(6)
#define AD5758_DIG_DIAG_RESULTS_DAC_LATCH_MON_ERR_MSK		NO_OS_BIT(8)
#define AD5758_DIG_DIAG_RESULTS_3WI_RC_ERR_MSK			NO_OS_BIT(9)
#define AD5758_DIG_DIAG_RESULTS_WDT_ERR_MSK			NO_OS_BIT(11)
#define AD5758_DIG_DIAG_RESULTS_ERR_3WI_MSK			NO_OS_BIT(12)
#define AD5758_DIG_DIAG_RESULTS_RES_OCCURRED_MSK		NO_OS_BIT(13)
#define AD5758_DIG_DIAG_RESULTS_SLEW_BUSY_MSK			NO_OS_BIT(14)
#define AD5758_DIG_DIAG_RESULTS_CAL_MEM_UNREFRESHED_MSK		NO_OS_BIT(15)

/* AD5758_REG_ANALOG_DIAG_RESULTS */
#define AD5758_ANA_DIAG_RESULTS_REGOUT_ERR_MSK			NO_OS_BIT(0)
#define AD5758_ANA_DIAG_RESULTS_INT_AVCC_ERR_MSK		NO_OS_BIT(1)
#define AD5758_ANA_DIAG_RESULTS_REFIN_ERR_MSK			NO_OS_BIT(2)
#define AD5758_ANA_DIAG_RESULTS_REFOUT_ERR_MSK			NO_OS_BIT(3)
#define AD5758_ANA_DIAG_RESULTS_MAIN_DIE_TEMP_ERR_MSK		NO_OS_BIT(4)
#define AD5758_ANA_DIAG_RESULTS_DCDC_DIE_TEMP_ERR_MSK		NO_OS_BIT(5)
#define AD5758_ANA_DIAG_RESULTS_VOUT_SC_ERR_MSK			NO_OS_BIT(6)
#define AD5758_ANA_DIAG_RESULTS_IOUT_OC_ERR_MSK			NO_OS_BIT(7)
#define AD5758_ANA_DIAG_RESULTS_DCDC_P_PWR_ERR_MSK		NO_OS_BIT(9)
#define AD5758_ANA_DIAG_RESULTS_DCDC_P_SC_ERR_MSK		NO_OS_BIT(11)
#define AD5758_ANA_DIAG_RESULTS_FAULT_PROT_SW_ERR_MSK		NO_OS_BIT(13)

/* AD5758_REG_STATUS */
#define AD5758_STATUS_ADC_DATA_MSK			NO_OS_GENMASK(11, 0)
#define AD5758_STATUS_ADC_CH_MSK			NO_OS_GENMASK(16, 12)
#define AD5758_STATUS_ADC_BUSY_MSK			NO_OS_BIT(17)
#define AD5758_STATUS_WDT_STATUS_MSK			NO_OS_BIT(18)
#define AD5758_STATUS_ANA_DIAG_STATUS_MSK		NO_OS_BIT(19)
#define AD5758_STATUS_DIG_DIAG_STATUS_MSK		NO_OS_BIT(20)

#define AD5758_REG_WRITE(x) 		((0x80) | (x & 0x1F))
#define AD5758_CRC8_POLY		0x07 // x^8 + x^2 + x^1 + x^0

/*****************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
enum ad5758_dc_dc_mode {
	DC_DC_POWER_OFF,
	DPC_CURRENT_MODE,
	DPC_VOLTAGE_MODE,
	PPC_CURRENT_MODE,
};

enum ad5758_dig_diag_flags {
	DIAG_SPI_CRC_ERR = 0,
	DIAG_SLIPBIT_ERR = 1,
	DIAG_SCLK_COUNT_ERR = 2,
	DIAG_INVALID_SPI_ACCESS_ERR = 4,
	DIAG_CAL_MEM_CRC_ERR = 5,
	DIAG_INVERSE_DAC_CHECK_ERR = 6,
	DIAG_DAC_LATCH_MON_ERR = 8,
	DIAG_THREE_WI_RC_ERR = 9,
	DIAG_WDT_ERR = 11,
	DIAG_ERR_3WI = 12,
	DIAG_RESET_OCCURRED = 13,
};

enum ad5758_clkout_config {
	CLKOUT_DISABLE,
	CLKOUT_ENABLE,
};

enum ad5758_clkout_freq {
	CLKOUT_FREQ_416_KHZ,
	CLKOUT_FREQ_435_KHZ,
	CLKOUT_FREQ_454_KHZ,
	CLKOUT_FREQ_476_KHZ,
	CLKOUT_FREQ_500_KHZ,
	CLKOUT_FREQ_526_KHZ,
	CLKOUT_FREQ_555_KHZ,
	CLKOUT_FREQ_588_KHZ,
};

enum ad5758_slew_rate_clk {
	SR_CLOCK_240_KHZ,
	SR_CLOCK_200_KHZ,
	SR_CLOCK_150_KHZ,
	SR_CLOCK_128_KHZ,
	SR_CLOCK_64_KHZ,
	SR_CLOCK_32_KHZ,
	SR_CLOCK_16_KHZ,
	SR_CLOCK_8_KHZ,
	SR_CLOCK_4_KHZ,
	SR_CLOCK_2_KHZ,
	SR_CLOCK_1_KHZ,
	SR_CLOCK_512_HZ,
	SR_CLOCK_256_HZ,
	SR_CLOCK_128_HZ,
	SR_CLOCK_64_HZ,
	SR_CLOCK_16_HZ,
};

enum ad5758_dc_dc_ilimt {
	ILIMIT_150_mA,
	ILIMIT_200_mA,
	ILIMIT_250_mA,
	ILIMIT_300_mA,
	ILIMIT_350_mA,
	ILIMIT_400_mA,
};

enum ad5758_output_range {
	RANGE_0V_5V = 0,
	RANGE_0V_10V = 1,
	RANGE_M5V_5V = 2,
	RANGE_M10V_10V = 3,
	RANGE_0mA_20mA = 8,
	RANGE_0mA_24mA = 9,
	RANGE_4mA_24mA = 10,
	RANGE_M20mA_20mA = 11,
	RANGE_M24mA_24mA = 12,
	RANGE_M1mA_22mA = 13,
};

enum ad5758_adc_ip {
	ADC_IP_MAIN_DIE_TEMP = 0,
	ADC_IP_DCDC_DIE_TEMP = 1,
	ADC_IP_REFIN = 3,
	ADC_IP_REF2 = 4,
	ADC_IP_VSENSE = 13,
	ADC_IP_MVSENSE = 14,
	ADC_IP_INT_AVCC = 20,
	ADC_IP_REGOUT = 21,
	ADC_IP_VLOGIC = 22,
	ADC_IP_INT_CURR_MON_VOUT = 23,
	ADC_IP_REFGND = 24,
	ADC_IP_AGND = 25,
	ADC_IP_DGND = 26,
	ADC_IP_VDPC = 27,
	ADC_IP_AVDD2 = 28,
	ADC_IP_AVSS = 29,
	ADC_IP_DCDC_DIE_NODE = 30,
	ADC_IP_REFOUT = 31,
};

enum ad5758_adc_mode {
	ADC_MODE_KEY_SEQ = 2,
	ADC_MODE_AUTO_SEQ = 3,
	ADC_MODE_SINGLE_CONV = 4,
	ADC_MODE_SINGLE_KEY_CONV = 5,
};

struct ad5758_dev {
	/* SPI */
	struct no_os_spi_desc *spi_desc;
	/* GPIO */
	struct no_os_gpio_desc *reset_n;
	struct no_os_gpio_desc *ldac_n;
	/* Device Settings */
	uint8_t crc_en;
	enum ad5758_dc_dc_mode dc_dc_mode;
	enum ad5758_clkout_config clkout_config;
	enum ad5758_clkout_freq clkout_freq;
	enum ad5758_dc_dc_ilimt dc_dc_ilimit;
	enum ad5758_output_range output_range;
	enum ad5758_slew_rate_clk slew_rate_clk;
};

struct ad5758_init_param {
	/* SPI */
	struct no_os_spi_init_param spi_init;
	/* GPIO */
	struct no_os_gpio_init_param reset_n;
	struct no_os_gpio_init_param ldac_n;
	/* Device Settings */
	uint8_t crc_en;
	enum ad5758_dc_dc_mode dc_dc_mode;
	enum ad5758_clkout_config clkout_config;
	enum ad5758_clkout_freq clkout_freq;
	enum ad5758_dc_dc_ilimt dc_dc_ilimit;
	enum ad5758_output_range output_range;
	enum ad5758_slew_rate_clk slew_rate_clk;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t ad5758_set_crc(struct ad5758_dev *dev, uint8_t crc_en);
int32_t ad5758_wait_for_refresh_cycle(struct ad5758_dev *dev);
int32_t ad5758_soft_reset(struct ad5758_dev *dev);
int32_t ad5758_calib_mem_refresh(struct ad5758_dev *dev);
int32_t ad5758_set_dc_dc_conv_mode(struct ad5758_dev *dev,
				   enum ad5758_dc_dc_mode mode);
int32_t ad5758_set_dc_dc_ilimit(struct ad5758_dev *dev,
				enum ad5758_dc_dc_ilimt ilimit);
int32_t ad5758_fault_prot_switch_en(struct ad5758_dev *dev, uint8_t enable);
int32_t ad5758_internal_buffers_en(struct ad5758_dev *dev, uint8_t enable);
int32_t ad5758_set_out_range(struct ad5758_dev *dev,
			     enum ad5758_output_range range);
int32_t ad5758_slew_rate_config(struct ad5758_dev *dev,
				enum ad5758_slew_rate_clk clk,
				uint8_t enable);
int32_t ad5758_dac_input_write(struct ad5758_dev *dev, uint16_t code);
int32_t ad5758_dac_output_en(struct ad5758_dev *dev, uint8_t enable);
int32_t ad5758_clear_dig_diag_flag(struct ad5758_dev *dev,
				   enum ad5758_dig_diag_flags flag);
int32_t ad5758_select_adc_ip(struct ad5758_dev *dev,
			     enum ad5758_adc_ip adc_ip_sel);
int32_t ad5758_set_clkout_config(struct ad5758_dev *dev,
				 enum ad5758_clkout_config config,
				 enum ad5758_clkout_freq freq);
int32_t ad5758_select_adc_depth(struct ad5758_dev *dev,
				uint8_t num_of_channels);
int32_t ad5758_set_adc_channel_input(struct ad5758_dev *dev,
				     uint8_t channel,
				     enum ad5758_adc_ip adc_ip_sel);
int32_t ad5758_set_adc_mode(struct ad5758_dev *dev,
			    enum ad5758_adc_mode adc_mode,
			    uint8_t enable);
int32_t ad5758_init(struct ad5758_dev **device,
		    struct ad5758_init_param *init_param);
#endif /* AD5758_H_ */
