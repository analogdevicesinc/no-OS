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
#include "xspi.h"

/*
 * CAPI uses native_cs == 0 as "no explicit native CS was supplied".
 * For AXI SPI we adapt that to native CS0, represented by bit 0.
 */
#define XSPI_CAPI_DEFAULT_NATIVE_CS_MASK	1U
/*
 * XSpi_SetSlaveSelect() takes a slave-select bitmask. Zero means no native
 * slave-select is asserted, which is what GPIO-CS users need.
 */
#define XSPI_CAPI_DESELECT_ALL_NATIVE		0U
/* A non-continuous transfer is split into byte-sized CAPI frames. */
#define XSPI_CAPI_SINGLE_BYTE_TRANSFER		1U
/* CAPI transfer lengths are uint16_t, so composite read-command stays here. */
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
	/* Preserve CAPI's success convention; already-started is benign here. */
	if (status == XST_SUCCESS || status == XST_DEVICE_IS_STARTED)
		return 0;

	/* Busy is observable API state, not a generic I/O failure. */
	if (status == XST_DEVICE_BUSY)
		return -EBUSY;

	/* Preserve CAPI's "requested hardware is not present" convention. */
	if (status == XST_DEVICE_NOT_FOUND)
		return -ENODEV;

	/* Transfer cannot run while stopped; this is a backend/device failure. */
	if (status == XST_DEVICE_IS_STOPPED)
		return -EIO;

	/* Missing or malformed slave selection is a bad CAPI device config. */
	if (status == XST_SPI_NO_SLAVE || status == XST_SPI_TOO_MANY_SLAVES)
		return -EINVAL;

	/* Master-only backend: Xilinx slave/not-master statuses are unsupported. */
	if (status == XST_SPI_NOT_MASTER || status == XST_SPI_SLAVE_ONLY)
		return -ENOTSUP;

	/* Remaining Xilinx driver failures become the CAPI generic I/O error. */
	return -EIO;
}

static int gpio_cs_assert(const struct capi_spi_device *device);
static int gpio_cs_deassert(const struct capi_spi_device *device);

