/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by the ADIN1110
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_irq.h"
#include "maxim_gpio.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"
#include "capi_dma.h"
#include "capi_spi.h"
#include "maxim_capi_dma.h"
#include "maxim_capi_spi.h"
#include "adin1110.h"

#if TARGET_NUM == 32690
/* AD-APARD32690-SL */
#define UART_DEVICE_ID  0
#define UART_BAUDRATE   115200

#define SPI_DEVICE_ID   4
#define SPI_BAUDRATE    25000000
#define SPI_CS          0

#define RST_GPIO_PORT   1
#define RST_GPIO_NUM    5

#define ADIN1110_INT_PORT	1
#define ADIN1110_INT_PIN	4
#else
/* MAX32650FTHR */
#define UART_DEVICE_ID  0
#define UART_BAUDRATE   57600

#define SPI_DEVICE_ID   1
#define SPI_BAUDRATE    15000000
#define SPI_CS          0

#define RST_GPIO_PORT   0
#define RST_GPIO_NUM    19
#endif

#define UART_OPS        &max_uart_ops
#define UART_EXTRA      &adin1110_uart_extra_ip

#define GPIO_OPS	&max_gpio_ops
#define RST_GPIO_EXTRA	&adin1110_reset_gpio_extra

extern struct max_uart_init_param adin1110_uart_extra_ip;
extern struct max_gpio_init_param adin1110_reset_gpio_extra;

#define CAPI_DMA_NUM_CHANS	2

extern struct capi_dma_config adin1110_capi_dma_cfg;
extern struct capi_spi_config adin1110_capi_spi_cfg;
extern struct capi_spi_device adin1110_capi_spi_dev;
extern struct capi_dma_handle *adin1110_capi_dma;

int adin1110_spi_init(struct adin1110_init_param *param);

#if defined(CONFIG_OA_TC6)
extern volatile uint32_t adin1110_irq_events;
extern volatile uint32_t adin1110_idle_cycles;

int adin1110_int_init(struct adin1110_desc *desc);
void adin1110_cpu_idle(uint32_t events_seen);
uint32_t adin1110_cpu_cycles(void);
uint32_t adin1110_cpu_hz(void);
#endif

#endif /* __PARAMETERS_H__ */
