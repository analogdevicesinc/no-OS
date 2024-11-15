/*******************************************************************************
 *   @file   pqlib_afe.h
 *   @brief  Analog frontend data polling header file
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __PQLIB_AFE_H__
#define __PQLIB_AFE_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "ade9430.h"
#include "adi_pqlib.h"
#include "afe_config.h"
#include "pqlib_example.h"
#include "status.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/** @brief angle for voltage*/
#define ANGLE_VOLTAGE 0
/** @brief angle for current*/
#define ANGLE_CURRENT 1

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/**
 * @details Get AFE input
 * @return 0 on Success
 *         1 on Failure
 */
int get_afe_input();

#endif /* __PQLIB_AFE_H__ */
