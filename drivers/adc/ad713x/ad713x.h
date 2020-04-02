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
#define AD713X_REG_SCTATCH_PAD			0x0A
#define AD713X_REG_SPI_REVISION			0x0B
#define AD713X_REG_VENDOR_ID_LSB		0x0C
#define AD713X_REG_VENDOR_ID_MSB		0x0D
#define AD713X_REG_STREAM_MODE			0x0E
#define AD713X_REG_TRANSFER_REGISTER		0x0F
#define AD713X_REG_DEVICE_CONFIG0		0x10
#define AD713X_REG_DEVICE_CONFIG1		0x11
#define AD713X_REG_DEVICE_CONFIG2		0x12
#define AD713X_REG_DEVICE_CONFIG3		0x13
#define AD713X_REG_DEVICE_CONFIG4		0x14
#define AD713X_REG_DEVICE_CONFIG5		0x15
#define AD713X_REG_POWER_DOWN_CONTROL		0x16
#define AD713X_REG_SDO_PIN_SRC_SEL		0x17
#define AD713X_REG_VCMBUF_CTRL			0x19
#define AD713X_REG_AIN_POWER_DOWN_CONTROL	0x1A
#define AD713X_REG_ERROR_PIN_SRC_CONTROL	0x1C
#define AD713X_REG_ERROR_PIN_CONTROL		0x1D
#define AD713X_REG_MCLK_CNT			0x1E
#define AD713X_REG_ODR_MCLK_RATIO_LSB		0x1F
#define AD713X_REG_ODR_MCLK_RATIO_MID_0		0x20
#define AD713X_REG_ODR_MCLK_RATIO_MID_1		0x21
#define AD713X_REG_ODR_MCLK_RATIO_MSB		0x22
#define AD713X_REG_GPIO_DIR_CTRL		0x23
#define AD713X_REG_GPIO_DATA			0x24
#define AD713X_REG_DIAGNOSTIC_CONTROL		0x27
#define AD713X_REG_CHAN_DIG_FILTER_SEL		0x28
#define AD713X_REG_FIR_BW_SEL			0x29
#define AD713X_REG_CHANNEL_ODR_SELECT		0x2A
#define AD713X_REG_CH0_GAIN_LSB			0x2C
#define AD713X_REG_CH0_GAIN_MID			0x2D
#define AD713X_REG_CH0_GAIN_MSB			0x2E
#define AD713X_REG_CH0_OFFSET_LSB		0x2F
#define AD713X_REG_CH0_OFFSET_MID		0x30
#define AD713X_REG_CH0_OFFSET_MSB		0x31
#define AD713X_REG_CH1_GAIN_LSB			0x32
#define AD713X_REG_CH1_GAIN_MID			0x33
#define AD713X_REG_CH1_GAIN_MSB			0x34
#define AD713X_REG_CH1_OFFSET_LSB		0x35
#define AD713X_REG_CH1_OFFSET_MID		0x36
#define AD713X_REG_CH1_OFFSET_MSB		0x37
#define AD713X_REG_CH2_GAIN_LSB			0x38
#define AD713X_REG_CH2_GAIN_MID			0x39
#define AD713X_REG_CH2_GAIN_MSB			0x3A
#define AD713X_REG_CH2_OFFSET_LSB		0x3B
#define AD713X_REG_CH2_OFFSET_MID		0x3C
#define AD713X_REG_CH2_OFFSET_MSB		0x3D
#define AD713X_REG_CH3_GAIN_LSB			0x3E
#define AD713X_REG_CH3_GAIN_MID			0x3F
#define AD713X_REG_CH3_GAIN_MSB			0x40
#define AD713X_REG_CH3_OFFSET_LSB		0x41
#define AD713X_REG_CH3_OFFSET_MID		0x42
#define AD713X_REG_CH3_OFFSET_MSB		0x43
#define AD713X_REG_DEVICE_STATUS		0x44
#define AD713X_REG_PWR_MON_FLAG_0		0x45
#define AD713X_REG_PWR_MON_FLAG_1		0x46
#define AD713X_REG_PWR_MON_FLAG_2		0x47
#define AD713X_REG_PWR_MON_FLAG_3		0x48
#define AD713X_REG_SPI_ERROR_FLAG		0x49
#define AD713X_REG_ADC_INPUT_FLAG_0		0x4A
#define AD713X_REG_DIG_FILTER_OFUF		0x4B
#define AD713X_REG_DIG_FILTER_SETTLED		0x4C
#define AD713X_REG_CLK_ERR			0x4D
#define AD713X_REG_INTERNAL_ERROR_FLAG		0x4E
#define AD713X_REG_TEMPERATURE_DATA		0x4F
#define AD713X_REG_AVDD1P8_DATA			0x50
#define AD713X_REG_DVDD1P8_DATA			0x51
#define AD713X_REG_CLKVDD1P8_DATA		0x52
#define AD713X_REG_IOVDD1P8_DATA		0x53
#define AD713X_REG_AVDD5P0_DATA			0x54
#define AD713X_REG_DVDD5P0_DATA			0x55
#define AD713X_REG_VREF_DATA			0x56
#define AD713X_REG_LDOIN_DATA			0x57

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

