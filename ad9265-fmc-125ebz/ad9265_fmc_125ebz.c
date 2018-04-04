/***************************************************************************//**
 *   @file   ad9434_fmc_500ebz.c
 *   @brief  Implementation of Main Function.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2015(c) Analog Devices, Inc.
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
#include "adc_core.h"
#include "dmac_core.h"
#include "platform_drivers.h"
#include "ad9265.h"

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	struct ad9265_dev		 *ad9265_device;
	adc_core				 ad9265_core;
	dmac_core				 ad9265_dma;
	dmac_xfer				 rx_xfer;
	struct ad9265_init_param ad9265_init_param;

	ad9265_init_param.spi_init.chip_select = 0x2;
	ad9265_init_param.spi_init.cpha = 0;
	ad9265_init_param.spi_init.cpol = 0;
	ad9265_init_param.spi_init.type = ZYNQ_PS7_SPI;

	/* base addresses */

	ad9265_core.base_address = XPAR_AXI_AD9265_BASEADDR;
	ad9265_dma.base_address = XPAR_AXI_AD9265_DMA_BASEADDR;

#ifdef ZYNQ
	rx_xfer.start_address = XPAR_DDR_MEM_BASEADDR + 0x800000;
#endif

	ad9265_dma.type = DMAC_RX;
	ad9265_dma.transfer = &rx_xfer;
	rx_xfer.id = 0;
	rx_xfer.no_of_samples = 32768;

	ad9265_core.no_of_channels = 1;
	ad9265_core.resolution = 16;

	adc_setup(ad9265_core);
	ad9265_setup(&ad9265_device, ad9265_init_param, ad9265_core);
	ad9265_testmode_set(ad9265_device, TESTMODE_ONE_ZERO_TOGGLE);

	if(!dmac_start_transaction(ad9265_dma)){
		ad_printf("ad9265: RX test capture done.\n");
	};

	ad9265_testmode_set(ad9265_device, TESTMODE_OFF);

	ad_printf("Done\n");

	return 0;
}
