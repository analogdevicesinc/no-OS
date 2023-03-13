/***************************************************************************//**
 *   @file   ad7779.h
 *   @brief  Header file of AD7779 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2016(c) Analog Devices, Inc.
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
#ifndef AD7779_H_
#define AD7779_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD7779_REG_CH_CONFIG(ch)		(0x00 + (ch))		// Channel Configuration
#define AD7779_REG_CH_DISABLE			0x08			// Disable clocks to ADC channel
#define AD7779_REG_CH_SYNC_OFFSET(ch)		(0x09 + (ch))		// Channel SYNC Offset
#define AD7779_REG_GENERAL_USER_CONFIG_1	0x11			// General User Config 1
#define AD7779_REG_GENERAL_USER_CONFIG_2	0x12			// General User Config 2
#define AD7779_REG_GENERAL_USER_CONFIG_3	0x13			// General User Config 3
#define AD7779_REG_DOUT_FORMAT			0x14			// Data out format
#define AD7779_REG_ADC_MUX_CONFIG		0x15			// Main ADC meter and reference Mux control
#define AD7779_REG_GLOBAL_MUX_CONFIG		0x16			// Global diagnostics mux
#define AD7779_REG_GPIO_CONFIG			0x17			// GPIO config
#define AD7779_REG_GPIO_DATA			0x18			// GPIO Data
#define AD7779_REG_BUFFER_CONFIG_1		0x19			// Buffer Config 1
#define AD7779_REG_BUFFER_CONFIG_2		0x1A			// Buffer Config 2
#define AD7779_REG_CH_OFFSET_UPPER_BYTE(ch)	(0x1C + (ch) * 6)	// Channel offset upper byte
#define AD7779_REG_CH_OFFSET_MID_BYTE(ch)	(0x1D + (ch) * 6)	// Channel offset middle byte
#define AD7779_REG_CH_OFFSET_LOWER_BYTE(ch)	(0x1E + (ch) * 6)	// Channel offset lower byte
#define AD7779_REG_CH_GAIN_UPPER_BYTE(ch)	(0x1F + (ch) * 6)	// Channel gain upper byte
#define AD7779_REG_CH_GAIN_MID_BYTE(ch)		(0x20 + (ch) * 6)	// Channel gain middle byte
#define AD7779_REG_CH_GAIN_LOWER_BYTE(ch)	(0x21 + (ch) * 6)	// Channel gain lower byte
#define AD7779_REG_CH_ERR_REG(ch)		(0x4C + (ch))		// Channel Status Register
#define AD7779_REG_CH0_1_SAT_ERR		0x54			// Channel 0/1 DSP errors
#define AD7779_REG_CH2_3_SAT_ERR		0x55			// Channel 2/3 DSP errors
#define AD7779_REG_CH4_5_SAT_ERR		0x56			// Channel 4/5 DSP errors
#define AD7779_REG_CH6_7_SAT_ERR		0x57			// Channel 6/7 DSP errors
#define AD7779_REG_CHX_ERR_REG_EN		0x58			// Channel 0-7 Error Reg Enable
#define AD7779_REG_GEN_ERR_REG_1		0x59			// General Errors Register 1
#define AD7779_REG_GEN_ERR_REG_1_EN		0x5A			// General Errors Register 1 Enable
#define AD7779_REG_GEN_ERR_REG_2		0x5B			// General Errors Register 2
#define AD7779_REG_GEN_ERR_REG_2_EN		0x5C			// General Errors Register 2 Enable
#define AD7779_REG_STATUS_REG_1			0x5D			// Error Status Register 1
#define AD7779_REG_STATUS_REG_2			0x5E			// Error Status Register 2
#define AD7779_REG_STATUS_REG_3			0x5F			// Error Status Register 3
#define AD7779_REG_SRC_N_MSB			0x60			// Decimation Rate (N) MSB
#define AD7779_REG_SRC_N_LSB			0x61			// Decimation Rate (N) LSB
#define AD7779_REG_SRC_IF_MSB			0x62			// Decimation Rate (IF) MSB
#define AD7779_REG_SRC_IF_LSB			0x63			// Decimation Rate (IF) LSB
#define AD7779_REG_SRC_UPDATE			0x64			// SRC load source and load update

/* AD7779_REG_CHx_CONFIG */
#define AD7779_CH_GAIN(x)			(((x) & 0x3) << 6)
#define AD7779_CH_RX				(1 << 4)

