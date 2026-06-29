/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Xilinx XSpi AXI Quad SPI driver (master mode only)
 *
 * SDT BSP only. IRQ is explicit via capi_spi_xilinx_config.
 * capi_irq_init() must be called before async ops will work.
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <capi_spi.h>
#include <xilinx_capi_spi.h>
#include <capi_gpio.h>
#include "capi_irq.h"
#include "xinterrupt_wrap.h"
#include "xspi.h"

static int capi_spi_pl_init(struct capi_spi_controller_handle **handle,
                            const struct capi_spi_config *config);
static int capi_spi_pl_deinit(struct capi_spi_controller_handle *handle);
static int capi_spi_pl_transceive(struct capi_spi_device *device,
                                  struct capi_spi_transfer *transfer);
static int capi_spi_pl_transceive_async(struct capi_spi_device *device,
                                        struct capi_spi_transfer *transfer, int timeout);
static int capi_spi_pl_read_command(struct capi_spi_device *device,
                                    struct capi_spi_transfer *transfer);
static int capi_spi_pl_read_command_async(struct capi_spi_device *device,
                struct capi_spi_transfer *transfer);
static int capi_spi_pl_abort_async(struct capi_spi_device *device);
static int capi_spi_pl_register_callback(struct capi_spi_controller_handle
                *handle,
                capi_spi_callback_t const callback, void *callback_arg);
static int capi_spi_pl_set_cs(struct capi_spi_device *device,
                              enum capi_spi_cs_control cs);
static void capi_spi_pl_isr(void *handle);
static void spi_status_handler(void *callback_ref, u32 status_event,
                               unsigned int byte_count);

struct capi_spi_ops capi_spi_xilinx_pl_ops = {
	.init = capi_spi_pl_init,
	.deinit = capi_spi_pl_deinit,
	.transceive = capi_spi_pl_transceive,
	.transceive_async = capi_spi_pl_transceive_async,
	.read_command = capi_spi_pl_read_command,
	.read_command_async = capi_spi_pl_read_command_async,
	.abort_async = capi_spi_pl_abort_async,
	.register_callback = capi_spi_pl_register_callback,
	.set_cs = capi_spi_pl_set_cs,
	.isr = capi_spi_pl_isr,
};

static XSpi *inst(struct capi_spi_xilinx_handle *xh)
{
	return (XSpi *)xh->instance;
}

static int gpio_cs_assert(const struct capi_spi_device *device);
static int gpio_cs_deassert(const struct capi_spi_device *device);

static int pl_async_cleanup(struct capi_spi_xilinx_handle *xh, bool copy_rx)
{
	int cs_ret = 0;

	if (copy_rx && xh->async_rx_temp != NULL &&
	    xh->async_user_rx_buf != NULL && xh->async_rx_size > 0) {
		memcpy(xh->async_user_rx_buf,
		       xh->async_rx_temp + xh->async_tx_size,
		       xh->async_rx_size);
	}
	free(xh->async_tx_temp);
	free(xh->async_rx_temp);
	xh->async_tx_temp = NULL;
	xh->async_rx_temp = NULL;
	xh->async_user_rx_buf = NULL;
	xh->async_tx_size = 0;
	xh->async_rx_size = 0;
	if (xh->async_device != NULL) {
		cs_ret = gpio_cs_deassert(xh->async_device);
		xh->async_device = NULL;
	}
	xh->async_in_progress = false;
	return cs_ret;
}

