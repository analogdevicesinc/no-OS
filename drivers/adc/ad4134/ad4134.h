/***************************************************************************//**
 *   @file   ad4134.h
 *   @brief  Header file for the ad4134 Driver.
 *   @author SPopa (stefan.popa@analog.com)
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

/*
 * Supported parts:
 *  - AD7134;
 *  - AD4134.
 */

#ifndef SRC_AD4134_H_
#define SRC_AD4134_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_util.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
/*
 * AD4134 registers definition
 */
#define AD4134_REG_INTERFACE_CONFIG_A		0x00
#define AD4134_REG_INTERFACE_CONFIG_B		0x01
#define AD4134_REG_DEVICE_CONFIG		0x02
#define AD4134_REG_CHIP_TYPE			0x03
#define AD4134_REG_PRODUCT_ID_LSB		0x04
#define AD4134_REG_PRODUCT_ID_MSB		0x05
#define AD4134_REG_CHIP_GRADE			0x06
#define AD4134_REG_CHIP_INDEX			0x07
#define AD4134_REG_SCTATCH_PAD			0x0A
#define AD4134_REG_SPI_REVISION			0x0B
#define AD4134_REG_VENDOR_ID_LSB		0x0C
#define AD4134_REG_VENDOR_ID_MSB		0x0D
#define AD4134_REG_STREAM_MODE			0x0E
#define AD4134_REG_TRANSFER_REGISTER		0x0F
#define AD4134_REG_DEVICE_CONFIG1		0x10
#define AD4134_REG_DATA_PACKET_CONFIG		0x11
#define AD4134_REG_DIGITAL_INTERFACE_CONFIG	0x12
#define AD4134_REG_POWER_DOWN_CONTROL		0x13
#define AD4134_REG_AIN_RANGE_SELECT		0x14
#define AD4134_REG_DEVICE_STATUS		0x15
#define AD4134_REG_ODR_VAL_INT_LSB		0x16
#define AD4134_REG_ODR_VAL_INT_MID		0x17
#define AD4134_REG_ODR_VAL_INT_MSB		0x18
#define AD4134_REG_ODR_VAL_FLT_LSB		0x19
#define AD4134_REG_ODR_VAL_FLT_MID0		0x1A
#define AD4134_REG_ODR_VAL_FLT_MID1		0x1B
#define AD4134_REG_ODR_VAL_FLT_MSB		0x1C
#define AD4134_REG_CHANNEL_ODR_SELECT		0x1D
#define AD4134_REG_CHAN_DIG_FILTER_SEL		0x1E
#define AD4134_REG_FIR_BW_SEL			0x1F
#define AD4134_REG_GPIO_DIR_CTRL		0x20
#define AD4134_REG_GPIO_DATA			0x21
#define AD4134_REG_ERROR_PIN_SRC_CONTROL	0x22
#define AD4134_REG_ERROR_PIN_CONTROL		0x23
#define AD4134_REG_VCMBUF_CTRL			0x24
#define AD4134_REG_DIAGNOSTIC_CONTROL		0x25
#define AD4134_REG_MPC_CONFIG			0x26
#define AD4134_REG_CH0_GAIN_LSB			0x27
#define AD4134_REG_CH0_GAIN_MID			0x28
#define AD4134_REG_CH0_GAIN_MSB			0x29
#define AD4134_REG_CH0_OFFSET_LSB		0x2A
#define AD4134_REG_CH0_OFFSET_MID		0x2B
#define AD4134_REG_CH0_OFFSET_MSB		0x2C
#define AD4134_REG_CH1_GAIN_LSB			0x2D
#define AD4134_REG_CH1_GAIN_MID			0x2E
#define AD4134_REG_CH1_GAIN_MSB			0x2F
#define AD4134_REG_CH1_OFFSET_LSB		0x30
#define AD4134_REG_CH1_OFFSET_MID		0x31
#define AD4134_REG_CH1_OFFSET_MSB		0x32
#define AD4134_REG_CH2_GAIN_LSB			0x33
#define AD4134_REG_CH2_GAIN_MID			0x34
#define AD4134_REG_CH2_GAIN_MSB			0x35
#define AD4134_REG_CH2_OFFSET_LSB		0x36
#define AD4134_REG_CH2_OFFSET_MID		0x37
#define AD4134_REG_CH2_OFFSET_MSB		0x38
#define AD4134_REG_CH3_GAIN_LSB			0x39
#define AD4134_REG_CH3_GAIN_MID			0x3A
#define AD4134_REG_CH3_GAIN_MSB			0x3B
#define AD4134_REG_CH3_OFFSET_LSB		0x3C
#define AD4134_REG_CH3_OFFSET_MID		0x3D
#define AD4134_REG_CH3_OFFSET_MSB		0x3E
#define AD4134_REG_MCLK_COUNTER			0x3F
#define AD4134_REG_DIG_FILTER_OFUF		0x40
#define AD4134_REG_DIG_FILTER_SETTLED		0x41
#define AD4134_REG_INTERNAL_ERROR		0x42
#define AD4134_REG_POWER_OV_ERROR_1		0x43
#define AD4134_REG_POWER_UV_ERROR_1		0x44
#define AD4134_REG_POWER_OV_ERROR_2		0x45
#define AD4134_REG_POWER_UV_ERROR_2		0x46
#define AD4134_REG_SPI_ERROR			0x47
#define AD4134_REG_AIN_OR_ERROR			0x48
#define AD4134_REG_AVDD5_VALUE			0x49
#define AD4134_REG_DVDD5_VALUE			0x4A
#define AD4134_REG_VREF_VALUE			0x4B
#define AD4134_REG_LDOIN_VALUE			0x4C
#define AD4134_REG_AVDD1V8_VALUE		0x4D
#define AD4134_REG_DVDD1V8_VALUE		0x4E
#define AD4134_REG_CLKVDD_VALUE			0x4F
#define AD4134_REG_IOVDD_VALUE			0x50
#define AD4134_REG_TEMPERATURE_DATA		0x51

