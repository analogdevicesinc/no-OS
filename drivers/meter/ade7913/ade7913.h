/***************************************************************************//**
 *   @file   ade7913.h
 *   @brief  Header file of ADE7913 Driver.
 *   @author Radu Etz (radu.etz@analog.com)
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
#define ADE7913_SPI_READ				NO_OS_BIT(2)

/* ENABLE and DISABLE */
#define ENABLE						1u
#define DISABLE						0u

/* ADE7913 Register Map */
#define ADE7913_REG_IWV					0x00
#define ADE7913_REG_V1WV				0x01
#define ADE7913_REG_V2WV				0x02
#define ADE7913_REG_ADC_CRC				0x04
#define ADE7913_REG_CTRL_CRC				0x05
#define ADE7913_REG_CNT_SNAPSHOT			0x07
#define ADE7913_REG_CONFIG				0x08
#define ADE7913_REG_STATUS0				0x09
#define ADE7913_REG_LOCK				0x0A
#define ADE7913_REG_SYNC_SNAP				0x0B
#define ADE7913_REG_COUNTER0				0x0C
#define ADE7913_REG_COUNTER1				0x0D
#define ADE7913_REG_EMI_CTRL				0x0E
#define ADE7913_REG_STATUS1				0x0F
#define ADE7913_REG_TEMPOS				0x08

/* ADE7913_REG_CNT_SNAPSHOT Bit Definition */
#define ADE7913_CNT_VAL_MSK				NO_OS_GENMASK(11, 0)

/* ADE7913_REG_CONFIG Bit Definition */
#define ADE7913_CLKOUT_EN_MSK				NO_OS_BIT(0)
#define ADE7913_PWRDWN_EN_MSK				NO_OS_BIT(2)
#define ADE7913_TEMP_EN_MSK				NO_OS_BIT(3)
#define ADE7913_ADC_FREQ_MSK				NO_OS_GENMASK(5, 4)
#define ADE7913_SWRST_MSK				NO_OS_BIT(6)
#define ADE7913_BW_MSK					NO_OS_BIT(7)

/* ADE7913_REG_STATUS0 Bit Definition */
#define ADE7913_RESET_ON_MSK				NO_OS_BIT(0)
#define ADE7913_CRC_STAT_MSK				NO_OS_BIT(1)
#define ADE7913_IC_PROT_MSK				NO_OS_BIT(2)

/* ADE7913_REG_LOCK Bit Definition */
#define ADE7913_LOCK_KEY_MSK				NO_OS_GENMASK(5, 4)

/* ADE7913_REG_SYNC_SNAP Bit Definition */
#define ADE7913_SYNC_MSK				NO_OS_BIT(0)
#define ADE7913_SNAP_MSK				NO_OS_BIT(1)

/* ADE7913_REG_EMI_CTRL Bit Definition */
#define ADE7913_SLOT0_MSK				NO_OS_BIT(0)
#define ADE7913_SLOT1_MSK				NO_OS_BIT(1)
#define ADE7913_SLOT2_MSK				NO_OS_BIT(2)
#define ADE7913_SLOT3_MSK				NO_OS_BIT(3)
#define ADE7913_SLOT4_MSK				NO_OS_BIT(4)
#define ADE7913_SLOT5_MSK				NO_OS_BIT(5)
#define ADE7913_SLOT6_MSK				NO_OS_BIT(6)
#define ADE7913_SLOT7_MSK				NO_OS_BIT(7)

/* ADE7913_REG_STATUS1 Bit Definition */
#define ADE7913_VERSION_MSK				NO_OS_GENMASK(2, 0)
#define ADE7913_ADC_NA_MSK				NO_OS_BIT(6)

/* Configuration Register Write Lock */
#define ADE7913_LOCK_KEY				0XCA
#define ADE7913_UNLOCK_KEY				0X9C

/* Version Product */
#define ADE7913_3_CHANNEL_ADE7913			3U
#define ADE7913_2_CHANNEL_ADE7912			2U

/* Nominal reference voltage */
#define ADE7913_VREF_V					(788)
#define ADE7913_VREF_I					(49)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum ade7913_adc_freq_e
 * @brief ADE7913 ADC output frequency.
 */
enum ade7913_adc_freq_e {
	/* 125 us period */
	ADE7913_ADC_FREQ_8KHZ,
	/* 250 us period */
	ADE7913_ADC_FREQ_4KHZ,
	/* 500 us period */
	ADE7913_ADC_FREQ_2KHZ,
	/* 1 ms period */
	ADE7913_ADC_FREQ_1KHZ
};

/**
 * @struct ade7913_init_param
 * @brief ADE7913 Device initialization parameters.
 */
