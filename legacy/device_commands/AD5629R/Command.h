/***************************************************************************//**
 *   @file   Command.h
 *   @brief  Header file of the commands driver.
 *   @author Lucian Sin (Lucian.Sin@analog.com)
 *
********************************************************************************
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

/* Set the current device type */
void DoDeviceLock(void);

/* Displays all available commands. */
void GetHelp(double* param, char paramNo);

/* Makes a power-on reset. */
void DoReset(double* param, char paramNo);

/* Selects a given power mode for selected DAC. */
void SetPowerMode(double* param, char paramNo);

/* Displays the power mode for one selected DAC. */
void GetPowerMode(double* param, char paramNo);

/* Turns on/off the internal reference. */
void SetIntRef(double* param, char paramNo);

/* Displays the status of the internal reference. */
void GetIntRef(double* param, char paramNo);

/* Loads selected DAC register with a given value. */
void SetLoadN(double* param, char paramNo);

/* Updates the selected DAC with the last value written in register. */
void SetUpdateN(double* param, char paramNo);

/* Loads and updates the selected DAC with a given value. */
void SetLoadNAndUpdateN(double* param, char paramNo);

/* Loads the selected DAC with a given value and updates all DACs. */
void SetLoadNAndUpdateAll(double* param, char paramNo);

/* Enables/Disables the LDAC pin for selected DAC. */
void SetEnLdacPin(double* param, char paramNo);

/* Displays the status(enabled or disabled) of the LDAC pin for a selected DAC*/
void GetEnLdacPin(double* param, char paramNo);

/* Loads Clear Code Register with specific clear code. */
void SetClrCode(double* param, char paramNo);

/* Displays the active clear code. */
void GetClrCode(double* param, char paramNo);

/* Sets the output value of LDAC pin. */
void SetLdacPin(double* param, char paramNo);

/* Sets the output value of CLR pin. */
void SetClrPin(double* param, char paramNo);

/* Displays the value of LDAC pin. */
void GetLdacPin(double* param, char paramNo);

/* Displays the value of CLR pin. */
void GetClrPin(double* param, char paramNo);

#endif  // __COMMAND_H__
