/***************************************************************************//**
 *   @file   example.c
 *   @brief  Implementation of EVAL_POWRMS example for demo project.
 *   @author Robert Budai (robert.budai@analog.com)
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
#include "example.h"
#include "powrms_gpios.h"
#include "powrms_utils.h"
#include "powrms_data_processing.h"
#include "m24512.h"
#include "subscreen_blank_screen.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_uart.h"
#include "no_os_print_log.h"
#include "maxim_uart_stdio.h"
#include "max32662.h"

#define SCREEN_UPDATE_PERIOD_MS     200

// Buffer for UI usage - must be defined in .c file to avoid multiple instantiation
// Use LVGL's memory alignment macro for proper alignment across platforms
LV_ATTRIBUTE_MEM_ALIGN uint8_t display_buffer[SSD1306_HOR_REZ *
				SSD1306_VER_REZ / 8 + 8];
bool show_menu = 1;
bool enter_pressed = 0;
enum display_entry_t display_entry = DISPLAY_ENTRY_SHOW;

void my_flush_cb(lv_display_t * disp, const lv_area_t * area,
		 uint8_t * color_p)
{
	uint8_t * lvgl_buff = &display_buffer[8];
	uint8_t buff_to_ssd1306[8 * 128] = {0};

	no_os_row_major_to_column_major_8bits(lvgl_buff, buff_to_ssd1306,
					      SSD1306_HOR_REZ, SSD1306_VER_REZ);

	pr_info("FLUSH: -> ssd1306 write (full frame)\n");
	ssd_1306_print_buffer(oled_display, (char *)buff_to_ssd1306);
	pr_info("FLUSH: <- ssd1306 write done\n");
	lv_display_flush_ready(disp);

	no_os_udelay(20);
}

/**
 * @brief Initialize the display task
 *
 * @return int
 */
int display_task()
{
	lv_init();
	memset(&display_buffer[8], 0x00, sizeof(display_buffer) - 8);
	lv_display_t * disp = lv_display_create(SSD1306_HOR_REZ, SSD1306_VER_REZ);
	lv_display_set_flush_cb(disp, my_flush_cb);
	lv_display_set_buffers(disp, display_buffer, NULL, sizeof(display_buffer),
			       LV_DISPLAY_RENDER_MODE_FULL);
	return 0;
}

/**
 * @brief Configure the ADC
 *
 * @return int
 */
int configure_adc()
{
	return ad7091r5_init(&adc_desc, &adc_ini_param);
}

/**
 * @brief Configure the display
 *
 * @return int
 */
int configure_display()
{
	int ret;

	ret = no_os_i2c_init(&oled_display_i2c_desc, &oled_display_i2c_init_param);
	if (ret)
		return ret;
	oled_display_extra.i2c_desc = oled_display_i2c_desc;
	ret = display_init(&oled_display, &oled_display_ini_param);
	if (ret) {
		no_os_free(oled_display_i2c_desc);
		return ret;
	}

	return 0;
}

/**
 * @brief Initialize the debug UART and route stdio (printf / pr_*) to it.
 *
 * Uses the same UART that the IIO transport runs on (iio_demo_usb_ip) so that
 * pr_info()/pr_debug()/pr_err() messages from the whole firmware appear on the
 * host serial console.
 *
 * @return int - 0 on success, negative error code on failure.
 */
int configure_debug_uart(void)
{
	int ret;

	ret = no_os_uart_init(&uart_iio_desc, &iio_demo_usb_ip);
	if (ret)
		return ret;

	/* Redirect printf()/scanf() (and therefore the pr_* macros) to UART. */
	no_os_uart_stdio(uart_iio_desc);

	return 0;
}

/**
 * @brief Return a monotonic millisecond timestamp from the system tick source.
 *
 * @return uint32_t - milliseconds since system start (wraps after ~49 days).
 */
static uint32_t screen_now_ms(void)
{
	struct no_os_time t = no_os_get_time();
	return (uint32_t)(t.s * 1000U + t.us / 1000U);
}

