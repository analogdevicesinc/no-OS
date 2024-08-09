/**
 * @file
 * @brief      This file contains the function definitions for the
 *             Secure Digital High Capacity (SDHC) peripheral module.
 */

/******************************************************************************
 *
 * Copyright (C) 2022-2023 Maxim Integrated Products, Inc. All Rights Reserved.
 * (now owned by Analog Devices, Inc.),
 * Copyright (C) 2023 Analog Devices, Inc. All Rights Reserved. This software
 * is proprietary to Analog Devices, Inc. and its licensors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

#ifndef LIBRARIES_SDHC_INCLUDE_SDHC_LIB_H_
#define LIBRARIES_SDHC_INCLUDE_SDHC_LIB_H_

/* **** Includes **** */
#include "mxc_assert.h"
#include "mxc_device.h"
#include "mxc_sys.h"
#include "sdhc.h"
#include "sdhc_resp_regs.h"
#include <stdio.h>
#include <string.h>

#ifndef SDHC_CLK_FREQ
/**
 * @brief SDHC target clock frequency.
 * @details Max freq. is limited by GCR register to be @ref SystemCoreClock / 2
 * or @ref SystemCoreClock / 4. This field is used as a target for the SDHC
 * peripheral's internal clock divider. R/W reliability issues can sometimes be
 * eliminated by reducing the clock frequency, which is a good first step for
 * troubleshooting.
 */
#define SDHC_CLK_FREQ 40000000
#endif

/**
 * @ingroup sdhc
 * @{
 */

/* **** Definitions **** */
typedef enum {
	MXC_SDHC_LIB_SINGLE_DATA = 1,
	MXC_SDHC_LIB_QUAD_DATA,
} mxc_sdhc_data_width;

typedef enum {
	MXC_SDHC_LIB_IDLE_STATE = 0,
	MXC_SDHC_LIB_READY_STATE = 1,
	MXC_SDHC_LIB_IDENT_STATE = 2,
	MXC_SDHC_LIB_STBY_STATE = 3,
	MXC_SDHC_LIB_TRAN_STATE = 4,
	MXC_SDHC_LIB_DATA_STATE = 5,
	MXC_SDHC_LIB_RCV_STATE = 6,
	MXC_SDHC_LIB_PRG_STATE = 7,
	MXC_SDHC_LIB_DIS_STATE = 8,
} mxc_sdhc_state;

typedef enum {
	CARD_NONE = 0,
	CARD_SDHC,
	CARD_MMC
} mxc_sdhc_lib_card_type;

mxc_sdhc_lib_card_type MXC_SDHC_Lib_Get_Card_Type (void);

/* **************************************************************************
 */
int MXC_SDHC_Lib_SetRCA (void);

/* **************************************************************************
 */
int MXC_SDHC_Lib_GetCSD (mxc_sdhc_csd_regs_t *csd);

/* **************************************************************************
 */
unsigned long long MXC_SDHC_Lib_GetCapacity (mxc_sdhc_csd_regs_t *csd);

/* **************************************************************************
 */
unsigned int MXC_SDHC_Lib_GetSectors (mxc_sdhc_csd_regs_t *csd);

/* **************************************************************************
 */
int MXC_SDHC_Lib_GetBlockSize (mxc_sdhc_csd_regs_t *csd);

/* **************************************************************************
 */
int MXC_SDHC_Lib_GetCurrentState (mxc_sdhc_state *state);

/* **************************************************************************
 */
int MXC_SDHC_Lib_SetDsr (void);

/* **************************************************************************
 */
int MXC_SDHC_Lib_SetBusWidth (mxc_sdhc_data_width bus_width);

/* **************************************************************************
 */
int MXC_SDHC_Lib_InitCard (int retries);

/* **************************************************************************
 */
void MXC_SDHC_Lib_Async_Handler (void);

/* **************************************************************************
 */
int MXC_SDHC_Lib_Prepare_Trans (mxc_sdhc_data_width width);

/* **************************************************************************
 */
int MXC_SDHC_Lib_Write (unsigned int dst_addr, void *src_addr,
			unsigned int cnt, mxc_sdhc_data_width width);

/* **************************************************************************
 */
int MXC_SDHC_Lib_Read (void *dst_addr, unsigned int src_addr, unsigned int cnt,
		       mxc_sdhc_data_width width);

/* **************************************************************************
 */
int MXC_SDHC_Lib_WriteAsync (unsigned int dst_addr, void *src_addr,
			     unsigned int cnt, mxc_sdhc_data_width width,
			     mxc_sdhc_callback_fn callback);

/* **************************************************************************
 */
int MXC_SDHC_Lib_ReadAsync (void *dst_addr, unsigned int src_addr,
			    unsigned int cnt, mxc_sdhc_data_width width,
			    mxc_sdhc_callback_fn callback);

/**@} end of group sdhc */

#endif // LIBRARIES_SDHC_INCLUDE_SDHC_LIB_H_
