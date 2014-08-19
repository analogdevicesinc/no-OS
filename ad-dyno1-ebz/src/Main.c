/***************************************************************************//**
 *   @file   Main.c
 *   @brief  Main.c implementation.
 *   @author Mihai Bancisor (mihai.bancisor@analog.com)
********************************************************************************
 * Copyright 2014(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "ST7565R.h"
#include "Time.h"
#include "menu.h" 
#include "Dyno.h" 
#include "math.h"

/******************************************************************************/
/**************************** Global Variables ********************************/
/******************************************************************************/


// DC filter config
#define FILTERSHIFT 7	            // for low pass filters to determine ADC offsets
#define FILTERROUNDING (1<<4)
volatile unsigned long filteredOffset = 2048 << FILTERSHIFT;
volatile unsigned long lastFilteredOffset = 2048 << FILTERSHIFT;

#define FILTERSHIFT1 1 	            // for low pass filters to determine ADC offsets
#define FILTERROUNDING1 (1<<4)
volatile unsigned short filteredOffset1 = 2048 << FILTERSHIFT1;
volatile unsigned short lastFilteredOffset1 = 2048 << FILTERSHIFT1;

// buttons
volatile unsigned int  ct10Ms = 0;
extern volatile unsigned char btMin;
extern volatile unsigned char btEnt;
extern volatile unsigned char btPlu;

volatile unsigned long delay25Us = 0;

volatile unsigned short lcdUpdateCt = 0;

volatile unsigned long long c1SquareSum = 0;
volatile unsigned long long c1SquareSumRMS = 0;
extern volatile unsigned char sampleADC;

volatile unsigned short currentCt = 0;

volatile unsigned char dutyCycle = 0;
volatile unsigned char maxDutyCycle = 80;
volatile unsigned char minDutyCycle = 20;
volatile unsigned char dutyCycleStep = 5;

unsigned char rampTime = 2;
volatile unsigned short rampCt = 0;

volatile int adc[1000];

volatile int adc1 = 0;

volatile unsigned char hallValue = 0;
volatile unsigned char prevHallValue = 0;
volatile unsigned short hallCounter = 0;
volatile unsigned short hallStoppedCounter = 0;
volatile unsigned short hallTime[10];
volatile unsigned char hallTimeCt = 0;

menuType measurementsMenu = 
{
	"Measurements",
	pfnMeasurements
};

menuType waveformsMenu = 
{
	"Waveforms",
};
	menuType stepResponeMenu = 
	{
		"Step Load",
	};
		menuType dutyCycleToggleMenu = 
		{
			"Step",
			pfnStep
		};
		menuType maxMenu = 
		{
			"Set max val",
			pfnSetMax
		};
		menuType minMenu = 
		{
			"Set min val",
			pfnSetMin
		};
	menuType rampMenu = 
	{
		"Ramp",
		pfnRamp
	};

menuType settingsMenu = 
{
	"Settings",
};
	menuType dutyCycleStepMenu = 
	{
		"Duty cycle step",
		pfnDutyCycleStep
	};
	menuType aboutMenu = 
	{
		"About",
		pfnAbout
	};
menuType mainMenu = {""};
menuType* pCurrentMenu;
/******************************************************************************/
/******************************* Functions ************************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Draws the ADI logo and displays the name of ADI component.
 *
 * @param componentName - The name of the ADI component.
 *
 * @return None.
*******************************************************************************/
void DisplayMenu()
{
	unsigned char i;
	
	for(i = 0; i < pCurrentMenu->submenuCount; i++)
	{
		if(pCurrentMenu->activeSubmenu == i)
		{
			ST7565R_String(i, 0, ">>", 0);
			if(i == 0 && pCurrentMenu->parent)
				ST7565R_String(i, 20, "...", 0);
			else
				ST7565R_String(i, 20, pCurrentMenu->submenu[i]->name, 0);
			ST7565R_String(i, 110, "<<", 0);
		}			
		else
		{
			ST7565R_String(i, 0, "  ", 0);
			if(i == 0 && pCurrentMenu->parent)
				ST7565R_String(i, 20, "...", 0);
			else
				ST7565R_String(i, 20, pCurrentMenu->submenu[i]->name, 0);
			ST7565R_String(i, 110, "  ", 0);
		}
	}
}

