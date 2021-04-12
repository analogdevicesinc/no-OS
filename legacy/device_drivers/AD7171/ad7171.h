/***************************************************************************//**
*   @file   ad7171.h
*   @brief  Header file of ad7171 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
********************************************************************************
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
*******************************************************************************/

#ifndef _ad7171_H_
#define _ad7171_H_
/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Communication.h"
#include "xil_types.h"
#include "xparameters.h"

/******************************************************************************/
/********************* Macros and Constants Definitions ***********************/
/******************************************************************************/
#define ad7171_SLAVE_ID     1

/* Supported devices */
typedef enum {
    ID_AD7170,
    ID_AD7171
} AD7171_type;

/******************************************************************************/
/*************************** Functions Declarations ***************************/
/******************************************************************************/
/* Initializes the communication with the device. */
char ad7171_Init(AD7171_type device);

/* Receives 24 bits from SPI and transmits 0x0. */
unsigned long ad7171_Read24Bits(unsigned long ctrl);

/* Calculates the input voltage and the pattern of the conversion as
parameter.*/
float ad7171_GetVoltage(unsigned long   ctrl,
                        unsigned char*  pattern,
                        float vRef);

/*  Returns the ADC code and the pattern of the conversion as parameter. */
unsigned short ad7171_GetAdcCode(unsigned long ctrl, unsigned char* pattern);

#endif /* _ad7171_H_ */
