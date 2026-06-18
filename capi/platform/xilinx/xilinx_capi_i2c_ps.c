/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Xilinx XIicPs I2C driver (Zynq PS I2C)
 *
 * SDT BSP only. IRQ is explicit via capi_i2c_xilinx_config.
 * capi_irq_init() must be called before async ops will work.
 *
 * Works as initiator or target (100k/400k). As a target the sampling divider
 * must be sized for the bus it will see -- see capi_i2c_ps_configure_bus_speed.
 */

#include <capi_i2c.h>
#include <xilinx_capi_i2c_priv.h>
#include <capi_irq.h>
#include <capi_alloc.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "xinterrupt_wrap.h"

#ifdef XPAR_XIICPS_NUM_INSTANCES

#define I2C_PS_DEFAULT_SCLK_HZ		100000U
#define I2C_PS_MAX_SCLK_HZ		400000U
#define I2C_PS_7BIT_MAX_ADDR		0x7FU
#define I2C_PS_10BIT_MAX_ADDR		0x3FEU
#define I2C_PS_BUS_BUSY_TIMEOUT		100000U

static int capi_i2c_ps_init(struct capi_i2c_controller_handle **handle,
			    const struct capi_i2c_config *config);
static int capi_i2c_ps_deinit(struct capi_i2c_controller_handle *handle);
static int capi_i2c_ps_transmit(struct capi_i2c_device *device,
				struct capi_i2c_transfer *transfer);
static int capi_i2c_ps_receive(struct capi_i2c_device *device,
			       struct capi_i2c_transfer *transfer);
static int capi_i2c_ps_register_callback(struct capi_i2c_controller_handle
		*handle,
		capi_i2c_callback const callback,
		void *const callback_arg);
static int capi_i2c_ps_configure_bus_speed(struct capi_i2c_controller_handle
		*handle,
		enum capi_i2c_speed speed, uint8_t duty_cycle);
static int capi_i2c_ps_transmit_async(struct capi_i2c_device *device,
				      struct capi_i2c_transfer *transfer);
static int capi_i2c_ps_receive_async(struct capi_i2c_device *device,
				     struct capi_i2c_transfer *transfer);
static int capi_i2c_ps_recover_bus(struct capi_i2c_controller_handle *handle);
static int capi_i2c_ps_register_target(struct capi_i2c_controller_handle
				       *handle, uint16_t addr);
static int capi_i2c_ps_unregister_target(struct capi_i2c_controller_handle
		*handle);
static void capi_i2c_ps_isr(void *handle);
static void xiicps_slave_tx_refill(struct capi_i2c_xilinx_handle *xh);
static int xiicps_reset_restore(struct capi_i2c_xilinx_handle *xh);
static void xiicps_status_handler(void *ref, u32 event);
static void xiicps_async_complete(struct capi_i2c_xilinx_handle *xh, int status,
				  enum capi_i2c_async_event ev);

const struct capi_i2c_ops capi_i2c_xilinx_ps_ops = {
	.init = capi_i2c_ps_init,
	.deinit = capi_i2c_ps_deinit,
	.transmit = capi_i2c_ps_transmit,
	.receive = capi_i2c_ps_receive,
	.register_callback = capi_i2c_ps_register_callback,
	.configure_bus_speed = capi_i2c_ps_configure_bus_speed,
	.transmit_async = capi_i2c_ps_transmit_async,
	.receive_async = capi_i2c_ps_receive_async,
	.recover_bus = capi_i2c_ps_recover_bus,
	.register_target = capi_i2c_ps_register_target,
	.unregister_target = capi_i2c_ps_unregister_target,
	.isr = capi_i2c_ps_isr,
};

