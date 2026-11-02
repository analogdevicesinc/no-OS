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
#ifdef HAL_TIM_MODULE_ENABLED
#include "capi_timer.h"
#endif

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#define STM32_SPI_CRC_POLYNOMIAL		10
#define STM32_SPI_DMA_DEFAULT_TIMEOUT_MS	1000
#define STM32_SPI_GPIO_PINS_PER_PORT		16
#define STM32_SPI_DMA_DEFAULT_MIN_LEN		32
#define STM32_SPI_PIO_IDLE_SPINS		0x4000U

#ifdef SPI_SR_FRLVL
/* RX FIFO depth in 8-bit frames: more in flight could overrun it */
#define STM32_SPI_PIO_INFLIGHT			4U
#else
#define STM32_SPI_PIO_INFLIGHT			1U
#endif

/* Forward declarations */
static int stm32_capi_spi_dma_abort(struct capi_spi_device *device);
#ifdef SPI_SR_TXE
static void stm32_capi_spi_dma_rx_done(uint32_t event, void *ctx);
static void stm32_capi_spi_dma_error(uint32_t event, void *ctx);
#endif

/* lookup table for mapping HAL handles to private handles */
#define MAX_SPI_INSTANCES 8
static struct {
	SPI_HandleTypeDef *hspi;
	struct stm32_spi_priv_handle *priv_handle;
} spi_handle_lookup[MAX_SPI_INSTANCES];
static uint8_t spi_handle_count = 0;

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
 * @brief Assert the GPIO chip select (active low).
 * @param priv_handle - Pointer to the STM32 SPI private handle.
 */
static inline void stm32_spi_cs_assert(struct stm32_spi_priv_handle
				       *priv_handle)
{
	if (priv_handle->cs_port)
		priv_handle->cs_port->BSRR = priv_handle->cs_pin_mask << 16;
}

/**
 * @brief Deassert the GPIO chip select (active low).
 * @param priv_handle - Pointer to the STM32 SPI private handle.
 */
static inline void stm32_spi_cs_deassert(struct stm32_spi_priv_handle
		*priv_handle)
{
	if (priv_handle->cs_port)
		priv_handle->cs_port->BSRR = priv_handle->cs_pin_mask;
}

#ifdef SPI_SR_TXE
/* TX source when tx_buf is NULL and RX sink when rx_buf is NULL */
static const uint8_t stm32_spi_dma_zero;
static uint8_t stm32_spi_dma_sink;

/**
 * @brief Number of bytes clocked by a full duplex transfer.
 * @param transfer - Pointer to the transfer descriptor.
 * @return The size of the longer of the buffers that are present.
 */
static inline uint32_t stm32_spi_xfer_len(const struct capi_spi_transfer
		*transfer)
{
	uint32_t tx_size = transfer->tx_buf ? transfer->tx_size : 0;
	uint32_t rx_size = transfer->rx_buf ? transfer->rx_size : 0;

	return max(tx_size, rx_size);
}

/**
 * @brief Full duplex register-level transfer of max(tx_size, rx_size) bytes.
 *
 * TX is fed whenever there is FIFO room and RX is drained as it arrives, so
 * the bus is not idle between bytes. The number of bytes in flight is bounded
 * by the RX FIFO depth, so RX cannot overrun. Missing TX bytes are sent as 0,
 * extra RX bytes are discarded.
 *
 * @param spi - SPI peripheral, enabled and configured for 8-bit frames.
 * @param transfer - Pointer to the transfer descriptor.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_spi_pio(SPI_TypeDef *spi,
			      const struct capi_spi_transfer *transfer)
{
	volatile uint8_t *dr = (volatile uint8_t *)&spi->DR;
	const uint8_t *tx = transfer->tx_buf;
	uint8_t *rx = transfer->rx_buf;
	uint32_t tx_size = tx ? transfer->tx_size : 0;
	uint32_t rx_size = rx ? transfer->rx_size : 0;
	uint32_t len = max(tx_size, rx_size);
	uint32_t tx_cnt = 0;
	uint32_t rx_cnt = 0;
	uint32_t idle = 0;
	uint32_t start = HAL_GetTick();
	uint32_t sr;
	uint8_t byte;

	while (rx_cnt < len) {
		sr = spi->SR;

		if ((sr & SPI_SR_TXE) && tx_cnt < len &&
		    tx_cnt - rx_cnt < STM32_SPI_PIO_INFLIGHT) {
			*dr = tx_cnt < tx_size ? tx[tx_cnt] : 0;
			tx_cnt++;
		}

		if (sr & SPI_SR_RXNE) {
			byte = *dr;
			if (rx_cnt < rx_size)
				rx[rx_cnt] = byte;
			rx_cnt++;
			idle = 0;
			continue;
		}

		if (sr & SPI_SR_OVR) {
			(void)*dr;
			(void)spi->SR;
			return -EIO;
		}

		if (transfer->timeout > 0 && ++idle >= STM32_SPI_PIO_IDLE_SPINS) {
			idle = 0;
			if (HAL_GetTick() - start > (uint32_t)transfer->timeout)
				return -ETIMEDOUT;
		}
	}

	return 0;
}

/**
 * @brief Check whether both DMA channels and their templates are present.
 * @param priv_handle - Pointer to the STM32 SPI private handle.
 * @return true if DMA transfers can be used.
 */
