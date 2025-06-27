/***************************************************************************//**
 *   @file   adf4030.h
 *   @brief  Implementation of ADF4030 Driver.
 *   @author Sirac Kucukarabacioglu (sirac.kucukarabacioglu@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
#ifndef ADF4030_H_
#define ADF4030_H_

#include <stdint.h>
#include <string.h>
#include "no_os_units.h"
#include "no_os_util.h"
#include "no_os_spi.h"

/* ADF4030 REG0000 Map */
#define ADF4030_SOFT_RESET_R_MSK		NO_OS_BIT(7)
#define ADF4030_LSB_FIRST_R_MSK			NO_OS_BIT(6)
#define ADF4030_ADDRESS_ASC_R_MSK		NO_OS_BIT(5)
#define ADF4030_SDO_ACTIVE_R_MSK		NO_OS_BIT(4)
#define ADF4030_SDO_ACTIVE_MSK			NO_OS_BIT(3)
#define ADF4030_ADDRESS_ASC_MSK			NO_OS_BIT(2)
#define ADF4030_LSB_FIRST_MSK			NO_OS_BIT(1)
#define ADF4030_SOFT_RESET_MSK			NO_OS_BIT(0)
#define ADF4030_RESET_CMD			0x81

/* ADF4030 REG0003 NO_OS_BIT Definition */
#define ADF4030_CHIP_TYPE			0x06

/* ADF4030 REG0004 NO_OS_BIT Definition */
#define ADF4030_PRODUCT_ID_LSB			0x0005

/* ADF4030 REG0005 NO_OS_BIT Definition */
#define ADF4030_PRODUCT_ID_MSB			0x0005

/* ADF4030 REG000A Map */
#define ADF4030_SCRATCHPAD_MSK			NO_OS_GENMASK(7, 0)

/* ADF4030 REG0010 Map */
#define ADF4030_TDC_TARGET			NO_OS_GENMASK(4, 0)

/* ADF4030 REG0011 Map */
#define ADF4030_MANUAL_MODE			NO_OS_BIT(7)
#define ADF4030_EN_ALIGN			NO_OS_BIT(6)
#define ADF4030_TDC_SOURCE			NO_OS_GENMASK(4, 0)

/* ADF4030 REG0012 Map */
#define ADF4030_EN_DRV7				NO_OS_BIT(7)
#define ADF4030_EN_DRV6				NO_OS_BIT(6)
#define ADF4030_EN_DRV5				NO_OS_BIT(5)
#define ADF4030_EN_DRV4				NO_OS_BIT(4)
#define ADF4030_EN_DRV3				NO_OS_BIT(3)
#define ADF4030_EN_DRV2				NO_OS_BIT(2)
#define ADF4030_EN_DRV1				NO_OS_BIT(1)
#define ADF4030_EN_DRV0				NO_OS_BIT(0)

/* ADF4030 REG0013 Map */
#define ADF4030_PRBS5				NO_OS_BIT(7)
#define ADF4030_PRBS4				NO_OS_BIT(6)
#define ADF4030_PRBS3				NO_OS_BIT(5)
#define ADF4030_PRBS2				NO_OS_BIT(4)
#define ADF4030_PRBS1				NO_OS_BIT(3)
#define ADF4030_PRBS0				NO_OS_BIT(2)
#define ADF4030_EN_DRV9				NO_OS_BIT(1)
#define ADF4030_EN_DRV8				NO_OS_BIT(0)

/* ADF4030 REG0014 Map */
#define ADF4030_CHAN_INV3			NO_OS_BIT(7)
#define ADF4030_CHAN_INV2			NO_OS_BIT(6)
#define ADF4030_CHAN_INV1			NO_OS_BIT(5)
#define ADF4030_CHAN_INV0			NO_OS_BIT(4)
#define ADF4030_PRBS9				NO_OS_BIT(3)
#define ADF4030_PRBS8				NO_OS_BIT(2)
#define ADF4030_PRBS7				NO_OS_BIT(1)
#define ADF4030_PRBS6				NO_OS_BIT(0)