static void spi_status_handler(void *callback_ref, u32 status_event,
                               unsigned int byte_count)
{
	struct capi_spi_xilinx_handle *xh = (struct capi_spi_xilinx_handle *)
	                                    callback_ref;
	(void)byte_count;

	if (xh == NULL)
		return;

	if (status_event == XST_SPI_RECEIVE_NOT_EMPTY ||
	    status_event == XST_SPI_SLAVE_MODE)
		return;

	if (status_event == XST_SPI_TRANSFER_DONE) {
		int ret = pl_async_cleanup(xh, true);
		if (xh->callback != NULL) {
			enum capi_async_event event = (ret == 0) ?
			                              CAPI_SPI_EVENT_XFR_DONE :
			                              CAPI_SPI_EVENT_ERROR;
			xh->callback(event, xh->callback_arg, ret);
		}
	} else {
		XSpi_IntrGlobalDisable(inst(xh));
		XSpi_Reset(inst(xh));
		(void)pl_async_cleanup(xh, false);
		if (XSpi_Start(inst(xh)) == XST_SUCCESS && !xh->use_irq)
			XSpi_IntrGlobalDisable(inst(xh));
		enum capi_async_event evt = CAPI_SPI_EVENT_ERROR;
		if (status_event == XST_SPI_TRANSMIT_UNDERRUN)
			evt = CAPI_SPI_EVENT_TX_FIFO_UNDERFLOW;
		else if (status_event == XST_SPI_RECEIVE_OVERRUN)
			evt = CAPI_SPI_EVENT_RX_FIFO_OVERFLOW;
		else if (status_event == XST_SPI_MODE_FAULT ||
		         status_event == XST_SPI_SLAVE_MODE_FAULT)
			evt = CAPI_SPI_EVENT_TX_COLLISION;

		if (xh->callback != NULL)
			xh->callback(evt, xh->callback_arg, status_event);
	}
}

static int gpio_cs_assert(const struct capi_spi_device *device)
{
	for (uint8_t i = 0; i < device->cs_gpio_num; i++) {
		int ret = capi_gpio_pin_set_value(&device->cs_gpio[i], CAPI_GPIO_HIGH);
		if (ret != 0) {
			while (i > 0) {
				i--;
				(void)capi_gpio_pin_set_value(&device->cs_gpio[i],
				                              CAPI_GPIO_LOW);
			}
			return ret;
		}
	}
	return 0;
}

static int gpio_cs_deassert(const struct capi_spi_device *device)
{
	for (uint8_t i = 0; i < device->cs_gpio_num; i++) {
		int ret = capi_gpio_pin_set_value(&device->cs_gpio[i], CAPI_GPIO_LOW);
		if (ret != 0)
			return ret;
	}
	return 0;
}

static u32 pl_build_options(struct capi_spi_device *device, bool loopback)
{
	u32 options = XSP_MASTER_OPTION | XSP_MANUAL_SSELECT_OPTION;
	if (device->mode & CAPI_SPI_CPHA)
		options |= XSP_CLK_PHASE_1_OPTION;
	if (device->mode & CAPI_SPI_CPOL)
		options |= XSP_CLK_ACTIVE_LOW_OPTION;
	if (loopback)
		options |= XSP_LOOPBACK_OPTION;
	return options;
}

static int pl_validate_transfer(const struct capi_spi_device *device,
                                const struct capi_spi_transfer *transfer,
                                bool async, bool read_command)
{
	if ((device->mode & ~(CAPI_SPI_CPHA | CAPI_SPI_CPOL)) != 0)
		return -EINVAL;
	if (device->flow_ctl_param.mode != CAPI_SPI_FLOW_CTL_DISABLE)
		return -ENOTSUP;
	if ((device->cs_gpio == NULL) != (device->cs_gpio_num == 0))
		return -EINVAL;
	if (device->cs_gpio_num > 0 && device->native_cs != 0)
		return -EINVAL;
	if (device->non_continuous_mode) {
		if (async || read_command || device->cs_gpio_num == 0)
			return -ENOTSUP;
	}
	if (device->cs_gpio_num == 0) {
		uint16_t mask = device->native_cs ? device->native_cs : 1U;
		if ((mask & (mask - 1U)) != 0U)
			return -EINVAL;
	}
	if (transfer->timeout > 0 || transfer->xfer_delay_clk_cycles != 0)
		return -ENOTSUP;
	return 0;
}

static int pl_select_device(struct capi_spi_xilinx_handle *xh,
                            const struct capi_spi_device *device)
{
	u32 mask = 0;
	if (device->cs_gpio_num == 0) {
		mask = device->native_cs ? device->native_cs : 1U;
		if ((mask & ~inst(xh)->SlaveSelectMask) != 0U ||
		    (mask & (mask - 1U)) != 0U)
			return -EINVAL;
	}
	return (XSpi_SetSlaveSelect(inst(xh), mask) == XST_SUCCESS) ? 0 : -EIO;
}

