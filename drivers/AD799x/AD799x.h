/***************************************************************************//**
 *   @file   AD799x.h
 *   @brief  Header file of AD799x Driver.
 *   @author DNechita (Dan.Nechita@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
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
********************************************************************************
 *   SVN Revision: $WCREV$
*******************************************************************************/

#ifndef __AD799X_H__
#define __AD799X_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Communication.h"

/******************************************************************************/
/******************************** AD799x **************************************/
/******************************************************************************/

/* Supported devices */
#define AD7991          0
#define AD7995          1
#define AD7999          2

#define AD799X_0_ADDRESS          0x28 // I2C address of AD799x-0 .
#define AD799X_1_ADDRESS          0x29 // I2C address of AD799x-1 .

/* Configuration Register definition. */
#define AD799X_CHANNEL(ch)        ((1 << ch) << 4)
#define AD799X_REF_SEL	          (1 << 3)
#define AD799X_FLTR		  (1 << 2)
#define AD799X_BIT_TRIAL_DELAY	  (1 << 1)
#define AD799X_SAMPLE_DELAY	  (1 << 0)

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Initializes I2C. */
char AD799x_Init(char partNumber, char deviceVersion);

/*! Writes data into the Configuration Register. */
void AD799x_SetConfigurationReg(unsigned char registerValue);

/*! Reads the High byte and the Low byte of the conversion. */
void AD799x_GetConversionResult(short* convValue, char* channel);

/*! Converts a raw sample to volts.*/
float AD799x_ConvertToVolts(short rawSample, float vRef);

#endif /* __AD799X_H__ */
