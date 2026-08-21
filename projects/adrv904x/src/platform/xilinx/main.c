/***************************************************************************//**
 *   @file   adrv904x/src/platform/xilinx/main.c
 *   @brief  Main file for XILINX platform of adrv904x project.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "adi_adrv904x_types.h"
#include "platform_includes.h"
#include "common_data.h"
#include "no_os_error.h"
#include "adrv904x.h"

extern int example_main();

static adi_adrv904x_SpiConfigSettings_t spiSettings = {
	.msbFirst = 1,
	.fourWireMode = 1,
	.cmosPadDrvStrength = ADI_ADRV904X_CMOSPAD_DRV_STRONG,
};

static adi_adrv904x_SpiOptions_t spiOptions = {
	.allowSpiStreaming = 0,
	.allowAhbAutoIncrement = 1,
	.allowAhbSpiFifoMode = 0,
};

/***************************************************************************//**
 * @brief Function for obtaining SPI settings.
 *
 * @return SPI settings
*******************************************************************************/
adi_adrv904x_SpiConfigSettings_t *adrv904x_spi_settings_get(void)
{
	return &spiSettings;
}

/***************************************************************************//**
 * @brief Function for obtaining SPI options.
 *
 * @return SPI settings
*******************************************************************************/
adi_adrv904x_SpiOptions_t *adrv904x_spi_options_get(void)
{
	return &spiOptions;
}


/***************************************************************************//**
 * @brief Main function execution for XILINX platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	return example_main();
}