static bool stm32_capi_spi_dma_ready(const struct stm32_spi_priv_handle
				     *priv_handle)
{
	return priv_handle->rxdma_ch && priv_handle->txdma_ch &&
	       priv_handle->rxdma_extra && priv_handle->txdma_extra;
}

/**
 * @brief Check whether a transfer can run over DMA.
 *
 * Both directions run for max(tx_size, rx_size) bytes, so a transfer with
 * both buffers must have equal sizes.
 *
 * @param priv_handle - Pointer to the STM32 SPI private handle.
 * @param transfer - Pointer to the transfer descriptor.
 * @return true if the transfer can use DMA.
 */
static bool stm32_capi_spi_dma_fits(const struct stm32_spi_priv_handle
				    *priv_handle,
				    const struct capi_spi_transfer *transfer)
{
	if (!stm32_capi_spi_dma_ready(priv_handle))
		return false;

	if (!stm32_spi_xfer_len(transfer))
		return false;

	if (transfer->tx_buf && transfer->rx_buf &&
	    transfer->tx_size != transfer->rx_size)
		return false;

	return true;
}

/**
 * @brief One-time DMA setup: fixed descriptor fields and channel callbacks.
 *
 * Completion is taken from the RX channel only: once the last byte has been
 * received the bus is idle. The TX channel only reports errors.
 *
 * @param priv_handle - Pointer to the STM32 SPI private handle.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_spi_dma_setup(struct stm32_spi_priv_handle *priv_handle)
{
	SPI_TypeDef *spi = priv_handle->hspi->Instance;
	int ret;

	priv_handle->dma_tx = (struct capi_dma_transfer) {
		.dst = (capi_dma_glbl_addr_t)&spi->DR,
		.xfer_type = CAPI_DMA_MEM_TO_DEV,
		.src_inc = CAPI_DMA_BYTE_INCREMENT,
		.dst_inc = CAPI_DMA_NO_INCREMENT,
		.src_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.dst_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.extra = priv_handle->txdma_extra,
	};
	priv_handle->dma_rx = (struct capi_dma_transfer) {
		.src = (capi_dma_glbl_addr_t)&spi->DR,
		.xfer_type = CAPI_DMA_DEV_TO_MEM,
		.src_inc = CAPI_DMA_NO_INCREMENT,
		.dst_inc = CAPI_DMA_BYTE_INCREMENT,
		.src_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.dst_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.extra = priv_handle->rxdma_extra,
	};

	ret = capi_dma_register_complete_callback(priv_handle->rxdma_ch,
			stm32_capi_spi_dma_rx_done,
			priv_handle);
	if (ret)
		return ret;

	ret = capi_dma_register_error_callback(priv_handle->rxdma_ch,
					       stm32_capi_spi_dma_error,
					       priv_handle);
	if (ret)
		return ret;

	return capi_dma_register_error_callback(priv_handle->txdma_ch,
						stm32_capi_spi_dma_error,
						priv_handle);
}

/**
 * @brief Program both DMA channels and start a transfer.
 *
 * Only addresses and length change between transfers; the channel
 * configuration is reapplied by the DMA backend only when it differs.
 *
 * @param priv_handle - Pointer to the STM32 SPI private handle.
 * @param device - Pointer to the SPI device descriptor.
 * @param transfer - Pointer to the transfer descriptor.
 * @param assert_cs - Assert CS before starting (first transfer of a frame).
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_spi_dma_kick(struct stm32_spi_priv_handle *priv_handle,
				   struct capi_spi_device *device,
				   struct capi_spi_transfer *transfer,
				   bool assert_cs)
{
	struct stm32_spi_device_extra *dev_extra = device->extra;
	SPI_TypeDef *spi = priv_handle->hspi->Instance;
	uint32_t len = stm32_spi_xfer_len(transfer);
	int ret;

	if (transfer->tx_buf) {
		priv_handle->dma_tx.src = (capi_dma_glbl_addr_t)transfer->tx_buf;
		priv_handle->dma_tx.src_inc = CAPI_DMA_BYTE_INCREMENT;
	} else {
		priv_handle->dma_tx.src = (capi_dma_glbl_addr_t)&stm32_spi_dma_zero;
		priv_handle->dma_tx.src_inc = CAPI_DMA_NO_INCREMENT;
	}

	if (transfer->rx_buf) {
		priv_handle->dma_rx.dst = (capi_dma_glbl_addr_t)transfer->rx_buf;
		priv_handle->dma_rx.dst_inc = CAPI_DMA_BYTE_INCREMENT;
	} else {
		priv_handle->dma_rx.dst = (capi_dma_glbl_addr_t)&stm32_spi_dma_sink;
		priv_handle->dma_rx.dst_inc = CAPI_DMA_NO_INCREMENT;
	}

	priv_handle->dma_tx.length = len;
	priv_handle->dma_rx.length = len;

	ret = capi_dma_config_xfer(priv_handle->rxdma_ch, &priv_handle->dma_rx);
	if (ret)
		return ret;

	ret = capi_dma_config_xfer(priv_handle->txdma_ch, &priv_handle->dma_tx);
	if (ret)
		return ret;

	/* Drop stale RX data and a pending overrun */
	while (spi->SR & SPI_SR_RXNE)
		(void)*(volatile uint8_t *)&spi->DR;
	(void)spi->SR;

	priv_handle->dma_device = device;
	priv_handle->dma_xfer_status = 0;
	priv_handle->dma_xfer_done = false;

	if (assert_cs) {
		stm32_spi_cs_assert(priv_handle);
		if (dev_extra && dev_extra->cs_delay_first)
			capi_wait_us(dev_extra->cs_delay_first);
	}

	SET_BIT(spi->CR2, SPI_CR2_RXDMAEN);

	ret = capi_dma_xfer_start(priv_handle->rxdma_ch);
	if (ret)
		goto stop;

	ret = capi_dma_xfer_start(priv_handle->txdma_ch);
	if (ret)
		goto stop;

	SET_BIT(spi->CR2, SPI_CR2_TXDMAEN);

	return 0;

