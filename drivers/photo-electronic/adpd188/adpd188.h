/***************************************************************************//**
 *   @file   adpd188.h
 *   @brief  ADPD188 driver header file.
 *   @author Andrei Drimbarean (Andrei.Drimbarean@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

#ifndef ADPD188_H_
#define ADPD188_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "no_os_i2c.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define ADPD188_REG_STATUS		0x00
#define ADPD188_REG_INT_MASK		0x01
#define ADPD188_REG_GPIO_DRV		0x02
#define ADPD188_REG_BG_STATUS		0x04
#define ADPD188_REG_FIFO_THRESH		0x06
#define ADPD188_REG_DEVID		0x08
#define ADPD188_REG_I2CS_ID		0x09
#define ADPD188_REG_CLK_RATIO		0x0A
#define ADPD188_REG_GPIO_CTRL		0x0B
#define ADPD188_REG_SLAVE_ADDR_KEY	0x0D
#define ADPD188_REG_SW_RESET		0x0F
#define ADPD188_REG_MODE		0x10
#define ADPD188_REG_SLOT_EN		0x11
#define ADPD188_REG_FSAMPLE		0x12
#define ADPD188_REG_PD_LED_SELECT	0x14
#define ADPD188_REG_NUM_AVG		0x15
#define ADPD188_REG_BG_MEAS_A		0x16
#define ADPD188_REG_INT_SEQ_A		0x17
#define ADPD188_REG_SLOTA_CH1_OFFSET	0x18
#define ADPD188_REG_SLOTA_CH2_OFFSET	0x19
#define ADPD188_REG_SLOTA_CH3_OFFSET	0x1A
#define ADPD188_REG_SLOTA_CH4_OFFSET	0x1B
#define ADPD188_REG_BG_MEAS_B		0x1C
#define ADPD188_REG_INT_SEQ_B		0x1D
#define ADPD188_REG_SLOTB_CH1_OFFSET	0x1E
#define ADPD188_REG_SLOTB_CH2_OFFSET	0x1F
#define ADPD188_REG_SLOTB_CH3_OFFSET	0x20
#define ADPD188_REG_SLOTB_CH4_OFFSET	0x21
#define ADPD188_REG_ILED3_COARSE	0x22
#define ADPD188_REG_ILED1_COARSE	0x23
#define ADPD188_REG_ILED2_COARSE	0x24
#define ADPD188_REG_ILED_FINE		0x25
#define ADPD188_REG_SLOTA_LED_PULSE	0x30
#define ADPD188_REG_SLOTA_NUMPULSES	0x31
#define ADPD188_REG_LED_DISABLE		0x34
#define ADPD188_REG_SLOTB_LED_PULSE	0x35
#define ADPD188_REG_SLOTB_NUMPULSES	0x36
#define ADPD188_REG_ALT_PWR_DN		0x37
#define ADPD188_REG_EXT_SYNC_STARTUP	0x38
#define ADPD188_REG_SLOTA_AFE_WINDOW	0x39
#define ADPD188_REG_SLOTB_AFE_WINDOW	0x3B
#define ADPD188_REG_AFE_PWR_CFG1	0x3C
#define ADPD188_REG_SLOTA_FLOAT_LED	0x3E
#define ADPD188_REG_SLOTB_FLOAT_LED	0x3F
#define ADPD188_REG_SLOTA_TIA_CFG	0x42
#define ADPD188_REG_SLOTA_AFE_CFG	0x43
#define ADPD188_REG_SLOTB_TIA_CFG	0x44
#define ADPD188_REG_SLOTB_AFE_CFG	0x45
#define ADPD188_REG_SAMPLE_CLK		0x4B
#define ADPD188_REG_CLK32M_ADJUST	0x4D
#define ADPD188_REG_EXT_SYNC_SEL	0x4F
#define ADPD188_REG_CLK32M_CAL_EN	0x50
#define ADPD188_REG_AFE_PWR_CFG2	0x54
#define ADPD188_REG_TIA_INDEP_GAIN	0x55
#define ADPD188_REG_MATH		0x58
#define ADPD188_REG_FLT_CONFIG_B	0x59
#define ADPD188_REG_FLT_LED_FIRE	0x5A
#define ADPD188_REG_FLT_CONFIG_A	0x5E
#define ADPD188_REG_DATA_ACCESS_CTL	0x5F
#define ADPD188_REG_FIFO_ACCESS		0x60
#define ADPD188_REG_SLOTA_PD1_16BIT	0x64
#define ADPD188_REG_SLOTA_PD2_16BIT	0x65
#define ADPD188_REG_SLOTA_PD3_16BIT	0x66
#define ADPD188_REG_SLOTA_PD4_16BIT	0x67
#define ADPD188_REG_SLOTB_PD1_16BIT	0x68
#define ADPD188_REG_SLOTB_PD2_16BIT	0x69
#define ADPD188_REG_SLOTB_PD3_16BIT	0x6A
#define ADPD188_REG_SLOTB_PD4_16BIT	0x6B
#define ADPD188_REG_A_PD1_LOW		0x70
#define ADPD188_REG_A_PD2_LOW		0x71
#define ADPD188_REG_A_PD3_LOW		0x72
#define ADPD188_REG_A_PD4_LOW		0x73
#define ADPD188_REG_A_PD1_HIGH		0x74
#define ADPD188_REG_A_PD2_HIGH		0x75
#define ADPD188_REG_A_PD3_HIGH		0x76
#define ADPD188_REG_A_PD4_HIGH		0x77
#define ADPD188_REG_B_PD1_LOW		0x78
#define ADPD188_REG_B_PD2_LOW		0x79
#define ADPD188_REG_B_PD3_LOW		0x7A
#define ADPD188_REG_B_PD4_LOW		0x7B
#define ADPD188_REG_B_PD1_HIGH		0x7C
#define ADPD188_REG_B_PD2_HIGH		0x7D
#define ADPD188_REG_B_PD3_HIGH		0x7E
#define ADPD188_REG_B_PD4_HIGH		0x7F

/* ADPD188_REG_STATUS */
#define ADPD188_STATUS_FIFO_SAMPLES_MASK	0xFF00
#define ADPD188_STATUS_SLOTB_INT_MASK		0x0040
#define ADPD188_STATUS_SLOTA_INT_MASK		0x0020
#define ADPD188_STATUS_FIFO_SAMPLES_POS		8
#define ADPD188_STATUS_SLOTB_INT_POS		6
#define ADPD188_STATUS_SLOTA_INT_POS		5

