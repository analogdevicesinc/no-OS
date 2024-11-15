/***************************************************************************//**
 *   @file   max149x6-base.h
 *   @brief  Header file of MAX149X6 Base Driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _MAX149X6_H
#define _MAX149X6_H

#include <stdint.h>
#include <stdbool.h>
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_util.h"

/* Common Frame Size */
#define MAX149X6_FRAME_SIZE		2

/* Common Registers */
#define MAX149X6_CHIP_ADDR_MASK		NO_OS_GENMASK(7, 6)
#define MAX149X6_ADDR_MASK		NO_OS_GENMASK(4, 1)
#define MAX149X6_RW_MASK		NO_OS_BIT(0)

/**
 * @brief Initialization parameter for the MAX149X6 device.
 */
struct max149x6_init_param {
	uint32_t chip_address;
	struct no_os_spi_init_param *comm_param;
	struct no_os_gpio_init_param *en_gpio_param;
	struct no_os_gpio_init_param *fault_gpio_param;
	struct no_os_gpio_init_param *ready_gpio_param;
	struct no_os_gpio_init_param *synch_gpio_param;
	bool crc_en;
};

/**
 * @brief Device descriptor for MAX149X6.
 */
struct max149x6_desc {
	uint32_t chip_address;
	struct no_os_spi_desc *comm_desc;
	struct no_os_gpio_desc *en_gpio;
	struct no_os_gpio_desc *fault_gpio;
	struct no_os_gpio_desc *ready_gpio;
	struct no_os_gpio_desc *synch_gpio;
	uint8_t buff[MAX149X6_FRAME_SIZE + 1];
	bool crc_en;
};

/** Write the value of a device register */
int max149x6_reg_write(struct max149x6_desc *, uint32_t, uint32_t);

/** Read the value of a device register */
int max149x6_reg_read(struct max149x6_desc *, uint32_t, uint32_t *);

/** Update the value of a device register */
int max149x6_reg_update(struct max149x6_desc *, uint32_t, uint32_t, uint32_t);

#endif
