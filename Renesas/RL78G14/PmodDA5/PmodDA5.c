/***************************************************************************//**
 *   @file   Main.c
 *   @brief  Implementation of the program's main function.
 *   @author DNechita (Dan.Nechita@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
#include "RDKRL78G14.h"         // RDKRL78G14 definitions.
#include "ST7579.h"             // ST7579 definitions.
#include "TIME.h"               // TIME definitions.
#include "AD5781.h"             // AD5781 definitions.

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
signed long   dacVal = 0;
signed short  step   = 0;
unsigned long result = 0;


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
 void  main(void)
{
    /* Initialize RDKRL78G14. */
    RDKRL78G14_Init();

    /* Enable interrupts. */
    __enable_interrupt();
        
    /* Initialize timer. */
    TIME_Init();
    
    /* Initialize the ST7579 Display. */
    ST7579_Init();

    /* Initialize AD5781. */
    if(AD5781_Init() == 0)
    {
        ADI_Component("AD5781 OK");
    }
    else
    {
        ADI_Component("AD5781 Error");
    }

    /* Resets the device to its power-on state. */
    AD5781_SoftInstruction(AD5781_SOFT_CTRL_RESET);

    /*Set the DAC output to the normal operating mode. */
    AD5781_DacOuputState(AD5781_OUT_NORMAL);

    /* Configure DAC to use offset binary coding. */
    AD5781_Setup(AD5781_CTRL_BIN2SC);

    /* Sets the value to which the DAC output is set when CLEAR is enabled. */
    AD5781_SetRegisterValue(AD5781_REG_CLR_CODE, AD5781_CLR_CODE_DATA(0x20000));

    /* Performs a soft CLEAR operation. */
    AD5781_SoftInstruction(AD5781_SOFT_CTRL_CLR);

    /* Reads and displays the internal registers. */

    /* Read DAC register. */
    result = AD5781_GetRegisterValue(AD5781_REG_DAC);
    result = (result & 0xFFFFC) >> 2;
    ST7579_String(3, 0, "DAC:", 0);
    ST7579_HexNumber(3, 36, 0x20000, 0);

    /* Read Control register. */
    result = AD5781_GetRegisterValue(AD5781_REG_CTRL);
    result = (result & 0x3FE);
    ST7579_String(4, 0, "CTRL:", 0);
    ST7579_HexNumber(4, 36, 0x0010, 0);

    /* Read ClearCode register. */
    result = AD5781_GetRegisterValue(AD5781_REG_CLR_CODE);
    result = (result & 0xFFFFC) >> 2;
    ST7579_String(5, 0, "CODE:", 0);
    ST7579_HexNumber(5, 36, 0x20000, 0);

    /* Generates a triangle signal. */
    while(1)
    {
        if(dacVal >= (0x00040000 - step))
        {
            step = -256;
        }
        else
        {
            if(dacVal <= (0x00000000 - step))
            {
                step = 256;
            }
        }
        dacVal += step;
        AD5781_SetDacValue(dacVal);
        TIME_DelayMs(1);
    }
}
