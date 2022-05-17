/***************************************************************************//**
 *   @file   ad7293.h
 *   @brief  Header file for ad7293 Driver.
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

#ifndef AD7293_H_
#define AD7293_H_

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

#define AD7293_R1B				NO_OS_BIT(16)
#define AD7293_R2B				NO_OS_BIT(17)
#define AD7293_PAGE_ADDR_MSK			NO_OS_GENMASK(15, 8)
#define AD7293_PAGE(x)				no_os_field_prep(AD7293_PAGE_ADDR_MSK, x)

/* AD7293 Register Map Common */
#define AD7293_REG_NO_OP			(AD7293_R1B | AD7293_PAGE(0x0) | 0x0)
#define AD7293_REG_PAGE_SELECT			(AD7293_R1B | AD7293_PAGE(0x0) | 0x1)
#define AD7293_REG_CONV_CMD			(AD7293_R2B | AD7293_PAGE(0x0) | 0x2)
#define AD7293_REG_RESULT			(AD7293_R1B | AD7293_PAGE(0x0) | 0x3)
#define AD7293_REG_DAC_EN			(AD7293_R1B | AD7293_PAGE(0x0) | 0x4)
#define AD7293_REG_DEVICE_ID			(AD7293_R2B | AD7293_PAGE(0x0) | 0xC)
#define AD7293_REG_SOFT_RESET			(AD7293_R2B | AD7293_PAGE(0x0) | 0xF)

/* AD7293 Register Map Page 0x0 */
#define AD7293_REG_VIN0				(AD7293_R2B | AD7293_PAGE(0x0) | 0x10)
#define AD7293_REG_VIN1				(AD7293_R2B | AD7293_PAGE(0x0) | 0x11)
#define AD7293_REG_VIN2				(AD7293_R2B | AD7293_PAGE(0x0) | 0x12)
#define AD7293_REG_VIN3				(AD7293_R2B | AD7293_PAGE(0x0) | 0x13)
#define AD7293_REG_TSENSE_INT			(AD7293_R2B | AD7293_PAGE(0x0) | 0x20)
#define AD7293_REG_TSENSE_D0			(AD7293_R2B | AD7293_PAGE(0x0) | 0x21)
#define AD7293_REG_TSENSE_D1			(AD7293_R2B | AD7293_PAGE(0x0) | 0x22)
#define AD7293_REG_ISENSE_0			(AD7293_R2B | AD7293_PAGE(0x0) | 0x28)
#define AD7293_REG_ISENSE_1			(AD7293_R2B | AD7293_PAGE(0x0) | 0x29)
#define AD7293_REG_ISENSE_2			(AD7293_R2B | AD7293_PAGE(0x0) | 0x2A)
#define AD7293_REG_ISENSE_3			(AD7293_R2B | AD7293_PAGE(0x0) | 0x2B)
#define AD7293_REG_UNI_VOUT0			(AD7293_R2B | AD7293_PAGE(0x0) | 0x30)
#define AD7293_REG_UNI_VOUT1			(AD7293_R2B | AD7293_PAGE(0x0) | 0x31)
#define AD7293_REG_UNI_VOUT2			(AD7293_R2B | AD7293_PAGE(0x0) | 0x32)
#define AD7293_REG_UNI_VOUT3			(AD7293_R2B | AD7293_PAGE(0x0) | 0x33)
#define AD7293_REG_BI_VOUT0			(AD7293_R2B | AD7293_PAGE(0x0) | 0x34)
#define AD7293_REG_BI_VOUT1			(AD7293_R2B | AD7293_PAGE(0x0) | 0x35)
#define AD7293_REG_BI_VOUT2			(AD7293_R2B | AD7293_PAGE(0x0) | 0x36)
#define AD7293_REG_BI_VOUT3			(AD7293_R2B | AD7293_PAGE(0x0) | 0x37)

