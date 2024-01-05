/***************************************************************************//**
 *   @file   ad463x_fmc.c
 *   @brief  Implementation of Main Function.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
 ********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
#include <stdio.h>
#include <inttypes.h>
#include <xil_cache.h>
#include "spi_engine.h"
#include "no_os_pwm.h"
#include "no_os_uart.h"
#include "no_os_units.h"
#include "axi_pwm_extra.h"
#include "ad463x.h"
#include "no_os_error.h"
#include "clk_axi_clkgen.h"
#include "no_os_gpio.h"
#include "xilinx_gpio.h"
#include "parameters.h"
#include "no_os_print_log.h"
#include "xilinx_uart.h"

#ifdef IIO_SUPPORT
#include "iio_app.h"
#include "iio_ad463x.h"
#endif
/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD463x_BUF_SIZE			3000
#define AD463x_EVB_SAMPLE_NO		1000
/* Main function */
int main()
{
	uint32_t buf[AD463x_BUF_SIZE] __attribute__ ((aligned));
	struct ad463x_dev *dev;
	int32_t ret;
	uint32_t i;

	struct xil_uart_init_param platform_uart_init_par = {
		.type = UART_PS,
		.irq_id = UART_IRQ_ID
	};

	struct no_os_uart_init_param iio_uart_ip = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = &platform_uart_init_par,
		.platform_ops = &xil_uart_ops
	};

#if IIO_SUPPORT
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
#endif
	struct no_os_uart_desc *uart_desc;

	struct spi_engine_offload_init_param spi_engine_offload_init_param = {
		.offload_config = OFFLOAD_RX_EN,
		.rx_dma_baseaddr = AD463x_DMA_BASEADDR,
	};

	struct spi_engine_init_param spi_eng_init_param  = {
		.ref_clk_hz = 160000000,
		.type = SPI_ENGINE,
		.spi_engine_baseaddr = AD463x_SPI_ENGINE_BASEADDR,
		.cs_delay = 0,
		.data_width = 32,
	};

	struct axi_clkgen_init clkgen_init = {
		.name = "rx_clkgen",
		.base = RX_CLKGEN_BASEADDR,
		.parent_rate = 100000000,
	};

	struct axi_pwm_init_param axi_pwm_init = {
		.base_addr = AXI_PWMGEN_BASEADDR,
		.ref_clock_Hz = 100000000,
		.channel = 0
	};

	struct no_os_pwm_init_param trigger_pwm_init = {
		.period_ns = 500,	/* 2Mhz */
		.duty_cycle_ns = AD463X_TRIGGER_PULSE_WIDTH_NS,
		.polarity = NO_OS_PWM_POLARITY_HIGH,
		.platform_ops = &axi_pwm_ops,
		.extra = &axi_pwm_init,
	};

	struct xil_gpio_init_param gpio_extra_param = {
		.device_id = GPIO_DEVICE_ID,
		.type = GPIO_PS,
	};

	struct no_os_gpio_init_param ad463x_resetn = {
		.number = GPIO_RESETN_1,
		.platform_ops = &xil_gpio_ops,
		.extra = &gpio_extra_param
	};
#if ADAQ4224_DEV
	/* PGIA gain control inputs */
	struct no_os_gpio_init_param ad463x_pgia_a0 = {
		.number = GPIO_PGIA_0,
		.platform_ops = &xil_gpio_ops,
		.extra = &gpio_extra_param
	};
	struct no_os_gpio_init_param ad463x_pgia_a1 = {
		.number = GPIO_PGIA_1,
		.platform_ops = &xil_gpio_ops,
		.extra = &gpio_extra_param
	};
