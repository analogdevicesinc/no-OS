/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for PmodTMP2.
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
#include "AD5160.h"
#include "Console.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/
/*!< List of available commands */
const char* cmdList[] = {"help?",
                         "resistance=",
                         "resistance?",
                         "register=",
                         "register?"};
const char* cmdDescription[] = {
"  -  Displays all available commands.",
"  -  Sets the output resistance between W and B. Accepted values: \r\n\
\t60 .. 10020 - resistance value to be set in ohms.",
"  -  Displays the last written resistance value in ohms.",
"  -  Writes to the DAC register. Accepted values:\r\n\
\t0 .. 255 - the value written to the DAC.",
"  -  Displays the last written value in register."};
const char* cmdExample[] = {
"",
"To set the resistance value to 1000 [ohms], type: resistance=1000",
"",
"To set the register value to 150, type: register=150",
""};
const char  cmdNo = (sizeof(cmdList) / sizeof(const char*));
const short R_AB  = 10000;  /*!< 10 KOhm. R_AB is the nominal resistance. */
const char  R_W   = 60;     /*!< 60 Ohm is the wiper contact resistance. */
/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[9] = {GetHelp, SetResistance, GetResistance,
                               SetRegister, GetRegister};
/*!< Variables holding information about the device */
 unsigned char registerValue = 0; /*!< Last register value written to the device */
 double        outResistance = 0; /*!< Last resistance value written to the device */

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
    if(AD5160_Init() == 0)
    {
        CONSOLE_Print("AD5160 OK\r\n");
        GetHelp(NULL, 0);

        return SUCCESS;
    }
    else
    {
        CONSOLE_Print("AD5160 Error\r\n");

        return ERROR;
    }
}

/***************************************************************************//**
 * @brief Sets the output resistance between W and B, in ohms.
 *
 * @return None.
*******************************************************************************/
void SetResistance(double* param, char paramNo) /*!< "resistance=" command */
{
    double tempFloat = 0;

    /*!< Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 60)
        {
            param[0] = 60;
        }
        if(param[0] > 10020)
        {
            param[0] = 10020;
        }
        outResistance = param[0];
        /*!< Find the binary value corresponding to the output voltage*/
        tempFloat = ((outResistance - R_W) / (double)R_AB) * 256;
        registerValue = (unsigned char)tempFloat;
        /*!< Round the value */
        if(tempFloat - registerValue >= 0.5)
        {
            registerValue ++;
        }
        /*!< Write to DAC register */
        AD5160_Set(registerValue);
        /*!< Send feedback to user */
        outResistance = R_AB * ((double)registerValue / 256) + R_W;
        /*!< Convert float to string and send it to user */
        CONSOLE_Print("%s%d [ohms]\r\n",(char*)cmdList[1],\
                      (unsigned short)outResistance);
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
 * @brief Displays the last written resistance value in ohms.
 *
 * @return None.
*******************************************************************************/
void GetResistance(double* param, char paramNo) /*!< "resistance?" command */
{
    /*!< Get the output resistance using data from registerValue */
    outResistance = R_AB * ((double)registerValue / 256) + R_W;
    /*!< Send the requested value to user */
    CONSOLE_Print("%s%d [ohms]\r\n",(char*)cmdList[1], \
                  (unsigned short)outResistance);
}

/***************************************************************************//**
 * @brief Writes to the DAC register.
 *
 * @return None.
*******************************************************************************/
void SetRegister(double* param, char paramNo) /*!< "register=" command */
{
    /*!< Check if the parameter is valid */
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        if(param[0] > 255)
        {
            param[0] = 255;
        }
        
        registerValue = (unsigned char)param[0];
        /*!< Write to DAC register */
        AD5160_Set(registerValue);
        /*!< Send feedback to user */
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
 * @brief Displays the last written value in register.
 *
 * @return None.
*******************************************************************************/
void GetRegister(double* param, char paramNo) /*!< "register?" command */
{
    /*!< Send the requested value to user */
    CONSOLE_Print("%s%d\r\n",(char*)cmdList[3],registerValue);
}
