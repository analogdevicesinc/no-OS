/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  Implementation of IIO example for iio_adpd1080 project.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

#include <stdlib.h>
#include "common_data.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_gpio.h"
#include "no_os_irq.h"
#include "no_os_timer.h"
#include "iio_adpd188.h"
#include "iio_app.h"

#define MAX_SIZE_BASE_ADDR		1024

static uint8_t in_buff[MAX_SIZE_BASE_ADDR];

#define ADC_DDR_BASEADDR	((uint32_t)in_buff)

static void adpd1080_sync_gpio_cb(void *ctx)
{
	int32_t *cnt = ctx;

	*cnt += 1;
}

static int32_t adpd1080pmod_32k_calib(struct adpd188_dev *adpd1080_dev)
{
	int32_t status;
	uint16_t temp_reg;
	uint32_t t_start, t_stop = 0;
	volatile int32_t sync_gpio_pulse_no;
	int32_t min_diff = 0x7fffffff;
	struct no_os_timer_desc *cal_timer;
	struct no_os_gpio_desc *sync_gpio;
	struct no_os_irq_ctrl_desc *cal_irq;

	status = no_os_timer_init(&cal_timer, &adpd1080_cal_timer_ip);
	if (status != 0)
		return -1;
	status = no_os_timer_start(cal_timer);
	if (status != 0)
		goto timer_finish;

	status = no_os_gpio_get(&sync_gpio, &adpd1080_sync_gpio_ip);
	if (status != 0)
		goto timer_finish;
	status = no_os_gpio_direction_input(sync_gpio);
	if (status != 0)
		goto gpio_finish;

	status = no_os_irq_ctrl_init(&cal_irq, &adpd1080_cal_irq_ip);
	if (status != 0)
		goto gpio_finish;

	struct no_os_callback_desc sync_gpio_cb = {
		.callback = adpd1080_sync_gpio_cb,
		.ctx = &sync_gpio_pulse_no,
		.event = NO_OS_EVT_GPIO,
		.handle = cal_irq->extra,
		.peripheral = NO_OS_GPIO_IRQ
	};
	status = no_os_irq_register_callback(cal_irq, ADPD1080_SYNC_GPIO_NUM,
					     &sync_gpio_cb);
	if (status != 0)
		goto gpio_finish;

	status = adpd188_adc_fsample_set(adpd1080_dev, 1000);
	if (status != 0)
		goto finish;

	status = adpd188_reg_write(adpd1080_dev, ADPD188_REG_GPIO_DRV,
				   ADPD188_GPIO_DRV_GPIO0_ENA_MASK);
	if (status != 0)
		goto finish;

	status = adpd188_reg_write(adpd1080_dev, ADPD188_REG_GPIO_CTRL, 2);
	if (status != 0)
		goto finish;

	status = adpd188_mode_set(adpd1080_dev, ADPD188_NORMAL);
	if (status != 0)
		goto finish;

	status = no_os_irq_enable(cal_irq, ADPD1080_SYNC_GPIO_NUM);
	if (status != 0)
		goto finish;

	while (true) {
		status = adpd188_reg_read(adpd1080_dev, ADPD188_REG_SAMPLE_CLK, &temp_reg);
		if (status != 0)
			goto finish;
		status = no_os_timer_counter_get(cal_timer, &t_start);
		if (status != 0)
			goto finish;
		sync_gpio_pulse_no = 0;
		while (t_start >= t_stop) {
			status = no_os_timer_counter_get(cal_timer, &t_stop);
			if (status != 0)
				goto finish;
		}
		if (sync_gpio_pulse_no < 1000)
			temp_reg -= 1;
		else if (sync_gpio_pulse_no > 1000)
			temp_reg += 1;
		if (abs(sync_gpio_pulse_no - 1000) >= min_diff)
			break;
		else
			min_diff = abs(sync_gpio_pulse_no - 1000);
		status = adpd188_reg_write(adpd1080_dev, ADPD188_REG_SAMPLE_CLK, temp_reg);
		if (status != 0)
			goto finish;
	}

	status = no_os_irq_disable(cal_irq, ADPD1080_SYNC_GPIO_NUM);
	if (status != 0)
		goto finish;

	status = no_os_irq_unregister_callback(cal_irq, ADPD1080_SYNC_GPIO_NUM,
					       &sync_gpio_cb);
	if (status != 0)
		goto gpio_finish;

	status = adpd188_mode_set(adpd1080_dev, ADPD188_PROGRAM);
	if (status != 0)
		goto finish;

	status = adpd188_adc_fsample_set(adpd1080_dev, 16);
	if (status != 0)
		goto finish;

	status = adpd188_reg_write(adpd1080_dev, ADPD188_REG_GPIO_DRV, 0);
	if (status != 0)
		goto finish;

	status = adpd188_reg_write(adpd1080_dev, ADPD188_REG_GPIO_CTRL, 0);
	if (status != 0)
		goto finish;

finish:
	no_os_irq_ctrl_remove(cal_irq);
gpio_finish:
	no_os_gpio_remove(sync_gpio);
timer_finish:
	no_os_timer_remove(cal_timer);

	return status;
}

