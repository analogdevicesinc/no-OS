/***************************************************************************//**
 *   @file   Main.c
 *   @brief  Implementation of the program's main function.
 *   @author DNechita(Dan.Nechita@analog.com)
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
#include "ADXL362.h"        // ADXL362 definitions.

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
unsigned char status      = 0;
float         xAxis       = 0;
float         yAxis       = 0;
float         zAxis       = 0;
float         temperature = 0;

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
    of the Pmod ACL2 to be connected to an output pin of the microcontroller. */
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
    
    /* Init device. */
    if(ADXL362_Init() == 0)
    {
        ADI_Component("ADXL362 OK");
    }
    else
    {
        ADI_Component("ADXL362 Error");
    }
    
    /* Put the device in standby mode. */
     ADXL362_SetPowerMode(0);
    
    /* Set accelerometer's output data rate to: 12.5 Hz. */
    ADXL362_SetOutputRate(ADXL362_ODR_12_5_HZ);
    
    /* Setup the activity and inactivity detection. */
    ADXL362_SetRegisterValue(
                  ADXL362_ACT_INACT_CTL_LINKLOOP(ADXL362_MODE_LINK),
                  ADXL362_REG_ACT_INACT_CTL,
                  1);
    ADXL362_SetupActivityDetection(1, 30, 1);
    ADXL362_SetupInactivityDetection(1, 700, 25);
    
    /* Start the measurement process. */
    ADXL362_SetPowerMode(1);
    
    /* Clear ACT and INACT bits by reading the Status Register. */
    ADXL362_GetRegisterValue(&status,
                             ADXL362_REG_STATUS,
                             1);
    
    /* Select the 4g measurement range. */
    ADXL362_SetRange(ADXL362_RANGE_4G);
    
    while(1)
    {
    
    /* Wait for the detection of an activity or inactivity or for available
    data. */
        do
        {
            ADXL362_GetRegisterValue(&status, ADXL362_REG_STATUS, 1);
            
        }while(
               ((status & ADXL362_STATUS_DATA_RDY) == 0) && 
               ((status & ADXL362_STATUS_INACT) == 0)    &&
               ((status & ADXL362_STATUS_ACT == 0)));
        
        /* Get the data from the device and display it. */    
        if(status & ADXL362_STATUS_DATA_RDY)
        {    
            //ADXL362_GetXyz(&xAxis, &yAxis, &zAxis);
            ADXL362_GetGxyz(&xAxis, &yAxis, &zAxis);
            temperature = ADXL362_ReadTemperature();
            ST7579_String(2, 0, "X:        g", 0);
            //ST7579_Number(2, 12, xAxis, 0);
            ST7579_FloatNumber(2, 18, xAxis, 3, 0);
            ST7579_String(3, 0, "Y:        g", 0);
            //ST7579_Number(3, 12, yAxis, 0);
            ST7579_FloatNumber(3, 18, yAxis, 3, 0);
            ST7579_String(4, 0, "Z:        g", 0);
            //ST7579_Number(4, 12, zAxis, 0);
            ST7579_FloatNumber(4, 18, zAxis, 3, 0);
            ST7579_String(5, 0, "TMP:       C", 0);
            ST7579_FloatNumber(5, 22, temperature, 2, 0);
        }
        
        /* Display the activity/inactivity status. */
        if(status & ADXL362_STATUS_INACT)
        {
            ST7579_String(6, 0, "          ", 0);
            ST7579_String(6, 0, "Inactivity", 0);
            ST7579_String(7, 0, "detected", 0);
        }
        else if(status & ADXL362_STATUS_ACT)
        {
            ST7579_String(6, 0, "          ", 0);
            ST7579_String(6, 0, "Activity", 0);
            ST7579_String(7, 0, "detected", 0);
        }
        TIME_DelayMs(50);
    }   
}