/* ADPD188_REG_INT_MASK */
#define ADPD188_INT_MASK_FIFO_INT_MASK_MASK	0x0100
#define ADPD188_INT_MASK_SLOTB_INT_MASK_MASK	0x0040
#define ADPD188_INT_MASK_SLOTA_INT_MASK_MASK	0x0020
#define ADPD188_INT_MASK_FIFO_INT_MASK_POS	8
#define ADPD188_INT_MASK_SLOTB_INT_MASK_POS	6
#define ADPD188_INT_MASK_SLOTA_INT_MASK_POS	5

/* ADPD188_REG_GPIO_DRV */
#define ADPD188_GPIO_DRV_GPIO1_DRV_MASK	0x0200
#define ADPD188_GPIO_DRV_GPIO1_POL_MASK	0x0100
#define ADPD188_GPIO_DRV_GPIO0_ENA_MASK	0x0004
#define ADPD188_GPIO_DRV_GPIO0_DRV_MASK	0x0002
#define ADPD188_GPIO_DRV_GPIO0_POL_MASK	0x0001
#define ADPD188_GPIO_DRV_GPIO1_DRV_POS	9
#define ADPD188_GPIO_DRV_GPIO1_POL_POS	8
#define ADPD188_GPIO_DRV_GPIO0_ENA_POS	2
#define ADPD188_GPIO_DRV_GPIO0_DRV_POS	1
#define ADPD188_GPIO_DRV_GPIO0_POL_POS	0

/* ADPD188_REG_BG_STATUS */
#define ADPD188_BG_STATUS_BG_STATUS_B_MASK	0x00F0
#define ADPD188_BG_STATUS_BG_STATUS_A_MASK	0x000F
#define ADPD188_BG_STATUS_BG_STATUS_B_POS	4
#define ADPD188_BG_STATUS_BG_STATUS_A_POS	0

/* ADPD188_REG_FIFO_THRESH */
#define ADPD188_FIFO_THRESH_FIFO_THRESH_MASK	0x3F00
#define ADPD188_FIFO_THRESH_FIFO_THRESH_POS	8
#define ADPD188_FIFO_THRESH_MAX_THRESHOLD	63

/* ADPD188_REG_DEVID */
#define ADPD188_DEVID_REV_NUM_MASK	0xFF00
#define ADPD188_DEVID_DEV_ID_MASK	0x00FF
#define ADPD188_DEVID_REV_NUM_POS	8
#define ADPD188_DEVID_DEV_ID_POS	0
/* ADPD188BI specific */
#define ADPD188_DEVICE_ID		0x0916
/* ADPD108X specific */
#define ADPD108X_DEVICE_ID		0x0A16

/* ADPD188_REG_I2CS_ID */
#define ADPD188_I2CS_ID_ADDRESS_WRITE_KEY_MASK	0xFF00
#define ADPD188_I2CS_ID_SLAVE_ADDRESS_MASK	0x00FE
#define ADPD188_I2CS_ID_ADDRESS_WRITE_KEY_POS	8
#define ADPD188_I2CS_ID_SLAVE_ADDRESS_POS	1

/* ADPD188_REG_CLK_RATIO */
#define ADPD188_CLK_RATIO_CLK_RATIO_MASK	0x0FFF
#define ADPD188_CLK_RATIO_CLK_RATIO_POS		0

/* ADPD188_REG_GPIO_CTRL */
#define ADPD188_GPIO_CTRL_GPIO1_ALT_CFG_MASK	0x1F00
#define ADPD188_GPIO_CTRL_GPIO0_ALT_CFG_MASK	0x001F
#define ADPD188_GPIO_CTRL_GPIO1_ALT_CFG_POS	8
#define ADPD188_GPIO_CTRL_GPIO0_ALT_CFG_POS	0

/* ADPD188_REG_SW_RESET */
#define ADPD188_SW_RESET_SW_RESET_MASK	0x0001
#define ADPD188_SW_RESET_SW_RESET_POS	0

/* ADPD188_REG_MODE */
#define ADPD188_MODE_MODE_MASK	0x0003
#define ADPD188_MODE_MODE_POS	0

