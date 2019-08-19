/***************************************************************************//**
 *   @file   ad713x.h
 *   @brief  Header file for ad713x Driver.
 *   @author SPopa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2017(c) Analog Devices, Inc.
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

#ifndef SRC_AD713X_H_
#define SRC_AD713X_H_

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
/*
 * Create a contiguous bitmask starting at bit position @l and ending at
 * position @h.
 */
#define GENMASK(h, l) \
		(((~0UL) - (1UL << (l)) + 1) & (~0UL >> (31 - (h))))

/*
 * AD713X registers definition
 */
#define AD713X_REG_INTERFACE_CONFIG_A		0x00
#define AD713X_REG_INTERFACE_CONFIG_B		0x01
#define AD713X_REG_DEVICE_CONFIG		0x02
#define AD713X_REG_CHIP_TYPE			0x03
#define AD713X_REG_PRODUCT_ID_LSB		0x04
#define AD713X_REG_PRODUCT_ID_MSB		0x05
#define AD713X_REG_CHIP_GRADE			0x06
#define AD713X_REG_CHIP_INDEX                   0x07
#define AD713X_REG_SCTATCH_PAD			0x0A
#define AD713X_REG_SPI_REVISION			0x0B
#define AD713X_REG_VENDOR_ID_LSB		0x0C
#define AD713X_REG_VENDOR_ID_MSB		0x0D
#define AD713X_REG_STREAM_MODE			0x0E
#define AD713X_REG_TRANSFER_REGISTER		0x0F
#define AD713X_REG_DEVICE_CONFIG1		0x10
#define AD713X_REG_DATA_PACKET_CONFIG		0x11
#define AD713X_REG_DIGITAL_INTERFACE_CONFIG	0x12
#define AD713X_REG_POWER_DOWN_CONTROL		0x13
#define AD713X_REG_AIN_RANGE_SELECT		0x14
#define AD713X_REG_DEVICE_STATUS		0x15
#define AD713X_REG_ODR_VAL_INT_LSB		0x16
#define AD713X_REG_ODR_VAL_INT_MID		0x17
#define AD713X_REG_ODR_VAL_INT_MSB		0x18
#define AD713X_REG_ODR_VAL_FLT_LSB		0x19
#define AD713X_REG_ODR_VAL_FLT_MID0		0x1A
#define AD713X_REG_ODR_VAL_FLT_MID1		0x1B
#define AD713X_REG_ODR_VAL_FLT_MSB		0x1C
#define AD713X_REG_CHANNEL_ODR_SELECT		0x1D
#define AD713X_REG_CHAN_DIG_FILTER_SEL		0x1E
#define AD713X_REG_FIR_BW_SEL			0x1F
#define AD713X_REG_GPIO_DIR_CTRL		0x20
#define AD713X_REG_GPIO_DATA			0x21
#define AD713X_REG_ERROR_PIN_SRC_CONTROL	0x22
#define AD713X_REG_ERROR_PIN_CONTROL		0x23
#define AD713X_REG_VCMBUF_CTRL			0x24
#define AD713X_REG_DIAGNOSTIC_CONTROL		0x25
#define AD713X_REG_MPC_CONFIG			0x26
#define AD713X_REG_CH0_GAIN_LSB			0x27
#define AD713X_REG_CH0_GAIN_MID			0x28
#define AD713X_REG_CH0_GAIN_MSB			0x29
#define AD713X_REG_CH0_OFFSET_LSB		0x2A
#define AD713X_REG_CH0_OFFSET_MID		0x2B
#define AD713X_REG_CH0_OFFSET_MSB		0x2C
#define AD713X_REG_CH1_GAIN_LSB			0x2D
#define AD713X_REG_CH1_GAIN_MID			0x2E
#define AD713X_REG_CH1_GAIN_MSB			0x2F
#define AD713X_REG_CH1_OFFSET_LSB		0x30
#define AD713X_REG_CH1_OFFSET_MID		0x31
#define AD713X_REG_CH1_OFFSET_MSB		0x32
#define AD713X_REG_CH2_GAIN_LSB			0x33
#define AD713X_REG_CH2_GAIN_MID			0x34
#define AD713X_REG_CH2_GAIN_MSB			0x35
#define AD713X_REG_CH2_OFFSET_LSB		0x36
#define AD713X_REG_CH2_OFFSET_MID		0x37
#define AD713X_REG_CH2_OFFSET_MSB		0x38
#define AD713X_REG_CH3_GAIN_LSB			0x39
#define AD713X_REG_CH3_GAIN_MID			0x3A
#define AD713X_REG_CH3_GAIN_MSB			0x3B
#define AD713X_REG_CH3_OFFSET_LSB		0x3C
#define AD713X_REG_CH3_OFFSET_MID		0x3D
#define AD713X_REG_CH3_OFFSET_MSB		0x3E
#define AD713X_REG_MCLK_COUNTER			0x3F
#define AD713X_REG_DIG_FILTER_OFUF		0x40
#define AD713X_REG_DIG_FILTER_SETTLED		0x41
#define AD713X_REG_INTERNAL_ERROR		0x42
#define AD713X_REG_POWER_OV_ERROR_1		0x43
#define AD713X_REG_POWER_UV_ERROR_1		0x44
#define AD713X_REG_POWER_OV_ERROR_2		0x45
#define AD713X_REG_POWER_UV_ERROR_2		0x46
#define AD713X_REG_SPI_ERROR			0x47
#define AD713X_REG_AIN_OR_ERROR			0x48
#define AD713X_REG_AVDD5_VALUE			0x49
#define AD713X_REG_DVDD5_VALUE			0x4A
#define AD713X_REG_VREF_VALUE			0x4B
#define AD713X_REG_LDOIN_VALUE			0x4C
#define AD713X_REG_AVDD1V8_VALUE		0x4D
#define AD713X_REG_DVDD1V8_VALUE		0x4E
#define AD713X_REG_CLKVDD_VALUE			0x4F
#define AD713X_REG_IOVDD_VALUE			0x50
#define AD713X_REG_TEMPERATURE_DATA		0x51

