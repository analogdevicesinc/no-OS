/***************************************************************************//**
 *   @file   adf4382.h
 *   @brief  Implementation of adf4382 Driver.
 *   @author Ciprian Hegbeli (ciprian.hegbeli@analog.com)
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

#include <stdint.h>
#include <string.h>
#include "no_os_units.h"
#include "no_os_util.h"
#include "no_os_spi.h"

/* ADF4382 REG0000 Map */
#define ADF4382_SOFT_RESET_R_MSK		NO_OS_BIT(7)
#define ADF4382_LSB_FIRST_R_MSK			NO_OS_BIT(6)
#define ADF4382_ADDRESS_ASC_R_MSK		NO_OS_BIT(5)
#define ADF4382_SDO_ACTIVE_R_MSK		NO_OS_BIT(4)
#define ADF4382_SDO_ACTIVE_MSK			NO_OS_BIT(3)
#define ADF4382_ADDRESS_ASC_MSK			NO_OS_BIT(2)
#define ADF4382_LSB_FIRST_MSK			NO_OS_BIT(1)
#define ADF4382_SOFT_RESET_MSK			NO_OS_BIT(0)
#define ADF4382_RESET_CMD			0x81

/* ADF4382 REG0000 NO_OS_BIT Definition */
#define ADF4382_SDO_ACTIVE_SPI_3W		0x0
#define ADF4382_SDO_ACTIVE_SPI_4W		0x1

#define ADF4382_ADDR_ASC_AUTO_DECR		0x0
#define ADF4382_ADDR_ASC_AUTO_INCR		0x1

#define ADF4382_LSB_FIRST_MSB			0x0
#define ADF4382_LSB_FIRST_LSB			0x1

#define ADF4382_SOFT_RESET_N_OP			0x0
#define ADF4382_SOFT_RESET_EN			0x1

/* ADF4382 REG0001 Map */
#define ADF4382_SINGLE_INSTR_MSK		NO_OS_BIT(7)
#define ADF4382_MASTER_RB_CTRL_MSK		NO_OS_BIT(5)

/* ADF4382 REG0001 NO_OS_BIT Definition */
#define ADF4382_SPI_STREAM_EN			0x0
#define ADF4382_SPI_STREAM_DIS			0x1

#define ADF4382_RB_SLAVE_REG			0x0
#define ADF4382_RB_MASTER_REG			0x1

/* ADF4382 REG0003 NO_OS_BIT Definition */
#define ADF4382_CHIP_TYPE			0x06

/* ADF4382 REG0004 NO_OS_BIT Definition */
#define ADF4382_PRODUCT_ID_LSB			0x0005

/* ADF4382 REG0005 NO_OS_BIT Definition */
#define ADF4382_PRODUCT_ID_MSB			0x0005

/* ADF4382 REG000A Map */
#define ADF4382_SCRATCHPAD_MSK			NO_OS_GENMASK(7, 0)

/* ADF4382 REG000C NO_OS_BIT Definition */
#define ADF4382_VENDOR_ID_LSB			0x56

/* ADF4382 REG000D NO_OS_BIT Definition */
#define ADF4382_VENDOR_ID_MSB			0x04

/* ADF4382 REG000F NO_OS_BIT Definition */
#define ADF4382_M_S_TRANSF_NO_OS_BIT_MSK	NO_OS_BIT(0)

/* ADF4382 REG0010 Map*/
#define ADF4382_N_INT_LSB_MSK			NO_OS_GENMASK(7, 0)

/* ADF4382 REG0011 Map*/
#define ADF4382_CLKOUT_DIV_MSK			NO_OS_GENMASK(7, 5)
#define ADF4382_INV_CLK_OUT_MSK			NO_OS_BIT(4)
#define ADF4382_N_INT_MSB_MSK			NO_OS_GENMASK(3, 0)

/* ADF4382 REG0012 Map */
#define ADF4382_FRAC1WORD_LSB_MSK		NO_OS_GENMASK(7, 0)

/* ADF4382 REG0013 Map */
#define ADF4382_FRAC1WORD_MID_MSK		NO_OS_GENMASK(7, 0)

/* ADF4382 REG0014 Map */
#define ADF4382_FRAC1WORD_MSB_MSK		NO_OS_GENMASK(7, 0)

/* ADF4382 REG0015 Map */
#define ADF4382_M_VCO_BAND_LSB_MSK		NO_OS_BIT(7)
#define ADF4382_M_VCO_CORE_MSK			NO_OS_BIT(6)
#define ADF4382_BIAS_DEC_MODE_MSK		NO_OS_GENMASK(5, 3)
#define ADF4382_INT_MODE_MSK			NO_OS_BIT(2)
#define ADF4382_PFD_POL_MSK			NO_OS_BIT(1)
#define ADF4382_FRAC1WORD_MSB			NO_OS_BIT(0)

