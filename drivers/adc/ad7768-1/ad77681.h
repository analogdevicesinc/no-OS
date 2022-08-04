/***************************************************************************//**
 *   @file   ad77681.h
 *   @brief  Header file of the AD7768-1 Driver.
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

#ifndef SRC_AD77681_H_
#define SRC_AD77681_H_

#include "no_os_spi.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define	AD77681_REG_CHIP_TYPE					0x3
#define	AD77681_REG_PROD_ID_L					0x4
#define	AD77681_REG_PROD_ID_H					0x5
#define	AD77681_REG_CHIP_GRADE					0x6
#define	AD77681_REG_SCRATCH_PAD					0x0A
#define	AD77681_REG_VENDOR_L					0x0C
#define	AD77681_REG_VENDOR_H					0x0D
#define	AD77681_REG_INTERFACE_FORMAT			0x14
#define AD77681_REG_POWER_CLOCK					0x15
#define AD77681_REG_ANALOG						0x16
#define AD77681_REG_ANALOG2						0x17
#define AD77681_REG_CONVERSION					0x18
#define AD77681_REG_DIGITAL_FILTER				0x19
#define AD77681_REG_SINC3_DEC_RATE_MSB			0x1A
#define AD77681_REG_SINC3_DEC_RATE_LSB			0x1B
#define AD77681_REG_DUTY_CYCLE_RATIO			0x1C
#define AD77681_REG_SYNC_RESET					0x1D
#define AD77681_REG_GPIO_CONTROL				0x1E
#define AD77681_REG_GPIO_WRITE					0x1F
#define AD77681_REG_GPIO_READ					0x20
#define AD77681_REG_OFFSET_HI					0x21
#define AD77681_REG_OFFSET_MID					0x22
#define AD77681_REG_OFFSET_LO					0x23
#define AD77681_REG_GAIN_HI						0x24
#define AD77681_REG_GAIN_MID					0x25
#define AD77681_REG_GAIN_LO						0x26
#define AD77681_REG_SPI_DIAG_ENABLE				0x28
#define AD77681_REG_ADC_DIAG_ENABLE				0x29
#define AD77681_REG_DIG_DIAG_ENABLE				0x2A
#define	AD77681_REG_ADC_DATA					0x2C
#define	AD77681_REG_MASTER_STATUS				0x2D
#define	AD77681_REG_SPI_DIAG_STATUS				0x2E
#define	AD77681_REG_ADC_DIAG_STATUS				0x2F
#define	AD77681_REG_DIG_DIAG_STATUS				0x30
#define	AD77681_REG_MCLK_COUNTER				0x31

/* AD77681_REG_INTERFACE_FORMAT */
#define AD77681_INTERFACE_CRC_EN_MSK			(0x1 << 6)
#define AD77681_INTERFACE_CRC_EN(x)				(((x) & 0x1) << 6)
#define AD77681_INTERFACE_CRC_TYPE_MSK			(0x1 << 5)
#define AD77681_INTERFACE_CRC_TYPE(x)			(((x) & 0x1) << 5)
#define AD77681_INTERFACE_STATUS_EN_MSK			(0x1 << 4)
#define AD77681_INTERFACE_STATUS_EN(x)			(((x) & 0x1) << 4)
#define AD77681_INTERFACE_CONVLEN_MSK			(0x1 << 3)
#define AD77681_INTERFACE_CONVLEN(x)			(((x) & 0x1) << 3)
#define AD77681_INTERFACE_RDY_EN_MSK			(0x1 << 2)
#define AD77681_INTERFACE_RDY_EN(x)				(((x) & 0x1) << 3)
#define AD77681_INTERFACE_CONT_READ_MSK			(0x1 << 0)
#define AD77681_INTERFACE_CONT_READ_EN(x)		(((x) & 0x1) << 0)
#define AD77681_REG_COEFF_CONTROL				0x32
#define AD77681_REG_COEFF_DATA					0x33
#define AD77681_REG_ACCESS_KEY					0x34

/* AD77681_REG_SCRATCH_PAD*/
#define AD77681_SCRATCHPAD_MSK					(0xFF << 0)
#define AD77681_SCRATCHPAD(x)					(((x) & 0xFF) << 0)

