/*
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "stm32_capi_spi_priv.h"
#include "stm32_capi_gpio_priv.h"
#include "capi_alloc.h"
#include "capi_time.h"
#include "capi_irq.h"
#ifdef HAL_TIM_MODULE_ENABLED
#include "capi_timer.h"
#endif

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#define STM32_SPI_CRC_POLYNOMIAL		10
#define STM32_SPI_DMA_MIN_TIMEOUT_MS		10U
#define STM32_SPI_GPIO_PINS_PER_PORT		16

#define STM32_SPI_DMA_DRAIN_TIMEOUT		10000U
#define STM32_SPI_POLL_FAST_MAX_LEN		32U
#define STM32_SPI_POLL_FAST_FLUSH		16U
#define STM32_SPI_POLL_FAST_INFLIGHT		2U
#define STM32_SPI_DMA_DEFAULT_MIN_LEN		64U
/* Bounded idle wait used when a burst is retired from the DMA interrupt. */
#define STM32_SPI_DMA_ISR_IDLE_TIMEOUT		1000U

/*
 * Cortex-M7 parts have a write-back D-cache the DMA does not snoop. When it is
 * enabled, DMA source buffers must be cleaned and destination buffers
 * invalidated, on whole 32-byte lines.
 */
#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
#define STM32_SPI_CACHE_LINE	32U
#define STM32_SPI_DMA_ALIGNED	__attribute__((aligned(STM32_SPI_CACHE_LINE)))
#else
#define STM32_SPI_CACHE_LINE	1U
#define STM32_SPI_DMA_ALIGNED
#endif

#ifdef HAL_TIM_MODULE_ENABLED
#define STM32_SPI_HAS_CS_TIMER(priv)	((priv)->cs_timer != NULL)
#else
#define STM32_SPI_HAS_CS_TIMER(priv)	(false)
#endif

/*
 * Forward declarations. The two DMA entry points keep external linkage - they
 * are declared in stm32_capi_spi.h and reachable both directly and through
 * stm32_capi_spi_extended_ops - and are re-declared here only because
 * transceive()/transceive_async() dispatch to them before their definitions.
 */
static int stm32_capi_spi_dma_abort(struct capi_spi_device *device);
static void stm32_capi_spi_isr(void *handle);
static void stm32_capi_spi_dma_release_pad(struct stm32_spi_priv_handle
		*priv_handle);
static void stm32_capi_spi_dma_cache_free(struct stm32_spi_priv_handle
		*priv_handle);
static int stm32_capi_spi_dma_reserve(struct stm32_spi_priv_handle *priv_handle,
				      uint32_t count);
static int stm32_capi_spi_pad_buf(uint8_t **buf, uint8_t **raw, uint32_t *cap,
				  uint32_t len);
static void stm32_capi_spi_dma_flush_rx(struct stm32_spi_priv_handle
					*priv_handle);
static int stm32_capi_spi_dma_stop(struct capi_spi_device *device, bool isr);
static int stm32_capi_spi_dma_arm(struct stm32_spi_priv_handle *priv_handle,
				  uint32_t index);
static void stm32_capi_spi_dma_requests(struct stm32_spi_priv_handle
					*priv_handle, bool enable);
static void stm32_capi_spi_dma_fail(struct capi_spi_device *device);
static void stm32_capi_spi_dma_sync_pad(struct stm32_spi_priv_handle
					*priv_handle);
int stm32_capi_spi_transfer_multiple_dma(struct capi_spi_device *device,
		struct capi_spi_transfer *transfers,
		uint32_t transfer_count);
int stm32_capi_spi_transfer_multiple_dma_async(
	struct capi_spi_device *device,
	struct capi_spi_transfer *transfers,
	uint32_t transfer_count,
	void (*callback)(void*),
	void* callback_arg);

/* lookup table for mapping HAL handles to private handles */
#define MAX_SPI_INSTANCES 8
static struct {
	SPI_HandleTypeDef *hspi;
	struct stm32_spi_priv_handle *priv_handle;
} spi_handle_lookup[MAX_SPI_INSTANCES];
static uint8_t spi_handle_count = 0;

/* One cache line each so neither shares a line with unrelated data. */
static uint8_t stm32_spi_dma_tx_dummy STM32_SPI_DMA_ALIGNED;
static uint8_t stm32_spi_dma_rx_sink STM32_SPI_DMA_ALIGNED;

#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
/** @brief True if the CPU data cache is enabled. */
static bool stm32_capi_spi_dcache_on(void)
{
	return (SCB->CCR & SCB_CCR_DC_Msk) != 0U;
}

/** @brief Round [addr, addr+len) outward to whole cache lines. */
static void stm32_capi_spi_dcache_span(const void *addr, uint32_t len,
				       uint32_t *start, uint32_t *size)
{
	uint32_t a = (uint32_t)(uintptr_t)addr & ~(STM32_SPI_CACHE_LINE - 1U);
	uint32_t e = ((uint32_t)(uintptr_t)addr + len + STM32_SPI_CACHE_LINE - 1U) &
		     ~(STM32_SPI_CACHE_LINE - 1U);

	*start = a;
	*size = e - a;
}

/** @brief Write a DMA source buffer back to memory. */
static void stm32_capi_spi_dcache_clean(const void *addr, uint32_t len)
{
	uint32_t start, size;

	if (!len || !stm32_capi_spi_dcache_on())
		return;

	stm32_capi_spi_dcache_span(addr, len, &start, &size);
	SCB_CleanDCache_by_Addr((uint32_t *)start, (int32_t)size);
}

/** @brief Write back then drop a DMA destination buffer before the burst. */
static void stm32_capi_spi_dcache_clean_inv(const void *addr, uint32_t len)
{
	uint32_t start, size;

	if (!len || !stm32_capi_spi_dcache_on())
		return;

	stm32_capi_spi_dcache_span(addr, len, &start, &size);
	SCB_CleanInvalidateDCache_by_Addr((uint32_t *)start, (int32_t)size);
}

/** @brief Drop a DMA destination buffer so the CPU re-reads memory. */
static void stm32_capi_spi_dcache_inv(const void *addr, uint32_t len)
{
	uint32_t start, size;

	if (!len || !stm32_capi_spi_dcache_on())
		return;

	stm32_capi_spi_dcache_span(addr, len, &start, &size);
	SCB_InvalidateDCache_by_Addr((uint32_t *)start, (int32_t)size);
}

/**
 * @brief True if a DMA destination cannot be invalidated safely in place.
 *
 * Invalidation works on whole lines, so a buffer that shares a line with other
 * data would lose whatever the CPU wrote to that data meanwhile.
 */
static bool stm32_capi_spi_dcache_unsafe(const void *addr, uint32_t len)
{
	return stm32_capi_spi_dcache_on() &&
	       ((((uint32_t)(uintptr_t)addr) | len) & (STM32_SPI_CACHE_LINE - 1U));
}
#else
#define stm32_capi_spi_dcache_clean(a, l)	((void)0)
#define stm32_capi_spi_dcache_clean_inv(a, l)	((void)0)
#define stm32_capi_spi_dcache_inv(a, l)		((void)0)
#define stm32_capi_spi_dcache_unsafe(a, l)	(false)
#endif

/**
 * @brief Effective wire length of a transfer, in bytes.
 * @param transfer - Pointer to the transfer descriptor.
 * @return Number of frames clocked on the bus.
 */
static uint32_t stm32_capi_spi_xfer_len(const struct capi_spi_transfer
					*transfer)
{
	uint32_t tx_len = transfer->tx_buf ? transfer->tx_size : 0U;
	uint32_t rx_len = transfer->rx_buf ? transfer->rx_size : 0U;

	return max(tx_len, rx_len);
}

/**
 * @brief Check whether a transfer should use DMA.
 * @param priv_handle - Pointer to the STM32 SPI private handle.
 * @param transfer - Pointer to the transfer descriptor.
 * @return true if DMA should be used, false to fall back to PIO/IT.
 */
static bool stm32_capi_spi_use_dma(struct stm32_spi_priv_handle *priv_handle,
				   const struct capi_spi_transfer *transfer)
{
	if (!priv_handle->dma_handle || !priv_handle->rxdma_ch ||
	    !priv_handle->txdma_ch)
		return false;

	return stm32_capi_spi_xfer_len(transfer) >= priv_handle->dma_threshold;
}

/**
 * @brief Find the private handle associated with a HAL SPI handle.
 * @param hspi - Pointer to the HAL SPI handle.
 * @return Pointer to the private handle, or NULL if not found.
 */
static struct stm32_spi_priv_handle *find_priv_handle(SPI_HandleTypeDef *hspi)
{
	for (uint8_t i = 0; i < spi_handle_count; i++) {
		if (spi_handle_lookup[i].hspi == hspi) {
			return spi_handle_lookup[i].priv_handle;
		}
	}
	return NULL;
}

/**
 * @brief Register a mapping between a HAL SPI handle and a private handle.
 * @param hspi - Pointer to the HAL SPI handle.
 * @param priv_handle - Pointer to the private handle.
 * @return 0 on success, negative error code otherwise.
 */
static int register_handle_mapping(SPI_HandleTypeDef *hspi,
				   struct stm32_spi_priv_handle *priv_handle)
{
	if (spi_handle_count >= MAX_SPI_INSTANCES) {
		return -ENOMEM;
	}

	spi_handle_lookup[spi_handle_count].hspi = hspi;
	spi_handle_lookup[spi_handle_count].priv_handle = priv_handle;
	spi_handle_count++;
	return 0;
}

/**
 * @brief Unregister the mapping for a HAL SPI handle.
 * @param hspi - Pointer to the HAL SPI handle.
 */
static void unregister_handle_mapping(SPI_HandleTypeDef *hspi)
{
	for (uint8_t i = 0; i < spi_handle_count; i++) {
		if (spi_handle_lookup[i].hspi == hspi) {
			/* Move last entry to this position */
			if (i < spi_handle_count - 1) {
				spi_handle_lookup[i] = spi_handle_lookup[spi_handle_count - 1];
			}
			spi_handle_count--;
			break;
		}
	}
}

/**
 * @brief Configure SPI peripheral based on device settings.
 * @param priv_handle - Pointer to the STM32 SPI private handle.
 * @param device - Pointer to the SPI device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_spi_config_peripheral(struct stm32_spi_priv_handle
		*priv_handle,
		struct capi_spi_device *device)
{
	uint32_t prescaler = 0u;
	int ret = 0;

	/* Verify SPI instance is configured */
	if (!priv_handle->hspi->Instance)
		return -EINVAL;

	/* A burst retired from interrupt context left its RX flush for us. */
	if (priv_handle->drain_pending)
		stm32_capi_spi_dma_flush_rx(priv_handle);

	/* automatically select prescaler based on max_speed_hz */
	if (device->max_speed_hz != 0u) {
		uint32_t div = priv_handle->input_clock / device->max_speed_hz;

		switch (div) {
		case 0 ... 2:
			prescaler = SPI_BAUDRATEPRESCALER_2;
			break;
		case 3 ... 4:
			prescaler = SPI_BAUDRATEPRESCALER_4;
			break;
		case 5 ... 8:
			prescaler = SPI_BAUDRATEPRESCALER_8;
			break;
		case 9 ... 16:
			prescaler = SPI_BAUDRATEPRESCALER_16;
			break;
		case 17 ... 32:
			prescaler = SPI_BAUDRATEPRESCALER_32;
			break;
		case 33 ... 64:
			prescaler = SPI_BAUDRATEPRESCALER_64;
			break;
		case 65 ... 128:
			prescaler = SPI_BAUDRATEPRESCALER_128;
			break;
		default:
			prescaler = SPI_BAUDRATEPRESCALER_256;
			break;
		}
	} else {
		prescaler = SPI_BAUDRATEPRESCALER_64;
	}

	/* Use the SPI instance that was set during init */
	priv_handle->hspi->Init.Mode = SPI_MODE_MASTER;
	priv_handle->hspi->Init.Direction = SPI_DIRECTION_2LINES;
	priv_handle->hspi->Init.DataSize = SPI_DATASIZE_8BIT;
	priv_handle->hspi->Init.CLKPolarity = device->mode & CAPI_SPI_CPOL ?
					      SPI_POLARITY_HIGH :
					      SPI_POLARITY_LOW;
	priv_handle->hspi->Init.CLKPhase = device->mode & CAPI_SPI_CPHA ?
					   SPI_PHASE_2EDGE :
					   SPI_PHASE_1EDGE;
	priv_handle->hspi->Init.NSS = SPI_NSS_SOFT;
	priv_handle->hspi->Init.BaudRatePrescaler = prescaler;
	priv_handle->hspi->Init.FirstBit = device->lsb_first ?
					   SPI_FIRSTBIT_LSB :
					   SPI_FIRSTBIT_MSB;
	priv_handle->hspi->Init.TIMode = SPI_TIMODE_DISABLE;
	priv_handle->hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	priv_handle->hspi->Init.CRCPolynomial = STM32_SPI_CRC_POLYNOMIAL;

	ret = HAL_SPI_Init(priv_handle->hspi);
	if (ret != HAL_OK) {
		return -EIO;
	}