/***************************************************************************//**
 * @brief Draws the ADI logo and displays the name of ADI component.
 *
 * @param componentName - The name of the ADI component.
 *
 * @return None.
*******************************************************************************/
void ADI_Component(char* string)
{
const unsigned char adiComponent[440] =
{
0x00, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0x3E, 0x7E, 0x7E, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE,
0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xF8, 0x3E, 0x0E, 0x3E, 0xF8, 0xC0, 0x00, 0x00,
0x00, 0x00, 0xFE, 0xFE, 0x38, 0x70, 0xE0, 0xC0, 0x80, 0x00, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00,
0xC0, 0xF8, 0x3E, 0x0E, 0x3E, 0xF8, 0xC0, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0xE0, 0xF8, 0x1C, 0x0E, 0x06, 0x06, 0x06, 0x06, 0x0E, 0x1C, 0xF8, 0xE0, 0x00,
0xC0, 0xF8, 0xFC, 0x0E, 0x06, 0x06, 0x06, 0x86, 0x86, 0x8E, 0x9C, 0x98, 0x00, 0x00, 0x00, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x07, 0x07, 0x0F,
0x0F, 0x1F, 0x3F, 0x3F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x38, 0x3F, 0x0F, 0x06, 0x06, 0x06, 0x06, 0x06, 0x0F, 0x3F, 0x38, 0x00, 0x00,
0x3F, 0x3F, 0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x3F, 0x3F, 0x00, 0x00, 0x38, 0x3F, 0x0F, 0x06,
0x06, 0x06, 0x06, 0x06, 0x0F, 0x3F, 0x38, 0x00, 0x3F, 0x3F, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
0x30, 0x03, 0x0F, 0x1E, 0x38, 0x30, 0x30, 0x30, 0x30, 0x38, 0x1E, 0x0F, 0x03, 0x00, 0x01, 0x0F,
0x1F, 0x38, 0x30, 0x30, 0x30, 0x31, 0x31, 0x19, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xC0, 0xE0, 0xE0, 0xF0,
0xF8, 0xF8, 0xFC, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
0xFE, 0xFE, 0x06, 0x06, 0x06, 0x06, 0x0E, 0x1C, 0x38, 0xF0, 0xC0, 0x00, 0x00, 0xFE, 0xFE, 0x86,
0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x00, 0x00, 0x0E, 0x7E, 0xF0, 0x80, 0x00, 0x00, 0x80, 0xF0,
0x7E, 0x0E, 0x00, 0xFE, 0xFE, 0x00, 0x00, 0xC0, 0xF0, 0x38, 0x1C, 0x0E, 0x06, 0x06, 0x06, 0x0E,
0x1C, 0x38, 0x30, 0x00, 0x00, 0xFE, 0xFE, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x06, 0x00, 0x78,
0xFC, 0xCE, 0x86, 0x86, 0x86, 0x86, 0x8E, 0x1C, 0x18, 0x00, 0x00, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F,
0x7F, 0x78, 0x7C, 0x7C, 0x7E, 0x7E, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F,
0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x7F,
0x60, 0x60, 0x60, 0x60, 0x70, 0x30, 0x3C, 0x1F, 0x07, 0x00, 0x00, 0x7F, 0x7F, 0x61, 0x61, 0x61,
0x61, 0x61, 0x61, 0x61, 0x60, 0x00, 0x00, 0x00, 0x07, 0x3F, 0x78, 0x78, 0x3F, 0x07, 0x00, 0x00,
0x00, 0x7F, 0x7F, 0x00, 0x00, 0x03, 0x0F, 0x1C, 0x38, 0x70, 0x60, 0x60, 0x60, 0x70, 0x38, 0x3C,
0x0C, 0x00, 0x00, 0x7F, 0x7F, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x60, 0x00, 0x18, 0x38, 0x31,
0x61, 0x61, 0x61, 0x61, 0x33, 0x3F, 0x0E, 0x00, 
};

    ST7565R_Image(0, 9, adiComponent, 110, 32);
    //ST7565R_String(0, 9 + 5, string, 0);
}