/* ADF4382 REG0016 Map */
#define ADF4382_M_VCO_BAND_MSB_MSK		NO_OS_GENMASK(7, 0)

/* ADF4382 REG0017 Map */
#define ADF4382_FRAC2WORD_LSB_MSK		NO_OS_GENMASK(7, 0)

/* ADF4382 REG0018 Map */
#define ADF4382_FRAC2WORD_MID_MSK		NO_OS_GENMASK(7, 0)

/* ADF4382 REG0019 Map */
#define ADF4382_FRAC2WORD_MSB_MSK		NO_OS_GENMASK(7, 0)

/* ADF4382 REG001A Map */
#define ADF4382_MOD2WORD_LSB_MSK		NO_OS_GENMASK(7, 0)

/* ADF4382 REG001B Map */
#define ADF4382_MOD2WORD_MID_MSK		NO_OS_GENMASK(7, 0)

/* ADF4382 REG001C Map */
#define ADF4382_MOD2WORD_MSB_MSK		NO_OS_GENMASK(7, 0)

/* ADF4382 REG001D Map */
#define ADF4382_FINE_BLEED_LSB_MSK		NO_OS_GENMASK(7, 0)

/* ADF4382 REG001E Map */
#define ADF4382_EN_PHASE_RESYNC_MSK		NO_OS_BIT(7)
#define ADF4382_EN_REF_RST_MSK			NO_OS_BIT(6)
#define ADF4382_TIMED_SYNC_MSK			NO_OS_BIT(5)
#define ADF4382_COARSE_BLEED_MSK		NO_OS_GENMASK(4, 1)
#define ADF4382_FINE_BLEED_MSB_MSK		NO_OS_BIT(0)

/* ADF4382 REG001F Map */
#define ADF4382_SW_SYNC_MSK			NO_OS_BIT(7)
#define ADF4382_SPARE_1F_MSK			NO_OS_BIT(6)
#define ADF4382_BLEED_POL_MSK			NO_OS_BIT(5)
#define ADF4382_EN_BLEED_MSK			NO_OS_BIT(4)
#define ADF4382_CP_I_MSK			NO_OS_GENMASK(3, 0)

/* ADF4382 REG0020 Map */
#define ADF4382_EN_AUTOCAL_MSK			NO_OS_BIT(7)
#define ADF4382_EN_RDBLR_MSK			NO_OS_BIT(6)
#define ADF4382_R_DIV_MSK			NO_OS_GENMASK(5, 0)

/* ADF4382 REG0021 Map */
#define ADF4382_PHASE_WORD_LSB_MSK		NO_OS_GENMASK(7, 0)

/* ADF4382 REG0022 Map */
#define ADF4382_PHASE_WORD_MID_MSK		NO_OS_GENMASK(7, 0)

/* ADF4382 REG0023 Map */
#define ADF4382_PHASE_WORD_MSB_MSK		NO_OS_GENMASK(7, 0)

/* ADF4382 REG0024 Map */
#define ADF4382_SPARE_24_MSK			NO_OS_GENMASK(7, 5)
#define ADF4382_DCLK_DIV_SEL_MSK		NO_OS_BIT(4)
#define ADF4382_DNCLK_DIV1_MSK			NO_OS_GENMASK(3, 2)
#define ADF4382_DCLK_DIV1_MSK			NO_OS_GENMASK(1, 0)

/* ADF4382 REG0025 Map */
#define ADF4382_RESYNC_WAIT_LSB_MSK		NO_OS_GENMASK(7, 0)

/* ADF4382 REG0026 Map */
#define ADF4382_RESYNC_WAIT_MSB_MSK		NO_OS_GENMASK(7, 0)

/* ADF4382 REG0027 Map */
#define ADF4382_CAL_BLEED_FINE_MIN_MSK		NO_OS_GENMASK(7, 4)
#define ADF4382_BLEED_ADJ_SCALE_MSK		NO_OS_GENMASK(3, 0)

/* ADF4382 REG0028 Map */
#define ADF4382_PH_RESYNC_RB_SEL_MSK		NO_OS_BIT(7)
#define ADF4382_LSB_P1_MSK			NO_OS_BIT(6)
#define ADF4382_VAR_MOD_EN_MSK			NO_OS_BIT(5)
#define ADF4382_DITHER1_SCALE_MSK		NO_OS_GENMASK(4, 2)
#define ADF4382_EN_DITHER2_MSK			NO_OS_BIT(1)
#define ADF4382_EN_DITHER1_MSK			NO_OS_BIT(0)