static int pl_drive_native_cs(struct capi_spi_xilinx_handle *xh,
                              uint16_t native_cs, bool assert)
{
	u32 selected = native_cs ? native_cs : 1U;
	if ((selected & ~inst(xh)->SlaveSelectMask) != 0U ||
	    (selected & (selected - 1U)) != 0U)
		return -EINVAL;
	u32 mask = assert ? selected : 0U;
	if (XSpi_SetSlaveSelect(inst(xh), mask) != XST_SUCCESS)
		return -EIO;
	XSpi_SetSlaveSelectReg(inst(xh), inst(xh)->SlaveSelectReg);
	return 0;
}

/* XSpi_SetOptions clears the raw CR — reapply LSB bit after every SetOptions call. */
static void pl_apply_lsb(XSpi *spi, bool lsb_first)
{
	u32 ctrl = XSpi_GetControlReg(spi);
	if (lsb_first)
		ctrl |= XSP_CR_LSB_MSB_FIRST_MASK;
	else
		ctrl &= ~XSP_CR_LSB_MSB_FIRST_MASK;
	XSpi_SetControlReg(spi, ctrl);
}

/**
 * @brief Initialize the CAPI backend instance.
 * @note PL: XSpi_Initialize()/XSpi_Start().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_spi_pl_init(struct capi_spi_controller_handle **handle,
                            const struct capi_spi_config *config)
{
	if (handle == NULL || config == NULL)
		return -EINVAL;
	if (config->three_pin_mode || config->dma_handle != NULL ||
	    config->clk_freq_hz != 0)
		return -ENOTSUP;

	bool alloc = (*handle == NULL);
	if (alloc) {
		*handle = malloc(sizeof(struct capi_spi_xilinx_handle));
		if (*handle == NULL)
			return -ENOMEM;
	}

	struct capi_spi_xilinx_handle *xh = (struct capi_spi_xilinx_handle *)*handle;
	memset(xh, 0, sizeof(*xh));
	xh->handle.init_allocated = alloc;
	xh->handle.ops = config->ops ? config->ops : &capi_spi_xilinx_pl_ops;
	xh->loopback = config->loopback;

	XSpi *spi = malloc(sizeof(XSpi));
	if (!spi) {
		if (alloc) {
			free(xh);
			*handle = NULL;
		}
		return -ENOMEM;
	}
	memset(spi, 0, sizeof(XSpi));

	if (XSpi_Initialize(spi, (UINTPTR)config->identifier) != XST_SUCCESS) {
		free(spi);
		if (alloc) {
			free(xh);
			*handle = NULL;
		}
		return -EIO;
	}

	XSpi_Reset(spi);
	xh->instance = spi;

	u32 options = XSP_MASTER_OPTION | XSP_MANUAL_SSELECT_OPTION;
	if (config->loopback)
		options |= XSP_LOOPBACK_OPTION;
	if (XSpi_SetOptions(spi, options) != XST_SUCCESS) {
		free(spi);
		xh->instance = NULL;
		if (alloc) {
			free(xh);
			*handle = NULL;
		}
		return -EIO;
	}

	XSpi_SetStatusHandler(spi, xh, spi_status_handler);
	if (XSpi_Start(spi) != XST_SUCCESS) {
		XSpi_IntrGlobalDisable(spi);
		XSpi_Reset(spi);
		XSpi_WriteReg(spi->BaseAddr, XSP_DGIER_OFFSET, 0);
		free(spi);
		xh->instance = NULL;
		if (alloc) {
			free(xh);
			*handle = NULL;
		}
		return -EIO;
	}
	XSpi_IntrGlobalDisable(
	        spi); /* start in polled mode; enabled when IRQ connects */

	const struct capi_spi_xilinx_config *xcfg =
	        (const struct capi_spi_xilinx_config *)config->extra;
	if (xcfg != NULL && xcfg->use_irq) {
		uint32_t irq_id = xcfg->irq_id;
		int ret = capi_irq_connect(irq_id, capi_spi_pl_isr, xh);
		if (ret == 0) {
			capi_irq_set_level_edge_trigger(irq_id, CAPI_IRQ_LEVEL_HIGH);
			ret = capi_irq_enable(irq_id);
			if (ret == 0) {
				xh->irq_id = irq_id;
				xh->use_irq = true;
				XSpi_IntrGlobalEnable(spi);
			}
		}
	}

	*handle = (struct capi_spi_controller_handle *)xh;
	return 0;
}

