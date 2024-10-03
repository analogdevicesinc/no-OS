/***************************************************************************//**
 *   @file   AD6673.h
 *   @brief  Header file of AD6673 Driver.
 *   @author DNechita (Dan.Nechita@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
#ifndef __AD6673_H__
#define __AD6673_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_spi.h"
#include "no_os_error.h"

/******************************************************************************/
/*********************************** AD6673 ***********************************/
/******************************************************************************/

/* Registers */

#define AD6673_READ                         (1 << 15)
#define AD6673_WRITE                        (0 << 15)
#define AD6673_CNT(x)                       ((((x) & 0x3) - 1) << 13)
#define AD6673_ADDR(x)                      ((x) & 0xFF)

#define AD6673_R1B                          (1 << 8)
#define AD6673_R2B                          (2 << 8)
#define AD6673_R3B                          (3 << 8)
#define AD6673_TRANSF_LEN(x)                (((x) >> 8) & 0xFF)
#define SHADOW(x)                           ((x) << 16)

/* Chip configuration registers */
#define AD6673_REG_SPI_CFG                  (AD6673_R1B | 0x00)
#define AD6673_REG_CHIP_ID                  (AD6673_R1B | 0x01)
#define AD6673_REG_CHIP_INFO                (AD6673_R1B | 0x02)

/* Channel index and transfer registers */
#define AD6673_REG_CH_INDEX                 (AD6673_R1B | 0x05)
#define AD6673_REG_DEVICE_UPDATE            (AD6673_R1B | 0xFF)

/* Program register map */
#define AD6673_REG_PDWN                     (AD6673_R1B | 0x08)
#define AD6673_REG_CLOCK                    (AD6673_R1B | 0x09 | SHADOW(1))
#define AD6673_REG_PLL_STAT                 (AD6673_R1B | 0x0A)
#define AD6673_REG_CLOCK_DIV                (AD6673_R1B | 0x0B | SHADOW(2))
#define AD6673_REG_TEST                     (AD6673_R1B | 0x0D | SHADOW(3))
#define AD6673_REG_BIST                     (AD6673_R1B | 0x0E | SHADOW(4))
#define AD6673_REG_OFFSET                   (AD6673_R1B | 0x10 | SHADOW(5))
#define AD6673_REG_OUT_MODE                 (AD6673_R1B | 0x14 | SHADOW(6))
#define AD6673_REG_CML                      (AD6673_R1B | 0x15)
#define AD6673_REG_VREF                     (AD6673_R1B | 0x18 | SHADOW(7))
#define AD6673_REG_USER_TEST1               (AD6673_R2B | 0x1A)
#define AD6673_REG_USER_TEST2               (AD6673_R2B | 0x1C)
#define AD6673_REG_USER_TEST3               (AD6673_R2B | 0x1E)
#define AD6673_REG_USER_TEST4               (AD6673_R2B | 0x20)
#define AD6673_REG_PLL_ENCODE               (AD6673_R1B | 0x21)
#define AD6673_REG_BIST_MISR                (AD6673_R2B | 0x25)
#define AD6673_REG_SYS_CTRL                 (AD6673_R1B | 0x3A | SHADOW(8))
#define AD6673_REG_NSR_CTRL                 (AD6673_R1B | 0x3C | SHADOW(9))
#define AD6673_REG_NSR_TUNING               (AD6673_R1B | 0x3E | SHADOW(10))
#define AD6673_REG_DCC_CTRL                 (AD6673_R1B | 0x40 | SHADOW(11))
#define AD6673_REG_DCC_VAL                  (AD6673_R2B | 0x42 | SHADOW(12))
#define AD6673_REG_FAST_DETECT              (AD6673_R1B | 0x45 | SHADOW(13))
#define AD6673_REG_FD_UPPER_THD             (AD6673_R2B | 0x48 | SHADOW(14))
#define AD6673_REG_FD_LOWER_THD             (AD6673_R2B | 0x4A | SHADOW(15))
#define AD6673_REG_FD_DWELL_TIME            (AD6673_R2B | 0x4C | SHADOW(16))
#define AD6673_REG_204B_QUICK_CFG           (AD6673_R1B | 0x5E)
#define AD6673_REG_204B_CTRL1               (AD6673_R1B | 0x5F)
#define AD6673_REG_204B_CTRL2               (AD6673_R1B | 0x60)
#define AD6673_REG_204B_CTRL3               (AD6673_R1B | 0x61)
#define AD6673_REG_204B_DID_CFG             (AD6673_R1B | 0x64)
#define AD6673_REG_204B_BID_CFG             (AD6673_R1B | 0x65)
#define AD6673_REG_204B_LID_CFG1            (AD6673_R1B | 0x67)
#define AD6673_REG_204B_LID_CFG2            (AD6673_R1B | 0x68)
#define AD6673_REG_204B_PARAM_SCR_L         (AD6673_R1B | 0x6E)
#define AD6673_REG_204B_PARAM_F             (AD6673_R1B | 0x6F)
#define AD6673_REG_204B_PARAM_K             (AD6673_R1B | 0x70)
#define AD6673_REG_204B_PARAM_M             (AD6673_R1B | 0x71)
#define AD6673_REG_204B_PARAM_CS_N          (AD6673_R1B | 0x72)
#define AD6673_REG_204B_PARAM_NP            (AD6673_R1B | 0x73)
#define AD6673_REG_204B_PARAM_S             (AD6673_R1B | 0x74)
#define AD6673_REG_204B_PARAM_HD_CF         (AD6673_R1B | 0x75)
#define AD6673_REG_204B_RESV1               (AD6673_R1B | 0x76)
#define AD6673_REG_204B_RESV2               (AD6673_R1B | 0x77)
#define AD6673_REG_204B_CHKSUM0             (AD6673_R1B | 0x79)
#define AD6673_REG_204B_CHKSUM1             (AD6673_R1B | 0x7A)
#define AD6673_REG_204B_LANE_ASSGN1         (AD6673_R1B | 0x82)
#define AD6673_REG_204B_LANE_ASSGN2         (AD6673_R1B | 0x83)
#define AD6673_REG_204B_LMFC_OFFSET         (AD6673_R1B | 0x8B)
#define AD6673_REG_204B_PRE_EMPHASIS        (AD6673_R1B | 0xA8)

