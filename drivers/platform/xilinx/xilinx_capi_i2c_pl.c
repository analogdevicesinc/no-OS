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
#include <xilinx_capi_i2c.h>
#include <capi_irq.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <capi_alloc.h>
#include <string.h>
#include "xinterrupt_wrap.h"
#include "xiic.h"
#include "xiic_l.h"

#define I2C_BUS_BUSY_TIMEOUT 100000

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

struct capi_i2c_ops capi_i2c_xilinx_pl_ops = {
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
	if (!config->initiator && config->address > 0x7F)
		return -EINVAL;

	/* XIic has no runtime SetSClk; bus timing is fixed in the IP. */
	if (config->clk_freq_hz != 0)
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
	xh->handle.ops = config->ops ? config->ops : &capi_i2c_xilinx_pl_ops;
	xh->is_initiator = config->initiator;
	xh->clk_freq_hz = config->clk_freq_hz;

	XIic_Config *cfg = XIic_LookupConfig((UINTPTR)config->identifier);
	if (!cfg) {
		if (alloc) {
			capi_free(xh);
			*handle = NULL;
		}
		return -ENODEV;
	}

	XIic *iic = capi_malloc(sizeof(XIic));
	if (!iic) {
		if (alloc) {
			capi_free(xh);
			*handle = NULL;
		}
		return -ENOMEM;
	}
	memset(iic, 0, sizeof(XIic));

	if (XIic_CfgInitialize(iic, cfg, cfg->BaseAddress) != XST_SUCCESS) {
		capi_free(iic);
		if (alloc) {
			capi_free(xh);
			*handle = NULL;
		}
		return -EIO;
	}

	XIic_SetSendHandler(iic, xh, xiic_send_handler);
	XIic_SetRecvHandler(iic, xh, xiic_recv_handler);
	XIic_SetStatusHandler(iic, xh, xiic_status_handler);
	xh->instance = iic;

	/* Apply initial slave address for target mode. */
	if (!config->initiator && config->address != 0) {
		if (XIic_SetAddress(iic, XII_ADDR_TO_RESPOND_TYPE,
		                    (int)config->address) != XST_SUCCESS) {
			capi_free(iic);
			xh->instance = NULL;
			if (alloc) {
				capi_free(xh);
				*handle = NULL;
			}
			return -EIO;
		}
		xh->target_addr = config->address;
	}

