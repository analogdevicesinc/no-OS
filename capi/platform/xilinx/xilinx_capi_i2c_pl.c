/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Xilinx XIic AXI IIC driver
 *
 * SDT BSP only. IRQ is explicit via capi_i2c_xilinx_config.
 * capi_irq_init() must be called before async ops will work.
 */

#include <capi_i2c.h>
#include <xilinx_capi_i2c_priv.h>
#include <capi_irq.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <capi_alloc.h>
#include <capi_time.h>
#include <string.h>
#include "xinterrupt_wrap.h"

#ifdef XPAR_XIIC_NUM_INSTANCES

#define I2C_PL_7BIT_MAX_ADDR		0x7FU
#define I2C_PL_10BIT_MAX_ADDR		0x3FEU
#define I2C_PL_BUS_BUSY_TIMEOUT		100000U

/* Bits clocked per byte on the wire: 8 data bits + 1 ACK/NACK. The address
 * phase costs the same, so we bill it as one extra byte (the "+ 1" below). */
#define I2C_PL_BITS_PER_BYTE		9U
/* Safety multiplier over the ideal transfer time: covers START/STOP, repeated
 * START, ISR latency, and clock stretching by a slow target. */
#define I2C_PL_TIMEOUT_SAFETY		3U
/* Fall back to 100 kHz (standard mode) if the handle has no SCL rate. */
#define I2C_PL_DEFAULT_SCL_HZ		100000U

/* Defaults for designs with nonstandard THIGH/TLOW registers. */
#define I2C_PL_DEFAULT_THIGH_OFFSET	0x13CU
#define I2C_PL_DEFAULT_TLOW_OFFSET	0x140U
#define I2C_PL_DEFAULT_DUTY_PERCENT	50U
#define I2C_PL_MIN_DUTY_PERCENT		10U
#define I2C_PL_MAX_DUTY_PERCENT		90U

/*
 * Frame size a bounced target receive is armed for (see the target branch of
 * capi_i2c_pl_receive_async). Unlike XIicPs, whose bounce ceiling is the
 * hardware's own XIICPS_MAX_TRANSFER_SIZE, the AXI IIC imposes no per-frame
 * limit -- an initiator may clock indefinitely -- so this is a driver-chosen
 * cap on how much overrun the target will silently absorb, not a hardware
 * property. An initiator sending MORE than this still gets NO_ACK once the
 * bounce buffer is exhausted, which is the correct I2C response to a receiver
 * that is genuinely out of room; the cap only decides where that point is.
 * 256 covers a byte-length-prefixed frame with room to spare.
 */
#define XIIC_TARGET_RX_ABSORB_LEN	256U

static int capi_i2c_pl_init(struct capi_i2c_controller_handle **handle,
			    const struct capi_i2c_config *config);
static int capi_i2c_pl_deinit(struct capi_i2c_controller_handle *handle);
static int capi_i2c_pl_transmit(struct capi_i2c_device *device,
				struct capi_i2c_transfer *transfer);
static int capi_i2c_pl_receive(struct capi_i2c_device *device,
			       struct capi_i2c_transfer *transfer);
static int capi_i2c_pl_register_callback(struct capi_i2c_controller_handle
		*handle,
		capi_i2c_callback const callback,
		void *const callback_arg);
static int capi_i2c_pl_configure_bus_speed(struct capi_i2c_controller_handle
		*handle,
		enum capi_i2c_speed speed, uint8_t duty_cycle);
static int capi_i2c_pl_transmit_async(struct capi_i2c_device *device,
				      struct capi_i2c_transfer *transfer);
static int capi_i2c_pl_receive_async(struct capi_i2c_device *device,
				     struct capi_i2c_transfer *transfer);
static int capi_i2c_pl_recover_bus(struct capi_i2c_controller_handle *handle);
static int capi_i2c_pl_register_target(struct capi_i2c_controller_handle
				       *handle, uint16_t addr);
static int capi_i2c_pl_unregister_target(struct capi_i2c_controller_handle
		*handle);
static void capi_i2c_pl_isr(void *handle);
static void xiic_send_handler(void *ref, int byte_count);
static void xiic_recv_handler(void *ref, int byte_count);
static void xiic_status_handler(void *ref, int status_event);
static void xiic_complete_transfer(struct capi_i2c_xilinx_handle *xh,
				   int status,
				   enum capi_i2c_async_event event);
static int capi_i2c_pl_set_bus_clock(struct capi_i2c_xilinx_handle *xh,
				     uint32_t bus_hz,
				     uint8_t duty_percent);
static int xiic_recv_polled_bounded(struct capi_i2c_xilinx_handle *xh,
				    u8 addr, uint8_t *buf, uint32_t len);

const struct capi_i2c_ops capi_i2c_xilinx_pl_ops = {
	.init = capi_i2c_pl_init,
	.deinit = capi_i2c_pl_deinit,
	.transmit = capi_i2c_pl_transmit,
	.receive = capi_i2c_pl_receive,
	.register_callback = capi_i2c_pl_register_callback,
	.configure_bus_speed = capi_i2c_pl_configure_bus_speed,
	.transmit_async = capi_i2c_pl_transmit_async,
	.receive_async = capi_i2c_pl_receive_async,
	.recover_bus = capi_i2c_pl_recover_bus,
	.register_target = capi_i2c_pl_register_target,
	.unregister_target = capi_i2c_pl_unregister_target,
	.isr = capi_i2c_pl_isr,
};

static XIic *inst(struct capi_i2c_xilinx_handle *xh)
{
	return (XIic *)xh->instance;
}

/*
 * Wall-clock timeout for an I2C transfer. Budget = 3× wire time + 2ms software
 * margin, floored at 5ms for no-progress detection. Wire time = (len+1)·9 bits
 * at scl_hz; the 3× covers START/STOP, repeated START, and clock stretching.
 */
static uint64_t xiic_transfer_timeout_us(struct capi_i2c_xilinx_handle *xh,
		uint32_t len)
{
	uint32_t scl_hz = xh->clk_freq_hz ? xh->clk_freq_hz :
			  I2C_PL_DEFAULT_SCL_HZ;

	uint64_t clocks = (uint64_t)(len + 1U) * I2C_PL_BITS_PER_BYTE;
	uint64_t wire_us = clocks * 1000000ULL / scl_hz;
	uint64_t timeout_us = wire_us * I2C_PL_TIMEOUT_SAFETY + 2000U;

	return timeout_us < 5000U ? 5000U : timeout_us;
}

/**
 * @brief Wall-clock deadline for the busy-wait loops (async completion and the
 * register-polling bus-state waits: START -> BUS_BUSY, per-byte RX_FULL,
 * STOP -> BNB, reset -> idle).
 *
 * A raw `--count` loop is not a unit of time: it drains faster on a faster CPU,
 * so a transfer legitimately in flight can be abandoned on a fast core. Poll
 * capi_uptime() instead. I2C_PL_BUS_BUSY_TIMEOUT is the spin backstop for the
 * case where no monotonic clock is available -- notably MicroBlaze, whose time
 * backend returns -ENOTSUP. Without this backstop a missing clock would collapse
 * to a single check and every wait would give up on its first iteration; with it
 * the loop still bounds-waits (CPU-relative, imprecise, but non-degenerate)
 * rather than either hanging forever or timing out immediately.
 */
struct xiic_deadline {
	uint64_t start;
	uint64_t budget_us;
	uint32_t spins;
	bool have_clock;
};

static void xiic_deadline_init(struct xiic_deadline *d, uint64_t budget_us)
{
	d->budget_us = budget_us;
	d->spins = I2C_PL_BUS_BUSY_TIMEOUT;
	d->have_clock = (capi_uptime(&d->start) == 0);
}

/* True while time remains; false once the budget (or spin backstop) expires. */
static bool xiic_deadline_ok(struct xiic_deadline *d)
{
	if (d->have_clock) {
		uint64_t now;

		if (capi_uptime(&now) == 0)
			return (now - d->start) < d->budget_us;
		d->have_clock = false;
	}
	return d->spins-- > 0U;
}

/**
 * @brief Block until an async transfer completes or the time budget expires.
 *
 * Uses the same xiic_deadline mechanism as the polled path, so the no-clock
 * fallback (MicroBlaze: capi_uptime -> -ENOTSUP) bounds-waits on the spin
 * backstop instead of doing a single check and reporting an immediate timeout.
 *
 * @param xh         Controller handle; xh->xfer_done is set by the ISR.
 * @param timeout_us Budget from xiic_transfer_timeout_us().
 * @return true if the transfer completed, false on timeout.
 */
