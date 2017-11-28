/******************************************************************************
*   @file   ADF4106.c
*   @brief  Implementation of ADF4106 Driver for Microblaze processor.
*   @author Istvan Csomortani (istvan.csomortani@analog.com)
*
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
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
******************************************************************************/

/*****************************************************************************/
/****************************** Include Files ********************************/
/*****************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include "platform_drivers.h"
#include "adf4106.h"

#define DATA_MASK_MSB8      0xFF0000
#define DATA_OFFSET_MSB8    16
#define DATA_MASK_MID8      0xFF00
#define DATA_OFFSET_MID8    8
#define DATA_MASK_LSB8      0xFF
#define DATA_OFFSET_LSB8    0
#define ADDRESS_MASK        3

#define FREQ_2_4_GHZ        2400000000
#define FREQ_5_2_GHZ        5200000000

/******************************************************************************/
/************************** Constants Definitions *****************************/
/******************************************************************************/

const adf4106_chip_info CHIP_INFO[] = {
    [ID_ADF4001] = {
        .vcoMaxFrequency = 200000000,           // 200  Mhz
        .pfdMaxFrequency = 55000000,            // 55   Mhz
        .vcoMinFrequency = 5000000,             // 5    Mhz
        .pfdMinFrequency = 5000000,             // 5    Mhz
    },
    [ID_ADF4002] = {
        .vcoMaxFrequency = 400000000,           // 400  Mhz
        .pfdMaxFrequency = 104000000,           // 104  Mhz
        .vcoMinFrequency = 5000000,             // 5    Mhz
        .pfdMinFrequency = 20000000,            // 20   Mhz
    },
    [ID_ADF4106] = {
        .vcoMaxFrequency = 6000000000,          // 6    Ghz
        .pfdMaxFrequency = 104000000,           // 104  Mhz
        .vcoMinFrequency = 500000000,           // 500  Mhz
        .pfdMinFrequency = 20000000,            // 20   Mhz
    }
};

/*****************************************************************************/
/*************************** Functions definitions ***************************/
/*****************************************************************************/

