/***************************************************************************//**
*   @file   ad9833.h
*   @brief  Header file of AD9833 Driver for Microblaze processor.
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
#ifndef _AD9833_H_
#define _AD9833_H_

/******************************************************************************/
/******************************* Include Files ********************************/
/******************************************************************************/
#include "Communication.h"

/******************************************************************************/
/********************* Macros and Constants Definitions ***********************/
/******************************************************************************/
/* Supported devices */
typedef enum {
    ID_AD9833,
    ID_AD9834,
    ID_AD9837,
    ID_AD9838,
} AD9833_type;

#define AD9833_CTRLB28          (1 << 13)
#define AD9833_CTRLHLB          (1 << 12)
#define AD9833_CTRLFSEL         (1 << 11)
#define AD9833_CTRLPSEL         (1 << 10)
#define AD9834_CTRLPINSW        (1 << 9)
#define AD9833_CTRLRESET        (1 << 8)
#define AD9833_CTRLSLEEP1       (1 << 7)
#define AD9833_CTRLSLEEP12      (1 << 6)
#define AD9833_CTRLOPBITEN      (1 << 5)
#define AD9834_CTRLSIGNPIB      (1 << 4)
#define AD9833_CTRLDIV2         (1 << 3)
#define AD9833_CTRLMODE         (1 << 1)

/* GPIOs */
#define AD9834_PSEL_OUT         GPIO0_PIN_OUT
#define AD9834_PSEL_LOW         GPIO0_LOW
#define AD9834_PSEL_HIGH        GPIO0_HIGH

#define AD9834_FSEL_OUT         GPIO1_PIN_OUT
#define AD9834_FSEL_LOW         GPIO1_LOW
#define AD9834_FSEL_HIGH        GPIO1_HIGH

#define AD9834_RESET_OUT        GPIO2_PIN_OUT
#define AD9834_RESET_LOW        GPIO2_LOW
#define AD9834_RESET_HIGH       GPIO2_HIGH

#define AD9834_SLEEP_OUT        GPIO3_PIN_OUT
#define AD9834_SLEEP_LOW        GPIO3_LOW
#define AD9834_SLEEP_HIGH       GPIO3_HIGH


#define BIT_F0ADDRESS           0x4000      // Frequency Register 0 address.
#define BIT_F1ADDRESS           0x8000      // Frequency Register 1 address.
#define BIT_P0ADDRESS           0xC000      // Phase Register 0 address.
#define BIT_P1ADDRESS           0xE000      // Phase Register 1 address.

#define AD9833_SLAVE_ID         1

/******************************************************************************/
/************************** Functions Declarations ****************************/
/******************************************************************************/
/* Initialize the SPI communication with the device. */
char AD9833_Init(AD9833_type device);
/* Transmits 16 bits on SPI. */
void AD9833_TxSpi(short value);
/* Selects the type of output. */
char AD9833_OutMode(unsigned char vOutMode);
/* Loads a frequency value in a register. */
void AD9833_SetFreq(unsigned char registerNumber, unsigned long frequencyValue);
/* Loads a phase value in a register. */
void AD9833_SetPhase(unsigned char registerNumber, float phaseValue);
/* Select the frequency register to be used. */
void AD9833_SelectFreqReg(unsigned char freqReg);
/* Select the phase register to be used. */
void AD9833_SelectPhaseReg(unsigned char phaseReg);
/* Enable / Disable the sleep function. */
void AD9833_SleepMode(unsigned char sleepMode);

void AD9834_SelectProgMethod(unsigned char value);

void AD9834_LogicOutput(unsigned char opbiten,
                        unsigned char signpib,
                        unsigned char div2);

#endif  /* _AD9833_H_ */
