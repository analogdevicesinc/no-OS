/***************************************************************************//**
 *   @file   AD5755.h
 *   @brief  Header file of AD5755 Driver. This driver supporting the following
 *           devices: AD5755, AD5755-1 and AD5757
 *   @author Istvan Csomortani (istvan.csomortani@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
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
#ifndef __AD5755_H__
#define __AD5755_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"

/******************************************************************************/
/******************* Macros and Constants Definitions *************************/
/******************************************************************************/

/******************************************************************************/
/******************************** AD5755 **************************************/
/******************************************************************************/

/* LDAC */
#define AD5755_LDAC_OUT        no_os_gpio_direction_output(dev->gpio_ldac,  \
			       NO_OS_GPIO_HIGH);
#define AD5755_LDAC_LOW        no_os_gpio_set_value(dev->gpio_ldac,         \
			       NO_OS_GPIO_LOW)
#define AD5755_LDAC_HIGH       no_os_gpio_set_value(dev->gpio_ldac,         \
			       NO_OS_GPIO_HIGH)

/* RESET */
#define AD5755_RESET_OUT       no_os_gpio_direction_output(dev->gpio_rst,  \
			       NO_OS_GPIO_HIGH);
#define AD5755_RESET_LOW       no_os_gpio_set_value(dev->gpio_rst,         \
			       NO_OS_GPIO_LOW)
#define AD5755_RESET_HIGH      no_os_gpio_set_value(dev->gpio_rst,         \
			       NO_OS_GPIO_HIGH)

/* CLEAR */
#define AD5755_CLEAR_OUT        no_os_gpio_direction_output(dev->gpio_clr, \
			        NO_OS_GPIO_HIGH);
#define AD5755_CLEAR_LOW        no_os_gpio_set_value(dev->gpio_clr,        \
			        NO_OS_GPIO_LOW)
#define AD5755_CLEAR_HIGH       no_os_gpio_set_value(dev->gpio_clr,        \
			        NO_OS_GPIO_HIGH)

/* POC */
#define AD5755_POC_OUT          no_os_gpio_direction_output(dev->gpio_poc, \
			        NO_OS_GPIO_HIGH);
#define AD5755_POC_LOW          no_os_gpio_set_value(dev->gpio_poc,        \
			        NO_OS_GPIO_LOW)
#define AD5755_POC_HIGH         no_os_gpio_set_value(dev->gpio_poc,        \
			        NO_OS_GPIO_HIGH)

/* Input Shift Register Contents for a Write Operation. */
#define AD5755_ISR_WRITE            (0ul << 23)           /* R/nW */
#define AD5755_ISR_DUT_AD1(x)       (((x) & 0x1) << 22)   /* Device AddrBit1*/
#define AD5755_ISR_DUT_AD0(x)       (((x) & 0x1) << 21)   /* Device AddrBit0*/
#define AD5755_ISR_DREG(x)          (((x) & 0x7) << 18)   /* Register AddrBits*/
#define AD5755_ISR_DAC_AD(x)        (((x) & 0x3) << 16)   /* Channel AddrBits */
#define AD5755_ISR_DATA(x)          ((x) & 0xFFFF)        /* Data Bits*/

/* Nop operation code. */
#define AD5755_ISR_NOP              0x1CE000

/* AD5755_ISR_DREG(x) options. (Register addresses) */
#define AD5755_DREG_WR_DAC          0
#define AD5755_DREG_WR_GAIN         2
#define AD5755_DREG_WR_GAIN_ALL     3
#define AD5755_DREG_WR_OFFSET       4
#define AD5755_DREG_WR_OFFSET_ALL   5
#define AD5755_DREG_WR_CLR_CODE     6
#define AD5755_DREG_WR_CTRL_REG     7

/* AD5755_ISR_DAC_AD(x) options. (Channel addresses) */
#define AD5755_DAC_A            0
#define AD5755_DAC_B            1
#define AD5755_DAC_C            2
#define AD5755_DAC_D            3

/* Gain register definition. */
#define AD5755_GAIN_ADJUSTMENT(x)       ((x) & 0xFFFF)

/* Offset register definition. */
#define AD5755_OFFSET_ADJUSTMENT(x)     ((x) & 0xFFFF)

/* Clear Code Register definition. */
#define AD5755_CLEAR_CODE(x)            ((x) & 0xFFFF)

/* Control Register definition. */
#define AD5755_CTRL_CREG(x)             (((x) & 0x7) << 13)
#define AD5755_CTRL_DATA(x)             ((x) & 0x1FFF)

