/***************************************************************************//**
 *   @file   ad413x.h
 *   @brief  Header file of AD411X Driver.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
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

#ifndef AD413X_H_
#define AD413X_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no-os/delay.h"
#include "no-os/gpio.h"
#include "no-os/spi.h"
#include "no-os/irq.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD413X_CMD_WR_COM_REG(x)	(0x00 | ((x) & 0x3F)) // Write to Register x
#define AD413X_CMD_RD_COM_REG(x) 	(0x40 | ((x) & 0x3F)) // Read from Register x

/*************************** ADC Register Lengths *****************************/
#define AD413X_R1B					(1 << 8)
#define AD413X_R2B					(2 << 8)
#define AD413X_R3B					(3 << 8)
#define AD413X_TRANSF_LEN(x)		(((x) >> 8) & 0xFF)

/***************************** ADC Register Map *******************************/
#define AD413X_REG_STATUS 			(AD413X_R1B | 0x0)
#define AD413X_REG_ADC_CTRL			(AD413X_R2B | 0x1)
#define AD413X_REG_DATA				(AD413X_R3B | 0x2)
#define AD413X_REG_IO_CTRL			(AD413X_R2B | 0x3)
#define AD413X_REG_VBIAS_CTRL		(AD413X_R2B | 0x4)
#define AD413X_REG_ID				(AD413X_R1B | 0x5)
#define AD413X_REG_ERROR			(AD413X_R2B | 0x6)
#define AD413X_REG_ERROR_EN			(AD413X_R2B | 0x7)
#define AD413X_REG_MCLK_CNT			(AD413X_R1B | 0x8)
#define AD413X_REG_CHN(x)			(AD413X_R3B | (0x09U + (x)))
#define AD413X_REG_CONFIG(x)		(AD413X_R2B | (0x19U + (x)))
#define AD413X_REG_FILTER(x)		(AD413X_R3B | (0x21U + (x)))
#define AD413X_REG_OFFSET(x)		(AD413X_R3B | (0x29U + (x)))
#define AD413X_REG_GAIN(x)			(AD413X_R3B | (0x31U + (x)))
#define AD413X_REG_MISC				(AD413X_R2B | 0x39)
#define AD413X_REG_FIFO_CTRL		(AD413X_R3B | 0x3A)
#define AD413X_REG_FIFO_STS			(AD413X_R1B | 0x3B)
#define AD413X_REG_FIFO_THRSHLD		(AD413X_R3B | 0x3C)
#define AD413X_REG_FIFO_DATA		(AD413X_R3B | 0x3D)


/* ADC_CNTROL Register */
#define AD413X_ADC_BIPOLAR				(1 << 14)
#define AD413X_ADC_REF_VAL				(1 << 13)
#define AD413X_ADC_DOUT_DIS_DEL			(1 << 12)
#define AD413X_ADC_CONT_READ			(1 << 11)
#define AD413X_ADC_DATA_STATUS			(1 << 10)
#define AD413X_ADC_CSB_EN				(1 <<  9)
#define AD413X_ADC_REF_EN				(1 <<  8)
#define AD413X_ADC_DUTY_CYC_RATIO		(1 <<  6)
#define AD413X_ADC_MODE(x)				(((x) & 0xF) << 2)
#define AD413X_ADC_CNTRL_MCLK(x)		((x) & 0x3)

/* IO_CONTROL Register */
#define AD413X_SYNCB_CLEAR 				(1 << 10)
#define AD413X_INT_PIN_SEL(x) 			(((x) & 0x3) << 8)
#define AD413X_GPO_DATA_P4 				(1 << 7)
#define AD413X_GPO_DATA_P3 				(1 << 6)
#define AD413X_GPO_DATA_P2 				(1 << 5)
#define AD413X_GPO_DATA_P1 				(1 << 4)
#define AD413X_GPO_CTRL_P4		 		(1 << 3)
#define AD413X_GPO_CTRL_P3		 		(1 << 2)
#define AD413X_GPO_CTRL_P2		 		(1 << 1)
#define AD413X_GPO_CTRL_P1		 		0x01

