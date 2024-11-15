/**************************************************************************//**
*   @file   Command.h
*   @brief  Header file of the commands driver.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __COMMAND_H__
#define __COMMAND_H__

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define NULL        ((void *)0)
#define SUCCESS      0
#define ERROR       -1

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
typedef void (*cmdFunction)(double* param, char paramNo);

struct cmd_info {
    char* name;
    char* description;
    char* acceptedValue;
    char* example;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Initializes the device. */
char DoDeviceInit(void);

/* Displays all available commands. */
void GetHelp(double* param, char paramNo);

/* Displays the temperature from AD7193 device. */
void GetAd7193Temp(double* param, char paramNo);

/* Displays the AD7193 device ID. */
void GetAd7193id(double* param, char paramNo);

/* Reset the serial interface with the AD7193. */
void DoAd7193Reset(double* param, char paramNo);

/* Displays the temperature from ADT7310 device. */
void GetAdt7310Temp(double* param, char paramNo);

/* Displays the ADT7310 device ID. */
void GetAdt7310id(double* param, char paramNo);

/* Resets the serial interface with the ADT7310. */
void DoAdt7310Reset(double* param, char paramNo);

/* Displays the input voltage on selected channel. */
void GetVoltage(double* param, char paramNo);

/* Displays the input current on selected channel. */
void GetCurrent(double* param, char paramNo);

/* Displays the temperature detected by the thermocouple connected on selected
channel. */
void GetTempTC(double* param, char paramNo);

#endif  // __COMMAND_H__
