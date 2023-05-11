/***************************************************************************//**
 *   @file   ad413x.h
 *   @brief  Header file of AD413X Driver.
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
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_irq.h"
#include "no_os_util.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD413X_CMD_WR_COM_REG(x)	(0x00 | ((x) & 0x3F)) // Write to Register x
#define AD413X_CMD_RD_COM_REG(x) 	(0x40 | ((x) & 0x3F)) // Read from Register x

/*************************** ADC Register Lengths *****************************/
#define AD413X_R1B				(1 << 8)
#define AD413X_R2B				(2 << 8)
#define AD413X_R3B				(3 << 8)
#define AD413X_TRANSF_LEN(x)			(((x) >> 8) & 0xFF)

/***************************** ADC Register Map *******************************/
#define AD413X_REG_STATUS 			(AD413X_R1B | 0x0)
#define AD413X_REG_ADC_CTRL			(AD413X_R2B | 0x1)
#define AD413X_REG_DATA				(AD413X_R3B | 0x2)
#define AD413X_REG_IO_CTRL			(AD413X_R2B | 0x3)
#define AD413X_REG_VBIAS_CTRL			(AD413X_R2B | 0x4)
#define AD413X_REG_ID				(AD413X_R1B | 0x5)
#define AD413X_REG_ERROR			(AD413X_R2B | 0x6)
#define AD413X_REG_ERROR_EN			(AD413X_R2B | 0x7)
#define AD413X_REG_MCLK_CNT			(AD413X_R1B | 0x8)
#define AD413X_REG_CHN(x)			(AD413X_R3B | (0x09U + (x)))
#define AD413X_REG_CONFIG(x)			(AD413X_R2B | (0x19U + (x)))
#define AD413X_REG_FILTER(x)			(AD413X_R3B | (0x21U + (x)))
#define AD413X_REG_OFFSET(x)			(AD413X_R3B | (0x29U + (x)))
#define AD413X_REG_GAIN(x)			(AD413X_R3B | (0x31U + (x)))
#define AD413X_REG_MISC				(AD413X_R2B | 0x39)
#define AD413X_REG_FIFO_CTRL			(AD413X_R3B | 0x3A)
#define AD413X_REG_FIFO_STS			(AD413X_R1B | 0x3B)
#define AD413X_REG_FIFO_THRSHLD			(AD413X_R3B | 0x3C)
#define AD413X_REG_FIFO_DATA			(AD413X_R3B | 0x3D)


/* ADC_CONTROL Register */
#define AD413X_ADC_BIPOLAR				NO_OS_BIT(14)
#define AD413X_ADC_REF_VAL				NO_OS_BIT(13)
#define AD413X_ADC_DOUT_DIS_DEL				NO_OS_BIT(12)
#define AD413X_ADC_CONT_READ				NO_OS_BIT(11)
#define AD413X_ADC_DATA_STATUS				NO_OS_BIT(10)
#define AD413X_ADC_CSB_EN				NO_OS_BIT(9)
#define AD413X_ADC_REF_EN				NO_OS_BIT(8)
#define AD413X_ADC_DUTY_CYC_RATIO			NO_OS_BIT(6)
#define AD413X_ADC_MODE(x)				(((x) & 0xF) << 2)
#define AD413X_ADC_CNTRL_MCLK(x)			((x) & 0x3)

/* IO_CONTROL Register */
#define AD413X_SYNCB_CLEAR 				NO_OS_BIT(10)
#define AD413X_INT_PIN_SEL(x) 				(((x) & 0x3) << 8)
#define AD413X_GPO_DATA_P4 				NO_OS_BIT(7)
#define AD413X_GPO_DATA_P3 				NO_OS_BIT(6)
#define AD413X_GPO_DATA_P2 				NO_OS_BIT(5)
#define AD413X_GPO_DATA_P1 				NO_OS_BIT(4)
#define AD413X_GPO_CTRL_P4		 		NO_OS_BIT(3)
#define AD413X_GPO_CTRL_P3		 		NO_OS_BIT(2)
#define AD413X_GPO_CTRL_P2		 		NO_OS_BIT(1)
#define AD413X_GPO_CTRL_P1		 		0x01

