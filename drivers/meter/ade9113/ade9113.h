/***************************************************************************//**
 *   @file   ade9113.h
 *   @brief  Header file of ADE9113 Driver.
 *   @author George Mois (george.mois@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#ifndef __ADE9113_H__
#define __ADE9113_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "no_os_util.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_irq.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/* SPI commands */
#define ADE9113_SPI_READ			NO_OS_BIT(7)

/* Long/Short operation bit, set for lon read/write */
#define ADE9113_OP_MODE_LONG			NO_OS_BIT(6)

/* ADE9113 CRC constants */
#define ADE9113_CRC8_POLY			0x07
#define ADE9113_CRC16_POLY			0x1021
#define ADE9113_CRC16_INIT_VAL			0xFFFF

/* ENABLE and DISABLE */
#define ENABLE					1u
#define DISABLE					0u

/* ADE9113 Register Map */
#define ADE9113_REG_SWRST			0x001
#define ADE9113_REG_CONFIG0			0x002
#define ADE9113_REG_CONFIG_FILT			0x003
#define ADE9113_REG_CONFIG_ISO_ACC		0x005
#define ADE9113_REG_CRC_RESULT_HI		0x006
#define ADE9113_REG_CRC_RESULT_LO		0x007
#define ADE9113_REG_EFUSE_REFRESH		0x008
#define ADE9113_REG_EMI_CONFIG			0x009
#define ADE9113_REG_EMI_HI_MASK			0x00A
#define ADE9113_REG_EMI_LO_MASK			0x00B
#define ADE9113_REG_EMI_HI_LIMIT		0x00C
#define ADE9113_REG_EMI_MID_LIMIT		0x00D
#define ADE9113_REG_EMI_LO_LIMIT		0x00E
#define ADE9113_REG_MASK0			0x00F
#define ADE9113_REG_MASK1			0x010
#define ADE9113_REG_MASK2			0x011
#define ADE9113_REG_CONFIG_ZX			0x012
#define ADE9113_REG_SCRATCH			0x013
#define ADE9113_REG_SYNC_SNAP			0x014
#define ADE9113_REG_COUNTER_HI			0x015
#define ADE9113_REG_COUNTER_LO			0x016
#define ADE9113_REG_SNAPSHOT_COUNT_HI		0x017
#define ADE9113_REG_SNAPSHOT_COUNT_LO		0x018
#define ADE9113_REG_WR_LOCK			0x01F
#define ADE9113_REG_STATUS0			0x020
#define ADE9113_REG_STATUS1			0x021
#define ADE9113_REG_STATUS2			0x022
#define ADE9113_REG_COM_FLT_TYPE		0x023
#define ADE9113_REG_COM_FLT_COUNT		0x024
#define ADE9113_REG_CONFIG_CRC			0x025
#define ADE9113_REG_I_WAV_HI			0x026
#define ADE9113_REG_I_WAV_MD			0x027
#define ADE9113_REG_I_WAV_LO 			0x028
#define ADE9113_REG_V1_WAV_HI			0x029
#define ADE9113_REG_V1_WAV_MD			0x02A
#define ADE9113_REG_V1_WAV_LO			0x02B
#define ADE9113_REG_V2_WAV_HI			0x02C
#define ADE9113_REG_V2_WAV_MD			0x02D
#define ADE9113_REG_V2_WAV_LO			0x02E
#define ADE9113_REG_UNIQUE_PART_ID_5		0x075
#define ADE9113_REG_UNIQUE_PART_ID_4		0x076
#define ADE9113_REG_UNIQUE_PART_ID_3		0x077
#define ADE9113_REG_UNIQUE_PART_ID_2		0x078
#define ADE9113_REG_UNIQUE_PART_ID_1		0x079
#define ADE9113_REG_UNIQUE_PART_ID_0		0x07A
#define ADE9113_REG_SILICON_REVISION		0x07D
#define ADE9113_REG_VERSION_PRODUCT		0x07E

/* ADE9113 SWRST command */
#define ADE9113_SWRST_CMD			0xD6

/* ADE9113_REG_CONFIG0 Bit Definition */
#define ADE9113_STREAM_DBG_MSK			NO_OS_GENMASK(3, 2)
#define ADE9113_CRC_EN_SPI_WRITE_MSK		NO_OS_BIT(1)
#define ADE9113_CLOUT_EN_MSK			NO_OS_BIT(0)

