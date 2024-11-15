/***************************************************************************//**
 *   @file   no_os_fifo.h
 *   @brief  Header file of fifo
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _NO_OS_FIFO_H_
#define _NO_OS_FIFO_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct no_os_fifo_element
 * @brief Structure holding the fifo element parameters.
 */
struct no_os_fifo_element {
	/** next FIFO element */
	struct no_os_fifo_element *next;
	/** FIFO data pointer */
	char *data;
	/** FIFO length */
	uint32_t len;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Insert element to fifo tail. */
int32_t no_os_fifo_insert(struct no_os_fifo_element **p_fifo, char *buff,
			  uint32_t len);

/* Remove fifo head. */
struct no_os_fifo_element *no_os_fifo_remove(struct no_os_fifo_element *p_fifo);

#endif // _NO_OS_FIFO_H_
