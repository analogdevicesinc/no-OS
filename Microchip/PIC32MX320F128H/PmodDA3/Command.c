/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for PmodDA3.
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
#include "AD5541A.h"
#include "Console.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/

/*!< List of available commands */
const char* cmdList[] = {"help?",
                         "voltage=",
                         "voltage?",
                         "register=",
                         "register?"};
const char* cmdDescription[] = {
"  -  Displays all available commands.",
"  -  Sets the DAC output voltage. Accepted values: \r\n\
\t0 .. 2500 - desired output voltage in milivolts.",
"  -  Displays last written voltage value to the DAC.",
"  -  Writes to the DAC register. Accepted values: \r\n\
\t0 .. 65535 - the value written to the DAC.",
"  -  Displays last written value in the DAC register."};
const char* cmdExample[] = {
"",
"To set the voltage to 1250.125mV, type: voltage=1250.125",
"",
"To set the register value to 10000, type: register=10000",
""};
const char  cmdNo = (sizeof(cmdList) / sizeof(const char*));
const float VREF = 2.5; /*!< Vref = 2.5V */

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[5] = {GetHelp, SetVoltage, GetVoltage, SetRegister,
                                GetRegister};
                               
/*!< Variables holding information about the device */
unsigned short registerValue   = 0; /*!< Last register value written to the device */

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
    if(AD5541A_Init() == 0)
    {
        CONSOLE_Print("AD5541A OK\r\n");
        GetHelp(NULL, 0);
        return SUCCESS;
    }
    else
    {
        CONSOLE_Print("AD5541A Error\r\n");
        return ERROR;
    }
}

/***************************************************************************//**
 * @brief Sets the DAC output voltage.
 *
 * @return None.
*******************************************************************************/
void SetVoltage(double* param, char paramNo) /*!< "voltage=" command */
{
    double outVoltage = 0;
    double tempFloat  = 0;

    /*!< Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        if(param[0] > 2499.999)
        {
            param[0] = 2499.999;
        }

        outVoltage = param[0] / 1000;
        /*!< Find the binary value corresponding to the output voltage*/
        tempFloat = (outVoltage / (double) VREF) * 65536;
        registerValue = (short) tempFloat;
        /*!< Write to DAC register */
        AD5541A_SetRegisterValue(registerValue);
        outVoltage = 1000 * VREF * ((double)registerValue / 65536);
         /*!< Send feedback to user */
        CONSOLE_Print("%s%.3f [mV]\r\n",(char*)cmdList[1],outVoltage);
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
 * @brief Displays last written voltage value to the DAC.
 *
 * @return None.
*******************************************************************************/
void GetVoltage(double* param, char paramNo) /*!< "voltage?" command */
{
    double outVoltage = 0;

    /*!< Get the output voltage using data from registerValue */
    outVoltage = 1000 * VREF * ((double)registerValue / 65536);
    /*!< Send feedback to user */
    CONSOLE_Print("%s%.3f [mV]\r\n",(char*)cmdList[1],outVoltage);
}

/***************************************************************************//**
 * @brief Writes to the DAC register.
 *
 * @return None.
*******************************************************************************/
void SetRegister(double* param, char paramNo) /*!< "register=" command */
{
    double outVoltage = 0;
    double tempFloat  = 0;

    /*!< Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        if(param[0] > 65535)
        {
            param[0] = 65535;
        }

        registerValue = (unsigned short)param[0];
        /*!< Write to DAC register */
        AD5541A_SetRegisterValue(registerValue);
        /*!< Send the requested value to user */
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[3],registerValue);
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
 * @brief Displays last written value in the DAC register.
 *
 * @return None.
*******************************************************************************/
void GetRegister(double* param, char paramNo) /*!< "register?" command */
{
    /*!< Send the requested value to user */
    CONSOLE_Print("%s%d\r\n",(char*)cmdList[3],registerValue);
}

