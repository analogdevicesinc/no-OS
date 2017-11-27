/******************************************************************************
*   @file   ADF4153.c
*   @brief  Implementation of ADF4153 Driver for Microblaze processor.
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
#include "adf4153.h"
#include "Communication.h"
#include "TIME.h"

/* For round up division */
#define CEIL(a, b) (((a) / (b)) + (((a) % (b)) > 0 ? 1 : 0))

#define DATA_MASK_MSB8      0xFF0000
#define DATA_OFFSET_MSB8    16
#define DATA_MASK_MID8      0xFF00
#define DATA_OFFSET_MID8    8
#define DATA_MASK_LSB8      0xFF
#define DATA_OFFSET_LSB8    0
#define ADDRESS_MASK        3

#define FREQ_2_GHZ          2000000000

/*****************************************************************************/
/**************************** Private variables ******************************/
/*****************************************************************************/

/* RF input frequency limits */
long ADF4153_RFIN_MIN_FRQ = 10000000;           // 10 Mhz
long ADF4153_RFIN_MAX_FRQ = 250000000;          // 250 Mhz
/* Maximum PFD frequency */
long ADF4153_PFD_MAX_FRQ = 32000000;            // 32 Mhz
/* VCO out frequency limits */
long ADF4153_VCO_MIN_FRQ = 500000000;           // 500 Mhz
long long ADF4153_VCO_MAX_FRQ = 4000000000;     // 4 Ghz
/* maximum interpolator modulus value */
short ADF4153_MOD_MAX    = 4095;                // the MOD is stored in 12 bits

/* Reference input frequency */
unsigned long refIn = 0;
unsigned long channelSpacing = 0;

/* Internal buffers for each latch */
unsigned long r0 = 0;               /* the actual value of N Divider Register */
unsigned long r1 = 0;               /* the actual value of R Divider Register */
unsigned long r2 = 0;               /* the actual value of Control Register */
unsigned long r3 = 0;               /* the actual value of Noise and Spur Reg*/

/*****************************************************************************/
/*************************** Functions definitions ***************************/
/*****************************************************************************/

/**************************************************************************//**
 * @brief Initialize SPI and Initial Values for ADF4106 Board.
 *
 * @param  ADF4106_st    - the structure with the initial set up values of the
 *                         registers
 *
 * @return success
******************************************************************************/
char ADF4153_Init(ADF4153_settings_t ADF4153_st)
{
    char status = -1;

    /* CPHA = 1; CPOL = 0; */
    status = SPI_Init(0, 100000, 0, 1);

    /* Bring CE high to put device to power up */
    ADF4153_CE_OUT;
    ADF4153_CE_HIGH;
    /* Initialize the load enable pin */
    ADF4153_LE_OUT;
    ADF4153_LE_LOW;

    /* Set up the reference input frequency */
    refIn = ADF4153_st.refIn;
    channelSpacing = ADF4153_st.channelSpacing;

    /* Write all zeros to the noise and spur register */
    ADF4153_UpdateLatch(ADF4153_CTRL_NOISE_SPUR |
                        0x0);
    /* selects the lowest noise mode by default */
    ADF4153_UpdateLatch(ADF4153_CTRL_NOISE_SPUR |
                        0x3C7);
    /* Set up the control register and enable the counter reset */
    ADF4153_UpdateLatch(ADF4153_CTRL_CONTROL |
                        ADF4153_R2_COUNTER_RST(ADF4153_CR_ENABLED) |
                        ADF4153_R2_CP_3STATE(ADF4153_st.cpThreeState) |
                        ADF4153_R2_POWER_DOWN(ADF4153_st.powerDown) |
                        ADF4153_R2_LDP(ADF4153_st.ldp) |
                        ADF4153_R2_PD_POL(ADF4153_st.pdPolarity) |
                        ADF4153_R2_CP_CURRENT(ADF4153_st.cpCurrent) |
                        ADF4153_R2_REF_DOUBLER(ADF4153_st.refDoubler) |
                        ADF4153_R2_RESYNC(ADF4153_st.resync)
                        );
    /* If resync feature is enabled */
    if(ADF4153_st.resync != 0x0)
    {
        /* Load the R divider register */
        ADF4153_UpdateLatch(ADF4153_CTRL_R_DIVIDER |
                            ADF4153_R1_MOD(10) |    //Resync Delay
                            ADF4153_R1_RCOUNTER(ADF4153_st.rCounter) |
                            ADF4153_R1_PRESCALE(ADF4153_st.prescaler) |
                            ADF4153_R1_MUXOUT(ADF4153_st.muxout) |
                            ADF4153_R1_LOAD(ADF4153_LOAD_RESYNC)
                            );
    }
    /* Load the R divider register */
    ADF4153_UpdateLatch(ADF4153_CTRL_R_DIVIDER |
                        ADF4153_R1_MOD(ADF4153_st.modValue) |
                        ADF4153_R1_RCOUNTER(ADF4153_st.rCounter) |
                        ADF4153_R1_PRESCALE(ADF4153_st.prescaler) |
                        ADF4153_R1_MUXOUT(ADF4153_st.muxout) |
                        ADF4153_R1_LOAD(ADF4153_LOAD_NORMAL)
                        );
    /* Load the N divider register */
    ADF4153_UpdateLatch(ADF4153_CTRL_N_DIVIDER |
                        ADF4153_R0_FRAC(ADF4153_st.fracValue) |
                        ADF4153_R0_INT(ADF4153_st.intValue) |
                        ADF4153_R0_FASTLOCK(ADF4153_st.fastlock)
                        );

    /* Disable the counter reset in the Control Register */
    r2 &= ~ADF4153_R2_COUNTER_RST(ADF4153_R2_COUNTER_RST_MASK);
    ADF4153_UpdateLatch(ADF4153_CTRL_CONTROL |
                        r2 |
                        ADF4153_R2_COUNTER_RST(ADF4153_CR_DISABLED)
                        );
return status;
}

