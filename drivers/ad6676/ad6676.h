/***************************************************************************//**
* @file ad6676.h
* @brief Header file of AD6676 Driver.
* @authors Dragos Bogdan (dragos.bogdan@analog.com)
* @authors Andrei Grozav (andrei.grozav@analog.com)
********************************************************************************
* Copyright 2015(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in
* the documentation and/or other materials provided with the
* distribution.
* - Neither the name of Analog Devices, Inc. nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
* - The use of this software may or may not infringe the patent rights
* of one or more patent holders. This license does not release you
* from the requirement that you obtain separate licenses from these
* patent holders to use this software.
* - Use of the software either in source or binary form, must be run
* on or directly connected to an Analog Devices Inc. component.
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
#ifndef AD6676_H_
#define AD6676_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_drivers.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD6676_SPI_CONFIG		0x000
#define AD6676_DEVICE_CONFIG		0x002
#define AD6676_CHIP_TYPE		0x003
#define AD6676_CHIP_ID0			0x004
#define AD6676_CHIP_ID1			0x005
#define AD6676_GRADE_REVISION		0x006
#define AD6676_VENDOR_ID0		0x00C
#define AD6676_VENDOR_ID1		0x00D
#define AD6676_PCBL_DONE		0x0FE

 /* CONFIGURATION SETTINGS */
#define AD6676_FADC_0			0x100
#define AD6676_FADC_1			0x101
#define AD6676_FIF_0			0x102
#define AD6676_FIF_1			0x103
#define AD6676_BW_0			0x104
#define AD6676_BW_1			0x105
#define AD6676_LEXT			0x106
#define AD6676_MRGN_L			0x107
#define AD6676_MRGN_U			0x108
#define AD6676_MRGN_IF			0x109
#define AD6676_XSCALE_1			0x10A

 /* BP SD ADC CALIBRATION/PROFILE */
#define AD6676_CAL_CTRL			0x115
#define AD6676_CAL_CMD			0x116
#define AD6676_CAL_DONE			0x117
#define AD6676_ADC_CONFIG		0x118
#define AD6676_FORCE_END_CAL		0x11A

 /* DIGITAL SIGNAL PATH */
#define AD6676_DEC_MODE			0x140
#define AD6676_MIX1_TUNING		0x141
#define AD6676_MIX2_TUNING		0x142
#define AD6676_MIX1_INIT		0x143
#define AD6676_MIX2_INIT_LSB		0x144
#define AD6676_MIX2_INIT_MSB		0x145
#define AD6676_DP_CTRL			0x146

 /* POWER CONTROL */
#define AD6676_STANDBY			0x150
#define AD6676_PD_DIG			0x151
#define AD6676_PD_PIN_CTRL		0x152
#define AD6676_STBY_DAC			0x250

 /* ATTENUATOR */
#define AD6676_ATTEN_MODE		0x180
#define AD6676_ATTEN_VALUE_PIN0		0x181
#define AD6676_ATTEN_VALUE_PIN1		0x182
#define AD6676_ATTEN_INIT		0x183
#define AD6676_ATTEN_CTL		0x184

 /* ADC RESET CONTROL */
#define AD6676_ADCRE_THRH		0x188
#define AD6676_ADCRE_PULSE_LEN		0x189
#define AD6676_ATTEN_STEP_RE		0x18A
#define AD6676_TIME_PER_STEP		0x18B

 /* PEAK DETECTOR AND AGC FLAG CONTROL */
#define AD6676_ADC_UNSTABLE		0x18F
#define AD6676_PKTHRH0_LSB		0x193
#define AD6676_PKTHRH0_MSB		0x194
#define AD6676_PKTHRH1_LSB		0x195
#define AD6676_PKTHRH1_MSB		0x196
#define AD6676_LOWTHRH_LSB		0x197
#define AD6676_LOWTHRH_MSB		0x198
#define AD6676_DWELL_TIME_MANTISSA	0x199
#define AD6676_DWELL_TIME_EXP		0x19A
#define AD6676_FLAG0_SEL		0x19B
#define AD6676_FLAG1_SEL		0x19C
#define AD6676_EN_FLAG			0x19E

 /* GPIO CONFIGURATION */
