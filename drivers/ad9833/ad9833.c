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
#include <stdint.h>
#include <stdlib.h>
#include "platform_drivers.h"
#include "ad9833.h"

/******************************************************************************/
/************************** Constants Definitions *****************************/
/******************************************************************************/
float PHASE_CONST = 651.8986469f;

static const ad9833_chip_info chip_info[] = {
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

/******************************************************************************/
/************************** Functions Definitions *****************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Initialize the SPI communication with the device.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return status - Result of the initialization procedure.
 *                  Example:  0 - if initialization was successful;
 *                           -1 - if initialization was unsuccessful.
*******************************************************************************/
char AD9833_Init(AD9833_dev **device,
		 AD9833_init_param init_param)
{
	AD9833_dev *dev;
    unsigned short spiData =  0;
    char           status  = -1;

	dev = (AD9833_dev *)malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* Setup GPIO pads. */
	status |= gpio_get(&dev->gpio_psel,
			   init_param.gpio_psel);
	status |= gpio_get(&dev->gpio_fsel,
			   init_param.gpio_fsel);
	status |= gpio_get(&dev->gpio_reset,
			   init_param.gpio_reset);
	status |= gpio_get(&dev->gpio_sleep,
			   init_param.gpio_sleep);

    AD9834_PSEL_OUT;
    AD9834_PSEL_LOW;
    AD9834_FSEL_OUT;
    AD9834_FSEL_LOW;
    AD9834_RESET_OUT;
    AD9834_RESET_LOW;
    AD9834_SLEEP_OUT;
    AD9834_SLEEP_LOW;

	dev->act_device = init_param.act_device;
	dev->prog_method = 0;
	dev->ctrlRegValue = 0;
	dev->testOpbiten = 0;

    /* Setup SPI interface. */
	status = spi_init(&dev->spi_desc,
			  init_param.spi_init);
    /* Initialize board. */
    spiData |= AD9833_CTRLRESET;
    AD9833_TxSpi(dev,
		 spiData);
    mdelay(10);
    spiData &= ~AD9833_CTRLRESET;
    AD9833_TxSpi(dev,
		 spiData);

    AD9833_SetFreq(dev, 0, 0);
    AD9833_SetFreq(dev, 1, 0);
    AD9833_SetPhase(dev, 0, 0);
    AD9833_SetPhase(dev, 1, 0);

	*device = dev;

    return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by AD9833_Init().
 *
 * @param dev - The device structure.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t AD9833_remove(AD9833_dev *dev)
{
	int32_t ret;

	ret = spi_remove(dev->spi_desc);

	ret |= gpio_remove(dev->gpio_psel);
	ret |= gpio_remove(dev->gpio_fsel);
	ret |= gpio_remove(dev->gpio_reset);
	ret |= gpio_remove(dev->gpio_sleep);

	free(dev);

	return ret;
}

/**************************************************************************//**
 * @brief Transmits 16 bits on SPI.
 *
 * @param dev   - The device structure.
 * @param value - Data which will be transmitted.
 *
 * @return none.
******************************************************************************/
void AD9833_TxSpi(AD9833_dev *dev,
		  short value)
{
    unsigned short spiData = 0;
	unsigned char  txBuffer[4]  = {0, 0, 0, 0}; // Tx SPI Buffer.

    txBuffer[0] = (unsigned char) ((value & 0x00ff00) >> 8); // data to be sent
    txBuffer[1] = (unsigned char) (value & 0x0000ff);        // in 8 bit packets
    if (spi_write_and_read(dev->spi_desc, txBuffer, 2) != SUCCESS)
    {
        /* Initialize board. */
        spiData |= AD9833_CTRLRESET;
        AD9833_TxSpi(dev,
		     spiData);
        mdelay(10);
        spiData &=~ AD9833_CTRLRESET;
        AD9833_TxSpi(dev,
		     spiData);
    }
}

/**************************************************************************//**
 * @brief Selects the type of output.
 *
 * @param dev     - The device structure.
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
char AD9833_OutMode(AD9833_dev *dev,
		    unsigned char OutMode)
{
    unsigned short spiData = 0;
    char status = 0;


    if ((dev->act_device == ID_AD9833) || (dev->act_device == ID_AD9837))
    {
        spiData = (dev->ctrlRegValue & ~(AD9833_CTRLMODE    |
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
        AD9833_TxSpi(dev,
		     spiData);
        dev->ctrlRegValue = spiData;
    }
    else
    {
        if ((dev->act_device == ID_AD9834) || (dev->act_device == ID_AD9838))
        {
            spiData = (dev->ctrlRegValue & ~AD9833_CTRLMODE);
            dev->testOpbiten = dev->ctrlRegValue & AD9833_CTRLOPBITEN;

            switch(OutMode)
            {
                case 1:     // Triangle
                    if (dev->testOpbiten == 0)
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
            AD9833_TxSpi(dev,
			 spiData);
            dev->ctrlRegValue = spiData;
        }
    }

    return status;
}

/**************************************************************************//**
 * @brief Enable / Disable the sleep function.
 *
 * @param dev       - The device structure.
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
void AD9833_SleepMode(AD9833_dev *dev,
		      unsigned char sleepMode)
{
    unsigned short spiData = 0;

    if (dev->prog_method == 0)
    {
        spiData = (dev->ctrlRegValue & ~(AD9833_CTRLSLEEP12 | AD9833_CTRLSLEEP1));
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
        AD9833_TxSpi(dev,
		     spiData);
        dev->ctrlRegValue = spiData;
    }
    else
    {
        if (dev->prog_method == 1)
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
 * @param dev            - The device structure.
 * @param registerNumber - Number of the register (0 / 1).
 * @param frequencyValue - Frequency value.
 *
 * @return None.
******************************************************************************/
void AD9833_SetFreq(AD9833_dev *dev,
		    unsigned char registerNumber,
		    unsigned long frequencyValue)
{
    unsigned long  ulFreqRegister;
    unsigned short iFreqLsb, iFreqMsb;

    ulFreqRegister = (unsigned long)(frequencyValue *
                     chip_info[dev->act_device].freq_const);
    iFreqLsb = (ulFreqRegister & 0x0003FFF);
    iFreqMsb = ((ulFreqRegister & 0xFFFC000) >> 14);
    dev->ctrlRegValue |= AD9833_CTRLB28;
    AD9833_TxSpi(dev,
		 dev->ctrlRegValue);
    if(registerNumber == 0)
    {
        AD9833_TxSpi(dev,
		     BIT_F0ADDRESS + iFreqLsb);
        AD9833_TxSpi(dev,
		     BIT_F0ADDRESS + iFreqMsb);
    }
    else
    {
        AD9833_TxSpi(dev,
		     BIT_F1ADDRESS + iFreqLsb);
        AD9833_TxSpi(dev,
		     BIT_F1ADDRESS + iFreqMsb);
    }
}

/**************************************************************************//**
 * @brief Loads a phase value in a register.
 *
 * @param dev            - The device structure.
 * @param registerNumber - Number of the register (0 / 1).
 * @param phaseValue     - Phase value.
 *
 * @return none
******************************************************************************/
void AD9833_SetPhase(AD9833_dev *dev,
		     unsigned char registerNumber,
		     float phaseValue)
{
    unsigned short phaseCalc;

    phaseCalc = (unsigned short)(phaseValue * PHASE_CONST);
    if(registerNumber == 0)
    {
        AD9833_TxSpi(dev,
		     BIT_P0ADDRESS + phaseCalc);
    }
    else
    {
        AD9833_TxSpi(dev,
		     BIT_P1ADDRESS + phaseCalc);
    }
}

/**************************************************************************//**
 * @brief Select the frequency register to be used.
 *
 * @param dev     - The device structure.
 * @param freqReg - Number of frequency register. (0 / 1)
 *
 * @return None.
******************************************************************************/
void AD9833_SelectFreqReg(AD9833_dev *dev,
			  unsigned char freqReg)
{
    unsigned short spiData = 0;

    if (dev->prog_method == 0)
    {
        spiData = (dev->ctrlRegValue & ~AD9833_CTRLFSEL);
        // Select soft the working frequency register according to parameter
        if(freqReg == 1)
        {
            spiData += AD9833_CTRLFSEL;
        }
        AD9833_TxSpi(dev,
		     spiData);
        dev->ctrlRegValue = spiData;
    }
    else
    {
        if (dev->prog_method == 1)
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
 * @param dev      - The device structure.
 * @param phaseReg - Number of phase register. (0 / 1)
 *
 * @return None.
******************************************************************************/
void AD9833_SelectPhaseReg(AD9833_dev *dev,
			   unsigned char phaseReg)
{
    unsigned short spiData = 0;

    if (dev->prog_method == 0)
    {
        spiData = (dev->ctrlRegValue & ~AD9833_CTRLPSEL);
        // Select soft the working phase register according to parameter
        if(phaseReg == 1)
        {
            spiData += AD9833_CTRLPSEL;
        }
        AD9833_TxSpi(dev,
		     spiData);
        dev->ctrlRegValue = spiData;
    }
    else
    {
        if (dev->prog_method == 1)
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
 * @param dev   - The device structure.
 * @param value - soft or hard method. (0 / 1)
 *
 * @return None.
******************************************************************************/
void AD9834_SelectProgMethod(AD9833_dev *dev,
			     unsigned char value)
{
    unsigned short spiData = (dev->ctrlRegValue & ~AD9834_CTRLPINSW);

    dev->prog_method = 0;        // software method
    if (value == 1)
    {
        spiData += AD9834_CTRLPINSW;
        dev->prog_method = 1;    // hardware method
    }
    AD9833_TxSpi(dev,
		 spiData);
    dev->ctrlRegValue = spiData;
}

/**************************************************************************//**
 * @brief Configures the control register for logic output.
 *        (only for AD9834 & AD9838)
 *
 * @param dev     - The device structure.
 * @param opbiten - Enables / disables the logic output.
 * @param signpib - Connects comparator / MSB to the SIGN BIT OUT pin.
 * @param div2    - MSB / MSB/2
 *
 * @return None.
******************************************************************************/
void AD9834_LogicOutput(AD9833_dev *dev,
			unsigned char opbiten,
                        unsigned char signpib,
                        unsigned char div2)
{
    unsigned short spiData = 0;

    spiData = (dev->ctrlRegValue & ~(AD9833_CTRLOPBITEN |
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
    AD9833_TxSpi(dev,
		 spiData);
    dev->ctrlRegValue = spiData;
}