/*
 * AD713X_REG_INTERFACE_CONFIG_B
 */
#define AD713X_INT_CONFIG_B_SINGLE_INSTR_MSK		BIT(7)
#define AD713X_INT_CONFIG_B_SINGLE_INSTR_MODE(x)	(((x) & 0x1) << 7)
#define AD713X_INT_CONFIG_B_M_S_RD_CTRL_MSK		BIT(5)
#define AD713X_INT_CONFIG_B_M_S_RD_CTRL_MODE(x)		(((x) & 0x1) << 5)
/*
 * AD713X_REG_DEVICE_CONFIG
 */
#define AD713X_DEV_CONFIG_PWR_MODE_MSK			GENMASK(1, 0)
#define AD713X_DEV_CONFIG_PWR_MODE(x)			(((x) & 0x3) << 0)

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
 * AD713X_REG_DEVICE_CONFIG0
 */
#define AD713X_DEV_CONFIG0_DOUT0_EN_MSK			BIT(2)
#define AD713X_DEV_CONFIG0_DOUT0_EN_MODE(x)		(((x) & 0x1) << 2)
#define AD713X_DEV_CONFIG0_REF_GAIN_CORR_EN_MSK		BIT(1)
#define AD713X_DEV_CONFIG0_REF_GAIN_CORR_EN_MODE(x)	(((x) & 0x1) << 1)
#define AD713X_DEV_CONFIG0_CLKOUT_EN_MSK		BIT(0)
#define AD713X_DEV_CONFIG0_CLKOUT_EN_MODE(x)		(((x) & 0x1) << 0)

/*
 * AD713X_REG_DEVICE_CONFIG1
 */
#define AD713X_DEV_CONFIG1_ODR_TRACK_CNT_MSK		GENMASK(6, 4)
#define AD713X_DEV_CONFIG1_ODR_TRACK_CNT_MODE(x)	(((x) & 0x7) << 4)
#define AD713X_DEV_CONFIG1_INTERPOLATION_SELECTT_MSK	GENMASK(3, 1)
#define AD713X_DEV_CONFIG1_INTERPOLATION_SELECT_MODE(x)	(((x) & 0x7) << 1)

/*
 * AD713X_REG_DEVICE_CONFIG2
 */
#define AD713X_DEV_CONFIG2_PLL_LOCK_TIME_SEL_MSK	GENMASK(7, 4)
#define AD713X_DEV_CONFIG2_PLL_LOCK_TIME_SEL_MODE(x)	(((x) & 0xF) << 4)
#define AD713X_DEV_CONFIG2_ASRC_CLK_DIV_MSK		GENMASK(3, 0)
#define AD713X_DEV_CONFIG2_ASRC_CLK_DIV_MODE(x)		(((x) & 0xF) << 0)

/*
 * AD713X_REG_DEVICE_CONFIG3
 */
