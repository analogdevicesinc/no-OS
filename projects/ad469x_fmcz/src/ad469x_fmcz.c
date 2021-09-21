/***************************************************************************//**
 *   @file   ad469x_fmcz.c
 *   @brief  Implementation of Main Function.
 *   @author Cristian Pop (cristian.pop@analog.com)
 ********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
#include "pwm.h"
#include "axi_pwm_extra.h"
#include "ad469x.h"
#include "error.h"
#include "clk_axi_clkgen.h"
#include "gpio.h"
#include "gpio_extra.h"
#include "parameters.h"

#ifdef IIO_SUPPORT
#include "iio.h"
#include "irq.h"
#include "iio_types.h"
#include "irq_extra.h"
#include "uart.h"
#include "uart_extra.h"
#include "xil_cache.h"
#endif // IIO_SUPPORT

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD469x_EVB_SAMPLE_NO		1000
#define TOTAL_CH					3

#ifdef IIO_SUPPORT

static uint32_t g_mask;

int32_t iio_ad469x_prepare_conversion(struct ad469x_dev *dev, uint32_t mask)
{
	int32_t	ret;

	if (mask == g_mask)
		return SUCCESS;

	ret = ad469x_exit_conversion_mode(dev);
	if (ret != SUCCESS)
		return ret;

	ret = ad469x_std_sequence_ch(dev, mask);
	if (ret != SUCCESS)
		return ret;

	ret = ad469x_set_channel_sequence(dev, AD469x_standard_seq);
	if (ret != SUCCESS)
		return ret;

	ret = ad469x_enter_conversion_mode(dev);
	if (ret != SUCCESS)
		return ret;

	g_mask = mask;

	return ret;
}

struct scan_type ad469x_scan_type = {
	.sign = 'u',
	.realbits = 19,
	.storagebits = 32,
	.shift = 0,
	.is_big_endian = false
};

static struct iio_channel ad469x_ch0 = {
	.ch_type = IIO_VOLTAGE,
	.channel = 0,
	.scan_index = 0,
	.scan_type = &ad469x_scan_type,
	.indexed = true,
};

static struct iio_channel ad469x_ch1 = {
	.ch_type = IIO_VOLTAGE,
	.channel = 1,
	.scan_index = 1,
	.scan_type = &ad469x_scan_type,
	.indexed = true,
};

static struct iio_channel *ad469x_iio_channels[] = {
	&ad469x_ch0,
	&ad469x_ch1,
	NULL,
};

struct iio_device ad469x_iio_descriptor = {
	.num_ch = 2,
	.channels = ad469x_iio_channels,
	.prepare_transfer = (int32_t (*)(void *, uint32_t))iio_ad469x_prepare_conversion,
	.read_dev = (int32_t (*)(void *, void *, uint32_t))ad469x_seq_read_data
};

#endif

int main()
{
	uint32_t buf[AD469x_EVB_SAMPLE_NO * TOTAL_CH] __attribute__ ((aligned));
	struct ad469x_dev *dev;
	uint32_t ch, i, j = 0;
	int32_t ret;

	struct spi_engine_offload_init_param spi_engine_offload_init_param = {
		.offload_config = OFFLOAD_RX_EN,
		.rx_dma_baseaddr = AD469x_DMA_BASEADDR,
	};

	struct spi_engine_init_param spi_eng_init_param  = {
		.ref_clk_hz = AD469x_SPI_ENG_REF_CLK_FREQ_HZ,
		.type = SPI_ENGINE,
		.spi_engine_baseaddr = AD469x_SPI_ENGINE_BASEADDR,
		.cs_delay = 0,
		.data_width = 16,
	};

	struct axi_clkgen_init clkgen_init = {
		.name = "rx_clkgen",
		.base = RX_CLKGEN_BASEADDR,
		.parent_rate = 100000000,
	};

	struct axi_pwm_init_param axi_pwm_init = {
		.base_addr = AXI_PWMGEN_BASEADDR,
		.ref_clock_Hz = 160000000,
		.channel = 0,
	};

	struct pwm_init_param trigger_pwm_init = {
		.period_ns = 1000,	/* 1Mhz */
		.duty_cycle_ns = 10,
		.polarity = PWM_POLARITY_HIGH,
		.extra = &axi_pwm_init,
	};

	struct xil_gpio_init_param gpio_extra_param = {
		.device_id = GPIO_DEVICE_ID,
		.type = GPIO_PS,
	};

	struct gpio_init_param ad469x_resetn = {
		.number = GPIO_RESETN_1,
		.platform_ops = &xil_gpio_ops,
		.extra = &gpio_extra_param
	};

	struct spi_init_param spi_init = {
		.chip_select = AD469x_SPI_CS,
		.max_speed_hz = 80000000,
		.mode = SPI_MODE_3,
		.platform_ops = &spi_eng_platform_ops,
		.extra = (void*)&spi_eng_init_param,
	};

	struct ad469x_init_param ad469x_init_param = {
		.spi_init = &spi_init,
		.offload_init_param = &spi_engine_offload_init_param,
		.trigger_pwm_init = &trigger_pwm_init,
		.gpio_resetn = &ad469x_resetn,
		.clkgen_init = &clkgen_init,
		.axi_clkgen_rate = 160000000,
		.reg_access_speed = 20000000,
		.reg_data_width = 8,
		.capture_data_width = 16,
		.dev_id = ID_AD4696, /* dev_id */
		.dcache_invalidate_range =
		(void (*)(uint32_t, uint32_t))Xil_DCacheInvalidateRange,
	};

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	ret = ad469x_init(&dev, &ad469x_init_param);
	if (ret < 0)
		return ret;