/**************************************************************************//**
 * @brief Update one of the latch via the SPI interface
 *
 * @param   latchData - the data which will be written to the latch
 *
 * @return
******************************************************************************/
void ADF4153_UpdateLatch(unsigned long latchData)
{
    unsigned char dataBuffer[3] = {0,};
    unsigned char latchType = latchData & 0x3;

    /* Update the internal buffers */
    switch(latchType)
    {
        case ADF4153_CTRL_N_DIVIDER :
            r0 = latchData;
            break;
        case ADF4153_CTRL_R_DIVIDER :
            r1 = latchData;
            break;
        case ADF4153_CTRL_CONTROL :
            r2 = latchData;
            break;
        case ADF4153_CTRL_NOISE_SPUR :
            r3 = latchData;
            break;
    }

    dataBuffer[0] = (latchData & DATA_MASK_MSB8) >> DATA_OFFSET_MSB8;
    dataBuffer[1] = (latchData & DATA_MASK_MID8) >> DATA_OFFSET_MID8;
    dataBuffer[2] = (latchData & DATA_MASK_LSB8) >> DATA_OFFSET_LSB8;

    SPI_Write(ADF4153_SLAVE_ID,
              dataBuffer,
              3);

    /* Generate a load pulse */
    ADF4153_LE_HIGH;
    ADF4153_LE_LOW;
}

/***************************************************************************//**
 * @brief Return the value of a desired latch
 *
 * @param   param[0] - the type of the latch:
 *                      0 - 'ADF4153_CTRL_N_DIVIDER'
 *                      1 - 'ADF4153_CTRL_R_DIVIDER'
 *                      2 - 'ADF4153_CTRL_CONTROL'
 *                      3 - 'ADF4153_CTRL_NOISE_SPUR'
 *
 * @return latchValue - the value of the desired latch
*******************************************************************************/
unsigned long ADF4153_ReadLatch(unsigned char latchType)
{
    switch(latchType)
    {
        case ADF4153_CTRL_N_DIVIDER :
            return r0;

        case ADF4153_CTRL_R_DIVIDER :
            return r1;

        case ADF4153_CTRL_CONTROL :
            return r2;

        case ADF4153_CTRL_NOISE_SPUR :
            return r3;

        default :
            return -1;
    }
}

/***************************************************************************//**
 * @brief Increases the R counter value until the ADF4106_PDF_MAX_FREQ is
 *        greater than PFD frequency.
 *
 * @param rCounter - pointer of the R counter variable.
 *
 * @return pfdFrequency - The value of the PFD frequency.
*******************************************************************************/
unsigned long ADF4153_TuneRcounter(unsigned short *rCounter)
{
    unsigned long pfdFrequency = 0;              // PFD frequency
    unsigned char refDoubler = 0;                // Reference Doubler

    refDoubler = (r2 & ADF4153_R2_REF_DOUBLER(ADF4153_R2_RESYNC_MASK)) >> \
                  ADF4153_R2_REF_DOUBLER_OFFSET; // the actual reference doubler
    do
    {
        (*rCounter)++;
        pfdFrequency = refIn * ((float)(1 + refDoubler) / (*rCounter));
    }
    while(pfdFrequency > ADF4153_PFD_MAX_FRQ);

return pfdFrequency;
}