/* ADF4382 REG0029 Map */
#define ADF4382_CLK2_OPWR_MSK			NO_OS_GENMASK(7, 4)
#define ADF4382_CLK1_OPWR_MSK			NO_OS_GENMASK(3, 0)

/* ADF4382 REG002A Map */
#define ADF4382_FN_DBL_MSK			NO_OS_BIT(7)
#define ADF4382_PD_NDIV_TL_MSK			NO_OS_BIT(6)
#define ADF4382_CLKOUT_BST_MSK			NO_OS_BIT(5)
#define ADF4382_PD_SYNC_MSK			NO_OS_BIT(4)
#define ADF4382_PD_CLK_MSK			NO_OS_BIT(3)
#define ADF4382_PD_RDET_MSK			NO_OS_BIT(2)
#define ADF4382_PD_ADC_MSK			NO_OS_BIT(1)
#define ADF4382_PD_CALGEN_MSK			NO_OS_BIT(0)

/* ADF4382 REG002B Map */
#define ADF4382_PD_ALL_MSK			NO_OS_BIT(7)
#define ADF4382_PD_RDIV_TL_MSK			NO_OS_BIT(6)
#define ADF4382_PD_NDIV_MSK			NO_OS_BIT(5)
#define ADF4382_PD_VCO_MSK			NO_OS_BIT(4)
#define ADF4382_PD_LD_MSK			NO_OS_BIT(3)
#define ADF4382_PD_PFDCP_MSK			NO_OS_BIT(2)
#define ADF4382_PD_CLKOUT1_MSK			NO_OS_BIT(1)
#define ADF4382_PD_CLKOUT2_MSK			NO_OS_BIT(0)

/* ADF4382 REG002C Map */
#define ADF4382_LDWIN_PW_MSK			NO_OS_GENMASK(7, 5)
#define ADF4382_LD_COUNT_OPWR_MSK		NO_OS_GENMASK(4, 0)

/* ADF4382 REG002D Map */
#define ADF4382_EN_DNCLK_MSK			NO_OS_BIT(7)
#define ADF4382_EN_DRCLK_MSK			NO_OS_BIT(6)
#define ADF4382_EN_LOL_MSK			NO_OS_BIT(5)
#define ADF4382_EN_LDWIN_MSK			NO_OS_BIT(4)
#define ADF4382_PDET_POL_MSK			NO_OS_BIT(3)
#define ADF4382_RST_LD_MSK			NO_OS_BIT(2)
#define ADF4382_LD_O_CTRL_MSK			NO_OS_GENMASK(1, 0)

/* ADF4382 REG002E Map */
#define ADF4382_MUXOUT_MSK			NO_OS_GENMASK(7, 4)
#define ADF4382_ABPW_WD_MSK			NO_OS_BIT(3)
#define ADF4382_EN_CPTEST_MSK			NO_OS_BIT(2)
#define ADF4382_CP_DOWN_MSK			NO_OS_BIT(1)
#define ADF4382_CP_UP_MSK			NO_OS_BIT(0)

/* ADF4382 REG002F Map*/
#define ADF4382_BST_REF_MSK			NO_OS_BIT(7)
#define ADF4382_FILT_REF_MSK			NO_OS_BIT(6)
#define ADF4382_RDBLR_DC_MSK			NO_OS_GENMASK(5, 0)

/* ADF4382 REG0030 Map */
#define ADF4382_MUTE_NCLK_MSK			NO_OS_BIT(7)
#define ADF4382_MUTE_RCLK_MSK			NO_OS_BIT(6)
#define ADF4382_REF_SEL_MSK			NO_OS_BIT(5)
#define ADF4382_INV_RDBLR_MSK			NO_OS_BIT(4)
#define ADF4382_RDBLR_DEL_SEL_MSK		NO_OS_GENMASK(3, 0)

/* ADF4382 REG0031 Map */
#define ADF4382_SYNC_DEL_MSK			NO_OS_GENMASK(7, 5)
#define ADF4382_RST_SYS_MSK			NO_OS_BIT(4)
#define ADF4382_EN_ADC_CLK_MSK			NO_OS_BIT(3)
#define ADF4382_EN_VCAL_MSK			NO_OS_BIT(2)
#define ADF4382_CAL_CT_SEL_MSK			NO_OS_BIT(1)
#define ADF4382_DCLK_MODE_MSK			NO_OS_BIT(0)

