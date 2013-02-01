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
#include "xparameters.h"
#include "AD9467.h"

extern char inbyte(void);
void Xil_DCacheFlush(void);
void xil_printf(const char *ctrl1, ...);

/*****************************************************************************/
/************************ Private Functions Prototypes ***********************/
/*****************************************************************************/
void DisplayTestMode(u32 mode, u32 format);

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
void delay_ms(u32 ms_count)
{
	u32 count;

	for (count = 0; count < ((ms_count * 100000) + 1); count++)
	{
		asm("nop");
	}
}

/***************************************************************************//**
 * @brief Captures a specified number of samples from the ADC.
 *
 * @param size - number of bytes to read from the device
 * @param address - capture start address
 *
 * @return None.
*******************************************************************************/
void adc_capture(u32 size, u32 address)
{
	Xil_Out32((DMA_BASEADDR + 0x030), 0); 				// clear dma operations
	Xil_Out32((DMA_BASEADDR + 0x030), 1); 				// enable dma operations
	Xil_Out32((DMA_BASEADDR + 0x048), address); 		// capture start address
	Xil_Out32((DMA_BASEADDR + 0x058), (size * 8));  	// number of bytes
	Xil_Out32((CF_BASEADDR + CF_REG_CAPTURE_CTRL),
			   CF_CAPTURE_CTRL_CAPTURE_START(0));   	// capture disable
	Xil_Out32((CF_BASEADDR + CF_REG_ADC_STATUS),
			   CF_ADC_STATUS_UNDERFLOW |
			   CF_ADC_STATUS_OVERFLOW |
			   CF_ADC_STATUS_BUSY);						// clear status
	Xil_Out32((CF_BASEADDR + CF_REG_DATA_MONITOR),
			   CF_DATA_MONITOR_PN_ERR |
			   CF_DATA_MONITOR_PN_SYNC |
			   CF_DATA_MONITOR_PN_OVER_RNG);			// clear status
	Xil_Out32((CF_BASEADDR + CF_REG_CAPTURE_CTRL),
			   CF_CAPTURE_CTRL_CAPTURE_START(1) |
			   CF_CAPTURE_CTRL_CAPTURE_COUNT(size)); 	// capture enable
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
 * @brief ADC delay.
 *
 * @param delay - Value of the delay.
 *
 * @return 0.
*******************************************************************************/
u32 adc_delay_1(u32 delay)
{
	u32 i;
	u32 rdata;

	for (i = 0; i < 8; i++)
	{
		Xil_Out32((CF_BASEADDR + CF_REG_DELAY_CTRL), 0x000000);	// clear bits
		Xil_Out32((CF_BASEADDR + CF_REG_DELAY_CTRL),
				   CF_DELAY_CTRL_SEL(1) |
				   CF_DELAY_CTRL_RW_ENABLE(0) |
				   CF_DELAY_CTRL_WR_ADDR(i) |
				  CF_DELAY_CTRL_WR_DATA(delay));				// delay write
		Xil_Out32((CF_BASEADDR + CF_REG_DELAY_CTRL),
				   CF_DELAY_CTRL_SEL(1) |
				   CF_DELAY_CTRL_RW_ENABLE(0) |
				   CF_DELAY_CTRL_WR_ADDR(i) |
				   CF_DELAY_CTRL_WR_DATA(delay));				// delay write
	}
	for (i = 0; i < 8; i++)
	{
		Xil_Out32((CF_BASEADDR + CF_REG_DELAY_CTRL), 0x000000);	// clear bits
		Xil_Out32((CF_BASEADDR + CF_REG_DELAY_CTRL),
				   CF_DELAY_CTRL_SEL(1) |
				   CF_DELAY_CTRL_RW_ENABLE(1) |
				   CF_DELAY_CTRL_WR_ADDR(i));					// delay read
		Xil_Out32((CF_BASEADDR + CF_REG_DELAY_CTRL),
				   CF_DELAY_CTRL_SEL(1) |
				   CF_DELAY_CTRL_RW_ENABLE(1) |
			       CF_DELAY_CTRL_WR_ADDR(i));					// delay read

		rdata = Xil_In32(CF_BASEADDR + CF_REG_DELAY_STATUS);
		if (rdata != (CF_DELAY_STATUS_LOCKED | CF_DELAY_CTRL_WR_DATA(delay)))
		{
			xil_printf("adc_delay_1: sel(%2d), data(%04x)\n\r", i, rdata);
		}
	}

	return(0);
}

/***************************************************************************//**
 * @brief ADC delay.
 *
 * @return 0.
*******************************************************************************/
u32 adc_delay(void)
{
	u32 delay;
	u32 error;
	u32 lasterr;
	u32 startdelay;
	u32 stopdelay;

	lasterr = 1;
	startdelay = 32;
	stopdelay = 31;
	for (delay = 0; delay < 32; delay++)
	{
		adc_delay_1(delay);
		delay_ms(10);
		Xil_Out32((CF_BASEADDR + CF_REG_DATA_MONITOR),
				   CF_DATA_MONITOR_PN_ERR |
				   CF_DATA_MONITOR_PN_SYNC |
				   CF_DATA_MONITOR_PN_OVER_RNG);		// writer ones to clear bits
		delay_ms(100);
		error = Xil_In32(CF_BASEADDR + CF_REG_DATA_MONITOR) &
						(CF_DATA_MONITOR_PN_ERR |
					     CF_DATA_MONITOR_PN_SYNC |
						 CF_DATA_MONITOR_PN_OVER_RNG);	// read the status of ADC data monitoring
		if (error == 0)
		{
			if (lasterr == 1)
			{
				startdelay = delay;
			}
			lasterr = 0;
		}
		else
		{
			if (lasterr == 0)
			{
				stopdelay = delay;
				break;
			}
			lasterr = 1;
		}
	}
	if (startdelay > 31)
	{
		adc_delay_1(0);
		return(1);
	}
	delay = startdelay + ((stopdelay - startdelay)/2);
	adc_delay_1(delay);

	return(0);
}

/***************************************************************************//**
 * @brief Configures the AD9467 device to run in the selected test mode
 * 		  and checks if the read data pattern corresponds to the expected
 * 		  values for the set test mode.
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
void adc_test(u32 mode, u32 format)
{
	u32 n;
	u32 rdata;
	u32 edata = 0;
	u32 error = 0;

	ad9467_output_format(format);
	ad9467_test_mode(mode);
	ad9467_transfer();
	adc_capture(16, DDR_BASEADDR);
	DisplayTestMode(mode, format);
	if ((mode == PN_23_SEQUENCE) || (mode == PN_9_SEQUENCE))
	{
		if (format == TWOS_COMPLEMENT) return;
		Xil_Out32((CF_BASEADDR + CF_REG_PN_TYPE),
				  ((mode == PN_23_SEQUENCE) ? 0x01 : 0x00));
		delay_ms(10);
		Xil_Out32((CF_BASEADDR + CF_REG_DATA_MONITOR),
				   CF_DATA_MONITOR_PN_ERR |
				   CF_DATA_MONITOR_PN_SYNC |
				   CF_DATA_MONITOR_PN_OVER_RNG);	// write ones to clear bits
		delay_ms(100);
		if ((Xil_In32(CF_BASEADDR + CF_REG_DATA_MONITOR) &
				     (CF_DATA_MONITOR_PN_ERR |
				     CF_DATA_MONITOR_PN_SYNC |
				     CF_DATA_MONITOR_PN_OVER_RNG)) != 0)
		{
			xil_printf("  ERROR: PN status(%04x).\n\r",
					   Xil_In32(CF_BASEADDR + CF_REG_DATA_MONITOR));
		}
		return;
	}
	for (n = 0; n < 32; n++)
	{
		rdata = Xil_In32(DDR_BASEADDR+(n*4));
		if ((mode == MIDSCALE) && (format == OFFSET_BINARY)) edata = 0x80008000;
		if ((mode == POS_FULLSCALE) && (format == OFFSET_BINARY)) edata = 0xffffffff;
		if ((mode == NEG_FULLSCALE) && (format == OFFSET_BINARY)) edata = 0x00000000;
		if ((mode == MIDSCALE) && (format == TWOS_COMPLEMENT)) edata = 0x00000000;
		if ((mode == POS_FULLSCALE) && (format == TWOS_COMPLEMENT)) edata = 0x7fff7fff;
		if ((mode == NEG_FULLSCALE) && (format == TWOS_COMPLEMENT)) edata = 0x80008000;
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
void adc_setup(u32 dco_delay)
{
	Xil_Out32((CF_BASEADDR + CF_REG_DATA_MODE), CF_DATA_MODE_BITS(0x1));		// Samples are interleaved rising edge first and then falling edge.
	Xil_Out32((CF_BASEADDR + CF_REG_PN_TYPE), CF_PN_TYPE_BIT(0));			    // Select PN9
	Xil_Out32((CF_BASEADDR + CF_REG_DATA_SELECT), CF_DATA_SELECT_BIT(0));		// First byte from DDR appears on rising edge
	Xil_Out32((CF_BASEADDR + CF_REG_DELAY_CTRL), 0x00000);						// clear bits
	Xil_Out32((CF_BASEADDR + CF_REG_DELAY_CTRL), CF_DELAY_CTRL_SEL(1) |
											   CF_DELAY_CTRL_WR_ADDR(0xF) |		// delay write
											   CF_DELAY_CTRL_WR_DATA(0x1F));
	Xil_Out32((CF_BASEADDR + CF_REG_DELAY_CTRL), CF_DELAY_CTRL_SEL(1) |			// delay write
											   CF_DELAY_CTRL_WR_ADDR(0xF) |
											   CF_DELAY_CTRL_WR_DATA(0x1F));
	ad9467_test_mode(PN_23_SEQUENCE);		// Select PN 23 sequence test mode
	ad9467_output_format(OFFSET_BINARY);	// Select output format as offset binary
	ad9467_transfer();				  		// Synchronously update registers
	Xil_Out32((CF_BASEADDR + CF_REG_PN_TYPE), CF_PN_TYPE_BIT(1));
	ad9467_dco_clock_invert(0);		  		// Activates the non-inverted DCO clock
	ad9467_transfer();				  		// Synchronously update registers
	delay_ms(10);
	if (adc_delay())
	{
		ad9467_dco_clock_invert(1); 	// Activates the inverted DCO clock
		ad9467_transfer();				// Synchronously update registers
		xil_printf("AD9467[REG_OUT_PHASE]: %02x\n\r",
					ad9467_dco_clock_invert(-1));	 // reads the dco clock invert bit state
		delay_ms(10);
		if (adc_delay())
		{
			xil_printf("adc_setup: can not set a zero error delay!\n\r");
			adc_delay_1(0);
		}
	}
}

void DisplayTestMode(u32 mode, u32 format)
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
