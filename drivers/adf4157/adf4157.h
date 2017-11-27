/***************************************************************************//**
*   @file   ADF4157.h
*   @brief  Header file of AD9833 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
********************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************/
#ifndef _ADF4157_H_
#define _ADF4157_H_

/******************************************************************************/
/******************************* Include Files ********************************/
/******************************************************************************/
#include "Communication.h"

/******************************************************************************/
/********************* Macros and Constants Definitions ***********************/
/******************************************************************************/
/* FRAC/INT Register R0 */
/* MUXOUT Control */
#define ADF4157_MUXOUT_Z            (0x00 << 27)
#define ADF4157_MUXOUT_DVdd         (0x01 << 27)
#define ADF4157_MUXOUT_DGND         (0x02 << 27)
#define ADF4157_MUXOUT_RDIV         (0x03 << 27)
#define ADF4157_MUXOUT_NDIV         (0x04 << 27)
#define ADF4157_MUXOUT_ALOCK        (0x05 << 27)
#define ADF4157_MUXOUT_DLOCK        (0x06 << 27)
#define ADF4157_MUXOUT_SDO          (0x07 << 27)
#define ADF4157_MUXOUT_CLKDIV       (0x0A << 27)
#define ADF4157_MUXOUT_FLS          (0x0C << 27)
#define ADF4157_MUXOUT_RDIV2        (0x0D << 27)
#define ADF4157_MUXOUT_NDIV2        (0x0E << 27)
#define ADF4157_MUXOUT_RESERVED     (0x0F << 27)
/* 12-Bit Integer Value INT */
#define ADF4157_INT_VAL(x)          ((x & 0xFFF) << 15)
/* 12-Bit Fractional Value FRAC */
#define ADF4157_FRAC_VAL_MSB(x)     ((x & 0x1FFE000) >> 10)
/* Control bits */
#define ADF4157_R0_CTRL             0x00

/* LSB FRAC Register R1 */
#define ADF4157_FRAC_VAL_LSB(x)     ((x & 0x1FFF) << 15)
#define ADF4157_R1_CTRL             0x01

/* MOD/R Register R2 */
/* Noise Mode */
#define ADF4157_LOW_NOISE           (0x00 << 29)
#define ADF4157_LOW_SPUR            (0x03 << 29)
/* CSR EN */
#define ADF4157_CSR_EN(x)           ((x & 0x1) << 28)
/* Current setting */
#define ADF4157_CURR_SET(x)         ((x & 0xF) << 24)
/* Prescaler */
#define ADF4157_PRESCALER(x)        ((x & 0x1) << 22)
/* RDIV2 */
#define ADF4157_RDIV2(x)            ((x & 0x1) << 21)
/* Reference Doubler */
#define ADF4157_REF_DBL(x)          ((x & 0x1) << 20)
/* 5-Bit R-Counter */
#define ADF4157_R_CNT(x)            ((x & 0x1F) << 15)
/* Control bits */
#define ADF4157_R2_CTRL             0x02

/* Function Register R3 */
/* Sigma Delta Reset */
#define ADF4157_SIG_DEL_RST(x)      ((x & 0x1) << 14)
/* LDP */
#define ADF4157_LDP(x)              ((x & 0x1) << 7)
/* PD Polarity */
#define ADF4157_PD_POL(x)           ((x & 0x1)  << 6)
/* Power down */
#define ADF4157_PD                  ((x & 0x1) << 5)
/* CP Three State */
#define ADF4157_CP_Z(x)             ((x & 0x1) << 4)
/* Counter Reset */
#define ADF4157_CNT_RST(x)          ((x & 0x1) << 3)
/* Control bits */
#define ADF4157_R3_CTRL             0x03

