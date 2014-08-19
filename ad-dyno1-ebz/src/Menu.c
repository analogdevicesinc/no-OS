/***************************************************************************//**
 *   @file   Menu.c
 *   @brief  Menu implementation.
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
#include "menu.h"
#include "ST7565R.h"
#include "math.h"
#include "dyno.h" 

/******************************************************************************/
/**************************** Global Variables ********************************/
/******************************************************************************/
extern volatile unsigned int  ct10Ms;
extern volatile unsigned char btMin;
extern volatile unsigned char btEnt;
extern volatile unsigned char btPlu;
extern volatile unsigned char prevBtEnt;
extern volatile unsigned char debounceCt;

extern volatile unsigned long delay25Us;

extern volatile unsigned short lcdUpdateCt;

extern volatile unsigned char dutyCycle;
extern volatile unsigned char maxDutyCycle;
extern volatile unsigned char minDutyCycle;
extern volatile unsigned char dutyCycleStep;

extern unsigned char rampTime;

extern volatile unsigned long long c1SquareSumRMS;
volatile float c1RMS = 0;
volatile unsigned char sampleADC = 1;

extern volatile int adc[];
int ct = 0;

extern volatile int adc1;

extern volatile short filteredOffset;

extern unsigned short hallTime[];
extern volatile unsigned short hallCounter;
extern volatile unsigned short hallStoppedCounter;
unsigned long rpm = 0;
unsigned short hallAverageCt = 0;

/******************************************************************************/
/******************************* Functions ************************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Adds a submenu.
 *
 * @param pMainMenu  - The menu where the submenu needs to be added.
 *
 * @param pSubMenu - The submenu.
 *
 * @return None 
 *
*******************************************************************************/
void AddSubmenu(menuType* pMainMenu, menuType* pSubMenu)
{
	if(pMainMenu->parent && pMainMenu->submenuCount == 0)
	{
		pMainMenu->submenu[pMainMenu->submenuCount] = pMainMenu->parent;
		pMainMenu->submenuCount++;
	}
	
	pMainMenu->submenu[pMainMenu->submenuCount] = pSubMenu;
	pMainMenu->submenuCount++;
	
	pSubMenu->parent = pMainMenu;
}

/***************************************************************************//**
 * @brief Scroll menu up.
 *
 * @param pMenu  - Current menu.
 *
 * @return None 
 *
*******************************************************************************/
void StepMenuUp(menuType* pMenu)
{
	if(pMenu->activeSubmenu < pMenu->submenuCount - 1)
		pMenu->activeSubmenu++;
}

/***************************************************************************//**
 * @brief Scroll menu down.
 *
 * @param pMenu  - Current menu.
 *
 * @return None 
 *
*******************************************************************************/
void StepMenuDown(menuType* pMenu)
{
	if(pMenu->activeSubmenu > 0)
		pMenu->activeSubmenu--;
}

menuType* GetActiveMenu(menuType* pMenu)
{
	return pMenu->submenu[pMenu->activeSubmenu];
}

/***************************************************************************//**
 * @brief Measurement function.
 *
 * @param up  - Up button state.
 *
 * @param down  - Down button state.
 *
 * @return None 
 *
*******************************************************************************/
void pfnMeasurements(char up, char down)
{		
	if(lcdUpdateCt > 20000)
	{
		sampleADC = 0;
		c1RMS = (sqrt(c1SquareSumRMS/4000))/151.55F;
		lcdUpdateCt = 0;
		ST7565R_String(0, 0, "Current:      A", 0);
		ST7565R_FloatNumber(0, 50, c1RMS,2,0);
		ST7565R_String(1, 0, "Speed:        RPM", 0);
		rpm = 0;
		// compute speed only if > 60rpm
		if((hallCounter < 5000) && (hallStoppedCounter < 5000))
		{
			for(hallAverageCt = 0; hallAverageCt < 10; hallAverageCt++)
			{
				rpm +=hallTime[hallAverageCt];
			}	
			rpm = 3000000 / rpm;
		}
		ST7565R_Number(1, 50, rpm, 0);
		ST7565R_String(3, 0, "Duty Cycle", 0);
		ST7565R_String(3, 105, "%", 0);
		sampleADC = 1;
	}
	if(up)
	{
		ST7565R_String(3, 80, "   ", 0);
		if(dutyCycle + dutyCycleStep <= 100)
			dutyCycle+=dutyCycleStep;
		else 
			dutyCycle = 100;
		ST7565R_Number(3, 80, dutyCycle, 0);
	}	
	else if(down)
	{
		ST7565R_String(3, 80, "   ", 0);
		if(dutyCycle - dutyCycleStep >= 0)
			dutyCycle -= dutyCycleStep;
		else 
			dutyCycle = 0;
		ST7565R_Number(3, 80, dutyCycle, 0);
	}
		else
			ST7565R_Number(3, 80, dutyCycle, 0);
}

