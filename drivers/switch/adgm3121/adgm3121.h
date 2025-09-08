/***************************************************************************//**
 *   @file   adgm3121.h
 *   @brief  Header file of ADGM3121 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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
#ifndef ADGM3121_H_
#define ADGM3121_H_

#include <stdint.h>
#include <stdbool.h>
#include "no_os_spi.h"
#include "no_os_gpio.h"

/* ADGM3121 Register Addresses */
#define ADGM3121_REG_SWITCH_DATA	0x20

/* ADGM3121 Register Bits */
#define ADGM3121_SW1_EN			NO_OS_BIT(0)
#define ADGM3121_SW2_EN			NO_OS_BIT(1)
#define ADGM3121_SW3_EN			NO_OS_BIT(2)
#define ADGM3121_SW4_EN			NO_OS_BIT(3)
#define ADGM3121_INTERNAL_ERROR		NO_OS_GENMASK(7, 6)
#define ADGM3121_RESERVED		NO_OS_GENMASK(5, 4)

/* SPI Command Format */
#define ADGM3121_SPI_READ		NO_OS_BIT(15)
#define ADGM3121_SPI_WRITE		0
#define ADGM3121_SPI_ADDR_MSK		NO_OS_GENMASK(14, 8)
#define ADGM3121_SPI_DAISY_CHAIN_CMD	0x2500

/* Default timeout values */
#define ADGM3121_SWITCHING_TIME_US	200
#define ADGM3121_POWER_UP_TIME_MS	45

/**
 * @enum adgm3121_switch
 * @brief Switch enumeration (valid for both devices)
 */
enum adgm3121_switch {
	ADGM3121_SW1,
	ADGM3121_SW2,
	ADGM3121_SW3,
	ADGM3121_SW4,
};

/**
 * @enum adgm3121_state
 * @brief Switch state enumeration
 */
enum adgm3121_state {
	ADGM3121_DISABLE,
	ADGM3121_ENABLE,
};

/**
 * @enum adgm3121_mode
 * @brief Control mode enumeration
 */
enum adgm3121_mode {
	ADGM3121_MODE_PARALLEL,	/* GPIO parallel control mode */
	ADGM3121_MODE_SPI,	/* SPI control mode */
};

/**
 * @struct adgm3121_dev
 * @brief Device structure
 */
struct adgm3121_dev {
	/* SPI descriptor */
	struct no_os_spi_desc *spi_desc;
	/* GPIO descriptors for parallel mode */
	struct no_os_gpio_desc *gpio_in1;
	struct no_os_gpio_desc *gpio_in2;
	struct no_os_gpio_desc *gpio_in3;
	struct no_os_gpio_desc *gpio_in4;
	/* PIN/SPI mode select GPIO */
	struct no_os_gpio_desc *gpio_pin_spi;
	/* Control mode */
	enum adgm3121_mode mode;
	/* Current switch states cache */
	uint8_t switch_states;
	/* SPI daisy chain mode */
	bool daisy_chain_mode;
};

/**
 * @struct adgm3121_init_param
 * @brief Initialization parameters
 */
struct adgm3121_init_param {
	/* SPI initialization parameters */
	struct no_os_spi_init_param spi_init;
	/* GPIO initialization parameters for parallel mode */
	struct no_os_gpio_init_param gpio_in1;
	struct no_os_gpio_init_param gpio_in2;
	struct no_os_gpio_init_param gpio_in3;
	struct no_os_gpio_init_param gpio_in4;
	/* PIN/SPI mode select GPIO */
	struct no_os_gpio_init_param gpio_pin_spi;
	/* Control mode */
	enum adgm3121_mode mode;
};

/* Function prototypes */

/* Initialize the device */
int adgm3121_init(struct adgm3121_dev **device,
		  struct adgm3121_init_param *init_param);

/* Free resources allocated by adgm3121_init() */
int adgm3121_remove(struct adgm3121_dev *dev);

/* Set the state of a specific switch */
int adgm3121_set_switch_state(struct adgm3121_dev *dev,
			      enum adgm3121_switch sw,
			      enum adgm3121_state state);

/* Get the current state of a specific switch */
int adgm3121_get_switch_state(struct adgm3121_dev *dev,
			      enum adgm3121_switch sw,
			      enum adgm3121_state *state);

/* Set multiple switches at once using bitmask */
int adgm3121_set_switches(struct adgm3121_dev *dev, uint8_t switch_mask);

/* Get all switch states as bitmask */
int adgm3121_get_switches(struct adgm3121_dev *dev, uint8_t *switch_mask);

/* Reset all switches to off state */
int adgm3121_reset_switches(struct adgm3121_dev *dev);

/* Enter SPI daisy chain mode */
int adgm3121_enter_daisy_chain_mode(struct adgm3121_dev *dev);

/* Check for internal errors */
int adgm3121_check_internal_error(struct adgm3121_dev *dev,
				  uint8_t *error_status);

/* Write to device register (SPI mode only) */
int adgm3121_write_register(struct adgm3121_dev *dev, uint8_t reg_addr,
			    uint8_t data);

/* Read from device register (SPI mode only) */
int adgm3121_read_register(struct adgm3121_dev *dev, uint8_t reg_addr,
			   uint8_t *data);

#endif /* ADGM3121_H_ */