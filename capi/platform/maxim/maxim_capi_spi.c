/*
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file maxim_capi_spi.c
 * @brief Common Maxim (MSDK) SPI backend.
 *
 * One backend source for every MSDK part. Written against the MSDK public SPI
 * API; the only per-generation difference (the MXC_SPI_Init() signature) is
 * isolated to _max_spi_hw_init() and gated on MAX_CAPI_SPI_V2. Controller mode
 * (blocking transceive, async transceive, blocking read_command) is fully
 * supported; async read_command is a documented follow-up (returns -ENOSYS).
 */

#include <errno.h>
#include <string.h>
#include "capi_spi.h"
#include "capi_irq.h"
#include "capi_alloc.h"
#include "maxim_capi_spi.h"
#include "maxim_capi_spi_priv.h"
#include "maxim_capi_irq.h"
#include "spi.h"

/** Static variables **********************************************************/

static struct capi_spi_controller_handle *spi[MXC_SPI_INSTANCES] = {NULL};

/** Forward declarations ******************************************************/

void max_capi_spi_isr(void *handle);

/** Helpers *******************************************************************/

/**
 * @brief Resolve the config identifier to an MSDK register block.
 *
 * The CAPI contract documents SPI identifier as a base address for internal
 * controllers; some harnesses instead pass the small instance index. Accept
 * both: a value smaller than the instance count is treated as an index.
 */
static mxc_spi_regs_t *_max_spi_regs_from_id(uint64_t identifier)
{
	if (identifier < MXC_SPI_INSTANCES)
		return MXC_SPI_GET_SPI((int)identifier);

	return (mxc_spi_regs_t *)(uintptr_t)identifier;
}

/**
 * @brief Translate an MSDK return code into a CAPI/errno code.
 */
static int _max_capi_spi_err(int msdk_ret)
{
	if (msdk_ret == E_NO_ERROR || msdk_ret == E_SUCCESS)
		return 0;
	if (msdk_ret == E_TIME_OUT)
		return -ETIMEDOUT;
	if (msdk_ret == E_BUSY)
		return -EBUSY;

	return -EIO;
}

/**
 * @brief Chip-select index from a native-CS bitmask (lowest set bit).
 */
static int _max_capi_spi_cs_index(uint16_t native_cs)
{
	if (native_cs == 0)
		return 0;

	return __builtin_ctz(native_cs);
}

/**
 * @brief The one signature shim: bring the SPI block up in controller mode.
 *
 * This is the only place the v2-vs-reva MXC_SPI_Init() difference lives.
 */
static int _max_spi_hw_init(struct max_capi_spi_priv *priv)
{
	struct max_capi_spi_extra *extra = &priv->extra;
	bool quad = (extra->bus_width == MAX_CAPI_SPI_BUS_WIDTH_QUAD);
	int num_targets = extra->num_targets ? (int)extra->num_targets : 1;

#if MAX_CAPI_SPI_HAS_PINS
	mxc_spi_pins_t pins = {
		.clock = true,
		.miso = true,
		.mosi = true,
		.sdio2 = quad,
		.sdio3 = quad,
		.ss0 = (extra->chip_select & MAX_CAPI_SPI_CS0) != 0,
		.ss1 = (extra->chip_select & MAX_CAPI_SPI_CS1) != 0,
		.ss2 = (extra->chip_select & MAX_CAPI_SPI_CS2) != 0,
		.vddioh = (extra->vssel == MAX_CAPI_GPIO_VSSEL_VDDIOH),
	};

	/* Default to CS0 if the caller left the bitmask empty. */
	if (extra->chip_select == 0)
		pins.ss0 = true;
#endif

#if MAX_CAPI_SPI_V2
	{
		mxc_spi_type_t type =
			(extra->device_role == MAX_CAPI_SPI_DEVICE_ROLE_TARGET) ?
			MXC_SPI_TYPE_TARGET : MXC_SPI_TYPE_CONTROLLER;
		mxc_spi_interface_t if_mode;

		switch (extra->bus_width) {
		case MAX_CAPI_SPI_BUS_WIDTH_QUAD:
			if_mode = MXC_SPI_INTERFACE_QUAD;
			break;
		case MAX_CAPI_SPI_BUS_WIDTH_DUAL:
			if_mode = MXC_SPI_INTERFACE_DUAL;
			break;
		case MAX_CAPI_SPI_BUS_WIDTH_3WIRE:
			if_mode = MXC_SPI_INTERFACE_3WIRE;
			break;
		default:
			if_mode = MXC_SPI_INTERFACE_STANDARD;
			break;
		}

		return MXC_SPI_Init(priv->regs, type, if_mode, num_targets,
				    extra->polarity_mask, priv->clock_freq, pins);
	}
#else
	{
		int master_mode =
			(extra->device_role == MAX_CAPI_SPI_DEVICE_ROLE_TARGET) ? 0 : 1;

#if MAX_CAPI_SPI_HAS_PINS
		return MXC_SPI_Init(priv->regs, master_mode, quad ? 1 : 0,
				    num_targets, extra->polarity_mask,
				    priv->clock_freq, pins);
#else
		/* me11-class: pinless 6-arg Init; the driver muxes its own pins. */
		return MXC_SPI_Init(priv->regs, master_mode, quad ? 1 : 0,
				    num_targets, extra->polarity_mask,
				    priv->clock_freq);
#endif
	}
#endif
}