/*
 * AD713X_REG_INTERFACE_CONFIG_A
 */
#define AD713X_INT_CONFIG_A_SOFT_RESET_MSK		BIT(7)
#define AD713X_INT_CONFIG_A_SOFT_RESET_MODE(x)		(((x) & 0x1) << 7)
#define AD713X_INT_CONFIG_A_ADDR_ASC_BIT_MSK		BIT(5)
#define AD713X_INT_CONFIG_A_ADDR_ASC_BIT_MODE(x)	(((x) & 0x1) << 5)
#define AD713X_INT_CONFIG_A_SDO_ACTIVE_BIT_MSK		BIT(4)
#define AD713X_INT_CONFIG_A_SDO_ACTIVE_BIT_MODE(x)	(((x) & 0x1) << 4)
#define AD713X_INT_CONFIG_A_SOFT_RESET_MIRR_MSK		BIT(0)
#define AD713X_INT_CONFIG_A_SOFT_RESET_MIRR_MODE(x)	(((x) & 0x1) << 0)
#define AD713X_INT_CONFIG_A_ADDR_ASC_MIRR_MSK		BIT(2)
#define AD713X_INT_CONFIG_A_ADDR_ASC_MIRR_MODE(x)	(((x) & 0x1) << 2)
#define AD713X_INT_CONFIG_A_SDO_ACTIVE_MIRR_MSK		BIT(3)
#define AD713X_INT_CONFIG_A_SDO_ACTIVE_MIRR_MODE(x)	(((x) & 0x1) << 3)

/*
 * AD713X_REG_INTERFACE_CONFIG_B
 */
#define AD713X_INT_CONFIG_B_SINGLE_INSTR_MSK		BIT(7)
#define AD713X_INT_CONFIG_B_SINGLE_INSTR_MODE(x)	(((x) & 0x1) << 7)
#define AD713X_INT_CONFIG_B_M_S_RD_CTRL_MSK		BIT(5)
#define AD713X_INT_CONFIG_B_M_S_RD_CTRL_MODE(x)		(((x) & 0x1) << 5)
#define AD713X_INT_CONFIG_B_DIG_IF_RST_MSK		BIT(1)
#define AD713X_INT_CONFIG_B_DIG_IF_RST_MODE(x)		(((x) & 0x1) << 1)

/*
 * AD713X_REG_DEVICE_CONFIG
 */
#define AD713X_DEV_CONFIG_OP_IN_PROGRESS_MSK		BIT(5)
#define AD713X_DEV_CONFIG_OP_IN_PROGRESS_MODE(x)	(((x) & 0x1) << 5)
#define AD713X_DEV_CONFIG_NO_CHIP_ERR_MSK		BIT(4)
#define AD713X_DEV_CONFIG_NO_CHIP_ERR_MODE(x)		(((x) & 0x1) << 4)
#define AD713X_DEV_CONFIG_PWR_MODE_MSK			BIT(0)
#define AD713X_DEV_CONFIG_PWR_MODE(x)			(((x) & 0x1) << 0)

/*
 * AD713X_REG_CHIP_TYPE
 */
#define AD713X_CHIP_TYPE_BITS_MSK			GENMASK(7, 0)
#define AD713X_CHIP_TYPE_BITS_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD713X_REG_PRODUCT_ID_LSB
 */
#define AD713X_PRODUCT_ID_LSB_BITS_MSK			GENMASK(7, 0)
#define AD713X_PRODUCT_ID_LSB_BITS_MODE(x)		(((x) & 0xFF) << 0)

/*
 * AD713X_REG_PRODUCT_ID_MSB
 */
#define AD713X_PRODUCT_ID_MSB_BITS_MSK			GENMASK(7, 0)
#define AD713X_PRODUCT_ID_MSB_BITS_MODE(x)		(((x) & 0xFF) << 0)

/*
 * AD713X_REG_CHIP_GRADE
 */
#define AD713X_CHIP_GRADE_PROD_GRADE_BITS_MSK		GENMASK(7, 4)
#define AD713X_CHIP_GRADE_PROD_GRADE_BITS_MODE(x) 	(((x) & 0x0F) << 4)
#define AD713X_CHIP_GRADE_DEV_VERSION_BITS_MSK		GENMASK(3, 0)
#define AD713X_CHIP_GRADE_DEV_VERSION_BITS_MODE(x) 	(((x) & 0x0F) << 0)

