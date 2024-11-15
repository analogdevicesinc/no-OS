/***************************************************************************//**
 *   @file   Command.h
 *   @brief  Header file of the commands for ADF4106 driver.
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

/* Update the specified latch with the current settings */
void SetRegister(double* param, char paramNo);

/* Print the specified latch current value in a human readable manner */
void GetRegister(double* param, char paramNo);

/* Set a desire frequency */
void SetVcoFrequency(double* param, char paramNo);

/* Get a desire frequency */
void GetVcoFrequency(double* param, char paramNo);

#endif  // __COMMAND_H__
