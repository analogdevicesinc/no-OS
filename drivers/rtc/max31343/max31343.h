/*******************************************************************************
 *   @file   max31343.h
 *   @brief  Max31343 Real Time Clock header file
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
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

#ifndef __MAX31343__
#define __MAX31343__

#include <stdint.h>
#include <string.h>
#include "no_os_util.h"
#include "no_os_i2c.h"

#define MAX31343_I2C_ADDRESS			0x68
#define MAX31343_R_STATUS			0x00
#define MAX31343_R_INT_EN			0x01
#define MAX31343_R_RTC_RESET			0x02
#define MAX31343_R_CFG1				0x03
#define MAX31343_R_CFG2				0x04
#define MAX31343_R_TIMER_CONFIG			0x05
#define MAX31343_R_SECONDS			0x06
#define MAX31343_R_MINUTES			0x07
#define MAX31343_R_HOURS			0x08
#define MAX31343_R_DAY				0x09
#define MAX31343_R_DATE				0x0A
#define MAX31343_R_MONTH			0x0B
#define MAX31343_R_YEAR				0x0C
#define MAX31343_R_ALM1_SEC			0x0D
#define MAX31343_R_ALM1_MIN			0x0E
#define MAX31343_R_ALM1_HRS			0x0F
#define MAX31343_R_ALM1DAY_DATE			0x10
#define MAX31343_R_ALM1_MON			0x11
#define MAX31343_R_ALM1_YEAR			0x12
#define MAX31343_R_ALM2_MIN			0x13
#define MAX31343_R_ALM2_HRS			0x14
#define MAX31343_R_ALM2DAY_DATE			0x15
#define MAX31343_R_TIMER_COUNT			0x16
#define MAX31343_R_TIMER_INIT			0x17
#define MAX31343_R_PWR_MGMT			0x18
#define MAX31343_R_TRICKLE			0x19
#define MAX31343_R_TEMP_MSB			0x1A
#define MAX31343_R_TEMP_LSB			0x1B
#define MAX31343_R_TS_CONFIG			0x1C
#define MAX31343_R_RAM_REG_START		0x22
#define MAX31343_R_RAM_REG_END			0x61

/* Status register bits */
#define MAX31343_F_STATUS_A1F			NO_OS_BIT(0)
#define MAX31343_F_STATUS_A2F			NO_OS_BIT(1)
#define MAX31343_F_STATUS_TIF			NO_OS_BIT(2)
#define MAX31343_F_STATUS_TSF			NO_OS_BIT(3)
#define MAX31343_F_STATUS_PFAIL			NO_OS_BIT(5)
#define MAX31343_F_STATUS_OSF			NO_OS_BIT(6)
#define MAX31343_F_STATUS_PSDECT		NO_OS_BIT(7)

/* Interrupt enable register bits */
#define MAX31343_F_INT_EN_A1IE			NO_OS_BIT(0)
#define MAX31343_F_INT_EN_A2IE			NO_OS_BIT(1)
#define MAX31343_F_INT_EN_TIE			NO_OS_BIT(2)
#define MAX31343_F_INT_EN_TSIE			NO_OS_BIT(3)
#define MAX31343_F_INT_EN_PFAILE		NO_OS_BIT(5)
#define MAX31343_F_INT_EN_DOSF			NO_OS_BIT(6)

/* RTC reset register bits */
#define MAX31343_F_RTC_RESET_SWRST		NO_OS_BIT(0)

/* Config 1 register bits */
#define MAX31343_F_CFG1_ENOSC			NO_OS_BIT(1)
#define MAX31343_F_CFG1_I2C_TIMEOUT		NO_OS_BIT(3)
#define MAX31343_F_CFG1_DATA_RET		NO_OS_BIT(4)

/* Config 2 register bits */
#define MAX31343_F_CFG2_SQW_HZ			NO_OS_BIT(0)
#define MAX31343_F_CFG2_CLKO_HZ			NO_OS_BIT(3)
#define MAX31343_F_CFG2_ENCLKO			NO_OS_BIT(7)

/* Timer config register bits */
#define MAX31343_F_TIMER_CONFIG_TFS		NO_OS_BIT(0)
#define MAX31343_F_TIMER_CONFIG_TRPT		NO_OS_BIT(2)
#define MAX31343_F_TIMER_CONFIG_TPAUSE		NO_OS_BIT(3)
#define MAX31343_F_TIMER_CONFIG_TE		NO_OS_BIT(4)

/* Power management register bits */
#define MAX31343_F_PWR_MGMT_DMAN_SEL		NO_OS_BIT(2)
#define MAX31343_F_PWR_MGMT_D_VBACK_SEL		NO_OS_BIT(3)
#define MAX31343_F_PWR_MGMT_PFVT		NO_OS_BIT(4)

/* Trickle register bits */
#define MAX31343_F_TRICKLE_D_TRICKLE		NO_OS_BIT(0)
#define MAX31343_F_TRICKLE_TCHE			NO_OS_BIT(4)

/* Temperature sensor config register bits */
#define MAX31343_F_TS_CONFIG_TTSINT		NO_OS_BIT(3)
#define MAX31343_F_TS_CONFIG_ONESHOT_MODE	NO_OS_BIT(6)
#define MAX31343_F_TS_CONFIG_AUTO_MODE		NO_OS_BIT(7)

/**
 * @struct max31343_time_stamp
 * @brief Structure holding the date parameters.
 */
struct max31343_time_stamp {
	uint8_t				sec;    /* Seconds              [0-61] */
	uint8_t				min;    /* Minutes              [0-59] */
	uint8_t				hr;     /* Hours                [0-23] */
	uint8_t				day;    /* Day of month         [1-31] */
	uint8_t				mon;    /* Month                [0-11] */
	uint8_t				year;   /* Year                 [2000-2199] */
};

/**
 * @struct max31343_init_param
 * @brief max31343 Device initialization parameters.
 */
struct max31343_init_param {
	/** Device communication descriptor */
	struct no_os_i2c_init_param 	*i2c_init;
	uint8_t				battery_en;
};

/**
 * @struct max31343_dev
 * @brief max31343 Device structure.
 */
struct max31343_dev {
	/** Device communication descriptor */
	struct no_os_i2c_desc		*i2c_desc;
	uint8_t				battery_en;
};

/* Read device register. */
int max31343_reg_read(struct max31343_dev *dev, uint8_t reg_addr,
		      uint8_t *reg_data);

/* Write device register. */
int max31343_reg_write(struct max31343_dev *dev, uint8_t reg_addr,
		       uint8_t reg_data);

/* Update specific register bits. */
int max31343_update_bits(struct max31343_dev *dev, uint8_t reg_addr,
			 uint8_t mask, uint8_t reg_data);

/* Set time stemp */
int max31343_set_time_stamp(struct max31343_dev *dev,
			    struct max31343_time_stamp ts);

/* Read time stamp */
int max31343_reg_read_time_stamp(struct max31343_dev *dev,
				 struct max31343_time_stamp *ts);

/* Initialize the device. */
int max31343_init(struct max31343_dev **device,
		  struct max31343_init_param init_param);

/* Remove the device and release resources. */
int max31343_remove(struct max31343_dev *dev);

#endif /* __MAX31343__ */