static bool xiic_wait_xfer_done(struct capi_i2c_xilinx_handle *xh,
				uint64_t timeout_us)
{
	struct xiic_deadline dl;

	xiic_deadline_init(&dl, timeout_us);
	while (!xh->xfer_done) {
		if (!xiic_deadline_ok(&dl))
			break;
	}

	return xh->xfer_done;
}

static void xiic_set_repeated_start(struct capi_i2c_xilinx_handle *xh,
				    bool enable)
{
	u32 options = inst(xh)->Options;

	if (enable)
		options |= XII_REPEATED_START_OPTION;
	else
		options &= ~XII_REPEATED_START_OPTION;
	XIic_SetOptions(inst(xh), options);
}

static void xilinx_i2c_free_allocated_handle(
	struct capi_i2c_controller_handle **handle)
{
	if (handle == NULL || *handle == NULL)
		return;

	capi_free((*handle)->priv);
	capi_free(*handle);
	*handle = NULL;
}

static void xilinx_i2c_clear_app_handle(
	struct capi_i2c_controller_handle *handle)
{
	if (handle != NULL)
		handle->ops = NULL;
}

static int xiic_status_to_errno(int status)
{
	switch (status) {
	case XST_SUCCESS:
		return 0;
	case XST_IIC_BUS_BUSY:
		return -EBUSY;
	case XST_IIC_GENERAL_CALL_ADDRESS:
		return -EINVAL;
	case XST_IIC_ARB_LOST:
		return -EIO;
	default:
		return -EIO;
	}
}

/* XIic has no transfer-time control for per-device timing fields. */
static int pl_check_device_fields(const struct capi_i2c_device *device)
{
	if (device->duty_cycle != 0)
		return -ENOTSUP;
	if (device->clk_stretch != 0)
		return -ENOTSUP;
	return 0;
}

/* XIic_SetAddress asserts unless the address is below 1023. */
static int pl_check_addr(uint16_t addr, bool b10addr)
{
	if (b10addr) {
		if (addr > I2C_PL_10BIT_MAX_ADDR)
			return -EINVAL;
	} else if (addr > I2C_PL_7BIT_MAX_ADDR) {
		return -EINVAL;
	}
	return 0;
}

/* XIic can prepend a sub-address only with a repeated START. */
static int i2c_check_read_subaddr(const struct capi_i2c_transfer *transfer)
{
	if (transfer->sub_address && transfer->sub_address_len > 0 &&
	    !transfer->repeated_start)
		return -ENOTSUP;
	return 0;
}

static void xiic_reset_async_state(struct capi_i2c_xilinx_handle *xh)
{
	if (xh->async_tx_temp) {
		capi_free(xh->async_tx_temp);
		xh->async_tx_temp = NULL;
	}
	/* Abandoned before completion: drop the bounce buffer unread. */
	if (xh->async_rx_bounce) {
		capi_free(xh->async_rx_bounce);
		xh->async_rx_bounce = NULL;
	}
	xh->async_rx_caller_len = 0;

	xh->xfer_done = true;
	xh->xfer_status = 0;
	xh->async_phase = CAPI_I2C_ASYNC_IDLE;
	xh->async_tx_buf = NULL;
	xh->async_tx_len = 0;
	xh->async_rx_buf = NULL;
	xh->async_rx_len = 0;
	xh->async_dev_addr = 0;
	xh->async_no_stop = false;
	xh->target_request_event = 0;
	/* Only hardware-release paths clear bus_held. */
}

static void xiic_cancel_rejected_async_start(struct capi_i2c_xilinx_handle *xh)
{
	/* A rejected master start can leave the BNB interrupt enabled. */
	xiic_reset_async_state(xh);
	inst(xh)->BNBOnly = FALSE;
	XIic_WriteIier(inst(xh)->BaseAddress,
		       XIic_ReadIier(inst(xh)->BaseAddress) & ~XIIC_INTR_BNB_MASK);
	XIic_ClearIisr(inst(xh)->BaseAddress, XIIC_INTR_BNB_MASK);

	if (XIic_Stop(inst(xh)) != XST_SUCCESS) {
		XIic_Reset(inst(xh));
		XIic_IntrGlobalDisable(inst(xh)->BaseAddress);
		XIic_WriteIier(inst(xh)->BaseAddress, 0);
		inst(xh)->IsStarted = 0;
	}

	xh->bus_held = false;
}

/* Stop hardware before releasing its async buffer. */
static void xiic_abort_inflight(struct capi_i2c_xilinx_handle *xh)
{
	inst(xh)->Options &= ~XII_REPEATED_START_OPTION;

	if (XIic_Stop(inst(xh)) != XST_SUCCESS) {
		XIic_Reset(inst(xh));
		XIic_IntrGlobalDisable(inst(xh)->BaseAddress);
		XIic_WriteIier(inst(xh)->BaseAddress, 0);
		inst(xh)->IsStarted = 0;
	}

	xiic_reset_async_state(xh);
	xh->bus_held = false;
}

static int capi_i2c_pl_set_bus_clock(struct capi_i2c_xilinx_handle *xh,
				     uint32_t bus_hz,
				     uint8_t duty_percent)
{
	if (!xh || !inst(xh))
		return -EINVAL;
	if (xh->pl_input_clock_hz == 0)
		return -ENOTSUP;
	if (bus_hz == 0)
		return -EINVAL;

	if (duty_percent == 0)
		duty_percent = xh->pl_default_duty_percent;
	if (duty_percent == 0)
		duty_percent = I2C_PL_DEFAULT_DUTY_PERCENT;
	if (duty_percent < I2C_PL_MIN_DUTY_PERCENT ||
	    duty_percent > I2C_PL_MAX_DUTY_PERCENT)
		return -EINVAL;
	if (XIic_IsIicBusy(inst(xh)) == TRUE)
		return -EBUSY;

	uint32_t cycles = xh->pl_input_clock_hz / bus_hz;
	if (cycles < 2U)
		return -ENOTSUP;

	uint32_t cycles_high = (uint32_t)(((uint64_t)cycles * duty_percent) /
					  100U);
	uint32_t cycles_low = cycles - cycles_high;
	if (cycles_high == 0 || cycles_low == 0)
		return -ENOTSUP;

	XIic_WriteReg(inst(xh)->BaseAddress, xh->pl_thigh_offset, cycles_high);
	XIic_WriteReg(inst(xh)->BaseAddress, xh->pl_tlow_offset, cycles_low);
	xh->clk_freq_hz = bus_hz;

	return 0;
}