/* VBIAS_CTRL Register */
#define AD413X_VBIAS_15					(1 << 15)
#define AD413X_VBIAS_14					(1 << 14)
#define AD413X_VBIAS_13					(1 << 13)
#define AD413X_VBIAS_12					(1 << 12)
#define AD413X_VBIAS_11					(1 << 11)
#define AD413X_VBIAS_10					(1 << 10)
#define AD413X_VBIAS_9					(1 <<  9)
#define AD413X_VBIAS_8					(1 <<  8)
#define AD413X_VBIAS_7					(1 <<  7)
#define AD413X_VBIAS_6					(1 <<  6)
#define AD413X_VBIAS_5					(1 <<  5)
#define AD413X_VBIAS_4					(1 <<  4)
#define AD413X_VBIAS_3					(1 <<  3)
#define AD413X_VBIAS_2					(1 <<  2)
#define AD413X_VBIAS_1					(1 <<  1)
#define AD413X_VBIAS_0					0x01

/* ERROR Register */
#define AD413X_AINP_OV_UV_ERR			(1 << 11)
#define AD413X_AINM_OV_UV_ERR			(1 << 10)
#define AD413X_REF_OV_UV_ERR			(1 <<  9)
#define AD413X_REF_DETECT_ERR			(1 <<  8)
#define AD413X_ADC_ERR					(1 <<  7)
#define AD413X_SPI_IGNORE_ERR			(1 <<  6)
#define AD413X_SPI_SCLK_CNT_ERR			(1 <<  5)
#define AD413X_SPI_READ_ERR				(1 <<  4)
#define AD413X_SPI_WRITE_ERR			(1 <<  3)
#define AD413X_SPI_CRC_ERR				(1 <<  2)
#define AD413X_MM_CRC_ERR				(1 <<  1)
#define AD413X_ROM_CRC_ERR				0x01

/* ERROR_EN Register */
#define AD413X_MCLK_CNT_EN				(1 << 12)
#define AD413X_AINP_OV_UV_ERR_EN		(1 << 11)
#define AD413X_AINM_OV_UV_ERR_EN		(1 << 10)
#define AD413X_REF_OV_UV_ERR_EN			(1 <<  9)
#define AD413X_REF_DETECT_ERR_EN		(1 <<  8)
#define AD413X_ADC_ERR_EN				(1 <<  7)
#define AD413X_SPI_IGNORE_ERR_EN		(1 <<  6)
#define AD413X_SPI_SCLK_CNT_ERR_EN		(1 <<  5)
#define AD413X_SPI_READ_ERR_EN			(1 <<  4)
#define AD413X_SPI_WRITE_ERR_EN			(1 <<  3)
#define AD413X_SPI_CRC_ERR_EN			(1 <<  2)
#define AD413X_MM_CRC_ERR_EN			(1 <<  1)
#define AD413X_ROM_CRC_ERR_EN			0x01

/* CHANNEL_M Register */
#define AD413X_ENABLE_M					(1 << 23)
#define AD413X_SETUP_M(x)				(((x) & 0x7) << 20)
#define AD413X_PDSW_M					(1 << 19)
#define AD413X_THRES_EN_M				(1 << 18)
#define AD413X_AINP_M(x)				(((x) & 0x1F) << 13)
#define AD413X_AINM_M(x)				(((x) & 0x1F) <<  8)
#define AD413X_I_OUT1_CH_M(x)			(((x) & 0xF) <<   4)
#define AD413X_I_OUT0_CH_M(x)			((x) & 0xF)

/* CONFIG_N Register */
#define AD413X_I_OUT1_N(x)				(((x) & 0x7) << 13)
#define AD413X_I_OUT0_N(x)				(((x) & 0x7) << 10)
#define AD413X_BURNOUT_N(x)				(((x) & 0x3) <<  8)
#define AD413X_REF_BUFP_N(x)			(1 <<  7)
#define AD413X_EF_BUFM_N				(1 <<  6)
#define AD413X_REF_SEL_N(x)				(((x) & 0x3) <<  4)
#define AD413X_PGA_N(x)					(((x) & 0x7) <<  1)
#define AD413X_PGA_BYP_N				0x01

/* FILTER_N Register */
#define AD413X_SETTLE_N(x)				(((x) & 0x7) <<  21)
#define AD413X_REPEAT_N(x)				(((x) & 0x1F) << 16)
#define AD413X_FILTER_MODE_N(x)			(((x) & 0xF) <<  12)
#define AD413X_FS_N(x)					((x) & 0x7FF)

/* OFFSET_N Register */
#define AD413X_OFFSET_N(x)				((x) & 0xFFFFFF)

/* GAIN_N Register */
#define AD413X_GAIN_N(x)				((x) & 0xFFFFFF)

