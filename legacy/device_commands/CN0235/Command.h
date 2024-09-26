/**************************************************************************//**
*   @file   Command.h
*   @brief  Header file of the commands driver.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2014(c) Analog Devices, Inc.
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
******************************************************************************/

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