/* ADF4030 REG0015 Map */
#define ADF4030_FALL_EDGE_SRC			NO_OS_BIT(7)
#define ADF4030_FALL_EDGE_TGT			NO_OS_BIT(6)
#define ADF4030_CHAN_INV9			NO_OS_BIT(5)
#define ADF4030_CHAN_INV8			NO_OS_BIT(4)
#define ADF4030_CHAN_INV7			NO_OS_BIT(3)
#define ADF4030_CHAN_INV6			NO_OS_BIT(2)
#define ADF4030_CHAN_INV5			NO_OS_BIT(1)
#define ADF4030_CHAN_INV4			NO_OS_BIT(0)

/* ADF4030 REG0016 Map */
#define ADF4030_TDC_ARM_M			NO_OS_BIT(7)
#define ADF4030_AVGRXP				NO_OS_GENMASK(4, 0)

/* ADF4030 REG0017 Map */
#define ADF4030_NDEL_ADJ			NO_OS_BIT(7)
#define ADF4030_STOP_FSM			NO_OS_BIT(6)
#define ADF4030_ADEL				NO_OS_GENMASK(5, 0)

/* ADF4030 REG0018 Map */
#define ADF4030_DELTA_NDEL_COAR_LSB		NO_OS_GENMASK(7, 0)

/* ADF4030 REG0019 Map */
#define ADF4030_DELTA_NDEL_COAR_MSB		NO_OS_GENMASK(7, 0)

/* ADF4030 REG001A Map */
#define ADF4030_TDC_OFFSET_COM_LSB		NO_OS_GENMASK(7, 0)

/* ADF4030 REG001B Map */
#define ADF4030_TDC_OFFSET_COM_MID		NO_OS_GENMASK(7, 0)

/* ADF4030 REG001C Map */
#define ADF4030_TDC_OFFSET_COM_MSB		NO_OS_GENMASK(4, 0)

/* ADF4030 REG001D Map */
#define ADF4030_TDC_OFFSET0_LSB			NO_OS_GENMASK(7, 0)

/* ADF4030 REG001E Map */
#define ADF4030_TDC_OFFSET0_MSB			NO_OS_GENMASK(7, 0)

/* ADF4030 REG001F Map */
#define ADF4030_TDC_OFFSET1_LSB			NO_OS_GENMASK(7, 0)

/* ADF4030 REG0020 Map */
#define ADF4030_TDC_OFFSET1_MSB			NO_OS_GENMASK(7, 0)

/* ADF4030 REG0021 Map */
#define ADF4030_TDC_OFFSET2_LSB			NO_OS_GENMASK(7, 0)

/* ADF4030 REG0022 Map */
#define ADF4030_TDC_OFFSET2_MSB			NO_OS_GENMASK(7, 0)

/* ADF4030 REG0023 Map */
#define ADF4030_TDC_OFFSET3_LSB			NO_OS_GENMASK(7, 0)

/* ADF4030 REG0024 Map */
#define ADF4030_TDC_OFFSET3_MSB			NO_OS_GENMASK(7, 0)

/* ADF4030 REG0025 Map */
#define ADF4030_TDC_OFFSET4_LSB			NO_OS_GENMASK(7, 0)

/* ADF4030 REG0026 Map */
#define ADF4030_TDC_OFFSET4_MSB			NO_OS_GENMASK(7, 0)

/* ADF4030 REG0027 Map */
#define ADF4030_TDC_OFFSET5_LSB			NO_OS_GENMASK(7, 0)

/* ADF4030 REG0028 Map */
#define ADF4030_TDC_OFFSET5_MSB			NO_OS_GENMASK(7, 0)

/* ADF4030 REG0029 Map */
#define ADF4030_TDC_OFFSET6_LSB			NO_OS_GENMASK(7, 0)

/* ADF4030 REG002A Map */
#define ADF4030_TDC_OFFSET6_MSB			NO_OS_GENMASK(7, 0)

/* ADF4030 REG002B Map */
#define ADF4030_TDC_OFFSET7_LSB			NO_OS_GENMASK(7, 0)

/* ADF4030 REG002C Map */
#define ADF4030_TDC_OFFSET7_MSB			NO_OS_GENMASK(7, 0)

/* ADF4030 REG002D Map */
#define ADF4030_TDC_OFFSET8_LSB			NO_OS_GENMASK(7, 0)

/* ADF4030 REG002E Map */
#define ADF4030_TDC_OFFSET8_MSB			NO_OS_GENMASK(7, 0)

