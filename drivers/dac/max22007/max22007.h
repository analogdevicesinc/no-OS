/***************************************************************************//**
 *   @file   max22007.h
 *   @brief  Header file of MAX22007 Driver.
 *   @author Janani Sunil (janani.sunil@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef _MAX22007_H
#define _MAX22007_H

#include "no_os_spi.h"
#include "no_os_util.h"

/* Register Map */
#define MAX22007_REV_ID_REG						0x00
#define MAX22007_STAT_AND_INTR_REG				0x01
#define MAX22007_INR_ENABLE_REG					0x02
#define MAX22007_CONFIG_REG						0x03
#define MAX22007_CTRL_REG						0x04
#define MAX22007_CHANNEL_MODE_REG				0x05
#define MAX22007_SOFT_RESET_REG					0x06
#define MAX22007_CHANNEL_DATA_REG(channel)		(0x07 + (channel))
#define MAX22007_GPIO_CTRL_REG					0x0B
#define MAX22007_GPIO_DATA_REG					0x0C
#define MAX22007_GPIO_EDGE_CTRL_REG				0x0D
#define MAX22007_GPIO_EDGE_DETECTION_STAT_REG	0x0E

/* Revision Register Masks */
#define MAX22007_REV_ID_MASK					NO_OS_GENMASK(7, 0)
#define MAX22007_PART_ID_MASK					NO_OS_GENMASK(15, 8)

/* Configuration Register Masks */
#define MAX22007_CONFIG_LD_CNFG_MASK			NO_OS_GENMASK(15, 12)
#define MAX22007_CONFIG_COMP_EDGE_MASK			NO_OS_GENMASK(11, 10)
#define MAX22007_CONFIG_REF_SEL_MASK			NO_OS_BIT(8)
#define MAX22007_TIMEOUT_SEL_MASK				NO_OS_GENMASK(7, 4)
#define MAX22007_TIMEOUT_CNFG_MASK				NO_OS_BIT(3)
#define MAX22007_TIMEOUT_EN_MASK				NO_OS_BIT(2)
#define MAX22007_OVERCURRENT_CTRL_MASK			NO_OS_BIT(1)
#define MAX22007_CRC_EN_MASK					NO_OS_BIT(0)

/* Control Register Masks */
#define MAX22007_LD_CTRL_MASK					NO_OS_GENMASK(15, 12)
#define LD_CTRL_CH_MASK(channel)				(NO_OS_BIT(12) << (channel))

/* Channel Mode Register Masks */
#define MAX22007_CH_MODE_MASK					NO_OS_GENMASK(15, 12)
#define MAX22007_CH_MODE_CH_MASK(channel)		(NO_OS_BIT(12) << (channel))
#define MAX22007_CH_PWR_MASK					NO_OS_GENMASK(11, 8)
#define MAX22007_CH_PWR_CH_MASK(channel)		(NO_OS_BIT(8) << (channel))

/* Soft Reset Register Masks */
#define MAX22007_SW_CLR_MASK					NO_OS_BIT(12)
#define MAX22007_SW_RST_MASK					NO_OS_BIT(8)

/* GPIO Control Register Masks */
#define MAX22007_GPIO_CTRL_MASK					NO_OS_GENMASK(15, 8)
#define MAX22007_GPIO_DIR_MASK					NO_OS_GENMASK(7, 0)

/* DAC Channel register masks */
#define DAC_CH_DATA_MASK						NO_OS_GENMASK(15, 4)

/* GPIO Data Register Masks */
#define MAX22007_GPO_DATA_MASK					NO_OS_GENMASK(15, 8)
#define MAX22007_GPI_DATA_MASK					NO_OS_GENMASK(7, 0)

#define MAX22007_SPI_READ						0x80
#define MAX22007_NUM_CHANNELS					4
#define MAX22007_REV_ID							0xBB45
#define MAX22007_ADRR_MASK						NO_OS_GENMASK(7, 1)
#define MAX22007_RW_MASK						NO_OS_BIT(0)
#define MAX22007_FRAME_SIZE						3
#define MAX22007_PAYLOAD_LSB_MASK				NO_OS_GENMASK(7, 0)
#define MAX22007_PAYLOAD_MSB_MASK				NO_OS_GENMASK(15, 8)

enum max22007_dac_latch_mode {
	LDAC_CONTROL,
	TRANSPARENT_LATCH
};

