/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to STM32 platform used by the ADIN1110
 *           project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "stm32_hal.h"
#include "stm32_irq.h"
#include "stm32_spi.h"
#include "stm32_gpio.h"
#include "stm32_uart.h"
#include "stm32_uart_stdio.h"
#if defined(CONFIG_OA_TC6)
#include "capi_dma.h"
#include "capi_spi.h"
#include "stm32_capi_dma.h"
#include "stm32_capi_spi.h"
#include "stm32_gpio_irq.h"
#include "adin1110.h"
#endif

extern UART_HandleTypeDef huart1;
#define UART_INSTANCE	(&huart1)
#define UART_IRQ_ID	USART1_IRQn

#define UART_DEVICE_ID	1
#define UART_BAUDRATE	115200
#define UART_EXTRA	&adin1110_uart_extra_ip
#define UART_OPS	&stm32_uart_ops

#define SPI_DEVICE_ID	2
/*
 * SPI2 kernel clock is PCLK1 (96 MHz). The prescaler is a power of two and
 * the driver picks the fastest rate not above this value: /4 -> 24 MHz, under
 * the ADIN1110's 25 MHz limit.
 */
#define SPI_BAUDRATE	25000000
#define SPI_CS		12
#define SPI_CS_PORT	1
#define SPI_OPS		&stm32_spi_ops
#define SPI_EXTRA	&adin1110_spi_extra_ip

#define GPIO_OPS	&stm32_gpio_ops
#define RST_GPIO_PORT	2
#define RST_GPIO_NUM	7
#define RST_GPIO_EXTRA	&adin1110_reset_gpio_extra_ip

extern struct stm32_uart_init_param adin1110_uart_extra_ip;
extern struct stm32_spi_init_param adin1110_spi_extra_ip;
extern struct stm32_gpio_init_param adin1110_reset_gpio_extra_ip;

#if defined(CONFIG_OA_TC6)
extern SPI_HandleTypeDef hspi2;

/* CAPI DMA channel IDs of the SPI2 RX and TX streams */
#define CAPI_SPI_RX_DMA_CH	1
#define CAPI_SPI_TX_DMA_CH	2
#define CAPI_DMA_NUM_CHANS	3

extern struct capi_dma_config adin1110_capi_dma_cfg;
extern struct capi_spi_config adin1110_capi_spi_cfg;
extern struct capi_spi_device adin1110_capi_spi_dev;
extern struct capi_dma_handle *adin1110_capi_dma;

void adin1110_capi_dma_irq_enable(void);

/* nINT (T1L_INT_N) is PB11, EXTI line 11 */
#define ADIN1110_INT_PORT	1
#define ADIN1110_INT_PIN	11

/* Incremented by every DMA and nINT interrupt */
extern volatile uint32_t adin1110_irq_events;
/* Core cycles spent asleep in adin1110_cpu_idle() */
extern volatile uint32_t adin1110_idle_cycles;

int adin1110_int_init(struct adin1110_desc *desc);
void adin1110_cpu_idle(uint32_t events_seen);
uint32_t adin1110_cpu_cycles(void);
uint32_t adin1110_cpu_hz(void);
#endif

#endif /* __PARAMETERS_H__ */
