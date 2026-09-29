/***************************************************************************//**
 * @file spi_dma_platform_init.c
 * @brief DMA controller bring-up for the STM32 CAPI SPI transfer paths.
 *
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include <stddef.h>
#include "parameters.h"

#ifdef SPI_DMA_PLATFORM_INIT

#include "common_data.h"
#include "stm32_hal.h"
#include "capi_dma.h"

static struct capi_dma_handle *spi_dma_handle;

/**
 * @brief Bring up the DMA controller the SPI backend transfers through.
 * @return 0 on success, negative error code otherwise.
 */
int spi_dma_platform_init(void)
{
	int ret;

	DMA_PLATFORM_INIT();

	ret = capi_dma_init(&spi_dma_handle, &dma_config);
	if (ret)
		return ret;

	spi_extra.dma_handle = spi_dma_handle;

	HAL_NVIC_SetPriority(SPI_RXDMA_IRQN, 1, 0);
	HAL_NVIC_EnableIRQ(SPI_RXDMA_IRQN);
	HAL_NVIC_SetPriority(SPI_TXDMA_IRQN, 1, 0);
	HAL_NVIC_EnableIRQ(SPI_TXDMA_IRQN);

	return 0;
}

void SPI_RXDMA_IRQ_HANDLER(void)
{
	if (spi_dma_handle)
		capi_dma_isr(spi_dma_handle);
}

void SPI_TXDMA_IRQ_HANDLER(void)
{
	if (spi_dma_handle)
		capi_dma_isr(spi_dma_handle);
}

#endif /* SPI_DMA_PLATFORM_INIT */
