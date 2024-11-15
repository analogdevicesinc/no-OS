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
#include "AD7091R.h"
#include "Console.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/
/*!< List of available commands */
const char* cmdList[] = {"help?",
                         "reset!",
                         "power?",
                         "power=",
                         "voltage?",
                         "register?"};
const char* cmdDescription[] = {
"  -  Displays all available commands.",
"  -  Initiates a software reset.",
"  -  Displays the power status of the device.",
"  -  Powers on/off the device. Accepted values: \r\n\
\t0 - Power off. \r\n\
\t1 - Power on.",
"  -  Initiates a conversion and displays the captured voltage.",
"  -  Initiates a conversion and displays the value from the register."};
const char* cmdExample[] = {
"",
"",
"",
"To power on the device, type: power=1",
"",
""};
const char  cmdNo = (sizeof(cmdList) / sizeof(const char*));
const float V_REF = 2.5; /*!< Vref = 2.5V */

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
cmdFunction cmdFunctions[6] = {GetHelp, DoReset, GetPower, SetPower, GetVoltage,
                               GetRegister};
/*!< Variables holding information about the device */
unsigned short registerValue = 0; /*!< Raw value of the conversion */
unsigned char  powerStatus   = 1; /*!< 0/1 - Device is powered off/on */
double         voltage       = 0; /*!< Value of the conversion in Volts */

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
    if(AD7091R_Init() == 0)
    {
        CONSOLE_Print("AD7091R OK\r\n");
        GetHelp(NULL, 0);
        return SUCCESS;
    }
    else
    {
        CONSOLE_Print("AD7091R Error\r\n");
        return ERROR;
    }
}

/***************************************************************************//**
 * @brief Resets the device.
 *
 * @return None.
*******************************************************************************/
void DoReset(double* param, char paramNo) /*!< "reset!" command */
{
    AD7091R_SoftwareReset();
    powerStatus = 1;
    /*!< Send feedback to user */
    CONSOLE_Print("Device was reset.\r\n");
}

/***************************************************************************//**
 * @brief Displays the power status of the device.
 *
 * @return None.
*******************************************************************************/
void GetPower(double* param, char paramNo) /*!< "power?" command */
{
    /*!< Send the requested value to user */
    CONSOLE_Print("%s%d\r\n",(char*)cmdList[3],powerStatus);
}

/***************************************************************************//**
 * @brief Powers on/off the device.
 *
 * @return None.
*******************************************************************************/
void SetPower(double* param, char paramNo) /*!< "power=" command */
{
    /*!< Check if the parameter is valid */
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

        powerStatus = (unsigned char)param[0];
        if(powerStatus == 1)
        {
            AD7091R_PowerUp();
        }
        else
        {
            AD7091R_PowerDown();
        }
        /* Send feedback to user */
         CONSOLE_Print("%s%d\r\n",(char*)cmdList[3],powerStatus);
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
 * @brief Initiates a conversion and displays the captured voltage.
 *
 * @return None.
*******************************************************************************/
void GetVoltage(double* param, char paramNo) /*!< "voltage?" command */
{
    if(powerStatus)
    {
        /*!< Start a conversion */
        registerValue = AD7091R_ReadSample();
        /*!< Get the input voltage from the straight binary raw data */
        voltage = ((double)registerValue / 4096) * V_REF;
        /*!< Send the requested value to user */
        CONSOLE_Print("voltage=%.3f [V]\r\n",voltage);
    }
    else
    {
        CONSOLE_Print("Device is powered off. Please use 'power=1' command in \
order to power on the device.\r\n");
    }
}

/***************************************************************************//**
 * @brief Initiates a conversion and displays the value of the data register.
 *
 * @return None.
*******************************************************************************/
void GetRegister(double* param, char paramNo) /*!< "register?" command */
{
    if(powerStatus)
    {
        /*!< Start a conversion */
        registerValue = AD7091R_ReadSample();
        /*!< Send the requested value to user */
        CONSOLE_Print("register=%d\r\n",registerValue);
    }
    else
    {
        CONSOLE_Print("Device is powered off. Please use 'power=1' command in \
order to power on the device.\r\n");
    }
}
