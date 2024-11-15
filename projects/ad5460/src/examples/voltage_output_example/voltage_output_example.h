/***************************************************************************//**
 *   @file   voltage_output_example.h
 *   @brief  Voltage output example header for eval-ad5460 project
 *   @author Akila Marimuthu (akila.marimuthu@analog.com)
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __VOLTAGE_OUTPUT_EXAMPLE_H__
#define __VOLTAGE_OUTPUT_EXAMPLE_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "ad5460.h"
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_print_log.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int voltage_output_example_main();

#endif /* __VOLTAGE_OUTPUT_EXAMPLE_H__ */
