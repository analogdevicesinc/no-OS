/***************************************************************************//**
 *   @file   Dyno.c
 *   @brief  Implementation of Dyno Driver.
 *   @author Mihai Bancisor (mihai.bancisor@analog.com)
********************************************************************************
 * Copyright 2014(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "ST7565R.h"        // ST7565R definitions
#include "Communication.h"  // Communication definitions
#include "Time.h"           // Time definitions
/******************************************************************************/
/************************* Constants Definitions ******************************/
/******************************************************************************/

/******************************************************************************/
/**************************** Global Variables ********************************/
/******************************************************************************/

volatile unsigned char btMin = 0;
volatile unsigned char btEnt = 0;
volatile unsigned char btPlu = 0;
volatile unsigned char prevBtEnt = 0;
volatile unsigned char debounceCt = 0;

/******************************************************************************/
/******************************* Functions ************************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief ADC initialization rountine.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void ADC_Init(void)
{
    ADCCON = 0x20;			//Power up ADC
	TIME_DelayMs(2);
    REFCON = 0x01;			// Internal 2.5V reference.
	ADCCON = (1 << 0) | 	// Enable Timer1 as a conversion input => 40KSPS.
			(1 << 5) |		// Power up ADC
			(1 << 7) |		// Start conversion
			(1 << 9) |		// acq. time = 8 clocks
			(1 << 10) ;		// fADC/2.
	ADCCP = 0x00;
}

/***************************************************************************//**
 * @brief Board initialization
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void Board_Init(void)
{
	/* PLLCON Write Sequence */
	POWKEY1 = 0xAA;
	PLLCON  = 0x01;    // Internal 32 kHz oscillator; Clocking mode: PLL.
	POWKEY2 = 0x55;

	/* POWCON0 Write Sequence */
	POWKEY1 = 0x01;
	POWCON0  = 0x00;    // Configure UCLK CPU Clock for 41.78MHz, CD=0.
	POWKEY2 = 0xF4;
				
	// Initilize Timer 1
	T1LD	= 0x414; 			// 40kHz --> 25uS,
	T1CON   = 0x4C0;    // Periodic, 41.78MHz UCLK, div/1

	// Initialize interrupts
	IRQEN 	= (1 << 3) 	// Enable Timer1 interrupt sources
		|(1 << 2) 				// Enable Timer0 interrupt sources
		| (1 << 6);				// Enable ADC interrupt sources

	GP1PAR |= ((1 << 30));
	GP2PAR |= (1 << 2);
	
/* POWCON1 Write Sequence */
	POWKEY3 = 0x76;
	POWCON1  = 0x900;    // Power up the SPI @ 41.78 MHz.
	POWKEY4 = 0xB1;
	
/* Configure the PWM pins. */
	GP1CON &= ~0x33000000;
	GP1CON |= 0x22000000;   // P1.7 - PWM3; P1.6 - PWM2.

	GP2CON &= ~0x00000003;
	GP2CON |= 0x00000002;   // P2.0 - PWM4.
	
	PWMCON1 = (1 << 6) | (1 << 7) 	// UCLK/16
			| (1 << 0); 								// Enable all PWM outputs.
    
	PWM1COM1 = 0x7F;	// PWM2 output pin goes low when the PWM timer reaches the count value stored in this register.
	PWM1COM2 = 0x7F;	// PWM3 output pin goes low when the PWM timer reaches the count value stored in this register.
	PWM1COM0 = 0xFF;	// PWM2 output pin goes high when the PWM timer reaches the count value stored in this register.    
	PWM1LEN = 0xFF;		// PWM3 output pin goes high when the PWM timer reaches the value stored in this register.

	
	PWM2COM1 = 0x7F;	// PWM2 output pin goes low when the PWM timer reaches the count value stored in this register.
	PWM2LEN = 0xFF;		// PWM3 output pin goes high when the PWM timer reaches the value stored in this register.WM3 output pin goes high when the PWM timer reaches the value stored in this register.

	ADC_Init();			// Initialize ADC

	//GP2DAT |= 0x04000000;   // P2.2 output HALL C
 	GP1DAT |= 0x20000000;   // P1.5 output Hall B
}

/***************************************************************************//**
 * @brief Reads button presses.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void buttons()
{
		if((debounceCt == 0) && (GP2DAT & (1 << 4)))
			prevBtEnt = 0;
		if(!(GP1DAT & (1 << 3)) && debounceCt == 0)
		{
			debounceCt = 25;
			btMin = 1;
		}
		if(!(GP2DAT & (1 << 4)) && (debounceCt == 0) && !prevBtEnt)
		{
			prevBtEnt = 1;
			debounceCt = 25;
			btEnt = 1;
		}
		if(!(GP1DAT & (1 << 2)) && debounceCt == 0)
		{
			debounceCt = 25;
			btPlu = 1;
		}		
		if(debounceCt != 0)
			debounceCt--;	
}
