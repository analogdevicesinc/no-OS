/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Xilinx XSpiPs SPI driver (Zynq PS SPI, master mode only)
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
#include "xspips.h"
#include "xspips_hw.h"

/*
 * CAPI uses native_cs == 0 as "no explicit native CS was supplied".
 * For Xilinx PS SPI we adapt that to native CS0, represented by bit 0.
 */
#define XSPIPS_CAPI_DEFAULT_NATIVE_CS_MASK	1U
/*
 * The PS SPI controller exposes native slave indexes 0..2 in normal
 * non-decoded mode. The largest valid one-hot CAPI bitmask is therefore bit 2.
 */
#define XSPIPS_CAPI_LAST_NATIVE_CS_MASK		0x04U
/*
 * XSpiPs_SetSlaveSelect() documents 0x0f as "all native chip-select lines
 * deasserted". Name it here so the CAPI adapter never hides a magic 0x0f.
 */
#define XSPIPS_CAPI_DESELECT_ALL_NATIVE		0x0FU
/* A non-continuous transfer is split into byte-sized CAPI frames. */
#define XSPIPS_CAPI_SINGLE_BYTE_TRANSFER	1U
/* Bounded RX FIFO drain used only during deinit/reset cleanup. */
#define XSPIPS_CAPI_RX_DRAIN_LIMIT		128U
/* CAPI transfer lengths are uint16_t, so composite read-command stays here. */
#define XSPIPS_CAPI_MAX_TRANSFER_SIZE		UINT16_MAX
/* XSpiPs encodes SPI clock as input_clk / 2^(prescaler + 1). */
#define XSPIPS_CAPI_PRESCALER_EXP_OFFSET	1U

static int capi_spi_ps_init(struct capi_spi_controller_handle **handle,
			    const struct capi_spi_config *config);
static int capi_spi_ps_deinit(struct capi_spi_controller_handle *handle);
static int capi_spi_ps_transceive(struct capi_spi_device *device,
				  struct capi_spi_transfer *transfer);
static int capi_spi_ps_transceive_async(struct capi_spi_device *device,
					struct capi_spi_transfer *transfer, int timeout);
static int capi_spi_ps_read_command(struct capi_spi_device *device,
				    struct capi_spi_transfer *transfer);
static int capi_spi_ps_read_command_async(struct capi_spi_device *device,
		struct capi_spi_transfer *transfer);
static int capi_spi_ps_abort_async(struct capi_spi_device *device);
static int capi_spi_ps_register_callback(struct capi_spi_controller_handle
		*handle,
		capi_spi_callback_t const callback, void *callback_arg);
static int capi_spi_ps_set_cs(struct capi_spi_device *device,
			      enum capi_spi_cs_control cs);
static void capi_spi_ps_isr(void *handle);
static void spips_status_handler(const void *callback_ref, u32 status_event,
				 u32 byte_count);

struct capi_spi_ops capi_spi_xilinx_ps_ops = {
	.init = capi_spi_ps_init,
	.deinit = capi_spi_ps_deinit,
	.transceive = capi_spi_ps_transceive,
	.transceive_async = capi_spi_ps_transceive_async,
	.read_command = capi_spi_ps_read_command,
	.read_command_async = capi_spi_ps_read_command_async,
	.abort_async = capi_spi_ps_abort_async,
	.register_callback = capi_spi_ps_register_callback,
	.set_cs = capi_spi_ps_set_cs,
	.isr = capi_spi_ps_isr,
};