/* AD6673_REG_SPI_CFG */
#define AD6673_SPI_CFG_LSB_FIRST            ((1 << 6) | (1 << 1))
#define AD6673_SPI_CFG_SOFT_RST             ((1 << 5) | (1 << 2))

/* AD6673_REG_CH_INDEX */
#define AD6673_CH_INDEX_ADC_A               (1 << 0)
#define AD6673_CH_INDEX_ADC_B               (1 << 1)

/* AD6673_REG_DEVICE_UPDATE */
#define AD6673_DEVICE_UPDATE_SW             (1 << 0)

/* AD6673_REG_PDWN */
#define AD6673_PDWN_EXTERN                  (1 << 5)
#define AD6673_PDWN_JTX                     (1 << 4)
#define AD6673_PDWN_JESD204B(x)             (((x) & 0x3) << 2)
#define AD6673_PDWN_CHIP(x)                 (((x) & 0x3) << 0)

/* AD6673_REG_CLOCK */
#define AD6673_CLOCK_SELECTION(x)           (((x) & 0x3) << 4)
#define AD6673_CLOCK_DUTY_CYCLE             (1 << 0)

/* AD6673_REG_PLL_STAT */
#define AD6673_PLL_STAT_LOCKED              (1 << 7)
#define AD6673_PLL_STAT_204B_LINK_RDY       (1 << 0)

/* AD6673_REG_CLOCK_DIV */
#define AD6673_CLOCK_DIV_PHASE(x)           (((x) & 0x7) << 3)
#define AD6673_CLOCK_DIV_RATIO(x)           (((x) & 0x7) << 0)

