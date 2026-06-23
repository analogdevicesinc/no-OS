/*
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _STM32_CAPI_SPI_H_
#define _STM32_CAPI_SPI_H_

#include "stm32_hal.h"
#include "capi_spi.h"
#include "capi_dma.h"
#include "capi_gpio.h"
#include "stm32_capi_gpio.h"
#ifdef HAL_TIM_MODULE_ENABLED
#include "capi_timer.h"
#endif

/**
 * @brief STM32 SPI extra configuration for CAPI interface
 */
struct stm32_spi_extra_config {
	/** SPI Handle - can be NULL for auto-init based on identifier */
	SPI_HandleTypeDef *hspi;
	/** Chip select port */
	uint32_t chip_select_port;
	/** Get peripheral source clock function */
	uint32_t (*get_input_clock)(void);
	/** Chip select alternate function */
	uint32_t alternate;
	/** DMA Handle for async operations */
	struct capi_dma_handle *dma_handle;
	/** RX DMA Channel ID */
	uint32_t rxdma_ch_id;
	/** TX DMA Channel ID */
	uint32_t txdma_ch_id;
	/** SPI interrupt number */
	uint32_t irq_num;
#ifdef HAL_TIM_MODULE_ENABLED
	/** CS timer handle for PWM-based CS control */
	struct capi_timer_handle *cs_timer;
	/** CS timer channel number */
	uint32_t cs_timer_channel;
	/** TX timer handle for PWM-based TX clocking */
	struct capi_timer_handle *tx_timer;
	/** TX timer channel number */
	uint32_t tx_timer_channel;
#endif
};

/**
 * @brief STM32 SPI device extra configuration for device-specific settings
 */
struct stm32_spi_device_extra {
	/** GPIO CS port for manual CS control */
	uint32_t chip_select_port;
	/** GPIO CS pin number */
	uint32_t chip_select_pin;
	/** CS delay timings */
	uint16_t cs_delay_first;
	uint16_t cs_delay_last;
	uint16_t cs_change_delay;
};

extern const struct capi_spi_ops stm32_capi_spi_ops;

/**
 * @brief Extended STM32 SPI operations structure with DMA support
 */
struct stm32_capi_spi_extended_ops {
	struct capi_spi_ops base_ops;
	int (*transfer_dma)(struct capi_spi_device *device,
			    struct capi_spi_transfer *transfer);
	int (*transfer_dma_async)(struct capi_spi_device *device,
				  struct capi_spi_transfer *transfer,
				  void (*callback)(void*),
				  void* callback_arg);
	int (*transfer_multiple_dma)(struct capi_spi_device *device,
				     struct capi_spi_transfer *transfers,
				     uint32_t transfer_count);
	int (*transfer_multiple_dma_async)(struct capi_spi_device *device,
					   struct capi_spi_transfer *transfers,
					   uint32_t transfer_count,
					   void (*callback)(void*),
					   void* callback_arg);
	int (*alternate_cs_enable)(struct capi_spi_device *device, bool enable);
};

extern const struct stm32_capi_spi_extended_ops stm32_capi_spi_extended_ops;

/* Extended CAPI functions for STM32-specific features */

/**
 * @brief Transfer multiple messages in sequence (like stm32_spi_transfer)
 * @param device - The SPI device descriptor
 * @param transfers - Array of transfer descriptors
 * @param transfer_count - Number of transfers
 * @return 0 in case of success, error codes otherwise
 */
int stm32_capi_spi_transfer_multiple(struct capi_spi_device *device,
				     struct capi_spi_transfer *transfers,
				     uint32_t transfer_count);

/**
 * @brief Async transfer single message using DMA
 * @param device - The SPI device descriptor
 * @param transfer - Transfer descriptor
 * @param callback - Completion callback
 * @param callback_arg - Callback argument
 * @return 0 in case of success, error codes otherwise
 */
int stm32_capi_spi_transfer_dma_async(struct capi_spi_device *device,
				      struct capi_spi_transfer *transfer,
				      void (*callback)(void*),
				      void* callback_arg);

/**
 * @brief Sync transfer single message using DMA
 * @param device - The SPI device descriptor
 * @param transfer - Transfer descriptor
 * @return 0 in case of success, error codes otherwise
 */
int stm32_capi_spi_transfer_dma(struct capi_spi_device *device,
				struct capi_spi_transfer *transfer);

/**
 * @brief Async transfer multiple messages using DMA
 * @param device - The SPI device descriptor
 * @param transfers - Array of transfer descriptors
 * @param transfer_count - Number of transfers
 * @param callback - Completion callback
 * @param callback_arg - Callback argument
 * @return 0 in case of success, error codes otherwise
 */
int stm32_capi_spi_transfer_multiple_dma_async(struct capi_spi_device *device,
		struct capi_spi_transfer *transfers,
		uint32_t transfer_count,
		void (*callback)(void*),
		void* callback_arg);

/**
 * @brief Sync transfer multiple messages using DMA
 * @param device - The SPI device descriptor
 * @param transfers - Array of transfer descriptors
 * @param transfer_count - Number of transfers
 * @return 0 in case of success, error codes otherwise
 */
int stm32_capi_spi_transfer_multiple_dma(struct capi_spi_device *device,
		struct capi_spi_transfer *transfers,
		uint32_t transfer_count);

/**
 * @brief Enable/disable alternate function for CS pin
 * @param device - The SPI device descriptor
 * @param enable - true for alternate function, false for GPIO mode
 * @return 0 in case of success, error codes otherwise
 */
int stm32_capi_spi_alternate_cs_enable(struct capi_spi_device *device,
				       bool enable);

#endif /* _STM32_CAPI_SPI_H_ */