/* AD7293 Register Map Page 0x01 */
#define AD7293_REG_AVDD				(AD7293_R2B | AD7293_PAGE(0x01) | 0x10)
#define AD7293_REG_DACVDD_UNI			(AD7293_R2B | AD7293_PAGE(0x01) | 0x11)
#define AD7293_REG_DACVDD_BI			(AD7293_R2B | AD7293_PAGE(0x01) | 0x12)
#define AD7293_REG_AVSS				(AD7293_R2B | AD7293_PAGE(0x01) | 0x13)
#define AD7293_REG_BI_VOUT0_MON			(AD7293_R2B | AD7293_PAGE(0x01) | 0x14)
#define AD7293_REG_BI_VIOU1_MON			(AD7293_R2B | AD7293_PAGE(0x01) | 0x15)
#define AD7293_REG_BI_VOUT2_MON			(AD7293_R2B | AD7293_PAGE(0x01) | 0x16)
#define AD7293_REG_BI_VOUT3_MON			(AD7293_R2B | AD7293_PAGE(0x01) | 0x17)
#define AD7293_REG_RS0_MON			(AD7293_R2B | AD7293_PAGE(0x01) | 0x28)
#define AD7293_REG_RS1_MON			(AD7293_R2B | AD7293_PAGE(0x01) | 0x29)
#define AD7293_REG_RS2_MON			(AD7293_R2B | AD7293_PAGE(0x01) | 0x2A)
#define AD7293_REG_RS3_MON			(AD7293_R2B | AD7293_PAGE(0x01) | 0x2B)

/* AD7293 Register Map Page 0x2 */
#define AD7293_REG_DIGITAL_OUT_EN		(AD7293_R2B | AD7293_PAGE(0x2) | 0x11)
#define AD7293_REG_DIGITAL_INOUT_FUNC		(AD7293_R2B | AD7293_PAGE(0x2) | 0x12)
#define AD7293_REG_DIGITAL_FUNC_POL		(AD7293_R2B | AD7293_PAGE(0x2) | 0x13)
#define AD7293_REG_GENERAL			(AD7293_R2B | AD7293_PAGE(0x2) | 0x14)
#define AD7293_REG_VINX_RANGE0			(AD7293_R2B | AD7293_PAGE(0x2) | 0x15)
#define AD7293_REG_VINX_RANGE1			(AD7293_R2B | AD7293_PAGE(0x2) | 0x16)
#define AD7293_REG_VINX_DIFF_SE			(AD7293_R2B | AD7293_PAGE(0x2) | 0x17)
#define AD7293_REG_VINX_FILTER			(AD7293_R2B | AD7293_PAGE(0x2) | 0x18)
#define AD7293_REG_BG_EN			(AD7293_R2B | AD7293_PAGE(0x2) | 0x19)
#define AD7293_REG_CONV_DELAY			(AD7293_R2B | AD7293_PAGE(0x2) | 0x1A)
#define AD7293_REG_TSENSE_BG_EN			(AD7293_R2B | AD7293_PAGE(0x2) | 0x1B)
#define AD7293_REG_ISENSE_BG_EN			(AD7293_R2B | AD7293_PAGE(0x2) | 0x1C)
#define AD7293_REG_ISENSE_GAIN			(AD7293_R2B | AD7293_PAGE(0x2) | 0x1D)
#define AD7293_REG_DAC_SNOOZE_O			(AD7293_R2B | AD7293_PAGE(0x2) | 0x1F)
#define AD7293_REG_DAC_SNOOZE_1			(AD7293_R2B | AD7293_PAGE(0x2) | 0x20)
#define AD7293_REG_RSX_MON_BG_EN		(AD7293_R2B | AD7293_PAGE(0x2) | 0x23)
#define AD7293_REG_INTEGR_CL			(AD7293_R2B | AD7293_PAGE(0x2) | 0x28)
#define AD7293_REG_PA_ON_CTRL			(AD7293_R2B | AD7293_PAGE(0x2) | 0x29)
#define AD7293_REG_RAMP_TIME_0			(AD7293_R2B | AD7293_PAGE(0x2) | 0x2A)
#define AD7293_REG_RAMP_TIME_1			(AD7293_R2B | AD7293_PAGE(0x2) | 0x2B)
#define AD7293_REG_RAMP_TIME_2			(AD7293_R2B | AD7293_PAGE(0x2) | 0x2C)
#define AD7293_REG_RAMP_TIME_3			(AD7293_R2B | AD7293_PAGE(0x2) | 0x2D)
#define AD7293_REG_CL_FR_IT			(AD7293_R2B | AD7293_PAGE(0x2) | 0x2E)
#define AD7293_REG_INTX_AVSS_AVDD		(AD7293_R2B | AD7293_PAGE(0x2) | 0x2F)

