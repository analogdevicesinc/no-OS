/***************************************************************************//**
 *   @file   adf4368.h
 *   @brief  Implementation of ADF4368 Driver.
 *   @author Sirac Kucukarabacioglu (sirac.kucukarabacioglu@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#ifndef ADF4368_H_
#define ADF4368_H_

#include <stdint.h>
#include <string.h>
#include "no_os_units.h"
#include "no_os_util.h"
#include "no_os_spi.h"

/* ADF4368 REG0000 Map */
#define ADF4368_SOFT_RESET_R_MSK		NO_OS_BIT(7)
#define ADF4368_LSB_FIRST_R_MSK			NO_OS_BIT(6)
#define ADF4368_ADDRESS_ASC_R_MSK		NO_OS_BIT(5)
#define ADF4368_SDO_ACTIVE_R_MSK		NO_OS_BIT(4)
#define ADF4368_SDO_ACTIVE_MSK			NO_OS_BIT(3)
#define ADF4368_ADDRESS_ASC_MSK			NO_OS_BIT(2)
#define ADF4368_LSB_FIRST_MSK			NO_OS_BIT(1)
#define ADF4368_SOFT_RESET_MSK			NO_OS_BIT(0)
#define ADF4368_RESET_CMD			0x81

/* ADF4368 REG0000 NO_OS_BIT Definition */
#define ADF4368_SDO_ACTIVE_SPI_3W		0x0
#define ADF4368_SDO_ACTIVE_SPI_4W		0x1

#define ADF4368_ADDR_ASC_AUTO_DECR		0x0
#define ADF4368_ADDR_ASC_AUTO_INCR		0x1

#define ADF4368_LSB_FIRST_MSB			0x0
#define ADF4368_LSB_FIRST_LSB			0x1

#define ADF4368_SOFT_RESET_N_OP			0x0
#define ADF4368_SOFT_RESET_EN			0x1

/* ADF4368 REG0001 NO_OS_BIT Definition */
#define ADF4368_SINGLE_INSTR_MSK		NO_OS_BIT(7)
#define ADF4368_MASTER_RB_CTRL_MSK		NO_OS_BIT(5)

#define ADF4368_SPI_STREAM_EN			0x0
#define ADF4368_SPI_STREAM_DIS			0x1

/* ADF4368 REG0003 NO_OS_BIT Definition */
#define ADF4368_CHIP_TYPE			0x06

/* ADF4368 REG0004 NO_OS_BIT Definition */
#define ADF4368_PRODUCT_ID_LSB			0x0005

/* ADF4368 REG0005 NO_OS_BIT Definition */
#define ADF4368_PRODUCT_ID_MSB			0x0005

/* ADF4368 REG000A Map */
#define ADF4368_SCRATCHPAD_MSK			NO_OS_GENMASK(7, 0)

/* ADF4368 REG0010 Map*/
#define ADF4368_N_INT_LSB_MSK			NO_OS_GENMASK(7, 0)

/* ADF4368 REG0011 Map*/
#define ADF4368_CLKOUT_DIV_MSK			NO_OS_GENMASK(7, 6)
#define ADF4368_INT_MODE_MSK			NO_OS_BIT(5)
#define ADF4368_INV_CLK_OUT_MSK			NO_OS_BIT(4)
#define ADF4368_N_INT_MSB_MSK			NO_OS_GENMASK(3, 0)

/* ADF4368 REG0012 Map */
#define ADF4368_FRAC1WORD_LSB_MSK		NO_OS_GENMASK(7, 0)

/* ADF4368 REG0013 Map */
#define ADF4368_FRAC1WORD_MID_MSK		NO_OS_GENMASK(7, 0)

/* ADF4368 REG0014 Map */
#define ADF4368_FRAC1WORD_MSB_MSK		NO_OS_GENMASK(7, 0)


/* ADF4368 REG0015 Map */
#define ADF4368_M_VCO_CORE_MSK			NO_OS_GENMASK(7, 6)
#define ADF4368_M_VCO_BIAS_MSK			NO_OS_GENMASK(5, 2)
#define ADF4368_CMOS_OV_MSK			NO_OS_BIT(1)
#define ADF4368_FRAC1WORD_MSB			NO_OS_BIT(0)

