/***************************************************************************//**
 *   @file   max11205.h
 *   @brief  Implementation of max11205.h
 *   @author RBolboac (ramona.bolboaca@analog.com)
 *******************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __MAX11205_H__
#define __MAX11205_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "no_os_gpio.h"
#include "no_os_irq.h"
#include "no_os_spi.h"
#include "no_os_util.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define MAX11205_VREF_MAX_MV 		3600
#define MAX11205_DATA_SIZE_BYTES 	2
#define MAX11205_SCALE 			NO_OS_GENMASK(14,0)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
struct max11205_dev {
	/** SPI device descriptor*/
	struct no_os_spi_desc   	*spi_desc;
	/** GPIO RDY descriptor used to signal when ADC data is available */
	struct no_os_gpio_desc  	*gpio_rdy;
	/** IRQ device descriptor used to handle interrupt routine for GPIO RDY */
	struct no_os_irq_ctrl_desc 	*irq_ctrl;
	/** IRQ callback used to handle interrupt routine for GPIO RDY */
	struct no_os_callback_desc	irq_cb;
	/** Reference voltage in millivolts */
	uint32_t			vref_mv;
	/** ADC raw data, updated at each interrupt */
	int16_t				adc_data_raw;
	/** True if data was updated since last read */
	bool				data_updated;
};

struct max11205_init_param {
	/** SPI device descriptor*/
	struct no_os_spi_init_param     spi_init;
	/** GPIO RDY descriptor used to signal when ADC data is available */
	struct no_os_gpio_init_param	*gpio_rdy;
	/** IRQ device descriptor used to handle interrupt routine for GPIO RDY */
	struct no_os_irq_ctrl_desc 	*irq_ctrl;
	/** Reference voltage in millivolts */
	uint32_t			vref_mv;
};

int max11205_init(struct max11205_dev **device,
		  struct max11205_init_param init_param);
int max11205_get_data_raw(struct max11205_dev *dev, bool *new_data_avail,
			  int16_t *data_raw);
int max11205_get_data_mv(struct max11205_dev *dev, int16_t raw_data,
			 int32_t *data_mv);

int max11205_remove(struct max11205_dev *dev);

#endif /* __MAX11205_H__ */