/***************************************************************************//**
 * @brief Interrupt Service Rountine.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void IRQ_Handler(void) __irq  
{
	unsigned long IRQSTATUS = 0;
  
	IRQSTATUS = IRQSTA;	 	// Read off IRQSTA register
	if(IRQSTA & 0x04) // Timer0 IRQ?
	{
		T0CLRI = 0; //clear Timer0 interrupt
		GP1DAT ^= 0x00200000;
//		T0CON = 0x00; //disable Timer0 interrupt
//		T0CON = 0xC8; //enable Timer0 interrupt
	}
	IRQSTATUS = IRQSTA;	 	// Read off IRQSTA register
	if ((IRQSTATUS & 0x08) == 0x08)	//Timer 1 interrupt source -- every 25us
	{	
		ct10Ms++;	// counter for reading the button states
		rampCt++;
		lcdUpdateCt++;
		if(delay25Us > 0)
			delay25Us--;

		if(pCurrentMenu->pfn == pfnMeasurements) 
		{
			hallValue  = GP2DAT & (1 << 2);
			if(hallValue)	// on rising edge start counting
			{
				hallCounter++;
			}
			else 
				if((!hallValue) && (prevHallValue))	// on falling edge stop counting
				{
					// 10 value circular array
					hallTime[hallTimeCt % 10] = hallCounter;
					hallTimeCt++;
					hallCounter 		= 0;
					hallStoppedCounter 	= 0;
				}
				else
				{
					hallStoppedCounter++;
				}
			prevHallValue = hallValue;
			if(hallCounter > 10000)
				hallCounter = 5001;
			if(hallStoppedCounter > 10000)
				hallStoppedCounter = 5001;
		}		
		if((pCurrentMenu->pfn == pfnRamp) && (rampCt >= rampTime*400))
		{
			rampCt = 0;
			if(dutyCycle < 100)
				dutyCycle++;
			else 
				dutyCycle = 0;
		}
		T1CLRI = 0x55;				// Clear the currently active Timer0 Irq
	}

	if ((IRQSTATUS & (1 << 6)) == (1 << 6))	// ADC interrupt source
	{

		if(currentCt == 3999)	// IF 1000 samples have been captured pass them to RMS calc code
		{
			currentCt = 0;
			c1SquareSumRMS = c1SquareSum;
			c1SquareSum = 0;
		}
		if(sampleADC)
		{
				adc1 = (unsigned short)(ADCDAT >> 16);	// Read ADC result
				// Compute DC offset by LPF of all adc1 values
				
				adc1 <<= 4;
				filteredOffset1 = (lastFilteredOffset1 - ( lastFilteredOffset1 >> FILTERSHIFT1 )) + (adc1 >> FILTERSHIFT1);                           // update the filter
				lastFilteredOffset1 = filteredOffset1;
				
				adc1 = lastFilteredOffset1;

				//			adc1 <<= 4;
				filteredOffset = (lastFilteredOffset - ( lastFilteredOffset >> FILTERSHIFT )) + (adc1 >> FILTERSHIFT);                           // update the filter
				lastFilteredOffset = filteredOffset;
				
				adc1 -= filteredOffset;		// Remove DC bias
				adc1 >>= 4;
				
				c1SquareSum += adc1*adc1;	// Compute square sum
				currentCt++;				// increment the current samples counter	
		}
		else
		{
			adc1 = (unsigned short)(ADCDAT >> 16);	// Read ADC result
			currentCt = 0;
			c1SquareSum = 0;
		}
	}                                                                                                                                                                          
}

/***************************************************************************//**
 * @brief main function.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
int main(void)
{
	int duty = 0;
	
	Board_Init();	// Initialize peripherals
    ST7565R_Init();	// Initialize display
	
    ADI_Component("Analog Devices");
	TIME_DelayMs(5000);
	ClearScreen();

/* Initialize menus*/	
	AddSubmenu(&mainMenu, &measurementsMenu);
	AddSubmenu(&mainMenu, &waveformsMenu);
		AddSubmenu(&waveformsMenu, &stepResponeMenu);
			AddSubmenu(&stepResponeMenu, &dutyCycleToggleMenu);
				AddSubmenu(&stepResponeMenu, &maxMenu);
				AddSubmenu(&stepResponeMenu, &minMenu);
			AddSubmenu(&waveformsMenu, &rampMenu);
	AddSubmenu(&mainMenu, &settingsMenu);
		AddSubmenu(&settingsMenu, &dutyCycleStepMenu);
		AddSubmenu(&settingsMenu, &aboutMenu);
	pCurrentMenu = &mainMenu;	
	
	while(1)
	{

		if(ct10Ms >= 400)
		{	
			buttons();
			ct10Ms = 0;
		}
	
		if((pCurrentMenu == &mainMenu)  || (pCurrentMenu == &waveformsMenu))
		{
			dutyCycle = 0;
		}
	
		duty = (51*dutyCycle)/20;
		//duty = (51*80)/20;
		PWM1COM1 = duty;		
		PWM1COM2 = duty;		
		PWM2COM1 = duty;

		if(btMin)
		{
			StepMenuDown(pCurrentMenu);
			if(pCurrentMenu->pfn)
				pCurrentMenu->pfn(0, 1);
			btMin = 0;
		}

		if(btPlu)
		{
			StepMenuUp(pCurrentMenu);
			if(pCurrentMenu->pfn)
				pCurrentMenu->pfn(1, 0);
			btPlu = 0;
		}

		if(btEnt)
		{
			ClearScreen();
			if(pCurrentMenu->submenuCount)
				pCurrentMenu = pCurrentMenu->submenu[pCurrentMenu->activeSubmenu];
			else
				pCurrentMenu = pCurrentMenu->parent;
			btEnt = 0;
		}
		if(pCurrentMenu->pfn)
			pCurrentMenu->pfn(0, 0);
		DisplayMenu();
// Discovery Control
		if(!(GP2DAT & (1 << 3)))
		{
			ClearScreen();
			//Disable PWM pins
			GP1CON &= ~0x33000000;
			GP2CON &= ~0x00000003;

			ST7565R_String(2, 12, "Analog Discovery", 0);
			while(!(GP2DAT & (1 << 3)))
			{}
			//Enable PWM pins
			GP1CON &= ~0x33000000;
			GP1CON |= 0x22000000;   // P1.7 - PWM3; P1.6 - PWM2.
			GP2CON &= ~0x00000003;
			GP2CON |= 0x00000002;   // P2.0 - PWM4.
			ClearScreen();
		}
	}
}