/* ADE9113_REG_CONFIG_FILT Bit Definition */
#define ADE9113_V2_ADC_INVERT_MSK		NO_OS_BIT(6)
#define ADE9113_V1_ADC_INVERT_MSK		NO_OS_BIT(5)
#define ADE9113_I_ADC_INVERT_MSK		NO_OS_BIT(4)
#define ADE9113_LPF_BW_MSK			NO_OS_BIT(3)
#define ADE9113_DATAPATH_CONFIG_MSK		NO_OS_GENMASK(2, 0)

/* ADE9113_REG_CONFIG_ISO_ACC Bit Definition */
#define ADE9113_ISO_WR_ACC_EN_MSK		NO_OS_BIT(0)

/* ADE9113_REG_EFUSE_REFRESH Bit Definition */
#define ADE9113_EFUSE_REFRESH_MSK		NO_OS_BIT(0)

/* ADE9113_REG_EMI_CONFIG Bit Definition */
#define ADE9113_EMI_CONFIG_MSK			NO_OS_GENMASK(2, 0)

/* ADE9113_REG_MASK0/ADE9113_REG_STATUS0 Bit Definition */
#define ADE9113_STATUS1X_MSK	 		NO_OS_BIT(7)
#define ADE9113_STATUS2X_MSK 			NO_OS_BIT(6)
#define ADE9113_COM_UP_MSK	 		NO_OS_BIT(4)
#define ADE9113_CRC_CHG_MSK 			NO_OS_BIT(3)
#define ADE9113_SPI_CRC_ERR_MSK 		NO_OS_BIT(1)
#define ADE9113_COMFLT_ERR_MSK			NO_OS_BIT(0)

/* ADE9113_REG_MASK1/ADE9113_REG_STATUS1 Bit Definition */
#define ADE9113_V2_WAV_OVRNG_MSK		NO_OS_BIT(3)
#define ADE9113_V1_WAV_OVRNG_MSK		NO_OS_BIT(2)
#define ADE9113_I_WAV_OVRNG_MSK			NO_OS_BIT(1)
#define ADE9113_ADC_SYNC_DONE_MSK		NO_OS_BIT(0)

/* ADE9113_REG_MASK2/ADE9113_REG_STATUS2 Bit Definition */
#define ADE9113_ISO_CLK_STBL_ERR_MSK		NO_OS_BIT(6)
#define ADE9113_ISO_PHY_CRC_ERR_MSK		NO_OS_BIT(5)
#define ADE9113_ISO_EFUSE_MEM_ERR_MSK		NO_OS_BIT(4)
#define ADE9113_ISO_DIG_MOD_V2_OVF_MSK		NO_OS_BIT(3)
#define ADE9113_ISO_DIG_MOD_V1_OVF_MSK		NO_OS_BIT(2)
#define ADE9113_ISO_DIG_MOD_I_OVF_MSK		NO_OS_BIT(1)
#define ADE9113_ISO_TEST_MMR_ERR_MSK		NO_OS_BIT(0)

/* ADE9113_REG_CONFIG_ZX Bit Definition */
#define ADE9113_ZX_EDGE_SEL_MSK			NO_OS_GENMASK(3, 2)
#define ADE9113_ZX_CHANNEL_CONFIG_MSK		NO_OS_GENMASK(1, 0)

/* ADE9113_REG_SYNC_SNAP Bit Definition */
#define ADE9113_PREP_BROADCAST_MSK		NO_OS_BIT(2)
#define ADE9113_ALIGN_MSK			NO_OS_BIT(1)
#define ADE9113_SNAPSHOT_MSK			NO_OS_BIT(0)

/* ADE9113_REG_COUNTER_HI Bit Definition */
#define ADE9113_COUNTER_HI_MSK			NO_OS_GENMASK(5, 0)

/* ADE9113_REG_COUNTER_LO Bit Definition */
#define ADE9113_COUNTER_LO_MSK			NO_OS_GENMASK(8, 0)

/* ADE9113_REG_SNAPSHOT_COUNT_HI Bit Definition */
#define ADE9113_SNAPSHOT_COUNT_HI_MSK		NO_OS_GENMASK(5, 0)

/* ADE9113_REG_SNAPSHOT_COUNT_LO Bit Definition */
#define ADE9113_SNAPSHOT_COUNT_LO_MSK		NO_OS_GENMASK(7, 0)