/*
 * AD4134_REG_INTERFACE_CONFIG_A
 */
#define AD4134_INT_CONFIG_A_SOFT_RESET_MSK		NO_OS_BIT(7)
#define AD4134_INT_CONFIG_A_ADDR_ASC_BIT_MSK		NO_OS_BIT(5)
#define AD4134_INT_CONFIG_A_SDO_ACTIVE_BIT_MSK		NO_OS_BIT(4)
#define AD4134_INT_CONFIG_A_SOFT_RESET_MIRR_MSK		NO_OS_BIT(0)
#define AD4134_INT_CONFIG_A_ADDR_ASC_MIRR_MSK		NO_OS_BIT(2)
#define AD4134_INT_CONFIG_A_SDO_ACTIVE_MIRR_MSK		NO_OS_BIT(3)

/*
 * AD4134_REG_INTERFACE_CONFIG_B
 */
#define AD4134_INT_CONFIG_B_SINGLE_INSTR_MSK		NO_OS_BIT(7)
#define AD4134_INT_CONFIG_B_M_S_RD_CTRL_MSK		NO_OS_BIT(5)
#define AD4134_INT_CONFIG_B_DIG_IF_RST_MSK		NO_OS_BIT(1)

/*
 * AD4134_REG_DEVICE_CONFIG
 */
#define AD4134_DEV_CONFIG_OP_IN_PROGRESS_MSK		NO_OS_BIT(5)
#define AD4134_DEV_CONFIG_NO_CHIP_ERR_MSK		NO_OS_BIT(4)
#define AD4134_DEV_CONFIG_PWR_MODE_MSK			NO_OS_BIT(0)

/*
 * AD4134_REG_CHIP_TYPE
 */
#define AD4134_CHIP_TYPE_BITS_MSK			NO_OS_GENMASK(7, 0)
#define AD4134_CHIP_TYPE_BITS_MODE(x)			(((x) & 0xFF) << 0)
#define AD4134_CHIP_TYPE				0x07

/*
 * AD4134_REG_PRODUCT_ID_LSB
 */
#define AD4134_PRODUCT_ID_LSB_BITS_MSK			NO_OS_GENMASK(7, 0)
#define AD4134_PRODUCT_ID_LSB_BITS_MODE(x)		(((x) & 0xFF) << 0)

/*
 * AD4134_REG_PRODUCT_ID_MSB
 */
#define AD4134_PRODUCT_ID_MSB_BITS_MSK			NO_OS_GENMASK(7, 0)
#define AD4134_PRODUCT_ID_MSB_BITS_MODE(x)		(((x) & 0xFF) << 0)

/*
 * AD4134_REG_CHIP_GRADE
 */
#define AD4134_CHIP_GRADE_PROD_GRADE_BITS_MSK		NO_OS_GENMASK(7, 4)
#define AD4134_CHIP_GRADE_PROD_GRADE_BITS_MODE(x) 	(((x) & 0x0F) << 4)
#define AD4134_CHIP_GRADE_DEV_VERSION_BITS_MSK		NO_OS_GENMASK(3, 0)
#define AD4134_CHIP_GRADE_DEV_VERSION_BITS_MODE(x) 	(((x) & 0x0F) << 0)

