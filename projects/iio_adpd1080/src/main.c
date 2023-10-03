/***************************************************************************//**
 *   @file   iio_adpd1080/src/main.c
 *   @brief  Implementation of Main Function.
 *   @author Drimbarean Andrei (andrei.drimbarean@analog.com)
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

#include "app_config.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "iio_adpd188.h"
#include "iio_app.h"
#include "aducm3029_gpio.h"
#include "no_os_gpio.h"
#include "no_os_timer.h"
#include "aducm3029_irq.h"
#include "aducm3029_i2c.h"
#include "aducm3029_uart.h"
#include "aducm3029_gpio_irq.h"
#include "aducm3029_timer.h"
#include "parameters.h"

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
	struct no_os_timer_init_param cal_timer_init = {
		.id = 0,
		.ticks_count = 0,
		.freq_hz = 1,
		.platform_ops = &aducm_timer_ops,
		.extra = NULL
	};
	struct no_os_gpio_desc *sync_gpio;
	struct no_os_gpio_init_param sync_gpio_init = {
		.number = 0x0D,
		.extra = NULL,
		.platform_ops = &aducm_gpio_ops
	};
	struct no_os_irq_ctrl_desc *cal_irq;
	struct no_os_irq_init_param cal_irq_init = {
		.irq_ctrl_id = ADUCM_GPIO_B_GROUP_SOFT_CTRL,
		.platform_ops = &aducm_gpio_irq_ops,
		.extra = NULL
	};

	status = no_os_timer_init(&cal_timer, &cal_timer_init);
	if(status != 0)
		return -1;
	status = no_os_timer_start(cal_timer);
	if(status != 0)
		goto timer_finish;

	status = no_os_gpio_get(&sync_gpio, &sync_gpio_init);
	if(status != 0)
		goto timer_finish;
	status = no_os_gpio_direction_input(sync_gpio);
	if(status != 0)
		goto gpio_finish;

	status = no_os_irq_ctrl_init(&cal_irq, &cal_irq_init);
	if(status != 0)
		goto gpio_finish;

	struct no_os_callback_desc sync_gpio_cb = {
		.callback = adpd1080_sync_gpio_cb,
		.ctx = &sync_gpio_pulse_no,
		.event = NO_OS_EVT_GPIO,
		.handle = cal_irq->extra,
		.peripheral = NO_OS_GPIO_IRQ
	};
	status = no_os_irq_register_callback(cal_irq, 0x0D, &sync_gpio_cb);
	if(status != 0)
		goto gpio_finish;

	status = adpd188_adc_fsample_set(adpd1080_dev, 1000);
	if(status != 0)
		goto finish;

	status = adpd188_reg_write(adpd1080_dev, ADPD188_REG_GPIO_DRV,
				   ADPD188_GPIO_DRV_GPIO0_ENA_MASK);
	if(status != 0)
		goto finish;

	status = adpd188_reg_write(adpd1080_dev, ADPD188_REG_GPIO_CTRL, 2);
	if(status != 0)
		goto finish;

	status = adpd188_mode_set(adpd1080_dev, ADPD188_NORMAL);
	if (status != 0)
		goto finish;

	status = no_os_irq_enable(cal_irq, 0x0D);
	if(status != 0)
		goto finish;

	while (true) {
		status = adpd188_reg_read(adpd1080_dev, ADPD188_REG_SAMPLE_CLK, &temp_reg);
		if(status != 0)
			goto finish;
		status = no_os_timer_counter_get(cal_timer, &t_start);
		if(status != 0)
			goto finish;
		sync_gpio_pulse_no = 0;
		while (t_start >= t_stop) {
			status = no_os_timer_counter_get(cal_timer, &t_stop);
			if(status != 0)
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
		if(status != 0)
			goto finish;
	}

	status = no_os_irq_disable(cal_irq, 0x0D);
	if(status != 0)
		goto finish;

	status = no_os_irq_unregister_callback(cal_irq, 0x0D, &sync_gpio_cb);
	if(status != 0)
		goto gpio_finish;

	status = adpd188_mode_set(adpd1080_dev, ADPD188_PROGRAM);
	if (status != 0)
		goto finish;

	status = adpd188_adc_fsample_set(adpd1080_dev, 16);
	if(status != 0)
		goto finish;

	status = adpd188_reg_write(adpd1080_dev, ADPD188_REG_GPIO_DRV, 0);
	if(status != 0)
		goto finish;

	status = adpd188_reg_write(adpd1080_dev, ADPD188_REG_GPIO_CTRL, 0);
	if(status != 0)
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
 * @brief main
*******************************************************************************/
int main(void)
{
	int32_t status;
	uint16_t reg_data;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	struct no_os_uart_init_param adpd1080_uart_ip = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID,
		.asynchronous_rx = true,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = NULL,
		.platform_ops = &aducm_uart_ops,
	};

	status = platform_init();
	if (NO_OS_IS_ERR_VALUE(status))
		return status;

	struct adpd188_iio_desc *adpd1080_iio_device;
	struct adpd188_iio_init_param adpd1080_iio_inital = {
		.drv_init_param.device = ADPD1080,
		.drv_init_param.phy_opt = ADPD188_I2C,
		.drv_init_param.phy_init.i2c_phy =
		{
			.slave_address = 0x64,
			.max_speed_hz = 400000,
			.platform_ops = &aducm_i2c_ops,
			.extra = NULL
		},
		.drv_init_param.gpio0_init =
		{
			.number = 0,
			.platform_ops = &aducm_gpio_ops,
			.extra = 0
		},
		.drv_init_param.gpio1_init =
		{
			.number = 0,
			.platform_ops = &aducm_gpio_ops,
			.extra = 0
		}
	};
	struct iio_data_buffer iio_adpd1080_read_buff = {
		.buff = ADC_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR,
	};

	status = adpd188_iio_init(&adpd1080_iio_device, &adpd1080_iio_inital);
	if (status < 0)
		return status;

	status = adpd188_adc_fsample_set(adpd1080_iio_device->drv_dev, 16);
	if(status != 0)
		return -1;

	status = adpd188_reg_read(adpd1080_iio_device->drv_dev, ADPD188_REG_SLOT_EN,
				  &reg_data);
	if(status != 0)
		return -1;
	reg_data |= ADPD188_SLOT_EN_RDOUT_MODE_MASK |
		    ADPD188_SLOT_EN_FIFO_OVRN_PREVENT_MASK;
	status = adpd188_reg_write(adpd1080_iio_device->drv_dev, ADPD188_REG_SLOT_EN,
				   reg_data);
	if(status != 0)
		return -1;

	/* Slot A chop mode is inverted, non-inverted, non-inverted, inverted */
	status = adpd188_reg_read(adpd1080_iio_device->drv_dev, ADPD188_REG_INT_SEQ_A,
				  &reg_data);
	if(status != 0)
		return -1;
	reg_data |= 0x9 & ADPD188_INT_SEQ_A_INTEG_ORDER_A_MASK;
	status = adpd188_reg_write(adpd1080_iio_device->drv_dev, ADPD188_REG_INT_SEQ_A,
				   reg_data);
	if(status != 0)
		return -1;
	/* Slot B chop mode is inverted, non-inverted, non-inverted, inverted */
	status = adpd188_reg_read(adpd1080_iio_device->drv_dev, ADPD188_REG_INT_SEQ_B,
				  &reg_data);
	if(status != 0)
		return -1;
	reg_data |= 0x9 & ADPD188_INT_SEQ_B_INTEG_ORDER_B_MASK;
	status = adpd188_reg_write(adpd1080_iio_device->drv_dev, ADPD188_REG_INT_SEQ_B,
				   reg_data);
	if(status != 0)
		return -1;

	/* Set blue LED 1 power */
	status = adpd188_reg_read(adpd1080_iio_device->drv_dev,
				  ADPD188_REG_ILED1_COARSE, &reg_data);
	if(status != 0)
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
	if(status != 0)
		return -1;

	/* Slot A 4 LED pulses with 15us period */
	status = adpd188_reg_read(adpd1080_iio_device->drv_dev,
				  ADPD188_REG_SLOTA_NUMPULSES, &reg_data);
	if(status != 0)
		return -1;
	reg_data &= ~ADPD188_SLOTA_NUMPULSES_SLOTA_PULSES_MASK;
	reg_data |= (0x4 << ADPD188_SLOTA_NUMPULSES_SLOTA_PULSES_POS) &
		    ADPD188_SLOTA_NUMPULSES_SLOTA_PULSES_MASK;
	reg_data &= ~ADPD188_SLOTA_NUMPULSES_SLOTA_PERIOD_MASK;
	reg_data |= (0xE << ADPD188_SLOTA_NUMPULSES_SLOTA_PERIOD_POS) &
		    ADPD188_SLOTA_NUMPULSES_SLOTA_PERIOD_MASK;
	status = adpd188_reg_write(adpd1080_iio_device->drv_dev,
				   ADPD188_REG_SLOTA_NUMPULSES, reg_data);
	if(status != 0)
		return -1;

	/* Slot B 4 LED pulses with 15us period */
	status = adpd188_reg_read(adpd1080_iio_device->drv_dev,
				  ADPD188_REG_SLOTB_NUMPULSES, &reg_data);
	if(status != 0)
		return -1;
	reg_data &= ~ADPD188_SLOTB_NUMPULSES_SLOTB_PULSES_MASK;
	reg_data |= (0x4 << ADPD188_SLOTB_NUMPULSES_SLOTB_PULSES_POS) &
		    ADPD188_SLOTB_NUMPULSES_SLOTB_PULSES_MASK;
	reg_data &= ~ADPD188_SLOTB_NUMPULSES_SLOTB_PERIOD_MASK;
	reg_data |= (0xE << ADPD188_SLOTB_NUMPULSES_SLOTB_PERIOD_POS) &
		    ADPD188_SLOTB_NUMPULSES_SLOTB_PERIOD_MASK;
	status = adpd188_reg_write(adpd1080_iio_device->drv_dev,
				   ADPD188_REG_SLOTB_NUMPULSES, reg_data);
	if(status != 0)
		return -1;

	/* Slot A integrator window */
	status = adpd188_reg_read(adpd1080_iio_device->drv_dev,
				  ADPD188_REG_SLOTA_AFE_WINDOW, &reg_data);
	if(status != 0)
		return -1;
	reg_data &= ~ADPD188_SLOTA_AFE_WINDOW_SLOTA_AFE_WIDTH_MASK;
	reg_data |= (0x4 << ADPD188_SLOTA_AFE_WINDOW_SLOTA_AFE_WIDTH_POS) &
		    ADPD188_SLOTA_AFE_WINDOW_SLOTA_AFE_WIDTH_MASK;
	reg_data &= ~ADPD188_SLOTA_AFE_WINDOW_SLOTA_AFE_OFFSET_MASK;
	reg_data |= (0x2F0 << ADPD188_SLOTA_AFE_WINDOW_SLOTA_AFE_OFFSET_POS) &
		    ADPD188_SLOTA_AFE_WINDOW_SLOTA_AFE_OFFSET_MASK;
	status = adpd188_reg_write(adpd1080_iio_device->drv_dev,
				   ADPD188_REG_SLOTA_AFE_WINDOW, reg_data);
	if(status != 0)
		return -1;

	/* Slot B integrator window */
	status = adpd188_reg_read(adpd1080_iio_device->drv_dev,
				  ADPD188_REG_SLOTB_AFE_WINDOW, &reg_data);
	if(status != 0)
		return -1;
	reg_data &= ~ADPD188_SLOTB_AFE_WINDOW_SLOTB_AFE_WIDTH_MASK;
	reg_data |= (0x4 << ADPD188_SLOTB_AFE_WINDOW_SLOTB_AFE_WIDTH_POS) &
		    ADPD188_SLOTB_AFE_WINDOW_SLOTB_AFE_WIDTH_MASK;
	reg_data &= ~ADPD188_SLOTB_AFE_WINDOW_SLOTB_AFE_OFFSET_MASK;
	reg_data |= (0x2F0 << ADPD188_SLOTB_AFE_WINDOW_SLOTB_AFE_OFFSET_POS) &
		    ADPD188_SLOTB_AFE_WINDOW_SLOTB_AFE_OFFSET_MASK;
	status = adpd188_reg_write(adpd1080_iio_device->drv_dev,
				   ADPD188_REG_SLOTB_AFE_WINDOW, reg_data);
	if(status != 0)
		return -1;

	/* Math for chop mode is inverted, non-inverted, non-inverted, inverted */
	status = adpd188_reg_read(adpd1080_iio_device->drv_dev, ADPD188_REG_MATH,
				  &reg_data);
	if(status != 0)
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
	if(status != 0)
		return -1;

	status = adpd1080pmod_32k_calib(adpd1080_iio_device->drv_dev);
	if (status < 0)
		return status;

	status = adpd188_clk32mhz_cal(adpd1080_iio_device->drv_dev);
	if(status != 0)
		return -1;

	status = adpd188_adc_fsample_set(adpd1080_iio_device->drv_dev, 512.0);
	if(status != 0)
		return -1;

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("adpd1080", adpd1080_iio_device, &iio_adpd188_device,
			       &iio_adpd1080_read_buff, NULL, NULL)
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = adpd1080_uart_ip;

	status = iio_app_init(&app, app_init_param);
	if (status)
		return status;

	return iio_app_run(app);
}

