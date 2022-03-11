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
#include "no_os_pwm.h"
#include "axi_pwm_extra.h"
#include "ad469x.h"
#include "no_os_error.h"
#include "clk_axi_clkgen.h"
#include "no_os_gpio.h"
#include "gpio_extra.h"
#include "parameters.h"

#ifdef IIO_SUPPORT
#include "iio.h"
#include "iio_app.h"
#include "iio_types.h"
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
		return 0;

	ret = ad469x_exit_conversion_mode(dev);
	if (ret != 0)
		return ret;

	ret = ad469x_std_sequence_ch(dev, mask);
	if (ret != 0)
		return ret;

	ret = ad469x_set_channel_sequence(dev, AD469x_standard_seq);
	if (ret != 0)
		return ret;

	ret = ad469x_enter_conversion_mode(dev);
	if (ret != 0)
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

#define IIO_AD469X_CH(_ch) {\
	.ch_type = IIO_VOLTAGE,\
	.channel = (_ch),\
	.scan_index = (_ch),\
	.scan_type = &ad469x_scan_type,\
	.indexed = true,\
}

static struct iio_channel ad469x_iio_channels[] = {
	IIO_AD469X_CH(0),
	IIO_AD469X_CH(1)
};

struct iio_device ad469x_iio_descriptor = {
	.num_ch = 2,
	.channels = ad469x_iio_channels,
	.pre_enable = (int32_t (*)(void *, uint32_t))iio_ad469x_prepare_conversion,
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

	struct no_os_pwm_init_param trigger_pwm_init = {
		.period_ns = 1000,	/* 1Mhz */
		.duty_cycle_ns = 10,
		.polarity = NO_OS_PWM_POLARITY_HIGH,
		.extra = &axi_pwm_init,
	};

	struct xil_gpio_init_param gpio_extra_param = {
		.device_id = GPIO_DEVICE_ID,
		.type = GPIO_PS,
	};

	struct no_os_gpio_init_param ad469x_resetn = {
		.number = GPIO_RESETN_1,
		.platform_ops = &xil_gpio_ops,
		.extra = &gpio_extra_param
	};

	struct no_os_spi_init_param spi_init = {
		.chip_select = AD469x_SPI_CS,
		.max_speed_hz = 80000000,
		.mode = NO_OS_SPI_MODE_3,
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

	struct iio_data_buffer read_buff = {
		.buff = (void *)buf,
		.size = sizeof(buf)
	};

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("ad469x", dev, &ad469x_iio_descriptor,
			       &read_buff, NULL),
	};

	return iio_app_run(devices, NO_OS_ARRAY_SIZE(devices));

#endif // IIO_SUPPORT

#ifdef ADVANCED_SEQ
	ret = ad469x_adv_sequence_set_num_slots(dev, 2);
	if (ret != 0)
		return ret;

	ret = ad469x_adv_sequence_set_slot(dev, 0, 0);
	if (ret != 0)
		return ret;

	ret = ad469x_adv_sequence_set_slot(dev, 1, 1);
	if (ret != 0)
		return ret;

	ret = ad469x_adv_seq_osr(dev, 0, AD469x_OSR_4);
	if (ret != 0)
		return ret;

	ret = ad469x_adv_seq_osr(dev, 1, AD469x_OSR_16);
	if (ret != 0)
		return ret;

	ret = ad469x_sequence_enable_temp(dev);
	if (ret != 0)
		return ret;

	ret = ad469x_set_channel_sequence(dev, AD469x_advanced_seq);
	if (ret != 0)
		return ret;

	ret = ad469x_enter_conversion_mode(dev);
	if (ret != 0)
		return ret;

	while (1) {
		ret = ad469x_seq_read_data(dev, buf, AD469x_EVB_SAMPLE_NO);
		if (ret != 0)
			return ret;

		for (i = 0; i < AD469x_EVB_SAMPLE_NO * 3; i += 3)
			printf("ADC_sample:%"PRIu32",ch0:%"PRIu32",ch1:%"PRIu32",temp:%"PRIu32"\n",
			       i, buf[i], buf[i + 1], buf[i + 2]);
	}
#elif defined(STANDARD_SEQ)
	ret = ad469x_std_sequence_ch(dev, AD469x_CHANNEL(1) | AD469x_CHANNEL(0));
	if (ret != 0)
		return ret;

	ret = ad469x_sequence_disable_temp(dev);
	if (ret != 0)
		return ret;

	ret = ad469x_set_channel_sequence(dev, AD469x_standard_seq);
	if (ret != 0)
		return ret;

	ret = ad469x_std_seq_osr(dev, AD469x_OSR_4);
	if (ret != 0)
		return ret;

	ret = ad469x_enter_conversion_mode(dev);
	if (ret != 0)
		return ret;

	while (1) {
		ret = ad469x_seq_read_data(dev, &buf[0], AD469x_EVB_SAMPLE_NO);
		if (ret != 0)
			return ret;

		for (i = 0; i < AD469x_EVB_SAMPLE_NO * 2; i += 2)
			printf("ADC_sample:%"PRIu32",ch0:%"PRIu32",ch1:%"PRIu32"\n", i, buf[i],
			       buf[i + 1]);
	}
#else
	ret = ad469x_set_channel_sequence(dev, AD469x_single_cycle);
	if (ret != 0)
		return ret;

	ret = ad469x_enter_conversion_mode(dev);
	if (ret != 0)
		return ret;

	while (1) {
		ch = j % 2;
		j++;
		ret = ad469x_read_data(dev, ch, buf, AD469x_EVB_SAMPLE_NO);
		printf("ch: %"PRIu32"\n", ch);
		if (ret != 0)
			return ret;

		for (i = 0; i < AD469x_EVB_SAMPLE_NO; i++)
			printf("ADC sample %"PRIu32" %"PRIu32" \n", i, buf[i]);
	}
#endif

	printf("Success\n\r");
}