/* AD7293 Register Map Page 0x3 */
#define AD7293_REG_VINX_SEQ			(AD7293_R2B | AD7293_PAGE(0x3) | 0x10)
#define AD7293_REG_ISENSEX_TSENSEX_SEQ		(AD7293_R2B | AD7293_PAGE(0x3) | 0x11)
#define AD7293_REG_RSX_MON_BI_VOUTX_SEQ		(AD7293_R2B | AD7293_PAGE(0x3) | 0x12)

/* AD7293 Register Map Page 0x05 */
#define AD7293_REG_AVDD_HL			(AD7293_R2B | AD7293_PAGE(0x05) | 0x10)
#define AD7293_REG_DACVDD_UNI_HL		(AD7293_R2B | AD7293_PAGE(0x05) | 0x11)
#define AD7293_REG_DACVDD_BI_HL			(AD7293_R2B | AD7293_PAGE(0x05) | 0x12)
#define AD7293_REG_AVSS_HL			(AD7293_R2B | AD7293_PAGE(0x05) | 0x13)
#define AD7293_REG_BI_VOUT0_MON_HL		(AD7293_R2B | AD7293_PAGE(0x05) | 0x14)
#define AD7293_REG_BI_VOUT1_MON_HL		(AD7293_R2B | AD7293_PAGE(0x05) | 0x15)
#define AD7293_REG_BI_VOUT2_MON_HL		(AD7293_R2B | AD7293_PAGE(0x05) | 0x16)
#define AD7293_REG_BI_VOUT3_MON_HL		(AD7293_R2B | AD7293_PAGE(0x05) | 0x17)
#define AD7293_REG_RS0_MON_HL			(AD7293_R2B | AD7293_PAGE(0x05) | 0x28)
#define AD7293_REG_RS1_MON_HL			(AD7293_R2B | AD7293_PAGE(0x05) | 0x29)
#define AD7293_REG_RS2_MON_HL			(AD7293_R2B | AD7293_PAGE(0x05) | 0x2A)
#define AD7293_REG_RS3_MON_HL			(AD7293_R2B | AD7293_PAGE(0x05) | 0x2B)

/* AD7293 Register Map Page 0x06 */
#define AD7293_REG_VIN0_LL			(AD7293_R2B | AD7293_PAGE(0x06) | 0x10)
#define AD7293_REG_VIN1_LL			(AD7293_R2B | AD7293_PAGE(0x06) | 0x11)
#define AD7293_REG_VIN2_LL			(AD7293_R2B | AD7293_PAGE(0x06) | 0x12)
#define AD7293_REG_VIN3_LL			(AD7293_R2B | AD7293_PAGE(0x06) | 0x13)
#define AD7293_REG_TSENSE_D0_LL			(AD7293_R2B | AD7293_PAGE(0x06) | 0x20)
#define AD7293_REG_TSENSE_D1_LL			(AD7293_R2B | AD7293_PAGE(0x06) | 0x21)
#define AD7293_REG_TSENSE_D2_LL			(AD7293_R2B | AD7293_PAGE(0x06) | 0x22)
#define AD7293_REG_ISENSE0_LL			(AD7293_R2B | AD7293_PAGE(0x06) | 0x28)
#define AD7293_REG_ISENSE1_LL			(AD7293_R2B | AD7293_PAGE(0x06) | 0x29)
#define AD7293_REG_ISENSE2_LL			(AD7293_R2B | AD7293_PAGE(0x06) | 0x2A)
#define AD7293_REG_ISENSE3_LL			(AD7293_R2B | AD7293_PAGE(0x06) | 0x2B)

/* AD7293 Register Map Page 0x07 */
#define AD7293_REG_AVDD_LL			(AD7293_R2B | AD7293_PAGE(0x07) | 0x10)
#define AD7293_REG_DACVDD_UNI_LL		(AD7293_R2B | AD7293_PAGE(0x07) | 0x11)
#define AD7293_REG_DACVDD_BI_LL			(AD7293_R2B | AD7293_PAGE(0x07) | 0x12)
#define AD7293_REG_AVSS_LL			(AD7293_R2B | AD7293_PAGE(0x07) | 0x13)
#define AD7293_REG_BI_VOUT0_MON_LL		(AD7293_R2B | AD7293_PAGE(0x07) | 0x14)
#define AD7293_REG_BI_VOUT1_MON_LL		(AD7293_R2B | AD7293_PAGE(0x07) | 0x15)
#define AD7293_REG_BI_VOUT2_MON_LL		(AD7293_R2B | AD7293_PAGE(0x07) | 0x16)
#define AD7293_REG_BI_VOUT3_MON_LL		(AD7293_R2B | AD7293_PAGE(0x07) | 0x17)
#define AD7293_REG_RS0_MON_LL			(AD7293_R2B | AD7293_PAGE(0x07) | 0x28)
#define AD7293_REG_RS1_MON_LL			(AD7293_R2B | AD7293_PAGE(0x07) | 0x29)
#define AD7293_REG_RS2_MON_LL			(AD7293_R2B | AD7293_PAGE(0x07) | 0x2A)
#define AD7293_REG_RS3_MON_LL			(AD7293_R2B | AD7293_PAGE(0x07) | 0x2B)

