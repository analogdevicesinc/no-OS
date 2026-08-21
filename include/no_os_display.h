/***************************************************************************//**
 *   @file   no_os_display.h
 *   @brief  Header file of display utilitary functions
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef _NO_OS_DISPLAY_H_
#define _NO_OS_DISPLAY_H_

#include <stdint.h>

/**
 * @brief Converts a 2D array from row-major to column-major format on 8 bits MONOCHROME display
 */
int32_t no_os_row_major_to_column_major_8bits(uint8_t *, uint8_t *, int, int);

#endif // _NO_OS_DISPLAY_H_
