/***************************************************************************//**
 *   @file   adf4377.h
 *   @brief  Header file for adf4377 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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

#ifndef ADF4377_H_
#define ADF4377_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_util.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/* Registers Control Bits */
#define ADF4377_REG(x)					(x)

/* ADF4377 REG0000 Map */
#define ADF4377_SOFT_RESET_R_MSK        NO_OS_BIT(7)
#define ADF4377_SOFT_RESET_R(x)			no_os_field_prep(ADF4377_SOFT_RESET_R_MSK, x)
#define ADF4377_SOFT_RESET_MSK         	NO_OS_BIT(0)
#define ADF4377_SOFT_RESET(x)			no_os_field_prep(ADF4377_SOFT_RESET_MSK, x)
#define ADF4377_LSB_FIRST_R_MSK			NO_OS_BIT(6)
#define ADF4377_LSB_FIRST_R(x)       	no_os_field_prep(ADF4377_LSB_FIRST_R_MSK, x)
#define ADF4377_LSB_FIRST_MSK			NO_OS_BIT(1)
#define ADF4377_LSB_FIRST(x)       		no_os_field_prep(ADF4377_LSB_FIRST_MSK, x)
#define ADF4377_ADDRESS_ASC_R_MSK		NO_OS_BIT(5)
#define ADF4377_ADDRESS_ASC_R(x)		no_os_field_prep(ADF4377_ADDRESS_ASC_R_MSK, x)
#define ADF4377_ADDRESS_ASC_MSK			NO_OS_BIT(2)
#define ADF4377_ADDRESS_ASC(x)			no_os_field_prep(ADF4377_ADDRESS_ASC_MSK, x)
#define ADF4377_SDO_ACTIVE_R_MSK       	NO_OS_BIT(4)
#define ADF4377_SDO_ACTIVE_R(x)			no_os_field_prep(ADF4377_SDO_ACTIVE_R_MSK, x)
#define ADF4377_SDO_ACTIVE_MSK         	NO_OS_BIT(3)
#define ADF4377_SDO_ACTIVE(x)			no_os_field_prep(ADF4377_SDO_ACTIVE_MSK, x)

/* ADF4377 REG0000 Bit Definition */
#define ADF4377_SDO_ACTIVE_SPI_3W       0x0
#define ADF4377_SDO_ACTIVE_SPI_4W		0x1

#define ADF4377_ADDR_ASC_AUTO_DECR      0x0
#define ADF4377_ADDR_ASC_AUTO_INCR      0x1

#define ADF4377_LSB_FIRST_MSB           0x0
#define ADF4377_LSB_FIRST_LSB           0x1

#define ADF4377_SOFT_RESET_N_OP         0x0
#define ADF4377_SOFT_RESET_EN           0x1

/* ADF4377 REG0001 Map */
#define ADF4377_SINGLE_INSTR_MSK		NO_OS_BIT(7)
#define ADF4377_SINGLE_INSTR(x)   		no_os_field_prep(ADF4377_SINGLE_INSTRUCTION_MSK, x)
#define ADF4377_MASTER_RB_CTRL_MSK		NO_OS_BIT(5)
#define ADF4377_MASTER_RB_CTRL(x)		no_os_field_prep(ADF4377_MASTER_RB_CTRL_MSK, x)

/* ADF4377 REG0001 Bit Definition */
#define ADF4377_SPI_STREAM_EN           0x0
#define ADF4377_SPI_STREAM_DIS          0x1

#define ADF4377_RB_SLAVE_REG            0x0
#define ADF4377_RB_MASTER_REG           0x1

/* ADF4377 REG0003 Bit Definition */
#define ADF4377_CHIP_TYPE               0x06

/* ADF4377 REG0004 Bit Definition */
#define ADF4377_PRODUCT_ID_LSB          0x0005

/* ADF4377 REG0005 Bit Definition */
#define ADF4377_PRODUCT_ID_MSB          0x0005

/* ADF4377 REG000A Map */
#define ADF4377_SCRATCHPAD_MSK			NO_OS_GENMASK(7, 0)
#define ADF4377_SCRATCHPAD(x)           no_os_field_prep(ADF4377_SCRATCHPAD_MSK, x)

/* ADF4377 REG000B Bit Definition */
#define ADF4377_SPI_REVISION            0x01

/* ADF4377 REG000C Bit Definition */
#define ADF4377_VENDOR_ID_LSB           0x456

/* ADF4377 REG000D Bit Definition */
#define ADF4377_VENDOR_ID_MSB			0x456

/* ADF4377 REG000F Bit Definition */
#define ADF4377_R00F_RSV1				0x14

/* ADF4377 REG0010 Map*/
#define ADF4377_N_INT_LSB_MSK			NO_OS_GENMASK(7, 0)
#define ADF4377_N_INT_LSB(x)            no_os_field_prep(ADF4377_N_INT_LSB_MSK, x)

