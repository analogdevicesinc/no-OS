#ifndef _STM32_CAPI_SPI_H_
#define _STM32_CAPI_SPI_H_

#include "stm32_hal.h"
#include "capi/capi_spi.h"
#include "capi/capi_dma.h"
#include "capi/capi_gpio.h"
#include "stm32_capi_gpio.h"
#ifdef HAL_TIM_MODULE_ENABLED
#include "no_os_pwm.h"
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
	uint32_t irq_num;
#ifdef HAL_TIM_MODULE_ENABLED
	/** CS PWM Initialization Parameters */
	const struct no_os_pwm_init_param *pwm_init;
	const struct no_os_pwm_init_param *tx_pwm_init;
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

/**
 * @brief STM32 SPI private handle with full feature support
 */
struct stm32_spi_priv_handle {
	/** SPI HAL Handle */
	SPI_HandleTypeDef hspi;
	/** SPI input clock frequency */
	uint32_t input_clock;
	/** Chip select alternate function */
	uint32_t alternate;
	/** Chip select GPIO port handle */
	struct capi_gpio_port_handle *cs_port_handle;
	/** Chip select GPIO pin descriptor */
	struct capi_gpio_pin chip_select;
	/** DMA handle for async operations */
	struct capi_dma_handle *dma_handle;
	/** RX DMA Channel */
	struct capi_dma_chan *rxdma_ch;
	/** TX DMA Channel */
	struct capi_dma_chan *txdma_ch;

	/** CAPI callback and context */
	capi_spi_callback_t callback;
	void *callback_arg;

	/** Current transfer state */
	struct capi_spi_transfer *current_transfer;
	struct capi_spi_transfer *transfer_queue;
	uint32_t transfer_count;
	uint32_t current_transfer_index;
	bool async_in_progress;

	/** DMA transfer management */
	bool dma_done;
	void (*dma_user_cb)(void *ctx);
	void *dma_user_ctx;
	struct capi_dma_transfer *tx_ch_xfer;
	struct capi_dma_transfer *rx_ch_xfer;

#ifdef HAL_TIM_MODULE_ENABLED
	/** PWM descriptors for advanced CS control */
	struct no_os_pwm_desc *pwm_desc;
	struct no_os_pwm_desc *tx_pwm_desc;
#endif

	/** GPIO CS configuration */
	struct stm32_capi_gpio_port_config cs_gpio_config;
	/** Flag indicating if CS GPIO is initialized */
	bool cs_initialized;
};

extern struct capi_spi_ops stm32_capi_spi_ops;

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

extern struct stm32_capi_spi_extended_ops stm32_capi_spi_extended_ops;

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
int stm32_capi_spi_alternate_cs_enable(struct capi_spi_device *device, bool enable);

#endif
