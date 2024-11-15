/***************************************************************************//**
 *   @file   Console.h
 *   @brief  Header file of Console Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __CONSOLE_H__
#define __CONSOLE_H__

/******************************************************************************/
/******************** Macros and Constants Definitions ************************/
/******************************************************************************/
#define UNKNOWN_CMD -1
#define DO_CMD       0
#define READ_CMD     1
#define WRITE_CMD    2

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/*!< Initializes the serial console. */
char CONSOLE_Init(unsigned long baudRate);

/*!< Prints formatted data to console. */
void CONSOLE_Print(char* str, ...);

/*!< Reads one command from console. */
void CONSOLE_GetCommand(char* command);

/*!< Compares two commands and returns the type of the command. */
char CONSOLE_CheckCommands(char*       receivedCmd,
                           const char* expectedCmd,
                           double*     param,
                           char*       paramNo);

#endif /*__CONSOLE_H__*/
