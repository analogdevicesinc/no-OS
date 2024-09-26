/***************************************************************************//**
 *   @file   Communication.h
 *   @brief  Header file of Communication Driver for PIC32MX320F128H 
             Processor.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
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
 *
*******************************************************************************/
#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <xc.h>

/******************************************************************************/
/**************************** GPIO Definitions ********************************/
/******************************************************************************/
#define SPI_CS_PIN_OUT     TRISGbits.TRISG9 = 0;
#define SPI_CS_LOW         LATGbits.LATG9 = 0;
#define SPI_CS_HIGH        LATGbits.LATG9 = 1;

#define SPI_MISO           PORTGbits.RG7

#define GPIO1_PIN_OUT      TRISDbits.TRISD8 = 0;
#define GPIO1_PIN_IN       TRISDbits.TRISD8 = 1;
#define GPIO1_LOW          LATDbits.LATD8 = 0;
#define GPIO1_HIGH         LATDbits.LATD8 = 1;
#define GPIO1_STATE        PORTDbits.RD8

#define GPIO2_PIN_OUT      TRISBbits.TRISB5 = 0;
#define GPIO2_PIN_IN       TRISBbits.TRISB5 = 1;
#define GPIO2_LOW          LATBbits.LATB5 = 0;
#define GPIO2_HIGH         LATBbits.LATB5 = 1;
#define GPIO2_STATE        PORTBbits.RB5

#define GPIO3_PIN_OUT      TRISBbits.TRISB4 = 0;
#define GPIO3_PIN_IN       TRISBbits.TRISB4 = 1;
#define GPIO3_LOW          LATBbits.LATB4 = 0;
#define GPIO3_HIGH         LATBbits.LATB4 = 1;
#define GPIO3_STATE        PORTBbits.RB4

#define GPIO4_PIN_OUT      TRISBbits.TRISB3 = 0;
#define GPIO4_PIN_IN       TRISBbits.TRISB3 = 1;
#define GPIO4_LOW          LATBbits.LATB3 = 0;
#define GPIO4_HIGH         LATBbits.LATB3 = 1;
#define GPIO4_STATE        PORTBbits.RB3

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/*!< Initializes the SPI communication peripheral. */
unsigned char SPI_Init(unsigned char lsbFirst,
                       unsigned long clockFreq,
                       unsigned char clockPol,
                       unsigned char clockEdg);

/*!< Writes data to SPI. */
unsigned char SPI_Write(unsigned char  slaveDeviceId,
                        unsigned char* data,
                        unsigned char  bytesNumber);

/*!< Reads data from SPI. */
unsigned char SPI_Read(unsigned char  slaveDeviceId,
                       unsigned char* data,
                       unsigned char  bytesNumber);

/*!< Initializes the I2C communication peripheral. */
unsigned char I2C_Init(unsigned long clockFreq);

/*!< Writes data to a slave device. */
unsigned char I2C_Write(unsigned char  slaveAddress,
                        unsigned char* dataBuffer,
                        unsigned char  bytesNumber,
                        unsigned char  stopBit);

/*!< Reads data from a slave device. */
unsigned char I2C_Read(unsigned char  slaveAddress,
                       unsigned char* dataBuffer,
                       unsigned char  bytesNumber,
                       unsigned char  stopBit);

/*!< Initializes the UART communication peripheral. */
char UART_Init(unsigned long baudRate);

/*!< Writes one character to UART. */
void UART_WriteChar(char data);

/*!< Reads one character from UART. */
void UART_ReadChar(char* data);

#endif /*__COMMUNICATION_H__*/
