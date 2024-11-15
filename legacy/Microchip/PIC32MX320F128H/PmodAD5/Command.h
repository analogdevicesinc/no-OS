/***************************************************************************//**
 *   @file   Command.h
 *   @brief  Header file of the commands driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
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

/*!< Displays the polarity value. */
void GetPolarity(double* param, char paramNo);

/*!< Sets the polarity value. */
void SetPolarity(double* param, char paramNo);

/*!< Displays the range value. */
void GetRange(double* param, char paramNo);

/*!< Sets the range value. */
void SetRange(double* param, char paramNo);

/*!< Displays the Pseudo Bit value (AD7193_REG_CONF). */
void GetPseudoBit(double* param, char paramNo);

/*!< Sets the Pseudo Bit Value (AD7193_REG_CONF). */
void SetPseudoBit(double* param, char paramNo);

/*!< Displays the value of the data register (AD7193_REG_DATA) for the specified channel. */
void GetRegister(double* param, char paramNo);

/*!< Displays the temperature. */
void GetTemperature(double* param, char paramNo);

/*!< Displays the voltage applied to the specified channel. */
void GetVoltage(double* param, char paramNo);

#endif  // __COMMAND_H__
