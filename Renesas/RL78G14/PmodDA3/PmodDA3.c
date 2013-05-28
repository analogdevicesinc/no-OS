/***************************************************************************//**
 *   @file   Main.c
 *   @brief  Implementation of the program's main function.
 *   @author DBogdan (dragos.bogdan@analog.com)
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
#include "RDKRL78G14.h"     // RDKRL78G14 definitions.
#include "ST7579.h"         // ST7579 definitions.
#include "TIME.h"           // TIME definitions.
#include "AD5541A.h"	    // AD5541A definitions.

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
const unsigned short sineTable[250] =
    { 32768, 33591, 34414, 35236, 36056, 36874, 37690, 38503, 39311, 40116,
      40916, 41711, 42501, 43284, 44061, 44830, 45592, 46346, 47091, 47827,
      48553, 49270, 49976, 50671, 51355, 52028, 52688, 53335, 53970, 54591,
      55198, 55792, 56370, 56934, 57482, 58015, 58532, 59033, 59517, 59984,
      60434, 60867, 61281, 61678, 62056, 62416, 62758, 63080, 63383, 63667,
      63931, 64176, 64401, 64605, 64790, 64955, 65099, 65222, 65326, 65408,
      65470, 65512, 65532, 65532, 65512, 65470, 65408, 65326, 65222, 65099,
      64955, 64790, 64605, 64401, 64176, 63931, 63667, 63383, 63080, 62758,
      62416, 62056, 61678, 61281, 60867, 60434, 59984, 59517, 59033, 58532,
      58015, 57482, 56934, 56370, 55792, 55198, 54591, 53970, 53335, 52688,
      52028, 51355, 50671, 49976, 49270, 48553, 47827, 47091, 46346, 45592,
      44830, 44061, 43284, 42501, 41711, 40916, 40116, 39311, 38503, 37690,
      36874, 36056, 35236, 34414, 33591, 32768, 31944, 31121, 30299, 29479,
      28661, 27845, 27032, 26224, 25419, 24619, 23824, 23034, 22251, 21474,
      20705, 19943, 19189, 18444, 17708, 16982, 16265, 15559, 14864, 14180,
      13507, 12847, 12200, 11565, 10944, 10337,  9743,  9165,  8601,  8053,
       7520,  7003,  6502,  6018,  5551,  5101,  4668,  4254,  3857,  3479,
       3119,  2777,  2455,  2152,  1868,  1604,  1359,  1134,   930,   745,
        580,   436,   313,   209,   127,    65,    23,     3,     3,    23,
         65,   127,   209,   313,   436,   580,   745,   930,  1134,  1359,
       1604,  1868,  2152,  2455,  2777,  3119,  3479,  3857,  4254,  4668,
       5101,  5551,  6018,  6502,  7003,  7520,  8053,  8601,  9165,  9743,
      10337, 10944, 11565, 12200, 12847, 13507, 14180, 14864, 15559, 16265,
      16982, 17708, 18444, 19189, 19943, 20705, 21474, 22251, 23034, 23824,
      24619, 25419, 26224, 27032, 27845, 28661, 29479, 30299, 31121, 31944 };
unsigned char point = 0;    // The index in the sineTable[] array.
float val = 0;

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
    
    /* Initialize AD5541A device. */
    if(AD5541A_Init() == 0)
    {
        ADI_Component("AD5541A OK");
    }
    else
    {
        ADI_Component("AD5541A Error");
    }
    /* Set the output voltage to 0V. */
    AD5541A_SetVoltage(0, 2.5);
    /* Generate an 1 Hz sine wave on the J2 connector. */
    ST7579_String(2, 0, "AD5541A outputs ", 0);
    ST7579_String(3, 0, "an 1 Hz sinewave", 0);
    ST7579_String(4, 0, "on the J2       ", 0);
    ST7579_String(5, 0, "connector.      ", 0);
    while(1)
    {
        for(point = 0;point < 250; point++)
        {
            AD5541A_SetRegisterValue(sineTable[point]);
            TIME_DelayMs(4);
        }
    }
}
