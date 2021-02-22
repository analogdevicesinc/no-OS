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
#include <stdint.h>
#include "spi.h"
#include "gpio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/* Registers Control Bits */
#define ADF4377_REG0000			0x00
#define ADF4377_REG0001			0x01
#define ADF4377_REG0003			0x03
#define ADF4377_REG0004			0x04
#define ADF4377_REG0005			0x05
#define ADF4377_REG000A			0x0A
#define ADF4377_REG000B			0x0B
#define ADF4377_REG000C			0x0C
#define ADF4377_REG000D			0x0D
#define ADF4377_REG0010			0x10
#define ADF4377_REG0011			0x11
#define ADF4377_REG0012			0x12
#define ADF4377_REG0013			0x13
#define ADF4377_REG0014			0x14
#define ADF4377_REG0015			0x15
#define ADF4377_REG0016			0x16
#define ADF4377_REG0017			0x17
#define ADF4377_REG0018			0x18
#define ADF4377_REG0019			0x19
#define ADF4377_REG001A			0x1A
#define ADF4377_REG001B			0x1B
#define ADF4377_REG001C			0x1C
#define ADF4377_REG001D 		0x1D
#define ADF4377_REG001F 		0x1F
#define ADF4377_REG0020 		0x20
#define ADF4377_REG0023 		0x23
#define ADF4377_REG0024 		0x24
#define ADF4377_REG0025 		0x25
#define ADF4377_REG0026 		0x26
#define ADF4377_REG0027 		0x27
#define ADF4377_REG0028 		0x28
#define ADF4377_REG0029 		0x29
#define ADF4377_REG002A 		0x2A
#define ADF4377_REG002D 		0x2D
#define ADF4377_REG002E 		0x2E
#define ADF4377_REG002F 		0x2F
#define ADF4377_REG0032 		0x32
#define ADF4377_REG003D 		0x3D
#define ADF4377_REG0045 		0x45
#define ADF4377_REG0049 		0x49
#define ADF4377_REG004B 		0x4B
#define ADF4377_REG004C 		0x4C
#define ADF4377_REG004D 		0x4D
#define ADF4377_REG004F 		0x4F
#define ADF4377_REG0054 		0x54

/* ADF4377 REG0000 Map */
#define ADF4377_SOFT_RESET_R(x)         (((x) & 0x1) << 7)
#define ADF4377_LSB_FIRST_R(x)          (((x) & 0x1) << 6)
#define ADF4377_ADDRESS_ASCENSION_R(x)  (((x) & 0x1) << 5)
#define ADF4377_SDO_ACTIVE_R(x)         (((x) & 0x1) << 4)
#define ADF4377_SDO_ACTIVE(x)           (((x) & 0x1) << 3)
#define ADF4377_ADDRESS_ASCENSION(x)    (((x) & 0x1) << 2)
#define ADF4377_LSB_FIRST(x)            (((x) & 0x1) << 1)
#define ADF4377_SOFT_RESET(x)           (((x) & 0x1) << 0)

/* ADF4377 REG0000 Bit Definition */
#define ADF4377_SDO_ACTIVE_SPI_3W       0x0
#define ADF4377_SDO_ACTIVE_SPI_4W       0x1

#define ADF4377_ADDR_ASC_AUTO_DECR      0x0
#define ADF4377_ADDR_ASC_AUTO_INCR      0x1

#define ADF4377_LSB_FIRST_MSB           0x0
#define ADF4377_LSB_FIRST_LSB           0x1

#define ADF4377_SOFT_RESET_N_OP         0x0
#define ADF4377_SOFT_RESET_EN           0x1

/* ADF4377 REG0001 Map */
#define ADF4377_SINGLE_INSTRUCTION(x)   (((x) & 0x1) << 7)
#define ADF4377_R001_RSV6               ((0x0) << 6)
#define ADF4377_MASTER_READBACK_CTRL(x) (((x) & 0x1) << 5)
#define ADF4377_R001_RSV4               ((0x0) << 4)
#define ADF4377_R001_RSV2               ((0x0) << 2)
#define ADF4377_R001_RSV1               ((0x0) << 1)
#define ADF4377_R001_RESERVED           ((0x0 << 3) | (0x0 << 0))

