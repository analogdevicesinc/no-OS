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
 */

#include <capi_i2c.h>
#include <xilinx_capi_i2c.h>
#include <capi_irq.h>
#include <capi_alloc.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "xinterrupt_wrap.h"
#include "xiicps.h"
#include "xiicps_hw.h"

#define I2C_BUS_BUSY_TIMEOUT 100000

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
static void xiicps_status_handler(void *ref, u32 event);
static void xiicps_async_complete(struct capi_i2c_xilinx_handle *xh, int status,
                                  enum capi_i2c_async_event ev);
static void xiicps_abort_transfer(struct capi_i2c_xilinx_handle *xh,
                                  int status);

struct capi_i2c_ops capi_i2c_xilinx_ps_ops = {
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

static void xiicps_abort_transfer(struct capi_i2c_xilinx_handle *xh, int status)
{
	XIicPs_Abort(inst(xh));
	XIicPs_ClearOptions(inst(xh), XIICPS_REP_START_OPTION);
	XIicPs_SetSClk(inst(xh), xh->clk_freq_hz > 0 ? xh->clk_freq_hz :
	               100000U);
	XIicPs_SetStatusHandler(inst(xh), xh, xiicps_status_handler);
	if (!xh->is_initiator && xh->target_addr != 0)
		XIicPs_SetupSlave(inst(xh), xh->target_addr);

	xh->xfer_done = true;
	xh->xfer_status = status;
	xh->async_phase = CAPI_I2C_ASYNC_IDLE;
	xh->async_rx_buf = NULL;
	xh->async_rx_len = 0;
	xh->async_dev_addr = 0;
	xh->async_no_stop = false;

	if (xh->async_tx_temp) {
		capi_free(xh->async_tx_temp);
		xh->async_tx_temp = NULL;
	}
}

/* XIicPs supports 10-bit addressing, but not per-device timing fields. */
static int ps_check_device_fields(const struct capi_i2c_device *device)
{
	if (device->duty_cycle != 0)
		return -ENOTSUP;
	if (device->clk_stretch != 0)
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
	if (!config->initiator && config->address > 0x7F)
		return -EINVAL;
	if (config->clk_freq_hz > 400000U)
		return -ENOTSUP;

	bool alloc = (*handle == NULL);
	if (alloc) {
		*handle = capi_malloc(sizeof(struct capi_i2c_xilinx_handle));
		if (!*handle)
			return -ENOMEM;
	}

	struct capi_i2c_xilinx_handle *xh = (struct capi_i2c_xilinx_handle *)*handle;
	memset(xh, 0, sizeof(*xh));
	xh->handle.init_allocated = alloc;
	xh->handle.ops = config->ops ? config->ops : &capi_i2c_xilinx_ps_ops;
	xh->is_initiator = config->initiator;
	xh->clk_freq_hz = config->clk_freq_hz;

	XIicPs_Config *cfg = XIicPs_LookupConfig((u32)config->identifier);
	if (!cfg) {
		if (alloc) {
			capi_free(xh);
			*handle = NULL;
		}
		return -ENODEV;
	}

	XIicPs *ps = capi_malloc(sizeof(XIicPs));
	if (!ps) {
		if (alloc) {
			capi_free(xh);
			*handle = NULL;
		}
		return -ENOMEM;
	}
	memset(ps, 0, sizeof(XIicPs));

	if (XIicPs_CfgInitialize(ps, cfg, cfg->BaseAddress) != XST_SUCCESS) {
		capi_free(ps);
		if (alloc) {
			capi_free(xh);
			*handle = NULL;
		}
		return -EIO;
	}

	/* Apply requested bus clock from config; fall back to 100 kHz if zero. */
	uint32_t target_clk = (config->clk_freq_hz != 0) ? config->clk_freq_hz :
	                      100000U;
	if (XIicPs_SetSClk(ps, target_clk) != XST_SUCCESS) {
		capi_free(ps);
		if (alloc) {
			capi_free(xh);
			*handle = NULL;
		}
		return -EIO;
	}
	xh->clk_freq_hz = target_clk;

	XIicPs_SetStatusHandler(ps, xh, xiicps_status_handler);
	xh->instance = ps;

	/* Apply initial slave address for target mode. */
	if (!config->initiator && config->address != 0) {
		XIicPs_SetupSlave(ps, (u16)config->address);
		xh->target_addr = config->address;
	}

	const struct capi_i2c_xilinx_config *xcfg =
	        (const struct capi_i2c_xilinx_config *)config->extra;
	if (xcfg != NULL && xcfg->use_irq) {
		uint32_t irq_id = xcfg->irq_id;
		int ret = capi_irq_connect(irq_id, capi_i2c_ps_isr, xh);
		if (ret == 0) {
			capi_irq_set_level_edge_trigger(irq_id, CAPI_IRQ_LEVEL_HIGH);
			ret = capi_irq_enable(irq_id);
			if (ret == 0) {
				xh->irq_id = irq_id;
				xh->use_irq = true;
			}
		}
	}

	*handle = (struct capi_i2c_controller_handle *)xh;
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

	struct capi_i2c_xilinx_handle *xh = (struct capi_i2c_xilinx_handle *)handle;

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

	if (xh->handle.init_allocated)
		capi_free(xh);

	return 0;
}

/**
 * @brief Run a synchronous transmit operation.
 * @note PS: XIicPs_MasterSend()/XIicPs_SlaveSend().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_ps_transmit(struct capi_i2c_device *device,
                                struct capi_i2c_transfer *transfer)
{
	if (!device || !device->controller || !transfer)
		return -EINVAL;
	if (!transfer->buf || transfer->len == 0)
		return -EINVAL;

	int unsup = ps_check_device_fields(device);
	if (unsup != 0)
		return unsup;

	struct capi_i2c_xilinx_handle *xh = (struct capi_i2c_xilinx_handle *)
	                                    device->controller;

	if (xh->is_initiator) {
		uint32_t timeout = I2C_BUS_BUSY_TIMEOUT;
		while (XIicPs_BusIsBusy(inst(xh)) && --timeout > 0)
			;
		if (timeout == 0)
			return -ETIMEDOUT;

		if (device->b10addr)
			XIicPs_SetOptions(inst(xh), XIICPS_10_BIT_ADDR_OPTION);
		else
			XIicPs_ClearOptions(inst(xh), XIICPS_10_BIT_ADDR_OPTION);

		uint8_t *tx_buf = transfer->buf;
		size_t tx_len = transfer->len;
		bool need_free = false;

		if (transfer->sub_address && transfer->sub_address_len > 0) {
			if (transfer->len > SIZE_MAX - transfer->sub_address_len)
				return -EINVAL;
			tx_len = transfer->sub_address_len + transfer->len;
			tx_buf = capi_malloc(tx_len);
			if (!tx_buf)
				return -ENOMEM;
			memcpy(tx_buf, transfer->sub_address, transfer->sub_address_len);
			memcpy(tx_buf + transfer->sub_address_len, transfer->buf, transfer->len);
			need_free = true;
		}

		if (transfer->no_stop)
			XIicPs_SetOptions(inst(xh), XIICPS_REP_START_OPTION);
		else
			XIicPs_ClearOptions(inst(xh), XIICPS_REP_START_OPTION);

		if (xh->use_irq) {
			/* Avoid mixing XIicPs polled and IRQ transfer state. */
			xh->xfer_done = false;
			xh->xfer_status = 0;
			xh->async_phase = CAPI_I2C_ASYNC_TX;
			xh->async_tx_temp = need_free ? tx_buf : NULL;
			XIicPs_MasterSend(inst(xh), tx_buf, tx_len, device->address);
			uint32_t tout = I2C_BUS_BUSY_TIMEOUT;
			while (!xh->xfer_done && --tout > 0)
				;
			if (tout == 0) {
				xiicps_abort_transfer(xh, -ETIMEDOUT);
				return -ETIMEDOUT;
			}
			if (xh->xfer_status != 0) {
				XIicPs_ClearOptions(inst(xh), XIICPS_REP_START_OPTION);
				return -EIO;
			}
		} else {
			int status =
			        XIicPs_MasterSendPolled(inst(xh), tx_buf, tx_len, device->address);
			if (need_free)
				capi_free(tx_buf);
			if (status != XST_SUCCESS) {
				XIicPs_ClearOptions(inst(xh), XIICPS_REP_START_OPTION);
				return -EIO;
			}
		}

		if (!transfer->no_stop) {
			XIicPs_ClearOptions(inst(xh), XIICPS_REP_START_OPTION);
			timeout = I2C_BUS_BUSY_TIMEOUT;
			while (XIicPs_BusIsBusy(inst(xh)) && --timeout > 0)
				;
			if (timeout == 0)
				return -ETIMEDOUT;
		}
	} else {
		if (XIicPs_SlaveSendPolled(inst(xh), transfer->buf,
		                           transfer->len) != XST_SUCCESS)
			return -EIO;
	}
	return 0;
}

