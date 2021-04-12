/***************************************************************************//**
*   @file   ad5751.h
*   @brief  Header file of ad5751 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
********************************************************************************
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

#ifndef _ad5751_H_
#define _ad5751_H_
/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Communication.h"
#include "xil_types.h"
#include "xparameters.h"

/******************************************************************************/
/********************* Macros and Constants Definitions ***********************/
/******************************************************************************/
#define ad5751_SLAVE_ID         1

/******************************************************************************/
/* GPIOs                                                                      */
/******************************************************************************/
#define ad5751_CLR_OUT          GPIO0_PIN_OUT
#define ad5751_CLR_LOW          GPIO0_LOW
#define ad5751_CLR_HIGH         GPIO0_HIGH
#define ad5751_CLR_PIN          (1 << 0) // ad5751 Clear Pin.

#define ad5751_FAULT_IN         GPIO6_PIN_IN
#define ad5751_FAULT_PIN        (1 << 6) // ad5751 Fault Pin.
/******************************************************************************/
/* Shift Registers                                                            */
/******************************************************************************/
/* Input Shift Register Contents for a Write Operation — Control Register */
#define ad5751_CONTROL_ADDR(x)  (((x) & 0x7) << 13) /* Part address. */
#define ad5751_CONTROL_WRITE    (0 << 12)           /* Write bit. */
#define ad5751_CONTROL_RANGE(x) (((x) & 0xF) << 7)  /* Output range. */
#define ad5751_CONTROL_CLRSEL   (1 << 6)            /* Zero scale or midscale. */
#define ad5751_CONTROL_OUTEN    (1 << 5)            /* Output enable bit. */
#define ad5751_CONTROL_CLEAR    (1 << 4)            /* Software clear bit. */
#define ad5751_CONTROL_RSET     (1 << 3)            /* Select int current resistor.*/
#define ad5751_CONTROL_RESET    (1 << 2)            /* Resets the part to its power-on state. */

/* Input Shift Register Contents for a Read Operation — Status Register */
#define ad5751_STATUS_ADDR(x)    (((x) & 0x7) << 13)/* Part address. */
#define ad5751_STATUS_READ       (1 << 12)          /* Read bit. */
#define ad5751_STATUS_RANGE(x)   (((x) & 0xF) << 7) /* Output range. */
#define ad5751_STATUS_CLRSEL     (1 << 6)           /* Zero scale or midscale. */
#define ad5751_STATUS_OUTEN      (1 << 5)           /* Output enable bit. */
#define ad5751_STATUS_RSET       (1 << 4)           /* Select int current resistor.*/
#define ad5751_STATUS_PEC_ERR    (1 << 3)           /* Interface error. */
#define ad5751_STATUS_OVER_TEMP  (1 << 2)           /* Temperature error. */
#define ad5751_STATUS_IOUT_FAULT (1 << 1)           /* Open circuit on the IOUT pin. */
#define ad5751_STATUS_VOUT_FAULT (1 << 0)           /* Short circuit on the VOUT pin. */

/******************************************************************************/
/*************************** Functions Declarations ***************************/
/******************************************************************************/
/* Initializes the communication with the device. */
char ad5751_Init(void);

/* Writes 16 bits to device through SPI. */
char ad5751_Write16Bits(unsigned short data);

/* Reads 16 bits from the device through SPI. */
unsigned short ad5751_Read16Bits(unsigned short data);

#endif /* _ad5751_H_ */
