/*
 * Copyright (c) 2025 Analog Devices, Inc.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Early initialization for ADIN1110 configuration pins
 * Must run before SPI/Ethernet drivers
 */

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
