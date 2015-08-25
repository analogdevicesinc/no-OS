/***************************************************************************//**
 *   @file   cf_ad9467.c
 *   @brief  Implementation of cf_ad9467 Driver.
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
#include "cf_ad9467.h"
#include "xil_io.h"
#include "AD9467.h"

extern char inbyte(void);
void Xil_DCacheFlush();
void xil_printf(const char *ctrl1, ...);

/*****************************************************************************/
/************************ Private Functions Prototypes ***********************/
/*****************************************************************************/
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
    uint32_t count;
#ifdef _XPARAMETERS_PS_H_
    for (count = 0; count < ((ms_count * 100000) + 1); count++)
    {
        asm("nop");
    }
#else
	for(count = 0; count < ms_count * 10000; count++);
#endif
}

/***************************************************************************//**
 * @brief adc_dma_read
*******************************************************************************/
void adc_dma_read(uint32_t regAddr, uint32_t *data)
{
	*data = Xil_In32(XPAR_AXI_AD9467_DMA_BASEADDR + regAddr);
}

/***************************************************************************//**
 * @brief adc_dma_write
*******************************************************************************/
void adc_dma_write(uint32_t regAddr, uint32_t data)
{
	Xil_Out32(XPAR_AXI_AD9467_DMA_BASEADDR + regAddr, data);
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
	uint32_t reg_val;
	uint32_t transfer_id;
	uint32_t length;

	length = (size * 2);

	adc_dma_write(AXI_DMAC_REG_CTRL, 0x0);
	adc_dma_write(AXI_DMAC_REG_CTRL, AXI_DMAC_CTRL_ENABLE);

	adc_dma_write(AXI_DMAC_REG_IRQ_MASK, 0x0);

	adc_dma_read(AXI_DMAC_REG_TRANSFER_ID, &transfer_id);
	adc_dma_read(AXI_DMAC_REG_IRQ_PENDING, &reg_val);
	adc_dma_write(AXI_DMAC_REG_IRQ_PENDING, reg_val);

	adc_dma_write(AXI_DMAC_REG_DEST_ADDRESS, address);
	adc_dma_write(AXI_DMAC_REG_DEST_STRIDE, 0x0);
	adc_dma_write(AXI_DMAC_REG_X_LENGTH, length - 1);
	adc_dma_write(AXI_DMAC_REG_Y_LENGTH, 0x0);

	adc_dma_write(AXI_DMAC_REG_START_TRANSFER, 0x1);
	/* Wait until the new transfer is queued. */
	do {
		adc_dma_read(AXI_DMAC_REG_START_TRANSFER, &reg_val);
	}
	while(reg_val == 1);

	/* Wait until the current transfer is completed. */
	do {
		adc_dma_read(AXI_DMAC_REG_IRQ_PENDING, &reg_val);
	}
	while(reg_val != (AXI_DMAC_IRQ_SOT | AXI_DMAC_IRQ_EOT));
	adc_dma_write(AXI_DMAC_REG_IRQ_PENDING, reg_val);

	/* Wait until the transfer with the ID transfer_id is completed. */
	do {
		adc_dma_read(AXI_DMAC_REG_TRANSFER_DONE, &reg_val);
	}
	while((reg_val & (1 << transfer_id)) != (1 << transfer_id));

#ifdef _XPARAMETERS_PS_H_
    Xil_DCacheFlush();
#else
    microblaze_flush_dcache();
    microblaze_invalidate_dcache();
#endif
}

