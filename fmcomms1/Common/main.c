/**************************************************************************//**
*   @file   main.c
*   @brief  XCOMM main program implementation.
*   @author acozma (andrei.cozma@analog.com)
*
*******************************************************************************
* Copyright 2011(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************
*   SVN Revision: $WCREV$
******************************************************************************/

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include "platform.h"
#include "xcomm.h"
#include "xparameters.h"
#include "test.h"

int main()
{
    int ret;
    int mode = 0;
	XCOMM_DefaultInit defInit = {100000000,		//adcSamplingRate
								 100000000,		//dacSamplingRate
								 20000,			//rxGain1000
								 2400000000ull, //rxFrequency
								 2400000000ull};//rxFrequency
    float retGain;
    uint64_t retFreqRx;
    uint64_t retFreqTx;

    init_platform();

    xil_printf("Running XCOMM Test Program\n\r");

    xil_printf("\n\rInitializing XCOMM Components...\n\r");
    ret = XCOMM_Init(&defInit);
	if(ret < 0)
	{
		xil_printf("XCOMM Init Failed!\n\r");
		return 0;
	}
	else
	{
		xil_printf("XCOMM Init OK!\n\r");
	}

    xil_printf("\n\rTesting the ADC communication... \n\r");
    
    XCOMM_SetAdcTestMode(0x01, XCOMM_AdcChannel_All);
    adc_capture(IICSEL_B0LPC, 1024, DDR_BASEADDR);

	for (mode = 0x1; mode <= 0x7; mode++)
	{
		XCOMM_SetAdcTestMode(mode, XCOMM_AdcChannel_All);
		adc_test(IICSEL_B0LPC, mode, 0x1);
	}
    xil_printf("ADC test complete.\n\r");

    xil_printf("\n\rTesting the DAC communication... \n\r");
    dac_test(IICSEL_B0LPC);
    xil_printf("DAC test complete.\n\r");

	xil_printf("\n\rSetting the VGA gain to: %d.%d dB\n\r", (int)defInit.rxGain1000/1000, (int)((defInit.rxGain1000 - (int)(defInit.rxGain1000/1000)*1000)));
	retGain = (float)XCOMM_SetRxGain(defInit.rxGain1000) / 1000.0f;
	xil_printf("Actual set VGA gain: %d.%d dB\n\r", (int)retGain, (int)((retGain - (int)retGain) * 1000));

	xil_printf("\n\rSetting the Rx frequency to: %lld%06lld\n\r", defInit.rxFrequency/(uint64_t)1e6, defInit.rxFrequency%(uint64_t)1e6);
    retFreqRx = XCOMM_SetRxFrequency(defInit.rxFrequency);
    xil_printf("Actual set Rx frequency: %lld%06lld\n\r", retFreqRx/(uint64_t)1e6, retFreqRx%(uint64_t)1e6);

	xil_printf("\n\rSetting the Tx frequency to: %lld%06lld\n\r", defInit.txFrequency/(uint64_t)1e6, defInit.txFrequency%(uint64_t)1e6);
    retFreqTx = XCOMM_SetTxFrequency(defInit.txFrequency);
    xil_printf("Actual set Tx frequency: %lld%06lld\n\r", retFreqTx/(uint64_t)1e6, retFreqTx%(uint64_t)1e6);

    xil_printf("\n\rSetting up the DDS... \n\r");
    dds_setup(IICSEL_B0LPC, 45, 45);
    xil_printf("DDS setup complete.\n\r");

    xil_printf("\n\rReading data from air... \n\r");
    XCOMM_SetAdcTestMode(XCOMM_AdcTestMode_Off, XCOMM_AdcChannel_All);
    while(1)
    {
    	adc_capture(IICSEL_B0LPC, 1024, DDR_BASEADDR);
    }
    xil_printf("Read data from air complete. \n\r");

    xil_printf("\n\rFinished XCOMM Test Program\n\r");

	cleanup_platform();

    return 0;
}