/*
 * AD713X_REG_CHIP_INDEX
 */
#define AD713X_SILICON_REV_ID_BITS_MSK			GENMASK(7, 0)
#define AD713X_SILICON_REV_ID_BITS_MODE(x)		(((x) & 0xFF) << 0)

/*
 * AD713X_REG_SCRATCH_PAD
 */
#define AD713X_SCRATCH_PAD_BITS_MSK			GENMASK(7, 0)
#define AD713X_SCRATCH_PAD_BITS_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD713X_REG_SPI_REVISION
 */
#define AD713X_SPI_REVISION_BITS_MSK			GENMASK(7, 0)
#define AD713X_SPI_REVISION_BITS_MODE(x)		(((x) & 0xFF) << 0)

/*
 * AD713X_REG_VENDOR_ID_LSB
 */
#define AD713X_VENDOR_ID_LSB_BITS_MSK			GENMASK(7, 0)
#define AD713X_VENDOR_ID_LSB_BITS_MODE(x)		(((x) & 0xFF) << 0)

/*
 * AD713X_REG_VENDOR_ID_MSB
 */
#define AD713X_VENDOR_ID_MSB_BITS_MSK			GENMASK(7, 0)
#define AD713X_VENDOR_ID_MSB_BITS_MODE(x)		(((x) & 0xFF) << 0)

/*
 * AD713X_REG_STREAM_MODE
 */
#define AD713X_STREAM_MODE_BITS_MSK			GENMASK(7, 0)
#define AD713X_STREAM_MODE_BITS_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD713X_REG_TRANSFER_REGISTER
 */
#define AD713X_TRANSFER_MASTER_SLAVE_TX_BIT_MSK		BIT(0)
#define AD713X_TRANSFER_MASTER_SLAVE_TX_BIT_MODE(x)	(((x) & 0x1) << 0)

/*
 * AD713X_REG_DEVICE_CONFIG1
 */
#define AD713X_DEV_CONFIG1_MPC_MAGPHA_EN_MSK		BIT(6)
#define AD713X_DEV_CONFIG1_MPC_MAGPHA_EN_MODE(x)	(((x) & 0x1) << 6)
#define AD713X_DEV_CONFIG1_MPC_MAG_EN_MSK		BIT(5)
#define AD713X_DEV_CONFIG1_MPC_MAG_EN_MODE(x)		(((x) & 0x1) << 5)
#define AD713X_DEV_CONFIG1_AA_MODE_MSK			BIT(4)
#define AD713X_DEV_CONFIG1_AA_MODE(x)			(((x) & 0x1) << 4)
#define AD713X_DEV_CONFIG1_SDO_PIN_SRC_SEL_MSK		BIT(2)
#define AD713X_DEV_CONFIG1_SDO_PIN_SRC_SEL_MODE(x)	(((x) & 0x1) << 2)
#define AD713X_DEV_CONFIG1_REF_GAIN_CORR_EN_MSK		BIT(1)
#define AD713X_DEV_CONFIG1_REF_GAIN_CORR_EN_MODE(x)	(((x) & 0x1) << 1)
#define AD713X_DEV_CONFIG1_CLKOUT_EN_MSK		BIT(0)
#define AD713X_DEV_CONFIG1_CLKOUT_EN_MODE(x)		(((x) & 0x1) << 0)

/*
 * AD713X_REG_DATA_PACKET_CONFIG
 */
#define AD713X_DATA_PACKET_CONFIG_CRC_POLY_RST_MSK	BIT(7)
#define AD713X_DATA_PACKET_CONFIG_CRC_POLY_RST_MODE(x)	(((x) & 0x1) << 7)
#define AD713X_DATA_PACKET_CONFIG_FRAME_MSK		GENMASK(6, 4)
#define AD713X_DATA_PACKET_CONFIG_FRAME_MODE(x)		(((x) & 0x7) << 4)
#define AD713X_DATA_PACKET_CONFIG_DCLK_FREQ_MSK		GENMASK(3, 0)
#define AD713X_DATA_PACKET_CONFIG_DCLK_FREQ_MODE(x)	(((x) & 0xF) << 0)

/*
 * AD713X_REG_DIGITAL_INTERFACE_CONFIG
 */
#define AD713X_DIG_INT_CONFIG_DAISY_CHAIN_NUM_MSK	GENMASK(7, 4)
#define AD713X_DIG_INT_CONFIG_DAISY_CHAIN_NUM_MODE(x)	(((x) & 0xF) << 4)
#define AD713X_DIG_INT_CONFIG_AVG_SEL_MSK		GENMASK(3, 2)
#define AD713X_DIG_INT_CONFIG_AVG_SEL_MODE(x)		(((x) & 0x3) << 2)
#define AD713X_DIG_INT_CONFIG_FORMAT_MSK		GENMASK(1, 0)
#define AD713X_DIG_INT_CONFIG_FORMAT_MODE(x)		(((x) & 0x3) << 0)

/*
 * AD713X_REG_POWER_DOWN_CONTROL
 */