/* VBIAS_CTRL Register */
#define AD413X_VBIAS_15					NO_OS_BIT(15)
#define AD413X_VBIAS_14					NO_OS_BIT(14)
#define AD413X_VBIAS_13					NO_OS_BIT(13)
#define AD413X_VBIAS_12					NO_OS_BIT(12)
#define AD413X_VBIAS_11					NO_OS_BIT(11)
#define AD413X_VBIAS_10					NO_OS_BIT(10)
#define AD413X_VBIAS_9					NO_OS_BIT(9)
#define AD413X_VBIAS_8					NO_OS_BIT(8)
#define AD413X_VBIAS_7					NO_OS_BIT(7)
#define AD413X_VBIAS_6					NO_OS_BIT(6)
#define AD413X_VBIAS_5					NO_OS_BIT(5)
#define AD413X_VBIAS_4					NO_OS_BIT(4)
#define AD413X_VBIAS_3					NO_OS_BIT(3)
#define AD413X_VBIAS_2					NO_OS_BIT(2)
#define AD413X_VBIAS_1					NO_OS_BIT(1)
#define AD413X_VBIAS_0					0x01

/* ERROR Register */
#define AD413X_AINP_OV_UV_ERR				NO_OS_BIT(11)
#define AD413X_AINM_OV_UV_ERR				NO_OS_BIT(10)
#define AD413X_REF_OV_UV_ERR				NO_OS_BIT(9)
#define AD413X_REF_DETECT_ERR				NO_OS_BIT(8)
#define AD413X_ADC_ERR					NO_OS_BIT(7)
#define AD413X_SPI_IGNORE_ERR				NO_OS_BIT(6)
#define AD413X_SPI_SCLK_CNT_ERR				NO_OS_BIT(5)
#define AD413X_SPI_READ_ERR				NO_OS_BIT(4)
#define AD413X_SPI_WRITE_ERR				NO_OS_BIT(3)
#define AD413X_SPI_CRC_ERR				NO_OS_BIT(2)
#define AD413X_MM_CRC_ERR				NO_OS_BIT(1)
#define AD413X_ROM_CRC_ERR				0x01

/* ERROR_EN Register */
#define AD413X_MCLK_CNT_EN				NO_OS_BIT(12)
#define AD413X_AINP_OV_UV_ERR_EN			NO_OS_BIT(11)
#define AD413X_AINM_OV_UV_ERR_EN			NO_OS_BIT(10)
#define AD413X_REF_OV_UV_ERR_EN				NO_OS_BIT(9)
#define AD413X_REF_DETECT_ERR_EN			NO_OS_BIT(8)
#define AD413X_ADC_ERR_EN				NO_OS_BIT(7)
#define AD413X_SPI_IGNORE_ERR_EN			NO_OS_BIT(6)
#define AD413X_SPI_SCLK_CNT_ERR_EN			NO_OS_BIT(5)
#define AD413X_SPI_READ_ERR_EN				NO_OS_BIT(4)
#define AD413X_SPI_WRITE_ERR_EN				NO_OS_BIT(3)
#define AD413X_SPI_CRC_ERR_EN				NO_OS_BIT(2)
#define AD413X_MM_CRC_ERR_EN				NO_OS_BIT(1)
#define AD413X_ROM_CRC_ERR_EN				0x01

/* CHANNEL_M Register */
#define AD413X_ENABLE_M					NO_OS_BIT(23)
#define AD413X_SETUP_M(x)				(((x) & 0x7) << 20)
#define AD413X_PDSW_M					NO_OS_BIT(19)
#define AD413X_THRES_EN_M				NO_OS_BIT(18)
#define AD413X_AINP_M(x)				(((x) & 0x1F) << 13)
#define AD413X_AINM_M(x)				(((x) & 0x1F) <<  8)
#define AD413X_I_OUT1_CH_M(x)				(((x) & 0xF) <<   4)
#define AD413X_I_OUT0_CH_M(x)				((x) & 0xF)
#define AD413X_I_OUT_CH_MSK				NO_OS_GENMASK(7,0)

