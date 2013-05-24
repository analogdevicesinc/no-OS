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
#include "ADXL345.h"        // ADXL345 definitions.

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
 float	x         = 0;  // X-axis's output data.
 float	y         = 0;  // Y-axis's output data.
 float	z         = 0;  // Z-axis's output data.
 char   intSource = 0;  // Value of the ADXL345_INT_SOURCE register.

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
    /* Set pin 7 and pin 8 (J11 connector) as input, to avoid the output pins
    of the Pmod ACL to be connected to an output pin of the microcontroller. */
    PM7 |= (1 << 6);    // P76 (PMOD-IRQA)
    PM7 |= (1 << 7);    // P77 (PMOD-IRQB)
    
    /* Initialize RDKRL78G14. */
    RDKRL78G14_Init();
    
    /* Enable interrupts. */
    __enable_interrupt();
        
    /* Initialize timer. */
    TIME_Init();
    
    /* Initialize the ST7579 Display. */
    ST7579_Init();
    
    /* Initialize ADXL345. */
    if(ADXL345_Init(ADXL345_SPI_COMM) == 0)
    {
        ADI_Component("ADXL345 OK");
    }
    else
    {
        ADI_Component("ADXL345 Error");
    }
    ADXL345_SetTapDetection(ADXL345_SINGLE_TAP |
                            ADXL345_DOUBLE_TAP, // Tap type.
                            ADXL345_TAP_Z_EN,   // Axis control.
                            0x10,               // Tap duration.
                            0x10,               // Tap latency.
                            0x40,               // Tap window. 
                            0x10,               // Tap threshold.
                            0x00);              // Interrupt Pin.
    ADXL345_SetFreeFallDetection(0x01,  // Free-fall detection enabled.
                                 0x05,  // Free-fall threshold.
                                 0x14,  // Time value for free-fall detection.
                                 0x00); // Interrupt Pin.
    
    /* Set the range and the resolution. */
    ADXL345_SetRangeResolution(ADXL345_RANGE_PM_4G, ADXL345_FULL_RES);
    ADXL345_SetPowerMode(0x1);          // Measure mode.
    while(1)
    {
        /* Read and display the output data of each axis. */ 
        ADXL345_GetGxyz(&x, &y, &z);
        ST7579_String(2, 0, "X data:      [g]", 0); 
        ST7579_FloatNumber(2, 42, x, 3, 0); 
        ST7579_String(3, 0, "Y data:      [g]", 0); 
        ST7579_FloatNumber(3, 42, y, 3, 0); 
        ST7579_String(4, 0, "Z data:      [g]", 0); 
        ST7579_FloatNumber(4, 42, z, 3, 0);
        intSource = ADXL345_GetRegisterValue(ADXL345_INT_SOURCE);
        if((intSource & ADXL345_SINGLE_TAP) != 0)
        {
            ST7579_String(5, 0, "Single Tap", 0);
        }
        if((intSource & ADXL345_DOUBLE_TAP) != 0)
        {
            ST7579_String(6, 0, "Double Tap", 0);
        }
        if((intSource & ADXL345_FREE_FALL) != 0)
        {
            ST7579_String(7, 0, "Free-Fall", 0);
        }
        TIME_DelayMs(100);
    }
}
