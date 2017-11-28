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
#include <stdint.h>
#include <stdlib.h>
#include "platform_drivers.h"
#include "adf4153.h"

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
char ADF4153_Init(ADF4153_dev **device,
		  ADF4153_init_param init_param)
{
	ADF4153_dev *dev;
    char status;

	dev = (ADF4153_dev *)malloc(sizeof(*dev));
	if (!dev)
		return -1;

	dev->ADF4153_st = init_param.ADF4153_st;

	dev->ADF4153_RFIN_MIN_FRQ = 10000000;   // 10 Mhz
	dev->ADF4153_RFIN_MAX_FRQ = 250000000;  // 250 Mhz
	dev->ADF4153_PFD_MAX_FRQ  = 32000000;   // 32 Mhz
	dev->ADF4153_VCO_MIN_FRQ  = 500000000;  // 500 Mhz
	dev->ADF4153_VCO_MAX_FRQ  = 4000000000; // 4 Ghz
	dev->ADF4153_MOD_MAX      = 4095;       // the MOD is stored in 12 bits
	dev->r0 = 0;
	dev->r1 = 0;
	dev->r2 = 0;
	dev->r3 = 0;

    /* CPHA = 1; CPOL = 0; */
	status = spi_init(&dev->spi_desc, init_param.spi_init);

	/* GPIO */
	status |= gpio_get(&dev->gpio_le, init_param.gpio_le);
	status |= gpio_get(&dev->gpio_ce, init_param.gpio_ce);
	status |= gpio_get(&dev->gpio_le2, init_param.gpio_le2);
	status |= gpio_get(&dev->gpio_ce2, init_param.gpio_ce2);

    /* Bring CE high to put device to power up */
    ADF4153_CE_OUT;
    ADF4153_CE_HIGH;
    /* Initialize the load enable pin */
    ADF4153_LE_OUT;
    ADF4153_LE_LOW;

    /* Write all zeros to the noise and spur register */
    ADF4153_UpdateLatch(dev,
			ADF4153_CTRL_NOISE_SPUR |
                        0x0);
    /* selects the lowest noise mode by default */
    ADF4153_UpdateLatch(dev,
			ADF4153_CTRL_NOISE_SPUR |
                        0x3C7);
    /* Set up the control register and enable the counter reset */
    ADF4153_UpdateLatch(dev,
			ADF4153_CTRL_CONTROL |
                        ADF4153_R2_COUNTER_RST(ADF4153_CR_ENABLED) |
                        ADF4153_R2_CP_3STATE(dev->ADF4153_st.cpThreeState) |
                        ADF4153_R2_POWER_DOWN(dev->ADF4153_st.powerDown) |
                        ADF4153_R2_LDP(dev->ADF4153_st.ldp) |
                        ADF4153_R2_PD_POL(dev->ADF4153_st.pdPolarity) |
                        ADF4153_R2_CP_CURRENT(dev->ADF4153_st.cpCurrent) |
                        ADF4153_R2_REF_DOUBLER(dev->ADF4153_st.refDoubler) |
                        ADF4153_R2_RESYNC(dev->ADF4153_st.resync)
                        );
    /* If resync feature is enabled */
    if(init_param.ADF4153_st.resync != 0x0)
    {
        /* Load the R divider register */
        ADF4153_UpdateLatch(dev,
			    ADF4153_CTRL_R_DIVIDER |
                            ADF4153_R1_MOD(10) |    //Resync Delay
                            ADF4153_R1_RCOUNTER(dev->ADF4153_st.rCounter) |
                            ADF4153_R1_PRESCALE(dev->ADF4153_st.prescaler) |
                            ADF4153_R1_MUXOUT(dev->ADF4153_st.muxout) |
                            ADF4153_R1_LOAD(ADF4153_LOAD_RESYNC)
                            );
    }
    /* Load the R divider register */
    ADF4153_UpdateLatch(dev,
			ADF4153_CTRL_R_DIVIDER |
                        ADF4153_R1_MOD(dev->ADF4153_st.modValue) |
                        ADF4153_R1_RCOUNTER(dev->ADF4153_st.rCounter) |
                        ADF4153_R1_PRESCALE(dev->ADF4153_st.prescaler) |
                        ADF4153_R1_MUXOUT(dev->ADF4153_st.muxout) |
                        ADF4153_R1_LOAD(ADF4153_LOAD_NORMAL)
                        );
    /* Load the N divider register */
    ADF4153_UpdateLatch(dev,
			ADF4153_CTRL_N_DIVIDER |
                        ADF4153_R0_FRAC(dev->ADF4153_st.fracValue) |
                        ADF4153_R0_INT(dev->ADF4153_st.intValue) |
                        ADF4153_R0_FASTLOCK(dev->ADF4153_st.fastlock)
                        );

    /* Disable the counter reset in the Control Register */
    dev->r2 &= ~ADF4153_R2_COUNTER_RST(ADF4153_R2_COUNTER_RST_MASK);
    ADF4153_UpdateLatch(dev,
			ADF4153_CTRL_CONTROL |
                        dev->r2 |
                        ADF4153_R2_COUNTER_RST(ADF4153_CR_DISABLED)
                        );

	*device = dev;

return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ADF4153_Init().
 *
 * @param dev - The device structure.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ADF4153_remove(ADF4153_dev *dev)
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
void ADF4153_UpdateLatch(ADF4153_dev *dev,
			 unsigned long latchData)
{
    unsigned char dataBuffer[3] = {0,};
    unsigned char latchType = latchData & 0x3;

    /* Update the internal buffers */
    switch(latchType)
    {
        case ADF4153_CTRL_N_DIVIDER :
            dev->r0 = latchData;
            break;
        case ADF4153_CTRL_R_DIVIDER :
            dev->r1 = latchData;
            break;
        case ADF4153_CTRL_CONTROL :
            dev->r2 = latchData;
            break;
        case ADF4153_CTRL_NOISE_SPUR :
            dev->r3 = latchData;
            break;
    }

    dataBuffer[0] = (latchData & DATA_MASK_MSB8) >> DATA_OFFSET_MSB8;
    dataBuffer[1] = (latchData & DATA_MASK_MID8) >> DATA_OFFSET_MID8;
    dataBuffer[2] = (latchData & DATA_MASK_LSB8) >> DATA_OFFSET_LSB8;

	spi_write_and_read(dev->spi_desc,
			   dataBuffer,
			   3);

    /* Generate a load pulse */
    ADF4153_LE_HIGH;
    ADF4153_LE_LOW;
}

/***************************************************************************//**
 * @brief Return the value of a desired latch
 *
 * @param dev        - The device structure.
 * @param   param[0] - the type of the latch:
 *                      0 - 'ADF4153_CTRL_N_DIVIDER'
 *                      1 - 'ADF4153_CTRL_R_DIVIDER'
 *                      2 - 'ADF4153_CTRL_CONTROL'
 *                      3 - 'ADF4153_CTRL_NOISE_SPUR'
 *
 * @return latchValue - the value of the desired latch
*******************************************************************************/
unsigned long ADF4153_ReadLatch(ADF4153_dev *dev,
				unsigned char latchType)
{
    switch(latchType)
    {
        case ADF4153_CTRL_N_DIVIDER :
            return dev->r0;

        case ADF4153_CTRL_R_DIVIDER :
            return dev->r1;

        case ADF4153_CTRL_CONTROL :
            return dev->r2;

        case ADF4153_CTRL_NOISE_SPUR :
            return dev->r3;

        default :
            return -1;
    }
}

/***************************************************************************//**
 * @brief Increases the R counter value until the ADF4106_PDF_MAX_FREQ is
 *        greater than PFD frequency.
 *
 * @param dev      - The device structure.
 * @param rCounter - pointer of the R counter variable.
 *
 * @return pfdFrequency - The value of the PFD frequency.
*******************************************************************************/
unsigned long ADF4153_TuneRcounter(ADF4153_dev *dev,
				   unsigned short *rCounter)
{
    unsigned long pfdFrequency = 0;              // PFD frequency
    unsigned char refDoubler = 0;                // Reference Doubler

    refDoubler = (dev->r2 & ADF4153_R2_REF_DOUBLER(ADF4153_R2_RESYNC_MASK)) >> \
                  ADF4153_R2_REF_DOUBLER_OFFSET; // the actual reference doubler
    do
    {
        (*rCounter)++;
        pfdFrequency = dev->ADF4153_st.refIn * \
		       ((float)(1 + refDoubler) / (*rCounter));
    }
    while(pfdFrequency > dev->ADF4153_PFD_MAX_FRQ);

return pfdFrequency;
}

/***************************************************************************//**
 * @brief Sets the output frequency.
 *
 * @param dev       - The device structure.
 * @param frequency - The desired frequency value.
 *
 * @return calculatedFrequency - The actual frequency value that was set.
*******************************************************************************/
unsigned long long ADF4153_SetFrequency(ADF4153_dev *dev,
					unsigned long long frequency)
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
    if(frequency <= dev->ADF4153_VCO_MAX_FRQ)
    {
        if(frequency >= dev->ADF4153_VCO_MIN_FRQ)
        {
            vcoFrequency = frequency;
        }
        else
        {
            vcoFrequency = dev->ADF4153_VCO_MIN_FRQ;
        }
    }
    else
    {
        vcoFrequency = dev->ADF4153_VCO_MAX_FRQ;
    }

    /* define the value of MOD */
    modValue = CEIL(dev->ADF4153_st.refIn,
		    dev->ADF4153_st.channelSpacing);
    /* if the modValue is too high, increase the channel spacing */
    if(modValue > dev->ADF4153_MOD_MAX)
    {
        do{
            dev->ADF4153_st.channelSpacing++;
            modValue = CEIL(dev->ADF4153_st.refIn,
			    dev->ADF4153_st.channelSpacing);
        }while(modValue <= dev->ADF4153_MOD_MAX);
    }
    /* define prescaler */
    devicePrescaler = (vcoFrequency <= FREQ_2_GHZ) ? ADF4153_PRESCALER_4_5 : \
                                                     ADF4153_PRESCALER_8_9;
    intMin = (devicePrescaler == ADF4153_PRESCALER_4_5) ? 31 : 91;
    /* define the PFD frequency, R counter ant INT value */
    do
    {
        /* define the PFD frequency and R Counter, using the TuneRCounter() */
        pfdFrequency = ADF4153_TuneRcounter(dev,
					    &rCounter);
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
    ADF4153_UpdateLatch(dev,
			ADF4153_CTRL_CONTROL |
                        dev->r2 |
                        ADF4153_R2_COUNTER_RST(ADF4153_CR_ENABLED));

    /* Load the R divider with the new values */
    dev->r1 &= (~ADF4153_R1_MOD(ADF4153_R1_MOD_MASK) &
           ~ADF4153_R1_RCOUNTER(ADF4153_R1_RCOUNTER_MASK) &
           ~ADF4153_R1_PRESCALE(ADF4153_R1_PRESCALE_MASK));
    ADF4153_UpdateLatch(dev,
			ADF4153_CTRL_R_DIVIDER |
                        dev->r1 |
                        ADF4153_R1_MOD(modValue) |
                        ADF4153_R1_RCOUNTER(rCounter) |
                        ADF4153_R1_PRESCALE(devicePrescaler));

    /* Load the N divider with the new values */
    dev->r0 &= (~ADF4153_R0_FRAC(ADF4153_R0_FRAC_MASK) &
           ~ADF4153_R0_INT(ADF4153_R0_INT_MASK));
    ADF4153_UpdateLatch(dev,
			ADF4153_CTRL_N_DIVIDER |
                        dev->r0 |
                        ADF4153_R0_FRAC(fracValue) |
                        ADF4153_R0_INT(intValue));
    /* Disable the Counter Reset */
    dev->r2 &= ~ADF4153_R2_COUNTER_RST(ADF4153_R2_COUNTER_RST_MASK);
    ADF4153_UpdateLatch(dev,
			ADF4153_CTRL_CONTROL |
                        dev->r2 |
                        ADF4153_R2_COUNTER_RST(ADF4153_CR_DISABLED));

    return calculatedFrequency;
}

/***************************************************************************//**
 * @brief Return the value of the channel spacing
 *
 * @param dev - The device structure.
 *
 * @return calculatedFrequency - The actual frequency value that was set.
*******************************************************************************/
unsigned long ADF4153_GetChannelSpacing(ADF4153_dev *dev)
{
    return dev->ADF4153_st.channelSpacing;
}