/* AD5755_CTRL_CREG(x) options. */
#define AD5755_CREG_SLEW        0 // Slew rate control register(one per channel)
#define AD5755_CREG_MAIN        1 // Main control register
#define AD5755_CREG_DAC         2 // DAC control register(one per channel)
#define AD5755_CREG_DC_DC       3 // DC-to-dc control register
#define AD5755_CREG_SOFT        4 // Software register

/* Slew Rate Control Register definition. */
#define AD5755_SLEW_SREN            (1 << 12)
#define AD5755_SLEW_SR_CLOCK(x)     (((x) & 0xF) << 3)
#define AD5755_SLEW_SR_STEP(x)      (((x) & 0x7) << 0)

/* AD5755_SLEW_SR_CLOCK(x) options. */
#define AD5755_SR_CLK_64K       0
#define AD5755_SR_CLK_32k       1
#define AD5755_SR_CLK_16k       2
#define AD5755_SR_CLK_8K        3
#define AD5755_SR_CLK_4K        4
#define AD5755_SR_CLK_2K        5
#define AD5755_SR_CLK_1K        6
#define AD5755_SR_CLK_500       7
#define AD5755_SR_CLK_250       8
#define AD5755_SR_CLK_125       9
#define AD5755_SR_CLK_64        10
#define AD5755_SR_CLK_32        11
#define AD5755_SR_CLK_16        12
#define AD5755_SR_CLK_8         13
#define AD5755_SR_CLK_4         14
#define AD5755_SR_CLK_0_5       15

/* AD5755_SLEW_SR_STEP(x) options. */
#define AD5755_STEP_1       0
#define AD5755_STEP_2       1
#define AD5755_STEP_4       2
#define AD5755_STEP_16      3
#define AD5755_STEP_32      4
#define AD5755_STEP_64      5
#define AD5755_STEP_128     6
#define AD5755_STEP_256     7

/* Main Control Register definition. */
#define AD5755_MAIN_POC             (1 << 12)
#define AD5755_MAIN_STATREAD        (1 << 11)
#define AD5755_MAIN_EWD             (1 <<10)
#define AD5755_MAIN_WD(x)           (((x) & 0x3) << 8)
#define AD5755_MAIN_SHTCCTLIM(x)    (((x) & 0x1) << 6)
#define AD5755_MAIN_OUTEN_ALL       (1 << 5)
#define AD5755_MAIN_DCDC_ALL        (1 << 4)

/* AD5755_MAIN_WD(x) options. */
#define AD5755_WD_5MS               0 // 5 ms timeout period
#define AD5755_WD_10MS              1 // 10 ms timeout period
#define AD5755_WD_100MS             2 // 100 ms timeout period
#define AD5755_WD_200MS             3 // 200 ms timeout period

/* AD5755_MAIN_SHTCCTLIM(x) options. */
#define AD5755_LIMIT_16_MA          0 // 16 mA (default)
#define AD5755_LIMIT_8_MA           1 // 8 mA

/* DAC Control Register definition. */
#define AD5755_DAC_INT_ENABLE       (1 << 8)
#define AD5755_DAC_CLR_EN           (1 << 7)
#define AD5755_DAC_OUTEN            (1 << 6)
#define AD5755_DAC_RSET             (1 << 5)
#define AD5755_DAC_DC_DC            (1 << 4)
#define AD5755_DAC_OVRNG            (1 << 3)
#define AD5755_DAC_R(x)             ((x) & 0x7)

/* AD5755_DAC_R(x) options. */
#define AD5755_R_0_5_V              0 // 0 V to 5 V voltage range (default)
#define AD5755_R_0_10_V             1 // 0 V to 10 V voltage range
#define AD5755_R_M5_P5_V            2 // -5 V to +5 V voltage range
#define AD5755_R_M10_P10_V          3 // -10 V to 10 V voltage range
#define AD5755_R_4_20_MA            4 // 4 mA to 20 mA current range
#define AD5755_R_0_20_MA            5 // 0 mA to 20 mA current range
#define AD5755_R_0_24_MA            6 // 0 mA to 24 mA current range

/* DC-to-DC Control Register definition. */
#define AD5755_DC_DC_COMP           (1 << 6)
#define AD5755_DC_DC_PHASE(x)       (((x) & 0x3) << 4)
#define AD5755_DC_DC_FREQ(x)        (((x) & 0x3) << 2)
#define AD5755_DC_DC_MAX_V(x)       (((x) & 0x3) << 0)

/* AD5755_DC_DC_PHASE(x) options. */
#define AD5755_PHASE_ALL_DC_DC      0 // all dc-dc converters clock on same edge
#define AD5755_PHASE_AB_CD          1 // Ch A,B clk same edge, C,D opposite edge
#define AD5755_PHASE_AC_BD          2 // Ch A,C clk same edge, B,D opposite edge
#define AD5755_PHASE_A_B_C_D_90     3 // A,B,C,D clock 90 degree out of phase