static XIicPs *inst(struct capi_i2c_xilinx_handle *xh)
{
	return (XIicPs *)xh->instance;
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

static int xiicps_xfer_status_to_errno(s32 status)
{
	switch (status) {
	case XST_SUCCESS:
		return 0;
	case XST_IIC_ARB_LOST:
		return -EIO;
	case XST_FAILURE:
		return -EIO;
	default:
		return -EIO;
	}
}

static int xiicps_wait_bus_free(struct capi_i2c_xilinx_handle *xh)
{
	uint32_t timeout = I2C_PS_BUS_BUSY_TIMEOUT;

	while (XIicPs_BusIsBusy(inst(xh)) && --timeout > 0U)
		;

	return timeout == 0U ? -ETIMEDOUT : 0;
}

static int xiicps_async_bus_available(struct capi_i2c_xilinx_handle *xh)
{
	/* A held bus is available to its current owner. */
	if (XIicPs_GetOptions(inst(xh)) & XIICPS_REP_START_OPTION)
		return 0;

	return XIicPs_BusIsBusy(inst(xh)) ? -EBUSY : 0;
}

static int xiicps_reset_restore(struct capi_i2c_xilinx_handle *xh)
{
	uint32_t clk_hz = xh->clk_freq_hz > 0 ? xh->clk_freq_hz :
			  I2C_PS_DEFAULT_SCLK_HZ;

	XIicPs_Reset(inst(xh));
	/* SetSClk rejects stale transfer-size state after an error. */
	XIicPs_WriteReg(inst(xh)->Config.BaseAddress, XIICPS_TRANS_SIZE_OFFSET, 0U);
	inst(xh)->IsRepeatedStart = 0;
	if (XIicPs_SetSClk(inst(xh), clk_hz) != XST_SUCCESS)
		return -EIO;
	inst(xh)->Options = XIicPs_GetOptions(inst(xh));
	XIicPs_SetStatusHandler(inst(xh), xh, xiicps_status_handler);

	if (!xh->is_initiator)
		XIicPs_SetupSlave(inst(xh), xh->target_addr);

	return 0;
}

/**
 * @brief Abandon an in-flight async transfer without notifying the caller.
 *
 * Silences the hardware first, then drops the software state. Order matters:
 * clearing async_phase while the interrupt is still unmasked leaves a window
 * where a late DATA/COMP lands on a transfer that no longer exists and the
 * status handler runs against freed or reused buffers.
 *
 * Does not fire the callback -- the caller decides whether this cancellation is
 * worth reporting, and when (see capi_i2c_ps_recover_bus, which notifies only
 * after the controller reset has completed).
 */
static void xiicps_cancel_inflight(struct capi_i2c_xilinx_handle *xh)
{
	XIicPs_DisableAllInterrupts(inst(xh)->Config.BaseAddress);

	/*
	 * Drop the BSP's own view of the transfer too. XIicPs tracks Send/Recv
	 * buffer pointers and counts independently of async_phase; leaving them
	 * set means the next SlaveSend/SlaveRecv inherits a stale direction latch
	 * (SlaveInterruptHandler picks send vs recv purely from RecvBufferPtr).
	 */
	inst(xh)->SendBufferPtr = NULL;
	inst(xh)->RecvBufferPtr = NULL;
	inst(xh)->SendByteCount = 0;
	inst(xh)->RecvByteCount = 0;

	if (xh->async_tx_temp) {
		capi_free(xh->async_tx_temp);
		xh->async_tx_temp = NULL;
	}

	if (xh->async_rx_bounce) {
		capi_free(xh->async_rx_bounce);
		xh->async_rx_bounce = NULL;
	}
	xh->async_rx_caller_len = 0;

	xh->xfer_done = true;
	xh->xfer_status = -ECANCELED;
	xh->async_phase = CAPI_I2C_ASYNC_IDLE;
	xh->async_rx_buf = NULL;
	xh->async_rx_len = 0;
	xh->async_tx_len = 0;
	xh->async_dev_addr = 0;
	xh->async_no_stop = false;
}

static int xiicps_role_change_allowed(struct capi_i2c_xilinx_handle *xh)
{
	/*
	 * A transfer genuinely in flight cannot be interrupted. But a lingering
	 * HOLD bit or bus-busy reading after a *completed* transfer (e.g. a NACK
	 * against a bus with no device, which leaves HOLD asserted) is a
	 * recoverable leftover, not an active operation - reset it and proceed
	 * rather than wedging every later role change / reconfigure at -EBUSY.
	 */
	if (xh->async_phase != CAPI_I2C_ASYNC_IDLE)
		return -EBUSY;

	uint32_t control = XIicPs_ReadReg(inst(xh)->Config.BaseAddress,
					  XIICPS_CR_OFFSET);
	if ((control & XIICPS_CR_HOLD_MASK) != 0U || XIicPs_BusIsBusy(inst(xh)))
		return xiicps_reset_restore(xh);

	return 0;
}

static int ps_check_device_fields(const struct capi_i2c_device *device)
{
	if (device->duty_cycle != 0)
		return -ENOTSUP;
	if (device->clk_stretch != 0)
		return -ENOTSUP;
	return 0;
}

/* Vendor transfers assert on out-of-range addresses. */
static int ps_check_addr(uint16_t addr, bool b10addr)
{
	if (b10addr) {
		if (addr > I2C_PS_10BIT_MAX_ADDR)
			return -EINVAL;
	} else if (addr > I2C_PS_7BIT_MAX_ADDR) {
		return -EINVAL;
	}
	return 0;
}

/* XIicPs can prepend a sub-address only with a repeated START. */
static int ps_check_read_subaddr(const struct capi_i2c_transfer *transfer)
{
	if (transfer->sub_address && transfer->sub_address_len > 0 &&
	    !transfer->repeated_start)
		return -ENOTSUP;
	return 0;
}

/**
 * @brief Initialize the CAPI backend instance.
 * @note PS: XIicPs_CfgInitialize()/XIicPs_SetSClk().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_ps_init(struct capi_i2c_controller_handle **handle,
			    const struct capi_i2c_config *config)
{
	if (!handle || !config)
		return -EINVAL;
	if (!config->initiator && config->address > I2C_PS_7BIT_MAX_ADDR)
		return -EINVAL;
	if (config->clk_freq_hz > I2C_PS_MAX_SCLK_HZ)
		return -ENOTSUP;
	if (config->dma_handle != NULL)
		return -ENOTSUP;

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
	h->ops = config->ops ? config->ops : &capi_i2c_xilinx_ps_ops;
	h->priv = xh;
	xh->is_initiator = config->initiator;
	xh->clk_freq_hz = config->clk_freq_hz;

	XIicPs_Config *cfg = XIicPs_LookupConfig((UINTPTR)config->identifier);
	if (!cfg) {
		if (alloc)
			xilinx_i2c_free_allocated_handle(handle);
		else
			xilinx_i2c_clear_app_handle(h);
		return -ENODEV;
	}

	XIicPs *ps = capi_malloc(sizeof(XIicPs));
	if (!ps) {
		if (alloc)
			xilinx_i2c_free_allocated_handle(handle);
		else
			xilinx_i2c_clear_app_handle(h);
		return -ENOMEM;
	}
	memset(ps, 0, sizeof(XIicPs));

	if (XIicPs_CfgInitialize(ps, cfg, cfg->BaseAddress) != XST_SUCCESS) {
		capi_free(ps);
		if (alloc)
			xilinx_i2c_free_allocated_handle(handle);
		else
			xilinx_i2c_clear_app_handle(h);
		return -EIO;
	}

	uint32_t target_clk = (config->clk_freq_hz != 0) ? config->clk_freq_hz :
			      I2C_PS_DEFAULT_SCLK_HZ;
	if (XIicPs_SetSClk(ps, target_clk) != XST_SUCCESS) {
		capi_free(ps);
		if (alloc)
			xilinx_i2c_free_allocated_handle(handle);
		else
			xilinx_i2c_clear_app_handle(h);
		return -EIO;
	}
	xh->clk_freq_hz = target_clk;

	XIicPs_SetStatusHandler(ps, xh, xiicps_status_handler);
	xh->instance = ps;

	if (!config->initiator) {
		XIicPs_SetupSlave(ps, (u16)config->address);
		xh->target_addr = config->address;
	}

	const struct capi_i2c_xilinx_config *xcfg =
		(const struct capi_i2c_xilinx_config *)config->extra;
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
		int ret = capi_irq_connect(irq_id, capi_i2c_ps_isr, h);
		if (ret == 0) {
			capi_irq_set_level_edge_trigger(irq_id, CAPI_IRQ_LEVEL_HIGH);
			ret = capi_irq_enable(irq_id);
		}
		if (ret != 0) {
			capi_free(ps);
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
 * @note PS: XIicPs_Abort()/XIicPs_Reset().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_ps_deinit(struct capi_i2c_controller_handle *handle)
{
	if (!handle)
		return -EINVAL;

	struct capi_i2c_xilinx_handle *xh = handle->priv;
	if (!xh)
		return -EINVAL;

	/*
	 * Abandon anything still in flight before tearing the hardware down.
	 *
	 * A transfer can legitimately be outstanding here: an armed target send
	 * or receive that no initiator ever addressed, or one whose peer gave up
	 * mid-frame. Resetting the controller underneath it leaves async_phase
	 * stuck off IDLE, and for an app-allocated handle (init_allocated false)
	 * the struct outlives this call -- so the next init inherits a handle that
	 * reports itself busy and rejects every transfer with -EBUSY.
	 *
	 * Cancel first, while the instance is still valid: xiicps_cancel_inflight
	 * masks the interrupt and clears both our state and the BSP's buffer
	 * pointers, so the reset below cannot race a late interrupt.
	 */
	if (xh->instance && xh->async_phase != CAPI_I2C_ASYNC_IDLE)
		xiicps_cancel_inflight(xh);

	if (xh->use_irq) {
		capi_irq_disable(xh->irq_id);
		xh->use_irq = false;
	}

	if (xh->instance) {
		XIicPs_Reset(inst(xh));
		XIicPs_ResetHw(inst(xh)->Config.BaseAddress);
		capi_free(xh->instance);
		xh->instance = NULL;
	}

	if (xh->async_tx_temp) {
		capi_free(xh->async_tx_temp);
		xh->async_tx_temp = NULL;
	}

	/*
	 * Reset the software state as well. Only the init_allocated case frees
	 * the handle; otherwise it is the caller's memory and will be reused.
	 */
	xh->async_phase = CAPI_I2C_ASYNC_IDLE;
	xh->async_tx_len = 0;
	xh->async_rx_len = 0;
	xh->xfer_done = true;
	xh->callback = NULL;
	xh->callback_arg = NULL;

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
 * @note PS: XIicPs_MasterSendPolled()/XIicPs_SlaveSendPolled().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_ps_transmit(struct capi_i2c_device *device,
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

	int unsup = ps_check_device_fields(device);
	if (unsup != 0)
		return unsup;

	struct capi_i2c_xilinx_handle *xh = device->controller->priv;
	if (xh->async_phase != CAPI_I2C_ASYNC_IDLE)
		return -EBUSY;
	if (transfer->len > INT32_MAX)
		return -EINVAL;

	int ret = 0;

	if (xh->is_initiator) {
		/* Do not wait for a bus this controller intentionally holds. */
		if (!(XIicPs_GetOptions(inst(xh)) & XIICPS_REP_START_OPTION)) {
			if (xiicps_wait_bus_free(xh) != 0) {
				/* Stuck bus / unresponsive target: recover and report
				 * as an I/O error, not raw -ETIMEDOUT. */
				(void)xiicps_reset_restore(xh);
				ret = -EIO;
				goto done;
			}
		}

		uint16_t addr = transfer->target_addr ? transfer->target_addr :
				device->address;
		ret = ps_check_addr(addr, device->b10addr);
		if (ret != 0)
			goto done;

		if (device->b10addr)
			XIicPs_SetOptions(inst(xh), XIICPS_10_BIT_ADDR_OPTION);
		else
			XIicPs_ClearOptions(inst(xh), XIICPS_10_BIT_ADDR_OPTION);

		uint8_t *tx_buf = transfer->buf;
		size_t tx_len = transfer->len;
		bool need_free = false;

		if (transfer->sub_address && transfer->sub_address_len > 0) {
			if ((uint64_t)transfer->len + transfer->sub_address_len >
			    INT32_MAX) {
				ret = -EINVAL;
				goto done;
			}
			tx_len = transfer->sub_address_len + transfer->len;
			tx_buf = capi_malloc(tx_len);
			if (!tx_buf) {
				ret = -ENOMEM;
				goto done;
			}
			memcpy(tx_buf, transfer->sub_address, transfer->sub_address_len);
			memcpy(tx_buf + transfer->sub_address_len, transfer->buf, transfer->len);
			need_free = true;
		}

		if (transfer->no_stop)
			XIicPs_SetOptions(inst(xh), XIICPS_REP_START_OPTION);
		else
			XIicPs_ClearOptions(inst(xh), XIICPS_REP_START_OPTION);

		/*
		 * XIicPs_MasterSend asserts MsgPtr != NULL even for a zero-length
		 * (address-only probe) transfer, so pass a valid dummy pointer when
		 * there is no data buffer. With tx_len == 0 no bytes are read from it.
		 */
		static uint8_t ps_zero_len_dummy;
		if (!tx_buf)
			tx_buf = &ps_zero_len_dummy;

		/* Polled send returns before STOP reaches the bus. */
		int status =
			XIicPs_MasterSendPolled(inst(xh), tx_buf, tx_len, addr);
		if (need_free)
			capi_free(tx_buf);
		if (status != XST_SUCCESS) {
			ret = xiicps_xfer_status_to_errno(status);
			(void)xiicps_reset_restore(xh);
			goto done;
		}

		if (!transfer->no_stop) {
			XIicPs_ClearOptions(inst(xh), XIICPS_REP_START_OPTION);
			if (xiicps_wait_bus_free(xh) != 0) {
				(void)xiicps_reset_restore(xh);
				ret = -EIO;
			}
		}
	} else {
		/* Same-core loopback must use async target transmit. */
		if (XIicPs_SlaveSendPolled(inst(xh), transfer->buf,
					   transfer->len) != XST_SUCCESS)
			ret = -EIO;
	}

done:
	xh->xfer_done = true;
	xh->xfer_status = ret;
	return ret;
}

/**
 * @brief Run a synchronous receive operation.
 * @note PS: XIicPs_MasterRecvPolled()/XIicPs_SlaveRecvPolled().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_ps_receive(struct capi_i2c_device *device,
			       struct capi_i2c_transfer *transfer)
{
	if (!device || !device->controller || !transfer)
		return -EINVAL;
	if (!transfer->buf || transfer->len == 0 ||
	    (transfer->sub_address_len > 0 && !transfer->sub_address))
		return -EINVAL;

	int unsup = ps_check_device_fields(device);
	if (unsup != 0)
		return unsup;
	unsup = ps_check_read_subaddr(transfer);
	if (unsup != 0)
		return unsup;

	struct capi_i2c_xilinx_handle *xh = device->controller->priv;
	if (xh->async_phase != CAPI_I2C_ASYNC_IDLE)
		return -EBUSY;
	if (transfer->len > INT32_MAX)
		return -EINVAL;

	int ret = 0;

	if (xh->is_initiator) {
		/* Do not wait for a bus this controller intentionally holds. */
		if (!(XIicPs_GetOptions(inst(xh)) & XIICPS_REP_START_OPTION)) {
			if (xiicps_wait_bus_free(xh) != 0) {
				/*
				 * The bus never went idle - either a genuinely stuck
				 * bus or a target that is not responding. Recover the
				 * controller and report it as an I/O error (the
				 * NACK-equivalent the CAPI contract expects), not a
				 * raw -ETIMEDOUT.
				 */
				(void)xiicps_reset_restore(xh);
				ret = -EIO;
				goto done;
			}
		}

		if (device->b10addr)
			XIicPs_SetOptions(inst(xh), XIICPS_10_BIT_ADDR_OPTION);
		else
			XIicPs_ClearOptions(inst(xh), XIICPS_10_BIT_ADDR_OPTION);

		uint16_t addr = transfer->target_addr ? transfer->target_addr :
				device->address;
		ret = ps_check_addr(addr, device->b10addr);
		if (ret != 0)
			goto done;

		if (transfer->sub_address && transfer->sub_address_len > 0 &&
		    transfer->repeated_start) {
			XIicPs_SetOptions(inst(xh), XIICPS_REP_START_OPTION);
			int status = XIicPs_MasterSendPolled(inst(xh), transfer->sub_address,
							     transfer->sub_address_len,
							     addr);
			if (status != XST_SUCCESS) {
				ret = xiicps_xfer_status_to_errno(status);
				(void)xiicps_reset_restore(xh);
				goto done;
			}
		}

		if (!transfer->no_stop)
			XIicPs_ClearOptions(inst(xh), XIICPS_REP_START_OPTION);

		int status = XIicPs_MasterRecvPolled(inst(xh), transfer->buf, transfer->len,
						     addr);
		if (status != XST_SUCCESS) {
			ret = xiicps_xfer_status_to_errno(status);
			(void)xiicps_reset_restore(xh);
			goto done;
		}

		if (!transfer->no_stop) {
			XIicPs_ClearOptions(inst(xh), XIICPS_REP_START_OPTION);
			if (xiicps_wait_bus_free(xh) != 0) {
				(void)xiicps_reset_restore(xh);
				ret = -EIO;
			}
		}
	} else {
		ret = -ENOTSUP;
	}