/**************************************************************************//**
 * @brief Initialize SPI and Initial Values for ADF4106 Board.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return success
******************************************************************************/
char ADF4106_Init(ADF4106_dev **device,
		  ADF4106_init_param init_param)
{
	ADF4106_dev *dev;
    char status = -1;

	dev = (ADF4106_dev *)malloc(sizeof(*dev));
	if (!dev)
		return -1;

    /* lock the current device */
	dev->chip_info = CHIP_INFO[init_param.this_device];

	dev->ADF4106_st = init_param.ADF4106_st;
	dev->this_device = init_param.this_device;

	dev->rLatch = 0;
	dev->nLatch = 0;
	dev->fLatch = 0;
	dev->iLatch = 0;

    /* CPHA = 1; CPOL = 0; */
	status = spi_init(&dev->spi_desc, init_param.spi_init);

	/* GPIO */
	status |= gpio_get(&dev->gpio_le, init_param.gpio_le);
	status |= gpio_get(&dev->gpio_ce, init_param.gpio_ce);
	status |= gpio_get(&dev->gpio_le2, init_param.gpio_le2);
	status |= gpio_get(&dev->gpio_ce2, init_param.gpio_ce2);

    /* Bring CE high to put device to power up */
    ADF4106_CE_OUT;
    ADF4106_CE_HIGH;
    /* Initialize the load enable pin */
    ADF4106_LE_OUT;
    ADF4106_LE_LOW;

    /* Import the PFD max limit, from user set up */
    if(dev->chip_info.pfdMaxFrequency > dev->ADF4106_st.pfdMax)
    {
    	dev->chip_info.pfdMaxFrequency = dev->ADF4106_st.pfdMax;
    }

    switch(init_param.initMethod)
    {
        case INIT_LATCH :
            ADF4106_InitLatchMethod(dev);
            break;
        case INIT_CEPIN :
            ADF4106_InitCEPinMethod(dev);
            break;
        case INIT_COUNTER_RESET :
            ADF4106_InitCounteResetMethod(dev);
            break;
    }

	*device = dev;

return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ADF4106_Init().
 *
 * @param dev - The device structure.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ADF4106_remove(ADF4106_dev *dev)
{
	int32_t ret;

	ret = spi_remove(dev->spi_desc);

	ret |= gpio_remove(dev->gpio_le);
	ret |= gpio_remove(dev->gpio_ce);
	ret |= gpio_remove(dev->gpio_le2);
	ret |= gpio_remove(dev->gpio_ce2);

	free(dev);

	return ret;
}

/**************************************************************************//**
 * @brief Update one of the latch via the SPI interface
 *
 * @param dev         - The device structure.
 * @param   latchData - the data which will be written to the latch
 *
 * @return
******************************************************************************/
void ADF4106_UpdateLatch(ADF4106_dev *dev,
			 unsigned long latchData)
{
    unsigned char dataBuffer[3] = {0,};
    unsigned char latchType = latchData & 0x3;

    /* Update the internal buffers */
    switch(latchType)
    {
        case ADF4106_CTRL_R_COUNTER :
            dev->rLatch = latchData;
            break;
        case ADF4106_CTRL_N_COUNTER :
            dev->nLatch = latchData;
            break;
        case ADF4106_CTRL_FUNCTION_LATCH :
            dev->fLatch = latchData;
            break;
        case ADF4106_CTRL_INIT_LATCH :
            dev->iLatch = latchData;
            dev->fLatch = ADF4106_CTRL_FUNCTION_LATCH |
                    (latchData & ~ADF4106_CTRL_MASK);
            break;
    }

    dataBuffer[0] = (latchData & DATA_MASK_MSB8) >> DATA_OFFSET_MSB8;
    dataBuffer[1] = (latchData & DATA_MASK_MID8) >> DATA_OFFSET_MID8;
    dataBuffer[2] = (latchData & DATA_MASK_LSB8) >> DATA_OFFSET_LSB8;

	spi_write_and_read(dev->spi_desc,
			   dataBuffer,
			   3);

    /* Generate a load pulse */
    ADF4106_LE_HIGH;
    ADF4106_LE_LOW;
}

/**************************************************************************//**
 * @brief Initialization latch method
 *
 * @param dev - The device structure.
 *
 * @return
******************************************************************************/
void ADF4106_InitLatchMethod(ADF4106_dev *dev)
{
    /* Program initialization latch (COUNTER RESET bit is zero) */
    ADF4106_UpdateLatch(dev,
			ADF4106_CTRL_INIT_LATCH | \
                        ADF4106_CR(dev->ADF4106_st.counterReset) | \
                        ADF4106_PD1(dev->ADF4106_st.powerDown1) | \
                        ADF4106_MUXOUT(dev->ADF4106_st.muxoutControl) | \
                        ADF4106_PDPOL(dev->ADF4106_st.phaseDetectorPol) | \
                        ADF4106_CP(dev->ADF4106_st.cpType) | \
                        ADF4106_FASTLOCK(dev->ADF4106_st.fastLockMode) | \
                        ADF4106_TCC(dev->ADF4106_st.timerCounterControl) |\
                        ADF4106_CS1(dev->ADF4106_st.currentSetting1) | \
                        ADF4106_CS2(dev->ADF4106_st.currentSetting2) | \
                        ADF4106_PD2(dev->ADF4106_st.powerDown2));

    /* Do an R load */
    ADF4106_UpdateLatch(dev,
			ADF4106_CTRL_R_COUNTER | \
                        ADF4106_R_COUNTER(dev->ADF4106_st.refCounter) | \
                        ADF4106_R_ABP(dev->ADF4106_st.antiBacklashWidth)| \
                        ADF4106_R_TMB(dev->ADF4106_st.testModeBits) | \
                        ADF4106_R_LDP(dev->ADF4106_st.lockDetectPrecision));

    /* Do an N load */
    ADF4106_UpdateLatch(dev,
			ADF4106_CTRL_N_COUNTER | \
                        ADF4106_N_COUNTER_B(dev->ADF4106_st.bNCounter) | \
                        ADF4106_N_CP(dev->ADF4106_st.cpGain));

    /* Update the local variable for Function Latch */
    dev->fLatch = ADF4106_CTRL_FUNCTION_LATCH | (dev->iLatch & 0xFFFFFFFC);
}

/**************************************************************************//**
 * @brief CE Pin method
 *
 * @param dev - The device structure.
 *
 * @return
******************************************************************************/
void ADF4106_InitCEPinMethod(ADF4106_dev *dev)
{
    /* Bring CE low to put the device into power down. */
    ADF4106_CE_LOW;
    /* Program the function latch */
    ADF4106_UpdateLatch(dev,
			ADF4106_CTRL_FUNCTION_LATCH | \
                        ADF4106_CR(dev->ADF4106_st.counterReset) | \
                        ADF4106_PD1(dev->ADF4106_st.powerDown1) | \
                        ADF4106_MUXOUT(dev->ADF4106_st.muxoutControl) | \
                        ADF4106_PDPOL(dev->ADF4106_st.phaseDetectorPol) | \
                        ADF4106_CP(dev->ADF4106_st.cpType) | \
                        ADF4106_FASTLOCK(dev->ADF4106_st.fastLockMode) | \
                        ADF4106_TCC(dev->ADF4106_st.timerCounterControl) |\
                        ADF4106_CS1(dev->ADF4106_st.currentSetting1) | \
                        ADF4106_CS2(dev->ADF4106_st.currentSetting2) | \
                        ADF4106_PD2(dev->ADF4106_st.powerDown2));
    /* Program the R counter latch */
    ADF4106_UpdateLatch(dev,
			ADF4106_CTRL_R_COUNTER | \
                        ADF4106_R_COUNTER(dev->ADF4106_st.refCounter) | \
                        ADF4106_R_ABP(dev->ADF4106_st.antiBacklashWidth)| \
                        ADF4106_R_TMB(dev->ADF4106_st.testModeBits) | \
                        ADF4106_R_LDP(dev->ADF4106_st.lockDetectPrecision));
    /* Program the N counter latch */
    ADF4106_UpdateLatch(dev,
			ADF4106_CTRL_N_COUNTER | \
			ADF4106_N_COUNTER_B(dev->ADF4106_st.bNCounter) | \
			ADF4106_N_CP(dev->ADF4106_st.cpGain));
    /* Bring CE high to take the device out of power-down */
    ADF4106_CE_HIGH;
    /* Wait for the input buffer bias to reach steady state */
    mdelay(1);
}

/**************************************************************************//**
 * @brief Counter reset method
 *
 * @param dev - The device structure.
 *
 * @return
******************************************************************************/
void ADF4106_InitCounteResetMethod(ADF4106_dev *dev)
{
    /* Program the function latch, enable the counter reset */
    ADF4106_UpdateLatch(dev,
			ADF4106_CTRL_FUNCTION_LATCH | \
                        ADF4106_CR_RESET | \
                        ADF4106_PD1(dev->ADF4106_st.powerDown1) | \
                        ADF4106_MUXOUT(dev->ADF4106_st.muxoutControl) | \
                        ADF4106_PDPOL(dev->ADF4106_st.phaseDetectorPol) | \
                        ADF4106_CP(dev->ADF4106_st.cpType) | \
                        ADF4106_FASTLOCK(dev->ADF4106_st.fastLockMode) | \
                        ADF4106_TCC(dev->ADF4106_st.timerCounterControl) |\
                        ADF4106_CS1(dev->ADF4106_st.currentSetting1) | \
                        ADF4106_CS2(dev->ADF4106_st.currentSetting2) | \
                        ADF4106_PD2(dev->ADF4106_st.powerDown2));
    /* Program the R counter latch */
    ADF4106_UpdateLatch(dev,
			ADF4106_CTRL_R_COUNTER | \
                        ADF4106_R_COUNTER(dev->ADF4106_st.refCounter) | \
                        ADF4106_R_ABP(dev->ADF4106_st.antiBacklashWidth)| \
                        ADF4106_R_TMB(dev->ADF4106_st.testModeBits) | \
                        ADF4106_R_LDP(dev->ADF4106_st.lockDetectPrecision));
    /* Program the N counter latch */
    ADF4106_UpdateLatch(dev,
			ADF4106_CTRL_N_COUNTER | \
			ADF4106_N_COUNTER_B(dev->ADF4106_st.bNCounter) | \
			ADF4106_N_CP(dev->ADF4106_st.cpGain));
    /* Do a Function Load, this time disable the counter reset */
    ADF4106_UpdateLatch(dev,
			ADF4106_CTRL_FUNCTION_LATCH | \
                        ADF4106_CR_NORMAL | \
                        ADF4106_PD1(dev->ADF4106_st.powerDown1) | \
                        ADF4106_MUXOUT(dev->ADF4106_st.muxoutControl) | \
                        ADF4106_PDPOL(dev->ADF4106_st.phaseDetectorPol) | \
                        ADF4106_CP(dev->ADF4106_st.cpType) | \
                        ADF4106_FASTLOCK(dev->ADF4106_st.fastLockMode) | \
                        ADF4106_TCC(dev->ADF4106_st.timerCounterControl) |\
                        ADF4106_CS1(dev->ADF4106_st.currentSetting1) | \
                        ADF4106_CS2(dev->ADF4106_st.currentSetting2) | \
                        ADF4106_PD2(dev->ADF4106_st.powerDown2));
}

/***************************************************************************//**
 * @brief Return the value of a desired latch
 *
 * @param dev        - The device structure.
 * @param   param[0] - the type of the latch:
 *                      0 - 'ADF4106_CTRL_R_COUNTER'
 *                      1 - 'ADF4106_CTRL_N_COUNTER'
 *                      2 - 'ADF4106_CTRL_FUNCTION_LATCH'
 *                      3 - 'ADF4106_CTRL_INIT_LATCH'
 *
 * @return latchValue - the value of the desired latch
*******************************************************************************/
unsigned long ADF4106_ReadLatch(ADF4106_dev *dev,
				unsigned char latchType)
{
    switch(latchType)
    {
        case ADF4106_CTRL_R_COUNTER :
            return dev->rLatch;

        case ADF4106_CTRL_N_COUNTER :
            return dev->nLatch;

        case ADF4106_CTRL_FUNCTION_LATCH :
            return dev->fLatch;

        case ADF4106_CTRL_INIT_LATCH :
            return dev->iLatch;

        default :
            return -1;
    }
}

/***************************************************************************//**
 * @brief Increases the R counter value until the ADF4106_PDF_MAX_FREQ is
 *        greater than PFD frequency.
 *
 * @param dev      - The device structure.
 * @param rCounter - R counter value.
 *
 * @return rCounter - modified R counter value.
*******************************************************************************/
unsigned short ADF4106_TuneRcounter(ADF4106_dev *dev,
				    unsigned short rCounter)
{
    unsigned long frequencyPfd = 0; // PFD frequency

    do
    {
        rCounter++;
        frequencyPfd = dev->ADF4106_st.refIn / rCounter;
    }
    while(frequencyPfd > dev->chip_info.pfdMaxFrequency);

    return rCounter;
}

/***************************************************************************//**
 * @brief Sets the output frequency.
 *
 * @param dev       - The device structure.
 * @param frequency - The desired frequency value.
 *
 * @return calculatedFrequency - The actual frequency value that was set.
*******************************************************************************/
unsigned long long ADF4106_SetFrequency(ADF4106_dev *dev,
					unsigned long long frequency)
{
    unsigned long long vcoFrequency        = 0; // VCO frequency
    unsigned long      frequencyPfd        = 0; // PFD frequency
    unsigned long      freqRatio           = 0; // VCOfreq / PFDfreq
    unsigned long long calculatedFrequency = 0; // Actual VCO frequency
    unsigned short     rCounterValue       = 0; // Value for R counter
    unsigned short     a                   = 0; // Value for A counter
    unsigned short     b                   = 0; // Value for B counter
    unsigned char      devicePrescaler     = 0;
    unsigned char      userPrescaler       = 0; // prescaler defined by user

    /* Force "frequency" parameter to the [minMHz...maxGHz] interval. */
    if(frequency <= dev->chip_info.vcoMaxFrequency)
    {
        if(frequency >= dev->chip_info.vcoMinFrequency)
        {
            vcoFrequency = frequency;
        }
        else
        {
            vcoFrequency = dev->chip_info.vcoMinFrequency;
        }
    }
    else
    {
        vcoFrequency = dev->chip_info.vcoMaxFrequency;
    }
    do
    {
        /* Get the actual PFD frequency. */
        rCounterValue = ADF4106_TuneRcounter(dev,
					     rCounterValue);
        frequencyPfd = dev->ADF4106_st.refIn / rCounterValue;
        /* Define the value of the prescaler
        *  By default the prescaler is constant for ADF4001 and ADF4002
        *  In case of ADF4106, the minimum admitted prescaler will be selected,
        *  or the user defined prescaler if it is valid (big enough)
        *  Valid min prescaler : VCO < 2.4Ghz -> 8/9
        *                        VCO > 2.4Ghz and VCO < 5.4Ghz -> 16/17
        *                        VCO > 5.4Ghz -> 32/33
        */
        userPrescaler = ADF4106_PRESCALE(dev->fLatch >> ADF4106_PS_OFFSET);
        devicePrescaler = (dev->this_device != ID_ADF4106) ? \
                                ADF4106_PRESCALE(ADF4106_PS_8_9) : \
                         (vcoFrequency <= FREQ_2_4_GHZ) ? \
                                ADF4106_PRESCALE(ADF4106_PS_8_9) : \
                         ((vcoFrequency > FREQ_2_4_GHZ) & \
                          (vcoFrequency < FREQ_5_2_GHZ)) ? \
                                ADF4106_PRESCALE(ADF4106_PS_16_17) : \
                                ADF4106_PRESCALE(ADF4106_PS_32_33);
        if(devicePrescaler < userPrescaler)
        {
            devicePrescaler = userPrescaler;
        }
        /* Find the values for Counter A and Counter B using VCO frequency and
        *  PFD frequency.
        */
        freqRatio = (unsigned short)((float)vcoFrequency / frequencyPfd);
        b = freqRatio / devicePrescaler;
        a = freqRatio % devicePrescaler;
    }while(a > b); // B must be greater or equal to A
    /* Find the actual VCO frequency. */
    calculatedFrequency = (unsigned long long)((b * devicePrescaler) + a) * frequencyPfd;
    /* Load the saved values into the registers using Counter Reset Method. */
    ADF4106_UpdateLatch(dev,
			ADF4106_CTRL_FUNCTION_LATCH |
                        dev->fLatch |
                        ADF4106_CR(ADF4106_CR_RESET));

    dev->rLatch &= ~ADF4106_R_COUNTER(ADF4106_R_COUNTER_MASK);
    ADF4106_UpdateLatch(dev,
			ADF4106_CTRL_R_COUNTER |
                        dev->rLatch |
                        ADF4106_R_COUNTER(rCounterValue));

    dev->nLatch &= ~(ADF4106_N_COUNTER_A(ADF4106_N_COUNTER_A_MASK) |
    			ADF4106_N_COUNTER_B(ADF4106_N_COUNTER_B_MASK));
    if(dev->this_device == ID_ADF4106)
    {
        ADF4106_UpdateLatch(dev,
			    ADF4106_CTRL_N_COUNTER |
                            dev->nLatch |
                            ADF4106_N_COUNTER_A(a) |
                            ADF4106_N_COUNTER_B(b));
    }
    else
    {
        ADF4106_UpdateLatch(dev,
			    ADF4106_CTRL_N_COUNTER |
                            dev->nLatch |
                            ADF4106_N_COUNTER_B((b * devicePrescaler) + a));
    }

    dev->fLatch &= ~ADF4106_CR(ADF4106_CR_MASK);
    ADF4106_UpdateLatch(dev,
			ADF4106_CTRL_FUNCTION_LATCH |
                        dev->fLatch |
                        ADF4106_CR(ADF4106_CR_NORMAL));

    return calculatedFrequency;
}