/* ADF4382 REG0032 Map */
#define ADF4382_SPARE_32_MSK			NO_OS_BIT(7)
#define ADF4382_BLEED_ADJ_CAL_MSK		NO_OS_BIT(6)
#define ADF4382_DEL_MODE_MSK			NO_OS_BIT(5)
#define ADF4382_EN_AUTO_ALIGN_MSK		NO_OS_BIT(4)
#define ADF4382_PHASE_ADJ_POL_MSK		NO_OS_BIT(3)
#define ADF4382_EFM3_MODE_MSK			NO_OS_GENMASK(2, 0)

/* ADF4382 REG0033 Map */
#define ADF4382_PHASE_ADJUST_MSK		NO_OS_GENMASK(7, 0)

/* ADF4382 REG0034 Map */
#define ADF4382_PHASE_ADJ_MSK			NO_OS_BIT(7)
#define ADF4382_DRCLK_DEL_MSK			NO_OS_GENMASK(6, 4)
#define ADF4382_DNCLK_DEL_MSK			NO_OS_GENMASK(3, 1)
#define ADF4382_RST_CNTR_MSK			NO_OS_BIT(0)

/* ADF4382 REG0035 Map */
#define ADF4382_SPARE_35_MSK			NO_OS_GENMASK(7, 6)
#define ADF4382_M_VCO_BIAS_MSK			NO_OS_GENMASK(5, 0)

/* ADF4382 REG0036 Map */
#define ADF4382_CLKODIV_DB_MSK			NO_OS_BIT(7)
#define ADF4382_DCLK_DIV_DB_MSK			NO_OS_BIT(6)
#define ADF4382_SPARE_36_MSK			NO_OS_GENMASK(5, 2)
#define ADF4382_EN_LUT_GEN_MSK			NO_OS_BIT(1)
#define ADF4382_EN_LUT_CAL_MSK			NO_OS_BIT(0)

/* ADF4382 REG0037 Map */
#define ADF4382_CAL_COUNT_TO_MSK		NO_OS_GENMASK(7, 0)

/* ADF4382 REG0038 Map */
#define ADF4382_CAL_VTUNE_TO_LSB_MSK		NO_OS_GENMASK(7, 0)

/* ADF4382 REG0039 Map */
#define ADF4382_O_VCO_DB_MSK			NO_OS_BIT(7)
#define ADF4382_CAL_VTUNE_TO_MSB_MSK		NO_OS_GENMASK(6, 0)

/* ADF4382 REG003A Map */
#define ADF4382_CAL_VCO_TO_LSB_MSK		NO_OS_GENMASK(7, 0)

/* ADF4382 REG003B Map */
#define ADF4382_DEL_CTRL_DB_MSK			NO_OS_BIT(7)
#define ADF4382_CAL_VCO_TO_MSB_MSK		NO_OS_GENMASK(6, 0)

/* ADF4382 REG003C Map */
#define ADF4382_CNTR_DIV_WORD_MSK		NO_OS_GENMASK(7, 0)

/* ADF4382 REG003D Map */
#define ADF4382_SPARE_3D_MSK			NO_OS_BIT(7)
#define ADF4382_SYNC_SP_DB_MSK			NO_OS_BIT(6)
#define ADF4382_CMOS_OV_MSK			NO_OS_BIT(5)
#define ADF4382_READ_MODE_MSK			NO_OS_BIT(4)
#define ADF4382_CNTR_DIV_WORD_MSB_MSK		NO_OS_GENMASK(3, 0)

/* ADF4382 REG003E Map */
#define ADF4382_ADC_CLK_DIV_MSK			NO_OS_GENMASK(7, 0)

/* ADF4382 REG003F Map */
#define ADF4382_EN_ADC_CNV_MSK			NO_OS_BIT(7)
#define ADF4382_EN_ADC_VTEST_MSK		NO_OS_BIT(6)
#define ADF4382_ADC_VTEST_SEL_MSK		NO_OS_BIT(5)
#define ADF4382_ADC_MUX_SEL_MSK			NO_OS_BIT(4)
#define ADF4382_ADC_F_CONV_MSK			NO_OS_BIT(3)
#define ADF4382_ADC_C_CONV_MSK			NO_OS_BIT(2)
#define ADF4382_EN_ADC_MSK			NO_OS_BIT(1)
#define ADF4382_SPARE_3F_MSK			NO_OS_BIT(0)