/* ADPD188_REG_SLOT_EN */
#define ADPD188_SLOT_EN_RDOUT_MODE_MASK		0x2000
#define ADPD188_SLOT_EN_FIFO_OVRN_PREVENT_MASK	0x1000
#define ADPD188_SLOT_EN_SLOTB_FIFO_MODE_MASK	0x01C0
#define ADPD188_SLOT_EN_SLOTB_EN_MASK		0x0020
#define ADPD188_SLOT_EN_SLOTA_FIFO_MODE_MASK	0x001C
#define ADPD188_SLOT_EN_SLOTA_EN_MASK		0x0001
#define ADPD188_SLOT_EN_RDOUT_MODE_POS		13
#define ADPD188_SLOT_EN_FIFO_OVRN_PREVENT_POS	12
#define ADPD188_SLOT_EN_SLOTB_FIFO_MODE_POS	6
#define ADPD188_SLOT_EN_SLOTB_EN_POS		5
#define ADPD188_SLOT_EN_SLOTA_FIFO_MODE_POS	2
#define ADPD188_SLOT_EN_SLOTA_EN_POS		0

/* ADPD188_REG_PD_LED_SELECT */
#define ADPD188_PD_LED_SELECT_SLOTB_PD_SEL_MASK		0x0F00
#define ADPD188_PD_LED_SELECT_SLOTA_PD_SEL_MASK		0x00F0
#define ADPD188_PD_LED_SELECT_SLOTB_LED_SEL_MASK	0x000C
#define ADPD188_PD_LED_SELECT_SLOTA_LED_SEL_MASK	0x0003
#define ADPD188_PD_LED_SELECT_SLOTB_PD_SEL_POS		8
#define ADPD188_PD_LED_SELECT_SLOTA_PD_SEL_POS		4
#define ADPD188_PD_LED_SELECT_SLOTB_LED_SEL_POS		2
#define ADPD188_PD_LED_SELECT_SLOTA_LED_SEL_POS		0

/* ADPD188_REG_NUM_AVG */
#define ADPD188_NUM_AVG_SLOTB_NUM_AVG_MASK	0x0700
#define ADPD188_NUM_AVG_SLOTA_NUM_AVG_MASK	0x0070
#define ADPD188_NUM_AVG_SLOTB_NUM_AVG_POS	8
#define ADPD188_NUM_AVG_SLOTA_NUM_AVG_POS	4

/* ADPD188_REG_BG_MEAS_A */
#define ADPD188_BG_MEAS_A_BG_COUNT_A_MASK	0xC000
#define ADPD188_BG_MEAS_A_BG_THRESH_A_MASK	0x3FFF
#define ADPD188_BG_MEAS_A_BG_COUNT_A_POS	14
#define ADPD188_BG_MEAS_A_BG_THRESH_A_POS	0

/* ADPD188_REG_INT_SEQ_A */
#define ADPD188_INT_SEQ_A_INTEG_ORDER_A_MASK	0x000F
#define ADPD188_INT_SEQ_A_INTEG_ORDER_A_POS	0

/* ADPD188_REG_BG_MEAS_B */
#define ADPD188_BG_MEAS_B_BG_COUNT_B_MASK	0xC000
#define ADPD188_BG_MEAS_B_BG_THRESH_B_MASK	0x3FFF
#define ADPD188_BG_MEAS_B_BG_COUNT_B_POS	14
#define ADPD188_BG_MEAS_B_BG_THRESH_B_POS	0

/* ADPD188_REG_INT_SEQ_B */
#define ADPD188_INT_SEQ_B_INTEG_ORDER_B_MASK	0x000F
#define ADPD188_INT_SEQ_B_INTEG_ORDER_B_POS	0

/* ADPD188_REG_ILED3_COARSE */
#define ADPD188_ILED3_COARSE_ILED3_SCALE_MASK	0x2000
#define ADPD188_ILED3_COARSE_ILED3_SLEW_MASK	0x0070
#define ADPD188_ILED3_COARSE_ILED3_COARSE_MASK	0x000F
#define ADPD188_ILED3_COARSE_ILED3_SCALE_POS	13
#define ADPD188_ILED3_COARSE_ILED3_SLEW_POS	4
#define ADPD188_ILED3_COARSE_ILED3_COARSE_POS	0

/* ADPD188_REG_ILED1_COARSE */
#define ADPD188_ILED1_COARSE_ILED1_SCALE_MASK	0x2000
#define ADPD188_ILED1_COARSE_ILED1_SLEW_MASK	0x0070
#define ADPD188_ILED1_COARSE_ILED1_COARSE_MASK	0x000F
#define ADPD188_ILED1_COARSE_ILED1_SCALE_POS	13
#define ADPD188_ILED1_COARSE_ILED1_SLEW_POS	4
#define ADPD188_ILED1_COARSE_ILED1_COARSE_POS	0

/* ADPD188_REG_ILED2_COARSE */
#define ADPD188_ILED2_COARSE_ILED2_SCALE_MASK	0x2000
#define ADPD188_ILED2_COARSE_ILED2_SLEW_MASK	0x0070
#define ADPD188_ILED2_COARSE_ILED2_COARSE_MASK	0x000F
#define ADPD188_ILED2_COARSE_ILED2_SCALE_POS	13
#define ADPD188_ILED2_COARSE_ILED2_SLEW_POS	4
#define ADPD188_ILED2_COARSE_ILED2_COARSE_POS	0