done:
	xh->xfer_done = true;
	xh->xfer_status = ret;
	return ret;
}

/**
 * @brief Register the CAPI asynchronous callback.
 * @note PS: Stores callback for XIicPs status handler.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_ps_register_callback(struct capi_i2c_controller_handle
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
 * @note PS (XIicPs_SetSClk): writes the CR DIV_A/DIV_B divisors. On the Cadence
 *       PS core those divisors size the clock-enable used by BOTH roles: the
 *       master's generated SCL AND the slave's SDA sampling/glitch-filter
 *       window. A target never drives SCL, but it still samples the bus off this
 *       SetSClk-derived timebase, so a target must be reconfigured to match the
 *       bus it will see -- a target left at 100k cannot correctly sample a 400k
 *       initiator. SetupSlave preserves the divisors, so this is the only call
 *       that sizes them. STANDARD -> 100k, FAST -> 400k (HW clamps to 384.6 kHz
 *       per CR#779290); FAST_PLUS/HIGH/ULTRA and any non-zero duty are -ENOTSUP.
 *       Contrast the PL note: XIic has no such shared window.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_ps_configure_bus_speed(struct capi_i2c_controller_handle
		*handle,
		enum capi_i2c_speed speed, uint8_t duty_cycle)
{
	if (!handle)
		return -EINVAL;
	/* XIicPs cannot set SCL duty cycle. */
	if (duty_cycle != 0)
		return -ENOTSUP;

	struct capi_i2c_xilinx_handle *xh = handle->priv;
	uint32_t speed_hz;

	/*
	 * A transfer genuinely in flight cannot be reconfigured. A lingering
	 * HOLD/REP_START or bus-busy from a *completed* (e.g. NACKed) transfer is
	 * a recoverable leftover - reset it and proceed rather than wedging every
	 * later reconfigure at -EBUSY (see xiicps_role_change_allowed).
	 */
	if (xh->async_phase != CAPI_I2C_ASYNC_IDLE)
		return -EBUSY;

	switch (speed) {
	case CAPI_I2C_SPEED_STANDARD:
		speed_hz = I2C_PS_DEFAULT_SCLK_HZ;
		break;
	case CAPI_I2C_SPEED_FAST:
		speed_hz = I2C_PS_MAX_SCLK_HZ;
		break;
	case CAPI_I2C_SPEED_FAST_PLUS:
	case CAPI_I2C_SPEED_HIGH:
	case CAPI_I2C_SPEED_ULTRA:
		return -ENOTSUP;
	default:
		return -EINVAL;
	}

	uint32_t control = XIicPs_ReadReg(inst(xh)->Config.BaseAddress,
					  XIICPS_CR_OFFSET);
	if ((control & XIICPS_CR_HOLD_MASK) != 0U ||
	    (XIicPs_GetOptions(inst(xh)) & XIICPS_REP_START_OPTION) != 0U ||
	    XIicPs_BusIsBusy(inst(xh))) {
		/* SetSClk requires an idle controller; clear the leftover state. */
		if (xiicps_reset_restore(xh) != 0)
			return -EIO;
	}
	if (XIicPs_SetSClk(inst(xh), speed_hz) != XST_SUCCESS)
		return -EIO;

	xh->clk_freq_hz = speed_hz;
	return 0;
}

