/***************************************************************************//**
 *   @file   adrv902x/src/platform/xilinx/main.c
 *   @brief  Main file for XILINX platform of adrv902x project.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "platform_includes.h"
#include "common_data.h"
#include "no_os_error.h"
#include "adrv9025.h"

/* SPI settings */
static adi_adrv9025_SpiSettings_t spiSettings = {
	.msbFirst = 1,
	.enSpiStreaming = 0,
	.autoIncAddrUp = 1,
	.fourWireMode = 1,
	.cmosPadDrvStrength = ADI_ADRV9025_CMOSPAD_DRV_STRONG,
};

/***************************************************************************//**
 * @brief Function for obtaining SPI settings.
 *
 * @return SPI settings
*******************************************************************************/
adi_adrv9025_SpiSettings_t *adrv9025_spi_settings_get(void)
{
	return &spiSettings;
}

/***************************************************************************//**
 * @brief Main function execution for XILINX platform.
 *
 * @return ret - Result of the example execution.
*******************************************************************************/
int main()
{
	return example_main();
}