/**
 * @brief Initialize the CAPI backend instance.
 * @note PL: XIic_CfgInitialize() plus XIic_Set*Handler().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_pl_init(struct capi_i2c_controller_handle **handle,
			    const struct capi_i2c_config *config)
{
	if (!handle || !config)
		return -EINVAL;
	if (!config->initiator && config->address > I2C_PL_7BIT_MAX_ADDR)
		return -EINVAL;
	if (config->dma_handle != NULL)
		return -ENOTSUP;

	const struct capi_i2c_xilinx_config *xcfg =
		(const struct capi_i2c_xilinx_config *)config->extra;

	bool alloc = (*handle == NULL);
	struct capi_i2c_controller_handle *h = *handle;
	struct capi_i2c_xilinx_handle *xh;

	if (alloc) {
		h = capi_malloc(sizeof(*h));
		if (!h)
			return -ENOMEM;
		memset(h, 0, sizeof(*h));

		xh = capi_malloc(sizeof(*xh));
		if (!xh) {
			capi_free(h);
			return -ENOMEM;
		}
		h->priv = xh;
		*handle = h;
	} else {
		xh = h->priv;
		if (!xh)
			return -EINVAL;
		if (h->ops != NULL || xh->instance != NULL)
			return -EBUSY;
	}

	memset(xh, 0, sizeof(*xh));
	h->init_allocated = alloc;
	h->ops = config->ops ? config->ops : &capi_i2c_xilinx_pl_ops;
	h->priv = xh;
	xh->is_initiator = config->initiator;
	xh->clk_freq_hz = 0;
	if (xcfg != NULL && xcfg->input_clock_hz != 0) {
		xh->pl_input_clock_hz = xcfg->input_clock_hz;
		xh->pl_thigh_offset = xcfg->thigh_offset != 0 ?
				      xcfg->thigh_offset :
				      I2C_PL_DEFAULT_THIGH_OFFSET;
		xh->pl_tlow_offset = xcfg->tlow_offset != 0 ?
				     xcfg->tlow_offset :
				     I2C_PL_DEFAULT_TLOW_OFFSET;
		xh->pl_default_duty_percent = xcfg->default_duty_percent != 0 ?
					      xcfg->default_duty_percent :
					      I2C_PL_DEFAULT_DUTY_PERCENT;
	}

	XIic_Config *cfg = XIic_LookupConfig((UINTPTR)config->identifier);
	if (!cfg) {
		if (alloc)
			xilinx_i2c_free_allocated_handle(handle);
		else
			xilinx_i2c_clear_app_handle(h);
		return -ENODEV;
	}

	XIic *iic = capi_malloc(sizeof(XIic));
	if (!iic) {
		if (alloc)
			xilinx_i2c_free_allocated_handle(handle);
		else
			xilinx_i2c_clear_app_handle(h);
		return -ENOMEM;
	}
	memset(iic, 0, sizeof(XIic));

	if (XIic_CfgInitialize(iic, cfg, cfg->BaseAddress) != XST_SUCCESS) {
		capi_free(iic);
		if (alloc)
			xilinx_i2c_free_allocated_handle(handle);
		else
			xilinx_i2c_clear_app_handle(h);
		return -EIO;
	}

	/* Replace the vendor target/arbitration assertion stubs. */
	XIic_SlaveInclude();
	XIic_MultiMasterInclude();

	XIic_SetSendHandler(iic, xh, xiic_send_handler);
	XIic_SetRecvHandler(iic, xh, xiic_recv_handler);
	XIic_SetStatusHandler(iic, xh, xiic_status_handler);
	xh->instance = iic;

	uint32_t init_bus_hz = config->clk_freq_hz;
	if (init_bus_hz == 0 && xcfg != NULL)
		init_bus_hz = xcfg->default_bus_hz;
	if (init_bus_hz != 0) {
		int ret = capi_i2c_pl_set_bus_clock(xh, init_bus_hz,
						    xh->pl_default_duty_percent);
		if (ret != 0) {
			capi_free(iic);
			xh->instance = NULL;
			if (alloc)
				xilinx_i2c_free_allocated_handle(handle);
			else
				xilinx_i2c_clear_app_handle(h);
			return ret;
		}
	}

	if (!config->initiator) {
		if (XIic_SetAddress(iic, XII_ADDR_TO_RESPOND_TYPE,
				    (int)config->address) != XST_SUCCESS) {
			capi_free(iic);
			xh->instance = NULL;
			if (alloc)
				xilinx_i2c_free_allocated_handle(handle);
			else
				xilinx_i2c_clear_app_handle(h);
			return -EIO;
		}
		xh->target_addr = config->address;

		/*
		 * Programming the response address is not enough to make the
		 * controller answer: XIic only drives an ACK once the device is
		 * enabled, so without this the target is addressed correctly but
		 * stays deaf and every initiator write NAKs (-EIO / CAPI_I2C_NAKD).
		 *
		 * capi_i2c_pl_register_target() already does Stop -> SetAddress ->
		 * Start for a runtime role switch; a controller opened as a target
		 * at init must reach the same state without the caller having to
		 * make a redundant register_target() call.
		 */
		if (XIic_Start(iic) != XST_SUCCESS) {
			capi_free(iic);
			xh->instance = NULL;
			if (alloc)
				xilinx_i2c_free_allocated_handle(handle);
			else
				xilinx_i2c_clear_app_handle(h);
			return -EIO;
		}
	}

	if (xcfg != NULL && xcfg->use_irq) {
		/*
		 * The caller asked for interrupt-driven operation, so a failure
		 * to wire up the IRQ is fatal, not a silent downgrade to polled.
		 * A target in particular cannot answer without its ISR: swallowing
		 * the error here would defer the failure to the first transfer,
		 * where it surfaces as an unexplained NAK/timeout instead of a
		 * clear init error. Unwind (drop the instance and handle) and
		 * report the failure.
		 */
		uint32_t irq_id = xcfg->irq_id;
		int ret = capi_irq_connect(irq_id, capi_i2c_pl_isr, h);
		if (ret == 0) {
			capi_irq_set_level_edge_trigger(irq_id, CAPI_IRQ_LEVEL_HIGH);
			ret = capi_irq_enable(irq_id);
		}
		if (ret != 0) {
			capi_free(iic);
			xh->instance = NULL;
			if (alloc)
				xilinx_i2c_free_allocated_handle(handle);
			else
				xilinx_i2c_clear_app_handle(h);
			return ret;
		}
		xh->irq_id = irq_id;
		xh->use_irq = true;
	}

	return 0;
}

/**
 * @brief Deinitialize the CAPI backend instance.
 * @note PL: XIic_Stop()/XIic_Reset().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_pl_deinit(struct capi_i2c_controller_handle *handle)
{
	if (!handle)
		return -EINVAL;

	struct capi_i2c_xilinx_handle *xh = handle->priv;
	if (!xh)
		return -EINVAL;

	if (xh->use_irq) {
		capi_irq_disable(xh->irq_id);
		xh->use_irq = false;
	}

	if (xh->instance) {
		XIic_Stop(inst(xh));
		XIic_IntrGlobalDisable(inst(xh)->BaseAddress);
		XIic_WriteIier(inst(xh)->BaseAddress, 0);
		XIic_Reset(inst(xh));
		XIic_IntrGlobalDisable(inst(xh)->BaseAddress);
		XIic_WriteIier(inst(xh)->BaseAddress, 0);
		capi_free(xh->instance);
		xh->instance = NULL;
	}

	if (xh->async_tx_temp) {
		capi_free(xh->async_tx_temp);
		xh->async_tx_temp = NULL;
	}

	/*
	 * Clear the software state too, not just the hardware. A transfer can
	 * still be armed here (a target listen nothing addressed, or one whose
	 * peer gave up mid-frame). For an app-allocated handle the struct
	 * outlives this call, so a stale async_phase would make the next init
	 * hand back a controller that reports itself busy and rejects every
	 * transfer with -EBUSY. Mirrors capi_i2c_ps_deinit.
	 */
	xiic_reset_async_state(xh);
	xh->bus_held = false;
	xh->callback = NULL;
	xh->callback_arg = NULL;
	xh->callback_pending = false;

	if (handle->init_allocated) {
		capi_free(xh);
		capi_free(handle);
	} else {
		handle->ops = NULL;
	}

	return 0;
}