/*
 * AD4134_REG_CHIP_INDEX
 */
#define AD4134_SILICON_REV_ID_BITS_MSK			NO_OS_GENMASK(7, 0)
#define AD4134_SILICON_REV_ID_BITS_MODE(x)		(((x) & 0xFF) << 0)

/*
 * AD4134_REG_SCRATCH_PAD
 */
#define AD4134_SCRATCH_PAD_BITS_MSK			NO_OS_GENMASK(7, 0)
#define AD4134_SCRATCH_PAD_BITS_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD4134_REG_SPI_REVISION
 */
#define AD4134_SPI_REVISION_BITS_MSK			NO_OS_GENMASK(7, 0)
#define AD4134_SPI_REVISION_BITS_MODE(x)		(((x) & 0xFF) << 0)

/*
 * AD4134_REG_VENDOR_ID_LSB
 */
#define AD4134_VENDOR_ID_LSB_BITS_MSK			NO_OS_GENMASK(7, 0)
#define AD4134_VENDOR_ID_LSB_BITS_MODE(x)		(((x) & 0xFF) << 0)

/*
 * AD4134_REG_VENDOR_ID_MSB
 */
#define AD4134_VENDOR_ID_MSB_BITS_MSK			NO_OS_GENMASK(7, 0)
#define AD4134_VENDOR_ID_MSB_BITS_MODE(x)		(((x) & 0xFF) << 0)

/*
 * AD4134_REG_STREAM_MODE
 */
#define AD4134_STREAM_MODE_BITS_MSK			NO_OS_GENMASK(7, 0)
#define AD4134_STREAM_MODE_BITS_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD4134_REG_TRANSFER_REGISTER
 */
#define AD4134_TRANSFER_MASTER_SLAVE_TX_BIT_MSK		NO_OS_BIT(0)

/*
 * AD4134_REG_DEVICE_CONFIG1
 */
#define AD4134_DEV_CONFIG1_MPC_MAGPHA_EN_MSK		NO_OS_BIT(6)
#define AD4134_DEV_CONFIG1_MPC_MAG_EN_MSK		NO_OS_BIT(5)
#define AD4134_DEV_CONFIG1_AA_MODE_MSK			NO_OS_BIT(4)
#define AD4134_DEV_CONFIG1_SDO_PIN_SRC_SEL_MSK		NO_OS_BIT(2)
#define AD4134_DEV_CONFIG1_REF_GAIN_CORR_EN_MSK		NO_OS_BIT(1)
#define AD4134_DEV_CONFIG1_CLKOUT_EN_MSK		NO_OS_BIT(0)

/*
 * AD4134_REG_DATA_PACKET_CONFIG
 */
#define AD4134_DATA_PACKET_CONFIG_CRC_POLY_RST_MSK	NO_OS_BIT(7)
#define AD4134_DATA_PACKET_CONFIG_FRAME_MSK		NO_OS_GENMASK(6, 4)
#define AD4134_DATA_PACKET_CONFIG_FRAME_MODE(x)		(((x) & 0x7) << 4)
#define AD4134_DATA_PACKET_CONFIG_DCLK_FREQ_MSK		NO_OS_GENMASK(3, 0)
#define AD4134_DATA_PACKET_CONFIG_DCLK_FREQ_MODE(x)	(((x) & 0xF) << 0)

/*
 * AD4134_REG_DIGITAL_INTERFACE_CONFIG
 */
#define AD4134_DIG_INT_CONFIG_DAISY_CHAIN_NUM_MSK	NO_OS_GENMASK(7, 4)
#define AD4134_DIG_INT_CONFIG_DAISY_CHAIN_NUM_MODE(x)	(((x) & 0xF) << 4)
#define AD4134_DIG_INT_CONFIG_AVG_SEL_MSK		NO_OS_GENMASK(3, 2)
#define AD4134_DIG_INT_CONFIG_AVG_SEL_MODE(x)		(((x) & 0x3) << 2)
#define AD4134_DIG_INT_CONFIG_FORMAT_MSK		NO_OS_GENMASK(1, 0)
#define AD4134_DIG_INT_CONFIG_FORMAT_MODE(x)		(((x) & 0x3) << 0)

/*
 * AD4134_REG_POWER_DOWN_CONTROL
 */