/**
 * @brief MSDK async completion trampoline; maps back to the CAPI callback.
 */
static void _max_capi_spi_complete_cb(void *req_ptr, int result)
{
	mxc_spi_req_t *req = req_ptr;
	int idx;
	struct capi_spi_controller_handle *handle;
	struct max_capi_spi_priv *priv;
	capi_spi_callback_t callback;
	void *callback_arg;
	enum capi_async_event event;

	if (!req)
		return;

	idx = MXC_SPI_GET_IDX(req->spi);
	if (idx < 0 || idx >= (int)MXC_SPI_INSTANCES)
		return;

	handle = spi[idx];
	if (!handle || !handle->priv)
		return;

	priv = handle->priv;
	priv->async_in_progress = false;

	/* Async transfer done: drop the NVIC line so a later blocking
	 * transaction is not raced by the ISR (see init). */
	capi_irq_disable(MXC_SPI_GET_IRQ(idx));

	/* Release any TX-pad allocated for an async read_command RX phase. */
	if (priv->async_tx_pad) {
		capi_free(priv->async_tx_pad);
		priv->async_tx_pad = NULL;
	}

	event = (result == E_NO_ERROR || result == E_SUCCESS) ?
		CAPI_SPI_EVENT_XFR_DONE : CAPI_SPI_EVENT_ERROR;

	callback = priv->callback;
	callback_arg = priv->callback_arg;
	if (callback)
		callback(event, callback_arg, result);
}

/**
 * @brief Populate an mxc_spi_req_t common to blocking and async paths.
 */
static void _max_capi_spi_build_req(struct max_capi_spi_priv *priv,
				    struct capi_spi_device *device,
				    struct capi_spi_transfer *transfer,
				    mxc_spi_req_t *req, int ss_deassert,
				    spi_complete_cb_t cb)
{
	*req = (mxc_spi_req_t) {
		.spi = priv->regs,
		.ssIdx = _max_capi_spi_cs_index(device->native_cs),
		.ssDeassert = ss_deassert,
		.txData = (uint8_t *)transfer->tx_buf,
		.rxData = transfer->rx_buf,
		.txLen = transfer->tx_size,
		.rxLen = transfer->rx_size,
		.completeCB = cb,
	};
#if MAX_CAPI_SPI_V2
	req->txDummyValue = 0;
#endif
}

/**
 * @brief Apply per-device mode/frequency before a transaction.
 */
static int _max_capi_spi_apply_device(struct max_capi_spi_priv *priv,
				      struct capi_spi_device *device)
{
	uint32_t hz = device->max_speed_hz ? device->max_speed_hz : priv->clock_freq;
	int ret;

	ret = MXC_SPI_SetMode(priv->regs, (mxc_spi_mode_t)device->mode);
	if (ret < 0)
		return -EINVAL;