/* ADF4368 REG0016 Map */
#define ADF4368_M_VCO_BAND_MSK			NO_OS_GENMASK(7, 0)

/* ADF4368 REG0017 Map */
#define ADF4368_FRAC2WORD_LSB_MSK		NO_OS_GENMASK(7, 0)

/* ADF4368 REG0018 Map */
#define ADF4368_FRAC2WORD_MID_MSK		NO_OS_GENMASK(7, 0)

/* ADF4368 REG0019 Map */
#define ADF4368_FRAC2WORD_MSB_MSK		NO_OS_GENMASK(7, 0)

/* ADF4368 REG001A Map */
#define ADF4368_MOD2WORD_LSB_MSK		NO_OS_GENMASK(7, 0)

/* ADF4368 REG001B Map */
#define ADF4368_MOD2WORD_MID_MSK		NO_OS_GENMASK(7, 0)

/* ADF4368 REG001C Map */
#define ADF4368_MOD2WORD_MSB_MSK		NO_OS_GENMASK(7, 0)

/* ADF4368 REG001D Map */
#define ADF4368_FINE_BLEED_LSB_MSK		NO_OS_GENMASK(7, 0)

/* ADF4368 REG001E Map */
#define ADF4368_EN_PHASE_RESYNC_MSK		NO_OS_BIT(7)
#define ADF4368_EN_REF_RST_MSK			NO_OS_BIT(6)
#define ADF4368_TIMED_SYNC_MSK			NO_OS_BIT(5)
#define ADF4368_COARSE_BLEED_MSK		NO_OS_GENMASK(4, 1)
#define ADF4368_FINE_BLEED_MSB_MSK		NO_OS_BIT(0)

/* ADF4368 REG001F Map */
#define ADF4368_SW_SYNC_MSK			NO_OS_BIT(7)
#define ADF4368_PHASE_ADJ_MSK			NO_OS_BIT(6)
#define ADF4368_BLEED_POL_MSK			NO_OS_BIT(5)
#define ADF4368_EN_BLEED_MSK			NO_OS_BIT(4)
#define ADF4368_CP_I_MSK			NO_OS_GENMASK(3, 0)

/* ADF4368 REG0020 Map */
#define ADF4368_EN_AUTOCAL_MSK			NO_OS_BIT(7)
#define ADF4368_EN_RDBLR_MSK			NO_OS_BIT(6)
#define ADF4368_R_DIV_MSK			NO_OS_GENMASK(5, 0)

/* ADF4368 REG0021 Map */
#define ADF4368_PHASE_WORD_LSB_MSK		NO_OS_GENMASK(7, 0)

/* ADF4368 REG0022 Map */
#define ADF4368_PHASE_WORD_MID_MSK		NO_OS_GENMASK(7, 0)

/* ADF4368 REG0023 Map */
#define ADF4368_PHASE_WORD_MSB_MSK		NO_OS_GENMASK(7, 0)

/* ADF4368 REG0024 Map */
#define ADF4368_PHASE_ADJUSTMENT_MSK		NO_OS_GENMASK(7, 0)

/* ADF4368 REG0025 Map */
#define ADF4368_RESYNC_WAIT_LSB_MSK		NO_OS_GENMASK(7, 0)

/* ADF4368 REG0026 Map */
#define ADF4368_RESYNC_WAIT_MID_MSK		NO_OS_GENMASK(7, 0)

/* ADF4368 REG0027 Map */
#define ADF4368_RESYNC_WAIT_MSB_MSK		NO_OS_GENMASK(7, 0)

/* ADF4368 REG0028 Map */
#define ADF4368_REG28_RSV1		        NO_OS_BIT(7)
#define ADF4368_LSB_P1				NO_OS_BIT(6)
#define ADF4368_VAR_MOD_EN_MSK			NO_OS_BIT(5)
#define ADF4368_REG28_RSV2		        NO_OS_GENMASK(4, 2)
#define ADF4368_REG28_RSV3		        NO_OS_BIT(1)
#define ADF4368_REG28_RSV4		        NO_OS_BIT(0)

/* ADF4368 REG0029 Map */
#define ADF4368_CLK2_OPWR_MSK			NO_OS_GENMASK(7, 4)
#define ADF4368_CLK1_OPWR_MSK			NO_OS_GENMASK(3, 0)

