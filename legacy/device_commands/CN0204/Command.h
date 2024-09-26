/**************************************************************************//**
*   @file   Command.h
*   @brief  Header file of the commands driver.
*   @author Lucian Sin (Lucian.Sin@analog.com)
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
