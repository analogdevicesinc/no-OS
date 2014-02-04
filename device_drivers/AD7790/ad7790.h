/***************************************************************************//**
*   @file   ad7790.c
*   @brief  Header file of AD7790 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2014(c) Analog Devices, Inc.
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
#ifndef _ad7790_H_
#define _ad7790_H_

/******************************************************************************/
/******************************* Include Files ********************************/
/******************************************************************************/
#include "xil_types.h"
#include "xparameters.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD7790_SLAVE_ID     1

/******************************************************************************/
/******************************** AD7790 **************************************/
/******************************************************************************/

/* AD7790 Registers */
#define AD7790_REG_COMM         0 // Communications Register (WO, 8-bit)
#define AD7790_REG_STAT         0 // Status Register         (RO, 8-bit)
#define AD7790_REG_MODE         1 // Mode Register           (RW, 8-bit)
#define AD7790_REG_FILTER       2 // Mode Register           (RW, 8-bit)
#define AD7790_REG_DATA         3 // Data Register           (RO, 16-bit)

/* AD7790_REG_COMM Bits */
#define AD7790_COMM_WEN         (1 << 7)            // Write Enable - active low
#define AD7790_COMM_ADDR(x)     (((x) & 0x3) << 4)  // Register Address
#define AD7790_COMM_WRITE       (0 << 3)            // Write Operation
#define AD7790_COMM_READ        (1 << 3)            // Read Operation
#define AD7790_COMM_CREAD       (1 << 2)            // Continuous Read of Data Register
#define AD7790_COMM_CH(x)       ((x) & 0x3)         // Channel Selection

/* AD7790_REG_STAT Bits */
#define AD7790_STAT_RDY         (1 << 7)    // Ready bit for ADC
#define AD7790_STAT_ERR         (1 << 6)    // ADC Error Bit
#define AD7790_STAT_WL          (0 << 2)    // 0 if the device is AD7790
#define AD7790_STAT_CH1         (1 << 1)    // Channel 1 bit
#define AD7790_STAT_CH0         (1 << 0)    // Channel 2 bit

/* AD7790_REG_MODE */
#define AD7790_MODE_SEL(x)      (((x) & 0x3) << 6) // Mode Select
#define AD7790_RANGE_SEL(x)     (((x) & 0x3) << 4) // Range Select
#define AD7790_MODE_BO          (1 << 3)           // Burnout Current Enable Bit
#define AD7790_MODE_BUF         (1 << 1)           // Buffered Mode Enable

/* AD7790_MODE_SEL(x) options */
#define AD7790_MODE_CONT        0 // Continuous Conversion Mode
#define AD7790_MODE_SINGLE      2 // Single Conversion Mode
#define AD7790_MODE_PWRDN       3 // Power-Down Mode

/* AD7790_RANGE_SEL(x) options */
#define AD7790_RANGE_DIV_1      0 // -Vref   to +Vref   input range
#define AD7790_RANGE_DIV_2      1 // -Vref/2 to +Vref/2 input range
#define AD7790_RANGE_DIV_4      2 // -Vref/4 to +Vref/4 input range
#define AD7790_RANGE_DIV_8      3 // -Vref/8 to +Vref/8 input range

/* AD7790_REG_FILTER */
#define AD7790_FILTER_CDIV(x)   (((x) & 0x3) << 4) // Divide clock.
#define AD7790_FILTER_FS(x)     ((x) & 0x7)        // Output update rate.

/* AD7790_FILTER_CDIV options */
#define AD7790_FILTER_CDIV_NORMAL   0   // Normal Mode
#define AD7790_FILTER_CDIV_DIV2     1   // Clock Divided by 2
#define AD7790_FILTER_CDIV_DIV4     2   // Clock Divided by 4
#define AD7790_FILTER_CDIV_DIV8     3   // Clock Divided by 8

/******************************************************************************/
/************************** Functions Declarations ****************************/
/******************************************************************************/

/* Initializes SPI and initial values for AD7790 device. */
char AD7790_Init(void);

/* Reads data from AD7790. */
unsigned long AD7790_Read(unsigned char regAddress,
                          unsigned char bytesNumber);

/* Writes data to AD7790. */
void AD7790_Write(unsigned char regAddress,
                  unsigned char regData);

/* Converts 24-bit raw data to millivolts. */
float AD7790_ConvertToVoltage(unsigned long rawData,
                              float         vRef,
                              unsigned char polarity);

/* Resets the serial interface with the AD7790. */
void AD7790_Reset(void);

#endif /* ad7790_H_ */
