/***************************************************************************//**
* @file platform_drivers.h
* @brief Header file of Platform Drivers.
* @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
* Copyright 2014-2015(c) Analog Devices, Inc.
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
#include <stdint.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define min(x, y) (((x) < (y)) ? (x) : (y))
#define min_t(type, x, y) (type)min((type)(x), (type)(y))
#define max(x, y) (((x) > (y)) ? (x) : (y))
#define max_t(type, x, y) (type)max((type)(x), (type)(y))
#define clamp(val, min_val, max_val) (max(min((val), (max_val)), (min_val)))
#define clamp_t(type, val, min_val, max_val) (type)clamp((type)(val), \
											 (type)(min_val), (type)(max_val))

#define GPIO_OUTPUT	1
#define GPIO_INPUT	0
#define GPIO_HIGH	1
#define GPIO_LOW	0

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t spi_init(uint32_t device_id,
				 uint8_t clk_pha,
				 uint8_t clk_pol);
int32_t spi_write_and_read(uint8_t ss,
						   uint8_t *data,
						   uint8_t bytes_number);
int32_t gpio_init(uint32_t device_id);
int32_t gpio_direction(uint8_t pin, uint8_t direction);
int32_t gpio_set_value(uint8_t pin, uint8_t data);
int32_t gpio_get_value(uint8_t pin, uint8_t *data);
void mdelay(uint32_t msecs);
uint64_t do_div(uint64_t* n, uint64_t base);
#endif
