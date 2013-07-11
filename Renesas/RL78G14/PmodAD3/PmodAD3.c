/***************************************************************************//**
 *   @file   Main.c
 *   @brief  Implementation of the program's main function.
 *   @author DNechita (dan.nechita@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
********************************************************************************
 *   SVN Revision: $WCREV$
*******************************************************************************/

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
#include "RDKRL78G14.h"     // RDKRL78G14.h definitions.
#include "ST7579.h"         // ST7579 definitions.
#include "TIME.h"           // TIME definitions.
#include "AD7780.h"         // AD7780 definitions.

/******************************************************************************/
/***************************** Global Variables *******************************/
/******************************************************************************/
unsigned char status;
long          rawData;
float         volts;

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
    
    /* Init timer. */
    TIME_Init();
        
    /* Initialize the ST7579 Display. */
    ST7579_Init();
    
    /* Initialize AD7780 device. */
    if(AD7780_Init() == 0)
    {
        ADI_Component("AD7780 OK");
    }
    else
    {
        ADI_Component("AD7780 Error");
    }
    
    /* Display device information on LCD. */
    ST7579_String(2, 0, "AIN:        [mV]", 0);
    ST7579_String(3, 0, "Gain:", 0);
    ST7579_String(4, 0, "Filter:", 0);
    ST7579_String(5, 0, "Conv Err:", 0);
    ST7579_String(6, 0, "Serial Err:", 0);    
    while(1)
    {
        /* Power-up the device. */
        AD7780_PDRST_HIGH;
        
        /* Wait until the device is ready to perform a conversion. */
        AD7780_WaitRdyGoLow();
        
        /* Read data. */
        rawData = AD7780_ReadSample(&status);
        
        /* Power-down the device. */
        AD7780_PDRST_LOW;
        
        /* Convert the raw value to volts. Vref on pmodAD3 is 3.3 by default. */
        volts = AD7780_ConvertToVoltage(rawData, 3.3, 1);

        /* Display the voltage. */
        ST7579_String(2, 36, "      ", 0);
        ST7579_FloatNumber(2, 24, volts, 2, 0);
        
        /* Check gain flag. */
        if(status & AD7780_STAT_GAIN)
        {
            ST7579_String(3, 30, "   ", 0);
            ST7579_String(3, 30, "1", 0);
        }
        else
        {
            ST7579_String(3, 30, "   ", 0);
            ST7579_String(3, 30, "128", 0);
        }
        
        /* Check filter flag. */
        if(status & AD7780_STAT_FILTER)
        {
            ST7579_String(4, 42, "        ", 0);
            ST7579_String(4, 42, "10[Hz]", 0);
        }
        else
        {
            ST7579_String(4, 42, "        ", 0);
            ST7579_String(4, 42, "16.7[Hz]", 0);
        }
        
        /* Check for conversion error. */
        if(status & AD7780_STAT_ERR)
        {
            ST7579_String(5, 54, "     ", 0);
            ST7579_String(5, 54, "Error", 0);
        }
        else
        {
            ST7579_String(5, 54, "     ", 0);
            ST7579_String(5, 54, "OK", 0);
        }
        
        /* Check for serial transfer error. */
        if((status & (AD7780_STAT_PAT1 | AD7780_STAT_PAT0)) != 0x01)
        {
            ST7579_String(6, 66, "     ", 0);
            ST7579_String(6, 66, "Error", 0);
        }
        else
        {
            ST7579_String(6, 66, "     ", 0);
            ST7579_String(6, 66, "OK", 0);
        }
        
        /* Wait 400 milliseconds. */
        TIME_DelayMs(400);
    }
}