/* ADF4377 REG0011 Map*/
#define ADF4377_EN_AUTOCAL_MSK			NO_OS_BIT(7)
#define ADF4377_EN_AUTOCAL(x)           no_os_field_prep(ADF4377_EN_AUTOCAL_MSK, x)
#define ADF4377_EN_RDBLR_MSK			NO_OS_BIT(6)
#define ADF4377_EN_RDBLR(x)             no_os_field_prep(ADF4377_EN_RDBLR_MSK, x)
#define ADF4377_DCLK_DIV2_MSK			NO_OS_GENMASK(5,4)
#define ADF4377_DCLK_DIV2(x)            no_os_field_prep(ADF4377_DCLK_DIV2_MSK, x)
#define ADF4377_N_INT_MSB_MSK			NO_OS_GENMASK(3,0)
#define ADF4377_N_INT_MSB(x)            no_os_field_prep(ADF4377_N_INT_MSB_MSK, x)

/* ADF4377 REG0011 Bit Definition */
#define ADF4377_VCO_CALIB_DIS           0x0
#define ADF4377_VCO_CALIB_EN            0x1

#define ADF4377_REF_DBLR_DIS            0x0
#define ADF4377_REF_DBLR_EN             0x1

#define ADF4377_DCLK_DIV2_1             0x0
#define ADF4377_DCLK_DIV2_2             0x1
#define ADF4377_DCLK_DIV2_4             0x2
#define ADF4377_DCLK_DIV2_8             0x3

/* ADF4377 REG0012 Map*/
#define ADF4377_CLKOUT_DIV_MSK			NO_OS_GENMASK(7, 6)
#define ADF4377_CLKOUT_DIV(x)           no_os_field_prep(ADF4377_CLKOUT_DIV_MSK, x)
#define ADF4377_R_DIV_MSK				NO_OS_GENMASK(5, 0)
#define ADF4377_R_DIV(x)                no_os_field_prep(ADF4377_R_DIV_MSK, x)

/* ADF4377 REG0012 Bit Definition */
#define ADF4377_CLKOUT_DIV_1            0x0
#define ADF4377_CLKOUT_DIV_2            0x1
#define ADF4377_CLKOUT_DIV_4            0x2
#define ADF4377_CLKOUT_DIV_8            0x3

#define ADF4377_MIN_R_DIV               0x00
#define ADF4378_MAX_R_DIV               0x3F

/* ADF4377 REG0013 Map */
#define ADF4377_M_VCO_CORE_MSK			NO_OS_GENMASK(5,4)
#define ADF4377_M_VCO_CORE(x)           no_os_field_prep(ADF4377_M_VCO_CORE_MSK, x)
#define ADF4377_M_VCO_BIAS_MSK          NO_OS_GENMASK(3,0)
#define ADF4377_M_VCO_BIAS(x)          	no_os_field_prep(ADF4377_M_VCO_BIAS_MSK, x)

/* ADF4377 REG0013 Bit Definition */
#define ADF4377_M_VCO_0                 0x0
#define ADF4377_M_VCO_1                 0x1
#define ADF4377_M_VCO_2                 0x2
#define ADF4377_M_VCO_3                 0x3

#define M_VCO_BIAS_MIN                  0xF
#define M_VCO_BIAS_MAX                  0x0

/* ADF4377 REG0014 Map */
#define ADF4377_M_VCO_BAND_MSK          NO_OS_GENMASK(7,0)
#define ADF4377_M_VCO_BAND(x)           no_os_field_prep(ADF4377_M_VCO_BAND_MSK, x)

/* ADF4377 REG0014 Bit Definition */
#define ADF4377_VCO_BAND_MIN            0xFF
#define ADF4377_VCO_BAND_MAX            0x00

/* ADF4377 REG0015 Map */
#define ADF4377_BLEED_I_LSB_MSK			NO_OS_GENMASK(7, 6)
#define ADF4377_BLEED_I_LSB(x)          no_os_field_prep(ADF4377_BLEED_I_LSB_MSK, x)
#define ADF4377_BLEED_POL_MSK			NO_OS_BIT(5)
#define ADF4377_BLEED_POL(x)            no_os_field_prep(ADF4377_BLEED_POL_MSK, x)
#define ADF4377_EN_BLEED_MSK			NO_OS_BIT(4)
#define ADF4377_EN_BLEED(x)             no_os_field_prep(ADF4377_EN_BLEED_MSK, x)
#define ADF4377_CP_I_MSK				NO_OS_GENMASK(3, 0)
#define ADF4377_CP_I(x)                 no_os_field_prep(ADF4377_CP_I_MSK, x)

/* ADF4377 REG0015 Bit Description */
#define ADF4377_CURRENT_SINK            0x0
#define ADF4377_CURRENT_SOURCE          0x1

