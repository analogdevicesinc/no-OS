/***************************************************************************//**
 *   @file   ad7091r8.h
 *   @brief  Implementation of AD7091R-8 driver header file.
 *   @author Marcelo Schmitt (marcelo.schmitt@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#ifndef __AD7091R8_H__
#define __AD7091R8_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdbool.h>

#include "no_os_spi.h"
#include "no_os_gpio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD7091R_NUM_CHANNELS(id)	(1 << ((id) + 1))
#define AD7091R8_BITS			12

#define AD7091R8_CONV_MASK		NO_OS_GENMASK(AD7091R8_BITS - 1, 0)

/* AD7091r8 registers */
#define AD7091R8_REG_RESULT		0x00
#define AD7091R8_REG_CHANNEL		0x01
#define AD7091R8_REG_CONF		0x02
#define AD7091R8_REG_ALERT		0x03
#define AD7091R8_REG_CH_LOW_LIMIT(ch)	((ch) * 3 + 4)
#define AD7091R8_REG_CH_HIGH_LIMIT(ch)	((ch) * 3 + 5)
#define AD7091R8_REG_CH_HYSTERESIS(ch)	((ch) * 3 + 6)

/* AD7091R8_REG_RESULT */
#define AD7091R8_REG_RESULT_DATA_MASK	NO_OS_GENMASK(11, 0)
#define AD7091R8_REG_RESULT_ALT_MASK	NO_OS_BIT(12)
#define AD7091R8_REG_RESULT_CH_ID_MASK	NO_OS_GENMASK(15, 13)

/* AD7091R8_REG_CONF */
#define REG_CONF_SLEEP_MODE_MASK	NO_OS_GENMASK(1, 0)
#define REG_CONF_GPO1_MASK		NO_OS_BIT(2)
#define REG_CONF_GPO0_MASK		NO_OS_BIT(3)
#define REG_CONF_GPO0_MODE_MASK		NO_OS_GENMASK(6, 4)
#define REG_CONF_ALERT_STICKY_MASK	NO_OS_BIT(7)
#define REG_CONF_RESET_MASK		NO_OS_BIT(9)

/* AD7091R8_REG_ALERT */
#define REG_ALERT_MASK(x, ch)		(x >> (ch * 2))

/* AD7091R8 read/write protocol masks */
#define AD7091R8_REG_DATA_MSK		NO_OS_GENMASK(9, 0)
#define AD7091R8_RD_WR_FLAG_MSK		NO_OS_BIT(10)
#define AD7091R8_REG_ADDR_MSK		NO_OS_GENMASK(15, 11)

/*****************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
enum ad7091r8_device_id {
	AD7091R2,
	AD7091R4,
	AD7091R8,
};

static char * const ad7091r8_names[] = {
	[AD7091R2] = "ad7091r-2",
	[AD7091R4] = "ad7091r-4",
	[AD7091R8] = "ad7091r-8",
};

/**
 * @enum ad7091r8_sleep_mode
 * @brief Converter supported sleep modes
 */
enum ad7091r8_sleep_mode {
	/** Default operation:
	 * Sleep mode Off, Internal reference Off */
	AD7091R8_SLEEP_MODE_0,
	/** Sleep mode Off, Internal reference On */
	AD7091R8_SLEEP_MODE_1,
	/** Sleep mode On, Internal reference Off */
	AD7091R8_SLEEP_MODE_2,
	/** Sleep mode On, Internal reference On */
	AD7091R8_SLEEP_MODE_3,
};

/**
 * @enum ad7091r8_port
 * @brief Converter general purpose outputs
 */
enum ad7091r8_port {
	/** GPO0 */
	AD7091R8_GPO0,
	/** GPO1 */
	AD7091R8_GPO1,
};

/**
 * @enum ad7091r8_gpo0_mode
 * @brief Port 0 configuration
 */
enum ad7091r8_gpo0_mode {
	/** GPO0 is output port */
	AD7091R8_GPO0_ENABLED = 0,
	/** GPO0 is Alert indicator */
	AD7091R8_GPO0_ALERT = 16,
	/** GPO0 is busy indicator, device is converting */
	AD7091R8_GPO0_BUSY = 48,
};

/**
 * @enum ad7091r8_limit
 * @brief Limit type
 */
