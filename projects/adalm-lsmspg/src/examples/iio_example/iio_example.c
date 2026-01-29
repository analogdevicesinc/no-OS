/***************************************************************************//**
 *   @file   iio_example.c
 *   @author Niel Acuna (niel.acuna@analog.com)
 *   @author Marc Paolo Sosa (MarcPaolo.Sosa@analog.com)
********************************************************************************
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
#include <assert.h>
#include "example.h"
#include "common_data.h"
#include "iio_ad559xr.h"
#include "iio_lm75.h"

#define DEFINE_AD559XR_IP(_spi_init, _ss_init, _i2c_init) { \
	.int_ref = true, \
	.spi_init = _spi_init, \
	.i2c_init = _i2c_init, \
	.ss_init = _ss_init, \
	.channel_modes = { \
		CH_MODE_DAC,         /* channel 0 */ \
		CH_MODE_ADC,         /* channel 1 */ \
		CH_MODE_DAC_AND_ADC, /* channel 2 */ \
		CH_MODE_DAC_AND_ADC, /* channel 3 */ \
		CH_MODE_GPI, 	     /* channel 4 */ \
		CH_MODE_GPO,         /* channel 5 */ \
		CH_MODE_GPI,         /* channel 6 */ \
		CH_MODE_GPO,         /* channel 7 */ \
	}, \
	.channel_offstate = { \
		CH_OFFSTATE_OUT_TRISTATE, /* channel 0 */ \
		CH_OFFSTATE_OUT_TRISTATE, /* channel 1 */ \
		CH_OFFSTATE_OUT_TRISTATE, /* channel 2 */ \
		CH_OFFSTATE_OUT_TRISTATE, /* channel 3 */ \
		CH_OFFSTATE_PULLDOWN,     /* channel 4 */ \
		CH_OFFSTATE_OUT_LOW,      /* channel 5 */ \
		CH_OFFSTATE_PULLDOWN,     /* channel 6 */ \
		CH_OFFSTATE_OUT_LOW,      /* channel 7 */ \
	}, \
	.adc_range = ZERO_TO_VREF, \
	.dac_range = ZERO_TO_VREF, \
	.adc_buf = false, \
}

/* 2 bytes per output channel.
 * and the hardware configuration is we have 1xadc + 2xaddac: 3 * 2 = 6 bytes
 * per scan line.
 * the IIO driver supports maximum of AD5599XR_MAX_SAMPLE_SIZE (1024) samples:
 * 6 * 1024 = 6,144 bytes * 2 chips = 12,288 RAM reserved for buffers */
static uint8_t ad5592r_read_buf[6 * AD5599XR_MAX_SAMPLE_SIZE];
static uint8_t ad5593r_read_buf[6 * AD5599XR_MAX_SAMPLE_SIZE];

static struct no_os_gpio_desc *led_gpio_desc;

enum led_state {
	led_state_off,
	led_state_pulse_1,
	led_state_pause,
	led_state_pulse_2,
};

static void lsmspg_heartbeat(void *ctx)
{
	static int ctr = 0;
	static enum led_state s = led_state_off;
	int value;

	switch (s) {
	case led_state_off:
		value = NO_OS_GPIO_LOW;
		if (++ctr == 120) {
			s = led_state_pulse_1;
			ctr = 0;
		}
		break;
	case led_state_pulse_1:
		value = NO_OS_GPIO_HIGH;
		if (++ctr == 2) {
			s = led_state_pause;
			ctr = 0;
		}
		break;

	case led_state_pause:
		value = NO_OS_GPIO_LOW;
		if (++ctr == 12) {
			s = led_state_pulse_2;
			ctr = 0;
		}
		break;

	case led_state_pulse_2:
		value = NO_OS_GPIO_HIGH;
		if (++ctr == 2) {
			s = led_state_off;
			ctr = 0;
		}
		break;
	}

	no_os_gpio_set_value(led_gpio_desc, value);

	return;
}

static int led_heartbeat_init(void)
{
	int err;

	/* initialize the GPIO */
	err = no_os_gpio_get(&led_gpio_desc, &led_gpio_ip);
	if (err)
		return err;

	err = no_os_gpio_direction_output(led_gpio_desc, NO_OS_GPIO_LOW);
	if (err)
		goto err_gpio_put;

	/* initialize the heartbeat system */
	err = heartbeat_init(lsmspg_heartbeat);
	if (err)
		goto err_gpio_put;

	return 0;

err_gpio_put:
	no_os_gpio_remove(led_gpio_desc);
	return err;
}

static void led_heartbeat_exit(void)
{
	/* deinit the heartbeat system */
	heartbeat_exit();

	/* deinit the GPIO */
	no_os_gpio_remove(led_gpio_desc);

	return;
}

static int init_iio_context(struct iio_app_init_param *iio_app_ip)
{
	if (!iio_app_ip)
		return -EINVAL;

	static struct iio_ctx_attr adalm_lsmspg_ctx_attrs[] = {
		{ .name = "fw_version", .value = "1.0.0" },
		{ .name = "fw_carrier", .value = "max32666_fthr" },
		{ .name = "hw_mezzanine", .value = "0000" },
		{ .name = "hw_name", .value = "adalm-lsmspg" },
		{ .name = "hw_mezzanine_status", .value = "detected" },
	};
	iio_app_ip->ctx_attrs = adalm_lsmspg_ctx_attrs;
	iio_app_ip->nb_ctx_attr = sizeof adalm_lsmspg_ctx_attrs / sizeof(
					  struct iio_ctx_attr);
	return 0;
}