/* AD77681_REG_POWER_CLOCK */
#define AD77681_POWER_CLK_PWRMODE_MSK			0x3
#define AD77681_POWER_CLK_PWRMODE(x)			(((x) & 0x3) << 0)
#define AD77681_POWER_CLK_MOD_OUT_MSK			(0x1 << 2)
#define AD77681_POWER_CLK_MOD_OUT(x)			(((x) & 0x1) << 2)
#define AD77681_POWER_CLK_POWER_DOWN			0x08
#define AD77681_POWER_CLK_MCLK_DIV_MSK			(0x3 << 4)
#define AD77681_POWER_CLK_MCLK_DIV(x)			(((x) & 0x3) << 4)
#define AD77681_POWER_CLK_CLOCK_SEL_MSK			(0x3 << 6)
#define AD77681_POWER_CLK_CLOCK_SEL(x)			(((x) & 0x3) << 6)

/* AD77681_CONVERSION_REG */
#define AD77681_CONVERSION_DIAG_MUX_MSK			(0xF << 4)
#define AD77681_CONVERSION_DIAG_MUX_SEL(x)		(((x) & 0xF) << 4)
#define AD77681_CONVERSION_DIAG_SEL_MSK			(0x1 << 3)
#define AD77681_CONVERSION_DIAG_SEL(x)			(((x) & 0x1) << 3)
#define AD77681_CONVERSION_MODE_MSK				(0x7 << 0)
#define AD77681_CONVERSION_MODE(x)				(((x) & 0x7) << 0)

/* AD77681_REG_ANALOG */
#define AD77681_ANALOG_REF_BUF_POS_MSK			(0x3 << 6)
#define AD77681_ANALOG_REF_BUF_POS(x)			(((x) & 0x3) << 6)
#define AD77681_ANALOG_REF_BUF_NEG_MSK			(0x3 << 4)
#define AD77681_ANALOG_REF_BUF_NEG(x)			(((x) & 0x3) << 4)
#define AD77681_ANALOG_AIN_BUF_POS_OFF_MSK		(0x1 << 1)
#define AD77681_ANALOG_AIN_BUF_POS_OFF(x)		(((x) & 0x1) << 1)
#define AD77681_ANALOG_AIN_BUF_NEG_OFF_MSK		(0x1 << 0)
#define AD77681_ANALOG_AIN_BUF_NEG_OFF(x)		(((x) & 0x1) << 0)

/* AD77681_REG_ANALOG2 */
#define AD77681_ANALOG2_VCM_MSK					(0x7 << 0)
#define AD77681_ANALOG2_VCM(x)					(((x) & 0x7) << 0)

/* AD77681_REG_DIGITAL_FILTER */
#define AD77681_DIGI_FILTER_60HZ_REJ_EN_MSK		(0x1 << 7)
#define AD77681_DIGI_FILTER_60HZ_REJ_EN(x)		(((x) & 0x1) << 7)
#define AD77681_DIGI_FILTER_FILTER_MSK			(0x7 << 4)
#define AD77681_DIGI_FILTER_FILTER(x)			(((x) & 0x7) << 4)
#define AD77681_DIGI_FILTER_DEC_RATE_MSK		(0x7 << 0)
#define AD77681_DIGI_FILTER_DEC_RATE(x)			(((x) & 0x7) << 0)

/* AD77681_REG_SINC3_DEC_RATE_MSB */
#define AD77681_SINC3_DEC_RATE_MSB_MSK			(0x0F << 0)
#define AD77681_SINC3_DEC_RATE_MSB(x)			(((x) & 0x0F) << 0)

/* AD77681_REG_SINC3_DEC_RATE_LSB */
#define AD77681_SINC3_DEC_RATE_LSB_MSK			(0xFF << 0)
#define AD77681_SINC3_DEC_RATE_LSB(x)			(((x) & 0xFF) << 0)

/* AD77681_REG_DUTY_CYCLE_RATIO */
#define AD77681_DC_RATIO_IDLE_TIME_MSK			(0xFF << 0)
#define AD77681_DC_RATIO_IDLE_TIME(x)			(((x) & 0xFF) << 0)

/* AD77681_REG_SYNC_RESET */
#define AD77681_SYNC_RST_SPI_STARTB_MSK			(0x1 << 7)
#define AD77681_SYNC_RST_SPI_STARTB(x)			(((x) & 0x1) << 7)
#define AD77681_SYNC_RST_SYNCOUT_EDGE_MSK		(0x1 << 6)
#define AD77681_SYNC_RST_SYNCOUT_EDGE(x)		(((x) & 0x1) << 6)
#define AD77681_SYNC_RST_GPIO_START_EN_MSK		(0x1 << 3)
#define AD77681_SYNC_RST_GPIO_START_EN(x)		(((x) & 0x1) << 3)
#define AD77681_SYNC_RST_SPI_RESET_MSK			(0x3 << 0)
#define AD77681_SYNC_RST_SPI_RESET(x)			(((x) & 0x3) << 0)