#define AD713X_DEV_CONFIG3_CRC_RST_SEL_MSK		BIT(7)
#define AD713X_DEV_CONFIG3_CRC_RST_SEL_MODE(x)		(((x) & 0x1) << 7)
#define AD713X_DEV_CONFIG3_FRAME_MSK			GENMASK(6, 4)
#define AD713X_DEV_CONFIG3_FRAME_MODE(x)		(((x) & 0x7) << 4)
#define AD713X_DEV_CONFIG3_DCLK_FREQ_SEL_MSK		GENMASK(3, 0)
#define AD713X_DEV_CONFIG3_DCLK_FREQ_SEL_MODE(x)	(((x) & 0xF) << 3)

/*
 * AD713X_REG_DEVICE_CONFIG4
 */
#define AD713X_DEV_CONFIG4_DAISY_CHAIN_DEV_NUM_MSK	GENMASK(7, 4)
#define AD713X_DEV_CONFIG4_DAISY_CHAIN_DEV_NUM_MODE(x)	(((x) & 0xF) << 4)
#define AD713X_DEV_CONFIG4_AVG_SEL_MSK			GENMASK(3, 2)
#define AD713X_DEV_CONFIG4_AVG_SEL_MODE(x)		(((x) & 0x3) << 2)
#define AD713X_DEV_CONFIG4_FORMAT_MSK			GENMASK(1, 0)
#define AD713X_DEV_CONFIG4_FORMAT_MODE(x)		(((x) & 0x3) << 0)

/*
 * AD713X_REG_DEVICE_CONFIG5
 */
#define AD713X_DEV_CONFIG5_MPC_CLKDEL_EN_MSK		BIT(4)
#define AD713X_DEV_CONFIG4_MPC_CLKDEL_EN_MODE(x)	(((x) & 0x1) << 4)
#define AD713X_DEV_CONFIG5_PAD_DRIVE_ST_DIG_MSK		GENMASK(3, 2)
#define AD713X_DEV_CONFIG4_PAD_DRIVE_ST_DIG_MODE(x)	(((x) & 0x3) << 2)
#define AD713X_DEV_CONFIG5_PAD_DRIVE_ST_SPI_MSK		GENMASK(1, 0)
#define AD713X_DEV_CONFIG4_PAD_DRIVE_ST_SPI_MODE(x)	(((x) & 0x3) << 0)

/*
 * AD713X_REG_POWER_DOWN_CONTROL
 */
#define AD713X_POWER_DOWN_CTRL_PWRDN_AUXADC1_MSK	BIT(7)
#define AD713X_POWER_DOWN_CTRL_PWRDN_AUXADC1_MODE(x)	(((x) & 0x1) << 7)
#define AD713X_POWER_DOWN_CTRL_PWRDN_AUXADC0_MSK	BIT(6)
#define AD713X_POWER_DOWN_CTRL_PWRDN_AUXADC0_MODE(x)	(((x) & 0x1) << 6)
#define AD713X_POWER_DOWN_CTRL_PWRDN_VCMBUF_MSK		BIT(4)
#define AD713X_POWER_DOWN_CTRL_PWRDN_VCMBUF_MODE(x)	(((x) & 0x1) << 4)
#define AD713X_POWER_DOWN_CTRL_PWRDN_ALDO_MSK		BIT(3)
#define AD713X_POWER_DOWN_CTRL_PWRDN_ALDO_MODE(x)	(((x) & 0x1) << 3)
#define AD713X_POWER_DOWN_CTRL_PWRDN_AFE_MSK		BIT(2)
#define AD713X_POWER_DOWN_CTRL_PWRDN_AFE_MODE(x)	(((x) & 0x1) << 2)
#define AD713X_POWER_DOWN_CTRL_SLEEP_MODE2_EN_MSK	BIT(1)
#define AD713X_POWER_DOWN_CTRL_SLEEP_MODE2_EN_MODE(x)	(((x) & 0x1) << 1)
#define AD713X_POWER_DOWN_CTRL_SLEEP_MODE1_EN_MSK	BIT(0)
#define AD713X_POWER_DOWN_CTRL_SLEEP_MODE1_EN_MODE(x)	(((x) & 0x1) << 0)