/* ADF4368 REG002A Map */
#define ADF4368_REG2A_RSV5		        NO_OS_BIT(7)
#define ADF4368_ADJ_POL_MSK		        NO_OS_BIT(6)
#define ADF4368_REG2A_RSV4		        NO_OS_BIT(5)
#define ADF4368_PD_SYNC		            	NO_OS_BIT(4)
#define ADF4368_REG2A_RSV3		        NO_OS_BIT(3)
#define ADF4368_PD_RDET		            	NO_OS_BIT(2)
#define ADF4368_REG2A_RSV2		        NO_OS_BIT(1)
#define ADF4368_REG2A_RSV1		        NO_OS_BIT(0)


/* ADF4368 REG002B Map */
#define ADF4368_PD_ALL			        NO_OS_BIT(7)
#define ADF4368_REG2B_RSV4			NO_OS_BIT(6)
#define ADF4368_REG2B_RSV3			NO_OS_BIT(5)
#define ADF4368_REG2B_RSV2			NO_OS_BIT(4)
#define ADF4368_PD_LD			        NO_OS_BIT(3)
#define ADF4368_REG2B_RSV1			NO_OS_BIT(2)
#define ADF4368_PD_CLKOUT1_MSK			NO_OS_BIT(1)
#define ADF4368_PD_CLKOUT2_MSK			NO_OS_BIT(0)

/* ADF4368 REG002C Map */
#define ADF4368_LDWIN_PW_MSK			NO_OS_GENMASK(7, 5)
#define ADF4368_LD_COUNT_MSK			NO_OS_GENMASK(4, 0)

/* ADF4368 REG002D Map */
#define ADF4368_EN_DNCLK_MSK			NO_OS_BIT(7)
#define ADF4368_EN_DRCLK_MSK			NO_OS_BIT(6)
#define ADF4368_EN_LOL_MSK			NO_OS_BIT(5)
#define ADF4368_EN_LDWIN_MSK			NO_OS_BIT(4)
#define ADF4368_REG2D_RSV2			NO_OS_BIT(3)
#define ADF4368_RST_LD_MSK			NO_OS_BIT(2)
#define ADF4368_REG2D_RSV1			NO_OS_GENMASK(1, 0)

/* ADF4368 REG002E Map */
#define ADF4368_MUXOUT_MSK			NO_OS_GENMASK(7, 4)
#define ADF4368_REG2E_RSV1			NO_OS_BIT(3)
#define ADF4368_EN_CPTEST_MSK			NO_OS_BIT(2)
#define ADF4368_CP_DOWN_MSK			NO_OS_BIT(1)
#define ADF4368_CP_UP_MSK			NO_OS_BIT(0)

/* ADF4368 REG002F Map*/
#define ADF4368_BST_REF_MSK			NO_OS_BIT(7)
#define ADF4368_FILT_REF_MSK			NO_OS_BIT(6)
#define ADF4368_REF_SEL_MSK			NO_OS_BIT(5)
#define ADF4368_REG2F_RSV2			NO_OS_BIT(5)
#define ADF4368_REG2F_RSV1			NO_OS_GENMASK(3, 0)

/* ADF4368 REG0030 Map */
#define ADF4368_MUTE_NCLK_MSK			NO_OS_BIT(7)
#define ADF4368_REG30_RSV3			NO_OS_BIT(6)
#define ADF4368_DRCLK_DEL_MSK			NO_OS_GENMASK(5, 3)
#define ADF4368_DNCLK_DEL_MSK			NO_OS_GENMASK(2, 0)

/* ADF4368 REG0031 Map */
#define ADF4368_SYNC_DEL_MSK			NO_OS_GENMASK(7, 5)
#define ADF4368_RST_SYS_MSK			NO_OS_BIT(4)
#define ADF4368_EN_ADC_CLK_MSK			NO_OS_BIT(3)
#define ADF4368_REG31_RSV3			NO_OS_BIT(2)
#define ADF4368_REG31_RSV2			NO_OS_BIT(1)
#define ADF4368_REG31_RSV1			NO_OS_BIT(0)

