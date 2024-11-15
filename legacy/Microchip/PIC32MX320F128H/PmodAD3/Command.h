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

/*!< Power off/on the device. */
void SetPower(double* param, char paramNo);

/*!< Select the filter frequency. */
void SetFilter(double* param, char paramNo);

/*!< Select gain. */
void SetGain(double* param, char paramNo);

/*!< Displays the conversion status for 'register?' and 'voltage?' commands. */
void SetStatus(double* param, char paramNo);

/*!< Reads one sample from the input channel and displays the data register. */
void GetRegister(double* param, char paramNo);

/*!< Reads one sample from the input channel, converts it to voltage and displays the voltage. */
void GetVoltage(double* param, char paramNo);

/*!< Reads a number of samples of the input channel and displays the voltage. */
void GetSamples(double* param, char paramNo);

#endif  // __COMMAND_H__