/**
 * @brief Deinitialize the CAPI backend instance.
 * @note PL: XSpi_Reset().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_spi_pl_deinit(struct capi_spi_controller_handle *handle)
{
	if (handle == NULL)
		return -EINVAL;

	struct capi_spi_xilinx_handle *xh = (struct capi_spi_xilinx_handle *)handle;

	if (xh->use_irq) {
		capi_irq_disable(xh->irq_id);
		xh->use_irq = false;
	}

	if (xh->instance != NULL) {
		XSpi_IntrGlobalDisable(inst(xh));
		XSpi_Reset(inst(xh));
		(void)pl_async_cleanup(xh, false);
		/* DGIER survives SRR soft reset; clear it explicitly. */
		XSpi_WriteReg(inst(xh)->BaseAddr, XSP_DGIER_OFFSET, 0);
		free(xh->instance);
		xh->instance = NULL;
	} else {
		(void)pl_async_cleanup(xh, false);
	}

	if (xh->handle.init_allocated)
		free(xh);

	return 0;
}

/**
 * @brief Run a synchronous SPI transceive operation.
 * @note PL: XSpi_SetSlaveSelect()/XSpi_Transfer().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_spi_pl_transceive(struct capi_spi_device *device,
                                  struct capi_spi_transfer *transfer)
{
	if (device == NULL || device->controller == NULL || transfer == NULL)
		return -EINVAL;
	int ret = pl_validate_transfer(device, transfer, false, false);
	if (ret != 0)
		return ret;

	struct capi_spi_xilinx_handle *xh = (struct capi_spi_xilinx_handle *)
	                                    device->controller;
	uint16_t len =
	        (transfer->tx_size > transfer->rx_size) ? transfer->tx_size : transfer->rx_size;
	if (len == 0)
		return -EINVAL;

	/* Always copy TX because XSpi takes a non-const buffer. */
	bool need_rx_temp = (transfer->rx_buf == NULL) || (transfer->rx_size < len);

	uint8_t *tx = malloc(len);
	if (!tx)
		return -ENOMEM;
	memset(tx, 0, len);
	if (transfer->tx_buf && transfer->tx_size > 0)
		memcpy(tx, transfer->tx_buf, transfer->tx_size);

	uint8_t *rx = transfer->rx_buf;
	if (need_rx_temp) {
		rx = malloc(len);
		if (!rx) {
			free(tx);
			return -ENOMEM;
		}
	}

	if (XSpi_SetOptions(inst(xh),
	                    pl_build_options(device, xh->loopback)) != XST_SUCCESS) {
		free(tx);
		if (need_rx_temp)
			free(rx);
		return -EIO;
	}
	pl_apply_lsb(inst(xh), device->lsb_first);

	bool use_gpio_cs = device->cs_gpio_num > 0;
	ret = pl_select_device(xh, device);
	if (ret != 0) {
		free(tx);
		if (need_rx_temp)
			free(rx);
		return ret;
	}
	XSpi_IntrGlobalDisable(inst(xh));

	int status;
	int gpio_error = 0;
	if (use_gpio_cs && device->non_continuous_mode) {
		status = XST_SUCCESS;
		for (uint16_t i = 0; i < len && status == XST_SUCCESS; i++) {
			ret = gpio_cs_assert(device);
			if (ret != 0) {
				gpio_error = ret;
				status = XST_FAILURE;
				break;
			}
			status = XSpi_Transfer(inst(xh), tx + i, rx ? rx + i : NULL, 1);
			ret = gpio_cs_deassert(device);
			if (ret != 0 && status == XST_SUCCESS) {
				gpio_error = ret;
				status = XST_FAILURE;
			}
		}
	} else {
		if (use_gpio_cs) {
			ret = gpio_cs_assert(device);
			if (ret != 0) {
				gpio_error = ret;
				status = XST_FAILURE;
			} else {
				status = XSpi_Transfer(inst(xh), tx, rx, len);
			}
			ret = gpio_cs_deassert(device);
			if (ret != 0 && status == XST_SUCCESS) {
				gpio_error = ret;
				status = XST_FAILURE;
			}
		} else {
			status = XSpi_Transfer(inst(xh), tx, rx, len);
		}
	}

	if (xh->use_irq)
		XSpi_IntrGlobalEnable(inst(xh));

	if (need_rx_temp && transfer->rx_buf && transfer->rx_size > 0)
		memcpy(transfer->rx_buf, rx, transfer->rx_size);

	free(tx);
	if (need_rx_temp)
		free(rx);

	return (gpio_error != 0) ? gpio_error :
	       ((status == XST_SUCCESS) ? 0 : -EIO);
}

