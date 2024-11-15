/***************************************************************************//**
 *   @file   no_os_eeprom.h
 *   @brief  Header file for EEPROM APIs
 *   @author Mahesh Phalke (mahesh.phalke@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _NO_OS_EEPROM_H_
#define _NO_OS_EEPROM_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/* Forward declaration of structure */
struct no_os_eeprom_platform_ops;

/**
 * @struct no_os_eeprom_init_param
 * @brief Structure holding the parameters for EEPROM initialization
 */
struct no_os_eeprom_init_param {
	/** Device ID */
	uint32_t device_id;
	const struct no_os_eeprom_platform_ops *platform_ops;
	/** EEPROM extra parameters (device specific) */
	void *extra;
};

/**
 * @struct no_os_eeprom_desc
 * @brief Structure holding the EEPROM descriptor
 */
struct no_os_eeprom_desc {
	/** Device ID */
	uint32_t device_id;
	const struct no_os_eeprom_platform_ops *platform_ops;
	/** EEPROM extra parameters (device specific) */
	void *extra;
};

/**
 * @struct no_os_eeprom_platform_ops
 * @brief Structure holding the EEPROM function pointers that point to the
 * platform specific function
 */
struct no_os_eeprom_platform_ops {
	/** EEPROM initialization function pointer */
	int32_t (*init)(struct no_os_eeprom_desc **,
			const struct no_os_eeprom_init_param *);
	/** EEPROM data write function pointer */
	int32_t (*write)(struct no_os_eeprom_desc *, uint32_t, uint8_t *, uint16_t);
	/** EEPROM data read function pointer */
	int32_t (*read)(struct no_os_eeprom_desc *, uint32_t, uint8_t *, uint16_t);
	/** EEPROM remove function pointer */
	int32_t (*remove)(struct no_os_eeprom_desc *);
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Initialize the EEPROM */
int32_t no_os_eeprom_init(struct no_os_eeprom_desc **desc,
			  const struct no_os_eeprom_init_param *param);

/* Write into EEPROM */
int32_t no_os_eeprom_write(struct no_os_eeprom_desc *desc, uint32_t address,
			   uint8_t *data, uint16_t bytes);

/* Read from EEPROM */
int32_t no_os_eeprom_read(struct no_os_eeprom_desc *desc, uint32_t address,
			  uint8_t *data, uint16_t bytes);

/* Free the resources allocated by no_os_eeprom_init() */
int32_t no_os_eeprom_remove(struct no_os_eeprom_desc *desc);

#endif // _NO_OS_EEPROM_H_
