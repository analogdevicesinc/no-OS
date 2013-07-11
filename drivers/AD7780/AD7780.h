/***************************************************************************//**
*   @file   AD7780.h
*   @brief  AD7780 header file.
*   @author DNechita (dan.nechita@analog.com)
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
#ifndef __AD7780_H__
#define __AD7780_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Communication.h"

/******************************************************************************/
/************************** AD7780 Definitions ********************************/
/******************************************************************************/

/* DOUT/RDY pin */
#define AD7780_RDY_STATE        SPI_MISO

/* PDRST pin */
#define AD7780_PDRST_PIN_OUT    SPI_CS_PIN_OUT
#define AD7780_PDRST_HIGH       SPI_CS_HIGH
#define AD7780_PDRST_LOW        SPI_CS_LOW

/* FILTER pin */
#define AD7780_FILTER_PIN_OUT   GPIO1_PIN_OUT
#define AD7780_FILTER_HIGH      GPIO1_HIGH
#define AD7780_FILTER_LOW       GPIO1_LOW

/* GAIN pin */
#define AD7780_GAIN_PIN_OUT     GPIO2_PIN_OUT
#define AD7780_GAIN_HIGH        GPIO2_HIGH
#define AD7780_GAIN_LOW         GPIO2_LOW

/* Status bits */
#define AD7780_STAT_RDY         (1 << 7) // Ready bit.
#define AD7780_STAT_FILTER      (1 << 6) // Filter bit.
#define AD7780_STAT_ERR         (1 << 5) // Error bit.
#define AD7780_STAT_ID1         (1 << 4) // ID bits.
#define AD7780_STAT_ID0         (1 << 3) // ID bits.
#define AD7780_STAT_GAIN        (1 << 2) // Gain bit.
#define AD7780_STAT_PAT1        (1 << 1) // Status pattern bits.
#define AD7780_STAT_PAT0        (1 << 0) // Status pattern bits.

#define AD7780_ID_NUMBER        0x08

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Initializes the communication peripheral and checks if the device is
present. */
char AD7780_Init(void);

/*! Waits for DOUT/RDY pin to go low. */
char AD7780_WaitRdyGoLow(void);

/*! Reads a 24-bit sample from the ADC. */
long AD7780_ReadSample(unsigned char* pStatus);

/*! Converts the 24-bit raw value to volts. */
float AD7780_ConvertToVoltage(unsigned long rawSample, float vRef, unsigned char gain);

#endif /* __AD7780_H__ */
