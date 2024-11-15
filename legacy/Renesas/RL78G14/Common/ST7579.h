/***************************************************************************//**
 *   @file   ST7579.h
 *   @brief  Header file of ST7579 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __ST7579_H__
#define __ST7579_H__

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Transmits 8 bits to ST7579 controller. */
void ST7579_WriteByte(unsigned char data);

/*! Sets X and Y adress of RAM. */
void ST7579_GoTo(unsigned char yPosition,
                 unsigned char xPosition);

/*! Clears ST7579 RAM. */
void ST7579_Clear(void);

/*! Initializes the ST7579 controller. */
char ST7579_Init(void);

/*! Sends a character to ST7579 controller. */
void ST7579_Char(unsigned char yPosition,
                 unsigned char xPosition,
                 unsigned char character,
                 unsigned char bigFont);

/*! Sends a string to ST7579 controller. */
void ST7579_String(unsigned char yPosition,
                   unsigned char xPosition,
                   unsigned char* string,
                   unsigned char bigFont);

/*! Sends an integer number to ST7579 controller. */
void ST7579_Number(unsigned char yPosition,
                   unsigned char xPosition,
                   long number,
                   unsigned char bigFont);

/*! Sends a float number to ST7579 controller. */
void ST7579_FloatNumber(unsigned char yPosition,
                        unsigned char xPosition,
                        float number,
                        unsigned char resolution,
                        unsigned char bigFont);

/*! Sends an integer number in Hexa format to ST7579 controller. */
void ST7579_HexNumber(unsigned char yPosition,
                      unsigned char xPosition,
                      long number,
                      unsigned char bigFont);

/*! Sends an image array to ST7579 controller. */
void ST7579_Image(unsigned char yPosition,
                  unsigned char xPosition,
                  const unsigned char image[],
                  unsigned char width,
                  unsigned char height);

#endif // __ST7579_H__
