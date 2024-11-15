/***************************************************************************//**
 *   @file   iio_timer_trigger_example.h
 *   @brief  IIO timer trigger example header for ad7091r8-sdz project
 *   @author Marcelo Schmitt (marcelo.schmitt@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __AD7091R8_IIO_TIMER_TRIGGER_EXAMPLE_H__
#define __AD7091R8_IIO_TIMER_TRIGGER_EXAMPLE_H__

#ifdef AD7091R2_DEV
#define ADC_CHANNELS 2
#elif defined AD7091R4_DEV
#define ADC_CHANNELS 4
#else
#define ADC_CHANNELS 8
#endif

#ifndef IIO_DATA_BUFFER_SIZE
#define IIO_DATA_BUFFER_SIZE (400 * ADC_CHANNELS * sizeof(int32_t))
#endif

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/
//extern uint16_t *ad7091r8_data_buffer;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int iio_timer_trigger_example_main();

#endif /* __AD7091R8_IIO_TIMER_TRIGGER_EXAMPLE_H__ */
