/**************************************************************************//**
*   @file   command.h
*   @brief  Command header file
*   @author Istvan Csomortani (istvan.csomortani@analog.com)
*
*******************************************************************************
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
*******************************************************************************
*   SVN Revision: 2044
******************************************************************************/

#ifndef __COMMAND__H__
#define __COMMAND__H__

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

/***************************************************************************/
/************************ Functions Definitions ****************************/
/***************************************************************************/

void DisplayError(unsigned char funcNo);
void DisplayCmdList();
void DoDeviceLock(void);
char DoDeviceInit(void);

/* help? implementation - Print available commands*/
void GetHelp(double* param, char paramNo);
/* dataReg= implementation - Set the value of a data registers*/
void SetDataRegister(double* param, char paramNo);
/* controlReg= implementation - Set the value of a control registers*/
void SetControlRegister(double* param, char paramNo);
/* register? implementation - Register ReadBack*/
void GetRegister(double* param, char paramNo);
/* power= implementation - Set the power state of a specified channel */
void SetPower(double* param, char paramNo);
/* power? implementation - Displays the power state of a specified channel */
void GetPower(double* param, char paramNo);
/* range= command implementation - Displays the range of a specified channel */
void SetRange(double* param, char paramNo);
/* range? command implementation - Displays the range of a specified channel */
void GetRange(double* param, char paramNo);
/* voltage= implementation - Set the value of voltage */
void SetVoltage(double* param, char paramNo);
/* voltage? implementation - Display the value of voltage*/
void GetVoltage(double* param, char paramNo);
/* current= implementation - Set the value of current*/
void SetCurrent(double* param, char paramNo);
/* current? implementation - Display the value of current */
void GetCurrent(double* param, char paramNo);
/*! Read back the Status register and print any faults or errors.*/
void EvaluateStatus(double* param, char paramNo);
/*! Ensure that the SPI pins are working correctly.*/
void TestSpi(double* param, char paramNo);

#endif
