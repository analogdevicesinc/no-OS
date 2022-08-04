/***************************************************************************//**
 *   @file   ad7091r5.h
 *   @brief  Header file for ad7091r5 Driver.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

#ifndef SRC_AD7091R5_H_
#define SRC_AD7091R5_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdbool.h>
#include "no_os_i2c.h"
#include "no_os_gpio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD7091R5_CHANNEL_NO		4
#define AD7091R5_BITS			12
#define AD7091R5_CONV_MASK		NO_OS_GENMASK(AD7091R5_BITS - 1, 0)

/* AD7091r5 registers */
#define AD7091R5_REG_RESULT		0
#define AD7091R5_REG_CHANNEL		1
#define AD7091R5_REG_CONF		2
#define AD7091R5_REG_ALERT		3
#define AD7091R5_REG_CH_LOW_LIMIT(ch)	((ch) * 3 + 4)
#define AD7091R5_REG_CH_HIGH_LIMIT(ch)	((ch) * 3 + 5)
#define AD7091R5_REG_CH_HYSTERESIS(ch)	((ch) * 3 + 6)

/* AD7091R5_REG_RESULT */
#define REG_RESULT_CH_ID(x)		(((x) >> 13) & 0x3)
#define REG_RESULT_CONV_DATA(x)		((x) & AD7091R5_CONV_MASK)

/* AD7091R5_REG_CONF */
#define REG_CONF_SLEEP_MODE_MASK	NO_OS_BIT(0)
#define REG_CONF_SLEEP_MODE(x)		((x & 0x03) << 0)

#define REG_CONF_GPO1_MASK		NO_OS_BIT(2)
#define REG_CONF_GPO1(x)		((x & 0x01) << 2)

#define REG_CONF_GPO0_MASK		NO_OS_BIT(3)
#define REG_CONF_GPO0(x)		((x & 0x01) << 3)

#define REG_CONF_GPO0_MODE_MASK		(NO_OS_BIT(15) | NO_OS_BIT(5) | NO_OS_BIT(4))
#define REG_CONF_GPO0_ALERT(x)		((x & 0x01) << 4)
#define REG_CONF_GPO0_BUSY(x)		((x & 0x01) << 5)
#define REG_CONF_GPO0_DRIVE_TYPE(x)	((x & 0x01) << 15)

#define REG_CONF_CYCLE_TIMER_MASK	(NO_OS_BIT(7) | NO_OS_BIT(6))
#define REG_CONF_CYCLE_TIMER(x)		((x & 0x03) << 6)

#define REG_CONF_GPO2_MASK		NO_OS_BIT(14)
#define REG_CONF_GPO2(x)		((x & 0x01) << 14)

#define REG_CONF_RESET_MASK		NO_OS_BIT(9)
#define REG_CONF_RESET(x)		((x & 0x01) << 9)

#define REG_CONF_MODE_MASK		(NO_OS_BIT(10) | NO_OS_BIT(8))
#define REG_CONF_AUTO(x)		((x & 0x01) << 8)
#define REG_CONF_CMD(x)			((x & 0x01) << 10)

/* AD7091R5_REG_ALERT */
#define REG_ALERT_MASK(x, ch)		(x >> (ch * 2))

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
/**
 * @enum ad7091r5_mode
 * @brief Converter supported modes
 */
enum ad7091r5_mode {
	/** Sample mode, conversion started by CONVST */
	AD7091R5_MODE_SAMPLE,
	/** Command mode, conversion starts on the first pos edge of SCL */
	AD7091R5_MODE_COMMAND,
	/** Sample mode, convert continuously */
	AD7091R5_MODE_AUTOCYCLE,
};

/**
 * @enum ad7091r5_sleep_mode
 * @brief Converter supported sleep modes
 */
enum ad7091r5_sleep_mode {
	/** Default operation:
	 * Sleep mode Off, Internal reference Off */
	AD7091R5_SLEEP_MODE_0,
	/** Sleep mode Off, Internal reference On */
	AD7091R5_SLEEP_MODE_1,
	/** Sleep mode On, Internal reference Off */
	AD7091R5_SLEEP_MODE_2,
	/** Sleep mode On, Internal reference On */
	AD7091R5_SLEEP_MODE_3,
};

/**
 * @enum ad7091r5_port
 * @brief Converter general purpose outputs
 */
enum ad7091r5_port {
	/** GPO0 */
	AD7091R5_GPO0,
	/** GPO1 */
	AD7091R5_GPO1,
	/** GPO2 */
	AD7091R5_GPO2,
};