	const struct capi_i2c_xilinx_config *xcfg =
	        (const struct capi_i2c_xilinx_config *)config->extra;
	if (xcfg != NULL && xcfg->use_irq) {
		uint32_t irq_id = xcfg->irq_id;
		int ret = capi_irq_connect(irq_id, capi_i2c_pl_isr, xh);
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
 * @note PL: XIic_Stop()/XIic_Reset().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_pl_deinit(struct capi_i2c_controller_handle *handle)
{
	if (!handle)
		return -EINVAL;

	struct capi_i2c_xilinx_handle *xh = (struct capi_i2c_xilinx_handle *)handle;

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

	if (xh->handle.init_allocated)
		capi_free(xh);

	return 0;
}

/**
 * @brief Run a synchronous transmit operation.
 * @note PL: XIic_Send()/XIic_SlaveSend().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_pl_transmit(struct capi_i2c_device *device,
                                struct capi_i2c_transfer *transfer)
{
	if (!device || !device->controller || !transfer)
		return -EINVAL;
	if (!transfer->buf || transfer->len == 0)
		return -EINVAL;

	struct capi_i2c_xilinx_handle *xh = (struct capi_i2c_xilinx_handle *)
	                                    device->controller;

	if (xh->is_initiator) {
		if (device->b10addr) {
			if (!inst(xh)->Has10BitAddr)
				return -ENOTSUP;
			XIic_SetOptions(inst(xh), XII_SEND_10_BIT_OPTION);
		} else {
			inst(xh)->Options &= ~XII_SEND_10_BIT_OPTION;
		}

		u8 option = transfer->no_stop ? XIIC_REPEATED_START : XIIC_STOP;
		XIic_SetAddress(inst(xh), XII_ADDR_TO_SEND_TYPE, device->address);

		if (transfer->sub_address && transfer->sub_address_len > 0) {
			/* Send sub-address and payload as one XIic write. */
			if (transfer->len > UINT32_MAX - transfer->sub_address_len)
				return -EINVAL;
			uint32_t tx_len = (uint32_t)transfer->sub_address_len + transfer->len;
			uint8_t *tx_buf = capi_malloc(tx_len);
			if (!tx_buf)
				return -ENOMEM;
			memcpy(tx_buf, transfer->sub_address, transfer->sub_address_len);
			memcpy(tx_buf + transfer->sub_address_len, transfer->buf, transfer->len);
			unsigned sent = XIic_Send(inst(xh)->BaseAddress, device->address, tx_buf,
			                          tx_len, option);
			capi_free(tx_buf);
			if (sent != tx_len)
				return -EIO;
		} else {
			unsigned sent = XIic_Send(inst(xh)->BaseAddress, device->address,
			                          transfer->buf, transfer->len, option);
			if (sent != transfer->len)
				return -EIO;
		}
	} else {
		int status = XIic_Start(inst(xh));
		if (status != XST_SUCCESS)
			return -EIO;

		status = XIic_SlaveSend(inst(xh), transfer->buf, transfer->len);
		if (status != XST_SUCCESS) {
			XIic_Stop(inst(xh));
			return -EIO;
		}

		uint32_t timeout = I2C_BUS_BUSY_TIMEOUT;
		while (XIic_IsIicBusy(inst(xh)) == TRUE && --timeout > 0)
			;
		XIic_Stop(inst(xh));
		if (timeout == 0)
			return -ETIMEDOUT;
	}
	return 0;
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
	if (!transfer->buf || transfer->len == 0)
		return -EINVAL;

	struct capi_i2c_xilinx_handle *xh = (struct capi_i2c_xilinx_handle *)
	                                    device->controller;

	if (xh->is_initiator) {
		if (device->b10addr) {
			if (!inst(xh)->Has10BitAddr)
				return -ENOTSUP;
			XIic_SetOptions(inst(xh), XII_SEND_10_BIT_OPTION);
		} else {
			inst(xh)->Options &= ~XII_SEND_10_BIT_OPTION;
		}

		u8 option = transfer->no_stop ? XIIC_REPEATED_START : XIIC_STOP;
		XIic_SetAddress(inst(xh), XII_ADDR_TO_SEND_TYPE, device->address);

		if (transfer->sub_address && transfer->sub_address_len > 0 &&
		    transfer->repeated_start) {
			unsigned count = XIic_Send(inst(xh)->BaseAddress, device->address,
			                           transfer->sub_address, transfer->sub_address_len,
			                           XIIC_REPEATED_START);
			if (count != transfer->sub_address_len)
				return -EIO;
		}

		unsigned count = XIic_Recv(inst(xh)->BaseAddress, device->address,
		                           transfer->buf,
		                           transfer->len, option);
		if (count != transfer->len)
			return -EIO;
	} else {
		int status = XIic_Start(inst(xh));
		if (status != XST_SUCCESS)
			return -EIO;

		status = XIic_SlaveRecv(inst(xh), transfer->buf, transfer->len);
		if (status != XST_SUCCESS) {
			XIic_Stop(inst(xh));
			return -EIO;
		}

		uint32_t timeout = I2C_BUS_BUSY_TIMEOUT;
		while (XIic_IsIicBusy(inst(xh)) == TRUE && --timeout > 0)
			;
		XIic_Stop(inst(xh));
		if (timeout == 0)
			return -ETIMEDOUT;
	}
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
	struct capi_i2c_xilinx_handle *xh = (struct capi_i2c_xilinx_handle *)handle;
	xh->callback = callback;
	xh->callback_arg = callback_arg;
	return 0;
}

