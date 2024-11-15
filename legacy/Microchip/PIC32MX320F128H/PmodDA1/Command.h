/***************************************************************************//**
 *   @file   Command.h
 *   @brief  Header file of the commands driver.
 *   @author DNechita (dan.nechita@analog.com)
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

/*!< Powers off/on the selected channel. */
void SetPower(double* param, char paramNo);

/*!< Displays the power status for selected channel. */
void GetPower(double* param, char paramNo);

/*!< Sets the output voltage of the selected channel. */
void SetVoltage(double* param, char paramNo);

/*!< Displays last written voltage value to the DAC for selected channel. */
void GetVoltage(double* param, char paramNo);

/*!< Writes to the DAC register of the selected channel. */
void SetRegister(double* param, char paramNo);

/*!< Displays last written value in the DAC register for selected channel. */
void GetRegister(double* param, char paramNo);

#endif  // __COMMAND_H__