/* AD6673_REG_TEST */
#define AD6673_TEST_USER_TEST_MODE(x)       (((x) & 0x3) << 6)
#define AD6673_TEST_RST_PN_LONG             (1 << 5)
#define AD6673_TEST_RST_PN_SHOR             (1 << 4)
#define AD6673_TEST_OUTPUT_TEST(x)          (((x) & 0xF) << 0)

/* AD6673_REG_BIST */
#define AD6673_BIST_RESET                   (1 << 2)
#define AD6673_BIST_ENABLE                  (1 << 0)

/* AD6673_REG_OFFSET */
#define AD6673_REG_OFFSET_ADJUST(x)             (((x) & 0x3F) << 0)

/* AD6673_REG_OUT_MODE */
#define AD6673_OUT_MODE_JTX_BIT_ASSIGN(x)       (((x) & 0x7) << 5)
#define AD6673_OUT_MODE_DISABLE                 (1 << 4)
#define AD6673_OUT_MODE_INVERT_DATA             (1 << 3)
#define AD6673_OUT_MODE_DATA_FORMAT(x)          (((x) & 0x1) << 0)

/* AD6673_REG_CML */
#define AD6673_CML_DIFF_OUT_LEVEL(x)            (((x) & 0x7) << 0)

/* AD6673_REG_VREF */
#define AD6673_VREF_FS_ADJUST(x)                (((x) & 0x1F) << 0)

/* AD6673_REG_PLL_ENCODE */
#define AD6673_PLL_ENCODE(x)                    (((x) & 0x3) << 3)

/* AD6673_REG_SYS_CTRL */
#define AD6673_SYS_CTRL_REALIGN_ON_SYNCINB      (1 << 4)
#define AD6673_SYS_CTRL_REALIGN_ON_SYSREF       (1 << 3)
#define AD6673_SYS_CTRL_SYSREF_MODE             (1 << 2)
#define AD6673_SYS_CTRL_SYSREF_EN               (1 << 1)
#define AD6673_SYS_CTRL_SYNCINB_EN              (1 << 0)

/* AD6673_REG_NSR_CTRL */
#define AD6673_NSR_CTRL_BW_MODE                 (1 << 1)
#define AD6673_NSR_CTRL_ENABLE                  (1 << 0)

/* AD6673_REG_NSR_TUNING */
#define AD6673_NSR_TUNING(x)                    (((x) & 0x3F) << 0)

/* AD6673_REG_DCC_CTRL */
#define AD6673_DCC_CTRL_FREEZE_DCC              (1 << 6)
#define AD6673_DCC_CTRL_DCC_BW(x)               (((x) & 0xF) << 2)
#define AD6673_DCC_CTRL_DCC_EN                  (1 << 1)

/* AD6673_REG_FAST_DETECT */
#define AD6673_FAST_DETECT_PIN_FCT              (1 << 4)
#define AD6673_FAST_DETECT_FORCE_FDA_FDB_PIN    (1 << 3)
#define AD6673_FAST_DETECT_FORCE_FDA_FDB_VAL    (1 << 2)
#define AD6673_FAST_DETECT_OUTPUT_ENABLE        (1 << 0)

/* AD6673_REG_204B_QUICK_CFG */
#define AD6673_204B_QUICK_CFG(x)                (((x) & 0xFF) << 0)

/* AD6673_REG_204B_CTRL1 */
#define AD6673_204B_CTRL1_TAIL_BITS             (1 << 6)
#define AD6673_204B_CTRL1_TEST_SAMPLE_EN        (1 << 5)
#define AD6673_204B_CTRL1_ILAS_MODE(x)          (((x) & 0x3) << 2)
#define AD6673_204B_CTRL1_POWER_DOWN            (1 << 0)

/* AD6673_REG_204B_CTRL2 */
#define AD6673_204B_CTRL2_INVERT_JESD_BITS      (1 << 1)

