/***************************************************************************//**
 *   @file   Communication.c
 *   @brief  Implementation of the Communication Driver for ADuC7023 processor.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2014(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <ADuC7026.h>

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
char SPI_Init(char lsbFirst,
              long clockFreq,
              char clockPol,
              char clockEdg)
{
    long uClkFreq = 41780000;
    
    /* Configure the SPI pins. */
    //GP0CON &= ~0x33000000;
    //GP0CON |= 0x11000000;   // P0.7 - MOSI; P0.6 - MISO.     

    GP0CON &= ~0x30000000;
    GP0CON |= 0x10000000;   // P0.7 - MOSI;
    
    GP1CON &= ~0x00000033;
    GP1CON |= 0x00000011;   // P1.1 - CS; P1.0 - SCLK.
    
    SPIDIV = (uClkFreq / (2 * clockFreq)) - 1;
    
    SPICON = 0x0043 |                   // Initiate transfer with a write to the SPITX register;
             ((clockPol & 0x1) << 3) |  // Set the polarity and the phase;
             (!(clockEdg & 0x1) << 2);  // Enable master mode; Enable the SPI.
    
    return 0;
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
char SPI_Write(char slaveDeviceId,
               unsigned char* data,
               char bytesNumber)
{
    unsigned char byte = 0;
    
    for(byte = 0; byte < bytesNumber; byte++)
    {
        SPITX = data[byte];                 // Transmit data.
        while((SPISTA & 0x0020) != 0x0020); // Wait for SPITX to be empty.
    }
    
    return bytesNumber;
}

/***************************************************************************//**
 * @brief Reads data from SPI.
 *
 * @param slaveDeviceId - The ID of the selected slave device.
 * @param data          - Data represents the write buffer as an input parameter 
 *                        and the read buffer as an output parameter.
 * @param bytesNumber   - Number of bytes to read.
 *
 * @return Number of read bytes.
*******************************************************************************/
char SPI_Read(char slaveDeviceId,
              unsigned char* data,
              char bytesNumber)
{
    unsigned char byte = 0;
    
    for(byte = 0; byte < bytesNumber; byte++)
    {
        SPITX = data[byte];             // Transmit data.
        while((SPISTA & 0x02) != 0x02); // Wait for SPITX to be empty.
        data[byte] = SPIRX;
    }
    
    return bytesNumber;
}