/* CONFIG_N Register */
#define AD413X_I_OUT1_N(x)				(((x) & 0x7) << 13)
#define AD413X_I_OUT0_N(x)				(((x) & 0x7) << 10)
#define AD413X_I_OUT_MSK				NO_OS_GENMASK(15,10)
#define AD413X_BURNOUT_N(x)				(((x) & 0x3) <<  8)
#define AD413X_REF_BUF_MSK				NO_OS_GENMASK(7,6)
#define AD413X_REF_BUFP_N				NO_OS_BIT(7)
#define AD413X_REF_BUFM_N				NO_OS_BIT(6)
#define AD413X_REF_SEL_N(x)				(((x) & 0x3) <<  4)
#define AD413X_PGA_N(x)					(((x) & 0x7) <<  1)
#define AD413X_PGA_BYP_N				0x01

/* FILTER_N Register */
#define AD413X_SETTLE_N(x)				(((x) & 0x7) <<  21)
#define AD413X_REPEAT_N(x)				(((x) & 0x1F) << 16)
#define AD413X_FILTER_MODE_N(x)				(((x) & 0xF) <<  12)
#define AD413X_FS_N(x)					((x) & 0x7FF)

/* OFFSET_N Register */
#define AD413X_OFFSET_N(x)				((x) & 0xFFFFFF)

/* GAIN_N Register */
#define AD413X_GAIN_N(x)				((x) & 0xFFFFFF)

/* MISC Register */
#define AD413X_BYPASS_OSC				NO_OS_BIT(15)
#define AD413X_PD_ALDO					NO_OS_BIT(14)
#define AD413X_CAL_RANGE_X2				NO_OS_BIT(13)
#define AD413X_RESERVED(x)				(((x) & 0xF) << 9)
#define AD413X_STBY_CTRL_MSK				NO_OS_GENMASK(8,0)
#define AD413X_STBY_OUT_EN				NO_OS_BIT(8)
#define AD413X_STBY_DIAGNOSTICS_EN			NO_OS_BIT(7)
#define AD413X_STBY_GPO_EN				NO_OS_BIT(6)
#define AD413X_STBY_PDSW_EN				NO_OS_BIT(5)
#define AD413X_STBY_BURNOUT_EN				NO_OS_BIT(4)
#define AD413X_STBY_VBIAS_EN				NO_OS_BIT(3)
#define AD413X_STBY_IEXC_EN				NO_OS_BIT(2)
#define AD413X_STBY_REFHOL_EN				NO_OS_BIT(1)
#define AD413X_STBY_INTREF_EN				0x01

/* FIFO_CONTROL Register */
#define AD413X_ADD_FIFO_STATUS				NO_OS_BIT(19)
#define AD413X_ADD_FIFO_HEADER				NO_OS_BIT(18)
#define AD413X_FIFO_MODE(x) 				(((x) & 0x3) << 16)
#define AD413X_FIFO_WRITE_ERR_INT_EN 			NO_OS_BIT(14)
#define AD413X_FIFO_READ_ERR_INT_EN 			NO_OS_BIT(13)
#define AD413X_THRES_HIGH_INT_EN 			NO_OS_BIT(12)
#define AD413X_THRES_LOW_INT_EN  			NO_OS_BIT(11)
#define AD413X_OVERRUN_INT_EN 				NO_OS_BIT(10)
#define AD413X_WATERMARK_INT_EN 			NO_OS_BIT(9)
#define AD413X_EMPTY_INT_EN 				NO_OS_BIT(8)
#define AD413X_WATERMARK(x) 				((x) & 0xFF)

/* FIFO_STATUS Register */
#define AD413X_MASTER_ERR 				NO_OS_BIT(7)
#define AD413X_FIFO_WRITE_ERR 				NO_OS_BIT(6)
#define AD413X_FIFO_READ_ERR 				NO_OS_BIT(5)
#define AD413X_THRES_HIGH_FLAG 				NO_OS_BIT(4)
#define AD413X_THRES_LOW_FLAG 				NO_OS_BIT(3)
#define AD413X_OVERRUN_FLAG 				NO_OS_BIT(2)
#define AD413X_WATERMARK_FLAG 				NO_OS_BIT(1)
#define AD413X_EMPTY_FLAG 				0x01