/* ADPD188_REG_ILED_FINE */
#define ADPD188_ILED_FINE_ILED3_FINE_MASK	0xF800
#define ADPD188_ILED_FINE_ILED2_FINE_MASK	0x07C0
#define ADPD188_ILED_FINE_ILED1_FINE_MASK	0x001F
#define ADPD188_ILED_FINE_ILED3_FINE_POS	11
#define ADPD188_ILED_FINE_ILED2_FINE_POS	6
#define ADPD188_ILED_FINE_ILED1_FINE_POS	0

/* ADPD188_REG_SLOTA_LED_PULSE */
#define ADPD188_SLOTA_LED_PULSE_SLOTA_LED_WIDTH_MASK	0x1F00
#define ADPD188_SLOTA_LED_PULSE_SLOTA_LED_OFFSET_MASK	0x00FF
#define ADPD188_SLOTA_LED_PULSE_SLOTA_LED_WIDTH_POS	8
#define ADPD188_SLOTA_LED_PULSE_SLOTA_LED_OFFSET_POS	0

/* ADPD188_REG_SLOTA_NUMPULSES */
#define ADPD188_SLOTA_NUMPULSES_SLOTA_PULSES_MASK	0xFF00
#define ADPD188_SLOTA_NUMPULSES_SLOTA_PERIOD_MASK	0x00FF
#define ADPD188_SLOTA_NUMPULSES_SLOTA_PULSES_POS	8
#define ADPD188_SLOTA_NUMPULSES_SLOTA_PERIOD_POS	0

/* ADPD188_REG_LED_DISABLE */
#define ADPD188_LED_DISABLE_SLOTB_LED_DIS_MASK	0x0200
#define ADPD188_LED_DISABLE_SLOTA_LED_DIS_MASK	0x0100
#define ADPD188_LED_DISABLE_SLOTB_LED_DIS_POS	9
#define ADPD188_LED_DISABLE_SLOTA_LED_DIS_POS	8

/* ADPD188_REG_SLOTB_LED_PULSE */
#define ADPD188_SLOTB_LED_PULSE_SLOTB_LED_WIDTH_MASK	0x1F00
#define ADPD188_SLOTB_LED_PULSE_SLOTB_LED_OFFSET_MASK	0x00FF
#define ADPD188_SLOTB_LED_PULSE_SLOTB_LED_WIDTH_POS	8
#define ADPD188_SLOTB_LED_PULSE_SLOTB_LED_OFFSET_POS	0

/* ADPD188_REG_SLOTB_NUMPULSES */
#define ADPD188_SLOTB_NUMPULSES_SLOTB_PULSES_MASK	0xFF00
#define ADPD188_SLOTB_NUMPULSES_SLOTB_PERIOD_MASK	0x00FF
#define ADPD188_SLOTB_NUMPULSES_SLOTB_PULSES_POS	8
#define ADPD188_SLOTB_NUMPULSES_SLOTB_PERIOD_POS	0

/* ADPD188_REG_ALT_PWR_DN */
#define ADPD188_ALT_PWR_DN_CH34_DISABLE_MASK	0xE000
#define ADPD188_ALT_PWR_DN_CH2_DISABLE_MASK	0x1C00
#define ADPD188_ALT_PWR_DN_SLOTB_PERIOD_MASK	0x0300
#define ADPD188_ALT_PWR_DN_SLOTA_PERIOD_MASK	0x0003
#define ADPD188_ALT_PWR_DN_CH34_DISABLE_POS	13
#define ADPD188_ALT_PWR_DN_CH2_DISABLE_POS	10
#define ADPD188_ALT_PWR_DN_SLOTB_PERIOD_POS	8
#define ADPD188_ALT_PWR_DN_SLOTA_PERIOD_POS	0

/* ADPD188_REG_SLOTA_AFE_WINDOW */
#define ADPD188_SLOTA_AFE_WINDOW_SLOTA_AFE_WIDTH_MASK	0xF800
#define ADPD188_SLOTA_AFE_WINDOW_SLOTA_AFE_OFFSET_MASK	0x07FF
#define ADPD188_SLOTA_AFE_WINDOW_SLOTA_AFE_WIDTH_POS	11
#define ADPD188_SLOTA_AFE_WINDOW_SLOTA_AFE_OFFSET_POS	0

/* ADPD188_REG_SLOTB_AFE_WINDOW */
#define ADPD188_SLOTB_AFE_WINDOW_SLOTB_AFE_WIDTH_MASK	0xF800
#define ADPD188_SLOTB_AFE_WINDOW_SLOTB_AFE_OFFSET_MASK	0x07FF
#define ADPD188_SLOTB_AFE_WINDOW_SLOTB_AFE_WIDTH_POS	11
#define ADPD188_SLOTB_AFE_WINDOW_SLOTB_AFE_OFFSET_POS	0

/* ADPD188_REG_AFE_PWR_CFG1 */
#define ADPD188_AFE_PWR_CFG1_V_CATHODE_MASK	0x0200
#define ADPD188_AFE_PWR_CFG1_AFE_POWERDOWN_MASK	0x01F8
#define ADPD188_AFE_PWR_CFG1_V_CATHODE_POS	9
#define ADPD188_AFE_PWR_CFG1_AFE_POWERDOWN_POS	3

