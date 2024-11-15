/***************************************************************************//**
 *   @file   no_os_ain.h
 *   @author PMallick (Pratyush.Mallick@analog.com)
********************************************************************************
 * Copyright (c) 2021-22 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef NO_OS_AIN_H
#define NO_OS_AIN_H

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

/**
 * @struct ain_init_param
 * @brief Structure holding parameters for analog input initialization
 */
struct no_os_ain_init_param {
	/* Analog input reference voltage */
	float vref;
	/* Analog extra parameters (device specific) */
	void *extra;
};

/**
 * @struct ain_desc
 * @brief Structure holding analog input descriptor
 */
struct no_os_ain_desc {
	/* Analog input reference voltage */
	float vref;
	/* Analog extra parameters (device specific) */
	void *extra;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Read analog input voltage */
int32_t no_os_ain_get_voltage(struct no_os_ain_desc *desc, float *value);

/* Initialize the analog input peripheral*/
int32_t no_os_ain_init(struct no_os_ain_desc **desc,
		       const struct no_os_ain_init_param *param);

/* Free the resources allocated by no_os_ain_init() */
int32_t no_os_ain_remove(struct no_os_ain_desc *desc);

#endif	// end of NO_OS_AIN_H