stop:
	capi_dma_xfer_abort(priv_handle->rxdma_ch);
	CLEAR_BIT(spi->CR2, SPI_CR2_RXDMAEN);
	priv_handle->dma_xfer_done = true;
	stm32_spi_cs_deassert(priv_handle);

	return ret;
}

/**
 * @brief Stop both DMA channels and the SPI DMA requests.
 * @param priv_handle - Pointer to the STM32 SPI private handle.
 */
static void stm32_capi_spi_dma_stop(struct stm32_spi_priv_handle *priv_handle)
{
	SPI_TypeDef *spi = priv_handle->hspi->Instance;

	CLEAR_BIT(spi->CR2, SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN);
	capi_dma_xfer_abort(priv_handle->txdma_ch);
	capi_dma_xfer_abort(priv_handle->rxdma_ch);

	while (spi->SR & SPI_SR_BSY)
		;
	while (spi->SR & SPI_SR_RXNE)
		(void)*(volatile uint8_t *)&spi->DR;
	(void)spi->SR;
}

/**
 * @brief End of a DMA transfer, called from the DMA ISR.
 *
 * Starts the next transfer of a multi-transfer sequence with CS held,
 * otherwise releases CS and reports the result.
 *
 * @param priv_handle - Pointer to the STM32 SPI private handle.
 * @param status - 0 on success, negative error code otherwise.
 */
static void stm32_capi_spi_dma_finish(struct stm32_spi_priv_handle
				      *priv_handle, int status)
{
	struct capi_spi_device *device = priv_handle->dma_device;
	struct stm32_spi_device_extra *dev_extra = device ? device->extra : NULL;
	void (*user_cb)(void *ctx);
	SPI_TypeDef *spi = priv_handle->hspi->Instance;

	if (priv_handle->dma_xfer_done)
		return;

	CLEAR_BIT(spi->CR2, SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN);

	if (status) {
		stm32_capi_spi_dma_stop(priv_handle);
	} else if (priv_handle->dma_seq_left) {
		priv_handle->dma_seq++;
		priv_handle->dma_seq_left--;
		status = stm32_capi_spi_dma_kick(priv_handle, device,
						 priv_handle->dma_seq, false);
		if (!status)
			return;
	}

	if (dev_extra && dev_extra->cs_delay_last)
		capi_wait_us(dev_extra->cs_delay_last);

	stm32_spi_cs_deassert(priv_handle);

	if (dev_extra && dev_extra->cs_change_delay)
		capi_wait_us(dev_extra->cs_change_delay);

	priv_handle->dma_seq_left = 0;
	priv_handle->dma_xfer_status = status;
	priv_handle->dma_xfer_done = true;

	user_cb = priv_handle->dma_user_cb;
	if (priv_handle->dma_async || user_cb) {
		priv_handle->dma_user_cb = NULL;
		priv_handle->async_in_progress = false;
		priv_handle->current_transfer = NULL;
	}

	if (priv_handle->dma_async && priv_handle->callback)
		priv_handle->callback(status ? CAPI_SPI_EVENT_ERROR :
				      CAPI_SPI_EVENT_XFR_DONE,
				      priv_handle->callback_arg, status);

	if (user_cb)
		user_cb(priv_handle->dma_user_ctx);
}

/**
 * @brief RX DMA channel transfer complete callback.
 * @param event - DMA event identifier.
 * @param ctx - Pointer to the STM32 SPI private handle.
 */
static void stm32_capi_spi_dma_rx_done(uint32_t event, void *ctx)
{
	(void)event;

	stm32_capi_spi_dma_finish(ctx, 0);
}

/**
 * @brief RX/TX DMA channel error callback.
 * @param event - DMA event identifier.
 * @param ctx - Pointer to the STM32 SPI private handle.
 */
static void stm32_capi_spi_dma_error(uint32_t event, void *ctx)
{
	(void)event;

	stm32_capi_spi_dma_finish(ctx, -EIO);
}

