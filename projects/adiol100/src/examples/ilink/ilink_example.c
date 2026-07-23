#include <stdio.h>
#include "ilink_example.h"
#include "common_data.h"
#include "adiol100_ilink_pl.h"
#include "app_handler.h"
#include "osal.h"
#include "osal_log.h"
#include "no_os_print_log.h"
#include "no_os_gpio.h"
#include "no_os_irq.h"
#include "FreeRTOS.h"
#include "task.h"

#define APP_MASTER_THREAD_STACK_SIZE  (4 * 1024)
#define APP_MASTER_THREAD_PRIO        4
#define APP_DL_THREAD_STACK_SIZE      (2 * 1024)
#define APP_DL_THREAD_PRIO            5
#define APP_HANDLER_THREAD_STACK_SIZE (4 * 1024)
#define APP_HANDLER_THREAD_PRIO       3

/* GPIO pin for IRQA — update to match your wiring. */
#define IRQA_PORT  0
#define IRQA_PIN   12

static iolink_pl_mode_t mode_ch[] = {
	iolink_mode_SDCI,
	iolink_mode_INACTIVE,
};

static void app_handler_task(void *ctx)
{
	iolink_m_cfg_t *cfg = (iolink_m_cfg_t *)ctx;

	app_handler(*cfg);
}

int ilink_example_main(void)
{
	iolink_hw_drv_t *hw;
	int ret;

	pr_info("ADIOL100 i-link example\n");

	/* IRQ GPIO config for IRQA pin (falling edge, active-low). */
	struct no_os_gpio_init_param irq_gpio_a_ip = {
		.port = IRQA_PORT,
		.number = IRQA_PIN,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	};

	struct no_os_irq_init_param irq_ip = {
		.irq_ctrl_id = 0,
		.platform_ops = GPIO_IRQ_OPS,
		.extra = NULL,
	};

	iolink_adiol100_cfg_t adiol100_cfg = {
		.adiol100_ip = &adiol100_ip,
		.irq_gpio_a = &irq_gpio_a_ip,
		.irq_gpio_b = NULL,
		.irq_ip = &irq_ip,
	};

	hw = iolink_adiol100_init(&adiol100_cfg);
	if (hw == NULL) {
		pr_info("iolink_adiol100_init failed\n");
		return -1;
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

	static iolink_m_cfg_t m_cfg;

	m_cfg.cb_arg                   = NULL;
	m_cfg.cb_smi                   = NULL;
	m_cfg.cb_pd                    = NULL;
	m_cfg.port_cnt                 = NELEMENTS(port_cfgs);
	m_cfg.port_cfgs                = port_cfgs;
	m_cfg.master_thread_prio       = APP_MASTER_THREAD_PRIO;
	m_cfg.master_thread_stack_size = APP_MASTER_THREAD_STACK_SIZE;
	m_cfg.dl_thread_prio           = APP_DL_THREAD_PRIO;
	m_cfg.dl_thread_stack_size     = APP_DL_THREAD_STACK_SIZE;

	xTaskCreate(
		app_handler_task,
		"app_handler",
		APP_HANDLER_THREAD_STACK_SIZE / sizeof(StackType_t),
		&m_cfg,
		APP_HANDLER_THREAD_PRIO,
		NULL);

	pr_info("Starting FreeRTOS scheduler\n");
	vTaskStartScheduler();

	return 0;
}