/* ADF4030 REG002F Map */
#define ADF4030_TDC_OFFSET9_LSB			NO_OS_GENMASK(7, 0)

/* ADF4030 REG0030 Map */
#define ADF4030_TDC_OFFSET9_MSB			NO_OS_GENMASK(7, 0)

/* ADF4030 REG0034 Map */
#define ADF4030_ONE_SHOT			NO_OS_BIT(7)
#define ADF4030_CYCLES				NO_OS_BIT(6)
#define ADF4030_DELCAL				NO_OS_GENMASK(5, 0)

/* ADF4030 REG0035 Map */
#define ADF4030_BSYNC_CAL_ON_1_0		NO_OS_GENMASK(7, 6)
#define ADF4030_ALIGN_THOLD			NO_OS_GENMASK(5, 0)

/* ADF4030 REG0036 Map */
#define ADF4030_BSYNC_CAL_ON_9_2		NO_OS_GENMASK(7, 0)

/* ADF4030 REG0037 Map */
#define ADF4030_ALIGN_CYCLES			NO_OS_GENMASK(7, 5)
#define ADF4030_AUTO_PF_BG			NO_OS_BIT(4)
#define ADF4030_EN_BKGND_ALGN			NO_OS_BIT(3)
#define ADF4030_EN_SERIAL_ALIGN			NO_OS_BIT(2)
#define ADF4030_EN_CYCS_RED			NO_OS_BIT(1)
#define ADF4030_EN_ITER				NO_OS_BIT(0)

/* ADF4030 REG0038 Map */
#define ADF4030_RST_BSYNC_CH_7_0		NO_OS_GENMASK(7, 0)

/* ADF4030 REG0039 Map */
#define ADF4030_RST_SYS				NO_OS_BIT(7)
#define ADF4030_MSTR_RST_BSYNC			NO_OS_BIT(6)
#define ADF4030_RST_BSYNC_CH_9_8		NO_OS_GENMASK(1, 0)

/* ADF4030 REG003A Map */
#define ADF4030_PD_DRV7				NO_OS_BIT(7)
#define ADF4030_PD_DRV6				NO_OS_BIT(6)
#define ADF4030_PD_DRV5				NO_OS_BIT(5)
#define ADF4030_PD_DRV4				NO_OS_BIT(4)
#define ADF4030_PD_DRV3				NO_OS_BIT(3)
#define ADF4030_PD_DRV2				NO_OS_BIT(2)
#define ADF4030_PD_DRV1				NO_OS_BIT(1)
#define ADF4030_PD_DRV0				NO_OS_BIT(0)

/* ADF4030 REG003B Map */
#define ADF4030_PD_TX_PATH5			NO_OS_BIT(7)
#define ADF4030_PD_TX_PATH4			NO_OS_BIT(6)
#define ADF4030_PD_TX_PATH3			NO_OS_BIT(5)
#define ADF4030_PD_TX_PATH2			NO_OS_BIT(4)
#define ADF4030_PD_TX_PATH1			NO_OS_BIT(3)
#define ADF4030_PD_TX_PATH0			NO_OS_BIT(2)
#define ADF4030_PD_DRV9				NO_OS_BIT(1)
#define ADF4030_PD_DRV8				NO_OS_BIT(0)

/* ADF4030 REG003C Map */
#define ADF4030_PD_ALL				NO_OS_BIT(7)
#define ADF4030_PD_PLL				NO_OS_BIT(6)
#define ADF4030_PD_TDC				NO_OS_BIT(5)
#define ADF4030_PD_ADC				NO_OS_BIT(4)
#define ADF4030_PD_TX_PATH9			NO_OS_BIT(3)
#define ADF4030_PD_TX_PATH8			NO_OS_BIT(2)
#define ADF4030_PD_TX_PATH7			NO_OS_BIT(1)
#define ADF4030_PD_TX_PATH6			NO_OS_BIT(0)

/* ADF4030 REG003F Map */
#define ADF4030_ODIV_SEL0			NO_OS_BIT(7)
#define ADF4030_BOOST0				NO_OS_BIT(6)
#define ADF4030_RCM0				NO_OS_GENMASK(5, 0)