#define AD6676_FORCE_GPIO		0x1B0
#define AD6676_FORCE_GPIO_OUT		0x1B1
#define AD6676_FORCE_GPIO_VAL		0x1B2
#define AD6676_READ_GPO			0x1B3
#define AD6676_READ_GPI			0x1B4

 /* AD6676 JESD204B INTERFACE */
#define AD6676_DID			0x1C0
#define AD6676_BID			0x1C1
#define AD6676_L			0x1C3
#define AD6676_F			0x1C4
#define AD6676_K			0x1C5
#define AD6676_M			0x1C6
#define AD6676_S			0x1C9
#define AD6676_HD			0x1CA
#define AD6676_RES1			0x1CB
#define AD6676_RES2			0x1CC
#define AD6676_LID0			0x1D0
#define AD6676_LID1			0x1D1
#define AD6676_FCHK0			0x1D8
#define AD6676_FCHK1			0x1D9
#define AD6676_EN_LFIFO			0x1E0
#define AD6676_SWAP			0x1E1
#define AD6676_LANE_PD			0x1E2
#define AD6676_MIS1			0x1E3
#define AD6676_SYNC_PIN			0x1E4
#define AD6676_TEST_GEN			0x1E5
#define AD6676_KF_ILAS			0x1E6
#define AD6676_SYNCB_CTRL		0x1E7
#define AD6676_MIX_CTRL			0x1E8
#define AD6676_K_OFFSET			0x1E9
#define AD6676_SYSREF			0x1EA
#define AD6676_SER1			0x1EB
#define AD6676_SER2			0x1EC

#define AD6676_CLKSYN_ENABLE		0x2A0
#define AD6676_CLKSYN_INT_N_LSB		0x2A1
#define AD6676_CLKSYN_INT_N_MSB		0x2A2
#define AD6676_CLKSYN_LOGEN		0x2A5
#define AD6676_CLKSYN_VCO_BIAS		0x2AA
#define AD6676_CLKSYN_VCO_CAL		0x2AB
#define AD6676_CLKSYN_I_CP		0x2AC
#define AD6676_CLKSYN_CP_CAL		0x2AD
#define AD6676_CLKSYN_VCO_VAR		0x2B7
#define AD6676_CLKSYN_R_DIV		0x2BB
#define AD6676_CLKSYN_STATUS		0x2BC
#define AD6676_JESDSYN_STATUS		0x2DC

#define AD6676_SHUFFLE_THREG0		0x342
#define AD6676_SHUFFLE_THREG1		0x343

/*
 * AD6676_SPI_CONFIG
 */

#define SPI_CONF_SW_RESET		(0x81)
#define SPI_CONF_SDIO_DIR		(0x18)


/*
 * AD6676_CLKSYN_STATUS, AD6676_JESDSYN_STATUS
 */
#define SYN_STAT_PLL_LCK		(1 << 3)
#define SYN_STAT_VCO_CAL_BUSY		(1 << 1)
#define SYN_STAT_CP_CAL_DONE		(1 << 0)

/*
 * AD6676_DP_CTRL
 */
#define DP_CTRL_OFFSET_BINARY		(1 << 0)
#define DP_CTRL_TWOS_COMPLEMENT		(0 << 0)

/*
 * AD6676_TEST_GEN
 */
#define TESTGENMODE_OFF			0x0
#define TESTGENMODE_ALT_CHECKERBOARD	0x1
#define TESTGENMODE_ONE_ZERO_TOGGLE	0x2
#define TESTGENMODE_PN23_SEQ		0x3
#define TESTGENMODE_PN9_SEQ		0x4
#define TESTGENMODE_REP_USER_PAT	0x5
#define TESTGENMODE_SING_USER_PAT	0x6
#define TESTGENMODE_RAMP		0x7
#define TESTGENMODE_MOD_RPAT		0x8
#define TESTGENMODE_JSPAT		0x10
#define TESTGENMODE_JTSPAT		0x11

/*
 * AD6676_CLKSYN_R_DIV
 */

#define R_DIV(x)			((x) << 6);
#define CLKSYN_R_DIV_RESERVED		0x3D

/*
 * AD6676_CLKSYN_ENABLE
 */