/* MISC Register */
#define AD413X_BYPASS_OSC				(1 << 15)
#define AD413X_PD_ALDO					(1 << 14)
#define AD413X_CAL_RANGE_X2				(1 << 13)
#define AD413X_RESERVED(x)				(((x) & 0xF) << 9)
#define AD413X_STBY_OUT_EN				(1 << 8)
#define AD413X_STBY_DIAGNOSTICS_EN		(1 << 7)
#define AD413X_STBY_GPO_EN				(1 << 6)
#define AD413X_STBY_PDSW_EN				(1 << 5)
#define AD413X_STBY_BURNOUT_EN			(1 << 4)
#define AD413X_STBY_VBIAS_EN			(1 << 3)
#define AD413X_STBY_IEXC_EN				(1 << 2)
#define AD413X_STBY_REFHOL_EN			(1 << 1)
#define AD413X_STBY_INTREF_EN			0x01

/* FIFO_CONTROL Register */
#define AD413X_ADD_FIFO_STATUS			(1 << 19)
#define AD413X_ADD_FIFO_HEADER			(1 << 18)
#define AD413X_FIFO_MODE(x) 			(((x) & 0x3) << 16)
#define AD413X_FIFO_WRITE_ERR_INT_EN 	(1 << 14)
#define AD413X_FIFO_READ_ERR_INT_EN 	(1 << 13)
#define AD413X_THRES_HIGH_INT_EN 		(1 << 12)
#define AD413X_THRES_LOW_INT_EN  		(1 << 11)
#define AD413X_OVERRUN_INT_EN 			(1 << 10)
#define AD413X_WATERMARK_INT_EN 		(1 <<  9)
#define AD413X_EMPTY_INT_EN 			(1 <<  8)
#define AD413X_WATERMARK(x) 			((x) & 0xFF)

/* FIFO_STATUS Register */
#define AD413X_MASTER_ERR 				(1 << 7)
#define AD413X_FIFO_WRITE_ERR 			(1 << 6)
#define AD413X_FIFO_READ_ERR 			(1 << 5)
#define AD413X_THRES_HIGH_FLAG 			(1 << 4)
#define AD413X_THRES_LOW_FLAG 			(1 << 3)
#define AD413X_OVERRUN_FLAG 			(1 << 2)
#define AD413X_WATERMARK_FLAG 			(1 << 1)
#define AD413X_EMPTY_FLAG 				0x01

/* FIFO_THRESHOLD Register */
#define AD413X_THRES_HIGH_VAL(x)		(((x) & 0xFFF) << 12)
#define AD413X_THRES_LOW_VAL(x)			((x) & 0xFFF)

/* 8-bits wide checksum generated using the polynomial */
#define AD413X_CRC8_POLY	0x07 // x^8 + x^2 + x^1 + x^0

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
enum ad413x_mclk_sel {
	AD413X_INT_76_8_KHZ_OUT_OFF,
	AD413X_INT_76_8_KHZ_OUT_ON,
	AD413X_EXT_76_8KHZ,
	AD413X_EXT_153_6_KHZ_DIV_2
};

enum ad413x_adc_mode {
	AD413X_CONTINOUS_CONV_MODE = 0,
	AD413X_SINGLE_CONV_MODE = 1,
	AD413X_STANDBY_MODE = 2,
	AD413X_PW_DOWN_MODE = 3,
	AD413X_IDLE_MODE = 4,
	AD413X_INT_OFFSET_CAL = 5,
	AD413X_INT_GAIN_CAL = 6,
	AD413X_SYS_OFFSET_CAL = 7,
	AD413X_SYS_GAIN_CAL = 8,
	AD413X_DUTY_CYCLING_MODE = 9,
	AD413X_SINGLE_CONV_SYNC_IDLE_MODE = 10,
	AD413X_DUTY_CYCLING_SYNC_STBY_MODE = 11
};

enum ad413x_ref_sel {
	AD413X_REFIN1,
	AD413X_REFIN2,
	AD413X_REFOUT_AVSS,
	AD413X_AVDD_AVSS
};

enum ad413x_filter {
	AD413X_SYNC4_STANDALONE,
	AD413X_SYNC4_SYNC1,
	AD413X_SYNC3_STANDALONE,
	AD413X_SYNC3_REJ60,
	AD413X_SYNC3_SYNC1,
	AD413X_SYNC3_PF1,
	AD413X_SYNC3_PF2,
	AD413X_SYNC3_PF3,
	AD413X_SYNC3_PF4
};

enum ad413x_gain {
	AD413X_GAIN_1,
	AD413X_GAIN_2,
	AD413X_GAIN_4,
	AD413X_GAIN_8,
	AD413X_GAIN_16,
	AD413X_GAIN_32,
	AD413X_GAIN_64,
	AD413X_GAIN_128,
};

