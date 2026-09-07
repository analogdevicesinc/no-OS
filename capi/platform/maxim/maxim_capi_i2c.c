/*
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file maxim_capi_i2c.c
 * @brief Common Maxim (MSDK) I2C backend.
 *
 * One backend source for every classic-I2C MAX32xxx/MAX78xxx part. Written
 * against the uniform MSDK public API (3-arg MXC_I2C_Init, identical
 * mxc_i2c_req_t, MXC_I2C_MasterTransaction[Async]) so there are no per-part
 * #ifdefs in the transfer flow. Controller mode (blocking + async) is fully
 * supported; target mode is a documented follow-up (returns -ENOSYS).
 */

#include <errno.h>
#include <string.h>
#include "capi_i2c.h"
#include "capi_irq.h"
#include "capi_alloc.h"
#include "maxim_capi_i2c.h"
#include "maxim_capi_i2c_priv.h"
#include "maxim_capi_irq.h"
#include "i2c.h"

/** Static variables **********************************************************/

static struct capi_i2c_controller_handle *i2c[MXC_I2C_INSTANCES] = {NULL};

/** Forward declarations ******************************************************/

void max_capi_i2c_isr(void *handle);

/** Helpers *******************************************************************/

/**
 * @brief Map the CAPI speed enum to a bus frequency in Hz.
 */
