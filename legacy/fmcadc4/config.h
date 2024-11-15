/***************************************************************************//**
 *   @file   config.h
 *   @brief  Config file of AD9361/API Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2015(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef CONFIG_H_
#define CONFIG_H_

// #define HAVE_VERBOSE_MESSAGES /* Recommended during development prints errors and warnings */
// #define DEBUG		 /* For Debug purposes only */

/******************************************************************************/
/****************************** Carrier Vendors *******************************/
/******************************************************************************/
// #define XILINX
// #define ALTERA

/******************************************************************************/
/********************************* CPU Arch ***********************************/
/******************************************************************************/
/*Xilinx*/
// #define ZYNQ /* Requires a family definition (below)*/
	// #define ZYNQ_PS7
	//#define ZYNQ_PSU
// #define MICROBLAZE

/*Altera*/
// #define SOC
// #define NIOS_II

/******************************************************************************/
/********************************* Warn User **********************************/
/******************************************************************************/

#if !defined(XILINX) && !defined(ALTERA)
	printf("\nWARNING no vendor defined in config.h (for carrier)\n");
#endif

#if !defined(MICROBLAZE) && !defined(NIOS_II) && !defined(SOC) && !defined(ZYNQ)
	printf("\nWARNING CPU arch defined in config.h\n");
#endif
#endif