#define ADF4377_CP_0MA7                 0x0
#define ADF4377_CP_0MA9                 0x1
#define ADF4377_CP_1MA1                 0x2
#define ADF4377_CP_1MA3                 0x3
#define ADF4377_CP_1MA4                 0x4
#define ADF4377_CP_1MA8                 0x5
#define ADF4377_CP_2MA2                 0x6
#define ADF4377_CP_2MA5                 0x7
#define ADF4377_CP_2MA9                 0x8
#define ADF4377_CP_3MA6                 0x9
#define ADF4377_CP_4MA3                 0xA
#define ADF4377_CP_5MA0                 0xB
#define ADF4377_CP_5MA7                 0xC
#define ADF4377_CP_7MA2                 0xD
#define ADF4377_CP_8MA6                 0xE
#define ADF4377_CP_10MA1                0xF

/* ADF4377 REG0016 Map */
#define ADF4377_BLEED_I_MSB_MSK			NO_OS_GENMASK(7, 0)
#define ADF4377_BLEED_I_MSB(x)          no_os_field_prep(ADF4377_BLEED_I_MSB_MSK, x)

/* ADF4377 REG0017 Map */
#define ADF4377_INV_CLKOUT_MSK			NO_OS_BIT(7)
#define ADF4377_INV_CLKOUT(x)           no_os_field_prep(ADF4377_INV_CLKOUT_MSK, x)
#define ADF4377_N_DEL_MSK				NO_OS_GENMASK(6, 0)
#define ADF4377_N_DEL(x)                no_os_field_prep(ADF4377_N_DEL_MSK, x)

/* ADF4377 REG0018 Map */
#define ADF4377_CMOS_OV_MSK				NO_OS_BIT(7)
#define ADF4377_CMOS_OV(x)              no_os_field_prep(ADF4377_CMOS_OV_MSK, x)
#define ADF4377_R_DEL_MSK				NO_OS_GENMASK(6, 0)
#define ADF4377_R_DEL(x)                no_os_field_prep(ADF4377_R_DEL_MSK, x)

/* ADF4377 REG0018 Bit Definition */
#define ADF4377_1V8_LOGIC               0x0
#define ADF4377_3V3_LOGIC               0x1

#define ADF4377_R_DEL_MIN               0x00
#define ADF4377_R_DEL_MAX               0x7F

/* ADF4377 REG0019 Map */
#define ADF4377_CLKOUT2_OP_MSK			NO_OS_GENMASK(7, 6)
#define ADF4377_CLKOUT2_OP(x)           no_os_field_prep(ADF4377_CLKOUT2_OP_MSK, x)
#define ADF4377_CLKOUT1_OP_MSK			NO_OS_GENMASK(5, 4)
#define ADF4377_CLKOUT1_OP(x)           no_os_field_prep(ADF4377_CLKOUT1_OP_MSK, x)
#define ADF4377_PD_CLK_MSK				NO_OS_BIT(3)
#define ADF4377_PD_CLK(x)               no_os_field_prep(ADF4377_PD_CLK_MSK, x)
#define ADF4377_PD_RDET_MSK				NO_OS_BIT(2)
#define ADF4377_PD_RDET(x)              no_os_field_prep(ADF4377_PD_RDET_MSK, x)
#define ADF4377_PD_ADC_MSK				NO_OS_BIT(1)
#define ADF4377_PD_ADC(x)               no_os_field_prep(ADF4377_PD_ADC_MSK, x)
#define ADF4377_PD_CALADC_MSK			NO_OS_BIT(0)
#define ADF4377_PD_CALADC(x)            no_os_field_prep(ADF4377_PD_CALADC_MSK, x)

/* ADF4377 REG0019 Bit Definition */
#define ADF4377_CLKOUT_320MV            0x0
#define ADF4377_CLKOUT_420MV            0x1
#define ADF4377_CLKOUT_530MV            0x2
#define ADF4377_CLKOUT_640MV            0x3

#define ADF4377_PD_CLK_N_OP             0x0
#define ADF4377_PD_CLK_PD               0x1

#define ADF4377_PD_RDET_N_OP            0x0
#define ADF4377_PD_RDET_PD              0x1

#define ADF4377_PD_ADC_N_OP             0x0
#define ADF4377_PD_ADC_PD               0x1

#define ADF4377_PD_CALADC_N_OP          0x0
#define ADF4377_PD_CALADC_PD            0x1

