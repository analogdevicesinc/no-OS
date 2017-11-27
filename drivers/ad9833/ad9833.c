/**************************************************************************//**
*   @file   ad9833.c
*   @brief  Implementation of ad9833 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
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

/******************************************************************************/
/******************************* Include Files ********************************/
/******************************************************************************/
#include "ad9833.h"
#include "TIME.h"

/******************************************************************************/
/************************** Constants Definitions *****************************/
/******************************************************************************/
float PHASE_CONST = 651.8986469f;

/******************************************************************************/
/************************** Variables Definitions *****************************/
/******************************************************************************/
unsigned char  txBuffer[4]  = {0, 0, 0, 0}; // Tx SPI Buffer.
unsigned char  rxBuffer[4]  = {0, 0, 0, 0}; // Rx SPI Buffer.
unsigned short ctrlRegValue = 0;
unsigned short testOpbiten  = 0;

struct ad9833_chip_info {
    unsigned long mclk;
    float freq_const;
};

static const struct ad9833_chip_info ad9833_chip_info[] = {
    [ID_AD9833] = {
        .mclk       = 25000000,
        .freq_const = 10.7374182f,
    },
    [ID_AD9834]  = {
        .mclk       = 75000000,
        .freq_const = 3.5791394f,
    },
    [ID_AD9837] = {
        .mclk       = 16000000,
        .freq_const = 16.777216f,
    },
    [ID_AD9838]  = {
        .mclk       = 16000000,
        .freq_const = 16.777216f,
    }
};

AD9833_type act_device;

unsigned char prog_method = 0;

/******************************************************************************/
/************************** Functions Definitions *****************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Initialize the SPI communication with the device.
 *
 * @param None.
 *
 * @return status - Result of the initialization procedure.
 *                  Example:  0 - if initialization was successful;
 *                           -1 - if initialization was unsuccessful.
*******************************************************************************/
char AD9833_Init(AD9833_type device)
{
    unsigned short spiData = 0;
    char           status  = -1;

    AD9834_PSEL_OUT;
    AD9834_PSEL_LOW;
    AD9834_FSEL_OUT;
    AD9834_FSEL_LOW;
    AD9834_RESET_OUT;
    AD9834_RESET_LOW;
    AD9834_SLEEP_OUT;
    AD9834_SLEEP_LOW;

    act_device = device;
    /* Setup SPI interface. */
    status = SPI_Init(0, 1000000, 1, 1);
    /* Initialize board. */
    spiData |= AD9833_CTRLRESET;
    AD9833_TxSpi(spiData);
    TIME_DelayMs(10);
    spiData &= ~AD9833_CTRLRESET;
    AD9833_TxSpi(spiData);

    AD9833_SetFreq(0, 0);
    AD9833_SetFreq(1, 0);
    AD9833_SetPhase(0, 0);
    AD9833_SetPhase(1, 0);

    return status;
}

/**************************************************************************//**
 * @brief Transmits 16 bits on SPI.
 *
 * @param value - Data which will be transmitted.
 *
 * @return none.
******************************************************************************/
void AD9833_TxSpi(short value)
{
    unsigned short spiData = 0;

    txBuffer[0] = (unsigned char) ((value & 0x00ff00) >> 8); // data to be sent
    txBuffer[1] = (unsigned char) (value & 0x0000ff);        // in 8 bit packets
    if (SPI_Write(AD9833_SLAVE_ID, txBuffer, 2) != 2)
    {
        /* Setup the SPI interface. */
        SPI_Init(0, 1000000, 1, 1);
        /* Initialize board. */
        spiData |= AD9833_CTRLRESET;
        AD9833_TxSpi(spiData);
        TIME_DelayMs(10);
        spiData &=~ AD9833_CTRLRESET;
        AD9833_TxSpi(spiData);
    }
}