/**
 * @brief Run a synchronous receive operation.
 * @note PS: XIicPs_MasterRecv()/XIicPs_SlaveRecv().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_ps_receive(struct capi_i2c_device *device,
                               struct capi_i2c_transfer *transfer)
{
	if (!device || !device->controller || !transfer)
		return -EINVAL;
	if (!transfer->buf || transfer->len == 0)
		return -EINVAL;

	int unsup = ps_check_device_fields(device);
	if (unsup != 0)
		return unsup;

	struct capi_i2c_xilinx_handle *xh = (struct capi_i2c_xilinx_handle *)
	                                    device->controller;

	if (xh->is_initiator) {
		/* Skip bus-busy check if HOLD is still asserted from a no_stop TX */
		if (!(XIicPs_GetOptions(inst(xh)) & XIICPS_REP_START_OPTION)) {
			uint32_t timeout = I2C_BUS_BUSY_TIMEOUT;
			while (XIicPs_BusIsBusy(inst(xh)) && --timeout > 0)
				;
			if (timeout == 0)
				return -ETIMEDOUT;
		}

		if (device->b10addr)
			XIicPs_SetOptions(inst(xh), XIICPS_10_BIT_ADDR_OPTION);
		else
			XIicPs_ClearOptions(inst(xh), XIICPS_10_BIT_ADDR_OPTION);

		if (transfer->sub_address && transfer->sub_address_len > 0 &&
		    transfer->repeated_start) {
			/* Send sub-address under repeated-start before RX. */
			XIicPs_SetOptions(inst(xh), XIICPS_REP_START_OPTION);
			if (xh->use_irq) {
				xh->xfer_done = false;
				xh->xfer_status = 0;
				xh->async_phase = CAPI_I2C_ASYNC_TX;
				XIicPs_MasterSend(inst(xh), transfer->sub_address,
				                  transfer->sub_address_len, device->address);
				uint32_t tout = I2C_BUS_BUSY_TIMEOUT;
				while (!xh->xfer_done && --tout > 0)
					;
				if (tout == 0 || xh->xfer_status != 0) {
					if (tout == 0)
						xiicps_abort_transfer(xh, -ETIMEDOUT);
					else
						XIicPs_ClearOptions(inst(xh),
						                    XIICPS_REP_START_OPTION);
					return (tout == 0) ? -ETIMEDOUT : -EIO;
				}
			} else {
				if (XIicPs_MasterSendPolled(inst(xh), transfer->sub_address,
				                            transfer->sub_address_len,
				                            device->address) != XST_SUCCESS) {
					XIicPs_ClearOptions(inst(xh), XIICPS_REP_START_OPTION);
					return -EIO;
				}
			}
		}

		if (!transfer->no_stop)
			XIicPs_ClearOptions(inst(xh), XIICPS_REP_START_OPTION);

		if (xh->use_irq) {
			xh->xfer_done = false;
			xh->xfer_status = 0;
			xh->async_phase = CAPI_I2C_ASYNC_RX;
			XIicPs_MasterRecv(inst(xh), transfer->buf, transfer->len, device->address);
			uint32_t tout = I2C_BUS_BUSY_TIMEOUT;
			while (!xh->xfer_done && --tout > 0)
				;
			if (tout == 0) {
				xiicps_abort_transfer(xh, -ETIMEDOUT);
				return -ETIMEDOUT;
			}
			if (xh->xfer_status != 0) {
				XIicPs_ClearOptions(inst(xh), XIICPS_REP_START_OPTION);
				return -EIO;
			}
		} else {
			if (XIicPs_MasterRecvPolled(inst(xh), transfer->buf, transfer->len,
			                            device->address) != XST_SUCCESS) {
				XIicPs_ClearOptions(inst(xh), XIICPS_REP_START_OPTION);
				return -EIO;
			}
		}

		if (!transfer->no_stop) {
			XIicPs_ClearOptions(inst(xh), XIICPS_REP_START_OPTION);
			uint32_t timeout = I2C_BUS_BUSY_TIMEOUT;
			while (XIicPs_BusIsBusy(inst(xh)) && --timeout > 0)
				;
			if (timeout == 0)
				return -ETIMEDOUT;
		}
	} else {
		if (XIicPs_SlaveRecvPolled(inst(xh), transfer->buf,
		                           transfer->len) != XST_SUCCESS)
			return -EIO;
	}
	return 0;
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
	struct capi_i2c_xilinx_handle *xh = (struct capi_i2c_xilinx_handle *)handle;
	xh->callback = callback;
	xh->callback_arg = callback_arg;
	return 0;
}