#define AD4134_PWRDN_CTRL_PWRDN_CH_MSK(ch)		NO_OS_BIT(ch)
#define AD4134_PWRDN_CTRL_PWRDN_AUXADC_MSK		NO_OS_BIT(2)
#define AD4134_PWRDN_CTRL_PWRDN_LDO_MSK			NO_OS_BIT(1)
#define AD4134_PWRDN_CTRL_PWRDN_SLEEP_MODE_EN_MSK	NO_OS_BIT(0)

/*
 * AD4134_REG_AIN_RANGE_SELECT
 */
#define AD4134_AIN_RANGE_SEL_CH_MSK(ch)			NO_OS_BIT(ch)

/*
 * AD4134_REG_DEVICE_STATUS
 */
#define AD4134_DEV_STAT_DCLKMODE_MSK			NO_OS_BIT(5)
#define AD4134_DEV_STAT_DCLKIO_MSK			NO_OS_BIT(4)
#define AD4134_DEV_STAT_MODE_MSK			NO_OS_BIT(3)
#define AD4134_DEV_STAT_CLKSEL_MSK			NO_OS_BIT(2)
#define AD4134_DEV_STAT_FUSE_ECC_MSK			NO_OS_BIT(1)
#define AD4134_DEV_STAT_PLL_LOCK_MSK			NO_OS_BIT(0)

/*
 * AD4134_REG_ODR_VAL_INT_LSB
 */
#define AD4134_ODR_VAL_INT_LSB_MSK			NO_OS_GENMASK(7, 0)
#define AD4134_ODR_VAL_INT_LSB_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD4134_REG_ODR_VAL_INT_MID
 */
#define AD4134_ODR_VAL_INT_MID_MSK			NO_OS_GENMASK(7, 0)
#define AD4134_ODR_VAL_INT_MID_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD4134_REG_ODR_VAL_INT_MSB
 */
#define AD4134_ODR_VAL_INT_MSB_MSK			NO_OS_GENMASK(7, 0)
#define AD4134_ODR_VAL_INT_MSB_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD4134_REG_ODR_VAL_FLT_LSB
 */
#define AD4134_ODR_VAL_FLT_LSB_MSK			NO_OS_GENMASK(7, 0)
#define AD4134_ODR_VAL_FLT_LSB_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD4134_REG_ODR_VAL_FLT_MID0
 */
#define AD4134_ODR_VAL_FLT_MID0_MSK			NO_OS_GENMASK(7, 0)
#define AD4134_ODR_VAL_FLT_MID0_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD4134_REG_ODR_VAL_FLT_MID1
 */
#define AD4134_ODR_VAL_FLT_MID1_MSK			NO_OS_GENMASK(7, 0)
#define AD4134_ODR_VAL_FLT_MID1_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD4134_REG_ODR_VAL_FLT_MSB
 */
#define AD4134_ODR_VAL_FLT_MSB_MSK			NO_OS_GENMASK(7, 0)
#define AD4134_ODR_VAL_FLT_MSB_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD4134_REG_CHANNEL_ODR_SELECT
 */
#define AD4134_ODR_RATE_SEL_CH_MSK(ch)			(NO_OS_GENMASK(1, 0) << (2 * ch))
#define AD4134_ODR_RATE_SEL_CH_MODE(x, ch)		(((x) & 0x3) << (2 * ch))

/*
 * AD4134_REG_CHAN_DIG_FILTER_SEL
 */
#define AD4134_DIGFILTER_SEL_CH_MSK(ch)			(NO_OS_GENMASK(1, 0) << (2 * ch))
#define AD4134_DIGFILTER_SEL_CH_MODE(x, ch)		(((x) & 0x3) << (2 * ch))

/*
 * AD4134_REG_FIR_BW_SEL
 */
#define AD4134_FIR_BW_SEL_CH_MSK(ch)			NO_OS_BIT(ch)

/*
 * AD4134_REG_GPIO_DIR_CTRL
 */
#define AD4134_GPIO_IO_CTRL_MSK				NO_OS_GENMASK(7, 0)
#define AD4134_GPIO_IO_CTRL_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD4134_REG_GPIO_DATA
 */
#define AD4134_GPIO_DATA_MSK				NO_OS_GENMASK(7, 0)
#define AD4134_GPIO_DATA_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD4134_REG_ERROR_PIN_SRC_CONTROL
 */
#define AD4134_ERR_PIN_EN_OR_AIN_MSK			NO_OS_BIT(5)
#define AD4134_ERR_PIN_EN_INTERNAL_MSK			NO_OS_BIT(4)
#define AD4134_ERR_PIN_EN_SPI_MSK			NO_OS_BIT(3)
#define AD4134_ERR_PIN_EN_LDO_XOSC_MSK			NO_OS_BIT(2)
#define AD4134_ERR_PIN_EN_TEMP_MSK			NO_OS_BIT(1)
#define AD4134_ERR_PIN_EN_PWR_MSK			NO_OS_BIT(0)