/*
 * AD713X_REG_SDO_PIN_SRC_SEL
 */
#define AD713X_SDO_PIN_SRC_SEL_BIT_MSK			BIT(0)
#define AD713X_SDO_PIN_SRC_SEL_BIT_MODE(x)		(((x) & 0x1) << 0)

/*
 * AD713X_REG_VCMBUF_CTRL
 */
#define AD713X_VCMBUF_CTRL_VCMBUF_REF_DIV_SEL_MSK	GENMASK(5, 1)
#define AD713X_VCMBUF_CTRL_VCMBUF_REF_DIV_SEL_MODE(x)	(((x) & 0x1F) << 1)
#define AD713X_VCMBUF_CTRL_VCMBUF_REF_SEL_MSK		BIT(0)
#define AD713X_VCMBUF_CTRL_VCMBUF_REF_SEL_MODE(x)	(((x) & 0x1) << 0)

/*
 * AD713X_REG_AIN_POWER_DOWN_CONTROL
 */
#define AD713X_AIN_POWER_DOWN_CONTROL_PWRDN_CH3_MSK	BIT(3)
#define AD713X_AIN_POWER_DOWN_CONTROL_PWRDN_CH3_MODE(x)	(((x) & 0x1) << 3)
#define AD713X_AIN_POWER_DOWN_CONTROL_PWRDN_CH2_MSK	BIT(2)
#define AD713X_AIN_POWER_DOWN_CONTROL_PWRDN_CH2_MODE(x)	(((x) & 0x1) << 2)
#define AD713X_AIN_POWER_DOWN_CONTROL_PWRDN_CH1_MSK	BIT(1)
#define AD713X_AIN_POWER_DOWN_CONTROL_PWRDN_CH1_MODE(x)	(((x) & 0x1) << 1)
#define AD713X_AIN_POWER_DOWN_CONTROL_PWRDN_CH0_MSK	BIT(0)
#define AD713X_AIN_POWER_DOWN_CONTROL_PWRDN_CH0_MODE(x)	(((x) & 0x1) << 0)

/*
 * AD713X_REG_ERROR_PIN_SRC_CONTROL
 */
#define AD713X_ERRO_PIN_EN_ADCINP_MSK		BIT(5)
#define AD713X_ERRO_PIN_EN_ADCINP_MODE(x)	(((x) & 0x1) << 5)

/*
 * AD713X_REG_DIAGNOSTIC_CONTROL
 */
#define AD713X_DIAG_CTRL_CRC_MSK		BIT(2)
#define AD713X_DIAG_CTRL_CRC_MODE(x)		(((x) & 0x1) << 2)

/*
 * AD713X_REG_CHAN_DIG_FILTER_SEL
 */
#define AD713X_DIGFILTER_SEL_CH_MSK(ch)		(GENMASK(1, 0) << (2 * ch))
#define AD713X_DIGFILTER_SEL_CH_MODE(x, ch)	(((x) & 0x3) << (2 * ch))

/*
 * AD713X_REG_FIR_BW_SEL
 */
#define AD713X_FIR_BW_SEL_CH_MSK(ch)		BIT(ch)
#define AD713X_FIR_BW_SEL_CH_MODE(x, ch)	(((x) & 0x1) << ch)

/*
 * AD713X_REG_CHANNEL_ODR_SELECT
 */
#define AD713X_ODR_SEL_CH_MSK(ch)		(GENMASK(1, 0) << (2 * ch))
#define AD713X_ODR_SEL_CH_MODE(x, ch)		(((x) & 0x3) << (2 * ch))

/*
 * AD713X_REG_CHx_GAIN_LSB
 */
#define AD713X_CH_GAIN_LSB_MSK			GENMASK(7, 0)
#define AD713X_CH_GAIN_LSB_MODE(x)		(((x) & 0xFF) << 0)

/*
 * AD713X_REG_CHx_GAIN_MID
 */
#define AD713X_CH_GAIN_MID_MSK			GENMASK(7, 0)
#define AD713X_CH_GAIN_MID_MODE(x)		(((x) & 0xFF) << 0)