/* ADPD188_REG_SLOTA_FLOAT_LED */
#define ADPD188_SLOTA_FLOAT_LED_FLT_LED_SELECT_A_MASK	0xC000
#define ADPD188_SLOTA_FLOAT_LED_FLT_LED_WIDTH_A_MASK	0x1F00
#define ADPD188_SLOTA_FLOAT_LED_FLT_LED_OFFSET_A_MASK	0x00FF
#define ADPD188_SLOTA_FLOAT_LED_FLT_LED_SELECT_A_POS	14
#define ADPD188_SLOTA_FLOAT_LED_FLT_LED_WIDTH_A_POS	8
#define ADPD188_SLOTA_FLOAT_LED_FLT_LED_OFFSET_A_POS	0

/* ADPD188_REG_SLOTB_FLOAT_LED */
#define ADPD188_SLOTB_FLOAT_LED_FLT_LED_SELECT_B_MASK	0xC000
#define ADPD188_SLOTB_FLOAT_LED_FLT_LED_WIDTH_B_MASK	0x1F00
#define ADPD188_SLOTB_FLOAT_LED_FLT_LED_OFFSET_B_MASK	0x00FF
#define ADPD188_SLOTB_FLOAT_LED_FLT_LED_SELECT_B_POS	14
#define ADPD188_SLOTB_FLOAT_LED_FLT_LED_WIDTH_B_POS	8
#define ADPD188_SLOTB_FLOAT_LED_FLT_LED_OFFSET_B_POS	0

/* ADPD188_REG_SLOTA_TIA_CFG */
/* ADPD188BI specific registers */
#define ADPD188_SLOTA_TIA_CFG_SLOTA_BUF_GAIN_MASK	0x0200
#define ADPD188_SLOTA_TIA_CFG_SLOTA_BUF_GAIN_POS	9
/* ADPD108X specific registers */
#define ADPD108X_SLOTA_TIA_CFG_SLOTA_BUF_GAIN_MASK	0x0300
#define ADPD108X_SLOTA_TIA_CFG_SLOTA_INT_GAIN_POS	8
/* ADPD188_REG_SLOTA_TIA_CFG common registers */
#define ADPD188_SLOTA_TIA_CFG_SLOTA_AFE_MODE_MASK	0xFC00
#define ADPD188_SLOTA_TIA_CFG_SLOTA_INT_AS_BUF_MASK	0x0080
#define ADPD188_SLOTA_TIA_CFG_SLOTA_TIA_IND_EN_MASK	0x0040
#define ADPD188_SLOTA_TIA_CFG_SLOTA_TIA_VREF_MASK	0x0030
#define ADPD188_SLOTA_TIA_CFG_SLOTA_TIA_GAIN_MASK	0x0003
#define ADPD188_SLOTA_TIA_CFG_SLOTA_AFE_MODE_POS	10
#define ADPD188_SLOTA_TIA_CFG_SLOTA_INT_AS_BUF_POS	7
#define ADPD188_SLOTA_TIA_CFG_SLOTA_TIA_IND_EN_POS	6
#define ADPD188_SLOTA_TIA_CFG_SLOTA_TIA_VREF_POS	4
#define ADPD188_SLOTA_TIA_CFG_SLOTA_TIA_GAIN_POS	0

/* ADPD188_REG_SLOTB_TIA_CFG */
/* ADPD188BI specific registers */
#define ADPD188_SLOTB_TIA_CFG_SLOTA_BUF_GAIN_MASK	0x0200
#define ADPD188_SLOTB_TIA_CFG_SLOTA_BUF_GAIN_POS	9
/* ADPD108X specific registers */
#define ADPD108X_SLOTB_TIA_CFG_SLOTA_BUF_GAIN_MASK	0x0300
#define ADPD108X_SLOTB_TIA_CFG_SLOTA_INT_GAIN_POS	8
/* ADPD188_REG_SLOTA_TIA_CFG common registers */
#define ADPD188_SLOTB_TIA_CFG_SLOTB_AFE_MODE_MASK	0xFC00
#define ADPD188_SLOTB_TIA_CFG_SLOTB_INT_AS_BUF_MASK	0x0080
#define ADPD188_SLOTB_TIA_CFG_SLOTB_TIA_IND_EN_MASK	0x0040
#define ADPD188_SLOTB_TIA_CFG_SLOTB_TIA_VREF_MASK	0x0030
#define ADPD188_SLOTB_TIA_CFG_SLOTB_TIA_GAIN_MASK	0x0003
#define ADPD188_SLOTB_TIA_CFG_SLOTB_AFE_MODE_POS	10
#define ADPD188_SLOTB_TIA_CFG_SLOTB_INT_AS_BUF_POS	7
#define ADPD188_SLOTB_TIA_CFG_SLOTB_TIA_IND_EN_POS	6
#define ADPD188_SLOTB_TIA_CFG_SLOTB_TIA_VREF_POS	4
#define ADPD188_SLOTB_TIA_CFG_SLOTB_TIA_GAIN_POS	0

/* ADPD188_REG_SAMPLE_CLK */
#define ADPD188_SAMPLE_CLK_CLK32K_BYP_MASK	0x0100
#define ADPD188_SAMPLE_CLK_CLK32K_EN_MASK	0x0080
#define ADPD188_SAMPLE_CLK_CLK32K_ADJUST_MASK	0x003F
#define ADPD188_SAMPLE_CLK_CLK32K_BYP_POS	8
#define ADPD188_SAMPLE_CLK_CLK32K_EN_POS	7
#define ADPD188_SAMPLE_CLK_CLK32K_ADJUST_POS	0