/**
 * @brief Configure the I2C bus speed.
 * @note PL: IP-fixed bus timing; returns -ENOTSUP.
 *
 * @return -ENOTSUP.
 */
static int capi_i2c_pl_configure_bus_speed(struct capi_i2c_controller_handle
                *handle,
                enum capi_i2c_speed speed, uint8_t duty_cycle)
{
	(void)handle;
	(void)speed;
	(void)duty_cycle;
	return -ENOTSUP;
}

/**
 * @brief Start an asynchronous transmit operation.
 * @note PL: XIic_Start()/XIic_MasterSend().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_pl_transmit_async(struct capi_i2c_device *device,
                                      struct capi_i2c_transfer *transfer)
{
	if (!device || !device->controller || !transfer)
		return -EINVAL;
	if (!transfer->buf || transfer->len == 0)
		return -EINVAL;

	struct capi_i2c_xilinx_handle *xh = (struct capi_i2c_xilinx_handle *)
	                                    device->controller;
	if (!xh->use_irq)
		return -ENOTSUP;
	if (xh->async_phase != CAPI_I2C_ASYNC_IDLE)
		return -EBUSY;

	if (xh->is_initiator) {
		uint8_t *tx_buf = transfer->buf;
		uint32_t tx_len = transfer->len;

		if (transfer->sub_address && transfer->sub_address_len > 0) {
			if (transfer->len > UINT32_MAX - transfer->sub_address_len)
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
				if (xh->async_tx_temp) {
					capi_free(xh->async_tx_temp);
					xh->async_tx_temp = NULL;
				}
				return -ENOTSUP;
			}
			XIic_SetOptions(inst(xh), XII_SEND_10_BIT_OPTION);
		} else {
			inst(xh)->Options &= ~XII_SEND_10_BIT_OPTION;
		}

		XIic_SetAddress(inst(xh), XII_ADDR_TO_SEND_TYPE, device->address);

		if (transfer->no_stop)
			XIic_SetOptions(inst(xh), XII_REPEATED_START_OPTION);
		else
			inst(xh)->Options &= ~XII_REPEATED_START_OPTION;

		if (XIic_Start(inst(xh)) != XST_SUCCESS) {
			if (xh->async_tx_temp) {
				capi_free(xh->async_tx_temp);
				xh->async_tx_temp = NULL;
			}
			return -EIO;
		}

		xh->async_phase = CAPI_I2C_ASYNC_TX;
		xh->xfer_done = false;
		xh->xfer_status = 0;

		if (XIic_MasterSend(inst(xh), tx_buf, (int)tx_len) != XST_SUCCESS) {
			xh->async_phase = CAPI_I2C_ASYNC_IDLE;
			if (xh->async_tx_temp) {
				capi_free(xh->async_tx_temp);
				xh->async_tx_temp = NULL;
			}
			XIic_Stop(inst(xh));
			return -EIO;
		}
	} else {
		if (XIic_Start(inst(xh)) != XST_SUCCESS)
			return -EIO;
		xh->async_phase = CAPI_I2C_ASYNC_TX;
		xh->xfer_done = false;
		xh->xfer_status = 0;
		if (XIic_SlaveSend(inst(xh), transfer->buf, transfer->len) != XST_SUCCESS) {
			xh->async_phase = CAPI_I2C_ASYNC_IDLE;
			XIic_Stop(inst(xh));
			return -EIO;
		}
	}
	return 0;
}

/**
 * @brief Start an asynchronous receive operation.
 * @note PL: XIic_MasterSend()/XIic_MasterRecv().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_pl_receive_async(struct capi_i2c_device *device,
                                     struct capi_i2c_transfer *transfer)
{
	if (!device || !device->controller || !transfer)
		return -EINVAL;
	if (!transfer->buf || transfer->len == 0)
		return -EINVAL;

	struct capi_i2c_xilinx_handle *xh = (struct capi_i2c_xilinx_handle *)
	                                    device->controller;
	if (!xh->use_irq)
		return -ENOTSUP;
	if (xh->async_phase != CAPI_I2C_ASYNC_IDLE)
		return -EBUSY;

	if (xh->is_initiator) {
		if (device->b10addr) {
			if (!inst(xh)->Has10BitAddr)
				return -ENOTSUP;
			XIic_SetOptions(inst(xh), XII_SEND_10_BIT_OPTION);
		} else {
			inst(xh)->Options &= ~XII_SEND_10_BIT_OPTION;
		}

		XIic_SetAddress(inst(xh), XII_ADDR_TO_SEND_TYPE, device->address);

		if (XIic_Start(inst(xh)) != XST_SUCCESS)
			return -EIO;

		if (transfer->sub_address && transfer->sub_address_len > 0 &&
		    transfer->repeated_start) {
			xh->async_phase = CAPI_I2C_ASYNC_RX_SUBADDR;
			xh->async_rx_buf = transfer->buf;
			xh->async_rx_len = transfer->len;
			xh->async_dev_addr = device->address;
			xh->async_no_stop = transfer->no_stop;
			xh->async_tx_temp = NULL;
			xh->xfer_done = false;
			xh->xfer_status = 0;

			XIic_SetOptions(inst(xh), XII_REPEATED_START_OPTION);

			if (XIic_MasterSend(inst(xh), transfer->sub_address,
			                    transfer->sub_address_len) != XST_SUCCESS) {
				xh->async_phase = CAPI_I2C_ASYNC_IDLE;
				XIic_Stop(inst(xh));
				return -EIO;
			}
			return 0;
		}

		xh->async_phase = CAPI_I2C_ASYNC_RX;
		xh->xfer_done = false;
		xh->xfer_status = 0;

		if (transfer->no_stop)
			XIic_SetOptions(inst(xh), XII_REPEATED_START_OPTION);
		else
			inst(xh)->Options &= ~XII_REPEATED_START_OPTION;

		if (XIic_MasterRecv(inst(xh), transfer->buf, transfer->len) != XST_SUCCESS) {
			xh->async_phase = CAPI_I2C_ASYNC_IDLE;
			XIic_Stop(inst(xh));
			return -EIO;
		}
	} else {
		if (XIic_Start(inst(xh)) != XST_SUCCESS)
			return -EIO;
		xh->async_phase = CAPI_I2C_ASYNC_RX;
		xh->xfer_done = false;
		xh->xfer_status = 0;
		if (XIic_SlaveRecv(inst(xh), transfer->buf, transfer->len) != XST_SUCCESS) {
			xh->async_phase = CAPI_I2C_ASYNC_IDLE;
			XIic_Stop(inst(xh));
			return -EIO;
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
	struct capi_i2c_xilinx_handle *xh = (struct capi_i2c_xilinx_handle *)handle;

	XIic_Reset(inst(xh));
	if (!xh->is_initiator && xh->target_addr != 0)
		XIic_SetAddress(inst(xh), XII_ADDR_TO_RESPOND_TYPE, xh->target_addr);

	return 0;
}

/**
 * @brief Register an I2C target address.
 * @note PL: XIic_SetAddress()/XIic_Start().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_pl_register_target(struct capi_i2c_controller_handle
                                       *handle, uint16_t addr)
{
	if (!handle)
		return -EINVAL;
	if (addr > 0x7F)
		return -EINVAL;

	struct capi_i2c_xilinx_handle *xh = (struct capi_i2c_xilinx_handle *)handle;
	if (xh->is_initiator)
		return -ENOTSUP;

	if (XIic_SetAddress(inst(xh), XII_ADDR_TO_RESPOND_TYPE,
	                    (int)addr) != XST_SUCCESS)
		return -EIO;
	if (XIic_Start(inst(xh)) != XST_SUCCESS)
		return -EIO;

	xh->target_addr = addr;
	return 0;
}

/**
 * @brief Register an I2C target address.
 * @note PL: XIic_Stop().
 *
 * @return 0 on success, negative errno on failure.
 */
