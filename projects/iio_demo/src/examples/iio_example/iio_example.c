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

#include <stdint.h>
#include <stdio.h>

#include "no_os_delay.h"

#include "no_os_i2c.h"
#include "maxim_i2c.h"

#include "display.h"
#include "ssd_1306.h"


#include "lvgl.h"

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

struct display_dev *oled_display;
struct no_os_i2c_desc *oled_display_i2c_desc;

struct max_i2c_init_param oled_display_i2c_maxim_extra_param = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};

struct no_os_i2c_init_param oled_display_i2c_init_param = {
	.device_id = 1,
	.max_speed_hz = 200000,
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
	printf("LVGL FLUSH CALLBACK\n\r");
    lv_display_flush_ready(disp);
}

/***************************************************************************//**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function app_run and will not return.
*******************************************************************************/
int iio_example_main()
{
	int ret;

	// Init stdio
	ret = no_os_uart_init(&demo_uart_desc, &demo_uart_ip);
	if (ret)
		return 0;

	no_os_uart_stdio(demo_uart_desc);

	printf("\n\r\n\r###############\n\r\n\r");
	printf("Starting firmware...\n\r");

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

	// create a test buffer filled with 1s to test flush_cb of lvg library
	int32_t buffer_size = (SSD1306_HOR_REZ * SSD1306_VER_REZ) / 8 + 1;

	uint8_t frame_buffer_1[buffer_size];
	uint8_t frame_buffer_2[buffer_size];

	memset(frame_buffer_1, 1, buffer_size);
	memset(frame_buffer_2, 1, buffer_size);
	
	printf("Buffer frame initialized\n\r");
	
	lv_display_t * my_disp = lv_display_create(SSD1306_HOR_REZ, SSD1306_VER_REZ);
	if (my_disp == NULL) {
		printf("Failed to create display.\n\r");
		return 0;
	}
	lv_display_set_flush_cb(my_disp, my_flush_cb); /* Set a flush callback to draw to the display */
	lv_display_set_buffers(my_disp, frame_buffer_1, frame_buffer_2, sizeof(frame_buffer_1), LV_DISPLAY_RENDER_MODE_DIRECT); 


	ret = ssd_1306_display_on_off(oled_display, 1);
	if (ret) {
		printf("Failed to turn display on.\n\r");
		return 0;
	}
	printf("Display set ON.\n\r");


	/* Infinite loop */
	while (1)
	{
		printf(".");
		lv_timer_handler();
		no_os_mdelay(50);
	}

	while (1) {
		for (int num = '0'; num <= '9'; num++) {
			ret = ssd_1306_move_cursor(oled_display, 0, 0);
			if (ret) {
				printf("Failed to move cursor on display.\n\r");
				return 0;
			}
			for (int column_nr = 0; column_nr < 127; column_nr++) {
				for (int page_Addr = 0; page_Addr < 7; page_Addr++) {
					ret = ssd_1306_print_ascii(oled_display, num, page_Addr, column_nr);
					// no_os_udelay(100U);
					if (ret) {
						printf("Failed to print character on display.\n\r");
						return 0;
					}
					// printf("%c", num);
				}
			}
			no_os_mdelay(1000U);
		}
	}

	// while(1) {
	// Do something
	printf("Hello world!\n\r");
	printf("Maxim SysTick Counter: %lu\n\r", MXC_TMR_GetCount(MXC_TMR0));
	no_os_mdelay(60);
	// }
	return 0;

}
