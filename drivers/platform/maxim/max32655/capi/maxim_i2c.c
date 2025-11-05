/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file maxim_i2c.c
 * @brief MAX32655 I2C CAPI implementation
 * @author Claude Code (noreply@anthropic.com)
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "maxim_i2c.h"
#include "capi_i2c.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "i2c.h"
#include "mxc_sys.h"

#define MAX_I2C_INSTANCES     3

/**
 * @brief Convert CAPI I2C speed to Maxim SDK speed
 * @param speed CAPI I2C speed
 * @return Maxim I2C speed
 */
static uint32_t capi_to_maxim_speed(enum capi_i2c_speed speed)
{
	switch (speed) {
	case CAPI_I2C_SPEED_STANDARD:
		return MXC_I2C_STD_MODE;
	case CAPI_I2C_SPEED_FAST:
		return MXC_I2C_FAST_SPEED;
	case CAPI_I2C_SPEED_FAST_PLUS:
		return MXC_I2C_FASTPLUS_SPEED;
	case CAPI_I2C_SPEED_HIGH:
		return MXC_I2C_HIGH_SPEED;
	default:
		return MXC_I2C_FAST_SPEED;
	}
}

/**
 * @brief I2C interrupt callback handler
 * @param req I2C request structure
 * @param result Transfer result
 */
static void maxim_i2c_callback_handler(mxc_i2c_req_t *req, int result)
{
	struct capi_i2c_controller_handle *handle = (struct capi_i2c_controller_handle *)req->callback;
	struct maxim_capi_i2c_handle *priv;
	enum capi_i2c_async_event event;

	// if (!handle || !handle->priv)
	// 	return;

	// priv = (struct maxim_capi_i2c_handle *)handle->priv;
	// priv->transfer_active = false;

	// /* Convert result to CAPI event */
	// if (result == E_NO_ERROR) {
	// 	event = CAPI_I2C_XFR_DONE;
	// } else if (result == E_COMM_ERR) {
	// 	event = CAPI_I2C_NAKD;
	// } else {
	// 	event = CAPI_I2C_TXUNDER;  /* Generic error */
	// }

	// /* Call user callback if registered */
	// if (priv->callback) {
	// 	priv->callback(event, priv->callback_arg, result);
	// }
}

/**
 * @brief Initialize I2C CAPI handle
 * @param handle Double pointer to I2C handle (allocated if *handle is NULL)
 * @param config I2C configuration
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_i2c_init(struct capi_i2c_controller_handle **handle,
			       const struct capi_i2c_config *config)
{
	struct capi_i2c_controller_handle *i2c_handle;
	struct maxim_capi_i2c_handle *priv;
	uint32_t speed;
	int ret;

	if (!handle || !config)
		return -EINVAL;

	if (config->identifier >= MAX_I2C_INSTANCES)
		return -EINVAL;

	/* Handle dynamic vs static allocation */
	if (!*handle) {
		/* Dynamic allocation */
		i2c_handle = no_os_calloc(1, sizeof(struct capi_i2c_controller_handle));
		if (!i2c_handle)
			return -ENOMEM;

		priv = no_os_calloc(1, sizeof(struct maxim_capi_i2c_handle));
		if (!priv) {
			no_os_free(i2c_handle);
			return -ENOMEM;
		}

		i2c_handle->init_allocated = true;
		i2c_handle->priv = priv;
	} else {
		/* Static allocation */
		i2c_handle = *handle;
		priv = (struct maxim_capi_i2c_handle *)i2c_handle->priv;
		if (!priv)
			return -EINVAL;

		/* Clear the private structure */
		memset(priv, 0, sizeof(struct maxim_capi_i2c_handle));
		i2c_handle->init_allocated = false;
	}

	/* Set up the handle */
	i2c_handle->ops = &maxim_capi_i2c_ops;

	/* Initialize private data */
	priv->instance = config->identifier;
	priv->regs = MXC_I2C_GET_I2C(config->identifier);
	priv->clk_freq_hz = config->clk_freq_hz ? config->clk_freq_hz : MXC_I2C_FAST_SPEED;
	priv->initiator = config->initiator;
	priv->address = config->address;
	priv->transfer_active = false;

	/* Convert clock frequency to speed enum */
	if (priv->clk_freq_hz <= 100000) {
		speed = MXC_I2C_STD_MODE;
	} else if (priv->clk_freq_hz <= 400000) {
		speed = MXC_I2C_FAST_SPEED;
	} else if (priv->clk_freq_hz <= 1000000) {
		speed = MXC_I2C_FASTPLUS_SPEED;
	} else {
		speed = MXC_I2C_HIGH_SPEED;
	}

	/* Initialize I2C hardware */
	ret = MXC_I2C_Init(priv->regs, priv->initiator ? 1 : 0, priv->address);
	if (ret != E_NO_ERROR) {
		ret = -EIO;
		goto cleanup;
	}

	/* Set I2C speed */
	ret = MXC_I2C_SetFrequency(priv->regs, speed);
	if (ret < 0) {
		ret = -EIO;
		goto cleanup;
	}

	// mxc_gpio_cfg_t i2c_pins = gpio_cfg_i2c1;
	// i2c_pins.vssel = 1;
	// MXC_GPIO_Config(&i2c_pins);

	*handle = i2c_handle;
	return 0;

