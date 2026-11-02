/*******************************************************************************
 *   @file   maxim_capi_spi.c
 *   @brief  Implementation of the MAX32650/MAX32690 CAPI SPI functions
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <errno.h>
#include <string.h>
#include "capi_alloc.h"
#include "capi_dma.h"
#if defined(CONFIG_CAPI_GPIO)
#include "capi_gpio.h"
#endif
#include "maxim_capi_dma_priv.h"
#include "maxim_capi_spi.h"
#include "maxim_capi_spi_priv.h"
#include "mxc_device.h"
#include "mxc_errors.h"
#include "mxc_pins.h"
#include "spi.h"

#define MAX_SPI_FIFO_DEPTH	MXC_SPI_FIFO_DEPTH
#define MAX_SPI_DMA_MIN_LEN	32
#define MAX_SPI_TX_THD		16
#define MAX_SPI_TIMEOUT_MS	1000
#define MAX_SPI_DMA_THD		(MAX_SPI_TX_THD << MXC_F_SPI_REVA_DMA_TX_THD_VAL_POS)
#define MAX_SPI_DMA_FIFO_EN	(MAX_SPI_DMA_THD |			\
				 MXC_F_SPI_REVA_DMA_TX_FIFO_EN |	\
				 MXC_F_SPI_REVA_DMA_RX_FIFO_EN)
#define MAX_SPI_DMA_REQ_EN	(MXC_F_SPI_REVA_DMA_DMA_TX_EN |		\
				 MXC_F_SPI_REVA_DMA_DMA_RX_EN)

#define MAX_SPI_LVL(reg, field) \
	(((reg) & MXC_F_SPI_REVA_DMA_##field) >> MXC_F_SPI_REVA_DMA_##field##_POS)

#if TARGET_NUM == 32650
#define MAX_SPI_SS_PINS		4

static const mxc_gpio_cfg_t *const max_spi_pins[MXC_SPI_INSTANCES] = {
	&gpio_cfg_spi0_1, &gpio_cfg_spi1, &gpio_cfg_spi2, &gpio_cfg_spi3,
};

static const mxc_gpio_cfg_t *const
max_spi_ss_pins[MXC_SPI_INSTANCES][MAX_SPI_SS_PINS] = {
	{ &gpio_cfg_spi0_0 },
	{ &gpio_cfg_spi1_ss0, &gpio_cfg_spi1_ss1, &gpio_cfg_spi1_ss2, &gpio_cfg_spi1_ss3 },
	{ &gpio_cfg_spi2_ss0, &gpio_cfg_spi2_ss1, &gpio_cfg_spi2_ss2, &gpio_cfg_spi2_ss3 },
	{ &gpio_cfg_spi3_ss0, &gpio_cfg_spi3_ss1, &gpio_cfg_spi3_ss2, &gpio_cfg_spi3_ss3 },
};

static const mxc_dma_reqsel_t max_spi_rx_req[MXC_SPI_INSTANCES] = {
	MXC_DMA_REQUEST_SPI0RX, MXC_DMA_REQUEST_SPI1RX,
	MXC_DMA_REQUEST_SPI2RX, MXC_DMA_REQUEST_SPI3RX,
};

static const mxc_dma_reqsel_t max_spi_tx_req[MXC_SPI_INSTANCES] = {
	MXC_DMA_REQUEST_SPI0TX, MXC_DMA_REQUEST_SPI1TX,
	MXC_DMA_REQUEST_SPI2TX, MXC_DMA_REQUEST_SPI3TX,
};
#elif TARGET_NUM == 32690
#define MAX_SPI_SS_PINS		3

static const mxc_gpio_cfg_t *const max_spi_pins[MXC_SPI_INSTANCES] = {
	&gpio_cfg_spi0, &gpio_cfg_spi1, &gpio_cfg_spi2, &gpio_cfg_spi3,
	&gpio_cfg_spi4,
};

static const mxc_gpio_cfg_t *const
max_spi_ss_pins[MXC_SPI_INSTANCES][MAX_SPI_SS_PINS] = {
	{ &gpio_cfg_spi0_ss0, &gpio_cfg_spi0_ss1 },
	{ &gpio_cfg_spi1_ss0, &gpio_cfg_spi1_ss1, &gpio_cfg_spi1_ss2 },
	{ &gpio_cfg_spi2_ss0, &gpio_cfg_spi2_ss1, &gpio_cfg_spi2_ss2 },
	{ &gpio_cfg_spi3_ss0, &gpio_cfg_spi3_ss1, &gpio_cfg_spi3_ss2 },
	{ &gpio_cfg_spi4_ss0, &gpio_cfg_spi4_ss1, &gpio_cfg_spi4_ss2 },
};

static const mxc_dma_reqsel_t max_spi_rx_req[MXC_SPI_INSTANCES] = {
	MXC_DMA_REQUEST_SPI0RX, MXC_DMA_REQUEST_SPI1RX, MXC_DMA_REQUEST_SPI2RX,
	MXC_DMA_REQUEST_SPI3RX, MXC_DMA_REQUEST_SPI4RX,
};

static const mxc_dma_reqsel_t max_spi_tx_req[MXC_SPI_INSTANCES] = {
	MXC_DMA_REQUEST_SPI0TX, MXC_DMA_REQUEST_SPI1TX, MXC_DMA_REQUEST_SPI2TX,
	MXC_DMA_REQUEST_SPI3TX, MXC_DMA_REQUEST_SPI4TX,
};
#else
#error "Unsupported target for the Maxim CAPI SPI driver"
#endif

static struct capi_spi_controller_handle *spi_handles[MXC_SPI_INSTANCES];
static const uint8_t max_spi_dma_zero;
static uint8_t max_spi_dma_sink;

static void max_spi_dma_rx_done(uint32_t event, void *ctx);
static void max_spi_dma_error(uint32_t event, void *ctx);

static inline uint32_t max_spi_xfer_len(const struct capi_spi_transfer *xfer)
{
	uint32_t tx_size = xfer->tx_buf ? xfer->tx_size : 0;
	uint32_t rx_size = xfer->rx_buf ? xfer->rx_size : 0;

	return tx_size > rx_size ? tx_size : rx_size;
}

/**
 * @brief Number of polling iterations that take at least timeout_ms.
 * @param timeout_ms - Timeout, 0 or -1 for MAX_SPI_TIMEOUT_MS.
 * @return Iteration limit.
 */