/* AD77681_REG_GPIO_CONTROL */
#define AD77681_GPIO_CNTRL_UGPIO_EN_MSK			(0x1 << 7)
#define AD77681_GPIO_CNTRL_UGPIO_EN(x)			(((x) & 0x1) << 7)
#define AD77681_GPIO_CNTRL_GPIO2_OD_EN_MSK		(0x1 << 6)
#define AD77681_GPIO_CNTRL_GPIO2_OD_EN(x)		(((x) & 0x1) << 6)
#define AD77681_GPIO_CNTRL_GPIO1_OD_EN_MSK		(0x1 << 5)
#define AD77681_GPIO_CNTRL_GPIO1_OD_EN(x)		(((x) & 0x1) << 5)
#define AD77681_GPIO_CNTRL_GPIO0_OD_EN_MSK		(0x1 << 4)
#define AD77681_GPIO_CNTRL_GPIO0_OD_EN(x)		(((x) & 0x1) << 4)
#define AD77681_GPIO_CNTRL_ALL_GPIOS_OD_EN_MSK	(0x7 << 4)
#define AD77681_GPIO_CNTRL_ALL_GPIOS_OD_EN(x)	(((x) & 0x7) << 4)
#define AD77681_GPIO_CNTRL_GPIO3_OP_EN_MSK		(0x1 << 3)
#define AD77681_GPIO_CNTRL_GPIO3_OP_EN(x)		(((x) & 0x1) << 3)
#define AD77681_GPIO_CNTRL_GPIO2_OP_EN_MSK		(0x1 << 2)
#define AD77681_GPIO_CNTRL_GPIO2_OP_EN(x)		(((x) & 0x1) << 2)
#define AD77681_GPIO_CNTRL_GPIO1_OP_EN_MSK		(0x1 << 1)
#define AD77681_GPIO_CNTRL_GPIO1_OP_EN(x)		(((x) & 0x1) << 1)
#define AD77681_GPIO_CNTRL_GPIO0_OP_EN_MSK		(0x1 << 0)
#define AD77681_GPIO_CNTRL_GPIO0_OP_EN(x)		(((x) & 0x1) << 0)
#define AD77681_GPIO_CNTRL_ALL_GPIOS_OP_EN_MSK	(0xF << 0)
#define AD77681_GPIO_CNTRL_ALL_GPIOS_OP_EN(x)	(((x) & 0xF) << 0)

/* AD77681_REG_GPIO_WRITE */
#define AD77681_GPIO_WRITE_3_MSK				(0x1 << 3)
#define AD77681_GPIO_WRITE_3(x)					(((x) & 0x1) << 3)
#define AD77681_GPIO_WRITE_2_MSK				(0x1 << 2)
#define AD77681_GPIO_WRITE_2(x)					(((x) & 0x1) << 2)
#define AD77681_GPIO_WRITE_1_MSK				(0x1 << 1)
#define AD77681_GPIO_WRITE_1(x)					(((x) & 0x1) << 1)
#define AD77681_GPIO_WRITE_0_MSK				(0x1 << 0)
#define AD77681_GPIO_WRITE_0(x)					(((x) & 0x1) << 0)
#define AD77681_GPIO_WRITE_ALL_MSK				(0xF << 0)
#define AD77681_GPIO_WRITE_ALL(x)				(((x) & 0xF))

/* AD77681_REG_GPIO_READ */
#define AD77681_GPIO_READ_3_MSK					(0x1 << 3)
#define AD77681_GPIO_READ_2_MSK					(0x1 << 2)
#define AD77681_GPIO_READ_1_MSK					(0x1 << 1)
#define AD77681_GPIO_READ_0_MSK					(0x1 << 0)
#define AD77681_GPIO_READ_ALL_MSK				(0xF << 0)

/* AD77681_REG_OFFSET_HI */
#define AD77681_OFFSET_HI_MSK					(0xFF << 0)
#define	AD77681_OFFSET_HI(x)					(((x) & 0xFF) << 0)

/* AD77681_REG_OFFSET_MID */
#define AD77681_OFFSET_MID_MSK					(0xFF << 0)
#define	AD77681_OFFSET_MID(x)					(((x) & 0xFF) << 0)

/* AD77681_REG_OFFSET_LO */
#define AD77681_OFFSET_LO_MSK					(0xFF << 0)
#define	AD77681_OFFSET_LO(x)					(((x) & 0xFF) << 0)

/* AD77681_REG_GAIN_HI */
#define AD77681_GAIN_HI_MSK						(0xFF << 0)
#define	AD77681_GAIN_HI(x)						(((x) & 0xFF) << 0)

