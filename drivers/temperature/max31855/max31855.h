/***************************************************************************//**
 *   @file   max31855.c
 *   @brief  Implementation of MAX31855 Driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __MAX31855_H__
#define __MAX31855_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include "no_os_spi.h"
#include "no_os_util.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define MAX31855_GET_INTERNAL_TEMP(x)		(no_os_field_get(NO_OS_GENMASK(15, 4), x))
#define MAX31855_GET_FAULT_BIT(x)		(no_os_field_get(NO_OS_BIT(16), x))
#define MAX31855_GET_THERMOCOUPLE_TEMP(x)	(no_os_field_get(NO_OS_GENMASK(31, 18), x))
#define MAX31855_GET_FAULTS(x)			(no_os_field_get(NO_OS_GENMASK(2, 0), x))

/**
 * Decimal resolution for the 2 temperatures (0.25 C and 0.0625 C / LSB).
 * integer_temp = raw_temp / dec_div.
 */
#define MAX31855_THERMOCOUPLE_TEMP_DEC_DIV	4
#define MAX31855_INTERNAL_TEMP_DEC_DIV	 	16

/** Sign bit position */
#define MAX31855_THERMOCOUPLE_TEMP_SIGN_POS	13
#define MAX31855_INTERNAL_TEMP_SIGN_POS		11

/**
 * @brief MAX31855 comm init param
 */
struct max31855_init_param {
	struct no_os_spi_init_param spi_init;
};

/**
 * @brief interger/decimal format used for temperature representation
 */
struct max31855_decimal {
	int32_t integer;
	int32_t decimal;
};

struct _max31855_fault_sts_mask {
	/** Thermocouple is short circuited to VCC */
	uint8_t SCV:1;
	/** Thermocouple is short circuited to GND */
	uint8_t SCG:1;
	/** Thermocouple probe not connected */
	uint8_t OC:1;
};

union max31855_fault_sts {
	struct _max31855_fault_sts_mask fields;
	uint8_t value;
};

/**
 * @brief MAX31855 descriptor
 */
struct max31855_dev {
	struct no_os_spi_desc *comm_desc;
	union max31855_fault_sts fault;
};

/** Device and comm init function */
int max31855_init(struct max31855_dev **, struct max31855_init_param *);

/** Free resources allocated by the init function */
int max31855_remove(struct max31855_dev *);

/** Read raw register value */
int max31855_read_raw(struct max31855_dev *, uint32_t *);

/** Read thermocouple and internal temperatures (converted in deg. C) */
int max31855_read_temp(struct max31855_dev *, struct max31855_decimal *,
		       struct max31855_decimal *);

#endif
