/***************************************************************************//**
 *   @file   Command.h
 *   @brief  Header file of the commands driver.
 *   @author Istvan Csomortani (istvan.csomortani@analog.com)
 *
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*******************************************************************************/
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

/* Loads selected DAC input register with a given value. */
void SetLoad(double* param, char paramNo);

/* Update selected DAC input register with a given value. */
void SetUpdate(double* param, char paramNo);

/* Loads and updates the selected DAC with a given value. */
void SetLoadAndUpdate(double* param, char paramNo);

/* Reads from the selected DAC register. */
void GetReadback(double* param, char paramNo);

/* Set up the Power Mode register */
void SetPowerMode(double* param, char paramNo);

/* Display the value of the Power Mode register*/
void GetPowerMode(double* param, char paramNo);

/* Sets the LDAC Mask register. */
void SetLdacMask(double* param, char paramNo);

/* Display the LDAC Mask register. */
void GetLdacMask(double* param, char paramNo);

/* Activate a software reset */
void SoftReset(double* param, char paramNo);

/* Activate a software reset */
void HardReset(double* param, char paramNo);

/* Sets the output value of LDAC pin. */
void SetLdacPin(double* param, char paramNo);

/* Displays the value of LDAC pin. */
void GetLdacPin(double* param, char paramNo);

/* Set up the Internal Reference register. */
void SetInternalReference(double* param, char paramNo);

/* Display the Internal Reference register. */
void GetInternalReference(double* param, char paramNo);

#endif  // __COMMAND_H__