#define AD713X_PWRDN_CTRL_PWRDN_CH_MSK(ch)		BIT(ch)
#define AD713X_PWRDN_CTRL_PWRDN_CH_MODE(x, ch)		(((x) & 0x1) << ch)
#define AD713X_PWRDN_CTRL_PWRDN_AUXADC_MSK		BIT(2)
#define AD713X_PWRDN_CTRL_PWRDN_AUXADC_MODE(x)		(((x) & 0x1) << 2)
#define AD713X_PWRDN_CTRL_PWRDN_LDO_MSK			BIT(1)
#define AD713X_PWRDN_CTRL_PWRDN_LDO_MODE(x)		(((x) & 0x1) << 1)
#define AD713X_PWRDN_CTRL_PWRDN_SLEEP_MODE_EN_MSK	BIT(0)
#define AD713X_PWRDN_CTRL_PWRDN_SLEEP_MODE_EN_MODE(x)	(((x) & 0x1) << 0)

/*
 * AD713X_REG_AIN_RANGE_SELECT
 */
#define AD713X_AIN_RANGE_SEL_CH_MSK(ch)			BIT(ch)
#define AD713X_AIN_RANGE_SEL_CH_MODE(x, ch)		(((x) & 0x1) << ch)

/*
 * AD713X_REG_DEVICE_STATUS
 */
#define AD713X_DEV_STAT_DCLKMODE_MSK			BIT(5)
#define AD713X_DEV_STAT_DCLKMODE_MODE(x)		(((x) & 0x1) << 5)
#define AD713X_DEV_STAT_DCLKIO_MSK			BIT(4)
#define AD713X_DEV_STAT_DCLKIO_MODE(x)			(((x) & 0x1) << 4)
#define AD713X_DEV_STAT_MODE_MSK			BIT(3)
#define AD713X_DEV_STAT_MODE_MODE(x)			(((x) & 0x1) << 3)
#define AD713X_DEV_STAT_CLKSEL_MSK			BIT(2)
#define AD713X_DEV_STAT_CLKSEL_MODE(x)			(((x) & 0x1) << 2)
#define AD713X_DEV_STAT_FUSE_ECC_MSK			BIT(1)
#define AD713X_DEV_STAT_FUSE_ECC_MODE(x)		(((x) & 0x1) << 1)
#define AD713X_DEV_STAT_PLL_LOCK_MSK			BIT(0)
#define AD713X_DEV_STAT_PLL_LOCK_MODE(x)		(((x) & 0x1) << 0)

/*
 * AD713X_REG_ODR_VAL_INT_LSB
 */
#define AD713X_ODR_VAL_INT_LSB_MSK			GENMASK(7, 0)
#define AD713X_ODR_VAL_INT_LSB_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD713X_REG_ODR_VAL_INT_MID
 */
#define AD713X_ODR_VAL_INT_MID_MSK			GENMASK(7, 0)
#define AD713X_ODR_VAL_INT_MID_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD713X_REG_ODR_VAL_INT_MSB
 */
#define AD713X_ODR_VAL_INT_MSB_MSK			GENMASK(7, 0)
#define AD713X_ODR_VAL_INT_MSB_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD713X_REG_ODR_VAL_FLT_LSB
 */
#define AD713X_ODR_VAL_FLT_LSB_MSK			GENMASK(7, 0)
#define AD713X_ODR_VAL_FLT_LSB_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD713X_REG_ODR_VAL_FLT_MID0
 */
#define AD713X_ODR_VAL_FLT_MID0_MSK			GENMASK(7, 0)
#define AD713X_ODR_VAL_FLT_MID0_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD713X_REG_ODR_VAL_FLT_MID1
 */
#define AD713X_ODR_VAL_FLT_MID1_MSK			GENMASK(7, 0)
#define AD713X_ODR_VAL_FLT_MID1_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD713X_REG_ODR_VAL_FLT_MSB
 */
#define AD713X_ODR_VAL_FLT_MSB_MSK			GENMASK(7, 0)
#define AD713X_ODR_VAL_FLT_MSB_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD713X_REG_CHANNEL_ODR_SELECT
 */
#define AD713X_ODR_RATE_SEL_CH_MSK(ch)			(GENMASK(1, 0) << (2 * ch))
#define AD713X_ODR_RATE_SEL_CH_MODE(x, ch)		(((x) & 0x3) << (2 * ch))

/*
 * AD713X_REG_CHAN_DIG_FILTER_SEL
 */
#define AD713X_DIGFILTER_SEL_CH_MSK(ch)			(GENMASK(1, 0) << (2 * ch))
#define AD713X_DIGFILTER_SEL_CH_MODE(x, ch)		(((x) & 0x3) << (2 * ch))

/*
 * AD713X_REG_FIR_BW_SEL
 */
#define AD713X_FIR_BW_SEL_CH_MSK(ch)			BIT(ch)
#define AD713X_FIR_BW_SEL_CH_MODE(x, ch)		(((x) & 0x1) << ch)

/*
 * AD713X_REG_GPIO_DIR_CTRL
 */
#define AD713X_GPIO_IO_CTRL_MSK				GENMASK(7, 0)
#define AD713X_GPIO_IO_CTRL_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD713X_REG_GPIO_DATA
 */
