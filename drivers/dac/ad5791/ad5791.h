/***************************************************************************//**
 *   @file   AD5791.h
 *   @brief  Header file of AD5791 Driver.
 *   @author DNechita (Dan.Nechita@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
 *
*******************************************************************************/
#ifndef __AD5791_H__
#define __AD5791_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_util.h"
#include "no_os_error.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
/* Supported devices */
enum ad5791_type {
	ID_AD5760,
	ID_AD5780,
	ID_AD5781,
	ID_AD5790,
	ID_AD5791
};

/* Possible voltage spans for linearity error compensation */
enum ad5791_lin_comp_select {
	AD5781_SPAN_UPTO_10V = 0,
	AD5781_SPAN_10V_TO_20V = 4,
	AD5791_SPAN_10V_TO_12V = 1,
	AD5791_SPAN_12V_TO_16V = 2,
	AD5791_SPAN_16V_TO_19V = 3,
	AD5791_SPAN_19V_TO_20V = 4
};

struct ad5791_chip_info {
	uint32_t resolution;
};

struct ad5791_dev {
	/* SPI */
	struct no_os_spi_desc	*spi_desc;
	/* GPIO */
	struct no_os_gpio_desc	*gpio_reset;
	struct no_os_gpio_desc	*gpio_clr;
	struct no_os_gpio_desc	*gpio_ldac;
	/* Device Settings */
	enum ad5791_type act_device;
};

struct ad5791_init_param {
	/* SPI */
	struct no_os_spi_init_param	spi_init;
	/* GPIO */
	struct no_os_gpio_init_param		gpio_reset;
	struct no_os_gpio_init_param		gpio_clr;
	struct no_os_gpio_init_param		gpio_ldac;
	/* Device Settings */
	enum ad5791_type act_device;
};

/******************************************************************************/
/*********************************** GPIO *************************************/
/******************************************************************************/
#define AD5791_RESET_OUT       no_os_gpio_direction_output(dev->gpio_reset, \
			       NO_OS_GPIO_HIGH);
#define AD5791_RESET_LOW       no_os_gpio_set_value(dev->gpio_reset,        \
			       NO_OS_GPIO_LOW)
#define AD5791_RESET_HIGH      no_os_gpio_set_value(dev->gpio_reset,        \
			       NO_OS_GPIO_HIGH)

#define AD5791_CLR_OUT         no_os_gpio_direction_output(dev->gpio_clr,  \
			       NO_OS_GPIO_HIGH);
#define AD5791_CLR_LOW         no_os_gpio_set_value(dev->gpio_clr,         \
			       NO_OS_GPIO_LOW)
#define AD5791_CLR_HIGH        no_os_gpio_set_value(dev->gpio_clr,         \
			       NO_OS_GPIO_HIGH)

#define AD5791_LDAC_OUT        no_os_gpio_direction_output(dev->gpio_ldac, \
			       NO_OS_GPIO_HIGH);
#define AD5791_LDAC_LOW        no_os_gpio_set_value(dev->gpio_ldac,        \
			       NO_OS_GPIO_LOW)
#define AD5791_LDAC_HIGH       no_os_gpio_set_value(dev->gpio_ldac,        \
			       NO_OS_GPIO_HIGH)

/******************************************************************************/
/********************************** AD5791 ************************************/
/******************************************************************************/

/* Maximum resolution */
#define MAX_RESOLUTION          20

/* Register Map */
#define AD5791_NOP                 0  // No operation (NOP).
#define AD5791_REG_DAC             1  // DAC register.
#define AD5791_REG_CTRL            2  // Control register.
#define AD5791_REG_CLR_CODE        3  // Clearcode register.
#define AD5791_CMD_WR_SOFT_CTRL    4  // Software control register(Write only).

/* Input Shift Register bit definition. */
#define AD5791_READ                (1ul << 23)
#define AD5791_WRITE               (0ul << 23)
#define AD5791_ADDR_REG(x)         (((uint32_t)(x) & 0x7) << 20)

/* Control Register bit Definition */
#define AD5791_CTRL_LINCOMP_MASK   NO_OS_GENMASK(9,6) // Linearity error compensation.
#define AD5791_CTRL_LINCOMP(x)     (((x) & 0xF) << 6)
#define AD5791_CTRL_SDODIS_MASK    NO_OS_BIT(5)       // SDO pin enable/disable control.
#define AD5791_CTRL_SDODIS(x)      ((x & 0x01) << 5)
#define AD5791_CTRL_BIN2SC_MASK    NO_OS_BIT(4)       // DAC register coding selection.
#define AD5791_CTRL_BIN2SC(x)      ((x & 0x01) << 4)
#define AD5791_CTRL_DACTRI         NO_OS_BIT(3)       // DAC tristate control.
#define AD5791_CTRL_OPGND          NO_OS_BIT(2)       // Output ground clamp control.
#define AD5791_CTRL_RBUF_MASK      NO_OS_BIT(1)       // Output amplifier configuration control.
#define AD5791_CTRL_RBUF(x)        ((x & 0x01) << 1)

/* Software Control Register bit definition */
#define AD5791_SOFT_CTRL_RESET      (1 << 2) // RESET function.
#define AD5791_SOFT_CTRL_CLR        (1 << 1) // CLR function.
#define AD5791_SOFT_CTRL_LDAC       (1 << 0) // LDAC function.

/* DAC OUTPUT STATES */
#define AD5791_OUT_NORMAL            0x0
#define AD5791_OUT_CLAMPED_6K        0x1
#define AD5791_OUT_TRISTATE          0x2

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Initializes the communication with the device. */
int32_t ad5791_init(struct ad5791_dev **device,
		    struct ad5791_init_param init_param);

/*! Free the resources allocated by ad5791_init(). */
int32_t ad5791_remove(struct ad5791_dev *dev);

/*! Writes data into a register. */
int32_t ad5791_set_register_value(struct ad5791_dev *dev,
				  uint8_t register_address,
				  uint32_t register_value);

/*! Reads the value of a register. */
int32_t ad5791_get_register_value(struct ad5791_dev *dev,
				  uint8_t register_address,
				  uint32_t *value);

/*! Sets the DAC output in one of the three states. */
int32_t ad5791_dac_ouput_state(struct ad5791_dev *dev,
			       uint8_t state);

/*! Writes to the DAC register. */
int32_t ad5791_set_dac_value(struct ad5791_dev *dev,
			     uint32_t value);

/*! Asserts RESET, CLR or LDAC in a software manner. */
int32_t ad5791_soft_instruction(struct ad5791_dev *dev,
				uint8_t instruction_bit);

/*! Configures the output amplifier, DAC coding, SDO state and the linearity
    error compensation. */
int32_t ad5791_setup(struct ad5791_dev *dev,
		     uint32_t setup_word);

/*! SPI write to device using a mask. */
int ad5791_spi_write_mask(struct ad5791_dev *dev,
			  uint8_t register_address,
			  uint32_t mask,
			  uint32_t value);

/*! Set Linearity error compensation based on the reference voltage span. */
int ad5791_set_lin_comp(struct ad5791_dev *dev,
			enum ad5791_lin_comp_select v_span);

/*! Trigger LDAC. */
int ad5791_ldac_trigger(struct ad5791_dev *dev);

/*! Clear DAC channel output with the clearcode. */
int ad5791_clear_async(struct ad5791_dev *dev);

#endif /* __AD5791_H__ */
