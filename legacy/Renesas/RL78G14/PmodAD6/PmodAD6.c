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
#include "AD7091R.h"        // AD7091R definitions.

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
float voltage = 0;
short sample  = 0;
long  delay   = 0;  

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
    
    /* Initialize AD7091R device. */
    if(AD7091R_Init() == 0)
    {
        ADI_Component("AD7091R OK");
    }
    else
    {
        ADI_Component("AD7091R Error");
    }
    while(1)
    {
        /* Power up the device. */
        AD7091R_PowerUp();
        /* A 50ms delay is required after power-up. */
        TIME_DelayMs(55);
               
        sample = AD7091R_ReadSample();
        /* Power-down the device to reduce power consumption. */
        AD7091R_PowerDown();
        /* Display data on LCD. */
        voltage = AD7091R_ConvertToVolts(sample, 2.5);
        ST7579_String(3, 0, "   Voltage[V]:", 0);
        ST7579_String(5, 2, "     ", 1);
        ST7579_FloatNumber(5, 2, voltage, 2, 1);
        TIME_DelayMs(500);
    }
}