static XSpiPs *inst(struct capi_spi_xilinx_handle *xh)
{
	return (XSpiPs *)xh->instance;
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

static int spips_status_to_errno(int status)
{
	/* Preserve CAPI's success convention. */
	if (status == XST_SUCCESS)
		return 0;

	/* Busy is observable API state, not a generic I/O failure. */
	if (status == XST_DEVICE_BUSY)
		return -EBUSY;

	/* Preserve CAPI's "requested hardware is not present" convention. */
	if (status == XST_DEVICE_NOT_FOUND)
		return -ENODEV;

	/* Missing or malformed slave selection is a bad CAPI device config. */
	if (status == XST_SPI_NO_SLAVE || status == XST_SPI_TOO_MANY_SLAVES)
		return -EINVAL;

	/* Master-only backend: Xilinx slave/not-master statuses are unsupported. */
	if (status == XST_SPI_NOT_MASTER || status == XST_SPI_SLAVE_ONLY)
		return -ENOTSUP;

	/* Remaining Xilinx driver failures become the CAPI generic I/O error. */
	return -EIO;
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

static u8 spips_calc_prescaler(u32 input_clk_hz, u32 desired_hz)
{
	if (desired_hz == 0 || input_clk_hz == 0)
		return XSPIPS_CLK_PRESCALE_64;

	u8 prescale;
	for (prescale = XSPIPS_CLK_PRESCALE_4; prescale <= XSPIPS_CLK_PRESCALE_256;
	     prescale++) {
		u32 actual_hz =
			input_clk_hz / (1U << (prescale + XSPIPS_CAPI_PRESCALER_EXP_OFFSET));
		if (actual_hz <= desired_hz)
			return prescale;
	}
	return XSPIPS_CLK_PRESCALE_256;
}

static int spips_set_frequency(XSpiPs *spi, u32 desired_hz)
{
	if (desired_hz == 0)
		return 0;
	if (spi->Config.InputClockHz == 0)
		return -ENOTSUP;

	u8 prescale = spips_calc_prescaler(spi->Config.InputClockHz, desired_hz);
	u32 actual_hz =
		spi->Config.InputClockHz /
		(1U << (prescale + XSPIPS_CAPI_PRESCALER_EXP_OFFSET));
	if (actual_hz > desired_hz)
		return -ENOTSUP;

	return spips_status_to_errno(XSpiPs_SetClkPrescaler(spi, prescale));
}

/* Convert a one-hot native_cs bitmask to PS SPI slave index (0, 1, or 2). */
static int ps_cs_to_index(uint16_t native_cs, u8 *index)
{
	/*
	 * CAPI native_cs == 0 means "driver default"; this adapter chooses CS0.
	 * A non-zero value must be exactly one bit because XSpiPs accepts one
	 * slave index, not an arbitrary multi-CS mask.
	 */
	uint16_t mask = native_cs;
	if (mask == 0U)
		mask = XSPIPS_CAPI_DEFAULT_NATIVE_CS_MASK;

	if (index == NULL || mask > XSPIPS_CAPI_LAST_NATIVE_CS_MASK ||
	    (mask & (mask - 1U)) != 0U)
		return -EINVAL;

	/* Convert the one-hot CAPI mask into the Xilinx slave index. */
	u8 idx = 0;
	while (mask > XSPIPS_CAPI_DEFAULT_NATIVE_CS_MASK) {
		mask >>= 1;
		idx++;
	}
	*index = idx;
	return 0;
}

static int ps_validate_transfer(const struct capi_spi_device *device,
				const struct capi_spi_transfer *transfer,
				bool async, bool read_command)
{
	if (device->lsb_first)
		return -ENOTSUP;
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
		u8 index;
		int ret = ps_cs_to_index(device->native_cs, &index);
		if (ret != 0)
			return ret;
	}
	if (transfer->timeout > 0 || transfer->xfer_delay_clk_cycles != 0)
		return -ENOTSUP;
	return 0;
}

static int ps_select_device(struct capi_spi_xilinx_handle *xh,
			    const struct capi_spi_device *device)
{
	/*
	 * GPIO chip-select users drive CS through CAPI GPIO, so keep all native
	 * PS chip-select lines inactive. Native-CS users get converted below.
	 */
	u8 index = XSPIPS_CAPI_DESELECT_ALL_NATIVE;
	if (device->cs_gpio_num == 0) {
		int ret = ps_cs_to_index(device->native_cs, &index);
		if (ret != 0)
			return ret;
	}
	return spips_status_to_errno(XSpiPs_SetSlaveSelect(inst(xh), index));
}

static int ps_drive_native_cs(struct capi_spi_xilinx_handle *xh,
			      uint16_t native_cs, bool assert)
{
	/* Manual CS is only valid for a single native CAPI CS bit. */
	u8 selected;
	int ret = ps_cs_to_index(native_cs, &selected);
	if (ret != 0)
		return ret;

	/* Deassert uses the documented XSpiPs all-lines-inactive selector. */
	u8 index = assert ? selected : XSPIPS_CAPI_DESELECT_ALL_NATIVE;
	ret = spips_status_to_errno(XSpiPs_SetSlaveSelect(inst(xh), index));
	if (ret != 0)
		return ret;

	u32 config = XSpiPs_ReadReg(inst(xh)->Config.BaseAddress, XSPIPS_CR_OFFSET);
	config &= ~XSPIPS_CR_SSCTRL_MASK;
	config |= inst(xh)->SlaveSelect;
	XSpiPs_WriteReg(inst(xh)->Config.BaseAddress, XSPIPS_CR_OFFSET, config);
	if (assert)
		XSpiPs_Enable(inst(xh));
	else
		XSpiPs_Disable(inst(xh));
	return 0;
}

static int ps_async_cleanup(struct capi_spi_xilinx_handle *xh, bool copy_rx)
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
	}
	memset(&xh->async, 0, sizeof(xh->async));
	xh->async_in_progress = false;
	return cs_ret;
}

