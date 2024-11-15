/***************************************************************************//**
 *   @file   Main.c
 *   @brief  Implementation of the program's main function.
 *   @author DBogdan (dragos.bogdan@analog.com)
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
#include "AD5933.h"         // AD5933 definitions.
#include <math.h>           // Math definitions.

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
float           temperature = 0;
float           impedanceK  = 0;
double          impedance   = 0;
double          gainFactor  = 0;

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
    
    /* Initialize AD5933 device. */
    if(AD5933_Init() == 0)
    {
        ADI_Component("AD5933 OK");
    }
    else
    {
        ADI_Component("AD5933 Error");
    }
    
    /* Reset the device. */
    AD5933_Reset();
    
    /* Select the source of the AD5933 system clock. */
    AD5933_SetSystemClk(AD5933_CONTROL_INT_SYSCLK, 0);
    
    /* Set range and gain. */
    AD5933_SetRangeAndGain(AD5933_RANGE_2000mVpp, AD5933_GAIN_X5);
    
    /* Read the temperature. */
    temperature = AD5933_GetTemperature();
    ST7579_String(2, 0, "Temp:      C", 0);
    ST7579_FloatNumber(2, 30, temperature, 2, 0);
    
    /* Configure the sweep parameters */
    AD5933_ConfigSweep(30000,       // 30 KHz
                       1000,        // 1000 Hz
                       500);        // 500 increments
    
    /* Start the sweep operation. */
    AD5933_StartSweep();
    
    /* Calculate the gain factor for an impedance of 47kohms. */
    gainFactor = AD5933_CalculateGainFactor(47000,
                                            AD5933_FUNCTION_REPEAT_FREQ);
    ST7579_String(3, 0, "Gain factor was", 0);
    ST7579_String(4, 0, "calc. for 47kohm", 0);
    ST7579_String(5, 0, "Impedance[kohm]:", 0);
    while(1)
    {
        /* Calculates the impedance between the VOUT and VIN pins. */
        impedance = AD5933_CalculateImpedance(gainFactor,
                                              AD5933_FUNCTION_REPEAT_FREQ);
        impedanceK = (float)impedance;
        impedanceK /= 1000;
        ST7579_String(6, 0, "       ", 0);
        ST7579_FloatNumber(6, 0, impedanceK, 3, 0);
    }   
}
