/***************************************************************************//**
 * @file spi_dma_platform_init.c
 * @brief DMA controller bring-up for the MAX32657 CAPI SPI transfer paths.
 *
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include <stddef.h>
#include "parameters.h"

#ifdef SPI_DMA_PLATFORM_INIT

#include "common_data.h"
#include "capi_dma.h"

/**
 * @brief Bring up DMA1 and hand it to the SPI backend.
 *
 * The shared dma_config enables IRQ-driven channel completion, which the SPI
 * async DMA path relies on. The backend reads the DMA controller from the
 * generic capi_spi_config.dma_handle at capi_spi_init(), so patching it here
 * makes every later init take the DMA path.
 *
 * @return 0 on success, negative error code otherwise.
 */
int spi_dma_platform_init(void)
{
	/* Always NULL: a cached handle could be stale after a DMA suite deinit. */
	struct capi_dma_handle *spi_dma_handle = NULL;
	int ret;

	ret = capi_dma_init(&spi_dma_handle, &dma_config);
	if (ret)
		return ret;

	spi_controller_config.dma_handle = spi_dma_handle;

	return 0;
}

#endif /* SPI_DMA_PLATFORM_INIT */
