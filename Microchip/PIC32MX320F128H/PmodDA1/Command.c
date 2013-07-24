/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for PmodDA1.
 *   @author DNechita (dan.nechita@analog.com)
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
#include "AD7303.h"
#include "Console.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/

/*!< List of available commands */
const char* cmdList[] = {"help?",
                         "power=",
                         "power?",
                         "voltage=",
                         "voltage?",
                         "register=",
                         "register?"};
const char* cmdDescription[] = {
"  -  Displays all available commands.",
"  -  Powers off/on the selected channel. Accepted values: \r\n\
\tchannel:\r\n\
\t0 - Select channel A1. \r\n\
\t1 - Select channel B1. \r\n\
\tpower status: \r\n\
\t0 - Power off. \r\n\
\t1 - Power on.",
"  -  Displays the power status of the selected channel. Accepted values: \r\n\
\t0 - Select channel A1. \r\n\
\t1 - Select channel B1.",
"  -  Sets the output voltage of the selected channel. Accepted values: \r\n\
\tchannel:\r\n\
\t0 - Select channel A1. \r\n\
\t1 - Select channel B1. \r\n\
\toutput voltage: \r\n\
\t0 .. 3300 - Desired output voltage in milivolts",
"  -  Displays last written voltage value to the DAC for selected channel. \
Accepted values: \r\n\
\t0 - Select channel A1. \r\n\
\t1 - Select channel B1.",
"  -  Writes to the DAC register of the selected channel. Accepted values: \r\n\
\tchannel:\r\n\
\t0 - Select channel A1. \r\n\
\t1 - Select channel B1. \r\n\
\tregister value: \r\n\
\t0 .. 255 - Value written to the DAC.",
"  -  Displays last written value in the DAC register for selected channel. \
Accepted values: \r\n\
\t0 - Select channel A1. \r\n\
\t1 - Select channel B1."
};
const char* cmdExample[] = {
"",
"To power off the channel B1, type: power=1 0",
"To display the power status of the channel B1, type: power?1",
"To set the voltage for channel A1 to 2575mV, type: voltage=0 2575",
"To display the last written voltage value for channel B1, type: voltage?1",
"To set the register value for channel A1 to 150, type: register=0 150",
"To display the last written value in register for channel A1, type: register?0"};
const char  cmdNo = (sizeof(cmdList) / sizeof(const char*));
const float VREF  = 1.65; /*!< Vref = VDD/2 */

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[7] = {GetHelp, SetPower, GetPower, SetVoltage,
                                GetVoltage, SetRegister, GetRegister};
                               
/*!< Variables holding information about the device */
unsigned short registerValue  = 0; /*!< Last register value written to the device */
unsigned char  powerMode      = 1; /*!< Last powerMode value written to the device */
unsigned char  currentChannel = 0; /*!< Last channel selected */
unsigned char  controlBits    = 0; /*!< Upper byte of the Shift Register */
unsigned char  dacRegArray[2] = {0, 0};
unsigned char  powerArray[2]  = {1, 1};

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
    if(AD7303_Init() == 0)
    {
        CONSOLE_Print("AD7303 OK\r\n");
        GetHelp(NULL, 0);
        return SUCCESS;
    }
    else
    {
        CONSOLE_Print("AD7303 Error\r\n");
        return ERROR;
    }
}

/***************************************************************************//**
 * @brief Powers off/on the current channel.
 *
 * @return None.
*******************************************************************************/
void SetPower(double* param, char paramNo) /*!< "power=" command */
{
    double outVoltage = 0;
    double tempFloat  = 0;

    /*!< Check if the parameters are valid */
    if(paramNo >= 2)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
         if(param[0] > 1)
        {
            param[0] = 1;
        }
        currentChannel = (char)param[0];

        if(param[1] < 0)
        {
            param[1] = 0;
        }
        if(param[1] > 1)
        {
            param[1] = 1;
        }
        powerMode = (unsigned char)param[1];

        powerArray[currentChannel] = powerMode;
        /*!< Set the power mode of the selected channel. */
        controlBits &= ~((AD7303_PDA * !currentChannel) |
                         (AD7303_PDB * currentChannel));
        if(powerMode == 0)
        {
            controlBits |= (AD7303_PDA * !currentChannel) |
                           (AD7303_PDB * currentChannel);
        }
        AD7303_Write(controlBits, dacRegArray[currentChannel]);

        /*!< Send feedback to user */
        CONSOLE_Print("%s%d for channel ",(char*)cmdList[1],powerMode);
        if(currentChannel == 0)
        {
            CONSOLE_Print("A1\r\n");
        }
        else
        {
            CONSOLE_Print("B1\r\n");
        }
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
 * @brief Displays the power status for selected channel.
 *
 * @return None.
*******************************************************************************/
void GetPower(double* param, char paramNo) /*!< "power?" command */
{
    /*!< Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if (param[0] < 0)
        {
            param[0] = 0;
        }
        if (param[0] > 1)
        {
            param[0] = 1;
        }
        currentChannel = (char)param[0];
        /*!< Send feedback to user */
        CONSOLE_Print("%s%d for channel ",(char*)cmdList[1],
                                           powerArray[currentChannel]);
        if(currentChannel == 0)
        {
            CONSOLE_Print("A1\r\n");
        }
        else
        {
            CONSOLE_Print("B1\r\n");
        }
    }
    else
    {
        /*!< Display error messages */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[2], (char*)cmdDescription[2]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[2]);
    }
}