/**************************************************************************//**
 * @brief Selects the type of output.
 *
 * @param OutMode - type of output
 *                   Example AD9833&AD9837: 0 - Sinusoid.
 *                                          1 - Triangle.
 *                                          2 - DAC Data MSB/2.
 *                                          3 - DAC Data MSB.
 *                   Example AD9834&AD9838: 0 - Sinusoid.
 *                                          1 - Triangle.
 *
 * @return status - output type could / couldn't be selected.
 *                  Example: 0 - output type possible for the given configuration.
 *                          -1 - output type not possible for the given configuration.
******************************************************************************/
char AD9833_OutMode(unsigned char OutMode)
{
    unsigned short spiData = 0;
    char status = 0;


    if ((act_device == ID_AD9833) || (act_device == ID_AD9837))
    {
        spiData = (ctrlRegValue & ~(AD9833_CTRLMODE    |
                                    AD9833_CTRLOPBITEN |
                                    AD9833_CTRLDIV2));
        switch(OutMode)
        {
            case 1:     // Triangle
                spiData += AD9833_CTRLMODE;
                break;
            case 2:     // DAC Data MSB/2
                spiData += AD9833_CTRLOPBITEN;
                break;
            case 3:     // DAC Data MSB
                spiData += AD9833_CTRLOPBITEN + AD9833_CTRLDIV2;
                break;
            default:    // Sinusoid
                break;
        }
        AD9833_TxSpi(spiData);
        ctrlRegValue = spiData;
    }
    else
    {
        if ((act_device == ID_AD9834) || (act_device == ID_AD9838))
        {
            spiData = (ctrlRegValue & ~AD9833_CTRLMODE);
            testOpbiten = ctrlRegValue & AD9833_CTRLOPBITEN;

            switch(OutMode)
            {
                case 1:     // Triangle
                    if (testOpbiten == 0)
                    {
                        spiData += AD9833_CTRLMODE;
                    }
                    else
                    {
                        status = -1;
                    }
                    break;
                default:    // Sinusoid
                    break;
            }
            AD9833_TxSpi(spiData);
            ctrlRegValue = spiData;
        }
    }

    return status;
}

/**************************************************************************//**
 * @brief Enable / Disable the sleep function.
 *
 * @param sleepMode - type of sleep
 *                    Example soft method(all devices):
 *                              0 - No power-down.
 *                              1 - DAC powered down.
 *                              2 - Internal clock disabled.
 *                              3 - DAC powered down and Internal
 *                                  clock disabled.
 *                    Example hard method(AD9834 & AD9838):
 *                              0 - No power-down.
 *                              1 - DAC powered down.
 *
 * @return None.
******************************************************************************/
void AD9833_SleepMode(unsigned char sleepMode)
{
    unsigned short spiData = 0;

    if (prog_method == 0)
    {
        spiData = (ctrlRegValue & ~(AD9833_CTRLSLEEP12 | AD9833_CTRLSLEEP1));
        switch(sleepMode)
        {
            case 1:     // DAC powered down
                spiData += AD9833_CTRLSLEEP12;
                break;
            case 2:     // Internal clock disabled
                spiData += AD9833_CTRLSLEEP1;
                break;
            case 3:     // DAC powered down and Internal clock disabled
                spiData += AD9833_CTRLSLEEP1 + AD9833_CTRLSLEEP12;
                break;
            default:    // No power-down
                break;
        }
        AD9833_TxSpi(spiData);
        ctrlRegValue = spiData;
    }
    else
    {
        if (prog_method == 1)
        {
            switch(sleepMode)
            {
                case 0:     // No power-down
                    AD9834_SLEEP_LOW;
                    break;
                case 1:     // DAC powered down
                    AD9834_SLEEP_HIGH;
                    break;
                default:    // Do nothing
                    break;
            }
        }
    }

}

/**************************************************************************//**
 * @brief Loads a frequency value in a register.
 *
 * @param registerNumber - Number of the register (0 / 1).
 * @param frequencyValue - Frequency value.
 *
 * @return None.
******************************************************************************/
void AD9833_SetFreq(unsigned char registerNumber, unsigned long frequencyValue)
{
    unsigned long  ulFreqRegister;
    unsigned short iFreqLsb, iFreqMsb;

    ulFreqRegister = (unsigned long)(frequencyValue *
                     ad9833_chip_info[act_device].freq_const);
    iFreqLsb = (ulFreqRegister & 0x0003FFF);
    iFreqMsb = ((ulFreqRegister & 0xFFFC000) >> 14);
    ctrlRegValue |= AD9833_CTRLB28;
    AD9833_TxSpi(ctrlRegValue);
    if(registerNumber == 0)
    {
        AD9833_TxSpi(BIT_F0ADDRESS + iFreqLsb);
        AD9833_TxSpi(BIT_F0ADDRESS + iFreqMsb);
    }
    else
    {
        AD9833_TxSpi(BIT_F1ADDRESS + iFreqLsb);
        AD9833_TxSpi(BIT_F1ADDRESS + iFreqMsb);
    }
}