enum ad7091r8_limit {
	/** Low limit */
	AD7091R8_LOW_LIMIT,
	/** High limit */
	AD7091R8_HIGH_LIMIT,
	/** Hysteresis */
	AD7091R8_HYSTERESIS,
};

/**
 * @enum ad7091r8_alert_type
 * @brief Alert status
 */
enum ad7091r8_alert_type {
	/** No alert */
	AD7091R8_NO_ALERT,
	/** High alert */
	AD7091R8_HIGH_ALERT,
	/** Low alert */
	AD7091R8_LOW_ALERT,
};

struct ad7091r8_dev {
	/** SPI descriptor **/
	//spi_desc *spi_desc;
	struct no_os_spi_desc *spi_desc;
	/* Reference voltage */
	int vref_mv;
	/* CONVST GPIO handler */
	struct no_os_gpio_desc *gpio_convst;
	/** RESET GPIO handler. */
	struct no_os_gpio_desc	*gpio_reset;
	/** ALERT GPIO handler. */
	struct no_os_gpio_desc	*gpio_alert;
	/* AD7091R specific device identifier */
	enum ad7091r8_device_id device_id;
	/* AD7091R device sleep mode */
	enum ad7091r8_sleep_mode sleep_mode;
};

struct ad7091r8_init_param {
	/* SPI initialization parameters */
	struct no_os_spi_init_param *spi_init;
	/* External Voltage Reference */
	int vref_mv;
	/* CONVST GPIO initialization parameters */
	struct no_os_gpio_init_param *gpio_convst;
	/* Reset GPIO initialization parameters */
	struct no_os_gpio_init_param *gpio_reset;
	/* Alert GPIO initialization parameters */
	struct no_os_gpio_init_param *gpio_alert;
	/* AD7091R specific device identifier */
	enum ad7091r8_device_id device_id;
	/* AD7091R device sleep mode */
	enum ad7091r8_sleep_mode sleep_mode;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Initialize the device. */
int ad7091r8_init(struct ad7091r8_dev **device,
		  struct ad7091r8_init_param *init_param);

/* Remove the device and release resources. */
int ad7091r8_remove(struct ad7091r8_dev *dev);

/* Set device sleep mode */
int ad7091r8_set_sleep_mode(struct ad7091r8_dev *dev,
			    enum ad7091r8_sleep_mode mode);

/* Set device set port value */
int ad7091r8_set_port(struct ad7091r8_dev *dev,
		      enum ad7091r8_port port,
		      bool value);

/* Set device set GPO0 mode */
int ad7091r8_set_gpo0_mode(struct ad7091r8_dev *dev,
			   enum ad7091r8_gpo0_mode mode,
			   bool is_cmos);

/* Set high limit, low limit, hysteresis. */
int ad7091r8_set_limit(struct ad7091r8_dev *dev,
		       enum ad7091r8_limit limit,
		       uint8_t channel,
		       uint16_t value);

/* Get alert. */
int ad7091r8_get_alert(struct ad7091r8_dev *dev,
		       uint8_t channel,
		       enum ad7091r8_alert_type *alert);

/* Get high limit, low limit, hysteresis. */
int ad7091r8_get_limit(struct ad7091r8_dev *dev,
		       enum ad7091r8_limit limit,
		       uint8_t channel,
		       uint16_t *value);

/* Select device channel. */
int ad7091r8_set_channel(struct ad7091r8_dev *dev,
			 uint8_t channel);

/* Read one sample. */
int ad7091r8_read_one(struct ad7091r8_dev *dev,
		      uint8_t chan,
		      uint16_t *read_val);

/* Read next channel set in the channel sequencer. */
int ad7091r8_sequenced_read(struct ad7091r8_dev *dev,
			    uint16_t *read_val);

/* Read device register. */
int ad7091r8_spi_reg_read(struct ad7091r8_dev *dev,
			  uint8_t reg_addr,
			  uint16_t *reg_data);

/* Write to device register. */
int ad7091r8_spi_reg_write(struct ad7091r8_dev *dev,
			   uint8_t reg_addr,
			   uint16_t reg_data);

/* SPI write to device using a mask. */
int ad7091r8_spi_write_mask(struct ad7091r8_dev* dev,
			    uint8_t reg_addr,
			    uint16_t mask,
			    uint16_t data);

int ad7091r8_pulse_convst(struct ad7091r8_dev *dev);

#endif // __AD7091R8_H__
