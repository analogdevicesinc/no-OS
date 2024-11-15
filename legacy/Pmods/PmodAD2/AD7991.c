/***************************************************************************//**
*   @file   AD7991.c
*   @brief  AD7991 Driver for Microblaze processor.
*   @author ATofan (alexandru.tofan@analog.com)
********************************************************************************
* Copyright 2012(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include "i2c_axi.h"
#include "AD7991.h"
#include "xil_io.h"

/*****************************************************************************/
/************************** Macros Definitions *******************************/
/*****************************************************************************/

/******************************************************************************
 * @brief Initialize the AD7911 device.
 *
 * @return None
******************************************************************************/
void AD7991_Init(void)
{
    int cfgValue = 0;
    unsigned char txBuffer[1] = {0x00};
    
    // Set default Configuration Register Values
    /*
    Channel 3 - Selected for conversion
    Channel 2 - Selected for conversion 
    Channel 1 - Selected for conversion
    Channel 0 - Selected for conversion
    reference Voltage - Supply Voltage
    filtering on SDA and SCL - Enabled
    bit Trial Delay - Enabled
    sample Interval Delay - Enabled
    */
    cfgValue = (1 << CH3)           |
               (1 << CH2)           |
               (1 << CH1)           |
               (1 << CH0)           |
               (0 << REF_SEL)       |
               (0 << FLTR)          |
               (0 << bitTrialDelay) |
               (0 << sampleDelay);

    txBuffer[0] = cfgValue;
    // Write to the Configuration Register
    I2C_Write_axi(IIC_BASEADDR, AD7991IICAddr, -1, 1, txBuffer);
}

/**************************************************************************//**
 * @brief Configure the AD7911 device.
 *
 * @param chan3 - CHAN3 bit in control register.
 *
 * @param chan2 - CHAN2 bit in control register.
 *
 * @param chan1 - CHAN1 bit in control register.
 *
 * @param chan0 - CHAN0 bit in control register.
 *
 * @param ref - REF bit in control register.
 *
 * @param filter - FILTER bit in control register.
 * 
 * @param bit - BIT bit in control register. 
 * 
 * @param sample - SAMPLE bit in control register.
 * 
 * @return None
******************************************************************************/
void AD7991_Config(char chan3, char chan2, char chan1, char chan0, char ref, char filter, char bit, char sample)
{
    int cfgValue = 0;
    unsigned char txBuffer[1] = {0x00};
    
    // Set Configuration Register Values
    cfgValue = (chan3 << CH3)         | // Read Channel 3
               (chan2 << CH2)         | // Read Channel 2
               (chan1 << CH1)         | // Read Channel 1
               (chan0 << CH0)         | // Read Channel 0
               (ref << REF_SEL)       | // Select External reference / Vcc as reference
               (filter << FLTR)       | // filter IIC Bus
               (bit << bitTrialDelay) | // Delay IIC Commands
               (sample << sampleDelay); // Delay IIC Messages

    txBuffer[0] = cfgValue;

    // Write to the Configuration Register
    I2C_Write_axi(IIC_BASEADDR, AD7991IICAddr, -1, 1, txBuffer);
}

/**************************************************************************//**
 * @brief Display read data from AD7911 device to the UART.
 *
 * @param nr_cnv - Number of conversions to be made.
 *
 * @param vref - Voltage Reference value in mV.
 *
 * @return None
******************************************************************************/
void AD7991_Print(u32 nr_cnv, u32 vref)
{
    int rxData;
    unsigned char rxBuffer[2] = {0x00, 0x00};
    char c[8] = {'0','.','0','0','0','0',0};
    u32 nr;
	int i;
	int j;
    
	for(j=0;j<nr_cnv;j++)
	{
		i = 5;
		c[7]= 0;
		c[6] = '0';
		c[5] = '0';
		c[4] = '0';
		c[3] = '0';
		c[2] = '0';
		c[1] = '.';
		c[0] = '0';
        
		// Read data from AD7991
    	I2C_Read_axi(IIC_BASEADDR, AD7991IICAddr, -1, 2, rxBuffer);
    	rxData = (rxBuffer[0] << 8) | rxBuffer[1];
        
    	// Process read voltage value
		nr = (rxData & ADCValue) * vref / 4096;
		while(nr>0)
		{
			// Transform hex value into char
			// for display purposes
			c[i] = (nr % 10) + 48;
			nr = nr / 10;
			i = i - 1;
            
			// Skip the 2nd position (it is pre-loaded with '.')
			if(i == 1)
			{
				i = i-1;
			}
		}
        
		i = 5;
        
		// Determine if received data from AD7991 is correct
		// by checking the first 2 Leading zeros
		if((rxData & LeadingZeros) == 0)
		{
			xil_printf("\n\r");
			xil_printf("Channel %x = ",((rxData & CHID) >> 12));
			xil_printf("%s V\n\r",c);
		}
		else
		{
			xil_printf("\n\rRead Error occurred!\n\r");
		}
	}
}