/* ADF4377 REG0001 Bit Definition */
#define ADF4377_SPI_STREAM_EN           0x0
#define ADF4377_SPI_STREAM_DIS          0x1

#define ADF4377_RB_SLAVE_REG            0x0
#define ADF4377_RB_MASTER_REG           0x1

/* ADF4377 REG0003 Bit Definition */
#define ADF4377_R003_RESERVED           (0x0 << 4)
#define ADF4377_CHIP_TYPE               0x06

/* ADF4377 REG0004 Bit Definition */
#define ADF4377_PRODUCT_ID_LSB          0x0005

/* ADF4377 REG0005 Bit Definition */
#define ADF4377_PRODUCT_ID_MSB          0x0005

/* ADF4377 REG000A Map */
#define ADF4377_SCRATCHPAD(x)           ((x) & 0xFF)

/* ADF4377 REG000B Bit Definition */
#define ADF4377_SPI_REVISION            0x01

/* ADF4377 REG000C Bit Definition */
#define ADF4377_VENDOR_ID_LSB           0x456

/* ADF4377 REG000D Bit Definition */
#define ADF4377_VENDOR_ID _MSB          0x456

/* ADF4377 REG0010 Map*/
#define ADF4377_N_INT_LSB(x)            ((x) & 0xFF)

/* ADF4377 REG0011 Map*/
#define ADF4377_EN_AUTOCAL(x)           (((x) & 0x1) << 7)
#define ADF4377_EN_RDBLR(x)             (((x) & 0x1) << 6)
#define ADF4377_DCLK_DIV2(x)            (((x) & 0x3) << 4)
#define ADF4377_N_INT_MSB(x)            (((x) & 0xF) << 0)

/* ADF4377 REG0011 Bit Definition */
#define ADF4377_VCO_CALIB_DIS           0x0
#define ADF4377_VCO_CALIB_EN            0x1

#define ADF4377_REF_DBLR_DIS            0x0
#define ADF4377_REF_DBLR_EN             0x1

#define ADF4377_CLK_DIV2_DIV_1          0x0
#define ADF4377_CLK_DIV2_DIV_2          0x1
#define ADF4377_CLK_DIV2_DIV_4          0x2
#define ADF4377_CLK_DIV2_DIV_8          0x3

/* ADF4377 REG0012 Map*/
#define ADF4377_CLKOUT_DIV(x)           (((x) & 0x3) << 6)
#define ADF4377_R_DIV(x)                (((x) & 0x3F) << 0)

/* ADF4377 REG0012 Bit Definition */
#define ADF4377_CLKOUT_DIV_1            0x0
#define ADF4377_CLKOUT_DIV_2            0x1
#define ADF4377_CLKOUT_DIV_4            0x2
#define ADF4377_CLKOUT_DIV_8            0x3

#define ADF4377_MIN_R_DIV               0x00
#define ADF4378_MAX_R_DIV               0x3F

/* ADF4377 REG0013 Map */
#define ADF4377_R013_RSV1               (0x0 << 6)
#define ADF4377_M_VCO_CORE(x)           (((x) & 0x3) << 4)
#define ADF4377_M_VCO_BIAS(x)           (((x) & 0xF) << 0)

/* ADF4377 REG0013 Bit Definition */
#define ADF4377_M_VCO_0                 0x0
#define ADF4377_M_VCO_1                 0x1
#define ADF4377_M_VCO_2                 0x2
#define ADF4377_M_VCO_3                 0x3

#define M_VCO_BIAS_MIN                  0xF
#define M_VCO_BIAS_MAX                  0x0

/* ADF4377 REG0014 Map */
#define ADF4377_M_VCO_BAND(x)           (((x) & 0xFF))

