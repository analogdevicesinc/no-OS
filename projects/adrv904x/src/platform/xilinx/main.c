/***************************************************************************//**
 *   @file   adrv902x/src/platform/xilinx/main.c
 *   @brief  Main file for XILINX platform of adrv902x project.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "adi_adrv904x_types.h"
#include "platform_includes.h"
#include "common_data.h"
#include "no_os_error.h"
#include "adrv904x.h"

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#endif

#ifdef DMA_EXAMPLE
#include "dma_example.h"
#endif

#ifdef IIO_EXAMPLE
#include "iio_example.h"
#endif

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
Please enable only one example and re-build the project.
#endif

	return ret;
}