/* AD7293 Register Map Page 0x08 */
#define AD7293_REG_VIN0_HYS			(AD7293_R2B | AD7293_PAGE(0x08) | 0x10)
#define AD7293_REG_VIN1_HYS			(AD7293_R2B | AD7293_PAGE(0x08) | 0x11)
#define AD7293_REG_VIN2_HYS			(AD7293_R2B | AD7293_PAGE(0x08) | 0x12)
#define AD7293_REG_VIN3_HYS			(AD7293_R2B | AD7293_PAGE(0x08) | 0x13)
#define AD7293_REG_TSENSE_INT_HYS		(AD7293_R2B | AD7293_PAGE(0x08) | 0x20)
#define AD7293_REG_TSENSE_D0_HYS		(AD7293_R2B | AD7293_PAGE(0x08) | 0x21)
#define AD7293_REG_TSENSE_D1_HYS		(AD7293_R2B | AD7293_PAGE(0x08) | 0x22)
#define AD7293_REG_ISENSE0_HYS			(AD7293_R2B | AD7293_PAGE(0x08) | 0x28)
#define AD7293_REG_ISENSE1_HYS			(AD7293_R2B | AD7293_PAGE(0x08) | 0x29)
#define AD7293_REG_ISENSE2_HYS			(AD7293_R2B | AD7293_PAGE(0x08) | 0x2A)
#define AD7293_REG_ISENSE3_HYS			(AD7293_R2B | AD7293_PAGE(0x08) | 0x2B)

/* AD7293 Register Map Page 0x09 */
#define AD7293_REG_AVDD_HYS			(AD7293_R2B | AD7293_PAGE(0x09) | 0x10)
#define AD7293_REG_DACVDD_UNI_HYS		(AD7293_R2B | AD7293_PAGE(0x09) | 0x11)
#define AD7293_REG_DACVDD_BI_HYS		(AD7293_R2B | AD7293_PAGE(0x09) | 0x12)
#define AD7293_REG_AVSS_HYS			(AD7293_R2B | AD7293_PAGE(0x09) | 0x13)
#define AD7293_REG_BI_VOUT0_MON_HYS		(AD7293_R2B | AD7293_PAGE(0x09) | 0x14)
#define AD7293_REG_BI_VOUT1_MON_HYS		(AD7293_R2B | AD7293_PAGE(0x09) | 0x15)
#define AD7293_REG_BI_VOUT2_MON_HYS		(AD7293_R2B | AD7293_PAGE(0x09) | 0x16)
#define AD7293_REG_BI_VOUT3_MON_HYS		(AD7293_R2B | AD7293_PAGE(0x09) | 0x17)
#define AD7293_REG_RS0_MON_HYS			(AD7293_R2B | AD7293_PAGE(0x09) | 0x28)
#define AD7293_REG_RS1_MON_HYS			(AD7293_R2B | AD7293_PAGE(0x09) | 0x29)
#define AD7293_REG_RS2_MON_HYS			(AD7293_R2B | AD7293_PAGE(0x09) | 0x2A)
#define AD7293_REG_RS3_MON_HYS			(AD7293_R2B | AD7293_PAGE(0x09) | 0x2B)

