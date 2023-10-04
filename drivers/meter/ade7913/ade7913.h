/***************************************************************************//**
 *   @file   ade7913.h
 *   @brief  Header file of ADE7913 Driver.
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
#ifndef __ADE7913_H__
#define __ADE7913_H__

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
#define ADE7913_SPI_READ			NO_OS_BIT(7)

/* Long/Short operation bit, set for lon read/write */
#define ADE7913_OP_MODE_LONG			NO_OS_BIT(6)

/* ADE7913 CRC constants */
#define ADE7913_CRC8_POLY			0x07
#define ADE7913_CRC16_POLY			0x1021
#define ADE7913_CRC16_INIT_VAL			0xFFFF

/* ENABLE and DISABLE */
#define ENABLE					1u
#define DISABLE					0u

/* ADE7913 Register Map */
#define ADE7913_REG_SWRST			0x001
#define ADE7913_REG_CONFIG0			0x002
#define ADE7913_REG_CONFIG_FILT			0x003
#define ADE7913_REG_CONFIG_ISO_ACC		0x005
#define ADE7913_REG_CRC_RESULT_HI		0x006
#define ADE7913_REG_CRC_RESULT_LO		0x007
#define ADE7913_REG_EFUSE_REFRESH		0x008
#define ADE7913_REG_EMI_CONFIG			0x009
#define ADE7913_REG_EMI_HI_MASK			0x00A
#define ADE7913_REG_EMI_LO_MASK			0x00B
#define ADE7913_REG_EMI_HI_LIMIT		0x00C
#define ADE7913_REG_EMI_MID_LIMIT		0x00D
#define ADE7913_REG_EMI_LO_LIMIT		0x00E
#define ADE7913_REG_MASK0			0x00F
#define ADE7913_REG_MASK1			0x010
#define ADE7913_REG_MASK2			0x011
#define ADE7913_REG_CONFIG_ZX			0x012
#define ADE7913_REG_SCRATCH			0x013
#define ADE7913_REG_SYNC_SNAP			0x014
#define ADE7913_REG_COUNTER_HI			0x015
#define ADE7913_REG_COUNTER_LO			0x016
#define ADE7913_REG_SNAPSHOT_COUNT_HI		0x017
#define ADE7913_REG_SNAPSHOT_COUNT_LO		0x018
#define ADE7913_REG_WR_LOCK			0x01F
#define ADE7913_REG_STATUS0			0x020
#define ADE7913_REG_STATUS1			0x021
#define ADE7913_REG_STATUS2			0x022
#define ADE7913_REG_COM_FLT_TYPE		0x023
#define ADE7913_REG_COM_FLT_COUNT		0x024
#define ADE7913_REG_CONFIG_CRC			0x025
#define ADE7913_REG_I_WAV_HI			0x026
#define ADE7913_REG_I_WAV_MD			0x027
#define ADE7913_REG_I_WAV_LO 			0x028
#define ADE7913_REG_V1_WAV_HI			0x029
#define ADE7913_REG_V1_WAV_MD			0x02A
#define ADE7913_REG_V1_WAV_LO			0x02B
#define ADE7913_REG_V2_WAV_HI			0x02C
#define ADE7913_REG_V2_WAV_MD			0x02D
#define ADE7913_REG_V2_WAV_LO			0x02E
#define ADE7913_REG_UNIQUE_PART_ID_5		0x075
#define ADE7913_REG_UNIQUE_PART_ID_4		0x076
#define ADE7913_REG_UNIQUE_PART_ID_3		0x077
#define ADE7913_REG_UNIQUE_PART_ID_2		0x078
#define ADE7913_REG_UNIQUE_PART_ID_1		0x079
#define ADE7913_REG_UNIQUE_PART_ID_0		0x07A
#define ADE7913_REG_SILICON_REVISION		0x07D
#define ADE7913_REG_VERSION_PRODUCT		0x07E

/* ADE7913 SWRST command */
#define ADE7913_SWRST_CMD			0xD6

/* ADE7913_REG_CONFIG0 Bit Definition */
#define ADE7913_STREAM_DBG_MSK			NO_OS_GENMASK(3, 2)
#define ADE7913_CRC_EN_SPI_WRITE_MSK		NO_OS_BIT(1)
#define ADE7913_CLOUT_EN_MSK			NO_OS_BIT(0)