/**
 * @brief Start an asynchronous SPI transceive operation.
 * @note PL: XSpi_Transfer() in interrupt mode.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_spi_pl_transceive_async(struct capi_spi_device *device,
                                        struct capi_spi_transfer *transfer, int timeout)
{
	if (device == NULL || device->controller == NULL || transfer == NULL)
		return -EINVAL;
	int ret = pl_validate_transfer(device, transfer, true, false);
	if (ret != 0)
		return ret;
	if (timeout != 0)
		return -ENOTSUP;

	struct capi_spi_xilinx_handle *xh = (struct capi_spi_xilinx_handle *)
	                                    device->controller;
	if (!xh->use_irq)
		return -ENOTSUP;
	if (xh->async_in_progress)
		return -EBUSY;

	uint16_t len =
	        (transfer->tx_size > transfer->rx_size) ? transfer->tx_size : transfer->rx_size;
	if (len == 0)
		return -EINVAL;

	xh->async_in_progress = true;

	/* Always copy TX because XSpi takes a non-const buffer. */
	bool need_rx_temp = (transfer->rx_buf == NULL) || (transfer->rx_size < len);

	uint8_t *tx = malloc(len);
	if (!tx) {
		xh->async_in_progress = false;
		return -ENOMEM;
	}
	memset(tx, 0, len);
	if (transfer->tx_buf && transfer->tx_size > 0)
		memcpy(tx, transfer->tx_buf, transfer->tx_size);
	xh->async_tx_temp = tx;

	uint8_t *rx = transfer->rx_buf;
	if (need_rx_temp) {
		rx = malloc(len);
		if (!rx) {
			free(tx);
			xh->async_tx_temp = NULL;
			xh->async_in_progress = false;
			return -ENOMEM;
		}
		xh->async_rx_temp = rx;
		xh->async_user_rx_buf = transfer->rx_buf;
		xh->async_rx_size = transfer->rx_size;
	} else {
		xh->async_rx_temp = NULL;
		xh->async_user_rx_buf = NULL;
	}
	xh->async_tx_size = 0;
	xh->async_rx_size = transfer->rx_size;

	if (XSpi_SetOptions(inst(xh),
	                    pl_build_options(device, xh->loopback)) != XST_SUCCESS) {
		(void)pl_async_cleanup(xh, false);
		return -EIO;
	}
	pl_apply_lsb(inst(xh), device->lsb_first);
	ret = pl_select_device(xh, device);
	if (ret != 0) {
		(void)pl_async_cleanup(xh, false);
		return ret;
	}

	if (device->cs_gpio_num > 0) {
		xh->async_device = device;
		ret = gpio_cs_assert(device);
		if (ret != 0) {
			(void)pl_async_cleanup(xh, false);
			return ret;
		}
	}

	XSpi_IntrGlobalEnable(inst(xh));
	if (XSpi_Transfer(inst(xh), tx, rx, len) != XST_SUCCESS) {
		(void)pl_async_cleanup(xh, false);
		return -EIO;
	}

	return 0;
}