/***************************************************************************//**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
*******************************************************************************/
int example_main()
{
	int32_t status;
	uint16_t reg_data;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	struct adpd188_iio_desc *adpd1080_iio_device;
	struct iio_data_buffer iio_adpd1080_read_buff = {
		.buff = (void *)ADC_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR,
	};

	status = adpd188_iio_init(&adpd1080_iio_device, &adpd1080_iio_ip);
	if (status < 0)
		return status;

	status = adpd188_adc_fsample_set(adpd1080_iio_device->drv_dev, 16);
	if (status != 0)
		return -1;

	status = adpd188_reg_read(adpd1080_iio_device->drv_dev, ADPD188_REG_SLOT_EN,
				  &reg_data);
	if (status != 0)
		return -1;
	reg_data |= ADPD188_SLOT_EN_RDOUT_MODE_MASK |
		    ADPD188_SLOT_EN_FIFO_OVRN_PREVENT_MASK;
	status = adpd188_reg_write(adpd1080_iio_device->drv_dev, ADPD188_REG_SLOT_EN,
				   reg_data);
	if (status != 0)
		return -1;

	/* Slot A chop mode is inverted, non-inverted, non-inverted, inverted */
	status = adpd188_reg_read(adpd1080_iio_device->drv_dev, ADPD188_REG_INT_SEQ_A,
				  &reg_data);
	if (status != 0)
		return -1;
	reg_data |= 0x9 & ADPD188_INT_SEQ_A_INTEG_ORDER_A_MASK;
	status = adpd188_reg_write(adpd1080_iio_device->drv_dev, ADPD188_REG_INT_SEQ_A,
				   reg_data);
	if (status != 0)
		return -1;
	/* Slot B chop mode is inverted, non-inverted, non-inverted, inverted */
	status = adpd188_reg_read(adpd1080_iio_device->drv_dev, ADPD188_REG_INT_SEQ_B,
				  &reg_data);
	if (status != 0)
		return -1;
	reg_data |= 0x9 & ADPD188_INT_SEQ_B_INTEG_ORDER_B_MASK;
	status = adpd188_reg_write(adpd1080_iio_device->drv_dev, ADPD188_REG_INT_SEQ_B,
				   reg_data);
	if (status != 0)
		return -1;

	/* Set blue LED 1 power */
	status = adpd188_reg_read(adpd1080_iio_device->drv_dev,
				  ADPD188_REG_ILED1_COARSE, &reg_data);
	if (status != 0)
		return -1;
	reg_data &= ~ADPD188_ILED1_COARSE_ILED1_COARSE_MASK;
	reg_data |= (0x6 << ADPD188_ILED1_COARSE_ILED1_COARSE_POS) &
		    ADPD188_ILED1_COARSE_ILED1_COARSE_MASK;
	reg_data &= ~ADPD188_ILED1_COARSE_ILED1_SLEW_MASK;
	reg_data |= (0x3 << ADPD188_ILED1_COARSE_ILED1_SLEW_POS) &
		    ADPD188_ILED1_COARSE_ILED1_SLEW_MASK;
	reg_data |= ADPD188_ILED1_COARSE_ILED1_SCALE_MASK;
	status = adpd188_reg_write(adpd1080_iio_device->drv_dev,
				   ADPD188_REG_ILED1_COARSE, reg_data);
	if (status != 0)
		return -1;

	/* Slot A 4 LED pulses with 15us period */
	status = adpd188_reg_read(adpd1080_iio_device->drv_dev,
				  ADPD188_REG_SLOTA_NUMPULSES, &reg_data);
	if (status != 0)
		return -1;
	reg_data &= ~ADPD188_SLOTA_NUMPULSES_SLOTA_PULSES_MASK;
	reg_data |= (0x4 << ADPD188_SLOTA_NUMPULSES_SLOTA_PULSES_POS) &
		    ADPD188_SLOTA_NUMPULSES_SLOTA_PULSES_MASK;
	reg_data &= ~ADPD188_SLOTA_NUMPULSES_SLOTA_PERIOD_MASK;
	reg_data |= (0xE << ADPD188_SLOTA_NUMPULSES_SLOTA_PERIOD_POS) &
		    ADPD188_SLOTA_NUMPULSES_SLOTA_PERIOD_MASK;
	status = adpd188_reg_write(adpd1080_iio_device->drv_dev,
				   ADPD188_REG_SLOTA_NUMPULSES, reg_data);
	if (status != 0)
		return -1;

	/* Slot B 4 LED pulses with 15us period */
	status = adpd188_reg_read(adpd1080_iio_device->drv_dev,
				  ADPD188_REG_SLOTB_NUMPULSES, &reg_data);
	if (status != 0)
		return -1;
	reg_data &= ~ADPD188_SLOTB_NUMPULSES_SLOTB_PULSES_MASK;
	reg_data |= (0x4 << ADPD188_SLOTB_NUMPULSES_SLOTB_PULSES_POS) &
		    ADPD188_SLOTB_NUMPULSES_SLOTB_PULSES_MASK;
	reg_data &= ~ADPD188_SLOTB_NUMPULSES_SLOTB_PERIOD_MASK;
	reg_data |= (0xE << ADPD188_SLOTB_NUMPULSES_SLOTB_PERIOD_POS) &
		    ADPD188_SLOTB_NUMPULSES_SLOTB_PERIOD_MASK;
	status = adpd188_reg_write(adpd1080_iio_device->drv_dev,
				   ADPD188_REG_SLOTB_NUMPULSES, reg_data);
	if (status != 0)
		return -1;

	/* Slot A integrator window */
	status = adpd188_reg_read(adpd1080_iio_device->drv_dev,
				  ADPD188_REG_SLOTA_AFE_WINDOW, &reg_data);
	if (status != 0)
		return -1;
	reg_data &= ~ADPD188_SLOTA_AFE_WINDOW_SLOTA_AFE_WIDTH_MASK;
	reg_data |= (0x4 << ADPD188_SLOTA_AFE_WINDOW_SLOTA_AFE_WIDTH_POS) &
		    ADPD188_SLOTA_AFE_WINDOW_SLOTA_AFE_WIDTH_MASK;
	reg_data &= ~ADPD188_SLOTA_AFE_WINDOW_SLOTA_AFE_OFFSET_MASK;
	reg_data |= (0x2F0 << ADPD188_SLOTA_AFE_WINDOW_SLOTA_AFE_OFFSET_POS) &
		    ADPD188_SLOTA_AFE_WINDOW_SLOTA_AFE_OFFSET_MASK;
	status = adpd188_reg_write(adpd1080_iio_device->drv_dev,
				   ADPD188_REG_SLOTA_AFE_WINDOW, reg_data);
	if (status != 0)
		return -1;

	/* Slot B integrator window */
	status = adpd188_reg_read(adpd1080_iio_device->drv_dev,
				  ADPD188_REG_SLOTB_AFE_WINDOW, &reg_data);
	if (status != 0)
		return -1;
	reg_data &= ~ADPD188_SLOTB_AFE_WINDOW_SLOTB_AFE_WIDTH_MASK;
	reg_data |= (0x4 << ADPD188_SLOTB_AFE_WINDOW_SLOTB_AFE_WIDTH_POS) &
		    ADPD188_SLOTB_AFE_WINDOW_SLOTB_AFE_WIDTH_MASK;
	reg_data &= ~ADPD188_SLOTB_AFE_WINDOW_SLOTB_AFE_OFFSET_MASK;
	reg_data |= (0x2F0 << ADPD188_SLOTB_AFE_WINDOW_SLOTB_AFE_OFFSET_POS) &
		    ADPD188_SLOTB_AFE_WINDOW_SLOTB_AFE_OFFSET_MASK;
	status = adpd188_reg_write(adpd1080_iio_device->drv_dev,
				   ADPD188_REG_SLOTB_AFE_WINDOW, reg_data);
	if (status != 0)
		return -1;

	/* Math for chop mode is inverted, non-inverted, non-inverted, inverted */
	status = adpd188_reg_read(adpd1080_iio_device->drv_dev, ADPD188_REG_MATH,
				  &reg_data);
	if (status != 0)
		return -1;
	reg_data &= ~ADPD188_MATH_FLT_MATH34_B_MASK;
	reg_data |= (0x01 << ADPD188_MATH_FLT_MATH34_B_POS) &
		    ADPD188_MATH_FLT_MATH34_B_MASK;
	reg_data &= ~ADPD188_MATH_FLT_MATH34_A_MASK;
	reg_data |= (0x01 << ADPD188_MATH_FLT_MATH34_A_POS) &
		    ADPD188_MATH_FLT_MATH34_A_MASK;
	reg_data &= ~ADPD188_MATH_FLT_MATH12_B_MASK;
	reg_data |= (0x02 << ADPD188_MATH_FLT_MATH12_B_POS) &
		    ADPD188_MATH_FLT_MATH12_B_MASK;
	reg_data &= ~ADPD188_MATH_FLT_MATH12_A_MASK;
	reg_data |= (0x02 << ADPD188_MATH_FLT_MATH12_A_POS) &
		    ADPD188_MATH_FLT_MATH12_A_MASK;

	status = adpd188_reg_write(adpd1080_iio_device->drv_dev, ADPD188_REG_MATH,
				   reg_data);
	if (status != 0)
		return -1;

	status = adpd1080pmod_32k_calib(adpd1080_iio_device->drv_dev);
	if (status < 0)
		return status;

	status = adpd188_clk32mhz_cal(adpd1080_iio_device->drv_dev);
	if (status != 0)
		return -1;

	status = adpd188_adc_fsample_set(adpd1080_iio_device->drv_dev, 512.0);
	if (status != 0)
		return -1;

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("adpd1080", adpd1080_iio_device, &iio_adpd188_device,
			       &iio_adpd1080_read_buff, NULL, NULL)
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = adpd1080_uart_ip;
#ifdef NO_OS_NETWORKING
	app_init_param.wifi_ssid = WIFI_SSID;
	app_init_param.wifi_pwd = WIFI_PWD;
#endif

	status = iio_app_init(&app, app_init_param);
	if (status)
		return status;

	return iio_app_run(app);
}