/**************************************************************************//**
 * @brief Loads a phase value in a register.
 *
 * @param registerNumber - Number of the register (0 / 1).
 * @param phaseValue     - Phase value.
 *
 * @return none
******************************************************************************/
void AD9833_SetPhase(unsigned char registerNumber, float phaseValue)
{
    unsigned short phaseCalc;

    phaseCalc = (unsigned short)(phaseValue * PHASE_CONST);
    if(registerNumber == 0)
    {
        AD9833_TxSpi(BIT_P0ADDRESS + phaseCalc);
    }
    else
    {
        AD9833_TxSpi(BIT_P1ADDRESS + phaseCalc);
    }
}

/**************************************************************************//**
 * @brief Select the frequency register to be used.
 *
 * @param freqReg - Number of frequency register. (0 / 1)
 *
 * @return None.
******************************************************************************/
void AD9833_SelectFreqReg(unsigned char freqReg)
{
    unsigned short spiData = 0;

    if (prog_method == 0)
    {
        spiData = (ctrlRegValue & ~AD9833_CTRLFSEL);
        // Select soft the working frequency register according to parameter
        if(freqReg == 1)
        {
            spiData += AD9833_CTRLFSEL;
        }
        AD9833_TxSpi(spiData);
        ctrlRegValue = spiData;
    }
    else
    {
        if (prog_method == 1)
        {
            // Select hard the working frequency register according to parameter
            if (freqReg == 1)
            {
                AD9834_FSEL_HIGH;
            }
            else
            {
                if (freqReg == 0)
                {
                    AD9834_FSEL_LOW;
                }
            }
        }
    }
}

/**************************************************************************//**
 * @brief Select the phase register to be used.
 *
 * @param phaseReg - Number of phase register. (0 / 1)
 *
 * @return None.
******************************************************************************/
void AD9833_SelectPhaseReg(unsigned char phaseReg)
{
    unsigned short spiData = 0;

    if (prog_method == 0)
    {
        spiData = (ctrlRegValue & ~AD9833_CTRLPSEL);
        // Select soft the working phase register according to parameter
        if(phaseReg == 1)
        {
            spiData += AD9833_CTRLPSEL;
        }
        AD9833_TxSpi(spiData);
        ctrlRegValue = spiData;
    }
    else
    {
        if (prog_method == 1)
        {
            // Select hard the working phase register according to parameter
            if (phaseReg == 1)
            {
                AD9834_PSEL_HIGH;
            }
            else
            {
                if (phaseReg == 0)
                {
                    AD9834_PSEL_LOW;
                }
            }
        }
    }
}

/**************************************************************************//**
 * @brief Sets the programming method. (only for AD9834 & AD9838)
 *
 * @param value - soft or hard method. (0 / 1)
 *
 * @return None.
******************************************************************************/
void AD9834_SelectProgMethod(unsigned char value)
{
    unsigned short spiData = (ctrlRegValue & ~AD9834_CTRLPINSW);

    prog_method = 0;        // software method
    if (value == 1)
    {
        spiData += AD9834_CTRLPINSW;
        prog_method = 1;    // hardware method
    }
    AD9833_TxSpi(spiData);
    ctrlRegValue = spiData;
}

/**************************************************************************//**
 * @brief Configures the control register for logic output.
 *        (only for AD9834 & AD9838)
 *
 * @param opbiten - Enables / disables the logic output.
 * @param signpib - Connects comparator / MSB to the SIGN BIT OUT pin.
 * @param div2    - MSB / MSB/2
 *
 * @return None.
******************************************************************************/
void AD9834_LogicOutput(unsigned char opbiten,
                        unsigned char signpib,
                        unsigned char div2)
{
    unsigned short spiData = 0;

    spiData = (ctrlRegValue & ~(AD9833_CTRLOPBITEN |
                                AD9833_CTRLMODE    |
                                AD9834_CTRLSIGNPIB |
                                AD9833_CTRLDIV2));


    if(opbiten == 1)
    {
        spiData |= AD9833_CTRLOPBITEN;

        if (signpib == 1)
        {
            spiData |= AD9834_CTRLSIGNPIB;
        }
        else
        {
            if (signpib == 0)
            {
                spiData &= ~AD9834_CTRLSIGNPIB;
            }
        }
        if (div2 == 1)
        {
            spiData |= AD9833_CTRLDIV2;
        }
        else
        {
            if (div2 == 0)
            {
                spiData &= ~AD9833_CTRLDIV2;
            }
        }
    }
    else
    {
        if (opbiten == 0)
        {
            spiData &= ~AD9833_CTRLOPBITEN;
        }
    }
    AD9833_TxSpi(spiData);
    ctrlRegValue = spiData;
}