/* AD77681_REG_GAIN_MID */
#define AD77681_GAIN_MID_MSK					(0xFF << 0)
#define	AD77681_GAIN_MID(x)						(((x) & 0xFF) << 0)

/* AD77681_REG_GAIN_HI */
#define AD77681_GAIN_LOW_MSK					(0xFF << 0)
#define	AD77681_GAIN_LOW(x)						(((x) & 0xFF) << 0)

/* AD77681_REG_SPI_DIAG_ENABLE */
#define AD77681_SPI_DIAG_ERR_SPI_IGNORE_MSK		(0x1 << 4)
#define AD77681_SPI_DIAG_ERR_SPI_IGNORE(x)		(((x) & 0x1) << 4)
#define AD77681_SPI_DIAG_ERR_SPI_CLK_CNT_MSK	(0x1 << 3)
#define AD77681_SPI_DIAG_ERR_SPI_CLK_CNT(x)		(((x) & 0x1) << 3)
#define AD77681_SPI_DIAG_ERR_SPI_RD_MSK			(0x1 << 2)
#define AD77681_SPI_DIAG_ERR_SPI_RD(x)			(((x) & 0x1) << 2)
#define AD77681_SPI_DIAG_ERR_SPI_WR_MSK			(0x1 << 1)
#define AD77681_SPI_DIAG_ERR_SPI_WR(x)			(((x) & 0x1) << 1)

/* AD77681_REG_ADC_DIAG_ENABLE */
#define AD77681_ADC_DIAG_ERR_DLDO_PSM_MSK		(0x1 << 5)
#define AD77681_ADC_DIAG_ERR_DLDO_PSM(x)		(((x) & 0x1) << 5)
#define AD77681_ADC_DIAG_ERR_ALDO_PSM_MSK		(0x1 << 4)
#define AD77681_ADC_DIAG_ERR_ALDO_PSM(x)		(((x) & 0x1) << 4)
#define AD77681_ADC_DIAG_ERR_FILT_SAT_MSK		(0x1 << 2)
#define AD77681_ADC_DIAG_ERR_FILT_SAT(x)			(((x) & 0x1) << 2)
#define AD77681_ADC_DIAG_ERR_FILT_NOT_SET_MSK	(0x1 << 1)
#define AD77681_ADC_DIAG_ERR_FILT_NOT_SET(x)		(((x) & 0x1) << 1)
#define AD77681_ADC_DIAG_ERR_EXT_CLK_QUAL_MSK	(0x1 << 0)
#define AD77681_ADC_DIAG_ERR_EXT_CLK_QUAL(x)	(((x) & 0x1) << 0)

/* AD77681_REG_DIG_DIAG_ENABLE */
#define AD77681_DIG_DIAG_ERR_MEMMAP_CRC_MSK		(0x1 << 4)
#define AD77681_DIG_DIAG_ERR_MEMMAP_CRC(x)		(((x) & 0x1) << 4)
#define AD77681_DIG_DIAG_ERR_RAM_CRC_MSK		(0x1 << 3)
#define AD77681_DIG_DIAG_ERR_RAM_CRC(x)			(((x) & 0x1) << 3)
#define AD77681_DIG_DIAG_ERR_FUSE_CRC_MSK		(0x1 << 2)
#define AD77681_DIG_DIAG_ERR_FUSE_CRC(x)		(((x) & 0x1) << 2)
#define AD77681_DIG_DIAG_FREQ_COUNT_EN_MSK		(0x1 << 0)
#define AD77681_DIG_DIAG_FREQ_COUNT_EN(x)		(((x) & 0x1) << 0)

/* AD77681_REG_MASTER_STATUS */
#define AD77681_MASTER_ERROR_MSK				(0x1 << 7)
#define AD77681_MASTER_ADC_ERROR_MSK			(0x1 << 6)
#define AD77681_MASTER_DIG_ERROR_MSK			(0x1 << 5)
#define AD77681_MASTER_DIG_ERR_EXT_CLK_MSK		(0x1 << 4)
#define AD77681_MASTER_FILT_SAT_MSK				(0x1 << 3)
#define AD77681_MASTER_FILT_NOT_SET_MSK			(0x1 << 2)
#define AD77681_MASTER_SPI_ERROR_MSK			(0x1 << 1)
#define AD77681_MASTER_POR_FLAG_MSK				(0x1 << 0)

