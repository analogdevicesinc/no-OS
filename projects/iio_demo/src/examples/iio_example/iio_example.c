/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  Implementation of IIO example for iio_demo project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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

#include "iio_example.h"
#include "iio_adc_demo.h"
#include "iio_dac_demo.h"
#include "common_data.h"
#include "no_os_util.h"
#include "mxc_delay.h"
#include "mxc_sys.h"
#include "nvic_table.h"

#include <stdint.h>
#include <stdio.h>

#include "no_os_delay.h"

#include "no_os_i2c.h"
#include "maxim_i2c.h"

#include "display.h"
#include "ssd_1306.h"

// #include "FreeRTOSConfig.h"

#include "lvgl.h"

// Systick setup

#define USE_SYSTEM_CLK 1
#define SYSTICK_PERIOD_SYS_CLK 4800000 //40ms with 120MHz system clock
#define SYSTICK_PERIOD_EXT_CLK 3277 //100ms with 32768Hz external RTC clock


// STDIO UART

#define INTC_DEVICE_ID	0
#define UART_DEVICE_ID	0
#define UART_BAUDRATE	115200
#define UART_IRQ_ID    	UART0_IRQn
#define UART_EXTRA      &demo_uart_extra_ip
#define UART_OPS        &max_uart_ops

struct no_os_uart_desc *demo_uart_desc;

struct max_uart_init_param demo_uart_extra_ip = {
	.flow = UART_FLOW_DIS
};

struct no_os_uart_init_param demo_uart_ip = {
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

// SSD1306 setup

#define SSD1306_HOR_REZ 128
#define SSD1306_VER_REZ 64
#define SSD1306_BUFFER_SIZE ((SSD1306_HOR_REZ * SSD1306_VER_REZ) / 8 + 8) 

struct display_dev *oled_display;


struct max_i2c_init_param oled_display_i2c_maxim_extra_param = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};

struct no_os_i2c_init_param oled_display_i2c_init_param = {
	.device_id = 1,
	.max_speed_hz = 400000,
	.slave_address = SSD1306_I2C_ADDR,
	.platform_ops = &max_i2c_ops,
	.extra = &oled_display_i2c_maxim_extra_param,
};

void my_flush_cb(lv_display_t * disp, const lv_area_t * area, lv_color_t * color_p)
{
	// Flush the buffer to the display
	// This function should be implemented to send the pixel data to the display
	// For example, using I2C or SPI communication
	// The area parameter indicates the region of the display to be updated
	// The color_p parameter points to the pixel data to be sent

    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing */
	// uint8_t *buff;
	// printf("LVGL FLUSH CALLBACK\n\r");
	// int ret = ssd_1306_print_buffer(oled_display, buff);
    lv_display_flush_ready(disp);
}

/***************************************************************************//**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function app_run and will not return.
*******************************************************************************/
int display_task()
{
	int ret;
	struct no_os_i2c_desc *oled_display_i2c_desc;
	// init i2c
	ret = no_os_i2c_init(&oled_display_i2c_desc, &oled_display_i2c_init_param);
	if (ret) {
		printf("Failed to initialize I2C.\n\r");
		return 0;
	}

	ssd_1306_extra oled_display_extra = {
		.comm_type = SSD1306_I2C,
		.i2c_desc = oled_display_i2c_desc,
		.i2c_ip = &oled_display_i2c_init_param,
	};

	struct display_init_param oled_display_ini_param = {
		.cols_nb = 128,
		.rows_nb = 64,
		.controller_ops = &ssd1306_ops,
		.extra = &oled_display_extra,
	};

	ret = display_init(&oled_display, &oled_display_ini_param);
	if (ret) {
		printf("Failed to initialize display.\n\r");
		return 0;
	}
	printf("Display initialized.\n\r");
	

	// Initialize lvgl library
	lv_init();

	uint8_t frame_buffer_1[SSD1306_BUFFER_SIZE];
	uint8_t frame_buffer_2[SSD1306_BUFFER_SIZE];

	memset(frame_buffer_1, 0x3C, SSD1306_BUFFER_SIZE);
	memset(frame_buffer_2, 0, SSD1306_BUFFER_SIZE);
	
	printf("Buffer frame initialized\n\r");
	
	lv_display_t * my_disp = lv_display_create(SSD1306_HOR_REZ, SSD1306_VER_REZ);
	if (my_disp == NULL) {
		printf("Failed to create display.\n\r");
		return 0;
	}
	lv_display_set_flush_cb(my_disp, my_flush_cb); /* Set a flush callback to draw to the display */
	lv_display_set_buffers(my_disp, frame_buffer_1, frame_buffer_2, SSD1306_BUFFER_SIZE, LV_DISPLAY_RENDER_MODE_FULL); 

	ret = ssd_1306_display_on_off(oled_display, 1);
	if (ret) {
		printf("Failed to turn display on.\n\r");
		return 0;
	}
	printf("Display set ON.\n\r");

    LV_DRAW_BUF_DEFINE_STATIC(draw_buf, 128, 64, LV_DRAW_SW_SUPPORT_I1);
    LV_DRAW_BUF_INIT_STATIC(draw_buf);

	/* Infinite loop */
	ret = ssd_1306_print_buffer(oled_display, frame_buffer_1);
	if (ret)
		printf("Error on buffer print \n\r");
	while (1)
	{
		printf("-");
		lv_timer_handler();
		ret = ssd_1306_print_buffer(oled_display, draw_buf.data);
		MXC_Delay(MXC_DELAY_MSEC((200)));
		// ret = ssd_1306_print_buffer(oled_display, frame_buffer_2);
		// MXC_Delay(MXC_DELAY_MSEC((200)));

	}
	return 0;
}

int iio_example_main(void)
{
	int ret;
	// TaskHandle_t iio_app_handle = NULL;
	// TaskHandle_t led_task_handle = NULL;

	// Init stdio
	ret = no_os_uart_init(&demo_uart_desc, &demo_uart_ip);
	if (ret)
		return 0;

	no_os_uart_stdio(demo_uart_desc);

	printf("\n\r\n\r###############\n\r\n\r");
	printf("Starting firmware...\n\r");
	// systick_setup();
	display_task();

	return -1;
}