/* ADF4377 REG001A Map */
#define ADF4377_PD_ALL_MSK				NO_OS_BIT(7)
#define ADF4377_PD_ALL(x)               no_os_field_prep(ADF4377_PD_ALL_MSK, x)
#define ADF4377_PD_RDIV_MSK				NO_OS_BIT(6)
#define ADF4377_PD_RDIV(x)              no_os_field_prep(ADF4377_PD_RDIV_MSK, x)
#define ADF4377_PD_NDIV_MSK				NO_OS_BIT(5)
#define ADF4377_PD_NDIV(x)              no_os_field_prep(ADF4377_PD_NDIV_MSK, x)
#define ADF4377_PD_VCO_MSK				NO_OS_BIT(4)
#define ADF4377_PD_VCO(x)               no_os_field_prep(ADF4377_PD_VCO_MSK, x)
#define ADF4377_PD_LD_MSK				NO_OS_BIT(3)
#define ADF4377_PD_LD(x)                no_os_field_prep(ADF4377_PD_LD_MSK, x)
#define ADF4377_PD_PFDCP_MSK			NO_OS_BIT(2)
#define ADF4377_PD_PFDCP(x)             no_os_field_prep(ADF4377_PD_PFDCP_MSK, x)
#define ADF4377_PD_CLKOUT1_MSK			NO_OS_BIT(1)
#define ADF4377_PD_CLKOUT1(x)           no_os_field_prep(ADF4377_PD_CLKOUT1_MSK, x)
#define ADF4377_PD_CLKOUT2_MSK			NO_OS_BIT(0)
#define ADF4377_PD_CLKOUT2(x)           no_os_field_prep(ADF4377_PD_CLKOUT2_MSK, x)

/* ADF4377 REG001A Bit Definition */
#define ADF4377_PD_ALL_N_OP             0x0
#define ADF4377_PD_ALL_PD               0x1

#define ADF4377_PD_RDIV_N_OP            0x0
#define ADF4377_PD_RDIV_PD              0x1

#define ADF4377_PD_NDIV_N_OP            0x0
#define ADF4377_PD_NDIV_PD              0x1

#define ADF4377_PD_VCO_N_OP             0x0
#define ADF4377_PD_VCO_PD               0x1

#define ADF4377_PD_LD_N_OP              0x0
#define ADF4377_PD_LD_PD                0x1

#define ADF4377_PD_PFDCP_N_OP           0x0
#define ADF4377_PD_PFDCP_PD             0x1

#define ADF4377_PD_CLKOUT1_N_OP         0x0
#define ADF4377_PD_CLKOUT1_PD           0x1

#define ADF4377_PD_CLKOUT2_N_OP         0x0
#define ADF4377_PD_CLKOUT2_PD           0x1

/* ADF4377 REG001B Map */
#define ADF4377_EN_LOL_MSK				NO_OS_BIT(7)
#define ADF4377_EN_LOL(x)               no_os_field_prep(ADF4377_EN_LOL_MSK, x)
#define ADF4377_LDWIN_PW_MSK			NO_OS_BIT(6)
#define ADF4377_LDWIN_PW(x)             no_os_field_prep(ADF4377_LDWIN_PW_MSK, x)
#define ADF4377_EN_LDWIN_MSK			NO_OS_BIT(5)
#define ADF4377_EN_LDWIN(x)             no_os_field_prep(ADF4377_EN_LDWIN_MSK, x)
#define ADF4377_LD_COUNT_MSK			NO_OS_GENMASK(4, 0)
#define ADF4377_LD_COUNT(x)             no_os_field_prep(ADF4377_LD_COUNT_MSK, x)

/* ADF4377 REG001B Bit Definition */

#define ADF4377_LDWIN_PW_NARROW         0x0
#define ADF4377_LDWIN_PW_WIDE           0x1

/* ADF4377 REG001C Map */
#define ADF4377_EN_DNCLK_MSK			NO_OS_BIT(7)
#define ADF4377_EN_DNCLK(x)            	no_os_field_prep(ADF4377_EN_DNCLK_MSK, x)
#define ADF4377_EN_DRCLK_MSK			NO_OS_BIT(6)
#define ADF4377_EN_DRCLK(x)             no_os_field_prep(ADF4377_EN_DRCLK_MSK, x)
#define ADF4377_RST_LD_MSK				NO_OS_BIT(2)
#define ADF4377_RST_LD(x)               no_os_field_prep(ADF4377_RST_LD_MSK, x)
#define ADF4377_R01C_RSV1_MSK			NO_OS_BIT(0)
#define ADF4377_R01C_RSV1(x)			no_os_field_prep(ADF4377_R01C_RSV1_MSK, x)

/* ADF4377 REG001C Bit Definition */
#define ADF4377_EN_DNCLK_OFF            0x0
#define ADF4377_EN_DNCLK_ON             0x1

#define ADF4377_EN_DRCLK_OFF            0x0
#define ADF4377_EN_DRCLK_ON             0x1

#define ADF4377_RST_LD_INACTIVE         0x0
#define ADF4377_RST_LD_ACTIVE           0x1

/* ADF4377 REG001D Map */
#define ADF4377_MUXOUT_MSK				NO_OS_GENMASK(7, 4)
#define ADF4377_MUXOUT(x)               no_os_field_prep(ADF4377_MUXOUT_MSK, x)
#define ADF4377_EN_CPTEST_MSK			NO_OS_BIT(2)
#define ADF4377_EN_CPTEST(x)            no_os_field_prep(ADF4377_EN_CPTEST_MSK, x)
#define ADF4377_CP_DOWN_MSK				NO_OS_BIT(1)
#define ADF4377_CP_DOWN(x)              no_os_field_prep(ADF4377_CP_DOWN_MSK, x)
#define ADF4377_CP_UP_MSK				NO_OS_BIT(0)
#define ADF4377_CP_UP(x)                no_os_field_prep(ADF4377_CP_UP_MSK, x)

