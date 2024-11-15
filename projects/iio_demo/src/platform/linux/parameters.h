/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to linux platform used by iio_demo
 *           project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "common_data.h"
#include "no_os_util.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
/* This value can be modified based on the number
of samples needed to be stored in the device buffer
and based on the available RAM memory of the platform */
#define SAMPLES_PER_CHANNEL_PLATFORM 1000000

#define UART_DEVICE_ID  0
#define UART_BAUDRATE   115200
#define UART_IRQ_ID     20
#define UART_EXTRA      NULL
#define UART_OPS        NULL

#endif /* __PARAMETERS_H__ */