/* ADF4368 REG0035 Map */
#define ADF4368_REG35_RSV4			NO_OS_BIT(7)
#define ADF4368_REG35_RSV3			NO_OS_GENMASK(6, 3)
#define ADF4368_DCLK_MODE_MSK			NO_OS_BIT(2)
#define ADF4368_REG35_RSV2			NO_OS_BIT(1)
#define ADF4368_REG35_RSV1			NO_OS_BIT(0)

/* ADF4368 REG0036 Map */
#define ADF4368_CLKODIV_DB_MSK			NO_OS_BIT(7)
#define ADF4368_DCLK_DIV_DB_MSK			NO_OS_BIT(6)
#define ADF4368_REG36_RSV3			NO_OS_BIT(5)
#define ADF4368_REG36_RSV2			NO_OS_BIT(4)
#define ADF4368_REG36_RSV1			NO_OS_GENMASK(3, 0)

/* ADF4368 REG0037 Map */
#define ADF4368_VCO_BAND_DIV			NO_OS_GENMASK(7, 0)

/* ADF4368 REG0038 Map */
#define ADF4368_SYNTH_LOCK_TIMEOUT_LSB_MSK	NO_OS_GENMASK(7, 0)

/* ADF4368 REG0039 Map */
#define ADF4368_O_VCO_DB_MSK			NO_OS_BIT(7)
#define ADF4368_SYNTH_LOCK_TIMEOUT_MSB_MSK	NO_OS_GENMASK(6, 0)


/* ADF4368 REG003A Map */
#define ADF4368_VCO_ALC_TIMEOUT_LSB_MSK		NO_OS_GENMASK(7, 0)

/* ADF4368 REG003B Map */
#define ADF4368_DEL_CTRL_DB_MSK			NO_OS_BIT(7)
#define ADF4368_VCO_ALC_TIMEOUT_MSB_MSK		NO_OS_GENMASK(6, 0)

/* ADF4368 REG003E Map */
#define ADF4368_ADC_CLK_DIV_MSK			NO_OS_GENMASK(7, 0)

/* ADF4368 REG003F Map */
#define ADF4368_EN_ADC_CNV_MSK			NO_OS_BIT(7)
#define ADF4368_REG3F_RSV5		        NO_OS_BIT(6)
#define ADF4368_REG3F_RSV4		        NO_OS_BIT(5)
#define ADF4368_REG3F_RSV3			NO_OS_BIT(4)
#define ADF4368_REG3F_RSV2			NO_OS_BIT(3)
#define ADF4368_REG3F_RSV1			NO_OS_BIT(2)
#define ADF4368_EN_ADC_MSK			NO_OS_BIT(1)
#define ADF4368_ADC_A_CONV_MSK			NO_OS_BIT(0)

/* ADF4368 REG0040 Map */
#define ADF4368_REG40_RSV1			NO_OS_GENMASK(5, 3)
#define ADF4368_MUTE_CLKOUT2_MSK		NO_OS_GENMASK(5, 3)
#define ADF4368_MUTE_CLKOUT1_MSK		NO_OS_GENMASK(2, 0)

/* ADF4368 REG0043 Map */
#define ADF4368_REG43_RSV5			NO_OS_BIT(7)
#define ADF4368_ADC_CLK_SEL_MSK			NO_OS_BIT(6)
#define ADF4368_REG43_RSV4			NO_OS_BIT(5)
#define ADF4368_REG43_RSV3			NO_OS_BIT(4)
#define ADF4368_REG43_RSV2			NO_OS_BIT(3)
#define ADF4368_REG43_RSV1			NO_OS_GENMASK(2, 0)

/* ADF4368 REG004E Map */
#define ADF4368_REG4E_RSV2			NO_OS_GENMASK(7, 6)
#define ADF4368_DCLK_DIV1_MSK			NO_OS_GENMASK(5, 4)
#define ADF4368_O_VCO_BAND_MSK			NO_OS_BIT(3)
#define ADF4368_O_VCO_CORE_MSK			NO_OS_BIT(2)
#define ADF4368_O_VCO_BIAS_MSK			NO_OS_BIT(1)
#define ADF4368_REG4E_RSV1			NO_OS_BIT(0)