#ifdef SPI_SR_TXE
	__HAL_SPI_ENABLE(priv_handle->hspi);
#endif

	return 0;
}

/**
 * @brief Get SPI base address from identifier.
 * @param identifier - SPI peripheral identifier or base address.
 * @return Pointer to the SPI peripheral, or NULL if invalid.
 */
static SPI_TypeDef *get_spi_base_from_identifier(uint64_t identifier)
{
	/* If identifier is a base address, use it directly */
	if (identifier >= PERIPH_BASE) {
		return (SPI_TypeDef *)identifier;
	}

	/* Otherwise, map device ID to base address */
	switch (identifier) {
#if defined(SPI1)
	case 1:
		return SPI1;
#endif
#if defined(SPI2)
	case 2:
		return SPI2;
#endif
#if defined(SPI3)
	case 3:
		return SPI3;
#endif
#if defined(SPI4)
	case 4:
		return SPI4;
#endif
#if defined(SPI5)
	case 5:
		return SPI5;
#endif
#if defined(SPI6)
	case 6:
		return SPI6;
#endif
	default:
		return NULL;
	}
}

/**
 * @brief Initialize the STM32 SPI controller.
 * @param handle - Pointer to store the allocated SPI controller handle.
 * @param config - Pointer to the SPI configuration.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_spi_init(struct capi_spi_controller_handle **handle,
			       const struct capi_spi_config *config)
{
	struct capi_spi_controller_handle *spi_handle;
	struct stm32_spi_priv_handle *spi_priv_handle;
	struct stm32_spi_extra_config *spi_extra_config;
	SPI_TypeDef *base;
	int ret = 0;

	if (!handle || !config || !config->extra)
		return -EINVAL;

	if (*handle == NULL) {
		spi_handle = (struct capi_spi_controller_handle *)capi_calloc(1,
				sizeof(*spi_handle));
		if (!spi_handle)
			return -ENOMEM;

		spi_priv_handle = (struct stm32_spi_priv_handle *)capi_calloc(1,
				  sizeof(*spi_priv_handle));
		if (!spi_priv_handle) {
			capi_free(spi_handle);
			return -ENOMEM;
		}

		spi_handle->priv = spi_priv_handle;
		spi_handle->init_allocated = true;
	} else {
		spi_handle = *handle;
		spi_handle->init_allocated = false;

		if (!spi_handle->priv)
			return -EINVAL;

		spi_priv_handle = spi_handle->priv;
	}

	spi_handle->ops = config->ops;

	spi_extra_config = config->extra;

	/* Set up SPI instance */
	if (spi_extra_config && spi_extra_config->hspi) {
		/*
		 * Alias the caller's HAL handle - do NOT copy it. IT/DMA transfers
		 * are serviced by the platform NVIC vector (e.g. CubeMX's
		 * SPI1_IRQHandler -> HAL_SPI_IRQHandler(&hspi1)), which acts on the
		 * caller's handle object. Operating on the same object keeps the HAL
		 * state machine and completion-callback routing consistent between
		 * the driver and the ISR.
		 */
		spi_priv_handle->hspi = spi_extra_config->hspi;
	} else {
		/* Auto-configure based on identifier using internal storage. */
		base = get_spi_base_from_identifier(config->identifier);
		if (!base) {
			ret = -EINVAL;
			goto error;
		}
		spi_priv_handle->hspi = &spi_priv_handle->hspi_storage;
		spi_priv_handle->hspi->Instance = base;
	}

	/*
	 * Force the handle into the RESET state so the first HAL_SPI_Init()
	 * runs HAL_SPI_MspInit() (peripheral clock + GPIO + NVIC).
	 *
	 * HAL_SPI_Init() only calls MspInit when State == HAL_SPI_STATE_RESET.
	 * When hspi is copied from a user template (e.g. CubeMX's global, which
	 * boots in the READY state) or reused after a previous deinit, the copy
	 * would otherwise carry a non-RESET state and MspInit would be skipped -
	 * leaving the peripheral clock gated off after a deinit/init cycle and
	 * hanging the first transfer. Family-generic (HAL state machine is
	 * common to all STM32 SPI IPs).
	 */
	spi_priv_handle->hspi->State = HAL_SPI_STATE_RESET;

	/* Initialize other fields */
	spi_priv_handle->callback = NULL;
	spi_priv_handle->callback_arg = NULL;
	spi_priv_handle->current_transfer = NULL;
	spi_priv_handle->async_in_progress = false;
	spi_priv_handle->dma_done = false;
	spi_priv_handle->dma_error = false;
	spi_priv_handle->irq_num = 0;
	spi_priv_handle->sync_cfg_cached = false;
	spi_priv_handle->sync_cfg_key = 0;

	if (spi_extra_config) {
		if (spi_extra_config->get_input_clock)
			spi_priv_handle->input_clock = spi_extra_config->get_input_clock();
		else
			spi_priv_handle->input_clock = config->clk_freq_hz;

		spi_priv_handle->alternate = spi_extra_config->alternate;

		/* Set up DMA if provided */
		if (spi_extra_config->dma_handle) {
			spi_priv_handle->dma_handle = spi_extra_config->dma_handle;
			spi_priv_handle->rxdma_extra = spi_extra_config->rxdma_extra;
			spi_priv_handle->txdma_extra = spi_extra_config->txdma_extra;
			spi_priv_handle->dma_threshold = spi_extra_config->dma_min_len ?
							 spi_extra_config->dma_min_len :
							 STM32_SPI_DMA_DEFAULT_MIN_LEN;

			/* Initialize DMA channels if configured */
			if (spi_extra_config->rxdma_ch_id != 0) {
				ret = capi_dma_init_chan(spi_priv_handle->dma_handle,
							 &spi_priv_handle->rxdma_ch,
							 spi_extra_config->rxdma_ch_id);
				if (ret)
					goto error;
			}

			if (spi_extra_config->txdma_ch_id != 0) {
				ret = capi_dma_init_chan(spi_priv_handle->dma_handle,
							 &spi_priv_handle->txdma_ch,
							 spi_extra_config->txdma_ch_id);
				if (ret)
					goto error;
			}
		}

#ifdef HAL_TIM_MODULE_ENABLED
		/* Store timer handles for PWM-based CS/TX control */
		if (spi_extra_config->cs_timer) {
			spi_priv_handle->cs_timer = spi_extra_config->cs_timer;
			spi_priv_handle->cs_timer_channel = spi_extra_config->cs_timer_channel;

			ret = capi_timer_channel_disable(spi_priv_handle->cs_timer,
							 spi_priv_handle->cs_timer_channel);
			if (ret)
				goto error;
		}
		if (spi_extra_config->tx_timer) {
			spi_priv_handle->tx_timer = spi_extra_config->tx_timer;
			spi_priv_handle->tx_timer_channel = spi_extra_config->tx_timer_channel;

			ret = capi_timer_channel_disable(spi_priv_handle->tx_timer,
							 spi_priv_handle->tx_timer_channel);
			if (ret)
				goto error;
		}
#endif
	} else {
		spi_priv_handle->input_clock = config->clk_freq_hz;
	}

	/* Register handle mapping for callbacks */
	ret = register_handle_mapping(spi_priv_handle->hspi, spi_priv_handle);
	if (ret != 0) {
		goto error;
	}

	if (spi_extra_config && spi_extra_config->irq_num) {
		ret = capi_irq_connect(spi_extra_config->irq_num,
				       stm32_capi_spi_isr, spi_handle);
		if (ret)
			goto error_unregister;

		ret = capi_irq_enable(spi_extra_config->irq_num);
		if (ret)
			goto error_unregister;

		spi_priv_handle->irq_num = spi_extra_config->irq_num;
	}

	*handle = spi_handle;
	return 0;

error_unregister:
	unregister_handle_mapping(spi_priv_handle->hspi);
error:
	if (spi_handle->init_allocated) {
		capi_free(spi_priv_handle);
		capi_free(spi_handle);
	}
	return ret;
}

/**
 * @brief Deinitialize the STM32 SPI controller and free resources.
 * @param handle - Pointer to the SPI controller handle.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_spi_deinit(struct capi_spi_controller_handle *handle)
{
	struct stm32_spi_priv_handle *spi_priv_handle;

	if (!handle)
		return -EINVAL;

	spi_priv_handle = handle->priv;

	if (spi_priv_handle) {
		if (spi_priv_handle->irq_num) {
			capi_irq_disable(spi_priv_handle->irq_num);
			spi_priv_handle->irq_num = 0;
		}

#ifdef HAL_TIM_MODULE_ENABLED
		if (spi_priv_handle->cs_timer)
			capi_timer_channel_disable(spi_priv_handle->cs_timer,
						   spi_priv_handle->cs_timer_channel);
		if (spi_priv_handle->tx_timer)
			capi_timer_channel_disable(spi_priv_handle->tx_timer,
						   spi_priv_handle->tx_timer_channel);
#endif

		/* Clean up DMA channels */
		if (spi_priv_handle->rxdma_ch) {
			capi_dma_deinit_chan(spi_priv_handle->rxdma_ch);
			spi_priv_handle->rxdma_ch = NULL;
		}
		if (spi_priv_handle->txdma_ch) {
			capi_dma_deinit_chan(spi_priv_handle->txdma_ch);
			spi_priv_handle->txdma_ch = NULL;
		}

		stm32_capi_spi_dma_cache_free(spi_priv_handle);

#ifdef SPI_SR_TXE
		__HAL_SPI_DISABLE(spi_priv_handle->hspi);
#endif
		HAL_SPI_DeInit(spi_priv_handle->hspi);

		/* Unregister handle mapping */
		unregister_handle_mapping(spi_priv_handle->hspi);

		/* Clean up CAPI GPIO for CS */
		if (spi_priv_handle->cs_initialized) {
			capi_gpio_port_deinit(&spi_priv_handle->cs_port_handle);
			spi_priv_handle->cs_initialized = false;
		}
	}

	if (handle->init_allocated) {
		capi_free(handle->priv);
		capi_free(handle);
	}

	return 0;
}

