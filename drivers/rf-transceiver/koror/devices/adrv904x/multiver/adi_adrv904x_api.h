/**
 * Copyright 2015 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/*!
* \file adi_adrv904x_api.h
* \brief A convenience header that includes all adrv904x header files.
*
* ADRV904X API Version: 2.10.0.4
*/

#ifndef ADI_ADRV904X_API_H
#define ADI_ADRV904X_API_H

/* Regardless of single or multi-version all applications need the API types. */
#include "adi_adrv904x_all_types.h"

#ifdef ADI_ADRV904X_MULTIVERSION
/* The application will load a specific API version at run-time using api_ver_init(); Include the function pointer
 * declarations for the redirection table. */
#include "adi_adrv904x_ver_redirect.h"
#else
/* The application will compile in or statically link to a single API version; Include the standard function decls */
#include "adi_adrv904x_agc.h"
#include "adi_adrv904x_cals.h"
#include "adi_adrv904x_core.h"
#include "adi_adrv904x_cpu.h"
#include "adi_adrv904x_cpu_cmd_dc_offset.h"
#include "adi_adrv904x_cpu_cmd_run_serdes_eye_sweep.h"
#include "adi_adrv904x_datainterface.h"
#include "adi_adrv904x_error.h"
#include "adi_adrv904x_error_type_action.h"
#include "adi_adrv904x_gpio.h"
#include "adi_adrv904x_hal.h"
#include "adi_adrv904x_platform_pack.h"
#include "adi_adrv904x_radioctrl.h"
#include "adi_adrv904x_rx.h"
#include "adi_adrv904x_rx_nco.h"
#include "adi_adrv904x_tx.h"
#include "adi_adrv904x_tx_nco.h"
#include "adi_adrv904x_user.h"
#include "adi_adrv904x_utilities.h"
#include "adi_adrv904x_version.h"
#endif

#endif