static void spips_status_handler(const void *callback_ref, u32 status_event,
				 u32 byte_count)
{
	struct capi_spi_xilinx_handle *xh =
		(struct capi_spi_xilinx_handle *)(uintptr_t)callback_ref;
	(void)byte_count;

	if (xh == NULL)
		return;

	if (status_event == XST_SPI_TRANSFER_DONE) {
		int ret = ps_async_cleanup(xh, true);
		if (xh->callback != NULL) {
			enum capi_async_event event = (ret == 0) ?
						      CAPI_SPI_EVENT_XFR_DONE :
						      CAPI_SPI_EVENT_ERROR;
			xh->callback(event, xh->callback_arg, ret);
		}
	} else {
		XSpiPs_WriteReg(inst(xh)->Config.BaseAddress, XSPIPS_IDR_OFFSET,
				XSPIPS_IXR_DISABLE_ALL_MASK);
		XSpiPs_Abort(inst(xh));
		(void)ps_async_cleanup(xh, false);
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

/**
 * @brief Initialize the CAPI backend instance.
 * @note PS: XSpiPs_LookupConfig()/XSpiPs_CfgInitialize().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_spi_ps_init(struct capi_spi_controller_handle **handle,
			    const struct capi_spi_config *config)
{
	if (handle == NULL || config == NULL)
		return -EINVAL;
	if (config->three_pin_mode || config->loopback ||
	    config->dma_handle != NULL)
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
	h->ops = config->ops ? config->ops : &capi_spi_xilinx_ps_ops;
	h->priv = xh;
	xh->loopback = config->loopback;

	XSpiPs_Config *cfg = XSpiPs_LookupConfig((UINTPTR)config->identifier);
	if (cfg == NULL) {
		if (alloc)
			xilinx_spi_free_allocated_handle(handle);
		else
			xilinx_spi_clear_app_handle(h);
		return -ENODEV;
	}

	XSpiPs *ps = capi_malloc(sizeof(XSpiPs));
	if (ps == NULL) {
		if (alloc)
			xilinx_spi_free_allocated_handle(handle);
		else
			xilinx_spi_clear_app_handle(h);
		return -ENOMEM;
	}
	memset(ps, 0, sizeof(XSpiPs));

	int ret = spips_status_to_errno(XSpiPs_CfgInitialize(ps, cfg,
					cfg->BaseAddress));
	if (ret != 0) {
		capi_free(ps);
		if (alloc)
			xilinx_spi_free_allocated_handle(handle);
		else
			xilinx_spi_clear_app_handle(h);
		return ret;
	}

	XSpiPs_Reset(ps);
	xh->instance = ps;

	u32 options = XSPIPS_MASTER_OPTION | XSPIPS_FORCE_SSELECT_OPTION;
	ret = spips_status_to_errno(XSpiPs_SetOptions(ps, options));
	if (ret != 0) {
		capi_free(ps);
		xh->instance = NULL;
		if (alloc)
			xilinx_spi_free_allocated_handle(handle);
		else
			xilinx_spi_clear_app_handle(h);
		return ret;
	}

	if (config->clk_freq_hz == 0) {
		ret = spips_status_to_errno(XSpiPs_SetClkPrescaler(ps,
					    XSPIPS_CLK_PRESCALE_64));
	} else {
		ret = spips_set_frequency(ps, config->clk_freq_hz);
	}
	if (ret != 0) {
		capi_free(ps);
		xh->instance = NULL;
		if (alloc)
			xilinx_spi_free_allocated_handle(handle);
		else
			xilinx_spi_clear_app_handle(h);
		return ret;
	}

	XSpiPs_SetStatusHandler(ps, xh, spips_status_handler);

	const struct capi_spi_xilinx_config *xcfg =
		(const struct capi_spi_xilinx_config *)config->extra;
	if (xcfg != NULL && xcfg->use_irq) {
		uint32_t irq_id = xcfg->irq_id;
		ret = capi_irq_connect(irq_id, capi_spi_ps_isr, h);
		if (ret == 0) {
			capi_irq_set_level_edge_trigger(irq_id, CAPI_IRQ_LEVEL_HIGH);
			ret = capi_irq_enable(irq_id);
			if (ret == 0) {
				xh->irq_id = irq_id;
				xh->use_irq = true;
			}
		}
	}

	return 0;
}

/**
 * @brief Deinitialize the CAPI backend instance.
 * @note PS: XSpiPs_Abort()/XSpiPs_Disable().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_spi_ps_deinit(struct capi_spi_controller_handle *handle)
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
		if (xh->async_in_progress || inst(xh)->IsBusy)
			XSpiPs_Abort(inst(xh));
		(void)ps_async_cleanup(xh, false);
		XSpiPs_Disable(inst(xh));
		XSpiPs_WriteReg(inst(xh)->Config.BaseAddress, XSPIPS_IDR_OFFSET,
				XSPIPS_IXR_DISABLE_ALL_MASK);
		XSpiPs_WriteReg(inst(xh)->Config.BaseAddress, XSPIPS_SR_OFFSET,
				XSPIPS_IXR_WR_TO_CLR_MASK);
		for (uint32_t i = 0; i < XSPIPS_CAPI_RX_DRAIN_LIMIT; i++) {
			if (!(XSpiPs_ReadReg(inst(xh)->Config.BaseAddress, XSPIPS_SR_OFFSET) &
			      XSPIPS_IXR_RXNEMPTY_MASK))
				break;
			(void)XSpiPs_ReadReg(inst(xh)->Config.BaseAddress, XSPIPS_RXD_OFFSET);
		}
		XSpiPs_Reset(inst(xh));
		capi_free(xh->instance);
		xh->instance = NULL;
	} else {
		(void)ps_async_cleanup(xh, false);
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
 * @note PS: XSpiPs_SetSlaveSelect()/XSpiPs_PolledTransfer().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_spi_ps_transceive(struct capi_spi_device *device,
				  struct capi_spi_transfer *transfer)
{
	/* CAPI entry-point validation: thin wrappers already check ops only. */
	if (device == NULL || device->controller == NULL || transfer == NULL)
		return -EINVAL;

	/* Backend capability validation lives here, not in generic CAPI tests. */
	int ret = ps_validate_transfer(device, transfer, false, false);
	if (ret != 0)
		return ret;

	struct capi_spi_xilinx_handle *xh = device->controller->priv;
	if (xh->async_in_progress || inst(xh)->IsBusy)
		return -EBUSY;

	/* CAPI transceive length is the larger TX/RX phase; NULL TX means zeroes. */
	uint16_t len =
		(transfer->tx_size > transfer->rx_size) ? transfer->tx_size : transfer->rx_size;
	if (len == 0)
		return -EINVAL;

	/* Always copy TX because XSpiPs takes a non-const buffer. */
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

	/* Translate CAPI SPI mode into XSpiPs transfer options for this call. */
	u32 options = XSPIPS_MASTER_OPTION | XSPIPS_FORCE_SSELECT_OPTION;
	if (device->mode & CAPI_SPI_CPHA)
		options |= XSPIPS_CLK_PHASE_1_OPTION;
	if (device->mode & CAPI_SPI_CPOL)
		options |= XSPIPS_CLK_ACTIVE_LOW_OPTION;
	ret = spips_status_to_errno(XSpiPs_SetOptions(inst(xh), options));
	if (ret != 0) {
		capi_free(tx);
		if (need_rx_temp)
			capi_free(rx);
		return ret;
	}

	/* Apply the per-device max speed before selecting the target. */
	ret = spips_set_frequency(inst(xh), device->max_speed_hz);
	if (ret != 0) {
		capi_free(tx);
		if (need_rx_temp)
			capi_free(rx);
		return ret;
	}

	/* Select native CS or leave native CS inactive when GPIO CS is used. */
	bool use_gpio_cs = device->cs_gpio_num > 0;
	ret = ps_select_device(xh, device);
	if (ret != 0) {
		capi_free(tx);
		if (need_rx_temp)
			capi_free(rx);
		return ret;
	}

	/* This is the synchronous CAPI API, so force the Xilinx polled path. */
	XSpiPs_WriteReg(inst(xh)->Config.BaseAddress, XSPIPS_IDR_OFFSET,
			XSPIPS_IXR_DISABLE_ALL_MASK);

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
			status = XSpiPs_PolledTransfer(inst(xh), tx + i, rx ? rx + i : NULL,
						       XSPIPS_CAPI_SINGLE_BYTE_TRANSFER);
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
				status = XSpiPs_PolledTransfer(inst(xh), tx, rx, len);
			}
			ret = gpio_cs_deassert(device);
			if (ret != 0 && status == XST_SUCCESS) {
				gpio_error = ret;
				status = XST_FAILURE;
			}
		} else {
			status = XSpiPs_PolledTransfer(inst(xh), tx, rx, len);
		}
	}

	if (!use_gpio_cs)
		(void)XSpiPs_SetSlaveSelect(inst(xh), XSPIPS_CAPI_DESELECT_ALL_NATIVE);

	/* Copy only the CAPI-requested RX bytes back out of any temporary buffer. */
	if (need_rx_temp && transfer->rx_buf && transfer->rx_size > 0)
		memcpy(transfer->rx_buf, rx, transfer->rx_size);

	capi_free(tx);
	if (need_rx_temp)
		capi_free(rx);

	return (gpio_error != 0) ? gpio_error :
	       spips_status_to_errno(status);
}

