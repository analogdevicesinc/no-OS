/***************************************************************************//**
 *   @file   Communication.h
 *   @brief  Header file of the Communication Driver for ADuC7023 processor.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2014(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Initializes the SPI communication peripheral. */
char SPI_Init(char lsbFirst,
              long clockFreq,
              char clockPol,
              char clockEdg);

/*! Writes data to SPI. */
char SPI_Write(char slaveDeviceId,
               unsigned char* data,
               char bytesNumber);

/*! Reads data from SPI. */
char SPI_Read(char slaveDeviceId,
              unsigned char* data,
              char bytesNumber);

#endif // __COMMUNICATION_H__