/* AD7779_REG_CH_DISABLE */
#define AD7779_CH_DISABLE(x)			(1 << (x))

/* AD7779_REG_GENERAL_USER_CONFIG_1 */
#define AD7779_ALL_CH_DIS_MCLK_EN		(1 << 7)
#define AD7779_MOD_POWERMODE			(1 << 6)
#define AD7779_PDB_VCM				(1 << 5)
#define AD7779_PDB_REFOUT_BUF			(1 << 4)
#define AD7779_PDB_SAR				(1 << 3)
#define AD7779_PDB_RC_OSC			(1 << 2)
#define AD7779_SOFT_RESET(x)			(((x) & 0x3) << 0)

/* AD7779_REG_GENERAL_USER_CONFIG_2 */
#define AD7771_FILTER_MODE			(1 << 6)
#define AD7779_SAR_DIAG_MODE_EN			(1 << 5)
#define AD7779_SDO_DRIVE_STR(x)			(((x) & 0x3) << 3)
#define AD7779_DOUT_DRIVE_STR(x)		(((x) & 0x3) << 1)
#define AD7779_SPI_SYNC				(1 << 0)

/* AD7779_REG_GENERAL_USER_CONFIG_3 */
#define AD7779_CONVST_DEGLITCH_DIS(x)		(((x) & 0x3) << 6)
#define AD7779_SPI_SLAVE_MODE_EN		(1 << 4)
#define AD7779_CLK_QUAL_DIS			(1 << 0)

/* AD7779_REG_DOUT_FORMAT */
#define AD7779_DOUT_FORMAT(x)			(((x) & 0x3) << 6)
#define AD7779_DOUT_HEADER_FORMAT		(1 << 5)
#define AD7779_DCLK_CLK_DIV(x)			(((x) & 0x7) << 1)

/* AD7779_REG_ADC_MUX_CONFIG */
#define AD7779_REF_MUX_CTRL(x)			(((x) & 0x3) << 6)

/* AD7779_REG_GLOBAL_MUX_CONFIG */
#define AD7779_GLOBAL_MUX_CTRL(x)		(((x) & 0x1F) << 3)

/* AD7779_REG_BUFFER_CONFIG_1 */
#define AD7779_REF_BUF_POS_EN			(1 << 4)
#define AD7779_REF_BUF_NEG_EN			(1 << 3)

/* AD7779_REG_BUFFER_CONFIG_2 */
#define AD7779_REFBUFP_PREQ			(1 << 7)
#define AD7779_REFBUFN_PREQ			(1 << 6)
#define AD7779_PDB_ALDO1_OVRDRV			(1 << 2)
#define AD7779_PDB_ALDO2_OVRDRV			(1 << 1)
#define AD7779_PDB_DLDO_OVRDRV			(1 << 0)

/* AD7779_REG_GEN_ERR_REG_1_EN */
#define AD7779_MEMMAP_CRC_TEST_EN		(1 << 5)
#define AD7779_ROM_CRC_TEST_EN			(1 << 4)
#define AD7779_SPI_CLK_COUNT_TEST_EN		(1 << 3)
#define AD7779_SPI_INVALID_READ_TEST_EN		(1 << 2)
#define AD7779_SPI_INVALID_WRITE_TEST_EN	(1 << 1)
#define AD7779_SPI_CRC_TEST_EN			(1 << 0)

#define AD7779_CRC8_POLY			0x07

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
typedef enum {
	AD7779_PIN_CTRL,
	AD7779_SPI_CTRL,
} ad7779_ctrl_mode;

typedef enum {
	AD7779_INT_REG,
	AD7779_SD_CONV,
	AD7779_SAR_CONV,
} ad7779_spi_op_mode;