/* AD77681_REG_SPI_DIAG_STATUS */
#define AD77681_SPI_IGNORE_ERROR_MSK			(0x1 << 4)
#define AD77681_SPI_IGNORE_ERROR_CLR(x)			(((x) & 0x1) << 4)
#define AD77681_SPI_CLK_CNT_ERROR_MSK			(0x1 << 3)
#define AD77681_SPI_READ_ERROR_MSK				(0x1 << 2)
#define AD77681_SPI_READ_ERROR_CLR(x)			(((x) & 0x1) << 2)
#define AD77681_SPI_WRITE_ERROR_MSK				(0x1 << 1)
#define AD77681_SPI_WRITE_ERROR_CLR(x)			(((x) & 0x1) << 1)
#define AD77681_SPI_CRC_ERROR_MSK				(0x1 << 0)
#define AD77681_SPI_CRC_ERROR_CLR(x)			(((x) & 0x1) << 0)

/* AD77681_REG_ADC_DIAG_STATUS */
#define AD77681_ADC_DLDO_PSM_ERROR_MSK			(0x1 << 5)
#define AD77681_ADC_ALDO_PSM_ERROR_MSK			(0x1 << 4)
#define AD77681_ADC_REF_DET_ERROR_MSK           (0x1 << 3)
#define AD77681_ADC_FILT_SAT_MSK				(0x1 << 2)
#define AD77681_ADC_FILT_NOT_SET_MSK			(0x1 << 1)
#define AD77681_ADC_DIG_ERR_EXT_CLK_MSK			(0x1 << 0)

/* AD77681_REG_DIG_DIAG_STATUS */
#define AD77681_DIG_MEMMAP_CRC_ERROR_MSK		(0x1 << 4)
#define AD77681_DIG_RAM_CRC_ERROR_MSK			(0x1 << 3)
#define AD77681_DIG_FUS_CRC_ERROR_MSK			(0x1 << 2)

/* AD77681_REG_MCLK_COUNTER */
#define AD77681_MCLK_COUNTER_MSK				(0xFF << 0)
#define	AD77681_MCLK_COUNTER(x)					(((x) & 0xFF) << 0)

/* AD77681_REG_COEFF_CONTROL */
#define AD77681_COEF_CONTROL_COEFFACCESSEN_MSK	(0x1 << 7)
#define AD77681_COEF_CONTROL_COEFFACCESSEN(x)	(((x) & 0x1) << 7)
#define AD77681_COEF_CONTROL_COEFFWRITEEN_MSK	(0x1 << 6)
#define AD77681_COEF_CONTROL_COEFFWRITEEN(x)	(((x) & 0x1) << 6)
#define AD77681_COEF_CONTROL_COEFFADDR_MSK		(0x3F << 5)
#define AD77681_COEF_CONTROL_COEFFADDR(x)		(((x) & 0x3F) << 5)

/* AD77681_REG_COEFF_DATA */
#define AD77681_COEFF_DATA_USERCOEFFEN_MSK		(0x1 << 23)
#define	AD77681_COEFF_DATA_USERCOEFFEN(x)		(((x) & 0x1) << 23)
#define AD77681_COEFF_DATA_COEFFDATA_MSK		(0x7FFFFF << 22)
#define	AD77681_COEFF_DATA_COEFFDATA(x)			(((x) & 0x7FFFFF) << 22)

/* AD77681_REG_ACCESS_KEY */
#define AD77681_ACCESS_KEY_MSK					(0xFF << 0)
#define	AD77681_ACCESS_KEY(x)					(((x) & 0xFF) << 0)
#define AD77681_ACCESS_KEY_CHECK_MSK			(0x1 << 0)

#define AD77681_REG_READ(x)						( (1 << 6) | (x & 0xFF) )		// Read from register x
#define AD77681_REG_WRITE(x)					( (~(1 << 6)) & (x & 0xFF) )  	// Write to register x

/* 8-bits wide checksum generated using the polynomial */
#define AD77681_CRC8_POLY	0x07 // x^8 + x^2 + x^1 + x^0

/* Initial CRC for continuous read mode */
#define INITIAL_CRC_CRC8						0x03
#define INITIAL_CRC_XOR							0x6C
#define INITIAL_CRC								0x00

#define CRC_DEBUG

/* AD7768-1 */
/* A special key for exit the contiuous read mode, taken from the AD7768-1 datasheet */
#define EXIT_CONT_READ							0x6C
/* Bit resolution of the AD7768-1 */
#define AD7768_N_BITS							24
/* Full scale of the AD7768-1 = 2^24 = 16777216 */
#define AD7768_FULL_SCALE						(1 << AD7768_N_BITS)
/* Half scale of the AD7768-1 = 2^23 = 8388608 */
#define AD7768_HALF_SCALE						(1 << (AD7768_N_BITS - 1))

