/***************************************************************************//**
 *   @file   Main.c
 *   @brief  Implementation of the program's main function.
 *   @author DBogdan (Dragos.Bogdan@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