/* ADF4368 REG0053 Map */
#define ADF4368_REG53_RSV2			NO_OS_BIT(7)
#define ADF4368_PD_SYNC_MON_MSK			NO_OS_BIT(6)
#define ADF4368_SYNC_SEL_MSK			NO_OS_BIT(5)
#define ADF4368_RST_SYNC_MON_MSK		NO_OS_BIT(4)
#define ADF4368_REG53_RSV1			NO_OS_GENMASK(3, 0)

/* ADF4368 REG0054 Map */
#define ADF4368_REG54_RSV1			NO_OS_GENMASK(7, 1)
#define ADF4368_ADC_ST_CNV_MSK			NO_OS_BIT(1)

/* ADF4368 REG0058 Map */
#define ADF4368_EN_CLK2_MSK 			NO_OS_BIT(7)
#define ADF4368_EN_CLK1_MSK		        NO_OS_BIT(6)
#define ADF4368_SYNC_OK_MSK			NO_OS_BIT(5)
#define ADF4368_REG58_RSV1			NO_OS_BIT(4)
#define ADF4368_REF_OK_MSK			NO_OS_BIT(3)
#define ADF4368_ADC_BUSY_MSK			NO_OS_BIT(2)
#define ADF4368_FSM_BUSY_MSK			NO_OS_BIT(1)
#define ADF4368_LOCKED_MSK			NO_OS_BIT(0)

/* ADF4368 REG005A Map */
#define ADF4368_REG5A_RSV1			NO_OS_GENMASK(7, 2)
#define ADF4368_VCO_CORE_MSK			NO_OS_GENMASK(1, 0)

/* ADF4368 REG005B Map */
#define ADF4368_CHIP_TEMP_LSB_MSK		NO_OS_GENMASK(7, 0)

/* ADF4368 REG005C Map */
#define ADF4368_REG5C_RSV1			NO_OS_GENMASK(7, 1)
#define ADF4368_CHIP_TEMP_MSB_MSK		NO_OS_BIT(0)

/* ADF4368 REG005E Map */
#define ADF4368_VCO_BAND_MSK			NO_OS_GENMASK(7, 0)

/* ADF4368 REG0060 Map */
#define ADF4368_REG60_RSV1			NO_OS_GENMASK(7, 4)
#define ADF4368_VCO_BIAS_MSK			NO_OS_GENMASK(3, 0)

/* ADF4368 REG0063 Map */
#define ADF4368_VERSION_MSK		        NO_OS_GENMASK(7, 0)



#define ADF4368_SPI_4W_CFG(x)	(no_os_field_prep(ADF4368_SDO_ACTIVE_MSK, x) | \
					no_os_field_prep(ADF4368_SDO_ACTIVE_R_MSK, x))

#define ADF4368_SPI_LSB_CFG(x)	(no_os_field_prep(ADF4368_LSB_FIRST_MSK, x) | \
					no_os_field_prep(ADF4368_LSB_FIRST_R_MSK, x))

#define ADF4368_BLEED_MSB_MSK			(ADF4368_COARSE_BLEED_MSK | \
						 ADF4368_FINE_BLEED_MSB_MSK)

#define ADF4368_SPI_SCRATCHPAD_TEST		0x5A

/* Specifications */
#define ADF4368_SPI_WRITE_CMD			0x0
#define ADF4368_SPI_READ_CMD			0x8000
#define ADF4368_SPI_DUMMY_DATA			0x00
#define ADF4368_BUFF_SIZE_BYTES			3
#define ADF4368_VCO_FREQ_MIN			6400000000U	// 6.4GHz
#define ADF4368_VCO_FREQ_MAX			12800000000U	// 12.8GHz
#define ADF4368_MOD1WORD			0x2000000U	// 2^25
#define ADF4368_MOD2WORD_MAX			0xFFFFFFU	// 2^24 - 1
#define ADF4368_PHASE_RESYNC_MOD2WORD_MAX	0x1FFFFU	// 2^17 - 1
#define ADF4368_CHANNEL_SPACING_MAX		78125U
#define ADF4368_PFD_FREQ_MAX			625000000U	// 625MHz
#define ADF4368_PFD_FREQ_FRAC_MAX		250000000U	// 250MHz
#define ADF4368_DCLK_DIV1_0_MAX			160000000U	// 160MHz
#define ADF4368_DCLK_DIV1_1_MAX			320000000U	// 320MHz
#define ADF4368_CLKOUT_DIV_REG_VAL_MAX		3

