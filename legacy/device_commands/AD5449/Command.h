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

/* Loads and updates the selected DAC with a given value. */
void SetLoadAndUpdate(double* param, char paramNo);

/* Loads selected DAC input register with a given value. */
void SetLoad(double* param, char paramNo);

/* Sets the output value of LDAC pin. */
void SetLdacPin(double* param, char paramNo);

/* Sets the output value of CLR pin. */
void SetClrPin(double* param, char paramNo);

/* Reads from the selected DAC register. */
void GetReadback(double* param, char paramNo);

/* Loads both DAC input registers with a given value. */
void SetLoadAll(double* param, char paramNo);

/* Updates both DAC outputs. */
void SetUpdateAll(double* param, char paramNo);

/* Clears both DAC outputs to zero scale. */
void SetClearToZero(double* param, char paramNo);

/* Clears both DAC outputs to midscale. */
void SetClearToMid(double* param, char paramNo);

/* Displays the value of LDAC pin. */
void GetLdacPin(double* param, char paramNo);

/* Displays the value of CLR pin. */
void GetClrPin(double* param, char paramNo);

#endif  // __COMMAND_H__
