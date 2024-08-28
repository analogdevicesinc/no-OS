/***************************************************************************//**
 *   @file   ade9153a.h
 *   @brief  Header file of ADE9153A Driver.
 *   @author Radu Etz (radu.etz@analog.com)
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
#ifndef __ADE9153A_H__
#define __ADE9153A_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "no_os_util.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_irq.h"
#include "no_os_delay.h"
#include "no_os_units.h"
#include "no_os_alloc.h"
#include "no_os_crc16.h"
#include "no_os_print_log.h"
#include <stdlib.h>
#include <errno.h>
#include <math.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/* SPI commands */
#define ADE9153A_SPI_READ				    NO_OS_BIT(3)

/* Lock write key */
#define ADE9153A_LOCK_KEY					0x3C64

/* Unlock write key */
#define ADE9153A_UNLOCK_KEY					0x4AD1

/* COMPMODE val */
#define ADE9153A_COMPMODEVAL				0x0005

/* RUN */
#define ADE9153A_RUN						0x0001
#define ADE9153A_STOP						0x0000

/* Version product */
#define ADE9153A_VERSION					0x9153a

/* Address range of 16 bit registers */
#define ADE9153A_START_16ADDR 				0x473
#define ADE9153A_END_16ADDR 				0x600

/* ADE9153A CRC constants */
#define ADE9153A_CRC16_POLY			        0x1021
#define ADE9153A_CRC16_INIT_VAL			    0xFFFF

#define ADE9153A_NO_BYTES_W_16				0x0004
#define ADE9153A_NO_BYTES_W_32				0x0006
#define ADE9153A_NO_BYTES_R_16				0x0006
#define ADE9153A_NO_BYTES_R_32				0x0008


/* ENABLE and DISABLE */
#define ENABLE					            1u
#define DISABLE					            0u

/* ADE9153A Register Map */
#define ADE9153A_REG_AIGAIN                 0x000
#define ADE9153A_REG_APHASECAL              0x001
#define ADE9153A_REG_AVGAIN                 0x002
#define ADE9153A_REG_AIRMS_OS               0x003
#define ADE9153A_REG_AVRMS_OS               0x004
#define ADE9153A_REG_APGAIN                 0x005
#define ADE9153A_REG_AWATT_OS               0x006
#define ADE9153A_REG_AFVAR_OS               0x007
#define ADE9153A_REG_AVRMS_OC_OS            0x008
#define ADE9153A_REG_AIRMS_OC_OS            0x009
#define ADE9153A_REG_BIGAIN                 0x010
#define ADE9153A_REG_BPHASECAL              0x011
#define ADE9153A_REG_BIRMS_OS               0x013
#define ADE9153A_REG_BIRMS_OC_OS            0x019
#define ADE9153A_REG_CONFIG0                0x020
#define ADE9153A_REG_VNOM                   0x021
#define ADE9153A_REG_DICOEFF                0x022
#define ADE9153A_REG_BI_PGAGAIN             0x023
#define ADE9153A_REG_MS_ACAL_CFG            0x030
#define ADE9153A_REG_MS_AICC_USER           0x045
#define ADE9153A_REG_MS_BICC_USER           0x046
#define ADE9153A_REG_MS_AVCC_USER           0x047
#define ADE9153A_REG_CT_PHASE_DELAY         0x049
#define ADE9153A_REG_CT_CORNER              0x04A
#define ADE9153A_REG_VDIV_RSMALL            0x04C
#define ADE9153A_REG_AI_WAV                 0x200
#define ADE9153A_REG_AV_WAV                 0x201
#define ADE9153A_REG_AIRMS                  0x202
#define ADE9153A_REG_AVRMS                  0x203
#define ADE9153A_REG_AWATT                  0x204
#define ADE9153A_REG_AVA                    0x206
#define ADE9153A_REG_AFVAR                  0x207
#define ADE9153A_REG_APF                    0x208
#define ADE9153A_REG_AIRMS_OC               0x209
#define ADE9153A_REG_AVRMS_OC               0x20A
#define ADE9153A_REG_BI_WAV                 0x210
#define ADE9153A_REG_BIRMS                  0x212
#define ADE9153A_REG_BIRMS_OC               0x219
#define ADE9153A_REG_MS_ACAL_AICC           0x220
#define ADE9153A_REG_MS_ACAL_AICERT         0x221
#define ADE9153A_REG_MS_ACAL_BICC           0x222
#define ADE9153A_REG_MS_ACAL_BICERT         0x223
#define ADE9153A_REG_MS_ACAL_AVCC           0x224
#define ADE9153A_REG_MS_ACAL_AVCERT         0x225
#define ADE9153A_REG_MS_STATUS_CURRENT      0x240
#define ADE9153A_REG_VERSION_DSP            0x241
#define ADE9153A_REG_VERSION_PRODUCT        0x242
#define ADE9153A_REG_AWATT_ACC              0x39D
#define ADE9153A_REG_AWATTHR_LO             0x39E
#define ADE9153A_REG_AWATTHR_HI             0x39F
#define ADE9153A_REG_AVA_ACC                0x3B1
#define ADE9153A_REG_AVAHR_LO               0x3B2
#define ADE9153A_REG_AVAHR_HI               0x3B3
#define ADE9153A_REG_AFVAR_ACC              0x3BB
#define ADE9153A_REG_AFVARHR_LO             0x3BC
#define ADE9153A_REG_AFVARHR_HI             0x3BD
#define ADE9153A_REG_PWATT_ACC              0x3EB
#define ADE9153A_REG_NWATT_ACC              0x3EF
#define ADE9153A_REG_PFVAR_ACC              0x3F3
#define ADE9153A_REG_NFVAR_ACC              0x3F7
#define ADE9153A_REG_IPEAK                  0x400
#define ADE9153A_REG_VPEAK                  0x401
#define ADE9153A_REG_STATUS                 0x402
#define ADE9153A_REG_MASK                   0x405
#define ADE9153A_REG_OI_LVL                 0x409
#define ADE9153A_REG_OIA                    0x40A
#define ADE9153A_REG_OIB                    0x40B
#define ADE9153A_REG_USER_PERIOD            0x40E
#define ADE9153A_REG_VLEVEL                 0x40F
#define ADE9153A_REG_DIP_LVL                0x410
#define ADE9153A_REG_DIPA                   0x411
#define ADE9153A_REG_SWELL_LVL              0x414
#define ADE9153A_REG_SWELLA                 0x415
#define ADE9153A_REG_APERIOD                0x418
#define ADE9153A_REG_ACT_NL_LVL             0x41C
#define ADE9153A_REG_REACT_NL_LVL           0x41D
#define ADE9153A_REG_APP_NL_LVL             0x41E
#define ADE9153A_REG_PHNOLOAD               0x41F
#define ADE9153A_REG_WTHR                   0x420
#define ADE9153A_REG_VARTHR                 0x421
#define ADE9153A_REG_VATHR                  0x422
#define ADE9153A_REG_LAST_DATA_32           0x423
#define ADE9153A_REG_CT_PHASE_MEAS          0x424
#define ADE9153A_REG_CF_LCFG                0x425
#define ADE9153A_REG_TEMP_TRIM              0x471
#define ADE9153A_REG_CHIP_ID_HI             0x472
#define ADE9153A_REG_CHIP_ID_LO             0x473
#define ADE9153A_REG_RUN                    0x480
#define ADE9153A_REG_CONFIG1                0x481
#define ADE9153A_REG_ANGL_AV_AI             0x485
#define ADE9153A_REG_ANGL_AI_BI             0x488
#define ADE9153A_REG_DIP_CYC                0x48B
#define ADE9153A_REG_SWELL_CYC              0x48C
#define ADE9153A_REG_CFMODE                 0x490
#define ADE9153A_REG_COMPMODE               0x491
#define ADE9153A_REG_ACCMODE                0x492
#define ADE9153A_REG_CONFIG3                0x493
#define ADE9153A_REG_CF1DEN                 0x494
#define ADE9153A_REG_CF2DEN                 0x495
#define ADE9153A_REG_ZXTOUT                 0x498
#define ADE9153A_REG_ZXTHRSH                0x499
#define ADE9153A_REG_ZX_CFG                 0x49A
#define ADE9153A_REG_PHSIGN                 0x49D
#define ADE9153A_REG_CRC_RSLT               0x4A8
#define ADE9153A_REG_CRC_SPI                0x4A9
#define ADE9153A_REG_LAST_DATA_16           0x4AC
#define ADE9153A_REG_LAST_CMD               0x4AE
#define ADE9153A_REG_CONFIG2                0x4AF
#define ADE9153A_REG_EP_CFG                 0x4B0
#define ADE9153A_REG_PWR_TIME               0x4B1
#define ADE9153A_REG_EGY_TIME               0x4B2
#define ADE9153A_REG_CRC_FORCE              0x4B4
#define ADE9153A_REG_TEMP_CFG               0x4B6
#define ADE9153A_REG_TEMP_RSLT              0x4B7
#define ADE9153A_REG_AI_PGAGAIN             0x4B9
#define ADE9153A_REG_WR_LOCK                0x4BF
#define ADE9153A_REG_MS_STATUS_IRQ          0x4C0
#define ADE9153A_REG_EVENT_STATUS           0x4C1
#define ADE9153A_REG_CHIP_STATUS            0x4C2
#define ADE9153A_REG_UART_BAUD_SWITCH       0x4DC
#define ADE9153A_REG_VERSION                0x4FE
#define ADE9153A_REG_AI_WAV_1               0x600
#define ADE9153A_REG_AV_WAV_1               0x601
#define ADE9153A_REG_BI_WAV_1               0x602
#define ADE9153A_REG_AIRMS_1                0x604
#define ADE9153A_REG_BIRMS_1                0x605
#define ADE9153A_REG_AVRMS_1                0x606
#define ADE9153A_REG_AWATT_1                0x608
#define ADE9153A_REG_AFVAR_1                0x60A
#define ADE9153A_REG_AVA_1                  0x60C
#define ADE9153A_REG_APF_1                  0x60E
#define ADE9153A_REG_ AI_WAV_2              0x610
#define ADE9153A_REG_AV_WAV_2               0x611
#define ADE9153A_REG_AIRMS_2                0x612
#define ADE9153A_REG_AVRMS_2                0x613
#define ADE9153A_REG_AWATT_2                0x614
#define ADE9153A_REG_AVA_2                  0x615
#define ADE9153A_REG_AFVAR_2                0x616
#define ADE9153A_REG_APF_2                  0x617
#define ADE9153A_REG_BI_WAV_2               0x618
#define ADE9153A_REG_BIRMS_2                0x61A

