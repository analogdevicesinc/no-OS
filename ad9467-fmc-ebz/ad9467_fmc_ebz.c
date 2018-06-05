/***************************************************************************//**
 *   @file   Main.c
 *   @brief  Implementation of the program's main function.
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
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
 *
********************************************************************************
 *   SVN Revision: $WCREV$
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdio.h>
#include "platform_drivers.h"
#include "xil_cache.h"
#include "xparameters.h"
#include "adc_core.h"
#include "dmac_core.h"
#include "ad9467.h"
#include "ad9517.h"

/* ADC registers */
#define CF_REG_DELAY_CTRL			0x60
#define CF_REG_DELAY_STATUS			0x20
#define CF_REG_DATA_MONITOR			0x404

/* CF_REG_DATA_MONITOR bit definition. */
#define CF_DATA_MONITOR_PN_ERR         (1 << 2) // (Write 1 to clear)
#define CF_DATA_MONITOR_PN_SYNC        (1 << 1) // (Write 1 to clear)
#define CF_DATA_MONITOR_PN_OVER_RNG    (1 << 0) // (Write 1 to clear)

typedef enum typeOutputModes {
    OFFSET_BINARY = 0,
    TWOS_COMPLEMENT,
    GRAY_CODE,
}typeOutputModes;

typedef enum typeTestModes {
    TEST_DISABLE = 0,
    MIDSCALE,
    POS_FULLSCALE,
    NEG_FULLSCALE,
    CHECKERBOARD,
    PN_23_SEQUENCE,
    PN_9_SEQUENCE,
    ONE_ZERO_TOGGLE
}typeTestModes;

void adc_test(adc_core adc,
			  dmac_core dma,
			  struct ad9467_dev *dev,
			  uint32_t mode,
			  uint32_t format);
void DisplayTestMode(uint32_t mode, uint32_t format);