#define ENABLE		1
#define DISABLE		0

/*****************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
enum ad77681_power_mode {
	AD77681_ECO = 0,
	AD77681_MEDIAN = 2,
	AD77681_FAST = 3,
};

enum ad77681_mclk_div {
	AD77681_MCLK_DIV_16 = 0,
	AD77681_MCLK_DIV_8 = 1,
	AD77681_MCLK_DIV_4 = 2,
	AD77681_MCLK_DIV_2 = 3
};

enum ad77681_conv_mode {
	AD77681_CONV_CONTINUOUS = 0,
	AD77681_CONV_ONE_SHOT = 1,
	AD77681_CONV_SINGLE = 2,
	AD77681_CONV_PERIODIC = 3,
	AD77681_CONV_STANDBY = 4
};

enum ad77681_conv_len {
	AD77681_CONV_24BIT = 0,
	AD77681_CONV_16BIT = 1
};

enum ad77681_rdy_dout {
	AD77681_RDY_DOUT_EN,
	AD77681_RDY_DOUT_DIS
};

enum ad77681_conv_diag_mux {
	AD77681_TEMP_SENSOR = 0x0,
	AD77681_AIN_SHORT= 0x8,
	AD77681_POSITIVE_FS = 0x9,
	AD77681_NEGATIVE_FS = 0xA
};

enum ad77681_crc_sel {
	AD77681_CRC,
	AD77681_XOR,
	AD77681_NO_CRC
};

/* Filter tye FIR, SINC3, SINC5 */
enum ad77681_filter_type {
	AD77681_SINC5			= 0,
	AD77681_SINC5_DECx8		= 1,
	AD77681_SINC5_DECx16	= 2,
	AD77681_SINC3			= 3,
	AD77681_FIR				= 4
};

/* Dectimation ratios for SINC5 and FIR */
enum ad77681_sinc5_fir_decimate {
	AD77681_SINC5_FIR_DECx32	= 0,
	AD77681_SINC5_FIR_DECx64	= 1,
	AD77681_SINC5_FIR_DECx128	= 2,
	AD77681_SINC5_FIR_DECx256	= 3,
	AD77681_SINC5_FIR_DECx512	= 4,
	AD77681_SINC5_FIR_DECx1024	= 5
};

/* Sleep / Power up */
enum ad77681_sleep_wake {
	AD77681_SLEEP   = 1,
	AD77681_WAKE = 0
};

/* Reset option */
enum ad7761_reset_option {
	AD77681_SOFT_RESET,
	AD77681_HARD_RESET
};
/* AIN- precharge */
enum ad77681_AINn_precharge {
	AD77681_AINn_ENABLED   = 0,
	AD77681_AINn_DISABLED  = 1
};

/* AIN+ precharge */
enum ad77681_AINp_precharge {
	AD77681_AINp_ENABLED  = 0,
	AD77681_AINp_DISABLED = 1
};

/* REF- buffer */
enum ad77681_REFn_buffer {
	AD77681_BUFn_ENABLED        = 0,
	AD77681_BUFn_DISABLED       = 1,
	AD77681_BUFn_FULL_BUFFER_ON = 2
};

/* REF+ buffer */
enum ad77681_REFp_buffer {
	AD77681_BUFp_ENABLED        = 0,
	AD77681_BUFp_DISABLED       = 1,
	AD77681_BUFp_FULL_BUFFER_ON = 2
};

/* VCM output voltage */
enum ad77681_VCM_out {
	AD77681_VCM_HALF_VCC	= 0,
	AD77681_VCM_2_5V		= 1,
	AD77681_VCM_2_05V   	= 2,
	AD77681_VCM_1_9V		= 3,
	AD77681_VCM_1_65V		= 4,
	AD77681_VCM_1_1V		= 5,
	AD77681_VCM_0_9V		= 6,
	AD77681_VCM_OFF  		= 7
};

/* Global GPIO enable/disable */
enum ad77681_gobal_gpio_enable {
	AD77681_GLOBAL_GPIO_ENABLE		= 1,
	AD77681_GLOBAL_GPIO_DISABLE		= 0
};

/* ADCs GPIO numbering */
enum ad77681_gpios {
	AD77681_GPIO0					= 0,
	AD77681_GPIO1					= 1,
	AD77681_GPIO2					= 2,
	AD77681_GPIO3					= 3,
	AD77681_ALL_GPIOS				= 4
};

enum ad77681_gpio_output_type {
	AD77681_GPIO_STRONG_DRIVER		= 0,
	AD77681_GPIO_OPEN_DRAIN			= 1
};

