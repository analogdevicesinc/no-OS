/***************************************************************************//**
 *   @file   Main.c
 *   @brief  Implementation of the program's main function.
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
#include "cf_ad6673.h"
#include "AD6673.h"

void xil_printf(const char *ctrl1, ...);

/***************************************************************************//**
 * @brief Main function.
 *
 * @return 0.
*******************************************************************************/
int main(void)
{
    uint32_t mode;

    Xil_ICacheEnable();
    Xil_DCacheEnable();

    /* Initialize JESD FPGA core. */
    jesd_core_setup();

    /* Initialize DMA core. */
    dma_core_setup();

    /* AD6673 Setup. */
    ad6673_setup(SPI_BASEADDR, 1);

    /* Read the device ID and Speed grade of AD6673. */
    xil_printf("\n\r********************************************************************\r\n");
    xil_printf("  ADI AD6673-250EBZ Reference Design\n\r");
    xil_printf("  AD6673 CHIP ID: 0x%02x\n\r", ad6673_read(AD6673_REG_CHIP_ID));
    xil_printf("  AD6673 SPEED GRADE: 0x%02x", ad6673_read(AD6673_REG_CHIP_INFO));
    xil_printf("\n\r********************************************************************\r\n");

    for (mode = MIDSCALE; mode <= ONE_ZERO_TOGGLE; mode++)       // Data pattern checks
    {
        if((mode != PN_23_SEQUENCE) && (mode !=PN_9_SEQUENCE))   // skip PN23 and PN9 test modes
        {
            adc_test(mode, OFFSET_BINARY);      // Data format is offset binary
            adc_test(mode, TWOS_COMPLEMENT);    // Data format is twos complement
        }
    }

    xil_printf("Testing done.\n\r");

    /* AD6673 Setup for data acquisition. */
    ad6673_output_format(OFFSET_BINARY);    // Offset binary
    ad6673_output_invert(0);                // Output invert Off
    ad6673_output_disable(0);               // Output disable Off
    ad6673_test_mode(TEST_DISABLE);         // Test mode Off
    ad6673_transfer();                      // Synchronously update registers

    xil_printf("Start capturing data...\n\r");

    while(1)
    {
        adc_capture(1024, DDR_BASEADDR);
    }

    Xil_DCacheDisable();
    Xil_ICacheDisable();

    return 0;
}