/**
 * @brief Start an asynchronous transmit operation.
 * @note PS: XIicPs_MasterSend()/XIicPs_SlaveSend().
 *
 * Non-blocking: this arms the hardware and returns immediately. Completion is
 * reported later from the interrupt path (xiicps_status_handler ->
 * xiicps_async_complete), which is where xfer_done/xfer_status are finalized,
 * any temp buffer is freed, and the CAPI callback fires.
 *
 * Flow, initiator role:
 *   1. Reject if no IRQ (async needs it) or a transfer is already in flight.
 *   2. Confirm the bus is free (or intentionally held from a no_stop frame).
 *   3. Select 7/10-bit addressing.
 *   4. If a sub-address is present, concatenate [sub_address || data] into a
 *      temp buffer that must outlive this call (freed in the ISR completion).
 *   5. Map no_stop to the repeated-start/HOLD option.
 *   6. Set phase = TX and kick XIicPs_MasterSend, then return 0.
 * Target role: just arm XIicPs_SlaveSend; the ISR completes it when an
 * initiator clocks the bytes out.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_ps_transmit_async(struct capi_i2c_device *device,
				      struct capi_i2c_transfer *transfer)
{
	if (!device || !device->controller || !transfer)
		return -EINVAL;
	/* Zero-length write is a valid address-only probe; buf only needed for data. */
	if ((transfer->len != 0 && !transfer->buf) ||
	    (transfer->sub_address_len > 0 && !transfer->sub_address))
		return -EINVAL;

	int unsup = ps_check_device_fields(device);
	if (unsup != 0)
		return unsup;

	struct capi_i2c_xilinx_handle *xh = device->controller->priv;
	if (!xh->use_irq)
		return -ENOTSUP;
	if (xh->async_phase != CAPI_I2C_ASYNC_IDLE)
		return -EBUSY;
	if (transfer->len > INT32_MAX)
		return -EINVAL;

	if (xh->is_initiator) {
		uint16_t send_addr = transfer->target_addr ? transfer->target_addr :
				     device->address;
		int ret = ps_check_addr(send_addr, device->b10addr);
		if (ret != 0)
			return ret;

		ret = xiicps_async_bus_available(xh);
		if (ret != 0)
			return ret;

		if (device->b10addr)
			XIicPs_SetOptions(inst(xh), XIICPS_10_BIT_ADDR_OPTION);
		else
			XIicPs_ClearOptions(inst(xh), XIICPS_10_BIT_ADDR_OPTION);

		uint8_t *tx_buf = transfer->buf;
		size_t tx_len = transfer->len;

		if (transfer->sub_address && transfer->sub_address_len > 0) {
			if ((uint64_t)transfer->len + transfer->sub_address_len >
			    INT32_MAX)
				return -EINVAL;
			tx_len = transfer->sub_address_len + transfer->len;
			tx_buf = capi_malloc(tx_len);
			if (!tx_buf)
				return -ENOMEM;
			memcpy(tx_buf, transfer->sub_address, transfer->sub_address_len);
			memcpy(tx_buf + transfer->sub_address_len, transfer->buf, transfer->len);
			/* The buffer must outlive the interrupt-driven transfer. */
			xh->async_tx_temp = tx_buf;
		} else {
			xh->async_tx_temp = NULL;
		}

		if (transfer->no_stop)
			XIicPs_SetOptions(inst(xh), XIICPS_REP_START_OPTION);
		else
			XIicPs_ClearOptions(inst(xh), XIICPS_REP_START_OPTION);

		uint16_t addr = transfer->target_addr ? transfer->target_addr :
				device->address;

		/*
		 * XIicPs_MasterSend asserts MsgPtr != NULL even for a zero-length
		 * (address-only probe) transfer; pass a dummy pointer when there is
		 * no data buffer. With tx_len == 0 no bytes are read from it.
		 */
		static uint8_t ps_async_zero_len_dummy;
		if (!tx_buf)
			tx_buf = &ps_async_zero_len_dummy;

		xh->xfer_done = false;
		xh->xfer_status = 0;
		xh->async_phase = CAPI_I2C_ASYNC_TX;
		XIicPs_MasterSend(inst(xh), tx_buf, tx_len, addr);
	} else {
		xh->xfer_done = false;
		xh->xfer_status = 0;
		xh->async_phase = CAPI_I2C_ASYNC_TX;

		/*
		 * Clear the TX FIFO before arming the send.
		 *
		 * XIicPs_SlaveSend() preloads nothing: it only records the buffer and
		 * unmasks DATA/COMP, leaving the ISR's TransmitFifoFill() to do every
		 * write. That helper sizes each burst as
		 *
		 *     AvailBytes = XIICPS_FIFO_DEPTH - read(XIICPS_TRANS_SIZE_OFFSET)
		 *
		 * but in SLAVE-TRANSMITTER mode TRANS_SIZE is not a live occupancy
		 * counter -- xiicps_hw.h defines it there as "number of bytes remaining
		 * in the FIFO after the master terminates the transfer", and only
		 * CLR_FIFO resets it. A residue N left by the previous transaction
		 * therefore under-reports free space by N, the first fill falls N bytes
		 * short, and because SlaveSend never gets a second DATA interrupt once
		 * the master has drained what was loaded, COMP arrives with
		 * SendByteCount > 0 -- which xiicps_slave.c reports as
		 * XIICPS_EVENT_ERROR, with the tail bytes never sent.
		 *
		 * A 1-byte send survives any residue 0..15, so short transfers hide the
		 * bug; XIICPS_FIFO_DEPTH (16) is the first length that needs AvailBytes
		 * at its maximum and so tolerates no residue at all. Clearing the FIFO
		 * here forces TRANS_SIZE to 0 and makes every length behave like the
		 * short ones. This is the same CLR_FIFO that XIicPs_SetupSlave() issues
		 * at setup -- it is just never re-issued per transfer.
		 *
		 * Wait for the bus to go idle first. XIicPs_SlaveInterruptHandler()
		 * raises COMPLETE_SEND as soon as SendByteCount hits 0 -- that is, when
		 * the driver has pushed the last byte INTO the FIFO, not when the FIFO
		 * has drained onto the wire. A caller that re-arms the target the
		 * instant its completion callback fires can therefore land here while
		 * the previous transaction is still clocking out, and clearing the FIFO
		 * underneath it truncates that transfer. The wait is bounded and only
		 * costs anything when a transfer really is still in flight.
		 */
		int busy = xiicps_wait_bus_free(xh);
		if (busy != 0) {
			xh->async_phase = CAPI_I2C_ASYNC_IDLE;
			return busy;
		}

		XIicPs_WriteReg(inst(xh)->Config.BaseAddress, XIICPS_CR_OFFSET,
				XIicPs_ReadReg(inst(xh)->Config.BaseAddress,
					       XIICPS_CR_OFFSET) |
				XIICPS_CR_CLR_FIFO_MASK);

		/*
		 * Widen the bus watchdog to its maximum for the duration.
		 *
		 * TIME_OUT counts SCL cycles the slave holds the clock low. As an
		 * interrupt-driven target we stretch the clock every time the ISR
		 * refills the FIFO, and against a polled initiator sharing this CPU
		 * that stretch lasts as long as it takes the foreground loop to be
		 * scheduled again. The narrower the window, the more often the
		 * watchdog fires on a perfectly healthy transfer. 0xFF is what
		 * XIicPs_ResetHw() itself programs, so it is the BSP's own default
		 * rather than an arbitrary loosening.
		 */
		XIicPs_WriteReg(inst(xh)->Config.BaseAddress,
				XIICPS_TIME_OUT_OFFSET, XIICPS_TO_RESET_VALUE);

		/* Length decides whether the ISR needs to top the FIFO up. */
		xh->async_tx_len = transfer->len;

		XIicPs_SlaveSend(inst(xh), transfer->buf, transfer->len);
	}
	return 0;
}