/* ADF4030 REG0040 Map */
#define ADF4030_AUTO_PD_RCV0			NO_OS_BIT(7)
#define ADF4030_FLOAT_RX0			NO_OS_BIT(5)
#define ADF4030_FLOAT_TX0			NO_OS_BIT(4)
#define ADF4030_LINK_RX0			NO_OS_BIT(3)
#define ADF4030_LINK_TX0			NO_OS_BIT(2)
#define ADF4030_AC_COUPLED0			NO_OS_BIT(1)

/* ADF4030 REG0041 Map */
#define ADF4030_ODIV_SEL1			NO_OS_BIT(7)
#define ADF4030_BOOST1				NO_OS_BIT(6)
#define ADF4030_RCM1				NO_OS_GENMASK(5, 0)

/* ADF4030 REG0042 Map */
#define ADF4030_AUTO_PD_RCV1			NO_OS_BIT(7)
#define ADF4030_FLOAT_RX1			NO_OS_BIT(5)
#define ADF4030_FLOAT_TX1			NO_OS_BIT(4)
#define ADF4030_LINK_RX1			NO_OS_BIT(3)
#define ADF4030_LINK_TX1			NO_OS_BIT(2)
#define ADF4030_AC_COUPLED1			NO_OS_BIT(1)

/* ADF4030 REG0043 Map */
#define ADF4030_ODIV_SEL2			NO_OS_BIT(7)
#define ADF4030_BOOST2				NO_OS_BIT(6)
#define ADF4030_RCM2				NO_OS_GENMASK(5, 0)

/* ADF4030 REG0044 Map */
#define ADF4030_AUTO_PD_RCV2			NO_OS_BIT(7)
#define ADF4030_FLOAT_RX2			NO_OS_BIT(5)
#define ADF4030_FLOAT_TX2			NO_OS_BIT(4)
#define ADF4030_LINK_RX2			NO_OS_BIT(3)
#define ADF4030_LINK_TX2			NO_OS_BIT(2)
#define ADF4030_AC_COUPLED2			NO_OS_BIT(1)

/* ADF4030 REG0045 Map */
#define ADF4030_ODIV_SEL3			NO_OS_BIT(7)
#define ADF4030_BOOST3				NO_OS_BIT(6)
#define ADF4030_RCM3				NO_OS_GENMASK(5, 0)

/* ADF4030 REG0046 Map */
#define ADF4030_AUTO_PD_RCV3			NO_OS_BIT(7)
#define ADF4030_FLOAT_RX3			NO_OS_BIT(5)
#define ADF4030_FLOAT_TX3			NO_OS_BIT(4)
#define ADF4030_LINK_RX3			NO_OS_BIT(3)
#define ADF4030_LINK_TX3			NO_OS_BIT(2)
#define ADF4030_AC_COUPLED3			NO_OS_BIT(1)

/* ADF4030 REG0047 Map */
#define ADF4030_ODIV_SEL4			NO_OS_BIT(7)
#define ADF4030_BOOST4				NO_OS_BIT(6)
#define ADF4030_RCM4				NO_OS_GENMASK(5, 0)

/* ADF4030 REG0048 Map */
#define ADF4030_AUTO_PD_RCV4			NO_OS_BIT(7)
#define ADF4030_FLOAT_RX4			NO_OS_BIT(5)
#define ADF4030_FLOAT_TX4			NO_OS_BIT(4)
#define ADF4030_LINK_RX4			NO_OS_BIT(3)
#define ADF4030_LINK_TX4			NO_OS_BIT(2)
#define ADF4030_AC_COUPLED4			NO_OS_BIT(1)

/* ADF4030 REG0049 Map */
#define ADF4030_ODIV_SEL5			NO_OS_BIT(7)
#define ADF4030_BOOST5				NO_OS_BIT(6)
#define ADF4030_RCM5				NO_OS_GENMASK(5, 0)

/* ADF4030 REG004A Map */
#define ADF4030_AUTO_PD_RCV5			NO_OS_BIT(7)
#define ADF4030_FLOAT_RX5			NO_OS_BIT(5)
#define ADF4030_FLOAT_TX5			NO_OS_BIT(4)
#define ADF4030_LINK_RX5			NO_OS_BIT(3)
#define ADF4030_LINK_TX5			NO_OS_BIT(2)
#define ADF4030_AC_COUPLED5			NO_OS_BIT(1)