/**
 * @brief Setup GPIO CS pin for the device using CAPI GPIO.
 * @param priv_handle - Pointer to the STM32 SPI private handle.
 * @param device - Pointer to the SPI device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
static int setup_cs_gpio(struct stm32_spi_priv_handle *priv_handle,
			 struct capi_spi_device *device)
{
	struct stm32_spi_device_extra *dev_extra = device->extra;
	struct capi_gpio_port_config port_config;
	int ret;

	if (!dev_extra || !dev_extra->chip_select_port)
		return 0; /* No CS setup needed */

	/* Skip if already initialized */
	if (priv_handle->cs_initialized)
		return 0;

	/* Configure GPIO port for CS pin */
	priv_handle->cs_gpio_config.mode = GPIO_MODE_OUTPUT_PP;
	priv_handle->cs_gpio_config.speed = GPIO_SPEED_FREQ_LOW;
	priv_handle->cs_gpio_config.pull = GPIO_NOPULL;
	priv_handle->cs_gpio_config.alternate = 0;

	/* Initialize GPIO port */
	port_config.ops = &stm32_capi_gpio_ops;
	port_config.identifier = dev_extra->chip_select_port;
	port_config.num_pins = STM32_SPI_GPIO_PINS_PER_PORT;
	port_config.flags = NULL;
	port_config.extra = &priv_handle->cs_gpio_config;

	ret = capi_gpio_port_init(&priv_handle->cs_port_handle, &port_config);
	if (ret)
		return ret;

	/* Configure chip select pin */
	priv_handle->chip_select.port_handle = priv_handle->cs_port_handle;
	priv_handle->chip_select.number = dev_extra->chip_select_pin;
	priv_handle->chip_select.flags = CAPI_GPIO_ACTIVE_LOW; /* CS is active low */

	/* Set CS as output */
	ret = capi_gpio_pin_set_direction(&priv_handle->chip_select, CAPI_GPIO_OUTPUT);
	if (ret) {
		capi_gpio_port_deinit(&priv_handle->cs_port_handle);
		return ret;
	}

	/* Set CS high (inactive) - using raw value since ACTIVE_LOW flag is set */
	ret = capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_HIGH);
	if (ret) {
		capi_gpio_port_deinit(&priv_handle->cs_port_handle);
		return ret;
	}

	priv_handle->cs_initialized = true;
	return 0;
}

#if !defined(STM32H5)
/**
 * @brief Check that the peripheral is in the plain 8-bit frame setup the fast
 * polled path assumes.
 *
 * The path moves bytes through DR and does no CRC handling, so it is only used
 * when the live registers say that is exactly what the peripheral expects;
 * anything else goes through the HAL.
 *
 * @param SPIx - SPI peripheral instance.
 * @return true if the fast path can be used.
 */
static bool stm32_spi_poll_fast_ok(const SPI_TypeDef *SPIx)
{
	if (SPIx->CR1 & (SPI_CR1_BIDIMODE | SPI_CR1_RXONLY | SPI_CR1_CRCEN))
		return false;

#if defined(SPI_CR2_DS)
	/* 8-bit frames, and RXNE raised per byte rather than per half-word. */
	return (SPIx->CR2 & SPI_CR2_DS) == SPI_DATASIZE_8BIT &&
	       (SPIx->CR2 & SPI_CR2_FRXTH);
#else
	return !(SPIx->CR1 & SPI_CR1_DFF);
#endif
}

/**
 * @brief Clear a pending overrun or mode-fault flag and empty the RX FIFO.
 * @param SPIx - SPI peripheral instance.
 */
static void stm32_spi_poll_clear_errors(SPI_TypeDef *SPIx)
{
	volatile uint8_t *dr = (volatile uint8_t *)&SPIx->DR;
	uint32_t guard = STM32_SPI_POLL_FAST_FLUSH;
	uint8_t discard;

	/* OVR is cleared by reading DR then SR. */
	discard = *dr;
	(void)SPIx->SR;

	/* MODF is cleared by reading SR then writing CR1 (SPE is already off). */
	if (SPIx->SR & SPI_SR_MODF) {
		(void)SPIx->SR;
		CLEAR_BIT(SPIx->CR1, SPI_CR1_SPE);
	}

#if defined(SPI_SR_FRLVL)
	while ((SPIx->SR & SPI_SR_FRLVL) && guard--)
		discard = *dr;
#else
	while ((SPIx->SR & SPI_SR_RXNE) && guard--)
		discard = *dr;
#endif
	(void)discard;
}

/**
 * @brief Clock out a short full-duplex transfer without going through the HAL.
 *
 * Mirrors the HAL's behaviour: the timeout is the caller's, in milliseconds
 * (HAL_MAX_DELAY waits forever), overrun and mode-fault errors are reported as
 * -EIO and cleared, and SPE is left as it was found.
 *
 * @param SPIx - SPI peripheral instance (8-bit frames, see
 *               stm32_spi_poll_fast_ok()).
 * @param tx - Transmit buffer.
 * @param rx - Receive buffer.
 * @param len - Number of bytes to exchange.
 * @param timeout_ms - Timeout in milliseconds, or HAL_MAX_DELAY.
 * @return 0 on success, -ETIMEDOUT on timeout, -EIO on a bus error.
 */
static int stm32_spi_poll_small_xfer(SPI_TypeDef *SPIx, const uint8_t *tx,
				     uint8_t *rx, uint32_t len,
				     uint32_t timeout_ms)
{
	volatile uint8_t *dr = (volatile uint8_t *)&SPIx->DR;
	const uint32_t start = HAL_GetTick();
	const bool was_enabled = (SPIx->CR1 & SPI_CR1_SPE) != 0U;
	uint32_t tx_left = len;
	uint32_t rx_left = len;
	int ret = 0;

	if (!was_enabled)
		SET_BIT(SPIx->CR1, SPI_CR1_SPE);

	while (rx_left) {
		if (SPIx->SR & (SPI_SR_OVR | SPI_SR_MODF)) {
			ret = -EIO;
			break;
		}

		if (tx_left &&
		    (tx_left + STM32_SPI_POLL_FAST_INFLIGHT) > rx_left &&
		    (SPIx->SR & SPI_SR_TXE)) {
			*dr = *tx++;
			tx_left--;
		}

		if (SPIx->SR & SPI_SR_RXNE) {
			*rx++ = *dr;
			rx_left--;
			continue;
		}

		if (timeout_ms != HAL_MAX_DELAY &&
		    (HAL_GetTick() - start) >= timeout_ms) {
			ret = -ETIMEDOUT;
			break;
		}
	}

	while (!ret && (SPIx->SR & SPI_SR_BSY)) {
		if (timeout_ms != HAL_MAX_DELAY &&
		    (HAL_GetTick() - start) >= timeout_ms)
			ret = -ETIMEDOUT;
	}

	if (!ret && (SPIx->SR & (SPI_SR_OVR | SPI_SR_MODF)))
		ret = -EIO;

	if (ret)
		stm32_spi_poll_clear_errors(SPIx);

	if (!was_enabled)
		CLEAR_BIT(SPIx->CR1, SPI_CR1_SPE);

	return ret;
}
#endif /* !STM32H5 */

/**
 * @brief Perform SPI transfer with proper CS control and timing.
 * @param priv_handle - Pointer to the STM32 SPI private handle.
 * @param device - Pointer to the SPI device descriptor.
 * @param transfer - Pointer to the transfer descriptor.
 * @param assert_cs - Whether to assert CS before transfer.
 * @param deassert_cs - Whether to deassert CS after transfer.
 * @return 0 on success, negative error code otherwise.
 */
static int perform_spi_transfer_with_cs(struct stm32_spi_priv_handle
					*priv_handle,
					struct capi_spi_device *device,
					struct capi_spi_transfer *transfer,
					bool assert_cs, bool deassert_cs)
{
	struct stm32_spi_device_extra *dev_extra = device->extra;
	uint64_t cfg_key;
	int ret = 0;
	HAL_StatusTypeDef hal_ret;

	/*
	 * Reconfigure the peripheral only when the effective device settings
	 * change. The key is stored per-controller (not file-static) so it is
	 * reset by init and a fresh/re-initialized controller always configures
	 * on its first transfer; it also folds in every field config_peripheral
	 * consumes (CS, clock mode, bit order, speed) so a change to any of them
	 * re-applies the config.
	 */
	cfg_key = ((uint64_t)(device->cs_gpio ? device->cs_gpio->number :
			      device->native_cs)) |
		  ((uint64_t)device->mode << 8) |
		  ((uint64_t)(device->lsb_first ? 1U : 0U) << 16) |
		  ((uint64_t)device->max_speed_hz << 32);

	if (!priv_handle->sync_cfg_cached || cfg_key != priv_handle->sync_cfg_key) {
		ret = stm32_capi_spi_config_peripheral(priv_handle, device);
		if (ret)
			return ret;
		priv_handle->sync_cfg_key = cfg_key;
		priv_handle->sync_cfg_cached = true;
	}

	/* Assert CS if requested (drive low for active-low CS) */
	if (assert_cs && priv_handle->cs_initialized) {
		capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_LOW);
	}

	/* CS delay first */
	if (dev_extra && dev_extra->cs_delay_first) {
		capi_wait_us(dev_extra->cs_delay_first);
	}

	/* Use HAL functions for reliable operation */
	if (transfer->tx_buf && transfer->rx_buf) {
		uint32_t xfer_len = max(transfer->tx_size, transfer->rx_size);
		uint32_t timeout_val = transfer->timeout > 0 ?
				       transfer->timeout : HAL_MAX_DELAY;

		if (transfer->tx_size == transfer->rx_size) {
#if !defined(STM32H5)
			/*
			 * Short equal-length transfers are almost entirely
			 * fixed overhead inside the HAL; clock them out
			 * directly instead.
			 */
			if (xfer_len <= STM32_SPI_POLL_FAST_MAX_LEN &&
			    stm32_spi_poll_fast_ok(priv_handle->hspi->Instance)) {
				ret = stm32_spi_poll_small_xfer(
					      priv_handle->hspi->Instance,
					      transfer->tx_buf,
					      transfer->rx_buf, xfer_len,
					      timeout_val);
				goto cs_deassert;
			}
#endif
			hal_ret = HAL_SPI_TransmitReceive(priv_handle->hspi,
							  (uint8_t *)transfer->tx_buf,
							  transfer->rx_buf,
							  xfer_len, timeout_val);
		} else {
			uint8_t *tmp_tx = capi_calloc(1, xfer_len);
			uint8_t *tmp_rx = capi_calloc(1, xfer_len);

			if (!tmp_tx || !tmp_rx) {
				capi_free(tmp_tx);
				capi_free(tmp_rx);
				ret = -ENOMEM;
				goto cs_deassert;
			}
			memcpy(tmp_tx, transfer->tx_buf, transfer->tx_size);
			hal_ret = HAL_SPI_TransmitReceive(priv_handle->hspi,
							  tmp_tx, tmp_rx,
							  xfer_len, timeout_val);
			if (hal_ret == HAL_OK)
				memcpy(transfer->rx_buf, tmp_rx, transfer->rx_size);
			capi_free(tmp_tx);
			capi_free(tmp_rx);
		}
	} else if (transfer->tx_buf) {
		hal_ret = HAL_SPI_Transmit(priv_handle->hspi,
					   (uint8_t *)transfer->tx_buf,
					   transfer->tx_size,
					   transfer->timeout > 0 ? transfer->timeout : HAL_MAX_DELAY);
	} else {
		uint8_t *zero_tx = capi_calloc(1, transfer->rx_size);

		if (!zero_tx) {
			ret = -ENOMEM;
			goto cs_deassert;
		}
		hal_ret = HAL_SPI_TransmitReceive(priv_handle->hspi,
						  zero_tx, transfer->rx_buf,
						  transfer->rx_size,
						  transfer->timeout > 0 ? transfer->timeout : HAL_MAX_DELAY);
		capi_free(zero_tx);
	}

	if (hal_ret != HAL_OK) {
		if (hal_ret == HAL_TIMEOUT)
			ret = -ETIMEDOUT;
		else
			ret = -EIO;
	}

