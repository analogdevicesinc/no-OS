/***************************************************************************//**
 *   @file   cf_ad9250.c
 *   @brief  Implementation of cf_ad9250 Driver.
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
#include "xil_io.h"
#include "cf_ad9250.h"
#include "AD9250.h"
#include "xaxidma.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
static    XAxiDma *axiDma;

/******************************************************************************/
/************************ Private Functions Prototypes ************************/
/******************************************************************************/
void DisplayTestMode(uint32_t mode, uint32_t format);

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Creates a delay of milliseconds.
 *
 * @param ms_count - Delay in milliseconds.
 *
 * @return None.
*******************************************************************************/
void delay_ms(uint32_t ms_count)
{
    volatile uint32_t count;

    for (count = 0; count < ((ms_count * CYCLES_FOR_1_MS) + 1); count++)
    {
        asm("nop");
    }
}

/***************************************************************************//**
 * @brief Initializes JESD FPGA core.
 *
 * @return None.
*******************************************************************************/
void jesd_core_setup(void)
{
    Xil_Out32((JESD_BASEADDR + 0x14), 0x30);        // reset
    delay_ms(100);
    Xil_Out32((JESD_BASEADDR + JESD_REG_LANE_CTRL),
              JESD_LANE_CTRL_SYSREF_SEL(0) |        // hardware sysref generation
              JESD_LANE_CTRL_LANE_SYNC_EN(1) |      // enable lane synchronization
              JESD_LANE_CTRL_SCR_EN(1) |            // enable scrambling
              JESD_LANE_CTRL_SYSREF_EN(0) |         // disable re-alignment at every sysref pulses
              JESD_LANE_CTRL_ERR_DISB(0));          // enable error reporting via sync
    Xil_Out32((JESD_BASEADDR + JESD_REG_FRAMES),
              JESD_FRAMES_FRM_CNT(32) |             // 32 frames per multi-frame
              JESD_FRAMES_BYTE_CNT(2));             // 2 bytes (octets) per frame
}

/***************************************************************************//**
 * @brief Initializes the DMA core.
 *
 * @return None.
*******************************************************************************/
void dma_core_setup(void)
{
    XAxiDma_Config *axiDmaCfg;

    /* DMA initialization */
    axiDmaCfg = XAxiDma_LookupConfig(XPAR_AXI_DMA_0_DEVICE_ID);
    axiDmaCfg->HasMm2S = 0;
    axiDmaCfg->HasS2Mm = 1;
    axiDmaCfg->HasSg = 0;
    XAxiDma_CfgInitialize(axiDma, axiDmaCfg);
}

/***************************************************************************//**
 * @brief Captures a specified number of samples from the ADC.
 *
 * @param size    - number of bytes to read from the device
 * @param address - capture start address
 *
 * @return None.
*******************************************************************************/
void adc_capture(uint32_t size, uint32_t address)
{
    XAxiDma_SimpleTransfer(axiDma, address, (size * 8), XAXIDMA_DEVICE_TO_DMA);

    Xil_Out32((CF_BASEADDR + CF_REG_CAPTURE_CTRL),
               CF_CAPTURE_CTRL_CAPTURE_START(0));       // capture disable
    Xil_Out32((CF_BASEADDR + CF_REG_ADC_STATUS),
               CF_ADC_STATUS_UNDERFLOW |
               CF_ADC_STATUS_OVERFLOW |
               CF_ADC_STATUS_BUSY);                     // clear status
    Xil_Out32((CF_BASEADDR + CF_REG_DATA_MONITOR),
               CF_DATA_MONITOR_PN_ERR |
               CF_DATA_MONITOR_PN_OUT_OF_SYNC |
               CF_DATA_MONITOR_OVER_RNG);               // clear status
    Xil_Out32((CF_BASEADDR + CF_REG_CAPTURE_CTRL),
               CF_CAPTURE_CTRL_CAPTURE_START(1) |
               CF_CAPTURE_CTRL_CAPTURE_COUNT(size));    // capture enable
    do
    {
        delay_ms(1);
    }
    while ((Xil_In32(CF_BASEADDR + CF_REG_ADC_STATUS) & CF_ADC_STATUS_BUSY));
    if ((Xil_In32(CF_BASEADDR + CF_REG_ADC_STATUS) & CF_ADC_STATUS_OVERFLOW))
    {
        xil_printf("overflow occurred, capture may be corrupted\n\r");
    }
#ifdef _XPARAMETERS_PS_H_
    Xil_DCacheFlush();
#else
    microblaze_flush_dcache();
    microblaze_invalidate_dcache();
#endif
}