/**
 * @enum ad7091r5_gpo0_mode
 * @brief Port 0 configuration
 */
enum ad7091r5_gpo0_mode {
	/** GPO0 is output port */
	AD7091R5_GPO0_ENABLED,
	/** GPO0 is Alert indicator */
	AD7091R5_GPO0_ALERT,
	/** GPO0 is busy indicator, device is converting */
	AD7091R5_GPO0_BUSY,
};

/**
 * @enum ad7091r5_cycle_timer
 * @brief Timer used for autocycle mode
 */
enum ad7091r5_cycle_timer {
	/** 100 uS */
	AD7091R5_CYCLE_TIMER_100uS,
	/** 200 uS */
	AD7091R5_CYCLE_TIMER_200uS,
	/** 400 uS */
	AD7091R5_CYCLE_TIMER_400uS,
	/** 800 uS */
	AD7091R5_CYCLE_TIMER_800uS,
};

/**
 * @enum ad7091r5_alert_type
 * @brief Alert status
 */
enum ad7091r5_alert_type {
	/** No alert */
	AD7091R5_NO_ALERT,
	/** High alert */
	AD7091R5_HIGH_ALERT,
	/** Low alert */
	AD7091R5_LOW_ALERT,
};

/**
 * @enum ad7091r5_limit
 * @brief Limit type
 */
enum ad7091r5_limit {
	/** Low limit */
	AD7091R5_LOW_LIMIT,
	/** High limit */
	AD7091R5_HIGH_LIMIT,
	/** Hysteresis */
	AD7091R5_HYSTERESIS,
};

/**
 * @struct ad7091r5_init_param
 * @brief  Structure containing the init parameters needed by the ad7091r5 device
 */
struct ad7091r5_init_param {
	/* I2C */
	struct no_os_i2c_init_param		*i2c_init;
	/** RESET GPIO initialization structure. */
	struct no_os_gpio_init_param	*gpio_resetn;
};

/**
 * @struct ad7091r5_dev
 * @brief  Structure representing an ad7091r5 device
 */
struct ad7091r5_dev {
	/* I2C descriptor */
	struct no_os_i2c_desc 	*i2c_desc;
	/** RESET GPIO handler. */
	struct no_os_gpio_desc	*gpio_resetn;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Initialize the device. */
int32_t ad7091r5_init(struct ad7091r5_dev **device,
		      struct ad7091r5_init_param *init_param);

/* Remove the device and release resources. */
int32_t ad7091r5_remove(struct ad7091r5_dev *dev);

/* Set device mode */
int32_t ad7091r5_set_mode(struct ad7091r5_dev *dev,
			  enum ad7091r5_mode mode);

/* Set device sleep mode */
int32_t ad7091r5_sleep_mode(struct ad7091r5_dev *dev,
			    enum ad7091r5_sleep_mode mode);

/* Set device set port value */
int32_t ad7091r5_set_port(struct ad7091r5_dev *dev,
			  enum ad7091r5_port port, bool value);

/* Set device set GPO0 mode */
int32_t ad7091r5_set_gpo0_mode(struct ad7091r5_dev *dev,
			       enum ad7091r5_gpo0_mode mode,
			       bool is_cmos);
/* Set cycle timer for autocycle mode*/
int32_t ad7091r5_set_cycle_timer(struct ad7091r5_dev *dev,
				 enum ad7091r5_cycle_timer timer);

/* Set high limit, low limit, hysteresis. */
int32_t ad7091r5_set_limit(struct ad7091r5_dev *dev,
			   enum ad7091r5_limit limit,
			   uint8_t channel,
			   uint16_t value);

/* Get alert. */
int32_t ad7091r5_get_alert(struct ad7091r5_dev *dev,
			   uint8_t channel,
			   enum ad7091r5_alert_type *alert);

/* Get high limit, low limit, hysteresis. */
int32_t ad7091r5_get_limit(struct ad7091r5_dev *dev,
			   enum ad7091r5_limit limit,
			   uint8_t channel,
			   uint16_t *value);

/* Select device channel. */
int32_t ad7091r5_set_channel(struct ad7091r5_dev *dev,
			     uint8_t channel);

/* Read one sample. */
int32_t ad7091r5_read_one(struct ad7091r5_dev *dev,
			  uint8_t channel,
			  uint16_t *read_val);

#endif /* SRC_AD7091R5_H_ */