static int capi_i2c_pl_unregister_target(struct capi_i2c_controller_handle
                *handle)
{
	if (!handle)
		return -EINVAL;

	struct capi_i2c_xilinx_handle *xh = (struct capi_i2c_xilinx_handle *)handle;
	if (xh->is_initiator)
		return -ENOTSUP;

	XIic_Stop(inst(xh));
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
	struct capi_i2c_xilinx_handle *xh = (struct capi_i2c_xilinx_handle *)handle;
	XIic_InterruptHandler(inst(xh));
}

static void xiic_send_handler(void *ref, int byte_count)
{
	struct capi_i2c_xilinx_handle *xh = (struct capi_i2c_xilinx_handle *)ref;

	if (xh->async_phase == CAPI_I2C_ASYNC_RX_SUBADDR) {
		/* Sub-address phase done; switch to receive */
		xh->async_phase = CAPI_I2C_ASYNC_RX;
		if (!xh->async_no_stop)
			inst(xh)->Options &= ~XII_REPEATED_START_OPTION;
		if (XIic_MasterRecv(inst(xh), xh->async_rx_buf,
		                    (int)xh->async_rx_len) != XST_SUCCESS)
			xiic_complete_transfer(xh, -EIO, CAPI_I2C_NONE);
		return;
	}

	xiic_complete_transfer(xh, (byte_count == 0) ? 0 : -EIO,
	                       (byte_count == 0) ? CAPI_I2C_XFR_DONE :
	                       CAPI_I2C_ALOSS);
}

