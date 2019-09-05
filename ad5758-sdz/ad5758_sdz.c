/***************************************************************************//**
* @file ad5758_sdz.c
* @brief Implementation of Main Function.
* @author SPopa (stefan.popa@analog.com)
********************************************************************************
* Copyright 2018(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in
* the documentation and/or other materials provided with the
* distribution.
* - Neither the name of Analog Devices, Inc. nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
* - The use of this software may or may not infringe the patent rights
* of one or more patent holders. This license does not release you
* from the requirement that you obtain separate licenses from these
* patent holders to use this software.
* - Use of the software either in source or binary form, must be run
* on or directly connected to an Analog Devices Inc. component.
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
#include <stdio.h>
#include <sleep.h>
#include <stdint.h>
#include <stdlib.h>

#include <xil_cache.h>
#include <xparameters.h>
#include "xil_printf.h"

#include "platform_drivers.h"
#include "ad5758.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define SPI_DEVICE_ID                   XPAR_PS7_SPI_0_DEVICE_ID
#define AD5758_SPI_CS                   0
#define GPIO_DEVICE_ID			XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_OFFSET			54
#define GPIO_DAC_FAULT_N		GPIO_OFFSET + 32
#define GPIO_DAC_RESET_N		GPIO_OFFSET + 33
#define GPIO_DAC_LDAC_N			GPIO_OFFSET + 34

struct ad5758_init_param ad5758_default_init_param = {
	/* SPI */
	{ PS7_SPI, SPI_DEVICE_ID, 1000000, SPI_MODE_1, AD5758_SPI_CS },
	/* GPIO */
	{ PS7_GPIO, GPIO_DEVICE_ID, GPIO_DAC_RESET_N }, // reset_n
	{ PS7_GPIO, GPIO_DEVICE_ID, GPIO_DAC_LDAC_N }, 	// ldac_n
	/* Device Settings */
	1, 			// crc_en
	DPC_VOLTAGE_MODE, 	// dc_dc_mode
	CLKOUT_DISABLE,		// clkout_config
	CLKOUT_FREQ_500_KHZ,	// clkout_freq
	ILIMIT_200_mA,		// dc_dc_ilimt
	RANGE_0V_10V,		// output_range
	SR_CLOCK_240_KHZ, 	// slew_rate_clk
};

int main()
{
	struct ad5758_dev *dev;

	ad5758_init(&dev, ad5758_default_init_param);

	/* Write mid-scale DAC code to the input register */
	ad5758_dac_input_write(dev, 0x7FFF);

	printf("Done\n");

	return 0;
}