/* AD6673_REG_204B_CTRL3 */
#define AD6673_204B_CTRL3_TEST_DATA_INJ_PT(x)   (((x) & 0x3) << 4)
#define AD6673_204B_CTRL3_JESD_TEST_MODE(x)     (((x) & 0xF) << 0)

/* AD6673_REG_204B_PARAM_SCR_L */
#define AD6673_204B_PARAM_SCR_L_SCRAMBLING      (1 << 7)
#define AD6673_204B_PARAM_SCR_L_LANES           (1 << 0)

/* AD6673_REG_204B_PARAM_CS_N */
#define AD6673_204B_PARAM_CS_N_NR_CTRL_BITS(x)      (((x) & 0x3) << 6)
#define AD6673_204B_PARAM_CS_N_ADC_RESOLUTION(x)    (((x) & 0xF) << 0)

/* AD6673_REG_204B_PARAM_NP */
#define AD6673_204B_PARAM_NP_JESD_SUBCLASS(x)       (((x) & 0x3) << 5)
#define AD6673_204B_PARAM_NP_JESD_N_VAL(x)          (((x) & 0xF) << 0)

/* AD6673_REG_204B_PARAM_S */
#define AD6673_204B_PARAM_S(x)                  (((x) << 0x1F) << 0)

/* AD6673_REG_204B_PARAM_HD_CF */
#define AD6673_204B_PARAM_HD_CF_HD_VAL          (1 << 7)
#define AD6673_204B_PARAM_HD_CF_CF_VAL(x)       (((x) & 0x1F) << 0)

/* AD6673_REG_204B_LANE_ASSGN1 */
#define AD6673_204B_LANE_ASSGN1(x)              (((x) & 0x3) << 4)

/* AD6673_REG_204B_LANE_ASSGN2 */
#define AD6673_204B_LANE_ASSGN2(x)              (((x) &0x3) << 0)

/* AD6673_REG_204B_LMFC_OFFSET */
#define AD6673_204B_LMFC_OFFSET(x)              (((x) & 0x1F) << 0)

/*****************************************************************************/
/************************** Types Declarations *******************************/
/*****************************************************************************/

/**
 * @struct ad6673_platform_data
 * @brief Platform specific information.
 */
struct ad6673_platform_data {
	/**
	 * External PDWN mode.
	 * 0 = PDWN is full power down
	 * 1 = PDWN puts device in standby
	 */
	int8_t extrn_pdwnmode;
	/**
	 * Clock duty cycle stabilizer enable.
	 * 0 = disable
	 * 1 = enable
	 */
	int8_t en_clk_dcs;
	/**
	 * Clock selection.
	 * 0 = Nyquist clock
	 * 2 = RF clock divide by 4
	 * 3 = clock off
	 */
	int8_t clk_selection;
	/**
	 * Clock divider ratio relative to the encode clock.
	 * 0x00 = divide by 1
	 * 0x01 = divide by 2
	 * ...
	 * 0x07 = divide by 8
	 */
	int8_t clk_div_ratio;
	/**
	 * Clock divide phase relative to the encode clock.
	 * 0x0 = 0 input clock cycles delayed
	 * 0x1 = 1 input clock cycles delayed
	 * ...
	 * 0x7 = 7 input clock cycles delayed
	 */
	int8_t clk_div_phase;
	/**
	 * Main reference full-scale VREF adjustment.
	 * 0x0f = internal 2.087 V p-p
	 * ...
	 * 0x01 = internal 1.772 V p-p
	 * 0x00 = internal 1.75 V p-p [default]
	 * 0x1F = internal 1.727 V p-p
	 * ...
	 * 0x10 = internal 1.383 V p-p
	 */
	int8_t adc_vref;
	/**
	 * PLL low encode.
	 * 0 = for lane speeds > 2 Gbps
	 * 1 = for lane speeds < 2 Gbps
	 */
	int8_t pll_low_encode;
	/** Device name */
	int8_t  name[16];
};

/**
 * @struct ad6673_jesd204b_cfg
 * @brief JESD204B interface configuration.
 */
