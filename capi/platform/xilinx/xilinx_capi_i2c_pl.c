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
#include <string.h>
#include "xinterrupt_wrap.h"

#ifdef XPAR_XIIC_NUM_INSTANCES

#define I2C_PL_7BIT_MAX_ADDR		0x7FU
#define I2C_PL_10BIT_MAX_ADDR		0x3FEU
#define I2C_PL_BUS_BUSY_TIMEOUT		100000U

/* Defaults for designs with nonstandard THIGH/TLOW registers. */
#define I2C_PL_DEFAULT_THIGH_OFFSET	0x13CU
#define I2C_PL_DEFAULT_TLOW_OFFSET	0x140U
#define I2C_PL_DEFAULT_DUTY_PERCENT	50U
#define I2C_PL_MIN_DUTY_PERCENT		10U
#define I2C_PL_MAX_DUTY_PERCENT		90U

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
	}

	if (xcfg != NULL && xcfg->use_irq) {
		uint32_t irq_id = xcfg->irq_id;
		int ret = capi_irq_connect(irq_id, capi_i2c_pl_isr, h);
		if (ret == 0) {
			capi_irq_set_level_edge_trigger(irq_id, CAPI_IRQ_LEVEL_HIGH);
			ret = capi_irq_enable(irq_id);
			if (ret == 0) {
				xh->irq_id = irq_id;
				xh->use_irq = true;
			}
		}
		/* Initiator sync operations remain available if IRQ setup fails. */
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

		uint32_t timeout = I2C_PL_BUS_BUSY_TIMEOUT;
		while (!xh->xfer_done && --timeout > 0)
			;
		if (timeout == 0) {
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

		uint32_t timeout = I2C_PL_BUS_BUSY_TIMEOUT;
		while (!xh->xfer_done && --timeout > 0)
			;
		if (timeout == 0) {
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
	unsigned count = XIic_Recv(inst(xh)->BaseAddress, addr, transfer->buf,
				   transfer->len, XIIC_STOP);
	xh->bus_held = false;
	if (count != transfer->len)
		return -EIO;
	return 0;
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
 * @note PL: Optional custom timing registers; standard XIic returns -ENOTSUP.
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
		xh->async_phase = CAPI_I2C_ASYNC_RX;
		xh->xfer_done = false;
		xh->xfer_status = 0;
		xh->async_rx_buf = transfer->buf;
		xh->async_rx_len = transfer->len;
		if ((xh->target_request_event & XII_MASTER_WRITE_EVENT) != 0) {
			xh->target_request_event = 0;
			int status = XIic_SlaveRecv(inst(xh), xh->async_rx_buf,
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
	if (xh->async_phase != CAPI_I2C_ASYNC_IDLE)
		return -EBUSY;

	XIic_Reset(inst(xh));
	xh->bus_held = false;
	inst(xh)->IsStarted = 0;
	inst(xh)->Options = 0;

	/* Reset cannot clock a target-held SDA, so verify the bus became idle. */
	uint32_t timeout = I2C_PL_BUS_BUSY_TIMEOUT;
	while (XIic_CheckIsBusBusy(inst(xh)->BaseAddress) && --timeout > 0U)
		;
	if (timeout == 0U)
		return -EIO;

	if (!xh->is_initiator) {
		if (XIic_SetAddress(inst(xh), XII_ADDR_TO_RESPOND_TYPE,
				    xh->target_addr) != XST_SUCCESS)
			return -EIO;
		if (XIic_Start(inst(xh)) != XST_SUCCESS)
			return -EIO;
	}

	return 0;
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
		ret = XIic_SlaveRecv(inst(xh), xh->async_rx_buf,
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
