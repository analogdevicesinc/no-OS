/***************************************************************************//**
 *   @file   Command.h
 *   @brief  Header file of the commands driver.
 *   @author Mihai Bancisor (Mihai.Bancisor@analog.com)
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

/*!< Resets the ADT7420. */
void DoReset(double* param, char paramNo);

/*!< Sets the operational mode for ADT7420. */
void SetMode(double* param, char paramNo);

/*!< Displays the selected operational mode. */
void GetMode(double* param, char paramNo);

/*!< Sets the resolution for ADT7420. */
void SetResolution(double* param, char paramNo);

/*!< Displays the selected resolution. */
void GetResolution(double* param, char paramNo);

/*!< Displays the temperature. */
void GetTemperature(double* param, char paramNo);

#endif  // __COMMAND_H__
