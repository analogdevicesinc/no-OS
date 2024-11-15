/***************************************************************************//**
 *   @file   app_config.h
 *   @brief  Config file of ADV7511 no-OS project.
 *   @author DBogdan (dragos.bogdan@analog.com)
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef CONFIG_H_
#define CONFIG_H_

// #define HAVE_VERBOSE_MESSAGES /* Recommended during development prints errors and warnings */
// #define DEBUG		 /* For Debug purposes only */

/******************************************************************************/
/****************************** Carrier Vendor ********************************/
/******************************************************************************/
#define XILINX

/******************************************************************************/
/**************************** Platform used ***********************************/
/******************************************************************************/
//#define PLATFORM_AC701
//#define PLATFORM_KC705
//#define PLATFORM_VC707
//#define PLATFORM_ZC702
//#define PLATFORM_ZC706
//#define PLATFORM_ZED

#if !defined(PLATFORM_AC701) && !defined(PLATFORM_KC705) && \
	!defined(PLATFORM_VC707) && !defined(PLATFORM_ZC702) && \
	!defined(PLATFORM_ZC706) && !defined(PLATFORM_ZED)
#error "ERROR: Must define a platform in app_config.h"
#endif

#endif
