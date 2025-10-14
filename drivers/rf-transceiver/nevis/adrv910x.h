/***************************************************************************//**
 *   @file   adrv910x.h
 *   @brief  adrv910x driver header.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
#ifndef IIO_TRX_ADRV910X_H_
#define IIO_TRX_ADRV910X_H_

#include "adi_adrv910x_types.h"
#include "adi_common_types.h"

#include "no_os_platform.h"

#define api_call(phy, func, args...)	({				\
	int __ret = func((phy)->adrv910x, ##args);			\
									\
	if (__ret)							\
		__ret = adrv910x_dev_err(phy);	\
									\
	__ret;								\
})

struct adrv910x_rf_phy {
	struct spi_device *spi;
	struct no_os_gpio_desc *reset_gpio;
	struct no_os_clk_desc *dev_clk;

	adi_adrv910x_Device_t adrv910x_device;
	adi_adrv910x_Device_t *adrv910x;
	struct adrv910x_hal_cfg hal;
};

struct adrv910x_init_param {
	struct no_os_clk_desc		*dev_clk;
	struct adi_adrv9001_Device	*adrv9001_device;
};

int __adrv910x_dev_err(const struct adrv910x_rf_phy *phy, const char *function,
		       const int line);

#define adrv910x_dev_err(phy)	__adrv910x_dev_err(phy, __func__, __LINE__)

/* Initialize the device. */
int32_t adrv910x_dev_init(struct adrv910x_rf_phy *device,
			  const struct adrv910x_init_param *init_param);

int adrv910x_init(struct adrv910x_rf_phy *phy);

struct adi_adrv910x_SpiSettings *adrv910x_spi_settings_get(void);

#endif
