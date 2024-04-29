/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by iio examples.
 *   @author Axel Haslam (ahaslam@baylibre.com)
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "common_data.h"
#include "no_os_gpio.h"
#include "no_os_pwm.h"
#include "ad796x.h"
#include "axi_dmac.h"
#include "clk_axi_clkgen.h"
#include "axi_adc_core.h"
#include "axi_pwm_extra.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#ifdef IIO_EXAMPLE
struct no_os_uart_init_param iio_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
	.platform_ops = UART_OPS,
};
#endif

struct axi_adc_init ad796x_core_ip = {
	.name = "ad796x_core",
	.num_channels = 1,
	.base = RX_CORE_BASEADDR
};

struct axi_clkgen_init clkgen_ip = {
	.name = "rx_clkgen",
	.base = RX_CLKGEN_BASEADDR,
	.parent_rate = 100000000,
};

struct axi_dmac_init dmac_ip = {
	.name = "ad796x_dmac",
	.base = RX_DMA_BASEADDR,
	.irq_option = IRQ_DISABLED
};

struct axi_pwm_init_param axi_pwm_0_extra = {
	.base_addr = AXI_PWMGEN_BASEADDR,
	.ref_clock_Hz = 125000000,
	.channel = 0
};

struct no_os_pwm_init_param axi_pwm_0_ip = {
	.period_ns = PWM_0_PERIOD_NS,
	.duty_cycle_ns = PWM_0_DUTY_NS,
	.phase_ns = PWM_0_PHASE,
	.platform_ops = &axi_pwm_ops,
	.extra = &axi_pwm_0_extra
};

struct axi_pwm_init_param axi_pwm_1_extra = {
	.base_addr = AXI_PWMGEN_BASEADDR,
	.ref_clock_Hz = 125000000,
	.channel = 1
};

struct no_os_pwm_init_param axi_pwm_1_ip = {
	.period_ns = PWM_1_PERIOD_NS,
	.duty_cycle_ns = PWM_1_DUTY_NS,
	.phase_ns = PWM_1_PHASE,
	.platform_ops = &axi_pwm_ops,
	.extra = &axi_pwm_1_extra
};

struct xil_gpio_init_param xil_gpio_init = {
	.device_id = GPIO_DEVICE_ID,
	.type = GPIO_PS
};

struct no_os_gpio_init_param gpio_adc_en3_fmc_ip = {
	.number = GPIO_EN3_FMC,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

struct no_os_gpio_init_param gpio_adc_en2_fmc_ip = {
	.number = GPIO_EN2_FMC,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

struct no_os_gpio_init_param gpio_adc_en1_fmc_ip = {
	.number = GPIO_EN1_FMC,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

struct no_os_gpio_init_param gpio_adc_en0_fmc_ip = {
	.number = GPIO_EN0_FMC,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

struct ad796x_init_param ad796x_init_param = {
	.ad796x_core_ip = &ad796x_core_ip,
	.clkgen_ip = &clkgen_ip,
	.dmac_ip = &dmac_ip,
	.axi_pwm_0_ip = &axi_pwm_0_ip,
	.axi_pwm_1_ip = &axi_pwm_1_ip,
	.gpio_ip[3] = &gpio_adc_en3_fmc_ip,
	.gpio_ip[2] = &gpio_adc_en2_fmc_ip,
	.gpio_ip[1] = &gpio_adc_en1_fmc_ip,
	.gpio_ip[0] = &gpio_adc_en0_fmc_ip,
	.mode = AD796X_MODE1_EXT_REF_5P0,
};
