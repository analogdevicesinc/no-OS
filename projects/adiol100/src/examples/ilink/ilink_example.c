/***************************************************************************//**
 *   @file   ilink_example.c
 *   @brief  ADIOL100 i-link IO-Link stack example.
 *   @author Liviu Stan (liviu.stan@analog.com)
********************************************************************************
 * Copyright 2021 rt-labs AB, Sweden.
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-only
*******************************************************************************/

#include <errno.h>
#include <stdio.h>
#include "ilink_example.h"
#include "common_data.h"
#include "adiol100_ilink_pl.h"
#include "iolink_app.h"
#include "osal.h"
#include "no_os_print_log.h"
#include "no_os_irq.h"
#include "FreeRTOS.h"
#include "task.h"

/* i-link stack thread configuration */
#define APP_MASTER_THREAD_STACK_SIZE  (4 * 1024)
#define APP_MASTER_THREAD_PRIO        4
#define APP_DL_THREAD_STACK_SIZE      (2 * 1024)
#define APP_DL_THREAD_PRIO            5
#define APP_HANDLER_THREAD_STACK_SIZE (4 * 1024)
#define APP_HANDLER_THREAD_PRIO       3

/* IO-Link master context */
static struct iolink_app_master app_master;

/* Port modes: channel A = IO-Link (SDCI), channel B = inactive. */
static iolink_pl_mode_t mode_ch[] = {
	iolink_mode_SDCI,
	iolink_mode_INACTIVE,
};

/* Generic IRQ callback slots — PL layer fills these during init. */
static void (*irq_cb[ADIOL100_NUM_CHANNELS])(void *);
static void *irq_cb_ctx[ADIOL100_NUM_CHANNELS];

static void iolink_ch_a_isr(void *arg)
{
	if (irq_cb[ADIOL100_CH_A])
		irq_cb[ADIOL100_CH_A](irq_cb_ctx[ADIOL100_CH_A]);
}

static void iolink_ch_b_isr(void *arg)
{
	if (irq_cb[ADIOL100_CH_B])
		irq_cb[ADIOL100_CH_B](irq_cb_ctx[ADIOL100_CH_B]);
}

/**
 * @brief FreeRTOS task that initializes and runs the IO-Link app.
 * @param ctx - Unused.
 */
static void app_task(void *ctx)
{
	(void)ctx;
	iolink_hw_drv_t *hw;
	struct no_os_irq_ctrl_desc *irq_ctrl;
	iolink_adiol100_drv_t *drv;
	int ret;

	iolink_adiol100_cfg_t adiol100_cfg = {
		.adiol100_ip = &adiol100_ip,
		.irq_cb = irq_cb,
		.irq_cb_ctx = irq_cb_ctx,
	};

	hw = iolink_adiol100_init(&adiol100_cfg);
	if (hw == NULL) {
		pr_info("iolink_adiol100_init failed\n");
		return;
	}

	drv = (iolink_adiol100_drv_t *)hw;

	ret = no_os_irq_ctrl_init(&irq_ctrl, &gpio_irq_ip);
	if (ret) {
		pr_info("irq_ctrl_init failed: %d\n", ret);
		return;
	}

	struct no_os_callback_desc cb_a = {
		.callback = iolink_ch_a_isr,
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
		.handle = drv->dev->gpio_irqa,
	};
	ret = no_os_irq_register_callback(irq_ctrl, drv->dev->gpio_irqa->number,
					  &cb_a);
	if (ret) {
		pr_info("irq register_callback failed: %d\n", ret);
		goto err_irq_ctrl;
	}

	ret = no_os_irq_trigger_level_set(irq_ctrl, drv->dev->gpio_irqa->number,
					  NO_OS_IRQ_EDGE_FALLING);
	if (ret) {
		pr_info("irq trigger_level_set failed: %d\n", ret);
		goto err_irq_cb;
	}

	ret = no_os_irq_enable(irq_ctrl, drv->dev->gpio_irqa->number);
	if (ret) {
		pr_info("irq enable failed: %d\n", ret);
		goto err_irq_cb;
	}

	pr_info("ADIOL100 port layer initialized\n");

	iolink_port_cfg_t port_cfgs[] = {
		{
			.name = "/adiol100/0",
			.mode = &mode_ch[0],
			.drv = hw,
			.arg = (void *)(uintptr_t)ADIOL100_CH_A,
		},
		{
			.name = "/adiol100/1",
			.mode = &mode_ch[1],
			.drv = hw,
			.arg = (void *)(uintptr_t)ADIOL100_CH_B,
		},
	};

	iolink_m_cfg_t m_cfg = {0};

	m_cfg.port_cnt                 = NELEMENTS(port_cfgs);
	m_cfg.port_cfgs                = port_cfgs;
	m_cfg.master_thread_prio       = APP_MASTER_THREAD_PRIO;
	m_cfg.master_thread_stack_size = APP_MASTER_THREAD_STACK_SIZE;
	m_cfg.dl_thread_prio           = APP_DL_THREAD_PRIO;
	m_cfg.dl_thread_stack_size     = APP_DL_THREAD_STACK_SIZE;

	if (iolink_app_init(&app_master, &m_cfg) != 0) {
		pr_info("iolink_app_init failed\n");
		goto err_irq_cb;
	}

	iolink_app_run(&app_master);
	return;

err_irq_cb:
	no_os_irq_unregister_callback(irq_ctrl, drv->dev->gpio_irqa->number,
				      &cb_a);
err_irq_ctrl:
	no_os_irq_ctrl_remove(irq_ctrl);
}

/**
 * @brief Run the i-link IO-Link stack example.
 *
 * Starts a FreeRTOS task that initializes the ADIOL100 port layer, configures
 * port A for IO-Link (SDCI auto mode), and runs the IO-Link application event
 * loop. Process data from connected devices is printed to the console.
 *
 * @return 0 on success, negative error code otherwise.
 */
int ilink_example_main(void)
{
	TaskHandle_t app_handle = NULL;
	int ret;

	pr_info("ADIOL100 i-link example\n");

	ret = xTaskCreate(
		      app_task,
		      "iolink_app",
		      APP_HANDLER_THREAD_STACK_SIZE / sizeof(StackType_t),
		      NULL,
		      APP_HANDLER_THREAD_PRIO,
		      &app_handle);
	if (ret != pdPASS) {
		pr_info("Failed to create app task\n");
		if (app_handle)
			vTaskDelete(app_handle);
		return -ENOMEM;
	}

	pr_info("Starting FreeRTOS scheduler\n");
	vTaskStartScheduler();

	return -1;
}
