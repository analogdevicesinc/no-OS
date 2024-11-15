/***************************************************************************//**
 *   @file   Command.h
 *   @brief  Header file of the commands driver.
 *   @author Istvan Csomortani (istvan.csomortani@analog.com)
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

/* Loads selected DAC input register with a given value. */
void SetLoad(double* param, char paramNo);

/* Update selected DAC input register with a given value. */
void SetUpdate(double* param, char paramNo);

/* Loads and updates the selected DAC with a given value. */
void SetLoadAndUpdate(double* param, char paramNo);

/* Reads from the selected DAC register. */
void GetReadback(double* param, char paramNo);

/* Set up the Power Mode register */
void SetPowerMode(double* param, char paramNo);

/* Display the value of the Power Mode register*/
void GetPowerMode(double* param, char paramNo);

/* Sets the LDAC Mask register. */
void SetLdacMask(double* param, char paramNo);

/* Display the LDAC Mask register. */
void GetLdacMask(double* param, char paramNo);

/* Activate a software reset */
void SoftReset(double* param, char paramNo);

/* Activate a software reset */
void HardReset(double* param, char paramNo);

/* Sets the output value of LDAC pin. */
void SetLdacPin(double* param, char paramNo);

/* Displays the value of LDAC pin. */
void GetLdacPin(double* param, char paramNo);

/* Set up the Internal Reference register. */
void SetInternalReference(double* param, char paramNo);

/* Display the Internal Reference register. */
void GetInternalReference(double* param, char paramNo);

#endif  // __COMMAND_H__
