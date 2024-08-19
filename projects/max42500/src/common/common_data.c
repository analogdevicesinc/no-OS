/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by max42500 examples.
 *   @author Joshua Maniti (Joshua.Maniti@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
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

#include "common_data.h"
#include "maxim_gpio.h"
#include "maxim_i2c.h"
#include "maxim_irq.h"
#include "maxim_timer.h"
#include "maxim_uart.h"


struct max_uart_init_param uart_extra_ip = {
	.flow = UART_FLOW_DIS
};

struct no_os_uart_init_param uart_ip = {
	.device_id             = 0,
	.asynchronous_rx       = false,
	.baud_rate             = 115200,
	.size                  = NO_OS_UART_CS_8,
	.parity                = NO_OS_UART_PAR_NO,
	.stop                  = NO_OS_UART_STOP_1_BIT,
	.extra                 = &uart_extra_ip,
	.platform_ops          = &max_uart_ops,
};

/* MAX42500 init parameters */
const struct max_i2c_init_param max42500_i2c_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

const struct max_gpio_init_param max42500_gpio_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};

struct max42500_init_param max42500_ip = {
	.regs       = max42500_regs,
	.comm_param = {
		.device_id      = MAX42500_I2C_DEVICE_ID,
		.max_speed_hz   = MAX42500_I2C_CLK_SPEED,
		.slave_address  = MAX42500_I2C_ADDR,
		.platform_ops   = &max_i2c_ops,
		.extra          = (void *)&max42500_i2c_ip,
	},
	.timer_param = {
		.id             = MAX42500_TIMER_ID,
		.freq_hz        = MAX42500_TIMER_ID,
		.ticks_count    = MAX42500_TIMER_COUNT,
		.platform_ops   = &max_timer_ops,
		.extra          = NULL,
	},
	.irq_param = {
		.irq_ctrl_id    = MAX42500_IRQ_ID,
		.platform_ops   = &max_irq_ops,
		.extra          = NULL,
	},
	.en0_param = {
		.port           = MAX42500_EN0_PORT,
		.number         = MAX42500_EN0_PIN,
		.pull           = NO_OS_PULL_DOWN,
		.platform_ops   = &max_gpio_ops,
		.extra          = (void *)&max42500_gpio_extra,
	},
	.en1_param = {
		.port           = MAX42500_EN1_PORT,
		.number         = MAX42500_EN1_PIN,
		.pull           = NO_OS_PULL_DOWN,
		.platform_ops   = &max_gpio_ops,
		.extra          = (void *)&max42500_gpio_extra,
	},
	.addr_param = {
		.port           = MAX42500_ADDR_PORT,
		.number         = MAX42500_ADDR_PIN,
		.pull           = NO_OS_PULL_DOWN,
		.platform_ops   = &max_gpio_ops,
		.extra          = (void *)&max42500_gpio_extra,
	},
	.addr_sel   = MAX42500_ADDR_SEL,
	.pece       = MAX42500_PECE,
	.vmon_en    = MAX42500_VMON_EN,
	.vmon_vmpd  = MAX42500_VMON_VMPD,
	.reset_map  = MAX42500_RESET_MAP,
	.wd_mode    = MAX42500_WD_MODE,
	.wd_en      = MAX42500_WD_EN,
};