typedef enum {
	AD7779_CH0,
	AD7779_CH1,
	AD7779_CH2,
	AD7779_CH3,
	AD7779_CH4,
	AD7779_CH5,
	AD7779_CH6,
	AD7779_CH7,
} ad7779_ch;

typedef enum {
	AD7779_ENABLE,
	AD7779_DISABLE,
} ad7779_state;

typedef enum {
	AD7779_GAIN_1,
	AD7779_GAIN_2,
	AD7779_GAIN_4,
	AD7779_GAIN_8,
} ad7779_gain;

typedef enum {
	AD7779_DCLK_DIV_1,
	AD7779_DCLK_DIV_2,
	AD7779_DCLK_DIV_4,
	AD7779_DCLK_DIV_8,
	AD7779_DCLK_DIV_16,
	AD7779_DCLK_DIV_32,
	AD7779_DCLK_DIV_64,
	AD7779_DCLK_DIV_128,
} ad7779_dclk_div;

typedef enum {
	AD7779_LOW_PWR,
	AD7779_HIGH_RES,
} ad7779_pwr_mode;

typedef enum {
	AD7779_EXT_REF,
	AD7779_INT_REF,
	AD7779_EXT_SUPPLY,
	AD7779_EXT_REF_INV,
} ad7779_ref_type;

typedef enum {
	AD7779_REFX_P,
	AD7779_REFX_N,
} ad7779_refx_pin;

typedef enum {
	AD7779_REF_BUF_ENABLED,
	AD7779_REF_BUF_PRECHARGED,
	AD7779_REF_BUF_DISABLED,
} ad7779_ref_buf_op_mode;

typedef enum {
	AD7779_AUXAINP_AUXAINN,
	AD7779_DVBE_AVSSX,
	AD7779_REF1P_REF1N,
	AD7779_REF2P_REF2N,
	AD7779_REF_OUT_AVSSX,
	AD7779_VCM_AVSSX,
	AD7779_AREG1CAP_AVSSX_ATT,
	AD7779_AREG2CAP_AVSSX_ATT,
	AD7779_DREGCAP_DGND_ATT,
	AD7779_AVDD1A_AVSSX_ATT,
	AD7779_AVDD1B_AVSSX_ATT,
	AD7779_AVDD2A_AVSSX_ATT,
	AD7779_AVDD2B_AVSSX_ATT,
	AD7779_IOVDD_DGND_ATT,
	AD7779_AVDD4_AVSSX,
	AD7779_DGND_AVSS1A_ATT,
	AD7779_DGND_AVSS1B_ATT,
	AD7779_DGND_AVSSX_ATT,
	AD7779_AVDD4_AVSSX_ATT,
	AD7779_REF1P_AVSSX,
	AD7779_REF2P_AVSSX,
	AD7779_AVSSX_AVDD4_ATT,
} ad7779_sar_mux;

typedef struct {
	/* SPI */
	struct no_os_spi_desc		*spi_desc;
	/* GPIO */
	struct no_os_gpio_desc	*gpio_reset;
	struct no_os_gpio_desc	*gpio_mode0;
	struct no_os_gpio_desc	*gpio_mode1;
	struct no_os_gpio_desc	*gpio_mode2;
	struct no_os_gpio_desc	*gpio_mode3;
	struct no_os_gpio_desc	*gpio_dclk0;
	struct no_os_gpio_desc	*gpio_dclk1;
	struct no_os_gpio_desc	*gpio_dclk2;
	struct no_os_gpio_desc	*gpio_sync_in;
	struct no_os_gpio_desc	*gpio_convst_sar;
	/* Device Settings */
	ad7779_ctrl_mode	ctrl_mode;
	ad7779_state		spi_crc_en;
	ad7779_spi_op_mode	spi_op_mode;
	ad7779_state		state[8];
	ad7779_gain		gain[8];
	uint16_t		dec_rate_int;
	uint16_t		dec_rate_dec;
	ad7779_ref_type		ref_type;
	ad7779_pwr_mode		pwr_mode;
	ad7779_dclk_div		dclk_div;
	uint8_t			sync_offset[8];
	uint32_t		offset_corr[8];
	uint32_t		gain_corr[8];
	ad7779_ref_buf_op_mode	ref_buf_op_mode[2];
	ad7779_state		sar_state;
	ad7779_sar_mux		sar_mux;
	ad7779_state		sinc5_state;	// Can be enabled only for AD7771
	bool				read_from_cache;
	uint8_t			cached_reg_val[AD7779_REG_SRC_UPDATE + 1];
} ad7779_dev;

