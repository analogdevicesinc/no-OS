/***************************************************************************//**
 *   @file   Communication.h
 *   @brief  Header file of Communication Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2012-2015(c) Analog Devices, Inc.
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
 *
*******************************************************************************/
#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_

/******************************************************************************/
/* Include Files                                                              */
/******************************************************************************/

/******************************************************************************/
/* Functions Prototypes                                                       */
/******************************************************************************/

/*! Initializes the I2C communication peripheral. */
unsigned char I2C_Init(unsigned long clockFreq);

/*! Writes data to a slave device. */
unsigned char I2C_Write(unsigned char slaveAddress,
                        unsigned char* dataBuffer,
                        unsigned char bytesNumber,
                        unsigned char stopBit);

/*! Reads data from a slave device. */
unsigned char I2C_Read(unsigned char slaveAddress,
                       unsigned char* dataBuffer,
                       unsigned char bytesNumber,
                       unsigned char stopBit);

/*! Initializes the SPI communication peripheral. */
unsigned char SPI_Init(unsigned char lsbFirst,
                       unsigned long clockFreq,
                       unsigned char clockPol,
                       unsigned char clockEdg);

/*! Initializes the SPI communication peripheral. */
unsigned char SPI_Init(unsigned char lsbFirst,
                       unsigned long clockFreq,
                       unsigned char clockPol,
                       unsigned char clockEdg);

/*! Reads data from SPI. */
unsigned char SPI_Read(unsigned char slaveDeviceId,
                       unsigned char* data,
                       unsigned char bytesNumber);

/*! Writes data to SPI. */
unsigned char SPI_Write(unsigned char slaveDeviceId,
                        unsigned char* data,
                        unsigned char bytesNumber);

#endif /* _COMMUNICATION_H_ */