/*
 * AD713X_REG_CHx_GAIN_MSB
 */
#define AD713X_CH_GAIN_MSB_MSK			GENMASK(3, 0)
#define AD713X_CH_GAIN_MSB_MODE(x)		(((x) & 0xF) << 0)

/*
 * AD713X_REG_CHx_OFFSET_LSB
 */
#define AD713X_CH_OFFSET_LSB_MSK		GENMASK(7, 0)
#define AD713X_CH_OFFSET_LSB_MODE(x)		(((x) & 0xFF) << 0)

/*
 * AD713X_REG_CHx_OFFSET_MID
 */
#define AD713X_CH_OFFSET_MID_MSK		GENMASK(7, 0)
#define AD713X_CH_OFFSET_MID_MODE(x)		(((x) & 0xFF) << 0)

/*
 * AD713X_REG_CHx_OFFSET_MSB
 */
#define AD713X_CH_OFFSET_MSB_MSK		GENMASK(6, 0)
#define AD713X_CH_OFFSET_MSB_MODE(x)		(((x) & 0x7F) << 0)

#define AD713X_REG_READ(x) 			((1 << 7) | (x & 0x7F))

typedef enum {
	ID_AD7132,
	ID_AD7134,
	ID_AD7136
} ad713x_supported_dev_ids;

typedef enum {
	LOW_POWER = 0x01,
	HIGH_POWER
} ad713x_power_mode;

typedef enum {
	ADC_16_BIT_DATA,
	ADC_24_BIT_DATA,
	ADC_32_BIT_DATA,
	INVALID
} ad713x_adc_data_len;

typedef enum {
	NO_CRC,
	CRC_6,
	CRC_8
} ad713x_crc_header;

typedef enum {
	SINGLE_CH_DC,
	DUAL_CH_DC,
	QUAD_CH_DC,
	CH_AVG_MODE
} ad713x_doutx_format;

typedef enum {
	FIR,
	SINC6,
	SINC3,
	SINC3_50_60_REJ
} ad713x_dig_filter_sel;

typedef enum {
	CH0,
	CH1,
	CH2,
	CH3
} ad713x_channels;

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

typedef struct {
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
	ad713x_supported_dev_ids dev_id;
	ad713x_adc_data_len	adc_data_len;
	ad713x_crc_header	crc_header;
} ad713x_dev;

typedef struct {
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
	ad713x_supported_dev_ids dev_id;
	ad713x_adc_data_len	adc_data_len;
	ad713x_crc_header	crc_header;
	ad713x_power_mode 	power_mode;
	bool			dout0_en;
	ad713x_doutx_format	format;
	bool 			clk_delay_en;
} ad713x_init_param;

int32_t ad713x_spi_reg_read(ad713x_dev *dev,
			    uint8_t reg_addr,
			    uint8_t *reg_data);
int32_t ad713x_spi_reg_write(ad713x_dev *dev,
			     uint8_t reg_addr,
			     uint8_t reg_data);
int32_t ad713x_spi_write_mask(ad713x_dev *dev,
			      uint8_t reg_addr,
			      uint32_t mask,
			      uint8_t data);
int32_t ad713x_set_power_mode(ad713x_dev *dev,
			      ad713x_power_mode mode);
int32_t ad713x_dout0_enable(ad713x_dev *dev,
			    bool dout0_en);
int32_t ad713x_set_out_data_frame(ad713x_dev *dev,
				  ad713x_adc_data_len adc_data_len,
				  ad713x_crc_header crc_header);
int32_t ad713x_dout_format_config(ad713x_dev *dev,
				  ad713x_doutx_format format);
int32_t ad713x_mag_phase_clk_delay(ad713x_dev *dev,
				   bool clk_delay_en);
int32_t ad713x_dig_filter_sel_ch(ad713x_dev *dev,
				 ad713x_dig_filter_sel filter,
				 ad713x_channels	ch);
int32_t ad713x_init(ad713x_dev **device,
		    ad713x_init_param init_param);
int32_t ad713x_remove(ad713x_dev *dev);
#endif /* SRC_AD713X_H_ */

