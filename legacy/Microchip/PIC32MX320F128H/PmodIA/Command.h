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

/*!< Displays the temperature. */
void GetTemperature(double* param, char paramNo);

/*!< Sets the start frequency parameter for sweep operation. */
void SetStartFreq(double* param, char paramNo);

/*!< Sets the frequency increment parameter for sweep operation. */
void SetIncFreq(double* param, char paramNo);

/*!< Sets the number of increments parameter for sweep operation. */
void SetIncNum(double* param, char paramNo);

/*!< Displays all three sweep parameters. */
void GetSweepParam(double* param, char paramNo);

/*!< Sets the calibration impedance value. */
void SetCalibImpedance(double* param, char paramNo);

/*!< Displays the measured impedance. */
void GetImpedance(double* param, char paramNo);

/*!< Displays the current frequency. */
void GetCurrentFreq(double* param, char paramNo);

#endif  // __COMMAND_H__
