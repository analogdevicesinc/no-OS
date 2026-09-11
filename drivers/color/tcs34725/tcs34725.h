/***************************************************************************//**
 *   @file   tcs34725.h
 *   @brief  Header file of TCS34725 Driver.
 *   @author Monica Constandachi (monica.constandachi@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __TCS34725_H__
#define __TCS34725_H__

#include <stdint.h>
#include "no_os_i2c.h"
#include "no_os_util.h"

#define TCS34725_COMMAND_BIT	0x80
#define TCS34725_COMMAND_INC	0x20
#define TCS34725_ENABLE			0x00
#define TCS34725_ENABLE_PON		NO_OS_BIT(0)
#define TCS34725_ENABLE_AEN		NO_OS_BIT(1)
#define TCS34725_ATIME			0x01
#define TCS34725_WTIME			0x03
#define TCS34725_AILTL			0x04
#define TCS34725_AILTH			0x05
#define TCS34725_AIHTL			0x06
#define TCS34725_AIHTH			0x07
#define TCS34725_PERS			0x0C
#define TCS34725_CONFIG			0x0D
#define TCS34725_CONTROL		0x0F
#define TCS34725_ID				0x12
#define TCS34725_STATUS			0x13
#define TCS34725_CDATAL			0x14
#define TCS34725_CDATAH			0x15
#define TCS34725_RDATAL			0x16
#define TCS34725_RDATAH			0x17
#define TCS34725_GDATAL			0x18
#define TCS34725_GDATAH			0x19
#define TCS34725_BDATAL			0x1A
#define TCS34725_BDATAH			0x1B

/**
 * @brief Initialization parameter for the device descriptor
 */

struct tcs34725_init_param {
	struct no_os_i2c_init_param *comm_param; /**< I2C init parameters */
};

/**
 * @brief Device descriptor
 */
struct tcs34725_desc {
	struct no_os_i2c_desc *comm_desc; /**< I2C descriptor */
};

/** Write a register value */
int tcs34725_reg_write(struct tcs34725_desc *desc, uint8_t addr, uint8_t val);

/** Read a register value */
int tcs34725_reg_read(struct tcs34725_desc *desc, uint8_t addr, uint8_t *val);

/** Read the current RGBC sample (clear, red, green, blue). */
int tcs34725_get_rgbc(struct tcs34725_desc *desc, uint16_t *r, uint16_t *g,
		      uint16_t *b, uint16_t *c);

/** Initialize the device structure */
int tcs34725_init(struct tcs34725_desc **desc,
		  struct tcs34725_init_param *param);

/** Free the device descriptor */
int tcs34725_remove(struct tcs34725_desc *desc);

#endif /* __TCS34725_H__ */
