/***************************************************************************//**
 *   @file   Main.c
 *   @brief  Implementation of the program's main function.
 *   @author Mihai Bancisor (Mihai.Bancisor@analog.com)
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
#include "ADP5589.h"	    // ADP5589 definitions.

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
unsigned char val  = 0;
unsigned char keyR = 0;
unsigned char keyP = 0;

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
    
    /* Init ADP5589 device. */
    if(ADP5589_Init() == 0)
    {
        ADI_Component("ADP5589 OK");
    }
    else
    {
        ADI_Component("ADP5589 Error");
    }
    
    /* Init ADP5589 as keyboard decoder on J1 connector. */
    ADP5589_InitKey(PMOD_IOXP_J1);	   
    
    /* Init PWM on pin R3. */
    ADP5589_InitPwm();		            
    ADP5589_SetPwm(1, 1);
    
    ST7579_String(3, 10, "Press 1A", 0);
    ST7579_String(4, 10, "to unlock", 0);
    
    /* Lock keypad, press 1A to unlock. */
    ADP5589_KeyLock(0x25, 0x04, PMOD_IOXP_J1);
    ST7579_String(2, 10, "         ", 0);
    ST7579_String(3, 10, "         ", 0);
    ST7579_String(7 ,0, "R3: 0.5Mhz PWM  ", 0);
    ADP5589_GpioDirection(ADP5589_ADR_GPIO_DIRECTION_A, 0xFE);
    while(1)
    {
        /* Read the value of FIFO1 register. */
        val = ADP5589_GetRegisterValue(ADP5589_ADR_FIFO1);
        
        /* Decode the last key pressed. */
        keyP = ADP5589_KeyDecode(val, ADP5589_EVENT_KEY_PRESSED, PMOD_IOXP_J1);
        
        /* Decode the last key released. */
        keyR = ADP5589_KeyDecode(val, ADP5589_EVENT_KEY_RELEASED, PMOD_IOXP_J1);
        
        /* Display the last key pressed. */
        ST7579_String(2, 0, "Last Pressed", 0);
        ST7579_String(3, 0, "Key: ", 0);
        if (keyP != 'x')
        {
            ST7579_Char(3, 30, keyP, 0);
        }
        
        /* Display the last key released. */
        ST7579_String(4, 0, "Last Released", 0);
        ST7579_String(5, 0, "Key: ", 0);
        if (keyR != 'x')
        {
            ST7579_Char(5, 30, keyR, 0);
        }
        
        /* Read and display the status of R0 pin. */
        if(ADP5589_GetPinState(ADP5589_ADR_GPI_STATUS_A) & 0x01)
        {
            ST7579_String(6, 0, "R0 is HIGH", 0);
        }
        else 
        {
            ST7579_String(6, 0, "R0 is LOW  ", 0);
        }
    }   
}