struct ad6673_jesd204b_cfg {
	/**
	 * JTX in standby.
	 * 0 = 204B core is unaffected in standby
	 * 1 = 204B core is powered down except for PLL during standby
	 */
	int8_t jtx_in_standby;
	/**
	 * JESD204B CML differential output drive level adjustment.
	 * 0 = 81% of nominal (that is, 238 mV)
	 * 1 = 89% of nominal (that is, 262 mV)
	 * 2 = 98% of nominal (that is, 286 mV)
	 * 3 = nominal [default] (that is, 293 mV)
	 * 6 = 126% of nominal (that is, 368 mV)
	 */
	int8_t cml_level;
	/**
	 * Quick configuration register.
	 * 0x11 = M = 1, L = 1; one converter, one lane
	 * 0x12 = M = 1, L = 2; one converter, two lanes
	 * 0x21 = M = 2, L = 1; two converters, one lane
	 * 0x22 = M = 2, L = 2; two converters, two lanes
	 */
	int8_t quick_cfg_option;
	/**
	 * JESD204B subclass.
	 * 0 = Subclass 0
	 * 1 = Subclass 1
	 */
	int8_t subclass;
	/**
	 * Number of control bits (CS).
	 * 0 = no control bits(CS = 0)
	 * 1 = 1 control bit  (CS = 1)
	 * 2 = 2 control bits (CS = 2)
	 */
	int8_t ctrl_bits_no;
	/**
	 * JTX CS bits assignment.
	 * 0 = {overrange||underrange, valid}
	 * 1 = {overrange||underrange}
	 * 2 = {overrange||underrange, blank}
	 * 3 = {blank, valid}
	 * 4 = {blank, blank}
	 * All others = {overrange||underrange, valid}
	 */
	int8_t ctrl_bits_assign;
	/**
	 * Tail bits: If CS bits are not enabled.
	 * 0 = extra bits are 0
	 * 1 = extra bits are 9-bit PN
	 */
	int8_t tail_bits_mode;
	/** JESD204B device identification value: DID[7:0] */
	int8_t did;
	/** JESD204B bank identification value : BID[3:0] */
	int8_t bid;
	/** JESD204B lane0 identification value: LID[4:0] */
	int8_t lid0;
	/** JESD204B lane1 identification value: LID[4:0] */
	int8_t lid1;
	/**
	 * JESD204B number of frames per multiframe (K); set value of K per JESD204B
	 * specifications, but also must be a multiple of 4 octets.
	 */
	int8_t k;
	/**
	 * JESD204B scrambling (SCR).
	 * 0 = disabled
	 * 1 = enabled
	 */
	int8_t scrambling;
	/**
	 * Initial lane alignment sequence (ILAS) mode.
	 * 1 = ILAS normal mode enabled
	 * 3 = ILAS always on, test mode
	 */
	int8_t ilas_mode;
	/**
	 * JESD204B test sample.
	 * 0 = disabled
	 * 1 = enabled
	 */
	int8_t en_ilas_test;
	/**
	 * Invert logic of JESD204B bits.
	 * 0 = non-invert
	 * 1 = invert
	 */
	int8_t invert_logic_bits;
	/**
	 * SYSREF+- enable.
	 * 0 = disabled
	 * 1 = enabled
	 */
	int8_t en_sys_ref;
	/**
	 * Enable SYNCINB+- buffer.
	 * 0 = buffer disabled
	 * 1 = buffer enabled
	 */
	int8_t en_sync_in_b;
	/**
	 * SYSREF+- mode.
	 * 0 = continuous reset clock dividers
	 * 1 = sync on next SYSREF+- rising edge only
	 */
	int8_t sys_ref_mode;
	/**
	 * Options for interpreting single on SYNCINB+-.
	 * 0 = normal mode
	 * 1 = realign lanes on every active SYNCINB+-
	 */
	int8_t align_sync_in_b;
	/**
	 * Options for interpreting single on SYSREF+-.
	 * 0 = normal mode;
	 * 1 = realign lanes on every active SYSREF+-
	 */
	int8_t align_sys_ref;
	/**
	 * Option to remap converter and lane assignments.
	 * 0 = assign Logical Lane 0 to Physical Lane A [default]
	 * 1 = assign Logical Lane 0 to Physical Lane B
	 */
	int8_t lane0_assign;
	/* Option to remap converter and lane assignments.
	 * 0 = assign Logical Lane 1 to Physical Lane A
	 * 1 = assign Logical Lane 1 to Physical Lane B [default]
	 */
	int8_t lane1_assign;
};

