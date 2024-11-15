/***************************************************************************//**
 *   @file   app_talise.h
 *   @brief  Talise initialization and control routines.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __APP_TALISE_H
#define __APP_TALISE_H

#include <stdint.h>
#include "talise_types.h"
#include "adi_hal.h"

enum taliseDeviceId {
	TALISE_A = 0u,
#if defined(ZU11EG) || defined(FMCOMMS8_ZCU102)
	TALISE_B,
#endif

	TALISE_DEVICE_ID_MAX
};

#define TALISE_NUM_SUBCHANNELS		2 /* I - in-phase and Q - quadrature channels */
#define TALISE_NUM_CHAIN_CHANNELS	2 /* channels per RX/TX chain */
#define TALISE_NUM_CHANNELS		(TALISE_DEVICE_ID_MAX * TALISE_NUM_CHAIN_CHANNELS * TALISE_NUM_SUBCHANNELS)

adiHalErr_t talise_setup(taliseDevice_t * const talDev,
			 taliseInit_t * const talInit);
int talise_multi_chip_sync(taliseDevice_t * pd, int step);
void talise_shutdown(taliseDevice_t * const pd);
bool adrv9009_check_sysref_rate(uint32_t lmfc, uint32_t sysref);

#endif /* __APP_TALISE_H */