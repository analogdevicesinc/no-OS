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
#include "adf4106.h"
#include "Communication.h"
#include "TIME.h"

#define DATA_MASK_MSB8      0xFF0000
#define DATA_OFFSET_MSB8    16
#define DATA_MASK_MID8      0xFF00
#define DATA_OFFSET_MID8    8
#define DATA_MASK_LSB8      0xFF
#define DATA_OFFSET_LSB8    0
#define ADDRESS_MASK        3

#define FREQ_2_4_GHZ        2400000000
#define FREQ_5_2_GHZ        5200000000

/*****************************************************************************/
/**************************** Private variables ******************************/
/*****************************************************************************/
struct adf4106_chip_info {
    unsigned long long vcoMaxFrequency;
    unsigned long pfdMaxFrequency;
    unsigned long vcoMinFrequency;
    unsigned long pfdMinFrequency;
};

struct adf4106_chip_info ADF4106_CHIP_INFO[] = {
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

/* Actual device type */
ADF4106_type_t this_device;

/* Reference input frequency */
unsigned long refIn = 0;

/* Internal buffers for each latch */
unsigned long rLatch = 0;
unsigned long nLatch = 0;
unsigned long fLatch = 0;
unsigned long iLatch = 0;

/*****************************************************************************/
/*************************** Functions definitions ***************************/
/*****************************************************************************/

/**************************************************************************//**
 * @brief Initialize SPI and Initial Values for ADF4106 Board.
 *
 * @param device        - The current device type.
 *        initMethod    - Initialization method
 *                        0 - 'INIT_LATCH',
 *                        1 - 'INIT_CEPIN',
 *                        2 - 'INIT_COUNTER_RESET'
 *        ADF4106_st    - the structure with the initial set up values of the
 *                        registers
 *
 * @return success
******************************************************************************/
char ADF4106_Init(ADF4106_type_t device,
                  ADF4106_init_t initMethod,
                  ADF4106_settings_t ADF4106_st)
{
    char status = -1;

    /* lock the current device */
    this_device = device;

    /* CPHA = 1; CPOL = 0; */
    status = SPI_Init(0, 100000, 0, 1);

    /* Bring CE high to put device to power up */
    ADF4106_CE_OUT;
    ADF4106_CE_HIGH;
    /* Initialize the load enable pin */
    ADF4106_LE_OUT;
    ADF4106_LE_LOW;

    /* Set up the reference input frequency */
    refIn = ADF4106_st.refIn;

    /* Import the PFD max limit, from user set up */
    if(ADF4106_CHIP_INFO[this_device].pfdMaxFrequency > ADF4106_st.pfdMax)
    {
    	ADF4106_CHIP_INFO[this_device].pfdMaxFrequency = ADF4106_st.pfdMax;
    }

    switch(initMethod)
    {
        case INIT_LATCH :
            ADF4106_InitLatchMethod(ADF4106_st);
            break;
        case INIT_CEPIN :
            ADF4106_InitCEPinMethod(ADF4106_st);
            break;
        case INIT_COUNTER_RESET :
            ADF4106_InitCounteResetMethod(ADF4106_st);
            break;
    }

return status;
}

/**************************************************************************//**
 * @brief Update one of the latch via the SPI interface
 *
 * @param   latchData - the data which will be written to the latch
 *
 * @return
******************************************************************************/
void ADF4106_UpdateLatch(unsigned long latchData)
{
    unsigned char dataBuffer[3] = {0,};
    unsigned char latchType = latchData & 0x3;

    /* Update the internal buffers */
    switch(latchType)
    {
        case ADF4106_CTRL_R_COUNTER :
            rLatch = latchData;
            break;
        case ADF4106_CTRL_N_COUNTER :
            nLatch = latchData;
            break;
        case ADF4106_CTRL_FUNCTION_LATCH :
            fLatch = latchData;
            break;
        case ADF4106_CTRL_INIT_LATCH :
            iLatch = latchData;
            fLatch = ADF4106_CTRL_FUNCTION_LATCH |
                    (latchData & ~ADF4106_CTRL_MASK);
            break;
    }

    dataBuffer[0] = (latchData & DATA_MASK_MSB8) >> DATA_OFFSET_MSB8;
    dataBuffer[1] = (latchData & DATA_MASK_MID8) >> DATA_OFFSET_MID8;
    dataBuffer[2] = (latchData & DATA_MASK_LSB8) >> DATA_OFFSET_LSB8;

    SPI_Write(ADF4106_SLAVE_ID,
              dataBuffer,
              3);

    /* Generate a load pulse */
    ADF4106_LE_HIGH;
    ADF4106_LE_LOW;
}

/**************************************************************************//**
 * @brief Initialization latch method
 *
 * @param None
 *
 * @return
******************************************************************************/
void ADF4106_InitLatchMethod(ADF4106_settings_t ADF4106_st)
{
    /* Program initialization latch (COUNTER RESET bit is zero) */
    ADF4106_UpdateLatch(ADF4106_CTRL_INIT_LATCH | \
                        ADF4106_CR(ADF4106_st.counterReset) | \
                        ADF4106_PD1(ADF4106_st.powerDown1) | \
                        ADF4106_MUXOUT(ADF4106_st.muxoutControl) | \
                        ADF4106_PDPOL(ADF4106_st.phaseDetectorPol) | \
                        ADF4106_CP(ADF4106_st.cpType) | \
                        ADF4106_FASTLOCK(ADF4106_st.fastLockMode) | \
                        ADF4106_TCC(ADF4106_st.timerCounterControl) |\
                        ADF4106_CS1(ADF4106_st.currentSetting1) | \
                        ADF4106_CS2(ADF4106_st.currentSetting2) | \
                        ADF4106_PD2(ADF4106_st.powerDown2));

    /* Do an R load */
    ADF4106_UpdateLatch(ADF4106_CTRL_R_COUNTER | \
                        ADF4106_R_COUNTER(ADF4106_st.refCounter) | \
                        ADF4106_R_ABP(ADF4106_st.antiBacklashWidth)| \
                        ADF4106_R_TMB(ADF4106_st.testModeBits) | \
                        ADF4106_R_LDP(ADF4106_st.lockDetectPrecision));

    /* Do an N load */
    ADF4106_UpdateLatch(ADF4106_CTRL_N_COUNTER | \
                        ADF4106_N_COUNTER_B(ADF4106_st.bNCounter) | \
                        ADF4106_N_CP(ADF4106_st.cpGain));

    /* Update the local variable for Function Latch */
    fLatch = ADF4106_CTRL_FUNCTION_LATCH | (iLatch & 0xFFFFFFFC);
}

/**************************************************************************//**
 * @brief CE Pin method
 *
 * @param None
 *
 * @return
******************************************************************************/
void ADF4106_InitCEPinMethod(ADF4106_settings_t ADF4106_st)
{
    /* Bring CE low to put the device into power down. */
    ADF4106_CE_LOW;
    /* Program the function latch */
    ADF4106_UpdateLatch(ADF4106_CTRL_FUNCTION_LATCH | \
                        ADF4106_CR(ADF4106_st.counterReset) | \
                        ADF4106_PD1(ADF4106_st.powerDown1) | \
                        ADF4106_MUXOUT(ADF4106_st.muxoutControl) | \
                        ADF4106_PDPOL(ADF4106_st.phaseDetectorPol) | \
                        ADF4106_CP(ADF4106_st.cpType) | \
                        ADF4106_FASTLOCK(ADF4106_st.fastLockMode) | \
                        ADF4106_TCC(ADF4106_st.timerCounterControl) |\
                        ADF4106_CS1(ADF4106_st.currentSetting1) | \
                        ADF4106_CS2(ADF4106_st.currentSetting2) | \
                        ADF4106_PD2(ADF4106_st.powerDown2));
    /* Program the R counter latch */
    ADF4106_UpdateLatch(ADF4106_CTRL_R_COUNTER | \
                        ADF4106_R_COUNTER(ADF4106_st.refCounter) | \
                        ADF4106_R_ABP(ADF4106_st.antiBacklashWidth)| \
                        ADF4106_R_TMB(ADF4106_st.testModeBits) | \
                        ADF4106_R_LDP(ADF4106_st.lockDetectPrecision));
    /* Program the N counter latch */
    ADF4106_UpdateLatch(ADF4106_CTRL_N_COUNTER | \
                         ADF4106_N_COUNTER_B(ADF4106_st.bNCounter) | \
                         ADF4106_N_CP(ADF4106_st.cpGain));
    /* Bring CE high to take the device out of power-down */
    ADF4106_CE_HIGH;
    /* Wait for the input buffer bias to reach steady state */
    TIME_DelayUs(1);
}

/**************************************************************************//**
 * @brief Counter reset method
 *
 * @param None
 *
 * @return
******************************************************************************/
void ADF4106_InitCounteResetMethod(ADF4106_settings_t ADF4106_st)
{
    /* Program the function latch, enable the counter reset */
    ADF4106_UpdateLatch(ADF4106_CTRL_FUNCTION_LATCH | \
                        ADF4106_CR_RESET | \
                        ADF4106_PD1(ADF4106_st.powerDown1) | \
                        ADF4106_MUXOUT(ADF4106_st.muxoutControl) | \
                        ADF4106_PDPOL(ADF4106_st.phaseDetectorPol) | \
                        ADF4106_CP(ADF4106_st.cpType) | \
                        ADF4106_FASTLOCK(ADF4106_st.fastLockMode) | \
                        ADF4106_TCC(ADF4106_st.timerCounterControl) |\
                        ADF4106_CS1(ADF4106_st.currentSetting1) | \
                        ADF4106_CS2(ADF4106_st.currentSetting2) | \
                        ADF4106_PD2(ADF4106_st.powerDown2));
    /* Program the R counter latch */
    ADF4106_UpdateLatch(ADF4106_CTRL_R_COUNTER | \
                        ADF4106_R_COUNTER(ADF4106_st.refCounter) | \
                        ADF4106_R_ABP(ADF4106_st.antiBacklashWidth)| \
                        ADF4106_R_TMB(ADF4106_st.testModeBits) | \
                        ADF4106_R_LDP(ADF4106_st.lockDetectPrecision));
    /* Program the N counter latch */
    ADF4106_UpdateLatch(ADF4106_CTRL_N_COUNTER | \
                         ADF4106_N_COUNTER_B(ADF4106_st.bNCounter) | \
                         ADF4106_N_CP(ADF4106_st.cpGain));
    /* Do a Function Load, this time disable the counter reset */
    ADF4106_UpdateLatch(ADF4106_CTRL_FUNCTION_LATCH | \
                        ADF4106_CR_NORMAL | \
                        ADF4106_PD1(ADF4106_st.powerDown1) | \
                        ADF4106_MUXOUT(ADF4106_st.muxoutControl) | \
                        ADF4106_PDPOL(ADF4106_st.phaseDetectorPol) | \
                        ADF4106_CP(ADF4106_st.cpType) | \
                        ADF4106_FASTLOCK(ADF4106_st.fastLockMode) | \
                        ADF4106_TCC(ADF4106_st.timerCounterControl) |\
                        ADF4106_CS1(ADF4106_st.currentSetting1) | \
                        ADF4106_CS2(ADF4106_st.currentSetting2) | \
                        ADF4106_PD2(ADF4106_st.powerDown2));
}

/***************************************************************************//**
 * @brief Return the value of a desired latch
 *
 * @param   param[0] - the type of the latch:
 *                      0 - 'ADF4106_CTRL_R_COUNTER'
 *                      1 - 'ADF4106_CTRL_N_COUNTER'
 *                      2 - 'ADF4106_CTRL_FUNCTION_LATCH'
 *                      3 - 'ADF4106_CTRL_INIT_LATCH'
 *
 * @return latchValue - the value of the desired latch
*******************************************************************************/
unsigned long ADF4106_ReadLatch(unsigned char latchType)
{
    switch(latchType)
    {
        case ADF4106_CTRL_R_COUNTER :
            return rLatch;

        case ADF4106_CTRL_N_COUNTER :
            return nLatch;

        case ADF4106_CTRL_FUNCTION_LATCH :
            return fLatch;

        case ADF4106_CTRL_INIT_LATCH :
            return iLatch;

        default :
            return -1;
    }
}

/***************************************************************************//**
 * @brief Increases the R counter value until the ADF4106_PDF_MAX_FREQ is
 *        greater than PFD frequency.
 *
 * @param rCounter - R counter value.
 *
 * @return rCounter - modified R counter value.
*******************************************************************************/
unsigned short ADF4106_TuneRcounter(unsigned short rCounter)
{
    unsigned long frequencyPfd = 0; // PFD frequency

    do
    {
        rCounter++;
        frequencyPfd = refIn / rCounter;
    }
    while(frequencyPfd > ADF4106_CHIP_INFO[this_device].pfdMaxFrequency);

    return rCounter;
}

/***************************************************************************//**
 * @brief Sets the output frequency.
 *
 * @param frequency - The desired frequency value.
 *
 * @return calculatedFrequency - The actual frequency value that was set.
*******************************************************************************/
unsigned long long ADF4106_SetFrequency(unsigned long long frequency)
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
    if(frequency <= ADF4106_CHIP_INFO[this_device].vcoMaxFrequency)
    {
        if(frequency >= ADF4106_CHIP_INFO[this_device].vcoMinFrequency)
        {
            vcoFrequency = frequency;
        }
        else
        {
            vcoFrequency = ADF4106_CHIP_INFO[this_device].vcoMinFrequency;
        }
    }
    else
    {
        vcoFrequency = ADF4106_CHIP_INFO[this_device].vcoMaxFrequency;
    }
    do
    {
        /* Get the actual PFD frequency. */
        rCounterValue = ADF4106_TuneRcounter(rCounterValue);
        frequencyPfd = refIn / rCounterValue;
        /* Define the value of the prescaler
        *  By default the prescaler is constant for ADF4001 and ADF4002
        *  In case of ADF4106, the minimum admitted prescaler will be selected,
        *  or the user defined prescaler if it is valid (big enough)
        *  Valid min prescaler : VCO < 2.4Ghz -> 8/9
        *                        VCO > 2.4Ghz and VCO < 5.4Ghz -> 16/17
        *                        VCO > 5.4Ghz -> 32/33
        */
        userPrescaler = ADF4106_PRESCALE(fLatch >> ADF4106_PS_OFFSET);
        devicePrescaler = (this_device != ID_ADF4106) ? \
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
    ADF4106_UpdateLatch(ADF4106_CTRL_FUNCTION_LATCH |
                        fLatch |
                        ADF4106_CR(ADF4106_CR_RESET));

    rLatch &= ~ADF4106_R_COUNTER(ADF4106_R_COUNTER_MASK);
    ADF4106_UpdateLatch(ADF4106_CTRL_R_COUNTER |
                        rLatch |
                        ADF4106_R_COUNTER(rCounterValue));

    nLatch &= ~(ADF4106_N_COUNTER_A(ADF4106_N_COUNTER_A_MASK) |
    			ADF4106_N_COUNTER_B(ADF4106_N_COUNTER_B_MASK));
    if(this_device == ID_ADF4106)
    {
        ADF4106_UpdateLatch(ADF4106_CTRL_N_COUNTER |
                            nLatch |
                            ADF4106_N_COUNTER_A(a) |
                            ADF4106_N_COUNTER_B(b));
    }
    else
    {
        ADF4106_UpdateLatch(ADF4106_CTRL_N_COUNTER |
                            nLatch |
                            ADF4106_N_COUNTER_B((b * devicePrescaler) + a));
    }

    fLatch &= ~ADF4106_CR(ADF4106_CR_MASK);
    ADF4106_UpdateLatch(ADF4106_CTRL_FUNCTION_LATCH |
                        fLatch |
                        ADF4106_CR(ADF4106_CR_NORMAL));

    return calculatedFrequency;
}
