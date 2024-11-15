/***************************************************************************//**
 *   @file   Command.h
 *   @brief  Header file of the commands driver.
 *   @author Dan Nechita (Dan.Nechita@analog.com)
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

/*!< Displays the set range for the selected channel. */
void GetRange(double* param, char paramNo);

/*!< Selects the range of the selected channel. */
void SetRange(double* param, char paramNo);

/*!< Displays the current power mode. */
void GetPowerMode(double* param, char paramNo);

/*!< Selects the power mode of the device. */
void SetPowerMode(double* param, char paramNo);

/*!< Enables or disabled conversions on selected channel. */
void EnableCh(double* param, char paramNo);

/*!< Reads 50 samples from the enabled channels and displays the data in pF. */
void GetSamples(double* param, char paramNo);

/*!< Checks if any of the two buttons are pressed or released. */
void GetButtonsStatus(double* param, char paramNo);

/*!< Resets the device. */
void DoReset(double* param, char paramNo);

#endif  // __COMMAND_H__