	if (hz) {
		ret = MXC_SPI_SetFrequency(priv->regs, hz);
		if (ret < 0)
			return -EINVAL;
	}

	return 0;
}

/** SPI functions implementation **********************************************/

/**
 * @brief Initialize the SPI peripheral.
 */
int max_capi_spi_init(struct capi_spi_controller_handle **handle,
		      const struct capi_spi_config *config)
{
	int ret, idx;
	struct capi_spi_controller_handle *spi_handle;
	struct max_capi_spi_priv *priv;
	mxc_spi_regs_t *regs;
	IRQn_Type irq;

	if (!handle || !config || !config->extra)
		return -EINVAL;

	regs = _max_spi_regs_from_id(config->identifier);
	idx = MXC_SPI_GET_IDX(regs);
	if (idx < 0 || idx >= (int)MXC_SPI_INSTANCES)
		return -EINVAL;

	/* Re-init of a live instance returns the existing handle. */
	if (spi[idx] != NULL) {
		*handle = spi[idx];
		return 0;
	}

	if (*handle == NULL) {
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
		spi_handle->init_allocated = false;
	}

	spi_handle->ops = config->ops;
	priv->spi_handle = spi_handle;
	priv->regs = regs;
	priv->identifier = (uint32_t)idx;
	priv->extra = *(struct max_capi_spi_extra *)config->extra;
	priv->dma_handle = config->dma_handle;
	priv->clock_freq = config->clk_freq_hz ? config->clk_freq_hz : 1000000;

	/* Reset transient async state in case the caller reused a dirty handle. */
	priv->async_in_progress = false;
	priv->async_tx_pad = NULL;
	priv->cs_hold = false;

	ret = _max_spi_hw_init(priv);
	if (ret < 0) {
		ret = _max_capi_spi_err(ret);
		goto free_handle;
	}

	/*
	 * CAPI SPI transfers are byte-oriented (uint8_t buffers), so the frame
	 * MUST be 8 bits. On reva the CTRL2 NUMBITS field resets to 0, which
	 * MXC_SPI_GetDataSize() reports as *16* -- and the reva blocking engine
	 * then drains 2x the requested rx count from the FIFO (rx_length =
	 * rxLen * 2), writing past the caller's rx buffer and smashing the
	 * stack. MXC_SPI_Init does not set this, so pin it to 8 here.
	 */
	MXC_SPI_SetDataSize(priv->regs, 8);

	/*
	 * Register the ISR but leave the NVIC line DISABLED. The blocking
	 * MXC_SPI_MasterTransaction is fully self-polling: it spins on
	 * MasterTransHandler and MST_DONE while the block's own inten bits are
	 * set. If the NVIC SPI line were enabled, MXC_SPI_AsyncHandler would run
	 * on the same transfer -- it clears inten and intfl (including MST_DONE)
	 * and drains the RX FIFO -- stealing the blocking read (stale 0xFF) and
	 * hanging the MST_DONE poll. The line is enabled only around an async
	 * transfer (see transceive_async) and disabled again on completion/abort.
	 */
	irq = MXC_SPI_GET_IRQ(idx);
	ret = capi_irq_connect(irq, max_capi_spi_isr, spi_handle);
	if (ret)
		goto shutdown;

	spi[idx] = spi_handle;
	*handle = spi_handle;

	return 0;

shutdown:
	MXC_SPI_Shutdown(regs);
free_handle:
	if (spi_handle->init_allocated) {
		capi_free(priv);
		capi_free(spi_handle);
	}

	return ret;
}

/**
 * @brief Deinitialize the SPI peripheral.
 */
int max_capi_spi_deinit(struct capi_spi_controller_handle *handle)
{
	int ret;
	struct max_capi_spi_priv *priv;
	uint32_t idx;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;
	idx = priv->identifier;

	MXC_SPI_AbortAsync(priv->regs);
	capi_irq_disable(MXC_SPI_GET_IRQ(idx));

	ret = MXC_SPI_Shutdown(priv->regs);

	if (handle->init_allocated) {
		capi_free(priv);
		capi_free(handle);
	}

	spi[idx] = NULL;

	return _max_capi_spi_err(ret);
}

/**
 * @brief Blocking full-duplex transceive.
 */
