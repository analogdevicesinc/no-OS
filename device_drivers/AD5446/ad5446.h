/***************************************************************************//**
*   @file   AD5446.h
*   @brief  Header file of AD5446 Driver. This driver supporting the following
*              devices: AD5553, AD5543, AD5542A, AD5541A, AD5512A, AD5453,
*                       AD5452, AD5451, AD5450, AD5446, AD5444
*
*   @author Istvan Csomortani (istvan.csomortani@analog.com)
********************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification,
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL,SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
*
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Communication.h"

/* Supported output types */
typedef enum
{
    unipolar,       /* 0 .. Vref */
    unipolar_inv,   /* 0 .. -Vref */
    bipolar         /* -Vref .. Vref*/
} vout_type_t;

/* Supported devices */
typedef enum {
    ID_AD5553,
    ID_AD5543,
    ID_AD5542A,
    ID_AD5541A,
    ID_AD5512A,
    ID_AD5453,
    ID_AD5452,
    ID_AD5451,
    ID_AD5450,
    ID_AD5446,
    ID_AD5444,
} AD5446_type_t;

/* Control Bits */
#define AD5446_CTRL_LOAD_UPDATE     0x0
#define AD5446_CTRL_ACTIVE_POSEDGE  0x3

/* AD5446 GPIO */
#define AD5446_LDAC_OUT             GPIO1_PIN_OUT
#define AD5446_LDAC_LOW             GPIO1_LOW
#define AD5446_LDAC_HIGH            GPIO1_HIGH

#define AD5446_CLR_OUT              GPIO0_PIN_OUT
#define AD5446_CLR_LOW              GPIO0_LOW
#define AD5446_CLR_HIGH             GPIO0_HIGH

/* SPI slave device ID
 * These are board specific defines
 * It should be defined in function of the current system configuration and architecture
 * */
#if(defined(AD5446) || defined(AD5444))
#define AD5446_SLAVE_ID             2
#else
#define AD5446_SLAVE_ID             1
#endif

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Initialize SPI and Initial Values for AD5446 Board. */
char AD5446_Init(AD5446_type_t device);

/* Write to shift register via SPI. */
void AD5446_SetRegister(unsigned char command,
                         unsigned short data);

/* Sets the output voltage. */
float AD5446_SetVoltage(float voltage, float vref, vout_type_t vout_type);
