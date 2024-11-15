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

/*!< Selects the communication port for ADXL345. */
void SetCommunication(double* param, char paramNo);

/*!< Displays the selected communication port. */
void GetCommunication(double* param, char paramNo);

/*!< Displays the acceleration on all 3 axes. */
void GetAcceleration(double* param, char paramNo);

/*!< Displays the acceleration on X axis. */
void GetAccelerationX(double* param, char paramNo);

/*!< Displays the acceleration on Y axis. */
void GetAccelerationY(double* param, char paramNo);

/*!< Displays the acceleration on Z axis. */
void GetAccelerationZ(double* param, char paramNo);

/*!< Displays the state of the interrupts. */
void GetInterrupts(double* param, char paramNo);

#endif  // __COMMAND_H__
