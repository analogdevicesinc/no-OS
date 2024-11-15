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

/* Sets the desired output frequency in MHz. */
void SetFrequency(double* param, char paramNo);

/* Displays the output frequency in MHz. */
void GetFrequency(double* param, char paramNo);

/* Sets the value of the desired register. */
void SetRegister(double* param, char paramNo);

/* Displays the value of the desired register. */
void GetRegister(double* param, char paramNo);

/* Displays the current PFD frequency in MHz. */
void GetPFDFrequency(double* param, char paramNo);

#endif  // __COMMAND_H__