#endif

	struct no_os_spi_init_param spi_init = {
		.chip_select = AD463x_SPI_CS,
		.max_speed_hz = 80000000,
		.mode = NO_OS_SPI_MODE_0,
		.platform_ops = &spi_eng_platform_ops,
		.extra = (void*)&spi_eng_init_param,
	};

	struct ad463x_init_param ad463x_init_param = {
		.spi_init = &spi_init,
		.offload_init_param = &spi_engine_offload_init_param,
		.trigger_pwm_init = &trigger_pwm_init,
		.gpio_resetn = &ad463x_resetn,
		.clkgen_init = &clkgen_init,
		.axi_clkgen_rate = 160000000,
		.reg_access_speed = 20000000,
		.reg_data_width = 8,
		.output_mode = AD463X_32_PATTERN,
		.lane_mode = AD463X_ONE_LANE_PER_CH,
		.clock_mode = AD463X_SPI_COMPATIBLE_MODE,
		.data_rate = AD463X_SDR_MODE,
#if ADAQ4224_DEV
		.vref = 4096 * MILLI,
		.device_id = ID_ADAQ4224, /* dev_id */
		.gpio_pgia_a0 = &ad463x_pgia_a0,
		.gpio_pgia_a1 = &ad463x_pgia_a1,
#elif AD4030_DEV
		.vref = 5000 * MILLI,
		.device_id = ID_AD4030, /* dev_id */
#else
		.vref = 5000 * MILLI,
		.device_id = ID_AD4630_24, /* dev_id */
#endif
		.dcache_invalidate_range =
		(void (*)(uint32_t, uint32_t))Xil_DCacheInvalidateRange,
	};

#ifndef IIO_SUPPORT
	/** Initialize UART when IIO is not enabled */
	ret = no_os_uart_init(&uart_desc, &iio_uart_ip);
	if (ret < 0)
		return ret;

	no_os_uart_stdio(uart_desc);
#endif

	/* Enable the instruction cache. */
	Xil_ICacheEnable();
	/* Enable the data cache. */
	Xil_DCacheEnable();

	ret = ad463x_init(&dev, &ad463x_init_param);
	if (ret != 0) {
		pr_err("AD463x Initialization failed!\n");
		return ret;
	}

	pr_info("AD463x Successfully initialized!\n");

	/* Exit register configuration mode */
	ret = ad463x_exit_reg_cfg_mode(dev);
	if (ret != 0)
		return ret;

	/* Test Pattern Mode, 32-bit output data, 1 lane per channel */
	ret = ad463x_read_data(dev, buf, AD463x_EVB_SAMPLE_NO);
	if (ret != 0)
		return ret;

	for (i = 0; i < (AD463x_EVB_SAMPLE_NO); i += 2) {
		if (buf[i] != AD463X_OUT_DATA_PAT) {
			pr_err("AD463x Test Pattern Data read failed! :%d/n",i);
			return -1;
		}
	}

	pr_info("AD463x Test Pattern Data successfully read!/n");

	ad463x_remove(dev);

	ad463x_init_param.output_mode = AD463X_24_DIFF;

	ret = ad463x_init(&dev, &ad463x_init_param);
	if (ret != 0)
		return ret;

#if ADAQ4224_DEV
	/* Estimating a gain of 1/3, calculate the closest accepted pgia gain value */
	int32_t gain_int = 0;
	int32_t gain_fract = 1464843;
	enum ad463x_pgia_gain pgia_gain_idx;
	ret = ad463x_calc_pgia_gain(gain_int, gain_fract, dev->vref,
				    dev->real_bits_precision, &pgia_gain_idx);
	if (ret != 0)
		return ret;
	/** set pgia gain */
	ret = ad463x_set_pgia_gain(dev, pgia_gain_idx);
	if (ret != 0)
		return ret;
#endif

	/* Exit register configuration mode */
	ret = ad463x_exit_reg_cfg_mode(dev);
	if (ret != 0)
		return ret;

#ifndef IIO_SUPPORT
	/* Read data */
	while (true) {
		ret = ad463x_read_data(dev, buf, AD463x_EVB_SAMPLE_NO);
		if (ret != 0)
			return ret;
		for (i = 0; i < AD463x_EVB_SAMPLE_NO; i+=2)
			pr_info("ADC sample ch1: %lu : %lu \n", i, buf[i]);
		for (i = 1; i < AD463x_EVB_SAMPLE_NO; i+=2)
			pr_info("ADC sample ch2: %lu : %lu \n", i, buf[i]);
	}
#else
	struct iio_ad463x *iio_ad463x;

	struct iio_data_buffer rd_buff = {
		.buff = (void *)buf,
		.size = sizeof(buf)
	};

	ret = iio_ad463x_init(&iio_ad463x, dev);
	if(ret < 0)
		return ret;

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("ad463x", iio_ad463x, &iio_ad463x->iio_dev_desc,
			       &rd_buff, NULL, NULL),
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = iio_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);

#endif

	ad463x_remove(dev);

	pr_info("Done.\n");

	/* Disable the instruction cache. */
	Xil_DCacheDisable();
	/* Disable the data cache. */
	Xil_ICacheDisable();

	return 0;
}
