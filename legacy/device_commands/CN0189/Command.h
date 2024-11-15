/**************************************************************************//**
*   @file   Command.h
*   @brief  Header file of the commands driver.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
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

/* Displays all available commands. */
void GetHelp(double* param, char paramNo);

/* Displays the raw data on both axes.(40 samples) */
void GetRawData(double* param, char paramNo);

/* Displays the acceleration on both axes.(40 samples) */
void GetAcceleration(double* param, char paramNo);

/* Displays the tilt on both axes.(40 samples) */
void GetTilt(double* param, char paramNo);

/* Calculates the parameters for calibration. */
void DoCalibrationCalculateParam(double* param, char paramNo);

/* Acquire data for calibration, when Xacc=+1[g]. */
void GetCalibrationXplus1G(double* param, char paramNo);

/* Acquire data for calibration, when Xacc=-1[g]. */
void GetCalibrationXminus1G(double* param, char paramNo);

/* Acquire data for calibration, when Yacc=+1[g]. */
void GetCalibrationYplus1G(double* param, char paramNo);

/* Acquire data for calibration, when Yacc=-1[g]. */
void GetCalibrationYminus1G(double* param, char paramNo);

#endif  // __COMMAND_H__
