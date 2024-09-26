/**************************************************************************//**
*   @file   ad5172.h
*   @brief  Header file of AD5172 Driver for Microblaze processor. This driver
*           supporting the following devices : AD5162, AD5172, AD5173, AD8403,
*           AD8402, AD8400
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
******************************************************************************/
#ifndef __AD5172_H__
#define __AD5172_H__

#include "Communication.h"

/* Custom boolean type */
typedef enum
{
    false,
    true
} bool_t;

/* Communication types */
typedef enum
{
    SPI,
    I2C
} comm_type_t;

/* Supported devices */
typedef enum {
    ID_AD5162,
    ID_AD5172,
    ID_AD5173,
    ID_AD8403,
    ID_AD8402,
    ID_AD8400,
} AD5172_type_t;

/*****************************************************************************/
/*  Device specific MACROs                                                   */
/*****************************************************************************/
/* GPIOs */
#define AD5172_RESET_OUT            GPIO1_PIN_OUT
#define AD5172_RESET_LOW            GPIO1_LOW
#define AD5172_RESET_HIGH           GPIO1_HIGH

#define AD5172_SHUTDOWN_OUT         GPIO3_PIN_OUT
#define AD5172_SHUTDOWN_LOW         GPIO3_LOW
#define AD5172_SHUTDOWN_HIGH        GPIO3_HIGH

/* I2C addresses */
#define AD5172_I2C_HARD_ADDR        0xB
/* Package pin-programmable address bits */
#define AD5172_I2C_PIN_ADDR1        0
#define AD5172_I2C_PIN_ADDR0        0
#define AD5172_I2C_PIN_ADDR_MASK    0x03

/* AD5172/3 use special commands */
#define AD5172_CNTR_MASK            0x68        // Mask of the control bits
#define AD5172_CNTR_A0_MASK         0x80        // Mask of the address bit
#define AD5172_CNTR_SD              (1 << 6)    // Shutdown bit
#define AD5172_OTP_T                (1 << 5)    // OTP programming bit
#define AD5172_OTP_OW               (1 << 3)    // Overwrites the fuse settings
#define AD5172_CNTR_A0_OFFSET       0x07        // Offset of the address bits
#define AD5172_OTP_VALID_MASK       0x03        // Mask of the OTP validation bits
#define AD5172_OTP_VALID_OFFSET     0x06        // Offset of the OTP validation bits
/* OTP validation bits */
#define AD5172_FUSE_READY           0x00
#define AD5172_FUSE_ERROR           0x01
#define AD5172_FUSE_SUCCESS         0x02

#define AD5172_SLAVE_ID             1

/*****************************************************************************/
/*  Functions Prototypes                                                     */
/*****************************************************************************/
/* Initialize the communication with the device */
char AD5172_Init(AD5172_type_t device);

/* Send a command to the device */
void AD5172_Write(unsigned char channel,
                  unsigned char command,
                  unsigned char data);

/* Read back RDAC value from the device */
short AD5172_Read(unsigned char channel, unsigned char* fuseStatus);

#endif // __AD5172_H__