static void xiic_recv_handler(void *ref, int byte_count)
{
	struct capi_i2c_xilinx_handle *xh = (struct capi_i2c_xilinx_handle *)ref;

	xiic_complete_transfer(xh, (byte_count == 0) ? 0 : -EIO,
	                       (byte_count == 0) ? CAPI_I2C_XFR_DONE :
	                       CAPI_I2C_ALOSS);
}

static void xiic_complete_transfer(struct capi_i2c_xilinx_handle *xh,
                                   int status,
                                   enum capi_i2c_async_event event)
{
	if (status != 0) {
		if (XIic_Stop(inst(xh)) != XST_SUCCESS) {
			XIic_Reset(inst(xh));
			XIic_IntrGlobalDisable(inst(xh)->BaseAddress);
			XIic_WriteIier(inst(xh)->BaseAddress, 0);
			inst(xh)->IsStarted = 0;
		}
		inst(xh)->Options &= ~XII_REPEATED_START_OPTION;
	}

	if (xh->async_tx_temp) {
		capi_free(xh->async_tx_temp);
		xh->async_tx_temp = NULL;
	}

	xh->xfer_done = true;
	xh->xfer_status = status;
	xh->async_phase = CAPI_I2C_ASYNC_IDLE;
	xh->async_rx_buf = NULL;
	xh->async_rx_len = 0;
	xh->async_dev_addr = 0;
	xh->async_no_stop = false;

	if (xh->callback)
		xh->callback(event, xh->callback_arg, status);
}

static void xiic_status_handler(void *ref, int status_event)
{
	struct capi_i2c_xilinx_handle *xh = (struct capi_i2c_xilinx_handle *)ref;
	enum capi_i2c_async_event ev;
	int status = -EIO;

	switch (status_event) {
		case XII_ARB_LOST_EVENT:
			xiic_complete_transfer(xh, -EIO, CAPI_I2C_ALOSS);
			return;
		case XII_SLAVE_NO_ACK_EVENT:
			xiic_complete_transfer(xh, -EIO, CAPI_I2C_NAKD);
			return;
		case XII_MASTER_WRITE_EVENT:
			ev = CAPI_I2C_STXREQ;
			status = 0;
			break;
		case XII_MASTER_READ_EVENT:
			ev = CAPI_I2C_SRXREQ;
			status = 0;
			break;
		default:
			ev = CAPI_I2C_NONE;
			break;
	}

	if (xh->callback)
		xh->callback(ev, xh->callback_arg, status);
}