/* ADE9113_REG_STATUS0 Bit Definition */
#define ADE9113_RESET_DONE_MSK			NO_OS_BIT(5)
#define ADE9113_EFUSE_MEM_ERR_MSK		NO_OS_BIT(2)

/* ADE9113_REG_COM_FLT_TYPE Bit Definition */
#define ADE9113_ISO_STATUS_RD_ECC_ERR_MSK	NO_OS_BIT(2)
#define ADE9113_ISO_PHY_ERR_MSK			NO_OS_BIT(1)
#define ADE9113_ISO_ECC_ERR_MSK			NO_OS_BIT(1)

/* ADE9113_REG_CONFIG_CRC Bit Definition */
#define ADE9113_CRC_DONE_MSK			NO_OS_BIT(1)
#define ADE9113_CRC_FORCE_MSK			NO_OS_BIT(0)

/* ADE9113_REG_SILICON_REVISION Bit Definition */
#define ADE9113_NONISO_CHIP_REV_MSK		NO_OS_GENMASK(7, 4)
#define ADE9113_ISO_CHIP_REV_MSK		NO_OS_GENMASK(3, 0)

/* Configuration Register Write Lock */
#define ADE9113_LOCK_KEY			0XD4
#define ADE9113_UNLOCK_KEY			0X5E

/* Version Product */
#define ADE9113_3_CHANNEL_ADE9113		0U
#define ADE9113_2_CHANNEL_ADE9112		1U
#define ADE9113_NONISOLATED_ADE9103		3U

/* Nominal reference voltage */
#define ADE9113_VREF				(1249810)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum ade9113_stream_debug_e
 * @brief ADE9113 Stream Debug.
 */
enum ade9113_stream_debug_e {
	/* Normal Mode */
	ADE9113_STREAM_NORMAL_MODE = 0,
	/* Static Mode */
	ADE9113_STREAM_STATIC_MODE,
	/* Data Increments at ADC Conversion Rate */
	ADE9113_STREAM_INCREMENTS_MODE,
	/* Same as functional mode */
	ADE9113_STREAM_FUNCTIONAL_MODE
};

/**
 * @enum ade9113_datapath_config_e
 * @brief ADE9113 digital signal processing configuration.
 */
enum ade9113_datapath_config_e {
	/* Sinc3, 32KHz Sampling */
	ADE9113_SINC3_32_KHZ_SAMPLING = 0,
	/* Sinc3, Low-Pass Filter (LPF) Enabled, 32kHz Sampling */
	ADE9113_SINC3_LPF_EN_32_KHZ_SAMPLING,
	/* Sinc3, Compensation Enabled, LPF Enabled, 32KHz Sampling */
	ADE9113_SINC3_COMP_EN_LPF_EN_32_KHZ_SAMPLING,
	/* Sinc3, LPF Enabled, 8KHz Sampling */
	ADE9113_SINC3_LPF_EN_8_KHZ_SAMPLING,
	/* Sinc3, Compensation Enabled, LPF Enabled, 8KHz Sampling */
	ADE9113_SINC3_COMP_EN_LPF_EN_8_KHZ_SAMPLING,
	/* Sinc3, LPF Enabled, 4kHz Sampling. */
	ADE9113_SINC3_LPF_EN_4_KHZ_SAMPLING,
	/* Sinc3, LPF Enabled, 2kHz Sampling. */
	ADE9113_SINC3_LPF_EN_2_KHZ_SAMPLING,
	/* Sinc3, LPF Enabled, 1kHz Sampling. */
	ADE9113_SINC3_LPF_EN_1_KHZ_SAMPLING
};

/**
 * @enum ade9113_emi_config_e
 * @brief ADE9113 EMI Frequency Hopping Selection.
 */
enum ade9113_emi_config_e {
	/* Sawtooth Frequency Rising */
	ADE9113_SAWTOOTH_FREQUENCY_RISING = 0,
	/* Sawtooth Frequency Falling */
	ADE9113_SAWTOOTH_FREQUENCY_FALLING,
	/* Ramp */
	ADE9113_RAMP,
	/* Random Hopping Frequency */
	ADE9113_RANDOM_HOPPING_FREQUENCY
};

/**
 * @enum ade9113_zx_edge_sel_e
 * @brief ADE9113 Zero Crossing Edge Select.
 */