/* ADF4377 REG001D Bit Definitons */
#define ADF4377_MUXOUT_HIGH_Z           0x0
#define ADF4377_MUXOUT_LKDET            0x1
#define ADF4377_MUXOUT_LOW              0x2
#define ADF4377_MUXOUT_DIV_RCLK_2       0x4
#define ADF4377_MUXOUT_DIV_NCLK_2       0x5
#define ADF4377_MUXOUT_HIGH             0x8

#define ADF4377_EN_CPTEST_OFF           0x0
#define ADF4377_EN_CPTEST_ON            0x1

#define ADF4377_CP_DOWN_OFF             0x0
#define ADF4377_CP_DOWN_ON              0x1

#define ADF4377_CP_UP_OFF               0x0
#define ADF4377_CP_UP_ON                0x1

/* ADF4377 REG001F Map */
#define ADF4377_BST_REF_MSK				NO_OS_BIT(7)
#define ADF4377_BST_REF(x)              no_os_field_prep(ADF4377_BST_REF_MSK, x)
#define ADF4377_FILT_REF_MSK			NO_OS_BIT(6)
#define ADF4377_FILT_REF(x)             no_os_field_prep(ADF4377_FILT_REF_MSK, x)
#define ADF4377_REF_SEL_MSK				NO_OS_BIT(5)
#define ADF4377_REF_SEL(x)              no_os_field_prep(ADF4377_REF_SEL_MSK, x)
#define ADF4377_R01F_RSV1_MSK           NO_OS_GENMASK(2, 0)
#define ADF4377_R01F_RSV1(x) 			no_os_field_prep(ADF4377_R01F_RSV1_MSK, x)

/* ADF4377 REG001F Bit Description */
#define ADF4377_BST_LARGE_REF_IN        0x0
#define ADF4377_BST_SMALL_REF_IN        0x1

#define ADF4377_FILT_REF_OFF            0x0
#define ADF4377_FILT_REF_ON             0x1

#define ADF4377_REF_SEL_DMA             0x0
#define ADF4377_REF_SEL_LNA             0x1

/* ADF4377 REG0020 Map */
#define ADF4377_RST_SYS_MSK				NO_OS_BIT(4)
#define ADF4377_RST_SYS(x)              no_os_field_prep(ADF4377_RST_SYS_MSK, x)
#define ADF4377_EN_ADC_CLK_MSK			NO_OS_BIT(3)
#define ADF4377_EN_ADC_CLK(x)           no_os_field_prep(ADF4377_EN_ADC_CLK_MSK, x)
#define ADF4377_R020_RSV1_MSK           NO_OS_BIT(0)
#define ADF4377_R020_RSV1(x)			no_os_field_prep(ADF4377_R020_RSV1_MSK, x)

/* ADF4377 REG0020 Bit Description */
#define ADF4377_RST_SYS_INACTIVE        0x0
#define ADF4377_RST_SYS_ACTIVE          0x1

/* ADF4377 REG0021 Map */
#define ADF4377_R021_RSV1               0xD3

/* ADF4377 REG0022 Map */
#define ADF4377_R022_RSV1               0x32

/* ADF4377 REG0023 Map */
#define ADF4377_R023_RSV1               0x18

/* ADF4377 REG0024 Map */
#define ADF4377_DCLK_MODE_MSK			NO_OS_BIT(2)
#define ADF4377_DCLK_MODE(x)            no_os_field_prep(ADF4377_DCLK_MODE_MSK, x)

/* ADF4377 REG0025 Map */
#define ADF4377_CLKODIV_DB_MSK			NO_OS_BIT(7)
#define ADF4377_CLKODIV_DB(x)           no_os_field_prep(ADF4377_CLKODIV_DB_MSK, x)
#define ADF4377_DCLK_DB_MSK				NO_OS_BIT(6)
#define ADF4377_DCLK_DB(x)              no_os_field_prep(ADF4377_DCLK_DB_MSK, x)
#define ADF4377_R025_RSV1_MSK           NO_OS_BIT(4) | NO_OS_BIT(2) | NO_OS_BIT(1)
#define ADF4377_R025_RSV1(x)			no_os_field_prep(ADF4377_R025_RSV1_MSK, x)

/* ADF4377 REG0026 Map */
#define ADF4377_VCO_BAND_DIV_MSK    	NO_OS_GENMASK(7, 0)
#define ADF4377_VCO_BAND_DIV(x)         no_os_field_prep(ADF4377_VCO_BAND_DIV_MSK, x)