int max_capi_spi_transceive(struct capi_spi_device *device,
			    struct capi_spi_transfer *transfer)
{
	struct max_capi_spi_priv *priv;
	mxc_spi_req_t req;
	int ret;

	if (!device || !device->controller || !device->controller->priv || !transfer)
		return -EINVAL;

	priv = device->controller->priv;

	ret = _max_capi_spi_apply_device(priv, device);
	if (ret)
		return ret;

	/*
	 * When CS is under manual control and currently asserted, keep it asserted
	 * across the transfer (ssDeassert = 0); set_cs(MANUAL_DEASSERT) releases it.
	 * Otherwise CS is auto-framed per transaction (ssDeassert = 1).
	 */
	_max_capi_spi_build_req(priv, device, transfer, &req,
				priv->cs_hold ? 0 : 1, NULL);

	/*
	 * reva full-duplex clocks TX_NUM_CHAR characters; RX_NUM_CHAR only sets
	 * the receive threshold. When rx is LONGER than tx (both buffers present),
	 * the controller under-clocks: it stops after tx_size characters, so the
	 * self-polling MXC_SPI_MasterTransaction spins forever waiting for the
	 * remaining RX bytes -> hang. Pad TX up to rx_size with dummy (0x00) bytes
	 * so enough clocks are generated; the extra RX bytes are the caller's to
	 * ignore. (tx==NULL is handled by the MSDK itself; tx>=rx needs nothing.)
	 */
	uint8_t *tx_pad = NULL;
	uint8_t *rx_pad = NULL;

	if (transfer->tx_buf && transfer->rx_buf &&
	    transfer->tx_size < transfer->rx_size) {
		tx_pad = capi_calloc(1, transfer->rx_size);
		if (!tx_pad)
			return -ENOMEM;

		memcpy(tx_pad, transfer->tx_buf, transfer->tx_size);
		req.txData = tx_pad;
		req.txLen = transfer->rx_size;
	}

	/*
	 * Mirror case: when tx is LONGER than rx, reva clocks tx_size characters
	 * and drains ALL of them from the RX FIFO -- writing past the caller's
	 * rx_size and clobbering whatever follows rx_buf. Give reva a scratch RX
	 * buffer sized to the clock count, then copy back only the requested
	 * rx_size bytes so the caller's buffer is never overrun.
	 */
	if (transfer->tx_buf && transfer->rx_buf &&
	    transfer->tx_size > transfer->rx_size) {
		rx_pad = capi_calloc(1, transfer->tx_size);
		if (!rx_pad) {
			if (tx_pad)
				capi_free(tx_pad);
			return -ENOMEM;
		}

		req.rxData = rx_pad;
		req.rxLen = transfer->tx_size;
	}

	ret = MXC_SPI_MasterTransaction(&req);

	if (rx_pad) {
		memcpy(transfer->rx_buf, rx_pad, transfer->rx_size);
		capi_free(rx_pad);
	}
	if (tx_pad)
		capi_free(tx_pad);

	return _max_capi_spi_err(ret);
}

/**
 * @brief Async full-duplex transceive.
 */
int max_capi_spi_transceive_async(struct capi_spi_device *device,
				  struct capi_spi_transfer *transfer,
				  int timeout)
{
	struct max_capi_spi_priv *priv;
	int ret;

	(void)timeout;

	if (!device || !device->controller || !device->controller->priv || !transfer)
		return -EINVAL;

	priv = device->controller->priv;

	if (priv->async_in_progress)
		return -EBUSY;

	ret = _max_capi_spi_apply_device(priv, device);
	if (ret)
		return ret;

	_max_capi_spi_build_req(priv, device, transfer, &priv->async_req, 1,
				_max_capi_spi_complete_cb);

	priv->async_in_progress = true;

	/* Arm the NVIC line only for the async transfer (see init). */
	capi_irq_enable(MXC_SPI_GET_IRQ(priv->identifier));

	ret = MXC_SPI_MasterTransactionAsync(&priv->async_req);
	if (ret != E_NO_ERROR) {
		capi_irq_disable(MXC_SPI_GET_IRQ(priv->identifier));
		priv->async_in_progress = false;
		return _max_capi_spi_err(ret);
	}

	return 0;
}

