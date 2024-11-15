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
#include "AD5628.h"         // AD5628 definitions.

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/

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
    
    /* Initialize AD5628 device. */
    if(AD5628_Init() == 0)
    {
        ADI_Component("AD5628 OK");
    }
    else
    {
        ADI_Component("AD5628 Error");
    }
    ST7579_String(3, 0, "Ch A - Vfs/2", 0);
    ST7579_String(4, 0, "Ch B - Vfs/4", 0);
    ST7579_String(5, 0, "Ch C - Vfs/8", 0);
    
    /* Powers on all channels. */
    AD5628_PowerMode(AD5628_PWRDN_NONE, AD5628_ADDR_DAC_ALL);
    
    /* Writes to Channel A, a value corresponding to 1/2 of Full Scale. */
    AD5628_SetInputRegister(AD5628_CMD(AD5628_CMD_WRITE_INPUT_N_UPDATE_N)|
                            AD5628_ADDR(AD5628_ADDR_DAC_A)|
                            AD5628_DATA_BITS(0x07FF));
    
    /* Writes to Channel B a value corresponding to 1/4 of Full Scale. */
    AD5628_SetInputRegister(AD5628_CMD(AD5628_CMD_WRITE_INPUT_N_UPDATE_N)|
                            AD5628_ADDR(AD5628_ADDR_DAC_B)|
                            AD5628_DATA_BITS(0x03FF));
    
    /* Writes to Channel C a value corresponding to 1/8 of Full Scale. */
    AD5628_SetInputRegister(AD5628_CMD(AD5628_CMD_WRITE_INPUT_N_UPDATE_N)|
                            AD5628_ADDR(AD5628_ADDR_DAC_C)|
                            AD5628_DATA_BITS(0x01FF));
    while(1)
    {
        ;
    }
}