/**
 * @brief Start an asynchronous receive operation.
 * @note PS: XIicPs_MasterSend()/XIicPs_MasterRecv().
 *
 * Non-blocking: arms the hardware and returns; the interrupt path reports
 * completion. Two initiator sub-cases:
 *
 *   Direct read (no sub-address): set phase = RX, map no_stop to HOLD, and
 *   kick XIicPs_MasterRecv. The status handler completes the CAPI transfer on
 *   COMPLETE_RECV.
 *
 *   Register read (sub_address + repeated_start): a two-phase frame. Phase 1
 *   sends the sub-address with HOLD asserted (bus kept owned) via
 *   XIicPs_MasterSend under phase = RX_SUBADDR. On COMPLETE_SEND the status
 *   handler flips phase RX_SUBADDR -> RX and issues the MasterRecv for phase 2
 *   itself, WITHOUT firing the CAPI callback; only phase 2's COMPLETE_RECV
 *   completes the transfer. The async_rx_* fields saved here are what the
 *   handler reads to launch phase 2.
 *
 * Target receive is unsupported because XIicPs does not bound slave receive
 * writes by ByteCount.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_ps_receive_async(struct capi_i2c_device *device,
				     struct capi_i2c_transfer *transfer)
{
	if (!device || !device->controller || !transfer)
		return -EINVAL;
	if (!transfer->buf || transfer->len == 0 ||
	    (transfer->sub_address_len > 0 && !transfer->sub_address))
		return -EINVAL;

	int unsup = ps_check_device_fields(device);
	if (unsup != 0)
		return unsup;
	unsup = ps_check_read_subaddr(transfer);
	if (unsup != 0)
		return unsup;

	struct capi_i2c_xilinx_handle *xh = device->controller->priv;
	if (!xh->use_irq)
		return -ENOTSUP;
	if (xh->async_phase != CAPI_I2C_ASYNC_IDLE)
		return -EBUSY;
	if (transfer->len > INT32_MAX)
		return -EINVAL;

	/*
	 * Target (slave) role: arm XIicPs_SlaveRecv and let the ISR complete the
	 * transfer when an initiator writes to us (status handler fires
	 * COMPLETE_RECV -> xiicps_async_complete). Mirrors the target branch of
	 * transmit_async (XIicPs_SlaveSend).
	 *
	 * XIicPs_SlaveRecv() takes ByteCount but never uses it -- the vendor
	 * source discards it with "(void)ByteCount;" -- and SlaveRecvData()
	 * drains the RX FIFO into RecvBufferPtr for as long as the initiator
	 * keeps clocking bytes in, with no bound. An initiator sending more
	 * than transfer->len therefore overruns the caller's buffer.
	 *
	 * Work around the missing bound in software: if the caller's buffer is
	 * smaller than the hardware's own max transfer size (the most an
	 * initiator could ever clock into one frame), arm the receive against
	 * an internal bounce buffer sized to that ceiling instead, and copy
	 * back only the caller's requested length on completion
	 * (xiicps_async_complete). A buffer already >= the ceiling can never be
	 * overrun and is armed directly.
	 */
	if (!xh->is_initiator) {
		uint8_t *rx_buf = transfer->buf;
		xh->async_rx_bounce = NULL;
		xh->async_rx_caller_len = transfer->len;

		if (transfer->len < XIICPS_MAX_TRANSFER_SIZE) {
			xh->async_rx_bounce = capi_malloc(XIICPS_MAX_TRANSFER_SIZE);
			if (!xh->async_rx_bounce)
				return -ENOMEM;
			rx_buf = xh->async_rx_bounce;
		}

		xh->async_rx_buf = transfer->buf;
		xh->async_phase = CAPI_I2C_ASYNC_RX;
		xh->xfer_done = false;
		xh->xfer_status = 0;
		/*
		 * RecvByteCount is a free-running up-counter in slave-receive mode
		 * (RecvByteCount++, xiicps_xfer.c) that no vendor call resets per
		 * transfer -- neither SlaveRecv nor Reset/Abort touch it. Since
		 * xiicps_async_complete reads it to size the bounce copy-back, zero
		 * it here so it counts only this transfer. Without this the second
		 * and later target receives copy back a stale tail from an earlier
		 * frame.
		 */
		inst(xh)->RecvByteCount = 0;
		XIicPs_SlaveRecv(inst(xh), rx_buf,
				 xh->async_rx_bounce ?
				 (s32)XIICPS_MAX_TRANSFER_SIZE : (s32)transfer->len);
		return 0;
	}

	uint16_t addr = transfer->target_addr ? transfer->target_addr :
			device->address;
	int ret = ps_check_addr(addr, device->b10addr);
	if (ret != 0)
		return ret;

	ret = xiicps_async_bus_available(xh);
	if (ret != 0)
		return ret;

	if (device->b10addr)
		XIicPs_SetOptions(inst(xh), XIICPS_10_BIT_ADDR_OPTION);
	else
		XIicPs_ClearOptions(inst(xh), XIICPS_10_BIT_ADDR_OPTION);

	if (transfer->sub_address && transfer->sub_address_len > 0 &&
	    transfer->repeated_start) {
		/* The send completion starts the receive phase. */
		xh->async_phase = CAPI_I2C_ASYNC_RX_SUBADDR;
		xh->async_rx_buf = transfer->buf;
		xh->async_rx_len = transfer->len;
		xh->async_dev_addr = addr;
		xh->async_no_stop = transfer->no_stop;
		xh->xfer_done = false;
		xh->xfer_status = 0;

		XIicPs_SetOptions(inst(xh), XIICPS_REP_START_OPTION);
		XIicPs_MasterSend(inst(xh), transfer->sub_address,
				  transfer->sub_address_len, addr);
		return 0;
	}

	xh->async_phase = CAPI_I2C_ASYNC_RX;
	xh->xfer_done = false;
	xh->xfer_status = 0;

	if (transfer->no_stop)
		XIicPs_SetOptions(inst(xh), XIICPS_REP_START_OPTION);
	else
		XIicPs_ClearOptions(inst(xh), XIICPS_REP_START_OPTION);

	XIicPs_MasterRecv(inst(xh), transfer->buf, transfer->len, addr);
	return 0;
}