/**
 * @brief Start a DMA sequence of one or more transfers under one CS assertion.
 * @param priv_handle - Pointer to the STM32 SPI private handle.
 * @param device - Pointer to the SPI device descriptor.
 * @param transfers - Array of transfer descriptors.
 * @param transfer_count - Number of transfers.
 * @param async - Report the end through the registered CAPI callback.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_spi_dma_start_seq(struct stm32_spi_priv_handle
					*priv_handle,
					struct capi_spi_device *device,
					struct capi_spi_transfer *transfers,
					uint32_t transfer_count, bool async)
{
	uint32_t i;

	for (i = 0; i < transfer_count; i++)
		if (!stm32_capi_spi_dma_fits(priv_handle, &transfers[i]))
			return -EINVAL;

	priv_handle->dma_async = async;
	priv_handle->dma_seq = transfers;
	priv_handle->dma_seq_left = transfer_count - 1;

	return stm32_capi_spi_dma_kick(priv_handle, device, transfers, true);
}

/**
 * @brief Wait for the in-flight DMA transfer, aborting it on timeout.
 *
 * A timeout of 0 or -1 still uses STM32_SPI_DMA_DEFAULT_TIMEOUT_MS, so a
 * missing DMA interrupt cannot hang the caller.
 *
 * @param priv_handle - Pointer to the STM32 SPI private handle.
 * @param timeout - Timeout in milliseconds.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_spi_dma_wait(struct stm32_spi_priv_handle *priv_handle,
				   int timeout)
{
	uint32_t limit = timeout > 0 ? (uint32_t)timeout :
			 STM32_SPI_DMA_DEFAULT_TIMEOUT_MS;
	uint32_t start = HAL_GetTick();

	while (!priv_handle->dma_xfer_done) {
		if (HAL_GetTick() - start > limit) {
			stm32_capi_spi_dma_stop(priv_handle);
			stm32_spi_cs_deassert(priv_handle);
			priv_handle->dma_seq_left = 0;
			priv_handle->dma_xfer_done = true;
			return -ETIMEDOUT;
		}
	}

	return priv_handle->dma_xfer_status;
}
#endif /* SPI_SR_TXE */

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
	spi_priv_handle->sync_cfg_cached = false;
	spi_priv_handle->sync_cfg_key = 0;
	spi_priv_handle->cs_port = NULL;
	spi_priv_handle->cs_pin_mask = 0;
	spi_priv_handle->dma_handle = NULL;
	spi_priv_handle->rxdma_ch = NULL;
	spi_priv_handle->txdma_ch = NULL;
	spi_priv_handle->rxdma_extra = NULL;
	spi_priv_handle->txdma_extra = NULL;
	spi_priv_handle->dma_user_cb = NULL;
	spi_priv_handle->dma_seq_left = 0;
	spi_priv_handle->dma_xfer_done = true;

	if (spi_extra_config) {
		if (spi_extra_config->get_input_clock)
			spi_priv_handle->input_clock = spi_extra_config->get_input_clock();
		else
			spi_priv_handle->input_clock = config->clk_freq_hz;

		spi_priv_handle->alternate = spi_extra_config->alternate;

		/* Set up DMA if provided */
		spi_priv_handle->dma_handle = spi_extra_config->dma_handle ?
					      spi_extra_config->dma_handle :
					      config->dma_handle;
		if (spi_priv_handle->dma_handle) {
			spi_priv_handle->rxdma_extra = spi_extra_config->rxdma_extra;
			spi_priv_handle->txdma_extra = spi_extra_config->txdma_extra;
			spi_priv_handle->dma_min_len = spi_extra_config->dma_min_len ?
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
					goto error_dma;
			}

#ifdef SPI_SR_TXE
			if (stm32_capi_spi_dma_ready(spi_priv_handle)) {
				ret = stm32_capi_spi_dma_setup(spi_priv_handle);
				if (ret)
					goto error_dma;
			}
#endif
		}

#ifdef HAL_TIM_MODULE_ENABLED
		/* Store timer handles for PWM-based CS/TX control */
		if (spi_extra_config->cs_timer) {
			spi_priv_handle->cs_timer = spi_extra_config->cs_timer;
			spi_priv_handle->cs_timer_channel = spi_extra_config->cs_timer_channel;

			ret = capi_timer_channel_disable(spi_priv_handle->cs_timer,
							 spi_priv_handle->cs_timer_channel);
			if (ret)
				goto error_dma;
		}
		if (spi_extra_config->tx_timer) {
			spi_priv_handle->tx_timer = spi_extra_config->tx_timer;
			spi_priv_handle->tx_timer_channel = spi_extra_config->tx_timer_channel;

			ret = capi_timer_channel_disable(spi_priv_handle->tx_timer,
							 spi_priv_handle->tx_timer_channel);
			if (ret)
				goto error_dma;
		}
#endif
	} else {
		spi_priv_handle->input_clock = config->clk_freq_hz;
	}

	/* Register handle mapping for callbacks */
	ret = register_handle_mapping(spi_priv_handle->hspi, spi_priv_handle);
	if (ret != 0) {
		goto error_dma;
	}

	*handle = spi_handle;
	return 0;

error_dma:
	if (spi_priv_handle->txdma_ch)
		capi_dma_deinit_chan(spi_priv_handle->txdma_ch);
	if (spi_priv_handle->rxdma_ch)
		capi_dma_deinit_chan(spi_priv_handle->rxdma_ch);
	spi_priv_handle->txdma_ch = NULL;
	spi_priv_handle->rxdma_ch = NULL;
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

	priv_handle->cs_port = ((struct stm32_capi_gpio_port_priv *)
				priv_handle->cs_port_handle->priv)->port;
	priv_handle->cs_pin_mask = 1U << dev_extra->chip_select_pin;
	priv_handle->cs_initialized = true;
	return 0;
}

