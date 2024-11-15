/***************************************************************************//**
 *   @file   Command.h
 *   @brief  Header file of the commands driver.
 *   @author DNechita (Dan.Nechita@analog.com)
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

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*!< Initializes the device. */
char DoDeviceInit(void);

/*!< Displays all available commands. */
void GetHelp(double* param, char paramNo);

/*!< Resets the device. */
void DoReset(double* param, char paramNo);

/*!< Displays the power status of the device. */
void GetPower(double* param, char paramNo);

/*!< Powers on/off the device. */
void SetPower(double* param, char paramNo);

/*!< Initiates a conversion and displays the captured voltage. */
void GetVoltage(double* param, char paramNo);

/*!< Initiates a conversion and displays the value of the data register. */
void GetRegister(double* param, char paramNo);

#endif  // __COMMAND_H__