/**
 * @struct ad6673_fast_detect_cfg
 * @brief Fast Detect module configuration.
 */
struct ad6673_fast_detect_cfg {
	/**
	 * Enable fast detect output.
	 * 0 = disable
	 * 1 = enable
	 */
	int8_t  en_fd;
	/**
	 * Pin function.
	 * 0 = fast detect
	 * 1 = overrange
	 */
	int8_t  pin_function;
	/**
	 * Force FDA/FDB pins
	 * 0 = normal function
	 * 1 = force to value
	 */
	int8_t  force_pins;
	/**
	 * Force value of FDA/FDB pins.
	 * 0 = output on FD pins will be 0
	 * 1 = output on FD pins will be 1
	 */
	int8_t  pin_force_value;
	/** Fast Detect Upper Threshold[14:0]. */
	int16_t fd_upper_tresh;
	/** Fast Detect Lower Threshold[14:0]. */
	int16_t fd_lower_tresh;
	/** Fast Detect Dwell Time[15:0]. */
	int16_t df_dwell_time;
};

struct ad6673_type_band {
	int32_t f0;
	int32_t f_center;
	int32_t f1;
};

struct ad6673_state {
	struct ad6673_platform_data   *pdata;
	struct ad6673_jesd204b_cfg    *p_jesd204b;
	struct ad6673_fast_detect_cfg *p_fd;
};

enum shadow_registers {
	AD6673_SHD_REG_CLOCK = 1,
	AD6673_SHD_REG_CLOCK_DIV,
	AD6673_SHD_REG_TEST,
	AD6673_SHD_REG_BIST,
	AD6673_SHD_REG_OFFSET,
	AD6673_SHD_REG_OUT_MODE,
	AD6673_SHD_REG_VREF,
	AD6673_SHD_REG_SYS_CTRL,
	AD6673_REG_SHD_NSR_CTRL,
	AD6673_REG_SHD_NSR_TUNING,
	AD6673_SHD_REG_DCC_CTRL,
	AD6673_SHD_REG_DCC_VAL,
	AD6673_SHD_REG_FAST_DETECT,
	AD6673_SHD_REG_FD_UPPER_THD,
	AD6673_SHD_REG_FD_LOWER_THD,
	AD6673_SHD_REG_FD_DWELL_TIME,
	SHADOW_REGISTER_COUNT
};

struct ad6673_dev {
	/* SPI */
	struct no_os_spi_desc		*spi_desc;
	/* Device Settings */
	struct ad6673_state ad6673_st;
	int32_t shadow_regs[SHADOW_REGISTER_COUNT];
};