/**
 * @brief Set up CS and apply the device configuration if it changed.
 *
 * The peripheral is reconfigured only when the effective device settings
 * change. The key is stored per-controller (not file-static) so it is reset
 * by init and a fresh/re-initialized controller always configures on its
 * first transfer; it also folds in every field config_peripheral consumes
 * (CS, clock mode, bit order, speed) so a change to any of them re-applies
 * the config.
 *
 * @param priv_handle - Pointer to the STM32 SPI private handle.
 * @param device - Pointer to the SPI device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_spi_prepare(struct stm32_spi_priv_handle *priv_handle,
				  struct capi_spi_device *device)
{
	uint64_t cfg_key;
	int ret;

	ret = setup_cs_gpio(priv_handle, device);
	if (ret)
		return ret;

	cfg_key = ((uint64_t)(device->cs_gpio ? device->cs_gpio->number :
			      device->native_cs)) |
		  ((uint64_t)device->mode << 8) |
		  ((uint64_t)(device->lsb_first ? 1U : 0U) << 16) |
		  ((uint64_t)device->max_speed_hz << 32);

	if (priv_handle->sync_cfg_cached && cfg_key == priv_handle->sync_cfg_key)
		return 0;

	ret = stm32_capi_spi_config_peripheral(priv_handle, device);
	if (ret)
		return ret;

	priv_handle->sync_cfg_key = cfg_key;
	priv_handle->sync_cfg_cached = true;

	return 0;
}

#ifndef SPI_SR_TXE
/**
 * @brief Full duplex HAL transfer, for SPI IPs without the register-level path.
 * @param priv_handle - Pointer to the STM32 SPI private handle.
 * @param transfer - Pointer to the transfer descriptor.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_spi_hal_xfer(struct stm32_spi_priv_handle *priv_handle,
				   struct capi_spi_transfer *transfer)
{
	uint32_t timeout_val = transfer->timeout > 0 ?
			       transfer->timeout : HAL_MAX_DELAY;
	uint32_t xfer_len = max(transfer->tx_size, transfer->rx_size);
	HAL_StatusTypeDef hal_ret;
	uint8_t *tmp_tx;
	uint8_t *tmp_rx;

	if (transfer->tx_buf && transfer->rx_buf) {
		if (transfer->tx_size == transfer->rx_size) {
			hal_ret = HAL_SPI_TransmitReceive(priv_handle->hspi,
							  (uint8_t *)transfer->tx_buf,
							  transfer->rx_buf,
							  xfer_len, timeout_val);
		} else {
			tmp_tx = capi_calloc(1, xfer_len);
			tmp_rx = capi_calloc(1, xfer_len);
			if (!tmp_tx || !tmp_rx) {
				capi_free(tmp_tx);
				capi_free(tmp_rx);
				return -ENOMEM;
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
					   transfer->tx_size, timeout_val);
	} else {
		tmp_tx = capi_calloc(1, transfer->rx_size);
		if (!tmp_tx)
			return -ENOMEM;
		hal_ret = HAL_SPI_TransmitReceive(priv_handle->hspi,
						  tmp_tx, transfer->rx_buf,
						  transfer->rx_size, timeout_val);
		capi_free(tmp_tx);
	}

	if (hal_ret == HAL_TIMEOUT)
		return -ETIMEDOUT;
	if (hal_ret != HAL_OK)
		return -EIO;

	return 0;
}
#endif

/**
 * @brief Perform SPI transfer with proper CS control and timing.
 *
 * The caller must have run stm32_capi_spi_prepare() for this device.
 *
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
	int ret;

	if (assert_cs)
		stm32_spi_cs_assert(priv_handle);

	if (dev_extra && dev_extra->cs_delay_first)
		capi_wait_us(dev_extra->cs_delay_first);

#ifdef SPI_SR_TXE
	ret = stm32_capi_spi_pio(priv_handle->hspi->Instance, transfer);
#else
	ret = stm32_capi_spi_hal_xfer(priv_handle, transfer);
#endif

	if (dev_extra && dev_extra->cs_delay_last)
		capi_wait_us(dev_extra->cs_delay_last);

	if (deassert_cs)
		stm32_spi_cs_deassert(priv_handle);

	if (dev_extra && dev_extra->cs_change_delay)
		capi_wait_us(dev_extra->cs_change_delay);

	return ret;
}

/**
 * @brief Perform a synchronous SPI transceive operation.
 *
 * Transfers of at least dma_min_len bytes run over DMA when it is
 * configured, shorter ones use the register-level PIO loop.
 *
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

	if (priv_handle->async_in_progress)
		return -EBUSY;

	ret = stm32_capi_spi_prepare(priv_handle, device);
	if (ret)
		return ret;

#ifdef SPI_SR_TXE
	if (stm32_spi_xfer_len(transfer) >= priv_handle->dma_min_len &&
	    stm32_capi_spi_dma_fits(priv_handle, transfer)) {
		ret = stm32_capi_spi_dma_start_seq(priv_handle, device, transfer, 1,
						   false);
		if (ret)
			return ret;

		return stm32_capi_spi_dma_wait(priv_handle, transfer->timeout);
	}
#endif

	return perform_spi_transfer_with_cs(priv_handle, device, transfer, true, true);
}

/**
 * @brief Perform an asynchronous SPI transceive operation.
 *
 * Runs over DMA when it is configured (completion from the RX DMA channel
 * interrupt), otherwise falls back to the HAL interrupt driven transfer.
 *
 * @param device - Pointer to the SPI device descriptor.
 * @param transfer - Pointer to the transfer descriptor.
 * @param timeout - Timeout value in milliseconds.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_spi_transceive_async(struct capi_spi_device *device,
		struct capi_spi_transfer *transfer,
		int timeout)
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

	ret = stm32_capi_spi_prepare(priv_handle, device);
	if (ret)
		return ret;

	priv_handle->current_transfer = transfer;
	priv_handle->async_in_progress = true;

#ifdef SPI_SR_TXE
	if (stm32_capi_spi_dma_fits(priv_handle, transfer)) {
		ret = stm32_capi_spi_dma_start_seq(priv_handle, device, transfer, 1,
						   true);
		if (ret) {
			priv_handle->async_in_progress = false;
			priv_handle->current_transfer = NULL;
		}

		return ret;
	}
#endif

	stm32_spi_cs_assert(priv_handle);

	if (dev_extra && dev_extra->cs_delay_first)
		capi_wait_us(dev_extra->cs_delay_first);

	if (transfer->tx_buf && transfer->rx_buf) {
		hal_ret = HAL_SPI_TransmitReceive_IT(priv_handle->hspi,
						     (uint8_t *)transfer->tx_buf,
						     transfer->rx_buf,
						     max(transfer->tx_size, transfer->rx_size));
	} else if (transfer->tx_buf) {
		hal_ret = HAL_SPI_Transmit_IT(priv_handle->hspi,
					      (uint8_t *)transfer->tx_buf,
					      transfer->tx_size);
	} else if (transfer->rx_buf) {
		hal_ret = HAL_SPI_Receive_IT(priv_handle->hspi,
					     transfer->rx_buf,
					     transfer->rx_size);
	} else {
		priv_handle->async_in_progress = false;
		priv_handle->current_transfer = NULL;
		stm32_spi_cs_deassert(priv_handle);
		return -EINVAL;
	}

	if (hal_ret != HAL_OK) {
		priv_handle->async_in_progress = false;
		priv_handle->current_transfer = NULL;
		stm32_spi_cs_deassert(priv_handle);
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
#ifdef SPI_SR_TXE
	struct capi_spi_transfer phase;
#else
	HAL_StatusTypeDef hal_ret;
#endif
	int ret = 0;

	if (!device || !device->controller || !transfer)
		return -EINVAL;

	priv_handle = device->controller->priv;
	dev_extra = device->extra;

	if (priv_handle->async_in_progress)
		return -EBUSY;

	ret = stm32_capi_spi_prepare(priv_handle, device);
	if (ret)
		return ret;

	stm32_spi_cs_assert(priv_handle);

	/* CS delay first */
	if (dev_extra && dev_extra->cs_delay_first)
		capi_wait_us(dev_extra->cs_delay_first);

