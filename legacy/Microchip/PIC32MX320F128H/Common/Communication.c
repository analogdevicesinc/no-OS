/***************************************************************************//**
 *   @file   Communication.c
 *   @brief  Implementation of Communication Driver for PIC32MX320F128H 
             Processor.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Communication.h"    /*!< Communication definitions */

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Initializes the SPI communication peripheral.
 *
 * @param lsbFirst  - Transfer format (0 or 1).
 *                    Example: 0x0 - MSB first.
 *                             0x1 - LSB first.
 * @param clockFreq - SPI clock frequency (Hz).
 *                    Example: 1000 - SPI clock frequency is 1 kHz.
 * @param clockPol  - SPI clock polarity (0 or 1).
 *                    Example: 0x0 - Idle state for clock is a low level; active
 *                                   state is a high level;
 *                             0x1 - Idle state for clock is a high level; active
 *                                   state is a low level.
 * @param clockEdg  - SPI clock edge (0 or 1).
 *                    Example: 0x0 - Serial output data changes on transition
 *                                   from idle clock state to active clock state;
 *                             0x1 - Serial output data changes on transition
 *                                   from active clock state to idle clock state.
 *
 * @return status   - Result of the initialization procedure.
 *                    Example:  0 - if initialization was successful;
 *                             -1 - if initialization was unsuccessful.
*******************************************************************************/
unsigned char SPI_Init(unsigned char lsbFirst,
                       unsigned long clockFreq,
                       unsigned char clockPol,
                       unsigned char clockEdg)
{
    unsigned char   status      = 0;
    unsigned long   pbFrequency = 80000000;
    unsigned short  brgValue    = 0;

    SPI_CS_HIGH;
    SPI_CS_PIN_OUT;
    /*!< Fsck = Fpb / (2 * (SPIxBRG + 1)) */
    brgValue = pbFrequency / (2 * clockFreq) - 1;
    SPI2CON = 0;                /*!< Clear the content of SPI2CON register */
    SPI2BRG = brgValue;
    SPI2CONbits.CKE = clockEdg; /*!< SPI Clock Edge Select bit */
    SPI2CONbits.CKP = clockPol; /*!< Clock Polarity Select bit */
    SPI2CONbits.MSTEN = 1;      /*!< Enable master mode */
    SPI2CONbits.ON = 1;         /*!< Enable SPI peripheral */

    return status;
}

/***************************************************************************//**
 * @brief Writes data to SPI.
 *
 * @param slaveDeviceId - The ID of the selected slave device.
 * @param data          - Data represents the write buffer.
 * @param bytesNumber   - Number of bytes to write.
 *
 * @return Number of written bytes.
*******************************************************************************/
unsigned char SPI_Write(unsigned char slaveDeviceId,
                        unsigned char* data,
                        unsigned char bytesNumber)
{
    unsigned char byte     = 0;
    unsigned char tempByte = 0;

    if(slaveDeviceId == 1)
    {
        SPI_CS_LOW;
    }
    for(byte = 0; byte < bytesNumber; byte++)
    {
        SPI2BUF = data[byte];
        while(SPI2STATbits.SPIRBF == 0);
        tempByte = SPI2BUF;
    }
    while(SPI2STATbits.SPIBUSY == 1);
    if(slaveDeviceId == 1)
    {
        SPI_CS_HIGH;
    }

    return bytesNumber;
}

