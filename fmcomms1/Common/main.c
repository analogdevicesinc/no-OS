/**************************************************************************//**
*   @file   main.c
*   @brief  XCOMM main program implementation.
*   @author acozma (andrei.cozma@analog.com)
*
*******************************************************************************
* Copyright 2011-2015(c) Analog Devices, Inc.
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
#include "xil_cache.h"
#include "xcomm.h"
#include "xparameters.h"
#include "test.h"

extern void xil_printf(const char *ctrl1, ...);

/* Program main loop. */
int main()
{
    int32_t ret;
    int32_t mode = 0;
    float retGain;
    uint64_t retFreqRx;
    uint64_t retFreqTx;
    int32_t fmcSel;
    int32_t i;
    int32_t valArray[17];
    XCOMM_Version boardVersion;
    XCOMM_DefaultInit defInit = {FMC_LPC,		//fmcPort
    							 XILINX_ZC706,	//carrierBoard
    							 122880000,		//adcSamplingRate
    							 122880000,		//dacSamplingRate
								 4500,			//rxGain1000
								 2400000000ull, //rxFrequency
								 2400000000ull};//txFrequency

    Xil_ICacheEnable();
    Xil_DCacheEnable();

    xil_printf("Running XCOMM Test Program\n\r");

    if(defInit.carrierBoard == XILINX_ZC702)
    {
    	fmcSel = (defInit.fmcPort == FMC_LPC ? IICSEL_B0LPC_PS7 : IICSEL_B1HPC_PS7);
    }
    else
    {
        if(defInit.carrierBoard == XILINX_ZC706)
        {
        	fmcSel = (defInit.fmcPort == FMC_LPC ? IICSEL_B0LPC_PS7_ZC706 :
        										   IICSEL_B1HPC_PS7_ZC706);
        }
        else
        {
        	fmcSel = (defInit.fmcPort == FMC_LPC ? IICSEL_B0LPC_AXI : IICSEL_B1HPC_AXI);
        }
    }

    xil_printf("\n\rInitializing XCOMM I2C...\n\r");
    ret = XCOMM_InitI2C(&defInit);
	if(ret < 0)
	{
		xil_printf("XCOMM Init I2C Failed!\n\r");
		return 0;
	}
	else
	{
		xil_printf("XCOMM Init I2C OK!\n\r");
	}

    xil_printf("\n\rGetting XCOMM Revision...\n\r");
    boardVersion = XCOMM_GetBoardVersion(XCOMM_ReadMode_FromHW);
    if(boardVersion.error == -1)
    {
    	xil_printf("\n\rGetting XCOMM Revision Failed!\n\r");
    }
    else
    {
    	xil_printf("Board Version: %s\n\r", boardVersion.value);
    }

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

	xil_printf("\n\rInitializing the Rx path...\n\r");
	ret = XCOMM_InitRx(&defInit);
	if(ret < 0)
	{
		xil_printf("XCOMM Rx Init Failed!\n\r");
		return 0;
	}
	else
	{
		xil_printf("XCOMM Rx Init OK!\n\r");
	}

	xil_printf("\n\rInitializing the Tx path...\n\r");
	ret = XCOMM_InitTx(&defInit);
	if(ret < 0)
	{
		xil_printf("XCOMM Tx Init Failed!\n\r");
		return 0;
	}
	else
	{
		xil_printf("XCOMM Tx Init OK!\n\r");
	}

	xil_printf("\n\rADC sampling rate [Hz]: ");
	ret = XCOMM_GetAdcSamplingRate(XCOMM_ReadMode_FromHW);
	xil_printf("%d \n\r", ret);

	xil_printf("\n\rDAC sampling rate [Hz]: ");
	ret = XCOMM_GetDacSamplingRate(XCOMM_ReadMode_FromHW);
	xil_printf("%d \n\r", ret);

	xil_printf("\n\rDAC available interpolation frequencies [Hz]: ");
	XCOMM_GetDacAvailableInterpolationFreq(valArray);
	i = 0;
	while((valArray[i] != 0) && (i < 5))
	{
		xil_printf("%d ", valArray[i]);
		i++;
	}
	xil_printf("\n\r");
	XCOMM_SetDacInterpolationFreq(valArray[i - 1]);

	xil_printf("\n\rDAC available center shift frequencies [Hz]: ");
	XCOMM_GetDacAvailableCenterShiftFreq(valArray);
	i = 0;
	while((valArray[i] != -1) && (i < 17))
	{
		xil_printf("%d ", valArray[i]);
		i++;
	}
	xil_printf("\n\r");

    xil_printf("\n\rTesting the ADC communication... \n\r");
    XCOMM_SetAdcTestMode(0x01, XCOMM_AdcChannel_All);
    adc_capture(fmcSel, 1024, DDR_BASEADDR);

	for (mode = 0x1; mode <= 0x7; mode++)
	{
		XCOMM_SetAdcTestMode(mode, XCOMM_AdcChannel_All);
		adc_test(fmcSel, mode, 0x1);
	}
    xil_printf("ADC test complete.\n\r");

    xil_printf("\n\rTesting the DAC communication... \n\r");
    dac_test(fmcSel);
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

#ifndef DAC_DMA
    xil_printf("\n\rSetting up the DDS... \n\r");
    dds_setup(fmcSel, 1000000, 1000000);
    xil_printf("DDS setup complete.\n\r");
#else
    xil_printf("\n\rSetting up the DAC DMA... \n\r");
    dac_dma_setup(fmcSel);
    xil_printf("DAC DMA setup complete.\n\r");
#endif

    xil_printf("\n\rReading data from air... \n\r");
    XCOMM_SetAdcTestMode(XCOMM_AdcTestMode_Off, XCOMM_AdcChannel_All);
    delay_ms(1000);
    adc_capture(fmcSel, 16384, DDR_BASEADDR);
    xil_printf("Read data from air complete. \n\r");

    xil_printf("\n\rFinished XCOMM Test Program\n\r");

    Xil_DCacheDisable();
    Xil_ICacheDisable();

    return 0;
}
