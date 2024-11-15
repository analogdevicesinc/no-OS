/***************************************************************************//**
 *   @file   Communication.h
 *   @brief  Header file of the Communication Driver for RL78G14 processor.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "RDKRL78G14.h"

/******************************************************************************/
/*************************** Macros Definitions *******************************/
/******************************************************************************/
#define SPI_CS_PIN_OUT  PMOD1_CS_OUT
#define SPI_CS_LOW      PMOD1_CS_LOW
#define SPI_CS_HIGH     PMOD1_CS_HIGH
#define SPI_MISO        PMOD1_MISO

#define GPIO1_PIN_OUT   PMOD1_GPIO1_PIN_OUT
#define GPIO1_LOW       PMOD1_GPIO1_LOW
#define GPIO1_HIGH      PMOD1_GPIO1_HIGH

#define GPIO2_PIN_OUT   PMOD1_GPIO2_PIN_OUT
#define GPIO2_LOW       PMOD1_GPIO2_LOW
#define GPIO2_HIGH      PMOD1_GPIO2_HIGH

#define GPIO3_PIN_OUT   PMOD1_GPIO3_PIN_OUT
#define GPIO3_LOW       PMOD1_GPIO3_LOW
#define GPIO3_HIGH      PMOD1_GPIO3_HIGH

#define GPIO4_PIN_OUT   PMOD1_GPIO4_PIN_OUT
#define GPIO4_LOW       PMOD1_GPIO4_LOW
#define GPIO4_HIGH      PMOD1_GPIO4_HIGH

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

/*! Initializes the I2C communication peripheral. */
char I2C_Init(long clockFreq);

/*! Writes data to a slave device. */
char I2C_Write(char slaveAddress,
               unsigned char* dataBuffer,
               char bytesNumber,
               char stopBit);

/*! Reads data from a slave device. */
char I2C_Read(char slaveAddress,
              unsigned char* dataBuffer,
              char bytesNumber,
              char stopBit);

#endif // __COMMUNICATION_H__
