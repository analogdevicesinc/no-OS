/***************************************************************************//**
 *   @file   no_os_mdio.h
 *   @brief  Header file for MDIO interface driver.
 *   @author Darius Berghe (darius.berghe@analog.com)
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
