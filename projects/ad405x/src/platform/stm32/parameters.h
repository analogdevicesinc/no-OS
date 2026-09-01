/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to STM32 platform used by ad405x
 *           project.
 *   @author Jorge Marques (jorge.marques@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "stm32_hal.h"
#include "stm32_irq.h"
#include "stm32_gpio_irq.h"
#include "stm32_gpio.h"
#include "stm32_uart.h"
#include "stm32_uart_stdio.h"
#ifdef AD405X_I3C
#include "stm32_i3c.h"
#include "stm32_dma.h"
#endif
#ifdef AD405X_SPI
#include "stm32_spi.h"
#endif

#ifdef AD405X_I3C
extern I3C_HandleTypeDef hi3c1;
extern DMA_HandleTypeDef handle_GPDMA1_Channel0;
extern DMA_HandleTypeDef handle_GPDMA1_Channel1;
extern DMA_HandleTypeDef handle_GPDMA1_Channel2;
#define I3C_INSTANCE	(&hi3c1)

#define I3C_CRDMA_INSTANCE	(&handle_GPDMA1_Channel0)
#define I3C_RXDMA_INSTANCE	(&handle_GPDMA1_Channel1)
#define I3C_TXDMA_INSTANCE	(&handle_GPDMA1_Channel2)
#define I3C_IRQ_ID		I3C1_EV_IRQn
#define TX_DMA_IRQ_ID	GPDMA1_Channel2_IRQn
#define RX_DMA_IRQ_ID	GPDMA1_Channel1_IRQn
#define CR_DMA_IRQ_ID	GPDMA1_Channel0_IRQn
#define I3C_CRDMA_CHANNEL_NUM	(uint32_t)GPDMA1_Channel0
#define I3C_RXDMA_CHANNEL_NUM	(uint32_t)GPDMA1_Channel1
#define I3C_TXDMA_CHANNEL_NUM	(uint32_t)GPDMA1_Channel2
#endif
extern UART_HandleTypeDef huart3;
#define UART_INSTANCE	(&huart3)

#define UART_IRQ_ID    USART3_IRQn

#define UART_DEVICE_ID	1U
#define UART_BAUDRATE	115200
#define UART_OPS	&stm32_uart_ops
#define UART_EXTRA	&ad405x_uart_extra_ip

#define I3C_DEVICE_ID			1U
#define EEPROM_IS_I2C			0
#define EEPROM_STATIC_ADDR_(x)		0x50+x
#define EEPROM_STATIC_ADDR		EEPROM_STATIC_ADDR_(2)
#define AD405X_DYN_ADDR			0x32
#define AD405X_NO_STATIC_ADDR		0x0
#define AD405X_IS_I3C			1
#define I3C_OPS		&stm32_i3c_ops
#define SPI_OPS		&stm32_spi_ops
#define GPIO_OPS	&stm32_gpio_ops
#define DMA_OPS	    &stm32_gpdma_ops
#define SPI_DEVICE_ID 	1U

#ifdef NUCLEO_H563ZI
#define GPIO_GPIO0_PORT	3
#define GPIO_GPIO0_PIN	15
#define GPIO_GPIO1_PORT	5
#define GPIO_GPIO1_PIN	3
#ifdef AD405X_SPI
#define GPIO_CNV_PORT	4
#define GPIO_CNV_PIN	9
#define GPIO_CS_PORT	3
#define GPIO_CS_PIN	14
#endif
#endif

#ifdef NUCLEO_H503RB
#define GPIO_GPIO0_PORT	2
#define GPIO_GPIO0_PIN	6
#define GPIO_GPIO1_PORT	2
#define GPIO_GPIO1_PIN	7
#ifdef AD405X_SPI
#define GPIO_CNV_PORT	1
#define GPIO_CNV_PIN	10
#define GPIO_CS_PORT	2
#define GPIO_CS_PIN	9
#endif
#endif

extern struct stm32_uart_init_param ad405x_uart_extra_ip;
#ifdef AD405X_I3C
extern struct stm32_i3c_init_param ad405x_i3c_extra_ip;
#endif
#ifdef AD405X_SPI
extern struct stm32_spi_init_param ad405x_spi_extra_ip;
#endif

#endif /* __PARAMETERS_H__ */
