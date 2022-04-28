/***************************************************************************//**
 *   @file   max31855.c
 *   @brief  Implementation of MAX31855 Driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