/* AD5755_DC_DC_FREQ(x) options. */
#define AD5755_FREQ_250_HZ          0 // 250 +/- 10% kHz
#define AD5755_FREQ_410_HZ          1 // 410 +/- 10% kHz
#define AD5755_FREQ_650_HZ          2 // 650 +/- 10% kHz

/* AD5755_DC_DC_MAX_V(x) options. */
#define AD5755_MAX_23V          0 // 23 V + 1 V/-1.5 V (default)
#define AD5755_MAX_24_5V        1 // 24.5 V +/- 1 V
#define AD5755_MAX_27V          2 // 27 V +/- 1 V
#define AD5755_MAX_29_5V        3 // 29.5 V +/- 1V

/* Software Register definition. */
#define AD5755_SOFT_USER_BIT        (1 << 12)
#define AD5755_SOFT_RESET_CODE(x)   ((x) & 0xFFF)

/* AD5755_SOFT_RESET_CODE(x) options. */
#define AD5755_RESET_CODE       0x555 // Performs a reset of the AD5755.
#define AD5755_SPI_CODE         0x195 // If watchdog is enabled, 0x195 must be
// written to the software register within
// the programmed timeout period.

/* Input Shift Register Contents for a Read Operation. */
#define AD5755_ISR_READ             (1 << 23)
/* Same as Input Shift Register Contents for a Write Operation. */
/*
#define AD5755_ISR_DUT_AD1(x)       (((x) & 0x1) << 22)
#define AD5755_ISR_DUT_AD0(x)       (((x) & 0x1) << 21)
*/
#define AD5755_ISR_RD(x)            (((x) & 0x1F) << 16)

/* AD5755_ISR_RD(x) options. (Read address decoding) */
#define AD5755_RD_DATA_REG(x)           (((x) & 0x3) + 0)
#define AD5755_RD_CTRL_REG(x)           (((x) & 0x3) + 4)
#define AD5755_RD_GAIN_REG(x)           (((x) & 0x3) + 8)
#define AD5755_RD_OFFSET_REG(x)         (((x) & 0x3) + 12)
#define AD5755_RD_CODE_REG(x)           (((x) & 0x3) + 16)
#define AD5755_RD_SR_CTRL_REG(x)        (((x) & 0x3) + 20)
#define AD5755_RD_STATUS_REG            24
#define AD5755_RD_MAIN_CTRL_REG         25
#define AD5755_RD_Dc_DC_CTRL_REG        26

/* Status Register definition. */
/* channelA = 0 ... channelD = 3 */
#define AD5755_STATUS_DC_DC(x)          (1 << (12 + (x)))
#define AD5755_STATUS_USER_BIT          (1 << 11)
#define AD5755_STATUS_PEC_ERROR         (1 << 10)
#define AD5755_STATUS_RAMP_ACTIVE       (1 << 9)
#define AD5755_STATUS_OVER_TEMP         (1 << 8)
/* channelA = 0 ... channelD = 3 */
#define AD5755_STATUS_VOUT_FAULT(x)     (1 << (4 + (x)))
#define AD5755_STATUS_IOUT_FAULT(x)     (1 << (0 + (x)))

#define AD5755_CRC_POLYNOMIAL   0x07    // P(x)=x^8+x^2+x^1+1 = 100000111
#define AD5755_CRC_CHECK_CODE   0x00

/*****************************************************************************/
/************************** Types Declarations *******************************/
/*****************************************************************************/
/**
 * @struct ad5755_setup
 * @brief Stores the settings that will be written to
 *        the device when the "AD5755_Init" functions is called.
 */
struct ad5755_setup {
	/** Reflects the logic state of the external pin AD0. Range 0..1 */
	uint8_t pin_ad0state;
	/** Reflects the logic state of the external pin AD1. Range 0..1 */
	uint8_t pin_ad1state;
	/** Enables/Disables the Packet Error Checking that is used during all
	 * SPI transfers. Range 0..1
	 */
	uint8_t enable_packet_error_check;

	/** Power-On Condition. Determines the state of the voltage output
	 * channels during normal operation.
	 * 0 - The output goes to the value set by the POC hardware pin when
	 * the voltage output is not enabled;
	 * 1 - The output goes to the opposite value of the POC hardware pin
	 * if the voltage output is not enabled.
	 */
	uint8_t poc_bit;
	/** Enables/Disables status readback during a write. Range 0..1 */
	uint8_t stat_readbit;
	/** Programmable short-circuit limit on the VOUT_x pin in the event
	 * of a short-circuit condition: 0 - 16 mA
	 *                               1 - 8 mA
	 */
	uint8_t sht_cc_lim_bit;

