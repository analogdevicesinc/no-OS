/***************************************************************************//**
 *   @file   maxim_spi.c
 *   @brief  Source file of the SPI driver for the maxim platform
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "maxim_spi.h"
#include "maxim_pinctrl.h"
#include "capi_spi.h"
#include "capi_pinctrl.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "spi.h"
#include "mxc_sys.h"
#include "mxc_errors.h"
#include "no_os_delay.h"

#define MAX_SPI_INSTANCES     3

static uintptr_t last_spi_device[MXC_SPI_INSTANCES];

/**
 * @brief Convert CAPI SPI mode to Maxim SDK mode
 * @param mode CAPI SPI mode
 * @return Maxim SPI mode
 */
static mxc_spi_mode_t capi_to_maxim_mode(enum capi_spi_mode mode)
{
	switch (mode) {
	case CAPI_SPI_MODE_0:
		return SPI_MODE_0;
	case CAPI_SPI_MODE_1:
		return SPI_MODE_1;
	case CAPI_SPI_MODE_2:
		return SPI_MODE_2;
	case CAPI_SPI_MODE_3:
		return SPI_MODE_3;
	default:
		return SPI_MODE_0;
	}
}

/**
 * @brief Configure SPI pins using pinctrl if provided
 * @param device SPI device descriptor
 * @param priv Private handle structure
 * @return 0 on success, negative error code otherwise
 */
static int configure_spi_pins(struct capi_spi_device *device,
			      struct maxim_capi_spi_handle *priv)
{
	struct maxim_spi_pinctrl_config *pin_config;
	int ret;

	if (!device->extra)
		return 0; /* No pin configuration provided */

	pin_config = (struct maxim_spi_pinctrl_config *)device->extra;
	if (!pin_config->pins || pin_config->num_pins == 0)
		return 0; /* No pins to configure */

	/* Initialize pinctrl if not already done */
	if (!priv->pinctrl_handle) {
		/* Set up default pinctrl configuration */
		struct capi_pinctrl_config pinctrl_config = {
			.ops = &maxim_pinctrl_ops,
			.identifier = 0, /* Global pinctrl instance */
			.flags = NULL,
			.extra = NULL
		};

		/* Use provided pinctrl config if available */
		if (pin_config->pinctrl_config.ops) {
			pinctrl_config = pin_config->pinctrl_config;
		}

		ret = capi_pinctrl_init(&priv->pinctrl_handle, &pinctrl_config);
		if (ret)
			return ret;
	}

	/* Configure the pins */
	ret = capi_pinctrl_set_pins(priv->pinctrl_handle, pin_config->pins, pin_config->num_pins);
	if (ret) {
		/* Cleanup on failure */
		capi_pinctrl_deinit(&priv->pinctrl_handle);
		priv->pinctrl_handle = NULL;
		return ret;
	}

	return 0;
}

/**
 * @brief SPI interrupt callback handler
 * @param req SPI request structure
 * @param result Transfer result
 */
static void maxim_spi_callback_handler(mxc_spi_req_t *req, int result)
{
	struct capi_spi_controller_handle *handle = (struct capi_spi_controller_handle *)req->completeCB;
	struct maxim_capi_spi_handle *priv;
	enum capi_async_event event;

	if (!handle || !handle->priv)
		return;

	priv = (struct maxim_capi_spi_handle *)handle->priv;
	priv->transfer_active = false;

	/* Convert result to CAPI event */
	if (result == E_NO_ERROR) {
		event = CAPI_SPI_EVENT_XFR_DONE;
	} else if (result == E_TIME_OUT) {
		event = CAPI_SPI_EVENT_XFR_TIMEOUT;
	} else {
		event = CAPI_SPI_EVENT_ERROR;
	}

	/* Call user callback if registered */
	if (priv->callback) {
		priv->callback(event, priv->callback_arg, result);
	}
}

