/********************************************************************************
 *   @file   adin1110_early_init.c
 *   @brief  Early initialization for ADIN1110 configuration pins
 *   @author Radu Etz (radu.etz@analog.com)
 ********************************************************************************
 * Copyright (c) 2026 Analog Devices, Inc.
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
 ******************************************************************************/

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(adin1110_early_init, LOG_LEVEL_INF);

/* ADIN1110 Configuration pins on Port 2 */
#define RESET_PIN    1  /* Hardware Reset (active low) */
#define SWPD_PIN    25  /* Software Power Down Enable */
#define TX2P4_PIN   10  /* 2.4V Transmit Amplitude Enable */
#define MSSEL_PIN    9  /* Master/Slave Select */
#define CFG0_PIN     3  /* Configuration 0 (shared with SPI_MISO) */
#define CFG1_PIN     0  /* Configuration 1 */

#define GPIO_VDDIOH  0x200  /* MAX32_GPIO_VSEL_VDDIOH flag */

static int adin1110_pins_early_init(void)
{
	const struct device *gpio2 = DEVICE_DT_GET(DT_NODELABEL(gpio2));
	int ret;

	if (!device_is_ready(gpio2)) {
		/* Too early, GPIO not ready yet */
		return -ENODEV;
	}

	/* Don't touch the reset pin - let the driver handle it */

	/* Configure SWPD (P2.25) = HIGH to enable normal operation */
	ret = gpio_pin_configure(gpio2, SWPD_PIN,
				 GPIO_OUTPUT_HIGH | GPIO_VDDIOH);
	if (ret < 0) {
		LOG_ERR("Failed to configure SWPD: %d", ret);
		return ret;
	}

	/* Configure TX2P4 (P2.10) = LOW initially */
	ret = gpio_pin_configure(gpio2, TX2P4_PIN,
				 GPIO_OUTPUT_LOW | GPIO_VDDIOH);
	if (ret < 0) {
		LOG_ERR("Failed to configure TX2P4: %d", ret);
		return ret;
	}

	/* Configure MSSEL (P2.9) = HIGH (no-OS uses HIGH) */
	ret = gpio_pin_configure(gpio2, MSSEL_PIN,
				 GPIO_OUTPUT_HIGH | GPIO_VDDIOH);
	if (ret < 0) {
		LOG_ERR("Failed to configure MSSEL: %d", ret);
		return ret;
	}

	/* Configure CFG1 (P2.0) = HIGH for Generic SPI mode */
	ret = gpio_pin_configure(gpio2, CFG1_PIN,
				 GPIO_OUTPUT_HIGH | GPIO_VDDIOH);
	if (ret < 0) {
		LOG_ERR("Failed to configure CFG1: %d", ret);
		return ret;
	}

	/* CFG0 (P2.3) is shared with SPI_MISO and will be configured by SPI driver.
	 * The ADIN1110 samples CFG0/CFG1 at power-up or reset.
	 * With CFG1=HIGH, the mode is determined by CFG0 state at that time. */

	LOG_DBG("ADIN1110 control pins configured: SWPD=1, TX2P4=0, MSSEL=1, CFG1=1");

	/* Small delay to ensure pins are stable */
	k_busy_wait(1000); /* 1ms */

	return 0;
}

/* Run at POST_KERNEL priority 40, before SPI (priority 70) and Ethernet (priority 80) */
SYS_INIT(adin1110_pins_early_init, POST_KERNEL, 40);
