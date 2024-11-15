/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of stm32 platform data used by iio_demo project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "parameters.h"

#if defined NO_OS_USB_UART
struct stm32_usb_uart_init_param iio_demo_usb_uart_extra_ip = {
	.husbdevice = IIO_APP_HUSBUART,
};
#else
struct stm32_uart_init_param iio_demo_uart_extra_ip = {
	.huart = IIO_APP_HUART,
};
#endif

#ifdef IIO_TIMER_TRIGGER_EXAMPLE
extern TIM_HandleTypeDef htim13;
struct stm32_timer_init_param adc_demo_xtip = {
	.htimer = &htim13,
};

extern TIM_HandleTypeDef htim14;
struct stm32_timer_init_param dac_demo_xtip = {
	.htimer = &htim14,
};
#endif
