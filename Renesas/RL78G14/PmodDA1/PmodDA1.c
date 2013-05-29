/***************************************************************************//**
 *   @file   Main.c
 *   @brief  Implementation of the program's main function.
 *   @author Mihai Bancisor(Mihai.Bancisor@analog.com)
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
#include "AD7303.h"	    // AD7303 definitions.

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
const unsigned char sinetable[250] =
{
    128, 131, 134, 137, 140, 143, 147, 150, 153, 156,
    159, 162, 165, 168, 171, 174, 177, 180, 183, 186,
    189, 192, 194, 197, 200, 202, 205, 208, 210, 212,
    215, 217, 219, 222, 224, 226, 228, 230, 232, 233,
    235, 237, 238, 240, 241, 243, 244, 245, 247, 248,
    249, 250, 251, 251, 252, 253, 253, 254, 254, 255,
    255, 255, 255, 255, 255, 255, 255, 254, 254, 253,
    253, 252, 251, 251, 250, 249, 248, 247, 245, 244,
    243, 241, 240, 238, 237, 235, 233, 232, 230, 228,
    226, 224, 222, 219, 217, 215, 212, 210, 208, 205,
    202, 200, 197, 194, 192, 189, 186, 183, 180, 177,
    174, 171, 168, 165, 162, 159, 156, 153, 150, 147,
    143, 140, 137, 134, 131, 128, 124, 121, 118, 115,
    112, 108, 105, 102,  99,  96,  93,  90,  87,  84,
     81,  78,  75,  72,  69,  66,  63,  61,  58,  55,
     53,  50,  47,  45,  43,  40,  38,  36,  33,  31,
     29,  27,  25,  23,  22,  20,  18,  17,  15,  14,
     12,  11,  10,   8,   7,   6,   5,   4,   4,   3,
      2,   2,   1,   1,   0,   0,   0,   0,   0,   0,
      0,   0,   1,   1,   2,   2,   3,   4,   4,   5,
      6,   7,   8,  10,  11,  12,  14,  15,  17,  18,
     20,  22,  23,  25,  27,  29,  31,  33,  36,  38,
     40,  43,  45,  47,  50,  53,  55,  58,  61,  63,
     66,  69,  72,  75,  78,  81,  84,  87,  90,  93,
     96,  99, 102, 105, 108, 112, 115, 118, 121, 124
};
unsigned char  ct1 = 0;
unsigned short ct2 = 0;

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
    
    /* Initialize AD7303 device. */
    if(AD7303_Init() == 0)
    {
        ADI_Component("AD7303 OK");
    }
    else
    {
        ADI_Component("AD7303 Error");
    }
    ST7579_String(2 ,0, "DAC A: ", 0);
    ST7579_String(3, 10, "4 Hz Sinewave ", 0);
    ST7579_String(4, 0, "DAC B: ", 0);
    ST7579_String(5, 10, "One val/sec ", 0);
    
    while(1)
    {
        /* Output a sine wave on channel A. */
        AD7303_Write((AD7303_LDAC | AD7303_A), sinetable[ct1]);
        /* After second 1 set the channel B output to zero scale. */
        if(ct2 < 1000)
        {
            AD7303_Write((AD7303_LDAC | AD7303_B), 0);
        }
        /* After second 2 set the channel B output to middle scale. */
        if((ct2 >= 1000) && (ct2 < 2000))
        {
            AD7303_Write((AD7303_LDAC | AD7303_B), 128);
        }
        /* After second 2 set the channel B output to full scale. */
        if((ct2 >= 2000) && (ct2 < 3000))
        {
            AD7303_Write((AD7303_LDAC | AD7303_B), 255);
        }
        if(ct1 < 249)
        {
            ct1++;	
        }
        else
        {
            ct1 = 0;    // After 250 while loops, reset ct1.
        }
        if(ct2 < 3000 )
        {
            ct2++;
        }
        else
        {
            ct2 = 0;    // After 3 seconds, reset ct2.
        }
        TIME_DelayMs(1);
    }
}