#define EN_EXT_CK			(1 << 7)
#define EN_ADC_CK			(1 << 6)
#define EN_SYNTH			(1 << 5)
#define EN_VCO_PTAT			(1 << 4)
#define EN_VCO_ALC			(1 << 3)
#define EN_VCO				(1 << 2)
#define EN_OVER_IDE_CAL			(1 << 1)
#define EN_OVER_IDE			(1 << 0)

/*
 * AD6676_CLKSYN_LOGEN
 */

#define RESET_CAL			(1 << 3)

/*
 * AD6676_CLKSYN_VCO_CAL
 */

#define INIT_ALC_VALUE(x)		((x) << 4)
#define ALC_DIS				(1 << 3)

/*
 * AD6676_CLKSYN_CP_CAL
 */
#define CP_CAL_EN			(1 << 7)

/*
 * AD6676_DEC_MODE
 */

#define DEC_32				1
#define DEC_24				2
#define DEC_16				3
#define DEC_12				4

/*
 * AD6676_CAL_CMD
 */

#define XCMD3				(1 << 7)
#define XCMD2				(1 << 6)
#define XCMD1				(1 << 5)
#define XCMD0				(1 << 4)
#define RESON1_CAL      		(1 << 3)
#define FLASH_CAL       		(1 << 2)
#define INIT_ADC        		(1 << 1)
#define TUNE_ADC			(1 << 0)

/*
 * AD6676_SYNCB_CTRL
 */
#define PD_SYSREF_RX			(1 << 3)
#define LVDS_SYNCB			(1 << 2)

/* AD6676_L */
#define SCR				(1 << 7)

/* AD6676_FORCE_END_CAL */
#define FORCE_END_CAL			(1 << 0)

/* AD6676_CAL_DONE */
#define CAL_DONE			(1 << 0)

#define MHz 				1000000UL
#define MIN_FADC			2000000000ULL /* SPS */
#define MIN_FADC_INT_SYNTH		2925000000ULL /* SPS REVISIT */
#define MAX_FADC			3200000000ULL /* SPS */

#define MIN_FIF				70000000ULL /* Hz */
#define MAX_FIF				450000000ULL /* Hz */

#define MIN_BW				20000000ULL /* Hz */
#define MAX_BW				160000000ULL /* Hz */

#define CHIP_ID1_AD6676			0x03
#define CHIP_ID0_AD6676			0xBB

typedef struct {
	uint32_t 	ref_clk; // reference_clk rate Hz
	uint32_t	f_adc_hz; // adc frequency Hz
	uint32_t	f_if_hz; // intermediate frequency hz
	uint32_t	bw_hz; // bandwidth Hz;
	uint8_t		bw_margin_low_mhz;
	uint8_t		bw_margin_high_mhz;
	int8_t		bw_margin_if_mhz;
	uint8_t		decimation; // decimation
	uint8_t		ext_l; // external inductance l_nh
	uint8_t		attenuation; //
	uint8_t		scale; // fullscale adjust
	uint8_t		use_extclk; // use external clk enable
	uint8_t		spi3wire; // set device spi intereface 3/4 wires
	// shuffle
	uint8_t		shuffle_ctrl; // shuffler control
	uint8_t		shuffle_thresh; // shuffler threshold
	// jesd
	uint8_t 	scrambling_en; // jesd_scrambling_enable
	uint8_t 	lvds_syncb; // jesd_use_lvds_syncb_enable
	uint8_t 	sysref_pd; // jesd_powerdown_sysref_enable
	uint8_t 	n_lanes; // lanes
	uint8_t 	frames_per_multiframe;
	uint64_t	m;
} ad6676_init_param;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t ad6676_spi_read(spi_device *dev,
							uint16_t reg_addr,
							uint8_t *reg_data);
int32_t ad6676_spi_write(spi_device *dev,
							uint16_t reg_addr,
							uint8_t reg_data);
int32_t ad6676_setup(spi_device *dev,
							ad6676_init_param *init_param);
int32_t ad6676_set_attenuation(spi_device *dev,
							ad6676_init_param *init_param);
int32_t ad6676_set_fif(spi_device *dev,
							ad6676_init_param *init_param);
uint64_t ad6676_get_fif(spi_device *dev,
							ad6676_init_param *init_param);
int32_t ad6676_test(spi_device *dev,
							uint32_t test_mode);
#endif