/* ADF4382 REG0040 Map */
#define ADF4382_EXT_DIV_DEC_SEL_MSK		NO_OS_BIT(7)
#define ADF4382_ADC_CLK_TEST_SEL_MSK		NO_OS_BIT(6)
#define ADF4382_MUTE_CLKOUT2_MSK		NO_OS_GENMASK(5, 3)
#define ADF4382_MUTE_CLKOUT1_MSK		NO_OS_GENMASK(2, 0)

/* ADF4382 REG0041 Map */
#define ADF4382_EXT_DIV_MSK			NO_OS_GENMASK(7, 5)
#define ADF4382_EN_VCO_CAP_TEST_MSK		NO_OS_BIT(4)
#define ADF4382_EN_CALGEN_CAP_TEST_MSK		NO_OS_BIT(3)
#define ADF4382_EN_CP_CAP_TEST_MSK		NO_OS_BIT(2)
#define ADF4382_CAP_TEST_STATE_MSK		NO_OS_BIT(1)
#define ADF4382_TRANS_LOOP_SEL_MSK		NO_OS_BIT(0)

/* ADF4382 REG0042 Map */
#define ADF4382_NDIV_PWRUP_TIMEOUT_MSK		NO_OS_GENMASK(7, 0)

/* ADF4382 REG0043 Map */
#define ADF4382_CAL_BLEED_FINE_MAX_MSK		NO_OS_GENMASK(7, 0)

/* ADF4382 REG0044 Map */
#define ADF4382_VCAL_ZERO_MSK			NO_OS_BIT(7)
#define ADF4382_VPTAT_CALGEN_MSK		NO_OS_GENMASK(6, 0)

/* ADF4382 REG0045 Map */
#define ADF4382_SPARE_45_MSK			NO_OS_BIT(7)
#define ADF4382_VCTAT_CALGEN_MSK		NO_OS_GENMASK(6, 0)

/* ADF4382 REG0046 Map */
#define ADF4382_NVMDIN_MSK			NO_OS_GENMASK(7, 0)

/* ADF4382 REG0047 Map */
#define ADF4382_SPARE_47_MSK			NO_OS_BIT(7)
#define ADF4382_NVMADDR_MSK			NO_OS_GENMASK(6, 3)
#define ADF4382_NVMNO_OS_BIT_SEL		NO_OS_GENMASK(2, 0)

/* ADF4382 REG0048 Map */
#define ADF4382_TRIM_LATCH_MSK			NO_OS_BIT(7)
#define ADF4382_NVMTEST_MSK			NO_OS_BIT(6)
#define ADF4382_NVMPROG_MSK			NO_OS_BIT(5)
#define ADF4382_NVMRD_MSK			NO_OS_BIT(4)
#define ADF4382_NVMSTART_MSK			NO_OS_BIT(3)
#define ADF4382_NVMON_MSK			NO_OS_BIT(2)
#define ADF4382_MARGIN_MSK			NO_OS_GENMASK(1, 0)

/* ADF4382 REG0049 Map */
#define ADF4382_NVMDOUT_MSK			NO_OS_GENMASK(7, 0)

/* ADF4382 REG004A Map */
#define ADF4382_SCAN_MODE_CODE_MSK		NO_OS_GENMASK(7, 0)

/* ADF4382 REG004B Map */
#define ADF4382_TEMP_OFFSET_MSK			NO_OS_GENMASK(7, 0)

/* ADF4382 REG004C Map */
#define ADF4382_SPARE_4C_MSK			NO_OS_GENMASK(7, 6)
#define ADF4382_TEMP_SLOPE_MSK			NO_OS_GENMASK(5, 0)

/* ADF4382 REG004D Map */
#define ADF4382_VCO_FSM_TEST_MUX_MSK		NO_OS_GENMASK(7, 5)
#define ADF4382_SPARE_4D_MSK			NO_OS_GENMASK(4, 3)
#define ADF4382_O_VCO_BIAS_MSK			NO_OS_BIT(2)
#define ADF4382_O_VCO_BAND_MSK			NO_OS_BIT(1)
#define ADF4382_O_VCO_CORE_MSK			NO_OS_BIT(0)

/* ADF4382 REG004E Map */
#define ADF4382_SPARE_4E_MSK			NO_OS_GENMASK(7, 5)
#define ADF4382_EN_TWO_PASS_CAL_MSK		NO_OS_BIT(4)
#define ADF4382_TWO_PASS_BAND_START_MSK		NO_OS_GENMASK(3, 0)

/* ADF4382 REG004F Map */
#define ADF4382_LUT_SCALE_MSK			NO_OS_GENMASK(7, 0)

