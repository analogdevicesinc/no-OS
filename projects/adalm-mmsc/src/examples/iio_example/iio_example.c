/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  Implementation of iio_example.c.
 *   @author Niel Acuna (niel.acuna@analog.com)
 *   @author Marc Paolo Sosa (MarcPaolo.Sosa@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
*******************************************************************************/
#include <stdio.h>
#include <assert.h>
#include "no_os_timer.h"
#include "common_data.h"
#include "iio_example.h"
#include "iio_ad4080.h"
#include "iio_app.h"

static struct ad4080_init_param default_ad4080_init_param = {
	.cfg = {
		.spi = &cfg_spi_init_param,
		.ss = &cfg_ss_init_param,
	},

	.data = {
		.spi = &data_spi_init_param,
		.ss = &data_ss_init_param,
	},
	.spi3wire = false,
	.addr_asc = AD4080_ADDR_INCR,
	.single_instr = AD4080_SINGLE_INST,
	.short_instr = AD4080_15_BIT_ADDR,
	.strict_reg = AD4080_REG_NORMAL_MODE,
	.intf_chk_en = AD4080_DATA,
	.cnv_spi_lvds_lanes = AD4080_ONE_LANE,
	.conv_data_spi_lvds = AD4080_CONV_DATA_SPI,
	.lvds_cnv_clk_cnt = 0,
	.lvds_self_clk_mode = AD4080_SELF_CLK_MODE,
	.cnv_clk_mode = AD4080_CNV_CMOS_MODE,
	.lvds_vod = AD4080_185mVPP,
	.ana_dig_ldo_pd = AD4080_AD_LDO_EN,
	.intf_ldo_pd = AD4080_INTF_LDO_EN,
	.fifo_mode = AD4080_FIFO_DISABLE,
	.op_mode = AD4080_OP_NORMAL,
};

static struct no_os_gpio_desc *gate_ctrl_q1 = NULL;
static struct no_os_gpio_desc *gate_ctrl_q2 = NULL;
static struct no_os_gpio_desc *gate_ctrl_q3 = NULL;
static struct no_os_gpio_desc *gate_ctrl_q4 = NULL;

static void heartbeat(void *ctx);
extern struct no_os_gpio_desc *led_ctrl;

struct no_os_callback_desc heartbeat_callback = {
	.callback = heartbeat,
	.event = NO_OS_EVT_TIM_ELAPSED,
	.peripheral = NO_OS_TIM_IRQ,
};

static int clock_gen_init(void)
{
	int err;

	/* pull down gate ctrl q1 */
	err = no_os_gpio_get(&gate_ctrl_q1, &gate_ctrl_q1_init_param);
	if (err)
		return err;
	no_os_gpio_direction_output(gate_ctrl_q1, NO_OS_GPIO_LOW);

	/* pull down gate ctrl q2 */
	err = no_os_gpio_get(&gate_ctrl_q2, &gate_ctrl_q2_init_param);
	if (err)
		goto err_q2;
	no_os_gpio_direction_output(gate_ctrl_q2, NO_OS_GPIO_LOW);

	/* pull down gate ctrl q3 */
	err = no_os_gpio_get(&gate_ctrl_q3, &gate_ctrl_q3_init_param);
	if (err)
		goto err_q3;
	no_os_gpio_direction_output(gate_ctrl_q3, NO_OS_GPIO_LOW);

	/* pull down gate ctrl q4 */
	err = no_os_gpio_get(&gate_ctrl_q4, &gate_ctrl_q4_init_param);
	if (err)
		goto err_q4;
	no_os_gpio_direction_output(gate_ctrl_q4, NO_OS_GPIO_LOW);

	return 0;

err_q4:
	no_os_gpio_remove(gate_ctrl_q3);
err_q3:
	no_os_gpio_remove(gate_ctrl_q2);
err_q2:
	no_os_gpio_remove(gate_ctrl_q1);
	return err;
}

static void clock_gen_fini(void)
{
	no_os_gpio_remove(gate_ctrl_q4);
	no_os_gpio_remove(gate_ctrl_q3);
	no_os_gpio_remove(gate_ctrl_q2);
	no_os_gpio_remove(gate_ctrl_q1);
	return;
}

enum led_state {
	led_state_off,
	led_state_pulse_1,
	led_state_pause,
	led_state_pulse_2,
};

static void heartbeat(void *ctx)
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

	no_os_gpio_set_value(led_ctrl, value);

	return;
}

static uint8_t adc_buffer[AD4080_MAX_FIFO_WATERMARK * sizeof(uint32_t)];

static int init_iio_context(struct iio_app_init_param *iio_app_ip)
{
	assert(iio_app_ip);
	static struct iio_ctx_attr adalm_mmsc_iio_ctx[] = {
		{ .name = "fw_version", .value = "1.0.0", },
		{ .name = "fw_carrier", .value = "max32666_fthr", },
		{ .name = "hw_mezzanine", .value = "0000", },
		{ .name = "hw_name", .value = "adalm-mmsc", },
		{ .name = "hw_mezzanine_status", .value = "detected", },
	};
	iio_app_ip->ctx_attrs = adalm_mmsc_iio_ctx;
	iio_app_ip->nb_ctx_attr = sizeof adalm_mmsc_iio_ctx / sizeof(
					  struct iio_ctx_attr);
	return 0;
}

int iio_example_main(void)
{
	int err;
	struct iio_ad4080_init_param iio_ad4080_init_param = {
		.ad4080_init_param = &default_ad4080_init_param,
		.ff_full_init_param = &gp3_init_param,
		.afe_ctrl_init_param = &afe_ctrl_init_param,
		.i_gp = AD4080_GPIO_3,
		.watermark = AD4080_MAX_FIFO_WATERMARK,
	};
	struct iio_ad4080_desc *iio_ad4080;
	struct iio_app_desc *iio_app;
	struct iio_app_init_param iio_app_init_param = {0};
	struct ad4080_dev *ad4080 = NULL;
	struct iio_device single_ad4080_iio_device = {0};
	struct iio_data_buffer read_data_buffer;

	/* give the mezannine time to ramp up its power rails */
	no_os_mdelay(100);

	err = iio_ad4080_init(&iio_ad4080, &iio_ad4080_init_param);
	if (err)
		goto err_exit;

	/* now, pull down low all the clock generator control signals.
	 * we don't use them since the target board has a rotary switch
	 * to let the user control the conversion clock anyway. */
	err = clock_gen_init();
	if (err)
		goto err_clock_gen;

	read_data_buffer.size = sizeof adc_buffer;
	read_data_buffer.buff = adc_buffer;

	ad4080_device(iio_ad4080, &ad4080);
	ad4080_iio_device(iio_ad4080, &single_ad4080_iio_device);

	struct iio_app_device single_ad4080_iio_app_device =
		IIO_APP_DEVICE("ad4080",
			       ad4080,
			       &single_ad4080_iio_device,
			       &read_data_buffer,
			       NULL,
			       NULL);

	iio_app_init_param.devices = &single_ad4080_iio_app_device;
	iio_app_init_param.nb_devices = 1;
	iio_app_init_param.uart_init_params = uart_init_param;
	err = init_iio_context(&iio_app_init_param);
	if (err)
		goto err_iio_app_init;

	err = iio_app_init(&iio_app, iio_app_init_param);
	if (err)
		goto err_iio_app_init;

	err = iio_app_run(iio_app);

err_iio_app_init:
	clock_gen_fini();
err_clock_gen:
	iio_ad4080_fini(iio_ad4080);
err_exit:
	return err;
}
