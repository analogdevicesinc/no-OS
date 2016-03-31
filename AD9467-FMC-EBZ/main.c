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
#include "xil_cache.h"
#include "xparameters.h"
#include "cf_ad9467.h"
#include "AD9467.h"
#include "AD9517.h"

void xil_printf(const char *ctrl1, ...);

#ifdef _XPARAMETERS_PS_H_
#define SPI_DEVICE_ID	XPAR_PS7_SPI_0_DEVICE_ID
#else
#define SPI_DEVICE_ID	XPAR_SPI_0_DEVICE_ID
#endif

/***************************************************************************//**
 * @brief Main function.
 *
 * @return 0.
*******************************************************************************/
int main(){

    uint32_t mode;

    Xil_ICacheEnable();
    Xil_DCacheEnable();

    /* AD9467 Setup. */
    ad9467_setup(SPI_DEVICE_ID, 0);

    /* AD9517 Setup. */
    ad9517_setup(SPI_DEVICE_ID, 1);    // Initialize device.
    ad9517_power_mode(3, 0);                     // Set channel 3 for normal operation
    ad9517_frequency(3, 250000000);              // Set the channel 3 frequency to 250Mhz
    ad9517_update();                             // Update registers

    /* Read the device ID for AD9467 and AD9517. */
    xil_printf("\n\r********************************************************************\r\n");
    xil_printf("  ADI AD9467-FMC-EBZ Reference Design\n\r");
    xil_printf("  AD9467 CHIP ID: 0x%02x\n\r", ad9467_read(AD9467_REG_CHIP_ID));
    xil_printf("  AD9467 CHIP GRADE: 0x%02x\n\r", ad9467_read(AD9467_REG_CHIP_GRADE));
    xil_printf("  AD9517 CHIP ID: 0x%02x", ad9517_read(AD9517_REG_PART_ID));
    xil_printf("\n\r********************************************************************\r\n");

    /* AD9467 test. */
    adc_setup(0);

    for (mode = MIDSCALE; mode <= ONE_ZERO_TOGGLE; mode++)        // Data pattern checks
    {
        adc_test(mode, OFFSET_BINARY);       // Data format is offset binary
        adc_test(mode, TWOS_COMPLEMENT);     // Data format is twos complement
    }
    xil_printf("Testing done.\n\r");
    /* AD9467 Setup for data acquisition */
    ad9467_output_invert(0);    // Output invert Off
    ad9467_transfer();          // Synchronously update registers
    ad9467_output_format(1);    // Twos complement
    ad9467_transfer();          // Synchronously update registers
    ad9467_reset_PN9(0);        // Clear PN9 bit
    ad9467_transfer();          // Synchronously update registers
    ad9467_reset_PN23(0);       // Clear PN23 bit
    ad9467_transfer();          // Synchronously update registers
    ad9467_test_mode(0);        // Test mode Off
    ad9467_transfer();          // Synchronously update registers

    xil_printf("Start capturing data...\n\r");

    adc_capture(16384, DDR_BASEADDR);

    xil_printf("Done.\n\r");

    Xil_DCacheDisable();
    Xil_ICacheDisable();

    return 0;
}