/***************************************************************************//**
 * @brief ADC delay.
 *
 * @param delay - Value of the delay.
 *
 * @return 0.
*******************************************************************************/
uint32_t adc_delay_1(uint32_t no_of_lanes, uint32_t delay)
{
    uint32_t i;
    uint32_t rdata;
    uint32_t pcore_version;

    pcore_version = Xil_In32(CF_BASEADDR + 0x0);
    pcore_version >>= 16;
    xil_printf("pcore_version: %d\n\r", pcore_version);
    if (pcore_version < 9) {
		for (i = 0; i < no_of_lanes; i++) {
			Xil_Out32((CF_BASEADDR + 0x060), 0x0); // delay write
			Xil_Out32((CF_BASEADDR + 0x060), (0x20000 | (i<<8) | delay)); // delay write
			while (Xil_In32((CF_BASEADDR + 0x064) & 0x100) == 0x100) {}
		}

		for (i = 0; i < no_of_lanes; i++) {
			Xil_Out32((CF_BASEADDR + 0x060), 0x0); // delay write
			Xil_Out32((CF_BASEADDR + 0x060), (0x30000 | (i<<8) | delay)); // delay write
			while (Xil_In32((CF_BASEADDR + 0x064) & 0x100) == 0x100) {}
			rdata = Xil_In32(CF_BASEADDR + 0x064) & 0xff;
			if (rdata != delay) {
				xil_printf("adc_delay_1: sel(%2d), data(%04x)\n\r", i, rdata);
			}
		}
    } else {
		for (i = 0; i < no_of_lanes; i++) {
			Xil_Out32((CF_BASEADDR + ((0x200 + i)*4)), delay);
			rdata = Xil_In32(CF_BASEADDR + ((0x200 + i)*4));
			if (rdata != delay) {
				xil_printf("adc_delay_1: sel(%2d), rcv(%04x), exp(%04x)\n\r", i, rdata, delay);
			}
		}
    }

    return(0);
}

/***************************************************************************//**
 * @brief adc_pn_mon
 *
 * @param pn23_pn9_n - PN type.
 *
 * @return 0 or error.
*******************************************************************************/
u32 adc_pn_mon(u32 pn23_pn9_n)
{
	uint32_t rdata;
	uint32_t error;

	Xil_Out32((CF_BASEADDR + 0x400), 0x1); // signextn, fmt & ch enable
	Xil_Out32((CF_BASEADDR + 0x418), ((pn23_pn9_n == 0x1) ? 0x10000 : 0x00000));
	delay_ms(10);

	Xil_Out32((CF_BASEADDR + 0x404), 0xff);
	delay_ms(100);

	error = 0;
	rdata = Xil_In32(CF_BASEADDR + 0x404);
	if (rdata != 0) {
		error = 1;
	}

	return(error);
}

/***************************************************************************//**
 * @brief ADC delay.
 *
 * @return 0.
*******************************************************************************/
uint32_t adc_delay(u32 no_of_lanes, uint32_t pn23_pn9_n)
{
    uint32_t delay;
    uint32_t lasterr;
    uint32_t startdelay;
    uint32_t stopdelay;

	lasterr = 1;
	startdelay = 32;
	stopdelay = 31;

	for (delay = 0; delay < 32; delay++) {
		adc_delay_1(no_of_lanes, delay);
		delay_ms(10);
		if (adc_pn_mon(pn23_pn9_n) == 0) {
			if (lasterr == 1) {
				startdelay = delay;
			}
			lasterr = 0;
		} else {
			if (lasterr == 0) {
				stopdelay = delay;
				break;
			}
			lasterr = 1;
		}
	}
	if (startdelay > 31) {
		adc_delay_1(no_of_lanes, 0);
		return(1);
	}
	delay = startdelay + ((stopdelay - startdelay)/2);
	xil_printf("adc_delay: setting zero error delay (%d)\n\r", delay);
	adc_delay_1(no_of_lanes, delay);

    return(0);
}

/***************************************************************************//**
* @brief adc_set_pnsel
*******************************************************************************/
int32_t adc_set_pnsel(uint32_t ch, enum adc_pnsel sel)
{
	uint32_t reg_data;

	reg_data = Xil_In32(CF_BASEADDR + ADC_REG_CHAN_CNTRL_3(ch));
	reg_data &= ~ADC_ADC_PN_SEL(~0);
	reg_data |= ADC_ADC_PN_SEL(sel);
	Xil_Out32(CF_BASEADDR + ADC_REG_CHAN_CNTRL_3(ch), reg_data);

	return 0;
}

