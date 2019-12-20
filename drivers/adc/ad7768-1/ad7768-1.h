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

#ifndef AD77681_H_
#define AD77681_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "spi.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD77681_REG_CHIP_TYPE			0x03
#define AD77681_REG_PROD_ID_L			0x04
#define AD77681_REG_PROD_ID_H			0x05
#define AD77681_REG_CHIP_GRADE			0x06
#define AD77681_REG_SCRATCH_PAD			0x0A
#define AD77681_REG_VENDOR_L			0x0C
#define AD77681_REG_VENDOR_H			0x0D
#define AD77681_REG_INTERFACE_FORMAT		0x14
#define AD77681_REG_POWER_CLOCK			0x15
#define AD77681_REG_ANALOG			0x16
#define AD77681_REG_ANALOG2			0x17
#define AD77681_REG_CONVERSION			0x18
#define AD77681_REG_DIGITAL_FILTER		0x19
#define AD77681_REG_SINC3_DEC_RATE_MSB		0x1A
#define AD77681_REG_SINC3_DEC_RATE_LSB		0x1B
#define AD77681_REG_DUTY_CYCLE_RATIO		0x1C
#define AD77681_REG_SYNC_RESET			0x1D
#define AD77681_REG_GPIO_CONTROL		0x1E
#define AD77681_REG_GPIO_WRITE			0x1F
#define AD77681_REG_GPIO_READ			0x20
#define AD77681_REG_OFFSET_HI			0x21
#define AD77681_REG_OFFSET_MID			0x22
#define AD77681_REG_OFFSET_LO			0x23
#define AD77681_REG_GAIN_HI			0x24
#define AD77681_REG_GAIN_MID			0x25
#define AD77681_REG_GAIN_LO			0x26
#define AD77681_REG_SPI_DIAG_ENABLE		0x28
#define AD77681_REG_ADC_DIAG_ENABLE		0x29
#define AD77681_REG_DIG_DIAG_ENABLE		0x2A
#define AD77681_REG_ADC_DATA			0x2C
#define AD77681_REG_MASTER_STATUS		0x2D
#define AD77681_REG_SPI_DIAG_STATUS		0x2E
#define AD77681_REG_ADC_DIAG_STATUS		0x2F
#define AD77681_REG_DIG_DIAG_STATUS		0x30
#define AD77681_REG_MCLK_COUNTER		0x31

/* AD77681_REG_INTERFACE_FORMAT */
#define AD77681_INTERFACE_CRC_EN_MSK		(0x1 << 6)
#define AD77681_INTERFACE_CRC_EN(x)		(((x) & 0x1) << 6)
#define AD77681_INTERFACE_CRC_TYPE_MSK		(0x1 << 5)
#define AD77681_INTERFACE_CRC_TYPE(x)		(((x) & 0x1) << 5)
#define AD77681_INTERFACE_STATUS_EN_MSK 	(0x1 << 4)
#define AD77681_INTERFACE_STATUS_EN(x)		(((x) & 0x1) << 4)
#define AD77681_INTERFACE_CONVLEN_MSK		(0x1 << 3)
#define AD77681_INTERFACE_CONVLEN(x)		(((x) & 0x1) << 3)
#define AD77681_INTERFACE_RDY_EN_MSK		(0x1 << 2)
#define AD77681_INTERFACE_RDY_EN(x)		(((x) & 0x1) << 3)
#define AD77681_INTERFACE_CONT_READ_MSK 	(0x1 << 0)
#define AD77681_INTERFACE_CONT_READ_EN(x)	(((x) & 0x1) << 0)

/* AD77681_REG_POWER_CLOCK */
#define AD77681_POWER_CLK_PWRMODE_MSK		0x03
#define AD77681_POWER_CLK_PWRMODE(x)		(((x) & 0x3) << 0)
#define AD77681_POWER_CLK_MOD_OUT_MSK		(0x1 << 2)
#define AD77681_POWER_CLK_MOD_OUT(x)		(((x) & 0x1) << 2)
#define AD77681_POWER_CLK_POWER_DOWN		0x08
#define AD77681_POWER_CLK_MCLK_DIV_MSK		(0x3 << 4)
#define AD77681_POWER_CLK_MCLK_DIV(x)		(((x) & 0x3) << 4)
#define AD77681_POWER_CLK_CLOCK_SEL_MSK 	(0x3 << 6)
#define AD77681_POWER_CLK_CLOCK_SEL(x)		(((x) & 0x3) << 6)