int iio_example(void)
{
	int err;
	struct ad5592r_dev *ad5592r;
	struct ad5592r_dev *ad5593r;
	struct iio_ad559xr_desc *iio_ad5592r;
	struct iio_ad559xr_desc *iio_ad5593r;
	struct iio_ad559xr_desc *iio_ad5592r_1bit_adcdac;
	struct iio_app_desc *iio_app;
	struct iio_app_init_param iio_app_ip = {0};
	struct iio_app_device adalm_lsmspg_devices[4] = {0};
	struct iio_data_buffer ad5592r_read_data_buffer;
	struct iio_data_buffer ad5593r_read_data_buffer;
	struct ad5592r_init_param ad559xr_ip = DEFINE_AD559XR_IP(&ad5592r_spi_ip,
					       &ad5592r_spi_ss_ip, &ad5593r_i2c_ip);
	struct lm75_init_param lm75_ip = {
		.fault_count = 0, /* POR state */
		.os_polarity = lm75_os_active_low,
		.i2c_ip = &lm75_i2c_ip,
	};
	struct lm75_dev *lm75;
	struct iio_lm75 *iio_lm75;

	err = led_heartbeat_init();
	if (err)
		return err;

	err = ad5592r_init(&ad5592r, &ad559xr_ip);
	if (err)
		goto err_led_heartbeat_exit;

	err = iio_ad559xr_aio_init(&iio_ad5592r, ad5592r);
	if (err)
		goto err_teardown_ad5592r;

	err = ad5593r_init(&ad5593r, &ad559xr_ip);
	if (err)
		goto err_teardown_ad5592r_iio;

	err = iio_ad559xr_aio_init(&iio_ad5593r, ad5593r);
	if (err)
		goto err_teardown_ad5593r;

	/* developer notes:
	 * the gpio map here models 1:1 with the rpi-adalm-lsmspg-overlay.dts
	 * to make the curve tracer scripts compatible across RPI and
	 * Max326xx FTHR boards.
	 */
	struct iio_ad559xr_gpio_map gpio_map[2] = {
		{ .ad559xr = ad5592r, .map = {0, 0, 0, 0, 0, 0, 1, 1}, },
		{ .ad559xr = ad5593r, .map = {0, 0, 0, 0, 2, 2, 3, 3}, },
	};
	err = iio_ad559xr_gpio_init(&iio_ad5592r_1bit_adcdac, gpio_map, 2);
	if (err)
		goto err_teardown_ad5593r_iio;

	err = lm75_init(&lm75, &lm75_ip);
	if (err)
		goto err_teardown_ad559xr_gpio;

	err = iio_lm75_init(&iio_lm75, lm75);
	if (err)
		goto err_teardown_lm75;

	ad5592r_read_data_buffer.size = sizeof ad5592r_read_buf;
	ad5592r_read_data_buffer.buff = ad5592r_read_buf;
	ad5593r_read_data_buffer.size = sizeof ad5593r_read_buf;
	ad5593r_read_data_buffer.buff = ad5593r_read_buf;

	adalm_lsmspg_devices[0].name = "lm75";
	adalm_lsmspg_devices[0].dev = iio_lm75;
	adalm_lsmspg_devices[0].dev_descriptor = &iio_lm75->iio_dev;
	adalm_lsmspg_devices[1].name = "ad5592r";
	adalm_lsmspg_devices[1].dev = iio_ad5592r;
	adalm_lsmspg_devices[1].dev_descriptor = &iio_ad5592r->iiodev;
	adalm_lsmspg_devices[1].read_buff = &ad5592r_read_data_buffer;
	adalm_lsmspg_devices[2].name = "ad5593r";
	adalm_lsmspg_devices[2].dev = iio_ad5593r;
	adalm_lsmspg_devices[2].dev_descriptor = &iio_ad5593r->iiodev;
	adalm_lsmspg_devices[2].read_buff = &ad5593r_read_data_buffer;
	adalm_lsmspg_devices[3].name = "one-bit-adc-dac";
	adalm_lsmspg_devices[3].dev = iio_ad5592r_1bit_adcdac;
	adalm_lsmspg_devices[3].dev_descriptor = &iio_ad5592r_1bit_adcdac->iiodev;

	iio_app_ip.devices = adalm_lsmspg_devices;
	iio_app_ip.nb_devices = 4;
	iio_app_ip.uart_init_params = uart_ip;
	err = init_iio_context(&iio_app_ip);
	if (err)
		goto err_teardown_lm75_iio;

	err = iio_app_init(&iio_app, iio_app_ip);
	if (err)
		goto err_teardown_lm75_iio;

	err = iio_app_run(iio_app);

	iio_app_remove(iio_app);
err_teardown_lm75_iio:
	iio_lm75_remove(iio_lm75);
err_teardown_lm75:
	lm75_remove(lm75);
err_teardown_ad559xr_gpio:
	iio_ad559xr_remove(iio_ad5592r_1bit_adcdac);
err_teardown_ad5593r_iio:
	iio_ad559xr_remove(iio_ad5593r);
err_teardown_ad5593r:
	ad5593r_remove(ad5593r);
err_teardown_ad5592r_iio:
	iio_ad559xr_remove(iio_ad5592r);
err_teardown_ad5592r:
	ad5592r_remove(ad5592r);
err_led_heartbeat_exit:
	led_heartbeat_exit();
	return err;
}
example_main("iio_example");