cs_deassert:
	/* CS delay last */
	if (dev_extra && dev_extra->cs_delay_last) {
		capi_wait_us(dev_extra->cs_delay_last);
	}

	/* Deassert CS if requested (drive high for active-low CS) */
	if (deassert_cs && priv_handle->cs_initialized) {
		capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_HIGH);
	}

	/* CS change delay */
	if (dev_extra && dev_extra->cs_change_delay) {
		capi_wait_us(dev_extra->cs_change_delay);
	}

	return ret;
}

/**
 * @brief Perform a synchronous SPI transceive operation.
 * @param device - Pointer to the SPI device descriptor.
 * @param transfer - Pointer to the transfer descriptor.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_spi_transceive(struct capi_spi_device *device,
				     struct capi_spi_transfer *transfer)
{
	struct stm32_spi_priv_handle *priv_handle;
	int ret;

	if (!device || !device->controller || !transfer)
		return -EINVAL;

	priv_handle = device->controller->priv;

	/* Set up CS GPIO if needed */
	ret = setup_cs_gpio(priv_handle, device);
	if (ret)
		return ret;

	/* Perform the transfer with CS control */
	return perform_spi_transfer_with_cs(priv_handle, device, transfer, true, true);
}

/**
 * @brief Trampoline that reports DMA transceive completion through the
 * registered CAPI callback, matching the completion signalling of the IT
 * path (see HAL_SPI_TxCpltCallback() etc.).
 * @param ctx - Pointer to the SPI device descriptor.
 */
static void stm32_capi_spi_transceive_dma_done(void *ctx)
{
	struct capi_spi_device *device = (struct capi_spi_device *)ctx;
	struct stm32_spi_priv_handle *priv_handle = device->controller->priv;

	priv_handle->async_in_progress = false;
	priv_handle->current_transfer = NULL;

	if (priv_handle->callback)
		priv_handle->callback(CAPI_SPI_EVENT_XFR_DONE,
				      priv_handle->callback_arg, 0);
}

/**
 * @brief Perform an asynchronous SPI transceive operation using interrupts.
 * @param device - Pointer to the SPI device descriptor.
 * @param transfer - Pointer to the transfer descriptor.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_spi_transceive_async(struct capi_spi_device *device,
		struct capi_spi_transfer *transfer)
{
	struct stm32_spi_priv_handle *priv_handle;
	struct stm32_spi_device_extra *dev_extra;
	HAL_StatusTypeDef hal_ret;
	uint32_t tx_len, rx_len;
	bool enomem = false;
	int ret;

	if (!device || !device->controller || !transfer)
		return -EINVAL;

	priv_handle = device->controller->priv;
	dev_extra = device->extra;
	tx_len = transfer->tx_buf ? transfer->tx_size : 0U;
	rx_len = transfer->rx_buf ? transfer->rx_size : 0U;

	if (priv_handle->async_in_progress)
		return -EBUSY;

	/* Set up CS GPIO if needed */
	ret = setup_cs_gpio(priv_handle, device);
	if (ret)
		return ret;

	if (stm32_capi_spi_use_dma(priv_handle, transfer)) {
		priv_handle->current_transfer = transfer;
		priv_handle->async_in_progress = true;

		ret = stm32_capi_spi_transfer_multiple_dma_async(
			      device, transfer, 1,
			      stm32_capi_spi_transceive_dma_done, device);
		if (ret) {
			priv_handle->async_in_progress = false;
			priv_handle->current_transfer = NULL;
		}
		return ret;
	}

	/* Configure SPI for this device */
	ret = stm32_capi_spi_config_peripheral(priv_handle, device);
	if (ret)
		return ret;

	priv_handle->current_transfer = transfer;
	priv_handle->async_in_progress = true;

	/* Assert CS (drive low for active-low CS) */
	if (priv_handle->cs_initialized)
		capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_LOW);

	/* CS delay first */
	if (dev_extra && dev_extra->cs_delay_first)
		capi_wait_us(dev_extra->cs_delay_first);

	if (tx_len && rx_len && tx_len == rx_len) {
		hal_ret = HAL_SPI_TransmitReceive_IT(priv_handle->hspi,
						     (uint8_t *)transfer->tx_buf,
						     transfer->rx_buf, tx_len);
	} else if (tx_len && rx_len) {
		uint32_t len = max(tx_len, rx_len);
		struct stm32_spi_dma_pad *pad;

		/* Bounce buffers come from the reusable cache, not the heap. */
		if (stm32_capi_spi_dma_reserve(priv_handle, 1) ||
		    stm32_capi_spi_pad_buf(&priv_handle->dma_pad[0].tx_tmp,
					   &priv_handle->dma_pad[0].tx_raw,
					   &priv_handle->dma_pad[0].tx_cap, len) ||
		    stm32_capi_spi_pad_buf(&priv_handle->dma_pad[0].rx_tmp,
					   &priv_handle->dma_pad[0].rx_raw,
					   &priv_handle->dma_pad[0].rx_cap, len)) {
			hal_ret = HAL_ERROR;
			enomem = true;
		} else {
			pad = &priv_handle->dma_pad[0];
			memcpy(pad->tx_tmp, transfer->tx_buf, tx_len);
			memset(pad->tx_tmp + tx_len, 0, len - tx_len);
			pad->rx_dst = transfer->rx_buf;
			pad->rx_copy_len = rx_len;
			priv_handle->transfer_count = 1;
			hal_ret = HAL_SPI_TransmitReceive_IT(priv_handle->hspi,
							     pad->tx_tmp, pad->rx_tmp,
							     len);
			if (hal_ret != HAL_OK)
				stm32_capi_spi_dma_release_pad(priv_handle);
		}
	} else if (tx_len) {
		hal_ret = HAL_SPI_Transmit_IT(priv_handle->hspi,
					      (uint8_t *)transfer->tx_buf, tx_len);
	} else if (rx_len) {
		/* Receive_IT clocks out rx_buf's contents in master mode. */
		memset(transfer->rx_buf, 0, rx_len);
		hal_ret = HAL_SPI_Receive_IT(priv_handle->hspi,
					     transfer->rx_buf, rx_len);
	} else {
		priv_handle->async_in_progress = false;
		/* Deassert CS on error */
		if (priv_handle->cs_initialized)
			capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_HIGH);
		return -EINVAL;
	}

	if (hal_ret != HAL_OK) {
		priv_handle->async_in_progress = false;
		priv_handle->current_transfer = NULL;
		/* Deassert CS on error */
		if (priv_handle->cs_initialized)
			capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_HIGH);
		if (enomem)
			return -ENOMEM;
		return hal_ret == HAL_BUSY ? -EBUSY : -EIO;
	}

	return 0;
}

/**
 * @brief Perform a synchronous SPI read command (TX then RX).
 * @param device - Pointer to the SPI device descriptor.
 * @param transfer - Pointer to the transfer descriptor.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_spi_read_command(struct capi_spi_device *device,
				       struct capi_spi_transfer *transfer)
{
	struct stm32_spi_priv_handle *priv_handle;
	struct stm32_spi_device_extra *dev_extra;
	HAL_StatusTypeDef hal_ret;
	int ret = 0;

	if (!device || !device->controller || !transfer)
		return -EINVAL;

	priv_handle = device->controller->priv;
	dev_extra = device->extra;

	/* Set up CS GPIO if needed */
	ret = setup_cs_gpio(priv_handle, device);
	if (ret)
		return ret;

	/* Configure SPI for this device */
	ret = stm32_capi_spi_config_peripheral(priv_handle, device);
	if (ret)
		return ret;

	/* Assert CS (drive low for active-low CS) */
	if (priv_handle->cs_initialized)
		capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_LOW);

	/* CS delay first */
	if (dev_extra && dev_extra->cs_delay_first)
		capi_wait_us(dev_extra->cs_delay_first);

	/* First transmit command/address */
	if (transfer->tx_buf && transfer->tx_size > 0) {
		hal_ret = HAL_SPI_Transmit(priv_handle->hspi,
					   (uint8_t *)transfer->tx_buf,
					   transfer->tx_size,
					   transfer->timeout > 0 ? transfer->timeout : HAL_MAX_DELAY);
		if (hal_ret != HAL_OK) {
			switch (hal_ret) {
			case HAL_BUSY:
				ret = -EBUSY;
				break;
			case HAL_TIMEOUT:
				ret = -ETIMEDOUT;
				break;
			default:
				ret = -EIO;
				break;
			}
			goto deassert_cs;
		}
	}

	/* Then receive data */
	if (transfer->rx_buf && transfer->rx_size > 0) {
		/*
		 * In master + 2-line mode HAL_SPI_Receive() redirects to
		 * HAL_SPI_TransmitReceive(hspi, rx_buf, rx_buf, ...): it clocks
		 * the RX buffer's *current* contents out on MOSI. Zero it first
		 * so we clock defined 0x00 dummy bytes (never the caller's stale
		 * data) during the read phase.
		 */
		memset(transfer->rx_buf, 0, transfer->rx_size);
		hal_ret = HAL_SPI_Receive(priv_handle->hspi,
					  transfer->rx_buf,
					  transfer->rx_size,
					  transfer->timeout > 0 ? transfer->timeout : HAL_MAX_DELAY);
		switch (hal_ret) {
		case HAL_OK:
			break;
		case HAL_BUSY:
			ret = -EBUSY;
			break;
		case HAL_TIMEOUT:
			ret = -ETIMEDOUT;
			break;
		default:
			ret = -EIO;
			break;
		}
	}

deassert_cs:
	/* CS delay last */
	if (dev_extra && dev_extra->cs_delay_last)
		capi_wait_us(dev_extra->cs_delay_last);

	/* Deassert CS (drive high for active-low CS) */
	if (priv_handle->cs_initialized)
		capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_HIGH);

	/* CS change delay */
	if (dev_extra && dev_extra->cs_change_delay)
		capi_wait_us(dev_extra->cs_change_delay);

	return ret;
}

/**
 * @brief Perform an asynchronous SPI read command using interrupts.
 * @param device - Pointer to the SPI device descriptor.
 * @param transfer - Pointer to the transfer descriptor.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_spi_read_command_async(struct capi_spi_device *device,
		struct capi_spi_transfer *transfer)
{
	struct stm32_spi_priv_handle *priv_handle;
	struct stm32_spi_device_extra *dev_extra;
	HAL_StatusTypeDef hal_ret;
	int ret;

	if (!device || !device->controller || !transfer)
		return -EINVAL;

	priv_handle = device->controller->priv;
	dev_extra = device->extra;

	if (priv_handle->async_in_progress)
		return -EBUSY;

	/* Set up CS GPIO if needed */
	ret = setup_cs_gpio(priv_handle, device);
	if (ret)
		return ret;

	/* Configure SPI for this device */
	ret = stm32_capi_spi_config_peripheral(priv_handle, device);
	if (ret)
		return ret;

	priv_handle->current_transfer = transfer;
	priv_handle->async_in_progress = true;

	/* Assert CS (drive low for active-low CS) */
	if (priv_handle->cs_initialized)
		capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_LOW);

	/* CS delay first */
	if (dev_extra && dev_extra->cs_delay_first)
		capi_wait_us(dev_extra->cs_delay_first);

	if (transfer->tx_buf && transfer->tx_size > 0) {
		hal_ret = HAL_SPI_Transmit_IT(priv_handle->hspi,
					      (uint8_t *)transfer->tx_buf,
					      transfer->tx_size);
	} else if (transfer->rx_buf && transfer->rx_size > 0) {
		hal_ret = HAL_SPI_Receive_IT(priv_handle->hspi,
					     transfer->rx_buf,
					     transfer->rx_size);
	} else {
		priv_handle->async_in_progress = false;
		/* Deassert CS on error */
		if (priv_handle->cs_initialized)
			capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_HIGH);
		return -EINVAL;
	}

	if (hal_ret != HAL_OK) {
		priv_handle->async_in_progress = false;
		priv_handle->current_transfer = NULL;
		/* Deassert CS on error */
		if (priv_handle->cs_initialized)
			capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_HIGH);
		switch (hal_ret) {
		case HAL_BUSY:
			return -EBUSY;
		case HAL_ERROR:
			return -EIO;
		default:
			return -EIO;
		}
	}

	return 0;
}

