/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by the AD4170 examples.
 *   @author Janani Sunil (janani.sunil@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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

#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "ad4170.h"
#include "no_os_uart.h"

/*
 * Device variant driven by this project. Change to ID_AD4170, ID_AD4171,
 * ID_AD4172, ID_AD4190 or ID_AD4195 to match the board being used.
 * ad4170_init() fails with -EFAULT when this does not match the product ID
 * the device reports.
 */
#ifndef AD4170_DEV_ID
#define AD4170_DEV_ID		ID_AD4171_8
#endif

/* Internal reference output, used as the ADC reference by the example. */
#define AD4170_VREF_MV		2500

/* Digital gain adjustment register value corresponding to a gain of 1. */
#define AD4170_GAIN_DEFAULT	0x555555

/* fADC = fCLK / (32 * FILTER_FS), i.e. 125 kSPS off the 16 MHz clock. */
#define AD4170_FILTER_FS	4

/* RDY GPIO monitor timeout loop counter. */
#define AD4170_RDY_CONV_TIMEOUT	10000000

extern struct no_os_uart_init_param ad4170_uart_ip;
extern struct ad4170_init_param ad4170_ip;

#endif /* __COMMON_DATA_H__ */
