/***************************************************************************//**
 *   @file   Command.c
 *   @brief  Implementation of the commands given by user through UART for PmodAD4.
 *   @author DNechita (dan.nechita@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Command.h"
#include "AD7980.h"
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
"  -  Initiates a conversion and displays the captured voltage.",
"  -  Initiates a conversion and displays the data register in decimal format."};

const char* cmdExample[] = {
"",
"",
""};
const char  cmdNo = (sizeof(cmdList) / sizeof(const char*));
const float VREF = 2.5; /*!< VREF = 2.5V */

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[3] = {GetHelp, GetVoltage, GetRegister};
                               
/*!< Variables holding information about the device */
unsigned short registerValue  = 0; /*!< Last value captured from convertor register */

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
    if(AD7980_Init() == 0)
    {
        CONSOLE_Print("AD7980 OK\r\n");
        GetHelp(NULL, 0);
        return SUCCESS;
    }
    else
    {
        CONSOLE_Print("AD7980 Error\r\n");
        return ERROR;
    }
}

/***************************************************************************//**
 * @brief Initiates a conversion and displays the captured voltage.
 *
 * @return None.
*******************************************************************************/
void GetRegister(double* param, char paramNo) /*!< "register?" command */
{
    unsigned long data = 0;

    /*!< Start a conversion */
    data = AD7980_Conversion();
    /*!< Send the requested value to user */
    CONSOLE_Print("register=%d\r\n",data);
}

/***************************************************************************//**
 * @brief Initiates a conversion and displays the data register in decimal format.
 *
 * @return None.
*******************************************************************************/
void GetVoltage(double* param, char paramNo) /*!< "voltage?" command */
{
    unsigned long data    = 0;
    double        voltage = 0;

    /*!< Start a conversion */
    data = AD7980_Conversion();
    /*!< Get the input voltage from the straight binary raw data */
    voltage = AD7980_ConvertToVolts(data, VREF);
    /*!< Send the requested value to user */
    CONSOLE_Print("voltage=%.3f [V]\r\n",voltage);
}