/* AD7293 Register Map Page 0x0A */
#define AD7293_REG_VIN0_MIN			(AD7293_R2B | AD7293_PAGE(0x0A) | 0x10)
#define AD7293_REG_VIN1_MIN			(AD7293_R2B | AD7293_PAGE(0x0A) | 0x11)
#define AD7293_REG_VIN2_MIN			(AD7293_R2B | AD7293_PAGE(0x0A) | 0x12)
#define AD7293_REG_VIN3_MIN			(AD7293_R2B | AD7293_PAGE(0x0A) | 0x13)
#define AD7293_REG_TSENSE_INT_MIN		(AD7293_R2B | AD7293_PAGE(0x0A) | 0x20)
#define AD7293_REG_TSENSE_D0_MIN		(AD7293_R2B | AD7293_PAGE(0x0A) | 0x21)
#define AD7293_REG_TSENSE_D1_MIN		(AD7293_R2B | AD7293_PAGE(0x0A) | 0x22)
#define AD7293_REG_ISENSE0_MIN			(AD7293_R2B | AD7293_PAGE(0x0A) | 0x28)
#define AD7293_REG_ISENSE1_MIN			(AD7293_R2B | AD7293_PAGE(0x0A) | 0x29)
#define AD7293_REG_ISENSE2_MIN			(AD7293_R2B | AD7293_PAGE(0x0A) | 0x2A)
#define AD7293_REG_ISENSE3_MIN			(AD7293_R2B | AD7293_PAGE(0x0A) | 0x2B)

/* AD7293 Register Map Page 0x0B */
#define AD7293_REG_AVDD_MIN			(AD7293_R2B | AD7293_PAGE(0x0B) | 0x10)
#define AD7293_REG_DACVDD_UNI_MIN		(AD7293_R2B | AD7293_PAGE(0x0B) | 0x11)
#define AD7293_REG_DACVDD_BI_MIN		(AD7293_R2B | AD7293_PAGE(0x0B) | 0x12)
#define AD7293_REG_AVSS_MIN			(AD7293_R2B | AD7293_PAGE(0x0B) | 0x13)
#define AD7293_REG_BI_VOUT0_MON_MIN		(AD7293_R2B | AD7293_PAGE(0x0B) | 0x14)
#define AD7293_REG_BI_VOUT1_MON_MIN		(AD7293_R2B | AD7293_PAGE(0x0B) | 0x15)
#define AD7293_REG_BI_VOUT2_MON_MIN		(AD7293_R2B | AD7293_PAGE(0x0B) | 0x16)
#define AD7293_REG_BI_VOUT3_MON_MIN		(AD7293_R2B | AD7293_PAGE(0x0B) | 0x17)
#define AD7293_REG_RS0_MON_MIN			(AD7293_R2B | AD7293_PAGE(0x0B) | 0x28)
#define AD7293_REG_RS1_MON_MIN			(AD7293_R2B | AD7293_PAGE(0x0B) | 0x29)
#define AD7293_REG_RS2_MON_MIN			(AD7293_R2B | AD7293_PAGE(0x0B) | 0x2A)
#define AD7293_REG_RS3_MON_MIN			(AD7293_R2B | AD7293_PAGE(0x0B) | 0x2B)

/* AD7293 Register Map Page 0x0C */
#define AD7293_REG_VIN0_MAX			(AD7293_R2B | AD7293_PAGE(0x0C) | 0x10)
#define AD7293_REG_VIN1_MAX			(AD7293_R2B | AD7293_PAGE(0x0C) | 0x11)
#define AD7293_REG_VIN2_MAX			(AD7293_R2B | AD7293_PAGE(0x0C) | 0x12)
#define AD7293_REG_VIN3_MAX			(AD7293_R2B | AD7293_PAGE(0x0C) | 0x13)
#define AD7293_REG_TSENSE_INT_MAX		(AD7293_R2B | AD7293_PAGE(0x0C) | 0x20)
#define AD7293_REG_TSENSE_D0_MAX		(AD7293_R2B | AD7293_PAGE(0x0C) | 0x21)
#define AD7293_REG_TSENSE_D1_MAX		(AD7293_R2B | AD7293_PAGE(0x0C) | 0x22)
#define AD7293_REG_ISENSE0_MAX			(AD7293_R2B | AD7293_PAGE(0x0C) | 0x28)
#define AD7293_REG_ISENSE1_MAX			(AD7293_R2B | AD7293_PAGE(0x0C) | 0x29)
#define AD7293_REG_ISENSE2_MAX			(AD7293_R2B | AD7293_PAGE(0x0C) | 0x2A)
#define AD7293_REG_ISENSE3_MAX			(AD7293_R2B | AD7293_PAGE(0x0C) | 0x2B)

