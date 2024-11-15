/**************************************************************************//**
*   @file   AD5425.c
*   @brief  Implementation of AD5425 Driver for Microblaze processor.
*   @author Istvan Csomortani (istvan.csomortani@analog.com)
*
*******************************************************************************
* Copyright 2011(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*****************************************************************************/
/* Include Files                                                             */
/*****************************************************************************/
#include "AD5425.h"
#include "Communication.h"


/**************************************************************************//**
 * @brief Initialize SPI and Initial Values for AD5425 Board.
 *
 * @param None.
 *
 * @return retValue - Result of the initialization.
 *                    Example: 0 - if initialization was successful;
 *                            -1 - if initialization was unsuccessful.
******************************************************************************/
int AD5425_Init(void)
{
    char status = -1;

    /* Initialize the SPI interface */
    status = SPI_Init(0, NULL, 0, 0);

    /* Initialize the LDAC GPIO pin */
    AD5425_LDAC_OUT;
    AD5425_LDAC_LOW;

    return status;
}

/***************************************************************************//*
 * @brief Set the shift register of the DAC with the given value
 *
 * @param The value which will be written to the shift register
 *
 * @return None
******************************************************************************/
void AD5425_SetRegister(unsigned char data)
{
    unsigned char buff = data;

    SPI_Write(AD5425_SLAVE_ID, &buff, 1);
}