/* FIFO_THRESHOLD Register */
#define AD413X_THRES_HIGH_VAL(x)		(((x) & 0xFFF) << 12)
#define AD413X_THRES_LOW_VAL(x)			((x) & 0xFFF)

/* 8-bits wide checksum generated using the polynomial */
#define AD413X_CRC8_POLY	0x07 // x^8 + x^2 + x^1 + x^0

#define ENABLE				1U
#define DISABLE				0U

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
/**
 * @enum ad413x_input
 * @brief ADC input sources for each channel.
 */
enum ad413x_input {
	AD413X_AIN0,
	AD413X_AIN1,
	AD413X_AIN2,
	AD413X_AIN3,
	AD413X_AIN4,
	AD413X_AIN5,
	AD413X_AIN6,
	AD413X_AIN7,
	AD413X_AIN8,
	AD413X_AIN9,
	AD413X_AIN10,
	AD413X_AIN11,
	AD413X_AIN12,
	AD413X_AIN13,
	AD413X_AIN14,
	AD413X_AIN15,
	AD413X_TEMP,
	AD413X_AVSS,
	AD413X_INT_REF,
	AD413X_DGND,
	AD413X_AVDD_AVSS_6P,
	AD413X_AVDD_AVSS_6M,
	AD413X_IOVDD_DGND_6P,
	AD413X_IOVDD_DGND_6M,
	AD413X_ALDO_AVSS_6P,
	AD413X_ALDO_AVSS_6M,
	AD413X_DLDO_DGND_6P,
	AD413X_DLDO_DGND_6M,
	AD413X_V_MV_P,
	AD413X_V_MV_M,
};

/**
 * @enum ad413x_preset_nb
 * @brief Preset number.
 */
enum ad413x_preset_nb {
	AD413X_PRESET_0,
	AD413X_PRESET_1,
	AD413X_PRESET_2,
	AD413X_PRESET_3,
	AD413X_PRESET_4,
	AD413X_PRESET_5,
	AD413X_PRESET_6,
	AD413X_PRESET_7
};

/**
 * @enum ad413x_mclk_sel
 * @brief Master clock options.
 */
enum ad413x_mclk_sel {
	AD413X_INT_76_8_KHZ_OUT_OFF,
	AD413X_INT_76_8_KHZ_OUT_ON,
	AD413X_EXT_76_8KHZ,
	AD413X_EXT_153_6_KHZ_DIV_2
};

/**
 * @enum ad413x_adc_mode
 * @brief ADC conversion modes.
 */
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

/**
 * @struct ad413x_ref_buf
 * @brief ADC reference buffer selection.
 */
struct ad413x_ref_buf {
	bool ref_buf_p_en;
	bool ref_buf_m_en;
};

/**
 * @enum ad413x_ref_sel
 * @brief ADC reference selection.
 */
enum ad413x_ref_sel {
	AD413X_REFIN1,
	AD413X_REFIN2,
	AD413X_REFOUT_AVSS,
	AD413X_AVDD_AVSS
};

/**
 * @enum ad413x_int_ref
 * @brief Internal reference selection.
 */
enum ad413x_int_ref {
	AD413X_INTREF_DISABLED,
	AD413X_INTREF_2_5V,
	AD413X_INTREF_1_25V
};

/**
 * @enum ad413x_filter
 * @brief Filter types.
 */
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

/**
 * @enum ad413x_gain
 * @brief Gain options.
 */
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

/**
 * @enum ad413x_chip_id
 * @brief Chip IDs.
 */
enum ad413x_chip_id {
	AD4130_8 = 0x04
};

/**
 * @enum ad413x_settle_time
 * @brief Channel settle time.
 */