/* ADF4377 REG0026 Bit Definition */
#define ADF4377_VCO_BAND_DIV_MIN        0x00
#define ADF4377_VCO_BAND_DIV_MAX        0xFF

/* ADF4377 REG0027 Map */
#define ADF4377_SYNTH_LOCK_TO_LSB_MSK   NO_OS_GENMASK(7, 0)
#define ADF4377_SYNTH_LOCK_TO_LSB(x)   	no_os_field_prep(ADF4377_SYNTH_LOCK_TO_LSB_MSK, x)

/* ADF4377 REG0028 Map */
#define ADF4377_O_VCO_DB_MSK			NO_OS_BIT(7)
#define ADF4377_O_VCO_DB(x)             no_os_field_prep(ADF4377_O_VCO_DB_MSK, x)
#define ADF4377_SYNTH_LOCK_TO_MSB_MSK	NO_OS_GENMASK(6, 0)
#define ADF4377_SYNTH_LOCK_TO_MSB(x)   	no_os_field_prep(ADF4377_SYNTH_LOCK_TO_MSB_MSK, x)

/* ADF4377 REG0029 Map */
#define ADF4377_VCO_ALC_TO_LSB_MSK		NO_OS_GENMASK(7, 0)
#define ADF4377_VCO_ALC_TO_LSB(x)     	no_os_field_prep(ADF4377_VCO_ALC_TO_LSB_MSK, x)

/* ADF4377 REG002A Map */
#define ADF4377_DEL_CTRL_DB_MSK			NO_OS_BIT(7)
#define ADF4377_DEL_CTRL_DB(x)          no_os_field_prep(ADF4377_DEL_CTRL_DB_MSK, x)
#define ADF4377_VCO_ALC_TO_MSB_MSK		NO_OS_GENMASK(6, 0)
#define ADF4377_VCO_ALC_TO_MSB(x)      	no_os_field_prep(ADF4377_VCO_ALC_TO_MSB_MSK, x)

/* ADF4377 REG002C Map */
#define ADF4377_R02C_RSV1               0xC0

/* ADF4377 REG002D Map */
#define ADF4377_ADC_CLK_DIV_MSK			NO_OS_GENMASK(7, 0)
#define ADF4377_ADC_CLK_DIV(x)          no_os_field_prep(ADF4377_ADC_CLK_DIV_MSK, x)

/* ADF4377 REG002E Map */
#define ADF4377_EN_ADC_CNV_MSK			NO_OS_BIT(7)
#define ADF4377_EN_ADC_CNV(x)           no_os_field_prep(ADF4377_EN_ADC_CNV_MSK, x)
#define ADF4377_EN_ADC_MSK				NO_OS_BIT(1)
#define ADF4377_EN_ADC(x)               no_os_field_prep(ADF4377_EN_ADC_MSK, x)
#define ADF4377_ADC_A_CONV_MSK			NO_OS_BIT(0)
#define ADF4377_ADC_A_CONV(x)           no_os_field_prep(ADF4377_ADC_A_CONV_MSK, x)

/* ADF4377 REG002E Bit Definition */
#define ADF4377_ADC_A_CONV_ADC_ST_CNV   0x0
#define ADF4377_ADC_A_CONV_VCO_CALIB    0x1

/* ADF4377 REG002F Map */
#define ADF4377_DCLK_DIV1_MSK			NO_OS_GENMASK(1, 0)
#define ADF4377_DCLK_DIV1(x)            no_os_field_prep(ADF4377_DCLK_DIV1_MSK, x)

/* ADF4377 REG002F Bit Definition */
#define ADF4377_DCLK_DIV1_1             0x0
#define ADF4377_DCLK_DIV1_2             0x1
#define ADF4377_DCLK_DIV1_8             0x2
#define ADF4377_DCLK_DIV1_32            0x3

/* ADF4377 REG0031 Map */
#define ADF4377_R031_RSV1				0x09

/* ADF4377 REG0032 Map */
#define ADF4377_ADC_CLK_SEL_MSK			NO_OS_BIT(6)
#define ADF4377_ADC_CLK_SEL(x)          no_os_field_prep(ADF4377_ADC_CLK_SEL_MSK, x)
#define ADF4377_R032_RSV1_MSK           NO_OS_BIT(3) | NO_OS_BIT(0)
#define ADF4377_R032_RSV1(x)			no_os_field_prep(ADF4377_R032_RSV1_MSK, x)

/* ADF4377 REG0032 Bit Definition */
#define ADF4377_ADC_CLK_SEL_N_OP        0x0
#define ADF4377_ADC_CLK_SEL_SPI_CLK     0x1

/* ADF4377 REG0033 Map */
#define ADF4377_R033_RSV1               0x18

/* ADF4377 REG0034 Map */
#define ADF4377_R034_RSV1               0x08

/* ADF4377 REG003A Map */
#define ADF4377_R03A_RSV1               0x5C

/* ADF4377 REG003B Map */
#define ADF4377_R03B_RSV1               0x2B

