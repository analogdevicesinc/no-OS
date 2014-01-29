/**************************************************************************//**
*   @file   Commanad.c
*   @brief  Implementation of the commands given by user through UART for CN0188.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2014(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Command.h"
#include "Console.h"
#include "ad7171.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/

const struct cmd_info cmdList[] = {
    [0] = {
        .name = "help?",
        .description = "Displays all available commands.",
        .acceptedValue = "",
        .example = "",
    },
    [1] = {
        .name = "voltage?",
        .description = "Displays the input voltage.",
        .acceptedValue = "",
        .example = "",
    },
    [2] = {
        .name = "adcCode?",
        .description = "Displays the output ADC code.",
        .acceptedValue = "",
        .example = "",
    },
};

const char cmdNo = (sizeof(cmdList) / sizeof(struct cmd_info));

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[] = {GetHelp, GetVoltage, GetAdcCode};

/* Variables holding information about the device */
float gain      = 0;
float vRef      = 0;
float IdPattern = 0;
AD7171_type deviceType;

/***************************************************************************//**
 * @brief Displays all available commands.
 *
 * @return None.
*******************************************************************************/
void GetHelp(double* param, char paramNo) // "help?" command
{
    unsigned char displayCmd;

    CONSOLE_Print("Available commands:\r\n");
    for(displayCmd = 0; displayCmd < cmdNo; displayCmd++)
    {
        CONSOLE_Print("%s - %s %s\r\n", (char*)cmdList[displayCmd].name,
                                    (char*)cmdList[displayCmd].description,
                                    (char*)cmdList[displayCmd].acceptedValue);
    }
}

/***************************************************************************//**
 * @brief Internal function for displaying all the command with its description.
 *
 * @return None.
*******************************************************************************/
void DisplayCmdList()
{
    unsigned char displayCmd;

    for(displayCmd = 0; displayCmd < cmdNo; displayCmd++)
    {
        CONSOLE_Print("%s - %s\r\n", (char*)cmdList[displayCmd].name, \
                                     (char*)cmdList[displayCmd].description);
    }
}

/***************************************************************************//**
 * @brief Displays error message.
 *
 * @return None.
*******************************************************************************/
void DisplayError(unsigned char funcNo)
{
    /* Display error messages */
    CONSOLE_Print("Invalid parameter!\r\n");
    CONSOLE_Print("%s - %s %s\r\n", (char*)cmdList[funcNo].name, \
                                    (char*)cmdList[funcNo].description, \
                                    (char*)cmdList[funcNo].acceptedValue);
    CONSOLE_Print("Example: %s\r\n", (char*)cmdList[funcNo].example);
}

/***************************************************************************//**
 * @brief Sets the current device type.
 *
 * @return None.
*******************************************************************************/
void DoDeviceLock(void)
{
    char device = 0;
    char deviceLocked = -1;

    while(deviceLocked < 0)
    {
        CONSOLE_Print("Please specify your device.\r\n\
For CN0188 type: 0\r\n\
For CN0218 type: 1\r\n\
For CN0240 type: 2\r\n\
");
        CONSOLE_GetCommand(&device);
        if((device >= 0x30) && (device <= 0x32))
        {
            deviceLocked = 1;
            switch(device)
            {
                case 0x30 :
                {
                	deviceType = ID_AD7171;
                	gain      = 49.7;
                	vRef      = 2.5;
                	IdPattern = 8 + 5;
                    break;
                }
                case 0x31 :
                {
                	deviceType = ID_AD7171;
                    gain      = 5;
                    vRef      = 2.5;
                    IdPattern = 8 + 5;
                    break;
                }
                case 0x32 :
                {
                	deviceType = ID_AD7170;
                    gain      = 40;
                    vRef      = 5;
                    IdPattern = 8 + 5;
                    break;
                }
            }
        }
        else
        {
            CONSOLE_Print("Please type the number corresponding for \
your device!\r\n");
        }
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
	DoDeviceLock();

    if(ad7171_Init(deviceType) == 0)
    {
        CONSOLE_Print("Device OK\r\n");
        DisplayCmdList();
        return SUCCESS;
    }
    else
    {
        CONSOLE_Print("Device Error\r\n");
        return ERROR;
    }
}

/***************************************************************************//**
 * @brief Displays the input voltage in [mV].
 *
 * @return None.
*******************************************************************************/
void GetVoltage(double* param, char paramNo) // "voltage?" command
{
    float          vin     = 0;
    unsigned char  pattern = 0;

    vin = ad7171_GetVoltage(0x0, &pattern, vRef);
    vin = (vin / gain) * 1000;

    /* Send feedback to user */
    if (pattern == IdPattern)
    {
        CONSOLE_Print("The serial transfer was correct.\r\n");
        CONSOLE_Print("Voltage=%.3f[mV].\r\n", vin);
    }
    else
    {
        CONSOLE_Print("The serial transfer was not correct.\r\n");
    }
}

/***************************************************************************//**
 * @brief Displays the output ADC code.
 *
 * @return None.
*******************************************************************************/
void GetAdcCode(double* param, char paramNo) // "adcCode?" command
{
    unsigned short adcCode = 0;
    unsigned char  pattern = 0;

    adcCode = ad7171_GetAdcCode(0x0, &pattern);
    /* Send feedback to user */
    if (pattern == IdPattern)
    {
        CONSOLE_Print("The serial transfer was correct.\r\n");
        CONSOLE_Print("ADC Code=%d.\r\n", adcCode);
    }
    else
    {
        CONSOLE_Print("The serial transfer was not correct.\r\n");
    }
}
