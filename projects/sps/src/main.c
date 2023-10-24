/***************************************************************************//**
 *   @file   main.c
 *   @brief  Implementation of Main Function.
 *   @author Darius Berghe (darius.berghe@analog.com)
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
#include <stdio.h>
#include <math.h>
#include "no_os_error.h"
#include "no_os_util.h"
#include "iio_adpd188.h"
#include "iio_app.h"
#include "maxim_gpio.h"
#include "no_os_gpio.h"
#include "no_os_timer.h"
#include "maxim_irq.h"
#include "maxim_i2c.h"
#include "maxim_spi.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"
#include "maxim_gpio_irq.h"
#include "maxim_timer.h"
#include "parameters.h"
#include "adin1110.h"
#include "lwip_adin1110.h"
#include "iio_sps.h"
#include "LCD_Driver.h"
#include "LCD_GUI.h"
#include "fonts.h"
#include "ad7799.h"
#include "iio_ad7793.h"
#include "iio_ad7799.h"

uint8_t adin1110_mac_address[6] = {0x00, 0x18, 0x80, 0x03, 0x25, 0x80};

const struct no_os_gpio_init_param adin1110_reset_ip = {
	.port = 0,
	.number = 15,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = &(struct max_gpio_init_param) {
		.vssel = MXC_GPIO_VSSEL_VDDIOH,
	},
};

const struct no_os_spi_init_param adin1110_spi_ip = {
	.device_id = 3,
	.max_speed_hz = 25000000,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = &max_spi_ops,
	.chip_select = 0,
	.extra = &(struct max_spi_init_param) {
		.num_slaves = 1,
		.polarity = SPI_SS_POL_LOW,
		.vssel = MXC_GPIO_VSSEL_VDDIOH,
	},
};

struct adin1110_init_param adin1110_ip = {
	.chip_type = ADIN1110,
	.comm_param = adin1110_spi_ip,
	.reset_param = adin1110_reset_ip,
	.append_crc = false,
};

static uint8_t in_buff[1024];

static void adpd1080_sync_gpio_cb(void *ctx)
{
	int32_t *cnt = ctx;

	*cnt += 1;
}

const font_width = 17;
const font_height = 24;

void print_line(unsigned int line, unsigned int xcharoffset, unsigned int xcharclear, char *str)
{
	const int xstart = font_width * xcharoffset;
	const int ystart = font_height * line;
	
	LCD_SetArealColor(xstart, ystart, xstart + font_width * xcharclear, ystart + font_height, WHITE);
	GUI_DisString_EN(xstart, ystart, str, &Font24, WHITE, BLACK);
}

void print_io1(char *io1)
{
	static char prev[16];
	if (!strncmp(io1, prev, sizeof(prev)))
		return;
	print_line(1, 11, sizeof(prev), io1);
	strncpy(prev, io1, sizeof(prev));
}

void print_io2(char *io2)
{
	static char prev[16];
	if (!strncmp(io2, prev, sizeof(prev)))
		return;
	print_line(2, 11, sizeof(prev), io2);
	strncpy(prev, io2, sizeof(prev));
}

void print_io3(char *io3)
{
	static char prev[16];
	if (!strncmp(io3, prev, sizeof(prev)))
		return;
	print_line(3, 11, sizeof(prev), io3);
	strncpy(prev, io3, sizeof(prev));
}

void print_pH(int si_val)
{
	static int prev;
	char str[5];
	if (prev == si_val)
		return;
	float pH = 7.0 - ((si_val - 20) / 59.71);
	if (pH > 14)
		pH = 14;
	if (pH < 0)
		pH = 0;
	sprintf(str, "%.2f", pH);
	print_line(4, 11, 4, str);
	prev = si_val;
}

void display(struct sps_iio_desc *iiodev)
{
	static int64_t no_gestures = 0;
	int si_val;
	if (iiodev->d_gestures) {
		print_line(0, 11, 5, gestures[no_os_find_first_set_bit(iiodev->d_gestures)]);
		iiodev->d_gestures = 0;
		no_gestures = 50;
	} else if (no_gestures > 0)
		no_gestures--;
	
	if (no_gestures == 0) {
		print_line(0, 11, 5, "     ");
		no_gestures--;
	}

	print_io1(iiodev->io1);
	print_io2(iiodev->io2);
	print_io3(iiodev->io3);

	ad7799_read_channel(iiodev->pHdev, 0, &si_val);
	print_pH(si_val);
}

static int32_t adpd1080pmod_32k_calib(struct adpd188_dev *adpd1080_dev)
{
	int32_t status;
	uint16_t temp_reg;
	uint32_t t_start, t_stop = 0;
	volatile int32_t sync_gpio_pulse_no;
	struct no_os_timer_desc *cal_timer;
	struct no_os_timer_init_param cal_timer_init = {
		.id = 0,
		.ticks_count = 0,
		.freq_hz = 1000000,
		.platform_ops = &max_timer_ops,
	};
	struct no_os_irq_ctrl_desc *nvic;
	struct no_os_irq_init_param nvic_param = {
		.irq_ctrl_id = 0,
		.platform_ops = &max_irq_ops,
	};
	struct no_os_irq_ctrl_desc *gpio0_irqc;
	struct no_os_irq_init_param gpio0_irqc_config = {
		.irq_ctrl_id = GPIO0_PORT,
		.platform_ops = &max_gpio_irq_ops,
		.extra = NULL
	};

	status = no_os_timer_init(&cal_timer, &cal_timer_init);
	if(status != 0)
		return -1;
	status = no_os_timer_start(cal_timer);
	if(status != 0)
		goto timer_finish;

	status = no_os_irq_ctrl_init(&nvic, &nvic_param);
	if (status)
		return status;
	status = no_os_irq_ctrl_init(&gpio0_irqc, &gpio0_irqc_config);
	if(status != 0)
		goto finish;

	struct no_os_callback_desc sync_gpio_cb = {
		.callback = adpd1080_sync_gpio_cb,
		.ctx = &sync_gpio_pulse_no,
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ
	};
	status = no_os_irq_register_callback(gpio0_irqc, GPIO0_PIN, &sync_gpio_cb);
	if(status != 0)
		goto finish;

	status = no_os_irq_trigger_level_set(gpio0_irqc, GPIO0_PIN, NO_OS_IRQ_EDGE_FALLING);
	if(status != 0)
		goto finish;

	status = no_os_irq_set_priority(gpio0_irqc, GPIO0_PIN, 1);
	if (status != 0)
		goto finish;

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

	status = no_os_irq_enable(gpio0_irqc, GPIO0_PIN);
	if(status != 0)
		goto finish;

	status = no_os_irq_enable(nvic, GPIO0_IRQn);
	if (status)
		goto finish;

	while (true) {
		status = adpd188_reg_read(adpd1080_dev, ADPD188_REG_SAMPLE_CLK, &temp_reg);
		if(status != 0)
			goto finish;
		status = no_os_timer_counter_get(cal_timer, &t_start);
		if(status != 0)
			goto finish;
		sync_gpio_pulse_no = 0;
		no_os_mdelay(100);
		status = no_os_timer_counter_get(cal_timer, &t_stop);
		if(status != 0)
			goto finish;
		if (abs(sync_gpio_pulse_no - 100) < 1)
			break;
		if (sync_gpio_pulse_no < 100)
			temp_reg -= 1;
		else if (sync_gpio_pulse_no > 100)
			temp_reg += 1;
		status = adpd188_reg_write(adpd1080_dev, ADPD188_REG_SAMPLE_CLK, temp_reg);
		if(status != 0)
			goto finish;
	}

	status = no_os_irq_disable(gpio0_irqc, GPIO0_PIN);
	if(status != 0)
		goto finish;

	status = no_os_irq_unregister_callback(gpio0_irqc, GPIO0_PIN, &sync_gpio_cb);
	if(status != 0)
		goto finish;

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
	// TODO FIX THIS: no_os_irq_ctrl_remove(gpio0_irqc);
timer_finish:
	no_os_timer_remove(cal_timer);

	return status;
}

int app_step(void *arg)
{
	static int cnt1 = 0;
	static int cnt2 = 0;

	cnt1++;
	if (cnt1 == 100) {
		cnt1 = 0;
		sps_gesture_detection(arg);
	}

	cnt2++;
	if (cnt2 == 1000) {
		cnt2 = 0;
		display(arg);
	}

	return 0;
}

int main(void)
{
	int32_t status;
	uint16_t reg_data;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	struct no_os_uart_init_param adpd1080_uart_ip = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID, // unused in this demo
		.asynchronous_rx = false,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = &(struct max_uart_init_param) {
			.flow = UART_FLOW_DIS,
			.vssel = MXC_GPIO_VSSEL_VDDIOH,
		},
		.platform_ops = &max_uart_ops,
	};

	struct adpd188_iio_desc *adpd1080_iio_device;
	struct adpd188_iio_init_param adpd1080_iio_inital = {
		.drv_init_param.device = ADPD1080,
		.drv_init_param.phy_opt = ADPD188_I2C,
		.drv_init_param.phy_init.i2c_phy =
		{
			.slave_address = 0x64,
			.max_speed_hz = 400000,
			.platform_ops = &max_i2c_ops,
			.extra = &(struct max_i2c_init_param) {
				.vssel = MXC_GPIO_VSSEL_VDDIOH,
			},
		},
		.drv_init_param.gpio0_init =
		{
			.port = GPIO0_PORT,
			.number = GPIO0_PIN,
			.platform_ops = &max_gpio_ops,
			.extra = &(struct max_gpio_init_param) {
				.vssel = MXC_GPIO_VSSEL_VDDIOH,
			},
		},
		.drv_init_param.gpio1_init =
		{
			.port = GPIO1_PORT,
			.number = GPIO1_PIN,
			.platform_ops = &max_gpio_ops,
			.extra = &(struct max_gpio_init_param) {
				.vssel = MXC_GPIO_VSSEL_VDDIOH,
			},
		}
	};
	struct iio_data_buffer iio_adpd1080_read_buff = {
		.buff = in_buff,
		.size = NO_OS_ARRAY_SIZE(in_buff),
	};

	no_os_init();

struct max_spi_init_param ad7799_spi_extra_ip  = {
		.num_slaves = 1,
		.polarity = SPI_SS_POL_LOW,
		.vssel = MXC_GPIO_VSSEL_VDDIOH,
	};

	const struct no_os_spi_init_param ad7799_spi_ip = {
		.device_id = 4,
		.max_speed_hz = 1000000,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_3,
		.platform_ops = &max_spi_ops,
		.chip_select = 0,
		.extra = &ad7799_spi_extra_ip,
	};

	const struct ad7799_init_param ad7799_ip = {
		.spi_init = ad7799_spi_ip,
		.chip_type = ID_AD7793,
		.polarity = 0,
		.vref_mv = 1050,
		.gain = 1,
		.precision = AD7799_PRECISION_MV
	};
	struct ad7793_iio_desc *ad7793_iiodev;
	struct ad7793_iio_param ad7793_param = {
		.ad7793_ip = ad7799_ip,
	};

	status = ad7793_iio_init(&ad7793_iiodev, &ad7793_param);
	if (status)
		return status;

	status = ad7799_read(ad7793_iiodev->ad7793_desc, AD7799_REG_CONF, &reg_data);

	/* Enable excitation current */
	status = ad7799_read(ad7793_iiodev->ad7793_desc, AD7799_REG_IO, &reg_data);
	reg_data |= NO_OS_BIT(3) | NO_OS_BIT(2) | NO_OS_BIT(1);
	status = ad7799_write(ad7793_iiodev->ad7793_desc, AD7799_REG_IO, reg_data);
	status = ad7799_set_mode(ad7793_iiodev->ad7793_desc, 0x3);

	status = ad7799_read(ad7793_iiodev->ad7793_desc, AD7799_REG_MODE, &reg_data);
	reg_data |= NO_OS_GENMASK(3, 0);
	status = ad7799_write(ad7793_iiodev->ad7793_desc, AD7799_REG_MODE, reg_data);

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
		return status;

	status = adpd188_adc_fsample_set(adpd1080_iio_device->drv_dev, 512.0);
	if(status != 0)
		return status;
	
	int ch;
	int32_t raw[8];
	while(sps_get_fifo_data(adpd1080_iio_device->drv_dev, raw) == -EAGAIN)
		; // blocking wait
	for (ch = 0; ch < 8; ch++) {
		uint16_t offset;
		uint8_t offset_reg = ch / 4 ? ADPD188_REG_SLOTB_CH1_OFFSET : ADPD188_REG_SLOTA_CH1_OFFSET + (ch % 4);
		status = adpd188_reg_read(adpd1080_iio_device->drv_dev, offset_reg,
					&offset);
		if (status != 0)
			return status;
		status = adpd188_reg_write(adpd1080_iio_device->drv_dev, offset_reg,
					raw[ch] + offset);
		if (status != 0)
			return status;
	}

	status = adpd188_mode_set(adpd1080_iio_device->drv_dev, ADPD188_NORMAL);
	if (status != 0)
		return status;
	
	struct sps_iio_desc *sps;
	struct sps_iio_init_param sps_config = {
		.dev = adpd1080_iio_device->drv_dev,
		.th_intensity = 1200000,
		.th_click = 60,
		.pHdev = ad7793_iiodev->ad7793_desc,
	};
	status = sps_iio_init(&sps, &sps_config);

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("adpd1080", adpd1080_iio_device, &iio_adpd188_device,
			       &iio_adpd1080_read_buff, NULL, NULL),
		IIO_APP_DEVICE("ad7793", ad7793_iiodev, &ad7799_iio_descriptor,
				NULL, NULL, NULL),
		IIO_APP_DEVICE("sps", sps, &iio_sps_device,
				NULL, NULL, NULL)
	};

	memcpy(adin1110_ip.mac_address, adin1110_mac_address, NETIF_MAX_HWADDR_LEN);
	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = adpd1080_uart_ip;
	app_init_param.post_step_callback = app_step;
	app_init_param.arg = sps;
	app_init_param.lwip_param.platform_ops = &adin1110_lwip_ops;
	app_init_param.lwip_param.mac_param = &adin1110_ip;
	app_init_param.lwip_param.extra = NULL;
	memcpy(app_init_param.lwip_param.hwaddr, adin1110_mac_address,
	       NETIF_MAX_HWADDR_LEN);

	status = iio_app_init(&app, app_init_param);
	if (status)
		return status;
	
	LCD_Init(U2D_R2L, 800);
	LCD_Clear(WHITE);
	print_line(0, 0, 10, "Gesture  :");
	print_line(1, 0, 10, "IO1  (mA):");
	print_line(2, 0, 10, "IO2      :");
	print_line(3, 0, 10, "IO3   (V):");
	print_line(4, 0, 10, "pH       :");

	return iio_app_run(app);
}

