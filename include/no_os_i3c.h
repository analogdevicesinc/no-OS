/***************************************************************************//**
 *   @file   no_os_i3c.h
 *   @brief  Header file of I3C Interface
 *   @author @author Jorge Marques (jorge.marques@analog.com)
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

#ifndef _NO_OS_I3C_H_
#define _NO_OS_I3C_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include "no_os_util.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define I3C_MAX_BUS_NUMBER 4
#define I2C_MAX_ADDR NO_OS_GENMASK(6, 0)
// TODO or if non constant GENMASK implementation
//#define I2C_MAX_ADDR 0x7f
#define I3C_MAX_ADDR I2C_MAX_ADDR
#define I3C_BROADCAST_ADDR		0x7e

/**
 * enum i3c_addr_slot_status - I3C address slot status
 * @I3C_ADDR_SLOT_FREE: address is free
 * @I3C_ADDR_SLOT_RSVD: address is reserved
 * @I3C_ADDR_SLOT_I2C_DEV: address is assigned to an I2C device
 * @I3C_ADDR_SLOT_I3C_DEV: address is assigned to an I3C device
 * @I3C_ADDR_SLOT_STATUS_MASK: address slot mask
 *
 * On an I3C bus, addresses are assigned dynamically, and we need to know which
 * addresses are free to use and which ones are already assigned.
 *
 * Addresses marked as reserved are those reserved by the I3C protocol
 * (broadcast address, ...).
 */
enum i3c_addr_slot_status {
	I3C_ADDR_SLOT_FREE,
	I3C_ADDR_SLOT_RSVD,
	I3C_ADDR_SLOT_I2C_DEV,
	I3C_ADDR_SLOT_I3C_DEV,
	I3C_ADDR_SLOT_STATUS_MASK = 3,
};

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct no_os_i3c_platform_ops
 * @brief Structure holding I3C function pointers that point to the platform
 * specific function
 */
struct no_os_i3c_platform_ops ;

/**
 * @struct no_os_init_param*
 * @brief Structure holding the pointers to the peripherals parameters for I3C
 * DAA and general management.
 */
extern const struct no_os_i3c_init_param *i3c_devs_param[];
/**
 * @struct no_os_init_param*
 * @brief Structure holding the pointers to the peripherals for I3C.
 */
extern struct no_os_i3c_slave_desc *i3c_devs_desc[];
/**
 * @const int length_i3c_devs
 * @brief Stores the length of i3c_devs at compile time.
 */
extern const int length_i3c_devs;
extern struct no_os_i3c_slave_desc **i3c_devs_stop_addr;

/**
 * @struct no_os_i3c_master_desc
 * @addrslots: a bitmap with 2-bits per-slot to encode the address status and
 *	       ease the DAA (Dynamic Address Assignment) procedure (see
 *	       &enum i3c_addr_slot_status)
 * @brief Structure holding I3C master descriptor
 */
struct no_os_i3c_master_desc {
	/** I3C master mutex(lock)*/
	void*	mutex;
	/** slave number*/
	//uint8_t         slave_number;
	/** Device ID */
	uint8_t		device_id;
	/** Provisioned ID */
	//uint32_t	pid;
	/** Dynamic or static address */
	//uint8_t		address;
	/** Encoded devices address status */
	uint32_t addrslots[((I2C_MAX_ADDR + 1) * 2) / NO_OS_BITS_PER_LONG*8];
	/** I3C platform specific functions */
	const struct no_os_i3c_platform_ops *platform_ops;
	/** I3C master extra parameters (device specific parameters, same pointer to all peripherals and controller in the same bus) */
	void		*extra;
};

/**
 * @struct no_os_i3c_init_param
 * @brief Structure holding the parameters for I3C initialization.
 * Works like a device tree, the values are copied to no_os_i3c_slave_desc
 * during I3C initialization.
 * Device ID is the I3C controller that the peripheral is attached to.
 * Static addresses have higher precedence during the I3C controller
 * initialization, and the dynamic_addr field will be ignored.
 * To not use the static_addr field, set it to 0x0.
 */
struct no_os_i3c_init_param {
	/** Device ID */
	uint32_t	device_id;
	/** Provisioned ID */
	uint32_t	pid;
	/** Dynamic address */
	uint8_t		dynamic_addr;
	/** Static address */
	uint8_t		static_addr;
	/** Is I3C or I2C */
	uint8_t		is_i3c;
};