/***************************************************************************//**
 * @brief Reads data from SPI.
 *
 * @param slaveDeviceId - The ID of the selected slave device.
 * @param data          - Data represents the read buffer.
 * @param bytesNumber   - Number of bytes to read.
 *
 * @return Number of read bytes.
*******************************************************************************/
unsigned char SPI_Read(unsigned char slaveDeviceId,
                       unsigned char* data,
                       unsigned char bytesNumber)
{
    unsigned char   byte            = 0;
    unsigned char   writeBuffer[4]  = {0, 0, 0, 0};

    for(byte = 0; byte < bytesNumber; byte++)
    {
        writeBuffer[byte] = data[byte];
    }
    if(slaveDeviceId == 1)
    {
        SPI_CS_LOW;
    }
    for(byte = 0; byte < bytesNumber; byte++)
    {
        SPI2BUF = writeBuffer[byte];
        while(SPI2STATbits.SPIRBF == 0);
        data[byte] = SPI2BUF;
    }
    while(SPI2STATbits.SPIBUSY == 1);
    if(slaveDeviceId == 1)
    {
        SPI_CS_HIGH;
    }

    return bytesNumber;
}

/***************************************************************************//**
 * @brief Initializes the I2C communication peripheral.
 *
 * @param clockFreq - I2C clock frequency (Hz).
 *                    Example: 100000 - I2C clock frequency is 100 kHz.
 * @return status   - Result of the initialization procedure.
 *                    Example:  0 - if initialization was successful;
 *                             -1 - if initialization was unsuccessful.
*******************************************************************************/
unsigned char I2C_Init(unsigned long clockFreq)
{
    unsigned char   status      = 0;
    unsigned long   pbFrequency = 80000000;
    unsigned short  brgValue    = 0;

    I2C1CON = 0;            /*!< Clear the content of I2C1CON register */
    /*!< Fsck = Fpb / ((I2CxBRG + 2) * 2) */
    brgValue = pbFrequency / (2 * clockFreq) - 2;
    I2C1BRG = brgValue;
    I2C1CONbits.ON = 1;     /*!< Enable the I2C module and configure the SDA and */
                            /*!< SCL pins as serial port pins */
    I2C1CONbits.ACKEN = 1;
    return status;
}

/***************************************************************************//**
 * @brief Writes data to a slave device.
 *
 * @param slaveAddress - Adress of the slave device.
 * @param dataBuffer   - Pointer to a buffer storing the transmission data.
 * @param bytesNumber  - Number of bytes to write.
 * @param stopBit      - Stop condition control.
 *                       Example: 0 - A stop condition will not be sent;
 *                                1 - A stop condition will be sent.
 *
 * @return status      - Number of read bytes or 0xFF if the slave address was not
 *                       acknowledged by the device.
*******************************************************************************/
unsigned char I2C_Write(unsigned char slaveAddress,
                        unsigned char* dataBuffer,
                        unsigned char bytesNumber,
                        unsigned char stopBit)
{
    unsigned char status = 0;
    unsigned char acknowledge = 0;
    unsigned char byte = 0;

    I2C1CONbits.SEN = 1;            /*!< Initiate a start condition */
    while(I2C1CONbits.SEN);         /*!< Wait for start condition to complete */
    I2C1TRN = slaveAddress << 1;
    while(I2C1STATbits.TRSTAT);     /*!< Wait for transmition to complete */
    acknowledge = I2C1STATbits.ACKSTAT;
    if(acknowledge == 0)            /*!< Acknowledge was received */
    {
        for(byte = 0; byte < bytesNumber; byte++)
        {
            I2C1TRN = dataBuffer[byte];
            while(I2C1STATbits.TRSTAT); /*!< Wait for transmition to complete */
        }
    }
    else
    {
        status = 0xFF;
    }
    if(stopBit)
    {
        I2C1CONbits.PEN = 1;            /*!< Initiate a stop condition */
        while(I2C1CONbits.PEN);         /*!< Wait for stop condition to copmlete */
    }

    return status;
}

