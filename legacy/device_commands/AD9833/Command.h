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
/* Selects the type of output. */
void SetOutput(double* param, char paramNo);
/* Displays the type of output. */
void GetOutput(double* param, char paramNo);
/* Loads a frequency value in one selected register. */
void LoadFreqReg(double* param, char paramNo);
/* Displays the value from one selected frequency register. */
void GetFreqRegVal(double* param, char paramNo);
/* Loads a phase value in one selected register. */
void LoadPhaseReg(double* param, char paramNo);
/* Displays the value from one selected phase register. */
void GetPhaseRegVal(double* param, char paramNo);
/* Select the frequency register to be used. */
void SelFreqRegNo(double* param, char paramNo);
/* Displays the selected frequency register. */
void GetFreqRegNo(double* param, char paramNo);
/* Select the phase register to be used. */
void SelPhaseRegNo(double* param, char paramNo);
/* Displays the selected phase register. */
void GetPhaseRegNo(double* param, char paramNo);
/* Select one sleep mode. */
void SetSleepMode(double* param, char paramNo);
/* Displays the selected sleep mode. */
void GetSleepMode(double* param, char paramNo);
/* Sets the programming method. (only for AD9834 & AD9838) */
void SetProgram(double* param, char paramNo);
/* Displays the programming method. */
void GetProgram(double* param, char paramNo);
/* Sets the logic output type. (only for AD9834 & AD9838) */
void SetLogicOut(double* param, char paramNo);
/*  Displays the logic output type. (only for AD9834 & AD9838) */
void GetLogicOut(double* param, char paramNo);


#endif  // __COMMAND_H__