#define ADF4368_RFOUT_MAX			12800000000U // 12.8GHz
#define ADF4368_RFOUT_MIN			800000000U // 0.8GHz
#define ADF4368_REF_CLK_MAX			4000000000U // 4GHz
#define ADF4368_REF_CLK_MIN			10000000 // 10MHz
#define ADF4368_REF_DIV_MAX			63
#define ADF4368_OUT_PWR_MAX			15
#define ADF4368_CPI_VAL_MAX			15
#define ADF4368_BLEED_WORD_MAX			8191
#define ADF4368_FRAC_N_INT_MIN			19
#define ADF4368_INT_N_INT_MIN			4

#define ADF4368_PHASE_BLEED_CNST		2044000
#define ADF4368_BLEED_N_INT_TH			35
#define ADF4368_COARSE_BLEED_CNST		202 // 202ua
#define ADF4368_FINE_BLEED_CNST1		250 // 250ua
#define ADF4368_FINE_BLEED_CNST2		512 // 250ua
#define ADF4368_FINE_BLEED_CNST3		567 // 567na

#define ADF4368_SIGMA_DELTA_MOD_CNST		4096 // 250ua

#define ADF4368_POR_DELAY_US			200
#define ADF4368_LKD_DELAY_MS			9

#define ADF4368_MHZ					MEGA
#define ADF4368_S_TO_NS					NANO
#define ADF4368_NS_TO_PS				KHZ_PER_MHZ
#define ADF4368_PS_TO_NS				KHZ_PER_MHZ
#define ADF4368_PS_TO_US				MICRO
#define ADF4368_US_TO_MS				MILLI
#define ADF4368_MS_TO_S					MILLI
#define ADF4368_PS_TO_S					PICO
#define ADF4368_US_TO_FS				NANO
#define ADF4368_NS_TO_S					NANO

/**
 * @struct adf4368_init_param
 * @brief ADF4368 Initialization Parameters structure.
 */
struct adf4368_init_param {
	/** SPI Initialization parameters */
	struct no_os_spi_init_param	*spi_init;
	bool				spi_4wire_en;
	bool				cmos_3v3;
	bool				ref_doubler_en;
	uint8_t				ref_div;
	uint8_t				cp_i;
	uint8_t				ld_count;
	uint64_t			ref_freq_hz;
	uint64_t			freq;
};

/**
 * @struct adf4368_dev
 * @brief ADF4368 Device Descriptor.
 */
struct adf4368_dev {
	/** SPI Descriptor */
	struct no_os_spi_desc		*spi_desc;
	bool				spi_4wire_en;
	bool				cmos_3v3;
	bool				phase_pol;
	bool				ref_doubler_en;
	uint8_t				ref_div;
	uint8_t				cp_i;
	uint8_t				ld_count;
	uint8_t				clkout_div_reg_val_max;
	uint16_t 			n_int;
	int16_t				bleed_word;
	uint32_t			phase_adj;
	uint64_t			vco_max;
	uint64_t			vco_min;
	uint64_t			freq_max;
	uint64_t			freq_min;
	uint64_t			ref_freq_hz;
	uint64_t			freq;
};

/** ADF4368 SPI write */
int adf4368_spi_write(struct adf4368_dev *dev, uint16_t reg_addr, uint8_t data);

/** ADF4368 SPI Read */
int adf4368_spi_read(struct adf4368_dev *dev, uint16_t reg_addr, uint8_t *data);

/** ADF4368 updates a bit in the register space over SPI */
int adf4368_spi_update_bits(struct adf4368_dev *dev, uint16_t reg_addr,
			    uint8_t mask, uint8_t data);

/** ADF4368 Register dump */
int adf4368_reg_dump(struct adf4368_dev *dev);

/** ADF4368 Set output power attributes */
int adf4368_set_out_power(struct adf4368_dev *dev, uint8_t ch, int32_t pwr);

/** ADF4368 Get output power attributes */
int adf4368_get_out_power(struct adf4368_dev *dev, uint8_t ch, int32_t *pwr);