static inline uint32_t max_spi_spin_limit(int timeout_ms)
{
	uint32_t ms = timeout_ms > 0 ? (uint32_t)timeout_ms : MAX_SPI_TIMEOUT_MS;

	return ms * (SystemCoreClock / 8000U);
}

static inline void max_spi_cs_gpio_set(struct capi_spi_device *device,
				       bool active)
{
#if defined(CONFIG_CAPI_GPIO)
	if (device->cs_gpio)
		capi_gpio_pin_set_value(device->cs_gpio,
					active ? CAPI_GPIO_HIGH : CAPI_GPIO_LOW);
#else
	(void)device;
	(void)active;
#endif
}

/**
 * @brief Reset the block, which aborts a transaction and releases SS.
 * @param priv - SPI private data.
 */
static void max_spi_reset(struct max_capi_spi_priv *priv)
{
	mxc_spi_reva_regs_t *spi = priv->regs;
	uint32_t ctrl0 = spi->ctrl0 & ~(MXC_F_SPI_REVA_CTRL0_START |
					MXC_F_SPI_REVA_CTRL0_SS_CTRL);

	spi->dma = MAX_SPI_DMA_THD;
	spi->ctrl0 = ctrl0 & ~MXC_F_SPI_REVA_CTRL0_EN;
	spi->ctrl0 = ctrl0;
	spi->intfl = spi->intfl;
	priv->ss_released = true;
}

/**
 * @brief Prepare the block for a transaction of len bytes in both directions.
 *
 * The block has to be disabled and re-enabled after a transaction that
 * released SS.
 *
 * @param priv - SPI private data.
 * @param len - Number of bytes.
 */
static void max_spi_setup(struct max_capi_spi_priv *priv, uint32_t len)
{
	mxc_spi_reva_regs_t *spi = priv->regs;
	uint32_t ctrl0 = spi->ctrl0 & ~MXC_F_SPI_REVA_CTRL0_START;

	if (priv->ss_released)
		spi->ctrl0 = ctrl0 & ~MXC_F_SPI_REVA_CTRL0_EN;

	spi->ctrl1 = (len << MXC_F_SPI_REVA_CTRL1_TX_NUM_CHAR_POS) |
		     (len << MXC_F_SPI_REVA_CTRL1_RX_NUM_CHAR_POS);
	spi->dma = MAX_SPI_DMA_FIFO_EN | MXC_F_SPI_REVA_DMA_TX_FLUSH |
		   MXC_F_SPI_REVA_DMA_RX_FLUSH;
	spi->ctrl0 = ctrl0 | MXC_F_SPI_REVA_CTRL0_EN;
	spi->intfl = spi->intfl;
}

/**
 * @brief Start the prepared transaction.
 *
 * Writes to CTRL0 never carry START back, which would start the transaction
 * again. SS_CTRL is cleared after START so that a held SS is released at the
 * end of this transaction.
 *
 * @param priv - SPI private data.
 * @param hold - Keep SS asserted after this transaction.
 */
static void max_spi_go(struct max_capi_spi_priv *priv, bool hold)
{
	mxc_spi_reva_regs_t *spi = priv->regs;
	uint32_t ctrl0 = spi->ctrl0 & ~MXC_F_SPI_REVA_CTRL0_START;

	if (hold && !(ctrl0 & MXC_F_SPI_REVA_CTRL0_SS_CTRL)) {
		ctrl0 |= MXC_F_SPI_REVA_CTRL0_SS_CTRL;
		spi->ctrl0 = ctrl0;
	}

	spi->ctrl0 = ctrl0 | MXC_F_SPI_REVA_CTRL0_START;

	if (!hold && (ctrl0 & MXC_F_SPI_REVA_CTRL0_SS_CTRL))
		spi->ctrl0 = ctrl0 & ~MXC_F_SPI_REVA_CTRL0_SS_CTRL;

	priv->ss_released = !hold;
}

