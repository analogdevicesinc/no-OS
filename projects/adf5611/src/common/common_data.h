/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by adf5611 examples.
 *   @author Josemene (jude.osemene@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include "adf5611.h"
#include "no_os_spi.h"
#include "no_os_uart.h"
#include "no_os_util.h"
#include "no_os_i2c.h"
#include "no_os_eeprom.h"
#include "24xx32a.h"

#define HW_MEZZANINE_NAME		        "EVAL-ADF5611-SD1Z"
#define HW_CARRIER				"SDP_K1"
#define ACTIVE_DEVICE_NAME              	"DEV_ADF5611"
#define ACTIVE_DEVICE_ID                	"ID_ADF5611"
#define ACTIVE_DEVICE_VENDOR		    	"Analog Devices"
#define NUM_CTX_ATTR                    	4

extern struct no_os_uart_init_param	adf5611_uart_ip;
extern struct no_os_spi_init_param	adf5611_spi_ip;
extern struct adf5611_init_param 	adf5611_ip;
extern struct no_os_i2c_init_param	adf5611_i2c_ip;
extern struct no_os_eeprom_init_param	adf5611_eeprom_ip;
extern struct eeprom_24xx32a_init_param adf5611_eeprom_init_param;

#endif /* __COMMON_DATA_H__ */