/***************************************************************************//**
 * @brief Sets the output voltage of the selected channel.
 *
 * @return None.
*******************************************************************************/
void SetVoltage(double* param, char paramNo) /*!< "voltage=" command */
{
    double outVoltage = 0;
    double tempFloat  = 0;

    /*!< Check if the parameters are valid */
    if(paramNo >= 2)
    {
        if (param[0] < 0)
        {
            param[0] = 0;
        }
        if (param[0] > 1)
        {
            param[0] = 1;
        }
        currentChannel = (char)param[0];

        if(param[1] < 0)
        {
            param[1] = 0;
        }
        if(param[1] > 3299.999)
        {
            param[1] = 3299.999;
        }
        outVoltage = param[1] / 1000;
        
        /*!< Find the binary value corresponding to the output voltage*/
        tempFloat = (outVoltage / (double)(2 * VREF)) * 256;
        registerValue = (unsigned char) tempFloat;
        dacRegArray[currentChannel] = registerValue;
        /*!< Select a channel and write to its register */
        AD7303_Write(controlBits, registerValue);
        outVoltage = 2000 * VREF * ((double)registerValue / 256);
        /*!< Send feedback to user */
        if (powerArray[currentChannel] == 0)
        {
            CONSOLE_Print("The selected channel is disabled. Please use 'power=%d 1'\
 command in order to enable it.\r\n",currentChannel);
        }
        else
        {
            CONSOLE_Print("%s%d [mV] for channel ",(char*)cmdList[3],(short)outVoltage);
            if(currentChannel == 0)
            {
                CONSOLE_Print("A1\r\n");
            }
            else
            {
                CONSOLE_Print("B1\r\n");
            }
        }
    }
    else
    {
        /*!< Display error messages */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[3], (char*)cmdDescription[3]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[3]);
    }
}

/***************************************************************************//**
 * @brief Displays last written voltage value to the DAC for selected channel.
 *
 * @return None.
*******************************************************************************/
void GetVoltage(double* param, char paramNo) /*!< "voltage?" command */
{
    double outVoltage = 0;

    /*!< Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if (param[0] < 0)
        {
            param[0] = 0;
        }
        if (param[0] > 1)
        {
            param[0] = 1;
        }

        currentChannel = (char)param[0];
        registerValue = dacRegArray[currentChannel];
        /*!< Get the output voltage using data from registerValue */
        outVoltage = 2000 * VREF * ((double)registerValue / 256);
        /*!< Send the requested value to user */
        if (powerArray[currentChannel] == 0)
        {
            CONSOLE_Print("The selected channel is disabled. Please use 'power=%d 1'\
 command in order to enable it.\r\n",currentChannel);
        }
        else
        {
            CONSOLE_Print("%s%d [mV] for channel ",(char*)cmdList[3],(short)outVoltage);
            if(currentChannel == 0)
            {
                CONSOLE_Print("A1\r\n");
            }
            else
            {
                CONSOLE_Print("B1\r\n");
            }
        }
    }
    else
    {
        /*!< Display error messages */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[4], (char*)cmdDescription[4]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[4]);
    }
}

/***************************************************************************//**
 * @brief Writes to the DAC register of the selected channel.
 *
 * @return None.
*******************************************************************************/
void SetRegister(double* param, char paramNo) /*!< "register=" command */
{
    double outVoltage = 0;

    /*!< Check if the parameters are valid */
    if(paramNo >= 2)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        if(param[0] > 1)
        {
            param[0] = 1;
        }
        currentChannel = (char)param[0];
        if(param[1] < 0)
        {
            param[1] = 0;
        }
        if(param[1] > 255)
        {
            param[1] = 255;
        }
        registerValue = (unsigned short)param[1];
        dacRegArray[currentChannel] = registerValue;
        /* Select a channel and write to its register */
        AD7303_Write(controlBits, registerValue);
        /*!< Send the requested value to user */
         if (powerArray[currentChannel] == 0)
        {
            CONSOLE_Print("The selected channel is disabled. Please use 'power=%d 1'\
 command in order to enable it.\r\n",currentChannel);
        }
        else
        {
            CONSOLE_Print("%s%d for channel ",(char*)cmdList[5],registerValue);
            if(currentChannel == 0)
            {
                CONSOLE_Print("A1\r\n");
            }
            else
            {
                CONSOLE_Print("B1\r\n");
            }
        }
    }
    else
    {
        /*!< Display error messages */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[5], (char*)cmdDescription[5]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[5]);
    }
}

/***************************************************************************//**
 * @brief Displays last written value in the DAC register for selected channel.
 *
 * @return None.
*******************************************************************************/
void GetRegister(double* param, char paramNo) /*!< "register?" command */
{

    /*!< Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if (param[0] < 0)
        {
            param[0] = 0;
        }
        if (param[0] > 1)
        {
            param[0] = 1;
        }

        currentChannel = (char)param[0];
        registerValue = dacRegArray[currentChannel];
        /*!< Send the requested value to user */
        if (powerArray[currentChannel] == 0)
        {
            CONSOLE_Print("The selected channel is disabled. Please use 'power=%d 1'\
 command in order to enable it.\r\n",currentChannel);
        }
        else
        {
            CONSOLE_Print("%s%d for channel ",(char*)cmdList[5],registerValue);
            if(currentChannel == 0)
            {
                CONSOLE_Print("A1\r\n");
            }
            else
            {
                CONSOLE_Print("B1\r\n");
            }
        }
    }
    else
    {
        /*!< Display error messages */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[6], (char*)cmdDescription[6]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[6]);
    }
}