enum ade9113_zx_edge_sel_e {
	/* ZX Pin Reflects the Sign of the Input Signal */
	ADE9113_ZX_INPUT_SIGNAL_SIGN = 0,
	/* Detect Zero Crossings with Positive Slope */
	ADE9113_ZX_DETECT_POSITIVE_SLOPE,
	/* Detect Zero Crossings with Negative Slope */
	ADE9113_ZX_DETECT_NEGATIVE_SLOPE,
	/* Detect Zero Crossings with Positive or Negative Slopes */
	ADE9113_ZX_DETECT_BOTH_SLOPES
};

/**
 * @enum ade9113_zx_channel_cfg_e
 * @brief ADE9113 Zero Crossing Channel Select.
 */
enum ade9113_zx_channel_cfg_e {
	/* Disable Zero Crossing Output */
	ADE9113_ZX_DISABLE = 0,
	/* Output Zero Crossing Function from the I Channel on ZX Pin */
	ADE9113_ZX_I_SEL,
	/* Output Zero Crossing Function from the V1 Channel on ZX Pin */
	ADE9113_ZX_V1_SEL,
	/* Output Zero Crossing Function from the V2 Channel on ZX Pin */
	ADE9113_ZX_V2_SEL
};

/**
 * @enum ade9113_operation_e
 * @brief ADE9113 long/short operation mode.
 */
enum ade9113_operation_e {
	/* Long read/write operations */
	ADE9113_L_OP = 0,
	/* Short read/write operations */
	ADE9113_S_OP
};

/**
 * @enum ade9113_wav_e
 * @brief ADE9113 waveorm data.
 */
enum ade9113_wav_e {
	/* I_WAV */
	ADE9113_I_WAV = 0,
	/* V1_WAV */
	ADE9113_V1_WAV,
	/* V1_WAV */
	ADE9113_V2_WAV
};


/**
 * @struct ade9113_init_param
 * @brief ADE9113 Device initialization parameters.
 */
struct ade9113_init_param {
	/* Device communication descriptor */
	struct no_os_spi_init_param 	*spi_init;
	/** GPIO RDY descriptor used to signal when ADC data is available */
	struct no_os_gpio_init_param	*gpio_rdy;
	/** GPIO RESET descriptor used to reset device (HW reset) */
	struct no_os_gpio_init_param  	*gpio_reset;
	/** IRQ device descriptor used to handle interrupt routine for GPIO RDY */
	struct no_os_irq_ctrl_desc 	*irq_ctrl;
	/** External callback used to handle interrupt routine for GPIO RDY */
	/** Set to NULL if callback defined in driver used */
	void (*drdy_callback)(void *context);
};

/**
 * @struct ade9113_dev
 * @brief ADE9113 Device structure.
 */