/* ADF4030 REG004B Map */
#define ADF4030_ODIV_SEL6			NO_OS_BIT(7)
#define ADF4030_BOOST6				NO_OS_BIT(6)
#define ADF4030_RCM6				NO_OS_GENMASK(5, 0)

/* ADF4030 REG004C Map */
#define ADF4030_AUTO_PD_RCV6			NO_OS_BIT(7)
#define ADF4030_FLOAT_RX6			NO_OS_BIT(5)
#define ADF4030_FLOAT_TX6			NO_OS_BIT(4)
#define ADF4030_LINK_RX6			NO_OS_BIT(3)
#define ADF4030_LINK_TX6			NO_OS_BIT(2)
#define ADF4030_AC_COUPLED6			NO_OS_BIT(1)

/* ADF4030 REG004D Map */
#define ADF4030_ODIV_SEL7			NO_OS_BIT(7)
#define ADF4030_BOOST7				NO_OS_BIT(6)
#define ADF4030_RCM7				NO_OS_GENMASK(5, 0)

/* ADF4030 REG004E Map */
#define ADF4030_AUTO_PD_RCV7			NO_OS_BIT(7)
#define ADF4030_FLOAT_RX7			NO_OS_BIT(5)
#define ADF4030_FLOAT_TX7			NO_OS_BIT(4)
#define ADF4030_LINK_RX7			NO_OS_BIT(3)
#define ADF4030_LINK_TX7			NO_OS_BIT(2)
#define ADF4030_AC_COUPLED7			NO_OS_BIT(1)

/* ADF4030 REG004F Map */
#define ADF4030_ODIV_SEL8			NO_OS_BIT(7)
#define ADF4030_BOOST8				NO_OS_BIT(6)
#define ADF4030_RCM8				NO_OS_GENMASK(5, 0)

/* ADF4030 REG0050 Map */
#define ADF4030_AUTO_PD_RCV8			NO_OS_BIT(7)
#define ADF4030_FLOAT_RX8			NO_OS_BIT(5)
#define ADF4030_FLOAT_TX8			NO_OS_BIT(4)
#define ADF4030_LINK_RX8			NO_OS_BIT(3)
#define ADF4030_LINK_TX8			NO_OS_BIT(2)
#define ADF4030_AC_COUPLED8			NO_OS_BIT(1)

/* ADF4030 REG0051 Map */
#define ADF4030_ODIV_SEL9			NO_OS_BIT(7)
#define ADF4030_BOOST9				NO_OS_BIT(6)
#define ADF4030_RCM9				NO_OS_GENMASK(5, 0)

/* ADF4030 REG0052 Map */
#define ADF4030_AUTO_PD_RCV9			NO_OS_BIT(7)
#define ADF4030_FLOAT_RX9			NO_OS_BIT(5)
#define ADF4030_FLOAT_TX9			NO_OS_BIT(4)
#define ADF4030_LINK_RX9			NO_OS_BIT(3)
#define ADF4030_LINK_TX9			NO_OS_BIT(2)
#define ADF4030_AC_COUPLED9			NO_OS_BIT(1)

/* ADF4030 REG0053 Map */
#define ADF4030_ODIVA_LSB			NO_OS_GENMASK(7, 0)

/* ADF4030 REG0054 Map */
#define ADF4030_ODIVB_LSB			NO_OS_GENMASK(7, 4)
#define ADF4030_ODIVA_MSB			NO_OS_GENMASK(3, 0)

/* ADF4030 REG0055 Map */
#define ADF4030_ODIVB_MSB			NO_OS_GENMASK(7, 0)

/* ADF4030 REG0056 Map */
#define ADF4030_NDIV				NO_OS_GENMASK(7, 0)

/* ADF4030 REG0057 Map */
#define ADF4030_CP_I				NO_OS_GENMASK(6, 5)
#define ADF4030_RDIV				NO_OS_GENMASK(4, 0)

/* ADF4030 REG005C Map */
#define ADF4030_RST_PLL_CAL			NO_OS_BIT(7)
#define ADF4030_PLL_CAL_EN			NO_OS_BIT(6)

