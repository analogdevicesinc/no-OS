/***************************************************************************//**
*   @file   AD5425.h
*   @brief  Header file of AD5425 Driver for Microblaze processor.
*   @author Istvan Csomortani (istvan.csomortani@analog.com)
*
********************************************************************************
* Copyright 2011(c) Analog Devices, Inc.
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
********************************************************************************
*   SVN Revision: $WCREV$
*******************************************************************************/
#ifndef _AD5425_H_
#define _AD5425_H_

/******************************************************************************/
/*********************************** Include **********************************/
/******************************************************************************/
#include "Communication.h"

/* SPI slave device ID */
#define AD5425_SLAVE_ID    		1

/* AD5415 GPIO */
#define AD5425_LDAC_OUT         GPIO1_PIN_OUT
#define AD5425_LDAC_LOW         GPIO1_LOW
#define AD5425_LDAC_HIGH        GPIO1_HIGH

/******************************************************************************/
/************************** Functions Declarations ****************************/
/******************************************************************************/

int AD5425_Init(void);
void AD5425_SetRegister(unsigned char data);

#endif
