/***************************************************************************//**
 *   @file   adaq8092.h
 *   @brief  Header file of ADAQ8092 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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
#ifndef __ADAQ8092_H__
#define __ADAQ8092_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <string.h>
#include "no_os_util.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
/* SPI commands */
#define ADAQ8092_SPI_READ          	NO_OS_BIT(7)
#define ADAQ8092_ADDR(x)		((x) & 0xFF)

/* ADAQ8092 Register Map */
#define ADAQ8092_REG_RESET		0x00
#define ADAQ8092_REG_POWERDOWN		0x01
#define ADAQ8092_REG_TIMING		0x02
#define ADAQ8092_REG_OUTPUT_MODE	0x03
#define ADAQ8092_REG_DATA_FORMAT	0x04

/* ADAQ8092_REG_RESET Bit Definition */
#define ADAQ8092_RESET			NO_OS_BIT(7)

/* ADAQ8092_REG_POWERDOWN Bit Definition */
#define ADAQ8092_POWERDOWN_MODE		NO_OS_GENMASK(1, 0)

/* ADAQ8092_REG_TIMING Bit Definition */
#define ADAQ8092_CLK_INVERT		NO_OS_BIT(3)
#define ADAQ8092_CLK_PHASE		NO_OS_GENMASK(2, 1)
#define ADAQ8092_CLK_DUTYCYCLE		NO_OS_BIT(0)

/* ADAQ8092_REG_OUTPUT_MODE Bit Definition */
#define ADAQ8092_ILVDS			NO_OS_GENMASK(6, 4)
#define ADAQ8092_TERMON			NO_OS_BIT(3)
#define ADAQ8092_OUTOFF			NO_OS_BIT(2)
#define ADAQ8092_OUTMODE		NO_OS_GENMASK(1, 0)

/* ADAQ8092_REG_DATA_FORMAT Bit Definition */
#define ADAQ8092_OUTTEST		NO_OS_GENMASK(5, 3)
#define ADAQ8092_ABP			NO_OS_BIT(2)
#define ADAQ8092_RAND			NO_OS_BIT(1)
#define ADAQ8092_TWOSCOMP		NO_OS_BIT(0)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
/* ADAQ8092 Power Down Modes */
enum adaq8092_powerdown_modes {
	ADAQ8092_NORMAL_OP,
	ADAQ8092_CH1_NORMAL_CH2_NAP,
	ADAQ8092_CH1_CH2_NAP,
	ADAQ8092_SLEEP
};

/* ADAQ8092 Output Clock Invert */
enum adaq8092_clk_invert {
	ADAQ8092_CLK_POL_NORMAL,
	ADAQ8092_CLK_POL_INVERTED
};

/* ADAQ8092 Output Clock Phase Delay Bits */
enum adaq8092_clk_phase_delay {
	ADAQ8092_NO_DELAY,
	ADAQ8092_CLKOUT_DELAY_45DEG,
	ADAQ8092_CLKOUT_DELAY_90DEG,
	ADAQ8092_CLKOUT_DELAY_180DEG
};

/*ADAQ8092 Clock Duty Cycle Stabilizer */
enum adaq8092_clk_dutycycle {
	ADAQ8092_CLK_DC_STABILIZER_OFF,
	ADAQ8092_CLK_DC_STABILIZER_ON,
};

/* ADAQ8092 LVDS Output Current */
enum adaq8092_lvds_out_current {
	ADAQ8092_3M5A = 0,
	ADAQ8092_4MA = 1,
	ADAQ8092_4M5A = 2,
	ADAQ8092_3MA = 4,
	ADAQ8092_2M5A = 5,
	ADAQ8092_2M1A = 6,
	ADAQ8092_1M75 = 7
};

/* ADAQ8092 LVDS Internal Termination */
enum adaq8092_internal_term {
	ADAQ8092_TERM_OFF,
	ADAQ8092_TERM_ON
};

/* ADAQ8092 Digital Output */
enum adaq8092_dout_enable {
	ADAQ8092_DOUT_ON,
	ADAQ8092_DOUT_OFF
};

