/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ad74413r examples.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_includes.h"
#include "adt75.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
extern struct no_os_uart_init_param adt75_uart_ip;
extern struct adt75_init_param adt75_ip;

#endif /* __COMMON_DATA_H__ */
