/***************************************************************************//**
 *   @file   Menu.h
 *   @brief  Header file of Menu.c
 *   @author Mihai Bancisor (mihai.bancisor@analog.com)
********************************************************************************
 * Copyright 2014(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
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











