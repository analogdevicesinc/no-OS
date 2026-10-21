/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by iio_demo examples.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"

#ifdef ENABLE_LOOPBACK
static uint16_t loopback_buffs[DEMO_CHANNELS][SAMPLES_PER_CHANNEL];
#endif

struct no_os_uart_init_param iio_demo_uart_ip = {
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

struct adc_demo_init_param adc_init_par = {
	.ext_buff_len = SAMPLES_PER_CHANNEL,
	.ext_buff = (uint16_t **)loopback_buffs,
	.dev_global_attr = 3333,
	.dev_ch_attr = {
		1111, 1112, 1113, 1114, 1115, 1116, 1117, 1118,
		1119, 1120,	1121, 1122, 1123, 1124, 1125, 1126
	}
};

struct dac_demo_init_param dac_init_par = {
	.loopback_buffer_len = SAMPLES_PER_CHANNEL,
	.loopback_buffers = (uint16_t **)loopback_buffs,
	.dev_global_attr = 4444,
	.dev_ch_attr = {
		1111, 1112, 1113, 1114, 1115, 1116, 1117, 1118,
		1119, 1120, 1121, 1122, 1123, 1124, 1125, 1126
	}
};

uint8_t in_buff[MAX_SIZE_BASE_ADDR] = {0};
uint8_t out_buff[MAX_SIZE_BASE_ADDR] = {0};

#ifdef CONFIG_IIO_DEMO_IIO_SW_TRIGGER_EXAMPLE
struct iio_sw_trig_init_param adc_trig_ip = {
	.name = ADC_DEMO_TRIG_NAME
};

struct iio_sw_trig_init_param dac_trig_ip = {
	.name = DAC_DEMO_TRIG_NAME
};
#endif

#ifdef CONFIG_IIO_DEMO_IIO_TIMER_TRIGGER_EXAMPLE
/* Adc Demo timer init parameter */
struct no_os_timer_init_param adc_demo_tip = {
	.id = ADC_DEMO_TIMER_DEVICE_ID,
	.freq_hz = ADC_DEMO_TIMER_FREQ_HZ,
	.ticks_count = ADC_DEMO_TIMER_TICKS_COUNT,
	.platform_ops = TIMER_OPS,
	.extra = ADC_DEMO_TIMER_EXTRA,
};

/* Adc Demo timer irq init parameter */
struct no_os_irq_init_param adc_demo_timer_irq_ip = {
	.irq_ctrl_id = 0,
	.platform_ops = TIMER_IRQ_OPS,
	.extra = ADC_DEMO_TIMER_IRQ_EXTRA,
};

/* Adc Demo timer trigger callback info */
const struct iio_hw_trig_cb_info adc_demo_timer_cb_info = {
	.event = NO_OS_EVT_TIM_ELAPSED,
	.peripheral = NO_OS_TIM_IRQ,
	.handle = ADC_DEMO_TIMER_CB_HANDLE,
};

/* Adc Demo timer trigger init parameter */
struct iio_hw_trig_init_param adc_demo_timer_trig_ip = {
	.irq_id = ADC_DEMO_TIMER_TRIG_IRQ_ID,
	.cb_info = adc_demo_timer_cb_info,
	.name = ADC_DEMO_TIMER_TRIG_NAME,
};
struct no_os_uart_init_param adxl355_uart_ip = {
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
/* Dac Demo timer init parameter */
struct no_os_timer_init_param dac_demo_tip = {
	.id = DAC_DEMO_TIMER_DEVICE_ID,
	.freq_hz = DAC_DEMO_TIMER_FREQ_HZ,
	.ticks_count = DAC_DEMO_TIMER_TICKS_COUNT,
	.platform_ops = TIMER_OPS,
	.extra = DAC_DEMO_TIMER_EXTRA,
};

/* Dac Demo timer irq init parameter */
struct no_os_irq_init_param dac_demo_timer_irq_ip = {
	.irq_ctrl_id = 0,
	.platform_ops = TIMER_IRQ_OPS,
	.extra = DAC_DEMO_TIMER_IRQ_EXTRA,
};

/* Dac Demo timer trigger callback info */
const struct iio_hw_trig_cb_info dac_demo_timer_cb_info = {
	.event = NO_OS_EVT_TIM_ELAPSED,
	.peripheral = NO_OS_TIM_IRQ,
	.handle = DAC_DEMO_TIMER_CB_HANDLE,
};

/* Adc Demo timer trigger init parameter */
struct iio_hw_trig_init_param dac_demo_timer_trig_ip = {
	.irq_id = DAC_DEMO_TIMER_TRIG_IRQ_ID,
	.cb_info = dac_demo_timer_cb_info,
	.name = DAC_DEMO_TIMER_TRIG_NAME,
};
#endif

#ifdef CONFIG_IIO_DEMO_LIBIIO_EXAMPLE
/*
 * What the libiio example serves: adc_demo and dac_demo, unmodified, with
 * the init parameters above; with IIO_ADC_MAXIM also the part's own ADC.
 * Their transport settings come from the platform's parameters.
 */
#include <noos_iiod.h>
#include <iio_noos_driver.h>
#include "iio_adc_demo.h"
#include "iio_dac_demo.h"
#ifdef CONFIG_IIO_ADC_MAXIM
#include "maxim_iio_adc.h"
#endif

NOOS_IIOD_DRIVER_DEFINE(adc_demo, "adc_demo", adc_demo_init,
			struct adc_demo_desc, &adc_init_par,
			&adc_demo_iio_descriptor);
NOOS_IIOD_DRIVER_DEFINE(dac_demo, "dac_demo", dac_demo_init,
			struct dac_demo_desc, &dac_init_par,
			&dac_demo_iio_descriptor);

static const struct noos_iiod_device iiod_devices[] = {
	NOOS_IIOD_DRIVER(adc_demo),
	NOOS_IIOD_DRIVER(dac_demo),
#ifdef CONFIG_IIO_ADC_MAXIM
	NOOS_IIOD_ONCHIP_ADC(),
#endif
};

#ifdef CONFIG_LIBIIO_TRANSPORT_USB
static struct no_os_irq_init_param libiio_irq_ip = {
	.irq_ctrl_id = 0,
	.platform_ops = LIBIIO_IRQ_OPS,
};

static struct no_os_timer_init_param libiio_usb_tick_timer_ip = {
	.id = LIBIIO_USB_TICK_TIMER_ID,
	.freq_hz = LIBIIO_TIMER_FREQ_HZ,
	.ticks_count = LIBIIO_USB_TICK_TICKS,
	.platform_ops = LIBIIO_TIMER_OPS,
};

static const struct noos_iiod_usb libiio_usb = {
	.tick = {
		.timer = &libiio_usb_tick_timer_ip,
		.irq = &libiio_irq_ip,
		.irq_id = LIBIIO_USB_TICK_IRQ_ID,
		.irq_handle = LIBIIO_USB_TICK_IRQ_HANDLE,
		.irq_priority = LIBIIO_TIMER_IRQ_PRIORITY,
	},
	.vendor_id = LIBIIO_USB_VENDOR_ID,
	.product_id = LIBIIO_USB_PRODUCT_ID,
	.manufacturer = "Analog Devices",
	.product = "iio_demo",
	.serial = "000000001",
};
#endif

const struct noos_iiod_param iiod_param = {
	.devices = iiod_devices,
	.nb_devices = NO_OS_ARRAY_SIZE(iiod_devices),
#ifdef CONFIG_LIBIIO_TRANSPORT_UART
	.uart = &libiio_uart_ip,
#else
	.console = &libiio_uart_ip,
#endif
#ifdef CONFIG_LIBIIO_TRANSPORT_USB
	.usb = &libiio_usb,
#endif
#ifdef CONFIG_LIBIIO_TRANSPORT_NETWORK
	.lwip = &libiio_lwip_ip,
#endif
};
#endif /* CONFIG_IIO_DEMO_LIBIIO_EXAMPLE */