/** ADF4368 Set channel enable attributes */
int adf4368_set_en_chan(struct adf4368_dev *dev, uint8_t ch, bool en);

/** ADF4368 Get channel enable attributes */
int adf4368_get_en_chan(struct adf4368_dev *dev, uint8_t ch, bool *en);

/** ADF4368 Set reference frequency attribute */
int adf4368_set_ref_clk(struct adf4368_dev *dev, uint64_t val);

/** ADF4368 Get reference frequency attribute */
int adf4368_get_ref_clk(struct adf4368_dev *dev, uint64_t *val);

/** ADF4368 Set reference doubler attribute */
int adf4368_set_en_ref_doubler(struct adf4368_dev *dev, bool en);

/** ADF4368 Get reference doubler attribute */
int adf4368_get_en_ref_doubler(struct adf4368_dev *dev, bool *en);

/** ADF4368 Set reference divider attribute */
int adf4368_set_ref_div(struct adf4368_dev *dev, int32_t div);

/** ADF4368 Get reference divider attribute */
int adf4368_get_ref_div(struct adf4368_dev *dev, int32_t *div);

/** ADF4368 Set charge pump current attribute */
int adf4368_set_cp_i(struct adf4368_dev *dev, int32_t reg_val);

/** ADF4368 Get charge pump current attribute */
int adf4368_get_cp_i(struct adf4368_dev *dev, int32_t *reg_val);

/** ADF4368 Set bleed current attribute */
int adf4368_set_bleed_word(struct adf4368_dev *dev, int32_t word);

/** ADF4368 Get bleed current attribute */
int adf4368_get_bleed_word(struct adf4368_dev *dev, int32_t *word);

/** ADF4368 Set output frequency attribute */
int adf4368_set_rfout(struct adf4368_dev *dev, uint64_t val);

/** ADF4368 Get output frequency attribute */
int adf4368_get_rfout(struct adf4368_dev *dev, uint64_t *val);

/** ADF4368 Set sync_setup attribute */
int adf4368_set_sync_setup(struct adf4368_dev *dev, bool en);

/** ADF4368 Get sync_setup attribute */
int adf4368_get_sync_setup(struct adf4368_dev *dev, bool *en);

/** ADF4368 Set sw_sync attribute */
int adf4368_set_sw_sync(struct adf4368_dev *dev, uint8_t sw_sync);

/** ADF4368 Get sw_sync attribute */
int adf4368_get_sw_sync(struct adf4368_dev *dev, uint8_t *sw_sync);

/** ADF4368 Set temperature readback setup attribute */
int adf4368_set_temperature(struct adf4368_dev *dev, bool en);

/** ADF4368 Get temperature readback attribute */
int adf4368_get_temperature(struct adf4368_dev *dev, int32_t *temp);

/** ADF4368 Set phase adjust in frac mode attribute */
int adf4368_set_phase_sdm(struct adf4368_dev *dev, uint32_t phase_fs);

/** ADF4368 Get phase adjust in frac mode attribute */
int adf4368_get_phase_sdm(struct adf4368_dev *dev, uint32_t *phase_fs);

/** ADF4368 Set phase adjust in int mode attribute */
int adf4368_set_phase_bleedi(struct adf4368_dev *dev, uint32_t phase_fs,
			     bool phase_pol);

/** ADF4368 Set phase adjustment value */
int adf4368_set_phase(struct adf4368_dev *dev, uint32_t phase_fs,
		      bool phase_pol);

/** ADF4368 Get phase adjustment value */
int adf4368_get_phase(struct adf4368_dev *dev, uint32_t *phase_fs,
		      bool *phase_pol);

/** ADF4368 Set default registers and SPI 4 Wire mode */
int adf4368_set_default_regs(struct adf4368_dev *dev, bool spi_4wire);

/** ADF4368 Get default registers and check */
int adf4368_get_default_regs(struct adf4368_dev *dev, bool *spi_4wire);

/** ADF4368 Sets frequency */
int adf4368_set_freq(struct adf4368_dev *dev);

/** ADF4368 Initialization */
int adf4368_init(struct adf4368_dev **device,
		 struct adf4368_init_param *init_param);

/** ADF4368 Remove */
int adf4368_remove(struct adf4368_dev *dev);



#endif /* ADF4368_H_ */
