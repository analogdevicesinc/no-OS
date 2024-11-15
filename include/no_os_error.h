/***************************************************************************//**
 *   @file   no_os_error.h
 *   @brief  Error codes definition
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _NO_OS_ERROR_H_
#define _NO_OS_ERROR_H_

#include <errno.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#ifndef __NO_OS_ELASTERROR
#define __NO_OS_ELASTERROR 2000
#endif

#define NO_OS_EOVERRUN	(__NO_OS_ELASTERROR + 1) /* Circular buffer overrun */


#define NO_OS_IS_ERR_VALUE(x)	((x) < 0)

#endif // _NO_OS_ERROR_H_