/***************************************************************************//**
 * @brief Step load function.
 *
 * @param up  - Up button state.
 *
 * @param down  - Down button state.
 *
 * @return None 
 *
*******************************************************************************/
void pfnStep(char up, char down)
{	
	static unsigned char stateFlag = 0;
	if(stateFlag)
	{
		dutyCycle = maxDutyCycle;
	}	
	else 
	{
		dutyCycle = minDutyCycle;
	}
	
	ST7565R_String(1, 20, "Duty Cycle:", 0);
	ST7565R_String(1, 114, "%", 0);

	if(up || down)
	{
		stateFlag = !stateFlag;
		if(stateFlag)
		{
			ST7565R_String(1, 90, "     ", 0);
		}	
		else 
		{
			ST7565R_String(1, 90, "     ", 0);
		}
	}
	else
	{
		ST7565R_Number(1, 90, dutyCycle, 0);
	}
	if(stateFlag)
	{
		dutyCycle = maxDutyCycle;
		ST7565R_Number(1, 90, dutyCycle, 0);
	}	
	else 
	{
		dutyCycle = minDutyCycle;
		ST7565R_Number(1, 90, dutyCycle, 0);		
	}
}

/***************************************************************************//**
 * @brief Set maximum duty cycle function.
 *
 * @param up  - Up button state.
 *
 * @param down  - Down button state.
 *
 * @return None 
 *
*******************************************************************************/
void pfnSetMax(char up, char down)
{
	ST7565R_String(0, 0, "Set upper duty cycle", 0);
	ST7565R_String(2, 80, "%", 0);
	if(up && (maxDutyCycle < 100))
	{
		ST7565R_String(2, 60, "   ", 0);
		if(maxDutyCycle + dutyCycleStep <= 100)
			maxDutyCycle+=dutyCycleStep;
		else 
			maxDutyCycle = 100;
		ST7565R_Number(2, 60, maxDutyCycle, 0);
	}	
	else if(down && (maxDutyCycle > 0))
	{
		ST7565R_String(2, 60, "   ", 0);
		if((maxDutyCycle - dutyCycleStep >= 0) && (maxDutyCycle - dutyCycleStep >= minDutyCycle))
			maxDutyCycle-=dutyCycleStep;
		
		ST7565R_Number(2, 60, maxDutyCycle, 0);
	}
		else
		ST7565R_Number(2, 60, maxDutyCycle, 0);
	
}

/***************************************************************************//**
 * @brief Set minimum duty cycle function.
 *
 * @param up  - Up button state.
 *
 * @param down  - Down button state.
 *
 * @return None 
 *
*******************************************************************************/
void pfnSetMin(char up, char down)
{
	ST7565R_String(0, 0, "Set lower duty cycle", 0);
	ST7565R_String(2, 80, "%", 0);
	if(up)
	{
		ST7565R_String(2, 60, "   ", 0);
		if((minDutyCycle + dutyCycleStep <= 100) && (minDutyCycle + dutyCycleStep <= maxDutyCycle))
			minDutyCycle+=dutyCycleStep;
		ST7565R_Number(2, 60, minDutyCycle, 0);
	}	
	else if(down)
	{
		ST7565R_String(2, 60, "   ", 0);
		if(minDutyCycle - dutyCycleStep >= 0)
			minDutyCycle -= dutyCycleStep;
		else 
			minDutyCycle = 0;
		ST7565R_Number(2, 60, minDutyCycle, 0);
	}
		else
		ST7565R_Number(2, 60, minDutyCycle, 0);
}

/***************************************************************************//**
 * @brief Ramp load function.
 *
 * @param up  - Up button state.
 *
 * @param down  - Down button state.
 *
 * @return None 
 *
*******************************************************************************/
void pfnRamp(char up, char down)
{

	ST7565R_String(1, 0, "Ramp period:", 0);
	ST7565R_String(1, 94, "sec", 0);
	if(up)
	{
		ST7565R_String(1, 80, "   ", 0);
		if(rampTime + 1 <= 10)
			rampTime += 1;
		else 
			rampTime = 10;
		ST7565R_Number(1, 80, rampTime, 0);
	}	
	else if(down)
	{
		ST7565R_String(1, 80, "   ", 0);
		if(rampTime - 1 >= 1)
			rampTime -= 1;
		else 
			rampTime = 1;
		ST7565R_Number(1, 80, rampTime, 0);
	}
		else
			ST7565R_Number(1, 80, rampTime, 0);
}

/***************************************************************************//**
 * @brief Set duty cycle step function.
 *
 * @param up  - Up button state.
 *
 * @param down  - Down button state.
 *
 * @return None 
 *
*******************************************************************************/
void pfnDutyCycleStep(char up, char down)
{
	ST7565R_String(0, 0, "Set duty cycle step", 0);
	if(up && (dutyCycleStep < 25))
	{
		ST7565R_String(2, 60, "   ", 0);
		ST7565R_Number(2, 60, ++dutyCycleStep, 0);
	}	
	else if(down && (dutyCycleStep > 1))
	{
		ST7565R_String(2, 60, "   ", 0);
		ST7565R_Number(2, 60, --dutyCycleStep, 0);
	}
		else
		ST7565R_Number(2, 60, dutyCycleStep, 0);
}

/***************************************************************************//**
 * @brief About function.
 *
 * @param up  - Up button state.
 *
 * @param down  - Down button state.
 *
 * @return None 
 *
*******************************************************************************/
void pfnAbout(char up, char down)
{
	ST7565R_String(1, 10, "AD-DYNO1-EBZ REV A", 0);
	ST7565R_String(2, 30, "SW REV 0.1", 0);
}