#define AD713X_GPIO_DATA_MSK				GENMASK(7, 0)
#define AD713X_GPIO_DATA_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD713X_REG_ERROR_PIN_SRC_CONTROL
 */
#define AD713X_ERR_PIN_EN_OR_AIN_MSK			BIT(5)
#define AD713X_ERR_PIN_EN_OR_AIN_MODE(x)		(((x) & 0x1) << 5)
#define AD713X_ERR_PIN_EN_INTERNAL_MSK			BIT(4)
#define AD713X_ERR_PIN_EN_INTERNAL_MODE(x)		(((x) & 0x1) << 4)
#define AD713X_ERR_PIN_EN_SPI_MSK			BIT(3)
#define AD713X_ERR_PIN_EN_SPI_MODE(x)			(((x) & 0x1) << 3)
#define AD713X_ERR_PIN_EN_LDO_XOSC_MSK			BIT(2)
#define AD713X_ERR_PIN_EN_LDO_XOSC_MODE(x)		(((x) & 0x1) << 2)
#define AD713X_ERR_PIN_EN_TEMP_MSK			BIT(1)
#define AD713X_ERR_PIN_EN_TEMP_MODE(x)			(((x) & 0x1) << 1)
#define AD713X_ERR_PIN_EN_PWR_MSK			BIT(0)
#define AD713X_ERR_PIN_EN_PWR_MODE(x)			(((x) & 0x1) << 0)

/*
 * AD713X_REG_ERROR_PIN_CONTROL
 */
#define AD713X_ERR_PIN_IN_STATUS_MSK			BIT(2)
#define AD713X_ERR_PIN_IN_STATUS_MODE(x)		(((x) & 0x1) << 2)
#define AD713X_ERR_PIN_IN_EN_MSK			BIT(1)
#define AD713X_ERR_PIN_IN_EN_MODE(x)			(((x) & 0x1) << 1)
#define AD713X_ERR_PIN_OUT_EN_MSK			BIT(0)
#define AD713X_ERR_PIN_OUT_EN_MODE(x)			(((x) & 0x1) << 0)

/*
 * AD713X_REG_VCMBUF_CTRL
 */
#define AD713X_VCMBUF_CTRL_PWRDN_MSK			BIT(6)
#define AD713X_VCMBUF_CTRL_PWRDN_MODE(x)		(((x) & 0x1) << 6)
#define AD713X_VCMBUF_CTRL_REF_DIV_SEL_MSK		GENMASK(5, 1)
#define AD713X_VCMBUF_CTRL_REF_DIV_SEL_MODE(x)		(((x) & 0x1F) << 1)
#define AD713X_VCMBUF_CTRL_REF_SEL_MSK			BIT(0)
#define AD713X_VCMBUF_CTRL_REF_SEL_MODE(x)		(((x) & 0x1) << 0)

/*
 * AD713X_REG_DIAGNOSTIC_CONTROL
 */
#define AD713X_DIAGCTRL_ERR_OR_AIN_EN_MSK		BIT(5)
#define AD713X_DIAGCTRL_ERR_OR_AIN_EN_MODE(x)		(((x) & 0x1) << 5)
#define AD713X_DIAGCTRL_ERR_PWR_MON_EN_MSK		BIT(4)
#define AD713X_DIAGCTRL_ERR_PWR_MON_EN_MODE(x)		(((x) & 0x1) << 4)
#define AD713X_DIAGCTRL_MCLK_CNT_EN_MSK			BIT(3)
#define AD713X_DIAGCTRL_MCLK_CNT_EN_MODE(x)		(((x) & 0x1) << 3)
#define AD713X_DIAGCTRL_ERR_SPI_CRC_EN_MSK		BIT(2)
#define AD713X_DIAGCTRL_ERR_SPI_CRC_EN_MODE(x)		(((x) & 0x1) << 2)
#define AD713X_DIAGCTRL_ERR_MM_CRC_EN_MSK		BIT(1)
#define AD713X_DIAGCTRL_ERR_MM_CRC_EN_MODE(x)		(((x) & 0x1) << 1)
#define AD713X_DIAGCTRL_FUSE_CRC_CHECK_MSK		BIT(0)
#define AD713X_DIAGCTRL_FUSE_CRC_CHECK_MODE(x)		(((x) & 0x1) << 0)

/*
 * AD713X_REG_MPC_CONFIG
 */
#define AD713X_MPC_CLKDEL_EN_CH_MSK(ch)			(GENMASK(1, 0) << (2 * ch))
#define AD713X_MPC_CLKDEL_EN_CH_MODE(x, ch)		(((x) & 0x3) << (2 * ch))

/*
 * AD713X_REG_CHx_GAIN_LSB
 */
#define AD713X_CH_GAIN_LSB_MSK				GENMASK(7, 0)
#define AD713X_CH_GAIN_LSB_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD713X_REG_CHx_GAIN_MID
 */
#define AD713X_CH_GAIN_MID_MSK				GENMASK(7, 0)
#define AD713X_CH_GAIN_MID_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD713X_REG_CHx_GAIN_MSB
 */