/**
 * @brief Tear down an in-flight IT transfer without relying on the SPI ISR.
 *
 * Both HAL_SPI_Abort() and HAL_SPI_Abort_IT() busy-wait for the abort ISR
 * (SPI_AbortTx_ISR / SPI_AbortRx_ISR, driven from HAL_SPI_IRQHandler) to move
 * the handle to HAL_SPI_STATE_ABORT. When the caller has masked global
 * interrupts that ISR never runs, so both HAL paths time out and fail. This
 * does the equivalent unwind synchronously at the register level: mask the SPI
 * interrupt sources, disable the peripheral, clear residual error flags and
 * reset the HAL transfer state machine back to READY.
 *
 * @param hspi - Pointer to the HAL SPI handle to abort.
 */
static void stm32_capi_spi_abort_hw(SPI_HandleTypeDef *hspi)
{
	/* Mask every SPI interrupt source so no ISR is needed to unwind. */
#if defined(SPI_CR2_TXEIE)
	/* Classic SPI IP: interrupt enables live in CR2. */
	CLEAR_BIT(hspi->Instance->CR2,
		  SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE);
#else
	/* Newer SPI IP (H7/H5/...): interrupt enables live in IER. */
	hspi->Instance->IER = 0U;
#endif

	/* Disable the peripheral and clear any residual error flags. */
	__HAL_SPI_DISABLE(hspi);
	__HAL_SPI_CLEAR_OVRFLAG(hspi);
	__HAL_SPI_CLEAR_FREFLAG(hspi);

	/* Reset the HAL transfer state machine by hand. */
	hspi->TxISR = NULL;
	hspi->RxISR = NULL;
	hspi->pTxBuffPtr = NULL;
	hspi->pRxBuffPtr = NULL;
	hspi->TxXferCount = 0U;
	hspi->RxXferCount = 0U;
	hspi->ErrorCode = HAL_SPI_ERROR_NONE;
	hspi->State = HAL_SPI_STATE_READY;
}

/**
 * @brief Abort an ongoing asynchronous SPI operation.
 * @param device - Pointer to the SPI device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_spi_abort_async(struct capi_spi_device *device)
{
	struct stm32_spi_priv_handle *priv_handle;
	int ret = 0;

	if (!device || !device->controller)
		return -EINVAL;

	priv_handle = device->controller->priv;

	if (!priv_handle->async_in_progress)
		return 0;

	/* Abort DMA transfers if they are active */
	if (priv_handle->dma_handle && (priv_handle->rxdma_ch
					|| priv_handle->txdma_ch)) {
		ret = stm32_capi_spi_dma_abort(device);
		if (ret)
			return ret;
	}

	/*
	 * Tear the transfer down synchronously at the register level rather
	 * than via HAL_SPI_Abort[_IT]() — both of those busy-wait for the abort
	 * ISR, which cannot run when the caller has masked global interrupts
	 * (as the abort test does). This unwind does not drive HAL's
	 * AbortCpltCallback, so deassert CS and fire the terminal callback here.
	 */
	stm32_capi_spi_abort_hw(priv_handle->hspi);
	stm32_capi_spi_dma_release_pad(priv_handle);

	if (priv_handle->cs_initialized)
		capi_gpio_pin_set_raw_value(&priv_handle->chip_select,
					    CAPI_GPIO_HIGH);
	priv_handle->async_in_progress = false;
	priv_handle->current_transfer = NULL;

	if (priv_handle->callback)
		priv_handle->callback(CAPI_SPI_EVENT_ERROR,
				      priv_handle->callback_arg, 0);

	return 0;
}

/**
 * @brief Register a callback for SPI events.
 * @param handle - Pointer to the SPI controller handle.
 * @param callback - Callback function to register.
 * @param callback_arg - User context passed to the callback.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_spi_register_callback(struct capi_spi_controller_handle
		*handle,
		capi_spi_callback_t const callback,
		void *callback_arg)
{
	struct stm32_spi_priv_handle *priv_handle;

	if (!handle)
		return -EINVAL;

	priv_handle = handle->priv;
	priv_handle->callback = callback;
	priv_handle->callback_arg = callback_arg;

	return 0;
}

/**
 * @brief Manually control the chip select line.
 * @param device - Pointer to the SPI device descriptor.
 * @param cs_control - CS control mode (auto, assert, deassert).
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_spi_set_cs(struct capi_spi_device *device,
				 enum capi_spi_cs_control cs_control)
{
	struct stm32_spi_priv_handle *priv_handle;

	if (!device || !device->controller)
		return -EINVAL;

	priv_handle = device->controller->priv;

	switch (cs_control) {
	case CAPI_SPI_CS_AUTO:
		/* HAL handles CS automatically - nothing to do */
		break;
	case CAPI_SPI_CS_MANUAL_ASSERT:
		/*
		 * With a GPIO CS, assert it (drive low for active-low CS).
		 * With native CS (no GPIO CS configured), soft-NSS master mode
		 * has no externally-driven CS line to hold, so manual assert is
		 * a harmless no-op that still reports success.
		 */
		if (priv_handle->cs_initialized)
			capi_gpio_pin_set_raw_value(&priv_handle->chip_select,
						    CAPI_GPIO_LOW);
		break;
	case CAPI_SPI_CS_MANUAL_DEASSERT:
		/* Deassert GPIO CS (drive high for active-low CS); no-op for
		 * native CS - see CAPI_SPI_CS_MANUAL_ASSERT above. */
		if (priv_handle->cs_initialized)
			capi_gpio_pin_set_raw_value(&priv_handle->chip_select,
						    CAPI_GPIO_HIGH);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief SPI interrupt service routine handler.
 * @param handle - Pointer to the SPI controller handle.
 */
static void stm32_capi_spi_isr(void *handle)
{
	struct capi_spi_controller_handle *controller_handle = (struct
			capi_spi_controller_handle *)handle;
	struct stm32_spi_priv_handle *priv_handle;

	if (!controller_handle)
		return;

	priv_handle = controller_handle->priv;
	HAL_SPI_IRQHandler(priv_handle->hspi);
}

/**
 * @brief Transfer multiple SPI messages in sequence.
 * @param device - Pointer to the SPI device descriptor.
 * @param transfers - Array of transfer descriptors.
 * @param transfer_count - Number of transfers.
 * @return 0 on success, negative error code otherwise.
 */
