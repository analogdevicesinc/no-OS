/***************************************************************************//**
*   @file   ad9833.h
*   @brief  Header file of AD9833 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
********************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef _AD9833_H_
#define _AD9833_H_

/******************************************************************************/
/******************************* Include Files ********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"

/******************************************************************************/
/********************* Macros and Constants Definitions ***********************/
/******************************************************************************/

#define AD9833_CTRLB28          (1 << 13)
#define AD9833_CTRLHLB          (1 << 12)
#define AD9833_CTRLFSEL         (1 << 11)
#define AD9833_CTRLPSEL         (1 << 10)
#define AD9834_CTRLPINSW        (1 << 9)
#define AD9833_CTRLRESET        (1 << 8)
#define AD9833_CTRLSLEEP1       (1 << 7)
#define AD9833_CTRLSLEEP12      (1 << 6)
#define AD9833_CTRLOPBITEN      (1 << 5)
#define AD9834_CTRLSIGNPIB      (1 << 4)
#define AD9833_CTRLDIV2         (1 << 3)
#define AD9833_CTRLMODE         (1 << 1)

/* GPIOs */
#define AD9834_PSEL_OUT         no_os_gpio_direction_output(dev->gpio_psel,  \
			        NO_OS_GPIO_HIGH)
#define AD9834_PSEL_LOW         no_os_gpio_set_value(dev->gpio_psel,         \
			        NO_OS_GPIO_LOW)
#define AD9834_PSEL_HIGH        no_os_gpio_set_value(dev->gpio_psel,         \
			        NO_OS_GPIO_HIGH)

#define AD9834_FSEL_OUT         no_os_gpio_direction_output(dev->gpio_fsel,  \
			        NO_OS_GPIO_HIGH)
#define AD9834_FSEL_LOW         no_os_gpio_set_value(dev->gpio_fsel,         \
			        NO_OS_GPIO_LOW)
#define AD9834_FSEL_HIGH        no_os_gpio_set_value(dev->gpio_fsel,         \
			        NO_OS_GPIO_HIGH)

#define AD9834_RESET_OUT        no_os_gpio_direction_output(dev->gpio_reset, \
			        NO_OS_GPIO_HIGH)
#define AD9834_RESET_LOW        no_os_gpio_set_value(dev->gpio_reset,        \
			        NO_OS_GPIO_LOW)
#define AD9834_RESET_HIGH       pio_set_value(dev->gpio_reset,         \
			        NO_OS_GPIO_HIGH)

#define AD9834_SLEEP_OUT        no_os_gpio_direction_output(dev->gpio_sleep, \
			        NO_OS_GPIO_HIGH)
#define AD9834_SLEEP_LOW        no_os_gpio_set_value(dev->gpio_sleep,        \
			        NO_OS_GPIO_LOW)
#define AD9834_SLEEP_HIGH       no_os_gpio_set_value(dev->gpio_sleep,         \
			        NO_OS_GPIO_HIGH)


#define BIT_F0ADDRESS           0x4000      // Frequency Register 0 address.
#define BIT_F1ADDRESS           0x8000      // Frequency Register 1 address.
#define BIT_P0ADDRESS           0xC000      // Phase Register 0 address.
#define BIT_P1ADDRESS           0xE000      // Phase Register 1 address.

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/* Supported devices */
enum ad9833_type {
	ID_AD9833,
	ID_AD9834,
	ID_AD9837,
	ID_AD9838,
};

struct ad9833_dev {
	/* SPI */
	struct no_os_spi_desc		*spi_desc;
	/* GPIO */
	struct no_os_gpio_desc		*gpio_psel;
	struct no_os_gpio_desc		*gpio_fsel;
	struct no_os_gpio_desc		*gpio_reset;
	struct no_os_gpio_desc		*gpio_sleep;
	/* Device Settings */
	enum ad9833_type		act_device;
	uint8_t				prog_method;
	uint16_t			ctrl_reg_value;
	uint16_t			test_opbiten;
};

struct ad9833_init_param {
	/* SPI */
	struct no_os_spi_init_param		spi_init;
	/* GPIO */
	struct no_os_gpio_init_param		gpio_psel;
	struct no_os_gpio_init_param		gpio_fsel;
	struct no_os_gpio_init_param		gpio_reset;
	struct no_os_gpio_init_param		gpio_sleep;
	/* Device Settings */
	enum ad9833_type		act_device;
};

struct ad9833_chip_info {
	uint32_t	mclk;
	float		freq_const;
};

/******************************************************************************/
/************************** Functions Declarations ****************************/
/******************************************************************************/
/* Initialize the SPI communication with the device. */
int8_t ad9833_init(struct ad9833_dev **device,
		   struct ad9833_init_param init_param);
/* Free the resources allocated by adf7023_init(). */
int32_t ad9833_remove(struct ad9833_dev *dev);
/* Transmits 16 bits on SPI. */
void ad9833_tx_spi(struct ad9833_dev *dev,
		   int16_t value);
/* Selects the type of output. */
int8_t ad9833_out_mode(struct ad9833_dev *dev,
		       uint8_t v_out_mode);
/* Loads a frequency value in a register. */
void ad9833_set_freq(struct ad9833_dev *dev,
		     uint8_t register_number,
		     uint32_t frequency_value);
/* Loads a phase value in a register. */
void ad9833_set_phase(struct ad9833_dev *dev,
		      uint8_t register_number,
		      float phase_value);
/* Select the frequency register to be used. */
void ad9833_select_freq_reg(struct ad9833_dev *dev,
			    uint8_t freq_reg);
/* Select the phase register to be used. */
void ad9833_select_phase_reg(struct ad9833_dev *dev,
			     uint8_t phase_reg);
/* Enable / Disable the sleep function. */
void ad9833_sleep_mode(struct ad9833_dev *dev,
		       uint8_t sleep_mode);

void ad9834_select_prog_method(struct ad9833_dev *dev,
			       uint8_t value);

void ad9834_logic_output(struct ad9833_dev *dev,
			 uint8_t opbiten,
			 uint8_t signpib,
			 uint8_t div2);

#endif  /* _AD9833_H_ */