/* ADE7913_REG_CONFIG_FILT Bit Definition */
#define ADE7913_V2_ADC_INVERT_MSK		NO_OS_BIT(6)
#define ADE7913_V1_ADC_INVERT_MSK		NO_OS_BIT(5)
#define ADE7913_I_ADC_INVERT_MSK		NO_OS_BIT(4)
#define ADE7913_LPF_BW_MSK			NO_OS_BIT(3)
#define ADE7913_DATAPATH_CONFIG_MSK		NO_OS_GENMASK(2, 0)

/* ADE7913_REG_CONFIG_ISO_ACC Bit Definition */
#define ADE7913_ISO_WR_ACC_EN_MSK		NO_OS_BIT(0)

/* ADE7913_REG_EFUSE_REFRESH Bit Definition */
#define ADE7913_EFUSE_REFRESH_MSK		NO_OS_BIT(0)

/* ADE7913_REG_EMI_CONFIG Bit Definition */
#define ADE7913_EMI_CONFIG_MSK			NO_OS_GENMASK(2, 0)

/* ADE7913_REG_MASK0/ADE7913_REG_STATUS0 Bit Definition */
#define ADE7913_STATUS1X_MSK	 		NO_OS_BIT(7)
#define ADE7913_STATUS2X_MSK 			NO_OS_BIT(6)
#define ADE7913_COM_UP_MSK	 		NO_OS_BIT(4)
#define ADE7913_CRC_CHG_MSK 			NO_OS_BIT(3)
#define ADE7913_SPI_CRC_ERR_MSK 		NO_OS_BIT(1)
#define ADE7913_COMFLT_ERR_MSK			NO_OS_BIT(0)

/* ADE7913_REG_MASK1/ADE7913_REG_STATUS1 Bit Definition */
#define ADE7913_V2_WAV_OVRNG_MSK		NO_OS_BIT(3)
#define ADE7913_V1_WAV_OVRNG_MSK		NO_OS_BIT(2)
#define ADE7913_I_WAV_OVRNG_MSK			NO_OS_BIT(1)
#define ADE7913_ADC_SYNC_DONE_MSK		NO_OS_BIT(0)

/* ADE7913_REG_MASK2/ADE7913_REG_STATUS2 Bit Definition */
#define ADE7913_ISO_CLK_STBL_ERR_MSK		NO_OS_BIT(6)
#define ADE7913_ISO_PHY_CRC_ERR_MSK		NO_OS_BIT(5)
#define ADE7913_ISO_EFUSE_MEM_ERR_MSK		NO_OS_BIT(4)
#define ADE7913_ISO_DIG_MOD_V2_OVF_MSK		NO_OS_BIT(3)
#define ADE7913_ISO_DIG_MOD_V1_OVF_MSK		NO_OS_BIT(2)
#define ADE7913_ISO_DIG_MOD_I_OVF_MSK		NO_OS_BIT(1)
#define ADE7913_ISO_TEST_MMR_ERR_MSK		NO_OS_BIT(0)

/* ADE7913_REG_CONFIG_ZX Bit Definition */
#define ADE7913_ZX_EDGE_SEL_MSK			NO_OS_GENMASK(3, 2)
#define ADE7913_ZX_CHANNEL_CONFIG_MSK		NO_OS_GENMASK(1, 0)

/* ADE7913_REG_SYNC_SNAP Bit Definition */
#define ADE7913_PREP_BROADCAST_MSK		NO_OS_BIT(2)
#define ADE7913_ALIGN_MSK			NO_OS_BIT(1)
#define ADE7913_SNAPSHOT_MSK			NO_OS_BIT(0)

/* ADE7913_REG_COUNTER_HI Bit Definition */
#define ADE7913_COUNTER_HI_MSK			NO_OS_GENMASK(5, 0)

/* ADE7913_REG_COUNTER_LO Bit Definition */
#define ADE7913_COUNTER_LO_MSK			NO_OS_GENMASK(8, 0)

/* ADE7913_REG_SNAPSHOT_COUNT_HI Bit Definition */
#define ADE7913_SNAPSHOT_COUNT_HI_MSK		NO_OS_GENMASK(5, 0)