int stm32_capi_spi_transfer_multiple(struct capi_spi_device *device,
				     struct capi_spi_transfer *transfers,
				     uint32_t transfer_count)
{
	struct stm32_spi_priv_handle *priv_handle;
	int ret;

	if (!device || !device->controller || !transfers || !transfer_count)
		return -EINVAL;

	priv_handle = device->controller->priv;

	/* Set up CS GPIO if needed */
	ret = setup_cs_gpio(priv_handle, device);
	if (ret)
		return ret;

	for (uint32_t i = 0; i < transfer_count; i++) {
		bool assert_cs = (i == 0);
		bool deassert_cs = (i == transfer_count - 1);

		ret = perform_spi_transfer_with_cs(priv_handle, device, &transfers[i],
						   assert_cs, deassert_cs);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Enable or disable alternate function mode for the CS pin.
 * @param device - Pointer to the SPI device descriptor.
 * @param enable - true for alternate function, false for GPIO mode.
 * @return 0 on success, negative error code otherwise.
 */
int stm32_capi_spi_alternate_cs_enable(struct capi_spi_device *device,
				       bool enable)
{
	struct stm32_spi_priv_handle *priv_handle;
	struct stm32_capi_gpio_port_priv *gpio_priv;
	struct stm32_spi_device_extra *dev_extra;
	GPIO_InitTypeDef gpio_config;
	int ret;

	if (!device || !device->controller)
		return -EINVAL;

	priv_handle = device->controller->priv;
	dev_extra = device->extra;

	if (!priv_handle->cs_initialized)
		return -ENOSYS;

	if (!dev_extra)
		return -EINVAL;

	/* Get GPIO port private data for direct HAL access */
	gpio_priv = priv_handle->cs_port_handle->priv;

	/* Configure GPIO for alternate function or regular output */
	gpio_config.Pin = (1 << (priv_handle->chip_select.number));
	gpio_config.Pull = GPIO_NOPULL;
	gpio_config.Speed = GPIO_SPEED_FREQ_LOW;

	if (enable) {
		gpio_config.Mode = GPIO_MODE_AF_PP;
		gpio_config.Alternate = priv_handle->alternate;
	} else {
		gpio_config.Mode = GPIO_MODE_OUTPUT_PP;
		gpio_config.Alternate = 0;
	}

	HAL_GPIO_Init(gpio_priv->port, &gpio_config);

	/* Set CS high (inactive) when in GPIO mode */
	if (!enable) {
		ret = capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_HIGH);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Make sure a bounce buffer holds at least len bytes.
 *
 * Grow-only: a buffer that is already large enough is kept, so steady-state
 * transfers never touch the heap. Growing allocates, so this must run from
 * thread context (it does, except for a transfer chained from a completion
 * callback that needs a bigger buffer than any earlier one).
 *
 * The buffer starts on a cache line and spans whole lines, so it can be
 * cleaned or invalidated without touching neighbouring heap data.
 *
 * @param buf - Aligned buffer pointer to update.
 * @param raw - Underlying allocation to update (the one to free).
 * @param cap - Current usable capacity, updated on growth.
 * @param len - Required size in bytes.
 * @return 0 on success, -ENOMEM on allocation failure.
 */
static int stm32_capi_spi_pad_buf(uint8_t **buf, uint8_t **raw, uint32_t *cap,
				  uint32_t len)
{
	uint32_t usable = (len + STM32_SPI_CACHE_LINE - 1U) &
			  ~(STM32_SPI_CACHE_LINE - 1U);
	uint8_t *grown;

	if (*cap >= len)
		return 0;

	grown = capi_calloc(1, usable + STM32_SPI_CACHE_LINE);
	if (!grown)
		return -ENOMEM;

	capi_free(*raw);
	*raw = grown;
	*buf = (uint8_t *)(((uintptr_t)grown + STM32_SPI_CACHE_LINE - 1U) &
			   ~(uintptr_t)(STM32_SPI_CACHE_LINE - 1U));
	*cap = usable;

	return 0;
}

/**
 * @brief Make sure the per-transfer descriptor cache holds count entries.
 *
 * Grow-only, see stm32_capi_spi_pad_buf(). Existing bounce buffers are carried
 * over to the new pad array. Must not be called while a burst is armed.
 *
 * @param priv_handle - Pointer to the STM32 SPI private handle.
 * @param count - Number of transfers the caller needs room for.
 * @return 0 on success, -ENOMEM on allocation failure.
 */
static int stm32_capi_spi_dma_reserve(struct stm32_spi_priv_handle *priv_handle,
				      uint32_t count)
{
	struct capi_dma_transfer *rx, *tx;
	struct stm32_spi_dma_pad *pad;

	if (priv_handle->dma_cap >= count)
		return 0;

	rx = capi_calloc(count, sizeof(*rx));
	tx = capi_calloc(count, sizeof(*tx));
	pad = capi_calloc(count, sizeof(*pad));
	if (!rx || !tx || !pad) {
		capi_free(rx);
		capi_free(tx);
		capi_free(pad);
		return -ENOMEM;
	}

	if (priv_handle->dma_pad)
		memcpy(pad, priv_handle->dma_pad,
		       priv_handle->dma_cap * sizeof(*pad));

	capi_free(priv_handle->rx_ch_xfer);
	capi_free(priv_handle->tx_ch_xfer);
	capi_free(priv_handle->dma_pad);
	priv_handle->rx_ch_xfer = rx;
	priv_handle->tx_ch_xfer = tx;
	priv_handle->dma_pad = pad;
	priv_handle->dma_cap = count;

	return 0;
}

/**
 * @brief Finish with the padding of a burst, keeping its buffers for reuse.
 *
 * Safe from interrupt context: it only forgets the caller's RX destination.
 *
 * @param priv_handle - Pointer to the STM32 SPI private handle.
 */
static void stm32_capi_spi_dma_release_pad(struct stm32_spi_priv_handle
		*priv_handle)
{
	uint32_t i;

	if (!priv_handle->dma_pad)
		return;

	for (i = 0; i < priv_handle->dma_cap; i++) {
		priv_handle->dma_pad[i].rx_dst = NULL;
		priv_handle->dma_pad[i].rx_copy_len = 0;
	}
}

/**
 * @brief Free the descriptor and bounce-buffer cache. Thread context only.
 * @param priv_handle - Pointer to the STM32 SPI private handle.
 */
static void stm32_capi_spi_dma_cache_free(struct stm32_spi_priv_handle
		*priv_handle)
{
	uint32_t i;

	for (i = 0; priv_handle->dma_pad && i < priv_handle->dma_cap; i++) {
		capi_free(priv_handle->dma_pad[i].tx_raw);
		capi_free(priv_handle->dma_pad[i].rx_raw);
	}

	capi_free(priv_handle->dma_pad);
	capi_free(priv_handle->tx_ch_xfer);
	capi_free(priv_handle->rx_ch_xfer);
	priv_handle->dma_pad = NULL;
	priv_handle->tx_ch_xfer = NULL;
	priv_handle->rx_ch_xfer = NULL;
	priv_handle->dma_cap = 0;
	priv_handle->transfer_count = 0;
	priv_handle->dma_active = false;
}

/**
 * @brief Copy the requested prefix out of every oversized RX bounce buffer.
 * @param priv_handle - Pointer to the STM32 SPI private handle.
 */
static void stm32_capi_spi_dma_sync_pad(struct stm32_spi_priv_handle
					*priv_handle)
{
	uint32_t i;

	if (!priv_handle->dma_pad)
		return;

	for (i = 0; i < priv_handle->transfer_count; i++) {
		struct stm32_spi_dma_pad *pad = &priv_handle->dma_pad[i];

		if (pad->rx_dst && pad->rx_tmp)
			memcpy(pad->rx_dst, pad->rx_tmp, pad->rx_copy_len);
	}
}

/**
 * @brief Point one transfer's DMA descriptors at padded buffers if needed.
 *
 * Both channels are always armed for the full wire length so that the RX
 * channel - which carries the completion callback - fires exactly once the
 * last frame has been clocked, whatever the transfer's shape:
 *   - a direction with no buffer (or a zero length) is pointed at the shared
 *     dummy/sink byte with the memory increment disabled, costing nothing;
 *   - a TX buffer shorter than the wire length is copied into a zero-padded
 *     bounce buffer so the tail clocks out defined 0x00 bytes;
 *   - an RX buffer shorter than the wire length receives into a bounce buffer
 *     whose first rx_size bytes are copied back on completion, matching what
 *     perform_spi_transfer_with_cs() returns for the same transfer.
 *
 * @param transfer - Pointer to the transfer descriptor.
 * @param tx_xfer - TX DMA descriptor to fill in.
 * @param rx_xfer - RX DMA descriptor to fill in.
 * @param pad - Padding bookkeeping for this transfer.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_spi_dma_setup_pad(const struct capi_spi_transfer
					*transfer,
					struct capi_dma_transfer *tx_xfer,
					struct capi_dma_transfer *rx_xfer,
					struct stm32_spi_dma_pad *pad)
{
	uint32_t len = stm32_capi_spi_xfer_len(transfer);

	if (!len)
		return -EINVAL;

	if (!transfer->tx_buf || !transfer->tx_size) {
		/* Nothing to send: clock out dummy frames from one byte. */
		tx_xfer->src = (capi_dma_glbl_addr_t)&stm32_spi_dma_tx_dummy;
		tx_xfer->src_inc = CAPI_DMA_NO_INCREMENT;
	} else if (transfer->tx_size < len) {
		if (stm32_capi_spi_pad_buf(&pad->tx_tmp, &pad->tx_raw, &pad->tx_cap, len))
			return -ENOMEM;
		memcpy(pad->tx_tmp, transfer->tx_buf, transfer->tx_size);
		memset(pad->tx_tmp + transfer->tx_size, 0,
		       len - transfer->tx_size);
		tx_xfer->src = (capi_dma_glbl_addr_t)pad->tx_tmp;
		tx_xfer->src_inc = CAPI_DMA_BYTE_INCREMENT;
	} else {
		tx_xfer->src = (capi_dma_glbl_addr_t)transfer->tx_buf;
		tx_xfer->src_inc = CAPI_DMA_BYTE_INCREMENT;
	}
	tx_xfer->length = len;

	if (!transfer->rx_buf || !transfer->rx_size) {
		/* Nothing to keep: drain every frame into one byte. */
		rx_xfer->dst = (capi_dma_glbl_addr_t)&stm32_spi_dma_rx_sink;
		rx_xfer->dst_inc = CAPI_DMA_NO_INCREMENT;
	} else if (transfer->rx_size < len ||
		   stm32_capi_spi_dcache_unsafe(transfer->rx_buf,
						transfer->rx_size)) {
		/*
		 * Bounce when the caller's buffer is short of the wire length,
		 * or when it shares cache lines with other data and cannot be
		 * invalidated in place.
		 */
		if (stm32_capi_spi_pad_buf(&pad->rx_tmp, &pad->rx_raw, &pad->rx_cap,
					   len))
			return -ENOMEM;
		pad->rx_dst = transfer->rx_buf;
		pad->rx_copy_len = transfer->rx_size;
		rx_xfer->dst = (capi_dma_glbl_addr_t)pad->rx_tmp;
		rx_xfer->dst_inc = CAPI_DMA_BYTE_INCREMENT;
	} else {
		rx_xfer->dst = (capi_dma_glbl_addr_t)transfer->rx_buf;
		rx_xfer->dst_inc = CAPI_DMA_BYTE_INCREMENT;
	}
	rx_xfer->length = len;

	/* Make memory hold the TX data and ignore stale lines of the RX data. */
	stm32_capi_spi_dcache_clean((const void *)tx_xfer->src,
				    tx_xfer->src_inc == CAPI_DMA_BYTE_INCREMENT ?
				    len : 1U);
	if (rx_xfer->dst_inc == CAPI_DMA_BYTE_INCREMENT)
		stm32_capi_spi_dcache_clean_inv((const void *)rx_xfer->dst, len);

	return 0;
}

/**
 * @brief Abandon the current DMA burst and report the error. Interrupt safe.
 * @param device - Pointer to the SPI device descriptor.
 */
static void stm32_capi_spi_dma_fail(struct capi_spi_device *device)
{
	struct stm32_spi_priv_handle *priv_handle = device->controller->priv;

#ifdef HAL_TIM_MODULE_ENABLED
	if (priv_handle->cs_timer)
		capi_timer_channel_disable(priv_handle->cs_timer,
					   priv_handle->cs_timer_channel);

	if (priv_handle->tx_timer)
		capi_timer_channel_disable(priv_handle->tx_timer,
					   priv_handle->tx_timer_channel);
#endif

	stm32_capi_spi_dma_stop(device, true);

	priv_handle->dma_error = true;
	priv_handle->dma_done = true;
	priv_handle->async_in_progress = false;
	priv_handle->current_transfer = NULL;

	if (priv_handle->callback)
		priv_handle->callback(CAPI_SPI_EVENT_ERROR,
				      priv_handle->callback_arg, -EIO);
}

/**
 * @brief Enable or disable the SPI's RX and TX DMA requests.
 * @param priv_handle - Pointer to the STM32 SPI private handle.
 * @param enable - True to enable, false to disable.
 */
static void stm32_capi_spi_dma_requests(struct stm32_spi_priv_handle
					*priv_handle, bool enable)
{
	SPI_TypeDef *SPIx = priv_handle->hspi->Instance;

#if defined(STM32H5)
	if (enable) {
		SET_BIT(SPIx->CFG1, SPI_CFG1_RXDMAEN);
		SET_BIT(SPIx->CFG1, SPI_CFG1_TXDMAEN);
	} else {
		CLEAR_BIT(SPIx->CFG1, SPI_CFG1_RXDMAEN | SPI_CFG1_TXDMAEN);
	}
#else
	if (enable) {
		SET_BIT(SPIx->CR2, SPI_CR2_RXDMAEN);
		SET_BIT(SPIx->CR2, SPI_CR2_TXDMAEN);
	} else {
		CLEAR_BIT(SPIx->CR2, SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN);
	}
#endif
}

/**
 * @brief Configure, start and release the DMA channels for one transfer.
 *
 * The channels are started before the SPI's DMA requests are enabled, so
 * neither side can produce or consume data before both are ready (RX first,
 * so a received frame always has somewhere to go).
 *
 * @param priv_handle - Pointer to the STM32 SPI private handle.
 * @param index - Index of the transfer descriptor to arm.
 * @return 0 on success, negative error code otherwise. On failure the
 *         channels may be partly started; the caller aborts them.
 */
static int stm32_capi_spi_dma_arm(struct stm32_spi_priv_handle *priv_handle,
				  uint32_t index)
{
	int ret;

	ret = capi_dma_config_xfer(priv_handle->rxdma_ch,
				   &priv_handle->rx_ch_xfer[index]);
	if (ret)
		return ret;

	ret = capi_dma_config_xfer(priv_handle->txdma_ch,
				   &priv_handle->tx_ch_xfer[index]);
	if (ret)
		return ret;

	ret = capi_dma_xfer_start(priv_handle->txdma_ch);
	if (ret)
		return ret;

	ret = capi_dma_xfer_start(priv_handle->rxdma_ch);
	if (ret)
		return ret;

	stm32_capi_spi_dma_requests(priv_handle, true);

	return 0;
}

/**
 * @brief DMA completion callback for SPI transfers.
 * @param event - DMA event identifier.
 * @param ctx - User context (pointer to the SPI device).
 */
static void stm32_capi_spi_dma_callback(uint32_t event, void *ctx)
{
	struct capi_spi_device *device = (struct capi_spi_device *)ctx;
	struct stm32_spi_priv_handle *priv_handle = device->controller->priv;

	/*
	 * More transfers pending: this one is on the wire in full, so the SPI is
	 * idle. Stop its requests, move to the next descriptor and arm it. CS
	 * stays asserted across the whole burst.
	 */
	if (priv_handle->current_transfer_index < priv_handle->transfer_count - 1) {
		stm32_capi_spi_dma_requests(priv_handle, false);
		priv_handle->current_transfer_index++;
		if (stm32_capi_spi_dma_arm(priv_handle,
					   priv_handle->current_transfer_index))
			stm32_capi_spi_dma_fail(device);
		return;
	}

#ifdef HAL_TIM_MODULE_ENABLED
	if (priv_handle->cs_timer)
		capi_timer_channel_disable(priv_handle->cs_timer,
					   priv_handle->cs_timer_channel);

	if (priv_handle->tx_timer)
		capi_timer_channel_disable(priv_handle->tx_timer,
					   priv_handle->tx_timer_channel);
#endif

	/* The DMA wrote memory behind the cache: drop any stale RX lines. */
	for (uint32_t i = 0; i < priv_handle->transfer_count; i++) {
		if (priv_handle->rx_ch_xfer[i].dst_inc == CAPI_DMA_BYTE_INCREMENT)
			stm32_capi_spi_dcache_inv(
				(const void *)priv_handle->rx_ch_xfer[i].dst,
				priv_handle->rx_ch_xfer[i].length);
	}

	/* Harvest the bounce buffers before the abort releases them. */
	stm32_capi_spi_dma_sync_pad(priv_handle);

	/* Interrupt context: defer the RX FIFO flush to the next transfer. */
	stm32_capi_spi_dma_stop(device, true);

	priv_handle->dma_done = true;

	if (priv_handle->dma_user_cb)
		priv_handle->dma_user_cb(priv_handle->dma_user_ctx);
}

/**
 * @brief DMA error callback for SPI transfers.
 * @param event - HAL DMA error code.
 * @param ctx - User context (pointer to the SPI device).
 */
static void stm32_capi_spi_dma_error_callback(uint32_t event, void *ctx)
{
	struct capi_spi_device *device = (struct capi_spi_device *)ctx;
	struct stm32_spi_priv_handle *priv_handle = device->controller->priv;

	(void)event;

	/* Both channels can report the same fault; only the first one acts. */
	if (!priv_handle->dma_active)
		return;

	stm32_capi_spi_dma_fail(device);
}

/**
 * @brief Configure and start DMA transfers.
 * @param device - Pointer to the SPI device descriptor.
 * @param transfers - Array of transfer descriptors.
 * @param transfer_count - Number of transfers.
 * @param completion_callback - Callback invoked on DMA completion.
 * @param callback_ctx - User context passed to the callback.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_config_dma_and_start(struct capi_spi_device *device,
		struct capi_spi_transfer *transfers,
		uint32_t transfer_count,
		capi_dma_xfer_complete_cb completion_callback,
		void *callback_ctx)
{
	struct stm32_spi_priv_handle *priv_handle = device->controller->priv;
	struct capi_dma_transfer *rx_ch_xfer;
	struct capi_dma_transfer *tx_ch_xfer;
	struct stm32_spi_dma_pad *dma_pad;
	SPI_TypeDef *SPIx = priv_handle->hspi->Instance;
	int ret;
	uint32_t i;

	if (!device || !transfers || !priv_handle->dma_handle)
		return -EINVAL;

	if (!priv_handle->rxdma_ch || !priv_handle->txdma_ch)
		return -EINVAL;

	if (priv_handle->dma_active)
		return -EBUSY;

	/*
	 * A hardware-generated CS or TX pacing timer covers one transfer; it
	 * cannot be carried across the re-arm between chained transfers.
	 */
	if (transfer_count > 1 && STM32_SPI_HAS_CS_TIMER(priv_handle))
		return -ENOTSUP;
#ifdef HAL_TIM_MODULE_ENABLED
	if (transfer_count > 1 && priv_handle->tx_timer)
		return -ENOTSUP;
#endif

	/*
	 * The CS pin is only set up lazily by the blocking/IT transfer paths,
	 * so a device driven exclusively over DMA would otherwise never get
	 * one.
	 */
	ret = setup_cs_gpio(priv_handle, device);
	if (ret)
		return ret;

	ret = stm32_capi_spi_config_peripheral(priv_handle, device);
	if (ret)
		return ret;

	ret = stm32_capi_spi_dma_reserve(priv_handle, transfer_count);
	if (ret)
		return ret;

	rx_ch_xfer = priv_handle->rx_ch_xfer;
	tx_ch_xfer = priv_handle->tx_ch_xfer;
	dma_pad = priv_handle->dma_pad;
	priv_handle->transfer_count = transfer_count;
	priv_handle->dma_active = true;
	memset(rx_ch_xfer, 0, transfer_count * sizeof(*rx_ch_xfer));
	memset(tx_ch_xfer, 0, transfer_count * sizeof(*tx_ch_xfer));

	for (i = 0; i < transfer_count; i++) {
		/* Configure TX DMA transfer */
		tx_ch_xfer[i].extra = priv_handle->txdma_extra;
#ifndef SPI_SR_TXE
		tx_ch_xfer[i].dst = (capi_dma_glbl_addr_t) & (SPIx->TXDR);
#else
		tx_ch_xfer[i].dst = (capi_dma_glbl_addr_t) & (SPIx->DR);
#endif
		tx_ch_xfer[i].xfer_type = CAPI_DMA_MEM_TO_DEV;
		tx_ch_xfer[i].dst_inc = CAPI_DMA_NO_INCREMENT;

		/* Configure RX DMA transfer */
		rx_ch_xfer[i].extra = priv_handle->rxdma_extra;
#ifndef SPI_SR_RXNE
		rx_ch_xfer[i].src = (capi_dma_glbl_addr_t) & (SPIx->RXDR);
#else
		rx_ch_xfer[i].src = (capi_dma_glbl_addr_t) & (SPIx->DR);
#endif
		rx_ch_xfer[i].xfer_type = CAPI_DMA_DEV_TO_MEM;
		rx_ch_xfer[i].src_inc = CAPI_DMA_NO_INCREMENT;

		ret = stm32_capi_spi_dma_setup_pad(&transfers[i], &tx_ch_xfer[i],
						   &rx_ch_xfer[i], &dma_pad[i]);
		if (ret)
			goto cleanup_dma;
	}

	priv_handle->current_transfer_index = 0;

	/* Register completion callback on the RX DMA channel */
	if (completion_callback) {
		ret = capi_dma_register_complete_callback(priv_handle->rxdma_ch,
				completion_callback,
				callback_ctx);
		if (ret)
			goto cleanup_dma;
	}

	priv_handle->dma_error = false;
	ret = capi_dma_register_error_callback(priv_handle->rxdma_ch,
					       stm32_capi_spi_dma_error_callback,
					       device);
	if (ret)
		goto cleanup_dma;
	ret = capi_dma_register_error_callback(priv_handle->txdma_ch,
					       stm32_capi_spi_dma_error_callback,
					       device);
	if (ret)
		goto cleanup_dma;

	/*
	 * Drive CS low for the whole burst. Below, CS is only handled when a
	 * cs_timer generates it in hardware; with SPI_NSS_SOFT and no such
	 * timer nothing else asserts it, so the frames would be clocked out
	 * with CS still deasserted and the peripheral would ignore them.
	 * stm32_capi_spi_dma_abort() releases it again once the burst ends.
	 */
	if (!STM32_SPI_HAS_CS_TIMER(priv_handle) && priv_handle->cs_initialized) {
		ret = capi_gpio_pin_set_raw_value(&priv_handle->chip_select,
						  CAPI_GPIO_LOW);
		if (ret)
			goto cleanup_dma;
	}

	ret = stm32_capi_spi_dma_arm(priv_handle, 0);
	if (ret)
		goto abort_transfer;

#ifdef HAL_TIM_MODULE_ENABLED
	if (priv_handle->cs_timer) {
		stm32_capi_spi_alternate_cs_enable(device, true);
		ret = capi_timer_channel_enable(priv_handle->cs_timer,
						priv_handle->cs_timer_channel);
		if (ret)
			goto abort_transfer;
		ret = capi_timer_start(priv_handle->cs_timer);
		if (ret)
			goto abort_transfer;
	}
	if (priv_handle->tx_timer) {
		ret = capi_timer_channel_enable(priv_handle->tx_timer,
						priv_handle->tx_timer_channel);
		if (ret)
			goto abort_transfer;
		ret = capi_timer_start(priv_handle->tx_timer);
		if (ret)
			goto abort_transfer;
	}
#endif

	return 0;

abort_transfer:
	capi_dma_xfer_abort(priv_handle->txdma_ch);
	capi_dma_xfer_abort(priv_handle->rxdma_ch);
cleanup_dma:
	if (!STM32_SPI_HAS_CS_TIMER(priv_handle) && priv_handle->cs_initialized)
		capi_gpio_pin_set_raw_value(&priv_handle->chip_select,
					    CAPI_GPIO_HIGH);
	stm32_capi_spi_dma_release_pad(priv_handle);
	priv_handle->dma_active = false;
	return ret;
}

