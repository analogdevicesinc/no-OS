/***************************************************************************//**
 *   @file   ST7565R.h
 *   @brief  Header file of ST7565R Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
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