/* ADF4382 REG0050 Map */
#define ADF4382_SPARE0_MSK			NO_OS_GENMASK(7, 0)

/* ADF4382 REG0051 Map */
#define ADF4382_SPARE1_MSK			NO_OS_GENMASK(7, 0)

/* ADF4382 REG0052 Map */
#define ADF4382_SYNC_REF_SPARE_MSK		NO_OS_GENMASK(7, 4)
#define ADF4382_SYNC_MON_DEL_MSK		NO_OS_GENMASK(3, 0)

/* ADF4382 REG0053 Map */
#define ADF4382_SPARE_53_MSK			NO_OS_BIT(7)
#define ADF4382_PD_SYNC_MON_MSK			NO_OS_BIT(6)
#define ADF4382_SYNC_SEL_MSK			NO_OS_BIT(5)
#define ADF4382_RST_SYNC_MON_MSK		NO_OS_BIT(4)
#define ADF4382_SYNC_SH_DEL_MSK			NO_OS_GENMASK(3, 0)

/* ADF4382 REG0054 Map */
#define ADF4382_ADC_ST_CNV_MSK			NO_OS_BIT(0)

/* ADF4382 REG0058 Map */
#define ADF4382_LOCKED_MSK			NO_OS_BIT(0)

#define ADF4382_SPI_3W_CFG(x)			(no_os_field_prep(ADF4382_SDO_ACTIVE_MSK, x) | \
						 no_os_field_prep(ADF4382_SDO_ACTIVE_R_MSK, x))
#define ADF4382_BLEED_MSB_MSK			(ADF4382_COARSE_BLEED_MSK | \
						 ADF4382_FINE_BLEED_MSB_MSK)

#define ADF4382_SPI_SCRATCHPAD_TEST		0x5A

/* Specifications */
#define ADF4382_SPI_WRITE_CMD			0x0
#define ADF4382_SPI_READ_CMD			0x8000
#define ADF4382_SPI_DUMMY_DATA			0x00
#define ADF4382_BUFF_SIZE_BYTES			3
#define ADF4382_VCO_FREQ_MIN			11000000000U	// 11GHz
#define ADF4382_VCO_FREQ_MAX			22000000000U	// 22GHz
#define ADF4382A_VCO_FREQ_MIN			11500000000U	// 11.5GHz
#define ADF4382A_VCO_FREQ_MAX			21000000000U	// 21GHz
#define ADF4382_MOD1WORD			0x2000000U	// 2^25
#define ADF4382_MOD2WORD_MAX			0xFFFFFFU	// 2^24 - 1
#define ADF4382_PHASE_RESYNC_MOD2WORD_MAX	0x1FFFFU	// 2^17 - 1
#define ADF4382_CHANNEL_SPACING_MAX		78125U
#define ADF4382_PFD_FREQ_MAX			625000000U	// 625MHz
#define ADF4382_PFD_FREQ_FRAC_MAX		250000000U	// 250MHz
#define ADF4382_PFD_FREQ_MIN			5400000U	// 5.4MHz
#define ADF4382_DCLK_DIV1_0_MAX			160000000U	// 160MHz
#define ADF4382_DCLK_DIV1_1_MAX			320000000U	// 320MHz
#define ADF4382_CLKOUT_DIV_REG_VAL_MAX		4
#define ADF4382A_CLKOUT_DIV_REG_VAL_MAX		2

#define ADF4382_RFOUT_MAX			22000000000U
#define ADF4382_RFOUT_MIN			687500000U
#define ADF4382A_RFOUT_MAX			21000000000U
#define ADF4382A_RFOUT_MIN			2875000000U
#define ADF4382_REF_CLK_MAX			5000000000U
#define ADF4382_REF_CLK_MIN			10000000
#define ADF4382_REF_DIV_MAX			63
#define ADF4382_OUT_PWR_MAX			15
#define ADF4382_CPI_VAL_MAX			15
#define ADF4382_BLEED_WORD_MAX			8191

#define ADF4382_PHASE_BLEED_CNST		2044000
#define ADF4382_VCO_CAL_CNT			202
#define ADF4382_VCO_CAL_VTUNE			124
#define ADF4382_VCO_CAL_ALC			250
#define ADF4382_POR_DELAY_US			200
#define ADF4382_LKD_DELAY_US			500

#define MHZ					MEGA
#define S_TO_NS					NANO
#define NS_TO_PS				KHZ_PER_MHZ

/**
 * @brief Supported device ids.
*/
enum adf4382_dev_id {
	ID_ADF4382,
	ID_ADF4382A,
};