/* ADE7913_REG_SNAPSHOT_COUNT_LO Bit Definition */
#define ADE7913_SNAPSHOT_COUNT_LO_MSK		NO_OS_GENMASK(7, 0)

/* ADE7913_REG_STATUS0 Bit Definition */
#define ADE7913_RESET_DONE_MSK			NO_OS_BIT(5)
#define ADE7913_EFUSE_MEM_ERR_MSK		NO_OS_BIT(2)

/* ADE7913_REG_COM_FLT_TYPE Bit Definition */
#define ADE7913_ISO_STATUS_RD_ECC_ERR_MSK	NO_OS_BIT(2)
#define ADE7913_ISO_PHY_ERR_MSK			NO_OS_BIT(1)
#define ADE7913_ISO_ECC_ERR_MSK			NO_OS_BIT(1)

/* ADE7913_REG_CONFIG_CRC Bit Definition */
#define ADE7913_CRC_DONE_MSK			NO_OS_BIT(1)
#define ADE7913_CRC_FORCE_MSK			NO_OS_BIT(0)

/* ADE7913_REG_SILICON_REVISION Bit Definition */
#define ADE7913_NONISO_CHIP_REV_MSK		NO_OS_GENMASK(7, 4)
#define ADE7913_ISO_CHIP_REV_MSK		NO_OS_GENMASK(3, 0)

/* Configuration Register Write Lock */
#define ADE7913_LOCK_KEY			0XD4
#define ADE7913_UNLOCK_KEY			0X5E

/* Version Product */
#define ADE7913_3_CHANNEL_ADE7913		0U
#define ADE7913_2_CHANNEL_ADE9112		1U
#define ADE7913_NONISOLATED_ADE9103		3U

/* Revision Value of ISO and NONISO Silicon */
#define ADE7913_SILICON_REVISION		0x12

/* Nominal reference voltage */
#define ADE7913_VREF				(883883)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum ade7913_stream_debug_e
 * @brief ADE7913 Stream Debug.
 */
enum ade7913_stream_debug_e {
	/* Normal Mode */
	ADE7913_STREAM_NORMAL_MODE = 0,
	/* Static Mode */
	ADE7913_STREAM_STATIC_MODE,
	/* Data Increments at ADC Conversion Rate */
	ADE7913_STREAM_INCREMENTS_MODE,
	/* Same as functional mode */
	ADE7913_STREAM_FUNCTIONAL_MODE
};

/**
 * @enum ade7913_datapath_config_e
 * @brief ADE7913 digital signal processing configuration.
 */
enum ade7913_datapath_config_e {
	/* Sinc3, 32KHz Sampling */
	ADE7913_SINC3_32_KHZ_SAMPLING = 0,
	/* Sinc3, Low-Pass Filter (LPF) Enabled, 32kHz Sampling */
	ADE7913_SINC3_LPF_EN_32_KHZ_SAMPLING,
	/* Sinc3, Compensation Enabled, LPF Enabled, 32KHz Sampling */
	ADE7913_SINC3_COMP_EN_LPF_EN_32_KHZ_SAMPLING,
	/* Sinc3, LPF Enabled, 8KHz Sampling */
	ADE7913_SINC3_LPF_EN_8_KHZ_SAMPLING,
	/* Sinc3, Compensation Enabled, LPF Enabled, 8KHz Sampling */
	ADE7913_SINC3_COMP_EN_LPF_EN_8_KHZ_SAMPLING,
	/* Sinc3, LPF Enabled, 4kHz Sampling. */
	ADE7913_SINC3_LPF_EN_4_KHZ_SAMPLING,
	/* Sinc3, LPF Enabled, 2kHz Sampling. */
	ADE7913_SINC3_LPF_EN_2_KHZ_SAMPLING,
	/* Sinc3, LPF Enabled, 1kHz Sampling. */
	ADE7913_SINC3_LPF_EN_1_KHZ_SAMPLING
};

/**
 * @enum ade7913_emi_config_e
 * @brief ADE7913 EMI Frequency Hopping Selection.
 */