/*
 * AD4134_REG_ERROR_PIN_CONTROL
 */
#define AD4134_ERR_PIN_IN_STATUS_MSK			NO_OS_BIT(2)
#define AD4134_ERR_PIN_IN_EN_MSK			NO_OS_BIT(1)
#define AD4134_ERR_PIN_OUT_EN_MSK			NO_OS_BIT(0)

/*
 * AD4134_REG_VCMBUF_CTRL
 */
#define AD4134_VCMBUF_CTRL_PWRDN_MSK			NO_OS_BIT(6)
#define AD4134_VCMBUF_CTRL_REF_DIV_SEL_MSK		NO_OS_GENMASK(5, 1)
#define AD4134_VCMBUF_CTRL_REF_DIV_SEL_MODE(x)		(((x) & 0x1F) << 1)
#define AD4134_VCMBUF_CTRL_REF_SEL_MSK			NO_OS_BIT(0)

/*
 * AD4134_REG_DIAGNOSTIC_CONTROL
 */
#define AD4134_DIAGCTRL_ERR_OR_AIN_EN_MSK		NO_OS_BIT(5)
#define AD4134_DIAGCTRL_ERR_PWR_MON_EN_MSK		NO_OS_BIT(4)
#define AD4134_DIAGCTRL_MCLK_CNT_EN_MSK			NO_OS_BIT(3)
#define AD4134_DIAGCTRL_ERR_SPI_CRC_EN_MSK		NO_OS_BIT(2)
#define AD4134_DIAGCTRL_ERR_MM_CRC_EN_MSK		NO_OS_BIT(1)
#define AD4134_DIAGCTRL_FUSE_CRC_CHECK_MSK		NO_OS_BIT(0)

/*
 * AD4134_REG_MPC_CONFIG
 */
#define AD4134_MPC_CLKDEL_EN_CH_MSK(ch)			(NO_OS_GENMASK(1, 0) << (2 * ch))
#define AD4134_MPC_CLKDEL_EN_CH_MODE(x, ch)		(((x) & 0x3) << (2 * ch))

/*
 * AD4134_REG_CHx_GAIN_LSB
 */
#define AD4134_CH_GAIN_LSB_MSK				NO_OS_GENMASK(7, 0)
#define AD4134_CH_GAIN_LSB_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD4134_REG_CHx_GAIN_MID
 */
#define AD4134_CH_GAIN_MID_MSK				NO_OS_GENMASK(7, 0)
#define AD4134_CH_GAIN_MID_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD4134_REG_CHx_GAIN_MSB
 */
#define AD4134_CH_GAIN_CAL_SEL_MSK			NO_OS_BIT(4)
#define AD4134_CH_GAIN_MSB_MSK				NO_OS_GENMASK(3, 0)
#define AD4134_CH_GAIN_MSB_MODE(x)			(((x) & 0xF) << 0)

/*
 * AD4134_REG_CHx_OFFSET_LSB
 */
#define AD4134_CH_OFFSET_LSB_MSK			NO_OS_GENMASK(7, 0)
#define AD4134_CH_OFFSET_LSB_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD4134_REG_CHx_OFFSET_MID
 */
#define AD4134_CH_OFFSET_MID_MSK			NO_OS_GENMASK(7, 0)
#define AD4134_CH_OFFSET_MID_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD4134_REG_CHx_OFFSET_MSB
 */
#define AD4134_CH_OFFSET_CAL_EN_MSK			NO_OS_BIT(7)
#define AD4134_CH_OFFSET_MSB_MSK			NO_OS_GENMASK(6, 0)
#define AD4134_CH_OFFSET_MSB_MODE(x)			(((x) & 0x7F) << 0)

/*
 * AD4134_REG_MCLK_COUNTER
 */
#define AD4134_MCLK_COUNT_MSK				NO_OS_GENMASK(7, 0)
#define AD4134_MCLK_COUNT_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD4134_REG_DIG_FILTER_OFUF
 */
#define AD4134_DIGFILTER_ERR_OFUF_CH_MSK(ch)		NO_OS_BIT(ch)

/*
 * AD4134_REG_DIG_FILTER_SETTLED
 */
#define AD4134_DIGFILTER_CH_SETTLED_MSK(ch)		NO_OS_BIT(ch)