/* AD77681_CONVERSION_REG */
#define AD77681_CONVERSION_DIAG_MUX_MSK 	(0xF << 4)
#define AD77681_CONVERSION_DIAG_MUX_SEL(x)	(((x) & 0xF) << 4)
#define AD77681_CONVERSION_DIAG_SEL_MSK 	(0x1 << 3)
#define AD77681_CONVERSION_DIAG_SEL(x)		(((x) & 0x1) << 3)
#define AD77681_CONVERSION_MODE_MSK		(0x7 << 0)
#define AD77681_CONVERSION_MODE(x)		(((x) & 0x7) << 0)

/* AD77681_REG_ANALOG */
#define AD77681_ANALOG_REF_BUF_POS_MSK		(0x3 << 6)
#define AD77681_ANALOG_REF_BUF_POS(x)		(((x) & 0x3) << 6)
#define AD77681_ANALOG_REF_BUF_NEG_MSK		(0x3 << 4)
#define AD77681_ANALOG_REF_BUF_NEG(x)		(((x) & 0x3) << 4)
#define AD77681_ANALOG_REF_BUF_POS_OFF_MSK	(0x1 << 1)
#define AD77681_ANALOG_REF_BUF_POS_OFF(x)	(((x) & 0x1) << 1)
#define AD77681_ANALOG_REF_BUF_NEG_OFF_MSK	(0x1 << 0)
#define AD77681_ANALOG_REF_BUF_NEG_OFF(x)	(((x) & 0x1) << 0)

/* AD77681_REG_ANALOG2 */
#define AD77681_ANALOG2_VCM_MSK 		(0x7 << 0)
#define AD77681_ANALOG2_VCM(x)			(((x) & 0x7) << 0)

/* AD77681_REG_DIGITAL_FILTER */
#define AD77681_DIGI_FILTER_60HZ_REJ_EN_MSK	(0x1 << 7)
#define AD77681_DIGI_FILTER_60HZ_REJ_EN(X)	(((x) & 0x1) << 7)
#define AD77681_DIGI_FILTER_FILTER_MSK		(0x7 << 4)
#define AD77681_DIGI_FILTER_FILTER(X)		(((x) & 0x7) << 4)
#define AD77681_DIGI_FILTER_DEC_RATE_MSK	(0x7 << 0)
#define AD77681_DIGI_FILTER_DEC_RATE(X) 	(((x) & 0x7) << 0)

/* AD77681_REG_SINC3_DEC_RATE_MSB */
#define AD77681_SINC3_DEC_RATE_MSB_MSK		(0x0F << 0)
#define AD77681_SINC3_DEC_RATE_MSB(x)		(((x) & 0x0F) << 0)

/* AD77681_REG_SINC3_DEC_RATE_LSB */
#define AD77681_SINC3_DEC_RATE_LSB_MSK		(0xFF << 0)
#define AD77681_SINC3_DEC_RATE_LSB(x)		(((x) & 0xFF) << 0)

/* AD77681_REG_DUTY_CYCLE_RATIO */
#define AD77681_DC_RATIO_IDLE_TIME_MSK		(0xFF << 0)
#define AD77681_DC_RATIO_IDLE_TIME(x)		(((x) & 0xFF) << 0)

/* AD77681_REG_SYNC_RESET */
#define AD77681_SYNC_RST_SPI_STARTB_MSK 	(0x1 << 7)
#define AD77681_SYNC_RST_SYNCOUT_EDGE_MSK	(0x1 << 6)
#define AD77681_SYNC_RST_SYNCOUT_EDGE(x)	(((x) & 0x1) << 6)
#define AD77681_SYNC_RST_GPIO_START_EN_MSK	(0x1 << 3)
#define AD77681_SYNC_RST_GPIO_START_EN(x)	(((x) & 0x1) << 3)
#define AD77681_SYNC_RST_SPI_RESET_MSK		(0x3 << 0)
#define AD77681_SYNC_RST_SPI_RESET(x)		(((x) & 0x3) << 0)

/* AD77681_REG_GPIO_CONTROL */
#define AD77681_GPIO_CNTRL_UGPIO_EN_MSK 	(0x1 << 7)
#define AD77681_GPIO_CNTRL_UGPIO_EN(x)		(((x) & 0x1) << 7)
#define AD77681_GPIO_CNTRL_GPIO2_OD_EN_MSK	(0x1 << 6)
#define AD77681_GPIO_CNTRL_GPIO2_OD_EN(x)	(((x) & 0x1) << 6)
#define AD77681_GPIO_CNTRL_GPIO1_OD_EN_MSK	(0x1 << 5)
#define AD77681_GPIO_CNTRL_GPIO1_OD_EN(x)	(((x) & 0x1) << 5)
#define AD77681_GPIO_CNTRL_GPIO0_OD_EN_MSK	(0x1 << 4)
#define AD77681_GPIO_CNTRL_GPIO0_OD_EN(x)	(((x) & 0x1) << 4)
#define AD77681_GPIO_CNTRL_GPIO3_OP_EN_MSK	(0x1 << 3)
#define AD77681_GPIO_CNTRL_GPIO3_OP_EN(x)	(((x) & 0x1) << 3)
#define AD77681_GPIO_CNTRL_GPIO2_OP_EN_MSK	(0x1 << 2)
#define AD77681_GPIO_CNTRL_GPIO2_OP_EN(x)	(((x) & 0x1) << 2)
#define AD77681_GPIO_CNTRL_GPIO1_OP_EN_MSK	(0x1 << 1)
#define AD77681_GPIO_CNTRL_GPIO1_OP_EN(x)	(((x) & 0x1) << 1)
#define AD77681_GPIO_CNTRL_GPIO0_OP_EN_MSK	(0x1 << 0)
#define AD77681_GPIO_CNTRL_GPIO0_OP_EN(x)	(((x) & 0x1) << 0)