/**
 * @brief Run a synchronous transmit operation.
 * @note PL: XIic_Send()/XIic_SlaveSend().
 *
 * Blocking target transfers are not supported because they cannot be armed
 * safely before the target is addressed.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_pl_transmit(struct capi_i2c_device *device,
				struct capi_i2c_transfer *transfer)
{
	if (!device || !device->controller || !transfer)
		return -EINVAL;
	/*
	 * A zero-length write is a valid address-only probe (SMBus quick command
	 * / bus scan): allow len == 0. A data buffer is only required when there
	 * are data bytes to send.
	 */
	if ((transfer->len != 0 && !transfer->buf) ||
	    (transfer->sub_address_len > 0 && !transfer->sub_address))
		return -EINVAL;

	int unsup = pl_check_device_fields(device);
	if (unsup != 0)
		return unsup;

	struct capi_i2c_xilinx_handle *xh = device->controller->priv;
	if (xh->async_phase != CAPI_I2C_ASYNC_IDLE)
		return -EBUSY;
	if (transfer->len > INT_MAX)
		return -EINVAL;

	if (!xh->is_initiator)
		return -ENOTSUP;

	uint16_t addr = transfer->target_addr ? transfer->target_addr :
			device->address;
	int addr_ret = pl_check_addr(addr, device->b10addr);
	if (addr_ret != 0)
		return addr_ret;
	uint8_t *tx_buf = transfer->buf;
	uint32_t tx_len = transfer->len;
	bool need_free = false;
	int ret;

	if (device->b10addr) {
		if (!inst(xh)->Has10BitAddr)
			return -ENOTSUP;
		XIic_SetOptions(inst(xh), inst(xh)->Options |
				XII_SEND_10_BIT_OPTION);
	} else {
		inst(xh)->Options &= ~XII_SEND_10_BIT_OPTION;
	}

	int status = XIic_SetAddress(inst(xh), XII_ADDR_TO_SEND_TYPE, addr);
	if (status != XST_SUCCESS)
		return xiic_status_to_errno(status);

	if (transfer->sub_address && transfer->sub_address_len > 0) {
		if (transfer->len > (uint32_t)INT_MAX - transfer->sub_address_len)
			return -EINVAL;
		tx_len = (uint32_t)transfer->sub_address_len + transfer->len;
		tx_buf = capi_malloc(tx_len);
		if (!tx_buf)
			return -ENOMEM;
		memcpy(tx_buf, transfer->sub_address, transfer->sub_address_len);
		memcpy(tx_buf + transfer->sub_address_len, transfer->buf,
		       transfer->len);
		need_free = true;
	}

	/*
	 * Route to the interrupt (MasterSend) path only when the polled
	 * XIic_Send cannot do the job on an unmodified upstream BSP:
	 *
	 *   - 10-bit addressing: XIic_Send formats only a 7-bit header.
	 *   - held-bus continuation (bus_held): XIic_Send calls
	 *     XIic_WaitBusFree() unconditionally at its START. A prior no_stop
	 *     transfer left this controller holding the bus (MSMS asserted), so
	 *     that entry wait never sees the bus go idle and times out (~1s).
	 *
	 * A fresh 7-bit write (bus free), including the FIRST no_stop write of a
	 * chain, is fine polled: the entry wait passes, and XIIC_REPEATED_START
	 * leaves the bus held for the continuation below. That covers the common
	 * write(no_stop) -> read(stop) chain with no IRQ.
	 */
	if (device->b10addr || xh->bus_held) {
		if (!xh->use_irq) {
			ret = -ENOTSUP;
			goto out_free;
		}

		if (transfer->no_stop)
			xiic_set_repeated_start(xh, true);
		else
			xiic_set_repeated_start(xh, false);

		/* XIic_Start would release an existing hold. */
		if (!xh->bus_held) {
			status = XIic_Start(inst(xh));
			if (status != XST_SUCCESS) {
				ret = xiic_status_to_errno(status);
				goto out_free;
			}
		}

		capi_i2c_callback saved_cb = xh->callback;
		xh->callback = NULL;
		xh->async_phase = CAPI_I2C_ASYNC_TX;
		xh->xfer_done = false;
		xh->xfer_status = 0;
		xh->async_no_stop = transfer->no_stop;

		status = XIic_MasterSend(inst(xh), tx_buf, (int)tx_len);
		if (status != XST_SUCCESS) {
			xiic_cancel_rejected_async_start(xh);
			xh->callback = saved_cb;
			ret = xiic_status_to_errno(status);
			goto out_free;
		}

		uint64_t timeout_us = xiic_transfer_timeout_us(xh, tx_len);
		if (!xiic_wait_xfer_done(xh, timeout_us)) {
			xiic_abort_inflight(xh);
			xh->callback = saved_cb;
			if (need_free)
				capi_free(tx_buf);
			return -ETIMEDOUT;
		}
		xh->callback = saved_cb;
		if (need_free)
			capi_free(tx_buf);
		return xh->xfer_status;
	}

	u8 option = transfer->no_stop ? XIIC_REPEATED_START : XIIC_STOP;
	unsigned sent = XIic_Send(inst(xh)->BaseAddress, addr, tx_buf, tx_len,
				  option);
	ret = (sent == tx_len) ? 0 : -EIO;

	/* Mirror the hold left by XIic_Send. */
	xh->bus_held = (ret == 0) && transfer->no_stop;

out_free:
	if (need_free)
		capi_free(tx_buf);
	return ret;
}

/**
 * @brief Bounded polled master receive (replaces XIic_Recv).
 *
 * XIic_Recv and its RecvData() helper are correct on their register sequence
 * but every wait inside them is an unbounded `while` with no timeout:
 *
 *   - the post-START spin for SR.BUS_BUSY to assert,
 *   - RecvData()'s per-byte `while (1)` waiting for IISR.RX_FULL, which for a
 *     single-byte read does not even watch TX_ERROR,
 *   - the closing wait for IISR.BNB after the STOP.
 *
 * On this board that is not a theoretical risk. The initiator is polled while
 * the target is interrupt-driven, and both run on the same CPU: if the target
 * is late producing a byte, the initiator does not slow down, it spins forever
 * and the CPU never returns to the code that would service the target. The
 * result is a dead board with no diagnostic.
 *
 * This is a transcription of the BSP flow with identical register writes and
 * bus semantics, differing only in that each wait is bounded and reports where
 * it gave up. A stalled peer now yields -ETIMEDOUT (or -EIO on a bus error)
 * instead of wedging, so the caller can recover and the failure is visible.
 *
 * Only the 7-bit STOP case reaches here; 10-bit and no_stop are handled on the
 * interrupt path above.
 *
 * @return 0 on success, -ETIMEDOUT if a wait expired, -EIO on a bus error.
 */
static int xiic_recv_polled_bounded(struct capi_i2c_xilinx_handle *xh,
				    u8 addr, uint8_t *buf, uint32_t len)
{
	UINTPTR base = inst(xh)->BaseAddress;
	uint64_t budget_us = xiic_transfer_timeout_us(xh, len);
	struct xiic_deadline dl;
	u32 cr;

	XIic_ClearIisr(base, XIIC_INTR_RX_FULL_MASK | XIIC_INTR_TX_ERROR_MASK |
		       XIIC_INTR_ARB_LOST_MASK);

	/* Receive FIFO occupancy depth of one byte (zero based). */
	XIic_WriteReg(base, XIIC_RFD_REG_OFFSET, 0);

	cr = XIic_ReadReg(base, XIIC_CR_REG_OFFSET);
	if ((cr & XIIC_CR_REPEATED_START_MASK) == 0) {
		XIic_Send7BitAddress(base, addr, XIIC_READ_OPERATION);

		/*
		 * MSMS is set after the address is in the FIFO. A single-byte
		 * read must NAK immediately so the target knows to stop.
		 */
		cr = XIIC_CR_MSMS_MASK | XIIC_CR_ENABLE_DEVICE_MASK;
		if (len == 1U)
			cr |= XIIC_CR_NO_ACK_MASK;
		XIic_WriteReg(base, XIIC_CR_REG_OFFSET, cr);

		/* BNB must be cleared while the bus is busy, so wait for busy. */
		xiic_deadline_init(&dl, budget_us);
		while ((XIic_ReadReg(base, XIIC_SR_REG_OFFSET) &
			XIIC_SR_BUS_BUSY_MASK) == 0) {
			if (!xiic_deadline_ok(&dl))
				goto stall;
		}

		XIic_ClearIisr(base, XIIC_INTR_BNB_MASK);
	} else {
		/* Continuing a held bus: clear TX direction before addressing. */
		cr &= ~XIIC_CR_DIR_IS_TX_MASK;
		if (len == 1U)
			cr |= XIIC_CR_NO_ACK_MASK;
		XIic_WriteReg(base, XIIC_CR_REG_OFFSET, cr);
		XIic_Send7BitAddress(base, addr, XIIC_READ_OPERATION);
	}

	while (len > 0U) {
		/*
		 * The final byte is NAKed deliberately, so TX_ERROR is expected
		 * there and must not be treated as a fault.
		 */
		u32 err_mask = (len == 1U) ?
			       (XIIC_INTR_ARB_LOST_MASK | XIIC_INTR_BNB_MASK) :
			       (XIIC_INTR_ARB_LOST_MASK | XIIC_INTR_TX_ERROR_MASK |
				XIIC_INTR_BNB_MASK);

		xiic_deadline_init(&dl, budget_us);
		for (;;) {
			u32 isr = XIic_ReadIisr(base);

			if (isr & XIIC_INTR_RX_FULL_MASK)
				break;
			if (isr & err_mask)
				goto bus_error;
			if (!xiic_deadline_ok(&dl))
				goto stall;
		}

		cr = XIic_ReadReg(base, XIIC_CR_REG_OFFSET);

		/*
		 * Both of these must be programmed BEFORE the byte is read out
		 * of the FIFO, while the bus is still throttled.
		 */
		if (len == 1U) {
			/* Drop MSMS, stay enabled, so the STOP is issued. */
			XIic_WriteReg(base, XIIC_CR_REG_OFFSET,
				      XIIC_CR_ENABLE_DEVICE_MASK);
		} else if (len == 2U) {
			/* NAK the byte after this one to mark the end. */
			XIic_WriteReg(base, XIIC_CR_REG_OFFSET,
				      cr | XIIC_CR_NO_ACK_MASK);
		}

		/* Reading DRR unthrottles the bus for the next byte. */
		*buf++ = (uint8_t)XIic_ReadReg(base, XIIC_DRR_REG_OFFSET);

		XIic_ClearIisr(base, XIIC_INTR_RX_FULL_MASK |
			       XIIC_INTR_TX_ERROR_MASK |
			       XIIC_INTR_ARB_LOST_MASK);
		len--;
	}

	/* Wait for the STOP to land: MSMS was dropped before the last read. */
	xiic_deadline_init(&dl, budget_us);
	while ((XIic_ReadIisr(base) & XIIC_INTR_BNB_MASK) == 0) {
		if (!xiic_deadline_ok(&dl))
			goto stall;
	}

	if ((XIic_ReadReg(base, XIIC_CR_REG_OFFSET) &
	     XIIC_CR_REPEATED_START_MASK) == 0)
		XIic_WriteReg(base, XIIC_CR_REG_OFFSET, 0);

	return 0;

bus_error:
	XIic_WriteReg(base, XIIC_CR_REG_OFFSET, 0);
	XIic_ClearIisr(base, XIIC_INTR_RX_FULL_MASK | XIIC_INTR_TX_ERROR_MASK |
		       XIIC_INTR_ARB_LOST_MASK | XIIC_INTR_BNB_MASK);
	return -EIO;

stall:
	/*
	 * Leave the controller disabled and the bus released rather than held,
	 * so the next transfer starts from a defined state instead of
	 * inheriting a half-finished frame.
	 */
	XIic_WriteReg(base, XIIC_CR_REG_OFFSET, 0);
	XIic_ClearIisr(base, XIIC_INTR_RX_FULL_MASK | XIIC_INTR_TX_ERROR_MASK |
		       XIIC_INTR_ARB_LOST_MASK | XIIC_INTR_BNB_MASK);
	return -ETIMEDOUT;
}