/* ADF4377 REG0014 Bit Definition */
#define ADF4377_VCO_BAND_MIN            0xFF
#define ADF4377_VCO_BAND_MAX            0x00

/* ADF4377 REG0015 Map */
#define ADF4377_BLEED_I_LSB(x)          (((x) & 0x3) << 6)
#define ADF4377_BLEED_POL(x)            (((x) & 0x1) << 5)
#define ADF4377_EN_BLEED(x)             (((x) & 0x1) << 4)
#define ADF4377_CP_I(x)                 (((x) & 0xF) << 0)

/* ADF4377 REG0015 Bit Description */
#define ADF4377_CURRENT_SINK            0x0
#define ADF4377_CURRENT_SOURCE          0x1

#define ADF4377_BLEED_CURR_DIS          0x0
#define ADF4377_BLEED_CURR_EN           0x1

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
#define ADF4377_BLEED_I_MSB(x)          ((x) & 0xFF)

/* ADF4377 REG0017 Map */
#define ADF4377_INV_CLKOUT(x)            (((x) & 0x1) << 7)
#define ADF4377_N_DEL(x)                 (((x) & 0x7F) << 0)

/* ADF4377 REG0017 Bit Definition */
#define ADF4377_CLKOUT_INV_DIS          0x0
#define ADF4377_CLKOUT_INV_EN           0x1

/* ADF4377 REG0018 Map */
#define ADF4377_CMOS_OV(x)              (((x) & 0x1) << 7)
#define ADF4377_R_DEL(x)                (((x) & 0x7F) << 0)

/* ADF4377 REG0018 Bit Definition */
#define ADF4377_1V8_LOGIC               0x0
#define ADF4377_3V3_LOGIC               0x1

#define ADF4377_R_DEL_MIN               0x00
#define ADF4377_R_DEL_MAX               0x7F

/* ADF4377 REG0019 Map */
#define ADF4377_CLKOUT2_OP(x)           (((x) & 0x3) << 6)
#define ADF4377_CLKOUT1_OP(x)           (((x) & 0x3) << 4)
#define ADF4377_PD_CLK(x)               (((x) & 0x1) << 3)
#define ADF4377_PD_RDET(x)              (((x) & 0x1) << 2)
#define ADF4377_PD_ADC(x)               (((x) & 0x1) << 1)
#define ADF4377_PD_CALADC(x)            (((x) & 0x1) << 0)

/* ADF4377 REG0019 Bit Definition */
#define ADF4377_CLKOUT2_320MV           0x0
#define ADF4377_CLKOUT2_427MV           0x1
#define ADF4377_CLKOUT2_533MV           0x2
#define ADF4377_CLKOUT2_640MV           0x3

#define ADF4377_CLKOUT1_320MV           0x0
#define ADF4377_CLKOUT1_427MV           0x1
#define ADF4377_CLKOUT1_533MV           0x2
#define ADF4377_CLKOUT1_640MV           0x3

#define ADF4377_PD_CLK_N_OP             0x0
#define ADF4377_PD_CLK_PD               0x1

#define ADF4377_PD_RDET_N_OP            0x0
#define ADF4377_PD_RDET_PD              0x1

#define ADF4377_PD_ADC_N_OP             0x0
#define ADF4377_PD_ADC_PD               0x1

#define ADF4377_PD_CALADC_N_OP          0x0
#define ADF4377_PD_CALADC_PD            0x1

/* ADF4377 REG001A Map */
#define ADF4377_PD_ALL(x)               (((x) & 0x1) << 7)
#define ADF4377_PD_RDIV(x)              (((x) & 0x1) << 6)
#define ADF4377_PD_NDIV(x)              (((x) & 0x1) << 5)
#define ADF4377_PD_VCO(x)               (((x) & 0x1) << 4)
#define ADF4377_PD_LD(x)                (((x) & 0x1) << 3)
#define ADF4377_PD_PFDCP(x)             (((x) & 0x1) << 2)
#define ADF4377_PD_CLKOUT1(x)           (((x) & 0x1) << 1)
#define ADF4377_PD_CLKOUT2(x)           (((x) & 0x1) << 0)