#define AD713X_CH_GAIN_CAL_SEL_MSK			BIT(4)
#define AD713X_CH_GAIN_CAL_SEL_MODE(x)			(((x) & 0x1) << 4)
#define AD713X_CH_GAIN_MSB_MSK				GENMASK(3, 0)
#define AD713X_CH_GAIN_MSB_MODE(x)			(((x) & 0xF) << 0)

/*
 * AD713X_REG_CHx_OFFSET_LSB
 */
#define AD713X_CH_OFFSET_LSB_MSK			GENMASK(7, 0)
#define AD713X_CH_OFFSET_LSB_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD713X_REG_CHx_OFFSET_MID
 */
#define AD713X_CH_OFFSET_MID_MSK			GENMASK(7, 0)
#define AD713X_CH_OFFSET_MID_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD713X_REG_CHx_OFFSET_MSB
 */
#define AD713X_CH_OFFSET_CAL_EN_MSK			BIT(7)
#define AD713X_CH_OFFSET_CAL_EN_MODE(x)			(((x) & 0x1) << 7)
#define AD713X_CH_OFFSET_MSB_MSK			GENMASK(6, 0)
#define AD713X_CH_OFFSET_MSB_MODE(x)			(((x) & 0x7F) << 0)

/*
 * AD713X_REG_MCLK_COUNTER
 */
#define AD713X_MCLK_COUNT_MSK				GENMASK(7, 0)
#define AD713X_MCLK_COUNT_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD713X_REG_DIG_FILTER_OFUF
 */
#define AD713X_DIGFILTER_ERR_OFUF_CH_MSK(ch)		BIT(ch)
#define AD713X_DIGFILTER_ERR_OFUF_CH_MODE(x, ch)	(((x) & 0x1) << ch)

/*
 * AD713X_REG_DIG_FILTER_SETTLED
 */
#define AD713X_DIGFILTER_CH_SETTLED_MSK(ch)		BIT(ch)
#define AD713X_DIGFILTER_CH_SETTLED_MODE(x, ch)		(((x) & 0x1) << ch)

/*
 * AD713X_REG_INTERNAL_ERROR
 */
#define AD713X_INT_ERR_NO_CLOCK_MSK			BIT(5)
#define AD713X_INT_ERR_NO_CLOCK_MODE(x)			(((x) & 0x1) << 5)
#define AD713X_INT_ERR_TEMP_MSK				BIT(4)
#define AD713X_INT_ERR_TEMP_MODE(x)			(((x) & 0x1) << 4)
#define AD713X_INT_ERR_DCLK_MSK				BIT(3)
#define AD713X_INT_ERR_DCLK_MODE(x)			(((x) & 0x1) << 3)
#define AD713X_INT_ERR_FUSE_CRC_MSK			BIT(2)
#define AD713X_INT_ERR_FUSE_CRC_MODE(x)			(((x) & 0x1) << 2)
#define AD713X_INT_ERR_ASRC_MSK				BIT(1)
#define AD713X_INT_ERR_ASRC_MODE(x)			(((x) & 0x1) << 1)
#define AD713X_INT_ERR_MM_CRC_MSK			BIT(0)
#define AD713X_INT_ERR_MM_CRC_MODE(x)			(((x) & 0x1) << 0)

/*
 * AD713X_REG_POWER_OV_ERROR_1
 */
#define AD713X_POWER_ERR_OV_IOVDD_MSK			BIT(3)
#define AD713X_POWER_ERR_OV_IOVDD_MODE(x)		(((x) & 0x1) << 3)
#define AD713X_POWER_ERR_OV_CLKVDD_MSK			BIT(2)
#define AD713X_POWER_ERR_OV_CLKVDD_MODE(x)		(((x) & 0x1) << 2)
#define AD713X_POWER_ERR_OV_DVDD1V8_MSK			BIT(1)
#define AD713X_POWER_ERR_OV_DVDD1V8_MODE(x)		(((x) & 0x1) << 1)
#define AD713X_POWER_ERR_OV_AVDD1V8_MSK			BIT(0)
#define AD713X_POWER_ERR_OV_AVDD1V8_MODE(x)		(((x) & 0x1) << 0)

/*
 * AD713X_REG_POWER_UV_ERROR_1
 */
#define AD713X_POWER_ERR_UV_IOVDD_MSK			BIT(3)
#define AD713X_POWER_ERR_UV_IOVDD_MODE(x)		(((x) & 0x1) << 3)
#define AD713X_POWER_ERR_UV_CLKVDD_MSK			BIT(2)
#define AD713X_POWER_ERR_UV_CLKVDD_MODE(x)		(((x) & 0x1) << 2)
#define AD713X_POWER_ERR_UV_DVDD1V8_MSK			BIT(1)
#define AD713X_POWER_ERR_UV_DVDD1V8_MODE(x)		(((x) & 0x1) << 1)
#define AD713X_POWER_ERR_UV_AVDD1V8_MSK			BIT(0)
#define AD713X_POWER_ERR_UV_AVDD1V8_MODE(x)		(((x) & 0x1) << 0)

/*
 * AD713X_REG_POWER_OV_ERROR_2
 */
