/***************************************************************************//**
 *   @file   RDKRL78G14.c
 *   @brief  Implementation of the RDKRL78G14 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "RDKRL78G14.h"	// RDKRL78G14 definitions

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Resets the RDKRL78G14.
 *
 * @return None.
*******************************************************************************/
void RDKRL78G14_Reset(void)
{
    short delay = 0;

    EXTRST_HIGH;
    for(delay = 0; delay < 1000; delay++)
    {
        NOP;
    }
    EXTRST_LOW;
}

/***************************************************************************//**
 * @brief Initializes the RDKRL78G14.
 *
 * @return status - Result of the initialization procedure.
 *                  Example: 0 - if initialization was successful;
 *                          -1 - if initialization was unsuccessful.
*******************************************************************************/
char RDKRL78G14_Init(void)
{
    long delay  = 0;
    char status = -1;

    for(delay = 0; delay < 4000000; delay++)
    {
        NOP;
    }
    RDKRL78G14_Reset();
    status = 0;

    return status;
}
