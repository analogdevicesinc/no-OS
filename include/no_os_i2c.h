/***************************************************************************//**
 *   @file   no_os_i2c.h
 *   @brief  Header file of I2C Interface
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _NO_OS_I2C_H_
#define _NO_OS_I2C_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define I2C_MAX_BUS_NUMBER 4

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct no_os_i2c_platform_ops
 * @brief Structure holding I2C function pointers that point to the platform
 * specific function
 */
struct no_os_i2c_platform_ops ;

/**
 * @struct no_os_i2c_init_param
 * @brief Structure holding the parameters for I2C initialization.
 */
struct no_os_i2c_init_param {
	/** Device ID */
	uint32_t	device_id;
	/** I2C maximum transfer speed supported */
	uint32_t	max_speed_hz;
	/** Slave address */
	uint8_t		slave_address;
	/** I2C platform specific functions */
	const struct no_os_i2c_platform_ops *platform_ops;
	/** I2C extra parameters (device specific parameters) */
	void		*extra;
};

/**
 * @struct no_os_i2cbus_desc
 * @brief Structure holding I2C bus descriptor
 */
struct no_os_i2cbus_desc {
	/** I2C bus mutex(lock)*/
	void* mutex;
	/** I2C bus slave number*/
	uint8_t         slave_number;
	/** I2C bus Device ID */
	uint32_t	device_id;
	/** I2C bus maximum transfer speed supported */
	uint32_t	max_speed_hz;
	/** I2C bus platform specific functions */
	const struct no_os_i2c_platform_ops *platform_ops;
	/** I2C bus extra parameters (device specific parameters) */
	void		*extra;
};


/**
 * @struct no_os_i2c_desc
 * @brief Structure holding I2C address descriptor
 */
struct no_os_i2c_desc {
	/** I2C bus address*/
	struct no_os_i2cbus_desc *bus;
	/** Device ID */
	uint32_t	device_id;
	/** I2C maximum transfer speed supported */
	uint32_t	max_speed_hz;
	/** Slave address */
	uint8_t		slave_address;
	/** I2C platform specific functions */
	const struct no_os_i2c_platform_ops *platform_ops;
	/** I2C extra parameters (device specific parameters) */
	void		*extra;
};

/**
 * @struct no_os_i2c_platform_ops
 * @brief Structure holding i2c function pointers that point to the platform
 * specific function
 */
struct no_os_i2c_platform_ops {
	/** i2c initialization function pointer */
	int32_t (*i2c_ops_init)(struct no_os_i2c_desc **,
				const struct no_os_i2c_init_param *);
	/** i2c write function pointer */
	int32_t (*i2c_ops_write)(struct no_os_i2c_desc *, uint8_t *, uint8_t, uint8_t);
	/** i2c write function pointer */
	int32_t (*i2c_ops_read)(struct no_os_i2c_desc *, uint8_t *, uint8_t, uint8_t);
	/** i2c remove function pointer */
	int32_t (*i2c_ops_remove)(struct no_os_i2c_desc *);
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Initialize the I2C communication peripheral. */
int32_t no_os_i2c_init(struct no_os_i2c_desc **desc,
		       const struct no_os_i2c_init_param *param);

/* Free the resources allocated by no_os_i2c_init(). */
int32_t no_os_i2c_remove(struct no_os_i2c_desc *desc);

/* Write data to a slave device. */
int32_t no_os_i2c_write(struct no_os_i2c_desc *desc,
			uint8_t *data,
			uint8_t bytes_number,
			uint8_t stop_bit);

/* Read data from a slave device. */
int32_t no_os_i2c_read(struct no_os_i2c_desc *desc,
		       uint8_t *data,
		       uint8_t bytes_number,
		       uint8_t stop_bit);

/* Initialize I2C bus descriptor*/
int32_t no_os_i2cbus_init(const struct no_os_i2c_init_param *param);

/* Free the resources allocated for I2C  bus desc*/
void no_os_i2cbus_remove(uint32_t bus_number);

#endif // _NO_OS_I2C_H_