/***************************************************************************//**
 * @brief Main function.
 *
 * @return 0.
*******************************************************************************/
int main(){
	struct ad9467_dev *ad9467_device;
	struct ad9517_dev *ad9517_device;
	struct ad9467_init_param ad9467_init;
	struct ad9517_init_param ad9517_init;
	adc_core ad9467_core;
	dmac_core ad9467_dma;
	dmac_xfer rx_xfer;
    uint32_t mode;
    uint8_t ret_val;
    uint32_t ret_val_32;
    int32_t status;
    uint32_t i;

    Xil_ICacheEnable();
    Xil_DCacheEnable();

    /* base addresses */
    ad9467_core.base_address = XPAR_AXI_AD9467_BASEADDR;
    ad9467_dma.base_address = XPAR_AXI_AD9467_DMA_BASEADDR;
#ifdef ZYNQ
	x_xfer.start_address = XPAR_DDR_MEM_BASEADDR + 0x800000;
#endif

#ifdef MICROBLAZE
	x_xfer.start_address = XPAR_AXI_DDR_CNTRL_BASEADDR + 0x800000;
#endif

	ad9467_core.no_of_channels = 1;
	ad9467_core.resolution = 16;
	ad9467_dma.transfer = &rx_xfer;
	ad9467_dma.type = DMAC_RX;
	rx_xfer.id = 0;
	rx_xfer.no_of_samples = 8192;
#ifdef ZYNQ
	ad9467_init.spi_init.type = ZYNQ_PS7_SPI,;
	ad9517_init.spi_init.type = ZYNQ_PS7_SPI,;
#endif

#ifdef MICROBLAZE
	ad9467_init.spi_init.type = MICROBLAZE_SPI;
	ad9517_init.spi_init.type = MICROBLAZE_SPI;
#endif
    ad9467_init.spi_init.chip_select = 0x0E;
	ad9467_init.spi_init.cpha = 0;
	ad9467_init.spi_init.cpol = 0;
	ad9517_init.spi_init.chip_select = 0x0D;
	ad9517_init.spi_init.cpha = 0;
	ad9517_init.spi_init.cpol = 0;

    /* AD9467 Setup. */
    ad9467_setup(&ad9467_device, ad9467_init);

    /* AD9517 Setup. */
    /* Initialize device. */
    ad9517_setup(&ad9517_device, ad9517_init);
    /* Set channel 3 for normal operation */
    ad9517_power_mode(ad9517_device, 3, 0);
    /* Set the channel 3 frequency to 250Mhz */
    ad9517_frequency(ad9517_device, 3, 250000000);
    /* Update registers */
    ad9517_update(ad9517_device);

    /* Read the device ID for AD9467 and AD9517. */
    printf("\n\r********************************************************************\r\n");
    printf("  ADI AD9467-FMC-EBZ Reference Design\n\r");
    ad9467_read(ad9467_device, AD9467_REG_CHIP_ID, &ret_val);
    printf("  AD9467 CHIP ID: 0x%02x\n\r", ret_val);
    ad9467_read(ad9467_device, AD9467_REG_CHIP_GRADE, &ret_val);
    printf("  AD9467 CHIP GRADE: 0x%02x\n\r", ret_val);
    ad9517_read(ad9517_device, AD9517_REG_PART_ID, &ret_val_32);
    printf("  AD9517 CHIP ID: 0x%02x", ret_val_32);
    printf("\n\r********************************************************************\r\n");

    /* AD9467 test. */
    adc_setup(ad9467_core);

	// setup device
    ad9467_write(ad9467_device, AD9467_REG_TEST_IO, 0x05); // pn23
    ad9467_write(ad9467_device, AD9467_REG_DEVICE_UPDATE, 0x01); // update
    ad9467_write(ad9467_device, AD9467_REG_DEVICE_UPDATE, 0x00);

    ad9467_read(ad9467_device, AD9467_REG_OUT_PHASE, &ret_val);
	printf("AD9467[0x016]: %02x\n\r", ret_val);
	// setup adc core
	adc_write(ad9467_core, ADC_REG_CNTRL, 0x2);
	for(i = 0; i < ad9467_core.no_of_channels; i++) {
		adc_write(ad9467_core, ADC_REG_CHAN_CNTRL(i), 0x03);
	}
	adc_write(ad9467_core, ADC_REG_DELAY_CNTRL, 0x0);
	adc_write(ad9467_core, ADC_REG_DELAY_CNTRL, 0x20F1F);

	mdelay(10);
	if (adc_delay_calibrate(ad9467_core, 8, 1)) {
		ad9467_read(ad9467_device, 0x16, &ret_val);
		printf("AD9467[0x016]: %02x\n\r", ret_val);
		ad9467_write(ad9467_device, AD9467_REG_OUT_PHASE, 0x80);
		ad9467_write(ad9467_device, AD9467_REG_DEVICE_UPDATE, 0x01);
		ad9467_write(ad9467_device, AD9467_REG_DEVICE_UPDATE, 0x00);
		ad9467_read(ad9467_device, 0x16, &ret_val);
		printf("AD9467[0x016]: %02x\n\r", ret_val);
		mdelay(10);
		if (adc_delay_calibrate(ad9467_core, 16, 1)) {
			printf("adc_setup: can not set a zero error delay!\n\r");
		}
	}

	/* Data pattern checks */
    for (mode = MIDSCALE; mode <= ONE_ZERO_TOGGLE; mode++)
    {
    	/* Data format is offset binary */
        adc_test(ad9467_core, ad9467_dma, ad9467_device, mode, OFFSET_BINARY);
        /* Data format is twos complement */
        adc_test(ad9467_core, ad9467_dma, ad9467_device, mode, TWOS_COMPLEMENT);
    }
    printf("Testing done.\n\r");
    /* AD9467 Setup for data acquisition */
    ad9467_output_invert(ad9467_device, 0, &status);    // Output invert Off
    ad9467_transfer(ad9467_device);          // Synchronously update registers
    ad9467_output_format(ad9467_device, 1, &status);    // Twos complement
    ad9467_transfer(ad9467_device);          // Synchronously update registers
    ad9467_reset_pn9(ad9467_device, 0, &status);        // Clear PN9 bit
    ad9467_transfer(ad9467_device);          // Synchronously update registers
    ad9467_reset_pn23(ad9467_device, 0, &status);       // Clear PN23 bit
    ad9467_transfer(ad9467_device);          // Synchronously update registers
    ad9467_test_mode(ad9467_device, 0, &status);        // Test mode Off
    ad9467_transfer(ad9467_device);          // Synchronously update registers

    printf("Start capturing data...\n\r");

    dmac_start_transaction(ad9467_dma);

    printf("Done.\n\r");

    ad9467_remove(ad9467_device);
    ad9517_remove(ad9517_device);

    Xil_DCacheDisable();
    Xil_ICacheDisable();

    return 0;
}