#ifdef SPI_SR_TXE
	phase = (struct capi_spi_transfer) {
		.tx_buf = transfer->tx_buf,
		.tx_size = transfer->tx_buf ? transfer->tx_size : 0,
		.timeout = transfer->timeout,
	};
	ret = stm32_capi_spi_pio(priv_handle->hspi->Instance, &phase);
	if (ret)
		goto deassert_cs;

	phase = (struct capi_spi_transfer) {
		.rx_buf = transfer->rx_buf,
		.rx_size = transfer->rx_buf ? transfer->rx_size : 0,
		.timeout = transfer->timeout,
	};
	ret = stm32_capi_spi_pio(priv_handle->hspi->Instance, &phase);
#else
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
#endif

deassert_cs:
	/* CS delay last */
	if (dev_extra && dev_extra->cs_delay_last)
		capi_wait_us(dev_extra->cs_delay_last);

	stm32_spi_cs_deassert(priv_handle);

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

	ret = stm32_capi_spi_prepare(priv_handle, device);
	if (ret)
		return ret;

	priv_handle->current_transfer = transfer;
	priv_handle->async_in_progress = true;

	stm32_spi_cs_assert(priv_handle);

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
	/* abort_hw disabled the peripheral, the next transfer reconfigures it */
	priv_handle->sync_cfg_cached = false;

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

	if (priv_handle->async_in_progress)
		return -EBUSY;

	ret = stm32_capi_spi_prepare(priv_handle, device);
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

#ifndef SPI_SR_TXE
/**
 * @brief DMA completion callback for SPI transfers.
 * @param event - DMA event identifier.
 * @param ctx - User context (pointer to the SPI device).
 */