/**
 * @brief Configure the I2C bus speed.
 * @note PS: XIicPs_SetSClk().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_ps_configure_bus_speed(struct capi_i2c_controller_handle
                *handle,
                enum capi_i2c_speed speed, uint8_t duty_cycle)
{
	if (!handle)
		return -EINVAL;
	(void)duty_cycle;

	struct capi_i2c_xilinx_handle *xh = (struct capi_i2c_xilinx_handle *)handle;
	uint32_t speed_hz;

	switch (speed) {
		case CAPI_I2C_SPEED_STANDARD:
			speed_hz = 100000;
			break;
		case CAPI_I2C_SPEED_FAST:
			speed_hz = 400000;
			break;
		case CAPI_I2C_SPEED_FAST_PLUS:
		case CAPI_I2C_SPEED_HIGH:
		case CAPI_I2C_SPEED_ULTRA:
			return -ENOTSUP;
		default:
			return -EINVAL;
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
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_ps_transmit_async(struct capi_i2c_device *device,
                                      struct capi_i2c_transfer *transfer)
{
	if (!device || !device->controller || !transfer)
		return -EINVAL;
	if (!transfer->buf || transfer->len == 0)
		return -EINVAL;

	int unsup = ps_check_device_fields(device);
	if (unsup != 0)
		return unsup;

	struct capi_i2c_xilinx_handle *xh = (struct capi_i2c_xilinx_handle *)
	                                    device->controller;
	if (!xh->use_irq)
		return -ENOTSUP;
	if (xh->async_phase != CAPI_I2C_ASYNC_IDLE)
		return -EBUSY;

	if (xh->is_initiator) {
		uint32_t timeout = I2C_BUS_BUSY_TIMEOUT;
		while (XIicPs_BusIsBusy(inst(xh)) && --timeout > 0)
			;
		if (timeout == 0)
			return -ETIMEDOUT;

		if (device->b10addr)
			XIicPs_SetOptions(inst(xh), XIICPS_10_BIT_ADDR_OPTION);
		else
			XIicPs_ClearOptions(inst(xh), XIICPS_10_BIT_ADDR_OPTION);

		uint8_t *tx_buf = transfer->buf;
		size_t tx_len = transfer->len;

		if (transfer->sub_address && transfer->sub_address_len > 0) {
			if (transfer->len > SIZE_MAX - transfer->sub_address_len)
				return -EINVAL;
			tx_len = transfer->sub_address_len + transfer->len;
			tx_buf = capi_malloc(tx_len);
			if (!tx_buf)
				return -ENOMEM;
			memcpy(tx_buf, transfer->sub_address, transfer->sub_address_len);
			memcpy(tx_buf + transfer->sub_address_len, transfer->buf, transfer->len);
			xh->async_tx_temp = tx_buf;
		} else {
			xh->async_tx_temp = NULL;
		}

		if (transfer->no_stop)
			XIicPs_SetOptions(inst(xh), XIICPS_REP_START_OPTION);
		else
			XIicPs_ClearOptions(inst(xh), XIICPS_REP_START_OPTION);

		xh->xfer_done = false;
		xh->xfer_status = 0;
		xh->async_phase = CAPI_I2C_ASYNC_TX;
		XIicPs_MasterSend(inst(xh), tx_buf, tx_len, device->address);
	} else {
		xh->xfer_done = false;
		xh->xfer_status = 0;
		xh->async_phase = CAPI_I2C_ASYNC_TX;
		XIicPs_SlaveSend(inst(xh), transfer->buf, transfer->len);
	}
	return 0;
}

/**
 * @brief Start an asynchronous receive operation.
 * @note PS: XIicPs_MasterSend()/XIicPs_MasterRecv().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_ps_receive_async(struct capi_i2c_device *device,
                                     struct capi_i2c_transfer *transfer)
{
	if (!device || !device->controller || !transfer)
		return -EINVAL;
	if (!transfer->buf || transfer->len == 0)
		return -EINVAL;

	int unsup = ps_check_device_fields(device);
	if (unsup != 0)
		return unsup;

	struct capi_i2c_xilinx_handle *xh = (struct capi_i2c_xilinx_handle *)
	                                    device->controller;
	if (!xh->use_irq)
		return -ENOTSUP;
	if (xh->async_phase != CAPI_I2C_ASYNC_IDLE)
		return -EBUSY;

	if (xh->is_initiator) {
		if (!(XIicPs_GetOptions(inst(xh)) & XIICPS_REP_START_OPTION)) {
			uint32_t timeout = I2C_BUS_BUSY_TIMEOUT;
			while (XIicPs_BusIsBusy(inst(xh)) && --timeout > 0)
				;
			if (timeout == 0)
				return -ETIMEDOUT;
		}

		if (device->b10addr)
			XIicPs_SetOptions(inst(xh), XIICPS_10_BIT_ADDR_OPTION);
		else
			XIicPs_ClearOptions(inst(xh), XIICPS_10_BIT_ADDR_OPTION);

		if (transfer->sub_address && transfer->sub_address_len > 0 &&
		    transfer->repeated_start) {
			xh->async_phase = CAPI_I2C_ASYNC_RX_SUBADDR;
			xh->async_rx_buf = transfer->buf;
			xh->async_rx_len = transfer->len;
			xh->async_dev_addr = device->address;
			xh->async_no_stop = transfer->no_stop;
			xh->xfer_done = false;
			xh->xfer_status = 0;

			XIicPs_SetOptions(inst(xh), XIICPS_REP_START_OPTION);
			XIicPs_MasterSend(inst(xh), transfer->sub_address,
			                  transfer->sub_address_len, device->address);
			return 0;
		}

		xh->async_phase = CAPI_I2C_ASYNC_RX;
		xh->xfer_done = false;
		xh->xfer_status = 0;

		if (transfer->no_stop)
			XIicPs_SetOptions(inst(xh), XIICPS_REP_START_OPTION);
		else
			XIicPs_ClearOptions(inst(xh), XIICPS_REP_START_OPTION);

		XIicPs_MasterRecv(inst(xh), transfer->buf, transfer->len, device->address);
	} else {
		xh->async_phase = CAPI_I2C_ASYNC_RX;
		xh->xfer_done = false;
		xh->xfer_status = 0;
		XIicPs_SlaveRecv(inst(xh), transfer->buf, transfer->len);
	}
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
	struct capi_i2c_xilinx_handle *xh = (struct capi_i2c_xilinx_handle *)handle;

	XIicPs_Reset(inst(xh));
	XIicPs_SetSClk(inst(xh), xh->clk_freq_hz > 0 ? xh->clk_freq_hz : 100000);
	XIicPs_SetStatusHandler(inst(xh), xh, xiicps_status_handler);

	if (!xh->is_initiator && xh->target_addr != 0)
		XIicPs_SetupSlave(inst(xh), xh->target_addr);

	return 0;
}

/**
 * @brief Register an I2C target address.
 * @note PS: XIicPs_SetupSlave().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_ps_register_target(struct capi_i2c_controller_handle
                                       *handle, uint16_t addr)
{
	if (!handle)
		return -EINVAL;
	if (addr > 0x7F)
		return -EINVAL;

	struct capi_i2c_xilinx_handle *xh = (struct capi_i2c_xilinx_handle *)handle;
	if (xh->is_initiator)
		return -ENOTSUP;

	XIicPs_SetupSlave(inst(xh), (u16)addr);
	xh->target_addr = addr;
	return 0;
}

/**
 * @brief Register an I2C target address.
 * @note PS: Clears cached target address.
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_ps_unregister_target(struct capi_i2c_controller_handle
                *handle)
{
	if (!handle)
		return -EINVAL;

	struct capi_i2c_xilinx_handle *xh = (struct capi_i2c_xilinx_handle *)handle;
	if (xh->is_initiator)
		return -ENOTSUP;

	XIicPs_Reset(inst(xh));
	XIicPs_SetSClk(inst(xh), xh->clk_freq_hz > 0 ? xh->clk_freq_hz : 100000);
	XIicPs_SetStatusHandler(inst(xh), xh, xiicps_status_handler);
	xh->target_addr = 0;
	return 0;
}

/**
 * @brief Dispatch the I2C interrupt into the Xilinx driver.
 * @note PS: XIicPs_MasterInterruptHandler()/SlaveInterruptHandler().
 */
