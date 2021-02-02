/**************************************************************************//**
*   @file   Command.h
*   @brief  Header file of the commands driver.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
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

/* Displays the raw data on both axes.(40 samples) */
void GetRawData(double* param, char paramNo);

/* Displays the acceleration on both axes.(40 samples) */
void GetAcceleration(double* param, char paramNo);

/* Displays the tilt on both axes.(40 samples) */
void GetTilt(double* param, char paramNo);

/* Calculates the parameters for calibration. */
void DoCalibrationCalculateParam(double* param, char paramNo);

/* Acquire data for calibration, when Xacc=+1[g]. */
void GetCalibrationXplus1G(double* param, char paramNo);

/* Acquire data for calibration, when Xacc=-1[g]. */
void GetCalibrationXminus1G(double* param, char paramNo);

/* Acquire data for calibration, when Yacc=+1[g]. */
void GetCalibrationYplus1G(double* param, char paramNo);

/* Acquire data for calibration, when Yacc=-1[g]. */
void GetCalibrationYminus1G(double* param, char paramNo);

#endif  // __COMMAND_H__