struct ade7913_init_param {
	/* Number of devices */
	uint8_t no_devs;
	/* burst mode */
	uint8_t burst_mode;
	/* Device 1 communication descriptor */
	struct no_os_spi_init_param 	*spi_init0;
	/* Device 2 communication descriptor */
	struct no_os_spi_init_param 	*spi_init1;
	/* Device 3 communication descriptor */
	struct no_os_spi_init_param 	*spi_init2;
};

/**
 * @struct ade7913_dev
 * @brief ADE7913 Device structure.
 */
struct ade7913_dev {
	/** Device communication descriptor */
	struct no_os_spi_desc		*spi_desc;
	/** Device 1 communication descriptor */
	struct no_os_spi_desc		*spi_desc0;
	/** Device 2 communication descriptor */
	struct no_os_spi_desc		*spi_desc1;
	/** Device 3 communication descriptor */
	struct no_os_spi_desc		*spi_desc2;
	/* Version product */
	uint8_t 			*ver_product;
	/* I_WAV */
	int32_t				*i_wav;
	/* V1_WAV */
	int32_t				*v1_wav;
	/* V2_WAV */
	int32_t				*v2_wav;
	/* I_WAV multiple devices */
	int32_t				*i_wav_m;
	/* V1_WAV multiple devices */
	int32_t				*v1_wav_m;
	/* V2_WAV multiple devices */
	int32_t				*v2_wav_m;
	/* ADC_CRC */
	uint16_t            		*adc_crc;
	/* Status 0 */
	uint8_t             		*status0;
	/* CNT_SNAPSHOT */
	uint16_t            		*cnt_snapshot;
	/* number of devices */
	uint8_t 			no_devs;
	/* burst mode */
	uint8_t 			burst_mode;
	/** IRQ device descriptor used to handle interrupt routine for GPIO RDY */
	struct no_os_irq_ctrl_desc 	*irq_ctrl;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

// Read device register.
int ade7913_read(struct ade7913_dev *dev, uint8_t reg_addr, uint8_t *reg_data);

// Read multiple devices
int ade7913_read_waveforms(struct ade7913_dev *dev, uint8_t reg_addr,
			   uint8_t *reg_data);

// Write device register.
int ade7913_write(struct ade7913_dev *dev, uint8_t reg_addr, uint8_t reg_data);

// Write broadcast.
int ade7913_write_broadcast(struct ade7913_dev *dev, uint8_t reg_addr,
			    uint8_t *reg_data);

// Update specific register bits.
static int ade7913_update_bits(struct ade7913_dev *dev, uint8_t reg_addr,
			       uint8_t mask, uint8_t reg_data);

// Initialize the device.
int ade7913_init(struct ade7913_dev **device,
		 struct ade7913_init_param init_param);

// Remove the device and release resources.
int ade7913_remove(struct ade7913_dev *dev);

// Reset the device using SW reset.
int ade7913_sw_reset(struct ade7913_dev *dev);

// Lock device.
int ade7913_wr_lock(struct ade7913_dev *dev);

// Unlock device.
int ade7913_wr_unlock(struct ade7913_dev *dev);

// Get synchronization counter value.
int ade7913_get_sync_cnt_val(struct ade7913_dev *dev, uint16_t *counter);

// Set clkout enable.
int ade7913_set_clkout_en(struct ade7913_dev *dev,
			  uint8_t clkout_en);

// Power down enable.
int ade7913_pwrdwn(struct ade7913_dev *dev,
		   uint8_t pwrdwn);

// Temperature enable.
int ade7913_temp_en(struct ade7913_dev *dev,
		    uint8_t temp_en);

// Sync enable.
int ade7913_sync_en(struct ade7913_dev *dev);

// Set ADC frequency
int ade7913_adc_freq(struct ade7913_dev *dev,
		     enum ade7913_adc_freq_e frequency);

// Digital lpf bandwith select.
int ade7913_lfp_bw(struct ade7913_dev *dev, uint8_t bw);

// CRC of config registers status
int ade7913_crc_status(struct ade7913_dev *dev, uint8_t *status);

// IC config regs protection status.
int ade7913_ic_prot_status(struct ade7913_dev *dev, uint8_t *status);

// Set EMI CTRL register.
int ade7913_emi_ctrl(struct ade7913_dev *dev,
		     uint8_t emi_ctrl);

// ADC not accesed during one period status.
int ade7913_adc_na_status(struct ade7913_dev *dev, uint8_t *status);

// Cnt snapshot.
int ade7913_cnt_snapshot_val(struct ade7913_dev *dev, uint16_t *val);

// Get version product value.
int ade7913_get_version_product(struct ade7913_dev *dev,
				uint8_t *ver_product);

#endif // __ADE7913_H__
