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

/*!< Selects a mode of operation for selected channel. */
void SetMode(double* param, char paramNo);

/*!< Displays the selected mode of operation for the selected channel. */
void GetMode(double* param, char paramNo);

/*!< Sets the output voltage for the selected channel. */
void SetVoltage(double* param, char paramNo);

/*!< Displays the output voltage of the selected channel. */
void GetVoltage(double* param, char paramNo);

/*!< Writes the value into the DAC register of the selected channel. */
void SetRegister(double* param, char paramNo);

/*!< Displays the value written in the DAC register of the selected channel. */
void GetRegister(double* param, char paramNo);

#endif  // __COMMAND_H__
