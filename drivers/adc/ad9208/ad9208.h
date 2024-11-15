/***************************************************************************//**
 *   @file   ad9208.h
 *   @brief  Header file of AD9208 Driver.
 *   @author Stefan Popa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __AD9208_H__
#define __AD9208_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "ad9208_api.h"
#include "ad9208_reg.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_delay.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD9208_FULL_BANDWIDTH_MODE 0
#define AD9208_1_DDC_MODE 1
#define AD9208_2_DDC_MODE 2
#define AD9208_4_DDC_MODE 4

#define AD9208_SYSREF_NONE 0	/* No SYSREF Support */
#define AD9208_SYSREF_ONESHOT 1	/* ONE-SHOT SYSREF */
#define AD9208_SYSREF_CONT 2	/* Continuous Sysref Synchronisation */
#define AD9208_SYSREF_MON 3	/* SYSREF monitor Mode */

#define AD9208_NCO_MODE_VIF 0	/* Variable IF Mode */
#define AD9208_NCO_MODE_ZIF 1	/* Zero IF Mode */
#define AD9208_NCO_MODE_TEST 3	/* Test Mode*/

#define AD9208_BUFF_CURR_400_UA  0x4	/* Buffer Current set to 400 uA */
#define AD9208_BUFF_CURR_500_UA  0x9	/* Buffer Current set to 500 uA */
#define AD9208_BUFF_CURR_600_UA  0x1E	/* Buffer Current set to 600 uA */
#define AD9208_BUFF_CURR_700_UA  0x23	/* Buffer Current set to 700 uA */
#define AD9208_BUFF_CURR_800_UA  0x28	/* Buffer Current set to 800 uA */
#define AD9208_BUFF_CURR_1000_UA  0x32	/* Buffer Current set to 1000 uA */

#define AD9208_CHIP_TYPE	0x03
#define AD9208_CHIP_ID		0xDF

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
struct ad9208_ddc {
	uint32_t decimation;
	uint32_t nco_mode;
	uint64_t carrier_freq_hz;
	uint64_t po;
	bool gain_db;
};

typedef struct ad9208_dev {
	/* SPI */
	struct no_os_spi_desc *spi_desc;
	/* GPIO */
	struct no_os_gpio_desc *gpio_powerdown;
	struct ad9208_state *st;
} ad9208_dev;

struct ad9208_state {
	ad9208_handle_t *adc_h;
	uint64_t sampling_frequency_hz;
	uint32_t input_div; /* input clock divider ratio */
	bool powerdown_pin_en;
	uint32_t powerdown_mode;
	bool duty_cycle_stabilizer_en;
	uint8_t current_scale;
	bool analog_input_mode;
	bool ext_vref_en;
	uint32_t buff_curr_n;
	uint32_t buff_curr_p;
	uint8_t fc_ch;
	struct ad9208_ddc ddc[4];
	uint32_t ddc_cnt;
	bool ddc_output_format_real_en;
	bool ddc_input_format_real_en;
	uint32_t test_mode_ch0;
	uint32_t test_mode_ch1;

	uint32_t sysref_lmfc_offset;
	bool sysref_edge_sel;
	bool sysref_clk_edge_sel;
	uint32_t sysref_neg_window_skew;
	uint32_t sysref_pos_window_skew;
	uint32_t sysref_mode;
	uint32_t sysref_count;

	jesd_param_t *jesd_param;
	uint32_t jesd_subclass;
};

typedef struct ad9208_init_param {
	/* SPI */
	struct no_os_spi_init_param *spi_init;
	/* GPIO */
	struct no_os_gpio_init_param gpio_powerdown;
	uint64_t sampling_frequency_hz;
	uint32_t input_div; /* input clock divider ratio */
	bool powerdown_pin_en;
	uint32_t powerdown_mode;
	bool duty_cycle_stabilizer_en;
	uint8_t current_scale;
	bool analog_input_mode;
	bool ext_vref_en;
	uint32_t buff_curr_n;
	uint32_t buff_curr_p;
	uint8_t fc_ch;
	struct ad9208_ddc *ddc;
	uint32_t ddc_cnt;
	bool ddc_output_format_real_en;
	bool ddc_input_format_real_en;
	uint32_t test_mode_ch0;
	uint32_t test_mode_ch1;

	uint32_t sysref_lmfc_offset;
	bool sysref_edge_sel;
	bool sysref_clk_edge_sel;
	uint32_t sysref_neg_window_skew;
	uint32_t sysref_pos_window_skew;
	uint32_t sysref_mode;
	uint32_t sysref_count;

	jesd_param_t *jesd_param;
	uint32_t jesd_subclass;
} ad9208_init_param;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Initialize the device. */
int32_t ad9208_initialize(ad9208_dev **device, ad9208_init_param *init_param);
/* Remove the device. */
int32_t ad9208_remove(ad9208_dev *device);

#endif // __AD9208_H__
