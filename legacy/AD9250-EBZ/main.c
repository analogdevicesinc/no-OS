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
#include "cf_ad9250.h"
#include "AD9250.h"

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

    /* AD9250 Setup. */
    ad9250_setup(SPI_BASEADDR, 1);

    /* Read the device ID and Speed grade of AD9250. */
    xil_printf("\n\r********************************************************************\r\n");
    xil_printf("  ADI AD9250-250EBZ Reference Design\n\r");
    xil_printf("  AD9250 CHIP ID: 0x%02x\n\r", ad9250_read(AD9250_REG_CHIP_ID));
    xil_printf("  AD9250 SPEED GRADE: 0x%02x", ad9250_read(AD9250_REG_CHIP_INFO));
    xil_printf("\n\r********************************************************************\r\n");

    for (mode = MIDSCALE; mode <= ONE_ZERO_TOGGLE; mode++)    // Data pattern checks
    {
        adc_test(mode, OFFSET_BINARY);      // Data format is offset binary
        adc_test(mode, TWOS_COMPLEMENT);    // Data format is twos complement
    }

    xil_printf("Testing done.\n\r");

    /* AD9250 Setup for data acquisition. */
    ad9250_output_format(OFFSET_BINARY);    // Offset binary
    ad9250_output_invert(0);                // Output invert Off
    ad9250_output_disable(0);               // Output disable Off
    ad9250_test_mode(TEST_DISABLE);         // Test mode Off
    ad9250_transfer();                      // Synchronously update register

    xil_printf("Start capturing data...\n\r");

    while(1)
    {
        adc_capture(1024, DDR_BASEADDR);
    }

    Xil_DCacheDisable();
    Xil_ICacheDisable();

    return 0;
}
