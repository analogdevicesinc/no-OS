/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by adf5611 examples.
 *   @author Josemene (jude.osemene@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
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