/* ADPD188_REG_CLK32M_ADJUST */
#define ADPD188_CLK32M_ADJUST_CLK32M_ADJUST_MASK	0x00FF
#define ADPD188_CLK32M_ADJUST_CLK32M_ADJUST_POS		0

/* ADPD188_REG_EXT_SYNC_SEL */
#define ADPD188_EXT_SYNC_SEL_GPIO1_OE_MASK	0x0040
#define ADPD188_EXT_SYNC_SEL_GPIO1_IE_MASK	0x0020
#define ADPD188_EXT_SYNC_SEL_EXT_SYNC_SEL_MASK	0x000C
#define ADPD188_EXT_SYNC_SEL_GPIO0_IE_MASK	0x0002
#define ADPD188_EXT_SYNC_SEL_GPIO1_OE_POS	6
#define ADPD188_EXT_SYNC_SEL_GPIO1_IE_POS	5
#define ADPD188_EXT_SYNC_SEL_EXT_SYNC_SEL_POS	2
#define ADPD188_EXT_SYNC_SEL_GPIO0_IE_POS	1

/* ADPD188_REG_CLK32M_CAL_EN */
#define ADPD188_CLK32M_CAL_EN_GPIO1_CTRL_MASK		0x0040
#define ADPD188_CLK32M_CAL_EN_CLK32M_CAL_EN_MASK	0x0020
#define ADPD188_CLK32M_CAL_EN_GPIO1_CTRL_POS		6
#define ADPD188_CLK32M_CAL_EN_CLK32M_CAL_EN_POS		5

/* ADPD188_REG_AFE_PWR_CFG2 */
#define ADPD188_AFE_PWR_CFG2_SLEEP_V_CATHODE_MASK	0x3000
#define ADPD188_AFE_PWR_CFG2_SLOTB_V_CATHODE_MASK	0x0C00
#define ADPD188_AFE_PWR_CFG2_SLOTA_V_CATHODE_MASK	0x0300
#define ADPD188_AFE_PWR_CFG2_REG54_VCAT_ENABLE_MASK	0x0080
#define ADPD188_AFE_PWR_CFG2_SLEEP_V_CATHODE_POS	12
#define ADPD188_AFE_PWR_CFG2_SLOTB_V_CATHODE_POS	10
#define ADPD188_AFE_PWR_CFG2_SLOTA_V_CATHODE_POS	8
#define ADPD188_AFE_PWR_CFG2_REG54_VCAT_ENABLE_POS	7

/* ADPD188_REG_TIA_INDEP_GAIN */
#define ADPD188_TIA_INDEP_GAIN_SLOTB_TIA_GAIN_4_MASK	0x0C00
#define ADPD188_TIA_INDEP_GAIN_SLOTB_TIA_GAIN_3_MASK	0x0300
#define ADPD188_TIA_INDEP_GAIN_SLOTB_TIA_GAIN_2_MASK	0x00C0
#define ADPD188_TIA_INDEP_GAIN_SLOTA_TIA_GAIN_4_MASK	0x0030
#define ADPD188_TIA_INDEP_GAIN_SLOTA_TIA_GAIN_3_MASK	0x000C
#define ADPD188_TIA_INDEP_GAIN_SLOTA_TIA_GAIN_2_MASK	0x0003
#define ADPD188_TIA_INDEP_GAIN_SLOTB_TIA_GAIN_4_POS	10
#define ADPD188_TIA_INDEP_GAIN_SLOTB_TIA_GAIN_3_POS	8
#define ADPD188_TIA_INDEP_GAIN_SLOTB_TIA_GAIN_2_POS	6
#define ADPD188_TIA_INDEP_GAIN_SLOTA_TIA_GAIN_4_POS	4
#define ADPD188_TIA_INDEP_GAIN_SLOTA_TIA_GAIN_3_POS	2
#define ADPD188_TIA_INDEP_GAIN_SLOTA_TIA_GAIN_2_POS	0

/* ADPD188_REG_MATH */
#define ADPD188_MATH_FLT_MATH34_B_MASK		0x0C00
#define ADPD188_MATH_FLT_MATH34_A_MASK		0x0300
#define ADPD188_MATH_ENA_INT_AS_BUF_MASK	0x0080
#define ADPD188_MATH_FLT_MATH12_B_MASK		0x0060
#define ADPD188_MATH_FLT_MATH12_A_MASK		0x0006
#define ADPD188_MATH_FLT_MATH34_B_POS		10
#define ADPD188_MATH_FLT_MATH34_A_POS		8
#define ADPD188_MATH_ENA_INT_AS_BUF_POS		7
#define ADPD188_MATH_FLT_MATH12_B_POS		5
#define ADPD188_MATH_FLT_MATH12_A_POS		1

/* ADPD188_REG_FLT_CONFIG_B */
#define ADPD188_FLT_CONFIG_B_FLT_EN_B_MASK	0x6000
#define ADPD188_FLT_CONFIG_B_FLT_PRECON_B_MASK	0x1F00
#define ADPD188_FLT_CONFIG_B_FLT_EN_B_POS	13
#define ADPD188_FLT_CONFIG_B_FLT_PRECON_B_POS	8