/**
 * @brief Run a synchronous receive operation.
 * @note PL: XIic_Send()/XIic_Recv().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_pl_receive(struct capi_i2c_device *device,
			       struct capi_i2c_transfer *transfer)
{
	if (!device || !device->controller || !transfer)
		return -EINVAL;
	if (!transfer->buf || transfer->len == 0 ||
	    (transfer->sub_address_len > 0 && !transfer->sub_address))
		return -EINVAL;

	int unsup = pl_check_device_fields(device);
	if (unsup != 0)
		return unsup;
	unsup = i2c_check_read_subaddr(transfer);
	if (unsup != 0)
		return unsup;

	struct capi_i2c_xilinx_handle *xh = device->controller->priv;
	if (xh->async_phase != CAPI_I2C_ASYNC_IDLE)
		return -EBUSY;
	if (transfer->len > INT_MAX)
		return -EINVAL;

	if (!xh->is_initiator)
		return -ENOTSUP;

	uint16_t addr = transfer->target_addr ? transfer->target_addr :
			device->address;
	int addr_ret = pl_check_addr(addr, device->b10addr);
	if (addr_ret != 0)
		return addr_ret;
	int status;

	if (device->b10addr) {
		if (!inst(xh)->Has10BitAddr)
			return -ENOTSUP;
		XIic_SetOptions(inst(xh), inst(xh)->Options |
				XII_SEND_10_BIT_OPTION);
	} else {
		inst(xh)->Options &= ~XII_SEND_10_BIT_OPTION;
	}

	status = XIic_SetAddress(inst(xh), XII_ADDR_TO_SEND_TYPE, addr);
	if (status != XST_SUCCESS)
		return xiic_status_to_errno(status);

	/*
	 * Route to the interrupt (MasterRecv) path only when the polled
	 * XIic_Recv cannot do the job on an unmodified upstream BSP:
	 *
	 *   - 10-bit addressing: XIic_Recv formats only a 7-bit header.
	 *   - no_stop read: XIic_Recv calls XIic_WaitBusFree() unconditionally
	 *     at its END. When the receive itself holds the bus (RSTA|MSMS left
	 *     asserted for a repeated START) that wait never sees the bus go
	 *     idle and times out (~1s). So a receive that must LEAVE the bus
	 *     held cannot use the polled path here.
	 *
	 * A 7-bit read that ENTERS on a held bus but ends with a STOP is fine
	 * polled: XIic_Recv has no entry bus-free wait, it continues the hold
	 * with a repeated START (it inspects the CR REPEATED_START bit), and the
	 * closing STOP frees the bus before the exit wait. That covers the
	 * common write(no_stop) -> read(stop) register-read chain with no IRQ.
	 */
	if (device->b10addr || transfer->no_stop) {
		if (!xh->use_irq)
			return -ENOTSUP;

		if (transfer->no_stop)
			xiic_set_repeated_start(xh, true);
		else
			xiic_set_repeated_start(xh, false);

		/* XIic_Start would release an existing hold. */
		if (!xh->bus_held) {
			status = XIic_Start(inst(xh));
			if (status != XST_SUCCESS)
				return xiic_status_to_errno(status);
		}

		capi_i2c_callback saved_cb = xh->callback;
		xh->callback = NULL;

		if (transfer->sub_address && transfer->sub_address_len > 0 &&
		    transfer->repeated_start) {
			xh->async_phase = CAPI_I2C_ASYNC_RX_SUBADDR;
			xh->async_rx_buf = transfer->buf;
			xh->async_rx_len = transfer->len;
			xh->async_dev_addr = addr;
			xh->async_no_stop = transfer->no_stop;
			xh->async_tx_temp = NULL;
			xh->xfer_done = false;
			xh->xfer_status = 0;

			xiic_set_repeated_start(xh, true);
			status = XIic_MasterSend(inst(xh),
						 transfer->sub_address,
						 transfer->sub_address_len);
		} else {
			xh->async_phase = CAPI_I2C_ASYNC_RX;
			xh->xfer_done = false;
			xh->xfer_status = 0;
			xh->async_no_stop = transfer->no_stop;

			status = XIic_MasterRecv(inst(xh), transfer->buf,
						 transfer->len);
		}
		if (status != XST_SUCCESS) {
			xiic_cancel_rejected_async_start(xh);
			xh->callback = saved_cb;
			return xiic_status_to_errno(status);
		}

		uint64_t timeout_us = xiic_transfer_timeout_us(xh,
				      transfer->len + transfer->sub_address_len);
		if (!xiic_wait_xfer_done(xh, timeout_us)) {
			xiic_abort_inflight(xh);
			xh->callback = saved_cb;
			return -ETIMEDOUT;
		}
		xh->callback = saved_cb;
		return xh->xfer_status;
	}

	/*
	 * Sub-address read: write the register pointer with the bus held, then
	 * read. XIic_Send here would stall in its entry XIic_WaitBusFree() if a
	 * prior no_stop transfer already holds the bus, so this chained-write
	 * form is only valid when the bus is currently free.
	 */
	if (transfer->sub_address && transfer->sub_address_len > 0 &&
	    transfer->repeated_start) {
		if (xh->bus_held)
			return -EBUSY;
		unsigned count = XIic_Send(inst(xh)->BaseAddress, addr,
					   transfer->sub_address,
					   transfer->sub_address_len,
					   XIIC_REPEATED_START);
		if (count != transfer->sub_address_len)
			return -EIO;
	}

	/*
	 * Polled read, always terminated with a STOP. no_stop reads are handled
	 * above on the interrupt path (a held receive would hang XIic_Recv's
	 * unconditional exit XIic_WaitBusFree on the bus it keeps asserted), so
	 * every read reaching here ends the transaction. XIic_Recv still
	 * continues an incoming held bus with a repeated START by inspecting the
	 * CR REPEATED_START bit, so write(no_stop) -> read(stop) chains natively.
	 */
	int ret = xiic_recv_polled_bounded(xh, (u8)addr, transfer->buf,
					   transfer->len);
	xh->bus_held = false;
	return ret;
}

