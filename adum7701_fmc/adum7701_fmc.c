/***************************************************************************//**
* @file adum7701_fmc.c
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

#include "clkgen_core.h"
#include "adc_core.h"

#include "platform_drivers.h"
#include "adum7701.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define GPIO_DEVICE_ID		XPAR_PS7_GPIO_0_DEVICE_ID
#define GPIO_OFFSET		54
#define GPIO_DEC_RATIO_32	GPIO_OFFSET + 35
#define GPIO_DEC_RATIO_64	GPIO_OFFSET + 36
#define GPIO_DEC_RATIO_128	GPIO_OFFSET + 37
#define GPIO_DEC_RATIO_256	GPIO_OFFSET + 38
#define GPIO_DEC_RATIO_512	GPIO_OFFSET + 39
#define GPIO_DEC_RATIO_1024	GPIO_OFFSET + 40
#define GPIO_DEC_RATIO_2048	GPIO_OFFSET + 41
#define GPIO_DEC_RATIO_4096	GPIO_OFFSET + 42
#define GPIO_RESET		GPIO_OFFSET + 48

struct adum7701_init_param adum7701_default_init_param = {
	/* GPIO */
	/* dec_ratio */
	{ PS7_GPIO, GPIO_DEVICE_ID, GPIO_DEC_RATIO_256 },
	/* filter_reset */
	{ PS7_GPIO, GPIO_DEVICE_ID, GPIO_RESET },
};

clkgen_init_params clkgen_default_init_params = {
	XPAR_AXI_ADC_CLKGEN_BASEADDR, // base_addr
	10000000, // 10Mhz rate
	100000000, // 100Mhz parent_rate
};

int main()
{
	struct adum7701_dev *dev;
	clkgen_device *clkgen_dev;
	adc_core  adum7701_adc_core;
	uint32_t *adc_data, i;

	/* Initialize the device */
	adum7701_init(&dev, adum7701_default_init_param);

	/* Initialize CLKGENs */
	clkgen_setup(&clkgen_dev, clkgen_default_init_params);

	/* ADC capture */
	adum7701_adc_core.start_address = 0x800000;
	adc_capture(&adum7701_adc_core, 1024);

	mdelay(10000);

	adc_data = (uint32_t*)adum7701_adc_core.start_address;
	for(i = 0; i < 1024; i++) {
		printf("%lu\r\n", *adc_data);
		adc_data += 1; // go to the next address in memory
	}

	adum7701_remove(dev);

	printf("Done\n");

	return 0;
}