/**
 * @brief Initialize SPI CAPI handle
 * @param handle Double pointer to SPI handle (allocated if *handle is NULL)
 * @param config SPI configuration
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_spi_init(struct capi_spi_controller_handle **handle,
			       const struct capi_spi_config *config)
{
	struct capi_spi_controller_handle *spi_handle;
	struct maxim_capi_spi_handle *priv;
	mxc_spi_pins_t spi_pins;
	int ret;

	if (!handle || !config)
		return -EINVAL;

	if (config->identifier >= MAX_SPI_INSTANCES)
		return -EINVAL;

	/* Handle dynamic vs static allocation */
	if (!*handle) {
		/* Dynamic allocation */
		spi_handle = no_os_calloc(1, sizeof(struct capi_spi_controller_handle));
		if (!spi_handle)
			return -ENOMEM;

		priv = no_os_calloc(1, sizeof(struct maxim_capi_spi_handle));
		if (!priv) {
			no_os_free(spi_handle);
			return -ENOMEM;
		}

		spi_handle->init_allocated = true;
		spi_handle->priv = priv;
	} else {
		/* Static allocation */
		spi_handle = *handle;
		priv = (struct maxim_capi_spi_handle *)spi_handle->priv;
		if (!priv)
			return -EINVAL;

		/* Clear the private structure */
		memset(priv, 0, sizeof(struct maxim_capi_spi_handle));
		spi_handle->init_allocated = false;
	}

	spi_pins.clock = 1;
	spi_pins.miso = 1;
	spi_pins.mosi = 1;
	spi_pins.sdio2 = 0;
	spi_pins.sdio3 = 0;
	spi_pins.ss0 = 1;
	spi_pins.ss1 = 0;
	spi_pins.ss2 = 0;
	spi_pins.vddioh = 1;

	/* Set up the handle */
	spi_handle->ops = &maxim_spi_ops;

	/* Initialize private data */
	priv->instance = config->identifier;
	priv->regs = MXC_SPI_GET_SPI(config->identifier);
	priv->clk_freq_hz = config->clk_freq_hz ? config->clk_freq_hz : 1000000;  /* Default 1MHz */
	priv->three_pin_mode = config->three_pin_mode;
	priv->loopback = config->loopback;
	priv->transfer_active = false;
	priv->pinctrl_handle = NULL;

	/* Initialize SPI hardware */
	ret = MXC_SPI_Init(priv->regs, 1, 0, 1, 0, priv->clk_freq_hz, spi_pins);
	if (ret != E_NO_ERROR) {
		ret = -EIO;
		goto cleanup;
	}

	ret = MXC_SPI_SetMode(priv->regs, 0);
	if (ret) {
		ret = -EINVAL;
		goto cleanup;
	}

	ret = MXC_SPI_SetWidth(priv->regs, SPI_WIDTH_STANDARD);
	if (ret) {
		ret = -EINVAL;
		goto cleanup;
	}

	ret = MXC_SPI_SetDataSize(priv->regs, 8);
	if (ret) {
		ret = -EINVAL;
		goto cleanup;
	}

	/* Configure SPI settings */
	if (priv->loopback) {
		/* Enable loopback mode if requested */
		/* Note: This would require platform-specific register manipulation */
	}

	*handle = spi_handle;
	return 0;

cleanup:
	if (spi_handle->init_allocated) {
		no_os_free(priv);
		no_os_free(spi_handle);
	}
	return ret;
}

/**
 * @brief Deinitialize SPI CAPI handle
 * @param handle SPI handle to deinitialize
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_spi_deinit(struct capi_spi_controller_handle *handle)
{
	struct maxim_capi_spi_handle *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = (struct maxim_capi_spi_handle *)handle->priv;

	/* Deinitialize pinctrl if it was initialized */
	// if (priv->pinctrl_handle) {
	// 	capi_pinctrl_deinit(&priv->pinctrl_handle);
	// 	priv->pinctrl_handle = NULL;
	// }

	/* Shutdown SPI */
	MXC_SPI_Shutdown(priv->regs);

	/* Free dynamically allocated memory */
	if (handle->init_allocated) {
		no_os_free(priv);
		no_os_free(handle);
	}

	return 0;
}

/**
 * @brief Configure SPI for device settings
 * @param priv Private handle structure
 * @param device SPI device descriptor
 * @return 0 on success, negative error code otherwise
 */
static int configure_spi_for_device(struct maxim_capi_spi_handle *priv,
				    struct capi_spi_device *device)
{
	mxc_spi_mode_t mode;
	int ret;

