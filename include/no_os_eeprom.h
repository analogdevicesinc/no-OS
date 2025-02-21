/***************************************************************************//**
 *   @file   no_os_eeprom.h
 *   @brief  Header file for EEPROM APIs
 *   @author Mahesh Phalke (mahesh.phalke@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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

#ifndef _NO_OS_EEPROM_H_
#define _NO_OS_EEPROM_H_

#include <stdint.h>


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
