/***************************************************************************//**
 *   @file   app_config.h
 *   @brief  Config file of AD9361/API Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2015(c) Analog Devices, Inc.
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