static void stm32_capi_spi_dma_callback(uint32_t event, void *ctx)
{
	struct capi_spi_device *device = (struct capi_spi_device *)ctx;
	struct stm32_spi_priv_handle *priv_handle = device->controller->priv;

	/* if more xfers pending dont do anything */
	if (priv_handle->current_transfer_index < priv_handle->transfer_count - 1)
		return;

#ifdef HAL_TIM_MODULE_ENABLED
	if (priv_handle->cs_timer)
		capi_timer_channel_disable(priv_handle->cs_timer,
					   priv_handle->cs_timer_channel);

	if (priv_handle->tx_timer)
		capi_timer_channel_disable(priv_handle->tx_timer,
					   priv_handle->tx_timer_channel);
#endif

	/* Perform abort SPI transfers */
	stm32_capi_spi_abort_async(device);

	priv_handle->dma_done = true;

	if (priv_handle->dma_user_cb)
		priv_handle->dma_user_cb(priv_handle->dma_user_ctx);
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
	SPI_TypeDef *SPIx = priv_handle->hspi->Instance;
	int ret;
	uint32_t i;

	if (!device || !transfers || !priv_handle->dma_handle)
		return -EINVAL;

	if (!priv_handle->rxdma_ch || !priv_handle->txdma_ch)
		return -EINVAL;

	rx_ch_xfer = capi_calloc(transfer_count, sizeof(*rx_ch_xfer));
	if (!rx_ch_xfer)
		return -ENOMEM;

	tx_ch_xfer = capi_calloc(transfer_count, sizeof(*tx_ch_xfer));
	if (!tx_ch_xfer) {
		ret = -ENOMEM;
		goto free_rx_ch_xfer;
	}

	for (i = 0; i < transfer_count; i++) {
		/* Configure TX DMA transfer */
		tx_ch_xfer[i].src = (capi_dma_glbl_addr_t)transfers[i].tx_buf;
#ifndef SPI_SR_TXE
		tx_ch_xfer[i].dst = (capi_dma_glbl_addr_t) & (SPIx->TXDR);
#else
		tx_ch_xfer[i].dst = (capi_dma_glbl_addr_t) & (SPIx->DR);
#endif
		tx_ch_xfer[i].xfer_type = CAPI_DMA_MEM_TO_DEV;
		tx_ch_xfer[i].src_inc = CAPI_DMA_BYTE_INCREMENT;
		tx_ch_xfer[i].dst_inc = CAPI_DMA_NO_INCREMENT;
		tx_ch_xfer[i].length = transfers[i].tx_size;

		/* Configure RX DMA transfer */
		rx_ch_xfer[i].dst = (capi_dma_glbl_addr_t)transfers[i].rx_buf;
#ifndef SPI_SR_RXNE
		rx_ch_xfer[i].src = (capi_dma_glbl_addr_t) & (SPIx->RXDR);
#else
		rx_ch_xfer[i].src = (capi_dma_glbl_addr_t) & (SPIx->DR);
#endif
		rx_ch_xfer[i].xfer_type = CAPI_DMA_DEV_TO_MEM;
		rx_ch_xfer[i].src_inc = CAPI_DMA_NO_INCREMENT;
		rx_ch_xfer[i].dst_inc = CAPI_DMA_BYTE_INCREMENT;
		rx_ch_xfer[i].length = transfers[i].rx_size;
	}

	priv_handle->tx_ch_xfer = tx_ch_xfer;
	priv_handle->rx_ch_xfer = rx_ch_xfer;
	priv_handle->transfer_count = transfer_count;
	priv_handle->current_transfer_index = 0;

	/* Register completion callback on the RX DMA channel */
	if (completion_callback) {
		ret = capi_dma_register_complete_callback(priv_handle->rxdma_ch,
				completion_callback,
				callback_ctx);
		if (ret)
			goto cleanup_dma;
	}

	/* Configure RX DMA channel */
	ret = capi_dma_config_xfer(priv_handle->rxdma_ch, &rx_ch_xfer[0]);
	if (ret)
		goto cleanup_dma;

	/* Configure TX DMA channel */
	ret = capi_dma_config_xfer(priv_handle->txdma_ch, &tx_ch_xfer[0]);
	if (ret)
		goto cleanup_dma;

	/* Start TX DMA transfer */
	ret = capi_dma_xfer_start(priv_handle->txdma_ch);
	if (ret)
		goto abort_transfer;

	/* Start RX DMA transfer */
	ret = capi_dma_xfer_start(priv_handle->rxdma_ch);
	if (ret)
		goto abort_transfer;

	/* Enable SPI DMA requests */
	if (priv_handle->txdma_ch) {
#if defined (STM32H5)
		SET_BIT(priv_handle->hspi->Instance->CFG1, SPI_CFG1_TXDMAEN);
#else
		SET_BIT(priv_handle->hspi->Instance->CR2, SPI_CR2_TXDMAEN);
#endif
	}

	if (priv_handle->rxdma_ch) {
#if defined (STM32H5)
		SET_BIT(priv_handle->hspi->Instance->CFG1, SPI_CFG1_RXDMAEN);
#else
		SET_BIT(priv_handle->hspi->Instance->CR2, SPI_CR2_RXDMAEN);
#endif
	}

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
	capi_free(tx_ch_xfer);
	priv_handle->tx_ch_xfer = NULL;
free_rx_ch_xfer:
	capi_free(rx_ch_xfer);
	priv_handle->rx_ch_xfer = NULL;
	return ret;
}