/* ADF4030 REG005C Map */
#define ADF4030_CMOS_OV				NO_OS_BIT(7)
#define ADF4030_RST_ALIGN_IRQ			NO_OS_BIT(5)
#define ADF4030_RST_TEMP			NO_OS_BIT(5)
#define ADF4030_MASK_TDC_ERR			NO_OS_BIT(4)
#define ADF4030_MASK_LD				NO_OS_BIT(3)
#define ADF4030_MASK_ALIGN_IRQ			NO_OS_BIT(2)
#define ADF4030_MASK_TEMP			NO_OS_BIT(1)
#define ADF4030_IRQB_OPEN_DRAIN			NO_OS_BIT(0)

/* ADF4030 REG0060 Map */
#define ADF4030_ADC_C_CNV			NO_OS_BIT(6)

/* ADF4030 REG0061 Map */
#define ADF4030_RST_TDC_ERR			NO_OS_BIT(7)
#define ADF4030_ADC_CLK_SEL			NO_OS_BIT(6)
#define ADF4030_EN_ADC_CNV			NO_OS_BIT(2)
#define ADF4030_EN_ADC_CLK			NO_OS_BIT(1)
#define ADF4030_EN_ADC				NO_OS_BIT(0)

/* ADF4030 REG0063 Map */
#define ADF4030_MUXCODE1			NO_OS_GENMASK(4, 0)

/* ADF4030 REG0064 Map */
#define ADF4030_MUXCODE2			NO_OS_GENMASK(4, 0)

/* ADF4030 REG0068 Map */
#define ADF4030_GPO2				NO_OS_BIT(7)
#define ADF4030_GPO1				NO_OS_BIT(6)

/* ADF4030 REG0072 Map */
#define ADF4030_ADC_ST_CNV				NO_OS_BIT(0)

/* ADF4030 REG0073 Map */
#define ADF4030_TDC_RSLT_UI_LSB			NO_OS_GENMASK(7, 0)

/* ADF4030 REG0074 Map */
#define ADF4030_TDC_RSLT_UI_MID			NO_OS_GENMASK(7, 0)

/* ADF4030 REG0075 Map */
#define ADF4030_TDC_RSLT_UI_MSB			NO_OS_GENMASK(7, 0)

/* ADF4030 REG008F Map */
#define ADF4030_REF_OK				NO_OS_BIT(6)
#define ADF4030_TDC_BUSY			NO_OS_BIT(4)
#define ADF4030_DL_BUSY				NO_OS_BIT(3)
#define ADF4030_MATH_BUSY			NO_OS_BIT(2)
#define ADF4030_ADC_BUSY			NO_OS_BIT(1)
#define ADF4030_FSM_BUSY			NO_OS_BIT(0)

/* ADF4030 REG0090 Map */
#define ADF4030_TEMP_MON			NO_OS_BIT(4)
#define ADF4030_TMP_ALIGN_ERR			NO_OS_BIT(3)
#define ADF4030_TDC_ERR				NO_OS_GENMASK(2, 1)
#define ADF4030_PLL_LD				NO_OS_BIT(0)

/* ADF4030 REG0092 Map */
#define ADF4030_TEMP_MEAS_LSB			NO_OS_GENMASK(7, 0)

/* ADF4030 REG0093 Map */
#define ADF4030_TEMP_MEAS_MSB			NO_OS_BIT(0)

#define ADF4030_SPI_4W_CFG(x)	(no_os_field_prep(ADF4030_SDO_ACTIVE_MSK, x) | \
					no_os_field_prep(ADF4030_SDO_ACTIVE_R_MSK, x))

#define ADF4030_SPI_LSB_CFG(x)	(no_os_field_prep(ADF4030_LSB_FIRST_MSK, x) | \
					no_os_field_prep(ADF4030_LSB_FIRST_R_MSK, x))

#define ADF4030_SPI_SCRATCHPAD_TEST		0x5A
#define ADF4030_CHANNEL_TX_PD_SEPARATOR		6
#define ADF4030_CHANNEL_DRV_SEPARATOR		8
#define ADF4030_CHANNEL_PRBS_SEPARATOR		6
#define ADF4030_CHANNEL_INV_SEPARATOR		4
#define ADF4030_POR_DELAY_US			200
#define ADF4030_RCM_CONST1			7000
#define ADF4030_RCM_CONST2			735
#define ADF4030_RCM_CONST3			10
#define ADF4030_RCM_CONST4			265
#define ADF4030_RCM_CURRENT0			14
#define ADF4030_RCM_CURRENT1			20
#define ADF4030_RCM_VOLTAGE_MIN0		504
#define ADF4030_RCM_VOLTAGE_MAX0		1304
#define ADF4030_RCM_VOLTAGE_MIN1		720
#define ADF4030_RCM_VOLTAGE_MAX1		1863

