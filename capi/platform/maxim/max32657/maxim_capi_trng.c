/*******************************************************************************
 *   @file   maxim_capi_trng.c
 *   @brief  Implementation of TRNG functions
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include <stdlib.h>
#include <errno.h>
#include "capi_trng.h"
#include "maxim_capi_trng.h"
#include "maxim_capi_irq.h"
#include "trng.h"
#include "max32657.h"

/** Static variables **********************************************************/

static struct capi_trng_handle *trng = NULL;

/** Forward declarations ******************************************************/

void max_capi_trng_isr(void *handle);

/** Helper functions **********************************************************/

/**
 * @brief Callback function to pass to MXC_TRNG_RandomAsync
 * @param req MSDK-managed parameter (not used)
 * @param result The result of the callback
 */
static void _max_capi_trng_msdk_callback(void *req, int result)
{
	struct max_capi_trng_priv *trng_priv;
	enum capi_trng_async_event event;

	if (!trng || !trng->priv)
		return;

	trng_priv = trng->priv;

	if (trng_priv->callback) {
		event = (result == E_NO_ERROR)
			? CAPI_TRNG_EVENT_GENERATION_COMPLETE
			: CAPI_TRNG_EVENT_ERROR;
		trng_priv->callback(event, trng_priv->callback_arg, result);
	}
}

/** Function implementations **************************************************/

/**
 * @brief Initialize the TRNG peripheral
 * @param handle Pointer to the TRNG handle
 * @param config Configuration struct
 * @return 0 on success, negative error code otherwise
 */
int max_capi_trng_init(struct capi_trng_handle **handle,
		       const struct capi_trng_config *config)
{
	int ret;
	struct capi_trng_handle *trng_handle;
	struct max_capi_trng_priv *trng_priv;

	if (!handle || !config)
		return -EINVAL;

	if (config->identifier != 0)
		return -EINVAL;

	if (trng != NULL) {
		*handle = trng;
		return 0;
	}

	if (*handle == NULL) {
		trng_handle = capi_calloc(1, sizeof(*trng_handle));
		if (!trng_handle)
			return -ENOMEM;
		trng_handle->init_allocated = true;
	} else {
		trng_handle = *handle;
		trng_handle->init_allocated = false;
	}

	trng_priv = capi_calloc(1, sizeof(*trng_priv));
	if (!trng_priv) {
		ret = -ENOMEM;
		goto free_handle;
	}

	trng_handle->priv = trng_priv;
	trng_handle->ops = config->ops;

	ret = MXC_TRNG_Init();
	if (ret)
		goto free_priv;

	struct capi_irq_config irq_config = {
		.irq_ctrl_id = 0,
	};

	ret = capi_irq_init(&irq_config);
	if (ret)
		goto free_priv;

	ret = capi_irq_connect(TRNG_IRQn, max_capi_trng_isr, trng_handle);
	if (ret)
		goto free_priv;

	ret = capi_irq_set_priority(TRNG_IRQn, config->irq_priority);
	if (ret)
		goto disconnect_irq;

	ret = capi_irq_enable(TRNG_IRQn);
	if (ret)
		goto disconnect_irq;

	trng = trng_handle;
	*handle = trng_handle;

	return 0;

disconnect_irq:
	capi_irq_connect(TRNG_IRQn, NULL, NULL);
free_priv:
	MXC_TRNG_Shutdown();
	capi_free(trng_priv);
free_handle:
	if (trng_handle->init_allocated)
		capi_free(trng_handle);

	trng = NULL;

	return ret;
}

/**
 * @brief Deinitialize the TRNG peripheral
 * @param handle The TRNG handle
 * @return 0 on success, negative error code otherwise
 */
int max_capi_trng_deinit(struct capi_trng_handle *handle)
{
	int ret;
	struct max_capi_trng_priv *trng_priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	trng_priv = handle->priv;

	capi_irq_disable(TRNG_IRQn);
	capi_irq_connect(TRNG_IRQn, NULL, NULL);

	ret = MXC_TRNG_Shutdown();

	capi_free(handle->priv);

	if (handle->init_allocated)
		capi_free(handle);

	trng = NULL;

	return ret;
}

/**
 * @brief Generate an unsigned 32-bit number (blocking)
 * @param handle The TRNG handle
 * @param value Where to store the unsigned 32-bit number
 * @return 0 on success, negative error code otherwise
 */
int max_capi_trng_generate_u32(struct capi_trng_handle *handle, uint32_t *value)
{
	if (!handle || !handle->priv || !value)
		return -EINVAL;

	*value = (uint32_t)MXC_TRNG_RandomInt();

	return 0;
}

/**
 * @brief Fill a buffer with random bytes (blocking)
 * @param handle The TRNG handle
 * @param buffer The data buffer to store bytes to
 * @param length The length of the buffer
 * @return 0 on success, negative error code otherwise
 */
int max_capi_trng_fill_buffer(struct capi_trng_handle *handle, uint8_t *buffer,
			      uint32_t length)
{
	int ret;

	if (!handle || !handle->priv || !buffer)
		return -EINVAL;

	if (length == 0)
		return -EINVAL;

	ret = MXC_TRNG_Random(buffer, length);
	if (ret != E_NO_ERROR)
		return -EINVAL;

	return 0;
}

/**
 * @brief Fill a buffer with random bytes (non-blocking)
 * @param handle The TRNG handle
 * @param buffer The data buffer to store bytes to
 * @param length The length of the buffer
 * @return 0 on success, negative error code otherwise
 */
int max_capi_trng_fill_buffer_async(struct capi_trng_handle *handle,
				    uint8_t *buffer, uint32_t length)
{
	if (!handle || !handle->priv || !buffer)
		return -EINVAL;

	if (length == 0)
		return -EINVAL;

	MXC_TRNG_RandomAsync(buffer, length, _max_capi_trng_msdk_callback);

	return 0;
}

/**
 * @brief Register a callback
 * @param handle The TRNG handle
 * @param callback The callback function
 * @param callback_arg The callback function argument
 * @return 0 on success, negative error code otherwise
 */
int max_capi_trng_register_callback(struct capi_trng_handle *handle,
				    capi_trng_callback_t callback,
				    void *callback_arg)
{
	struct max_capi_trng_priv *trng_priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	trng_priv = handle->priv;

	trng_priv->callback = callback;
	trng_priv->callback_arg = callback_arg;

	return 0;
}

/**
 * @brief Perform entropy source health test
 * @param handle The TRNG handle
 * @return 0 on success, negative error code otherwise
 */
int max_capi_trng_health_test(struct capi_trng_handle *handle)
{
	int ret;

	if (!handle || !handle->priv)
		return -EINVAL;

	ret = MXC_TRNG_HealthTest();
	if (ret != E_NO_ERROR)
		return -EINVAL;

	return 0;
}

/**
 * @brief Interrupt handler for the TRNG peripheral
 * @param handle The TRNG handle
 */
void max_capi_trng_isr(void *handle)
{
	struct capi_trng_handle *trng_handle = (struct capi_trng_handle *)handle;

	if (!trng_handle || !trng_handle->priv)
		return;

	MXC_TRNG_Handler();
}

const struct capi_trng_ops max_capi_trng_ops = {
	.init = max_capi_trng_init,
	.deinit = max_capi_trng_deinit,
	.generate_u32 = max_capi_trng_generate_u32,
	.fill_buffer = max_capi_trng_fill_buffer,
	.fill_buffer_async = max_capi_trng_fill_buffer_async,
	.register_callback = max_capi_trng_register_callback,
	.health_test = max_capi_trng_health_test,
	.isr = max_capi_trng_isr,
};