enum max22007_ref_mode {
	INTERNAL_REF,
	EXTERNAL_REF
};

enum max22007_timeout_sel {
	MAX22007_TIMEOUT_100MS,
	MAX22007_TIMEOUT_200MS,
	MAX22007_TIMEOUT_300MS,
	MAX22007_TIMEOUT_400MS,
	MAX22007_TIMEOUT_500MS,
	MAX22007_TIMEOUT_600MS,
	MAX22007_TIMEOUT_700MS,
	MAX22007_TIMEOUT_800MS,
	MAX22007_TIMEOUT_900MS,
	MAX22007_TIMEOUT_1000MS,
	MAX22007_TIMEOUT_1100MS,
	MAX22007_TIMEOUT_1200MS,
	MAX22007_TIMEOUT_1300MS,
	MAX22007_TIMEOUT_1400MS,
	MAX22007_TIMEOUT_1500MS,
	MAX22007_TIMEOUT_1600MS,
};

enum max22007_timeout_cnfg {
	TIMEOUT_EVENT_ONLY,
	TIMEOUT_RESET
};

enum max22007_channel_mode {
	MAX22007_VOLTAGE_MODE,
	MAX22007_CURRENT_MODE,
};

enum max22007_channel_power {
	MAX22007_CH_POWER_OFF,
	MAX22007_CH_POWER_ON,
};

struct max22007_spi_timeout_config {
	enum max22007_timeout_sel timeout_sel;
	enum max22007_timeout_cnfg timeout_cnfg;
	bool timeout_en;
};

struct max22007_channel_config {
	enum max22007_dac_latch_mode latch_mode;
	enum max22007_channel_mode channel_mode;
	enum max22007_channel_power channel_power;
};

struct max22007_init_param {
	struct no_os_spi_init_param *comm_param;
	struct max22007_channel_config channel_config[MAX22007_NUM_CHANNELS];
	struct max22007_spi_timeout_config timeout_config;
	enum max22007_ref_mode ref_mode;
	bool crc_en;
};

struct max22007_dev {
	struct no_os_spi_desc *comm_desc;
	uint8_t buff[MAX22007_FRAME_SIZE + 1];
	bool crc_en;
};

int max22007_reg_read(struct max22007_dev *dev, uint8_t reg_addr,
		      uint16_t *reg_data);
int max22007_reg_write(struct max22007_dev *dev, uint8_t reg_addr,
		       uint16_t reg_data);
int max22007_reg_write_msk(struct max22007_dev *dev, uint8_t reg_addr,
			   uint16_t data, uint16_t mask);
int max22007_set_latch_mode(struct max22007_dev *dev, uint8_t ch,
			    enum max22007_dac_latch_mode mode);
int max22007_get_latch_mode(struct max22007_dev *dev, uint8_t ch,
			    enum max22007_dac_latch_mode *mode);
int max22007_set_reference(struct max22007_dev *dev,
			   enum max22007_ref_mode mode);
int max22007_get_reference(struct max22007_dev *dev,
			   enum max22007_ref_mode *mode);
int max22007_set_timeout(struct max22007_dev *dev, bool timeout_en,
			 enum max22007_timeout_sel timeout_sel, enum max22007_timeout_cnfg timeout_cnfg);
int max22007_set_channel_mode(struct max22007_dev *dev, uint8_t ch,
			      enum max22007_channel_mode mode);
int max22007_get_channel_mode(struct max22007_dev *dev, uint8_t ch,
			      enum max22007_channel_mode *mode);
int max22007_set_channel_power(struct max22007_dev *dev, uint8_t ch,
			       enum max22007_channel_power mode);
int max22007_get_channel_power(struct max22007_dev *dev, uint8_t ch,
			       enum max22007_channel_power *mode);
int max22007_configure_crc(struct max22007_dev *dev, bool crc_en);
int max22007_write_ldac(struct max22007_dev *dev, uint8_t ch_mask);
int max22007_write_channel_data(struct max22007_dev *dev, uint8_t ch,
				uint16_t data);
int max22007_read_channel_data(struct max22007_dev *dev, uint8_t ch,
			       uint16_t *data);
int max22007_init(struct max22007_dev **device,
		  struct max22007_init_param init_param);
int max22007_remove(struct max22007_dev *dev);

#endif // _MAX22007_H