struct ade9113_dev {
	/* Device communication descriptor */
	struct no_os_spi_desc		*spi_desc;
	/* Version product */
	uint8_t 			ver_product;
	/* CRC setting */
	uint8_t				crc_en;
	/* I_WAV */
	int32_t				i_wav;
	/* V1_WAV */
	int32_t				v1_wav;
	/* V2_WAV */
	int32_t				v2_wav;
	/** GPIO RDY descriptor used to signal when ADC data is available */
	struct no_os_gpio_desc  	*gpio_rdy;
	/** GPIO RESET descriptor used to reset device (HW reset) */
	struct no_os_gpio_desc  	*gpio_reset;
	/** IRQ device descriptor used to handle interrupt routine for GPIO RDY */
	struct no_os_irq_ctrl_desc 	*irq_ctrl;
	/** IRQ callback used to handle interrupt routine for GPIO RDY */
	struct no_os_callback_desc	irq_cb;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Read device register. */
int ade9113_read(struct ade9113_dev *dev, uint8_t reg_addr,
		 uint8_t *reg_data, enum ade9113_operation_e op_mode);

/* Write device register. */
int ade9113_write(struct ade9113_dev *dev, uint8_t reg_addr,
		  uint8_t reg_data, enum ade9113_operation_e op_mode);

/* Initialize the device. */
int ade9113_init(struct ade9113_dev **device,
		 struct ade9113_init_param init_param);

/* Remove the device and release resources. */
int ade9113_remove(struct ade9113_dev *dev);

/* Reset the device using SW reset. */
int ade9113_sw_reset(struct ade9113_dev *dev);

/* Reset the device using HW reset. */
int ade9113_hw_reset(struct ade9113_dev *dev);

/* Convert a 24-bit raw sample to millivolts. */
int ade9113_convert_to_millivolts(struct ade9113_dev *dev,
				  enum ade9113_wav_e ch, int32_t *mv_val);

/* Get STREAM_DBG mode. */
int ade9113_get_stream_dbg_mode(struct ade9113_dev *dev,
				enum ade9113_stream_debug_e *stream_dbg);

/* Set STREAM_DBG mode. */
int ade9113_set_stream_dbg_mode(struct ade9113_dev *dev,
				enum ade9113_stream_debug_e stream_dbg);

/*  Get CRC enable on SPI write setting. */
int ade9113_get_crc_en_state(struct ade9113_dev *dev,
			     uint8_t *crc_en_state);

/* Set CRC enable on SPI write setting. */
int ade9113_set_crc_en_state(struct ade9113_dev *dev,
			     uint8_t crc_en_state);

/* Lock device. */
int ade9113_wr_lock(struct ade9113_dev *dev);

/* Unlock device. */
int ade9113_wr_unlock(struct ade9113_dev *dev);

/*  Write value in the scratchpad register. */
int ade9113_write_scratchpad(struct ade9113_dev *dev,
			     uint8_t val);

/* Get the value stired in the scratchpad register. */
int ade9113_read_scratchpad(struct ade9113_dev *dev,
			    uint8_t *val);

/* Set normal mode of operation. */
int ade9113_set_normal_mode(struct ade9113_dev *dev);

/* Set static mode of operation. */
int ade9113_set_static_mode(struct ade9113_dev *dev);

/* Set static mode of operation. */
int ade9113_set_data_increments_mode(struct ade9113_dev *dev);

/* Get ECC or PHY Error Count on ISO to NONISO Communications. */
int ade9113_get_err_count(struct ade9113_dev *dev,
			  uint8_t *err_count);

/* Invert V2 channel inputs. */
int ade9113_invert_v2_inputs(struct ade9113_dev *dev);

/* Invert V1 channel inputs. */
int ade9113_invert_v1_inputs(struct ade9113_dev *dev);

/* Invert I channel inputs. */
int ade9113_invert_i_inputs(struct ade9113_dev *dev);

/* Disable invert V2 channel inputs. */
int ade9113_invert_v2_inputs_disable(struct ade9113_dev *dev);

/* Disable invert V1 channel inputs. */
int ade9113_invert_v1_inputs_disable(struct ade9113_dev *dev);

/* Disable invert I channel inputs. */
int ade9113_invert_i_inputs_disable(struct ade9113_dev *dev);

/* Set filter bandwidth to 2.7 kHz at 8ksps output data rate. */
int ade9113_set_lpf_bw_2_7(struct ade9113_dev *dev);

/* Set filter bandwidth to 3.3 kHz at 8ksps output data rate. */
int ade9113_set_lpf_bw_3_3(struct ade9113_dev *dev);

/* Set digital signal processing configuration. */
int ade9113_set_dsp_config(struct ade9113_dev *dev,
			   enum ade9113_datapath_config_e config);

/* Enable write access to DC_OFFSET_MODE register. */
int ade9113_enable_wa_dc_offset_mode(struct ade9113_dev *dev);

/* Disable write access to DC_OFFSET_MODE register. */
int ade9113_disable_wa_dc_offset_mode(struct ade9113_dev *dev);

/* Get register map CRC. */
int ade9113_get_crc(struct ade9113_dev *dev, uint16_t *crc);

/* Refresh EFuse Memory. */
int ade9113_efuse_refresh(struct ade9113_dev *dev);

/* Select EMI frequency hopping. */
int ade9113_set_emi_config(struct ade9113_dev *dev,
			   enum ade9113_emi_config_e config);

/* Get EMI HI mask. */
int ade9113_get_emi_hi_mask(struct ade9113_dev *dev, uint8_t *msk);

/* Get EMI LO mask. */
int ade9113_get_emi_lo_mask(struct ade9113_dev *dev, uint8_t *msk);

/* Set EMI HI mask. */
int ade9113_set_emi_hi_mask(struct ade9113_dev *dev, uint8_t msk);

/* Set EMI LO mask. */
int ade9113_set_emi_lo_mask(struct ade9113_dev *dev, uint8_t msk);

/* Get EMI HI limit. */
int ade9113_get_emi_hi_limit(struct ade9113_dev *dev, uint8_t *limit);

/* Get EMI MID limit. */
int ade9113_get_emi_mid_limit(struct ade9113_dev *dev, uint8_t *limit);

/* Get EMI LO limit. */
int ade9113_get_emi_lo_limit(struct ade9113_dev *dev, uint8_t *limit);

/* Enable/Disable interrupt. */
int ade9113_control_interrupt(struct ade9113_dev *dev, uint8_t reg_addr,
			      uint8_t int_msk, uint8_t en);

/* Enable STATUS1X interrupt. */
int ade9113_enable_status1x_int(struct ade9113_dev *dev);

/* Disable STATUS1X interrupt. */
int ade9113_disable_status1x_int(struct ade9113_dev *dev);

/* Enable STATUS2X interrupt. */
int ade9113_enable_status2x_int(struct ade9113_dev *dev);

/* Disable STATUS2X interrupt. */
int ade9113_disable_status2x_int(struct ade9113_dev *dev);

/* Enable COM_UP interrupt. */
int ade9113_enable_com_up_int(struct ade9113_dev *dev);

/* Disable COM_UP interrupt. */
int ade9113_disable_com_up_int(struct ade9113_dev *dev);

/* Enable CRC_CHG interrupt. */
int ade9113_enable_crc_chg_int(struct ade9113_dev *dev);

/* Disable CRC_CHG interrupt. */
int ade9113_disable_crc_chg_int(struct ade9113_dev *dev);

/* Enable SPI_CRC_ERR interrupt. */
int ade9113_enable_spi_crc_err_int(struct ade9113_dev *dev);

/* Disable SPI_CRC_ERR interrupt. */
int ade9113_disable_spi_crc_err_int(struct ade9113_dev *dev);

/* Enable COMFLT_ERR interrupt. */
int ade9113_enable_comflt_err_int(struct ade9113_dev *dev);

/* Disable COMFLT_ERR interrupt. */
int ade9113_disable_comflt_err_int(struct ade9113_dev *dev);

/* Enable V2_WAV_OVRNG interrupt. */
int ade9113_enable_v2_wav_ovrng_int(struct ade9113_dev *dev);

/* Disable V2_WAV_OVRNG interrupt. */
int ade9113_disable_v2_wav_ovrng_int(struct ade9113_dev *dev);

/* Enable V1_WAV_OVRNG interrupt. */
int ade9113_enable_v1_wav_ovrng_int(struct ade9113_dev *dev);

/* Disable V1_WAV_OVRNG interrupt. */
int ade9113_disable_v1_wav_ovrng_int(struct ade9113_dev *dev);

/* Enable I_WAV_OVRNG interrupt. */
int ade9113_enable_i_wav_ovrng_int(struct ade9113_dev *dev);

/* Disable I_WAV_OVRNG interrupt. */
int ade9113_disable_i_wav_ovrng_int(struct ade9113_dev *dev);

/* Enable ADC_SYNC_DONE interrupt. */
int ade9113_enable_adc_sync_done_int(struct ade9113_dev *dev);

/* Disable ADC_SYNC_DONE interrupt. */
int ade9113_disable_adc_sync_done_int(struct ade9113_dev *dev);

/* Enable ISO_CLK_STBL_ERR interrupt. */
int ade9113_enable_iso_clk_stbl_err_int(struct ade9113_dev *dev);

/* Disable ISO_CLK_STBL_ERR interrupt. */
int ade9113_disable_iso_clk_stbl_err_int(struct ade9113_dev *dev);

/* Enable ISO_PHY_CRC_ERR interrupt. */
int ade9113_enable_iso_phy_crc_err_int(struct ade9113_dev *dev);

/* Disable ISO_PHY_CRC_ERR interrupt. */
int ade9113_disable_iso_phy_crc_err_int(struct ade9113_dev *dev);

/* Enable ISO_EFUSE_MEM_ERR interrupt. */
int ade9113_enable_iso_efuse_mem_err_int(struct ade9113_dev *dev);

/* Disable ISO_EFUSE_MEM_ERR interrupt. */
int ade9113_disable_iso_efuse_mem_err_int(struct ade9113_dev *dev);

/* Enable ISO_DIG_MOD_V2_OVF interrupt. */
int ade9113_enable_iso_dig_mod_v2_ovf_int(struct ade9113_dev *dev);

/* Disable ISO_DIG_MOD_V2_OVF interrupt. */
int ade9113_disable_iso_dig_mod_v2_ovf_int(struct ade9113_dev *dev);

/* Enable ISO_DIG_MOD_V1_OVF interrupt. */
int ade9113_enable_iso_dig_mod_v1_ovf_int(struct ade9113_dev *dev);

/* Disable ISO_DIG_MOD_V1_OVF interrupt. */
int ade9113_disable_iso_dig_mod_v1_ovf_int(struct ade9113_dev *dev);

/* Enable ISO_DIG_MOD_I_OVF interrupt. */
int ade9113_enable_iso_dig_mod_i_ovf_int(struct ade9113_dev *dev);

/* Disable ISO_DIG_MOD_I_OVF interrupt. */
int ade9113_disable_iso_dig_mod_i_ovf_int(struct ade9113_dev *dev);

/* Enable ISO_TEST_MMR_ERR interrupt. */
int ade9113_enable_iso_test_mmr_err_int(struct ade9113_dev *dev);

/* Disable ISO_TEST_MMR_ERR interrupt. */
int ade9113_disable_iso_test_mmr_err_int(struct ade9113_dev *dev);

/* Select zero crossing edge. */
int ade9113_select_zero_crossing_edge(struct ade9113_dev *dev,
				      enum ade9113_zx_edge_sel_e sel);

/* Select zero crossing channel. */
int ade9113_select_zero_crossing_channel(struct ade9113_dev *dev,
		enum ade9113_zx_channel_cfg_e cfg);

/* ADC prepare broadcast. */
int ade9113_adc_prepare_broadcast(struct ade9113_dev *dev);

/* ADC align. */
int ade9113_adc_align(struct ade9113_dev *dev);

/* ADC snapshot. */
int ade9113_adc_snapshot(struct ade9113_dev *dev);

/* Get interrupt indicator from STATUS register. */
int ade9113_get_int_status(struct ade9113_dev *dev, uint8_t addr, uint8_t msk,
			   uint8_t *status);

/* Get STATUSx register value. */
int ade9113_get_statusx_val(struct ade9113_dev *dev, uint8_t addr,
			    uint8_t *status);

/* Get STATUS1X Indicator. */
int ade9113_get_status1x(struct ade9113_dev *dev, uint8_t *status);

/* Get STATUS2 indicator. */
int ade9113_get_status2x(struct ade9113_dev *dev, uint8_t *status);

/* Get RESET_DONE indicator. */
int ade9113_get_reset_done(struct ade9113_dev *dev, uint8_t *status);

/* Get COM_UP indicator. */
int ade9113_get_com_up(struct ade9113_dev *dev, uint8_t *status);

/* Get CRC_CHG indicator. */
int ade9113_get_crc_chg(struct ade9113_dev *dev, uint8_t *status);

/* Get EFUSE_MEM_ERR indicator. */
int ade9113_get_efuse_mem_err(struct ade9113_dev *dev, uint8_t *status);

/* Get SPI_CRC_ERR indicator. */
int ade9113_get_spi_crc_err(struct ade9113_dev *dev, uint8_t *status);

/* Get COMFLT_ERR indicator. */
int ade9113_get_comflt_err(struct ade9113_dev *dev, uint8_t *status);

/* Clear the RESET_DONE int mask. */
int ade9113_clear_reset_done_int(struct ade9113_dev *dev);

/* Clear the COM_UP int mask. */
int ade9113_clear_com_up_int(struct ade9113_dev *dev);

/* Clear the CRC_CHG int mask. */
int ade9113_clear_crc_chg_int(struct ade9113_dev *dev);

/* Clear the SPI_CRC_ERR int mask. */
int ade9113_clear_spi_crc_err_int(struct ade9113_dev *dev);

/* Clear the COMFLT_ERR int mask. */
int ade9113_clear_comflt_err_int(struct ade9113_dev *dev);

/* Get V2_WAV_OVRNG indicator. */
int ade9113_get_v2_wav_ovrng(struct ade9113_dev *dev, uint8_t *status);

/* Clear the V2_WAV_OVRNG int mask. */
int ade9113_clear_v2_wav_ovrng_int(struct ade9113_dev *dev);

/* Get V1_WAV_OVRNG indicator. */
int ade9113_get_v1_wav_ovrng(struct ade9113_dev *dev, uint8_t *status);

/* Clear the V1_WAV_OVRNG int mask. */
int ade9113_clear_v1_wav_ovrng_int(struct ade9113_dev *dev);

/* Get I_WAV_OVRNG indicator. */
int ade9113_get_i_wav_ovrng(struct ade9113_dev *dev, uint8_t *status);

/* Clear the I_WAV_OVRNG int mask. */
int ade9113_clear_i_wav_ovrng_int(struct ade9113_dev *dev);

/* Get ADC_SYNC_DONE indicator. */
int ade9113_get_adc_sync_done(struct ade9113_dev *dev, uint8_t *status);

/* Clear the ADC_SYNC_DONE int mask. */
int ade9113_clear_adc_sync_done_int(struct ade9113_dev *dev);

/* Get ISO_CLK_STBL_ERR indicator. */
int ade9113_get_iso_clk_stbl_err(struct ade9113_dev *dev, uint8_t *status);

/* Clear the ISO_CLK_STBL_ERR int mask. */
int ade9113_clear_iso_clk_stbl_err_int(struct ade9113_dev *dev);

/* Get ISO_PHY_CRC_ERR indicator. */
int ade9113_get_iso_phy_crc_err(struct ade9113_dev *dev, uint8_t *status);

/* Clear the ISO_PHY_CRC_ERR int mask. */
int ade9113_clear_iso_phy_crc_err_int(struct ade9113_dev *dev);

/* Get ISO_EFUSE_MEM_ERR indicator. */
int ade9113_get_iso_efuse_mem_err_err(struct ade9113_dev *dev, uint8_t *status);

/* Clear the ISO_EFUSE_MEM_ERR int mask. */
int ade9113_clear_iso_efuse_mem_err_int(struct ade9113_dev *dev);

/* Get ISO_DIG_MOD_V2_OVF indicator. */
int ade9113_get_iso_dig_mod_v2_ovf(struct ade9113_dev *dev, uint8_t *status);

/* Clear the ISO_DIG_MOD_V2_OVF int mask. */
int ade9113_clear_iso_dig_mod_v2_ovf_int(struct ade9113_dev *dev);

/* Get ISO_DIG_MOD_V1_OVF indicator. */
int ade9113_get_iso_dig_mod_v1_ovf(struct ade9113_dev *dev, uint8_t *status);

/* Clear the ISO_DIG_MOD_V1_OVF int mask. */
int ade9113_clear_iso_dig_mod_v1_ovf_int(struct ade9113_dev *dev);

/* Get ISO_DIG_MOD_I_OVF indicator. */
int ade9113_get_iso_dig_mod_i_ovf(struct ade9113_dev *dev, uint8_t *status);

/* Clear the ISO_DIG_MOD_I_OVF int mask. */
int ade9113_clear_iso_dig_mod_i_ovf_int(struct ade9113_dev *dev);

/* Get ISO_TEST_MMR_ERR indicator. */
int ade9113_get_iso_test_mmr_err(struct ade9113_dev *dev, uint8_t *status);

/* Clear the ISO_TEST_MMR_ERR int mask. */
int ade9113_clear_iso_test_mmr_err_int(struct ade9113_dev *dev);

/* Get ISO_STATUS_RD_ECC_ERR indicator. */
int ade9113_get_iso_status_rd_ecc_err(struct ade9113_dev *dev, uint8_t *status);

/* Get ISO_PHY_ERR indicator. */
int ade9113_get_iso_phy_err(struct ade9113_dev *dev, uint8_t *status);

/* Get ISO_ECC_ERR indicator. */
int ade9113_get_iso_ecc_err(struct ade9113_dev *dev, uint8_t *status);

/* Get CRC_DONE indicator. */
int ade9113_get_crc_done_flag(struct ade9113_dev *dev, uint8_t *status);

/* Clear the CRC_DONE int mask. */
int ade9113_clear_crc_done_int(struct ade9113_dev *dev);

/* Force background register map CRC recalculation. */
int ade9113_force_crc_recalculation(struct ade9113_dev *dev);

/* Get SILICON_REVISION value. */
int ade9113_get_silicon_revision(struct ade9113_dev *dev, uint8_t *silicon_rev);

/* Get VERSION_PRODUCT value. */
int ade9113_get_version_product(struct ade9113_dev *dev, uint8_t *ver_product);

/* Get wave value. */
int ade9113_get_wav(struct ade9113_dev *dev, enum ade9113_wav_e selection,
		    uint32_t *val);

/* DRDY inerrupt enable. */
int ade9113_drdy_int_enable(struct ade9113_dev *dev);

/* DRDY inerrupt disable. */
int ade9113_drdy_int_disable(struct ade9113_dev *dev);

#endif // __ADE9113_H__