typedef struct {
	/* SPI */
	struct no_os_spi_init_param		spi_init;
	/* GPIO */
	struct no_os_gpio_init_param	gpio_reset;
	struct no_os_gpio_init_param	gpio_mode0;
	struct no_os_gpio_init_param	gpio_mode1;
	struct no_os_gpio_init_param	gpio_mode2;
	struct no_os_gpio_init_param	gpio_mode3;
	struct no_os_gpio_init_param	gpio_dclk0;
	struct no_os_gpio_init_param	gpio_dclk1;
	struct no_os_gpio_init_param	gpio_dclk2;
	struct no_os_gpio_init_param	gpio_sync_in;
	struct no_os_gpio_init_param	gpio_convst_sar;
	/* Device Settings */
	ad7779_ctrl_mode	ctrl_mode;
	ad7779_state		spi_crc_en;
	ad7779_state		state[8];
	ad7779_gain		gain[8];
	uint16_t		dec_rate_int;
	uint16_t		dec_rate_dec;
	ad7779_ref_type		ref_type;
	ad7779_pwr_mode		pwr_mode;
	ad7779_dclk_div		dclk_div;
	uint8_t			sync_offset[8];
	uint32_t		offset_corr[8];
	uint32_t		gain_corr[8];
	ad7779_ref_buf_op_mode	ref_buf_op_mode[2];
	ad7779_state		sinc5_state;	// Can be enabled only for AD7771
	bool			read_from_cache;
} ad7779_init_param;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Compute CRC8 checksum. */
uint8_t ad7779_compute_crc8(uint8_t *data,
			    uint8_t data_size);
/* SPI read from device. */
int32_t ad7779_spi_int_reg_read(ad7779_dev *dev,
				uint8_t reg_addr,
				uint8_t *reg_data);
/* SPI write to device. */
int32_t ad7779_spi_int_reg_write(ad7779_dev *dev,
				 uint8_t reg_addr,
				 uint8_t reg_data);
/* SPI read from device using a mask. */
int32_t ad7779_spi_int_reg_read_mask(ad7779_dev *dev,
				     uint8_t reg_addr,
				     uint8_t mask,
				     uint8_t *data);
/* SPI write to device using a mask. */
int32_t ad7779_spi_int_reg_write_mask(ad7779_dev *dev,
				      uint8_t reg_addr,
				      uint8_t mask,
				      uint8_t data);
/* SPI SAR conversion code read. */
int32_t ad7779_spi_sar_read_code(ad7779_dev *dev,
				 ad7779_sar_mux mux_next_conv,
				 uint16_t *sar_code);
/* Set SPI operation mode. */
int32_t ad7779_set_spi_op_mode(ad7779_dev *dev,
			       ad7779_spi_op_mode mode);
/* Get SPI operation mode. */
int32_t ad7779_get_spi_op_mode(ad7779_dev *dev,
			       ad7779_spi_op_mode *mode);
/* Set the state (enable, disable) of the channel. */
int32_t ad7779_set_state(ad7779_dev *dev,
			 ad7779_ch ch,
			 ad7779_state state);
/* Get the state (enable, disable) of the selected channel. */
int32_t ad7779_get_state(ad7779_dev *dev,
			 ad7779_ch ch,
			 ad7779_state *state);
/* Update the state of the MODEx pins according to the settings specified in
 * the device structure. */
int32_t ad7779_do_update_mode_pins(ad7779_dev *dev);
/* Set the gain of the selected channel. */
int32_t ad7779_set_gain(ad7779_dev *dev,
			ad7779_ch ch,
			ad7779_gain gain);
/* Get the gain of the selected channel. */
int32_t ad7779_get_gain(ad7779_dev *dev,
			ad7779_ch ch,
			ad7779_gain *gain);
