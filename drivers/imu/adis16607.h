/***************************************************************************//**
 *   @file   adis16607.h
 *   @brief  Implementation of adis16607.h
 *   @author Radu Sabau (radu.sabau@analog.com)
 *******************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
 ******************************************************************************/
#ifndef __ADIS16607_H__
#define __ADIS16607_H__

#include "adis.h"

#define ADIS16607_USER_DATA_CFG_REG			0x34
#define ADIS16607_USER_SYNC_REG				0x33
#define ADIS16607_USER_GPIO_CFG_REG			0x2F
#define ADIS16607_DIAG_STAT_REG				0x05

#define ADIS16607_DATA_READY_GPIO_MASK			NO_OS_GENMASK(11, 9)
#define ADIS16607_SYNC_GPIO_MASK			NO_OS_GENMASK(8, 6)
#define ADIS16607_RESET_GPIO_MASK			NO_OS_GENMASK(2, 0)
#define ADIS16607_DATA_CNTR_EN_MASK			NO_OS_BIT(14)
#define ADIS16607_ID_NO_OFFSET(x)			((x) - ADIS16607_2)
#define ADIS16607_GPIO_FUNC_NO_OFFSET(x, y)		((x) - (y))

#define ADIS16607_I2C_ADDRESS_DOUTLOW_SCLKLOW		0x38
#define ADIS16607_I2C_ADDRESS_DOUTLOW_SCLKHIGH		0x39
#define ADIS16607_I2C_ADDRESS_DOUTHIGH_SCLKLOW		0x3A
#define ADIS16607_I2C_ADDRESS_DOUTHIGH_SCLKHIGH		0x3B

#define ADIS16607_SELF_TEST_DELAY_MS			10

extern const struct adis_chip_info adis16607_chip_info;

/* FIFO functions */
int adis16607_fifo_enable(struct adis_dev *adis, bool enable, uint16_t thr,
			  bool burst32);
int adis16607_fifo_pop(struct adis_dev *adis, struct adis_burst_data *data);

#endif /* __ADIS16607_H__*/
