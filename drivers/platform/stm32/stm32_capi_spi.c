#include <stdlib.h>
#include <errno.h>
#include "capi/capi_spi.h"
#include "capi/capi_gpio.h"
#include "stm32_capi_spi.h"
#include "stm32_capi_gpio.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_delay.h"
#include "capi/capi_dma.h"
#ifdef HAL_TIM_MODULE_ENABLED
#include "no_os_pwm.h"
#endif

/* Forward declarations */
static int stm32_capi_spi_dma_abort(struct capi_spi_device *device);

//lookup table for mapping HAL handles to our private handles
#define MAX_SPI_INSTANCES 8
static struct {
	SPI_HandleTypeDef *hspi;
	struct stm32_spi_priv_handle *priv_handle;
} spi_handle_lookup[MAX_SPI_INSTANCES];
static uint8_t spi_handle_count = 0;

// Helper function to find private handle from HAL handle
static struct stm32_spi_priv_handle *find_priv_handle(SPI_HandleTypeDef *hspi)
{
	for (uint8_t i = 0; i < spi_handle_count; i++) {
		if (spi_handle_lookup[i].hspi == hspi) {
			return spi_handle_lookup[i].priv_handle;
		}
	}
	return NULL;
}

// Helper function to register handle mapping
static int register_handle_mapping(SPI_HandleTypeDef *hspi, struct stm32_spi_priv_handle *priv_handle)
{
	if (spi_handle_count >= MAX_SPI_INSTANCES) {
		return -ENOMEM;
	}

	spi_handle_lookup[spi_handle_count].hspi = hspi;
	spi_handle_lookup[spi_handle_count].priv_handle = priv_handle;
	spi_handle_count++;
	return 0;
}

// Helper function to unregister handle mapping
static void unregister_handle_mapping(SPI_HandleTypeDef *hspi)
{
	for (uint8_t i = 0; i < spi_handle_count; i++) {
		if (spi_handle_lookup[i].hspi == hspi) {
			// Move last entry to this position
			if (i < spi_handle_count - 1) {
				spi_handle_lookup[i] = spi_handle_lookup[spi_handle_count - 1];
			}
			spi_handle_count--;
			break;
		}
	}
}

/**
 * @brief Configure SPI peripheral based on device settings
 */
static int stm32_capi_spi_config_peripheral(struct stm32_spi_priv_handle *priv_handle,
					     struct capi_spi_device *device)
{
	uint32_t prescaler = 0u;
	int ret = 0;

	/* Verify SPI instance is configured */
	if (!priv_handle->hspi.Instance)
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
	priv_handle->hspi.Init.Mode = SPI_MODE_MASTER;
	priv_handle->hspi.Init.Direction = SPI_DIRECTION_2LINES;
	priv_handle->hspi.Init.DataSize = SPI_DATASIZE_8BIT;
	priv_handle->hspi.Init.CLKPolarity = device->mode & CAPI_SPI_CPOL ?
					     SPI_POLARITY_HIGH :
					     SPI_POLARITY_LOW;
	priv_handle->hspi.Init.CLKPhase = device->mode & CAPI_SPI_CPHA ?
					  SPI_PHASE_2EDGE :
					  SPI_PHASE_1EDGE;
	priv_handle->hspi.Init.NSS = SPI_NSS_SOFT;
	priv_handle->hspi.Init.BaudRatePrescaler = prescaler;
	priv_handle->hspi.Init.FirstBit = device->lsb_first ?
					  SPI_FIRSTBIT_LSB :
					  SPI_FIRSTBIT_MSB;
	priv_handle->hspi.Init.TIMode = SPI_TIMODE_DISABLE;
	priv_handle->hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	priv_handle->hspi.Init.CRCPolynomial = 10;

	ret = HAL_SPI_Init(&priv_handle->hspi);
	if (ret != HAL_OK) {
		return -EIO;
	}

#ifdef SPI_SR_TXE
	__HAL_SPI_ENABLE(&priv_handle->hspi);
#endif

	return 0;
}

/**
 * @brief Get SPI base address from identifier
 */