/* Continuous ADC read */
enum ad77681_continuous_read {
	AD77681_CONTINUOUS_READ_ENABLE = 1,
	AD77681_CONTINUOUS_READ_DISABLE = 0,
};

/* ADC data read mode */
enum ad77681_data_read_mode {
	AD77681_REGISTER_DATA_READ = 0,
	AD77681_CONTINUOUS_DATA_READ = 1,
};

/* ADC data structure */
struct adc_data {
	bool		finish;
	uint16_t	count;
	uint16_t	samples;
	uint32_t	raw_data[4096];
};
/* ADC status registers structure */
struct ad77681_status_registers {
	bool							master_error;
	bool							adc_error;
	bool							dig_error;
	bool							adc_err_ext_clk_qual;
	bool							adc_filt_saturated;
	bool							adc_filt_not_settled;
	bool							spi_error;
	bool							por_flag;
	bool							spi_ignore;
	bool							spi_clock_count;
	bool							spi_read_error;
	bool							spi_write_error;
	bool							spi_crc_error;
	bool							dldo_psm_error;
	bool							aldo_psm_error;
	bool                            ref_det_error;
	bool                            filt_sat_error;
	bool                            filt_not_set_error;
	bool                            ext_clk_qual_error;
	bool							memoy_map_crc_error;
	bool							ram_crc_error;
	bool							fuse_crc_error;
};

struct ad77681_dev {
	/* SPI */
	struct no_os_spi_desc			*spi_desc;
	/* Configuration */
	enum ad77681_power_mode		power_mode;
	enum ad77681_mclk_div		mclk_div;
	enum ad77681_conv_mode 		conv_mode;
	enum ad77681_conv_diag_mux 	diag_mux_sel;
	bool 					conv_diag_sel;
	enum ad77681_conv_len		conv_len;
	enum ad77681_crc_sel 		crc_sel;
	uint8_t					status_bit;
	enum ad77681_VCM_out            VCM_out;
	enum ad77681_AINn_precharge     AINn;
	enum ad77681_AINp_precharge     AINp;
	enum ad77681_REFn_buffer        REFn;
	enum ad77681_REFp_buffer        REFp;
	enum ad77681_filter_type        filter;
	enum ad77681_sinc5_fir_decimate decimate;
	uint16_t                        sinc3_osr;
	uint16_t                        vref;               /* Reference voltage*/
	uint16_t                        mclk;               /* Mater clock*/
	uint32_t                        sample_rate;        /* Sample rate*/
	uint8_t                         data_frame_byte;    /* SPI 8bit frames*/
};