/**
 * @brief Wait for the end of the current transaction.
 * @param priv - SPI private data.
 * @param limit - Polling iteration limit.
 * @return 0 on success, -ETIMEDOUT otherwise.
 */
static int max_spi_wait_done(struct max_capi_spi_priv *priv, uint32_t limit)
{
	mxc_spi_reva_regs_t *spi = priv->regs;

	while (!(spi->intfl & MXC_F_SPI_REVA_INTFL_MST_DONE))
		if (!limit--)
			return -ETIMEDOUT;

	spi->intfl = MXC_F_SPI_REVA_INTFL_MST_DONE;

	return 0;
}

/**
 * @brief Full duplex register-level transfer of max(tx_size, rx_size) bytes.
 *
 * At most a FIFO depth of bytes is in flight, so RX cannot overrun. Missing
 * TX bytes are sent as 0, extra RX bytes are discarded.
 *
 * @param priv - SPI private data.
 * @param xfer - Transfer descriptor.
 * @param hold - Keep SS asserted after the transfer.
 * @return 0 on success, negative error code otherwise.
 */
static int max_spi_pio(struct max_capi_spi_priv *priv,
		       const struct capi_spi_transfer *xfer, bool hold)
{
	mxc_spi_reva_regs_t *spi = priv->regs;
	volatile uint8_t *fifo = &spi->fifo8[0];
	const uint8_t *tx = xfer->tx_buf;
	uint8_t *rx = xfer->rx_buf;
	uint32_t tx_size = tx ? xfer->tx_size : 0;
	uint32_t rx_size = rx ? xfer->rx_size : 0;
	uint32_t len = tx_size > rx_size ? tx_size : rx_size;
	uint32_t limit = max_spi_spin_limit(xfer->timeout);
	uint32_t tx_cnt = 0;
	uint32_t rx_cnt = 0;
	uint32_t spins = 0;
	uint32_t lvl;
	uint8_t byte;

	if (!len)
		return 0;

	max_spi_setup(priv, len);

	while (tx_cnt < len && tx_cnt < MAX_SPI_FIFO_DEPTH) {
		*fifo = tx_cnt < tx_size ? tx[tx_cnt] : 0;
		tx_cnt++;
	}

	max_spi_go(priv, hold);

	while (rx_cnt < len) {
		lvl = MAX_SPI_LVL(spi->dma, RX_LVL);
		if (lvl) {
			while (lvl--) {
				byte = *fifo;
				if (rx_cnt < rx_size)
					rx[rx_cnt] = byte;
				rx_cnt++;
			}
			spins = 0;
		} else if (++spins > limit) {
			max_spi_reset(priv);
			return -ETIMEDOUT;
		}

		while (tx_cnt < len && tx_cnt - rx_cnt < MAX_SPI_FIFO_DEPTH) {
			*fifo = tx_cnt < tx_size ? tx[tx_cnt] : 0;
			tx_cnt++;
		}
	}

	if (max_spi_wait_done(priv, limit)) {
		max_spi_reset(priv);
		return -ETIMEDOUT;
	}

	return 0;
}

static inline bool max_spi_dma_fits(const struct max_capi_spi_priv *priv,
				    const struct capi_spi_transfer *xfer)
{
	if (!priv->rx_dma || !priv->tx_dma || !max_spi_xfer_len(xfer))
		return false;

	return !(xfer->tx_buf && xfer->rx_buf && xfer->tx_size != xfer->rx_size);
}

/**
 * @brief Program both DMA channels and start one transfer.
 * @param priv - SPI private data.
 * @param xfer - Transfer descriptor.
 * @return 0 on success, negative error code otherwise.
 */
static int max_spi_dma_kick(struct max_capi_spi_priv *priv,
			    struct capi_spi_transfer *xfer)
{
	mxc_spi_reva_regs_t *spi = priv->regs;
	uint32_t len = max_spi_xfer_len(xfer);
	int ret;

	if (xfer->tx_buf) {
		priv->dma_tx.src = (capi_dma_glbl_addr_t)xfer->tx_buf;
		priv->dma_tx.src_inc = CAPI_DMA_BYTE_INCREMENT;
	} else {
		priv->dma_tx.src = (capi_dma_glbl_addr_t)&max_spi_dma_zero;
		priv->dma_tx.src_inc = CAPI_DMA_NO_INCREMENT;
	}

	if (xfer->rx_buf) {
		priv->dma_rx.dst = (capi_dma_glbl_addr_t)xfer->rx_buf;
		priv->dma_rx.dst_inc = CAPI_DMA_BYTE_INCREMENT;
	} else {
		priv->dma_rx.dst = (capi_dma_glbl_addr_t)&max_spi_dma_sink;
		priv->dma_rx.dst_inc = CAPI_DMA_NO_INCREMENT;
	}