/**
 * @brief Blocking command-then-read within a single CS frame.
 *
 * Tx phase holds CS asserted (ssDeassert = 0); Rx phase releases it
 * (ssDeassert = 1). A zero-length Rx degenerates into a plain write.
 */
int max_capi_spi_read_command(struct capi_spi_device *device,
			      struct capi_spi_transfer *transfer)
{
	struct max_capi_spi_priv *priv;
	mxc_spi_req_t req;
	int ret;

	if (!device || !device->controller || !device->controller->priv || !transfer)
		return -EINVAL;

	priv = device->controller->priv;

	ret = _max_capi_spi_apply_device(priv, device);
	if (ret)
		return ret;

	/* Tx phase: send command/address, keep CS asserted. */
	if (transfer->tx_size) {
		_max_capi_spi_build_req(priv, device, transfer, &req,
					transfer->rx_size ? 0 : 1, NULL);
		req.rxData = NULL;
		req.rxLen = 0;

		ret = MXC_SPI_MasterTransaction(&req);
		if (ret != E_NO_ERROR)
			return _max_capi_spi_err(ret);
	}

	/* Rx phase: read the response, release CS at the end. */
	if (transfer->rx_size) {
		_max_capi_spi_build_req(priv, device, transfer, &req, 1, NULL);
		req.txData = NULL;
		req.txLen = 0;

		ret = MXC_SPI_MasterTransaction(&req);
		if (ret != E_NO_ERROR)
			return _max_capi_spi_err(ret);
	}

	return 0;
}

/**
 * @brief Async command-then-read within a single CS frame.
 *
 * The MSDK async engine drives one request, so the command (Tx) phase is issued
 * blocking with CS held (ssDeassert = 0, ISR line still down), then the response
 * (Rx) phase is armed async and delivers the completion callback. A zero-length
 * Rx degenerates into a blocking write with a synchronous XFR_DONE callback.
 *
 * reva under-clocks a tx-less transfer (it clocks txLen characters and only
 * thresholds on rxLen), so the Rx phase is given a zero-filled TX-pad sized to
 * rx_size -- mirroring the blocking transceive fix. The pad is owned by priv for
 * the async lifetime and freed by the completion trampoline / abort path.
 */
int max_capi_spi_read_command_async(struct capi_spi_device *device,
				    struct capi_spi_transfer *transfer)
{
	struct max_capi_spi_priv *priv;
	capi_spi_callback_t callback;
	void *callback_arg;
	mxc_spi_req_t tx_req;
	uint8_t *tx_pad;
	int ret;

	if (!device || !device->controller || !device->controller->priv || !transfer)
		return -EINVAL;

	priv = device->controller->priv;

	if (priv->async_in_progress)
		return -EBUSY;

	ret = _max_capi_spi_apply_device(priv, device);
	if (ret)
		return ret;

	/* Tx phase: send command/address blocking, keep CS asserted if a read
	 * follows. Runs with the ISR line disabled -- see init. */
	if (transfer->tx_size) {
		_max_capi_spi_build_req(priv, device, transfer, &tx_req,
					transfer->rx_size ? 0 : 1, NULL);
		tx_req.rxData = NULL;
		tx_req.rxLen = 0;

		ret = MXC_SPI_MasterTransaction(&tx_req);
		if (ret != E_NO_ERROR)
			return _max_capi_spi_err(ret);
	}

	/* No read requested: the write is already done. Deliver the terminal
	 * callback synchronously so the caller's completion contract holds. */
	if (!transfer->rx_size) {
		callback = priv->callback;
		callback_arg = priv->callback_arg;
		if (callback)
			callback(CAPI_SPI_EVENT_XFR_DONE, callback_arg, 0);
		return 0;
	}

	/* Rx phase: TX-pad of zeros so reva generates rx_size clocks; the pad
	 * lives until completion/abort frees it. */
	tx_pad = capi_calloc(1, transfer->rx_size);
	if (!tx_pad)
		return -ENOMEM;