enum ade7913_emi_config_e {
	/* Sawtooth Frequency Rising */
	ADE7913_SAWTOOTH_FREQUENCY_RISING = 0,
	/* Sawtooth Frequency Falling */
	ADE7913_SAWTOOTH_FREQUENCY_FALLING,
	/* Ramp */
	ADE7913_RAMP,
	/* Random Hopping Frequency */
	ADE7913_RANDOM_HOPPING_FREQUENCY
};

/**
 * @enum ade7913_zx_edge_sel_e
 * @brief ADE7913 Zero Crossing Edge Select.
 */
enum ade7913_zx_edge_sel_e {
	/* ZX Pin Reflects the Sign of the Input Signal */
	ADE7913_ZX_INPUT_SIGNAL_SIGN = 0,
	/* Detect Zero Crossings with Positive Slope */
	ADE7913_ZX_DETECT_POSITIVE_SLOPE,
	/* Detect Zero Crossings with Negative Slope */
	ADE7913_ZX_DETECT_NEGATIVE_SLOPE,
	/* Detect Zero Crossings with Positive or Negative Slopes */
	ADE7913_ZX_DETECT_BOTH_SLOPES
};

/**
 * @enum ade7913_zx_channel_cfg_e
 * @brief ADE7913 Zero Crossing Channel Select.
 */
enum ade7913_zx_channel_cfg_e {
	/* Disable Zero Crossing Output */
	ADE7913_ZX_DISABLE = 0,
	/* Output Zero Crossing Function from the I Channel on ZX Pin */
	ADE7913_ZX_I_SEL,
	/* Output Zero Crossing Function from the V1 Channel on ZX Pin */
	ADE7913_ZX_V1_SEL,
	/* Output Zero Crossing Function from the V2 Channel on ZX Pin */
	ADE7913_ZX_V2_SEL
};

/**
 * @enum ade7913_operation_e
 * @brief ADE7913 long/short operation mode.
 */
enum ade7913_operation_e {
	/* Long read/write operations */
	ADE7913_L_OP = 0,
	/* Short read/write operations */
	ADE7913_S_OP
};

/**
 * @enum ade7913_wav_e
 * @brief ADE7913 waveorm data.
 */
enum ade7913_wav_e {
	/* I_WAV */
	ADE7913_I_WAV = 0,
	/* V1_WAV */
	ADE7913_V1_WAV,
	/* V1_WAV */
	ADE7913_V2_WAV
};


/**
 * @struct ade7913_init_param
 * @brief ADE7913 Device initialization parameters.
 */
