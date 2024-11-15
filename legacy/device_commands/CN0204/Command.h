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

/* Sets the shift register of the DAC with the given value. */
void SetRegisterDac(double* param, char paramNo);

/* Displays the last value written to the DAC register. */
void GetRegisterDac(double* param, char paramNo);

/* Sets the output value of ad5751 CLR pin. */
void Setad5751clrPin(double* param, char paramNo);

/* Displays the value of ad5751 CLR pin. */
void Getad5751clrPin(double* param, char paramNo);

/* Sets the A0 address bit from ad5751(from JP1 on the board). */
void Setad5751AddressA0(double* param, char paramNo);

/* Displays the value of A0 address bit from ad5751. */
void Getad5751AddressA0(double* param, char paramNo);

/* Sets the value of ad5751 output range. */
void Setad5751Range(double* param, char paramNo);

/* Displays the value of ad5751 output range. */
void Getad5751Range(double* param, char paramNo);

/* Displays the list of possible faults. */
void Getad5751Fault(double* param, char paramNo);

#endif  // __COMMAND_H__
