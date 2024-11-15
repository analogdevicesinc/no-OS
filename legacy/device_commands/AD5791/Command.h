/***************************************************************************//**
 *   @file   Command.h
 *   @brief  Header file of the commands driver.
 *   @author Lucian Sin (Lucian.Sin@analog.com)
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
struct cmd_info {
    char* name;
    char* description;
    char* acceptedValue;
    char* example;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Internal function for displaying error messages. */
void DisplayError(unsigned char funcNo);

/* Internal function for displaying all the commands with their description. */
void DisplayCmdList();

/* Initializes the device. */
void DoDeviceLock(void);

/* Initializes the device. */
char DoDeviceInit(void);

/* Displays all available commands. */
void GetHelp(double* param, char paramNo);

/* Resets the AD5791 device. */
void DoReset(double* param, char paramNo);

/* Selects the coding style. */
void SetCoding(double* param, char paramNo);

/* Display the current coding style. */
void GetCoding(double* param, char paramNo);

/* Writes to the DAC register. */
void SetRegister(double* param, char paramNo);

/* Displays last written value to the DAC register. */
void GetRegister(double* param, char paramNo);

/* Sets the DAC output voltage. */
void SetVoltage(double* param, char paramNo);

/* Displays the output voltage. */
void GetVoltage(double* param, char paramNo);

/* Selects the DAC output state. */
void SetOutput(double* param, char paramNo);

/* Displays the DAC output state. */
void GetOutput(double* param, char paramNo);

/* Sets/resets the RBUF bit from control register. */
void SetRbuf(double* param, char paramNo);

/* Displays the value of RBUF bit from control register. */
void GetRbuf(double* param, char paramNo);

#endif  // __COMMAND_H__
