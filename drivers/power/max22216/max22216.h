/***************************************************************************//**
 *   @file   max22216.h
 *   @brief  Current control
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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

#ifndef MAX22216_H
#define MAX22216_H

#include <stdbool.h>
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_alloc.h"
#include "no_os_utils.h"
#include "no_os_error.h"

#define MAX22216_NR_OF_CHANNELS         4 // Number of channels supported by MAX22216

#define MAX22216_K_CDR                  1.017
#define MAX22216_GAIN                   0.25
#define MAX22216_SNSF                   1
#define MAX22216_GLOBAL_CTRL            0x00
#define MAX22216_GLOBAL_CFG             0x01
#define MAX22216_F_AC                   0x07
#define MAX22216_U_AC_SCAN              0x08
#define MAX22216_CFG_DC_L2H_0           0x09
#define MAX22216_CFG_DC_H_0             0x0A
#define MAX22216_CFG_L2H_TIME_0         0x0C
#define MAX22216_CFG_CTRL0_0            0x0D
#define MAX22216_CFG_CTRL1_0            0x0E

#define MAX22216_CFG_IND_0_0            0x13
#define MAX22216_CFG_IND_DITHERING_BIT 	11

#define MAX22216_CFG_P_0                0x15
#define MAX22216_DIAG_CURR_MON_0        0x45 // Diagnostics current monitor register for channel 0
#define MAX22216_CFG_I_0                0x16
#define MAX22216_FAULT0                 0x65
#define MAX22216_FAULT1                 0x66

#define MAX22216_CHANNEL_CONFIG_REG_SHIFT   0x14 // Channel configuration registers start at 0x14
#define MAX22216_DIAGNOSTICS_REG_SHIFT      0x09

typedef struct {
	uint8_t reg_addr; // Register address
	int16_t data; // Data to write
} max22216_reg_setting_t;

struct max22216_init_param {
	struct no_os_spi_init_param *spi_ip;
	struct no_os_gpio_init_param *drv_en_gpio_ip; // GPIO for driver enable
	struct no_os_gpio_init_param *fault_gpio_ip; // GPIO for fault indication
};

struct max22216_desc {
	struct no_os_spi_desc *spi_desc;
	struct no_os_gpio_desc *drv_en_gpio; // GPIO for driver enable
	struct no_os_gpio_desc *fault_gpio; // GPIO for fault indication
	uint16_t status_reg; // Status register value
};

int max22216_init(struct max22216_desc **desc,
		  struct max22216_init_param *param);
int max22216_remove(struct max22216_desc *desc);
int max22216_write_reg(struct max22216_desc *desc, uint8_t, uint16_t);
int max22216_read_reg(struct max22216_desc *desc, uint8_t, uint16_t*);
int max22216_write_reg_list(struct max22216_desc *desc,
			    max22216_reg_setting_t* list, uint8_t elem_nr);
int max22216_set_enable_pin(struct max22216_desc *desc, bool value);
int max22216_check_fault_pin(struct max22216_desc *desc, bool *fault_status);
int max22216_current_reg_control(struct max22216_desc *desc, uint8_t channel_nr,
				 uint16_t value);
int max22216_set_current_mA(struct max22216_desc *desc, uint8_t channel_nr,
			    uint16_t current_mA);
int max22216_turn_on(struct max22216_desc *desc, uint8_t channel_nr);
int max22216_turn_off(struct max22216_desc *desc, uint8_t channel_nr);

#endif // MAX22216_H