/* ADAQ8092 Digital Output Mode */
enum adaq8092_dout_modes {
	ADAQ8092_FULL_RATE_CMOS,
	ADAQ8092_DOUBLE_RATE_LVDS,
	ADAQ8092_DOUBLE_RATE_CMOS
};

/* ADAQ8092 Digital Test Pattern */
enum adaq8092_out_test_modes {
	ADAQ8092_TEST_OFF = 0,
	ADAQ8092_TEST_ONES = 1,
	ADAQ8092_TEST_ZEROS = 3,
	ADAQ8092_TEST_CHECKERBOARD = 5,
	ADAQ8092_TEST_ALTERNATING = 7
};

/* ADAQ8092 Alternate Bit Polarity Mode */
enum adaq8092_alt_bit_pol {
	ADAQ8092_ALT_BIT_POL_OFF,
	ADAQ8092_ALT_BIT_POL_ON
};

/* ADAQ8092 Data Output Randomizer*/
enum adaq8092_data_rand {
	ADAQ8092_DATA_RAND_OFF,
	ADAQ8092_DATA_RAND_ON
};

/* ADAQ8092 Twos Complement Mode */
enum adaq8092_twoscomp {
	ADAQ8092_OFFSET_BINARY,
	ADAQ8092_TWOS_COMPLEMENT
};

/**
 * @struct adaq8092_init_param
 * @brief ADAQ8092 Device initialization parameters.
 */
struct adaq8092_init_param {
	/** Device communication descriptor */
	struct no_os_spi_init_param 	*spi_init;
	struct no_os_gpio_init_param	*gpio_adc_pd1_param;
	struct no_os_gpio_init_param	*gpio_adc_pd2_param;
	struct no_os_gpio_init_param	*gpio_en_1p8_param;
	struct no_os_gpio_init_param	*gpio_par_ser_param;
	enum adaq8092_powerdown_modes	pd_mode;
	enum adaq8092_clk_invert	clk_pol_mode;
	enum adaq8092_clk_phase_delay	clk_phase_mode;
	enum adaq8092_clk_dutycycle	clk_dc_mode;
	enum adaq8092_lvds_out_current	lvds_cur_mode;
	enum adaq8092_internal_term	lvds_term_mode;
	enum adaq8092_dout_enable	dout_en;
	enum adaq8092_dout_modes	dout_mode;
	enum adaq8092_out_test_modes	test_mode;
	enum adaq8092_alt_bit_pol	alt_bit_pol_en;
	enum adaq8092_data_rand		data_rand_en;
	enum adaq8092_twoscomp		twos_comp;
};

/**
 * @struct adaq8092_dev
 * @brief ADAQ8092 Device structure.
 */
