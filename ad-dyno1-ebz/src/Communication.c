/***************************************************************************//**
 *   @file   Communication.c
 *   @brief  Implementation of the Communication Driver for ADuC7023 processor.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2014(c) Analog Devices, Inc.
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
*******************************************************************************/

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