/**
 * @brief Recover the I2C controller state.
 * @note PS: XIicPs_Reset()/XIicPs_SetSClk().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_ps_recover_bus(struct capi_i2c_controller_handle *handle)
{
	if (!handle)
		return -EINVAL;
	struct capi_i2c_xilinx_handle *xh = handle->priv;

	/*
	 * An async transfer still in flight does NOT block recovery -- it is the
	 * single most common reason recovery is wanted. An armed target receive
	 * that no initiator ever addresses, or a master transfer whose peer went
	 * away mid-frame, both leave async_phase stuck off IDLE forever: the
	 * completion interrupt that would clear it is never going to arrive.
	 * Refusing with -EBUSY in exactly that situation makes the one call that
	 * could unwedge the controller unusable, and since CAPI has no separate
	 * abort/cancel op it leaves the caller with no way out at all -- every
	 * later recover_bus, register_target, unregister_target and role change
	 * fails -EBUSY for the lifetime of the handle.
	 *
	 * So cancel the in-flight operation as part of recovering. The caller is
	 * explicitly asking for a controller reset; an outstanding transfer cannot
	 * survive one, and pretending otherwise only hides the reset from them.
	 */
	bool cancelled = xh->async_phase != CAPI_I2C_ASYNC_IDLE;
	if (cancelled)
		xiicps_cancel_inflight(xh);

	/*
	 * XIicPs has no register to drive the standalone 9-clock SDA-release
	 * sequence (that needs the pins bit-banged as GPIO, which this backend
	 * does not own), so a controller reset is the strongest recovery
	 * available. Perform it best-effort and report success: a persistent
	 * bus-busy reading here often just reflects an idle bus with no external
	 * activity (common right after init on the INTC/no-IRQ XSAs), not a
	 * failure of the recovery itself.
	 */
	int ret = xiicps_reset_restore(xh);

	/*
	 * Notify AFTER the reset, not before: the callback is entitled to start a
	 * new transfer, and resetting on top of one it just armed would silently
	 * destroy it.
	 */
	if (cancelled && xh->callback)
		xh->callback(CAPI_I2C_NONE, xh->callback_arg, -ECANCELED);

	return ret;
}