/**
 * @brief Register the CAPI asynchronous callback.
 * @note PL: Stores callback for XIic handlers.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_pl_register_callback(struct capi_i2c_controller_handle
		*handle,
		capi_i2c_callback const callback, void *const callback_arg)
{
	if (!handle)
		return -EINVAL;
	struct capi_i2c_xilinx_handle *xh = handle->priv;
	xh->callback = callback;
	xh->callback_arg = callback_arg;
	return 0;
}

/**
 * @brief Configure the I2C bus speed.
 * @note PL (AXI IIC): rewrites the THIGH/TLOW timing registers from
 *       pl_input_clock_hz / bus_hz (cycles = fabric_clk / SCL). These registers
 *       generate the MASTER's SCL only; the slave path oversamples SDA against
 *       the fixed fabric clock and has no master-speed-derived sampling window.
 *       So, unlike the PS core, reconfiguring a PL TARGET has nothing to size --
 *       it is a harmless no-op on the wire, and returns -ENOTSUP when the design
 *       provides no runtime timing (pl_input_clock_hz == 0), i.e. a stock XIic
 *       whose SCL ratio is fixed in HDL. Speeds map STANDARD..ULTRA; duty is
 *       honored via the THIGH/TLOW split. Contrast the PS note, where SetSClk
 *       sizes a divisor shared by both master and slave.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_pl_configure_bus_speed(struct capi_i2c_controller_handle
		*handle,
		enum capi_i2c_speed speed, uint8_t duty_cycle)
{
	static const uint32_t speed_hz[] = {
		[CAPI_I2C_SPEED_STANDARD] = 100000U,
		[CAPI_I2C_SPEED_FAST] = 400000U,
		[CAPI_I2C_SPEED_FAST_PLUS] = 1000000U,
		[CAPI_I2C_SPEED_HIGH] = 3400000U,
		[CAPI_I2C_SPEED_ULTRA] = 5000000U,
	};

	if (!handle || !handle->priv)
		return -EINVAL;
	if ((unsigned int)speed >= sizeof(speed_hz) / sizeof(speed_hz[0]))
		return -EINVAL;
	struct capi_i2c_xilinx_handle *xh = handle->priv;
	if (xh->async_phase != CAPI_I2C_ASYNC_IDLE || xh->bus_held)
		return -EBUSY;

	return capi_i2c_pl_set_bus_clock(xh, speed_hz[speed],
					 duty_cycle);
}

/**
 * @brief Start an asynchronous transmit operation.
 * @note PL: XIic_Start()/XIic_MasterSend().
 *
 * Non-blocking: arms the hardware and returns; completion is reported from the
 * interrupt path (xiic_send_handler -> xiic_complete_transfer).
 *
 * Flow, initiator role:
 *   1. Reject if no IRQ or a transfer is already in flight.
 *   2. If a sub-address is present, concatenate [sub_address || data] into a
 *      temp buffer that must outlive this call (freed on completion).
 *   3. Select 7/10-bit addressing (10-bit rejected if the core lacks it) and
 *      set the send target address.
 *   4. Map no_stop to the repeated-start option.
 *   5. XIic_Start enables the core, then set phase = TX and kick
 *      XIic_MasterSend.
 * Rollback differs by how far we got: before the transfer is kicked, use
 * xiic_reset_async_state; once XIic_MasterSend has been issued, use
 * xiic_cancel_rejected_async_start to also tear down the pending Xilinx state
 * so no stray completion interrupt arrives later.
 * Target role: the request is armed in software. XIic_SlaveSend starts only
 * after the addressed-as-target event identifies a master read.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_pl_transmit_async(struct capi_i2c_device *device,
				      struct capi_i2c_transfer *transfer)
{
	if (!device || !device->controller || !transfer)
		return -EINVAL;
	/* Zero-length write is a valid address-only probe; buf only needed for data. */
	if ((transfer->len != 0 && !transfer->buf) ||
	    (transfer->sub_address_len > 0 && !transfer->sub_address))
		return -EINVAL;

	int unsup = pl_check_device_fields(device);
	if (unsup != 0)
		return unsup;

	struct capi_i2c_xilinx_handle *xh = device->controller->priv;
	if (!xh->use_irq)
		return -ENOTSUP;
	if (xh->async_phase != CAPI_I2C_ASYNC_IDLE)
		return -EBUSY;
	if (transfer->len > INT_MAX)
		return -EINVAL;

	if (xh->is_initiator) {
		uint16_t addr = transfer->target_addr ? transfer->target_addr :
				device->address;
		int addr_ret = pl_check_addr(addr, device->b10addr);
		if (addr_ret != 0)
			return addr_ret;
		uint8_t *tx_buf = transfer->buf;
		uint32_t tx_len = transfer->len;

		if (transfer->sub_address && transfer->sub_address_len > 0) {
			if (transfer->len > (uint32_t)INT_MAX - transfer->sub_address_len)
				return -EINVAL;
			tx_len = (uint32_t)transfer->sub_address_len + transfer->len;
			tx_buf = capi_malloc(tx_len);
			if (!tx_buf)
				return -ENOMEM;
			memcpy(tx_buf, transfer->sub_address, transfer->sub_address_len);
			memcpy(tx_buf + transfer->sub_address_len, transfer->buf, transfer->len);
			xh->async_tx_temp = tx_buf;
		} else {
			xh->async_tx_temp = NULL;
		}

		if (device->b10addr) {
			if (!inst(xh)->Has10BitAddr) {
				xiic_reset_async_state(xh);
				return -ENOTSUP;
			}
			XIic_SetOptions(inst(xh), inst(xh)->Options |
					XII_SEND_10_BIT_OPTION);
		} else {
			inst(xh)->Options &= ~XII_SEND_10_BIT_OPTION;
		}

		int status = XIic_SetAddress(inst(xh), XII_ADDR_TO_SEND_TYPE,
					     addr);
		if (status != XST_SUCCESS) {
			xiic_reset_async_state(xh);
			return xiic_status_to_errno(status);
		}

		if (transfer->no_stop)
			xiic_set_repeated_start(xh, true);
		else
			xiic_set_repeated_start(xh, false);

		/* XIic_Start would release an existing hold. */
		if (!xh->bus_held) {
			status = XIic_Start(inst(xh));
			if (status != XST_SUCCESS) {
				xiic_reset_async_state(xh);
				return xiic_status_to_errno(status);
			}
		}

		xh->async_phase = CAPI_I2C_ASYNC_TX;
		xh->xfer_done = false;
		xh->xfer_status = 0;
		xh->async_no_stop = transfer->no_stop;

		status = XIic_MasterSend(inst(xh), tx_buf, (int)tx_len);
		if (status != XST_SUCCESS) {
			xiic_cancel_rejected_async_start(xh);
			return xiic_status_to_errno(status);
		}
	} else {
		if (transfer->len > UINT8_MAX)
			return -ENOTSUP;
		xh->async_phase = CAPI_I2C_ASYNC_TX;
		xh->xfer_done = false;
		xh->xfer_status = 0;
		xh->async_tx_buf = transfer->buf;
		xh->async_tx_len = transfer->len;
		if ((xh->target_request_event & XII_MASTER_READ_EVENT) != 0) {
			xh->target_request_event = 0;
			XIic_ClearStats(inst(xh));
			int status = XIic_SlaveSend(inst(xh), xh->async_tx_buf,
						    (int)xh->async_tx_len);
			if (status != XST_SUCCESS) {
				xiic_reset_async_state(xh);
				return xiic_status_to_errno(status);
			}
		}
	}
	return 0;
}