/* AD7293 Register Map Page 0x0D */
#define AD7293_REG_AVDD_MAX			(AD7293_R2B | AD7293_PAGE(0x0D) | 0x10)
#define AD7293_REG_DACVDD_UNI_MAX		(AD7293_R2B | AD7293_PAGE(0x0D) | 0x11)
#define AD7293_REG_DACVDD_BI_MAX		(AD7293_R2B | AD7293_PAGE(0x0D) | 0x12)
#define AD7293_REG_AVSS_MAX			(AD7293_R2B | AD7293_PAGE(0x0D) | 0x13)
#define AD7293_REG_BI_VOUT0_MON_MAX		(AD7293_R2B | AD7293_PAGE(0x0D) | 0x14)
#define AD7293_REG_BI_VOUT1_MON_MAX		(AD7293_R2B | AD7293_PAGE(0x0D) | 0x15)
#define AD7293_REG_BI_VOUT2_MON_MAX		(AD7293_R2B | AD7293_PAGE(0x0D) | 0x16)
#define AD7293_REG_BI_VOUT3_MON_MAX		(AD7293_R2B | AD7293_PAGE(0x0D) | 0x17)
#define AD7293_REG_RS0_MON_MAX			(AD7293_R2B | AD7293_PAGE(0x0D) | 0x28)
#define AD7293_REG_RS1_MON_MAX			(AD7293_R2B | AD7293_PAGE(0x0D) | 0x29)
#define AD7293_REG_RS2_MON_MAX			(AD7293_R2B | AD7293_PAGE(0x0D) | 0x2A)
#define AD7293_REG_RS3_MON_MAX			(AD7293_R2B | AD7293_PAGE(0x0D) | 0x2B)

/* AD7293 Register Map Page 0xE */
#define AD7293_REG_VIN0_OFFSET			(AD7293_R1B | AD7293_PAGE(0xE) | 0x10)
#define AD7293_REG_VIN1_OFFSET			(AD7293_R1B | AD7293_PAGE(0xE) | 0x11)
#define AD7293_REG_VIN2_OFFSET			(AD7293_R1B | AD7293_PAGE(0xE) | 0x12)
#define AD7293_REG_VIN3_OFFSET			(AD7293_R1B | AD7293_PAGE(0xE) | 0x13)
#define AD7293_REG_TSENSE_INT_OFFSET		(AD7293_R1B | AD7293_PAGE(0xE) | 0x20)
#define AD7293_REG_TSENSE_D0_OFFSET		(AD7293_R1B | AD7293_PAGE(0xE) | 0x21)
#define AD7293_REG_TSENSE_D1_OFFSET		(AD7293_R1B | AD7293_PAGE(0xE) | 0x22)
#define AD7293_REG_ISENSE0_OFFSET		(AD7293_R1B | AD7293_PAGE(0xE) | 0x28)
#define AD7293_REG_ISENSE1_OFFSET		(AD7293_R1B | AD7293_PAGE(0xE) | 0x29)
#define AD7293_REG_ISENSE2_OFFSET		(AD7293_R1B | AD7293_PAGE(0xE) | 0x2A)
#define AD7293_REG_ISENSE3_OFFSET		(AD7293_R1B | AD7293_PAGE(0xE) | 0x2B)
#define AD7293_REG_UNI_VOUT0_OFFSET		(AD7293_R1B | AD7293_PAGE(0xE) | 0x30)
#define AD7293_REG_UNI_VOUT1_OFFSET		(AD7293_R1B | AD7293_PAGE(0xE) | 0x31)
#define AD7293_REG_UNI_VOUT2_OFFSET		(AD7293_R1B | AD7293_PAGE(0xE) | 0x32)
#define AD7293_REG_UNI_VOUT3_OFFSET		(AD7293_R1B | AD7293_PAGE(0xE) | 0x33)
#define AD7293_REG_BI_VOUT0_OFFSET		(AD7293_R1B | AD7293_PAGE(0xE) | 0x34)
#define AD7293_REG_BI_VOUT1_OFFSET		(AD7293_R1B | AD7293_PAGE(0xE) | 0x35)
#define AD7293_REG_BI_VOUT2_OFFSET		(AD7293_R1B | AD7293_PAGE(0xE) | 0x36)
#define AD7293_REG_BI_VOUT3_OFFSET		(AD7293_R1B | AD7293_PAGE(0xE) | 0x37)