/***************************************************************************//**
 * @brief Sets the output frequency.
 *
 * @param frequency - The desired frequency value.
 *
 * @return calculatedFrequency - The actual frequency value that was set.
*******************************************************************************/
unsigned long long ADF4153_SetFrequency(unsigned long long frequency)
{
    unsigned long long vcoFrequency        = 0;     // VCO frequency
    unsigned long      pfdFrequency        = 0;     // PFD frequency
    unsigned long long calculatedFrequency = 0;     // Actual VCO frequency
    unsigned long      intValue            = 0;     // INT value
    unsigned long      fracValue           = 0;     // FRAC value
    unsigned long      modValue            = 0;     // MOD value
    unsigned short     rCounter            = 0;     // R Counter
    float              buffer              = 0;
    unsigned char      devicePrescaler     = 0;
    unsigned char      intMin              = 0;
    /* validate the given frequency parameter */
    if(frequency <= ADF4153_VCO_MAX_FRQ)
    {
        if(frequency >= ADF4153_VCO_MIN_FRQ)
        {
            vcoFrequency = frequency;
        }
        else
        {
            vcoFrequency = ADF4153_VCO_MIN_FRQ;
        }
    }
    else
    {
        vcoFrequency = ADF4153_VCO_MAX_FRQ;
    }

    /* define the value of MOD */
    modValue = CEIL(refIn, channelSpacing);
    /* if the modValue is too high, increase the channel spacing */
    if(modValue > ADF4153_MOD_MAX)
    {
        do{
            channelSpacing++;
            modValue = CEIL(refIn, channelSpacing);
        }while(modValue <= ADF4153_MOD_MAX);
    }
    /* define prescaler */
    devicePrescaler = (vcoFrequency <= FREQ_2_GHZ) ? ADF4153_PRESCALER_4_5 : \
                                                     ADF4153_PRESCALER_8_9;
    intMin = (devicePrescaler == ADF4153_PRESCALER_4_5) ? 31 : 91;
    /* define the PFD frequency, R counter ant INT value */
    do
    {
        /* define the PFD frequency and R Counter, using the TuneRCounter() */
        pfdFrequency = ADF4153_TuneRcounter(&rCounter);
        intValue = vcoFrequency / pfdFrequency;
    }while(intValue < intMin);
    /*define FRAC value */
    do
    {
        fracValue++;
        buffer = intValue + ((float)fracValue/modValue);
        calculatedFrequency = (unsigned long long)(buffer * pfdFrequency);
    }while(calculatedFrequency <= vcoFrequency);
    fracValue--;

    /* Find the actual VCO frequency. */
    buffer = intValue + ((float)fracValue/modValue);
    calculatedFrequency = (unsigned long long)(buffer * pfdFrequency);

    /* Enable the Counter Reset */
    ADF4153_UpdateLatch(ADF4153_CTRL_CONTROL |
                        r2 |
                        ADF4153_R2_COUNTER_RST(ADF4153_CR_ENABLED));

    /* Load the R divider with the new values */
    r1 &= (~ADF4153_R1_MOD(ADF4153_R1_MOD_MASK) &
           ~ADF4153_R1_RCOUNTER(ADF4153_R1_RCOUNTER_MASK) &
           ~ADF4153_R1_PRESCALE(ADF4153_R1_PRESCALE_MASK));
    ADF4153_UpdateLatch(ADF4153_CTRL_R_DIVIDER |
                        r1 |
                        ADF4153_R1_MOD(modValue) |
                        ADF4153_R1_RCOUNTER(rCounter) |
                        ADF4153_R1_PRESCALE(devicePrescaler));

    /* Load the N divider with the new values */
    r0 &= (~ADF4153_R0_FRAC(ADF4153_R0_FRAC_MASK) &
           ~ADF4153_R0_INT(ADF4153_R0_INT_MASK));
    ADF4153_UpdateLatch(ADF4153_CTRL_N_DIVIDER |
                        r0 |
                        ADF4153_R0_FRAC(fracValue) |
                        ADF4153_R0_INT(intValue));
    /* Disable the Counter Reset */
    r2 &= ~ADF4153_R2_COUNTER_RST(ADF4153_R2_COUNTER_RST_MASK);
    ADF4153_UpdateLatch(ADF4153_CTRL_CONTROL |
                        r2 |
                        ADF4153_R2_COUNTER_RST(ADF4153_CR_DISABLED));

    return calculatedFrequency;
}

/***************************************************************************//**
 * @brief Return the value of the channel spacing
 *
 * @param frequency - The desired frequency value.
 *
 * @return calculatedFrequency - The actual frequency value that was set.
*******************************************************************************/
unsigned long ADF4153_GetChannelSpacing( void )
{
    return channelSpacing;
}