struct ad77681_init_param {
	/* SPI */
	struct no_os_spi_init_param			spi_eng_dev_init;
	/* Configuration */
	enum ad77681_power_mode		power_mode;
	enum ad77681_mclk_div		mclk_div;
	enum ad77681_conv_mode 		conv_mode;
	enum ad77681_conv_diag_mux 	diag_mux_sel;
	bool 					conv_diag_sel;
	enum ad77681_conv_len		conv_len;
	enum ad77681_crc_sel 		crc_sel;
	uint8_t					status_bit;
	enum ad77681_VCM_out            VCM_out;
	enum ad77681_AINn_precharge     AINn;
	enum ad77681_AINp_precharge     AINp;
	enum ad77681_REFn_buffer        REFn;
	enum ad77681_REFp_buffer        REFp;
	enum ad77681_filter_type        filter;
	enum ad77681_sinc5_fir_decimate decimate;
	uint16_t                        sinc3_osr;
	uint16_t                        vref;
	uint16_t                        mclk;
	uint32_t                        sample_rate;
	uint8_t                         data_frame_byte;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
uint8_t ad77681_compute_crc8(uint8_t *data,
			     uint8_t data_size,
			     uint8_t init_val);
uint8_t ad77681_compute_xor(uint8_t *data,
			    uint8_t data_size,
			    uint8_t init_val);
int32_t ad77681_setup(struct ad77681_dev **device,
		      struct ad77681_init_param init_param,
		      struct ad77681_status_registers **status);
int32_t ad77681_spi_reg_read(struct ad77681_dev *dev,
			     uint8_t reg_addr,
			     uint8_t *reg_data);
int32_t ad77681_spi_read_mask(struct ad77681_dev *dev,
			      uint8_t reg_addr,
			      uint8_t mask,
			      uint8_t *data);
int32_t ad77681_spi_reg_write(struct ad77681_dev *dev,
			      uint8_t reg_addr,
			      uint8_t reg_data);
int32_t ad77681_spi_write_mask(struct ad77681_dev *dev,
			       uint8_t reg_addr,
			       uint8_t mask,
			       uint8_t data);
int32_t ad77681_set_power_mode(struct ad77681_dev *dev,
			       enum ad77681_power_mode mode);
int32_t ad77681_set_mclk_div(struct ad77681_dev *dev,
			     enum ad77681_mclk_div clk_div);
int32_t ad77681_spi_read_adc_data(struct ad77681_dev *dev,
				  uint8_t *adc_data,
				  enum ad77681_data_read_mode mode);
int32_t ad77681_set_conv_mode(struct ad77681_dev *dev,
			      enum ad77681_conv_mode conv_mode,
			      enum ad77681_conv_diag_mux diag_mux_sel,
			      bool conv_diag_sel);
int32_t ad77681_set_convlen(struct ad77681_dev *dev,
			    enum ad77681_conv_len conv_len);
int32_t ad77681_soft_reset(struct ad77681_dev *dev);
int32_t ad77681_initiate_sync(struct ad77681_dev *dev);
int32_t ad77681_programmable_filter(struct ad77681_dev *dev,
				    const float *coeffs,
				    uint8_t num_coeffs);
int32_t ad77681_gpio_read(struct ad77681_dev *dev,
			  uint8_t *value,
			  enum ad77681_gpios gpio_number);
int32_t ad77681_apply_offset(struct ad77681_dev *dev,
			     uint32_t value);
int32_t ad77681_apply_gain(struct ad77681_dev *dev,
			   uint32_t value);
int32_t ad77681_set_crc_sel(struct ad77681_dev *dev,
			    enum ad77681_crc_sel crc_sel);
int32_t ad77681_gpio_open_drain(struct ad77681_dev *dev,
				enum ad77681_gpios gpio_number,
				enum ad77681_gpio_output_type output_type);
int32_t ad77681_set_continuos_read(struct ad77681_dev *dev,
				   enum ad77681_continuous_read continuous_enable);
int32_t ad77681_clear_error_flags(struct ad77681_dev *dev);
int32_t ad77681_data_to_voltage(struct ad77681_dev *dev,
				uint32_t *raw_code,
				double *voltage);
int32_t ad77681_CRC_status_handling(struct ad77681_dev *dev,
				    uint16_t *data_buffer);
int32_t ad77681_set_AINn_buffer(struct ad77681_dev *dev,
				enum ad77681_AINn_precharge AINn);
int32_t ad77681_set_AINp_buffer(struct ad77681_dev *dev,
				enum ad77681_AINp_precharge AINp);
int32_t ad77681_set_REFn_buffer(struct ad77681_dev *dev,
				enum ad77681_REFn_buffer REFn);
int32_t ad77681_set_REFp_buffer(struct ad77681_dev *dev,
				enum ad77681_REFp_buffer REFp);
int32_t ad77681_set_filter_type(struct ad77681_dev *dev,
				enum ad77681_sinc5_fir_decimate decimate,
				enum ad77681_filter_type filter,
				uint16_t sinc3_osr);
int32_t ad77681_set_50HZ_rejection(struct ad77681_dev *dev,
				   uint8_t enable);
int32_t ad77681_power_down(struct ad77681_dev *dev,
			   enum ad77681_sleep_wake sleep_wake);
int32_t ad77681_set_status_bit(struct ad77681_dev *dev,
			       bool status_bit);
int32_t ad77681_set_VCM_output(struct ad77681_dev *dev,
			       enum ad77681_VCM_out VCM_out);
int32_t ad77681_gpio_write(struct ad77681_dev *dev,
			   uint8_t value,
			   enum ad77681_gpios gpio_number);
int32_t ad77681_gpio_inout(struct ad77681_dev *dev,
			   uint8_t direction,
			   enum ad77681_gpios gpio_number);
int32_t ad77681_global_gpio(struct ad77681_dev *devices,
			    enum ad77681_gobal_gpio_enable gpio_enable);
int32_t ad77681_scratchpad(struct ad77681_dev *dev,
			   uint8_t *sequence);
int32_t ad77681_error_flags_enabe(struct ad77681_dev *dev);
int32_t ad77681_update_sample_rate(struct ad77681_dev *dev);
int32_t ad77681_SINC3_ODR(struct ad77681_dev *dev,
			  uint16_t *sinc3_dec_reg,
			  float sinc3_odr);
int32_t ad77681_status(struct ad77681_dev *dev,
		       struct ad77681_status_registers *status);
#endif /* SRC_AD77681_H_ */