	/** Selects an internal or external current sense resistor for the
	 * selected DAC channel: 0 - selects the external resistor
	 *                       1 - selects the internal resistor
	 */
	uint8_t rset_bits[4];
	/** Enables 20% overrange on voltage output channel only. Range 0..1 */
	uint8_t ovrng_bits[4];

	/** Selects between an internal and external compensation resistor
	 * for the dc-to-dc converter. Range 0..1
	 */
	uint8_t dc_dc_comp_bit;
	/** User programmable dc-to-dc converter phase (between channels).
	 *  Range 0..3
	 */
	uint8_t dc_dc_phase_bit;
	/** DC-to-dc switching frequency. Range 0..2 */
	uint8_t dc_dc_freq_bit;
	/** Maximum allowed VBOOST_x voltage supplied by the dc-to-dc
	 * converter. Range 0..3
	 */
	uint8_t dc_dc_max_vbit;
};

/* Supported devices */
enum ad5755_type_t {
	ID_AD5755,
	ID_AD5755_1,
	ID_AD5757,
};

struct ad5755_dev {
	/* SPI */
	struct no_os_spi_desc		*spi_desc;
	/* GPIO */
	struct no_os_gpio_desc	*gpio_ldac;
	struct no_os_gpio_desc	*gpio_rst;
	struct no_os_gpio_desc	*gpio_clr;
	struct no_os_gpio_desc	*gpio_poc;
	/* Device Settings */
	struct ad5755_setup *p_ad5755_st;
	enum ad5755_type_t this_device;
};

struct ad5755_init_param {
	/* SPI */
	struct no_os_spi_init_param	spi_init;
	/* GPIO */
	struct no_os_gpio_init_param	gpio_ldac;
	struct no_os_gpio_init_param	gpio_rst;
	struct no_os_gpio_init_param	gpio_clr;
	struct no_os_gpio_init_param	gpio_poc;
	/* Device Settings */
	enum ad5755_type_t this_device;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Initializes the device and powers-up all channels. */
int8_t ad5755_init(struct ad5755_dev **device,
		   struct ad5755_init_param init_param);

/*! Free the resources allocated by ad5755_init(). */
int32_t ad5755_remove(struct ad5755_dev *dev);

/*! Reads the value of a register. */
int32_t ad5755_get_register_value(struct ad5755_dev *dev,
				  uint8_t register_address);

/*! Writes data into a register. */
uint16_t ad5755_set_register_value(struct ad5755_dev *dev,
				   uint8_t register_address,
				   uint8_t channel,
				   uint16_t register_value);

/*! Performs a software reset to the device. */
void ad5755_software_reset(struct ad5755_dev *dev);

/*! Enables/Disables watchdog timer and sets the timeout period. */
void ad5755_watch_dog_setup(struct ad5755_dev *dev,
			    uint8_t wtd_enable,
			    uint8_t timeout);

/*! Writes a "service pulse" to the AD5755 watchdog timer when enabled. */
void ad5755_feed_watch_dog_timer(struct ad5755_dev *dev);

/*! Configures one of the control registers. */
void ad5755_set_control_registers(struct ad5755_dev *dev,
				  uint8_t  ctrl_reg_address,
				  uint8_t  channel,
				  uint16_t reg_value);

/*! Computes the CRC for a data buffer. */
uint8_t ad5755_check_crc(uint8_t* data,
			 uint8_t bytes_number);

/*! Allows power-up/down of the dc-to-dc converter, DAC and internal amplifiers
    for the selected channel. */
void ad5755_set_channel_power(struct ad5755_dev *dev,
			      uint8_t channel,
			      uint8_t pwr_status);

/*! Sets the range of a channel. */
void ad5755_set_channel_range(struct ad5755_dev *dev,
			      uint8_t channel,
			      uint8_t range);

/*! Selects if the channel clears when CLEAR pin is activated. */
void ad5755_channel_clear_enable(struct ad5755_dev *dev,
				 uint8_t channel,
				 uint8_t clear_en);

/*! Configures the Digital Slew Rate Control. */
void ad5755_slew_rate_ctrl(struct ad5755_dev *dev,
			   int8_t channel,
			   int8_t sr_en,
			   int8_t updt_freq,
			   int8_t step_size);

/*! Sets the output voltage of a channel. */
float ad5755_set_voltage(struct ad5755_dev *dev,
			 uint8_t channel,
			 float voltage);

/*! Sets the output current of a channel. */
float ad5755_set_current(struct ad5755_dev *dev,
			 uint8_t channel,
			 float m_acurrent);

#endif // __AD5755_H__