/* ADPD188_REG_FLT_LED_FIRE */
#define ADPD188_FLT_LED_FIRE_FLT_LED_FIRE_B_MASK	0xF000
#define ADPD188_FLT_LED_FIRE_FLT_LED_FIRE_A_MASK	0x0F00
#define ADPD188_FLT_LED_FIRE_FLT_LED_FIRE_B_POS		12
#define ADPD188_FLT_LED_FIRE_FLT_LED_FIRE_A_POS		8

/* ADPD188_REG_FLT_CONFIG_A */
#define ADPD188_FLT_CONFIG_A_FLT_EN_A_MASK	0x6000
#define ADPD188_FLT_CONFIG_A_FLT_PRECON_A_MASK	0x1F00
#define ADPD188_FLT_CONFIG_A_FLT_EN_A_POS	13
#define ADPD188_FLT_CONFIG_A_FLT_PRECON_A_POS	8

/* ADPD188_REG_DATA_ACCESS_CTL */
#define ADPD188_DATA_ACCESS_CTL_SLOTB_DATA_HOLD_MASK	0x0004
#define ADPD188_DATA_ACCESS_CTL_SLOTA_DATA_HOLD_MASK	0x0002
#define ADPD188_DATA_ACCESS_CTL_DIGITAL_CLOCK_ENA_MASK	0x0001
#define ADPD188_DATA_ACCESS_CTL_SLOTB_DATA_HOLD_POS	2
#define ADPD188_DATA_ACCESS_CTL_SLOTA_DATA_HOLD_POS	1
#define ADPD188_DATA_ACCESS_CTL_DIGITAL_CLOCK_ENA_POS	0

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum adpd_supported_devices
 * @brief Devices supported by the driver.
 */
enum adpd_supported_devices {
	ADPD188BI,
	ADPD1080,
	ADPD1081
};

/**
 * @union adpd188_phy_init
 * @brief Communication physical protocol initialization structure. Can be I2C
 *        or SPI.
 */
union adpd188_phy_init {
	/** I2C initialization structure. */
	struct no_os_i2c_init_param i2c_phy;
	/** SPI initialization structure. */
	struct no_os_spi_init_param spi_phy;
};

/**
 * @enum adpd188_phy_opt
 * @brief Types of physical communication protocol.
 */
enum adpd188_phy_opt {
	/** SPI communication. */
	ADPD188_SPI,
	/** I2C communication. */
	ADPD188_I2C
};

/**
 * @enum adpd188_mode
 * @brief ADPD188 operation modes.
 */
enum adpd188_mode {
	/** Standby mode. */
	ADPD188_STANDBY,
	/** Program mode. */
	ADPD188_PROGRAM,
	/** Normal mode. */
	ADPD188_NORMAL
};

/**
 * @enum adpd188_interrupt
 * @brief Interrupt flags of the ADPD188.
 */
enum adpd188_interrupt {
	/** Slot A conversion interrupt flag. */
	ADPD188_SLOTA_INT = 0x1,
	/** Slot B conversion interrupt flag. */
	ADPD188_SLOTB_INT = 0x2,
	/** FIFO threshold reached interrupt flag. */
	ADPD188_FIFO_INT = 0x4
};

/**
 * @struct adpd188_gpio_config
 * @brief GPIO level configuration.
 */
struct adpd188_gpio_config {
	/** GPIO ID (0 or 1) */
	uint8_t gpio_id;
	/** GPIO polarity */
	uint8_t gpio_pol;
	/** Status of the GPIO driver (driven or open-drain) */
	uint8_t gpio_drv;
	/** GPIO enable (only for GPIO0) */
	uint8_t gpio_en;
};

/**
 * @enum adpd188_gpio_alt_config
 * @brief GPIO source configuration.
 */
enum adpd188_gpio_alt_config {
	/** GPIO backwards compatible with the ADPD103 INT functionality. */
	ADPD188_ADPD103 = 0x00,
	/** Interrupt function provided on GPIO. */
	ADPD188_INT_FUNC = 0x01,
	/**
	 * Asserts at the start of the first time slot and deasserts at end of last
	 * time slot.
	 */
	ADPD188_ACTIVE_PULSE = 0x02,
	/** Time Slot A pulse output. */
	ADPD188_SLOTA_PULSE = 0x05,
	/** Time Slot B pulse output. */
	ADPD188_SLOTB_PULSE = 0x06,
	/** Pulse output of both time slots. */
	ADPD188_ANYSLOT_PULSE = 0x07,
	/** Output data cycle occurred for Time Slot A. */
	ADPD188_SLOTA_OUT = 0x0C,
	/** Output data cycle occurred for Time Slot B. */
	ADPD188_SLOTB_OUT = 0x0D,
	/** Output data cycle occurred. */
	ADPD188_ANYSLOT_OUT = 0x0E,
	/**
	 * Toggles on every sample, which provides a signal at half the sampling
	 * rate.
	 */
	ADPD188_HALF_SAMPLING = 0x0F,
	/** Output = 0. */
	ADPD188_OUT_LOW = 0x10,
	/** Output = 1. */
	ADPD188_OUT_HIGH = 0x11,
	/** 32 kHz oscillator output. */
	ADPD188_32KHZ_OSC = 0x13
};

/**
 * @enum adpd188_slots
 * @brief ADPD188 time slots.
 */