struct ad6673_init_param {
	/* SPI */
	struct no_os_spi_init_param	spi_init;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Configures the device. */
int32_t ad6673_setup(struct ad6673_dev **device,
		     struct ad6673_init_param init_param);
/*! Free the resources allocated by ad6673_setup(). */
int32_t ad6673_remove(struct ad6673_dev *dev);
/*! Reads the value of the selected register. */
int32_t ad6673_read(struct ad6673_dev *dev,
		    int32_t register_address);
/*! Writes a value to the selected register. */
int32_t ad6673_write(struct ad6673_dev *dev,
		     int32_t register_address,
		     int32_t register_value);
/*! Initiates a transfer and waits for the operation to end. */
int32_t ad6673_transfer(struct ad6673_dev *dev);
/*! Resets all registers to their default values. */
int32_t ad6673_soft_reset(struct ad6673_dev *dev);
/*! Configures the power mode of the chip. */
int32_t ad6673_chip_pwr_mode(struct ad6673_dev *dev,
			     int32_t mode);
/*! Selects a channel as the current channel for further configurations. */
int32_t ad6673_select_channel_for_config(struct ad6673_dev *dev,
		int32_t channel);
/*! Sets the ADC's test mode. */
int32_t ad6673_test_mode(struct ad6673_dev *dev,
			 int32_t mode);
/*! Sets the offset adjustment. */
int32_t ad6673_offset_adj(struct ad6673_dev *dev,
			  int32_t adj);
/*! Disables (1) or enables (0) the data output. */
int32_t ad6673_output_disable(struct ad6673_dev *dev,
			      int32_t en);
/*! Activates the inverted (1) or normal (0) output mode. */
int32_t ad6673_output_invert(struct ad6673_dev *dev,
			     int32_t invert);
/*! Specifies the output format. */
int32_t ad6673_output_format(struct ad6673_dev *dev,
			     int32_t format);
/*! Sets (1) or clears (0) the reset short PN sequence bit(PN9). */
int32_t ad6673_reset_pn9(struct ad6673_dev *dev,
			 int32_t rst);
/*! Sets (1) or clears (0) the reset long PN sequence bit(PN23). */
int32_t ad6673_reset_pn23(struct ad6673_dev *dev,
			  int32_t rst);
/*! Configures a User Test Pattern. */
int32_t ad6673_set_user_pattern(struct ad6673_dev *dev,
				int32_t pattern_no,
				int32_t user_pattern);
/*! Enables the Build-In-Self-Test. */
int32_t ad6673_bist_enable(struct ad6673_dev *dev,
			   int32_t enable);
/*! Resets the Build-In-Self-Test. */
int32_t ad6673_bist_reset(struct ad6673_dev *dev,
			  int32_t reset);
/*! Configures the JESD204B interface. */
int32_t ad6673_jesd204b_setup(struct ad6673_dev *dev);
/*! Configures the power mode of the JESD204B data transmit block. */
int32_t ad6673_jesd204b_pwr_mode(struct ad6673_dev *dev,
				 int32_t mode);
/*! Selects the point in the processing path of a lane, where the test data will
    be inserted. */
int32_t ad6673_jesd204b_select_test_injection_point(struct ad6673_dev *dev,
		int32_t inj_point);
/*! Selects a JESD204B test mode. */
int32_t ad6673_jesd204b_test_mode(struct ad6673_dev *dev,
				  int32_t test_mode);
/*! Inverts the logic of JESD204B bits. */
int32_t ad6673_jesd204b_invert_logic(struct ad6673_dev *dev,
				     int32_t invert);
/*! Configures the Fast-Detect module. */
int32_t ad6673_fast_detect_setup(struct ad6673_dev *dev);
/*! Enables DC correction for use in the output data signal path. */
int32_t ad6673_dcc_enable(struct ad6673_dev *dev,
			  int32_t enable);
/*! Selects the bandwidth value for the DC correction circuit. */
int32_t ad6673_dcc_bandwidth(struct ad6673_dev *dev,
			     int32_t bw);
/*! Freezes DC correction value. */
int32_t ad6673_dcc_freeze(struct ad6673_dev *dev,
			  int32_t freeze);
/*! Enables the Noise shaped requantizer(NRS). */
int32_t ad6673_nsr_enable(struct ad6673_dev *dev,
			  int32_t enable);
/*! Selects the NSR Bandwidth mode. */
int32_t ad6673_nsr_bandwidth_mode(struct ad6673_dev *dev,
				  int32_t mode);
/*! Sets the NSR frequency range. */
int32_t ad6673_nsr_tuning_freq(int64_t tune_freq,
			       int64_t f_adc,
			       struct ad6673_type_band *p_band);

#endif /* __AD6673_H__ */