/*
 * AD4134_REG_INTERNAL_ERROR
 */
#define AD4134_INT_ERR_NO_CLOCK_MSK			NO_OS_BIT(5)
#define AD4134_INT_ERR_TEMP_MSK				NO_OS_BIT(4)
#define AD4134_INT_ERR_DCLK_MSK				NO_OS_BIT(3)
#define AD4134_INT_ERR_FUSE_CRC_MSK			NO_OS_BIT(2)
#define AD4134_INT_ERR_ASRC_MSK				NO_OS_BIT(1)
#define AD4134_INT_ERR_MM_CRC_MSK			NO_OS_BIT(0)

/*
 * AD4134_REG_POWER_OV_ERROR_1
 */
#define AD4134_POWER_ERR_OV_IOVDD_MSK			NO_OS_BIT(3)
#define AD4134_POWER_ERR_OV_CLKVDD_MSK			NO_OS_BIT(2)
#define AD4134_POWER_ERR_OV_DVDD1V8_MSK			NO_OS_BIT(1)
#define AD4134_POWER_ERR_OV_AVDD1V8_MSK			NO_OS_BIT(0)

/*
 * AD4134_REG_POWER_UV_ERROR_1
 */
#define AD4134_POWER_ERR_UV_IOVDD_MSK			NO_OS_BIT(3)
#define AD4134_POWER_ERR_UV_CLKVDD_MSK			NO_OS_BIT(2)
#define AD4134_POWER_ERR_UV_DVDD1V8_MSK			NO_OS_BIT(1)
#define AD4134_POWER_ERR_UV_AVDD1V8_MSK			NO_OS_BIT(0)

/*
 * AD4134_REG_POWER_OV_ERROR_2
 */
#define AD4134_POWER_ERR_OV_VREF_MSK			NO_OS_BIT(3)
#define AD4134_POWER_ERR_OV_LDOIN_MSK			NO_OS_BIT(2)
#define AD4134_POWER_ERR_OV_DVDD5_MSK			NO_OS_BIT(1)
#define AD4134_POWER_ERR_OV_AVDD5_MSK			NO_OS_BIT(0)

/*
 * AD4134_REG_POWER_UV_ERROR_2
 */
#define AD4134_POWER_ERR_UV_VREF_MSK			NO_OS_BIT(3)
#define AD4134_POWER_ERR_UV_LDOIN_MSK			NO_OS_BIT(2)
#define AD4134_POWER_ERR_UV_DVDD5_MSK			NO_OS_BIT(1)
#define AD4134_POWER_ERR_UV_AVDD5_MSK			NO_OS_BIT(0)

/*
 * AD4134_REG_SPI_ERROR
 */
#define AD4134_SPI_ERROR_CRC_MSK			NO_OS_BIT(3)
#define AD4134_SPI_ERROR_SCLK_CNT_MSK			NO_OS_BIT(2)
#define AD4134_SPI_ERROR_WRITE_MSK			NO_OS_BIT(1)
#define AD4134_SPI_ERROR_READ_MSK			NO_OS_BIT(0)

/*
 * AD4134_REG_AIN_OR_ERROR
 */
#define AD4134_ERR_OR_AIN_MSK(ch)			NO_OS_BIT(ch)

/*
 * AD4134_REG_AVDD5_VALUE
 */
#define AD4134_AVDD5_VALUE_PIN_MSK			NO_OS_GENMASK(7, 0)
#define AD4134_AVDD5_VALUE_PIN_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD4134_REG_DVDD5_VALUE
 */
#define AD4134_DVDD5_VALUE_PIN_MSK			NO_OS_GENMASK(7, 0)
#define AD4134_DVDD5_VALUE_PIN_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD4134_REG_VREF_VALUE
 */
#define AD4134_VREF_VALUE_PIN_MSK			NO_OS_GENMASK(7, 0)
#define AD4134_VREF_VALUE_PIN_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD4134_REG_LDOIN_VALUE
 */
#define AD4134_LDOIN_VALUE_PIN_MSK			NO_OS_GENMASK(7, 0)
#define AD4134_LDOIN_VALUE_PIN_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD4134_REG_AVDD1V8_VALUE
 */
#define AD4134_AVDD1V8_VALUE_PIN_MSK			NO_OS_GENMASK(7, 0)
#define AD4134_AVDD1V8_VALUE_PIN_MODE(x)		(((x) & 0xFF) << 0)

/*
 * AD4134_REG_DVDD1V8_VALUE
 */
