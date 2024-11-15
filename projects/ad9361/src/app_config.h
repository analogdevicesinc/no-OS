/***************************************************************************//**
 *   @file   app_config.h
 *   @brief  Config file of AD9361/API Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2015(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef CONFIG_H_
#define CONFIG_H_

#define HAVE_SPLIT_GAIN_TABLE	1 /* only set to 0 in case split_gain_table_mode_enable = 0*/
#define HAVE_TDD_SYNTH_TABLE	1 /* only set to 0 in case split_gain_table_mode_enable = 0*/

#define AD9361_DEVICE			1 /* set it 1 if AD9361 device is used, 0 otherwise */
#define AD9364_DEVICE			0 /* set it 1 if AD9364 device is used, 0 otherwise */
#define AD9363A_DEVICE			0 /* set it 1 if AD9363A device is used, 0 otherwise */

//#define XILINX_PLATFORM
//#define ALTERA_PLATFORM
//#define LINUX_PLATFORM
//#define FMCOMMS5
//#define ADI_RF_SOM
//#define ADI_RF_SOM_CMOS
//#define DMA_IRQ_ENABLE
//#define DMA_EXAMPLE
//#define AXI_ADC_NOT_PRESENT
//#define TDD_SWITCH_STATE_EXAMPLE

//#define IIO_SUPPORT

#ifndef IIO_SUPPORT
#define HAVE_VERBOSE_MESSAGES /* Recommended during development prints errors and warnings */
//#define HAVE_DEBUG_MESSAGES /* For Debug purposes only */
#endif // USE_LIBIIO
/*
 * In case memory footprint is a concern these options allow
 * to disable unused functionality which may free up a few kb
 */
#endif