/**
 * @struct adf4382_init_param
 * @brief ADF4382 Initialization Parameters structure.
 */
struct adf4382_init_param {
	/** SPI Initialization parameters */
	struct no_os_spi_init_param	*spi_init;
	bool				spi_3wire_en;
	bool				cmos_3v3;
	uint64_t			ref_freq_hz;
	uint64_t			freq;
	bool				ref_doubler_en;
	uint8_t				ref_div;
	uint8_t				cp_i;
	uint16_t			bleed_word;
	uint8_t				ld_count;
	enum adf4382_dev_id		id;
};

/**
 * @struct adf4382_dev
 * @brief ADF4382 Device Descriptor.
 */
struct adf4382_dev {
	/** SPI Descriptor */
	struct no_os_spi_desc		*spi_desc;
	bool				spi_3wire_en;
	bool				cmos_3v3;
	uint64_t			ref_freq_hz;
	uint64_t			freq;
	bool				ref_doubler_en;
	uint8_t				ref_div;
	uint8_t				cp_i;
	uint16_t			bleed_word;
	uint8_t				ld_count;
	uint32_t			phase_adj;
	uint64_t			vco_max;
	uint64_t			vco_min;
	uint64_t			freq_max;
	uint64_t			freq_min;
	uint8_t				clkout_div_reg_val_max;
};

/**
 * @struct reg_sequence
 * @brief ADF4382 register format structure for default values
 */
struct reg_sequence {
	uint16_t reg;
	uint8_t val;
};

/**
 * @struct adf4382_reg_defaults
 * @brief ADF4382 register initialization
 */
static const struct reg_sequence adf4382_reg_defaults[] = {
	{ 0x000, 0x18 },
	{ 0x00a, 0xA5 },
	{ 0x200, 0x00 },
	{ 0x201, 0x00 },
	{ 0x202, 0x00 },
	{ 0x203, 0x00 },
	{ 0x203, 0x00 },
	{ 0x203, 0x00 },
	{ 0x100, 0x25 },
	{ 0x101, 0x3F },
	{ 0x102, 0x3F },
	{ 0x103, 0x3F },
	{ 0x104, 0x3F },
	{ 0x105, 0x3F },
	{ 0x106, 0x3F },
	{ 0x107, 0x3F },
	{ 0x108, 0x3F },
	{ 0x109, 0x25 },
	{ 0x10A, 0x25 },
	{ 0x10B, 0x3F },
	{ 0x10C, 0x3F },
	{ 0x10D, 0x3F },
	{ 0x10E, 0x3F },
	{ 0x10F, 0x3F },
	{ 0x110, 0x3F },
	{ 0x111, 0x3F },
	{ 0x054, 0x00 },
	{ 0x053, 0x45 },
	{ 0x052, 0x00 },
	{ 0x051, 0x00 },
	{ 0x050, 0x00 },
	{ 0x04f, 0x08 },
	{ 0x04e, 0x06 },
	{ 0x04d, 0x00 },
	{ 0x04c, 0x2B },
	{ 0x04b, 0x5D },
	{ 0x04a, 0x00 },
	{ 0x048, 0x00 },
	{ 0x047, 0x00 },
	{ 0x046, 0x00 },
	{ 0x045, 0x62 },
	{ 0x044, 0x3F },
	{ 0x043, 0xB8 },
	{ 0x042, 0x01 },
	{ 0x041, 0x00 },
	{ 0x040, 0x00 },
	{ 0x03f, 0x82 },
	{ 0x03e, 0x4E },
	{ 0x03c, 0x00 },
	{ 0x03b, 0x00 },
	{ 0x03a, 0xFA },
	{ 0x039, 0x00 },
	{ 0x038, 0x71 },
	{ 0x037, 0x82 },
	{ 0x036, 0xC0 },
	{ 0x035, 0x00 },
	{ 0x034, 0x36 },
	{ 0x033, 0x00 },
	{ 0x032, 0x40 },
	{ 0x031, 0x63 },
	{ 0x030, 0x0F },
	{ 0x02f, 0x3F },
	{ 0x02e, 0x00 },
	{ 0x02d, 0xF1 },
	{ 0x02c, 0x0E },
	{ 0x02b, 0x01 },
	{ 0x02a, 0x30 },
	{ 0x029, 0x09 },
	{ 0x028, 0x00 },
	{ 0x027, 0xF0 },
	{ 0x026, 0x00 },
	{ 0x025, 0x01 },
	{ 0x024, 0x01 },
	{ 0x023, 0x00 },
	{ 0x022, 0x00 },
	{ 0x021, 0x00 },
	{ 0x020, 0xC1 },
	{ 0x01f, 0x0F },
	{ 0x01e, 0x20 },
	{ 0x01d, 0x00 },
	{ 0x01c, 0x00 },
	{ 0x01b, 0x00 },
	{ 0x01a, 0x00 },
	{ 0x019, 0x00 },
	{ 0x018, 0x00 },
	{ 0x017, 0x00 },
	{ 0x016, 0x00 },
	{ 0x015, 0x06 },
	{ 0x014, 0x00 },
	{ 0x013, 0x00 },
	{ 0x012, 0x00 },
	{ 0x011, 0x00 },
	{ 0x010, 0x50 },
};

