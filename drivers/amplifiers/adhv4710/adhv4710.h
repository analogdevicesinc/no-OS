/***************************************************************************//**
 *   @file   adhv4710.h
 *   @brief  Header file of ADHV4710 Driver.
 *   @author REtz (radu.etz@analog.com)
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
#ifndef __ADHV4710_H__
#define __ADHV4710_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "no_os_util.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_irq.h"
#include "no_os_delay.h"
#include "no_os_units.h"
#include "no_os_alloc.h"
#include "no_os_crc16.h"
#include "no_os_print_log.h"
#include <stdlib.h>
#include <errno.h>
#include <math.h>

/* SPI commands */
#define ADHV4710_SPI_READ					NO_OS_BIT(7)

/* Version product */
#define ADHV4710_SILICON_REV					0x4
#define ADHV4710_VERSION_PRODUCT     				0x46

/* ADHV4710 Register Map */
/* 8 BIT REGISTERS */
#define ADHV4710_REG_CTRL_REG_00				0x000
#define ADHV4710_REG_CTRL_REG_03				0x003
#define ADHV4710_REG_CTRL_REG_04				0x004
#define ADHV4710_REG_CTRL_REG_08				0x008
#define ADHV4710_REG_CTRL_REG_09				0x009
#define ADHV4710_REG_CTRL_REG_10				0x00A
#define ADHV4710_REG_CTRL_REG_11              			0x00B
#define ADHV4710_REG_CTRL_REG_12				0x00C
#define ADHV4710_REG_CTRL_REG_13				0x00D
#define ADHV4710_REG_CTRL_REG_14				0x00E
#define ADHV4710_REG_CTRL_REG_25				0x019
#define ADHV4710_REG_CTRL_REG_26				0x01A

/* ADHV4710_REG_CTRL_REG_00 Bit Definition */
#define ADHV4710_HV_RESET_MSK					NO_OS_BIT(7)
#define ADHV4710_HV_SLEEP_MSK					NO_OS_BIT(4)

/* ADHV4710_REG_CTRL_REG_03 Bit Definition */
#define ADHV4710_SOFT_RESET_MSK					NO_OS_BIT(0)

/* ADHV4710_REG_CTRL_REG_04 Bit Definition */
#define ADHV4710_SET_IQ_MSK					NO_OS_GENMASK(7, 0)

/* ADHV4710_REG_CTRL_REG_08 Bit Definition */
#define ADHV4710_OC_SRC_ARM_MSK					NO_OS_BIT(7)
#define ADEHV4710_OC_SRC_LIMIT_MSK				NO_OS_GENMASK(6, 0)

/* ADHV4710_REG_CTRL_REG_09 Bit Definition */
#define ADHV4710_OC_SNK_ARM_MSK					NO_OS_BIT(7)
#define ADEHV4710_OC_SNK_LIMIT_MSK				NO_OS_GENMASK(6, 0)

/* ADHV4710_REG_CTRL_REG_10 Bit Definition */
#define ADHV4710_OV_POS_ARM_MSK					NO_OS_BIT(7)
#define ADEHV4710_OV_POS_LIMIT_MSK				NO_OS_GENMASK(6, 0)

/* ADHV4710_REG_CTRL_REG_011 Bit Definition */
#define ADHV4710_OV_NEG_ARM_MSK					NO_OS_BIT(7)
#define ADEHV4710_OV_NEG_LIMIT_MSK				NO_OS_GENMASK(6, 0)

/* ADHV4710_REG_CTRL_REG_12 Bit Definition */
#define ADHV4710_OT_ARM_MSK					NO_OS_BIT(7)
#define ADEHV4710_OT_LIMIT_MSK					NO_OS_GENMASK(6, 0)

/* ADHV4710_REG_CTRL_REG_13 Bit Definition */
#define ADHV4710_OC_SRC_ALARM_LATCH_MSK				NO_OS_BIT(4)
#define ADHV4710_OC_SNK_ALARM_LATCH_MSK				NO_OS_BIT(3)
#define ADHV4710_OV_POS_ALARM_LATCH_MSK				NO_OS_BIT(2)
#define ADHV4710_OV_NEG_ALARM_LATCH_MSK				NO_OS_BIT(1)
#define ADHV4710_OT_ALARM_LATCH_MSK				NO_OS_BIT(0)
#define ADHV4710_ALL_ALARM_LATCH_MSK				NO_OS_GENMASK(4, 0)