	priv->dma_tx.length = len;
	priv->dma_rx.length = len;

	ret = capi_dma_config_xfer(priv->rx_dma, &priv->dma_rx);
	if (ret)
		return ret;

	ret = capi_dma_config_xfer(priv->tx_dma, &priv->dma_tx);
	if (ret)
		return ret;

	max_spi_setup(priv, len);

	priv->xfer_status = 0;
	priv->xfer_done = false;

	ret = capi_dma_xfer_start(priv->rx_dma);
	if (ret)
		goto err;

	ret = capi_dma_xfer_start(priv->tx_dma);
	if (ret)
		goto err;

	spi->dma = MAX_SPI_DMA_FIFO_EN | MAX_SPI_DMA_REQ_EN;
	max_spi_go(priv, priv->seq_left || priv->cs_hold);

	return 0;

err:
	capi_dma_xfer_abort(priv->rx_dma);
	max_spi_reset(priv);
	priv->xfer_done = true;

	return ret;
}

/**
 * @brief Stop both DMA channels and abort the transaction.
 * @param priv - SPI private data.
 */
static void max_spi_dma_stop(struct max_capi_spi_priv *priv)
{
	priv->regs->dma = MAX_SPI_DMA_THD;
	capi_dma_xfer_abort(priv->tx_dma);
	capi_dma_xfer_abort(priv->rx_dma);
	max_spi_reset(priv);
}

/**
 * @brief End of a DMA transfer, called from the DMA ISR.
 *
 * Starts the next transfer of a sequence with SS held, otherwise reports the
 * result.
 *
 * @param priv - SPI private data.
 * @param status - 0 on success, negative error code otherwise.
 */
static void max_spi_dma_finish(struct max_capi_spi_priv *priv, int status)
{
	struct capi_spi_device *device = priv->xfer_device;

	if (priv->xfer_done)
		return;

	priv->regs->dma = MAX_SPI_DMA_FIFO_EN;

	if (!status)
		status = max_spi_wait_done(priv, max_spi_spin_limit(0));

	if (status) {
		max_spi_dma_stop(priv);
	} else if (priv->seq_left) {
		priv->seq++;
		priv->seq_left--;
		status = max_spi_dma_kick(priv, priv->seq);
		if (!status)
			return;
	}

	if (!priv->cs_hold)
		max_spi_cs_gpio_set(device, false);

	priv->seq_left = 0;
	priv->xfer_status = status;
	priv->xfer_done = true;

	if (priv->xfer_async) {
		priv->async_in_progress = false;
		if (priv->callback)
			priv->callback(status ? CAPI_SPI_EVENT_ERROR :
				       CAPI_SPI_EVENT_XFR_DONE,
				       priv->callback_arg, status);
	}
}

static void max_spi_dma_rx_done(uint32_t event, void *ctx)
{
	(void)event;

	max_spi_dma_finish(ctx, 0);
}

static void max_spi_dma_error(uint32_t event, void *ctx)
{
	(void)event;

	max_spi_dma_finish(ctx, -EIO);
}

/**
 * @brief Start a DMA sequence of one or more transfers under one CS.
 * @param priv - SPI private data.
 * @param device - SPI device.
 * @param xfers - Transfers.
 * @param count - Number of transfers.
 * @param async - Report the end through the CAPI callback.
 * @return 0 on success, negative error code otherwise.
 */
static int max_spi_dma_start_seq(struct max_capi_spi_priv *priv,
				 struct capi_spi_device *device,
				 struct capi_spi_transfer *xfers,
				 uint32_t count, bool async)
{
	int ret;

	priv->xfer_device = device;
	priv->xfer_async = async;
	priv->seq = xfers;
	priv->seq_left = count - 1;

	max_spi_cs_gpio_set(device, true);

	ret = max_spi_dma_kick(priv, xfers);
	if (ret) {
		priv->seq_left = 0;
		if (!priv->cs_hold)
			max_spi_cs_gpio_set(device, false);
	}

	return ret;
}

/**
 * @brief Wait for the in-flight DMA sequence, aborting it on timeout.
 * @param priv - SPI private data.
 * @param timeout - Timeout in ms, 0 or -1 for MAX_SPI_TIMEOUT_MS.
 * @return 0 on success, negative error code otherwise.
 */
static int max_spi_dma_wait(struct max_capi_spi_priv *priv, int timeout)
{
	uint32_t limit = max_spi_spin_limit(timeout);

	while (!priv->xfer_done) {
		if (!limit--) {
			max_spi_dma_stop(priv);
			if (!priv->cs_hold)
				max_spi_cs_gpio_set(priv->xfer_device, false);
			priv->seq_left = 0;
			priv->xfer_done = true;
			return -ETIMEDOUT;
		}
	}

	return priv->xfer_status;
}

/**
 * @brief Run a sequence of PIO transfers under one CS.
 * @param priv - SPI private data.
 * @param device - SPI device.
 * @param xfers - Transfers.
 * @param count - Number of transfers.
 * @return 0 on success, negative error code otherwise.
 */
