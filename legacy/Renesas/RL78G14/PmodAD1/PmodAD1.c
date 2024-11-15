/***************************************************************************//**
 *   @file   Main.c
 *   @brief  Implementation of the program's main function.
 *   @author DNechita(Dan.Nechita@analog.com)
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
#include "AD74xx.h"         // AD74xx definitions.

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
unsigned short sample  = 0;
float          voltage = 0;
long           count   = 0;

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
    
    /* Initialize AD7476. */
    if(AD74xx_Init(AD7476) == 0)
    {
        ADI_Component("AD7476 OK");
    }
    else
    {
        ADI_Component("AD7476 Error");
    }
    ST7579_String(2, 0, "Conversion", 0);
    ST7579_String(3, 0, "Result A0:", 0);	
    ST7579_String(5, 0, "HEX:", 0);
    ST7579_String(6, 0, "DEC:", 0);
    ST7579_String(7, 0, "VOLT:", 0);
    
    while(1)
    {
        /* Power up the device.*/
        AD74xx_PowerUp();
        
        /* Acquire one sample.*/
        sample = AD74xx_GetRegisterValue();
  	
        /* Power down device while other tasks are being executed. */
        AD74xx_PowerDown();
        
        /* Convert the sample in voltage. */
        voltage = AD74xx_ConvertToVolts(sample, 3.3); 
        
        /* Display the value. */
        ST7579_String(5, 36, "      ", 0);
        ST7579_HexNumber(5, 36, sample, 0);
        ST7579_String(6, 36, "      ", 0);
        ST7579_Number(6, 36, sample, 0);
        ST7579_String(7, 36, "      ", 0);
        ST7579_FloatNumber(7, 36, voltage, 2, 0);
        
        /* Wait for 300 milliseconds. */
        TIME_DelayMs(300);
    }
}