#define AD713X_POWER_ERR_OV_VREF_MSK			BIT(3)
#define AD713X_POWER_ERR_OV_VREF_MODE(x)		(((x) & 0x1) << 3)
#define AD713X_POWER_ERR_OV_LDOIN_MSK			BIT(2)
#define AD713X_POWER_ERR_OV_LDOIN_MODE(x)		(((x) & 0x1) << 2)
#define AD713X_POWER_ERR_OV_DVDD5_MSK			BIT(1)
#define AD713X_POWER_ERR_OV_DVDD5_MODE(x)		(((x) & 0x1) << 1)
#define AD713X_POWER_ERR_OV_AVDD5_MSK			BIT(0)
#define AD713X_POWER_ERR_OV_AVDD5_MODE(x)		(((x) & 0x1) << 0)

/*
 * AD713X_REG_POWER_UV_ERROR_2
 */
#define AD713X_POWER_ERR_UV_VREF_MSK			BIT(3)
#define AD713X_POWER_ERR_UV_VREF_MODE(x)		(((x) & 0x1) << 3)
#define AD713X_POWER_ERR_UV_LDOIN_MSK			BIT(2)
#define AD713X_POWER_ERR_UV_LDOIN_MODE(x)		(((x) & 0x1) << 2)
#define AD713X_POWER_ERR_UV_DVDD5_MSK			BIT(1)
#define AD713X_POWER_ERR_UV_DVDD5_MODE(x)		(((x) & 0x1) << 1)
#define AD713X_POWER_ERR_UV_AVDD5_MSK			BIT(0)
#define AD713X_POWER_ERR_UV_AVDD5_MODE(x)		(((x) & 0x1) << 0)

/*
 * AD713X_REG_SPI_ERROR
 */
#define AD713X_SPI_ERROR_CRC_MSK			BIT(3)
#define AD713X_SPI_ERROR_CRC_MODE(x)			(((x) & 0x1) << 3)
#define AD713X_SPI_ERROR_SCLK_CNT_MSK			BIT(2)
#define AD713X_SPI_ERROR_SCLK_CNT_MODE(x)		(((x) & 0x1) << 2)
#define AD713X_SPI_ERROR_WRITE_MSK			BIT(1)
#define AD713X_SPI_ERROR_WRITE_MODE(x)			(((x) & 0x1) << 1)
#define AD713X_SPI_ERROR_READ_MSK			BIT(0)
#define AD713X_SPI_ERROR_READ_MODE(x)			(((x) & 0x1) << 0)

/*
 * AD713X_REG_AIN_OR_ERROR
 */
#define AD713X_ERR_OR_AIN_MSK(ch)			BIT(ch)
#define AD713X_ERR_OR_AIN_MODE(x, ch)			(((x) & 0x1) << ch)

/*
 * AD713X_REG_AVDD5_VALUE
 */
#define AD713X_AVDD5_VALUE_PIN_MSK			GENMASK(7, 0)
#define AD713X_AVDD5_VALUE_PIN_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD713X_REG_DVDD5_VALUE
 */
#define AD713X_DVDD5_VALUE_PIN_MSK			GENMASK(7, 0)
#define AD713X_DVDD5_VALUE_PIN_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD713X_REG_VREF_VALUE
 */
#define AD713X_VREF_VALUE_PIN_MSK			GENMASK(7, 0)
#define AD713X_VREF_VALUE_PIN_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD713X_REG_LDOIN_VALUE
 */
#define AD713X_LDOIN_VALUE_PIN_MSK			GENMASK(7, 0)
#define AD713X_LDOIN_VALUE_PIN_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD713X_REG_AVDD1V8_VALUE
 */
#define AD713X_AVDD1V8_VALUE_PIN_MSK			GENMASK(7, 0)
#define AD713X_AVDD1V8_VALUE_PIN_MODE(x)		(((x) & 0xFF) << 0)

/*
 * AD713X_REG_DVDD1V8_VALUE
 */
#define AD713X_DVDD1V8_VALUE_PIN_MSK			GENMASK(7, 0)
#define AD713X_DVDD1V8_VALUE_PIN_MODE(x)		(((x) & 0xFF) << 0)

/*
 * AD713X_REG_CLKVDD_VALUE
 */
#define AD713X_CLKVDD_VALUE_PIN_MSK			GENMASK(7, 0)
#define AD713X_CLKVDD_VALUE_PIN_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD713X_REG_IOVDD_VALUE
 */
#define AD713X_IOVDD_VALUE_PIN_MSK			GENMASK(7, 0)
#define AD713X_IOVDD_VALUE_PIN_MODE(x)			(((x) & 0xFF) << 0)

/*
 * AD713X_REG_TEMPERATURE_DATA
 */
#define AD713X_TEMP_DATA_MSK				GENMASK(7, 0)
#define AD713X_TEMP_DATA_MODE(x)			(((x) & 0xFF) << 0)

#define AD713X_REG_READ(x) 				((1 << 7) | (x & 0x7F))

#define QUAD_CH_DC QUAD_CH_PO /* This macro is to ensure backwards compatibility. */
/* Can be removed if the aplication is updated. */