/**
 * @brief IIO post-step callback.
 *
 * Runs after every iio_step(). The heavy screen/ADC work is rate-limited to
 * SCREEN_UPDATE_PERIOD_MS so it does not block the IIO transport on every poll.
 * LVGL's tick is advanced with the real elapsed time rather than a fixed value.
 *
 * @return int - always 0; a screen-side error must never tear down the IIO app.
 */
int screen_while()
{
	static uint32_t last_update_ms;
	static bool first_call = true;
	static uint32_t iter;
	uint32_t now_ms = screen_now_ms();
	uint32_t elapsed_ms;

	// Reset watchdog - handle potential failure gracefully
	(void)powrms_watchdog_reset();

	if (first_call) {
		first_call = false;
		last_update_ms = now_ms;
		pr_info("LOOP: first call, t=%lu ms\n", (unsigned long)now_ms);
		return 0;
	}

	elapsed_ms = now_ms - last_update_ms;
	if (elapsed_ms < SCREEN_UPDATE_PERIOD_MS)
		return 0;

	last_update_ms = now_ms;

	// Per-update step trace. If the board browns out mid-update, the last
	// LOOP line printed identifies the step that was running when power
	// collapsed.
	iter++;
	pr_info("LOOP[%lu]: begin (t=%lu ms, dt=%lu)\n", (unsigned long)iter,
		(unsigned long)now_ms, (unsigned long)elapsed_ms);

	// Advance LVGL's timebase by the real elapsed time
	lv_tick_inc(elapsed_ms);

	// Update ADC data
	pr_info("LOOP[%lu]: -> ADC read\n", (unsigned long)iter);
	_update_adc_data();
	pr_info("LOOP[%lu]: <- ADC read done\n", (unsigned long)iter);

	// Process screen based on current display entry
	pr_info("LOOP[%lu]: -> screen (entry=%d)\n", (unsigned long)iter,
		(int)display_entry);
	switch (display_entry) {
	case DISPLAY_ENTRY_MENU:
		subscreen_main_menu_screen();
		break;
	case DISPLAY_ENTRY_SHOW:
		subscreen_show_screen();
		break;
	case DISPLAY_ENTRY_SETTINGS:
		subscreen_settings_screen();
		break;
	case DISPLAY_ENTRY_BLANK:
		subscreen_blank_screen();
		break;
	default:
		// Invalid display entry - reset to safe default
		display_entry = DISPLAY_ENTRY_SHOW;
		break;
	}
	pr_info("LOOP[%lu]: <- screen done\n", (unsigned long)iter);

	pr_info("LOOP[%lu]: end\n", (unsigned long)iter);
	return 0;
}

/**
 * @brief Print and decode the cause of the most recent reset.
 *
 * The MAX32662 latches the reset source in the GCR RST0 register. The lower
 * bits of RST0 are peripheral-reset triggers; the upper bits report why the
 * core last reset and survive across the reset:
 *   bit 1  (WDT)    - watchdog timeout reset
 *   bit 29 (SOFT)   - software reset (SYSRESETREQ / soft reset request)
 *   bit 30 (PERIPH) - peripheral reset
 *   bit 31 (SYS)    - system reset (external nRESET pin / system-level)
 * If none of these are set the reset was a power-on / brownout (POR/BOR).
 *
 * The raw register is printed first so the value can always be decoded by
 * hand. RST0 is intentionally NOT written back here: writing a 1 to any of the
 * low peripheral-reset bits would reset that peripheral.
 */
static void print_reset_cause(void)
{
	uint32_t rst0 = MXC_GCR->rst0;

	pr_info("Reset cause: GCR->RST0 = 0x%08lX\n", (unsigned long)rst0);

	if (rst0 & MXC_F_GCR_RST0_WDT)
		pr_info("  - WATCHDOG reset (WDT timeout)\n");
	if (rst0 & MXC_F_GCR_RST0_SOFT)
		pr_info("  - SOFTWARE reset (soft reset request)\n");
	if (rst0 & MXC_F_GCR_RST0_PERIPH)
		pr_info("  - PERIPHERAL reset\n");
	if (rst0 & MXC_F_GCR_RST0_SYS)
		pr_info("  - SYSTEM reset (external nRESET / system)\n");
	if (!(rst0 & (MXC_F_GCR_RST0_WDT | MXC_F_GCR_RST0_SOFT |
		      MXC_F_GCR_RST0_PERIPH | MXC_F_GCR_RST0_SYS)))
		pr_info("  - POWER-ON / BROWNOUT reset (POR/BOR)\n");
}

