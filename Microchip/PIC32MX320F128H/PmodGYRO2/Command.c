/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for PmodACL.
 *   @author DBogdan (dragos.bogdan@analog.com)
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
#include "Communication.h"
#include "Command.h"
#include "ADXRS453.h"
#include "Console.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/

/*!< List of available commands */
const char* cmdList[] = {"help?",
                         "temperature?",
                         "measure!"};
const char* cmdDescription[] = {
    "  - Displays all available commands.",
    "  - Displays the ambient temperature.",
    "  - Starts measurement for 30 samples."};
const char* cmdExample[] = {
"",
"",
""};

const char cmdNo = (sizeof(cmdList) / sizeof(const char*));

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[3] = {GetHelp, GetTemperature, DoMeasure};

/******************************************************************************/
/************************ Functions Description *******************************/
/******************************************************************************/

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

    if(ADXRS453_Init() == 0)
    {
        CONSOLE_Print("ADXRS453 OK\r\n");
        GetHelp(NULL, 0);
        return SUCCESS;
    }
    else
    {
        CONSOLE_Print("ADXRS453 Error\r\n");

        return ERROR;
    }
}


/***************************************************************************//**
 * @brief Displays the temperature.
 *
 * @return None.
*******************************************************************************/
void GetTemperature(double* param, char paramNo) /*!< "temperature?" command */
{
    float temperature = 0; /*!< Last read value. */

    /*!< Read temperature from the device */
    temperature = ADXRS453_GetTemperature();
    /*!< Send the requested value to user */
    CONSOLE_Print("temperature=%.1f [C]\r\n",temperature);
}

/***************************************************************************//**
 * @brief Displays the angular velocity for 30 samples.
 *
 * @return None.
*******************************************************************************/
void DoMeasure(double* param, char paramNo) /*!< "measure!" command */
{
    unsigned long  samplesNr       = 0; /*!< Number of samples read from the device */
    unsigned short rxData          = 0; /*!< Angular velocity(raw data)*/
    double         angularVelocity = 0; /*!< Angular velocity(degrees/second) */

    samplesNr = 30;
    while(samplesNr)
    {
        angularVelocity = ADXRS453_GetRate();

        /*!< Send the requested value to user */
        CONSOLE_Print("%.2f [deg/s]\r\n",angularVelocity);
        TIME_DelayMs(200);
        samplesNr --;
    }
    /*!< Measurement process has finished */
    CONSOLE_Print("Measurement done.\r\n");
}