	/* Configure SPI pins if provided */
	// ret = configure_spi_pins(device, priv);
	// if (ret)
	// 	return ret;

	/* Convert and set SPI mode */
	mode = capi_to_maxim_mode(device->mode);
	ret = MXC_SPI_SetMode(priv->regs, mode);
	if (ret != E_NO_ERROR)
		return -EIO;

	/* Set frequency */
	ret = MXC_SPI_SetFrequency(priv->regs, device->max_speed_hz);
	if (ret != E_NO_ERROR)
		return -EIO;

	/* Configure data order */
	ret = MXC_SPI_SetDataSize(priv->regs, 8);  /* 8-bit data */
	if (ret != E_NO_ERROR)
		return -EIO;

	return 0;
}

/**
 * @brief Handle chip select control
 * @param priv Private handle structure
 * @param device SPI device descriptor
 * @param assert True to assert CS, false to deassert
 * @return 0 on success, negative error code otherwise
 */
static int handle_chip_select(struct maxim_capi_spi_handle *priv,
			      struct capi_spi_device *device, bool assert)
{
	/* Handle native chip select */
	if (device->native_cs != 0) {
		/* Use hardware CS - Maxim SDK handles this automatically */
		return 0;
	}

	/* Handle GPIO chip select */
	if (device->cs_gpio && device->cs_gpio_num > 0) {
		/* GPIO CS would be handled here using CAPI GPIO */
		/* For now, return success as GPIO CS implementation */
		/* would require capi_gpio.h inclusion */
		return 0;
	}

	return 0;
}

/**
 * @brief Synchronous SPI transceive using lower-level FIFO access
 * @param device SPI device descriptor
 * @param transfer SPI transfer descriptor
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_spi_transceive(struct capi_spi_device *device,
				     struct capi_spi_transfer *transfer)
{
	struct no_os_time t_start, t_current;
	uint32_t elapsed_ms;
	struct maxim_capi_spi_handle *priv;
	mxc_spi_regs_t *spi;
	uint32_t tx_cnt = 0, rx_cnt = 0;
	bool rx_done = true, tx_done = true;
	int ret;

	if (!device || !device->controller || !device->controller->priv || !transfer)
		return -EINVAL;

	priv = (struct maxim_capi_spi_handle *)device->controller->priv;
	spi = priv->regs;

	if (priv->transfer_active)
		return -EBUSY;

	/* Reconfigure the SPI  */
	if (device != last_spi_device[MXC_SPI_GET_IDX(spi)]){
		ret = configure_spi_for_device(priv, device);
		if (ret)
			return ret;
	}

	/* Handle chip select assertion */
	ret = handle_chip_select(priv, device, true);
	if (ret)
		return ret;

	priv->transfer_active = true;

	/* Assert CS for this device */
	spi->ctrl0 &= ~MXC_F_SPI_CTRL0_SS_ACTIVE;
	spi->ctrl0 |= no_os_field_prep(MXC_F_SPI_CTRL0_SS_ACTIVE,
				       NO_OS_BIT(device->native_cs ? device->native_cs : 0));

	/* Flush the RX and TX FIFOs */
	spi->dma |= MXC_F_SPI_DMA_RX_FLUSH | MXC_F_SPI_DMA_TX_FLUSH;

	/* Enable SPI and clear done flag */
	spi->intfl |= MXC_F_SPI_INTFL_MST_DONE;
	spi->ctrl1 = 0;

	/* Configure chip select control */
	spi->ctrl0 &= ~MXC_F_SPI_CTRL0_SS_CTRL;  /* Hardware controlled CS */

	if (transfer->tx_buf && transfer->tx_size > 0) {
		/* Set the transfer size in the TX direction */
		spi->ctrl1 = transfer->tx_size;
		tx_done = false;
		/* Enable the TX FIFO */
		spi->dma |= MXC_F_SPI_DMA_TX_FIFO_EN;
		/* Fill initial TX FIFO */
		tx_cnt += MXC_SPI_WriteTXFIFO(spi, (uint8_t *)&transfer->tx_buf[tx_cnt],
					      transfer->tx_size - tx_cnt);
		tx_done = (tx_cnt == transfer->tx_size) ? true : false;
	}

	if (transfer->rx_buf && transfer->rx_size > 0) {
		/* Set the transfer size in the RX direction */
		spi->ctrl1 |= no_os_field_prep(MXC_F_SPI_CTRL1_RX_NUM_CHAR,
					       transfer->rx_size);
		/* Enable the RX FIFO */
		spi->dma |= MXC_F_SPI_DMA_RX_FIFO_EN;
		rx_done = false;
	}

	t_start = no_os_get_time();

	/* Start the transaction */
	spi->ctrl0 |= MXC_F_SPI_CTRL0_START;

	/* Handle the transfer with FIFO read/write */
	while (!(rx_done && tx_done)) {
		/* Continue writing to TX FIFO if needed */
		if (transfer->tx_buf && tx_cnt < transfer->tx_size) {
			tx_cnt += MXC_SPI_WriteTXFIFO(spi, (uint8_t *)&transfer->tx_buf[tx_cnt],
						      transfer->tx_size - tx_cnt);
			tx_done = (tx_cnt == transfer->tx_size) ? true : false;
		}

		/* Read from RX FIFO if needed */
		if (transfer->rx_buf && rx_cnt < transfer->rx_size) {
			rx_cnt += MXC_SPI_ReadRXFIFO(spi, &transfer->rx_buf[rx_cnt],
						     transfer->rx_size - rx_cnt);
			rx_done = (rx_cnt == transfer->rx_size) ? true : false;
		}

		t_current = no_os_get_time();
		elapsed_ms = (t_current.s - t_start.s) * 1000 - (t_current.us - t_start.us) / 1000;

		if (transfer->timeout > 0 && elapsed_ms > transfer->timeout){
			ret = -ETIMEDOUT;
			goto out;
		}
	}

	/* Wait for the RX and TX FIFOs to empty and transfer to complete */
	while (!(spi->intfl & MXC_F_SPI_INTFL_MST_DONE)){
		t_current = no_os_get_time();
		elapsed_ms = (t_current.s - t_start.s) * 1000 - (t_current.us - t_start.us) / 1000;

		if (transfer->timeout > 0 && elapsed_ms > transfer->timeout){
			ret = -ETIMEDOUT;
			goto out;
		}
	}