/* ADE9153A_REG_CONFIG0 Bit Definition */
#define ADE9153A_DISRPLPF_MSK			    NO_OS_BIT(8)
#define ADE9153A_DISAPLPF_MSK			    NO_OS_BIT(7)
#define ADE9153A_VNOMA_EN_MSK			    NO_OS_BIT(5)
#define ADE9153A_RMS_OC_SRC_MSK			    NO_OS_BIT(4)
#define ADE9153A_ZX_SRC_SEL_MSK			    NO_OS_BIT(3)
#define ADE9153A_INTEN_BI_MSK			    NO_OS_BIT(2)
#define ADE9153A_HPFDIS_MSK			        NO_OS_BIT(0)

/* ADE9153A_REG_BI_PGAGAIN Bit Definition */
#define ADE9153A_BI_PGAGAIN_MSK			    NO_OS_GENMASK(31, 0)

/* ADE9153A_REG_MS_ACAL_CFG Bit Definition */
#define ADE9153A_AUTOCAL_AV_MSK			    NO_OS_BIT(6)
#define ADE9153A_AUTOCAL_BI_MSK			    NO_OS_BIT(5)
#define ADE9153A_AUTOCAL_AI_MSK			    NO_OS_BIT(4)
#define ADE9153A_ACALMODE_BI_MSK	        NO_OS_BIT(3)
#define ADE9153A_ACALMODE_AI_MSK		    NO_OS_BIT(2)
#define ADE9153A_ACAL_RUN_MSK		        NO_OS_BIT(1)
#define ADE9153A_ACAL_MODE_MSK		        NO_OS_BIT(0)

/* ADE9153A_REG_MS_STATUS_CURRENT Bit Definition */
#define ADE9153A_MS_SYSRDYP_MSK			    NO_OS_BIT(0)

/* ADE9153A_REG_IPEAK Bit Definition */
#define ADE9153A_IPPHASE_MSK			    NO_OS_GENMASK(26, 24)
#define ADE9153A_IPEAKVAL_MSK			    NO_OS_GENMASK(23, 0)

/* ADE9153A_REG_VPEAK Bit Definition */
#define ADE9153A_VPEAKVAL_MSK			    NO_OS_GENMASK(23, 0)

/* ADE9153A_REG_STATUS / ADE9153A_REG_MASK Bit Definition */
#define ADE9153A_CHIP_STAT_MSK			    NO_OS_BIT(31)
#define ADE9153A_EVENT_STAT_MSK	            NO_OS_BIT(30)
#define ADE9153A_MS_STAT_MSK	            NO_OS_BIT(29)
#define ADE9153A_PF_RDY_MSK	                NO_OS_BIT(25)
#define ADE9153A_CRC_CHG_MSK	            NO_OS_BIT(24)
#define ADE9153A_CRC_DONE_MSK               NO_OS_BIT(23)
#define ADE9153A_ZXTOAV_MSK	                NO_OS_BIT(21)
#define ADE9153A_ZXBI_MSK	                NO_OS_BIT(20)
#define ADE9153A_ZXAI_MSK	                NO_OS_BIT(19)
#define ADE9153A_ZXAV_MSK	                NO_OS_BIT(17)
#define ADE9153A_RSTDONE_MSK	            NO_OS_BIT(16)
#define ADE9153A_FVARNL_MSK	                NO_OS_BIT(15)
#define ADE9153A_VANL_MSK	                NO_OS_BIT(14)
#define ADE9153A_WATTNL_MSK	                NO_OS_BIT(13)
#define ADE9153A_TEMP_RDY_MSK	            NO_OS_BIT(12)
#define ADE9153A_RMS_OC_RDY_MSK	            NO_OS_BIT(11)
#define ADE9153A_PWRRDY_MSK	                NO_OS_BIT(10)
#define ADE9153A_DREADY_MSK	                NO_OS_BIT(9)
#define ADE9153A_EGYRDY_MSK	                NO_OS_BIT(8)
#define ADE9153A_CF2_MSK	                NO_OS_BIT(7)
#define ADE9153A_CF1_MSK	                NO_OS_BIT(6)
#define ADE9153A_REVPCF2_MSK	            NO_OS_BIT(5)
#define ADE9153A_REVPCF1_MSK	            NO_OS_BIT(4)
#define ADE9153A_REVRPA_MSK	                NO_OS_BIT(2)
#define ADE9153A_REVAPA_MSK	                NO_OS_BIT(0)

/* ADE9153A_REG_OI_LVL Bit Definition */
#define ADE9153A_OILVL_VAL_MSK			    NO_OS_GENMASK(23, 0)

/* ADE9153A_REG_OIA Bit Definition */
#define ADE9153A_OIA_VAL_MSK	            NO_OS_GENMASK(23, 0)

/* ADE9153A_REG_OIB Bit Definition */
#define ADE9153A_OIB_VAL_MSK	            NO_OS_GENMASK(23, 0)

/* ADE9153A_REG_VLEVEL Bit Definition */
#define ADE9153A_VLEVEL_VAL_MSK	            NO_OS_GENMASK(23, 0)

