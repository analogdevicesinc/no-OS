/***************************************************************************//**
 *   @file   ad413x/src/app/main.c
 *   @brief  Implementation of Main Function.
 *   @author Porumb Andrei (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "no-os/irq.h"
#include "irq_extra.h"
#include "gpio_irq_extra.h"
#include "no-os/spi.h"
#include "no-os/gpio.h"
#include "gpio_extra.h"
#include "spi_extra.h"
#include "ad413x.h"
#include "no-os/error.h"
#include "xparameters.h"
#include "parameters.h"
#include "no-os/print_log.h"
#include <stdint.h>
#include <inttypes.h>

/***************************************************************************//**
 * @brief main
*******************************************************************************/
int main()
{
	uint32_t ret;
	int32_t i;

	/* SPI instance */
	struct xil_spi_init_param spi_extra = {
		.type = SPI_PS,
		.flags = 0U
	};
	struct spi_init_param spi_ip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 100000,
		.mode = SPI_MODE_3,
		.chip_select = 0U,
		.bit_order = SPI_BIT_ORDER_MSB_FIRST,
		.platform_ops = &xil_spi_ops,
		.extra = &spi_extra
	};

	/* IRQ instance. */
	struct irq_ctrl_desc *irq_desc;
	struct xil_irq_init_param irq_extra = {
		.type = IRQ_PS
	};
	struct irq_init_param irq_ip = {
		.irq_ctrl_id = 0U,
		.platform_ops = &xil_irq_ops,
		.extra = &irq_extra
	};

	ret = irq_ctrl_init(&irq_desc, &irq_ip);
	if(ret)
		return FAILURE;
	ret = irq_global_enable(irq_desc);
	if(ret)
		return FAILURE;

	/* GPIO IRQ instance. */
	struct irq_ctrl_desc *gpio_irq_desc;
	struct xil_gpio_irq_init_param gpio_irq_extra = {
		.parent_desc = irq_desc,
		.gpio_device_id = XPAR_PS7_GPIO_0_DEVICE_ID
	};
	struct irq_init_param gpio_irq_ip = {
		.irq_ctrl_id = GPIO_IRQ_ID,
		.platform_ops = &xil_gpio_irq_ops,
		.extra = &gpio_irq_extra
	};

	ret = irq_ctrl_init(&gpio_irq_desc, &gpio_irq_ip);
	if(ret)
		return FAILURE;

	/* Device AD413X instance. */
	struct ad413x_dev *ad413x_dev;
	struct ad413x_init_param ad413x_dev_ip = {
		.spi_init = &spi_ip,
		.chip_id = AD4130_8,
		.mclk = AD413X_INT_76_8_KHZ_OUT_OFF,
		.op_mode = AD413X_SINGLE_CONV_MODE,
		.irq_desc = gpio_irq_desc,
		.rdy_pin = RDY_PIN,
		.preset[0] = { AD413X_REFIN1, AD413X_GAIN_4, AD413X_SYNC4_STANDALONE},
		.preset[1] = { AD413X_REFOUT_AVSS, AD413X_GAIN_16, AD413X_SYNC3_PF1},
		.preset[2] = { AD413X_REFIN2, AD413X_GAIN_64, AD413X_SYNC3_STANDALONE},
		.ch[0] = { 0, 1, 0, 0x13}, //ch 0, preset 0, enable, AIN0, DGND
		.ch[1] = { 2, 1, 1, 0x13},   //ch 1, preset 2, enable, AIN1, DGND
		.ch[2] = { 1, 1, 2, 0x13}, //ch 2, preset 1, enable, AIN2, DGND
		.ch[6] = { 0, 1, 3, 0x13}, //ch 6, preset 0, enable, AIN3, DGND
		.ch[12] = { 2, 0, 4, 0x13}, //ch 12, preset 2, disabled, AIN4, DGND
		.bipolar = 0,
		.int_ref = 1
	};

	ret = ad413x_init(&ad413x_dev, ad413x_dev_ip);
	if(ret)
		return FAILURE;

	return SUCCESS;
}
