/***************************************************************************//**
 *   @file   Main.c
 *   @brief  Implementation of the program's main function.
 *   @author Mihai Bancisor (Mihai.Bancisor@analog.com)
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
#include "ADXRS453.h"	    // ADXRS453 definitions.
#include "Communication.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
float adxrs453Rate        = 0;
float adxrs453Temperature = 0;

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
    
    /* Initialize ADXL345. */
    if(ADXRS453_Init() == 0)
    {
        ADI_Component("ADXRS453 OK");
    }
    else
    {
        ADI_Component("ADXRS453 Error");
    }
   
    while(1)
    {
        /* Read and display the Rate data. */
        adxrs453Rate = ADXRS453_GetRate();
        ST7579_String(3, 6, "Rate:        ", 0);
        ST7579_FloatNumber(3, 42, adxrs453Rate, 2, 0);
        ST7579_String(4, 42, "deg/sec", 0);
        
        /* Read and display the Temperature data. */
        adxrs453Temperature = ADXRS453_GetTemperature();
        ST7579_String(6, 6, "Temp:       C", 0);
        ST7579_FloatNumber(6, 42, adxrs453Temperature, 1, 0);
        
        /* Add a short delay between measurements. */
        TIME_DelayMs(150);
    }
}