/**
 * @brief Start an asynchronous SPI transceive operation.
 * @note PS: XSpiPs_Transfer() plus IRQ enable.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_spi_ps_transceive_async(struct capi_spi_device *device,
					struct capi_spi_transfer *transfer, int timeout)
{
	/* CAPI entry-point validation: this backend owns transfer semantics. */
	if (device == NULL || device->controller == NULL || transfer == NULL)
		return -EINVAL;

	/* Async PS SPI needs IRQ support and does not emulate timeout in software. */
	int ret = ps_validate_transfer(device, transfer, true, false);
	if (ret != 0)
		return ret;
	if (timeout != 0)
		return -ENOTSUP;

	struct capi_spi_xilinx_handle *xh = device->controller->priv;
	if (!xh->use_irq)
		return -ENOTSUP;
	if (xh->async_in_progress || inst(xh)->IsBusy)
		return -EBUSY;

	/* CAPI transceive length is the larger TX/RX phase; NULL TX means zeroes. */
	uint16_t len =
		(transfer->tx_size > transfer->rx_size) ? transfer->tx_size : transfer->rx_size;
	if (len == 0)
		return -EINVAL;

	xh->async_in_progress = true;

	/* Always copy TX because XSpiPs takes a non-const buffer. */
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

	/* Translate CAPI SPI mode into XSpiPs options before starting IRQ transfer. */
	u32 options = XSPIPS_MASTER_OPTION | XSPIPS_FORCE_SSELECT_OPTION;
	if (device->mode & CAPI_SPI_CPHA)
		options |= XSPIPS_CLK_PHASE_1_OPTION;
	if (device->mode & CAPI_SPI_CPOL)
		options |= XSPIPS_CLK_ACTIVE_LOW_OPTION;
	ret = spips_status_to_errno(XSpiPs_SetOptions(inst(xh), options));
	if (ret != 0) {
		(void)ps_async_cleanup(xh, false);
		return ret;
	}

	/* Apply the per-device max speed before selecting the target. */
	ret = spips_set_frequency(inst(xh), device->max_speed_hz);
	if (ret != 0) {
		(void)ps_async_cleanup(xh, false);
		return ret;
	}

	/* Select native CS or leave native CS inactive when GPIO CS is used. */
	ret = ps_select_device(xh, device);
	if (ret != 0) {
		(void)ps_async_cleanup(xh, false);
		return ret;
	}

	/* GPIO CS must stay asserted until the Xilinx status callback completes. */
	if (device->cs_gpio_num > 0) {
		xh->async.gpio_cs_device = device;
		ret = gpio_cs_assert(device);
		if (ret != 0) {
			(void)ps_async_cleanup(xh, false);
			return ret;
		}
	}

	/* True async path: XSpiPs_Transfer returns immediately; ISR reports done. */
	ret = spips_status_to_errno(XSpiPs_Transfer(inst(xh), tx, rx, len));
	if (ret != 0) {
		(void)ps_async_cleanup(xh, false);
		return ret;
	}

	return 0;
}

