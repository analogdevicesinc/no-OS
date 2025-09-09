/***************************************************************************//**
 *   @file   adgs6414d.h
 *   @brief  Header file for ADGS6414D Driver.
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
#ifndef ADGS6414D_H_
#define ADGS6414D_H_

#include <stdint.h>
#include <stdbool.h>
#include "no_os_spi.h"

#define ADGS6414D_NUM_SWITCHES	8

/* ADGS6414D Register Map */
#define ADGS6414D_REG_SW_DATA		0x01
#define ADGS6414D_REG_ERR_FLAGS		0x03
#define ADGS6414D_REG_BURST_EN		0x05
#define ADGS6414D_REG_SOFT_RESETB	0x0B

/* SPI Command Format */
#define ADGS6414D_CMD_WRITE		0x00
#define ADGS6414D_CMD_READ		0x80

/* Soft Reset Values */
#define ADGS6414D_SOFT_RESET_VAL1	0xA3
#define ADGS6414D_SOFT_RESET_VAL2	0x05

/* ADGS6414D Switch Channels */
#define ADGS6414D_SW1		0
#define ADGS6414D_SW2		1
#define ADGS6414D_SW3		2
#define ADGS6414D_SW4		3
#define ADGS6414D_SW5		4
#define ADGS6414D_SW6		5
#define ADGS6414D_SW7		6
#define ADGS6414D_SW8		7

/**
 * @struct adgs6414d_dev
 * @brief ADGS6414D Device Descriptor.
 */
struct adgs6414d_dev {
	struct no_os_spi_desc	*spi_desc;
	uint8_t			switch_state;
};

/**
 * @struct adgs6414d_init_param
 * @brief ADGS6414D Initialization Parameters structure.
 */
struct adgs6414d_init_param {
	struct no_os_spi_init_param	*spi_init;
	uint8_t				initial_state;
};

/* SPI write operation. */
int adgs6414d_spi_write(struct adgs6414d_dev *dev, uint8_t reg_addr,
			uint8_t data);

/* SPI read operation. */
int adgs6414d_spi_read(struct adgs6414d_dev *dev, uint8_t reg_addr,
		       uint8_t *data);

/* Perform software reset. */
int adgs6414d_soft_reset(struct adgs6414d_dev *dev);

/* Set switch state for a specific channel. */
int adgs6414d_set_switch(struct adgs6414d_dev *dev, uint8_t channel,
			 bool state);

/* Get switch state for a specific channel. */
int adgs6414d_get_switch(struct adgs6414d_dev *dev, uint8_t channel,
			 bool *state);

/* Set multiple switches at once using bitmask. */
int adgs6414d_set_switches(struct adgs6414d_dev *dev, uint8_t mask);

/* Get all switch states as bitmask. */
int adgs6414d_get_switches(struct adgs6414d_dev *dev, uint8_t *mask);

/* Initialize the device. */
int adgs6414d_init(struct adgs6414d_dev **device,
		   struct adgs6414d_init_param *init_param);

/* Free the resources allocated by adgs6414d_init(). */
int adgs6414d_remove(struct adgs6414d_dev *dev);

#endif /* ADGS6414D_H_ */