/* ADF4377 REG001A Bit Definition */
#define ADF4377_PD_ALL_N_OP             0x0
#define ADF4377_PD_ALL_PD               0x1

#define ADF4377_PD_RDIV_N_OP            0x0
#define ADF4377_PD_RDIV_PD              0x1

#define ADF4377_PD_NDIV_N_OP             0x0
#define ADF4377_PD_NDIV_PD               0x1

#define ADF4377_PD_VCO_N_OP          0x0
#define ADF4377_PD_VCO_PD            0x1

#define ADF4377_PD_LD_N_OP             0x0
#define ADF4377_PD_LD_PD               0x1

#define ADF4377_PD_PFDCP_N_OP            0x0
#define ADF4377_PD_PFDCP_PD              0x1

#define ADF4377_PD_CLKOUT1_N_OP             0x0
#define ADF4377_PD_CLKOUT1_PD               0x1

#define ADF4377_PD_CLKOUT2_N_OP          0x0
#define ADF4377_PD_CLKOUT2_PD            0x1

/* ADF4377 REG001B Map */
#define ADF4377_EN_LOL(x)               (((x) & 0x1) << 7)
#define ADF4377_LDWIN_PW(x)             (((x) & 0x1) << 6)
#define ADF4377_EN_LDWIN(x)             (((x) & 0x1) << 5)
#define ADF4377_LD_COUNT(x)             (((x) & 0x1F) << 0)

/* ADF4377 REG001B Bit Definition */
#define ADF4377_EN_LOL_DIS              0x0
#define ADF4377_EN_LOL_EN               0x1

#define ADF4377_LDWIN_PW_NARROW         0x0
#define ADF4377_LDWIN_PW_WIDE           0x1

#define ADF4377_EN_LDWIN_DIS            0x0
#define ADF4377_EN_LDWIN_EN             0x1

/* ADF4377 REG001C Map */
#define ADF4377_EN_DNCLK(x)             (((x) & 0x1) << 7)
#define ADF4377_EN_DRCLK(x)             (((x) & 0x1) << 6)
#define ADF4377_R01C_RSV4               0x0 << 5
#define ADF4377_R01C_RSV3               0x0 << 4
#define ADF4377_R01C_RSV2               0x0 << 3
#define ADF4377_RST_LD(x)               (((x) & 0x1) << 2)
#define ADF4377_R01C_RSV1               0x0 << 0

/* ADF4377 REG001C Bit Definition */
#define ADF4377_EN_DNCLK_OFF            0x0
#define ADF4377_EN_DNCLK_ON             0x1

#define ADF4377_EN_DRCLK_OFF            0x0
#define ADF4377_EN_DRCLK_ON             0x1

#define ADF4377_RST_LD_INACTIVE         0x0
#define ADF4377_RST_LD_ACTIVE           0x1

/* ADF4377 REG001D Map */
#define ADF4377_MUXOUT(x)               (((x) & 0xF) << 4)
#define ADF4377_R01D_RSV1               (0x0 << 3)
#define ADF4377_EN_CPTEST(x)            (((x) & 0x1) << 2)
#define ADF4377_CP_DOWN(x)              (((x) & 0x1) << 1)
#define ADF4377_CP_UP(x)                (((x) & 0x1) << 0)

/* ADF4377 REG001D Bit Definitons */
#define ADF4377_MUXOUT_HIGH_Z           0x0
#define ADF4377_MUXOUT_LKDET            0x1
#define ADF4377_MUXOUT_LOW              0x2
#define ADF4377_MUXOUT_DIV_RCLK_2       0x4
#define ADF4377_MUXOUT_DIV_NCLK_2       0x5
#define ADF4377_MUXOUT_HIGH             0x8
#define ADF4377_MUXOUT_REF_OK           0xE

#define ADF4377_EN_CPTEST_OFF           0x0
#define ADF4377_EN_CPTEST_ON            0x1