/* Set the decimation rate. */
int32_t ad7779_set_dec_rate(ad7779_dev *dev,
			    uint16_t int_val,
			    uint16_t dec_val);
/* Get the decimation rate. */
int32_t ad7779_get_dec_rate(ad7779_dev *dev,
			    uint16_t *int_val,
			    uint16_t *dec_val);
/* Set the power mode. */
int32_t ad7779_set_power_mode(ad7779_dev *dev,
			      ad7779_pwr_mode pwr_mode);
/* Get the power mode. */
int32_t ad7779_get_power_mode(ad7779_dev *dev,
			      ad7779_pwr_mode *pwr_mode);
/* Set the reference type. */
int32_t ad7779_set_reference_type(ad7779_dev *dev,
				  ad7779_ref_type ref_type);
/* Get the reference type. */
int32_t ad7779_get_reference_type(ad7779_dev *dev,
				  ad7779_ref_type *ref_type);
/* Set the DCLK divider. */
int32_t ad7779_set_dclk_div(ad7779_dev *dev,
			    ad7779_dclk_div div);
/* Get the DCLK divider. */
int32_t ad7779_get_dclk_div(ad7779_dev *dev,
			    ad7779_dclk_div *div);
/* Set the synchronization offset of the selected channel. */
int32_t ad7779_set_sync_offset(ad7779_dev *dev,
			       ad7779_ch ch,
			       uint8_t sync_offset);
/* Get the synchronization offset of the selected channel. */
int32_t ad7779_get_sync_offset(ad7779_dev *dev,
			       ad7779_ch ch,
			       uint8_t *sync_offset);
/* Set the offset correction of the selected channel. */
int32_t ad7779_set_offset_corr(ad7779_dev *dev,
			       ad7779_ch ch,
			       uint32_t offset);
/* Get the offset correction of the selected channel. */
int32_t ad7779_get_offset_corr(ad7779_dev *dev,
			       ad7779_ch ch,
			       uint32_t *offset);
/* Set the gain correction of the selected channel. */
int32_t ad7779_set_gain_corr(ad7779_dev *dev,
			     ad7779_ch ch,
			     uint32_t gain);
/* Get the gain correction of the selected channel. */
int32_t ad7779_get_gain_corr(ad7779_dev *dev,
			     ad7779_ch ch,
			     uint32_t *gain);
/* Set the reference buffer operation mode of the selected pin. */
int32_t ad7779_set_ref_buf_op_mode(ad7779_dev *dev,
				   ad7779_refx_pin refx_pin,
				   ad7779_ref_buf_op_mode mode);
/* Get the reference buffer operation mode of the selected pin. */
int32_t ad7779_get_ref_buf_op_mode(ad7779_dev *dev,
				   ad7779_refx_pin refx_pin,
				   ad7779_ref_buf_op_mode *mode);
/* Set the SAR ADC configuration. */
int32_t ad7779_set_sar_cfg(ad7779_dev *dev,
			   ad7779_state state,
			   ad7779_sar_mux mux);
/* Get the SAR ADC configuration. */
int32_t ad7779_get_sar_cfg(ad7779_dev *dev,
			   ad7779_state *state,
			   ad7779_sar_mux *mux);
/* Do a single SAR conversion. */
int32_t ad7779_do_single_sar_conv(ad7779_dev *dev,
				  ad7779_sar_mux mux,
				  uint16_t *sar_code);
/* Do a SPI software reset. */
int32_t ad7779_do_spi_soft_reset(ad7779_dev *dev);
/* Set the state (enable, disable) of the SINC5 filter. */
int32_t ad7771_set_sinc5_filter_state(ad7779_dev *dev,
				      ad7779_state state);
/* Get the state (enable, disable) of the SINC5 filter. */
int32_t ad7771_get_sinc5_filter_state(ad7779_dev *dev,
				      ad7779_state *state);
/* Initialize the device. */
int32_t ad7779_init(ad7779_dev **device,
		    ad7779_init_param init_param);

/* Free the resources allocated by ad7779_init(). */
int32_t ad7779_remove(ad7779_dev *dev);

#endif // AD7779_H_