static int max_spi_pio_seq(struct max_capi_spi_priv *priv,
			   struct capi_spi_device *device,
			   struct capi_spi_transfer *xfers, uint32_t count)
{
	uint32_t i;
	int ret = 0;

	max_spi_cs_gpio_set(device, true);

	for (i = 0; i < count && !ret; i++)
		ret = max_spi_pio(priv, &xfers[i], i + 1 < count || priv->cs_hold);

	if (!ret && !priv->ss_released && !priv->cs_hold)
		max_spi_reset(priv);

	if (!priv->cs_hold)
		max_spi_cs_gpio_set(device, false);

	return ret;
}

/**
 * @brief Apply the device configuration when it differs from the last one.
 * @param priv - SPI private data.
 * @param device - SPI device.
 * @return 0 on success, negative error code otherwise.
 */
static int max_spi_prepare(struct max_capi_spi_priv *priv,
			   struct capi_spi_device *device)
{
	const struct max_capi_spi_device_extra *dev_extra = device->extra;
	mxc_spi_reva_regs_t *spi = priv->regs;
	uint32_t speed = device->max_speed_hz ? device->max_speed_hz :
			 priv->clock_freq;
	uint32_t first = dev_extra ? dev_extra->cs_delay_first : 0;
	uint32_t last = dev_extra ? dev_extra->cs_delay_last : 0;
	uint32_t ticks_per_us, pre, post, ctrl0, pclk, div;

	if (priv->cfg_valid && speed == priv->cfg_speed &&
	    device->mode == priv->cfg_mode && device->native_cs == priv->cfg_cs &&
	    first == priv->cfg_delay_first && last == priv->cfg_delay_last &&
	    spi->clkctrl == priv->cfg_clkctrl && spi->ctrl2 == priv->cfg_ctrl2 &&
	    (spi->ctrl0 & MXC_F_SPI_REVA_CTRL0_SS_ACTIVE) == priv->cfg_ss_active)
		return 0;

	if (device->lsb_first || device->native_cs >= MXC_SPI_SS_INSTANCES)
		return -EINVAL;

	/* The MSDK rounds the divider down, which would exceed speed */
	pclk = MXC_SPI_GetPeripheralClock((mxc_spi_regs_t *)spi);
	div = (pclk + speed - 1) / speed;
	if (div < 2 || MXC_SPI_SetFrequency((mxc_spi_regs_t *)spi,
					    pclk / div) != E_NO_ERROR)
		return -EINVAL;

	if (MXC_SPI_SetMode((mxc_spi_regs_t *)spi,
			    (mxc_spi_mode_t)device->mode) != E_NO_ERROR)
		return -EINVAL;

	ctrl0 = spi->ctrl0 & ~(MXC_F_SPI_REVA_CTRL0_START |
			       MXC_F_SPI_REVA_CTRL0_SS_ACTIVE);
	spi->ctrl0 = ctrl0 | ((1U << device->native_cs) <<
			      MXC_F_SPI_REVA_CTRL0_SS_ACTIVE_POS);

	if (first || last) {
		ticks_per_us = pclk / 1000000U;
		pre = first * ticks_per_us / 1000U;
		post = last * ticks_per_us / 1000U;
		pre = pre ? (pre > 255 ? 255 : pre) : 1;
		post = post ? (post > 255 ? 255 : post) : 1;
		spi->sstime = (spi->sstime & ~(MXC_F_SPI_REVA_SSTIME_PRE |
					       MXC_F_SPI_REVA_SSTIME_POST)) |
			      (pre << MXC_F_SPI_REVA_SSTIME_PRE_POS) |
			      (post << MXC_F_SPI_REVA_SSTIME_POST_POS);
	}

	priv->cfg_speed = speed;
	priv->cfg_mode = device->mode;
	priv->cfg_cs = device->native_cs;
	priv->cfg_delay_first = first;
	priv->cfg_delay_last = last;
	priv->cfg_clkctrl = spi->clkctrl;
	priv->cfg_ctrl2 = spi->ctrl2;
	priv->cfg_ss_active = spi->ctrl0 & MXC_F_SPI_REVA_CTRL0_SS_ACTIVE;
	priv->cfg_valid = true;

	return 0;
}

/**
 * @brief Initialize the SPI block and route its pins.
 * @param id - SPI instance.
 * @param extra - Platform specific configuration, may be NULL.
 * @param freq - Initial SCLK frequency.
 * @return 0 on success, negative error code otherwise.
 */