static void capi_i2c_ps_isr(void *handle)
{
	if (!handle)
		return;
	struct capi_i2c_xilinx_handle *xh = (struct capi_i2c_xilinx_handle *)handle;

	if (xh->is_initiator)
		XIicPs_MasterInterruptHandler(inst(xh));
	else
		XIicPs_SlaveInterruptHandler(inst(xh));
}

static void xiicps_async_complete(struct capi_i2c_xilinx_handle *xh, int status,
                                  enum capi_i2c_async_event ev)
{
	if (status != 0)
		XIicPs_ClearOptions(inst(xh), XIICPS_REP_START_OPTION);

	xh->xfer_done = true;
	xh->xfer_status = status;
	xh->async_phase = CAPI_I2C_ASYNC_IDLE;
	xh->async_rx_buf = NULL;
	xh->async_rx_len = 0;
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

	if (event & XIICPS_EVENT_NACK) {
		xiicps_async_complete(xh, -EIO, CAPI_I2C_NAKD);
		return;
	}
	if (event & XIICPS_EVENT_ARB_LOST) {
		xiicps_async_complete(xh, -EIO, CAPI_I2C_ALOSS);
		return;
	}
	if (event & (XIICPS_EVENT_ERROR | XIICPS_EVENT_TIME_OUT)) {
		xiicps_async_complete(xh, -EIO, CAPI_I2C_NONE);
		return;
	}
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