/* AD77681_REG_GPIO_WRITE */
#define AD77681_GPIO_WRITE_3_MSK		(0x1 << 3)
#define AD77681_GPIO_WRITE_3(x) 		(((x) & 0x1) << 3)
#define AD77681_GPIO_WRITE_2_MSK		(0x1 << 2)
#define AD77681_GPIO_WRITE_2(x) 		(((x) & 0x1) << 2)
#define AD77681_GPIO_WRITE_1_MSK		(0x1 << 1)
#define AD77681_GPIO_WRITE_1(x) 		(((x) & 0x1) << 1)
#define AD77681_GPIO_WRITE_0_MSK		(0x1 << 0)
#define AD77681_GPIO_WRITE_0(x) 		(((x) & 0x1) << 0)

/* AD77681_REG_GPIO_READ */
#define AD77681_GPIO_READ_3_MSK 		(0x1 << 3)
#define AD77681_GPIO_READ_2_MSK 		(0x1 << 2)
#define AD77681_GPIO_READ_1_MSK 		(0x1 << 1)
#define AD77681_GPIO_READ_0_MSK 		(0x1 << 0)

/* AD77681_REG_OFFSET_HI */
#define AD77681_OFFSET_HI_MSK			(0xFF << 0)
#define AD77681_OFFSET_HI(x)			(((x) & 0xFF) << 0)

/* AD77681_REG_OFFSET_MID */
#define AD77681_OFFSET_MID_MSK			(0xFF << 0)
#define AD77681_OFFSET_MID(x)			(((x) & 0xFF) << 0)

/* AD77681_REG_OFFSET_LO */
#define AD77681_OFFSET_LO_MSK			(0xFF << 0)
#define	AD77681_OFFSET_LO(x)			(((x) & 0xFF) << 0)

/* AD77681_REG_GAIN_HI */
#define AD77681_GAIN_HI_MSK			(0xFF << 0)
#define AD77681_GAIN_HI(x)			(((x) & 0xFF) << 0)

/* AD77681_REG_GAIN_MID */
#define AD77681_GAIN_MID_MSK			(0xFF << 0)
#define AD77681_GAIN_MID(x)			(((x) & 0xFF) << 0)

/* AD77681_REG_GAIN_HI */
#define AD77681_GAIN_LOW_MSK			(0xFF << 0)
#define AD77681_GAIN_LOW(x)			(((x) & 0xFF) << 0)

/* AD77681_REG_SPI_DIAG_ENABLE */
#define AD77681_SPI_DIAG_ERR_SPI_IGNORE_MSK	(0x1 << 4)
#define AD77681_SPI_DIAG_ERR_SPI_IGNORE(x)	(((x) & 0x1) << 4)
#define AD77681_SPI_DIAG_ERR_SPI_CLK_CNT_MSK	(0x1 << 3)
#define AD77681_SPI_DIAG_ERR_SPI_CLK_CNT(x)	(((x) & 0x1) << 3)
#define AD77681_SPI_DIAG_ERR_SPI_RD_MSK 	(0x1 << 2)
#define AD77681_SPI_DIAG_ERR_SPI_RD(x)		(((x) & 0x1) << 2)
#define AD77681_SPI_DIAG_ERR_SPI_WR_MSK 	(0x1 << 1)
#define AD77681_SPI_DIAG_ERR_SPI_WR(x)		(((x) & 0x1) << 1)