/**
 * @struct no_os_i3c_slave_desc
 * @brief Structure holding I3C slave descriptor
 */
struct no_os_i3c_slave_desc {
	/** Device ID */
	uint8_t		device_id;
	/** Provisioned ID */
	uint32_t	pid;
	/** BCR and DCR */
	uint16_t	bcr_dcr;
	/** Dynamic or static address (see is_static flag)*/
	uint8_t		addr;
	/** I3C maximum transfer speed supported */
	uint32_t	max_speed_hz;
	/** Is I3C or I2C? */
	uint8_t		is_i3c;
	/** Is attached? */
	uint8_t		is_attached;
	/** Is the address static? */
	uint8_t		is_static;
	/** I3C platform specific functions */
	const struct no_os_i3c_platform_ops *platform_ops;
	/** I3C controller */
	struct no_os_i3c_master_desc *controller;
};

/**
 * @struct no_os_i3c_platform_ops
 * @brief Structure holding i3c function pointers that point to the platform
 * specific function
 */
struct no_os_i3c_platform_ops {
	/** i3c initialization function pointer */
	int32_t (*i3c_ops_init)(struct no_os_i3c_master_desc **, const struct no_os_i3c_platform_ops *, uint8_t);
	/** i3c do daa function pointer */
	int32_t (*i3c_ops_do_daa)(struct no_os_i3c_master_desc *);
	/** i3c send ccc function pointer */
	int32_t (*i3c_ops_send_ccc)(struct no_os_i3c_slave_desc *, uint8_t *, uint8_t);
	/** i3c private write function pointer */
	int32_t (*i3c_ops_priv_write)(struct no_os_i3c_slave_desc *, uint8_t *, uint8_t);
	/** i3c private read function pointer */
	int32_t (*i3c_ops_priv_read)(struct no_os_i3c_slave_desc *, uint8_t *, uint8_t);
	/** i3c remove function pointer */
	int32_t (*i3c_ops_remove)(struct no_os_i3c_master_desc *);
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Initialize the I3C communication peripheral. */
int32_t no_os_i3c_init(struct no_os_i3c_master_desc **desc,
			const struct no_os_i3c_platform_ops *ops,
			uint8_t device_id);

/* Allocate an attached slave to its driver. */
int32_t no_os_i3c_init_driver(struct no_os_i3c_slave_desc **desc,
		       const struct no_os_i3c_init_param *param);

/* Do I3C DAA to assign the dynamic addresses. */
int32_t no_os_i3c_do_daa(struct no_os_i3c_master_desc *desc);

/* Free the resources allocated by no_os_i3c_init(). */
int32_t no_os_i3c_remove(struct no_os_i3c_master_desc *desc);

/* Free the resources allocated by no_os_i3c_init_driver(). */
int32_t no_os_i3c_remove_driver(struct no_os_i3c_slave_desc **desc);

/* Send CCC to a slave device. */
int32_t no_os_i3c_send_ccc(struct no_os_i3c_slave_desc *desc,
			uint8_t *data,
			uint8_t bytes_number);

/* Write data to a slave device. */
int32_t no_os_i3c_priv_write(struct no_os_i3c_slave_desc *desc,
			uint8_t *data,
			uint8_t bytes_number);

/* Read data from a slave device. */
int32_t no_os_i3c_priv_read(struct no_os_i3c_slave_desc *desc,
		       uint8_t *data,
		       uint8_t bytes_number);

/* Get status from address. */
//static enum i3c_addr_slot_status
//no_os_i3c_addr_get_status(struct no_os_i3c_master_desc *desc,
//		       uint8_t addr);

/* Set status of a address. */
void no_os_i3c_addr_set_status(struct no_os_i3c_master_desc *desc,
		       uint8_t addr, enum i3c_addr_slot_status status);

/* Check if address is available. */
//static bool no_os_i3c_addr_is_avail(struct no_os_i3c_master_desc *desc,
//		       uint8_t addr);

/* Get a free address. */
int32_t no_os_i3c_addr_get_free(struct no_os_i3c_master_desc *desc,
		       uint8_t start_addr);

/* Init the address slots. */
//static void no_os_i3c_addr_init(struct no_os_i3c_master_desc *desc);

///* . */
//int32_t no_os_i3c_(struct no_os_i3c_master_desc *desc,
//		       uint16_t addr);
#endif // _NO_OS_I3C_H_