/* ADE9153A_REG_DIPA Bit Definition */
#define ADE9153A_DIPA_VAL_MSK	            NO_OS_GENMASK(23, 0)

/* ADE9153A_REG_SWELLA Bit Definition */
#define ADE9153A_SWELLA_VAL_MSK	            NO_OS_GENMASK(23, 0)

/* ADE9153A_REG_PHNOLOAD Bit Definition */
#define ADE9153A_AFVARNL_MSK	            NO_OS_BIT(2)
#define ADE9153A_AVANL_MSK	                NO_OS_BIT(1)
#define ADE9153A_AWATTNL_MSK	            NO_OS_BIT(0)

/* ADE9153A_REG_CF_LCFG Bit Definition */
#define ADE9153A_CF2_LT_MSK	                NO_OS_BIT(20)
#define ADE9153A_CF1_LT_MSK	                NO_OS_BIT(19)
#define ADE9153A_CF_LTMR_MSK	            NO_OS_GENMASK(18, 0)

/* ADE9153A_REG_TEMP_TRIM Bit Definition */
#define ADE9153A_TEMP_OFFSET_MSK	        NO_OS_GENMASK(31, 16)
#define ADE9153A_TEMP_GAIN_MSK	            NO_OS_GENMASK(15, 0)

/* ADE9153A_REG_CONFIG1 Bit Definition */
#define ADE9153A_EXT_REF_MSK	            NO_OS_BIT(15)
#define ADE9153A_DIP_SWELL_IRQ_MODE_MSK     NO_OS_BIT(14)
#define ADE9153A_BURST_EN_MSK               NO_OS_BIT(11)
#define ADE9153A_PWR_SETTLE_MSK	            NO_OS_GENMASK(9, 8)
#define ADE9153A_CF_ACC_CLR_MSK             NO_OS_BIT(5)
#define ADE9153A_ZX_OUT_OE_MSK              NO_OS_BIT(2)
#define ADE9153A_DREADY_OE_MSK              NO_OS_BIT(1)
#define ADE9153A_SWRST_MSK                  NO_OS_BIT(0)

/* ADE9153A_REG_CFMODE Bit Definition */
#define ADE9153A_CF2DIS_MSK	                NO_OS_BIT(7)
#define ADE9153A_CF1DIS_MSK                 NO_OS_BIT(6)
#define ADE9153A_CF2SEL_MSK	                NO_OS_GENMASK(5, 3)
#define ADE9153A_CF1SEL_MSK	                NO_OS_GENMASK(2, 0)

/* ADE9153A_REG_ACCMODE Bit Definition */
#define ADE9153A_SELFREQ_MSK	            NO_OS_BIT(4)
#define ADE9153A_VARACC_MSK	                NO_OS_GENMASK(3, 2)
#define ADE9153A_WATTACC_MSK	            NO_OS_GENMASK(1, 0)

/* ADE9153A_REG_CONFIG3 Bit Definition */
#define ADE9153A_PEAK_SEL_MSK	            NO_OS_GENMASK(3, 2)
#define ADE9153A_OIB_EN_MSK	                NO_OS_BIT(1)
#define ADE9153A_OIA_EN_MSK	                NO_OS_BIT(0)

/* ADE9153A_REG_ZX_CFG Bit Definition */
#define ADE9153A_DISZXLPF_MSK	            NO_OS_BIT(0)

/* ADE9153A_REG_PHSIGN Bit Definition */
#define ADE9153A_CF2SIGN_MSK	            NO_OS_BIT(7)
#define ADE9153A_CF1SIGN_MSK                NO_OS_BIT(6)
#define ADE9153A_AVARSIGN_MSK	            NO_OS_BIT(1)
#define ADE9153A_AWSIGN_MSK	                NO_OS_BIT(0)

/* ADE9153A_REG_CONFIG2 Bit Definition */
#define ADE9153A_UPERIOD_SEL_MSK	        NO_OS_BIT(12)
#define ADE9153A_HPF_CRN_MSK                NO_OS_GENMASK(11, 9)

/* ADE9153A_REG_EP_CFG Bit Definition */
#define ADE9153A_NOLOAD_TMR_MSK	            NO_OS_GENMASK(7, 5)
#define ADE9153A_RD_RST_EN_MSK	            NO_OS_BIT(3)
#define ADE9153A_EGY_LD_ACCUM_MSK	        NO_OS_BIT(2)
#define ADE9153A_EGY_TMR_MODE_MSK	        NO_OS_BIT(1)
#define ADE9153A_EGY_PWR_EN_MSK	            NO_OS_BIT(0)

/* ADE9153A_REG_CRC_FORCE Bit Definition */
#define ADE9153A_FORCE_CRC_UPDATE_MSK	    NO_OS_BIT(0)

/* ADE9153A_REG_TEMP_CFG Bit Definition */
#define ADE9153A_TEMP_START_MSK	            NO_OS_BIT(3)
#define ADE9153A_TEMP_EN_MSK	            NO_OS_BIT(2)
#define ADE9153A_TEMP_TIME_MSK	            NO_OS_GENMASK(1, 0)

/* ADE9153A_REG_TEMP_RSLT Bit Definition */
#define ADE9153A_TEMP_RESULT_MSK	        NO_OS_GENMASK(11, 0)

/* ADE9153A_REG_AI_PGAGAIN Bit Definition */
#define ADE9153A_AI_SWAP_MSK	            NO_OS_BIT(4)
#define ADE9153A_AI_GAIN_MSK	            NO_OS_GENMASK(2, 0)

/* ADE9153A_REG_MS_STATUS_IRQ Bit Definition */
#define ADE9153A_MS_SYSRDY_MSK	            NO_OS_BIT(14)
#define ADE9153A_MS_CONFERR_MSK	            NO_OS_BIT(13)
#define ADE9153A_MS_ABSENT_MSK	            NO_OS_BIT(12)
#define ADE9153A_MS_TIMEOUT_MSK	            NO_OS_BIT(3)
#define ADE9153A_MS_READY_MSK	            NO_OS_BIT(1)
#define ADE9153A_MS_SHIFT_MSK	            NO_OS_BIT(0)

/* ADE9153A_REG_EVENT_STATUS Bit Definition */
#define ADE9153A_OIB_MSK	                NO_OS_BIT(5)
#define ADE9153A_OIA_MSK	                NO_OS_BIT(4)
#define ADE9153A_SWELLA_MSK	                NO_OS_BIT(2)
#define ADE9153A_DIPA_MSK	                NO_OS_BIT(0)

/* ADE9153A_REG_CHIP_STATUS Bit Definition */
#define ADE9153A_UART_RESET_MSK	            NO_OS_BIT(7)
#define ADE9153A_UART_ERROR2_MSK	        NO_OS_BIT(6)
#define ADE9153A_UART_ERROR1_MSK	        NO_OS_BIT(5)
#define ADE9153A_UART_ERROR0_MSK	        NO_OS_BIT(4)
#define ADE9153A_ERROR3_MSK	                NO_OS_BIT(3)
#define ADE9153A_ERROR2_MSK	                NO_OS_BIT(2)
#define ADE9153A_ERROR1_MSK	                NO_OS_BIT(1)
#define ADE9153A_ERROR0_MSK	                NO_OS_BIT(0)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum ade9153a_bi_gain_e
 * @brief ADE9153A Bi Gain.
 */
enum ade9153a_bi_gain_e {
	/* PGA gain for Current Channel B */
	/* Gain 1 */
	ADE9153a_PGA_CHB_GAIN_1 = 0,
	/* Gain 2 */
	ADE9153a_PGA_CHB_GAIN_2,
	/* Gain 4 */
	ADE9153a_PGA_CHB_GAIN_4
};

/**
 * @enum ade9153a_acal_ch_e
 * @brief select channels for autocalibration
 */
