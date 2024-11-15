/**************************************************************************//**
*   @file   ad2s1205.c
*   @brief  Implementation of ad2s1205 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "AD2S1205.h"
#include "Communication.h"
#include "TIME.h"

/******************************************************************************/
/*************************** Functions Definitions ****************************/
/******************************************************************************/

/**************************************************************************//**
 * @brief Initializes the parallel communication with the device.
 *
 * @param None.
 *
 * @return 0 = SUCCES.
******************************************************************************/
char AD2S1205_Init(void)
{
    /* Initialize PARALLEL output pins */
    GPIO_SET_PINS_OUT(PAR_BASEADDR, (PAR_SAMPLE | PAR_RDVEL | PAR_CS | PAR_RD));
    GPIO_SET_BITS(PAR_BASEADDR, (PAR_SAMPLE | PAR_RDVEL | PAR_CS | PAR_RD));

    return 0;
}

/**************************************************************************//**
 * @brief Reads the position and velocity information through parallel port.
 *
 * @param *position - pointer to position information;
 *        *velocity - pointer to velocity information.
 *
 * @return None.
******************************************************************************/
void AD2S1205_Pos_Vel(unsigned short* position, short* velocity)
{
    /* Set /SAMPLE signal low */
    GPIO_CLEAR_BITS(PAR_BASEADDR, PAR_SAMPLE);
    /* Wait t2 */
    TIME_DelayNs(T2);
    /* Set /SAMPLE signal high */
    GPIO_SET_BITS(PAR_BASEADDR, PAR_SAMPLE);
    /* Set RD/CS low */
    GPIO_CLEAR_BITS(PAR_BASEADDR, (PAR_RD | PAR_CS));
    /* Wait t6 */
    TIME_DelayNs(T6);
    /* Set RDVEL low */
    GPIO_CLEAR_BITS(PAR_BASEADDR, PAR_RDVEL);
    /* Read the position */
    *position = (GPIO_GET_DATA(PAR_BASEADDR) & PAR_DATA_MASK) >> 6;
    /* Set RD high */
    GPIO_SET_BITS(PAR_BASEADDR, PAR_RD);
    /* Wait t3 */
    TIME_DelayNs(T3);
    /* Set RD low */
    GPIO_CLEAR_BITS(PAR_BASEADDR, PAR_RD);
    /* Wait t6 */
    TIME_DelayNs(T6);
    /* Read the velocity */
    *velocity = (GPIO_GET_DATA(PAR_BASEADDR) & PAR_DATA_MASK) >> 6;
    if(*velocity >= 2048)
    {
        *velocity = *velocity - 4096;
    }
    /* Set RDVEL high */
    GPIO_SET_BITS(PAR_BASEADDR, PAR_RDVEL);
    /* Set RD/CS high */
    GPIO_SET_BITS(PAR_BASEADDR, (PAR_RD | PAR_CS));
}