#define ADF4377_CP_DOWN_OFF             0x0
#define ADF4377_CP_DOWN_ON              0x1

#define ADF4377_CP_UP_OFF               0x0
#define ADF4377_CP_UP_ON                0x1

/* ADF4377 REG001F Map */
#define ADF4377_BST_REF(x)              (((x) & 0x1) << 7)
#define ADF4377_FILT_REF(x)             (((x) & 0x1) << 6)
#define ADF4377_REF_SEL(x)              (((x) & 0x1) << 5)
#define ADF4377_R01F_RSV2               (0x0 << 4)
#define ADF4377_R01F_RSV1               (0x0 << 0)

/* ADF4377 REG001F Bit Description */
#define ADF4377_BST_LARGE_REF_IN        0x0
#define ADF4377_BST_SMALL_REF_IN        0x1

#define ADF4377_FILT_REF_OFF            0x0
#define ADF4377_FILT_REF_ON             0x1

#define ADF4377_REF_SEL_DMA             0x0
#define ADF4377_REF_SEL_LNA             0x1

/* ADF4377 REG0020 Map */
#define ADF4377_R020_RSV5                   (0x0 << 6)
#define ADF4377_R020_RSV4                   (0x0 << 5)
#define ADF4377_RST_SYS(x)                  (((x) & 0x1) << 4)
#define ADF4377_EN_ADC_CLK(x)               (((x) & 0x1) << 3)
#define ADF4377_R020_RSV3                   (0x0 << 2)
#define ADF4377_R020_RSV2                   (0x0 << 1)
#define ADF4377_R020_RSV1                   (0x0 << 0)

/* ADF4377 REG0020 Bit Description */
#define ADF4377_RST_SYS_INACTIVE            0x0
#define ADF4377_RST_SYS_ACTIVE              0x1

#define ADF4377_EN_ADC_CLK_DIS              0x0
#define ADF4377_EN_ADC_CLK_EN               0x1

/* ADF4377 REG0023 Map */
#define ADF4377_R023_RSV7                   (0x0 << 7)
#define ADF4377_R023_RSV6                   (0x0 << 6)
#define ADF4377_R023_RSV0                   (0x0 << 0)

/* ADF4377 REG0024 Map */
#define ADF4377_R024_RSV4                   (0x0 << 7)
#define ADF4377_R024_RSV3                   (0x0 << 3)
#define ADF4377_DCLK_MODE(x)                (((x) & 0x1) << 2)
#define ADF4377_R024_RSV2                   (0x0 << 1)
#define ADF4377_R024_RSV1                   (0x0 << 0)

/* ADF4377 REG0024 Bit Definition */
#define ADF4377_DCLK_DIS                    0x0
#define ADF4377_DCLK_EN                     0x1

/* ADF4377 REG0025 Map */
#define ADF4377_CLKODIV_DB(x)               (((x) & 0x1) << 7)
#define ADF4377_DCLK_DB(x)                  (((x) & 0x1) << 6)
#define ADF4377_R025_RSV3                   (0x0 << 5)
#define ADF4377_R025_RSV2                   (0x0 << 4)
#define ADF4377_R025_RSV1                   (0x0 << 0)

/* ADF4377 REG0025 Bit Definition */
#define ADF4377_CLKODIV_DB_DIS              0x0
#define ADF4377_CLKODIV_DB_EN               0x1

#define ADF4377_DCLK_DIV_DB_DIS             0x0
#define ADF4377_DCLK_DIV_DB_EN              0x1

/* ADF4377 REG0026 Map */
#define ADF4377_VCO_BAND_DIV(x)             ((x) & 0xFF)

/* ADF4377 REG0026 Bit Definition */
#define ADF4377_VCO_BAND_DIV_MIN            0x00
#define ADF4377_VCO_BAND_DIV_MAX            0xFF

/* ADF4377 REG0027 Map */
#define ADF4377_SYNTH_LOCK_TIMEOUT_LSB(x)   ((x) & 0xFF)