static SPI_TypeDef *get_spi_base_from_identifier(uint64_t identifier)
{
	// If identifier is a base address, use it directly
	if (identifier >= 0x40000000) {
		return (SPI_TypeDef *)identifier;
	}

	// Otherwise, map device ID to base address
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

int stm32_capi_spi_init(struct capi_spi_controller_handle **handle,
			const struct capi_spi_config *config)
{
	struct capi_spi_controller_handle *spi_handle;
	struct stm32_spi_priv_handle *spi_priv_handle;
	struct stm32_spi_extra_config *spi_extra_config;
	SPI_TypeDef *base;
	int ret = 0;

	if (!handle || !config)
		return -EINVAL;

	spi_handle = (struct capi_spi_controller_handle *)no_os_calloc(1, sizeof(*spi_handle));
	if (!spi_handle)
		return -ENOMEM;

	spi_priv_handle = (struct stm32_spi_priv_handle *)no_os_calloc(1, sizeof(*spi_priv_handle));
	if (!spi_priv_handle) {
		ret = -ENOMEM;
		goto error;
	}

	spi_handle->init_allocated = true;
	spi_handle->priv = spi_priv_handle;
	spi_handle->ops = config->ops;

	spi_extra_config = config->extra;

	// Set up SPI instance
	if (spi_extra_config && spi_extra_config->hspi) {
		// Use provided HAL handle
		spi_priv_handle->hspi = *spi_extra_config->hspi;
	} else {
		// Auto-configure based on identifier
		base = get_spi_base_from_identifier(config->identifier);
		if (!base) {
			ret = -EINVAL;
			goto error;
		}
		spi_priv_handle->hspi.Instance = base;
	}

	// Initialize other fields
	spi_priv_handle->callback = NULL;
	spi_priv_handle->callback_arg = NULL;
	spi_priv_handle->current_transfer = NULL;
	spi_priv_handle->async_in_progress = false;
	spi_priv_handle->dma_done = false;

	if (spi_extra_config) {
		if (spi_extra_config->get_input_clock)
			spi_priv_handle->input_clock = spi_extra_config->get_input_clock();
		else
			spi_priv_handle->input_clock = config->clk_freq_hz;

		spi_priv_handle->alternate = spi_extra_config->alternate;

		// Set up DMA if provided
		if (spi_extra_config->dma_handle) {
			spi_priv_handle->dma_handle = spi_extra_config->dma_handle;

			// Initialize DMA channels if configured
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
		// Set up PWM if provided
		if (spi_extra_config->pwm_init) {
			ret = no_os_pwm_init(&spi_priv_handle->pwm_desc, spi_extra_config->pwm_init);
			if (ret)
				goto error;

			ret = no_os_pwm_disable(spi_priv_handle->pwm_desc);
			if (ret)
				goto error_pwm;
		}
		if (spi_extra_config->tx_pwm_init) {
			ret = no_os_pwm_init(&spi_priv_handle->tx_pwm_desc, spi_extra_config->tx_pwm_init);
			if (ret)
				goto error;

			ret = no_os_pwm_disable(spi_priv_handle->tx_pwm_desc);
			if (ret)
				goto error_pwm;
		}
#endif
	} else {
		spi_priv_handle->input_clock = config->clk_freq_hz;
	}

	// Register handle mapping for callbacks
	ret = register_handle_mapping(&spi_priv_handle->hspi, spi_priv_handle);
	if (ret != 0) {
		goto error;
	}

	*handle = spi_handle;
	return 0;

error_pwm:
#ifdef HAL_TIM_MODULE_ENABLED
	no_os_pwm_remove(spi_priv_handle->pwm_desc);
	no_os_pwm_remove(spi_priv_handle->tx_pwm_desc);
#endif
error:
	no_os_free(spi_priv_handle);
	no_os_free(spi_handle);
	return ret;
}

int stm32_capi_spi_deinit(struct capi_spi_controller_handle *handle)
{
	struct stm32_spi_priv_handle *spi_priv_handle;

	if (!handle)
		return -EINVAL;

	if (handle->init_allocated) {
		spi_priv_handle = handle->priv;

		if (spi_priv_handle) {
#ifdef HAL_TIM_MODULE_ENABLED
			no_os_pwm_remove(spi_priv_handle->pwm_desc);
			no_os_pwm_remove(spi_priv_handle->tx_pwm_desc);
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
			__HAL_SPI_DISABLE(&spi_priv_handle->hspi);
#endif
			HAL_SPI_DeInit(&spi_priv_handle->hspi);

			/* Unregister handle mapping */
			unregister_handle_mapping(&spi_priv_handle->hspi);

			/* Clean up CAPI GPIO for CS */
			if (spi_priv_handle->cs_initialized) {
				capi_gpio_port_deinit(&spi_priv_handle->cs_port_handle);
				spi_priv_handle->cs_initialized = false;
			}
		}

		no_os_free(handle->priv);
		no_os_free(handle);
	}

	return 0;
}

/**
 * @brief Setup GPIO CS pin for the device using CAPI GPIO
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
	port_config.num_pins = 16;
	port_config.flags = NULL;
	port_config.priv = &priv_handle->cs_gpio_config;

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

/**
 * @brief Perform SPI transfer with proper CS control and timing
 */
static int perform_spi_transfer_with_cs(struct stm32_spi_priv_handle *priv_handle,
					struct capi_spi_device *device,
					struct capi_spi_transfer *transfer,
					bool assert_cs, bool deassert_cs)
{
	struct stm32_spi_device_extra *dev_extra = device->extra;
	uint64_t slave_id;
	static uint64_t last_slave_id = 0;
	int ret = 0;
	HAL_StatusTypeDef hal_ret;

	/* Compute slave ID for configuration optimization */
	slave_id = ((uint64_t)(uintptr_t)priv_handle->hspi.Instance << 32) |
		   (device->cs_gpio ? device->cs_gpio->number : device->native_cs);

	if (slave_id != last_slave_id) {
		last_slave_id = slave_id;
		ret = stm32_capi_spi_config_peripheral(priv_handle, device);
		if (ret)
			return ret;
	}

	/* Assert CS if requested (drive low for active-low CS) */
	if (assert_cs && priv_handle->cs_initialized) {
		capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_LOW);
	}

	/* CS delay first */
	if (dev_extra && dev_extra->cs_delay_first) {
		no_os_udelay(dev_extra->cs_delay_first);
	}

	/* Use HAL functions for reliable operation */
	if (transfer->tx_buf && transfer->rx_buf) {
		hal_ret = HAL_SPI_TransmitReceive(&priv_handle->hspi,
						  (uint8_t *)transfer->tx_buf,
						  transfer->rx_buf,
						  no_os_max(transfer->tx_size, transfer->rx_size),
						  transfer->timeout > 0 ? transfer->timeout : HAL_MAX_DELAY);
	} else if (transfer->tx_buf) {
		hal_ret = HAL_SPI_Transmit(&priv_handle->hspi,
					   (uint8_t *)transfer->tx_buf,
					   transfer->tx_size,
					   transfer->timeout > 0 ? transfer->timeout : HAL_MAX_DELAY);
	} else {
		hal_ret = HAL_SPI_Receive(&priv_handle->hspi,
					  transfer->rx_buf,
					  transfer->rx_size,
					  transfer->timeout > 0 ? transfer->timeout : HAL_MAX_DELAY);
	}

	if (hal_ret != HAL_OK) {
		if (hal_ret == HAL_TIMEOUT)
			ret = -ETIMEDOUT;
		else
			ret = -EIO;
	}

	/* CS delay last */
	if (dev_extra && dev_extra->cs_delay_last) {
		no_os_udelay(dev_extra->cs_delay_last);
	}

	/* Deassert CS if requested (drive high for active-low CS) */
	if (deassert_cs && priv_handle->cs_initialized) {
		capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_HIGH);
	}

	// CS change delay
	if (dev_extra && dev_extra->cs_change_delay) {
		no_os_udelay(dev_extra->cs_change_delay);
	}

	return ret;
}