cleanup:
	if (i2c_handle->init_allocated) {
		no_os_free(priv);
		no_os_free(i2c_handle);
	}
	return ret;
}

/**
 * @brief Deinitialize I2C CAPI handle
 * @param handle I2C handle to deinitialize
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_i2c_deinit(struct capi_i2c_controller_handle *handle)
{
	struct maxim_capi_i2c_handle *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = (struct maxim_capi_i2c_handle *)handle->priv;

	/* Shutdown I2C */
	MXC_I2C_Shutdown(priv->regs);

	/* Free dynamically allocated memory */
	if (handle->init_allocated) {
		no_os_free(priv);
		no_os_free(handle);
	}

	return 0;
}

/**
 * @brief Synchronous I2C transmit
 * @param device I2C device descriptor
 * @param transfer I2C transfer descriptor
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_i2c_transmit(struct capi_i2c_device *device,
				   struct capi_i2c_transfer *transfer)
{
	struct maxim_capi_i2c_handle *priv;
	mxc_i2c_req_t req;
	int ret;

	if (!device || !device->controller || !device->controller->priv || !transfer)
		return -EINVAL;

	priv = (struct maxim_capi_i2c_handle *)device->controller->priv;

	if (priv->transfer_active)
		return -EBUSY;

	/* Set up I2C request */
	memset(&req, 0, sizeof(req));
	req.i2c = priv->regs;
	req.addr = device->address;
	req.tx_buf = transfer->buf;
	req.tx_len = transfer->len;
	req.rx_buf = NULL;
	req.rx_len = 0;
	req.restart = transfer->repeated_start ? 1 : 0;
	req.callback = NULL;  /* Blocking call */

	/* Handle sub-address if provided */
	if (transfer->sub_address && transfer->sub_address_len > 0) {
		/* For simplicity, we'll prepend sub-address to the data buffer */
		/* In a full implementation, this would be handled more elegantly */
		return -ENOSYS;  /* Sub-address not implemented yet */
	}

	priv->transfer_active = true;
	ret = MXC_I2C_MasterTransaction(&req);
	priv->transfer_active = false;

	if (ret != E_NO_ERROR)
		return -EIO;

	return 0;
}

/**
 * @brief Synchronous I2C receive
 * @param device I2C device descriptor
 * @param transfer I2C transfer descriptor
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_i2c_receive(struct capi_i2c_device *device,
				  struct capi_i2c_transfer *transfer)
{
	struct maxim_capi_i2c_handle *priv;
	mxc_i2c_req_t req;
	int ret;

	if (!device || !device->controller || !device->controller->priv || !transfer)
		return -EINVAL;

	priv = (struct maxim_capi_i2c_handle *)device->controller->priv;

	if (priv->transfer_active)
		return -EBUSY;

	/* Set up I2C request */
	memset(&req, 0, sizeof(req));
	req.i2c = priv->regs;
	req.addr = device->address;
	req.tx_buf = transfer->sub_address;
	req.tx_len = transfer->sub_address_len;
	req.rx_buf = transfer->buf;
	req.rx_len = transfer->len;
	req.restart = transfer->repeated_start ? 1 : 0;
	req.callback = NULL;  /* Blocking call */

	priv->transfer_active = true;
	ret = MXC_I2C_MasterTransaction(&req);
	priv->transfer_active = false;

	if (ret != E_NO_ERROR)
		return -EIO;

	return 0;
}