/* ADF4377 REG0028 Map */
#define ADF4377_O_VCO_DB(x)                 (((x) & 0x1) << 7)
#define ADF4377_SYNTH_LOCK_TIMEOUT_MSB(x)   (((x) & 0x7F) << 0)

/* ADF4377 REG0028 Bit Definition */
#define ADF4377_O_VCO_DB_DIS            0x0
#define ADF4377_O_VCO_DB_EN             0x1

/* ADF4377 REG0029 Map */
#define ADF4377_VCO_ALC_TIMEOUT_LSB(x)     ((x) & 0xFF)

/* ADF4377 REG002A Map */
#define ADF4377_DEL_CTRL_DB(x)              (((x) & 0x1) << 7)
#define ADF4377_VCO_ALC_TIMEOUT_MSB(x)      (((x) & 0xFF) << 0)

/* ADF4377 REG002A Bit Definition */
#define ADF4377_DEL_CTRL_DB_DIS             0x0
#define ADF4377_DEL_CTRL_DB_EN              0x1

/* ADF4377 REG002D Map */
#define ADF4377_ADC_CLK_DIV(x)              ((x) & 0xFF)

/* ADF4377 REG002E Map */
#define ADF4377_EN_ADC_CNV(x)               (((x) & 0x1) << 7)
#define ADF4377_R02E_RSV5                   (0x0 << 6)
#define ADF4377_R02E_RSV4                   (0x0 << 5)
#define ADF4377_R02E_RSV3                   (0x0 << 4)
#define ADF4377_R02E_RSV2                   (0x0 << 3)
#define ADF4377_R02E_RSV1                   (0x0 << 2)
#define ADF4377_EN_ADC(x)                   (((x) & 0x1) << 1)
#define ADF4377_ADC_A_CONV(x)               (((x) & 0x1) << 0)

/* ADF4377 REG002E Bit Definition */
#define ADF4377_EN_ADC_CNV_DIS              0x0
#define ADF4377_EN_ADC_CNV_EN               0x1

#define ADF4377_EN_ADC_DIS                  0x0
#define ADF4377_EN_ADC_EN                   0x1

#define ADF4377_ADC_A_CONV_ADC_ST_CNV       0x0
#define ADF4377_ADC_A_CONV_VCO_CALIB        0x1

/* ADF4377 REG002F Map */
#define ADF4377_R02F_RSV5                   (0x0 << 7)
#define ADF4377_R02F_RSV4                   (0x0 << 6)
#define ADF4377_R02F_RSV3                   (0x0 << 5)
#define ADF4377_R02F_RSV2                   (0x0 << 4)
#define ADF4377_R02F_RSV1                   (0x0 << 3)
#define ADF4377_DCLK_DIV1(x)                (((x) & 0x3) << 0)

/* ADF4377 REG002F Bit Definition */
#define ADF4377_DCLK_DIV1_1                 0x0
#define ADF4377_DCLK_DIV1_2                 0x1
#define ADF4377_DCLK_DIV1_8                 0x2
#define ADF4377_DCLK_DIV1_32                0x3

/* ADF4377 REG0032 Map */
#define ADF4377_R032_RSV5                   (0x0 << 7)
#define ADF4377_ADC_CLK_SEL(x)              (((x) & 0x1) << 6)
#define ADF4377_R032_RSV4                   (0x0 << 5)
#define ADF4377_R032_RSV3                   (0x0 << 4)
#define ADF4377_R032_RSV2                   (0x0 << 3)
#define ADF4377_R032_RSV1                   (0x0 << 0)

/* ADF4377 REG0032 Bit Definition */
#define ADF4377_ADC_CLK_SEL_N_OP            0x0
#define ADF4377_ADC_CLK_SEL_SPI_CLK         0x1

/* ADF4377 REG003D Map */
#define ADF4377_R03D_RSV2                   (0x0 << 4)
#define ADF4377_O_VCO_BAND(x)               (((x) & 0x1) << 3)
#define ADF4377_O_VCO_CORE(x)               (((x) & 0x1) << 2)
#define ADF4377_O_VCO_BIAS(x)               (((x) & 0x1) << 1)
#define ADF4377_R03D_RSV1                   (0x0 << 0)