/* ADHV4710_REG_CTRL_REG_14 Bit Definition */
#define ADHV4710_SHUTDOWN_FLAG_MSK				NO_OS_BIT(7)
#define ADHV4710_OC_SRC_ALARM_MSK				NO_OS_BIT(4)
#define ADHV4710_OC_SNK_ALARM_MSK				NO_OS_BIT(3)
#define ADHV4710_OV_POS_ALARM_MSK				NO_OS_BIT(2)
#define ADHV4710_OV_NEG_ALARM_MSK				NO_OS_BIT(1)
#define ADHV4710_OT_ALARM_MSK					NO_OS_BIT(0)

/* ADHV4710_REG_CTRL_25 Bit Definition */
#define ADHV4710_DIE_REV_MSK					NO_OS_GENMASK(3, 0)

/* ADHV4710_REG_CTRL_26 Bit Definition */
#define ADHV4710_CHIP_ID_MSK					NO_OS_GENMASK(7, 0)

#define ADHV4710_INC_DEC_MSK 					NO_OS_BIT(7)
#define ADHV4710_INCREASE_I 					1
#define ADHV4710_DECREASE_I 					0

/**
* @struct adhv4710_init_param
* @brief ADHV4710 Device initialization parameters.
*/
struct adhv4710_init_param {
	/** Device communication descriptor */
	struct no_os_spi_init_param 	*spi_init;
	/** GPIO RESET descriptor used to reset device (HW reset) */
	struct no_os_gpio_init_param  	*gpio_reset;
};

/**
* @struct adhv4710_dev
* @brief ADHV4710 Device structure.
*/
struct adhv4710_dev {
	/** Device communication descriptor */
	struct no_os_spi_desc		*spi_desc;
	/** GPIO RESET descriptor used to reset device (HW reset) */
	struct no_os_gpio_desc  	*gpio_reset;
};

/* Initialize the device. */
int adhv4710_init(struct adhv4710_dev **device,
		  struct adhv4710_init_param init_param);

/* Read device register. */
int adhv4710_read(struct adhv4710_dev *dev, uint8_t reg_addr,
		  int8_t *reg_data);

/* Write device register. */
int adhv4710_write(struct adhv4710_dev *dev, uint8_t reg_addr,
		   uint8_t reg_data);

/* Update specific register bits. */
int adhv4710_update_bits(struct adhv4710_dev *dev, uint8_t reg_addr,
			 uint8_t mask, uint8_t reg_data);

/* Get status for a specific bit setting. */
int adhv4710_get_status(struct adhv4710_dev *dev, uint8_t reg_addr, uint8_t msk,
			uint8_t *status);

/* Remove the device and release resources. */
int adhv4710_remove(struct adhv4710_dev *dev);

/* Reset the device using SW reset. */
int adhv4710_sw_reset(struct adhv4710_dev *dev);

/* Reset the device using HW reset. */
int adhv4710_hw_reset(struct adhv4710_dev *dev);

/* Clear shutdown latch and re-enable amplifier from shutdown */
int adhv4710_clear_shutdown_latch(struct adhv4710_dev *dev);

/* Shut down the amplifier */
int adhv4710_shutdown_amplifier(struct adhv4710_dev *dev);

/* Program quiescent current of amplifier */
int adhv4710_set_quiescent_current(struct adhv4710_dev *dev, uint8_t value,
				   uint8_t direction);

/* Enable source overcurrent protection */
int adhv4710_enable_source_overcurrent_protection(struct adhv4710_dev *dev);

/* Set level of overcurrent source protection */
int adhv4710_set_source_overcurrent_level(struct adhv4710_dev *dev,
		uint8_t level);

/* Enable sink overcurrent protection */
int adhv4710_enable_sink_overcurrent_protection(struct adhv4710_dev *dev);

/* Set level of overcurrent sink protection */
int adhv4710_set_sink_overcurrent_level(struct adhv4710_dev *dev,
					uint8_t level);

/* Enable overvoltage positive protection */
int adhv4710_enable_overvoltage_pos_protection(struct adhv4710_dev *dev);

/* Set level of overvoltage positive protection */
int adhv4710_set_overvoltage_pos_level(struct adhv4710_dev *dev, uint8_t level);

/* Enable overvoltage negative protection */
int adhv4710_enable_overvoltage_neg_protection(struct adhv4710_dev *dev);

/* Set level of overvoltage negative protection */
int adhv4710_set_overvoltage_neg_level(struct adhv4710_dev *dev, uint8_t level);

/* Enable overtemperature protection */
int adhv4710_enable_overtemperature_protection(struct adhv4710_dev *dev);

/* Set level of overtemperature protection */
int adhv4710_set_overtemperature_level(struct adhv4710_dev *dev, uint8_t level);

/* Version product */
int adhv4710_version_product(struct adhv4710_dev *dev, uint8_t *data_read);

#endif /* __ADHV4710_H__ */