enum ade9153a_acal_ch_e {
	/* Enable autocalibration on the voltage channel. */
	AUTOCAL_AV = 0,
	/* Enable autocalibration on Current Channel B. */
	AUTOCAL_BI,
	/* Enable autocalibration on Current Channel A. */
	AUTOCAL_AI
};

/**
 * @enum ade9153a_acalmode_e
 * @brief select power mode for ch B, ch A
 */
enum ade9153a_acalmode_e {
	/* Normal mode. */
	NORMAL = 0,
	/* Turbo mode */
	TURBO
};

/**
 * @enum ade9153a_selfreq_e
 * @brief select frequency
 */
enum ade9153a_selfreq_e {
	/* 50 Hz */
	F_50_HZ = 0,
	/* 60 Hz */
	F_60_HZ
};

/**
 * @enum ade9153a_pwr_settle_e
 * @brief ADE9153A Power settle.
 */
enum ade9153a_pwr_settle_e {
	/* Configure the time for the power and filter-based */
	/* rms measurements to settle before starting the */
	/* power, energy, and CF accumulations */
	/* 64 ms settle */
	ADE9153a_SETTLE_64_MS = 0,
	/* 128 ms settle */
	ADE9153a_SETTLE_128_MS,
	/* 256 ms settle */
	ADE9153a_SETTLE_256_MS,
	/* 0 ms settle */
	ADE9153a_SETTLE_0_MS
};

/**
 * @enum ade9153a_cf2sel_e
 * @brief ADE9153A CF2SEL type of energy output at CF2 Pin.
 */
enum ade9153a_cf2sel_e {
	/* Configure the time for the power and filter-based */
	/* rms measurements to settle before starting the */
	/* power, energy, and CF accumulations */
	/* Total active power */
	ADE9153a_TOTAL_ACTIVE_POWER = 0,
	/* Total apparent power */
	ADE9153a_TOTAL_APPARENT_POWER,
	/* Fundamental reactive power */
	ADE9153a_TOTAL_FUNDAMENTAL_REACTIVE_POWER
};

/**
 * @enum ade9153a_accmode_e
 * @brief ADE9153A varacc/watacc acc mode.
 */
enum ade9153a_accmode_e {
	/* Fundamental reactive power / total active power accumulation mode */
	/* for energy registers and CFx pulses */
	/* Signed accumulation mode */
	ADE9153a_SIGNED_ACC_MODE = 0,
	/* Absolute value accumulation mode */
	ADE9153a_ABSOLUTE_VAL_ACC_MODE,
	/* Positive accumulation mode */
	ADE9153a_POSITIVE_ACC_MODE,
	/* Negative accumulation mode */
	ADE9153a_NEGATIVE_ACC_MODE
};

/**
 * @enum ade9153a_peak_sel_e
 * @brief ADE9153A Peak sel.
 */
enum ade9153a_peak_sel_e {
	/* Peak detection phase selection */
	/* Phase A and Phase B disabled from voltage and current peak detection */
	ADE9153a_PEAK_DETECTION_DISABLE_PHA_PHB = 0,
	/* Phase A Voltage and current peak detection enabled, */
	/* Phase B current peak detection disabled. */
	ADE9153a_PEAK_DETECTION_ENABLE_V_I_PHA,
	/* Phase A Voltage and current peak detection disabled */
	/* Phase B current peak detection enabled. */
	ADE9153a_PEAK_DETECTION_ENABLE_I_PHB,
	/* Phase A & Phase B peak detection enabled. */
	ADE9153a_PEAK_DETECTION_ENABLE_V_I_PHA_PHB
};

/**
* @enum ade9153a_hpf_crn_e
* @brief ADE9153A Hpf crn high pass filter corner freq.
*/
enum ade9153a_hpf_crn_e {
	/* Enabled when the HPFDIS bit in the CONFIG0 register is equal to zero */
	/* 38.695 Hz */
	ADE9153a_HPF_CORNER_38_695_HZ = 0,
	/* 19.6375 Hz */
	ADE9153a_HPF_CORNER_19_6375_HZ,
	/* 9.895 Hz */
	ADE9153a_HPF_CORNER_9_895_HZ,
	/* 4.9675 Hz */
	ADE9153a_HPF_CORNER_4_9675_HZ,
	/* 2.49 Hz */
	ADE9153a_HPF_CORNER_2_49_HZ,
	/* 1.2475 Hz */
	ADE9153a_HPF_CORNER_1_2475_HZ,
	/* 0.625 Hz */
	ADE9153a_HPF_CORNER_0_625_HZ,
	/* 0.3125 Hz */
	ADE9153a_HPF_CORNER_0_3125_HZ
};

/**
* @enum ade9153a_noload_tmr_e
* @brief ADE9153A Hpf no load tmr
*/
enum ade9153a_noload_tmr_e {
	/* Configures how many 4 kSPS samples over which to evaluate */
	/* the no load condition */
	/* 64 samples */
	ADE9153a_NOLOAD_TMR_SAMPLES_64 = 0,
	/* 128 samples */
	ADE9153a_NOLOAD_TMR_SAMPLES_128,
	/* 256 samples */
	ADE9153a_NOLOAD_TMR_SAMPLES_256,
	/* 512 samples */
	ADE9153a_NOLOAD_TMR_SAMPLES_512,
	/* 1024 samples */
	ADE9153a_NOLOAD_TMR_SAMPLES_1024,
	/* 2048 samples */
	ADE9153a_NOLOAD_TMR_SAMPLES_2048,
	/* 4096 samples */
	ADE9153a_NOLOAD_TMR_SAMPLES_4096,
	/* Disable no load threshold */
	ADE9153a_NOLOAD_TMR_DISABLE
};

/**
* @enum ade9153a_temp_time_e
* @brief ADE9153A Temperature readings
*/
enum ade9153a_temp_time_e {
	/* Select the number of temperature readings to average*/
	/* New temperature measurement every 1ms */
	ADE9153a_TEMP_TIME_SAMPLES_1 = 0,
	/* New temperature measurement every 256 ms */
	ADE9153a_TEMP_TIME_SAMPLES_256,
	/* New temperature measurement every 512 ms */
	ADE9153a_TEMP_TIME_SAMPLES_512,
	/* New temperature measurement every 1 sec */
	ADE9153a_TEMP_TIME_SAMPLES_1024
};

/**
* @enum ade9153a_ai_gain_e
* @brief ADE9153A PGA gain for current CH A
*/
enum ade9153a_ai_gain_e {
	/* PGA gain*/
	/* Gain = 16 */
	ADE9153a_AI_GAIN_16 = 2,
	/* Gain = 24 */
	ADE9153a_AI_GAIN_24 = 3,
	/* Gain = 32 */
	ADE9153a_AI_GAIN_32 = 4,
	/* Gain = 38.4 */
	ADE9153a_AI_GAIN_38_4 = 5
};

/**
* @enum ade9153a_chip_stat_err_e
* @brief ADE9153A chip status errors
*/
enum ade9153a_chip_stat_err_e {
	/* Error 0 - SW/HW reset to clear */
	ADE9153a_ERROR0 = 0,
	/* Error 1 - SW/HW reset to clear */
	ADE9153a_ERROR1,
	/* Error 2 - SW/HW reset to clear */
	ADE9153a_ERROR2,
	/* Error 3 - SW/HW reset to clear */
	ADE9153a_ERROR3,
	/* UART Error 0 - UART reset to clear */
	ADE9153a_UART_ERROR0,
	/* UART Error 1 - UART reset to clear */
	ADE9153a_UART_ERROR1,
	/* UART Error 2 - UART reset to clear */
	ADE9153a_UART_ERROR2,
	/* UART interface reset detected */
	ADE9153a_UART_RESET
};

