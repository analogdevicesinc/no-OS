/***************************************************************************//**
 *   @file   adrv910x.c
 *   @brief  adrv910x driver source.
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

#include "no_os_error.h"

#include "adi_adrv910x.h"
#include "adrv910x.h"

int __adrv910x_dev_err(const struct adrv910x_rf_phy *phy, const char *function,
		       const int line)
{
	int ret;

	printf("%s, %d: failed with \"%s\" (%d)\n", function, line,
	       phy->adrv910x->common.error.errormessage ?
	       phy->adrv910x->common.error.errormessage : "",
	       phy->adrv910x->common.error.errCode);

	switch (phy->adrv910x->common.error.errCode) {
	case ADI_COMMON_ERR_INV_PARAM:
	case ADI_COMMON_ERR_NULL_PARAM:
		ret = -EINVAL;
		break;
	case ADI_COMMON_ERR_API_FAIL:
		ret = -EFAULT;
		break;
	case ADI_COMMON_ERR_SPI_FAIL:
		ret = -EIO;
		break;
	case ADI_COMMON_ERR_MEM_ALLOC_FAIL:
		ret = -ENOMEM;
		break;
	default:
		ret = -EFAULT;
		break;
	}

	adi_common_ErrorClear(&phy->adrv910x->common);

	return ret;
}

int32_t adrv910x_dev_init(struct adrv910x_rf_phy *device,
			  const struct adrv910x_init_param *init_param)
{
	int c;

//	device->chip = init_param->chip;
	device->dev_clk = init_param->dev_clk;

//	device->rx2tx2 = device->chip->rx2tx2;
//
//	/* initialize channel numbers and ports here since these will never change */
//	for (c = 0; c < adrv910x_CHANN_MAX; c++) {
//		device->rx_channels[c].channel.idx = c;
//		device->rx_channels[c].channel.number = c + ADI_CHANNEL_1;
//		device->rx_channels[c].channel.port = ADI_RX;
//		device->channels[c * 2] = &device->rx_channels[c].channel;
//		device->tx_channels[c].channel.idx = c;
//		device->tx_channels[c].channel.number = c + ADI_CHANNEL_1;
//		device->tx_channels[c].channel.port = ADI_TX;
//		device->channels[c * 2 + 1] = &device->tx_channels[c].channel;
//		/* Initialize AGC */
//		memcpy(&device->rx_channels[c].agc, init_param->agcConfig_init_param,
//		       sizeof(struct adi_adrv910x_GainControlCfg));
//	}
//
//	device->profile_json = init_param->profile;
//	device->profile_length = init_param->profile_length;

	return 0;
}

int adrv910x_init(struct adrv910x_rf_phy *phy)
{
	int ret = 0;

	adi_common_ErrorClear(&phy->adrv910x->common);

	ret = api_call(phy, adi_adrv910x_HwOpen, adrv910x_spi_settings_get());
	if (ret)
		return ret;

	return ret;
}