struct ade7913_init_param {
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
 * @struct ade7913_dev
 * @brief ADE7913 Device structure.
 */
struct ade7913_dev {
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
int ade7913_read(struct ade7913_dev *dev, uint8_t reg_addr,
		 uint8_t *reg_data, enum ade7913_operation_e op_mode);

/* Write device register. */
int ade7913_write(struct ade7913_dev *dev, uint8_t reg_addr,
		  uint8_t reg_data, enum ade7913_operation_e op_mode);

/* Initialize the device. */
int ade7913_init(struct ade7913_dev **device,
		 struct ade7913_init_param init_param);

/* Remove the device and release resources. */
int ade7913_remove(struct ade7913_dev *dev);

/* Reset the device using SW reset. */
int ade7913_sw_reset(struct ade7913_dev *dev);

/* Reset the device using HW reset. */
int ade7913_hw_reset(struct ade7913_dev *dev);

/* Convert a 24-bit raw sample to millivolts. */
int ade7913_convert_to_millivolts(struct ade7913_dev *dev,
				  enum ade7913_wav_e ch, int32_t *mv_val);

/* Get STREAM_DBG mode. */
int ade7913_get_stream_dbg_mode(struct ade7913_dev *dev,
				enum ade7913_stream_debug_e *stream_dbg);

/* Set STREAM_DBG mode. */
int ade7913_set_stream_dbg_mode(struct ade7913_dev *dev,
				enum ade7913_stream_debug_e stream_dbg);

/*  Get CRC enable on SPI write setting. */
int ade7913_get_crc_en_state(struct ade7913_dev *dev,
			     uint8_t *crc_en_state);

/* Set CRC enable on SPI write setting. */
int ade7913_set_crc_en_state(struct ade7913_dev *dev,
			     uint8_t crc_en_state);

/* Lock device. */
int ade7913_wr_lock(struct ade7913_dev *dev);

/* Unlock device. */
int ade7913_wr_unlock(struct ade7913_dev *dev);

/*  Write value in the scratchpad register. */
int ade7913_write_scratchpad(struct ade7913_dev *dev,
			     uint8_t val);

/* Get the value stired in the scratchpad register. */
int ade7913_read_scratchpad(struct ade7913_dev *dev,
			    uint8_t *val);

/* Set normal mode of operation. */
int ade7913_set_normal_mode(struct ade7913_dev *dev);

/* Set static mode of operation. */
int ade7913_set_static_mode(struct ade7913_dev *dev);

/* Set static mode of operation. */
int ade7913_set_data_increments_mode(struct ade7913_dev *dev);

/* Get ECC or PHY Error Count on ISO to NONISO Communications. */
int ade7913_get_err_count(struct ade7913_dev *dev,
			  uint8_t *err_count);

/* Invert V2 channel inputs. */
int ade7913_invert_v2_inputs(struct ade7913_dev *dev);

/* Invert V1 channel inputs. */
int ade7913_invert_v1_inputs(struct ade7913_dev *dev);

/* Invert I channel inputs. */
int ade7913_invert_i_inputs(struct ade7913_dev *dev);

/* Disable invert V2 channel inputs. */
int ade7913_invert_v2_inputs_disable(struct ade7913_dev *dev);

/* Disable invert V1 channel inputs. */
int ade7913_invert_v1_inputs_disable(struct ade7913_dev *dev);

/* Disable invert I channel inputs. */
int ade7913_invert_i_inputs_disable(struct ade7913_dev *dev);

/* Set filter bandwidth to 2.7 kHz at 8ksps output data rate. */
int ade7913_set_lpf_bw_2_7(struct ade7913_dev *dev);

/* Set filter bandwidth to 3.3 kHz at 8ksps output data rate. */
int ade7913_set_lpf_bw_3_3(struct ade7913_dev *dev);

/* Set digital signal processing configuration. */
int ade7913_set_dsp_config(struct ade7913_dev *dev,
			   enum ade7913_datapath_config_e config);

/* Enable write access to DC_OFFSET_MODE register. */
int ade7913_enable_wa_dc_offset_mode(struct ade7913_dev *dev);

/* Disable write access to DC_OFFSET_MODE register. */
int ade7913_disable_wa_dc_offset_mode(struct ade7913_dev *dev);

/* Get register map CRC. */
int ade7913_get_crc(struct ade7913_dev *dev, uint16_t *crc);

/* Refresh EFuse Memory. */
int ade7913_efuse_refresh(struct ade7913_dev *dev);

/* Select EMI frequency hopping. */
int ade7913_set_emi_config(struct ade7913_dev *dev,
			   enum ade7913_emi_config_e config);

/* Get EMI HI mask. */
int ade7913_get_emi_hi_mask(struct ade7913_dev *dev, uint8_t *msk);

/* Get EMI LO mask. */
int ade7913_get_emi_lo_mask(struct ade7913_dev *dev, uint8_t *msk);

/* Set EMI HI mask. */
int ade7913_set_emi_hi_mask(struct ade7913_dev *dev, uint8_t msk);

/* Set EMI LO mask. */
int ade7913_set_emi_lo_mask(struct ade7913_dev *dev, uint8_t msk);

/* Get EMI HI limit. */
int ade7913_get_emi_hi_limit(struct ade7913_dev *dev, uint8_t *limit);

/* Get EMI MID limit. */
int ade7913_get_emi_mid_limit(struct ade7913_dev *dev, uint8_t *limit);

/* Get EMI LO limit. */
int ade7913_get_emi_lo_limit(struct ade7913_dev *dev, uint8_t *limit);

/* Enable/Disable interrupt. */
int ade7913_control_interrupt(struct ade7913_dev *dev, uint8_t reg_addr,
			      uint8_t int_msk, uint8_t en);

/* Enable STATUS1X interrupt. */
int ade7913_enable_status1x_int(struct ade7913_dev *dev);

/* Disable STATUS1X interrupt. */
int ade7913_disable_status1x_int(struct ade7913_dev *dev);

/* Enable STATUS2X interrupt. */
int ade7913_enable_status2x_int(struct ade7913_dev *dev);

/* Disable STATUS2X interrupt. */
int ade7913_disable_status2x_int(struct ade7913_dev *dev);

/* Enable COM_UP interrupt. */
int ade7913_enable_com_up_int(struct ade7913_dev *dev);

/* Disable COM_UP interrupt. */
int ade7913_disable_com_up_int(struct ade7913_dev *dev);

/* Enable CRC_CHG interrupt. */
int ade7913_enable_crc_chg_int(struct ade7913_dev *dev);

/* Disable CRC_CHG interrupt. */
int ade7913_disable_crc_chg_int(struct ade7913_dev *dev);

/* Enable SPI_CRC_ERR interrupt. */
int ade7913_enable_spi_crc_err_int(struct ade7913_dev *dev);

/* Disable SPI_CRC_ERR interrupt. */
int ade7913_disable_spi_crc_err_int(struct ade7913_dev *dev);

/* Enable COMFLT_ERR interrupt. */
int ade7913_enable_comflt_err_int(struct ade7913_dev *dev);

/* Disable COMFLT_ERR interrupt. */
int ade7913_disable_comflt_err_int(struct ade7913_dev *dev);

/* Enable V2_WAV_OVRNG interrupt. */
int ade7913_enable_v2_wav_ovrng_int(struct ade7913_dev *dev);

/* Disable V2_WAV_OVRNG interrupt. */
int ade7913_disable_v2_wav_ovrng_int(struct ade7913_dev *dev);

/* Enable V1_WAV_OVRNG interrupt. */
int ade7913_enable_v1_wav_ovrng_int(struct ade7913_dev *dev);

/* Disable V1_WAV_OVRNG interrupt. */
int ade7913_disable_v1_wav_ovrng_int(struct ade7913_dev *dev);

/* Enable I_WAV_OVRNG interrupt. */
int ade7913_enable_i_wav_ovrng_int(struct ade7913_dev *dev);

/* Disable I_WAV_OVRNG interrupt. */
int ade7913_disable_i_wav_ovrng_int(struct ade7913_dev *dev);

/* Enable ADC_SYNC_DONE interrupt. */
int ade7913_enable_adc_sync_done_int(struct ade7913_dev *dev);

/* Disable ADC_SYNC_DONE interrupt. */
int ade7913_disable_adc_sync_done_int(struct ade7913_dev *dev);

/* Enable ISO_CLK_STBL_ERR interrupt. */
int ade7913_enable_iso_clk_stbl_err_int(struct ade7913_dev *dev);

/* Disable ISO_CLK_STBL_ERR interrupt. */
int ade7913_disable_iso_clk_stbl_err_int(struct ade7913_dev *dev);

/* Enable ISO_PHY_CRC_ERR interrupt. */
int ade7913_enable_iso_phy_crc_err_int(struct ade7913_dev *dev);

/* Disable ISO_PHY_CRC_ERR interrupt. */
int ade7913_disable_iso_phy_crc_err_int(struct ade7913_dev *dev);

/* Enable ISO_EFUSE_MEM_ERR interrupt. */
int ade7913_enable_iso_efuse_mem_err_int(struct ade7913_dev *dev);

/* Disable ISO_EFUSE_MEM_ERR interrupt. */
int ade7913_disable_iso_efuse_mem_err_int(struct ade7913_dev *dev);

/* Enable ISO_DIG_MOD_V2_OVF interrupt. */
int ade7913_enable_iso_dig_mod_v2_ovf_int(struct ade7913_dev *dev);

/* Disable ISO_DIG_MOD_V2_OVF interrupt. */
int ade7913_disable_iso_dig_mod_v2_ovf_int(struct ade7913_dev *dev);

/* Enable ISO_DIG_MOD_V1_OVF interrupt. */
int ade7913_enable_iso_dig_mod_v1_ovf_int(struct ade7913_dev *dev);

/* Disable ISO_DIG_MOD_V1_OVF interrupt. */
int ade7913_disable_iso_dig_mod_v1_ovf_int(struct ade7913_dev *dev);

/* Enable ISO_DIG_MOD_I_OVF interrupt. */
int ade7913_enable_iso_dig_mod_i_ovf_int(struct ade7913_dev *dev);

/* Disable ISO_DIG_MOD_I_OVF interrupt. */
int ade7913_disable_iso_dig_mod_i_ovf_int(struct ade7913_dev *dev);

/* Enable ISO_TEST_MMR_ERR interrupt. */
int ade7913_enable_iso_test_mmr_err_int(struct ade7913_dev *dev);

/* Disable ISO_TEST_MMR_ERR interrupt. */
int ade7913_disable_iso_test_mmr_err_int(struct ade7913_dev *dev);

/* Select zero crossing edge. */
int ade7913_select_zero_crossing_edge(struct ade7913_dev *dev,
				      enum ade7913_zx_edge_sel_e sel);

/* Select zero crossing channel. */
int ade7913_select_zero_crossing_channel(struct ade7913_dev *dev,
		enum ade7913_zx_channel_cfg_e cfg);

/* ADC prepare broadcast. */
int ade7913_adc_prepare_broadcast(struct ade7913_dev *dev);

/* ADC align. */
int ade7913_adc_align(struct ade7913_dev *dev);

/* ADC snapshot. */
int ade7913_adc_snapshot(struct ade7913_dev *dev);

/* Get interrupt indicator from STATUS register. */
int ade7913_get_int_status(struct ade7913_dev *dev, uint8_t addr, uint8_t msk,
			   uint8_t *status);

/* Get STATUSx register value. */
int ade7913_get_statusx_val(struct ade7913_dev *dev, uint8_t addr,
			    uint8_t *status);

/* Get STATUS1X Indicator. */
int ade7913_get_status1x(struct ade7913_dev *dev, uint8_t *status);

/* Get STATUS2 indicator. */
int ade7913_get_status2x(struct ade7913_dev *dev, uint8_t *status);

/* Get RESET_DONE indicator. */
int ade7913_get_reset_done(struct ade7913_dev *dev, uint8_t *status);

/* Get COM_UP indicator. */
int ade7913_get_com_up(struct ade7913_dev *dev, uint8_t *status);

/* Get CRC_CHG indicator. */
int ade7913_get_crc_chg(struct ade7913_dev *dev, uint8_t *status);

/* Get EFUSE_MEM_ERR indicator. */
int ade7913_get_efuse_mem_err(struct ade7913_dev *dev, uint8_t *status);

/* Get SPI_CRC_ERR indicator. */
int ade7913_get_spi_crc_err(struct ade7913_dev *dev, uint8_t *status);

/* Get COMFLT_ERR indicator. */
int ade7913_get_comflt_err(struct ade7913_dev *dev, uint8_t *status);

/* Clear the RESET_DONE int mask. */
int ade7913_clear_reset_done_int(struct ade7913_dev *dev);

/* Clear the COM_UP int mask. */
int ade7913_clear_com_up_int(struct ade7913_dev *dev);

/* Clear the CRC_CHG int mask. */
int ade7913_clear_crc_chg_int(struct ade7913_dev *dev);

/* Clear the SPI_CRC_ERR int mask. */
int ade7913_clear_spi_crc_err_int(struct ade7913_dev *dev);

/* Clear the COMFLT_ERR int mask. */
int ade7913_clear_comflt_err_int(struct ade7913_dev *dev);

/* Get V2_WAV_OVRNG indicator. */
int ade7913_get_v2_wav_ovrng(struct ade7913_dev *dev, uint8_t *status);

/* Clear the V2_WAV_OVRNG int mask. */
int ade7913_clear_v2_wav_ovrng_int(struct ade7913_dev *dev);

/* Get V1_WAV_OVRNG indicator. */
int ade7913_get_v1_wav_ovrng(struct ade7913_dev *dev, uint8_t *status);

/* Clear the V1_WAV_OVRNG int mask. */
int ade7913_clear_v1_wav_ovrng_int(struct ade7913_dev *dev);

/* Get I_WAV_OVRNG indicator. */
int ade7913_get_i_wav_ovrng(struct ade7913_dev *dev, uint8_t *status);

/* Clear the I_WAV_OVRNG int mask. */
int ade7913_clear_i_wav_ovrng_int(struct ade7913_dev *dev);

/* Get ADC_SYNC_DONE indicator. */
int ade7913_get_adc_sync_done(struct ade7913_dev *dev, uint8_t *status);

/* Clear the ADC_SYNC_DONE int mask. */
int ade7913_clear_adc_sync_done_int(struct ade7913_dev *dev);

/* Get ISO_CLK_STBL_ERR indicator. */
int ade7913_get_iso_clk_stbl_err(struct ade7913_dev *dev, uint8_t *status);

/* Clear the ISO_CLK_STBL_ERR int mask. */
int ade7913_clear_iso_clk_stbl_err_int(struct ade7913_dev *dev);

/* Get ISO_PHY_CRC_ERR indicator. */
int ade7913_get_iso_phy_crc_err(struct ade7913_dev *dev, uint8_t *status);

/* Clear the ISO_PHY_CRC_ERR int mask. */
int ade7913_clear_iso_phy_crc_err_int(struct ade7913_dev *dev);

/* Get ISO_EFUSE_MEM_ERR indicator. */
int ade7913_get_iso_efuse_mem_err_err(struct ade7913_dev *dev, uint8_t *status);

/* Clear the ISO_EFUSE_MEM_ERR int mask. */
int ade7913_clear_iso_efuse_mem_err_int(struct ade7913_dev *dev);

/* Get ISO_DIG_MOD_V2_OVF indicator. */
int ade7913_get_iso_dig_mod_v2_ovf(struct ade7913_dev *dev, uint8_t *status);

/* Clear the ISO_DIG_MOD_V2_OVF int mask. */
int ade7913_clear_iso_dig_mod_v2_ovf_int(struct ade7913_dev *dev);

/* Get ISO_DIG_MOD_V1_OVF indicator. */
int ade7913_get_iso_dig_mod_v1_ovf(struct ade7913_dev *dev, uint8_t *status);

/* Clear the ISO_DIG_MOD_V1_OVF int mask. */
int ade7913_clear_iso_dig_mod_v1_ovf_int(struct ade7913_dev *dev);

/* Get ISO_DIG_MOD_I_OVF indicator. */
int ade7913_get_iso_dig_mod_i_ovf(struct ade7913_dev *dev, uint8_t *status);

/* Clear the ISO_DIG_MOD_I_OVF int mask. */
int ade7913_clear_iso_dig_mod_i_ovf_int(struct ade7913_dev *dev);

/* Get ISO_TEST_MMR_ERR indicator. */
int ade7913_get_iso_test_mmr_err(struct ade7913_dev *dev, uint8_t *status);

/* Clear the ISO_TEST_MMR_ERR int mask. */
int ade7913_clear_iso_test_mmr_err_int(struct ade7913_dev *dev);

/* Get ISO_STATUS_RD_ECC_ERR indicator. */
int ade7913_get_iso_status_rd_ecc_err(struct ade7913_dev *dev, uint8_t *status);

/* Get ISO_PHY_ERR indicator. */
int ade7913_get_iso_phy_err(struct ade7913_dev *dev, uint8_t *status);

/* Get ISO_ECC_ERR indicator. */
int ade7913_get_iso_ecc_err(struct ade7913_dev *dev, uint8_t *status);

/* Get CRC_DONE indicator. */
int ade7913_get_crc_done_flag(struct ade7913_dev *dev, uint8_t *status);

/* Clear the CRC_DONE int mask. */
int ade7913_clear_crc_done_int(struct ade7913_dev *dev);

/* Force background register map CRC recalculation. */
int ade7913_force_crc_recalculation(struct ade7913_dev *dev);

/* Get SILICON_REVISION value. */
int ade7913_get_silicon_revision(struct ade7913_dev *dev, uint8_t *silicon_rev);

/* Get VERSION_PRODUCT value. */
int ade7913_get_version_product(struct ade7913_dev *dev, uint8_t *ver_product);

/* Get wave value. */
int ade7913_get_wav(struct ade7913_dev *dev, enum ade7913_wav_e selection,
		    uint32_t *val);

/* DRDY inerrupt enable. */
int ade7913_drdy_int_enable(struct ade7913_dev *dev);

/* DRDY inerrupt disable. */
int ade7913_drdy_int_disable(struct ade7913_dev *dev);

#endif // __ADE7913_H__
