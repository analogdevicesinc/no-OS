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

/* Displays all available commands. */
void GetHelp(double* param, char paramNo);

/* Resets the AD5781 device. */
void DoReset(double* param, char paramNo);

/* Writes to the DAC register. */
void SetRegister(double* param, char paramNo);

/* Displays last written value to the DAC register. */
void GetRegister(double* param, char paramNo);

/* Writes to the Offset Adjust register. */
void SetOffset(double* param, char paramNo);

/* Displays last written value to the Offset Adjust register. */
void GetOffset(double* param, char paramNo);

/* Writes to the Gain Adjust register. */
void SetGain(double* param, char paramNo);

/* Displays last written value to the Gain Adjust register. */
void GetGain(double* param, char paramNo);

/* Displays the die temperature. */
void GetTemp(double* param, char paramNo);

/* Displays the Vloop - COM voltage. */
void GetVloop(double* param, char paramNo);

/* Displays the Fault register. */
void GetFaultReg(double* param, char paramNo);

/* Displays the output current. */
void GetCurrent(double* param, char paramNo);

/* Sets the output current. */
void SetCurrent(double* param, char paramNo);

#endif  // __COMMAND_H__