/**
 * @brief Wait for the SPI bus to go idle, bounded by a spin count.
 * @param priv_handle - Pointer to the STM32 SPI private handle.
 * @param timeout - Maximum number of polls.
 */
static void stm32_capi_spi_dma_wait_idle(struct stm32_spi_priv_handle
		*priv_handle, uint32_t timeout)
{
	SPI_TypeDef *SPIx = priv_handle->hspi->Instance;

#if defined(STM32H5)
	while (!(SPIx->SR & SPI_SR_TXC) && timeout)
		timeout--;
#else
	while ((SPIx->SR & SPI_SR_BSY) && timeout)
		timeout--;
#endif
}

/**
 * @brief Empty the SPI RX FIFO and clear the overrun flag.
 *
 * Done in thread context, either right away when a burst is aborted from
 * there, or at the start of the next transfer after one was retired from
 * interrupt context.
 *
 * @param priv_handle - Pointer to the STM32 SPI private handle.
 */
static void stm32_capi_spi_dma_flush_rx(struct stm32_spi_priv_handle
					*priv_handle)
{
	SPI_TypeDef *SPIx = priv_handle->hspi->Instance;
	uint32_t timeout;

#if defined(STM32H5)
	timeout = STM32_SPI_DMA_DRAIN_TIMEOUT;
	while ((SPIx->SR & SPI_SR_RXP) && timeout) {
		(void)*(volatile uint8_t *)&SPIx->RXDR;
		timeout--;
	}

	SPIx->IFCR = SPI_IFCR_OVRC;
#else
#if defined(SPI_SR_FRLVL)
	timeout = STM32_SPI_DMA_DRAIN_TIMEOUT;
	while ((SPIx->SR & SPI_SR_FRLVL) && timeout) {
		(void)*(volatile uint8_t *)&SPIx->DR;
		timeout--;
	}
#elif !defined(SPI_SR_RXNE)
	(void)timeout;
	(void)*(volatile uint8_t *)&SPIx->RXDR;
#else
	(void)timeout;
	(void)*(volatile uint8_t *)&SPIx->DR;
#endif

#if defined(SPI_SR_OVR)
	(void)SPIx->DR;
	(void)SPIx->SR;
#endif
#endif /* STM32H5 */

	priv_handle->drain_pending = false;
}

/**
 * @brief Stop the DMA channels and retire the current burst.
 *
 * Never touches the heap. With isr set (called from a DMA interrupt) it only
 * waits a short, bounded time for the bus to idle and leaves the RX FIFO flush
 * to the next transfer; otherwise it waits longer and flushes straight away.
 *
 * @param device - Pointer to the SPI device descriptor.
 * @param isr - True when called from interrupt context.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_spi_dma_stop(struct capi_spi_device *device, bool isr)
{
	struct stm32_spi_priv_handle *priv_handle;
	SPI_TypeDef *SPIx;
	int tx_ret;
	int ret = 0;

	if (!device || !device->controller)
		return -EINVAL;

	priv_handle = device->controller->priv;
	if (!priv_handle)
		return -EINVAL;

	SPIx = priv_handle->hspi->Instance;

	if (priv_handle->rxdma_ch) {
		ret = capi_dma_xfer_abort(priv_handle->rxdma_ch);

#if defined (STM32H5)
		CLEAR_BIT(SPIx->CFG1, SPI_CFG1_RXDMAEN);
#else
		CLEAR_BIT(SPIx->CR2, SPI_CR2_RXDMAEN);
#endif
	}

	if (priv_handle->txdma_ch) {
		tx_ret = capi_dma_xfer_abort(priv_handle->txdma_ch);
		if (tx_ret && !ret)
			ret = tx_ret;

#if defined (STM32H5)
		CLEAR_BIT(SPIx->CFG1, SPI_CFG1_TXDMAEN);
#else
		CLEAR_BIT(SPIx->CR2, SPI_CR2_TXDMAEN);
#endif
	}

	if (isr) {
		stm32_capi_spi_dma_wait_idle(priv_handle,
					     STM32_SPI_DMA_ISR_IDLE_TIMEOUT);
		priv_handle->drain_pending = true;
	} else {
		stm32_capi_spi_dma_wait_idle(priv_handle,
					     STM32_SPI_DMA_DRAIN_TIMEOUT);
		stm32_capi_spi_dma_flush_rx(priv_handle);
	}

	/*
	 * Retire the burst without touching the heap: this may run from the
	 * DMA interrupt. The descriptors and bounce buffers stay cached for the
	 * next transfer and are freed in deinit.
	 */
	stm32_capi_spi_dma_release_pad(priv_handle);
	priv_handle->dma_active = false;

	if (STM32_SPI_HAS_CS_TIMER(priv_handle)) {
		stm32_capi_spi_alternate_cs_enable(device, false);
	} else if (priv_handle->cs_initialized) {
		capi_gpio_pin_set_raw_value(&priv_handle->chip_select,
					    CAPI_GPIO_HIGH);
	}

	return ret;
}