/**
 * @brief Run a synchronous SPI command-read operation.
 * @note PS: Command+RX via XSpiPs_PolledTransfer().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_spi_ps_read_command(struct capi_spi_device *device,
				    struct capi_spi_transfer *transfer)
{
	/* CAPI read_command keeps TX command bytes and RX data in one CS frame. */
	if (device == NULL || device->controller == NULL || transfer == NULL)
		return -EINVAL;

	/* The backend rejects optional CAPI features that XSpiPs cannot honor. */
	int ret = ps_validate_transfer(device, transfer, false, true);
	if (ret != 0)
		return ret;

	struct capi_spi_xilinx_handle *xh = device->controller->priv;
	if (xh->async_in_progress || inst(xh)->IsBusy)
		return -EBUSY;

	/* Composite transfer length: command/address TX bytes followed by RX bytes. */
	uint32_t total = (uint32_t)transfer->tx_size + transfer->rx_size;
	if (total == 0)
		return 0;
	if (total > XSPIPS_CAPI_MAX_TRANSFER_SIZE)
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

	XSpiPs_WriteReg(inst(xh)->Config.BaseAddress, XSPIPS_IDR_OFFSET,
			XSPIPS_IXR_DISABLE_ALL_MASK);

	/* Translate CAPI SPI mode into XSpiPs transfer options for this call. */
	u32 options = XSPIPS_MASTER_OPTION | XSPIPS_FORCE_SSELECT_OPTION;
	if (device->mode & CAPI_SPI_CPHA)
		options |= XSPIPS_CLK_PHASE_1_OPTION;
	if (device->mode & CAPI_SPI_CPOL)
		options |= XSPIPS_CLK_ACTIVE_LOW_OPTION;
	ret = spips_status_to_errno(XSpiPs_SetOptions(inst(xh), options));
	if (ret != 0) {
		capi_free(tx);
		capi_free(rx);
		return ret;
	}

	/* Apply the per-device max speed before selecting the target. */
	ret = spips_set_frequency(inst(xh), device->max_speed_hz);
	if (ret != 0) {
		capi_free(tx);
		capi_free(rx);
		return ret;
	}

	/* Select native CS or leave native CS inactive when GPIO CS is used. */
	bool use_gpio_cs = device->cs_gpio_num > 0;
	ret = ps_select_device(xh, device);
	if (ret != 0) {
		capi_free(tx);
		capi_free(rx);
		return ret;
	}

	int status;
	int gpio_error = 0;
	if (use_gpio_cs) {
		/* GPIO CS covers the whole command+read sequence as one frame. */
		ret = gpio_cs_assert(device);
		if (ret != 0) {
			gpio_error = ret;
			status = XST_FAILURE;
		} else {
			status = XSpiPs_PolledTransfer(inst(xh), tx, rx, total);
		}
		ret = gpio_cs_deassert(device);
		if (ret != 0 && status == XST_SUCCESS) {
			gpio_error = ret;
			status = XST_FAILURE;
		}
	} else {
		status = XSpiPs_PolledTransfer(inst(xh), tx, rx, total);
	}
	if (!use_gpio_cs)
		(void)XSpiPs_SetSlaveSelect(inst(xh), XSPIPS_CAPI_DESELECT_ALL_NATIVE);

	/* Copy RX data after the command bytes; earlier RX bytes are protocol echo/dummy. */
	if (status == XST_SUCCESS && transfer->rx_buf && transfer->rx_size > 0)
		memcpy(transfer->rx_buf, rx + transfer->tx_size, transfer->rx_size);

	capi_free(tx);
	capi_free(rx);
	return (gpio_error != 0) ? gpio_error :
	       spips_status_to_errno(status);
}