/**
 * @brief Abort DMA transfers and free associated resources.
 * @param device - Pointer to the SPI device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_spi_dma_abort(struct capi_spi_device *device)
{
	struct stm32_spi_priv_handle *priv_handle;
	SPI_TypeDef *SPIx;
	int ret = 0;

	if (!device || !device->controller)
		return -EINVAL;

	priv_handle = device->controller->priv;
	if (!priv_handle)
		return -EINVAL;

	SPIx = priv_handle->hspi->Instance;

	if (priv_handle->rxdma_ch) {
		ret = capi_dma_xfer_abort(priv_handle->rxdma_ch);
		if (ret)
			return ret;

#if defined (STM32H5)
		CLEAR_BIT(SPIx->CFG1, SPI_CFG1_RXDMAEN);
#else
		CLEAR_BIT(SPIx->CR2, SPI_CR2_RXDMAEN);
#endif
	}

	if (priv_handle->txdma_ch) {
		ret = capi_dma_xfer_abort(priv_handle->txdma_ch);
		if (ret)
			return ret;

#if defined (STM32H5)
		CLEAR_BIT(SPIx->CFG1, SPI_CFG1_TXDMAEN);
#else
		CLEAR_BIT(SPIx->CR2, SPI_CR2_TXDMAEN);
#endif
	}

	/* Dummy read to clear any pending read on SPI */
#ifndef SPI_SR_RXNE
	*(volatile uint8_t *)&SPIx->RXDR;
#else
	*(volatile uint8_t *)&SPIx->DR;
#endif

	/* Free the allocated memory for tx and rx transfers */
	capi_free(priv_handle->tx_ch_xfer);
	capi_free(priv_handle->rx_ch_xfer);
	priv_handle->tx_ch_xfer = NULL;
	priv_handle->rx_ch_xfer = NULL;

	/* put CS pin back into gpio mode */
	stm32_capi_spi_alternate_cs_enable(device, false);

	return 0;
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

	/* Wait for completion with timeout */
	timeout = transfers[0].tx_size + transfers[0].rx_size;
	if (timeout == 0)
		timeout = STM32_SPI_DMA_DEFAULT_TIMEOUT_MS;

	while (timeout) {
		timeout--;
		capi_wait_ms(1);
		if (priv_handle->dma_done)
			break;
	}

	if (!timeout) {
		stm32_capi_spi_dma_abort(device);
		return -ETIMEDOUT;
	}

	return 0;
}

#else /* SPI_SR_TXE */
/**
 * @brief Abort an in-flight DMA transfer.
 * @param device - Pointer to the SPI device descriptor.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_spi_dma_abort(struct capi_spi_device *device)
{
	struct stm32_spi_priv_handle *priv_handle;

	if (!device || !device->controller || !device->controller->priv)
		return -EINVAL;

	priv_handle = device->controller->priv;

	if (!stm32_capi_spi_dma_ready(priv_handle) || priv_handle->dma_xfer_done)
		return 0;

	stm32_capi_spi_dma_stop(priv_handle);
	priv_handle->dma_seq_left = 0;
	priv_handle->dma_user_cb = NULL;
	priv_handle->dma_xfer_status = -ECANCELED;
	priv_handle->dma_xfer_done = true;

	return 0;
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
 *
 * The transfers run back to back under one CS assertion; each one is started
 * from the DMA ISR of the previous one.
 *
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
	int ret;

	if (!device || !device->controller || !transfers || !transfer_count)
		return -EINVAL;

	priv_handle = device->controller->priv;

	if (priv_handle->async_in_progress)
		return -EBUSY;

	ret = stm32_capi_spi_prepare(priv_handle, device);
	if (ret)
		return ret;

	priv_handle->async_in_progress = true;
	priv_handle->current_transfer = transfers;
	priv_handle->dma_user_cb = callback;
	priv_handle->dma_user_ctx = callback_arg;

	ret = stm32_capi_spi_dma_start_seq(priv_handle, device, transfers,
					   transfer_count, false);
	if (ret) {
		priv_handle->dma_user_cb = NULL;
		priv_handle->async_in_progress = false;
		priv_handle->current_transfer = NULL;
	}

	return ret;
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
	int ret;

	if (!device || !device->controller || !transfers || !transfer_count)
		return -EINVAL;

	priv_handle = device->controller->priv;

	if (priv_handle->async_in_progress)
		return -EBUSY;

	ret = stm32_capi_spi_prepare(priv_handle, device);
	if (ret)
		return ret;

	ret = stm32_capi_spi_dma_start_seq(priv_handle, device, transfers,
					   transfer_count, false);
	if (ret)
		return ret;

	return stm32_capi_spi_dma_wait(priv_handle, transfers[0].timeout);
}
#endif /* SPI_SR_TXE */

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

	/* Error - deassert CS */
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

	if (priv_handle->cs_initialized)
		capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_HIGH);

	priv_handle->async_in_progress = false;
	priv_handle->current_transfer = NULL;

	if (priv_handle->callback)
		priv_handle->callback(CAPI_SPI_EVENT_ERROR,
				      priv_handle->callback_arg, 0);
}