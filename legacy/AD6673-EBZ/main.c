/***************************************************************************//**
 *   @file   Main.c
 *   @brief  Implementation of the program's main function.
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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
