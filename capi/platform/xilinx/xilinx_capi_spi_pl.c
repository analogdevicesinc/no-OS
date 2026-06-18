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

#include <capi_alloc.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <capi_spi.h>
#include <xilinx_capi_spi_priv.h>
#include <capi_gpio.h>
#include "capi_irq.h"
#include "xinterrupt_wrap.h"

#ifdef XPAR_XSPI_NUM_INSTANCES

/* native_cs == 0 selects CS0. */
#define XSPI_CAPI_DEFAULT_NATIVE_CS_MASK	1U
#define XSPI_CAPI_DESELECT_ALL_NATIVE		0U
#define XSPI_CAPI_SINGLE_BYTE_TRANSFER		1U
#define XSPI_CAPI_MAX_TRANSFER_SIZE		UINT16_MAX

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
static void spi_finish_irq(struct capi_spi_xilinx_handle *xh);

const struct capi_spi_ops capi_spi_xilinx_pl_ops = {
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

static void xilinx_spi_free_allocated_handle(
	struct capi_spi_controller_handle **handle)
{
	if (handle == NULL || *handle == NULL)
		return;

	capi_free((*handle)->priv);
	capi_free(*handle);
	*handle = NULL;
}

static void xilinx_spi_clear_app_handle(
	struct capi_spi_controller_handle *handle)
{
	if (handle != NULL)
		handle->ops = NULL;
}

static int xspi_status_to_errno(int status)
{
	if (status == XST_SUCCESS || status == XST_DEVICE_IS_STARTED)
		return 0;

	if (status == XST_DEVICE_BUSY)
		return -EBUSY;

	if (status == XST_DEVICE_NOT_FOUND)
		return -ENODEV;

	if (status == XST_DEVICE_IS_STOPPED)
		return -EIO;

	if (status == XST_SPI_NO_SLAVE || status == XST_SPI_TOO_MANY_SLAVES)
		return -EINVAL;

	if (status == XST_SPI_NOT_MASTER || status == XST_SPI_SLAVE_ONLY)
		return -ENOTSUP;

	return -EIO;
}

static int gpio_cs_assert(const struct capi_spi_device *device);
static int gpio_cs_deassert(const struct capi_spi_device *device);

static int pl_async_cleanup(struct capi_spi_xilinx_handle *xh, bool copy_rx)
{
	int cs_ret = 0;

	if (copy_rx && xh->async.rx_buffer != NULL &&
	    xh->async.transfer.rx_buf != NULL && xh->async.transfer.rx_size > 0) {
		memcpy(xh->async.transfer.rx_buf,
		       xh->async.rx_buffer + xh->async.rx_offset,
		       xh->async.transfer.rx_size);
	}
	capi_free(xh->async.tx_buffer);
	capi_free(xh->async.rx_buffer);
	if (xh->async.gpio_cs_device != NULL) {
		cs_ret = gpio_cs_deassert(xh->async.gpio_cs_device);
		xh->async.gpio_cs_device = NULL;
	} else if (xh->instance != NULL) {
		(void)XSpi_SetSlaveSelect(inst(xh), XSPI_CAPI_DESELECT_ALL_NATIVE);
		XSpi_SetSlaveSelectReg(inst(xh), inst(xh)->SlaveSelectReg);
	}
	memset(&xh->async, 0, sizeof(xh->async));
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

	if (!xh->async_in_progress)
		return;

	/* An error supersedes a completion reported earlier by the same ISR. */
	if (!xh->terminal_pending ||
	    xh->pending_status == XST_SPI_TRANSFER_DONE) {
		xh->pending_status = status_event;
		xh->terminal_pending = true;
	}
}

static void spi_finish_irq(struct capi_spi_xilinx_handle *xh)
{
	if (!xh->terminal_pending)
		return;

	u32 status_event = xh->pending_status;
	xh->pending_status = 0;
	xh->terminal_pending = false;

	if (status_event != XST_SPI_TRANSFER_DONE) {
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
		return;
	}

	int ret = pl_async_cleanup(xh, true);
	if (xh->callback != NULL) {
		enum capi_async_event event = (ret == 0) ?
					      CAPI_SPI_EVENT_XFR_DONE :
					      CAPI_SPI_EVENT_ERROR;
		xh->callback(event, xh->callback_arg, ret);
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
	int first_error = 0;

	for (uint8_t i = 0; i < device->cs_gpio_num; i++) {
		int ret = capi_gpio_pin_set_value(&device->cs_gpio[i], CAPI_GPIO_LOW);
		if (ret != 0 && first_error == 0)
			first_error = ret;
	}
	return first_error;
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

static int pl_validate_data_width(struct capi_spi_xilinx_handle *xh)
{
	/* CAPI buffers are byte-oriented. */
	return (inst(xh)->DataWidth == XSP_DATAWIDTH_BYTE) ? 0 : -ENOTSUP;
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
	/* AXI SPI has no runtime SCLK divider. */
	if (device->max_speed_hz != 0)
		return -ENOTSUP;
	if (device->cs_gpio_num == 0) {
		uint16_t mask = device->native_cs;
		if (mask == 0U)
			mask = XSPI_CAPI_DEFAULT_NATIVE_CS_MASK;
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
	u32 mask = XSPI_CAPI_DESELECT_ALL_NATIVE;
	if (device->cs_gpio_num == 0) {
		mask = device->native_cs;
		if (mask == 0U)
			mask = XSPI_CAPI_DEFAULT_NATIVE_CS_MASK;
		if ((mask & ~inst(xh)->SlaveSelectMask) != 0U ||
		    (mask & (mask - 1U)) != 0U)
			return -EINVAL;
	}
	return xspi_status_to_errno(XSpi_SetSlaveSelect(inst(xh), mask));
}

static int pl_drive_native_cs(struct capi_spi_xilinx_handle *xh,
			      uint16_t native_cs, bool assert)
{
	u32 selected = native_cs;
	if (selected == 0U)
		selected = XSPI_CAPI_DEFAULT_NATIVE_CS_MASK;
	if ((selected & ~inst(xh)->SlaveSelectMask) != 0U ||
	    (selected & (selected - 1U)) != 0U)
		return -EINVAL;

	u32 mask = assert ? selected : XSPI_CAPI_DESELECT_ALL_NATIVE;
	int ret = xspi_status_to_errno(XSpi_SetSlaveSelect(inst(xh), mask));
	if (ret != 0)
		return ret;
	/* XSpi_SetSlaveSelect only updates the cached SSR value. */
	XSpi_SetSlaveSelectReg(inst(xh), inst(xh)->SlaveSelectReg);
	return 0;
}

/* XSpi_SetOptions clears the raw CR; reapply LSB bit after every SetOptions call. */
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
	if (*handle != NULL &&
	    ((*handle)->ops != NULL ||
	     ((*handle)->priv != NULL &&
	      ((struct capi_spi_xilinx_handle *)(*handle)->priv)->instance != NULL)))
		return -EBUSY;
	if (config->three_pin_mode || config->dma_handle != NULL ||
	    config->clk_freq_hz != 0)
		return -ENOTSUP;

	bool alloc = (*handle == NULL);
	struct capi_spi_controller_handle *h = *handle;
	struct capi_spi_xilinx_handle *xh;
	XSpi *spi = NULL;
	int ret;

	if (alloc) {
		h = capi_malloc(sizeof(*h));
		if (h == NULL)
			return -ENOMEM;
		memset(h, 0, sizeof(*h));

		xh = capi_malloc(sizeof(*xh));
		if (xh == NULL) {
			capi_free(h);
			return -ENOMEM;
		}
		h->priv = xh;
		*handle = h;
	} else {
		xh = h->priv;
		if (xh == NULL)
			return -EINVAL;
	}

	memset(xh, 0, sizeof(*xh));
	h->init_allocated = alloc;
	h->ops = config->ops ? config->ops : &capi_spi_xilinx_pl_ops;
	h->priv = xh;
	xh->loopback = config->loopback;

	spi = capi_malloc(sizeof(XSpi));
	if (!spi) {
		ret = -ENOMEM;
		goto err_handle;
	}
	memset(spi, 0, sizeof(XSpi));

	ret = xspi_status_to_errno(XSpi_Initialize(spi,
				   (UINTPTR)config->identifier));
	if (ret != 0)
		goto err_inst;

	XSpi_Reset(spi);
	xh->instance = spi;

	ret = pl_validate_data_width(xh);
	if (ret != 0)
		goto err_inst;

	u32 options = XSP_MASTER_OPTION | XSP_MANUAL_SSELECT_OPTION;
	if (config->loopback)
		options |= XSP_LOOPBACK_OPTION;
	ret = xspi_status_to_errno(XSpi_SetOptions(spi, options));
	if (ret != 0)
		goto err_inst;

	XSpi_SetStatusHandler(spi, xh, spi_status_handler);
	ret = xspi_status_to_errno(XSpi_Start(spi));
	if (ret != 0)
		goto err_start;
	/* XSpi_Start enables DGIER; initialization starts in polled mode. */
	XSpi_IntrGlobalDisable(spi);

	const struct capi_spi_xilinx_config *xcfg =
		(const struct capi_spi_xilinx_config *)config->extra;
	if (xcfg != NULL && xcfg->use_irq) {
		uint32_t irq_id = xcfg->irq_id;
		ret = capi_irq_connect(irq_id, capi_spi_pl_isr, h);
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

	return 0;

err_start:
	XSpi_IntrGlobalDisable(spi);
	XSpi_Reset(spi);
	/* SRR reset does not clear DGIER. */
	XSpi_WriteReg(spi->BaseAddr, XSP_DGIER_OFFSET, 0);
err_inst:
	capi_free(spi);
	xh->instance = NULL;
err_handle:
	if (alloc)
		xilinx_spi_free_allocated_handle(handle);
	else
		xilinx_spi_clear_app_handle(h);
	return ret;
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

	struct capi_spi_xilinx_handle *xh = handle->priv;
	if (xh == NULL)
		return -EINVAL;

	if (xh->use_irq) {
		capi_irq_disable(xh->irq_id);
		xh->use_irq = false;
	}

	if (xh->instance != NULL) {
		XSpi_IntrGlobalDisable(inst(xh));
		XSpi_Reset(inst(xh));
		(void)pl_async_cleanup(xh, false);
		XSpi_WriteReg(inst(xh)->BaseAddr, XSP_DGIER_OFFSET, 0);
		capi_free(xh->instance);
		xh->instance = NULL;
	} else {
		(void)pl_async_cleanup(xh, false);
	}

	if (handle->init_allocated) {
		capi_free(xh);
		capi_free(handle);
	} else {
		handle->ops = NULL;
	}

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

	struct capi_spi_xilinx_handle *xh = device->controller->priv;
	if (xh->async_in_progress || inst(xh)->IsBusy)
		return -EBUSY;

	ret = pl_validate_data_width(xh);
	if (ret != 0)
		return ret;

	uint16_t len =
		(transfer->tx_size > transfer->rx_size) ? transfer->tx_size : transfer->rx_size;
	if (len == 0)
		return -EINVAL;

	bool need_rx_temp = (transfer->rx_buf == NULL) || (transfer->rx_size < len);

	uint8_t *tx = capi_malloc(len);
	if (!tx)
		return -ENOMEM;
	memset(tx, 0, len);
	if (transfer->tx_buf && transfer->tx_size > 0)
		memcpy(tx, transfer->tx_buf, transfer->tx_size);

	uint8_t *rx = transfer->rx_buf;
	if (need_rx_temp) {
		rx = capi_malloc(len);
		if (!rx) {
			capi_free(tx);
			return -ENOMEM;
		}
	}

	ret = xspi_status_to_errno(XSpi_SetOptions(inst(xh),
				   pl_build_options(device, xh->loopback)));
	if (ret != 0) {
		capi_free(tx);
		if (need_rx_temp)
			capi_free(rx);
		return ret;
	}

	pl_apply_lsb(inst(xh), device->lsb_first);

	bool use_gpio_cs = device->cs_gpio_num > 0;
	ret = pl_select_device(xh, device);
	if (ret != 0) {
		capi_free(tx);
		if (need_rx_temp)
			capi_free(rx);
		return ret;
	}

	/* DGIER selects the interrupt-driven XSpi_Transfer path. */
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
			status = XSpi_Transfer(inst(xh), tx + i, rx ? rx + i : NULL,
					       XSPI_CAPI_SINGLE_BYTE_TRANSFER);
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

	/* MANUAL_SSELECT leaves native CS asserted. */
	if (!use_gpio_cs) {
		(void)XSpi_SetSlaveSelect(inst(xh), XSPI_CAPI_DESELECT_ALL_NATIVE);
		XSpi_SetSlaveSelectReg(inst(xh), inst(xh)->SlaveSelectReg);
	}

	if (xh->use_irq)
		XSpi_IntrGlobalEnable(inst(xh));

	/* Failed transfers may leave the temporary RX buffer incomplete. */
	if (status == XST_SUCCESS && need_rx_temp && transfer->rx_buf &&
	    transfer->rx_size > 0)
		memcpy(transfer->rx_buf, rx, transfer->rx_size);

	capi_free(tx);
	if (need_rx_temp)
		capi_free(rx);

	return (gpio_error != 0) ? gpio_error :
	       xspi_status_to_errno(status);
}

/**
 * @brief Start an asynchronous SPI transceive operation.
 * @note PL: XSpi_Transfer() in interrupt mode.
 *
 * Non-blocking: this arms the hardware and returns immediately. Completion is
 * reported later from the interrupt path (spi_status_handler ->
 * pl_async_cleanup), which frees the temp buffers, copies RX back out, and
 * fires the CAPI callback with XFR_DONE or an error event.
 *
 * Flow:
 *   1. Reject if no IRQ (async needs it) or a transfer is already in flight.
 *   2. Compute the framed length (max of TX/RX; NULL TX means zeroes) and mark
 *      async_in_progress so every later error path unwinds via pl_async_cleanup.
 *   3. Allocate the TX copy (XSpi needs a non-const buffer) and, when the
 *      caller's RX is absent/too small, a temp RX; both must outlive this call
 *      and are freed in the ISR completion. rx_offset = 0 (no command prefix).
 *   4. Apply CAPI mode options (then reapply bit order, which SetOptions clears)
 *      and select the target (native CS, or assert GPIO CS held until done).
 *   5. XSpi_IntrGlobalEnable then XSpi_Transfer, which returns after arming
 *      interrupts; return 0.
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

	struct capi_spi_xilinx_handle *xh = device->controller->priv;
	if (!xh->use_irq)
		return -ENOTSUP;
	if (xh->async_in_progress || inst(xh)->IsBusy)
		return -EBUSY;

	ret = pl_validate_data_width(xh);
	if (ret != 0)
		return ret;

	uint16_t len =
		(transfer->tx_size > transfer->rx_size) ? transfer->tx_size : transfer->rx_size;
	if (len == 0)
		return -EINVAL;

	xh->async_in_progress = true;

	bool need_rx_temp = (transfer->rx_buf == NULL) || (transfer->rx_size < len);

	uint8_t *tx = capi_malloc(len);
	if (!tx) {
		xh->async_in_progress = false;
		return -ENOMEM;
	}
	memset(tx, 0, len);
	if (transfer->tx_buf && transfer->tx_size > 0)
		memcpy(tx, transfer->tx_buf, transfer->tx_size);
	xh->async.tx_buffer = tx;

	uint8_t *rx = transfer->rx_buf;
	if (need_rx_temp) {
		rx = capi_malloc(len);
		if (!rx) {
			capi_free(tx);
			xh->async.tx_buffer = NULL;
			xh->async_in_progress = false;
			return -ENOMEM;
		}
		xh->async.rx_buffer = rx;
		xh->async.transfer.rx_buf = transfer->rx_buf;
		xh->async.transfer.rx_size = transfer->rx_size;
	} else {
		xh->async.rx_buffer = NULL;
		xh->async.transfer.rx_buf = NULL;
	}
	xh->async.rx_offset = 0;
	xh->async.transfer.rx_size = transfer->rx_size;

	ret = xspi_status_to_errno(XSpi_SetOptions(inst(xh),
				   pl_build_options(device, xh->loopback)));
	if (ret != 0) {
		(void)pl_async_cleanup(xh, false);
		return ret;
	}

	pl_apply_lsb(inst(xh), device->lsb_first);

	ret = pl_select_device(xh, device);
	if (ret != 0) {
		(void)pl_async_cleanup(xh, false);
		return ret;
	}

	if (device->cs_gpio_num > 0) {
		xh->async.gpio_cs_device = device;
		ret = gpio_cs_assert(device);
		if (ret != 0) {
			(void)pl_async_cleanup(xh, false);
			return ret;
		}
	}

	XSpi_IntrGlobalEnable(inst(xh));
	ret = xspi_status_to_errno(XSpi_Transfer(inst(xh), tx, rx, len));
	if (ret != 0) {
		(void)pl_async_cleanup(xh, false);
		return ret;
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

	struct capi_spi_xilinx_handle *xh = device->controller->priv;
	if (xh->async_in_progress || inst(xh)->IsBusy)
		return -EBUSY;

	ret = pl_validate_data_width(xh);
	if (ret != 0)
		return ret;

	uint32_t total = (uint32_t)transfer->tx_size + transfer->rx_size;
	if (total == 0)
		return 0;	/* Zero-length read command is a successful no-op. */
	if (total > XSPI_CAPI_MAX_TRANSFER_SIZE)
		return -EINVAL;

	uint8_t *tx = capi_malloc(total);
	uint8_t *rx = capi_malloc(total);
	if (!tx || !rx) {
		capi_free(tx);
		capi_free(rx);
		return -ENOMEM;
	}

	memset(tx, 0, total);
	if (transfer->tx_buf && transfer->tx_size > 0)
		memcpy(tx, transfer->tx_buf, transfer->tx_size);

	ret = xspi_status_to_errno(XSpi_SetOptions(inst(xh),
				   pl_build_options(device, xh->loopback)));
	if (ret != 0) {
		capi_free(tx);
		capi_free(rx);
		return ret;
	}

	pl_apply_lsb(inst(xh), device->lsb_first);

	ret = pl_select_device(xh, device);
	if (ret != 0) {
		capi_free(tx);
		capi_free(rx);
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

	if (device->cs_gpio_num == 0) {
		(void)XSpi_SetSlaveSelect(inst(xh), XSPI_CAPI_DESELECT_ALL_NATIVE);
		XSpi_SetSlaveSelectReg(inst(xh), inst(xh)->SlaveSelectReg);
	}

	if (xh->use_irq)
		XSpi_IntrGlobalEnable(inst(xh));

	if (status == XST_SUCCESS && transfer->rx_buf && transfer->rx_size > 0)
		memcpy(transfer->rx_buf, rx + transfer->tx_size, transfer->rx_size);

	capi_free(tx);
	capi_free(rx);
	return (gpio_error != 0) ? gpio_error :
	       xspi_status_to_errno(status);
}

/**
 * @brief Start an asynchronous SPI command-read operation.
 * @note PL: Async command+RX via XSpi_Transfer().
 *
 * Non-blocking, same completion path as capi_spi_pl_transceive_async. The only
 * difference is framing: command/address TX bytes and RX data share one CS
 * frame, so it builds a single full-duplex buffer of length tx_size + rx_size
 * (dummy TX clocks the read phase).
 *
 * SPI is full-duplex: the controller shifts out one TX byte and latches one RX
 * byte per clock, so the RX buffer fills for the whole frame, including while
 * the command bytes are being sent. Those first tx_size RX bytes are just the
 * bus echo during the command and must be discarded. The temporary buffers and
 * the offset that does this are held in xh->async (see the async_transfer
 * struct) so the ISR can finish the copy after this call returns:
 *
 *   tx (heap, tx_size+rx_size):  [ cmd bytes | 0 0 0 ... ]  (dummy TX clocks read)
 *   rx (heap, tx_size+rx_size):  [ echo bytes | real data ]
 *                                  \_ discard _/ \__ copied out __/
 *                                  rx_offset = tx_size
 *
 * On completion pl_async_cleanup copies rx[rx_offset .. rx_offset+rx_size) into
 * the caller's transfer->rx_buf, skipping the command-echo bytes. Both heap
 * buffers must outlive this call (the transfer is interrupt-driven) and are
 * freed by that same cleanup.
 *
 * Flow:
 *   1. Reject if no IRQ or a transfer is already in flight.
 *   2. Allocate the combined TX/RX buffers and copy the command bytes into the
 *      TX prefix (rest of TX stays zero as dummy read clocks).
 *   3. Save async state (rx_offset = tx_size) and mark async_in_progress so
 *      every later error path unwinds via pl_async_cleanup.
 *   4. Apply CAPI mode options (then reapply bit order, which SetOptions clears)
 *      and select the target (native CS, or assert GPIO CS held until done).
 *   5. XSpi_IntrGlobalEnable then XSpi_Transfer, which returns after arming
 *      interrupts; return 0.
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
	if (total32 > XSPI_CAPI_MAX_TRANSFER_SIZE)
		return -EINVAL;
	uint16_t total = (uint16_t)total32;
	if (total == 0)
		return -EINVAL;

	struct capi_spi_xilinx_handle *xh = device->controller->priv;
	if (!xh->use_irq)
		return -ENOTSUP;
	if (xh->async_in_progress || inst(xh)->IsBusy)
		return -EBUSY;

	ret = pl_validate_data_width(xh);
	if (ret != 0)
		return ret;

	uint8_t *tx = capi_malloc(total);
	uint8_t *rx = capi_malloc(total);
	if (!tx || !rx) {
		capi_free(tx);
		capi_free(rx);
		return -ENOMEM;
	}

	memset(tx, 0, total);
	memset(rx, 0, total);
	if (transfer->tx_buf && transfer->tx_size > 0)
		memcpy(tx, transfer->tx_buf, transfer->tx_size);

	xh->async.tx_buffer = tx;
	xh->async.rx_buffer = rx;
	xh->async.transfer.rx_buf = transfer->rx_buf;
	xh->async.rx_offset = transfer->tx_size;
	xh->async.transfer.rx_size = transfer->rx_size;
	xh->async_in_progress = true;

	ret = xspi_status_to_errno(XSpi_SetOptions(inst(xh),
				   pl_build_options(device, xh->loopback)));
	if (ret != 0) {
		(void)pl_async_cleanup(xh, false);
		return ret;
	}

	pl_apply_lsb(inst(xh), device->lsb_first);

	ret = pl_select_device(xh, device);
	if (ret != 0) {
		(void)pl_async_cleanup(xh, false);
		return ret;
	}

	if (device->cs_gpio_num > 0) {
		xh->async.gpio_cs_device = device;
		ret = gpio_cs_assert(device);
		if (ret != 0) {
			(void)pl_async_cleanup(xh, false);
			return ret;
		}
	}

	XSpi_IntrGlobalEnable(inst(xh));
	ret = xspi_status_to_errno(XSpi_Transfer(inst(xh), tx, rx, total));
	if (ret != 0) {
		(void)pl_async_cleanup(xh, false);
		return ret;
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

	struct capi_spi_xilinx_handle *xh = device->controller->priv;
	if (!xh->async_in_progress)
		return 0;

	XSpi_IntrGlobalDisable(inst(xh));
	XSpi_Reset(inst(xh));
	(void)pl_async_cleanup(xh, false);
	int ret = xspi_status_to_errno(XSpi_Start(inst(xh)));
	if (ret == XST_SUCCESS && !xh->use_irq)
		XSpi_IntrGlobalDisable(inst(xh));

	if (xh->callback)
		xh->callback(CAPI_SPI_EVENT_ERROR, xh->callback_arg, 0);

	return ret;
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
	struct capi_spi_xilinx_handle *xh = handle->priv;
	xh->callback = callback;
	xh->callback_arg = callback_arg;
	return 0;
}

/**
 * @brief Control the SPI chip select line.
 * @note PL: Manual controls drive GPIO CS logically or the native SSR
 *       directly; automatic CS is left to the per-transfer path.
 *
 * Supports the three CAPI CS controls:
 *   CAPI_SPI_CS_AUTO           - the transfer functions already assert CS at
 *                                the start of each frame and release it at the
 *                                end, so automatic CS needs no standing action
 *                                here; accepted as a no-op.
 *   CAPI_SPI_CS_MANUAL_ASSERT  - drive CS active outside of a transfer.
 *   CAPI_SPI_CS_MANUAL_DEASSERT- drive CS inactive outside of a transfer.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_spi_pl_set_cs(struct capi_spi_device *device,
			      enum capi_spi_cs_control cs)
{
	if (device == NULL || device->controller == NULL)
		return -EINVAL;
	struct capi_spi_xilinx_handle *xh = device->controller->priv;
	if (xh->async_in_progress || inst(xh)->IsBusy)
		return -EBUSY;

	if ((device->cs_gpio == NULL) != (device->cs_gpio_num == 0))
		return -EINVAL;
	if (device->cs_gpio_num > 0 && device->native_cs != 0)
		return -EINVAL;

	if (cs == CAPI_SPI_CS_AUTO)
		return 0;

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
	struct capi_spi_xilinx_handle *xh = ((struct capi_spi_controller_handle *)
					     handle)->priv;
	XSpi_InterruptHandler(inst(xh));
	spi_finish_irq(xh);
}

#endif /* XPAR_XSPI_NUM_INSTANCES */