	_max_capi_spi_build_req(priv, device, transfer, &priv->async_req, 1,
				_max_capi_spi_complete_cb);
	priv->async_req.txData = tx_pad;
	priv->async_req.txLen = transfer->rx_size;
	priv->async_tx_pad = tx_pad;

	priv->async_in_progress = true;

	/* Arm the NVIC line only for the async transfer (see init). */
	capi_irq_enable(MXC_SPI_GET_IRQ(priv->identifier));

	ret = MXC_SPI_MasterTransactionAsync(&priv->async_req);
	if (ret != E_NO_ERROR) {
		capi_irq_disable(MXC_SPI_GET_IRQ(priv->identifier));
		priv->async_in_progress = false;
		priv->async_tx_pad = NULL;
		capi_free(tx_pad);
		return _max_capi_spi_err(ret);
	}

	return 0;
}

/**
 * @brief Abort an in-flight async transfer.
 */
int max_capi_spi_abort_async(struct capi_spi_device *device)
{
	struct max_capi_spi_priv *priv;
	capi_spi_callback_t callback;
	void *callback_arg;
	bool was_active;

	if (!device || !device->controller || !device->controller->priv)
		return -EINVAL;

	priv = device->controller->priv;

	was_active = priv->async_in_progress;

	MXC_SPI_AbortAsync(priv->regs);
	capi_irq_disable(MXC_SPI_GET_IRQ(priv->identifier));
	priv->async_in_progress = false;

	/* Release any TX-pad from an aborted async read_command RX phase. */
	if (priv->async_tx_pad) {
		capi_free(priv->async_tx_pad);
		priv->async_tx_pad = NULL;
	}

	/*
	 * The CAPI abort contract requires a single terminal callback for the
	 * transfer that was torn down. MXC_SPI_AbortAsync does NOT invoke the
	 * registered completeCB, so deliver it here. Guard on was_active so an
	 * abort with nothing in flight (or after the ISR already completed the
	 * transfer) does not emit a spurious second callback. extra=0: the abort
	 * is a clean, caller-initiated teardown, not a hardware error code.
	 */
	if (was_active) {
		callback = priv->callback;
		callback_arg = priv->callback_arg;
		if (callback)
			callback(CAPI_SPI_EVENT_ERROR, callback_arg, 0);
	}

	return 0;
}

/**
 * @brief Register the async completion callback.
 */
int max_capi_spi_register_callback(struct capi_spi_controller_handle *handle,
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
 * @brief Chip-select control.
 *
 * The MSDK drives CS automatically for API-level transactions, so AUTO is a
 * no-op success. Manual assert/deassert is not exposed by the public API
 * without register pokes and is a documented follow-up.
 */
int max_capi_spi_set_cs(struct capi_spi_device *device,
			enum capi_spi_cs_control cs_control)
{
	if (!device || !device->controller || !device->controller->priv)
		return -EINVAL;

	struct max_capi_spi_priv *priv = device->controller->priv;

	switch (cs_control) {
	case CAPI_SPI_CS_AUTO:
		/* Return to per-transaction HW framing. */
		priv->cs_hold = false;
		return 0;

	case CAPI_SPI_CS_MANUAL_ASSERT:
		/* Hold CS asserted across the following transfer(s). */
		priv->cs_hold = true;
		return 0;

	case CAPI_SPI_CS_MANUAL_DEASSERT:
		/* Release a held CS: clear the SS-control/START bits reva latched
		 * during the ssDeassert = 0 transaction. */
		priv->cs_hold = false;
		priv->regs->ctrl0 &= ~(MXC_F_SPI_CTRL0_SS_CTRL | MXC_F_SPI_CTRL0_START);
		return 0;

	default:
		return -ENOSYS;
	}
}

/**
 * @brief Interrupt entry point; pumps the MSDK async engine.
 */
void max_capi_spi_isr(void *handle)
{
	struct capi_spi_controller_handle *spi_handle;
	struct max_capi_spi_priv *priv;

	if (!handle)
		return;

	spi_handle = (struct capi_spi_controller_handle *)handle;
	if (!spi_handle->priv)
		return;

	priv = spi_handle->priv;

	MXC_SPI_AsyncHandler(priv->regs);
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