struct adaq8092_dev {
	/** Device communication descriptor */
	struct no_os_spi_desc		*spi_desc;
	struct no_os_gpio_desc		*gpio_adc_pd1;
	struct no_os_gpio_desc		*gpio_adc_pd2;
	struct no_os_gpio_desc		*gpio_en_1p8;
	struct no_os_gpio_desc		*gpio_par_ser;
	enum adaq8092_powerdown_modes	pd_mode;
	enum adaq8092_clk_invert	clk_pol_mode;
	enum adaq8092_clk_phase_delay	clk_phase_mode;
	enum adaq8092_clk_dutycycle	clk_dc_mode;
	enum adaq8092_lvds_out_current	lvds_cur_mode;
	enum adaq8092_internal_term	lvds_term_mode;
	enum adaq8092_dout_enable	dout_en;
	enum adaq8092_dout_modes	dout_mode;
	enum adaq8092_out_test_modes	test_mode;
	enum adaq8092_alt_bit_pol	alt_bit_pol_en;
	enum adaq8092_data_rand		data_rand_en;
	enum adaq8092_twoscomp		twos_comp;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Read device register. */
int adaq8092_read(struct adaq8092_dev *dev, uint8_t reg_addr,
		  uint8_t *reg_data);

/* Write device register. */
int adaq8092_write(struct adaq8092_dev *dev, uint8_t reg_addr,
		   uint8_t reg_data);

/* Update specific register bits. */
int adaq8092_update_bits(struct adaq8092_dev *dev, uint8_t reg_addr,
			 uint8_t mask, uint8_t reg_data);

/* Initialize the device. */
int adaq8092_init(struct adaq8092_dev **device,
		  struct adaq8092_init_param init_param);

/* Remove the device and release resources. */
int adaq8092_remove(struct adaq8092_dev *dev);

/* Set the device powerodown mode. */
int adaq8092_set_pd_mode(struct adaq8092_dev *dev,
			 enum adaq8092_powerdown_modes mode);

/* Get the device powerdown mode. */
enum adaq8092_powerdown_modes adaq8092_get_pd_mode(struct adaq8092_dev *dev);

/* Set the clock polarity mode. */
int adaq8092_set_clk_pol_mode(struct adaq8092_dev *dev,
			      enum adaq8092_clk_invert mode);

/* Get the clock polarity mode. */
enum adaq8092_clk_invert adaq8092_get_clk_pol_mode(struct adaq8092_dev *dev);

/* Set the clock phase delay mode. */
int adaq8092_set_clk_phase_mode(struct adaq8092_dev *dev,
				enum adaq8092_clk_phase_delay mode);

/* Get the clock phase delay mode. */
enum adaq8092_clk_phase_delay adaq8092_get_clk_phase_mode(
	struct adaq8092_dev *dev);

/* Set the clock duty cycle stabilizer mode. */
int adaq8092_set_clk_dc_mode(struct adaq8092_dev *dev,
			     enum adaq8092_clk_dutycycle mode);

/* Get the clock duty cycle stabilizer mode. */
enum adaq8092_clk_dutycycle adaq8092_get_clk_dc_mode(struct adaq8092_dev *dev);

/* Set the LVDS output current mode. */
int adaq8092_set_lvds_cur_mode(struct adaq8092_dev *dev,
			       enum adaq8092_lvds_out_current mode);

/* Get the LVDS output current mode. */
enum adaq8092_lvds_out_current adaq8092_get_lvds_cur_mode(
	struct adaq8092_dev *dev);

/* Set the LVDS internal temination mode. */
int adaq8092_set_lvds_term_mode(struct adaq8092_dev *dev,
				enum adaq8092_internal_term mode);

/* Get the LVDS internal temination device mode. */
enum adaq8092_internal_term adaq8092_get_lvds_term_mode(
	struct adaq8092_dev *dev);

/* Set digital outputs. */
int adaq8092_set_dout_en(struct adaq8092_dev *dev,
			 enum adaq8092_dout_enable mode);

/* Get digital outputs. */
enum adaq8092_dout_enable adaq8092_get_dout_en(struct adaq8092_dev *dev);

/* Set the digital output mode. */
int adaq8092_set_dout_mode(struct adaq8092_dev *dev,
			   enum adaq8092_dout_modes mode);

/* Get the digital output mode. */
enum adaq8092_dout_modes adaq8092_get_dout_mode(struct adaq8092_dev *dev);

/* Set digital output test pattern mode. */
int adaq8092_set_test_mode(struct adaq8092_dev *dev,
			   enum adaq8092_out_test_modes mode);

/* Get digital output test pattern mode. */
enum adaq8092_out_test_modes adaq8092_get_test_mode(struct adaq8092_dev *dev);

/* Set the alternate bit polarity mode. */
int adaq8092_set_alt_pol_en(struct adaq8092_dev *dev,
			    enum adaq8092_alt_bit_pol mode);

/* Get the alternate bit polarity mode. */
enum adaq8092_alt_bit_pol adaq8092_get_alt_pol_en(struct adaq8092_dev *dev);

/* Set the data output randomizer mode. */
int adaq8092_set_data_rand_en(struct adaq8092_dev *dev,
			      enum adaq8092_data_rand mode);

/* Get the data output randomizer mode. */
enum adaq8092_data_rand adaq8092_get_data_rand_en(struct adaq8092_dev *dev);

/* Set the Tows Complement mode. */
int adaq8092_set_twos_comp(struct adaq8092_dev *dev,
			   enum adaq8092_twoscomp mode);

/* Get the Tows Complement mode. */
enum adaq8092_twoscomp adaq8092_get_twos_comp(struct adaq8092_dev *dev);

#endif /* __ADAQ8092_H__ */
