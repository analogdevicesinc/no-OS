/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main function of GNSS example.
 *   @author Radu Etz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2025 Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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
#include "no_os_uart.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "maxim_uart.h"
#include "maxim_gpio.h"
#include "maxim_uart_stdio.h"
#include "maxim_irq.h"
#include "nmea_ubx_gnss.h"
#include "common_data.h"
#include "platform.h"
#include "interrupt.h"
#include "no_os_gnss.h"

/* IRQ controller descriptor */
struct no_os_irq_ctrl_desc *gnss_nvic_desc;

int main(void)
{
	int ret;
	bool timing_valid;
	uint32_t fractional_seconds;
	uint32_t unix_epoch;
	struct no_os_gnss_position_data position_data;
	/* GNSS precise time */
	struct no_os_gnss_precise_time precise_time;
	/* GNSS device descriptor when using no-OS API */
	struct no_os_gnss_desc *gnss_desc;
	/* Console UART descriptor */
	struct no_os_uart_desc *uart_console_desc;
	/* GPIO LED descriptors */
	struct no_os_gpio_desc *gpio_led_desc;
	/* GPIO reset descriptor */
	struct no_os_gpio_desc *gpio_gnss_reset_desc;
	/* Initialize NVIC IRQ controller */
	struct no_os_irq_init_param gnss_nvic_ip = {
		.platform_ops = &max_irq_ops,
	};

	ret = no_os_irq_ctrl_init(&gnss_nvic_desc, &gnss_nvic_ip);
	if (ret) {
		pr_err("Failed to initialize IRQ controller\n");
		goto error;
	}

	ret = no_os_irq_set_priority(gnss_nvic_desc,
				     GPIO2_IRQn, 1);
	if (ret)
		goto remove_irq;

	ret = no_os_irq_enable(gnss_nvic_desc,
			       GPIO2_IRQn);
	if (ret)
		goto remove_irq;

	/* Initialize console UART (UART0) */
	ret = no_os_uart_init(&uart_console_desc, &uart_console_ip);
	if (ret) {
		pr_err("Failed to initialize console UART\n");
		goto remove_irq;
	}

	/* Set up console output */
	no_os_uart_stdio(uart_console_desc);

	/* Initialize status LED GPIO */
	ret = no_os_gpio_get_optional(&gpio_led_desc, &gpio_led_ip);
	if (ret) {
		pr_warning("Failed to initialize LED GPIO (continuing without LED)\n");
		gpio_led_desc = NULL;
	}
	if (gpio_led_desc) {
		ret = no_os_gpio_direction_output(gpio_led_desc, NO_OS_GPIO_LOW);
		if (ret) {
			pr_err("Failed to set LED GPIO direction\n");
			goto remove_led;
		}
	}

	/* Initialize GNSS reset GPIO */
	ret = no_os_gpio_get_optional(&gpio_gnss_reset_desc, &gpio_gnss_reset_ip);
	if (ret) {
		pr_warning("Failed to initialize GNSS reset GPIO (continuing without reset)\n");
		gpio_gnss_reset_desc = NULL;
	}

	if (gpio_gnss_reset_desc) {
		ret = no_os_gpio_direction_output(gpio_gnss_reset_desc, NO_OS_GPIO_HIGH);
		if (ret) {
			pr_err("Failed to set GNSS reset GPIO direction\n");
			goto remove_gnss_reset;
		}
	}

	pr_info("\n");
	pr_info("GNSS example\n");
	pr_info("========================\n");

	/* === DEVICE TYPE CONFIGURATION === */
	/* This unified driver supports different device types:
	 * 1. GNSS_DEVICE_UBX_CAPABLE: u-blox devices with UBX protocol using CFG commands (nanosecond precision)
	 * 2. GNSS_DEVICE_NMEA_ONLY: GNSS devices using NMEA protocol only (millisecond precision)
	 *    - NMEA_GPS_ONLY: Legacy GPS-only devices ($GPxxx sentences)
	 *    - NMEA_GNSS_MULTI: Modern multi-constellation devices ($GNxxx sentences)
	 */

	/* Configuration Example 1: u-blox Device (Default) */

	pr_info("Configuring for u-blox device with UBX protocol\n");
	/* Platform-specific initialization parameters */
	struct nmea_ubx_gnss_init_param platform_init_param = {
		.gnss_init_param = {
			.uart_init = &uart_gnss_ip,
			.gpio_reset = gpio_gnss_reset_desc,
			.device_type = GNSS_DEVICE_UBX_CAPABLE,
			.nmea_constellation_type = NMEA_GPS_ONLY,  /* Not used for UBX devices */
			.ubx_input_enable = ENABLE,
			.nmea_input_enable = DISABLE,
			.ubx_output_enable = ENABLE,
			.nmea_output_enable = DISABLE,
			.baud_rate = 38400
		}
	};

	/* Configure using standardized no-OS GNSS API */
	struct no_os_gnss_init_param no_os_gnss_init_param = {
		.device_id = 0,
		.pps_config = {
			.pps_enable = true,
			.frequency = GNSS_PPS_1HZ,
			.pulse_length = GNSS_PPS_LENGTH_100MS
		},
		.platform_ops = &nmea_ubx_gnss_ops,
		.extra = &platform_init_param
	};

	/* Configuration Example 2: Legacy GPS-only NMEA Device (Uncomment to test)
	 * Comment out Configuration Example 1 above and Example 3.
	 * For older GPS-only devices that use $GPZDA sentences:
	 */

	// pr_info("Configuring for legacy GPS-only NMEA device\n");
	// gnss_init_param.uart_init = &uart_gnss_ip;
	// gnss_init_param.gpio_reset = gpio_gnss_reset_desc;
	// gnss_init_param.device_type = GNSS_DEVICE_NMEA_ONLY;
	// gnss_init_param.nmea_constellation_type = NMEA_GPS_ONLY;
	// gnss_init_param.ubx_input_enable = DISABLE;    // Not used for NMEA devices
	// gnss_init_param.nmea_input_enable = ENABLE;    // Not used for NMEA devices
	// gnss_init_param.ubx_output_enable = DISABLE;   // Not used for NMEA devices
	// gnss_init_param.nmea_output_enable = ENABLE;   // Not used for NMEA devices
	// gnss_init_param.baud_rate = 9600;

	/* Configuration Example 3: Modern GNSS Multi-constellation NMEA Device (Uncomment to test)
	 * Comment out Configuration Example 1 above and Example 2.
	 * For modern GNSS devices that use $GNZDA sentences:
	 */

	// pr_info("Configuring for modern GNSS multi-constellation NMEA device\n");
	// gnss_init_param.uart_init = &uart_gnss_ip;
	// gnss_init_param.gpio_reset = gpio_gnss_reset_desc;
	// gnss_init_param.device_type = GNSS_DEVICE_NMEA_ONLY;
	// gnss_init_param.nmea_constellation_type = NMEA_GNSS_MULTI;
	// gnss_init_param.ubx_input_enable = DISABLE;    // Not used for NMEA devices
	// gnss_init_param.nmea_input_enable = ENABLE;    // Not used for NMEA devices
	// gnss_init_param.ubx_output_enable = DISABLE;   // Not used for NMEA devices
	// gnss_init_param.nmea_output_enable = ENABLE;   // Not used for NMEA devices
	// gnss_init_param.baud_rate = 9600;

	/* Initialize GNSS device using standardized no-OS API */
	ret = no_os_gnss_init(&gnss_desc, &no_os_gnss_init_param);
	if (ret) {
		pr_err("Failed to initialize GNSS device: %d\n", ret);
		goto remove_gnss_reset;
	}

	/* Get platform-specific data to access driver device for interrupt setup */
	struct nmea_ubx_gnss_extra *platform_extra = (struct nmea_ubx_gnss_extra *)
			gnss_desc->extra;

	/* Initialize interrupt */
	ret = interrupt_init(platform_extra->gnss_device);
	if (ret)
		goto free_gnss;

	pr_info("GNSS device initialized successfully\n");
	pr_info("Starting GNSS measurement loop...\n");

	/* PPS output configured automatically via no-OS API init parameters if UBX capable */
	if (platform_extra->gnss_device->device_type == GNSS_DEVICE_UBX_CAPABLE) {
		bool pps_enabled = gnss_ubx_is_time_pulse_enabled(platform_extra->gnss_device);
		pr_info("PPS configuration complete. Status: %s\n",
			pps_enabled ? "ENABLED" : "DISABLED");
	}

	/* Main measurement loop */
	platform_extra->gnss_device->led_desc = gpio_led_desc;
	reset_irq_flag_state();

	ret = no_os_irq_enable(platform_extra->gnss_device->irq_ctrl,
			       GPIO_IRQ_PIN);

	while (1) {
		if (get_irq_flag_state()) {
			reset_irq_flag_state();
			ret = platform_toggle_led(gpio_led_desc);
			if (ret) {
				pr_warning("Failed to toggle LED\n");
			}

			if (platform_extra->gnss_device->device_type == GNSS_DEVICE_UBX_CAPABLE) {
				pr_info("Device Type: UBX-capable (nanosecond precision)\n");
			} else {
				pr_info("Device Type: NMEA-only (millisecond precision)\n");
				pr_info("Constellation: %s\n",
					(platform_extra->gnss_device->nmea_constellation_type == NMEA_GPS_ONLY) ?
					"GPS-only ($GPRMC)" : "GNSS multi-constellation ($GNRMC)");
			}

			/* Step 1: Refresh timing data */
			ret = no_os_gnss_refresh_timing_data(gnss_desc);
			if (ret) {
				pr_err("Failed to refresh timing data: %d\n", ret);
				ret = no_os_irq_enable(platform_extra->gnss_device->irq_ctrl,
						       GPIO_IRQ_PIN);
				continue;
			}

			/* Step 2: Check if timing data is valid */
			ret = no_os_gnss_is_timing_valid(gnss_desc, &timing_valid);
			if (ret || !timing_valid) {
				pr_warning("Timing data is not valid, waiting for fix...\n");
				ret = no_os_irq_enable(platform_extra->gnss_device->irq_ctrl,
						       GPIO_IRQ_PIN);
				continue;
			}

			/* Step 3: Get comprehensive timing information */
			ret = no_os_gnss_get_unified_timing(gnss_desc, &precise_time);
			if (ret) {
				pr_warning("Failed to get unified timing: %d\n", ret);
				ret = no_os_irq_enable(platform_extra->gnss_device->irq_ctrl,
						       GPIO_IRQ_PIN);
				continue;
			}

			/* Step 4: Get Unix epoch with best available precision */
			unix_epoch = no_os_gnss_get_unix_epoch_unified(gnss_desc,
					&fractional_seconds);
			/* Display unified timing information */
			pr_info("=== Unified Timing Results ===\n");
			pr_info("Date/Time: %04d-%02d-%02d %02d:%02d:%02d\n",
				precise_time.year, precise_time.month, precise_time.day,
				precise_time.hour, precise_time.minute, precise_time.second);

			pr_info("Unix Epoch: %lu.%06lu\n", unix_epoch, fractional_seconds);
			pr_info("Nanoseconds: %ld\n", precise_time.nanoseconds);
			pr_info("Time Accuracy: %lu ns\n", precise_time.time_accuracy);
			pr_info("Flags: Valid=%s, Resolved=%s, Date Valid=%s\n",
				precise_time.time_valid ? "YES" : "NO",
				precise_time.time_fully_resolved ? "YES" : "NO",
				precise_time.date_valid ? "YES" : "NO");

			/* Display GPS position and fix information for NMEA devices */
			if (platform_extra->gnss_device->device_type == GNSS_DEVICE_NMEA_ONLY) {
				ret = no_os_gnss_get_position_data(gnss_desc, &position_data);
				if (!ret) {
					pr_info("GPS Fix: Quality=%d, Satellites=%d, HDOP=%.1f\n",
						position_data.fix_quality, position_data.satellites_used,
						position_data.hdop);
					pr_info("Position: Lat=%.6f°, Lon=%.6f°, Alt=%.1fm\n",
						position_data.latitude, position_data.longitude,
						position_data.altitude);
					pr_info("Fix Status: %s, Position Valid: %s\n",
						position_data.fix_valid ? "YES" : "NO",
						position_data.position_valid ? "YES" : "NO");
				} else {
					pr_info("GPS Position: Data not available (refreshed every 10 cycles)\n");
				}
			}

			pr_info("=============================\n\n");
			/* Re-enable IRQ for next measurement */
			ret = no_os_irq_enable(platform_extra->gnss_device->irq_ctrl,
					       GPIO_IRQ_PIN);
			if (ret)
				goto remove_interrupt;
		}
	}

remove_interrupt:
	interrupt_remove(platform_extra->gnss_device);
free_gnss:
	no_os_gnss_remove(gnss_desc);
remove_gnss_reset:
	if (gpio_gnss_reset_desc)
		no_os_gpio_remove(gpio_gnss_reset_desc);
remove_led:
	if (gpio_led_desc)
		no_os_gpio_remove(gpio_led_desc);
remove_console_uart:
	no_os_uart_remove(uart_console_desc);
remove_irq:
	no_os_irq_ctrl_remove(gnss_nvic_desc);
error:
	pr_err("ERROR\n");
	return ret;
}
