/***************************************************************************//**
 *   @file   adrv9009/src/app/app_config.h
 *   @brief  Config file for ADRV9009 project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

/* Uncomment if using adrv2crr-fmc + adrv9009-zu11eg: */
// #define ZU11EG

/* Uncomment if using zcu102 + fmcomms8: */
// #define FMCOMMS8_ZCU102

/* Provided ZU11EG or FMCOMMS8_ZCU102 were not defined, this project builds
 * by default for zc706 + adrv9009-w/pcbz.
 */

/* To build ADRV9008-1/ADRV9008-2, uncomment one (only one) of the lines below: */
//#define ADRV9008_1
//#define ADRV9008_2

/* To build a specific example, uncomment one (only one) of the lines below: */
// #define DMA_EXAMPLE
// #define IIO_SUPPORT

#endif /* APP_CONFIG_H_ */
