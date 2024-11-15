/***************************************************************************//**
 *   @file   no_os_mdio.h
 *   @brief  Header file for MDIO interface driver.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _NO_OS_MDIO_H_
#define _NO_OS_MDIO_H_

#include <stdbool.h>
#include <stdint.h>
#include "no_os_util.h"

#define NO_OS_MDIO_C22_REGS		32
#define NO_OS_MDIO_C45_START		0
#define NO_OS_MDIO_C22_START		1
#define NO_OS_MDIO_START_MASK		NO_OS_GENMASK(31, 30)
#define NO_OS_MDIO_OP_ADDRESS           0
#define NO_OS_MDIO_OP_WRITE		1
#define NO_OS_MDIO_OP_READ		2
#define NO_OS_MDIO_OP_MASK              NO_OS_GENMASK(29, 28)
#define NO_OS_MDIO_PHYADDR_MASK         NO_OS_GENMASK(27, 23)
#define NO_OS_MDIO_REGADDR_MASK         NO_OS_GENMASK(22, 18)
#define NO_OS_MDIO_TURNAROUND           2
#define NO_OS_MDIO_TURNAROUND_MASK      NO_OS_GENMASK(17, 16)
#define NO_OS_MDIO_DATA_MASK            NO_OS_GENMASK(15, 0)
#define NO_OS_MDIO_C45_DEVADDR_MASK     NO_OS_GENMASK(20, 16)
#define NO_OS_MDIO_C45_ADDR(dev, reg)   (NO_OS_BIT(31) | no_os_field_prep(NO_OS_MDIO_C45_DEVADDR_MASK, dev) | (uint16_t)reg)

/**
 * @struct no_os_mdio_init_param
 * @brief Parameters for an MDIO slave.
 */
struct no_os_mdio_init_param {
	/** Device ID (when using MDIO peripheral), optional. */
	int id;
	/** Specifies if clause 45 frame format is supported by the slave,
	 *  otherwise clause 22 frame format is used. */
	bool c45;
	/** MDIO slave address on the bus. */
	uint8_t addr;
	/** Specific implementations of the API. */
	struct no_os_mdio_ops *ops;
	/** Platform or implementation specific parameters. */
	void *extra;
};

/**
 * @struct no_os_mdio_desc
 * @brief MDIO device descriptor created with no_os_mdio_init().
 */
struct no_os_mdio_desc {
	int id;
	bool c45;
	uint8_t addr;
	struct no_os_mdio_ops *ops;
	void *extra;
};

/**
 * @struct no_os_mdio_ops
 * @brief Collection of MDIO ops that point to specific implementations.
 */
struct no_os_mdio_ops {
	/** MDIO initialization op */
	int (*init)(struct no_os_mdio_desc **, struct no_os_mdio_init_param *);
	/** MDIO write register op */
	int (*write)(struct no_os_mdio_desc *, uint32_t, uint16_t);
	/** MDIO read register op */
	int (*read)(struct no_os_mdio_desc *, uint32_t, uint16_t *);
	/** MDIO remove op */
	int (*remove)(struct no_os_mdio_desc *);
};

int no_os_mdio_init(struct no_os_mdio_desc **desc,
		    struct no_os_mdio_init_param *param);
int no_os_mdio_remove(struct no_os_mdio_desc *desc);
int no_os_mdio_write(struct no_os_mdio_desc *desc, uint32_t reg, uint16_t val);
int no_os_mdio_read(struct no_os_mdio_desc *desc, uint32_t reg, uint16_t *val);

#endif