#ifdef IIO_SUPPORT
	struct uart_desc *uart_desc;
	struct iio_desc *iio_app_desc;
	struct xil_irq_init_param xil_irq_init_par = {
		.type = IRQ_PS,
	};
	struct irq_init_param irq_init_param = {
		.irq_ctrl_id = INTC_DEVICE_ID,
		.extra = &xil_irq_init_par,
	};
	struct irq_ctrl_desc *irq_desc;
	ret = irq_ctrl_init(&irq_desc, &irq_init_param);
	if (ret < 0)
		return ret;
	struct xil_uart_init_param xil_uart_init_par = {
		.type = UART_PS,
		.irq_id = UART_IRQ_ID,
		.irq_desc = irq_desc,
	};
	struct uart_init_param uart_init_par = {
		.baud_rate = 115200,
		.device_id = UART_DEVICE_ID,
		.extra = &xil_uart_init_par,
	};

	ret = uart_init(&uart_desc, &uart_init_par);
	if (ret < 0)
		return ret;

	struct iio_init_param iio_init_par = {
		.phy_type = USE_UART,
		.uart_desc = uart_desc,
	};

	ret = irq_global_enable(irq_desc);
	if (ret < 0)
		return ret;

	ret = iio_init(&iio_app_desc, &iio_init_par);
	if (ret < 0)
		return ret;

	struct iio_data_buffer read_buff = {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR
	};
	ret = iio_register(iio_app_desc, &ad469x_iio_descriptor, "adc",
			   dev, &read_buff, NULL);
	if (ret < 0)
		return ret;

	do {
		ret = iio_step(iio_app_desc);
	} while (true);

#endif // IIO_SUPPORT

#ifdef ADVANCED_SEQ
	ret = ad469x_adv_sequence_set_num_slots(dev, 2);
	if (ret != SUCCESS)
		return ret;

	ret = ad469x_adv_sequence_set_slot(dev, 0, 0);
	if (ret != SUCCESS)
		return ret;

	ret = ad469x_adv_sequence_set_slot(dev, 1, 1);
	if (ret != SUCCESS)
		return ret;

	ret = ad469x_adv_seq_osr(dev, 0, AD469x_OSR_4);
	if (ret != SUCCESS)
		return ret;

	ret = ad469x_adv_seq_osr(dev, 1, AD469x_OSR_16);
	if (ret != SUCCESS)
		return ret;

	ret = ad469x_sequence_enable_temp(dev);
	if (ret != SUCCESS)
		return ret;

	ret = ad469x_set_channel_sequence(dev, AD469x_advanced_seq);
	if (ret != SUCCESS)
		return ret;

	ret = ad469x_enter_conversion_mode(dev);
	if (ret != SUCCESS)
		return ret;

	while (1) {
		ret = ad469x_seq_read_data(dev, buf, AD469x_EVB_SAMPLE_NO);
		if (ret != SUCCESS)
			return ret;

		for (i = 0; i < AD469x_EVB_SAMPLE_NO * 3; i += 3)
			printf("ADC_sample:%"PRIu32",ch0:%"PRIu32",ch1:%"PRIu32",temp:%"PRIu32"\n",
			       i, buf[i], buf[i + 1], buf[i + 2]);
	}
#elif defined(STANDARD_SEQ)
	ret = ad469x_std_sequence_ch(dev, AD469x_CHANNEL(1) | AD469x_CHANNEL(0));
	if (ret != SUCCESS)
		return ret;

	ret = ad469x_sequence_disable_temp(dev);
	if (ret != SUCCESS)
		return ret;

	ret = ad469x_set_channel_sequence(dev, AD469x_standard_seq);
	if (ret != SUCCESS)
		return ret;

	ret = ad469x_std_seq_osr(dev, AD469x_OSR_4);
	if (ret != SUCCESS)
		return ret;

	ret = ad469x_enter_conversion_mode(dev);
	if (ret != SUCCESS)
		return ret;

	while (1) {
		ret = ad469x_seq_read_data(dev, &buf[0], AD469x_EVB_SAMPLE_NO);
		if (ret != SUCCESS)
			return ret;

		for (i = 0; i < AD469x_EVB_SAMPLE_NO * 2; i += 2)
			printf("ADC_sample:%"PRIu32",ch0:%"PRIu32",ch1:%"PRIu32"\n", i, buf[i],
			       buf[i + 1]);
	}
#else
	ret = ad469x_set_channel_sequence(dev, AD469x_single_cycle);
	if (ret != SUCCESS)
		return ret;

	ret = ad469x_enter_conversion_mode(dev);
	if (ret != SUCCESS)
		return ret;

	while (1) {
		ch = j % 2;
		j++;
		ret = ad469x_read_data(dev, ch, buf, AD469x_EVB_SAMPLE_NO);
		printf("ch: %"PRIu32"\n", ch);
		if (ret != SUCCESS)
			return ret;

		for (i = 0; i < AD469x_EVB_SAMPLE_NO; i++)
			printf("ADC sample %"PRIu32" %"PRIu32" \n", i, buf[i]);
	}
#endif

	printf("Success\n\r");
}