/**
 * @brief Run a synchronous SPI command-read operation.
 * @note PL: Command+RX via XSpi_Transfer().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_spi_pl_read_command(struct capi_spi_device *device,
                                    struct capi_spi_transfer *transfer)
{
	if (device == NULL || device->controller == NULL || transfer == NULL)
		return -EINVAL;
	int ret = pl_validate_transfer(device, transfer, false, true);
	if (ret != 0)
		return ret;

	struct capi_spi_xilinx_handle *xh = (struct capi_spi_xilinx_handle *)
	                                    device->controller;

	uint32_t total = (uint32_t)transfer->tx_size + transfer->rx_size;
	if (total == 0)
		return 0;
	if (total > 0xFFFFU)
		return -EINVAL;

	uint8_t *tx = malloc(total);
	uint8_t *rx = malloc(total);
	if (!tx || !rx) {
		free(tx);
		free(rx);
		return -ENOMEM;
	}

	memset(tx, 0, total);
	if (transfer->tx_buf && transfer->tx_size > 0)
		memcpy(tx, transfer->tx_buf, transfer->tx_size);

	if (XSpi_SetOptions(inst(xh),
	                    pl_build_options(device, xh->loopback)) != XST_SUCCESS) {
		free(tx);
		free(rx);
		return -EIO;
	}
	pl_apply_lsb(inst(xh), device->lsb_first);
	ret = pl_select_device(xh, device);
	if (ret != 0) {
		free(tx);
		free(rx);
		return ret;
	}
	XSpi_IntrGlobalDisable(inst(xh));

	int status;
	int gpio_error = 0;
	if (device->cs_gpio_num > 0) {
		ret = gpio_cs_assert(device);
		if (ret != 0) {
			gpio_error = ret;
			status = XST_FAILURE;
		} else {
			status = XSpi_Transfer(inst(xh), tx, rx, total);
		}
		ret = gpio_cs_deassert(device);
		if (ret != 0 && status == XST_SUCCESS) {
			gpio_error = ret;
			status = XST_FAILURE;
		}
	} else {
		status = XSpi_Transfer(inst(xh), tx, rx, total);
	}

	if (xh->use_irq)
		XSpi_IntrGlobalEnable(inst(xh));

	if (status == XST_SUCCESS && transfer->rx_buf && transfer->rx_size > 0)
		memcpy(transfer->rx_buf, rx + transfer->tx_size, transfer->rx_size);

	free(tx);
	free(rx);
	return (gpio_error != 0) ? gpio_error :
	       ((status == XST_SUCCESS) ? 0 : -EIO);
}

/**
 * @brief Start an asynchronous SPI command-read operation.
 * @note PL: Async command+RX via XSpi_Transfer().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_spi_pl_read_command_async(struct capi_spi_device *device,
                struct capi_spi_transfer *transfer)
{
	if (device == NULL || device->controller == NULL || transfer == NULL)
		return -EINVAL;
	int ret = pl_validate_transfer(device, transfer, true, true);
	if (ret != 0)
		return ret;

	uint32_t total32 = (uint32_t)transfer->tx_size + transfer->rx_size;
	if (total32 > UINT16_MAX)
		return -EINVAL;
	uint16_t total = (uint16_t)total32;
	if (total == 0)
		return 0;

	struct capi_spi_xilinx_handle *xh = (struct capi_spi_xilinx_handle *)
	                                    device->controller;
	if (!xh->use_irq)
		return -ENOTSUP;
	if (xh->async_in_progress)
		return -EBUSY;

	uint8_t *tx = malloc(total);
	uint8_t *rx = malloc(total);
	if (!tx || !rx) {
		free(tx);
		free(rx);
		return -ENOMEM;
	}

	memset(tx, 0, total);
	memset(rx, 0, total);
	if (transfer->tx_buf && transfer->tx_size > 0)
		memcpy(tx, transfer->tx_buf, transfer->tx_size);

	xh->async_tx_temp = tx;
	xh->async_rx_temp = rx;
	xh->async_user_rx_buf = transfer->rx_buf;
	xh->async_tx_size = transfer->tx_size;
	xh->async_rx_size = transfer->rx_size;
	xh->async_in_progress = true;

	if (XSpi_SetOptions(inst(xh),
	                    pl_build_options(device, xh->loopback)) != XST_SUCCESS) {
		(void)pl_async_cleanup(xh, false);
		return -EIO;
	}
	pl_apply_lsb(inst(xh), device->lsb_first);
	ret = pl_select_device(xh, device);
	if (ret != 0) {
		(void)pl_async_cleanup(xh, false);
		return ret;
	}

	if (device->cs_gpio_num > 0) {
		xh->async_device = device;
		ret = gpio_cs_assert(device);
		if (ret != 0) {
			(void)pl_async_cleanup(xh, false);
			return ret;
		}
	}

	XSpi_IntrGlobalEnable(inst(xh));
	if (XSpi_Transfer(inst(xh), tx, rx, total) != XST_SUCCESS) {
		(void)pl_async_cleanup(xh, false);
		return -EIO;
	}

	return 0;
}

/**
 * @brief Abort an in-progress asynchronous operation.
 * @note PL: XSpi_Reset()/XSpi_Start().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_spi_pl_abort_async(struct capi_spi_device *device)
{
	if (device == NULL || device->controller == NULL)
		return -EINVAL;

	struct capi_spi_xilinx_handle *xh = (struct capi_spi_xilinx_handle *)
	                                    device->controller;
	if (!xh->async_in_progress)
		return 0;

	XSpi_IntrGlobalDisable(inst(xh));
	XSpi_Reset(inst(xh));
	(void)pl_async_cleanup(xh, false);
	int ret = XSpi_Start(inst(xh));
	if (ret == XST_SUCCESS && !xh->use_irq)
		XSpi_IntrGlobalDisable(inst(xh));

	if (xh->callback)
		xh->callback(CAPI_SPI_EVENT_ERROR, xh->callback_arg, 0);

	return (ret == XST_SUCCESS) ? 0 : -EIO;
}

/**
 * @brief Register the CAPI asynchronous callback.
 * @note PL: stores the CAPI callback used by the XSpi event path.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_spi_pl_register_callback(struct capi_spi_controller_handle
                *handle,
                capi_spi_callback_t const callback, void *callback_arg)
{
	if (handle == NULL)
		return -EINVAL;
	struct capi_spi_xilinx_handle *xh = (struct capi_spi_xilinx_handle *)handle;
	xh->callback = callback;
	xh->callback_arg = callback_arg;
	return 0;
}

/**
 * @brief Manually assert or deassert SPI chip select.
 * @note PL: Drives GPIO CS logically or the native SSR directly.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_spi_pl_set_cs(struct capi_spi_device *device,
                              enum capi_spi_cs_control cs)
{
	if (device == NULL || device->controller == NULL)
		return -EINVAL;
	struct capi_spi_xilinx_handle *xh = (struct capi_spi_xilinx_handle *)
	                                    device->controller;
	if (xh->async_in_progress || inst(xh)->IsBusy)
		return -EBUSY;

	if ((device->cs_gpio == NULL) != (device->cs_gpio_num == 0))
		return -EINVAL;
	if (device->cs_gpio_num > 0 && device->native_cs != 0)
		return -EINVAL;

	if (device->cs_gpio_num > 0) {
		if (cs == CAPI_SPI_CS_MANUAL_ASSERT)
			return gpio_cs_assert(device);
		else if (cs == CAPI_SPI_CS_MANUAL_DEASSERT)
			return gpio_cs_deassert(device);
		else
			return -EINVAL;
	}

	if (cs == CAPI_SPI_CS_MANUAL_ASSERT)
		return pl_drive_native_cs(xh, device->native_cs, true);
	else if (cs == CAPI_SPI_CS_MANUAL_DEASSERT)
		return pl_drive_native_cs(xh, device->native_cs, false);
	return -EINVAL;
}

/**
 * @brief Dispatch the backend interrupt handler.
 * @note PL: XSpi_InterruptHandler().
 *
 */
static void capi_spi_pl_isr(void *handle)
{
	if (handle == NULL)
		return;
	struct capi_spi_xilinx_handle *xh = (struct capi_spi_xilinx_handle *)handle;
	XSpi_InterruptHandler(inst(xh));
}
