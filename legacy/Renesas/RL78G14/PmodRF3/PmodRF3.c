/***************************************************************************//**
 *   @file   Main.c
 *   @brief  Implementation of the program's main function.
 *   @author DBogdan (Dragos.Bogdan@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
#include "RDKRL78G14.h"
#include "ST7579.h"
#include "ADF7023.h"

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
    char          status           = 0;
    unsigned char firstMessage[8]  = "Analog ";
    unsigned char secondMessage[8] = "Devices";
    unsigned char messageLength    = 0;
    unsigned long delay            = 0;
    
    RDKRL78G14_Init();
    EI;
    ST7579_Init();
    status = ADF7023_Init();
    if(status == 0)
    {
        ADI_Component("ADF7023 OK");
    }
    else
    {
        ADI_Component("ADF7023 Err");
    }
    while(1)
    {
#if 0   // Receiver
        ADF7023_ReceivePacket(firstMessage, &messageLength);
        ST7579_String(3, 0, "RX data:", 0);
        ST7579_String(4, 0, firstMessage, 0);
#else   // Transmitter
        ST7579_String(3, 0, "TX data:", 0);
        ST7579_String(4, 0, firstMessage, 0);
        ADF7023_TransmitPacket(firstMessage, 8);
        for(delay = 0; delay < 1000000; delay++)
        {
        };
        ST7579_String(4, 0, secondMessage, 0);
        ADF7023_TransmitPacket(secondMessage, 8);
        for(delay = 0; delay < 1000000; delay++)
        {
        };
#endif
    }
}
