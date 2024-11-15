/***************************************************************************//**
 *   @file   adrv902x/src/platform/xilinx/main.c
 *   @brief  Main file for XILINX platform of adrv902x project.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_includes.h"
#include "common_data.h"
#include "no_os_error.h"
#include "adrv9025.h"

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#endif

#ifdef DMA_EXAMPLE
#include "dma_example.h"
#endif

#ifdef IIO_EXAMPLE
#include "iio_example.h"
#endif

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
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret = -EINVAL;

#ifdef BASIC_EXAMPLE
	ret = basic_example_main();
#endif

#ifdef DMA_EXAMPLE
	ret = dma_example_main();
#endif

#ifdef IIO_EXAMPLE
	ret = iio_example_main();
#endif

#if (BASIC_EXAMPLE + IIO_EXAMPLE + DMA_EXAMPLE == 0)
#error At least one example has to be selected using y value in Makefile.
#elif (BASIC_EXAMPLE + IIO_EXAMPLE + DMA_EXAMPLE > 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and rebuild the project.
#endif

	return ret;
}