#define AD4134_DVDD1V8_VALUE_PIN_MSK			NO_OS_GENMASK(7, 0)
#define AD4134_DVDD1V8_VALUE_PIN_MODE(x)		(((x) & 0xFF) << 0)

/*
 * AD4134_REG_CLKVDD_VALUE
 */
#define AD4134_CLKVDD_VALUE_PIN_MSK			NO_OS_GENMASK(7, 0)
#define AD4134_CLKVDD_VALUE_PIN_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD4134_REG_IOVDD_VALUE
 */
#define AD4134_IOVDD_VALUE_PIN_MSK			NO_OS_GENMASK(7, 0)
#define AD4134_IOVDD_VALUE_PIN_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD4134_REG_TEMPERATURE_DATA
 */
#define AD4134_TEMP_DATA_MSK				NO_OS_GENMASK(7, 0)
#define AD4134_TEMP_DATA_MODE(x)			(((x) & 0xFF) << 0)

#define AD4134_REG_READ(x)				((1 << 7) | (x & 0x7F))

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum ad4134_supported_dev_ids
 * @brief ID of devices supported by the driver
 */
enum ad4134_supported_dev_ids {
	ID_AD7132,
	ID_AD7134,
	ID_AD7136,
	ID_AD4134
};

/**
 * @enum ad4134_power_mode
 * @brief AD4134 power modes
 */
enum ad4134_power_mode {
	/** Low power mode option */
	LOW_POWER,
	/** Full power mode option */
	HIGH_POWER
};

/**
 * @enum ad4134_adc_data_len
 * @brief AD4134 possible number of bits per data sample
 */
enum ad4134_adc_data_len {
	/** 16 bit data sample */
	ADC_16_BIT_DATA,
	/** 24 bit data sample */
	ADC_24_BIT_DATA,
	/** 32 bit data sample */
	ADC_32_BIT_DATA,
	/** To know when to stop when cycling between them */
	INVALID
};

/**
 * @enum ad4134_crc_header
 * @brief AD4134 possible data CRC header choices
 */
enum ad4134_crc_header {
	/** Data sample comes with no CRC attached */
	NO_CRC,
	/** Data sample comes with 6-bit CRC attached */
	CRC_6,
	/** Data sample comes with 8-bit CRC attached */
	CRC_8
};

/**
 * @enum ad4134_doutx_format
 * @brief AD4134 list for possible output modes
 */
enum ad4134_doutx_format {
	/** Single channel Daisy-chain mode */
	SINGLE_CH_DC,
	/** Dual channel Daisy-chain mode */
	DUAL_CH_DC,
	/** Quad-channel parallel output mode */
	QUAD_CH_PO,
	/** Channel average mode */
	CH_AVG_MODE
};

/**
 * @enum ad4134_dig_filter_sel
 * @brief AD4134 list of input filters.
 */
enum ad4134_dig_filter_sel {
	/** Wideband filter (Finite impulse response) */
	FIR,
	/** Sinc6 filter */
	SINC6,
	/** Sinc3 filter */
	SINC3,
	/** Sinc3 filter with 50Hz and 60Hz rejection */
	SINC3_50_60_REJ
};

/**
 * @enum ad4134_channels
 * @brief AD4134 list of channels
 */
enum ad4134_channels {
	/** Channel 0 */
	CH0,
	/** Channel 1 */
	CH1,
	/** Channel 2 */
	CH2,
	/** Channel 3 */
	CH3,
	/** Max number of channels */
	AD4134_CH_MAX
};

/**
 * @enum ad717x_mpc_clkdel
 * @brief AD4134 list of clock delays
 */
enum ad717x_mpc_clkdel {
	/** No delay */
	DELAY_NONE,
	/** One clock cycle delay */
	DELAY_1_CLOCKS,
	/** Two clock cycles delay */
	DELAY_2_CLOCKS
};

/**
 * @struct ad4134_dev
 * @brief AD4134 driver handler structure
 */
struct ad4134_dev {
	/** SPI layer handler. */
	struct no_os_spi_desc        	*spi_desc;
	/** MODE GPIO handler. */
	struct no_os_gpio_desc		*gpio_mode;
	/** DCLKMODE GPIO handler. */
	struct no_os_gpio_desc		*gpio_dclkmode;
	/** DCLKIO GPIO handler. */
	struct no_os_gpio_desc		*gpio_dclkio;
	/** RESET GPIO handler. */
	struct no_os_gpio_desc		*gpio_resetn;
	/** PDN GPIO handler. */
	struct no_os_gpio_desc		*gpio_pnd;
	/** CS Sync */
	struct no_os_spi_desc *gpio_cs_sync;
	/** ID of supported device. */
	enum ad4134_supported_dev_ids dev_id;
	/** Length of data in bits. */
	enum ad4134_adc_data_len	adc_data_len;
	/** CRC option. */
	enum ad4134_crc_header	crc_header;
	/** MODE GPIO starting value */
	bool mode_master_nslave;
};

