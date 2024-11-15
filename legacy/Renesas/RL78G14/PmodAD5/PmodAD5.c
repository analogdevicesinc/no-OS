/***************************************************************************//**
 *   @file   Main.c
 *   @brief  Implementation of the program's main function.
 *   @author DNechita (Dan.Nechita@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Option Bytes ***********************************/
/******************************************************************************/
#pragma location = "OPTBYTE"
__root const unsigned char opbyte0 = 0x6E; // Watchdog timer settings.
#pragma location = "OPTBYTE"
__root const unsigned char opbyte1 = 0xFF; // Low voltage detector settings.
#pragma location = "OPTBYTE"
__root const unsigned char opbyte2 = 0xE8; // Flash op. mode and osc. settings.
#pragma location = "OPTBYTE"
__root const unsigned char opbyte3 = 0x85; // On-chip Debug Option Byte.

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "RDKRL78G14.h"     // RDKRL78G14 definitions.
#include "ST7579.h"         // ST7579 definitions.
#include "TIME.h"           // TIME definitions.
#include "AD7193.h"         // AD7193 definitions.

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
unsigned long result   = 0;
float         degreesC = 0;
float         voltage  = 0;
long             delay = 0;

/***************************************************************************//**
* @brief Draws the ADI logo and displays the name of ADI component.
*
* @param componentName - The name of the ADI component.
*
* @return None.
*******************************************************************************/
void ADI_Component(unsigned char* string)
{
    const unsigned char adiComponent[7] =
    {
        0xFE, 0xFE, 0x82, 0xC6, 0xEE, 0xFE, 0xFE 
    };
    ST7579_Image(0, 0, adiComponent, 7, 8);
    ST7579_String(0, 9, string, 0);
}

/***************************************************************************//**
 * @brief Main function.
 *
 * @return None.
*******************************************************************************/
void main(void)
{        
    /* Initialize RDKRL78G14. */
    RDKRL78G14_Init();
    
    /* Enable interrupts. */
    __enable_interrupt();
        
    /* Initialize timer. */
    TIME_Init();
    
    /* Initialize the ST7579 Display. */
    ST7579_Init();
    
    /* Initialize AD7193 device. */
    if(AD7193_Init() == 0)
    {
        ADI_Component("AD7193 OK");
    }
    else
    {
        ADI_Component("AD7193 Error");
    }
    /* Reset the device. */
    AD7193_Reset();
    
    /* Allow a period of 500 us before accessing any of the on-chip registers. */
    TIME_DelayUs(500);
         
    /* Calibrate channel 0. */
    AD7193_Calibrate(AD7193_MODE_CAL_INT_ZERO, AD7193_CH_0);
    AD7193_Calibrate(AD7193_MODE_CAL_INT_FULL, AD7193_CH_0);
    
    while(1)
    {
        /* Select bipolar operation and ADC's input range to +-2.5V. */
        AD7193_RangeSetup(0, AD7193_CONF_GAIN_1);

        /* Select channel AIN1(+) - AIN2(-).*/
        AD7193_ChannelSelect(AD7193_CH_0);
        
        /* Perform a single conversion. */
        result = AD7193_SingleConversion();
        ST7579_String(2, 0, "CHANNEL 0:", 0);
        ST7579_String(3, 0, "RAW:          ", 0);
        ST7579_String(4, 0, "1RD:        [mV]", 0);
        ST7579_HexNumber(3, 30, result, 0);
        
        /* The value of the voltage reference on PmodAD5 is 2.5V by default. */
        voltage = AD7193_ConvertToVolts(result, 2.5);
        ST7579_FloatNumber(4, 24, voltage, 2, 0);
        
        /* Continuous reads. */
        result = AD7193_ContinuousReadAvg(10);
        voltage = AD7193_ConvertToVolts(result, 2.5);
        ST7579_String(5, 0, "RAW:        ", 0);
        ST7579_String(6, 0, "AVG:        [mV]", 0);
        ST7579_HexNumber(5, 30, result, 0);
        ST7579_FloatNumber(6, 24, voltage, 2, 0);    
        
        /* Read the temperature. */
        degreesC = AD7193_TemperatureRead();
        ST7579_String(7, 0, "TMP:       C", 0);
        ST7579_FloatNumber(7, 30, degreesC, 2, 0);
    }
}
