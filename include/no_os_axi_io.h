/***************************************************************************//**
 *   @file   no_os_axi_io.h
 *   @brief  Header file of AXI IO.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _NO_OS_AXI_IO_H_
#define _NO_OS_AXI_IO_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* AXI IO Read data */
int32_t no_os_axi_io_read(uint32_t base, uint32_t offset, uint32_t *data);

/* AXI IO Write data */
int32_t no_os_axi_io_write(uint32_t base, uint32_t offset, uint32_t data);

#endif // _NO_OS_AXI_IO_H_