/**
 * @brief Start an asynchronous SPI command-read operation.
 * @note PS: Async command+RX via XSpiPs_Transfer().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_spi_ps_read_command_async(struct capi_spi_device *device,
		struct capi_spi_transfer *transfer)
{
	/* Async read_command is still one CAPI CS frame, just IRQ-completed. */
	if (device == NULL || device->controller == NULL || transfer == NULL)
		return -EINVAL;

	/* The backend rejects optional CAPI features that XSpiPs cannot honor. */
	int ret = ps_validate_transfer(device, transfer, true, true);
	if (ret != 0)
		return ret;

	/* Composite transfer length: command/address TX bytes followed by RX bytes. */
	uint32_t total32 = (uint32_t)transfer->tx_size + transfer->rx_size;
	if (total32 > XSPIPS_CAPI_MAX_TRANSFER_SIZE)
		return -EINVAL;
	uint16_t total = (uint16_t)total32;
	if (total == 0)
		return 0;

	struct capi_spi_xilinx_handle *xh = device->controller->priv;
	if (!xh->use_irq)
		return -ENOTSUP;
	if (xh->async_in_progress || inst(xh)->IsBusy)
		return -EBUSY;

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

	xh->async.tx_buffer = tx;
	xh->async.rx_buffer = rx;
	xh->async.transfer.rx_buf = transfer->rx_buf;
	xh->async.rx_offset = transfer->tx_size;
	xh->async.transfer.rx_size = transfer->rx_size;
	xh->async_in_progress = true;

	/* Translate CAPI SPI mode into XSpiPs options before starting IRQ transfer. */
	u32 options = XSPIPS_MASTER_OPTION | XSPIPS_FORCE_SSELECT_OPTION;
	if (device->mode & CAPI_SPI_CPHA)
		options |= XSPIPS_CLK_PHASE_1_OPTION;
	if (device->mode & CAPI_SPI_CPOL)
		options |= XSPIPS_CLK_ACTIVE_LOW_OPTION;
	ret = spips_status_to_errno(XSpiPs_SetOptions(inst(xh), options));
	if (ret != 0) {
		(void)ps_async_cleanup(xh, false);
		return ret;
	}

	/* Apply the per-device max speed before selecting the target. */
	ret = spips_set_frequency(inst(xh), device->max_speed_hz);
	if (ret != 0) {
		(void)ps_async_cleanup(xh, false);
		return ret;
	}

	/* Select native CS or leave native CS inactive when GPIO CS is used. */
	ret = ps_select_device(xh, device);
	if (ret != 0) {
		(void)ps_async_cleanup(xh, false);
		return ret;
	}

	/* GPIO CS must stay asserted until the Xilinx status callback completes. */
	if (device->cs_gpio_num > 0) {
		xh->async.gpio_cs_device = device;
		ret = gpio_cs_assert(device);
		if (ret != 0) {
			(void)ps_async_cleanup(xh, false);
			return ret;
		}
	}

	/* True async path: XSpiPs_Transfer returns immediately; ISR reports done. */
	ret = spips_status_to_errno(XSpiPs_Transfer(inst(xh), tx, rx, total));
	if (ret != 0) {
		(void)ps_async_cleanup(xh, false);
		return ret;
	}

	return 0;
}

