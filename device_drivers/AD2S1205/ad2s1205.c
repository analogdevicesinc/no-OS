/**************************************************************************//**
*   @file   ad2s1205.c
*   @brief  Implementation of ad2s1205 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************/

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
