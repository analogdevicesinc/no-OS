/***************************************************************************//**
 * @file platform_drivers.h
 * @brief Header file of Platform Drivers.
 ********************************************************************************
 * Copyright 2017(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 * - Neither the name of Analog Devices, Inc. nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * - The use of this software may or may not infringe the patent rights
 * of one or more patent holders. This license does not release you
 * from the requirement that you obtain separate licenses from these
 * patent holders to use this software.
 * - Use of the software either in source or binary form, must be run
 * on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/
#ifndef PLATFORM_DRIVERS_H_
#define PLATFORM_DRIVERS_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <sleep.h>
#ifdef _XPARAMETERS_PS_H_
#include <xspips.h>
#include <xgpiops.h>
#else
#include <xspi.h>
#include <xgpio.h>
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#ifdef _XPARAMETERS_PS_H_
#define GPIO_OFFSET	54
#else
#define GPIO_OFFSET	0
#endif

#define DAC_GPIO_PLDDR_BYPASS	GPIO_OFFSET + 60
#define AD9528_RESET_B      GPIO_OFFSET + 59
#define AD9528_SYSREF_REQ   GPIO_OFFSET + 58
#define AD9371_TX1_ENABLE   GPIO_OFFSET + 57
#define AD9371_TX2_ENABLE   GPIO_OFFSET + 56
#define AD9371_RX1_ENABLE   GPIO_OFFSET + 55
#define AD9371_RX2_ENABLE   GPIO_OFFSET + 54
#define AD9371_TEST         GPIO_OFFSET + 53
#define AD9371_RESET_B      GPIO_OFFSET + 52
#define AD9371_GPINT        GPIO_OFFSET + 51
#define AD9371_GPIO_00      GPIO_OFFSET + 50
#define AD9371_GPIO_01      GPIO_OFFSET + 49
#define AD9371_GPIO_02      GPIO_OFFSET + 48
#define AD9371_GPIO_03      GPIO_OFFSET + 47
#define AD9371_GPIO_04      GPIO_OFFSET + 46
#define AD9371_GPIO_05      GPIO_OFFSET + 45
#define AD9371_GPIO_06      GPIO_OFFSET + 44
#define AD9371_GPIO_07      GPIO_OFFSET + 43
#define AD9371_GPIO_15      GPIO_OFFSET + 42
#define AD9371_GPIO_08      GPIO_OFFSET + 41
#define AD9371_GPIO_09      GPIO_OFFSET + 40
#define AD9371_GPIO_10      GPIO_OFFSET + 39
#define AD9371_GPIO_11      GPIO_OFFSET + 38
#define AD9371_GPIO_12      GPIO_OFFSET + 37
#define AD9371_GPIO_14      GPIO_OFFSET + 36
#define AD9371_GPIO_13      GPIO_OFFSET + 35
#define AD9371_GPIO_17      GPIO_OFFSET + 34
#define AD9371_GPIO_16      GPIO_OFFSET + 33
#define AD9371_GPIO_18      GPIO_OFFSET + 32

#define AD9528_CHIP_SELECT	1
#define AD9371_CHIP_SELECT	2

#ifdef _XPARAMETERS_PS_H_
#define mdelay(msecs)		usleep(1000*msecs)
#else
#define mdelay(msecs)		usleep(50*msecs)	// FIXME
#endif
#define udelay(usecs)		usleep(usecs)

#define ARRAY_SIZE(ar)		(sizeof(ar)/sizeof(ar[0]))

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
typedef struct {
	uint32_t	base_address;
	uint32_t	device_id;
	uint8_t		chip_select;
	uint32_t	cpha;
	uint32_t	cpol;
	uint8_t		id_no;
} spi_device;

typedef struct {
	uint32_t	device_id;
} gpio_device;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t platform_init(void);
int32_t spi_init(uint16_t device_id);
int32_t spi_write_and_read(spi_device *dev,
						   uint8_t *data,
						   uint8_t bytes_number);
int32_t gpio_init(uint16_t device_id);
int32_t gpio_direction_output(uint8_t gpio,
							  uint8_t value);
int32_t gpio_set_value(uint8_t gpio,
					   uint8_t value);
uint32_t ad_pow2(uint32_t number);
uint32_t find_first_bit(uint32_t word);
#endif