/* TBD */
enum ad413x_chip_id {
	AD4130_8 = 0x00
};

struct ad413x_preset {
	enum ad413x_ref_sel ref_sel;
	enum ad413x_gain gain;
	enum ad413x_filter filter;
};

struct ad413x_channel {
	uint8_t preset;
	uint8_t enable;
	uint8_t ain_p;
	uint8_t ain_m;
};

struct ad413x_dev {
	/* SPI */
	struct spi_desc			*spi_dev;
	/* GPIO - used to know when conversion is rdy */
	struct irq_ctrl_desc *irq_desc;
	uint32_t rdy_pin;
	/* Device Settings */
	struct ad413x_preset preset[8];
	struct ad413x_channel ch[16];
	enum ad413x_chip_id chip_id;
	enum ad413x_mclk_sel mclk;
	enum ad413x_adc_mode op_mode;
	uint8_t bipolar;
	uint8_t int_ref; //Internal Reference Enable
};

struct ad413x_init_param {
	/* SPI */
	struct spi_init_param	*spi_init;
	/* GPIO - used to know when conversion is rdy */
	struct irq_ctrl_desc *irq_desc;
	uint32_t rdy_pin;
	/* Device Settings */
	struct ad413x_preset preset[8];
	struct ad413x_channel ch[16];
	enum ad413x_chip_id chip_id;
	enum ad413x_mclk_sel mclk;
	enum ad413x_adc_mode op_mode;
	uint8_t bipolar;
	uint8_t int_ref; //Internal Reference Enable
};

struct ad413x_callback_ctx {
	struct ad413x_dev *dev;
	int32_t *buffer;
	int32_t buffer_size;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* SPI write to device using a mask. */
int32_t ad413x_spi_int_reg_write_msk(struct ad413x_dev *dev,
				     uint32_t reg_addr,
				     uint32_t data,
				     uint32_t mask);
/* Set the mode of the ADC. */
int32_t ad413x_set_adc_mode(struct ad413x_dev *dev, enum ad413x_adc_mode mode);

/* Set the gain. */
int32_t ad413x_set_gain(struct ad413x_dev *dev, enum ad413x_gain gain,
			uint8_t reg_nb);

/* Set the reference. */
int32_t ad413x_set_ref(struct ad413x_dev *dev, enum ad413x_ref_sel ref,
		       uint8_t reg_nb);

/* Set the filter */
int32_t ad413x_set_filter(struct ad413x_dev *dev, enum ad413x_filter filter,
			  uint8_t reg_nb);

/* Set channel preset */
int32_t ad413x_set_ch_preset(struct ad413x_dev *dev, uint8_t ch_nb,
			     uint8_t preset_nb);

/* Enable channel */
int32_t ad413x_ch_en(struct ad413x_dev *dev, uint8_t ch_nb, uint8_t enable);

/* Set ADC master clock mode. */
int32_t ad413x_set_mclk(struct ad413x_dev *dev, enum ad413x_mclk_sel clk);

/* Do a SPI software reset. */
int32_t ad413x_spi_do_soft_reset(struct ad413x_dev *dev);

/* SPI internal register write to device. */
int32_t ad413x_spi_int_reg_write(struct ad413x_dev *dev,
				 uint32_t reg_addr,
				 uint32_t reg_data);

/* SPI internal register read from device. */
int32_t ad413x_spi_int_reg_read(struct ad413x_dev *dev,
				uint32_t reg_addr,
				uint32_t *reg_data);

/* Fills buffer with data read from each active channel */
int32_t ad413x_single_read(struct ad413x_dev *dev, int32_t *buffer,
			   int32_t ch_nb);

/* Fills buffer with data read continuously from each active channel */
int32_t ad413x_continuous_read(struct ad413x_dev *dev, int32_t *buffer,
			       int32_t ch_nb, int32_t sample_nb);

/* Set adc bipolar/unipolar coding. */
int32_t ad413x_adc_bipolar(struct ad413x_dev *dev, uint8_t enable);

/* SPI internal DATA register read from device. */
int32_t ad413x_spi_int_data_reg_read(struct ad413x_dev *dev,
				     uint32_t *reg_data);

/* Store presets for adc channels */
int32_t ad413x_preset_store(struct ad413x_dev *dev, struct ad413x_preset preset,
			    uint8_t preset_nb);

/* Initialize the device. */
int32_t ad413x_init(struct ad413x_dev **device,
		    struct ad413x_init_param init_param);

/* Remove the device. */
int32_t ad413x_remove(struct ad413x_dev *dev);

#endif // AD413X_H_
