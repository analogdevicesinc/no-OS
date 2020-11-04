/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for PmodAD2.
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
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Command.h"
#include "AD799x.h"
#include "Console.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/

/*!< List of available commands */
const char* cmdList[] = {"help?",
                         "voltage?",
                         "register?"};
const char* cmdDescription[] = {
"  -  Diplays all available commands.",
"  -  Initiates a conversion and displays the captured voltage for selected \
channel. Accepted values: \r\n\
\t0..3 - selected channel",
"  -  Initiates a conversion and displays the data register in decimal format \
for selected channel. Accepted values:\r\n\
\t0..3 - selected channel"};
const char* cmdExample[] = {
"",
"To display the voltage captured on channel 1, type: voltage?1",
"To display the register for channel 0, type: register?0"};
const char  cmdNo = (sizeof(cmdList) / sizeof(const char*));
const float VREF = 3.3;    /*!< Vref = 3.3V */

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[3] = {GetHelp, GetVoltage, GetRegister};
                               
/*!< Variables holding information about the device */
unsigned short channelsValues[4] = {0, 0, 0, 0};

/***************************************************************************//**
 * @brief Displays all available commands.
 *
 * @return None.
*******************************************************************************/
void GetHelp(double* param, char paramNo) /*!< "help?" command */
{
    char displayCmd;

    CONSOLE_Print("Available commands:\r\n");
    for(displayCmd = 0; displayCmd < cmdNo; displayCmd++)
    {
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[displayCmd],
                                  (char*)cmdDescription[displayCmd]);
    }
}

/***************************************************************************//**
 * @brief Initializes the device.
 *
 * @return - The result of the initialization.
 *              Example: ERROR  - the device was not initialized or the device
 *                              is not present.
 *                       SUCCES - the device was initialized and the device
 *                              is present.
*******************************************************************************/
char DoDeviceInit(void)
{
    if(AD799x_Init(AD7991, 0) == 0)
    {
        CONSOLE_Print("AD7991 OK\r\n");
        GetHelp(NULL, 0);
        return SUCCESS;
    }
    else
    {
        CONSOLE_Print("AD7991 Error\r\n");
        return ERROR;
    }
}

/***************************************************************************//**
 * @brief Initiates a conversion and displays the captured voltage.
 *
 * @return None.
*******************************************************************************/
void GetVoltage(double* param, char paramNo) /*!< "voltage?" command */
{
    double         voltage        = 0;
    unsigned char  currentChannel = 0;
    unsigned short registerValue  = 0;

    /*!< Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        if(param[0] > 3)
        {
            param[0] = 3;
        }

        currentChannel = (unsigned char)param[0];
        /*!< Power on the selected channel and shutdown the others */
        AD799x_SetConfigurationReg(AD799X_CHANNEL(currentChannel));
        /*!< Start a conversion */
        AD799x_GetConversionResult(&channelsValues[currentChannel],
                                   &currentChannel);
        registerValue = channelsValues[currentChannel];
        /*!< Get the input voltage from the straight binary raw data */
        voltage = AD799x_ConvertToVolts(registerValue, VREF);
        /*!< Send the requested value to user */
        CONSOLE_Print("Voltage=%.3f [V] for channel %d.\r\n",voltage,
                       currentChannel);
    }
    else
    {
        /*!< Display error messages */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[1], (char*)cmdDescription[1]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[1]);
    }
}

/***************************************************************************//**
 * @brief Initiates a conversion and displays the data register in decimal format.
 *
 * @return None.
*******************************************************************************/
void GetRegister(double* param, char paramNo) /*!< "register?" command */
{
    unsigned char  currentChannel = 0;
    unsigned short registerValue  = 0;

    /*!< Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        if(param[0] > 3)
        {
            param[0] = 3;
        }

        currentChannel = (unsigned char)param[0];
        /*!< Power on the selected channel and shutdown the others */
        AD799x_SetConfigurationReg(AD799X_CHANNEL(currentChannel));
        /*!< Start a conversion */
        AD799x_GetConversionResult(&channelsValues[currentChannel],
                                    &currentChannel);
        registerValue = channelsValues[currentChannel];
        /*!< Send the requested value to user */
        CONSOLE_Print("Register=%d for channel %d.\r\n",registerValue,
                       currentChannel);
    }
    else
    {
        /*!< Display error messages */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[2], (char*)cmdDescription[2]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[2]);
    }
}
