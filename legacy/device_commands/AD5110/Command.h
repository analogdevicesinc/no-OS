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

/* Makes a software reset of the device. */
void DoReset(double* param, char paramNo);

/* Writes to the RDAC register. */
void SetRdac(double* param, char paramNo);

/* Displays the last written value in RDAC register. */
void GetRdac(double* param, char paramNo);

/* Writes the content of RDAC register to EEPROM. */
void DoRdacToEeprom(double* param, char paramNo);

/* Displays the wiper resistance from EEPROM. */
void GetWiper(double* param, char paramNo);

/* Displays the resistance tolerance from EEPROM. */
void GetTolerance(double* param, char paramNo);

/* Turns on/off the device. */
void SetPower(double* param, char paramNo);

/* Displays the power status of the device. */
void GetPower(double* param, char paramNo);

#endif  // __COMMAND_H__