/**
 * @brief Abort an in-progress asynchronous operation.
 * @note PS: XSpiPs_Abort().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_spi_ps_abort_async(struct capi_spi_device *device)
{
	if (device == NULL || device->controller == NULL)
		return -EINVAL;

	struct capi_spi_xilinx_handle *xh = device->controller->priv;
	if (!xh->async_in_progress)
		return 0;

	XSpiPs_WriteReg(inst(xh)->Config.BaseAddress, XSPIPS_IDR_OFFSET,
			XSPIPS_IXR_DISABLE_ALL_MASK);
	XSpiPs_Abort(inst(xh));
	(void)ps_async_cleanup(xh, false);

	if (xh->callback)
		xh->callback(CAPI_SPI_EVENT_ERROR, xh->callback_arg, 0);

	return 0;
}

/**
 * @brief Register the CAPI asynchronous callback.
 * @note PS: stores the CAPI callback used by the XSpiPs event path.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_spi_ps_register_callback(struct capi_spi_controller_handle
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
 * @note PS: Drives GPIO CS logically or the native SSCTRL field directly.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_spi_ps_set_cs(struct capi_spi_device *device,
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
		return ps_drive_native_cs(xh, device->native_cs, true);
	else if (cs == CAPI_SPI_CS_MANUAL_DEASSERT)
		return ps_drive_native_cs(xh, device->native_cs, false);
	return -EINVAL;
}

/**
 * @brief Dispatch the backend interrupt handler.
 * @note PS: XSpiPs_InterruptHandler().
 *
 */
static void capi_spi_ps_isr(void *handle)
{
	if (handle == NULL)
		return;
	struct capi_spi_xilinx_handle *xh = ((struct capi_spi_controller_handle *)
					     handle)->priv;
	XSpiPs_InterruptHandler(inst(xh));
}
