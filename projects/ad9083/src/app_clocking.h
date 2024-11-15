/***************************************************************************//**
 *   @file   app_clocking.h
 *   @brief  Clock setup and initialization routines.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __APP_CLOCKING_H
#define __APP_CLOCKING_H

#include <stdint.h>
#include "ad9528.h"

/**
 * @struct app_clocking_init
 * @brief Structure holding the parameters for clocking app initialization.
 */
struct app_clocking_init {
	/* Settings selection */
	uint8_t uc;
	/* jesd receive clock */
	uint32_t lmfc_rate_hz;
};

/**
 * @struct app_clocking
 * @brief Structure holding clocking app descriptor.
 */
struct app_clocking {
	/* Structure holding a clock device descriptor */
	struct ad9528_dev *clkchip_device;
};

/* @brief Application clocking setup. */
int32_t app_clocking_init(struct app_clocking **app,
			  struct app_clocking_init *init_param);

/* @brief Application clocking remove. */
int32_t app_clocking_remove(struct app_clocking *app);

#endif /* __APP_CLOCKING_H */