/* ADF4377 REG003D Map */
#define ADF4377_O_VCO_BAND_MSK			NO_OS_BIT(3)
#define ADF4377_O_VCO_BAND(x)           no_os_field_prep(ADF4377_O_VCO_BAND_MSK, x)
#define ADF4377_O_VCO_CORE_MSK			NO_OS_BIT(2)
#define ADF4377_O_VCO_CORE(x)           no_os_field_prep(ADF4377_O_VCO_CORE_MSK, x)
#define ADF4377_O_VCO_BIAS_MSK			NO_OS_BIT(1)
#define ADF4377_O_VCO_BIAS(x)           no_os_field_prep(ADF4377_O_VCO_BIAS_MSK, x)

/* ADF4377 REG003D Bit Definition */
#define ADF4377_O_VCO_BAND_VCO_CALIB    0x0
#define ADF4377_O_VCO_BAND_M_VCO        0x1

#define ADF4377_O_VCO_CORE_VCO_CALIB    0x0
#define ADF4377_O_VCO_CORE_M_VCO        0x1

#define ADF4377_O_VCO_BIAS_VCO_CALIB    0x0
#define ADF4377_O_VCO_BIAS_M_VCO        0x1

/* ADF4377 REG0042 Map */
#define ADF4377_R042_RSV1               0x05

/* ADF4377 REG0045 Map */
#define ADF4377_ADC_ST_CNV_MSK			NO_OS_BIT(0)
#define ADF4377_ADC_ST_CNV(x)           no_os_field_prep(ADF4377_ADC_ST_CNV_MSK, x)

/* ADF4377 REG0049 Map */
#define ADF4377_EN_CLK2_MSK				NO_OS_BIT(7)
#define ADF4377_EN_CLK2(x)              no_os_field_prep(ADF4377_EN_CLK2_MSK, x)
#define ADF4377_EN_CLK1_MSK				NO_OS_BIT(6)
#define ADF4377_EN_CLK1(x)              no_os_field_prep(ADF4377_EN_CLK1_MSK, x)
#define ADF4377_REF_OK_MSK				NO_OS_BIT(3)
#define ADF4377_REF_OK(x)               no_os_field_prep(ADF4377_REF_OK_MSK, x)
#define ADF4377_ADC_BUSY_MSK			NO_OS_BIT(2)
#define ADF4377_ADC_BUSY(x)             no_os_field_prep(ADF4377_ADC_BUSY_MSK, x)
#define ADF4377_FSM_BUSY_MSK			NO_OS_BIT(1)
#define ADF4377_FSM_BUSY(x)             no_os_field_prep(ADF4377_FSM_BUSY_MSK, x)
#define ADF4377_LOCKED_MSK				NO_OS_BIT(0)
#define ADF4377_LOCKED(x)               no_os_field_prep(ADF4377_LOCKED_MSK, x)

/* ADF4377 REG004B Map */
#define ADF4377_VCO_CORE_MSK			NO_OS_GENMASK(1, 0)
#define ADF4377_VCO_CORE(x)             no_os_field_prep(ADF4377_VCO_CORE_MSK, x)

/* ADF4377 REG004C Map */
#define ADF4377_CHIP_TEMP_LSB_MSK		NO_OS_GENMASK(7, 0)
#define ADF4377_CHIP_TEMP_LSB(x)        no_os_field_prep(ADF4377_CHIP_TEMP_LSB_MSK, x)

/* ADF4377 REG004D Map */
#define ADF4377_CHIP_TEMP_MSB_MSK		NO_OS_BIT(0)
#define ADF4377_CHIP_TEMP_MSB(x)        no_os_field_prep(ADF4377_CHIP_TEMP_MSB_MSK, x)

/* ADF4377 REG004F Map */
#define ADF4377_VCO_BAND_MSK			NO_OS_GENMASK(7, 0)
#define ADF4377_VCO_BAND(x)             no_os_field_prep(ADF4377_VCO_BAND_MSK, x)

/* ADF4377 REG0054 Map */
#define ADF4377_CHIP_VERSION_MSK		NO_OS_GENMASK(7, 0)
#define ADF4377_CHIP_VERSION(x)         no_os_field_prep(ADF4377_CHIP_VERSION_MSK, x)