static int max_spi_hw_init(uint32_t id, const struct max_capi_spi_extra *extra,
			   uint32_t freq)
{
	mxc_spi_regs_t *spi = MXC_SPI_GET_SPI(id);
	uint8_t cs_mask = (extra && extra->chip_select) ? extra->chip_select : 1;
	uint8_t polarity = extra ? extra->polarity_mask : 0;
	mxc_gpio_vssel_t vssel = extra ? (mxc_gpio_vssel_t)extra->vssel :
				 MXC_GPIO_VSSEL_VDDIO;
	mxc_gpio_cfg_t pin;
	uint32_t i;
	int ret;

#if TARGET_NUM == 32650
	ret = MXC_SPI_Init(spi, 1, 0, 1, polarity, freq);
#else
	mxc_spi_pins_t pins = {
		.ss0 = !!(cs_mask & 1),
		.ss1 = !!(cs_mask & 2),
		.ss2 = !!(cs_mask & 4),
		.vddioh = vssel == MXC_GPIO_VSSEL_VDDIOH,
		.clock = true,
		.miso = true,
		.mosi = true,
	};

	ret = MXC_SPI_Init(spi, MXC_SPI_TYPE_CONTROLLER,
			   MXC_SPI_INTERFACE_STANDARD, 1, polarity, freq, pins);
#endif
	if (ret != E_NO_ERROR)
		return -EINVAL;

	pin = *max_spi_pins[id];
	pin.vssel = vssel;
	MXC_GPIO_Config(&pin);

	for (i = 0; i < MAX_SPI_SS_PINS; i++) {
		if (!(cs_mask & (1U << i)))
			continue;

		if (!max_spi_ss_pins[id][i]) {
			ret = -EINVAL;
			goto shutdown;
		}

		pin = *max_spi_ss_pins[id][i];
		pin.vssel = vssel;
		MXC_GPIO_Config(&pin);
	}

	if (MXC_SPI_SetDataSize(spi, 8) != E_NO_ERROR ||
	    MXC_SPI_SetWidth(spi, SPI_WIDTH_STANDARD) != E_NO_ERROR) {
		ret = -EINVAL;
		goto shutdown;
	}

	return 0;

shutdown:
	MXC_SPI_Shutdown(spi);

	return ret;
}

/**
 * @brief Acquire the DMA channels and build the fixed descriptor fields.
 *
 * Completion is taken from the RX channel only; the TX channel only reports
 * errors.
 *
 * @param priv - SPI private data.
 * @param dma - DMA handle.
 * @param extra - Platform specific configuration, may be NULL.
 * @return 0 on success, negative error code otherwise.
 */
static int max_spi_dma_init(struct max_capi_spi_priv *priv,
			    struct capi_dma_handle *dma,
			    const struct max_capi_spi_extra *extra)
{
	const struct max_capi_dma_priv *dma_priv = dma->priv;
	uint32_t rx_id = extra ? extra->rx_dma_ch_id : 0;
	uint32_t tx_id = extra ? extra->tx_dma_ch_id : 1;
	int ret;

	/* Completion comes from the channel interrupts */
	if (!dma_priv || !dma_priv->use_irq)
		return -EINVAL;

	if (rx_id == tx_id) {
		rx_id = 0;
		tx_id = 1;
	}

	ret = capi_dma_init_chan(dma, &priv->rx_dma, rx_id);
	if (ret)
		return ret;

	ret = capi_dma_init_chan(dma, &priv->tx_dma, tx_id);
	if (ret)
		goto free_rx;

	ret = capi_dma_register_complete_callback(priv->rx_dma,
		max_spi_dma_rx_done, priv);
	if (ret)
		goto free_tx;

	ret = capi_dma_register_error_callback(priv->rx_dma, max_spi_dma_error,
					       priv);
	if (ret)
		goto free_tx;

	ret = capi_dma_register_error_callback(priv->tx_dma, max_spi_dma_error,
					       priv);
	if (ret)
		goto free_tx;

	priv->rx_req.reqsel = max_spi_rx_req[priv->id];
	priv->tx_req.reqsel = max_spi_tx_req[priv->id];

	priv->dma_rx = (struct capi_dma_transfer) {
		.src = (capi_dma_glbl_addr_t)&priv->regs->fifo8[0],
		.xfer_type = CAPI_DMA_DEV_TO_MEM,
		.src_inc = CAPI_DMA_NO_INCREMENT,
		.dst_inc = CAPI_DMA_BYTE_INCREMENT,
		.src_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.dst_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.extra = &priv->rx_req,
	};
	priv->dma_tx = (struct capi_dma_transfer) {
		.dst = (capi_dma_glbl_addr_t)&priv->regs->fifo8[0],
		.xfer_type = CAPI_DMA_MEM_TO_DEV,
		.src_inc = CAPI_DMA_BYTE_INCREMENT,
		.dst_inc = CAPI_DMA_NO_INCREMENT,
		.src_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.dst_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.extra = &priv->tx_req,
	};

	return 0;

free_tx:
	capi_dma_deinit_chan(priv->tx_dma);
	priv->tx_dma = NULL;
free_rx:
	capi_dma_deinit_chan(priv->rx_dma);
	priv->rx_dma = NULL;

	return ret;
}

