/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for PmodTMP2.
 *   @author Mihai Bancisor (Mihai.Bancisor@analog.com)
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
#include "ADT7420.h"
#include "Console.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/

/*!< List of available commands */
const char* cmdList[] = {
                         "help?",
                         "reset!",
                         "mode=",
                         "mode?",
                         "resolution=",
                         "resolution?",
                         "temperature?"};

const char* cmdDescription[] = {
"  -  Displays all available commands.",
"  -  Resets the ADT7420.",
"  -  Sets the operational mode for ADT7420. Accepted values: \r\n\
\t0 - Continuos conversions. \r\n\
\t1 - One shot. \r\n\
\t2 - 1 SPS mode. \r\n\
\t3 - Shutdown.",
"  -  Displays the selected operational mode.",
"  -  Sets the resolution for ADT7420. Accepted values: \r\n\
\t0 - 13 bits resolution. \r\n\
\t1 - 16 bits resolution.",
"  -  Displays the selected resolution.",
"  -  Displays the temperature."};

const char* cmdExample[] = {"",
                            "",
                            "To set the mode to 1 SPS, type: mode=2.",
                            "",
                            "To set the resolution to 16 bits, type: resolution=1.",
                            "",
                            ""};

const char cmdNo = (sizeof(cmdList) / sizeof(const char*));

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[7] = {GetHelp, DoReset, SetMode, GetMode,\
                               SetResolution, GetResolution, GetTemperature};
/*!< Variables holding information about the device */
unsigned char        currentMode = 0;
extern unsigned char resolutionSetting;
double               temperature = 0;

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
    if(ADT7420_Init() == 0)
    {
        CONSOLE_Print("ADT7420 OK\r\n");
        GetHelp(NULL, 0);

        return SUCCESS;
    }
    else
    {
        CONSOLE_Print("ADT7420 Error\r\n");

        return ERROR;
    }
}

/***************************************************************************//**
 * @brief Resets the ADT7420.
 *
 * @return None.
*******************************************************************************/
void DoReset(double* param, char paramNo) /*!< "reset!" command */
{
    ADT7420_Reset();
    currentMode = 0;
    resolutionSetting  = 0;
    /*!< Send feedback to user. */
    CONSOLE_Print("The device was reset.\r\n");
}
/***************************************************************************//**
 * @brief Sets the operational mode for ADT7420.
 *
 * @param param[0] - selected power mode;
 *        paramNo  - number of parameters (must be >=1); only the first parameter is taken into account;
 *
 * @return None.
*******************************************************************************/
void SetMode(double* param, char paramNo) /*!< "mode=" command */
{
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
        currentMode = (char)param[0];
        ADT7420_SetOperationMode(currentMode);
        /*!< Send feedback to user. */
        CONSOLE_Print("%s%d\r\n",(char*)cmdList[2],currentMode);
    }
    else
    {
        /*!< Display error messages. */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[2], (char*)cmdDescription[2]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[2]);
    }
}
/***************************************************************************//**
 * @brief Displays the selected operational mode.
 *
 * @return None.
*******************************************************************************/
void GetMode(double* param, char paramNo) /*!< "mode?" command */
{
    /* Send the requested value to user */
    CONSOLE_Print("%s%d\r\n",(char*)cmdList[2],currentMode);
}
/***************************************************************************//**
 * @brief Sets the resolution for ADT7420.
 *
 * @param param[0] - selected resolution;
 *        paramNo  - number of parameters (must be >=1); only the first parameter is taken into account;
 *
 * @return None.
*******************************************************************************/
void SetResolution(double* param, char paramNo) /*!< "resolution=" command */
{
    if(paramNo >= 1)
    {
        if(param[0] < 0)
        {
            param[0] = 0;
        }
        if(param[0] > 1)
        {
            param[0] = 1;
        }
        resolutionSetting = param[0];
        ADT7420_SetResolution(resolutionSetting);
        /*!< Send feedback to user */
        CONSOLE_Print("%s%d\r\n",cmdList[4],resolutionSetting);
    }
     else
    {
        /*!< Display error messages. */
        CONSOLE_Print("Invalid parameter!\r\n");
        CONSOLE_Print("%s%s\r\n", (char*)cmdList[4], (char*)cmdDescription[4]);
        CONSOLE_Print("Example: %s\r\n", (char*)cmdExample[4]);
    }
}

/***************************************************************************//**
 * @brief Displays the selected resolution.
 *
 * @return None.
*******************************************************************************/
void GetResolution(double* param, char paramNo) /*!< "resolution?" command */
{
    /* Send the requested value to user */
    CONSOLE_Print("%s%d\r\n",(char*)cmdList[4],resolutionSetting);
}
/***************************************************************************//**
 * @brief Displays the temperature.
 *
 * @return None.
*******************************************************************************/
void GetTemperature(double* param, char paramNo) /*!< "temperature?" command */
{
    if(currentMode != 3)
    {
        /*!< Read the temperature value from the device */
        temperature = ADT7420_GetTemperature();
        /*!< Send the requested value to user */
        CONSOLE_Print("temperature=%.3f [C]\r\n",temperature);
    }
     else
    {
        CONSOLE_Print("Device in shutdown mode. Please change the \
mode in order to display the temperature. \r\n");
    }
}
