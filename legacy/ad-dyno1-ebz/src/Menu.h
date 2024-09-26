/***************************************************************************//**
 *   @file   Menu.h
 *   @brief  Header file of Menu.c
 *   @author Mihai Bancisor (mihai.bancisor@analog.com)
********************************************************************************
 * Copyright 2014(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef _MENU_H_
#define _MENU_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/


/******************************************************************************/
/******************** Macros and Constants Definitions ************************/
/******************************************************************************/

#define MAX_LENGTH 20
#define MAX_SUBMENUS 4
#define MAIN_MENU_SIZE 3

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Menu structure. */
typedef struct _menuType
{
	char name[MAX_LENGTH];
	void (*pfn)(char, char);
	unsigned char submenuCount;
	unsigned char activeSubmenu;	
	struct _menuType *parent;
	struct _menuType *submenu[MAX_SUBMENUS];
} menuType;

void AddSubmenu(menuType* pMainMenu, menuType* pSubMenu);
void StepMenuUp(menuType* pMenu);
void StepMenuDown(menuType* pMenu);
menuType* GetActiveMenu(menuType* pMenu);

void pfnMeasurements(char up, char down);

void pfnSpeed(char up, char down);

void pfnSetMax(char up, char down);

void pfnSetMin(char up, char down);

void pfnDutyCycleStep(char up, char down);

void pfnStep(char up, char down);

void pfnRamp(char up, char down);

void pfnAbout(char up, char down);

#endif //_MENU_H_











