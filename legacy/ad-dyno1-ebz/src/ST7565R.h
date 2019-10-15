/***************************************************************************//**
 *   @file   ST7565R.h
 *   @brief  Header file of ST7565R Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
#ifndef __ST7565R_H__
#define __ST7565R_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <ADuC7023.h>

/******************************************************************************/
/******************** Macros and Constants Definitions ************************/
/******************************************************************************/

/* ST7565R definitions */
#define ST7565R_SLAVE_ID            1
#define ST7565R_PAGE_ADDR(x)        (0xB0 | (x & 0x0F))
#define ST7565R_COLUMN_ADDR_H(x)    (0x10 | (x & 0x0F))
#define ST7565R_COLUMN_ADDR_L(x)    (0x00 | (x & 0x0F))

/* GPIOs definitions */
#define ST7565R_A0_PIN_OUT      GP0DAT |= 0x40000000
#define ST7565R_A0_LOW          GP0DAT &= ~0x00400000
#define ST7565R_A0_HIGH         GP0DAT |=  0x00400000
#define ST7565R_RST_PIN_OUT     GP0DAT |= 0x20000000
#define ST7565R_RST_LOW         GP0DAT &= ~0x00200000
#define ST7565R_RST_HIGH        GP0DAT |= 0x00200000

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Initializes the ST7565R controller. */
char ST7565R_Init(void);

/*! Sends a character to ST7565R controller. */
void ST7565R_Char(unsigned char pageAddr,
                  unsigned char columnAddr,
                  char character,
                  unsigned char bigFont);

/*! Sends a string to ST7565R controller. */
void ST7565R_String(unsigned char pageAddr,
                    unsigned char columnAddr,
                    char* string,
                    unsigned char bigFont);

/* Sends an image array to ST7565R controller. */
void ST7565R_Image(unsigned char pageAddr,
                  unsigned char columnAddr,
                  const unsigned char image[],
                  unsigned char width,
                  unsigned char height);
				  
/*! Sends an integer number to ST7565R controller. */
void ST7565R_Number(unsigned char pageAddr,
                   unsigned char columnAddr,
                   long number,
                   unsigned char bigFont);
/*! Clears the screen. */				   
void ClearScreen(void);		

/*! Sends a float number to ST7565R controller. */
void ST7565R_FloatNumber(unsigned char pageAddr,
                        unsigned char columnAddr,
                        float number,
                        unsigned char resolution,
                        unsigned char bigFont);

#endif // __ST7565R_H__