static int _max_capi_i2c_speed_hz(enum capi_i2c_speed speed, uint32_t *hz)
{
	switch (speed) {
	case CAPI_I2C_SPEED_STANDARD:
		*hz = 100000;
		break;
	case CAPI_I2C_SPEED_FAST:
		*hz = 400000;
		break;
	case CAPI_I2C_SPEED_FAST_PLUS:
		*hz = 1000000;
		break;
	default:
		/* HIGH / ULTRA are not expressible through the classic I2C API. */
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Translate an MSDK return code into a CAPI/errno code.
 */
static int _max_capi_i2c_err(int msdk_ret)
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
 * @brief MSDK async completion trampoline; maps back to the CAPI callback.
 */
static void _max_capi_i2c_complete_cb(mxc_i2c_req_t *req, int result)
{
	int idx;
	struct capi_i2c_controller_handle *handle;
	struct max_capi_i2c_priv *priv;
	capi_i2c_callback callback;
	void *callback_arg;
	enum capi_i2c_async_event event;

	if (!req)
		return;

	idx = MXC_I2C_GET_IDX(req->i2c);
	if (idx < 0 || idx >= (int)MXC_I2C_INSTANCES)
		return;

	handle = i2c[idx];
	if (!handle || !handle->priv)
		return;

	priv = handle->priv;

	if (priv->async_combined) {
		capi_free(priv->async_combined);
		priv->async_combined = NULL;
	}

	priv->async_in_progress = false;

	event = (result == E_NO_ERROR || result == E_SUCCESS) ?
		CAPI_I2C_XFR_DONE : CAPI_I2C_NAKD;

	callback = priv->callback;
	callback_arg = priv->callback_arg;
	if (callback)
		callback(event, callback_arg, result);
}

/**
 * @brief Build an mxc_i2c_req_t for a write (optionally sub_address+data joined).
 *
 * On return, *combined holds a heap buffer that the caller must free once the
 * transaction is retired (immediately for blocking, in the completion callback
 * for async). NULL means no allocation was needed.
 */
static int _max_capi_i2c_build_write(struct capi_i2c_device *device,
				     struct capi_i2c_transfer *transfer,
				     mxc_i2c_req_t *req, uint8_t **combined)
{
	*combined = NULL;

	if (transfer->sub_address_len > 0 && !transfer->sub_address)
		return -EINVAL;

	req->rx_buf = NULL;
	req->rx_len = 0;

	if (transfer->sub_address && transfer->sub_address_len > 0) {
		uint8_t *buf = capi_malloc(transfer->sub_address_len + transfer->len);

		if (!buf)
			return -ENOMEM;

		memcpy(buf, transfer->sub_address, transfer->sub_address_len);
		memcpy(buf + transfer->sub_address_len, transfer->buf, transfer->len);

		req->tx_buf = buf;
		req->tx_len = transfer->sub_address_len + transfer->len;
		*combined = buf;
	} else {
		req->tx_buf = transfer->buf;
		req->tx_len = transfer->len;
	}

	return 0;
}

/**
 * @brief Build an mxc_i2c_req_t for a read (optionally preceded by a sub_address write).
 */
static int _max_capi_i2c_build_read(struct capi_i2c_device *device,
				    struct capi_i2c_transfer *transfer,
				    mxc_i2c_req_t *req)
{
	if (transfer->sub_address_len > 0 && !transfer->sub_address)
		return -EINVAL;

	req->rx_buf = transfer->buf;
	req->rx_len = transfer->len;

	if (transfer->sub_address && transfer->sub_address_len > 0) {
		req->tx_buf = transfer->sub_address;
		req->tx_len = transfer->sub_address_len;
	} else {
		req->tx_buf = NULL;
		req->tx_len = 0;
	}

	return 0;
}

/** I2C functions implementation **********************************************/

/**
 * @brief Initialize the I2C peripheral.
 * @param handle The I2C handle (allocated here when *handle == NULL).
 * @param config The I2C initialization config.
 * @return 0 on success, negative error code otherwise.
 */
int max_capi_i2c_init(struct capi_i2c_controller_handle **handle,
		      const struct capi_i2c_config *config)
{
	int ret;
	struct capi_i2c_controller_handle *i2c_handle;
	struct max_capi_i2c_priv *priv;
	mxc_i2c_regs_t *regs;
	IRQn_Type irq;
	uint8_t id;

	if (!handle || !config || !config->extra)
		return -EINVAL;

	if (config->identifier >= MXC_I2C_INSTANCES)
		return -EINVAL;

	id = config->identifier;

	/* Re-init of a live instance returns the existing handle. */
	if (i2c[id] != NULL) {
		*handle = i2c[id];
		return 0;
	}

	if (*handle == NULL) {
		i2c_handle = capi_calloc(1, sizeof(*i2c_handle));
		if (!i2c_handle)
			return -ENOMEM;

		priv = capi_calloc(1, sizeof(*priv));
		if (!priv) {
			capi_free(i2c_handle);
			return -ENOMEM;
		}

		i2c_handle->priv = priv;
		i2c_handle->init_allocated = true;
	} else {
		i2c_handle = *handle;

		if (!i2c_handle->priv)
			return -EINVAL;

		priv = i2c_handle->priv;
		i2c_handle->init_allocated = false;
	}

	regs = MXC_I2C_GET_I2C(id);

	i2c_handle->ops = config->ops;
	priv->i2c_handle = i2c_handle;
	priv->regs = regs;
	priv->identifier = id;
	priv->extra = config->extra;
	priv->dma_handle = config->dma_handle;
	priv->freq = config->clk_freq_hz ? config->clk_freq_hz : 100000;

	/* masterMode=1 for controller, slaveAddr ignored in master mode. */
	ret = MXC_I2C_Init(regs, config->initiator ? 1 : 0,
			   config->initiator ? 0 : config->address);
	if (ret != E_NO_ERROR) {
		ret = _max_capi_i2c_err(ret);
		goto free_handle;
	}

	ret = MXC_I2C_SetFrequency(regs, priv->freq);
	if (ret < 0) {
		ret = -EINVAL;
		goto shutdown;
	}

	irq = MXC_I2C_GET_IRQ(id);
	ret = capi_irq_connect(irq, max_capi_i2c_isr, i2c_handle);
	if (ret)
		goto shutdown;

	ret = capi_irq_enable(irq);
	if (ret)
		goto shutdown;

	i2c[id] = i2c_handle;
	*handle = i2c_handle;

	return 0;

shutdown:
	MXC_I2C_Shutdown(regs);
free_handle:
	if (i2c_handle->init_allocated) {
		capi_free(priv);
		capi_free(i2c_handle);
	}

	return ret;
}

/**
 * @brief Deinitialize the I2C peripheral.
 */
int max_capi_i2c_deinit(struct capi_i2c_controller_handle *handle)
{
	int ret;
	struct max_capi_i2c_priv *priv;
	uint8_t id;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;
	id = priv->identifier;

	capi_irq_disable(MXC_I2C_GET_IRQ(id));

	ret = MXC_I2C_Shutdown(priv->regs);

	if (priv->async_combined) {
		capi_free(priv->async_combined);
		priv->async_combined = NULL;
	}

	if (handle->init_allocated) {
		capi_free(priv);
		capi_free(handle);
	}

	i2c[id] = NULL;

	return _max_capi_i2c_err(ret);
}

/**
 * @brief Blocking transmit (controller mode).
 */
int max_capi_i2c_transmit(struct capi_i2c_device *device,
			  struct capi_i2c_transfer *transfer)
{
	struct max_capi_i2c_priv *priv;
	mxc_i2c_req_t req;
	uint8_t *combined;
	int ret;

	if (!device || !device->controller || !device->controller->priv ||
	    !transfer || !transfer->buf)
		return -EINVAL;

	priv = device->controller->priv;

	req = (mxc_i2c_req_t) {
		.i2c = priv->regs,
		.addr = transfer->target_addr ? transfer->target_addr : device->address,
		.restart = transfer->no_stop ? 1 : 0,
		.callback = NULL,
	};

	ret = _max_capi_i2c_build_write(device, transfer, &req, &combined);
	if (ret)
		return ret;

	ret = MXC_I2C_MasterTransaction(&req);

	if (combined)
		capi_free(combined);

	return _max_capi_i2c_err(ret);
}

/**
 * @brief Blocking receive (controller mode).
 */
int max_capi_i2c_receive(struct capi_i2c_device *device,
			 struct capi_i2c_transfer *transfer)
{
	struct max_capi_i2c_priv *priv;
	mxc_i2c_req_t req;
	int ret;

	if (!device || !device->controller || !device->controller->priv ||
	    !transfer || !transfer->buf)
		return -EINVAL;

	priv = device->controller->priv;

	req = (mxc_i2c_req_t) {
		.i2c = priv->regs,
		.addr = transfer->target_addr ? transfer->target_addr : device->address,
		.restart = transfer->no_stop ? 1 : 0,
		.callback = NULL,
	};

	ret = _max_capi_i2c_build_read(device, transfer, &req);
	if (ret)
		return ret;

	ret = MXC_I2C_MasterTransaction(&req);

	return _max_capi_i2c_err(ret);
}

/**
 * @brief Register the async completion callback.
 */
int max_capi_i2c_register_callback(struct capi_i2c_controller_handle *handle,
				   capi_i2c_callback const callback,
				   void *const callback_arg)
{
	struct max_capi_i2c_priv *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;
	priv->callback = callback;
	priv->callback_arg = callback_arg;

	return 0;
}

/**
 * @brief Configure the I2C bus speed.
 * @param duty_cycle Ignored - determined by the MSDK clock divider logic.
 */
int max_capi_i2c_configure_bus_speed(struct capi_i2c_controller_handle *handle,
				     enum capi_i2c_speed speed,
				     uint8_t duty_cycle)
{
	struct max_capi_i2c_priv *priv;
	uint32_t hz;
	int ret;

	(void)duty_cycle;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;

	ret = _max_capi_i2c_speed_hz(speed, &hz);
	if (ret)
		return ret;

	ret = MXC_I2C_SetFrequency(priv->regs, hz);
	if (ret < 0)
		return -EINVAL;

	priv->freq = hz;

	return 0;
}

/**
 * @brief Async transmit (controller mode).
 */
int max_capi_i2c_transmit_async(struct capi_i2c_device *device,
				struct capi_i2c_transfer *transfer)
{
	struct max_capi_i2c_priv *priv;
	int ret;

	if (!device || !device->controller || !device->controller->priv ||
	    !transfer || !transfer->buf)
		return -EINVAL;

	priv = device->controller->priv;

	if (priv->async_in_progress)
		return -EBUSY;

	priv->async_req = (mxc_i2c_req_t) {
		.i2c = priv->regs,
		.addr = transfer->target_addr ? transfer->target_addr : device->address,
		.restart = transfer->no_stop ? 1 : 0,
		.callback = _max_capi_i2c_complete_cb,
	};

	ret = _max_capi_i2c_build_write(device, transfer, &priv->async_req,
					&priv->async_combined);
	if (ret)
		return ret;

	priv->async_in_progress = true;

	ret = MXC_I2C_MasterTransactionAsync(&priv->async_req);
	if (ret != E_NO_ERROR) {
		priv->async_in_progress = false;
		if (priv->async_combined) {
			capi_free(priv->async_combined);
			priv->async_combined = NULL;
		}
		return _max_capi_i2c_err(ret);
	}

	return 0;
}

/**
 * @brief Async receive (controller mode).
 */
int max_capi_i2c_receive_async(struct capi_i2c_device *device,
			       struct capi_i2c_transfer *transfer)
{
	struct max_capi_i2c_priv *priv;
	int ret;

	if (!device || !device->controller || !device->controller->priv ||
	    !transfer || !transfer->buf)
		return -EINVAL;

	priv = device->controller->priv;

	if (priv->async_in_progress)
		return -EBUSY;

	priv->async_req = (mxc_i2c_req_t) {
		.i2c = priv->regs,
		.addr = transfer->target_addr ? transfer->target_addr : device->address,
		.restart = transfer->no_stop ? 1 : 0,
		.callback = _max_capi_i2c_complete_cb,
	};

	ret = _max_capi_i2c_build_read(device, transfer, &priv->async_req);
	if (ret)
		return ret;

	priv->async_combined = NULL;
	priv->async_in_progress = true;

	ret = MXC_I2C_MasterTransactionAsync(&priv->async_req);
	if (ret != E_NO_ERROR) {
		priv->async_in_progress = false;
		return _max_capi_i2c_err(ret);
	}

	return 0;
}

/**
 * @brief Attempt to recover a stuck I2C bus.
 */
int max_capi_i2c_recover_bus(struct capi_i2c_controller_handle *handle)
{
	struct max_capi_i2c_priv *priv;
	int ret;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;

	ret = MXC_I2C_Recover(priv->regs, 16);

	return _max_capi_i2c_err(ret);
}

/**
 * @brief Register in target mode.
 *
 * Target (peripheral) mode is a documented follow-up for the common backend;
 * the classic MSDK slave FSM (MXC_I2C_SlaveTransaction) is not yet wired here.
 */
int max_capi_i2c_register_target(struct capi_i2c_controller_handle *handle,
				 uint16_t addr)
{
	(void)handle;
	(void)addr;

	return -ENOSYS;
}

/**
 * @brief Unregister from target mode. See max_capi_i2c_register_target().
 */
int max_capi_i2c_unregister_target(struct capi_i2c_controller_handle *handle)
{
	(void)handle;

	return -ENOSYS;
}

/**
 * @brief Interrupt entry point; pumps the MSDK async engine.
 */
void max_capi_i2c_isr(void *handle)
{
	struct capi_i2c_controller_handle *i2c_handle;
	struct max_capi_i2c_priv *priv;

	if (!handle)
		return;

	i2c_handle = (struct capi_i2c_controller_handle *)handle;
	if (!i2c_handle->priv)
		return;

	priv = i2c_handle->priv;

	MXC_I2C_AsyncHandler(priv->regs);
}

const struct capi_i2c_ops max_capi_i2c_ops = {
	.init = max_capi_i2c_init,
	.deinit = max_capi_i2c_deinit,
	.transmit = max_capi_i2c_transmit,
	.receive = max_capi_i2c_receive,
	.register_callback = max_capi_i2c_register_callback,
	.configure_bus_speed = max_capi_i2c_configure_bus_speed,
	.transmit_async = max_capi_i2c_transmit_async,
	.receive_async = max_capi_i2c_receive_async,
	.recover_bus = max_capi_i2c_recover_bus,
	.register_target = max_capi_i2c_register_target,
	.unregister_target = max_capi_i2c_unregister_target,
	.isr = max_capi_i2c_isr,
};
