/***************************************************************************//**
 *   @file   Command.h
 *   @brief  Header file of the commands driver.
 *   @author Istvan Csomortani (istvan.csomortani@analog.com)
 *
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

/* Initializes the device. */
char DoDeviceInit(void);

/* Set the current device type */
void DoDeviceLock(void);

/* Displays all available commands. */
void GetHelp(double* param, char paramNo);

/* Set resistor value */
void SetRdac(double* param, char paramNo);

/* Get resistor value */
void GetRdac(double* param, char paramNo);

/* Reset all wiper register to its stored values */
void DoReset(double* param, char paramNo);

/* Restore the specified wiper register setting form the memory */
void DoRestore(double* param, char paramNo);

/* Save the current wiper register settings to the memory */
void SaveSettings(double* param, char paramNo);

/* Write to a given memory address */
void WriteToMem(double* param, char paramNo);

/* Read from a given memory address */
void ReadFromMem(double* param, char paramNo);

/* Decrement a wiper register by 6dB */
void DecRdac6dB(double* param, char paramNo);

/* Decrement all wiper register by 6dB */
void DecAllRdac6dB(double* param, char paramNo);

/* Decrement a wiper register by one */
void DecRdac(double* param, char paramNo);

/* Decrement all wiper register by one */
void DecAllRdac(double* param, char paramNo);

/* Increment a wiper register by 6dB */
void IncRdac6dB(double* param, char paramNo);

/* Increment all wiper register by 6dB */
void IncAllRdac6dB(double* param, char paramNo);

/* Increment a wiper register by one */
void IncRdac(double* param, char paramNo);

/* Increment all wiper register by one */
void IncAllRdac(double* param, char paramNo);

/* Set the state of the Write Protect (WP) pin */
void SetWrPin(double* param, char paramNo);

/* Return the current value of the Write Protect (WP) pin */
void GetWrPin(double* param, char paramNo);

/* Set the state of the Hardware Override Preset (PR) pin */
void SetPrPin(double* param, char paramNo);

/* Return the current value of the Hardware Override Preset (PR) pin */
void GetPrPin(double* param, char paramNo);

/* Read one of the Tolerance register */
void GetTolerance(double* param, char paramNo);

#endif  // __COMMAND_H__