/* AD77681_REG_ADC_DIAG_ENABLE */
#define AD77681_ADC_DIAG_ERR_DLDO_PSM_MSK	(0x1 << 5)
#define AD77681_ADC_DIAG_ERR_DLDO_PSM(x)	(((x) & 0x1) << 5)
#define AD77681_ADC_DIAG_ERR_ALDO_PSM_MSK	(0x1 << 4)
#define AD77681_ADC_DIAG_ERR_ALDO_PSM(x)	(((x) & 0x1) << 4)
#define AD77681_ADC_DIAG_ERR_FILT_SAT_MSK	(0x1 << 2)
#define AD77681_ADC_DIAG_ERR_FILT_SAT(x)	(((x) & 0x1) << 2)
#define AD77681_ADC_DIAG_ERR_FILT_NOT_SET_MSK	(0x1 << 1)
#define AD77681_ADC_DIAG_ERR_FILT_NOT_SET(x)	(((x) & 0x1) << 1)
#define AD77681_ADC_DIAG_ERR_EXT_CLK_QUAL_MSK	(0x1 << 0)
#define AD77681_ADC_DIAG_ERR_EXT_CLK_QUAL(x)	(((x) & 0x1) << 0)

/* AD77681_REG_DIG_DIAG_ENABLE */
#define AD77681_DIG_DIAG_ERR_MEMMAP_CRC_MSK	(0x1 << 4)
#define AD77681_DIG_DIAG_ERR_MEMMAP_CRC(x)	(((x) & 0x1) << 4)
#define AD77681_DIG_DIAG_ERR_RAM_CRC_MSK	(0x1 << 3)
#define AD77681_DIG_DIAG_ERR_RAM_CRC(x) 	(((x) & 0x1) << 3)
#define AD77681_DIG_DIAG_ERR_FUSE_CRC_MSK	(0x1 << 2)
#define AD77681_DIG_DIAG_ERR_FUSE_CRC(x)	(((x) & 0x1) << 2)
#define AD77681_DIG_DIAG_FREQ_COUNT_EN_MSK	(0x1 << 0)
#define AD77681_DIG_DIAG_FREQ_COUNT_EN(x)	(((x) & 0x1) << 0)

/* AD77681_REG_MASTER_STATUS */
#define AD77681_MASTER_ERROR_MSK		(0x1 << 7)
#define AD77681_MASTER_ADC_ERROR_MSK		(0x1 << 6)
#define AD77681_MASTER_DIG_ERROR_MSK		(0x1 << 5)
#define AD77681_MASTER_DIG_ERR_EXT_CLK_MSK	(0x1 << 4)
#define AD77681_MASTER_FILT_SAT_MSK		(0x1 << 3)
#define AD77681_MASTER_FILT_NOT_SAT_MSK 	(0x1 << 2)
#define AD77681_MASTER_SPI_ERROR_MSK		(0x1 << 1)
#define AD77681_MASTER_POR_FLAG_MSK		(0x1 << 0)

#define AD77681_REG_READ(x)			( (1 << 6) | (x & 0xFF) )
#define AD77681_REG_WRITE(x)			( (~(1 << 6)) & (x & 0xFF) )

/* 8-bits wide checksum generated using the polynomial */
#define AD77681_CRC8_POLY			0x07 // x^8 + x^2 + x^1 + x^0

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
	AD77681_CONV_PERIODIC = 3
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

struct ad77681_dev {
	/* SPI */
	spi_desc			*spi_desc;
	/* Configuration */
	enum ad77681_power_mode		power_mode;
	enum ad77681_mclk_div		mclk_div;
	enum ad77681_conv_mode 		conv_mode;
	enum ad77681_conv_diag_mux 	diag_mux_sel;
	bool 				conv_diag_sel;
	enum ad77681_conv_len		conv_len;
	enum ad77681_crc_sel 		crc_sel;
	uint8_t				status_bit;
};

struct ad77681_init_param {
	/* SPI */
	struct spi_init_param		*spi_init_param;
	/* Configuration */
	enum ad77681_power_mode		power_mode;
	enum ad77681_mclk_div		mclk_div;
	enum ad77681_conv_mode 		conv_mode;
	enum ad77681_conv_diag_mux 	diag_mux_sel;
	bool 				conv_diag_sel;
	enum ad77681_conv_len		conv_len;
	enum ad77681_crc_sel 		crc_sel;
	uint8_t				status_bit;
} ad77681_init_param;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
uint8_t ad77681_compute_crc8(uint8_t *data,
			     uint8_t data_size);
uint8_t ad77681_compute_xor(uint8_t *data,
			    uint8_t data_size);
int32_t ad77681_setup(struct ad77681_dev **device,
		      const struct ad77681_init_param init_param);
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
				  uint8_t *adc_data);
int32_t ad77681_set_conv_mode(struct ad77681_dev *dev,
			      enum ad77681_conv_mode conv_mode,
			      enum ad77681_conv_diag_mux diag_mux_sel,
			      bool conv_diag_sel);
int32_t ad77681_set_convlen(struct ad77681_dev *dev,
			    enum ad77681_conv_len conv_len);
int32_t ad77681_soft_reset(struct ad77681_dev *dev);
#endif /* D77681_H_ */
