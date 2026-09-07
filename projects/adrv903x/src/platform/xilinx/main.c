/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Xilinx platform of ADRV903X project.
 *   @author Analog Devices Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "adi_adrv903x_types.h"
#include "platform_includes.h"
#include "common_data.h"
#include "no_os_error.h"
#include "adrv903x.h"

extern int example_main();

static adi_adrv903x_SpiConfigSettings_t spiSettings = {
	.msbFirst = 1,
	.fourWireMode = 1,
	.cmosPadDrvStrength = ADI_ADRV903X_CMOSPAD_DRV_STRONG,
};

static adi_adrv903x_SpiOptions_t spiOptions = {
	.allowSpiStreaming = 0,
	.allowAhbAutoIncrement = 1,
	.allowAhbSpiFifoMode = 0,
};

adi_adrv903x_SpiConfigSettings_t *adrv903x_spi_settings_get(void)
{
	return &spiSettings;
}

adi_adrv903x_SpiOptions_t *adrv903x_spi_options_get(void)
{
	return &spiOptions;
}

int main()
{
	return example_main();
}
