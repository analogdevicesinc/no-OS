/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for PmodTMP2.
 *   @author DNechita (Dan.Nechita@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Command.h"
#include "AD74xx.h"
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
"  -  Initiates a conversion and displays the voltage captured on A0 channel.",
"  -  Initiates a conversion and displays the raw data in decimal format."};
const char* cmdExample[] = {
"",
"",
""};

const char  cmdNo = (sizeof(cmdList) / sizeof(const char*));
const float VDD = 3.3; /*!< VDD = 3.3V */
/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[3] = {GetHelp, GetVoltage, GetRegister};
                               
/*!< Variables holding information about the device */
unsigned short registerValue = 0; /*!< Raw value of the conversion */

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
    if(AD74xx_Init(AD7476) == 0)
    {
        CONSOLE_Print("AD7476 OK\r\n");
        GetHelp(NULL, 0);
        return SUCCESS;
    }
    else
    {
        CONSOLE_Print("AD7476 Error\r\n");
        return ERROR;
    }
}

/***************************************************************************//**
 * @brief Initiates a conversion and displays the voltage captured on A0 channel.
 *
 * @return None.
*******************************************************************************/
void GetVoltage(double* param, char paramNo) /*!< "voltage?" command */
{
    double voltage = 0; /*!< Value of the conversion in Volts */
    /*!< Start a conversion */
    registerValue = AD74xx_GetRegisterValue();
    /*!< Get the input voltage from the straight binary raw data */
    voltage = AD74xx_ConvertToVolts(registerValue, VDD);
    /*!< Send the requested value to user */
    CONSOLE_Print("voltage=%.3f [V]\r\n",voltage);
}

/***************************************************************************//**
 * @brief Initiates a conversion and displays the raw data in decimal format.
 *
 * @return None.
*******************************************************************************/
void GetRegister(double* param, char paramNo) /*!< "register?" command */
{
     /*!< Start a conversion */
        registerValue = AD74xx_GetRegisterValue();
        /*!< Send the requested value to user */
        CONSOLE_Print("register=%d\r\n",registerValue);
}