/**
 * @brief Start an asynchronous receive operation.
 * @note PL: XIic_MasterSend()/XIic_MasterRecv().
 *
 * Non-blocking: arms the hardware and returns; the interrupt path reports
 * completion. Two initiator sub-cases:
 *
 *   Direct read (no sub-address): XIic_Start, set phase = RX, map no_stop to
 *   repeated-start, and kick XIic_MasterRecv. The recv handler completes the
 *   CAPI transfer.
 *
 *   Register read (sub_address + repeated_start): a two-phase frame. Phase 1
 *   sends the sub-address with repeated-start held (bus kept owned) under
 *   phase = RX_SUBADDR. The send handler then flips to RX and issues phase 2's
 *   XIic_MasterRecv itself, without completing the CAPI transfer; only phase 2
 *   completes it. The async_rx_* fields saved here drive phase 2.
 *
 * Target role: the request is armed in software. XIic_SlaveRecv starts only
 * after the addressed-as-target event identifies a master write.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_pl_receive_async(struct capi_i2c_device *device,
				     struct capi_i2c_transfer *transfer)
{
	if (!device || !device->controller || !transfer)
		return -EINVAL;
	if (!transfer->buf || transfer->len == 0 ||
	    (transfer->sub_address_len > 0 && !transfer->sub_address))
		return -EINVAL;

	int unsup = pl_check_device_fields(device);
	if (unsup != 0)
		return unsup;
	struct capi_i2c_xilinx_handle *xh = device->controller->priv;
	if (!xh->use_irq)
		return -ENOTSUP;
	if (xh->async_phase != CAPI_I2C_ASYNC_IDLE)
		return -EBUSY;
	if (transfer->len > INT_MAX)
		return -EINVAL;
	if (xh->is_initiator) {
		unsup = i2c_check_read_subaddr(transfer);
		if (unsup != 0)
			return unsup;

		uint16_t addr = transfer->target_addr ? transfer->target_addr :
				device->address;
		int addr_ret = pl_check_addr(addr, device->b10addr);
		if (addr_ret != 0)
			return addr_ret;

		if (device->b10addr) {
			if (!inst(xh)->Has10BitAddr)
				return -ENOTSUP;
			XIic_SetOptions(inst(xh), inst(xh)->Options |
					XII_SEND_10_BIT_OPTION);
		} else {
			inst(xh)->Options &= ~XII_SEND_10_BIT_OPTION;
		}

		int status = XIic_SetAddress(inst(xh), XII_ADDR_TO_SEND_TYPE,
					     addr);
		if (status != XST_SUCCESS)
			return xiic_status_to_errno(status);

		/* XIic_Start would release an existing hold. */
		if (!xh->bus_held) {
			status = XIic_Start(inst(xh));
			if (status != XST_SUCCESS)
				return xiic_status_to_errno(status);
		}

		if (transfer->sub_address && transfer->sub_address_len > 0 &&
		    transfer->repeated_start) {
			/* The send completion starts the receive phase. */
			xh->async_phase = CAPI_I2C_ASYNC_RX_SUBADDR;
			xh->async_rx_buf = transfer->buf;
			xh->async_rx_len = transfer->len;
			xh->async_dev_addr = addr;
			xh->async_no_stop = transfer->no_stop;
			xh->async_tx_temp = NULL;
			xh->xfer_done = false;
			xh->xfer_status = 0;

			xiic_set_repeated_start(xh, true);

			status = XIic_MasterSend(inst(xh), transfer->sub_address,
						 transfer->sub_address_len);
			if (status != XST_SUCCESS) {
				xiic_cancel_rejected_async_start(xh);
				return xiic_status_to_errno(status);
			}
			return 0;
		}

		xh->async_phase = CAPI_I2C_ASYNC_RX;
		xh->xfer_done = false;
		xh->xfer_status = 0;
		xh->async_no_stop = transfer->no_stop;

		if (transfer->no_stop)
			xiic_set_repeated_start(xh, true);
		else
			xiic_set_repeated_start(xh, false);

		status = XIic_MasterRecv(inst(xh), transfer->buf, transfer->len);
		if (status != XST_SUCCESS) {
			xiic_cancel_rejected_async_start(xh);
			return xiic_status_to_errno(status);
		}
	} else {
		/*
		 * Target receive. XIic counts the armed ByteCount down and asserts
		 * NO_ACK the moment it reaches zero (see SlaveRecvData in the BSP's
		 * xiic_slave.c), so arming the caller's exact length makes an
		 * initiator that sends more than that get NAKed mid-frame and fail
		 * its write. Arm a bounce buffer big enough to absorb an over-long
		 * frame instead, and hand back only the requested length on
		 * completion; a caller whose buffer is already at least that large
		 * cannot be over-run and is armed directly.
		 *
		 * This mirrors what the PS backend does for the same CAPI contract
		 * (see capi_i2c_ps_receive_async), so both look identical to a
		 * caller despite the opposite vendor behaviour.
		 */
		uint8_t *rx_buf = transfer->buf;

		xh->async_rx_bounce = NULL;
		xh->async_rx_caller_len = transfer->len;

		if (transfer->len < XIIC_TARGET_RX_ABSORB_LEN) {
			xh->async_rx_bounce = capi_malloc(XIIC_TARGET_RX_ABSORB_LEN);
			if (!xh->async_rx_bounce)
				return -ENOMEM;
			rx_buf = xh->async_rx_bounce;
		}

		xh->async_phase = CAPI_I2C_ASYNC_RX;
		xh->xfer_done = false;
		xh->xfer_status = 0;
		xh->async_rx_buf = transfer->buf;
		xh->async_rx_len = xh->async_rx_bounce ?
				   XIIC_TARGET_RX_ABSORB_LEN : transfer->len;
		if ((xh->target_request_event & XII_MASTER_WRITE_EVENT) != 0) {
			xh->target_request_event = 0;
			int status = XIic_SlaveRecv(inst(xh), rx_buf,
						    (int)xh->async_rx_len);
			if (status != XST_SUCCESS) {
				xiic_reset_async_state(xh);
				return xiic_status_to_errno(status);
			}
		}
	}
	return 0;
}

/**
 * @brief Recover the I2C controller state.
 * @note PL: XIic_Reset()/XIic_SetAddress().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_pl_recover_bus(struct capi_i2c_controller_handle *handle)
{
	if (!handle)
		return -EINVAL;
	struct capi_i2c_xilinx_handle *xh = handle->priv;

	/*
	 * An in-flight async does not block recovery -- it is the usual reason to
	 * want it. An armed target receive nothing ever addresses, or a master
	 * transfer whose peer vanished mid-frame, leave async_phase stuck off IDLE
	 * with no completion interrupt coming. Refusing -EBUSY there would wedge
	 * every later recover_bus / register_target / unregister_target on this
	 * handle. Abandon the transfer as part of recovering; the reset below
	 * cannot preserve it anyway. Mirrors capi_i2c_ps_recover_bus.
	 */
	bool cancelled = xh->async_phase != CAPI_I2C_ASYNC_IDLE;
	if (cancelled)
		xiic_abort_inflight(xh);

	XIic_Reset(inst(xh));
	xh->bus_held = false;
	inst(xh)->IsStarted = 0;
	inst(xh)->Options = 0;

	int ret = 0;

	/* Reset cannot clock a target-held SDA, so verify the bus became idle. */
	struct xiic_deadline dl;
	bool idle = false;

	xiic_deadline_init(&dl, xiic_transfer_timeout_us(xh, 1U));
	while (XIic_CheckIsBusBusy(inst(xh)->BaseAddress)) {
		if (!xiic_deadline_ok(&dl))
			break;
	}
	idle = !XIic_CheckIsBusBusy(inst(xh)->BaseAddress);
	if (!idle) {
		ret = -EIO;
	} else if (!xh->is_initiator) {
		if (XIic_SetAddress(inst(xh), XII_ADDR_TO_RESPOND_TYPE,
				    xh->target_addr) != XST_SUCCESS)
			ret = -EIO;
		else if (XIic_Start(inst(xh)) != XST_SUCCESS)
			ret = -EIO;
	}

	/*
	 * Notify after the controller work is done, not before: the callback may
	 * arm a new transfer, and the reset above would destroy one started early.
	 *
	 * This fires on the failure paths too. Having cancelled the transfer we
	 * owe the caller exactly one terminal notification for it, whether or not
	 * the rest of the recovery succeeded -- skipping it on error would leave
	 * them waiting on a completion that can never arrive.
	 */
	if (cancelled && xh->callback)
		xh->callback(CAPI_I2C_NONE, xh->callback_arg, -ECANCELED);

	return ret;
}

/**
 * @brief Register an I2C target address.
 * @note PL: XIic_SetAddress()/XIic_Start().
 *
 * Stops an idle controller, programs the response address, and restarts it in
 * target mode. unregister_target() restores initiator mode.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_pl_register_target(struct capi_i2c_controller_handle
				       *handle, uint16_t addr)
{
	if (!handle)
		return -EINVAL;
	if (addr > I2C_PL_7BIT_MAX_ADDR)
		return -EINVAL;

	struct capi_i2c_xilinx_handle *xh = handle->priv;

	/* Switching role mid-transfer would corrupt the in-flight frame. */
	if (xh->async_phase != CAPI_I2C_ASYNC_IDLE || xh->bus_held ||
	    XIic_CheckIsBusBusy(inst(xh)->BaseAddress))
		return -EBUSY;

	/*
	 * XIic answers as a slave once a response address is programmed and the
	 * engine is (re)started; the role is not locked at init. Stop first so
	 * XIic_SetAddress can write the address register, then switch this
	 * controller into target mode. Target mode is available on the polled
	 * (sync) path too, so IRQ is not required here.
	 */
	if (XIic_Stop(inst(xh)) != XST_SUCCESS)
		return -EBUSY;
	if (XIic_SetAddress(inst(xh), XII_ADDR_TO_RESPOND_TYPE,
			    (int)addr) != XST_SUCCESS)
		return -EIO;
	if (XIic_Start(inst(xh)) != XST_SUCCESS)
		return -EIO;

	xh->is_initiator = false;
	xh->target_addr = addr;
	return 0;
}

/**
 * @brief Unregister the I2C target address.
 * @note PL: XIic_Stop().
 *
 * Restores initiator (master) mode so the controller can drive transfers
 * again, mirroring register_target's runtime role switch.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_pl_unregister_target(struct capi_i2c_controller_handle
		*handle)
{
	if (!handle)
		return -EINVAL;

	struct capi_i2c_xilinx_handle *xh = handle->priv;

	if (xh->async_phase != CAPI_I2C_ASYNC_IDLE || xh->bus_held ||
	    XIic_CheckIsBusBusy(inst(xh)->BaseAddress))
		return -EBUSY;

	if (XIic_Stop(inst(xh)) != XST_SUCCESS)
		return -EBUSY;
	XIic_Reset(inst(xh));
	inst(xh)->Options = 0;
	xh->is_initiator = true;
	xh->target_addr = 0;
	return 0;
}

/**
 * @brief Dispatch the I2C interrupt into the Xilinx driver.
 * @note PL: XIic_InterruptHandler().
 */