int stm32_capi_spi_transceive(struct capi_spi_device *device,
			      struct capi_spi_transfer *transfer)
{
	struct stm32_spi_priv_handle *priv_handle;
	int ret;

	if (!device || !device->controller || !transfer)
		return -EINVAL;

	priv_handle = device->controller->priv;

	// Set up CS GPIO if needed
	ret = setup_cs_gpio(priv_handle, device);
	if (ret)
		return ret;

	// Perform the transfer with CS control
	return perform_spi_transfer_with_cs(priv_handle, device, transfer, true, true);
}

int stm32_capi_spi_transceive_async(struct capi_spi_device *device,
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

	// Set up CS GPIO if needed
	ret = setup_cs_gpio(priv_handle, device);
	if (ret)
		return ret;

	// Configure SPI for this device
	ret = stm32_capi_spi_config_peripheral(priv_handle, device);
	if (ret)
		return ret;

	priv_handle->current_transfer = transfer;
	priv_handle->async_in_progress = true;

	// Assert CS (drive low for active-low CS)
	if (priv_handle->cs_initialized)
		capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_LOW);

	// CS delay first
	if (dev_extra && dev_extra->cs_delay_first)
		no_os_udelay(dev_extra->cs_delay_first);

	if (transfer->tx_buf && transfer->rx_buf) {
		hal_ret = HAL_SPI_TransmitReceive_IT(&priv_handle->hspi,
						     (uint8_t *)transfer->tx_buf,
						     transfer->rx_buf,
						     no_os_max(transfer->tx_size, transfer->rx_size));
	} else if (transfer->tx_buf) {
		hal_ret = HAL_SPI_Transmit_IT(&priv_handle->hspi,
					      (uint8_t *)transfer->tx_buf,
					      transfer->tx_size);
	} else if (transfer->rx_buf) {
		hal_ret = HAL_SPI_Receive_IT(&priv_handle->hspi,
					     transfer->rx_buf,
					     transfer->rx_size);
	} else {
		priv_handle->async_in_progress = false;
		// Deassert CS on error
		if (priv_handle->cs_initialized)
			capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_HIGH);
		return -EINVAL;
	}

	if (hal_ret != HAL_OK) {
		priv_handle->async_in_progress = false;
		priv_handle->current_transfer = NULL;
		// Deassert CS on error
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

int stm32_capi_spi_read_command(struct capi_spi_device *device,
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

	// Set up CS GPIO if needed
	ret = setup_cs_gpio(priv_handle, device);
	if (ret)
		return ret;

	// Configure SPI for this device
	ret = stm32_capi_spi_config_peripheral(priv_handle, device);
	if (ret)
		return ret;

	// Assert CS (drive low for active-low CS)
	if (priv_handle->cs_initialized)
		capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_LOW);

	// CS delay first
	if (dev_extra && dev_extra->cs_delay_first)
		no_os_udelay(dev_extra->cs_delay_first);

	// First transmit command/address
	if (transfer->tx_buf && transfer->tx_size > 0) {
		hal_ret = HAL_SPI_Transmit(&priv_handle->hspi,
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

	// Then receive data
	if (transfer->rx_buf && transfer->rx_size > 0) {
		hal_ret = HAL_SPI_Receive(&priv_handle->hspi,
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
	// CS delay last
	if (dev_extra && dev_extra->cs_delay_last)
		no_os_udelay(dev_extra->cs_delay_last);

	// Deassert CS (drive high for active-low CS)
	if (priv_handle->cs_initialized)
		capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_HIGH);

	// CS change delay
	if (dev_extra && dev_extra->cs_change_delay)
		no_os_udelay(dev_extra->cs_change_delay);

	return ret;
}

int stm32_capi_spi_read_command_async(struct capi_spi_device *device,
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

	// Set up CS GPIO if needed
	ret = setup_cs_gpio(priv_handle, device);
	if (ret)
		return ret;

	// Configure SPI for this device
	ret = stm32_capi_spi_config_peripheral(priv_handle, device);
	if (ret)
		return ret;

	priv_handle->current_transfer = transfer;
	priv_handle->async_in_progress = true;

	// Assert CS (drive low for active-low CS)
	if (priv_handle->cs_initialized)
		capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_LOW);

	// CS delay first
	if (dev_extra && dev_extra->cs_delay_first)
		no_os_udelay(dev_extra->cs_delay_first);

	if (transfer->tx_buf && transfer->tx_size > 0) {
		hal_ret = HAL_SPI_Transmit_IT(&priv_handle->hspi,
					      (uint8_t *)transfer->tx_buf,
					      transfer->tx_size);
	} else if (transfer->rx_buf && transfer->rx_size > 0) {
		hal_ret = HAL_SPI_Receive_IT(&priv_handle->hspi,
					     transfer->rx_buf,
					     transfer->rx_size);
	} else {
		priv_handle->async_in_progress = false;
		// Deassert CS on error
		if (priv_handle->cs_initialized)
			capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_HIGH);
		return -EINVAL;
	}

	if (hal_ret != HAL_OK) {
		priv_handle->async_in_progress = false;
		priv_handle->current_transfer = NULL;
		// Deassert CS on error
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

int stm32_capi_spi_abort_async(struct capi_spi_device *device)
{
	struct stm32_spi_priv_handle *priv_handle;
	HAL_StatusTypeDef hal_ret;
	int ret = 0;

	if (!device || !device->controller)
		return -EINVAL;

	priv_handle = device->controller->priv;

	if (!priv_handle->async_in_progress)
		return 0;

	// Abort DMA transfers if they are active
	if (priv_handle->dma_handle && (priv_handle->rxdma_ch || priv_handle->txdma_ch)) {
		ret = stm32_capi_spi_dma_abort(device);
		if (ret)
			return ret;
	}

	hal_ret = HAL_SPI_Abort_IT(&priv_handle->hspi);

	// Deassert CS immediately on abort
	if (priv_handle->cs_initialized)
		capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_HIGH);

	priv_handle->async_in_progress = false;
	priv_handle->current_transfer = NULL;

	if (hal_ret != HAL_OK) {
		switch (hal_ret) {
		case HAL_ERROR:
			ret = -EIO;
			break;
		default:
			ret = -EIO;
			break;
		}
	}

	// Notify callback about abort
	if (priv_handle->callback) {
		priv_handle->callback(CAPI_SPI_EVENT_ERROR,
				      priv_handle->callback_arg,
				      -ECANCELED);
	}

	return ret;
}

int stm32_capi_spi_register_callback(struct capi_spi_controller_handle *handle,
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

int stm32_capi_spi_set_cs(struct capi_spi_device *device,
			  enum capi_spi_cs_control cs_control)
{
	struct stm32_spi_priv_handle *priv_handle;

	if (!device || !device->controller)
		return -EINVAL;

	priv_handle = device->controller->priv;

	if (!priv_handle->cs_initialized)
		return -ENOSYS;

	switch (cs_control) {
	case CAPI_SPI_CS_AUTO:
		/* HAL handles CS automatically - nothing to do */
		break;
	case CAPI_SPI_CS_MANUAL_ASSERT:
		/* Assert CS (drive low for active-low CS) */
		capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_LOW);
		break;
	case CAPI_SPI_CS_MANUAL_DEASSERT:
		/* Deassert CS (drive high for active-low CS) */
		capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_HIGH);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

void stm32_capi_spi_isr(void *handle)
{
	struct capi_spi_controller_handle *controller_handle = (struct capi_spi_controller_handle *)handle;
	struct stm32_spi_priv_handle *priv_handle;

	if (!controller_handle)
		return;

	priv_handle = controller_handle->priv;
	HAL_SPI_IRQHandler(&priv_handle->hspi);
}

/* STM32-specific features */

int stm32_capi_spi_transfer_multiple(struct capi_spi_device *device,
				     struct capi_spi_transfer *transfers,
				     uint32_t transfer_count)
{
	struct stm32_spi_priv_handle *priv_handle;
	int ret;

	if (!device || !device->controller || !transfers || !transfer_count)
		return -EINVAL;

	priv_handle = device->controller->priv;

	// Set up CS GPIO if needed
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

int stm32_capi_spi_alternate_cs_enable(struct capi_spi_device *device, bool enable)
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
	gpio_config.Pin = NO_OS_BIT(priv_handle->chip_select.number);
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
 * @brief DMA completion callback for SPI transfers
 */
static void stm32_capi_spi_dma_callback(struct capi_dma_transfer *xfer, void *ctx)
{
	struct capi_spi_device *device = (struct capi_spi_device *)ctx;
	struct stm32_spi_priv_handle *priv_handle = device->controller->priv;

	/* if more xfers pending dont do anything */
	if (priv_handle->current_transfer_index < priv_handle->transfer_count - 1)
		return;

#ifdef HAL_TIM_MODULE_ENABLED
	if (priv_handle->pwm_desc)
		no_os_pwm_disable(priv_handle->pwm_desc);

	if (priv_handle->tx_pwm_desc)
		no_os_pwm_disable(priv_handle->tx_pwm_desc);
#endif

	/* Perform abort SPI transfers */
	stm32_capi_spi_abort_async(device);

	priv_handle->dma_done = true;

	if (priv_handle->dma_user_cb)
		priv_handle->dma_user_cb(priv_handle->dma_user_ctx);
}

/**
 * @brief Configure and start DMA transfers
 */
static int stm32_capi_config_dma_and_start(struct capi_spi_device *device,
					    struct capi_spi_transfer *transfers,
					    uint32_t transfer_count,
					    void (*completion_callback)(struct capi_dma_transfer *, void *),
					    void *callback_ctx)
{
	struct stm32_spi_priv_handle *priv_handle = device->controller->priv;
	struct capi_dma_transfer *rx_ch_xfer;
	struct capi_dma_transfer *tx_ch_xfer;
	SPI_TypeDef *SPIx = priv_handle->hspi.Instance;
	int ret;
	uint32_t i;

	if (!device || !transfers || !priv_handle->dma_handle)
		return -EINVAL;

	if (!priv_handle->rxdma_ch || !priv_handle->txdma_ch)
		return -EINVAL;

	rx_ch_xfer = no_os_calloc(transfer_count, sizeof(*rx_ch_xfer));
	if (!rx_ch_xfer)
		return -ENOMEM;

	tx_ch_xfer = no_os_calloc(transfer_count, sizeof(*tx_ch_xfer));
	if (!tx_ch_xfer) {
		ret = -ENOMEM;
		goto free_rx_ch_xfer;
	}

	for (i = 0; i < transfer_count; i++) {
		// Configure TX DMA transfer
		tx_ch_xfer[i].src = (capi_dma_cpu_addr_t)transfers[i].tx_buf;
#ifndef SPI_SR_TXE
		tx_ch_xfer[i].dst = (capi_dma_cpu_addr_t)&(SPIx->TXDR);
#else
		tx_ch_xfer[i].dst = (capi_dma_cpu_addr_t)&(SPIx->DR);
#endif
		tx_ch_xfer[i].xfer_type = CAPI_DMA_MEM_TO_DEV;
		tx_ch_xfer[i].src_inc = CAPI_DMA_BYTE_INCREMENT;
		tx_ch_xfer[i].dst_inc = CAPI_DMA_NO_INCREMENT;
		tx_ch_xfer[i].length = transfers[i].tx_size;
		tx_ch_xfer[i].xfer_complete_cb = NULL;
		tx_ch_xfer[i].user_data = NULL;

		// Configure RX DMA transfer
		rx_ch_xfer[i].dst = (capi_dma_cpu_addr_t)transfers[i].rx_buf;
#ifndef SPI_SR_RXNE
		rx_ch_xfer[i].src = (capi_dma_cpu_addr_t)&(SPIx->RXDR);
#else
		rx_ch_xfer[i].src = (capi_dma_cpu_addr_t)&(SPIx->DR);
#endif
		rx_ch_xfer[i].xfer_type = CAPI_DMA_DEV_TO_MEM;
		rx_ch_xfer[i].src_inc = CAPI_DMA_NO_INCREMENT;
		rx_ch_xfer[i].dst_inc = CAPI_DMA_BYTE_INCREMENT;
		rx_ch_xfer[i].length = transfers[i].rx_size;

		if (completion_callback && i == transfer_count - 1) {
			rx_ch_xfer[i].xfer_complete_cb = completion_callback;
			rx_ch_xfer[i].user_data = callback_ctx;
		} else {
			rx_ch_xfer[i].xfer_complete_cb = NULL;
			rx_ch_xfer[i].user_data = NULL;
		}
	}

	priv_handle->tx_ch_xfer = tx_ch_xfer;
	priv_handle->rx_ch_xfer = rx_ch_xfer;
	priv_handle->transfer_count = transfer_count;
	priv_handle->current_transfer_index = 0;

	// Configure RX DMA channel
	ret = capi_dma_config_xfer(priv_handle->rxdma_ch, &rx_ch_xfer[0]);
	if (ret)
		goto cleanup_dma;

	// Configure TX DMA channel
	ret = capi_dma_config_xfer(priv_handle->txdma_ch, &tx_ch_xfer[0]);
	if (ret)
		goto cleanup_dma;

	// Start TX DMA transfer
	ret = capi_dma_xfer_start(priv_handle->txdma_ch);
	if (ret)
		goto abort_transfer;

	// Start RX DMA transfer
	ret = capi_dma_xfer_start(priv_handle->rxdma_ch);
	if (ret)
		goto abort_transfer;

	// Enable SPI DMA requests
	if (priv_handle->txdma_ch) {
#if defined (STM32H5)
		SET_BIT(priv_handle->hspi.Instance->CFG1, SPI_CFG1_TXDMAEN);
#else
		SET_BIT(priv_handle->hspi.Instance->CR2, SPI_CR2_TXDMAEN);
#endif
	}

	if (priv_handle->rxdma_ch) {
#if defined (STM32H5)
		SET_BIT(priv_handle->hspi.Instance->CFG1, SPI_CFG1_RXDMAEN);
#else
		SET_BIT(priv_handle->hspi.Instance->CR2, SPI_CR2_RXDMAEN);
#endif
	}

#ifdef HAL_TIM_MODULE_ENABLED
	if (priv_handle->pwm_desc) {
		stm32_capi_spi_alternate_cs_enable(device, true);
		ret = no_os_pwm_enable(priv_handle->pwm_desc);
		if (ret)
			goto abort_transfer;
	}
	if (priv_handle->tx_pwm_desc) {
		ret = no_os_pwm_enable(priv_handle->tx_pwm_desc);
		if (ret)
			goto abort_transfer;
	}
#endif

	return 0;

abort_transfer:
	capi_dma_xfer_abort(priv_handle->txdma_ch);
	capi_dma_xfer_abort(priv_handle->rxdma_ch);
cleanup_dma:
	no_os_free(tx_ch_xfer);
free_rx_ch_xfer:
	no_os_free(rx_ch_xfer);
	return ret;
}

/**
 * @brief Abort DMA transfers and cleanup
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

	SPIx = priv_handle->hspi.Instance;

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
	no_os_free(priv_handle->tx_ch_xfer);
	no_os_free(priv_handle->rx_ch_xfer);
	priv_handle->tx_ch_xfer = NULL;
	priv_handle->rx_ch_xfer = NULL;

	/* put CS pin back into gpio mode */
	stm32_capi_spi_alternate_cs_enable(device, false);

	return 0;
}

int stm32_capi_spi_transfer_dma_async(struct capi_spi_device *device,
				      struct capi_spi_transfer *transfer,
				      void (*callback)(void*),
				      void* callback_arg)
{
	return stm32_capi_spi_transfer_multiple_dma_async(device, transfer, 1, callback, callback_arg);
}

int stm32_capi_spi_transfer_dma(struct capi_spi_device *device,
				struct capi_spi_transfer *transfer)
{
	return stm32_capi_spi_transfer_multiple_dma(device, transfer, 1);
}

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

	// Wait for completion with timeout
	timeout = transfers[0].tx_size + transfers[0].rx_size;
	if (timeout == 0)
		timeout = 1000; // Default 1 second timeout

	while (timeout--) {
		no_os_mdelay(1);
		if (priv_handle->dma_done)
			break;
	}

	if (timeout == 0) {
		stm32_capi_spi_dma_abort(device);
		return -ETIMEDOUT;
	}

	return 0;
}

struct capi_spi_ops stm32_capi_spi_ops = {
	.init = &stm32_capi_spi_init,
	.deinit = &stm32_capi_spi_deinit,
	.transceive = &stm32_capi_spi_transceive,
	.transceive_async = &stm32_capi_spi_transceive_async,
	.read_command = &stm32_capi_spi_read_command,
	.read_command_async = &stm32_capi_spi_read_command_async,
	.abort_async = &stm32_capi_spi_abort_async,
	.register_callback = &stm32_capi_spi_register_callback,
	.set_cs = &stm32_capi_spi_set_cs,
	.isr = &stm32_capi_spi_isr,
};

struct stm32_capi_spi_extended_ops stm32_capi_spi_extended_ops = {
	.base_ops = {
		.init = &stm32_capi_spi_init,
		.deinit = &stm32_capi_spi_deinit,
		.transceive = &stm32_capi_spi_transceive,
		.transceive_async = &stm32_capi_spi_transceive_async,
		.read_command = &stm32_capi_spi_read_command,
		.read_command_async = &stm32_capi_spi_read_command_async,
		.abort_async = &stm32_capi_spi_abort_async,
		.register_callback = &stm32_capi_spi_register_callback,
		.set_cs = &stm32_capi_spi_set_cs,
		.isr = &stm32_capi_spi_isr,
	},
	.transfer_dma = &stm32_capi_spi_transfer_dma,
	.transfer_dma_async = &stm32_capi_spi_transfer_dma_async,
	.transfer_multiple_dma = &stm32_capi_spi_transfer_multiple_dma,
	.transfer_multiple_dma_async = &stm32_capi_spi_transfer_multiple_dma_async,
	.alternate_cs_enable = &stm32_capi_spi_alternate_cs_enable,
};

/* HAL SPI Callbacks - these functions are called by HAL when async operations complete */

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	struct stm32_spi_priv_handle *priv_handle;
	struct capi_spi_transfer *transfer;

	priv_handle = find_priv_handle(hspi);
	if (!priv_handle) {
		return;
	}

	transfer = priv_handle->current_transfer;

	// For read command async, check if we need to start RX phase
	if (transfer && transfer->rx_buf && transfer->rx_size > 0 && transfer->tx_size > 0) {
		// Start RX phase for read command
		HAL_StatusTypeDef hal_ret = HAL_SPI_Receive_IT(hspi, transfer->rx_buf, transfer->rx_size);
		if (hal_ret != HAL_OK) {
			// Error in RX phase - deassert CS
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

	// TX complete - deassert CS
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

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	struct stm32_spi_priv_handle *priv_handle;

	priv_handle = find_priv_handle(hspi);
	if (!priv_handle) {
		return;
	}

	// RX complete - deassert CS
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

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	struct stm32_spi_priv_handle *priv_handle;

	priv_handle = find_priv_handle(hspi);
	if (!priv_handle) {
		return;
	}

	// TxRx complete - deassert CS
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

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	struct stm32_spi_priv_handle *priv_handle;

	priv_handle = find_priv_handle(hspi);
	if (!priv_handle) {
		return;
	}

	// Error - deassert CS
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

void HAL_SPI_AbortCpltCallback(SPI_HandleTypeDef *hspi)
{
	struct stm32_spi_priv_handle *priv_handle;

	priv_handle = find_priv_handle(hspi);
	if (!priv_handle) {
		return;
	}

	// Abort - deassert CS
	if (priv_handle->cs_initialized)
		capi_gpio_pin_set_raw_value(&priv_handle->chip_select, CAPI_GPIO_HIGH);

	priv_handle->async_in_progress = false;
	priv_handle->current_transfer = NULL;

	if (priv_handle->callback) {
		priv_handle->callback(CAPI_SPI_EVENT_ERROR,
				      priv_handle->callback_arg,
				      -ECANCELED);
	}
}