/* Specifications */
#define ADF4030_SPI_WRITE_CMD			0x0
#define ADF4030_SPI_READ_CMD			0x8000
#define ADF4030_CHIP_ADDRESS(x)			(x << 9)
#define ADF4030_SPI_DUMMY_DATA			0x00
#define ADF4030_BUFF_SIZE_BYTES			3
#define ADF4030_CHANNEL_NUMBER			10
#define ADF4030_ADC_CLK_DIV				64U

#define ADF4030_VCO_FREQ_MIN			2375000000U	// 2.375GHz
#define ADF4030_VCO_FREQ_MAX			2625000000U	// 2.625GHz
#define ADF4030_PFD_FREQ_MIN			10000000U	// 10MHz
#define ADF4030_PFD_FREQ_MAX			20000000U	// 20MHz

#define ADF4030_BSYNC_FREQ_MIN			650000U // 0.65MHz
#define ADF4030_BSYNC_FREQ_MAX			200000000U // 200MHz
#define ADF4030_REF_FREQ_MAX			250000000U // 250MHz
#define ADF4030_REF_FREQ_MIN			10000000 // 10MHz
#define ADF4030_REF_DIV_MAX			31
#define ADF4030_R_DIV_MIN			1U
#define ADF4030_R_DIV_MAX			31U
#define ADF4030_N_DIV_MIN			8U
#define ADF4030_N_DIV_MAX			255U
#define ADF4030_O_DIV_MIN			10U
#define ADF4030_O_DIV_MAX			4095U
#define ADF4030_ALIGN_CYCLES_MIN		(1U)
#define ADF4030_ALIGN_CYCLES_MAX		(8U)
#define ADF4030_CHIP_ADDRESS_MIN		0U
#define ADF4030_CHIP_ADDRESS_MAX		15U

enum adf4030_terminations_e {
	TX_VOLTAGE_DRIVER = 0,
	TX_CURRENT_DRIVER_UNTERMINATED,
	TX_CURRENT_DRIVER_TERMINATED,
	RX_DC_COUPLED_CLKS,
	RX_AC_COUPLED_CLKS,
	RX_DC_COUPLED_HCSL
};

struct adf4030_chan_spec {
	int64_t delay_fs;
	uint32_t rcm_mv;
	enum adf4030_terminations_e termination;
	bool invert_en;
	bool odivb_en;
	bool prbs_en;
	bool boost_en;
	bool tx_en;
};

/**
 * @struct adf4030_init_param
 * @brief ADF4030 Initialization Parameters structure.
 */
struct adf4030_dev {
	/** SPI Initialization parameters */
	struct no_os_spi_desc		*spi_desc;
	struct adf4030_chan_spec	channels[ADF4030_CHANNEL_NUMBER];
	uint32_t			ref_freq;
	uint32_t 			vco_freq;
	uint32_t			bsync_freq_odiv_a;
	uint32_t			bsync_freq_odiv_b;
	uint8_t				chip_addr;
	uint8_t				ref_div;
	uint8_t 			ndiv;
	uint8_t 			tdc_source;
	uint8_t 			tdc_target;
	bool 				tdc_status;
	bool				spi_4wire_en;
	bool				cmos_3v3;
};

/**
 * @struct adf4030_init_param
 * @brief ADF4030 Initialization Parameters structure.
 */
struct adf4030_init_param {
	/** SPI Initialization parameters */
	struct no_os_spi_init_param	*spi_init;
	uint32_t			ref_freq;
	uint32_t			vco_freq;
	uint32_t			bsync_freq;
	uint8_t				ref_div;
	uint8_t				chip_addr;
	bool				spi_4wire_en;
	bool				cmos_3v3;
};


int adf4030_spi_write(struct adf4030_dev *dev, uint16_t reg_addr, uint8_t data);

int adf4030_spi_read(struct adf4030_dev *dev, uint16_t reg_addr, uint8_t *data);

int adf4030_spi_update_bits(struct adf4030_dev *dev, uint16_t reg_addr,
			    uint8_t mask, uint8_t data);