/* ADF4377 REG003D Bit Definition */
#define ADF4377_O_VCO_BAND_VCO_CALIB        0x0
#define ADF4377_O_VCO_BAND_M_VCO            0x1

#define ADF4377_O_VCO_CORE_VCO_CALIB        0x0
#define ADF4377_O_VCO_CORE_M_VCO            0x1

#define ADF4377_O_VCO_BIAS_VCO_CALIB        0x0
#define ADF4377_O_VCO_BIAS_M_VCO            0x1

/* ADF4377 REG0045 Map */
#define ADF4377_R045_RESERVED               (0x0 << 1)
#define ADF4377_ADC_ST_CNV(x)               (((x) & 0x1) << 0)

/* ADF4377 REG0045 Bit Definition */
#define ADF4377_ADC_ST_ADC_DIS              0x0
#define ADF4377_ADC_ST_ADC_EN               0x0

/* ADF4377 REG0049 Map */
#define ADF4377_EN_CLK2(x)                  (((x) & 0x1) << 7)
#define ADF4377_EN_CLK1(x)                  (((x) & 0x1) << 6)
#define ADF4377_R049_RSV2                   (0x0 << 5)
#define ADF4377_R049_RSV1                   (0x0 << 4)
#define ADF4377_REF_OK(x)                   (((x) & 0x1) << 3)
#define ADF4377_ADC_BUSY(x)                 (((x) & 0x1) << 2)
#define ADF4377_FSM_BUSY(x)                 (((x) & 0x1) << 1)
#define ADF4377_LOCKED(x)                   (((x) & 0x1) << 0)

/* ADF4377 REG004B Map */
#define ADF4377_R04B_RESERVED               (0x0 << 2)
#define ADF4377_VCO_CORE(x)                 (((x) & 0x3) << 0)

/* ADF4377 REG004C Map */
#define ADF4377_CHIP_TEMP_LSB(x)            ((x) & 0xFF)

/* ADF4377 REG004D Map */
#define ADF4377_R04D_RESERVED               (0x0 << 1)
#define ADF4377_CHIP_TEMP_MSB(x)            (((x) & 0x1) << 0)

/* ADF4377 REG004F Map */
#define ADF4377_VCO_BAND(x)                 ((x) & 0xFF)

/* ADF4377 REG0054 Map */
#define ADF4377_CHIP_VERSION(x)             ((x) & 0xFF)

/* Specifications */
#define ADF4377_SPI_WRITE_CMD		(0x0 << 7)
#define ADF4377_SPI_READ_CMD		(0x1 << 7)
#define ADF4377_SPI_DUMMY_DATA		0x0
#define ADF4377_BUFF_SIZE_BYTES		3

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

struct adf4377_init_param {
	/* SPI Initialization parameters */
	struct spi_init_param	*spi_init;
	/* GPIO Chip Enable */
	struct gpio_init_param	*gpio_ce_param;
	/* SPI 3-Wire */
	bool spi3wire;
};

struct adf4377_dev {
	/* SPI Descriptor */
	struct spi_desc		*spi_desc;
	/* GPIO Chip Enable */
	struct gpio_desc	*gpio_ce;
	/* SPI 3-Wire */
	bool spi3wire;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/** ADF4377 SPI write */
int32_t adf4377_spi_write(struct adf4377_dev *dev, uint8_t reg_addr,
			  uint8_t data);

/** ADF4377 SPI Read */
int32_t adf4377_spi_read(struct adf4377_dev *dev, uint8_t reg_addr,
			 uint8_t *data);

/** ADF4377 Initialization */
int32_t adf4377_init(struct adf4377_dev **device,
		     struct adf4377_init_param *init_param);

/** ADF4377 Resources Deallocation */
int32_t adf4377_remove(struct adf4377_dev *dev);



#endif /* ADF4377_H_ */