/**
 * @brief Initialize an SPI controller.
 * @param handle - Pointer to the controller handle, allocated if *handle is
 *		   NULL.
 * @param config - Controller configuration. identifier is the SPI instance.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_spi_init(struct capi_spi_controller_handle **handle,
			     const struct capi_spi_config *config)
{
	const struct max_capi_spi_extra *extra;
	struct capi_spi_controller_handle *spi_handle;
	struct max_capi_spi_priv *priv;
	uint32_t id;
	int ret;

	if (!handle || !config)
		return -EINVAL;

	id = (uint32_t)config->identifier;
	if (id >= MXC_SPI_INSTANCES || config->three_pin_mode || config->loopback)
		return -EINVAL;

	if (spi_handles[id])
		return -EBUSY;

	if (!*handle) {
		spi_handle = capi_calloc(1, sizeof(*spi_handle));
		if (!spi_handle)
			return -ENOMEM;

		priv = capi_calloc(1, sizeof(*priv));
		if (!priv) {
			capi_free(spi_handle);
			return -ENOMEM;
		}

		spi_handle->priv = priv;
		spi_handle->init_allocated = true;
	} else {
		spi_handle = *handle;
		if (!spi_handle->priv)
			return -EINVAL;

		priv = spi_handle->priv;
		memset(priv, 0, sizeof(*priv));
		spi_handle->init_allocated = false;
	}

	extra = config->extra;
	priv->id = id;
	priv->regs = (mxc_spi_reva_regs_t *)MXC_SPI_GET_SPI(id);
	priv->clock_freq = config->clk_freq_hz ? config->clk_freq_hz : 1000000;
	priv->dma_min_len = (extra && extra->dma_min_len) ? extra->dma_min_len :
			    MAX_SPI_DMA_MIN_LEN;
	priv->ss_released = true;

	ret = max_spi_hw_init(id, extra, priv->clock_freq);
	if (ret)
		goto free_handle;

	if (config->dma_handle) {
		ret = max_spi_dma_init(priv, config->dma_handle, extra);
		if (ret)
			goto shutdown;
	}

	spi_handle->ops = config->ops;
	spi_handles[id] = spi_handle;
	*handle = spi_handle;

	return 0;

shutdown:
	MXC_SPI_Shutdown(MXC_SPI_GET_SPI(id));
free_handle:
	if (spi_handle->init_allocated) {
		capi_free(priv);
		capi_free(spi_handle);
	}

	return ret;
}

/**
 * @brief Release an SPI controller.
 * @param handle - Controller handle.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_spi_deinit(struct capi_spi_controller_handle *handle)
{
	struct max_capi_spi_priv *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;

	if (priv->rx_dma) {
		max_spi_dma_stop(priv);
		capi_dma_deinit_chan(priv->tx_dma);
		capi_dma_deinit_chan(priv->rx_dma);
	}

	MXC_SPI_Shutdown(MXC_SPI_GET_SPI(priv->id));
	spi_handles[priv->id] = NULL;

	if (handle->init_allocated) {
		capi_free(priv);
		capi_free(handle);
	}

	return 0;
}

/**
 * @brief Run a sequence of transfers under one CS and wait for its end.
 *
 * Sequences of DMA capable transfers with at least dma_min_len bytes in the
 * first one use DMA, the rest the register-level PIO loop.
 */
static int max_spi_xfer_seq(struct capi_spi_device *device,
			    struct capi_spi_transfer *xfers, uint32_t count)
{
	struct max_capi_spi_priv *priv = device->controller->priv;
	uint32_t i;
	int ret;

	if (priv->async_in_progress)
		return -EBUSY;

	ret = max_spi_prepare(priv, device);
	if (ret)
		return ret;

	if (max_spi_xfer_len(&xfers[0]) >= priv->dma_min_len) {
		for (i = 0; i < count; i++)
			if (!max_spi_dma_fits(priv, &xfers[i]))
				break;

		if (i == count) {
			ret = max_spi_dma_start_seq(priv, device, xfers, count,
						    false);
			if (ret)
				return ret;

			return max_spi_dma_wait(priv, xfers[0].timeout);
		}
	}

	return max_spi_pio_seq(priv, device, xfers, count);
}

/**
 * @brief Start a sequence of transfers under one CS without waiting.
 *
 * Without DMA the sequence runs in PIO mode before returning and the
 * callback is called from this context.
 */
static int max_spi_xfer_seq_async(struct capi_spi_device *device,
				  struct capi_spi_transfer *xfers,
				  uint32_t count)
{
	struct max_capi_spi_priv *priv = device->controller->priv;
	uint32_t i;
	int ret;

	if (priv->async_in_progress)
		return -EBUSY;

	ret = max_spi_prepare(priv, device);
	if (ret)
		return ret;

	for (i = 0; i < count; i++)
		if (!max_spi_dma_fits(priv, &xfers[i]))
			break;

	if (i == count) {
		priv->async_in_progress = true;
		ret = max_spi_dma_start_seq(priv, device, xfers, count, true);
		if (ret)
			priv->async_in_progress = false;

		return ret;
	}

	ret = max_spi_pio_seq(priv, device, xfers, count);
	if (priv->callback)
		priv->callback(ret ? CAPI_SPI_EVENT_ERROR : CAPI_SPI_EVENT_XFR_DONE,
			       priv->callback_arg, ret);

	return ret;
}

