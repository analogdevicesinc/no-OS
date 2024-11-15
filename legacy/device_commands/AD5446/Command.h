/***************************************************************************//**
 *   @file   Command.h
 *   @brief  Header file of the command.c for AD5541A.
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

/* Sets the DAC output voltage. */
void SetVoltage(double* param, char paramNo);

/* Displays last written voltage value to the DAC. */
void GetVoltage(double* param, char paramNo);

/* Writes to the DAC register. */
void SetRegister(double* param, char paramNo);

/* Displays last written value in the DAC register. */
void GetRegister(double* param, char paramNo);

/* Sets the output value of LDAC pin. */
void SetLdacPin(double* param, char paramNo);

/* Sets the output value of CLR pin. */
void SetClrPin(double* param, char paramNo);

/* Displays the value of LDAC pin. */
void GetLdacPin(double* param, char paramNo);

/* Displays the value of CLR pin. */
void GetClrPin(double* param, char paramNo);

#endif  // __COMMAND_H__
