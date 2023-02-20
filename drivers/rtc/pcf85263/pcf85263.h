/***************************************************************************//**
 *   @file   pcf85263.h
 *   @brief  Header file of pcf85263 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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

#ifndef __PCF85263_H__
#define __PCF85263_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <string.h>
#include "no_os_util.h"
#include "no_os_i2c.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define PCF85263_REG_100TH_SECONDS		0x00
#define PCF85263_REG_SECONDS			0x01
#define PCF85263_REG_MINUTES			0x02
#define PCF85263_REG_HOURS			0x03
#define PCF85263_REG_DAYS			0x04
#define PCF85263_REG_WEEKDAYS			0x05
#define PCF85263_REG_MONTHS			0x06
#define PCF85263_REG_YEARS			0x07
#define PCF85263_REG_SECOND_ALARM1		0x08
#define PCF85263_REG_MINUTE_ALARM1		0x09
#define PCF85263_REG_HOUR_ALARM1		0x0A
#define PCF85263_REG_DAY_ALARM1			0x0B
#define PCF85263_REG_MONTH_ALARM1		0x0C
#define PCF85263_REG_MINUTE_ALARM2		0x0D
#define PCF85263_REG_HOUR_ALARM2		0x0E
#define PCF85263_REG_WEEKDAY_ALARM2		0x0F
#define PCF85263_REG_ALARM_ENABLES		0x10
#define PCF85263_REG_TSR1_SECONDS		0x11
#define PCF85263_REG_TSR1_MINUTES		0x12
#define PCF85263_REG_TSR1_HOURS			0x13
#define PCF85263_REG_TSR1_DAYS			0x14
#define PCF85263_REG_TSR1_MONTHS		0x15
#define PCF85263_REG_TSR1_YEARS			0x16
#define PCF85263_REG_TSR2_SECONDS		0x17
#define PCF85263_REG_TSR2_MINUTES		0x18
#define PCF85263_REG_TSR2_HOURS			0x19
#define PCF85263_REG_TSR2_DAYS			0x1A
#define PCF85263_REG_TSR2_MONTHS		0x1B
#define PCF85263_REG_TSR2_YEARS			0x1C
#define PCF85263_REG_TSR3_SECONDS		0x1D
#define PCF85263_REG_TSR3_MINUTES		0x1E
#define PCF85263_REG_TSR3_HOURS			0x1F
#define PCF85263_REG_TSR3_DAYS			0x20
#define PCF85263_REG_TSR3_MONTHS		0x21
#define PCF85263_REG_TSR3_YEARS			0x22
#define PCF85263_REG_TSR_MODE			0x23
#define PCF85263_REG_OFFSET			0x24
#define PCF85263_REG_OSCILLATOR			0x25
#define PCF85263_REG_BATTERY_SWITCH		0x26
#define PCF85263_REG_PIN_IO			0x27
#define PCF85263_REG_FUNCTION			0x28
#define PCF85263_REG_INTA_ENABLE		0x29
#define PCF85263_REG_INTB_ENABLE		0x2A
#define PCF85263_REG_FLAGS			0x2B
#define PCF85263_REG_RAM_BYTE			0x2C
#define PCF85263_REG_WATCH_DOG			0x2D
#define PCF85263_REG_STOP_ENABLE		0x2E
#define PCF85263_REG_RESETS			0x2F

#define PCF85263_CPR				0xA4
#define PCF85263_BATTERY_SW_MSK			NO_OS_BIT(4)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct pcf85263_date
 * @brief Structure holding the date parameters.
 */
struct pcf85263_date {
	uint8_t				sec;
	uint8_t				min;
	uint8_t				hr;
	uint8_t				day;
	uint8_t				mon;
	uint8_t				year;
};

/**
 * @struct pcf85263_init_param
 * @brief PCF85263 Device initialization parameters.
 */
struct pcf85263_init_param {
	/** Device communication descriptor */
	struct no_os_i2c_init_param 	*i2c_init;
	uint8_t				battery_en;
};

/**
 * @struct pcf85263_dev
 * @brief PCF85263 Device structure.
 */
struct pcf85263_dev {
	/** Device communication descriptor */
	struct no_os_i2c_desc		*i2c_desc;
	uint8_t				battery_en;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Read device register. */
int pcf85263_read(struct pcf85263_dev *dev, uint8_t reg_addr,
		  uint8_t *reg_data);

/* Write device register. */
int pcf85263_write(struct pcf85263_dev *dev, uint8_t reg_addr,
		   uint8_t reg_data);

/* Update specific register bits. */
int pcf85263_update_bits(struct pcf85263_dev *dev, uint8_t reg_addr,
			 uint8_t mask, uint8_t reg_data);

/* Set date */
int pcf85263_set_date(struct pcf85263_dev *dev, struct pcf85263_date date);

/* Read time stamp */
int pcf85263_read_ts(struct pcf85263_dev *dev, struct pcf85263_date *ts);

/* Initialize the device. */
int pcf85263_init(struct pcf85263_dev **device,
		  struct pcf85263_init_param init_param);

/* Remove the device and release resources. */
int pcf85263_remove(struct pcf85263_dev *dev);

#endif //__PCF85263_H__