/** ADF4382 SPI write */
int adf4382_spi_write(struct adf4382_dev *dev, uint16_t reg_addr, uint8_t data);

/** ADF4382 SPI Read */
int adf4382_spi_read(struct adf4382_dev *dev, uint16_t reg_addr, uint8_t *data);

/** ADF4382 updates a bit in the register space over SPI */
int adf4382_spi_update_bits(struct adf4382_dev *dev, uint16_t reg_addr,
			    uint8_t mask, uint8_t data);

/** ADF4382 Register dump */
int adf4382_reg_dump(struct adf4382_dev *dev);

/** ADF4382 Set reference frequency attribute */
int adf4382_set_ref_clk(struct adf4382_dev *dev, uint64_t val);

/** ADF4382 Get reference frequency attribute */
int adf4382_get_ref_clk(struct adf4382_dev *dev, uint64_t *val);

/** ADF4382 Set reference doubler attribute */
int adf4382_set_en_ref_doubler(struct adf4382_dev *dev, bool en);

/** ADF4382 Get reference doubler attribute */
int adf4382_get_en_ref_doubler(struct adf4382_dev *dev, bool *en);

/** ADF4382 Set reference divider attribute */
int adf4382_set_ref_div(struct adf4382_dev *dev, int32_t div);

/** ADF4382 Get reference divider attribute */
int adf4382_get_ref_div(struct adf4382_dev *dev, int32_t *div);

/** ADF4382 Set charge pump current attribute */
int adf4382_set_cp_i(struct adf4382_dev *dev, int32_t reg_val);

/** ADF4382 Get charge pump current attribute */
int adf4382_get_cp_i(struct adf4382_dev *dev, int32_t *reg_val);

/** ADF4382 Set bleed current attribute */
int adf4382_set_bleed_word(struct adf4382_dev *dev, int32_t word);

/** ADF4382 Get bleed current attribute */
int adf4382_get_bleed_word(struct adf4382_dev *dev, int32_t *word);

/** ADF4382 Set output frequency attribute */
int adf4382_set_rfout(struct adf4382_dev *dev, uint64_t val);

/** ADF4382 Get output frequency attribute */
int adf4382_get_rfout(struct adf4382_dev *dev, uint64_t *val);

/** ADF4382 Set output power attributes */
int adf4382_set_out_power(struct adf4382_dev *dev, uint8_t ch, int32_t pwr);

/** ADF4382 Get output power attributes */
int adf4382_get_out_power(struct adf4382_dev *dev, uint8_t ch, int32_t *pwr);

/** ADF4382 Set channel enable attributes */
int adf4382_set_en_chan(struct adf4382_dev *dev, uint8_t ch, bool en);

/** ADF4382 Get channel enable attributes */
int adf4382_get_en_chan(struct adf4382_dev *dev, uint8_t ch, bool *en);

/** ADF4382 Set sync enable attributes */
int adf4382_set_en_sync(struct adf4382_dev *dev, bool en);

/** ADF4382 Get sync enable attributes */
int adf4382_get_en_sync(struct adf4382_dev *dev, bool *en);

/** ADF4382 Sets frequency */
int adf4382_set_freq(struct adf4382_dev *dev);

/** ADF4382 Sets Phase adjustment */
int adf4382_set_phase_adjust(struct adf4382_dev *dev, uint32_t phase_ps);

/** ADF4382 Sets Phase adjustment polarity*/
int adf4382_set_phase_pol(struct adf4382_dev *dev, bool polarity);

/** ADF4382 Gets Phase adjustment polarity*/
int adf4382_get_phase_pol(struct adf4382_dev *dev, bool *polarity);

/** ADF4382 Initialization */
int adf4382_init(struct adf4382_dev **device,
		 struct adf4382_init_param *init_param);

/** ADF4382 Remove */
int adf4382_remove(struct adf4382_dev *dev);