/**
 * @brief Register an I2C target address.
 * @note PS: XIicPs_SetupSlave().
 *
 * The XIicPs role is not locked at init: XIicPs_SetupSlave clears the
 * master-mode bit and programs the response address at runtime. This call
 * therefore switches any initialized controller into target mode and routes
 * subsequent interrupts to the slave handler (see the ISR dispatch), so a
 * controller opened as initiator can be turned into a target.
 *
 * Fixture note (Cora Z7, coraz7s_gic.xsa): a PS target does NOT inherit the
 * initiator's SCL rate. Its SDA sampling window is sized by XIicPs_SetSClk
 * (see capi_i2c_ps_configure_bus_speed), and SetupSlave preserves the divisors,
 * so a target left at its init-time 100k divider cannot sample a 400k initiator
 * and short-transfers to -EIO. This is a divider mismatch, not an electrical
 * limit: reconfigure the target's speed to match the bus (the loopback fixture
 * calls configure_bus_speed on the target each leg) and 400k runs cleanly over
 * the same wiring.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_ps_register_target(struct capi_i2c_controller_handle
				       *handle, uint16_t addr)
{
	if (!handle)
		return -EINVAL;
	if (addr > I2C_PS_7BIT_MAX_ADDR)
		return -EINVAL;

	struct capi_i2c_xilinx_handle *xh = handle->priv;

	int ret = xiicps_role_change_allowed(xh);
	if (ret != 0)
		return ret;

	XIicPs_SetupSlave(inst(xh), (u16)addr);
	xh->is_initiator = false;
	xh->target_addr = addr;
	return 0;
}

/**
 * @brief Unregister the I2C target address.
 * @note PS: resets the controller and clears the cached target address.
 *
 * Restores initiator (master) mode so the controller can drive transfers
 * again, mirroring register_target's runtime role switch.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_ps_unregister_target(struct capi_i2c_controller_handle
		*handle)
{
	if (!handle)
		return -EINVAL;

	struct capi_i2c_xilinx_handle *xh = handle->priv;

	int ret = xiicps_role_change_allowed(xh);
	if (ret != 0)
		return ret;

	xh->is_initiator = true;
	xh->target_addr = 0;
	return xiicps_reset_restore(xh);
}

/**
 * @brief Top up the slave transmit FIFO ahead of the BSP handler.
 *
 * XIicPs_SlaveInterruptHandler() refills via TransmitFifoFill(), which sizes
 * each burst as
 *
 *     AvailBytes = XIICPS_FIFO_DEPTH - read(XIICPS_TRANS_SIZE_OFFSET)
 *
 * That is only valid where TRANS_SIZE reports live FIFO occupancy. In
 * SLAVE-TRANSMITTER mode it does not: xiicps_hw.h defines it there as "number
 * of bytes remaining in the FIFO after the master terminates the transfer".
 * Reading it mid-transfer under-reports the free space, so the refill comes up
 * short or writes nothing at all; with no bytes queued the controller raises no
 * further DATA interrupt and the transfer stalls with the master still clocking.
 *
 * Transfers up to XIICPS_FIFO_DEPTH are unaffected because the arming path
 * clears the FIFO, making the single initial fill exact. Only a transfer long
 * enough to need a SECOND fill -- i.e. longer than the FIFO -- reaches the bad
 * arithmetic, which is why the failure starts just past 16 bytes.
 *
 * The BSP's own polled path (XIicPs_SlaveSendPolled) sidesteps this by clamping
 * to XIICPS_FIFO_DEPTH and never consulting TRANS_SIZE. Mirror that here: push
 * bytes while the hardware reports room, using SR.TXDV (a real FIFO status bit)
 * rather than TRANS_SIZE. Running before the BSP handler means SendByteCount is
 * normally already 0 by the time TransmitFifoFill() runs, so its faulty sizing
 * becomes a no-op rather than something we must fight.
 */
static void xiicps_slave_tx_refill(struct capi_i2c_xilinx_handle *xh)
{
	XIicPs *iic = inst(xh);
	UINTPTR base = iic->Config.BaseAddress;

	/* Slave transmit only: SlaveSend() nulls RecvBufferPtr, SlaveRecv sets it. */
	if (iic->RecvBufferPtr != NULL || iic->SendBufferPtr == NULL)
		return;
	if (iic->SendByteCount <= 0)
		return;

	/*
	 * Top the FIFO up, without ever waiting.
	 *
	 * This runs in interrupt context on a system where the initiator may be a
	 * POLLED driver on the same CPU. XIic_Recv drains its receive FIFO from a
	 * foreground loop, and each read is what unthrottles the bus so the target
	 * can clock out the next byte. Blocking here -- even on a condition as
	 * seemingly safe as "wait for our TX FIFO to drain" -- deadlocks the
	 * system: the drain cannot happen until the initiator runs, and the
	 * initiator cannot run until this ISR returns. So there is no spin, no
	 * poll, and no wait of any kind below.
	 *
	 * Write up to XIICPS_DATA_INTR_DEPTH bytes. The DATA interrupt is the
	 * hardware saying the initiator has consumed enough that it wants more, so
	 * that much room is available by construction. The TXDV term is a
	 * belt-and-braces stop for the case where the FIFO still reports data
	 * queued once we are past that safe count -- it bounds TX_OVR exposure
	 * without ever blocking on the flag.
	 */
	for (u32 i = 0U; i < (u32)XIICPS_FIFO_DEPTH && iic->SendByteCount > 0; i++) {
		if (i >= (u32)XIICPS_DATA_INTR_DEPTH &&
		    (XIicPs_ReadReg(base, XIICPS_SR_OFFSET) &
		     XIICPS_SR_TXDV_MASK) != 0U)
			break;
		XIicPs_SendByte(iic);
	}
}