out:
	/* End the transaction */
	spi->ctrl0 &= ~MXC_F_SPI_CTRL0_START;

	/* Disable the RX and TX FIFOs */
	spi->dma &= ~(MXC_F_SPI_DMA_TX_FIFO_EN | MXC_F_SPI_DMA_RX_FIFO_EN);

	priv->transfer_active = false;

	/* Handle chip select deassertion */
	handle_chip_select(priv, device, false);

	return ret;
}

/**
 * @brief Asynchronous SPI transceive
 * @param device SPI device descriptor
 * @param transfer SPI transfer descriptor
 * @param timeout Timeout in milliseconds
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_spi_transceive_async(struct capi_spi_device *device,
					   struct capi_spi_transfer *transfer,
					   int timeout)
{
	struct maxim_capi_spi_handle *priv;
	mxc_spi_req_t *req;
	int ret;

	if (!device || !device->controller || !device->controller->priv || !transfer)
		return -EINVAL;

	priv = (struct maxim_capi_spi_handle *)device->controller->priv;

	if (priv->transfer_active)
		return -EBUSY;

	/* Configure SPI for this device */
	ret = configure_spi_for_device(priv, device);
	if (ret)
		return ret;

	/* Use the stored request structure */
	req = &priv->req;
	memset(req, 0, sizeof(*req));
	req->spi = priv->regs;
	req->txData = (uint8_t *)transfer->tx_buf;
	req->rxData = transfer->rx_buf;
	req->txLen = transfer->tx_size;
	req->rxLen = transfer->rx_size;
	req->ssIdx = device->native_cs ? device->native_cs : 0;
	req->ssDeassert = 1;  /* Deassert CS after transfer */
	req->txCnt = 0;
	req->rxCnt = 0;
	req->completeCB = maxim_spi_callback_handler;

	/* Handle chip select assertion */
	ret = handle_chip_select(priv, device, true);
	if (ret)
		return ret;

	priv->transfer_active = true;
	ret = MXC_SPI_MasterTransactionAsync(req);

	if (ret != E_NO_ERROR) {
		priv->transfer_active = false;
		handle_chip_select(priv, device, false);
		return -EIO;
	}

	return 0;
}

