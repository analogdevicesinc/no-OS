/***************************************************************************//**
 *   @file   Command.h
 *   @brief  Header file of the commands driver.
 *   @author Istvan Csomortani (istvan.csomortani@analog.com)
 *
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