static void capi_i2c_pl_isr(void *handle)
{
	if (!handle)
		return;
	struct capi_i2c_xilinx_handle *xh = ((struct capi_i2c_controller_handle *)
					     handle)->priv;
	XIic_InterruptHandler(inst(xh));

	if (xh->callback_pending) {
		capi_i2c_callback callback = xh->callback;
		void *callback_arg = xh->callback_arg;
		enum capi_i2c_async_event event = xh->pending_callback_event;
		int status = xh->pending_callback_status;

		xh->callback_pending = false;
		if (callback)
			callback(event, callback_arg, status);
	}
}

/* Master callbacks report bytes remaining; target transmit reports bytes
 * sent. */
static void xiic_send_handler(void *ref, int byte_count)
{
	struct capi_i2c_xilinx_handle *xh = (struct capi_i2c_xilinx_handle *)ref;

	if (xh->async_phase == CAPI_I2C_ASYNC_RX_SUBADDR) {
		/* Finish the receive phase before notifying CAPI. */
		xh->async_phase = CAPI_I2C_ASYNC_RX;
		if (!xh->async_no_stop)
			inst(xh)->Options &= ~XII_REPEATED_START_OPTION;
		int status = XIic_MasterRecv(inst(xh), xh->async_rx_buf,
					     (int)xh->async_rx_len);
		if (status != XST_SUCCESS)
			xiic_complete_transfer(xh, -EIO, CAPI_I2C_NONE);
		return;
	}
	if (!xh->is_initiator) {
		if (xh->async_phase != CAPI_I2C_ASYNC_TX) {
			xh->target_request_event = 0;
			return;
		}
		int status = byte_count == (int)xh->async_tx_len ? 0 : -EIO;

		xiic_complete_transfer(xh, status,
				       status == 0 ? CAPI_I2C_XFR_DONE :
				       CAPI_I2C_NONE);
		return;
	}

	xiic_complete_transfer(xh, (byte_count == 0) ? 0 : -EIO,
			       (byte_count == 0) ? CAPI_I2C_XFR_DONE :
			       CAPI_I2C_NONE);
}

static void xiic_recv_handler(void *ref, int byte_count)
{
	struct capi_i2c_xilinx_handle *xh = (struct capi_i2c_xilinx_handle *)ref;
	if (!xh->is_initiator && xh->async_phase != CAPI_I2C_ASYNC_RX) {
		xh->target_request_event = 0;
		return;
	}

	/*
	 * A bounced target receive is armed for more than the caller asked for,
	 * so XIic reports the unused remainder as a non-zero byte_count on a
	 * perfectly good frame. Judge it on the bytes that actually arrived
	 * instead: short of the caller's length is a genuine truncation, at or
	 * over it means the request was satisfied and the surplus is discarded.
	 * XIic counts the armed ByteCount DOWN (unlike XIicPs, which counts up),
	 * hence armed - remaining.
	 */
	if (!xh->is_initiator && xh->async_rx_bounce) {
		uint32_t got = (byte_count >= 0 &&
				(uint32_t)byte_count <= xh->async_rx_len) ?
			       xh->async_rx_len - (uint32_t)byte_count : 0U;
		bool ok = got >= xh->async_rx_caller_len;

		xiic_complete_transfer(xh, ok ? 0 : -EIO,
				       ok ? CAPI_I2C_XFR_DONE : CAPI_I2C_NONE);
		return;
	}

	xiic_complete_transfer(xh, (byte_count == 0) ? 0 : -EIO,
			       (byte_count == 0) ? CAPI_I2C_XFR_DONE :
			       CAPI_I2C_NONE);
}

static void xiic_complete_transfer(struct capi_i2c_xilinx_handle *xh,
				   int status,
				   enum capi_i2c_async_event event)
{
	if (status != 0 && xh->is_initiator) {
		inst(xh)->BNBOnly = FALSE;
		if (XIic_Stop(inst(xh)) != XST_SUCCESS) {
			XIic_Reset(inst(xh));
			XIic_IntrGlobalDisable(inst(xh)->BaseAddress);
			XIic_WriteIier(inst(xh)->BaseAddress, 0);
			inst(xh)->IsStarted = 0;
		}
		inst(xh)->Options &= ~XII_REPEATED_START_OPTION;
	}

	xh->bus_held = xh->is_initiator && status == 0 && xh->async_no_stop;

	/*
	 * Bounced target receive: hand back only what the caller asked for and
	 * drop the surplus the bounce buffer absorbed. Runs on the failure path
	 * too, so the buffer is never leaked.
	 */
	if (xh->async_rx_bounce) {
		if (status == 0 && xh->async_rx_buf)
			memcpy(xh->async_rx_buf, xh->async_rx_bounce,
			       xh->async_rx_caller_len);
		capi_free(xh->async_rx_bounce);
		xh->async_rx_bounce = NULL;
	}
	xh->async_rx_caller_len = 0;

	if (xh->async_tx_temp) {
		capi_free(xh->async_tx_temp);
		xh->async_tx_temp = NULL;
	}

	xh->xfer_done = true;
	xh->xfer_status = status;
	xh->async_phase = CAPI_I2C_ASYNC_IDLE;
	xh->async_tx_buf = NULL;
	xh->async_tx_len = 0;
	xh->async_rx_buf = NULL;
	xh->async_rx_len = 0;
	xh->async_dev_addr = 0;
	xh->async_no_stop = false;
	xh->target_request_event = 0;

	if (xh->callback) {
		xh->pending_callback_event = event;
		xh->pending_callback_status = status;
		xh->callback_pending = true;
	}
}

static bool xiic_start_target_transfer(struct capi_i2c_xilinx_handle *xh,
				       int status_event)
{
	int ret;

	if (xh->is_initiator)
		return false;
	if ((status_event & XII_MASTER_WRITE_EVENT) != 0 &&
	    xh->async_phase == CAPI_I2C_ASYNC_RX) {
		xh->target_request_event = 0;
		/* Arm whatever receive_async chose: bounce buffer, or the
		 * caller's own buffer when it was already large enough. */
		ret = XIic_SlaveRecv(inst(xh),
				     xh->async_rx_bounce ? xh->async_rx_bounce :
				     xh->async_rx_buf,
				     (int)xh->async_rx_len);
	} else if ((status_event & XII_MASTER_READ_EVENT) != 0 &&
		   xh->async_phase == CAPI_I2C_ASYNC_TX) {
		xh->target_request_event = 0;
		XIic_ClearStats(inst(xh));
		ret = XIic_SlaveSend(inst(xh), xh->async_tx_buf,
				     (int)xh->async_tx_len);
	} else {
		return false;
	}

	if (ret != XST_SUCCESS)
		xiic_complete_transfer(xh, xiic_status_to_errno(ret),
				       CAPI_I2C_NONE);
	return true;
}

static void xiic_status_handler(void *ref, int status_event)
{
	struct capi_i2c_xilinx_handle *xh = (struct capi_i2c_xilinx_handle *)ref;
	enum capi_i2c_async_event ev;
	int status = -EIO;

	if ((status_event & XII_ARB_LOST_EVENT) != 0) {
		xiic_complete_transfer(xh, -EIO, CAPI_I2C_ALOSS);
		return;
	}
	if ((status_event & XII_SLAVE_NO_ACK_EVENT) != 0) {
		xiic_complete_transfer(xh, -EIO, CAPI_I2C_NAKD);
		return;
	}
	if (xiic_start_target_transfer(xh, status_event))
		return;
	if ((status_event & XII_MASTER_WRITE_EVENT) != 0) {
		ev = CAPI_I2C_SRXREQ;
		status = 0;
	} else if ((status_event & XII_MASTER_READ_EVENT) != 0) {
		ev = CAPI_I2C_STXREQ;
		status = 0;
	} else {
		ev = CAPI_I2C_NONE;
	}

	/* Armed target transfers complete through the send/receive handlers. */
	if (xh->async_phase != CAPI_I2C_ASYNC_IDLE)
		return;

	xh->target_request_event = status_event;
	if (xh->callback) {
		xh->pending_callback_event = ev;
		xh->pending_callback_status = status;
		xh->callback_pending = true;
	}
}

#endif /* XPAR_XIIC_NUM_INSTANCES */