int adf4030_set_default_regs(struct adf4030_dev *dev, bool spi_4wire);

int adf4030_set_temperature(struct adf4030_dev *dev, bool en);

int adf4030_get_temperature(struct adf4030_dev *dev, int16_t *temperature);

int adf4030_set_chip_address(struct adf4030_dev *dev, uint8_t addr);

int adf4030_set_ref_clk(struct adf4030_dev *dev, uint32_t val);

int adf4030_set_vco_freq(struct adf4030_dev *dev, uint32_t vco_freq);

int adf4030_get_vco_freq(struct adf4030_dev *dev, uint32_t *vco_freq);

int adf4030_set_bsync_freq(struct adf4030_dev *dev, uint32_t bsync_freq,
			   bool odivb_sel);

int adf4030_get_bsync_freq(struct adf4030_dev *dev, uint32_t *bsync_freq,
			   bool odivb_sel);

int adf4030_set_tdc_source(struct adf4030_dev *dev, uint8_t tdc_source);

int adf4030_get_tdc_source(struct adf4030_dev *dev, uint8_t *tdc_source);

int adf4030_set_tdc_measurement(struct adf4030_dev *dev, uint8_t tdc_target);

int adf4030_get_tdc_measurement(struct adf4030_dev *dev,
				int64_t *tdc_result_fs);

int adf4030_set_alignment_iter(struct adf4030_dev *dev, uint8_t iter_number);

int adf4030_get_alignment_iter(struct adf4030_dev *dev, uint8_t *iter_number);

int adf4030_set_alignment_threshold(struct adf4030_dev *dev,
				    uint32_t threshold_fs);

int adf4030_get_alignment_threshold(struct adf4030_dev *dev,
				    uint32_t *threshold_fs);

int adf4030_set_single_ch_alignment(struct adf4030_dev *dev,
				    uint8_t tdc_target_ch);

int adf4030_set_serial_alignment(struct adf4030_dev *dev,
				 uint16_t channel_flags);

int adf4030_get_serial_alignment(struct adf4030_dev *dev,
				 uint16_t *channel_flags);

int adf4030_set_background_serial_alignment(struct adf4030_dev *dev,
		uint16_t channel_flags);

int adf4030_get_background_serial_alignment(struct adf4030_dev *dev,
		uint16_t *channel_flags);

int adf4030_set_channel_delay(struct adf4030_dev *dev, uint8_t channel,
			      int64_t delay_fs);

int adf4030_get_channel_delay(struct adf4030_dev *dev, uint8_t channel,
			      int64_t *delay_fs);

int adf4030_set_channel_direction(struct adf4030_dev *dev, uint8_t channel,
				  bool tx_en);

int adf4030_get_channel_direction(struct adf4030_dev *dev, uint8_t channel,
				  bool *tx_en);

int adf4030_set_channel_termination(struct adf4030_dev *dev, uint8_t channel,
				    enum adf4030_terminations_e termination);

int adf4030_get_channel_termination(struct adf4030_dev *dev, uint8_t channel,
				    enum adf4030_terminations_e *termination);

int adf4030_set_channel_prbs(struct adf4030_dev *dev, uint8_t channel,
			     bool prbs_en);

int adf4030_get_channel_prbs(struct adf4030_dev *dev, uint8_t channel,
			     bool *prbs_en);

int adf4030_set_channel_odivb(struct adf4030_dev *dev, uint8_t channel,
			      bool odivb_en);

int adf4030_get_channel_odivb(struct adf4030_dev *dev, uint8_t channel,
			      bool *odivb_en);

int adf4030_set_channel_invert(struct adf4030_dev *dev, uint8_t channel,
			       bool invert_en);

int adf4030_get_channel_invert(struct adf4030_dev *dev, uint8_t channel,
			       bool *invert_en);

int adf4030_set_channel_voltage(struct adf4030_dev *dev, uint8_t channel,
				uint32_t voltage_mv);

int adf4030_get_channel_voltage(struct adf4030_dev *dev, uint8_t channel,
				uint32_t *voltage_mv);

int adf4030_init(struct adf4030_dev **dev,
		 struct adf4030_init_param *init_param);

int adf4030_remove(struct adf4030_dev *dev);

#endif /* ADF4030_H_ */
