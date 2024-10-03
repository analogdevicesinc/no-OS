/*******************************************************************************
 *   @file   pqlib_afe.h
 *   @brief  Analog frontend data polling header file
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
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

#ifndef __PQLIB_AFE_H__
#define __PQLIB_AFE_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "ade9430.h"
#include "adi_pqlib.h"
#include "afe_config.h"
#include "intrpt_handler.h"
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