/**
* @enum ade9153a_phnoload_e
* @brief Type of energy that is in no load condition
*/
enum ade9153a_phnoload_e {
	/* No load */
	/* Phase A fundamental reactive energy is in no load. */
	AFVARNL = 1,
	/* Phase A total apparent energy is in no load */
	AVANL,
	/* Phase A total active energy is in no load */
	AWATTNL
};

/**
* @enum dip_swell_irq_mode_en
* @brief Dip swell irq mode
*/
enum dip_swell_irq_mode_en {
	/* continuous */
	CONTINUOUSE = 0,
	/* One interrupt when entering cond. another when exiting cond. */
	ONE_INT,
};

/**
* @struct ade9153a_init_param
* @brief ADE9153a Device initialization parameters.
*/
struct ade9153a_init_param {
	/* Device communication descriptor */
	struct no_os_spi_init_param 	*spi_init;
	/** GPIO RDY descriptor used to signal when ADC data is available */
	struct no_os_gpio_init_param	*gpio_rdy;
	/** GPIO RESET descriptor used to reset device (HW reset) */
	struct no_os_gpio_init_param  	*gpio_reset;
	/** GPIO ss descriptor used to config comms */
	struct no_os_spi_init_param 	*gpio_ss;
	/** GPIO sck descriptor used to config comms */
	struct no_os_spi_init_param		*gpio_sck;
	/** Enable SPI interface */
	uint8_t							spi_en;
	/** Delay value in ms after reset */
	uint16_t						reset_delay;
	/** IRQ device descriptor used to handle interrupt routine for GPIO RDY */
	struct no_os_irq_ctrl_desc 		*irq_ctrl;
	/** operation of sensor */
	uint8_t							ai_swap;
	/** AI gain init value */
	enum ade9153a_ai_gain_e			ai_pga_gain;
	/** High pass filter corner freq init value */
	enum ade9153a_hpf_crn_e			hpf_crn;
	/** Energy accumulation freq select */
	enum ade9153a_selfreq_e			freq;
	/** Vlevel value */
	uint32_t						vlevel;
	/** Vdiv Rsmall */
	uint32_t						rsmall;
	/** Energy accumulation sample no */
	uint32_t						no_samples;
	/** Ai Gain */
	uint32_t						ai_gain;
	/** External callback used to handle interrupt routine for GPIO RDY */
	/** Set to NULL if callback defined in driver used */
	void (*drdy_callback)(void *context);
};

/**
* @struct ade9153a_dev
* @brief ADE9153A Device structure.
*/
struct ade9153a_dev {
	/** Device communication descriptor */
	struct no_os_spi_desc		*spi_desc;
	/** Burst mode selector */
	uint8_t						burst_en;
	/** GPIO RDY descriptor used to signal when ADC data is available */
	struct no_os_gpio_desc  	*gpio_rdy;
	/** GPIO RESET descriptor used to reset device (HW reset) */
	struct no_os_gpio_desc  	*gpio_reset;
	/** GPIO SS descriptor used to setup comms */
	struct no_os_gpio_desc 		*gpio_ss;
	/** GPIO sck descriptor used to setup comms  */
	struct no_os_gpio_desc		*gpio_sck;
	/** IRQ device descriptor used to handle interrupt routine for GPIO RDY */
	struct no_os_irq_ctrl_desc 	*irq_ctrl;
	/** IRQ callback used to handle interrupt routine for GPIO RDY */
	struct no_os_callback_desc	irq_cb;
	// delay after reset
	uint16_t					reset_delay;
};

/**
* @struct ade9153a_energy_values
* @brief ADE9153A energy registers values
*/
struct ade9153a_energy_values {
	/** Active energy register value */
	int32_t active_energy_reg_val;
	/** Fundamental reactive energy register value */
	int32_t fundamental_reactive_energy_reg_val;
	/** Apparent energy register value */
	int32_t apparent_energy_reg_val;
};

/**
 * @struct ade9153a_power_values
 * @brief ADE9153A power registers values
 */
struct ade9153a_power_values {
	/** Active power register value */
	int32_t active_power_reg_val;
	/** Fundamental reactive power register value */
	int32_t fundamental_reactive_power_reg_val;
	/** Apparent power register value */
	int32_t apparent_power_reg_val;
};

/**
 * @struct ade9153a_rms_values
 * @brief ADE9153A rms registers values
 */
struct ade9153a_rms_values {
	/** Current rms register value */
	int32_t current_rms_reg_val;
	/** Voltage rms register value */
	int32_t voltage_rms_reg_val;
	/** Current rms value */
};

/**
 * @struct ade9153a_half_rms_values
 * @brief ADE9153A half rms registers values
 */
struct ade9153a_half_rms_values {
	/** Current half rms register value */
	int32_t current_h_rms_reg_val;
	/** Voltage half rms register value */
	int32_t voltage_h_rms_reg_val;
};

/**
 * @struct ade9153a_pq_values
 * @brief ADE9153A power quality registers values
 */
struct ade9153a_pq_values {
	/** Power factor register value */
	int32_t power_factor_reg_val;
	/** Period register value */
	int32_t period_reg_val;
	/** Angle AV AI register value */
	int32_t angle_ai_av_reg_val;
};

/**
 * @struct ade9153a_temperature_value
 * @brief ADE9153A temperature value
 */
struct ade9153a_temperature_value {
	/** Temperature register value */
	int16_t temperature_reg_val;
	/** Offset register value */
	uint16_t offset_reg_val;
	/** Gain register value */
	uint16_t gain_reg_val;
};

/**
 * @struct ade9153a_autocal_vals
 * @brief ADE9153A temperature value
 */
