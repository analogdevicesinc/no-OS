/**************************************************************************//**
*   @file   Command.h
*   @brief  Header file of the commands driver.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2014(c) Analog Devices, Inc.
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

/* Displays the output ADC code of one channel from selected device. */
void GetAdcCode(double* param, char paramNo);

/* Displays the input voltage of one channel from selected device. */
void GetVoltage(double* param, char paramNo);

/* Displays the content of one register from selected device. */
void GetRegister(double* param, char paramNo);

/* Sets the content of one register from selected device. */
void SetRegister(double* param, char paramNo);

/* Performs the self test for both AD7280A devices on the board (one master and
 one slave). */
void DoSelfTestAD7280A(double* param, char paramNo);

/* Reads the status of Alert Pin from AD7280A. */
void GetAlertPinAD7280A(double* param, char paramNo);

/* Enables/disables the AD8280 device. */
void SetEnableAD8280(double* param, char paramNo);

/* Reads the status of Overvoltage Alarm Pin from AD8280. */
void GetAlarmPinOvAD8280(double* param, char paramNo);

/* Reads the status of Undervoltage Alarm Pin from AD8280. */
void GetAlarmPinUvAD8280(double* param, char paramNo);

/* Performs the self test for both AD8280 devices on the board (one master and
 one slave). */
void DoSelfTestAD8280(double* param, char paramNo);

#endif  // __COMMAND_H__