/***************************************************************************//**
 * @brief Configures the AD9467 device to run in the selected test mode
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
 *                 2 - gray code
 *
 * @return None.
*******************************************************************************/
void adc_test(uint32_t mode, uint32_t format)
{
    uint32_t n;
    uint32_t rdata;
    uint32_t edata = 0;
    uint32_t error = 0;

    ad9467_output_format(format);
    ad9467_test_mode(mode);
    ad9467_transfer();
    adc_capture(64, DDR_BASEADDR);
    DisplayTestMode(mode, format);
    if ((mode == PN_23_SEQUENCE) || (mode == PN_9_SEQUENCE))
    {
        if (format == TWOS_COMPLEMENT)
        {
            xil_printf("          Test skipped\r\n");
            return;
        }

        adc_set_pnsel(0, ((mode == PN_23_SEQUENCE) ? ADC_PN23A : ADC_PN9));
        delay_ms(10);
        Xil_Out32((CF_BASEADDR + CF_REG_DATA_MONITOR),
                   CF_DATA_MONITOR_PN_ERR |
                   CF_DATA_MONITOR_PN_SYNC |
                   CF_DATA_MONITOR_PN_OVER_RNG);    // write ones to clear bits
        delay_ms(100);
        if ((Xil_In32(CF_BASEADDR + CF_REG_DATA_MONITOR) &
                     (CF_DATA_MONITOR_PN_ERR |
                     CF_DATA_MONITOR_PN_SYNC |
                     CF_DATA_MONITOR_PN_OVER_RNG)) != 0)
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
        rdata = Xil_In32(DDR_BASEADDR+(n*4));
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
            xil_printf("  ERROR[%2d]: rcv(%08x), exp(%08x)\n\r", n, rdata,
                       edata);
            error = 1;
        }
    }
    if(error == 0)
    {
        xil_printf("          Test passed\r\n");
    }
}

/***************************************************************************//**
 * @brief Initializes the AD9467 FPGA core.
 *
 * @param dco_delay - ADC delay.
 *
 * @return None.
*******************************************************************************/
void adc_setup(uint32_t dco_delay)
{
	Xil_Out32((CF_BASEADDR + 0x040), 0x3);
	delay_ms(10);

	// setup device
    ad9467_write(AD9467_REG_TEST_IO, 0x05); // pn23
    ad9467_write(AD9467_REG_DEVICE_UPDATE, 0x01); // update
    ad9467_write(AD9467_REG_DEVICE_UPDATE, 0x00);

	xil_printf("AD9467[0x016]: %02x\n\r", ad9467_read(AD9467_REG_OUT_PHASE));
	// setup adc core
	Xil_Out32((CF_BASEADDR+0x44), 	0x2); 		// DDR_EDGESEL active
	Xil_Out32((CF_BASEADDR+0x400), 	0x3); 		// pn23
	Xil_Out32((CF_BASEADDR+0x60), 	0x0);		// clear Delay Control
	Xil_Out32((CF_BASEADDR+0x60), 	0x20F1F);	// Setup Delay Control

	delay_ms(10);
	if (adc_delay(8, 1)) {
		xil_printf("AD9467[0x016]: %02x\n\r", ad9467_read(0x16));
		ad9467_write(AD9467_REG_OUT_PHASE, 0x80);
		ad9467_write(AD9467_REG_DEVICE_UPDATE, 0x01);
		ad9467_write(AD9467_REG_DEVICE_UPDATE, 0x00);
		xil_printf("AD9467[0x016]: %02x\n\r", ad9467_read(0x16));
		delay_ms(10);
		if (adc_delay(16, 1)) {
			xil_printf("adc_setup: can not set a zero error delay!\n\r");
		}
	}
}

/***************************************************************************//**
 * @brief Display test mode.
 *
 * @param mode - Test mode.
 * @param format - Data format.
 *
 * @return None.
*******************************************************************************/
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
    xil_printf("ADC Test: mode - %s\r\n          format - %s\n\r", sMode, sFormat);
}