/***************************************************************************//**
 * @brief Reads data from a slave device.
 *
 * @param slaveAddress - Adress of the slave device.
 * @param dataBuffer   - Pointer to a buffer that will store the received data.
 * @param bytesNumber  - Number of bytes to read.
 * @param stopBit      - Stop condition control.
 *                       Example: 0 - A stop condition will not be sent;
 *                                1 - A stop condition will be sent.
 *
 * @return status      - Number of read bytes or 0xFF if the slave address was not
 *                       acknowledged by the device.
*******************************************************************************/
unsigned char I2C_Read(unsigned char slaveAddress,
                       unsigned char* dataBuffer,
                       unsigned char bytesNumber,
                       unsigned char stopBit)
{
    unsigned char status = bytesNumber;
    unsigned char acknowledge = 0;
    unsigned char byte = 0;

    I2C1CONbits.SEN = 1;            /*!< Initiate a start condition */
    while(I2C1CONbits.SEN);         /*!< Wait for start condition to complete */
    I2C1TRN = (slaveAddress << 1) + 1;
    while(I2C1STATbits.TRSTAT);     /*!< Wait for transmition to complete */
    acknowledge = I2C1STATbits.ACKSTAT;
    if(acknowledge == 0)            /*!< Acknowledge was received */
    {
        for(byte = 0; byte < bytesNumber; byte++)
        {
            I2C1CONbits.RCEN = 1;       /*!< Enable receive mode for I2C */
            if(byte == (bytesNumber - 1))
            {
                I2C1CONbits.ACKDT = 1;
            }
            else
            {
                I2C1CONbits.ACKDT = 0;
            }
            while(I2C1CONbits.RCEN);    /*!< Wait for reception to complete */
            I2C1CONbits.ACKEN = 1;
            while(I2C1CONbits.ACKEN);
            dataBuffer[byte] = I2C1RCV;
        }
    }
    else
    {
        status = 0xFF;
    }
    I2C1CONbits.PEN = 1;            /*!< Initiate a stop condition */
    while(I2C1CONbits.PEN);         /*!< Wait for stop condition to copmlete */

    return status;
}

/***************************************************************************//**
 * @brief Initializes the UART communication peripheral.
 *
 * @param baudRate - Baud rate value.
 *                   Example: 9600 - 9600 bps.
 *
 * @return status  - Result of the initialization procedure.
 *                   Example: -1 - if initialization was unsuccessful;
 *                             0 - if initialization was successful.
*******************************************************************************/
char UART_Init(unsigned long baudRate)
{
    unsigned long   pbFrequency = 80000000;
    unsigned short  brgValue    = 0;

    /*!< BaudRate = Fpb / (16 * (UxBRG + 1)) */
    brgValue = pbFrequency / (16 * baudRate) - 1;
    U1MODE   = 0;             /*!< Clear the content of U1MODE register */
    U1BRG    = brgValue;
    U1MODEbits.PDSEL1 = 0;
    U1MODEbits.PDSEL0 = 0;    /*!< 8-bit data, no parity */
    U1MODEbits.STSEL  = 0;    /*!< 1 Stop bit */
    U1STAbits.URXEN   = 1;    /*!< Enable UART1 receiver */
    U1STAbits.UTXEN   = 1;    /*!< Enable UART1 transmitter */
    U1MODEbits.ON     = 1;    /*!< Enable UART1 peripheral */

    return 0;
}

/***************************************************************************//**
 * @brief Writes one character to UART.
 *
 * @param data - Character to write.
 *
 * @return None.
*******************************************************************************/
void UART_WriteChar(char data)
{
    while(U1STAbits.TRMT == 0);
    U1TXREG = data;
}

/***************************************************************************//**
 * @brief Reads one character from UART.
 *
 * @param data - Read character.
 *
 * @return None.
*******************************************************************************/
void UART_ReadChar(char* data)
{
    while(U1STAbits.URXDA == 0);
    *data = U1RXREG;
}

/***************************************************************************//**
 * @brief Writes one string of characters to UART.
 *
 * @param string - String of characters to write.
 *
 * @return None.
*******************************************************************************/
void UART_WriteString(const char* string)
{
    while(*string)
    {
        UART_WriteChar(*string++);
    }
}
