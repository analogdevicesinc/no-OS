/***************************************************************************//**
 *   @file   app_clocking.h
 *   @brief  Clock setup and initialization routines.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
