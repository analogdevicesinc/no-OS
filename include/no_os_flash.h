/***************************************************************************//**
 *   @file   no_os_flash.h
 *   @brief  Header file for flash controller driver.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _NO_OS_FLASH_H_
#define _NO_OS_FLASH_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct no_os_flash_dev
 * @brief Flash controller device structure
 */
struct no_os_flash_dev {
	/** Flash Device ID */
	uint8_t id;
	/** Size of flash memory in bytes */
	uint32_t flash_size;
	/** Size of flash page in bytes */
	uint32_t page_size;
	/** Flash extra parameters */
	void *extra;
};

/**
 * @struct no_os_flash_init_param
 * @brief Flash controller initialization structure
 */
struct no_os_flash_init_param {
	/** Flash Device ID */
	uint8_t id;
	/** Size of flash memory in bytes */
	uint32_t flash_size;
	/** Size of flash page in bytes */
	uint32_t flash_page_size;
	/** Flash extra parameters */
	void *extra;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/** Initialize flash controller. */
int32_t no_os_flash_init(struct no_os_flash_dev **device,
			 struct no_os_flash_init_param *init_param);

/** Free memory allocated by no_os_flash_init(). */
int32_t no_os_flash_remove(struct no_os_flash_dev *dev);

/** Erase a flash page. */
int32_t no_os_flash_clear_page(struct no_os_flash_dev *dev, int32_t page_no);

/** Write a flash page. */
int32_t no_os_flash_write_page(struct no_os_flash_dev *dev, int32_t page_no,
			       uint32_t *data);

/** Read a flash page. */
int32_t flash_read_page(struct no_os_flash_dev *dev, int32_t page_no,
			uint32_t *data);

/** Write data in flash memory. */
int32_t no_os_flash_write(struct no_os_flash_dev *dev, uint32_t flash_addr,
			  uint32_t *array, uint32_t array_size);

/** Read data from the flash memory. */
int32_t no_os_flash_read(struct no_os_flash_dev *dev, uint32_t flash_addr,
			 uint32_t *array,
			 uint32_t size);

#endif // _NO_OS_FLASH_H_