enum ad413x_settle_time {
	AD413X_32_MCLK,
	AD413X_64_MCLK,
	AD413X_128_MCLK,
	AD413X_256_MCLK,
	AD413X_512_MCLK,
	AD413X_1024_MCLK,
	AD413X_2048_MCLK,
	AD413X_4096_MCLK
};

/**
 * @enum ad413x_exc_current
 * @brief Excitation current value
 */
enum ad413x_exc_current {
	AD413X_EXC_OFF,
	AD413X_EXC_10UA,
	AD413X_EXC_20UA,
	AD413X_EXC_50UA,
	AD413X_EXC_100UA,
	AD413X_EXC_150UA,
	AD413X_EXC_200UA,
	AD413X_EXC_100NA
};

/**
 * @struct ad413x_standby_ctrl
 * @brief Standby control flags
 */
struct ad413x_standby_ctrl {
	bool standby_int_ref_en;
	bool standby_ref_holder_en;
	bool standby_iexc_en;
	bool standby_vbias_en;
	bool standby_burnout_en;
	bool standby_pdsw_en;
	bool standby_gpio_en;
	bool standby_diagn_en;
	bool standby_output_en;
};

/**
 * @struct ad413x_preset
 * @brief Preset setting.
 */
struct ad413x_preset {
	struct ad413x_ref_buf ref_buf;
	enum ad413x_ref_sel ref_sel;
	enum ad413x_gain gain;
	enum ad413x_filter filter;
	/* By default, settle time = 32MCLK */
	enum ad413x_settle_time s_time;
	enum ad413x_exc_current iout0_exc_current;
	enum ad413x_exc_current iout1_exc_current;
};

/**
 * @struct ad413x_channel
 * @brief Channel setting.
 */
struct ad413x_channel {
	enum ad413x_preset_nb preset;
	uint8_t enable;
	enum ad413x_input ain_p;
	enum ad413x_input ain_m;
	enum ad413x_input iout0_exc_input;
	enum ad413x_input iout1_exc_input;
	bool pdsw_en;
};

/**
 * @struct ad413x_dev
 * @brief Device structure.
 */
struct ad413x_dev {
	/* SPI */
	struct no_os_spi_desc			*spi_dev;
	/* GPIO - used to know when conversion is rdy */
	struct no_os_gpio_desc *rdy_pin_desc;
	/* Device Settings */
	struct ad413x_preset preset[8];
	struct ad413x_channel ch[16];
	enum ad413x_chip_id chip_id;
	enum ad413x_mclk_sel mclk;
	enum ad413x_adc_mode op_mode;
	uint8_t bipolar;
	enum ad413x_int_ref int_ref;
	uint16_t v_bias;
	struct ad413x_standby_ctrl standby_ctrl;
	uint8_t data_stat;
	uint8_t spi_crc_en;
};

/**
 * @struct ad413x_init_param
 * @brief Initial parameter structure.
 */
struct ad413x_init_param {
	/* SPI */
	struct no_os_spi_init_param	*spi_init;
	/* GPIO - used to know when conversion is rdy */
	struct no_os_gpio_init_param *rdy_pin_init;
	/* Device Settings */
	struct ad413x_preset preset[8];
	struct ad413x_channel ch[16];
	enum ad413x_chip_id chip_id;
	enum ad413x_mclk_sel mclk;
	uint8_t bipolar;
	enum ad413x_int_ref int_ref;
	uint16_t v_bias;
	struct ad413x_standby_ctrl standby_ctrl;
	uint8_t data_stat;
	uint8_t spi_crc_en;
};

/**
 * @struct ad413x_callback_ctx
 * @brief Callback structure.
 */