/**
 * @brief Dispatch the I2C interrupt into the Xilinx driver.
 * @note PS: XIicPs_MasterInterruptHandler()/SlaveInterruptHandler().
 */
static void capi_i2c_ps_isr(void *handle)
{
	if (!handle)
		return;
	struct capi_i2c_xilinx_handle *xh = ((struct capi_i2c_controller_handle *)
					     handle)->priv;

	if (xh->is_initiator) {
		XIicPs_MasterInterruptHandler(inst(xh));
	} else {
		xiicps_slave_tx_refill(xh);
		XIicPs_SlaveInterruptHandler(inst(xh));
	}
}

static void xiicps_async_complete(struct capi_i2c_xilinx_handle *xh, int status,
				  enum capi_i2c_async_event ev)
{
	/*
	 * Target receive via a bounce buffer (see capi_i2c_ps_receive_async):
	 * judge the frame on how many bytes actually arrived (RecvByteCount, an
	 * up-counter in slave-receive mode -- see xiicps_slave.c's SlaveRecvData
	 * -- zeroed per arm in receive_async). At or over the caller's length the
	 * request is satisfied: hand back exactly that length and drop the
	 * surplus the bounce buffer absorbed. Short of it is a genuine truncation
	 * -> -EIO, copy nothing. This mirrors the PL backend (xiic_recv_handler),
	 * so a short target receive looks identical to a caller on both. Must run
	 * before xiicps_reset_restore(), which touches the same instance state
	 * below.
	 */
	if (xh->async_rx_bounce) {
		if (status == 0 && xh->async_rx_buf) {
			uint32_t got = (uint32_t)inst(xh)->RecvByteCount;
			if (got >= xh->async_rx_caller_len) {
				memcpy(xh->async_rx_buf, xh->async_rx_bounce,
				       xh->async_rx_caller_len);
			} else {
				status = -EIO;
				ev = CAPI_I2C_NONE;
			}
		}
		capi_free(xh->async_rx_bounce);
		xh->async_rx_bounce = NULL;
	}
	xh->async_rx_caller_len = 0;

	if (status != 0)
		(void)xiicps_reset_restore(xh);

	xh->xfer_done = true;
	xh->xfer_status = status;
	xh->async_phase = CAPI_I2C_ASYNC_IDLE;
	xh->async_rx_buf = NULL;
	xh->async_rx_len = 0;
	xh->async_tx_len = 0;
	xh->async_dev_addr = 0;
	xh->async_no_stop = false;

	if (xh->async_tx_temp) {
		capi_free(xh->async_tx_temp);
		xh->async_tx_temp = NULL;
	}

	if (xh->callback)
		xh->callback(ev, xh->callback_arg, status);
}

static void xiicps_status_handler(void *ref, u32 event)
{
	struct capi_i2c_xilinx_handle *xh = (struct capi_i2c_xilinx_handle *)ref;
	if (xh->async_phase == CAPI_I2C_ASYNC_IDLE)
		return;

	if (!xh->is_initiator && xh->async_phase == CAPI_I2C_ASYNC_TX &&
	    (event & XIICPS_EVENT_COMPLETE_SEND)) {
		xiicps_async_complete(xh, 0, CAPI_I2C_XFR_DONE);
		return;
	}
	if (event & XIICPS_EVENT_NACK) {
		xiicps_async_complete(xh, -EIO, CAPI_I2C_NAKD);
		return;
	}
	if (event & XIICPS_EVENT_ARB_LOST) {
		xiicps_async_complete(xh, -EIO, CAPI_I2C_ALOSS);
		return;
	}
	/*
	 * TIME_OUT is not a target-side failure.
	 *
	 * XIICPS_TIME_OUT is a bus watchdog: xiicps_hw.h defines it as firing
	 * "when the accessed slave holds the sclk line low for longer than the
	 * time out period", i.e. it exists to stop a MASTER hanging on a stuck
	 * peer. XIicPs_SlaveSend() enables it regardless of role, so when we are
	 * the target it reports our own legitimate clock stretching -- which is
	 * unavoidable here, since an interrupt-driven target necessarily holds SCL
	 * while its ISR refills the FIFO. Treating that as -EIO fails transfers
	 * whose data crossed the wire intact.
	 *
	 * So ignore it while acting as a target and let the transfer run to its
	 * COMPLETE_SEND/COMPLETE_RECV. As an initiator it keeps its real meaning
	 * (the peer is holding the bus hostage) and is still an error.
	 */
	if ((event & XIICPS_EVENT_TIME_OUT) && !xh->is_initiator)
		event &= ~(u32)XIICPS_EVENT_TIME_OUT;

	if (event & (XIICPS_EVENT_ERROR | XIICPS_EVENT_TIME_OUT)) {
		xiicps_async_complete(xh, -EIO, CAPI_I2C_NONE);
		return;
	}

	/* Masking TIME_OUT above may have left nothing to report. */
	if (event == 0U)
		return;
	if (event & XIICPS_EVENT_RX_OVR) {
		xiicps_async_complete(xh, -EIO, CAPI_I2C_RXOVER);
		return;
	}
	if (event & XIICPS_EVENT_TX_OVR) {
		xiicps_async_complete(xh, -EIO, CAPI_I2C_TXUNDER);
		return;
	}

	if (xh->async_phase == CAPI_I2C_ASYNC_RX_SUBADDR
	    && (event & XIICPS_EVENT_COMPLETE_SEND)) {
		/* Finish the receive phase before notifying CAPI. */
		xh->async_phase = CAPI_I2C_ASYNC_RX;
		if (xh->async_no_stop)
			XIicPs_SetOptions(inst(xh), XIICPS_REP_START_OPTION);
		else
			XIicPs_ClearOptions(inst(xh), XIICPS_REP_START_OPTION);
		XIicPs_MasterRecv(inst(xh), xh->async_rx_buf, xh->async_rx_len,
				  xh->async_dev_addr);
		return;
	}

	if (event & (XIICPS_EVENT_COMPLETE_SEND | XIICPS_EVENT_COMPLETE_RECV))
		xiicps_async_complete(xh, 0, CAPI_I2C_XFR_DONE);
}

#endif /* XPAR_XIICPS_NUM_INSTANCES */