/**
 * @brief Register callback for I2C events
 * @param handle I2C controller handle
 * @param callback Callback function
 * @param callback_arg Callback argument
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_i2c_register_callback(struct capi_i2c_controller_handle *handle,
					    capi_i2c_callback *const callback,
					    void *const callback_arg)
{
	struct maxim_capi_i2c_handle *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = (struct maxim_capi_i2c_handle *)handle->priv;

	priv->callback = *callback;
	priv->callback_arg = callback_arg;

	return 0;
}

/**
 * @brief Asynchronous I2C transmit
 * @param device I2C device descriptor
 * @param transfer I2C transfer descriptor
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_i2c_transmit_async(struct capi_i2c_device *device,
					 struct capi_i2c_transfer *transfer)
{
	struct maxim_capi_i2c_handle *priv;
	mxc_i2c_req_t *req;
	int ret;

	if (!device || !device->controller || !device->controller->priv || !transfer)
		return -EINVAL;

	priv = (struct maxim_capi_i2c_handle *)device->controller->priv;

	if (priv->transfer_active)
		return -EBUSY;

	/* Use the stored request structure */
	req = &priv->req;
	memset(req, 0, sizeof(*req));
	req->i2c = priv->regs;
	req->addr = device->address;
	req->tx_buf = transfer->buf;
	req->tx_len = transfer->len;
	req->rx_buf = NULL;
	req->rx_len = 0;
	req->restart = transfer->repeated_start ? 1 : 0;
	req->callback = NULL;

	priv->transfer_active = true;
	ret = MXC_I2C_MasterTransactionAsync(req);

	if (ret != E_NO_ERROR) {
		priv->transfer_active = false;
		return -EIO;
	}

	return 0;
}

/**
 * @brief Asynchronous I2C receive
 * @param device I2C device descriptor
 * @param transfer I2C transfer descriptor
 * @return 0 on success, negative error code otherwise
 */
static int maxim_capi_i2c_receive_async(struct capi_i2c_device *device,
					struct capi_i2c_transfer *transfer)
{
	struct maxim_capi_i2c_handle *priv;
	mxc_i2c_req_t *req;
	int ret;

	if (!device || !device->controller || !device->controller->priv || !transfer)
		return -EINVAL;

	priv = (struct maxim_capi_i2c_handle *)device->controller->priv;

	if (priv->transfer_active)
		return -EBUSY;

	/* Use the stored request structure */
	req = &priv->req;
	memset(req, 0, sizeof(*req));
	req->i2c = priv->regs;
	req->addr = device->address;
	req->tx_buf = transfer->sub_address;
	req->tx_len = transfer->sub_address_len;
	req->rx_buf = transfer->buf;
	req->rx_len = transfer->len;
	req->restart = transfer->repeated_start ? 1 : 0;
	req->callback = NULL;

	priv->transfer_active = true;
	ret = MXC_I2C_MasterTransactionAsync(req);

	if (ret != E_NO_ERROR) {
		priv->transfer_active = false;
		return -EIO;
	}

	return 0;
}

/**
 * @brief I2C interrupt service routine
 * @param handle I2C controller handle
 */
static void maxim_capi_i2c_isr(struct capi_i2c_controller_handle *handle)
{
	struct maxim_capi_i2c_handle *priv;

	if (!handle || !handle->priv)
		return;

	priv = (struct maxim_capi_i2c_handle *)handle->priv;

	/* Call Maxim SDK interrupt handler */
	MXC_I2C_AsyncHandler(priv->regs);

	if (priv->callback)
		priv->callback(CAPI_I2C_XFR_DONE, priv->callback_arg, 0);
}

/**
 * @brief MAX32655 I2C CAPI operations structure
 */
const struct capi_i2c_ops maxim_capi_i2c_ops = {
	.init = maxim_capi_i2c_init,
	.deinit = maxim_capi_i2c_deinit,
	.transmit = maxim_capi_i2c_transmit,
	.receive = maxim_capi_i2c_receive,
	.register_callback = maxim_capi_i2c_register_callback,
	.transmit_async = maxim_capi_i2c_transmit_async,
	.receive_async = maxim_capi_i2c_receive_async,
	.isr = maxim_capi_i2c_isr,
};