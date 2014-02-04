/**************************************************************************//**
*   @file   Commanad.c
*   @brief  Implementation of the commands given by user through UART for CN0271.
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
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Command.h"
#include "Console.h"
#include "ad7790.h"
#include "TIME.h"

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
        .name = "adcCode?",
        .description = "Displays the ADC Code.",
        .acceptedValue = "",
        .example = "",
    },
    [2] = {
        .name = "temperature?",
        .description = "Displays the temperature measured by the thermocouple.",
        .acceptedValue = "",
        .example = "",
    },
    [3] = {
        .name = "reset!",
        .description = "Resets the serial interface with AD7790.",
        .acceptedValue = "",
        .example = "",
    },
};

const char          cmdNo = (sizeof(cmdList) / sizeof(struct cmd_info));
const float         VREF  = 2.5;
const unsigned char GAIN  = 1;

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[] = {GetHelp, GetAdcCode, GetTemperature, DoReset};

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
    if(AD7790_Init() == 0)
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
 * @brief Displays the ADC Code from AD7790.
 *
 * @return None.
*******************************************************************************/
void GetAdcCode(double* param, char paramNo) // "adcCode?" command
{
    unsigned long dataDac = 0;
    unsigned long data    = 0;
    unsigned char count   = 0;

    /* Calculate the average for 50 samples */
    for (count = 0; count < 50; count++)
    {
        dataDac = AD7790_Read(AD7790_REG_DATA, 2);
        data += dataDac;
    }
    data /= 50;
    /* Send feedback to user */
    CONSOLE_Print("ADC Code=0x%x.\r\n", data);
}

/***************************************************************************//**
 * @brief Displays the temperature measured by the thermocouple.
 *
 * @return None.
*******************************************************************************/
void GetTemperature(double* param, char paramNo) // "temperature?" command
{
    unsigned long dataDac = 0;
    unsigned long data    = 0;
    unsigned char count   = 0;
    float         temp    = 0;
    float         voltage = 0;

    /* Calculate the average for 50 samples */
    for (count = 0; count < 50; count++)
    {
        dataDac = AD7790_Read(AD7790_REG_DATA, 2);
        data += dataDac;
    }
    data /= 50;
    //CONSOLE_Print("ADC Code=0x%x.\r\n", data);
    voltage = AD7790_ConvertToVoltage(data, VREF, GAIN) + 2500; // in mV
    //CONSOLE_Print("Voltage=%.3f[mV].\r\n", voltage);
    temp = voltage / 5; // 5mV/degreeCelsius
    /* Send feedback to user */
    CONSOLE_Print("Temperature=%.3f[C].\r\n", temp);
}

/***************************************************************************//**
 * @brief Resets the serial interface with AD7790.
 *
 * @return None.
*******************************************************************************/
void DoReset(double* param, char paramNo) // "reset!" command
{
    AD7790_Reset();
    /* Send feedback to user */
    CONSOLE_Print("AD7790 was reset.\r\n");
}