enum ad713x_supported_dev_ids {
	ID_AD7132,
	ID_AD7134,
	ID_AD7136
};

enum ad713x_power_mode {
	LOW_POWER,
	HIGH_POWER
};

enum ad713x_adc_data_len {
	ADC_16_BIT_DATA,
	ADC_24_BIT_DATA,
	ADC_32_BIT_DATA,
	INVALID
};

enum ad713x_crc_header {
	NO_CRC,
	CRC_6,
	CRC_8
};

enum ad713x_doutx_format {
	SINGLE_CH_DC,
	DUAL_CH_DC,
	QUAD_CH_PO,
	CH_AVG_MODE
};

enum ad713x_dig_filter_sel {
	FIR,
	SINC6,
	SINC3,
	SINC3_50_60_REJ
};

enum ad713x_channels {
	CH0,
	CH1,
	CH2,
	CH3
};

enum ad717x_mpc_clkdel {
	DELAY_NONE,
	DELAY_1_CLOCKS,
	DELAY_2_CLOCKS
};

static const int ad713x_output_data_frame[3][9][2] = {
	{
		{ADC_16_BIT_DATA, CRC_6},
		{ADC_24_BIT_DATA, CRC_6},
		{ADC_32_BIT_DATA, NO_CRC},
		{ADC_32_BIT_DATA, CRC_6},
		{ADC_16_BIT_DATA, NO_CRC},
		{ADC_24_BIT_DATA, NO_CRC},
		{ADC_24_BIT_DATA, CRC_8},
		{ADC_32_BIT_DATA, CRC_8},
		{INVALID}
	},
	{
		{ADC_16_BIT_DATA, NO_CRC},
		{ADC_16_BIT_DATA, CRC_6},
		{ADC_24_BIT_DATA, NO_CRC},
		{ADC_24_BIT_DATA, CRC_6},
		{ADC_16_BIT_DATA, CRC_8},
		{ADC_24_BIT_DATA, CRC_8},
		{INVALID}
	},
	{
		{ADC_16_BIT_DATA, NO_CRC},
		{ADC_16_BIT_DATA, CRC_6},
		{ADC_16_BIT_DATA, CRC_8},
		{INVALID}
	},
};

struct ad713x_dev {
	/* SPI */
	spi_desc        	*spi_desc;
	/* GPIO */
	gpio_desc		*gpio_mode1;
	gpio_desc		*gpio_mode2;
	gpio_desc		*gpio_dclkmode;
	gpio_desc		*gpio_dclkio1;
	gpio_desc		*gpio_dclkio2;
	gpio_desc		*gpio_resetn1;
	gpio_desc		*gpio_resetn2;
	gpio_desc		*gpio_pnd1;
	gpio_desc		*gpio_pnd2;
	/* Device Settings */
	enum ad713x_supported_dev_ids dev_id;
	enum ad713x_adc_data_len	adc_data_len;
	enum ad713x_crc_header	crc_header;
};

struct ad713x_init_param {
	/* SPI */
	spi_init_param      	spi_init;
	/* GPIO */
	gpio_init_param		gpio_mode1;
	gpio_init_param		gpio_mode2;
	gpio_init_param		gpio_dclkmode;
	gpio_init_param		gpio_dclkio1;
	gpio_init_param		gpio_dclkio2;
	gpio_init_param		gpio_resetn1;
	gpio_init_param		gpio_resetn2;
	gpio_init_param		gpio_pnd1;
	gpio_init_param		gpio_pnd2;
	/* Device Settings */
	enum ad713x_supported_dev_ids dev_id;
	enum ad713x_adc_data_len	adc_data_len;
	enum ad713x_crc_header	crc_header;
	enum ad713x_power_mode 	power_mode;
	enum ad713x_doutx_format	format;
	bool 			clk_delay_en;
};

int32_t ad713x_spi_reg_read(struct ad713x_dev *dev,
			    uint8_t reg_addr,
			    uint8_t *reg_data);
int32_t ad713x_spi_reg_write(struct ad713x_dev *dev,
			     uint8_t reg_addr,
			     uint8_t reg_data);
int32_t ad713x_spi_write_mask(struct ad713x_dev *dev,
			      uint8_t reg_addr,
			      uint32_t mask,
			      uint8_t data);
int32_t ad713x_set_power_mode(struct ad713x_dev *dev,
			      enum ad713x_power_mode mode);
int32_t ad713x_set_out_data_frame(struct ad713x_dev *dev,
				  enum ad713x_adc_data_len adc_data_len,
				  enum ad713x_crc_header crc_header);
int32_t ad713x_dout_format_config(struct ad713x_dev *dev,
				  enum ad713x_doutx_format format);
int32_t ad713x_mag_phase_clk_delay(struct ad713x_dev *dev,
				   bool clk_delay_en);
int32_t ad713x_dig_filter_sel_ch(struct ad713x_dev *dev,
				 enum ad713x_dig_filter_sel filter,
				 enum ad713x_channels	ch);
int32_t ad713x_init(struct ad713x_dev **device,
		    struct ad713x_init_param init_param);
int32_t ad713x_remove(struct ad713x_dev *dev);
#endif /* SRC_AD713X_H_ */