/* AD7293 Register Map Page 0x0F */
#define AD7293_REG_AVDD_OFFSET			(AD7293_R1B | AD7293_PAGE(0x0F) | 0x10)
#define AD7293_REG_DACVDD_UNI_OFFSET		(AD7293_R1B | AD7293_PAGE(0x0F) | 0x11)
#define AD7293_REG_DACVDD_BI_OFFSET		(AD7293_R1B | AD7293_PAGE(0x0F) | 0x12)
#define AD7293_REG_AVSS_OFFSET			(AD7293_R1B | AD7293_PAGE(0x0F) | 0x13)
#define AD7293_REG_BI_VOUT0_MON_OFFSET		(AD7293_R1B | AD7293_PAGE(0x0F) | 0x14)
#define AD7293_REG_BI_VOUT1_MON_OFFSET		(AD7293_R1B | AD7293_PAGE(0x0F) | 0x15)
#define AD7293_REG_BI_VOUT2_MON_OFFSET		(AD7293_R1B | AD7293_PAGE(0x0F) | 0x16)
#define AD7293_REG_BI_VOUT3_MON_OFFSET		(AD7293_R1B | AD7293_PAGE(0x0F) | 0x17)
#define AD7293_REG_RS0_MON_OFFSET		(AD7293_R1B | AD7293_PAGE(0x0F) | 0x28)
#define AD7293_REG_RS1_MON_OFFSET		(AD7293_R1B | AD7293_PAGE(0x0F) | 0x29)
#define AD7293_REG_RS2_MON_OFFSET		(AD7293_R1B | AD7293_PAGE(0x0F) | 0x2A)
#define AD7293_REG_RS3_MON_OFFSET		(AD7293_R1B | AD7293_PAGE(0x0F) | 0x2B)

/* AD7293 Register Map Page 0x10 */
#define AD7293_REG_ALERT_SUM			(AD7293_R2B | AD7293_PAGE(0x10) | 0x10)
#define AD7293_REG_VINX_ALERT			(AD7293_R2B | AD7293_PAGE(0x10) | 0x12)
#define AD7293_REG_TSENSEX_ALERT		(AD7293_R2B | AD7293_PAGE(0x10) | 0x14)
#define AD7293_REG_ISENSEX_ALERT		(AD7293_R2B | AD7293_PAGE(0x10) | 0x15)
#define AD7293_REG_BI_VOUTX_MON_ALERT		(AD7293_R2B | AD7293_PAGE(0x10) | 0x18)
#define AD7293_REG_RSX_MON_ALERT		(AD7293_R2B | AD7293_PAGE(0x10) | 0x19)
#define AD7293_REG_INT_LIMIT_AVSS_ALERT		(AD7293_R2B | AD7293_PAGE(0x10) | 0x1A)

/* AD7293 Register Map Page 0x11 */
#define AD7293_REG_VINX_ALERT0			(AD7293_R2B | AD7293_PAGE(0x11) | 0x12)
#define AD7293_REG_TSENSEX_ALERT0		(AD7293_R2B | AD7293_PAGE(0x11) | 0x14)
#define AD7293_REG_ISENSEX_ALERT0		(AD7293_R2B | AD7293_PAGE(0x11) | 0x15)
#define AD7293_REG_BI_VOUTX_MON_ALERT0		(AD7293_R2B | AD7293_PAGE(0x11) | 0x18)
#define AD7293_REG_RSX_MON_ALERT0		(AD7293_R2B | AD7293_PAGE(0x11) | 0x19)
#define AD7293_REG_INT_LIMIT_AVSS_ALERT0	(AD7293_R2B | AD7293_PAGE(0x11) | 0x1A)

/* AD7293 Register Map Page 0x12 */
#define AD7293_REG_VINX_ALERT1			(AD7293_R2B | AD7293_PAGE(0x12) | 0x12)
#define AD7293_REG_TSENSEX_ALERT1		(AD7293_R2B | AD7293_PAGE(0x12) | 0x14)
#define AD7293_REG_ISENSEX_ALERT1		(AD7293_R2B | AD7293_PAGE(0x12) | 0x15)
#define AD7293_REG_BI_VOUTX_MON_ALERT1		(AD7293_R2B | AD7293_PAGE(0x12) | 0x18)
#define AD7293_REG_RSX_MON_ALERT1		(AD7293_R2B | AD7293_PAGE(0x12) | 0x19)
#define AD7293_REG_INT_LIMIT_AVSS_ALERT1	(AD7293_R2B | AD7293_PAGE(0x12) | 0x1A)

