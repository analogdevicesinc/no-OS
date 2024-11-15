/**************************************************************************//**
*   @file   AD5570.c
*   @brief  Implementation of AD5570 Driver for Microblaze processor.
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
#include "AD5570.h"
#include "Communication.h"
#include "Console.h"

/**************************************************************************//**
 * @brief Initialize SPI and Initial Values for AD5570 Board.
 *
 * @param None.
 *
 * @return retValue - Result of the initialization.
 *                    Example: 0 - if initialization was successful;
 *                            -1 - if initialization was unsuccessful.
******************************************************************************/
int AD5570_Init(void)
{
    char status = -1;

    /* Initialize the SPI interface */
    status = SPI_Init(0, 1000000, 0, 0);

    /* Initialize the LDAC GPIO pin */
    AD5570_PD_OUT;
    AD5570_PD_HIGH;
    AD5570_CLR_OUT;
    AD5570_CLR_HIGH;
    AD5570_LDAC_OUT;
    AD5570_LDAC_LOW;

    return status;
}

/***************************************************************************//*
 * @brief Set the shift register of the DAC with the given value
 *
 * @param The value which will be written to the shift register
 *
 * @return None
******************************************************************************/
void AD5570_SetRegister(unsigned short data)
{
    unsigned char buff[2] = { 0,0 };

    buff[1] = data & 0xFF;
    buff[0] = (data & 0xFF00) >> 8;
    SPI_Write(AD5570_SLAVE_ID, buff, 2);
}