/* Specifications */
#define ADF4377_SPI_WRITE_CMD		    0x0
#define ADF4377_SPI_READ_CMD		    NO_OS_BIT(7)
#define ADF4377_BUFF_SIZE_BYTES		    3
#define ADF4377_MAX_VCO_FREQ		    12800000000ull /* Hz */
#define ADF4377_MIN_VCO_FREQ		    6400000000ull /* Hz */
#define ADF4377_MAX_REFIN_FREQ		    1000000000 /* Hz */
#define ADF4377_MIN_REFIN_FREQ		    10000000 /* Hz */
#define ADF4377_MAX_FREQ_PFD		    500000000 /* Hz */
#define ADF4377_MIN_FREQ_PFD		    3000000 /* Hz */
#define ADF4377_MAX_CLKPN_FREQ		    ADF4377_MAX_VCO_FREQ /* Hz */
#define ADF4377_MIN_CLKPN_FREQ		    (ADF4377_MIN_VCO_FREQ / 8) /* Hz */
#define ADF4377_FREQ_PFD_80MHZ		    80000000
#define ADF4377_FREQ_PFD_125MHZ		    125000000
#define ADF4377_FREQ_PFD_160MHZ		    160000000
#define ADF4377_FREQ_PFD_250MHZ		    250000000
#define ADF4377_FREQ_PFD_320MHZ		    320000000

/* ADF4377 Extra Definitions */
#define ADF4377_SPI_SCRATCHPAD_TEST_A	    0xA5u
#define ADF4377_SPI_SCRATCHPAD_TEST_B	    0x5Au
#define ADF4377_SPI_DUMMY_DATA		    0x00
#define ADF4377_CHECK_RANGE(freq, range) \
	((freq > ADF4377_MAX_ ## range) || (freq < ADF4377_MIN_ ## range))

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum adf4377_dev_id
 * @brief ID of Devices supported by the driver.
 */
enum adf4377_dev_id {
	ADF4377 = 0x05,
	ADF4378 = 0x06
};

/**
 * @struct adf4377_init_param
 * @brief ADF4377 Initialization Parameters structure.
 */
struct adf4377_init_param {
	/** SPI Initialization parameters */
	struct no_os_spi_init_param	*spi_init;
	/** GPIO Chip Enable */
	struct no_os_gpio_init_param	*gpio_ce_param;
	/** GPIO ENCLK1 */
	struct no_os_gpio_init_param	*gpio_enclk1_param;
	/** GPIO ENCLK2 */
	struct no_os_gpio_init_param	*gpio_enclk2_param;
	/** Device ID */
	enum adf4377_dev_id dev_id;
	/** SPI 3-Wire */
	bool spi4wire;
	/** Input Reference Clock */
	uint32_t clkin_freq;
	/** Output frequency */
	uint64_t f_clk;
	/** Charge Pump Current */
	uint8_t cp_i;
	/** MUXOUT Select */
	uint32_t muxout_select;
	/** Reference doubler enable */
	uint8_t ref_doubler_en;
	/** Output Amplitude */
	uint8_t	clkout_op;
};

/**
 * @struct adf4377_dev
 * @brief ADF4377 Device Descriptor.
 */
struct adf4377_dev {
	/** SPI Descriptor */
	struct no_os_spi_desc		*spi_desc;
	/** GPIO ENCLK1 */
	struct no_os_gpio_desc	*gpio_enclk1;
	/** GPIO ENCLK2 */
	struct no_os_gpio_desc	*gpio_enclk2;
	/** GPIO Chip Enable */
	struct no_os_gpio_desc	*gpio_ce;
	/** Device ID */
	enum adf4377_dev_id dev_id;
	/** SPI 3-Wire */
	bool spi4wire;
	/** PFD Frequency */
	uint32_t f_pfd;
	/** Output frequency */
	uint64_t f_clk;
	/** Output frequency of the VCO */
	uint64_t f_vco;
	/** Input Reference Clock */
	uint32_t clkin_freq;
	/** Charge Pump Current */
	uint8_t cp_i;
	/** MUXOUT Default */
	uint8_t muxout_default;
	/** Reference doubler enable */
	uint8_t	ref_doubler_en;
	/** Reference Divider */
	uint32_t ref_div_factor;
	/** CLKOUT Divider */
	uint8_t clkout_div_sel;
	/** Feedback Divider (N) */
	uint16_t n_int;
	/** Output Amplitude */
	uint8_t	clkout_op;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/** ADF4377 SPI write */
int32_t adf4377_spi_write(struct adf4377_dev *dev, uint8_t reg_addr,
			  uint8_t data);

/* ADF4377 Register Update */
int32_t adf4377_spi_write_mask(struct adf4377_dev *dev, uint8_t reg_addr,
			       uint8_t mask, uint8_t data);

/** ADF4377 SPI Read */
int32_t adf4377_spi_read(struct adf4377_dev *dev, uint8_t reg_addr,
			 uint8_t *data);

/* Software Reset */
int32_t adf4377_soft_reset(struct adf4377_dev *dev);

/* ADF4377 Scratchpad check */
int32_t adf4377_check_scratchpad(struct adf4377_dev *dev);

/* Set Output frequency */
int32_t adf4377_set_freq(struct adf4377_dev *dev, uint64_t freq);

/** ADF4377 Initialization */
int32_t adf4377_init(struct adf4377_dev **device,
		     struct adf4377_init_param *init_param);

/** ADF4377 Resources Deallocation */
int32_t adf4377_remove(struct adf4377_dev *dev);

#endif /* ADF4377_H_ */