enum adpd188_slots {
	/** First slot. */
	ADPD188_SLOTA,
	/** Second slot. */
	ADPD188_SLOTB
};

/**
 * @enum adpd188_slot_fifo_mode
 * @brief The way a time slot stores data in the FIFO.
 */
enum adpd188_slot_fifo_mode {
	/** No data to FIFO. */
	ADPD188_NO_FIFO,
	/** 16-bit sum of all four channels. */
	ADPD188_16BIT_SUM,
	/** 32-bit sum of all four channels. */
	ADPD188_32BIT_SUM,
	/** Four channels of 16-bit sample data for the time slot. */
	ADPD188_16BIT_4CHAN = 0x4,
	/** Four channels of 32-bit sample data for the time slot. */
	ADPD188_32BIT_4CHAN = 0x6
};

/**
 * @struct adpd188_slot_config
 * @brief Slot configuration initialization structure.
 */
struct adpd188_slot_config {
	/** Time slot ID. */
	enum adpd188_slots slot_id;
	/** Enable time slot. */
	bool slot_en;
	/** Time slot FIFO mode. */
	enum adpd188_slot_fifo_mode sot_fifo_mode;
};

/**
 * @struct adpd188_dev
 * @brief Driver descriptor structure.
 */
struct adpd188_dev {
	/** Selected device */
	enum adpd_supported_devices device;
	/** Communication physical type. */
	enum adpd188_phy_opt phy_opt;
	/** Communication physical descriptor. */
	void *phy_desc;
	/** GPIO 0 descriptor. */
	struct no_os_gpio_desc *gpio0;
	/** GPIO 1 descriptor. */
	struct no_os_gpio_desc *gpio1;
};

/**
 * @struct adpd188_init_param
 * @brief Driver initialization structure.
 */
struct adpd188_init_param {
	/** Selected device */
	enum adpd_supported_devices device;
	/** Communication physical type. */
	enum adpd188_phy_opt phy_opt;
	/** Communication physical initialization structure. */
	union adpd188_phy_init phy_init;
	/** GPIO 0 initialization structure. */
	struct no_os_gpio_init_param gpio0_init;
	/** GPIO 0 initialization structure. */
	struct no_os_gpio_init_param gpio1_init;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Initialize the ADPD188 driver. */
int32_t adpd188_init(struct adpd188_dev **device,
		     struct adpd188_init_param *init_param);

/* Free resources allocated by adpd188_init(). */
int32_t adpd188_remove(struct adpd188_dev *dev);

/* Read one 16 bit register of the ADPD188. */
int32_t adpd188_reg_read(struct adpd188_dev *dev, uint8_t reg_addr,
			 uint16_t *reg_val);

/* Write one 16 bit register of the ADPD188. */
int32_t adpd188_reg_write(struct adpd188_dev *dev, uint8_t reg_addr,
			  uint16_t reg_val);

/* Get the mode of operation of the ADPD188. */
int32_t adpd188_mode_get(struct adpd188_dev *dev, enum adpd188_mode *mode);

/* Set the mode of operation of the ADPD188. */
int32_t adpd188_mode_set(struct adpd188_dev *dev, enum adpd188_mode new_mode);

/* Get the number of bytes currently present in FIFO. */
int32_t adpd188_fifo_status_get(struct adpd188_dev *dev, uint8_t *bytes_no);

/* Empty the FIFO. */
int32_t adpd188_fifo_clear(struct adpd188_dev *dev);

/*
 * Set the number of 16 bit words that need to be in the FIFO to trigger an
 * interrupt.
 */
int32_t adpd188_fifo_thresh_set(struct adpd188_dev *dev, uint8_t word_no);

/* Get the slot and FIFO interrupt flags. */
int32_t adpd188_interrupt_get(struct adpd188_dev *dev, uint8_t *flags);

/* Clear the slot and FIFO interrupt flags. */
int32_t adpd188_interrupt_clear(struct adpd188_dev *dev, uint8_t flags);

/* Enable the slot and FIFO interrupt flags. */
int32_t adpd188_interrupt_en(struct adpd188_dev *dev, uint8_t flags);

/* Setup drive and polarity of the GPIOs. */
int32_t adpd188_gpio_setup(struct adpd188_dev *dev,
			   struct adpd188_gpio_config config);

/* Setup the GPIO source. */
int32_t adpd188_gpio_alt_setup(struct adpd188_dev *dev, uint8_t gpio_id,
			       enum adpd188_gpio_alt_config config);

/* Do software reset of the device. */
int32_t adpd188_sw_reset(struct adpd188_dev *dev);

/* Do internal 32MHz clock calibration. */
int32_t adpd188_clk32mhz_cal(struct adpd188_dev *dev);

/* Enable slot and setup its FIFO interaction. */
int32_t adpd188_slot_setup(struct adpd188_dev *dev,
			   struct adpd188_slot_config config);

/* Set sample frequency of the ADC. */
int32_t adpd188_adc_fsample_set(struct adpd188_dev *dev, uint16_t freq_hz);

/* Get sample frequency of the ADC. */
int32_t adpd188_adc_fsample_get(struct adpd188_dev *dev, uint16_t *freq_hz);

/* Do initial configuration of the device to use as a smoke detector. */
int32_t adpd188_smoke_detect_setup(struct adpd188_dev *dev);

#endif /* ADPD188_H_ */
