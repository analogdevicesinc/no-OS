/**
 * Copyright 2015 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

/*!
* \file adi_adrv903x_api.h
* \brief A convenience header that includes all adrv903x header files.
*
* ADRV903X API Version: 2.12.1.4
*/

#ifndef ADI_ADRV903X_API_H
#define ADI_ADRV903X_API_H

/* Regardless of single or multi-version all applications need the API types. */
#include "adi_adrv903x_all_types.h"

#ifdef ADI_ADRV903X_MULTIVERSION
/* The application will load a specific API version at run-time using api_ver_init(); Include the function pointer
 * declarations for the redirection table. */
#include "adi_adrv903x_ver_redirect.h"
#else
/* The application will compile in or statically link to a single API version; Include the standard function decls */
#include "adi_adrv903x_agc.h"
#include "adi_adrv903x_cals.h"
#include "adi_adrv903x_core.h"
#include "adi_adrv903x_cpu.h"
#include "adi_adrv903x_cpu_cmd_dc_offset.h"
#include "adi_adrv903x_cpu_cmd_run_serdes_eye_sweep.h"
#include "adi_adrv903x_datainterface.h"
#include "adi_adrv903x_error.h"
#include "adi_adrv903x_error_type_action.h"
#include "adi_adrv903x_gpio.h"
#include "adi_adrv903x_hal.h"
#include "adi_adrv903x_platform_pack.h"
#include "adi_adrv903x_radioctrl.h"
#include "adi_adrv903x_rx.h"
#include "adi_adrv903x_rx_nco.h"
#include "adi_adrv903x_tx.h"
#include "adi_adrv903x_tx_nco.h"
#include "adi_adrv903x_user.h"
#include "adi_adrv903x_utilities.h"
#include "adi_adrv903x_version.h"
#endif

#endif