/**
 * @brief Main function
 *
 * @return int
 */
int example_main(void)
{
	int ret;

	no_os_init();

	// Initialize the debug UART first so that every subsequent bring-up step
	// can report its progress over the serial console (printf / pr_*).
	// ret = configure_debug_uart();
	// if (ret)
	// 	return ret;

	pr_info("EVAL-POWRMS firmware v%d.%d.%d starting up\n",
		FW_VERSION_MAJOR, FW_VERSION_MINOR, FW_VERSION_PATCH);

	/* Report why the board last reset (helps diagnose reboot loops). */
	print_reset_cause();

	pr_info("Configuring startup GPIOs / power rails...\n");
	configure_gpios_upon_startup();
	pr_debug("Startup GPIOs configured\n");

	pr_info("Initializing MAX77986 charger (MODE 6, max current)...\n");
	ret = max77986_init(&max77986_i2c_desc, &max77986_i2c_init_param);
	if (ret) {
		pr_err("MAX77986 init failed: %d\n", ret);
		return ret;
	}
	pr_info("MAX77986 init done\n");

	pr_info("Initializing ADC (AD7091R5)...\n");
	ret = configure_adc();
	if (ret) {
		pr_err("ADC init failed: %d\n", ret);
		configure_gpios_upon_shutdown();
		return ret;
	}
	pr_info("ADC init done\n");

	pr_info("Initializing OLED display (SSD1306)...\n");
	ret = configure_display();
	if (ret) {
		pr_err("Display init failed: %d\n", ret);
		return ret;
	}
	pr_info("Display init done\n");

	ret = display_task();
	if (ret) {
		pr_err("Display task init failed: %d\n", ret);
		return ret;
	}
	pr_debug("LVGL display task initialized\n");

	pr_info("Initializing buttons...\n");
	ret = buttons_init();
	if (ret) {
		pr_err("Buttons init failed: %d\n", ret);
		return ret;
	}
	pr_debug("Buttons initialized\n");

	pr_info("Initializing EEPROM-backed memory...\n");
	ret = powrms_init_memory_upon_boot();
	if (ret) {
		pr_err("Memory init failed: %d\n", ret);
		return ret;
	}
	pr_info("Boot initialization complete\n");

	display_entry = DISPLAY_ENTRY_SHOW;
	pr_info("DIAG: forcing display_entry=SHOW (ignoring P0.14)\n");

	static int powrms_desc = 0;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};
	struct iio_data_buffer buff = {
		.buff = (void *)iio_data_buffer_loc,
		.size = MAX_SIZE_BASE_ADDR * sizeof(IIO_BUFF_TYPE),
	};

	memset(iio_data_buffer_loc, 0, MAX_SIZE_BASE_ADDR * sizeof(IIO_BUFF_TYPE));

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("powrms", &powrms_desc, &powrms_iio_descriptor, &buff, NULL, NULL)
	};

	static char firmware_version_str[16];
	powrms_get_firmware_version(firmware_version_str);

	struct iio_ctx_attr context_attributes[] = {
		{
			.name = "hw_model",
			.value = "EVAL-POWRMS"
		},
		{
			.name = "hw_vendor",
			.value = "Analog Devices Inc."
		},
		{
			.name = "firmware_version",
			.value = firmware_version_str
		}
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = iio_demo_usb_ip;
	app_init_param.ctx_attrs = context_attributes;
	app_init_param.nb_ctx_attr = NO_OS_ARRAY_SIZE(context_attributes);
	app_init_param.post_step_callback = &(screen_while);

	display_entry = DISPLAY_ENTRY_SHOW;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	// Due to UART initialization for iio, pin 0.20 VDRIVE looses voltage hence the I2C is failing, setting it back to proper values
	pr_info("Configuring startup GPIOs / power rails...\n");
	configure_gpios_upon_startup();
	pr_debug("Startup GPIOs configured\n");

	// Increase watchdog timer limits
	(void)powrms_watchdog_init(8000);

	ret = iio_app_run(app);

	iio_app_remove(app);

	return 0;
}
