/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for PmodACL.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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