/* AD7293 Miscellaneous Definitions */
#define AD7293_READ				NO_OS_BIT(7)
#define AD7293_TRANSF_LEN_MSK			NO_OS_GENMASK(17, 16)
#define AD7293_BUFF_SIZE_BYTES			3
#define AD7293_REG_ADDR_MSK			NO_OS_GENMASK(7, 0)
#define AD7293_REG_VOUT_OFFSET_MSK		NO_OS_GENMASK(5, 4)
#define AD7293_REG_DATA_RAW_MSK			NO_OS_GENMASK(15, 4)
#define AD7293_REG_VINX_RANGE_GET_CH_MSK(x, ch)	(((x) >> (ch)) & 0x1)
#define AD7293_REG_VINX_RANGE_SET_CH_MSK(x, ch)	(((x) & 0x1) << (ch))
#define AD7293_CHIP_ID				0x18
#define AD7293_SOFT_RESET_VAL			0x7293
#define AD7293_SOFT_RESET_CLR_VAL		0x0000
#define AD7293_CONV_CMD_VAL			0x82

/**
 * @enum ad7293_ch_type
 * @brief AD7293 Channel Type
 */
enum ad7293_ch_type {
	AD7293_ADC_VINX,
	AD7293_ADC_TSENSE,
	AD7293_ADC_ISENSE,
	AD7293_DAC,
};

/**
 * @struct ad7293_dev
 * @brief AD7293 Device Descriptor.
 */
struct ad7293_dev {
	/** SPI Descriptor */
	struct no_os_spi_desc		*spi_desc;
	struct no_os_gpio_desc		*gpio_reset;
	uint8_t				page_select;
};

/**
 * @struct ad7293_init_param
 * @brief AD7293 Initialization Parameters structure.
 */
struct ad7293_init_param {
	/** SPI Initialization parameters */
	struct no_os_spi_init_param	*spi_init;
	struct no_os_gpio_init_param	*gpio_reset;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/** AD7293 SPI read */
int ad7293_spi_read(struct ad7293_dev *dev, unsigned int reg, uint16_t *val);

/** AD7293 SPI write */
int ad7293_spi_write(struct ad7293_dev *dev, unsigned int reg, uint16_t val);

/** AD7293 SPI update bits */
int ad7293_spi_update_bits(struct ad7293_dev *dev, unsigned int reg,
			   uint16_t mask, uint16_t val);

/** AD7293 get ADC range */
int ad7293_adc_get_range(struct ad7293_dev *dev, unsigned int ch,
			 uint16_t *range);

/** AD7293 set ADC range */
int ad7293_adc_set_range(struct ad7293_dev *dev, unsigned int ch,
			 uint16_t range);

/** AD7293 set ISENSE gain */
int ad7293_isense_set_gain(struct ad7293_dev *dev, unsigned int ch,
			   uint16_t gain);

/** AD7293 get ISENSE gain */
int ad7293_isense_get_gain(struct ad7293_dev *dev, unsigned int ch,
			   uint16_t *gain);

/** AD7293 get offset */
int ad7293_get_offset(struct ad7293_dev *dev,  enum ad7293_ch_type type,
		      unsigned int ch, uint16_t *offset);

/** AD7293 set offset */
int ad7293_set_offset(struct ad7293_dev *dev,  enum ad7293_ch_type type,
		      unsigned int ch, uint16_t offset);

/** AD7293 write DAC value */
int ad7293_dac_write_raw(struct ad7293_dev *dev, unsigned int ch,
			 uint16_t raw);

/** AD7293 read raw value */
int ad7293_ch_read_raw(struct ad7293_dev *dev, enum ad7293_ch_type type,
		       unsigned int ch, uint16_t *raw);

/** AD7293 Software Reset */
int ad7293_soft_reset(struct ad7293_dev *dev);

/** AD7293 Reset */
int ad7293_reset(struct ad7293_dev *dev);

/** AD7293 Initialization */
int ad7293_init(struct ad7293_dev **device,
		struct ad7293_init_param *init_param);

/** AD7293 Resources Deallocation */
int ad7293_remove(struct ad7293_dev *dev);

#endif /* AD7293_H_ */