static int max_capi_spi_transceive(struct capi_spi_device *device,
				   struct capi_spi_transfer *transfer)
{
	if (!device || !device->controller || !transfer)
		return -EINVAL;

	return max_spi_xfer_seq(device, transfer, 1);
}

static int max_capi_spi_transceive_async(struct capi_spi_device *device,
		struct capi_spi_transfer *transfer,
		int timeout)
{
	(void)timeout;

	if (!device || !device->controller || !transfer)
		return -EINVAL;

	return max_spi_xfer_seq_async(device, transfer, 1);
}

/**
 * @brief Split a read command into its TX and RX phases.
 * @param priv - SPI private data.
 * @param transfer - Read command: tx_buf is sent, then rx_size bytes are read.
 * @return Number of transfers in priv->cmd_seq.
 */
static uint32_t max_spi_read_cmd_seq(struct max_capi_spi_priv *priv,
				     const struct capi_spi_transfer *transfer)
{
	priv->cmd_seq[0] = (struct capi_spi_transfer) {
		.tx_buf = transfer->tx_buf,
		.tx_size = transfer->tx_size,
		.timeout = transfer->timeout,
	};
	priv->cmd_seq[1] = (struct capi_spi_transfer) {
		.rx_buf = transfer->rx_buf,
		.rx_size = transfer->rx_size,
		.timeout = transfer->timeout,
	};

	return 2;
}

static int max_capi_spi_read_command(struct capi_spi_device *device,
				     struct capi_spi_transfer *transfer)
{
	struct max_capi_spi_priv *priv;
	uint32_t count;

	if (!device || !device->controller || !transfer || !transfer->rx_buf)
		return -EINVAL;

	priv = device->controller->priv;
	if (priv->async_in_progress)
		return -EBUSY;

	count = max_spi_read_cmd_seq(priv, transfer);

	return max_spi_xfer_seq(device, priv->cmd_seq, count);
}

static int max_capi_spi_read_command_async(struct capi_spi_device *device,
		struct capi_spi_transfer *transfer)
{
	struct max_capi_spi_priv *priv;
	uint32_t count;

	if (!device || !device->controller || !transfer || !transfer->rx_buf)
		return -EINVAL;

	priv = device->controller->priv;
	if (priv->async_in_progress)
		return -EBUSY;

	count = max_spi_read_cmd_seq(priv, transfer);

	return max_spi_xfer_seq_async(device, priv->cmd_seq, count);
}

static int max_capi_spi_abort_async(struct capi_spi_device *device)
{
	struct max_capi_spi_priv *priv;

	if (!device || !device->controller || !device->controller->priv)
		return -EINVAL;

	priv = device->controller->priv;
	if (!priv->async_in_progress)
		return 0;

	if (priv->rx_dma)
		max_spi_dma_stop(priv);
	else
		max_spi_reset(priv);

	if (!priv->cs_hold)
		max_spi_cs_gpio_set(device, false);

	priv->seq_left = 0;
	priv->xfer_status = -ECANCELED;
	priv->xfer_done = true;
	priv->async_in_progress = false;

	return 0;
}

static int max_capi_spi_register_callback(struct capi_spi_controller_handle
		*handle,
		capi_spi_callback_t const callback,
		void *callback_arg)
{
	struct max_capi_spi_priv *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;
	priv->callback = callback;
	priv->callback_arg = callback_arg;

	return 0;
}

/**
 * @brief Manual CS control.
 *
 * The hardware target select is only driven during a transaction, so a
 * manual assert keeps it asserted from the next transfer on, until a manual
 * deassert or CAPI_SPI_CS_AUTO.
 */
static int max_capi_spi_set_cs(struct capi_spi_device *device,
			       enum capi_spi_cs_control cs_control)
{
	struct max_capi_spi_priv *priv;

	if (!device || !device->controller || !device->controller->priv)
		return -EINVAL;

	priv = device->controller->priv;
	if (priv->async_in_progress)
		return -EBUSY;

	switch (cs_control) {
	case CAPI_SPI_CS_MANUAL_ASSERT:
		priv->cs_hold = true;
		max_spi_cs_gpio_set(device, true);
		break;
	case CAPI_SPI_CS_MANUAL_DEASSERT:
	case CAPI_SPI_CS_AUTO:
		priv->cs_hold = false;
		if (!priv->ss_released)
			max_spi_reset(priv);
		max_spi_cs_gpio_set(device, false);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static void max_capi_spi_isr(void *handle)
{
	(void)handle;
}

const struct capi_spi_ops max_capi_spi_ops = {
	.init = max_capi_spi_init,
	.deinit = max_capi_spi_deinit,
	.transceive = max_capi_spi_transceive,
	.transceive_async = max_capi_spi_transceive_async,
	.read_command = max_capi_spi_read_command,
	.read_command_async = max_capi_spi_read_command_async,
	.abort_async = max_capi_spi_abort_async,
	.register_callback = max_capi_spi_register_callback,
	.set_cs = max_capi_spi_set_cs,
	.isr = max_capi_spi_isr,
};