void adc_test(adc_core adc,
			  dmac_core dma,
			  struct ad9467_dev *dev,
			  uint32_t mode,
			  uint32_t format)
{
    uint32_t n = 0;
    uint32_t rdata;
    uint32_t edata = 0;
    uint32_t error = 0;
    int32_t read;

    ad9467_output_format(dev, format, &read);
    ad9467_test_mode(dev, mode, &read);
    ad9467_transfer(dev);
    dmac_start_transaction(dma);
    DisplayTestMode(mode, format);
    if ((mode == PN_23_SEQUENCE) || (mode == PN_9_SEQUENCE)) {
        if (format == TWOS_COMPLEMENT) {
            printf("          Test skipped\r\n");
            return;
        }

        adc_set_pnsel(adc, 0, ((mode == PN_23_SEQUENCE) ? ADC_PN23A : ADC_PN9));
        mdelay(10);
        adc_write(adc,
        		  CF_REG_DATA_MONITOR,
				  CF_DATA_MONITOR_PN_ERR |
				  CF_DATA_MONITOR_PN_SYNC |
				  CF_DATA_MONITOR_PN_OVER_RNG); // write ones to clear bits
        mdelay(100);
        adc_read(adc, CF_REG_DATA_MONITOR, &rdata);
        if ((rdata & (CF_DATA_MONITOR_PN_ERR |
                     CF_DATA_MONITOR_PN_SYNC |
                     CF_DATA_MONITOR_PN_OVER_RNG)) != 0) {
        	adc_read(adc, CF_REG_DATA_MONITOR, &rdata);
            printf("  ERROR: PN status(%04x).\n\r", rdata);
        } else {
            printf("          Test passed\r\n");
        }
        return;
    }

    for (n = 0; n < 32; n++) {
    	rdata = Xil_In32(dma.transfer->start_address+(n*4));
        if ((mode == MIDSCALE) && (format == OFFSET_BINARY))
            edata = 0x80008000;
        if ((mode == POS_FULLSCALE) && (format == OFFSET_BINARY))
            edata = 0xffffffff;
        if ((mode == NEG_FULLSCALE) && (format == OFFSET_BINARY))
            edata = 0x00000000;
        if ((mode == MIDSCALE) && (format == TWOS_COMPLEMENT))
            edata = 0x00000000;
        if ((mode == POS_FULLSCALE) && (format == TWOS_COMPLEMENT))
            edata = 0x7fff7fff;
        if ((mode == NEG_FULLSCALE) && (format == TWOS_COMPLEMENT))
            edata = 0x80008000;
        if (((mode == CHECKERBOARD) || (mode == ONE_ZERO_TOGGLE)) && (n == 0))
        {
            edata = (rdata & 0xffff);
        }
        if ((mode == CHECKERBOARD) && (n == 0))
            edata = (edata == 0xaaaa) ? 0x5555aaaa : 0xaaaa5555;
        if ((mode == ONE_ZERO_TOGGLE) && (n == 0))
            edata = (edata == 0xffff) ? 0x0000ffff : 0xffff0000;
        if (rdata != edata)
        {
            printf("  ERROR[%2d]: rcv(%08x), exp(%08x)\n\r", n, rdata,
                       edata);
            error = 1;
        }
    }
    if(error == 0)
    {
        printf("          Test passed\r\n");
    }
}

void DisplayTestMode(uint32_t mode, uint32_t format)
{
    char *sMode;
    char *sFormat;

    switch(format)
    {
        case OFFSET_BINARY:
            sFormat = "OFFSET BINARY";
            break;
        case TWOS_COMPLEMENT:
            sFormat = "TWOS_COMPLEMENT";
            break;
        case GRAY_CODE:
            sFormat = "GRAY_CODE";
            break;
        default:
            sFormat = "";
            break;
    }
    switch(mode)
        {
            case TEST_DISABLE:
                sMode = "TEST_DISABLE BINARY";
                break;
            case MIDSCALE:
                sMode = "MIDSCALE";
                break;
            case POS_FULLSCALE:
                sMode = "POS_FULLSCALE";
                break;
            case NEG_FULLSCALE:
                sMode = "NEG_FULLSCALE BINARY";
                break;
            case CHECKERBOARD:
                sMode = "CHECKERBOARD";
                break;
            case PN_23_SEQUENCE:
                sMode = "PN_23_SEQUENCE";
                break;
            case PN_9_SEQUENCE:
                sMode = "PN_9_SEQUENCE";
                break;
            case ONE_ZERO_TOGGLE:
                sMode = "ONE_ZERO_TOGGLE";
                break;
            default:
                sMode = "";
                break;
        }
    printf("ADC Test: mode - %s\r\n          format - %s\n\r",
    		   sMode,
			   sFormat);
}