struct ad413x_callback_ctx {
	struct ad413x_dev *dev;
	uint32_t *buffer;
	uint32_t buffer_size;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* SPI write to device using a mask. */
int32_t ad413x_reg_write_msk(struct ad413x_dev *dev,
			     uint32_t reg_addr,
			     uint32_t data,
			     uint32_t mask);

/* Set the mode of the ADC. */
int32_t ad413x_set_adc_mode(struct ad413x_dev *dev, enum ad413x_adc_mode mode);

/* Set the internal reference. */
int32_t ad413x_set_int_ref(struct ad413x_dev *dev, enum ad413x_int_ref int_ref);

/* Enable/disable DATA_STAT. */
int32_t ad413x_data_stat_en(struct ad413x_dev *dev, uint8_t enable);

/* Set the gain. */
int32_t ad413x_set_gain(struct ad413x_dev *dev, enum ad413x_gain gain,
			enum ad413x_preset_nb reg_nb);

/* Set the reference. */
int32_t ad413x_set_ref(struct ad413x_dev *dev, enum ad413x_ref_sel ref,
		       enum ad413x_preset_nb reg_nb);

/* Set the reference buffers */
int32_t ad413x_set_ref_buf(struct ad413x_dev *dev,
			   struct ad413x_ref_buf ref_buf,
			   enum ad413x_preset_nb reg_nb);

/* Set the filter */
int32_t ad413x_set_filter(struct ad413x_dev *dev, enum ad413x_filter filter,
			  enum ad413x_preset_nb reg_nb);

/* Set settle time */
int32_t ad413x_set_settle_time(struct ad413x_dev *dev,
			       enum ad413x_settle_time s_time,
			       enum ad413x_preset_nb reg_nb);

/* Set excitation currents */
int32_t ad413x_set_exc_current(struct ad413x_dev *dev,
			       enum ad413x_exc_current iout0_exc,
			       enum ad413x_exc_current iout1_exc,
			       enum ad413x_preset_nb reg_nb);

/* Set excitation source pins */
int32_t ad413x_ch_exc_input(struct ad413x_dev *dev, uint8_t ch_nb,
			    enum ad413x_input iout0_exc_inp,
			    enum ad413x_input iout1_exc_inp);

/* Set channel preset */
int32_t ad413x_set_ch_preset(struct ad413x_dev *dev, uint8_t ch_nb,
			     enum ad413x_preset_nb preset_nb);

/* Enable channel */
int32_t ad413x_ch_en(struct ad413x_dev *dev, uint8_t ch_nb, uint8_t enable);

/* Enable PDSW */
int32_t ad413x_pdsw_en(struct ad413x_dev *dev, uint8_t ch_nb, bool pdsw_en);

/* Set output VBIAS */
int32_t ad413x_set_v_bias(struct ad413x_dev *dev, uint16_t v_bias_val);

/* Set standby control flags */
int32_t ad413x_set_standby_ctrl(struct ad413x_dev *dev,
				struct ad413x_standby_ctrl standby_ctrl);

/* Set ADC master clock mode. */
int32_t ad413x_set_mclk(struct ad413x_dev *dev, enum ad413x_mclk_sel clk);

/* Do a SPI software reset. */
int32_t ad413x_do_soft_reset(struct ad413x_dev *dev);

/* SPI internal register write to device. */
int32_t ad413x_reg_write(struct ad413x_dev *dev,
			 uint32_t reg_addr,
			 uint32_t reg_data);

/* SPI internal register read from device. */
int32_t ad413x_reg_read(struct ad413x_dev *dev,
			uint32_t reg_addr,
			uint32_t *reg_data);

/* Fills buffer with data read from each active channel */
int32_t ad413x_single_conv(struct ad413x_dev *dev, uint32_t *buffer,
			   uint8_t ch_nb);

/* Fills buffer with data from continuous conv mode from each active channel */
int32_t ad413x_continuous_conv(struct ad413x_dev *dev, uint32_t *buffer,
			       uint8_t ch_nb, uint32_t sample_nb);

/* Set adc bipolar/unipolar coding. */
int32_t ad413x_adc_bipolar(struct ad413x_dev *dev, uint8_t enable);

/* Store presets for adc channels */
int32_t ad413x_preset_store(struct ad413x_dev *dev, struct ad413x_preset preset,
			    enum ad413x_preset_nb preset_nb);

/* Initialize the device. */
int32_t ad413x_init(struct ad413x_dev **device,
		    struct ad413x_init_param init_param);

/* Remove the device. */
int32_t ad413x_remove(struct ad413x_dev *dev);

#endif // AD413X_H_