/* CLK DIV Register R4 */
/* NEG BLEED Current*/
#define ADF4157_NEG_BLEED_CUR(x)    ((x && 0x3) << 23)
/* CLK DIV Mode */
#define ADF4157_CLK_DIV_OFF         (0x00 << 19)
#define ADF4157_CLK_FAST_LOCK       (0x01 << 19)
#define ADF4157_CLK_RESYNC_EN       (0x02 << 19)
/* CLK DIV VALUE */
#define ADF4157_CLK_DIV_VAL(x)      ((x & 0xFFF) << 7)
/* Control bits */
#define ADF4157_R4_CTRL             0x04

/* GPIO */
#define ADF4157_LE_OUT              GPIO0_PIN_OUT
#define ADF4157_LE_LOW              GPIO0_LOW
#define ADF4157_LE_HIGH             GPIO0_HIGH

#define ADF4157_MUX_OUT             GPIO1_PIN_OUT
#define ADF4157_MUX_LOW             GPIO1_LOW
#define ADF4157_MUX_HIGH            GPIO1_HIGH

#define ADF4157_CE_OUT              GPIO2_PIN_OUT
#define ADF4157_CE_LOW              GPIO2_LOW
#define ADF4157_CE_HIGH             GPIO2_HIGH

#define ADF4157_LE2_OUT             GPIO6_PIN_OUT
#define ADF4157_LE2_LOW             GPIO6_LOW
#define ADF4157_LE2_HIGH            GPIO6_HIGH

#define ADF4157_CE2_OUT             GPIO7_PIN_OUT
#define ADF4157_CE2_LOW             GPIO7_LOW
#define ADF4157_CE2_HIGH            GPIO7_HIGH

/* Specifications */
#define ADF4157_MAX_OUT_FREQ        6000          /* MHz */
#define ADF4157_MIN_OUT_FREQ        500           /* MHz */
#define ADF4157_MAX_FREQ_45_PRESC   3000          /* MHz */
#define ADF4157_MAX_FREQ_PFD        32000000      /* Hz */
#define ADF4157_MAX_FREQ_REFIN      300000000     /* Hz */
#define ADF4157_MIN_FREQ_REFIN      10000000      /* Hz */
#define ADF4157_FIXED_MODULUS       33554432      /* 2^25 */
#define ADF4157_MAX_R_CNT           32
#define ADF4157_MAX_REG_VAL         0x1FFFFFFF

/* Registers */
#define ADF4157_REG0                0
#define ADF4157_REG1                1
#define ADF4157_REG2                2
#define ADF4157_REG3                3
#define ADF4157_REG4                4

#define ADF4157_SLAVE_ID            1

/******************************************************************************/
/**************************** Types Declarations ******************************/
/******************************************************************************/
typedef struct
{
        unsigned long       clkin;
        unsigned char       ref_doubler_en;
        unsigned char       ref_div2_en;
        unsigned long       r0_user_settings;
        unsigned long       r2_user_settings;
        unsigned long       r3_user_settings;
        unsigned long       r4_user_settings;
}ADF4157_platform_data;

typedef struct
{
        ADF4157_platform_data   *pdata;
        unsigned long       fpfd;               /* Phase Frequency Detector */
        unsigned short      r_cnt;              /* R-counter */
        unsigned long       r0_fract;           /* Fractional value */
        unsigned long       r0_int;             /* Integer value */
        unsigned long       r2_mod;             /* Modulus value */
        float               channel_spacing;    /* Channel spacing */
        unsigned long       reg_val[5];         /* Actual register value */
}ADF4157_state;

/******************************************************************************/
/************************** Functions Declarations ****************************/
/******************************************************************************/
/* Initialize the SPI communication with the device. */
char ADF4157_Init();

/* Transmits 32 bits on SPI. */
char ADF4157_Set(unsigned long value);

/* Increases the R counter value until the PFD frequency is smaller than
ADF4157_MAX_FREQ_PFD. */
long ADF4157_tune_r_cnt(ADF4157_state *st, long r_cnt);

/* Computes the greatest common divider of two numbers. */
unsigned long gcd(unsigned long x, unsigned long y);

/* Sets the ADF4157 output frequency. */
double adf4157_set_freq(ADF4157_state *st, double freq);

#endif /* _ADF4157_H_ */