/**
 * @struct ad4134_init_param
 * @brief AD4134 driver initialization structure
 */
struct ad4134_init_param {
	/** SPI layer initialization structure. */
	struct no_os_spi_init_param spi_init_prm;
	/** MODE GPIO initialization structure. */
	struct no_os_gpio_init_param *gpio_mode;
	/** DCLKMODE GPIO initialization structure. */
	struct no_os_gpio_init_param *gpio_dclkmode;
	/** DCLKIO GPIO initialization structure. */
	struct no_os_gpio_init_param *gpio_dclkio;
	/** RESET GPIO initialization structure. */
	struct no_os_gpio_init_param *gpio_resetn;
	/** PDN GPIO initialization structure. */
	struct no_os_gpio_init_param *gpio_pnd;
	/** CS_GPIO */
	struct no_os_gpio_init_param *gpio_cs_sync;
	/** MODE GPIO starting value */
	bool mode_master_nslave;
	/** DCLKMODE GPIO starting value */
	bool dclkmode_free_ngated;
	/** DCLKIO GPIO starting value */
	bool dclkio_out_nin;
	/** PDN GPIO starting value */
	bool pnd;
	/** ID of supported device. */
	enum ad4134_supported_dev_ids dev_id;
	/** Length of data in bits. */
	enum ad4134_adc_data_len	adc_data_len;
	/** CRC option. */
	enum ad4134_crc_header	crc_header;
	enum ad4134_doutx_format	format;
	/** Clock delay state. */
	bool 			clk_delay_en;
	/** SPI layer handler if the SPI bus is shared with another device. In this
	 *  case the SPI should not be initialized again. */
	struct no_os_spi_desc *spi_common_dev;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/** Read from device. */
int32_t ad4134_spi_reg_read(struct ad4134_dev *dev, uint8_t reg_addr,
			    uint8_t *reg_data);

/** Write to device. */
int32_t ad4134_spi_reg_write(struct ad4134_dev *dev, uint8_t reg_addr,
			     uint8_t reg_data);

/** SPI write to device using a mask. */
int32_t ad4134_spi_write_mask(struct ad4134_dev *dev, uint8_t reg_addr,
			      uint32_t mask, uint8_t data);

/** Device power mode control. */
int32_t ad4134_set_power_mode(struct ad4134_dev *dev,
			      enum ad4134_power_mode mode);

/** ADC conversion data output frame control. */
int32_t ad4134_set_out_data_frame(struct ad4134_dev *dev,
				  enum ad4134_adc_data_len adc_data_len,
				  enum ad4134_crc_header crc_header);

/** DOUTx output format configuration. */
int32_t ad4134_dout_format_config(struct ad4134_dev *dev,
				  enum ad4134_doutx_format format);

/** Magnitude and phase matching calibration clock delay enable for all channels
 *  at 2 clock delay. */
int32_t ad4134_mag_phase_clk_delay(struct ad4134_dev *dev, bool clk_delay_en);

/** Digital filter type selection for each channel. */
int32_t ad4134_dig_filter_sel_ch(struct ad4134_dev *dev,
				 enum ad4134_dig_filter_sel filter, enum ad4134_channels ch);

/** Enable/Disable CLKOUT output. */
int32_t ad4134_clkout_output_en(struct ad4134_dev *dev, bool enable);

/** Enable/Disable reference gain correction. */
int32_t ad4134_ref_gain_correction_en(struct ad4134_dev *dev, bool enable);

/** Select the wideband filter bandwidth for a channel. */
int32_t ad4134_wideband_bw_sel(struct ad4134_dev *dev,
			       enum ad4134_channels ch, uint8_t wb_opt);

/** Initialize the device. */
int32_t ad4134_init(struct ad4134_dev **device,
		    struct ad4134_init_param *init_param);

/** Free the resources allocated by ad4134_init(). */
int32_t ad4134_remove(struct ad4134_dev *dev);

/** Print reg dump for the specific reg map of AD4134 dev*/
int32_t ad4134_spi_reg_dump(struct ad4134_dev *dev);

/** Multidevice synchronization */
int32_t ad4134_channel_sync(struct ad4134_dev *dev);

#endif /* SRC_AD4134_H_ */
