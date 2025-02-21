/***************************************************************************//**
 *   @file   no_os_flash.h
 *   @brief  Header file for flash controller driver.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
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

#ifndef _NO_OS_FLASH_H_
#define _NO_OS_FLASH_H_

#include <stdint.h>

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