/**
 * @brief Abort DMA transfers from thread context.
 * @param device - Pointer to the SPI device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_spi_dma_abort(struct capi_spi_device *device)
{
	return stm32_capi_spi_dma_stop(device, false);
}

/**
 * @brief Perform an asynchronous single DMA transfer.
 * @param device - Pointer to the SPI device descriptor.
 * @param transfer - Pointer to the transfer descriptor.
 * @param callback - Completion callback.
 * @param callback_arg - Callback argument.
 * @return 0 on success, negative error code otherwise.
 */
int stm32_capi_spi_transfer_dma_async(struct capi_spi_device *device,
				      struct capi_spi_transfer *transfer,
				      void (*callback)(void*),
				      void* callback_arg)
{
	return stm32_capi_spi_transfer_multiple_dma_async(device, transfer, 1, callback,
			callback_arg);
}

/**
 * @brief Perform a synchronous single DMA transfer.
 * @param device - Pointer to the SPI device descriptor.
 * @param transfer - Pointer to the transfer descriptor.
 * @return 0 on success, negative error code otherwise.
 */
int stm32_capi_spi_transfer_dma(struct capi_spi_device *device,
				struct capi_spi_transfer *transfer)
{
	return stm32_capi_spi_transfer_multiple_dma(device, transfer, 1);
}

/**
 * @brief Perform an asynchronous multi-transfer DMA operation.
 * @param device - Pointer to the SPI device descriptor.
 * @param transfers - Array of transfer descriptors.
 * @param transfer_count - Number of transfers.
 * @param callback - Completion callback.
 * @param callback_arg - Callback argument.
 * @return 0 on success, negative error code otherwise.
 */
int stm32_capi_spi_transfer_multiple_dma_async(struct capi_spi_device *device,
		struct capi_spi_transfer *transfers,
		uint32_t transfer_count,
		void (*callback)(void*),
		void* callback_arg)
{
	struct stm32_spi_priv_handle *priv_handle;

	if (!device || !device->controller || !transfers || !transfer_count)
		return -EINVAL;

	priv_handle = device->controller->priv;

	priv_handle->dma_user_cb = callback;
	priv_handle->dma_user_ctx = callback_arg;

	return stm32_capi_config_dma_and_start(device, transfers, transfer_count,
					       stm32_capi_spi_dma_callback, device);
}

/**
 * @brief Perform a synchronous multi-transfer DMA operation.
 * @param device - Pointer to the SPI device descriptor.
 * @param transfers - Array of transfer descriptors.
 * @param transfer_count - Number of transfers.
 * @return 0 on success, negative error code otherwise.
 */
int stm32_capi_spi_transfer_multiple_dma(struct capi_spi_device *device,
		struct capi_spi_transfer *transfers,
		uint32_t transfer_count)
{
	struct stm32_spi_priv_handle *priv_handle;
	uint32_t timeout;
	int ret;

	if (!device || !device->controller || !transfers || !transfer_count)
		return -EINVAL;

	priv_handle = device->controller->priv;

	priv_handle->dma_done = false;
	ret = stm32_capi_config_dma_and_start(device, transfers, transfer_count,
					      stm32_capi_spi_dma_callback, device);
	if (ret)
		return ret;

	timeout = 0U;
	for (uint32_t i = 0; i < transfer_count; i++)
		timeout += stm32_capi_spi_xfer_len(&transfers[i]);
	if (timeout < STM32_SPI_DMA_MIN_TIMEOUT_MS)
		timeout = STM32_SPI_DMA_MIN_TIMEOUT_MS;

	while (timeout && !priv_handle->dma_done) {
		timeout--;
		capi_wait_ms(1);
	}

	if (priv_handle->dma_error)
		return -EIO;

	if (!priv_handle->dma_done) {
		stm32_capi_spi_dma_abort(device);
		return -ETIMEDOUT;
	}

	return 0;
}

const struct capi_spi_ops stm32_capi_spi_ops = {
	.init = stm32_capi_spi_init,
	.deinit = stm32_capi_spi_deinit,
	.transceive = stm32_capi_spi_transceive,
	.transceive_async = stm32_capi_spi_transceive_async,
	.read_command = stm32_capi_spi_read_command,
	.read_command_async = stm32_capi_spi_read_command_async,
	.abort_async = stm32_capi_spi_abort_async,
	.register_callback = stm32_capi_spi_register_callback,
	.set_cs = stm32_capi_spi_set_cs,
	.isr = stm32_capi_spi_isr,
};

const struct stm32_capi_spi_extended_ops stm32_capi_spi_extended_ops = {
	.base_ops = {
		.init = stm32_capi_spi_init,
		.deinit = stm32_capi_spi_deinit,
		.transceive = stm32_capi_spi_transceive,
		.transceive_async = stm32_capi_spi_transceive_async,
		.read_command = stm32_capi_spi_read_command,
		.read_command_async = stm32_capi_spi_read_command_async,
		.abort_async = stm32_capi_spi_abort_async,
		.register_callback = stm32_capi_spi_register_callback,
		.set_cs = stm32_capi_spi_set_cs,
		.isr = stm32_capi_spi_isr,
	},
	.transfer_dma = stm32_capi_spi_transfer_dma,
	.transfer_dma_async = stm32_capi_spi_transfer_dma_async,
	.transfer_multiple_dma = stm32_capi_spi_transfer_multiple_dma,
	.transfer_multiple_dma_async = stm32_capi_spi_transfer_multiple_dma_async,
	.alternate_cs_enable = stm32_capi_spi_alternate_cs_enable,
};

/**
 * @brief HAL SPI TX complete callback.
 * @param hspi - Pointer to the HAL SPI handle.
 */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	struct stm32_spi_priv_handle *priv_handle;
	struct capi_spi_transfer *transfer;

	priv_handle = find_priv_handle(hspi);
	if (!priv_handle) {
		return;
	}

	transfer = priv_handle->current_transfer;

	/* For read command async, check if we need to start RX phase */
	if (transfer && transfer->rx_buf && transfer->rx_size > 0
	    && transfer->tx_size > 0) {
		/*
		 * Start RX phase for read command. HAL_SPI_Receive_IT() in
		 * master + 2-line mode redirects to TransmitReceive_IT(rx, rx),
		 * clocking the RX buffer's current contents out on MOSI; zero it
		 * first so defined 0x00 dummy bytes are clocked, not stale data.
		 */
		HAL_StatusTypeDef hal_ret;
		memset(transfer->rx_buf, 0, transfer->rx_size);
		hal_ret = HAL_SPI_Receive_IT(hspi, transfer->rx_buf,
					     transfer->rx_size);
		if (hal_ret != HAL_OK) {
			/* Error in RX phase - deassert CS */
			if (priv_handle->cs_initialized)
				capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_HIGH);
			priv_handle->async_in_progress = false;
			priv_handle->current_transfer = NULL;
			if (priv_handle->callback) {
				priv_handle->callback(CAPI_SPI_EVENT_ERROR,
						      priv_handle->callback_arg,
						      -EIO);
			}
		}
		return;
	}

	/* TX complete - deassert CS */
	if (priv_handle->cs_initialized)
		capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_HIGH);

	priv_handle->async_in_progress = false;
	priv_handle->current_transfer = NULL;

	if (priv_handle->callback) {
		priv_handle->callback(CAPI_SPI_EVENT_XFR_DONE,
				      priv_handle->callback_arg,
				      0);
	}
}

/**
 * @brief HAL SPI RX complete callback.
 * @param hspi - Pointer to the HAL SPI handle.
 */
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	struct stm32_spi_priv_handle *priv_handle;

	priv_handle = find_priv_handle(hspi);
	if (!priv_handle) {
		return;
	}

	/* RX complete - deassert CS */
	if (priv_handle->cs_initialized)
		capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_HIGH);

	priv_handle->async_in_progress = false;
	priv_handle->current_transfer = NULL;

	if (priv_handle->callback) {
		priv_handle->callback(CAPI_SPI_EVENT_XFR_DONE,
				      priv_handle->callback_arg,
				      0);
	}
}

/**
 * @brief HAL SPI TX/RX complete callback.
 * @param hspi - Pointer to the HAL SPI handle.
 */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	struct stm32_spi_priv_handle *priv_handle;

	priv_handle = find_priv_handle(hspi);
	if (!priv_handle) {
		return;
	}

	stm32_capi_spi_dma_sync_pad(priv_handle);
	stm32_capi_spi_dma_release_pad(priv_handle);

	/* TxRx complete - deassert CS */
	if (priv_handle->cs_initialized)
		capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_HIGH);

	priv_handle->async_in_progress = false;
	priv_handle->current_transfer = NULL;

	if (priv_handle->callback) {
		priv_handle->callback(CAPI_SPI_EVENT_XFR_DONE,
				      priv_handle->callback_arg,
				      0);
	}
}

/**
 * @brief HAL SPI error callback.
 * @param hspi - Pointer to the HAL SPI handle.
 */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	struct stm32_spi_priv_handle *priv_handle;

	priv_handle = find_priv_handle(hspi);
	if (!priv_handle) {
		return;
	}

	stm32_capi_spi_dma_release_pad(priv_handle);

	/* Error - deassert CS */
	if (priv_handle->cs_initialized)
		capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_HIGH);

	priv_handle->async_in_progress = false;
	priv_handle->current_transfer = NULL;

	if (priv_handle->callback) {
		enum capi_async_event event = CAPI_SPI_EVENT_ERROR;

		/* Report FIFO faults with their dedicated events, as Maxim does. */
		if (hspi->ErrorCode & HAL_SPI_ERROR_OVR)
			event = CAPI_SPI_EVENT_RX_FIFO_OVERFLOW;
#ifdef HAL_SPI_ERROR_UDR
		else if (hspi->ErrorCode & HAL_SPI_ERROR_UDR)
			event = CAPI_SPI_EVENT_TX_FIFO_UNDERFLOW;
#endif

		priv_handle->callback(event, priv_handle->callback_arg, -EIO);
	}
}

/**
 * @brief HAL SPI abort complete callback.
 * @param hspi - Pointer to the HAL SPI handle.
 */
void HAL_SPI_AbortCpltCallback(SPI_HandleTypeDef *hspi)
{
	struct stm32_spi_priv_handle *priv_handle;

	priv_handle = find_priv_handle(hspi);
	if (!priv_handle)
		return;

	stm32_capi_spi_dma_release_pad(priv_handle);

	if (priv_handle->cs_initialized)
		capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_HIGH);

	priv_handle->async_in_progress = false;
	priv_handle->current_transfer = NULL;

	if (priv_handle->callback)
		priv_handle->callback(CAPI_SPI_EVENT_ERROR,
				      priv_handle->callback_arg, 0);
}