/**
 * @brief SPI read command
 * @param device SPI device descriptor
 * @param transfer SPI transfer descriptor
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_spi_read_command(struct capi_spi_device *device,
				       struct capi_spi_transfer *transfer)
{
	/* For read command, we can use the same implementation as transceive */
	/* The difference is mainly in how the transfer is set up */
	return maxim_capi_spi_transceive(device, transfer);
}

/**
 * @brief Asynchronous SPI read command
 * @param device SPI device descriptor
 * @param transfer SPI transfer descriptor
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_spi_read_command_async(struct capi_spi_device *device,
					     struct capi_spi_transfer *transfer)
{
	/* For read command, we can use the same implementation as transceive_async */
	return maxim_capi_spi_transceive_async(device, transfer, transfer->timeout);
}

/**
 * @brief Abort asynchronous SPI transfer
 * @param device SPI device descriptor
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_spi_abort_async(struct capi_spi_device *device)
{
	struct maxim_capi_spi_handle *priv;

	if (!device || !device->controller || !device->controller->priv)
		return -EINVAL;

	priv = (struct maxim_capi_spi_handle *)device->controller->priv;

	if (!priv->transfer_active)
		return -EINVAL;

	/* Abort current transfer */
	MXC_SPI_AbortAsync(priv->regs);
	priv->transfer_active = false;

	/* Handle chip select deassertion */
	handle_chip_select(priv, device, false);

	return 0;
}

/**
 * @brief Register callback for SPI events
 * @param device SPI device descriptor
 * @param callback Callback function
 * @param callback_arg Callback argument
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_spi_register_callback(struct capi_spi_device *device,
					    capi_spi_callback_t const callback,
					    void *callback_arg)
{
	struct maxim_capi_spi_handle *priv;

	if (!device || !device->controller || !device->controller->priv)
		return -EINVAL;

	priv = (struct maxim_capi_spi_handle *)device->controller->priv;

	priv->callback = callback;
	priv->callback_arg = callback_arg;

	return 0;
}

/**
 * @brief Set chip select control
 * @param device SPI device descriptor
 * @param cs_control Chip select control mode
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_spi_set_cs(struct capi_spi_device *device,
				 enum capi_spi_cs_control cs_control)
{
	struct maxim_capi_spi_handle *priv;

	if (!device || !device->controller || !device->controller->priv)
		return -EINVAL;

	priv = (struct maxim_capi_spi_handle *)device->controller->priv;

	switch (cs_control) {
	case CAPI_SPI_CS_AUTO:
		/* Default hardware-controlled CS */
		break;
	case CAPI_SPI_CS_MANUAL_ASSERT:
		return handle_chip_select(priv, device, true);
	case CAPI_SPI_CS_MANUAL_DEASSERT:
		return handle_chip_select(priv, device, false);
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief SPI interrupt service routine
 * @param handle SPI controller handle
 */
static void maxim_capi_spi_isr(void *handle)
{
	struct capi_spi_controller_handle *spi_handle = (struct capi_spi_controller_handle *)handle;
	struct maxim_capi_spi_handle *priv;

	if (!spi_handle || !spi_handle->priv)
		return;

	priv = (struct maxim_capi_spi_handle *)spi_handle->priv;

	/* Call Maxim SDK interrupt handler */
	MXC_SPI_AsyncHandler(priv->regs);
}

/**
 * @brief MAX32655 SPI CAPI operations structure
 */
const struct capi_spi_ops maxim_spi_ops = {
	.init = maxim_capi_spi_init,
	.deinit = maxim_capi_spi_deinit,
	.transceive = maxim_capi_spi_transceive,
	.transceive_async = maxim_capi_spi_transceive_async,
	.read_command = maxim_capi_spi_read_command,
	.read_command_async = maxim_capi_spi_read_command_async,
	.abort_async = maxim_capi_spi_abort_async,
	.register_callback = maxim_capi_spi_register_callback,
	.set_cs = maxim_capi_spi_set_cs,
	.isr = maxim_capi_spi_isr,
};