/***************************************************************************//**
 * @brief Configures the AD9250 device to run in the selected test mode
 *           and checks if the read data pattern corresponds to the expected
 *           values for the set test mode.
 *
 * @param mode - The test mode. Range 0 ..7
 *                        0 - test mode off
 *                        1 - midscale short
 *                        2 - +FS short
 *                        3 - -FS short
 *                        4 - checkerboard output
 *                        5 - PN 23 sequence
 *                        6 - PN 9 sequence
 *                        7 - one/zero word toggle
 * @param format - Sets the data format:
 *                 0 - offset binary
 *                 1 - twos complement
 *
 * @return None.
*******************************************************************************/
void adc_test(uint32_t mode, uint32_t format)
{
    uint32_t i;
    uint32_t n;
    uint32_t error = 0;
    uint32_t rdata_2;
    uint32_t rdata[2];
    uint32_t edata[2];

    edata[0] = 0;
    edata[1] = 0;
    ad9250_output_format(format);
    ad9250_test_mode(mode);
    ad9250_transfer();
    adc_capture(16, DDR_BASEADDR);
    DisplayTestMode(mode, format);
    if ((mode == PN_23_SEQUENCE) || (mode == PN_9_SEQUENCE))
    {
        if (format == TWOS_COMPLEMENT)
        {
            xil_printf("          Test skipped\r\n");
            return;
        }
        Xil_Out32((CF_BASEADDR + CF_REG_PN_TYPE),
                  ((mode == PN_23_SEQUENCE) ? CF_PN_TYPE_BIT(1) : CF_PN_TYPE_BIT(0)));
        delay_ms(10);
        Xil_Out32((CF_BASEADDR + CF_REG_DATA_MONITOR),
                           CF_DATA_MONITOR_PN_ERR |
                           CF_DATA_MONITOR_PN_OUT_OF_SYNC |
                           CF_DATA_MONITOR_OVER_RNG);        // write ones to clear bits
        delay_ms(100);
        if ((Xil_In32(CF_BASEADDR + CF_REG_DATA_MONITOR) &
                             (CF_DATA_MONITOR_PN_ERR |
                              CF_DATA_MONITOR_PN_OUT_OF_SYNC |
                              CF_DATA_MONITOR_OVER_RNG)) != 0)
        {
            xil_printf("  ERROR: PN status(%04x).\n\r",
                       Xil_In32(CF_BASEADDR + CF_REG_DATA_MONITOR));
        }
        else
        {
            xil_printf("          Test passed\r\n");
        }
        return;
    }
    for (n = 0; n < 32; n++)
    {
        rdata_2 = Xil_In32(DDR_BASEADDR + (n * 4));
        rdata[0] = (rdata_2 >> 0) & 0xffff;
        rdata[1] = (rdata_2 >> 16) & 0xffff;
        for (i = 0; i < 2; i++)
        {
            if ((mode == MIDSCALE) && (format == OFFSET_BINARY))
            {
                edata[i] = 0x2000;
            }
            if ((mode == POS_FULLSCALE) && (format == OFFSET_BINARY))
            {
                edata[i] = 0x3fff;
            }
            if ((mode == NEG_FULLSCALE) && (format == OFFSET_BINARY))
            {
                edata[i] = 0x0000;
            }
            if ((mode == MIDSCALE) && (format == TWOS_COMPLEMENT))
            {
                edata[i] = 0x0000;
            }
            if ((mode == POS_FULLSCALE) && (format == TWOS_COMPLEMENT))
            {
                edata[i] = 0x1fff;
            }
            if ((mode == NEG_FULLSCALE) && (format == TWOS_COMPLEMENT))
            {
                edata[i] = 0x2000;
            }
            if (((mode == CHECKERBOARD) || (mode == ONE_ZERO_TOGGLE)) && (n == 0))
            {
                edata[i] = (rdata[i] & 0xffff);
            }
            if ((mode == CHECKERBOARD) && (n > 0))
            {
                edata[i] = (edata[i] == 0x2aaa) ? 0x1555 : 0x2aaa;
            }
            if ((mode == ONE_ZERO_TOGGLE) && (n > 0))
            {
                edata[i] = (edata[i] == 0x3fff) ? 0x0000 : 0x3fff;
            }
            if (rdata[i] != edata[i])
            {
                xil_printf("  ERROR[%2d]: ch(%d), rcv(%08x), exp(%08x)\n\r", n, i, rdata[i], edata[i]);
                error = 1;
            }
        }
    }
    if(error == 0)
    {
        xil_printf("          Test passed\r\n");
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
    xil_printf("ADC Test: mode - %s\r\n          format - %s\n\r", sMode, sFormat);
}