static int pl_async_cleanup(struct capi_spi_xilinx_handle *xh, bool copy_rx)
{
	int cs_ret = 0;

	if (copy_rx && xh->async.rx_buffer != NULL &&
	    xh->async.transfer.rx_buf != NULL && xh->async.transfer.rx_size > 0) {
		/*
		 * async.rx_offset is 0 for transceive (copy from byte 0) and
		 * tx_size for read_command (skip the command echo bytes first).
		 */
		memcpy(xh->async.transfer.rx_buf,
		       xh->async.rx_buffer + xh->async.rx_offset,
		       xh->async.transfer.rx_size);
	}
	capi_free(xh->async.tx_buffer);
	capi_free(xh->async.rx_buffer);
	if (xh->async.gpio_cs_device != NULL) {
		cs_ret = gpio_cs_deassert(xh->async.gpio_cs_device);
		xh->async.gpio_cs_device = NULL;
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

	/*
	 * These two events are informational notifications that XSpi fires
	 * alongside the real completion event. Handling them here would
	 * double-invoke pl_async_cleanup and corrupt the callback state.
	 */
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

static int pl_validate_data_width(struct capi_spi_xilinx_handle *xh)
{
	/*
	 * CAPI SPI buffers and sizes are byte-oriented. XSpi can also be built
	 * for 16-bit or 32-bit data registers, where embeddedsw advances buffer
	 * pointers by the configured word width. Reject those wider instances
	 * until a word-aware CAPI adapter is added.
	 */
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
	if (device->cs_gpio_num == 0) {
		/*
		 * CAPI native_cs == 0 means "driver default"; this adapter
		 * chooses CS0. A non-zero value must remain one-hot because
		 * this backend currently drives one target per CAPI device.
		 */
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
	/*
	 * GPIO chip-select users drive CS through CAPI GPIO, so keep all native
	 * AXI SPI chip-select lines inactive. Native-CS users get converted below.
	 */
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
	/* Manual CS is only valid for a single native CAPI CS bit. */
	u32 selected = native_cs;
	if (selected == 0U)
		selected = XSPI_CAPI_DEFAULT_NATIVE_CS_MASK;
	if ((selected & ~inst(xh)->SlaveSelectMask) != 0U ||
	    (selected & (selected - 1U)) != 0U)
		return -EINVAL;

	/* Deassert uses the AXI SPI "no native slave selected" mask. */
	u32 mask = assert ? selected : XSPI_CAPI_DESELECT_ALL_NATIVE;
	int ret = xspi_status_to_errno(XSpi_SetSlaveSelect(inst(xh), mask));
	if (ret != 0)
		return ret;
	/*
	 * XSpi_SetSlaveSelect only writes to the spi->SlaveSelectReg field in
	 * the driver struct; it does not write the hardware. XSpi_SetSlaveSelectReg
	 * takes that field and writes it to the AXI SPI slave-select register
	 * (SSR) in hardware. Both calls are needed to make the CS change take effect.
	 */
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
	if (config->three_pin_mode || config->dma_handle != NULL ||
	    config->clk_freq_hz != 0)
		return -ENOTSUP;

	bool alloc = (*handle == NULL);
	struct capi_spi_controller_handle *h = *handle;
	struct capi_spi_xilinx_handle *xh;

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

	XSpi *spi = capi_malloc(sizeof(XSpi));
	if (!spi) {
		if (alloc)
			xilinx_spi_free_allocated_handle(handle);
		else
			xilinx_spi_clear_app_handle(h);
		return -ENOMEM;
	}
	memset(spi, 0, sizeof(XSpi));

	int ret = xspi_status_to_errno(XSpi_Initialize(spi,
				       (UINTPTR)config->identifier));
	if (ret != 0) {
		capi_free(spi);
		if (alloc)
			xilinx_spi_free_allocated_handle(handle);
		else
			xilinx_spi_clear_app_handle(h);
		return ret;
	}

	XSpi_Reset(spi);
	xh->instance = spi;

	/* CAPI SPI is byte-buffered; reject wider AXI SPI data registers early. */
	ret = pl_validate_data_width(xh);
	if (ret != 0) {
		capi_free(spi);
		xh->instance = NULL;
		if (alloc)
			xilinx_spi_free_allocated_handle(handle);
		else
			xilinx_spi_clear_app_handle(h);
		return ret;
	}

	u32 options = XSP_MASTER_OPTION | XSP_MANUAL_SSELECT_OPTION;
	if (config->loopback)
		options |= XSP_LOOPBACK_OPTION;
	ret = xspi_status_to_errno(XSpi_SetOptions(spi, options));
	if (ret != 0) {
		capi_free(spi);
		xh->instance = NULL;
		if (alloc)
			xilinx_spi_free_allocated_handle(handle);
		else
			xilinx_spi_clear_app_handle(h);
		return ret;
	}

	XSpi_SetStatusHandler(spi, xh, spi_status_handler);
	ret = xspi_status_to_errno(XSpi_Start(spi));
	if (ret != 0) {
		XSpi_IntrGlobalDisable(spi);
		XSpi_Reset(spi);
		/*
		 * XSpi_Reset issues an SRR (software reset register) soft reset
		 * but does not clear DGIER (device global interrupt enable register).
		 * Write DGIER to zero explicitly so the hardware does not fire
		 * interrupts after the handle is freed.
		 */
		XSpi_WriteReg(spi->BaseAddr, XSP_DGIER_OFFSET, 0);
		capi_free(spi);
		xh->instance = NULL;
		if (alloc)
			xilinx_spi_free_allocated_handle(handle);
		else
			xilinx_spi_clear_app_handle(h);
		return ret;
	}
	/*
	 * XSpi_Start enables the AXI SPI core and sets DGIER (device global
	 * interrupt enable register) to 1. Disable global interrupts immediately
	 * so the driver starts in polled mode; async paths call
	 * XSpi_IntrGlobalEnable when they are ready for interrupt delivery.
	 */
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
		/* DGIER survives SRR soft reset; clear it explicitly. */
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
	/* CAPI entry-point validation: thin wrappers already check ops only. */
	if (device == NULL || device->controller == NULL || transfer == NULL)
		return -EINVAL;

	/* Backend capability validation lives here, not in generic CAPI tests. */
	int ret = pl_validate_transfer(device, transfer, false, false);
	if (ret != 0)
		return ret;

	struct capi_spi_xilinx_handle *xh = device->controller->priv;
	if (xh->async_in_progress || inst(xh)->IsBusy)
		return -EBUSY;

	ret = pl_validate_data_width(xh);
	if (ret != 0)
		return ret;

	/* CAPI transceive length is the larger TX/RX phase; NULL TX means zeroes. */
	uint16_t len =
		(transfer->tx_size > transfer->rx_size) ? transfer->tx_size : transfer->rx_size;
	if (len == 0)
		return -EINVAL;

	/* Always copy TX because XSpi takes a non-const buffer. */
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

	/* Translate CAPI SPI mode and loopback into XSpi transfer options. */
	ret = xspi_status_to_errno(XSpi_SetOptions(inst(xh),
				   pl_build_options(device, xh->loopback)));
	if (ret != 0) {
		capi_free(tx);
		if (need_rx_temp)
			capi_free(rx);
		return ret;
	}

	/* XSpi_SetOptions resets control bits, so reapply CAPI bit order. */
	pl_apply_lsb(inst(xh), device->lsb_first);

	/* Select native CS or leave native CS inactive when GPIO CS is used. */
	bool use_gpio_cs = device->cs_gpio_num > 0;
	ret = pl_select_device(xh, device);
	if (ret != 0) {
		capi_free(tx);
		if (need_rx_temp)
			capi_free(rx);
		return ret;
	}

	/*
	 * XSpi_Transfer runs polled when DGIER is disabled and interrupt-driven
	 * when enabled. Disable global interrupts for the synchronous path so
	 * the call blocks until the transfer finishes and returns XST_SUCCESS.
	 */
	XSpi_IntrGlobalDisable(inst(xh));

	int status;
	int gpio_error = 0;
	if (use_gpio_cs && device->non_continuous_mode) {
		status = XST_SUCCESS;
		for (uint16_t i = 0; i < len && status == XST_SUCCESS; i++) {
			/* Non-continuous CAPI mode toggles GPIO CS around each byte. */
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

	if (xh->use_irq)
		XSpi_IntrGlobalEnable(inst(xh));

	/* Copy only the CAPI-requested RX bytes back out of any temporary buffer. */
	if (need_rx_temp && transfer->rx_buf && transfer->rx_size > 0)
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
 * @return 0 on success, negative errno on failure.
 */
static int capi_spi_pl_transceive_async(struct capi_spi_device *device,
					struct capi_spi_transfer *transfer, int timeout)
{
	/* CAPI entry-point validation: this backend owns transfer semantics. */
	if (device == NULL || device->controller == NULL || transfer == NULL)
		return -EINVAL;

	/* Async AXI SPI needs IRQ support and does not emulate timeout in software. */
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

	/* CAPI transceive length is the larger TX/RX phase; NULL TX means zeroes. */
	uint16_t len =
		(transfer->tx_size > transfer->rx_size) ? transfer->tx_size : transfer->rx_size;
	if (len == 0)
		return -EINVAL;

	xh->async_in_progress = true;

	/* Always copy TX because XSpi takes a non-const buffer. */
	bool need_rx_temp = (transfer->rx_buf == NULL) || (transfer->rx_size < len);

	uint8_t *tx = capi_malloc(len);
	if (!tx) {
		xh->async_in_progress = false;
		return -ENOMEM;
	}
	memset(tx, 0, len);
	if (transfer->tx_buf && transfer->tx_size > 0)
		memcpy(tx, transfer->tx_buf, transfer->tx_size);
	/* Keep tx alive: XSpi DMA reads from it after this function returns. */
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
		/* Keep rx alive: XSpi writes into it until the ISR fires. */
		xh->async.rx_buffer = rx;
		xh->async.transfer.rx_buf = transfer->rx_buf;
		xh->async.transfer.rx_size = transfer->rx_size;
	} else {
		xh->async.rx_buffer = NULL;
		xh->async.transfer.rx_buf = NULL;
	}
	/* 0 = transceive: copy from byte 0 (no command prefix to skip). */
	xh->async.rx_offset = 0;
	xh->async.transfer.rx_size = transfer->rx_size;

	/* Translate CAPI SPI mode and loopback into XSpi transfer options. */
	ret = xspi_status_to_errno(XSpi_SetOptions(inst(xh),
				   pl_build_options(device, xh->loopback)));
	if (ret != 0) {
		(void)pl_async_cleanup(xh, false);
		return ret;
	}

	/* XSpi_SetOptions resets control bits, so reapply CAPI bit order. */
	pl_apply_lsb(inst(xh), device->lsb_first);

	/* Select native CS or leave native CS inactive when GPIO CS is used. */
	ret = pl_select_device(xh, device);
	if (ret != 0) {
		/*
		 * Once async_in_progress is set, all error paths must go through
		 * pl_async_cleanup rather than freeing directly; it also deasserts
		 * GPIO CS if it was already asserted.
		 */
		(void)pl_async_cleanup(xh, false);
		return ret;
	}

	/* GPIO CS must stay asserted until the Xilinx status callback completes. */
	if (device->cs_gpio_num > 0) {
		xh->async.gpio_cs_device = device;
		ret = gpio_cs_assert(device);
		if (ret != 0) {
			(void)pl_async_cleanup(xh, false);
			return ret;
		}
	}

	/* True async path: XSpi_Transfer returns after arming interrupts. */
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
	/* CAPI read_command keeps TX command bytes and RX data in one CS frame. */
	if (device == NULL || device->controller == NULL || transfer == NULL)
		return -EINVAL;

	/* The backend rejects optional CAPI features that XSpi cannot honor. */
	int ret = pl_validate_transfer(device, transfer, false, true);
	if (ret != 0)
		return ret;

	struct capi_spi_xilinx_handle *xh = device->controller->priv;
	if (xh->async_in_progress || inst(xh)->IsBusy)
		return -EBUSY;

	ret = pl_validate_data_width(xh);
	if (ret != 0)
		return ret;

	/* Composite transfer length: command/address TX bytes followed by RX bytes. */
	uint32_t total = (uint32_t)transfer->tx_size + transfer->rx_size;
	if (total == 0)
		return 0;
	if (total > XSPI_CAPI_MAX_TRANSFER_SIZE)
		return -EINVAL;

	/* Build a full-duplex Xilinx transfer; dummy TX clocks the read phase. */
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

	/* Translate CAPI SPI mode and loopback into XSpi transfer options. */
	ret = xspi_status_to_errno(XSpi_SetOptions(inst(xh),
				   pl_build_options(device, xh->loopback)));
	if (ret != 0) {
		capi_free(tx);
		capi_free(rx);
		return ret;
	}

	/* XSpi_SetOptions resets control bits, so reapply CAPI bit order. */
	pl_apply_lsb(inst(xh), device->lsb_first);

	/* Select native CS or leave native CS inactive when GPIO CS is used. */
	ret = pl_select_device(xh, device);
	if (ret != 0) {
		capi_free(tx);
		capi_free(rx);
		return ret;
	}

	/* This is the synchronous CAPI API, so force the Xilinx polled path. */
	XSpi_IntrGlobalDisable(inst(xh));

	int status;
	int gpio_error = 0;
	if (device->cs_gpio_num > 0) {
		/* GPIO CS covers the whole command+read sequence as one frame. */
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

	/* Copy RX data after the command bytes; earlier RX bytes are protocol echo/dummy. */
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
 * @return 0 on success, negative errno on failure.
 */
static int capi_spi_pl_read_command_async(struct capi_spi_device *device,
		struct capi_spi_transfer *transfer)
{
	/* Async read_command is still one CAPI CS frame, just IRQ-completed. */
	if (device == NULL || device->controller == NULL || transfer == NULL)
		return -EINVAL;

	/* The backend rejects optional CAPI features that XSpi cannot honor. */
	int ret = pl_validate_transfer(device, transfer, true, true);
	if (ret != 0)
		return ret;

	/* Composite transfer length: command/address TX bytes followed by RX bytes. */
	uint32_t total32 = (uint32_t)transfer->tx_size + transfer->rx_size;
	if (total32 > XSPI_CAPI_MAX_TRANSFER_SIZE)
		return -EINVAL;
	uint16_t total = (uint16_t)total32;
	if (total == 0)
		return 0;

	struct capi_spi_xilinx_handle *xh = device->controller->priv;
	if (!xh->use_irq)
		return -ENOTSUP;
	if (xh->async_in_progress || inst(xh)->IsBusy)
		return -EBUSY;

	ret = pl_validate_data_width(xh);
	if (ret != 0)
		return ret;

	/* Build one full-duplex Xilinx buffer; dummy TX clocks the read phase. */
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

	/* Both buffers must outlive this function: XSpi reads tx and writes rx. */
	xh->async.tx_buffer = tx;
	xh->async.rx_buffer = rx;
	xh->async.transfer.rx_buf = transfer->rx_buf;
	/*
	 * async.rx_offset is the command-prefix length. pl_async_cleanup copies
	 * rx[rx_offset .. rx_offset+transfer.rx_size) to transfer.rx_buf,
	 * skipping the full-duplex echo of the command bytes.
	 */
	xh->async.rx_offset = transfer->tx_size;
	xh->async.transfer.rx_size = transfer->rx_size;
	xh->async_in_progress = true;

	/* Translate CAPI SPI mode and loopback into XSpi transfer options. */
	ret = xspi_status_to_errno(XSpi_SetOptions(inst(xh),
				   pl_build_options(device, xh->loopback)));
	if (ret != 0) {
		(void)pl_async_cleanup(xh, false);
		return ret;
	}

	/* XSpi_SetOptions resets control bits, so reapply CAPI bit order. */
	pl_apply_lsb(inst(xh), device->lsb_first);

	/* Select native CS or leave native CS inactive when GPIO CS is used. */
	ret = pl_select_device(xh, device);
	if (ret != 0) {
		(void)pl_async_cleanup(xh, false);
		return ret;
	}

	/* GPIO CS must stay asserted until the Xilinx status callback completes. */
	if (device->cs_gpio_num > 0) {
		xh->async.gpio_cs_device = device;
		ret = gpio_cs_assert(device);
		if (ret != 0) {
			(void)pl_async_cleanup(xh, false);
			return ret;
		}
	}

	/* True async path: XSpi_Transfer returns after arming interrupts. */
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
	struct capi_spi_xilinx_handle *xh = device->controller->priv;
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
	struct capi_spi_xilinx_handle *xh = ((struct capi_spi_controller_handle *)
					     handle)->priv;
	XSpi_InterruptHandler(inst(xh));
}