struct ade9153a_autocal_vals {
	/** AICC value */
	uint32_t aicc_reg_val;
	/** AICERT value */
	uint32_t aicert_reg_val;
	/** AVCC value */
	uint32_t avcc_reg_val;
	/** AVCERT value */
	uint32_t avcert_reg_val;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

// Select comms interface SPI.
int ade9153a_set_interface_spi(struct ade9153a_dev *dev);

// Select comms interface Serial.
int ade9153a_set_interface_serial(struct ade9153a_dev *dev);

// GPIO interrupt handler for data ready.
static void ade9153a_irq_handler(void *dev);

// Initialize the device.
int ade9153a_init(struct ade9153a_dev **device,
		  struct ade9153a_init_param init_param);

// ADE9153A setup.
int ade9153a_setup(void *dev, struct ade9153a_init_param init_param);

// Read device register.
int ade9153a_read(struct ade9153a_dev *dev, uint16_t reg_addr,
		  uint32_t *reg_data);

// Write device register.
int ade9153a_write(struct ade9153a_dev *dev, uint16_t reg_addr,
		   uint32_t reg_data);

// Update specific register bits.
static int ade9153a_update_bits(struct ade9153a_dev *dev, uint16_t reg_addr,
				uint32_t mask, uint32_t reg_data);

// Remove the device and release resources.
int ade9153a_remove(struct ade9153a_dev *dev);

// Reset the device using SW reset.
int ade9153a_sw_reset(struct ade9153a_dev *dev);

// Reset the device using HW reset.
int ade9153a_hw_reset(struct ade9153a_dev *dev);

// Lock device.
int ade9153a_wr_lock(struct ade9153a_dev *dev);

// Unlock device.
int ade9153a_wr_unlock(struct ade9153a_dev *dev);

// Version product
int ade9153a_version_product(struct ade9153a_dev *dev, uint32_t *data_read);

// Disable the low pass filter in the fundamental reactive power datapath.
int ade9153a_lpf_rp_disable(struct ade9153a_dev *dev);

// Disable the low-pass filter in the total active power datapath.
int ade9153a_lpf_ap_disable(struct ade9153a_dev *dev);

// Use the nominal phase voltage rms, VNOM, in the computation
// of the Phase A total apparent power.
int ade9153a_vnoma_enable(struct ade9153a_dev *dev);

// Use x_WAV waveforms after the high-pass filter and phase
//compensation for the RMS_OC calculation.
int ade9153a_rms_oc_src_x_wav_enable(struct ade9153a_dev *dev);

// Use ADC samples, before the high-pass filter for
// the RMS_OC calculation.
int ade9153a_rms_oc_src_adc_samples_enable(struct ade9153a_dev *dev);

// ZX data source after hpf and phase compensation
int ade9153a_zx_src_after_hpf_enable(struct ade9153a_dev *dev);

// ZX data source before hpf and phase compensation
int ade9153a_zx_src_before_hpf_enable(struct ade9153a_dev *dev);

// Current channel B integrator enable
int ade9153a_i_ch_b_int_enable(struct ade9153a_dev *dev);

// Disable hpf for all channels
int ade9153a_hpf_disable(struct ade9153a_dev *dev);

// Set PGA gain for current channel B
int ade9153a_bi_pgagain_set(struct ade9153a_dev *dev,
			    enum ade9153a_bi_gain_e gain);

// Auto calibration config
int ade9153a_auto_calibration_cfg(struct ade9153a_dev *dev,
				  enum ade9153a_acal_ch_e ch, enum ade9153a_acalmode_e mode);

// Auto calibration run
int ade9153a_auto_calibration_run(struct ade9153a_dev *dev);

// Auto calibration stop
int ade9153a_auto_calibration_stop(struct ade9153a_dev *dev);

// mSure status
int ade9153a_msure_status(struct ade9153a_dev *dev, uint8_t *status);

// Ipeak channel phase
int ade9153a_ip_phase(struct ade9153a_dev *dev, uint8_t *ch);

// Ipeak val
int ade9153a_ipk_val(struct ade9153a_dev *dev, uint32_t *val);

// Vpeak val
int ade9153a_vpk_val(struct ade9153a_dev *dev, uint32_t *val);

// Get interrupt indicator from STATUS register.
int ade9153a_get_int_status(struct ade9153a_dev *dev, uint32_t msk,
			    uint8_t *status);

// Get PF ready indicator.
int ade9153a_get_pf_rdy(struct ade9153a_dev *dev, uint8_t *status);

// Get CRC change indicator.
int ade9153a_get_crc_chg(struct ade9153a_dev *dev, uint8_t *status);

// Get CRC done indicator.
int ade9153a_get_crc_done(struct ade9153a_dev *dev, uint8_t *status);

// Get zero crossing timout on V ch indicator.
int ade9153a_get_zxtoav(struct ade9153a_dev *dev, uint8_t *status);

// Get zero crossing detect on I ch B indicator.
int ade9153a_get_zxbi(struct ade9153a_dev *dev, uint8_t *status);

// Get zero crossing detect on I ch A indicator.
int ade9153a_get_zxai(struct ade9153a_dev *dev, uint8_t *status);

// Get zero crossing detect on V ch indicator.
ade9153a_get_zxav(struct ade9153a_dev *dev, uint8_t *status);

// Get reset done indicator.
int ade9153a_get_rstdone(struct ade9153a_dev *dev, uint8_t *status);

// Get fundamental reactive energy no load condition indicator.
int ade9153a_get_fvarnl(struct ade9153a_dev *dev, uint8_t *status);

// Get total apparent energy no load condition indicator.
int ade9153a_get_vanl(struct ade9153a_dev *dev, uint8_t *status);

// Get total active energy no load condition indicator.
int ade9153a_get_wattnl(struct ade9153a_dev *dev, uint8_t *status);

// Get new temperature reading ready indicator.
int ade9153a_get_temp_rdy(struct ade9153a_dev *dev, uint8_t *status);

// Get RMS_OC values update indicator.
int ade9153a_get_rms_oc_rdy(struct ade9153a_dev *dev, uint8_t *status);

// Get power values registers update indicator.
int ade9153a_get_pwrrdy(struct ade9153a_dev *dev, uint8_t *status);

// Get new waveform samples ready indicator.
int ade9153a_get_dready(struct ade9153a_dev *dev, uint8_t *status);

// Get power values egy ready indicator.
int ade9153a_get_egyrdy(struct ade9153a_dev *dev, uint8_t *status);

// Get CF2 pulse issued indicator.
int ade9153a_get_cf2(struct ade9153a_dev *dev, uint8_t *status);

// Get CF1 pulse issued indicator.
int ade9153a_get_cf1(struct ade9153a_dev *dev, uint8_t *status);

// Get CF2 polarity change indicator.
int ade9153a_get_cf2_chg(struct ade9153a_dev *dev, uint8_t *status);

// Get CF1 polarity change indicator.
int ade9153a_get_cf1_chg(struct ade9153a_dev *dev, uint8_t *status);

// Get RPA (reactive power) sign change indicator.
int ade9153a_get_rpa_chg_sgn(struct ade9153a_dev *dev, uint8_t *status);

// Get APA (active power) sign change indicator.
int ade9153a_get_apa_chg_sgn(struct ade9153a_dev *dev, uint8_t *status);

// Clear PF ready int mask.
int ade9153a_clear_pf_rdy(struct ade9153a_dev *dev);

// Clear CRC change int mask.
int ade9153a_clear_crc_chg(struct ade9153a_dev *dev);

// Clear CRC done int mask.
int ade9153a_clear_crc_done(struct ade9153a_dev *dev);

// Clear zero crossing timout on V ch int mask.
int ade9153a_clear_zxtoav(struct ade9153a_dev *dev);

// Clear zero crossing detect on I ch B int mask.
int ade9153a_clear_zxbi(struct ade9153a_dev *dev);

// Clear zero crossing detect on I ch A int mask.
int ade9153a_clear_zxai(struct ade9153a_dev *dev);

// Clear zero crossing detect on V ch int mask.
int ade9153a_clear_zxav(struct ade9153a_dev *dev);

// Clear reset done int mask.
int ade9153a_clear_rstdone(struct ade9153a_dev *dev);

// Clear fundamental reactive energy no load condition int mask.
int ade9153a_clear_fvarnl(struct ade9153a_dev *dev);

// Clear total apparent energy no load condition int mask.
int ade9153a_clear_vanl(struct ade9153a_dev *dev);

// Clear total active energy no load condition int mask.
int ade9153a_clear_wattnl(struct ade9153a_dev *dev);

// Clear new temperature reading ready int mask.
int ade9153a_clear_temp_rdy(struct ade9153a_dev *dev);

// Clear RMS_OC values update int mask.
int ade9153a_clear_rms_oc_rdy(struct ade9153a_dev *dev);

// Clear power values registers update int mask.
int ade9153a_clear_pwrrdy(struct ade9153a_dev *dev);

// Clear new waveform samples ready int mask.
int ade9153a_clear_dready(struct ade9153a_dev *dev);

// Clear power values egy ready int mask.
int ade9153a_clear_egyrdy(struct ade9153a_dev *dev);

// Clear CF2 pulse issued int mask.
int ade9153a_clear_cf2(struct ade9153a_dev *dev);

// Clear CF1 pulse issued int mask.
ade9153a_clear_cf1(struct ade9153a_dev *dev);

// Clear CF2 polarity change int mask.
int ade9153a_clear_cf2_chg(struct ade9153a_dev *dev);

// Clear CF1 polarity change int mask.
int ade9153a_clear_cf1_chg(struct ade9153a_dev *dev);

// Clear RPA (reactive power) sign change int mask.
int ade9153a_clear_rpa_chg_sgn(struct ade9153a_dev *dev);

// Clear APA (active power) sign change int mask.
int ade9153a_clear_apa_chg_sgn(struct ade9153a_dev *dev);

// Get chip status indicator.
int ade9153a_get_chip_stat(struct ade9153a_dev *dev, uint8_t *status);

// Clear chip status int.
int ade9153a_clear_chip_stat(struct ade9153a_dev *dev, uint32_t *reg_val);

// Get event status indicator.
int ade9153a_get_event_stat(struct ade9153a_dev *dev, uint8_t *status);

// Clear event status int.
int ade9153a_clear_event_stat(struct ade9153a_dev *dev, uint32_t *reg_val);

// Get MS status indicator.
int ade9153a_get_ms_stat(struct ade9153a_dev *dev, uint8_t *status);

// Clear MS status int.
int ade9153a_clear_ms_stat(struct ade9153a_dev *dev, uint32_t *reg_val);

// Enable/disable interrupt.
int ade9153a_control_interrupt(struct ade9153a_dev *dev, uint16_t reg_addr,
			       uint32_t int_msk, uint8_t en);

// Enable an interrupt when any bit in CHIP_STATUS reg is set
int ade9153a_enable_chip_stat_int(struct ade9153a_dev *dev);

// Disable CHIP_STAT interrupt.
int ade9153a_disable_chip_stat_int(struct ade9153a_dev *dev);

// Enable an interrupt when any bit in EVENT_STATUS reg is set
int ade9153a_enable_event_stat_int(struct ade9153a_dev *dev);

// Disable EVENT_STAT interrupt.
int ade9153a_disable_event_stat_int(struct ade9153a_dev *dev);

// Enable an interrupt when any bit in MS_STATUS_IRQ reg is set
int ade9153a_enable_ms_stat_int(struct ade9153a_dev *dev);

// Disable MS_STAT interrupt.
int ade9153a_disable_ms_stat_int(struct ade9153a_dev *dev);

// Enable pf ready interrupt
int ade9153a_enable_pf_rdy_int(struct ade9153a_dev *dev);

// Disable pf ready interrupt.
int ade9153a_disable_pf_rdy_int(struct ade9153a_dev *dev);

// Enable crc change interrupt
int ade9153a_enable_crc_chg_int(struct ade9153a_dev *dev);

// Disable crc change interrupt.
int ade9153a_disable_crc_chg_int(struct ade9153a_dev *dev);

// Enable crc done interrupt
int ade9153a_enable_crc_done_int(struct ade9153a_dev *dev);

// Disable crc done interrupt.
int ade9153a_disable_crc_done_int(struct ade9153a_dev *dev);

// Enable zero corssing timeout on voltage ch interrupt
int ade9153a_enable_zxtoav_int(struct ade9153a_dev *dev);

// Disable zero corssing timeout on voltage ch interrupt.
int ade9153a_disable_zxtoav_int(struct ade9153a_dev *dev);

// Enable zero corssing detected on I ch B interrupt
int ade9153a_enable_zxbi_int(struct ade9153a_dev *dev);

// Disable zero corssing detected on I ch B interrupt.
int ade9153a_disable_zxbi_int(struct ade9153a_dev *dev);

// Enable zero corssing detected on I ch A interrupt
int ade9153a_enable_zxai_int(struct ade9153a_dev *dev);

// Disable zero corssing detected on I ch A interrupt.
int ade9153a_disable_zxai_int(struct ade9153a_dev *dev);

// Enable zero corssing detected on V ch interrupt
int ade9153a_enable_zxav_int(struct ade9153a_dev *dev);

// Disable zero corssing detected on V ch interrupt.
int ade9153a_disable_zxav_int(struct ade9153a_dev *dev);

// Enable fundamental reactive energy no load condition interrupt.
int ade9153a_enable_fvarnl_int(struct ade9153a_dev *dev);

// Disable fundamental reactive energy no load condition interrupt.
int ade9153a_disable_fvarnl_int(struct ade9153a_dev *dev);

// Enable total apparent energy no load condition interrupt.
int ade9153a_enable_vanl_int(struct ade9153a_dev *dev);

// Disable total apparent energy no load condition interrupt.
int ade9153a_disable_vanl_int(struct ade9153a_dev *dev);

// Enable total active energy no load condition interrupt.
int ade9153a_enable_wattnl_int(struct ade9153a_dev *dev);

// Disable total active energy no load condition interrupt.
int ade9153a_disable_wattnl_int(struct ade9153a_dev *dev);

// Enable new temperature reading interrupt.
int ade9153a_enable_temp_rdy_int(struct ade9153a_dev *dev);

// Disable new temperature reading interrupt.
int ade9153a_disable_temp_rdy_int(struct ade9153a_dev *dev);

// Enable RMS_OC values update interrupt.
int ade9153a_enable_rms_oc_rdy_int(struct ade9153a_dev *dev);

// Disable RMS_OC values update interrupt.
int ade9153a_disable_rms_oc_rdy_int(struct ade9153a_dev *dev);

// Enable PWRRDY interrupt.
int ade9153a_enable_pwrrdy_int(struct ade9153a_dev *dev);

// Disable PWRRDY interrupt.
int ade9153a_disable_pwrrdy_int(struct ade9153a_dev *dev);

// Enable data ready interrupt.
int ade9153a_enable_dready_int(struct ade9153a_dev *dev);

// Disable data ready interrupt.
int ade9153a_disable_dready_int(struct ade9153a_dev *dev);

// Enable EGYRDY interrupt.
int ade9153a_enable_egyrdy_int(struct ade9153a_dev *dev);

// Disable EGYRDY interrupt.
int ade9153a_disable_egyrdy_int(struct ade9153a_dev *dev);

// Enable CF2 interrupt.
int ade9153a_enable_cf2_int(struct ade9153a_dev *dev);

// Disable CF2 interrupt.
int ade9153a_disable_cf2_int(struct ade9153a_dev *dev);

// Enable CF1 interrupt.
int ade9153a_enable_cf1_int(struct ade9153a_dev *dev);

// Disable CF1 interrupt.
int ade9153a_disable_cf1_int(struct ade9153a_dev *dev);

// Enable CF2 polarity change sign interrupt.
int ade9153a_enable_revpcf2_int(struct ade9153a_dev *dev);

// Disable CF2 polarity change sign interrupt.
int ade9153a_disable_revpcf2_int(struct ade9153a_dev *dev);

// Enable CF1 polarity change sign interrupt.
int ade9153a_enable_revpcf1_int(struct ade9153a_dev *dev);

// Disable CF1 polarity change sign interrupt.
int ade9153a_disable_revpcf1_int(struct ade9153a_dev *dev);

// Enable Phase A fundamental reactive power changed sign interrupt.
int ade9153a_enable_revrpa_int(struct ade9153a_dev *dev);

// Disable Phase A fundamental reactive power changed sign interrupt.
int ade9153a_disable_revrpa_int(struct ade9153a_dev *dev);

// Enable Phase A total active power changed sign interrupt.
int ade9153a_enable_revapa_int(struct ade9153a_dev *dev);

// Disable Phase A total active power changed sign interrupt.
int ade9153a_disable_revapa_int(struct ade9153a_dev *dev);

// Overcurrent detection threshold level val
int ade9153a_oi_lvl_val(struct ade9153a_dev *dev, uint32_t *val);

// Ch A overcurrent val
int ade9153a_oia_val(struct ade9153a_dev *dev, uint32_t *val);

// Ch B overcurrent val
int ade9153a_oib_val(struct ade9153a_dev *dev, uint32_t *val);

// Vlevel val
int ade9153a_vlevel_val(struct ade9153a_dev *dev, uint32_t *val);

// Voltage value during dip condition
int ade9153a_dipa_val(struct ade9153a_dev *dev, uint32_t *val);

// Voltage value during swell condition
int ade9153a_swella_val(struct ade9153a_dev *dev, uint32_t *val);

// Get phnoload status.
int ade9153a_get_phnoload_status(struct ade9153a_dev *dev, uint8_t *status);

// CF2_LT status
int ade9153a_cf2_lt_status(struct ade9153a_dev *dev, uint8_t *status);

// CF1_LT status
int ade9153a_cf1_lt_status(struct ade9153a_dev *dev, uint8_t *status);

// cf_ltmr write value.
int ade9153a_cf_ltmr_write(struct ade9153a_dev *dev, uint32_t reg_data);

// cf_ltmr read value.
int ade9153a_cf_ltmr_val(struct ade9153a_dev *dev, uint32_t *val);

// temperature sensor offset.
int ade9153a_temp_offset_val(struct ade9153a_dev *dev, uint32_t *val);

// temperature sensor gain.
int ade9153a_temp_gain_val(struct ade9153a_dev *dev, uint32_t *val);

// External voltage reference enable/disable.
int ade9153a_ext_ref(struct ade9153a_dev *dev, uint8_t en);

// External_reference status
int ade9153a_ext_ref_status(struct ade9153a_dev *dev, uint8_t *status);

// Dip swell interrupt mode.
int ade9153a_dip_swell_irq_mode(struct ade9153a_dev *dev,
				enum dip_swell_irq_mode_en mode);

// Burst read enable/disable.
int ade9153a_burst_en(struct ade9153a_dev *dev);

// Set PWR_SETTLE
int ade9153a_pwr_settle_set(struct ade9153a_dev *dev,
			    enum ade9153a_pwr_settle_e time);

// Clear accumulation in the digital freq conv.
int ade9153a_cf_acc_clr(struct ade9153a_dev *dev);

// ZX driven to CF2 pin.
int ade9153a_zx_out_oe(struct ade9153a_dev *dev);

// DREADY driven to CF2 pin.
int ade9153a_dready_oe(struct ade9153a_dev *dev);

// CF2 output disable.
int ade9153a_cf2dis(struct ade9153a_dev *dev);

// CF1 output disable.
int ade9153a_cf1dis(struct ade9153a_dev *dev);

// Set type of energy output on CF2 pin
int ade9153a_cf2sel(struct ade9153a_dev *dev,
		    enum ade9153a_cf2sel_e energy_type);

// Set type of energy output on CF1 pin
int ade9153a_cf1sel(struct ade9153a_dev *dev,
		    enum ade9153a_cf2sel_e energy_type);

// Frequency select.
int ade9153a_selfreq(struct ade9153a_dev *dev, enum ade9153a_selfreq_e freq);

// Frequency setting
int ade9153a_freq_s_status(struct ade9153a_dev *dev, uint8_t *status);

// Select fundamental reactive power accumulation mode
int ade9153a_varacc(struct ade9153a_dev *dev,
		    enum ade9153a_accmode_e varacc_type);

// Select total active power accumulation mode
int ade9153a_wattacc(struct ade9153a_dev *dev,
		     enum ade9153a_accmode_e wattacc_type);

// Select peak detection channels
int ade9153a_peak_sel(struct ade9153a_dev *dev,
		      enum ade9153a_peak_sel_e peak_sel);

// Ch B overcurretn detection enable.
int ade9153a_oib_en(struct ade9153a_dev *dev);

// Ch A overcurretn detection enable.
int ade9153a_oia_en(struct ade9153a_dev *dev);

// Zero crossing lpf disable.
int ade9153a_diszxlpf(struct ade9153a_dev *dev);

// Sign of CF2 power
int ade9153a_cf2sign_status(struct ade9153a_dev *dev, uint8_t *status);

// Sign of CF1 power
int ade9153a_cf1sign_status(struct ade9153a_dev *dev, uint8_t *status);

// Sign of reactive power
int ade9153a_avarsign_status(struct ade9153a_dev *dev, uint8_t *status);

// Sign of active power
int ade9153a_awsign_status(struct ade9153a_dev *dev, uint8_t *status);

// User period select.
int ade9153a_uperiod_sel(struct ade9153a_dev *dev, uint8_t en);

// Hpf corner freq
int ade9153a_hpf_crn(struct ade9153a_dev *dev,
		     enum ade9153a_hpf_crn_e hpf_corner_freq);

// Set no. of samples for no load condition
int ade9153a_noload_tmr(struct ade9153a_dev *dev,
			enum ade9153a_noload_tmr_e noload_samples);

// Energy register read with value reset.
int ade9153a_rd_rst_en(struct ade9153a_dev *dev);

// Internal energy register accum.
int ade9153a_egy_ld_accum(struct ade9153a_dev *dev, uint8_t en);

// Energy accumulated based on the number of
// 4 kSPS samples or zero-crossing events
int ade9153a_egy_tmr_mode(struct ade9153a_dev *dev, uint8_t en);

// Enable the energy and power accumulator when
// the run bit is also set.
int ade9153a_egy_pwr_en(struct ade9153a_dev *dev);

// Force CRC calculation to start (update).
int ade9153a_force_crc_update(struct ade9153a_dev *dev);

// Manually request a new temperature sensor reading.
int ade9153a_temp_start(struct ade9153a_dev *dev);

// Enable the temperature sensor.
int ade9153a_temp_en(struct ade9153a_dev *dev);

// Select the number of temperature readings to average.
int ade9153a_temp_time(struct ade9153a_dev *dev,
		       enum ade9153a_temp_time_e temp_no_samples);

// Temperature result
int ade9153a_temp_result(struct ade9153a_dev *dev, uint16_t *val);

// COMPMODE
int ade9153a_compmode(struct ade9153a_dev *dev);

// Start measurements.
int ade9153a_run(struct ade9153a_dev *dev);

// Stop measurements.
int ade9153a_stop(struct ade9153a_dev *dev);

// Set the signal side of the PGA.
int ade9153a_ai_swap(struct ade9153a_dev *dev, uint8_t set);

// Set the PGA gain for I chA.
int ade9153a_ai_gain(struct ade9153a_dev *dev, enum ade9153a_ai_gain_e gain);

// New run of mSure ready
int ade9153a_ms_sysrdy(struct ade9153a_dev *dev, uint8_t *status);

// Invalid config of mSure status
int ade9153a_ms_conferr(struct ade9153a_dev *dev, uint8_t *status);

// mSure not detected on the last enabled ch
int ade9153a_ms_absent(struct ade9153a_dev *dev, uint8_t *status);

// mSure timed out (600s)
int ade9153a_ms_timeout(struct ade9153a_dev *dev, uint8_t *status);

// mSure ready
int ade9153a_ms_ready(struct ade9153a_dev *dev, uint8_t *status);

// mSure shift detected in CC val
int ade9153a_ms_shift(struct ade9153a_dev *dev, uint8_t *status);

// Chip status
int ade9153a_chip_status(struct ade9153a_dev *dev, uint8_t *status);

// Temperature value in deg C
int ade9153a_temp_val(struct ade9153a_dev *dev,
		      struct ade9153a_temperature_value *data);

// Read energy values
int ade9153a_energy_vals(struct ade9153a_dev *dev,
			 struct ade9153a_energy_values *data);

// Read power values
int ade9153a_power_vals(struct ade9153a_dev *dev,
			struct ade9153a_power_values *data);

// Read rms values
int ade9153a_rms_vals(struct ade9153a_dev *dev,
		      struct ade9153a_rms_values *data);

// Read half rms values
int ade9153a_half_rms_vals(struct ade9153a_dev *dev,
			   struct ade9153a_half_rms_values *data);

// Read power quaility values
int ade9153a_power_quality_vals(struct ade9153a_dev *dev,
				struct ade9153a_pq_values *data);

// Start autocalibration AI channel
int ade9153a_start_autocal_ai(struct ade9153a_dev *dev,
			      enum ade9153a_acalmode_e mode);

// Start autocalibration BI channel
int ade9153a_start_autocal_bi(struct ade9153a_dev *dev,
			      enum ade9153a_acalmode_e mode);

// Start autocalibration AV channel
int ade9153a_start_autocal_av(struct ade9153a_dev *dev,
			      enum ade9153a_acalmode_e mode);

// Read autocalibration values
int ade9153a_read_autocal_vals(struct ade9153a_dev *dev,
			       struct ade9153a_autocal_vals *data);

#endif // __ADE9153A_H__