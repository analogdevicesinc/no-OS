/*
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include <errno.h>
#include "stm32_capi_i2c_priv.h"
#include "capi_alloc.h"

#define MAX_I2C_INSTANCES		4
#define STM32_I2C_DEFAULT_CLOCK_HZ	100000
#define STM32_I2C_DEFAULT_TIMING	0x10909CECU
#define STM32_I2C_SEQ_TIMEOUT_MS	1000

/* Lookup table for mapping HAL handles to private handles */
static struct {
	I2C_HandleTypeDef *hi2c;
	struct stm32_i2c_priv_handle *priv_handle;
} i2c_handle_lookup[MAX_I2C_INSTANCES];
static uint8_t i2c_handle_count = 0;

/**
 * @brief Find the private handle associated with a HAL I2C handle.
 * @param hi2c - Pointer to the HAL I2C handle.
 * @return Pointer to the private handle, or NULL if not found.
 */
static struct stm32_i2c_priv_handle *find_priv_handle(I2C_HandleTypeDef *hi2c)
{
	for (uint8_t i = 0; i < i2c_handle_count; i++) {
		if (i2c_handle_lookup[i].hi2c == hi2c)
			return i2c_handle_lookup[i].priv_handle;
	}
	return NULL;
}

/**
 * @brief Register a mapping between a HAL I2C handle and a private handle.
 * @param hi2c - Pointer to the HAL I2C handle.
 * @param priv_handle - Pointer to the private handle.
 * @return 0 on success, negative error code otherwise.
 */
static int register_handle_mapping(I2C_HandleTypeDef *hi2c,
				   struct stm32_i2c_priv_handle *priv_handle)
{
	if (i2c_handle_count >= MAX_I2C_INSTANCES)
		return -ENOMEM;

	i2c_handle_lookup[i2c_handle_count].hi2c = hi2c;
	i2c_handle_lookup[i2c_handle_count].priv_handle = priv_handle;
	i2c_handle_count++;
	return 0;
}

/**
 * @brief Unregister the mapping for a HAL I2C handle.
 * @param hi2c - Pointer to the HAL I2C handle.
 */
static void unregister_handle_mapping(I2C_HandleTypeDef *hi2c)
{
	for (uint8_t i = 0; i < i2c_handle_count; i++) {
		if (i2c_handle_lookup[i].hi2c == hi2c) {
			/* Move last entry to this position */
			if (i < i2c_handle_count - 1)
				i2c_handle_lookup[i] = i2c_handle_lookup[i2c_handle_count - 1];
			i2c_handle_count--;
			break;
		}
	}
}

/**
 * @brief Get I2C base address from identifier.
 * @param identifier - I2C peripheral identifier or base address.
 * @return Pointer to the I2C peripheral, or NULL if invalid.
 */
static I2C_TypeDef *get_i2c_base_from_identifier(uint64_t identifier)
{
	if (identifier >= PERIPH_BASE)
		return (I2C_TypeDef *)identifier;

	switch (identifier) {
#if defined(I2C1)
	case 1:
		return I2C1;
#endif
#if defined(I2C2)
	case 2:
		return I2C2;
#endif
#if defined(I2C3)
	case 3:
		return I2C3;
#endif
#if defined(I2C4)
	case 4:
		return I2C4;
#endif
	default:
		return NULL;
	}
}

/**
 * @brief Initialize the STM32 I2C controller.
 * @param handle - Pointer to a pointer of the I2C controller handle.
 * @param config - Pointer to the I2C configuration.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_i2c_init(struct capi_i2c_controller_handle **handle,
			       const struct capi_i2c_config *config)
{
	struct capi_i2c_controller_handle *i2c_handle;
	struct stm32_i2c_priv_handle *priv_handle;
	struct stm32_i2c_extra_config *extra_config;
	I2C_TypeDef *base;
	int ret;

	if (!handle || !config || !config->extra)
		return -EINVAL;

	if (*handle == NULL) {
		i2c_handle = capi_calloc(1, sizeof(*i2c_handle));
		if (!i2c_handle)
			return -ENOMEM;

		priv_handle = capi_calloc(1, sizeof(*priv_handle));
		if (!priv_handle) {
			capi_free(i2c_handle);
			return -ENOMEM;
		}

		i2c_handle->priv = priv_handle;
		i2c_handle->init_allocated = true;
	} else {
		i2c_handle = *handle;
		i2c_handle->init_allocated = false;

		if (!i2c_handle->priv)
			return -EINVAL;

		priv_handle = i2c_handle->priv;
	}
	i2c_handle->ops = config->ops;

	extra_config = config->extra;

	if (extra_config && extra_config->hi2c) {
		priv_handle->hi2c = *extra_config->hi2c;
	} else {
		base = get_i2c_base_from_identifier(config->identifier);
		if (!base) {
			ret = -EINVAL;
			goto error;
		}
		priv_handle->hi2c.Instance = base;
	}

#if defined(STM32F4) || defined(STM32F1) || defined(STM32F2) || defined(STM32L1)
	priv_handle->hi2c.Init.ClockSpeed = config->clk_freq_hz ?
					    config->clk_freq_hz :
					    STM32_I2C_DEFAULT_CLOCK_HZ;
	priv_handle->hi2c.Init.DutyCycle = I2C_DUTYCYCLE_2;
#else
	if (extra_config)
		priv_handle->hi2c.Init.Timing = extra_config->i2c_timing;
	else
		priv_handle->hi2c.Init.Timing = STM32_I2C_DEFAULT_TIMING;
#endif
	priv_handle->hi2c.Init.OwnAddress1 = config->address;
	priv_handle->hi2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	priv_handle->hi2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	priv_handle->hi2c.Init.OwnAddress2 = 0;
	priv_handle->hi2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	priv_handle->hi2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

	ret = HAL_I2C_Init(&priv_handle->hi2c);
	if (ret != HAL_OK) {
		ret = -EIO;
		goto error;
	}

	ret = register_handle_mapping(&priv_handle->hi2c, priv_handle);
	if (ret)
		goto error_deinit;

	priv_handle->callback = NULL;
	priv_handle->callback_arg = NULL;
	priv_handle->async_in_progress = false;

	*handle = i2c_handle;
	return 0;

error_deinit:
	HAL_I2C_DeInit(&priv_handle->hi2c);
error:
	if (i2c_handle->init_allocated) {
		capi_free(priv_handle);
		capi_free(i2c_handle);
	}
	return ret;
}

/**
 * @brief Deinitialize the STM32 I2C controller.
 * @param handle - Pointer to the I2C controller handle.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_i2c_deinit(struct capi_i2c_controller_handle *handle)
{
	struct stm32_i2c_priv_handle *priv_handle;

	if (!handle)
		return -EINVAL;

	priv_handle = handle->priv;

	if (priv_handle) {
		unregister_handle_mapping(&priv_handle->hi2c);
		HAL_I2C_DeInit(&priv_handle->hi2c);
	}

	if (handle->init_allocated) {
		capi_free(priv_handle);
		capi_free(handle);
	}

	return 0;
}

/**
 * @brief Synchronous I2C transmit operation.
 * @param device - Pointer to the I2C device descriptor.
 * @param transfer - Pointer to the I2C transfer descriptor.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_i2c_transmit(struct capi_i2c_device *device,
				   struct capi_i2c_transfer *transfer)
{
	struct stm32_i2c_priv_handle *priv_handle;
	HAL_StatusTypeDef hal_ret;
	uint16_t dev_addr;

	if (!device || !device->controller || !transfer)
		return -EINVAL;

	if (!transfer->buf && transfer->len > 0)
		return -EINVAL;

	priv_handle = device->controller->priv;
	if (!priv_handle)
		return -EINVAL;

	dev_addr = (transfer->target_addr ? transfer->target_addr : device->address) <<
		   1;

	if (transfer->sub_address && transfer->sub_address_len > 0) {
		hal_ret = HAL_I2C_Mem_Write(&priv_handle->hi2c,
					    dev_addr,
					    transfer->sub_address[0],
					    transfer->sub_address_len == 1 ?
					    I2C_MEMADD_SIZE_8BIT : I2C_MEMADD_SIZE_16BIT,
					    transfer->buf,
					    transfer->len,
					    HAL_MAX_DELAY);
	} else if (transfer->no_stop) {
		hal_ret = HAL_I2C_Master_Seq_Transmit_IT(&priv_handle->hi2c,
				dev_addr,
				transfer->buf,
				transfer->len,
				I2C_FIRST_FRAME);
		if (hal_ret == HAL_OK) {
			uint32_t start = HAL_GetTick();
			while (HAL_I2C_GetState(&priv_handle->hi2c) != HAL_I2C_STATE_READY) {
				if (HAL_GetTick() - start > STM32_I2C_SEQ_TIMEOUT_MS)
					return -ETIMEDOUT;
			}
		}
	} else {
		hal_ret = HAL_I2C_Master_Transmit(&priv_handle->hi2c,
						  dev_addr,
						  transfer->buf,
						  transfer->len,
						  HAL_MAX_DELAY);
	}

	switch (hal_ret) {
	case HAL_OK:
		return 0;
	case HAL_BUSY:
		return -EBUSY;
	case HAL_TIMEOUT:
		return -ETIMEDOUT;
	default:
		return -EIO;
	}
}

/**
 * @brief Synchronous I2C receive operation.
 * @param device - Pointer to the I2C device descriptor.
 * @param transfer - Pointer to the I2C transfer descriptor.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_i2c_receive(struct capi_i2c_device *device,
				  struct capi_i2c_transfer *transfer)
{
	struct stm32_i2c_priv_handle *priv_handle;
	HAL_StatusTypeDef hal_ret;
	uint16_t dev_addr;

	if (!device || !device->controller || !transfer)
		return -EINVAL;

	if (!transfer->buf && transfer->len > 0)
		return -EINVAL;

	priv_handle = device->controller->priv;
	if (!priv_handle)
		return -EINVAL;

	dev_addr = (transfer->target_addr ? transfer->target_addr : device->address) <<
		   1;

	if (transfer->sub_address && transfer->sub_address_len > 0) {
		hal_ret = HAL_I2C_Mem_Read(&priv_handle->hi2c,
					   dev_addr,
					   transfer->sub_address[0],
					   transfer->sub_address_len == 1 ?
					   I2C_MEMADD_SIZE_8BIT : I2C_MEMADD_SIZE_16BIT,
					   transfer->buf,
					   transfer->len,
					   HAL_MAX_DELAY);
	} else if (transfer->no_stop) {
		hal_ret = HAL_I2C_Master_Seq_Receive_IT(&priv_handle->hi2c,
							dev_addr,
							transfer->buf,
							transfer->len,
							I2C_LAST_FRAME);
		if (hal_ret == HAL_OK) {
			uint32_t start = HAL_GetTick();
			while (HAL_I2C_GetState(&priv_handle->hi2c) != HAL_I2C_STATE_READY) {
				if (HAL_GetTick() - start > STM32_I2C_SEQ_TIMEOUT_MS)
					return -ETIMEDOUT;
			}
		}
	} else {
		hal_ret = HAL_I2C_Master_Receive(&priv_handle->hi2c,
						 dev_addr,
						 transfer->buf,
						 transfer->len,
						 HAL_MAX_DELAY);
	}

	switch (hal_ret) {
	case HAL_OK:
		return 0;
	case HAL_BUSY:
		return -EBUSY;
	case HAL_TIMEOUT:
		return -ETIMEDOUT;
	default:
		return -EIO;
	}
}

/**
 * @brief Register an async event callback for I2C operations.
 * @param handle - Pointer to the I2C controller handle.
 * @param callback - User callback function.
 * @param callback_arg - User callback argument.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_i2c_register_callback(struct capi_i2c_controller_handle
		*handle,
		capi_i2c_callback const callback,
		void *const callback_arg)
{
	struct stm32_i2c_priv_handle *priv_handle;

	if (!handle)
		return -EINVAL;

	priv_handle = handle->priv;
	priv_handle->callback = callback;
	priv_handle->callback_arg = callback_arg;

	return 0;
}

/**
 * @brief Configure I2C bus speed.
 * @param handle - Pointer to the I2C controller handle.
 * @param speed - Desired I2C bus speed mode.
 * @param duty_cycle - Desired I2C SCLK duty cycle percentage.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_i2c_configure_bus_speed(struct capi_i2c_controller_handle
		*handle,
		enum capi_i2c_speed speed,
		uint8_t duty_cycle)
{
	struct stm32_i2c_priv_handle *priv_handle;
	uint32_t clock_speed;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv_handle = handle->priv;

	switch (speed) {
	case CAPI_I2C_SPEED_STANDARD:
		clock_speed = STM32_I2C_DEFAULT_CLOCK_HZ;
		break;
	case CAPI_I2C_SPEED_FAST:
		clock_speed = 400000;
		break;
	case CAPI_I2C_SPEED_FAST_PLUS:
		clock_speed = 1000000;
		break;
	default:
		return -EINVAL;
	}

	HAL_I2C_DeInit(&priv_handle->hi2c);

#if defined(STM32F4) || defined(STM32F1) || defined(STM32F2) || defined(STM32L1)
	priv_handle->hi2c.Init.ClockSpeed = clock_speed;
	if (duty_cycle > 50)
		priv_handle->hi2c.Init.DutyCycle = I2C_DUTYCYCLE_16_9;
	else
		priv_handle->hi2c.Init.DutyCycle = I2C_DUTYCYCLE_2;
#else
	(void)clock_speed;
	(void)duty_cycle;
#endif

	if (HAL_I2C_Init(&priv_handle->hi2c) != HAL_OK)
		return -EIO;

	return 0;
}

/**
 * @brief Asynchronous I2C transmit operation.
 * @param device - Pointer to the I2C device descriptor.
 * @param transfer - Pointer to the I2C transfer descriptor.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_i2c_transmit_async(struct capi_i2c_device *device,
		struct capi_i2c_transfer *transfer)
{
	struct stm32_i2c_priv_handle *priv_handle;
	HAL_StatusTypeDef hal_ret;
	uint16_t dev_addr;

	if (!device || !device->controller || !transfer)
		return -EINVAL;

	if (!transfer->buf && transfer->len > 0)
		return -EINVAL;

	priv_handle = device->controller->priv;
	if (!priv_handle)
		return -EINVAL;

	if (priv_handle->async_in_progress)
		return -EBUSY;

	priv_handle->async_in_progress = true;
	priv_handle->current_transfer = transfer;

	dev_addr = (transfer->target_addr ? transfer->target_addr : device->address) <<
		   1;

	if (transfer->sub_address && transfer->sub_address_len > 0) {
		hal_ret = HAL_I2C_Mem_Write_IT(&priv_handle->hi2c,
					       dev_addr,
					       transfer->sub_address[0],
					       transfer->sub_address_len == 1 ?
					       I2C_MEMADD_SIZE_8BIT : I2C_MEMADD_SIZE_16BIT,
					       transfer->buf,
					       transfer->len);
	} else {
		hal_ret = HAL_I2C_Master_Transmit_IT(&priv_handle->hi2c,
						     dev_addr,
						     transfer->buf,
						     transfer->len);
	}

	if (hal_ret != HAL_OK) {
		priv_handle->async_in_progress = false;
		priv_handle->current_transfer = NULL;
		switch (hal_ret) {
		case HAL_BUSY:
			return -EBUSY;
		default:
			return -EIO;
		}
	}

	return 0;
}

/**
 * @brief Asynchronous I2C receive operation.
 * @param device - Pointer to the I2C device descriptor.
 * @param transfer - Pointer to the I2C transfer descriptor.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_i2c_receive_async(struct capi_i2c_device *device,
					struct capi_i2c_transfer *transfer)
{
	struct stm32_i2c_priv_handle *priv_handle;
	HAL_StatusTypeDef hal_ret;
	uint16_t dev_addr;

	if (!device || !device->controller || !transfer)
		return -EINVAL;

	if (!transfer->buf && transfer->len > 0)
		return -EINVAL;

	priv_handle = device->controller->priv;
	if (!priv_handle)
		return -EINVAL;

	if (priv_handle->async_in_progress)
		return -EBUSY;

	priv_handle->async_in_progress = true;
	priv_handle->current_transfer = transfer;

	dev_addr = (transfer->target_addr ? transfer->target_addr : device->address) <<
		   1;

	if (transfer->sub_address && transfer->sub_address_len > 0) {
		hal_ret = HAL_I2C_Mem_Read_IT(&priv_handle->hi2c,
					      dev_addr,
					      transfer->sub_address[0],
					      transfer->sub_address_len == 1 ?
					      I2C_MEMADD_SIZE_8BIT : I2C_MEMADD_SIZE_16BIT,
					      transfer->buf,
					      transfer->len);
	} else {
		hal_ret = HAL_I2C_Master_Receive_IT(&priv_handle->hi2c,
						    dev_addr,
						    transfer->buf,
						    transfer->len);
	}

	if (hal_ret != HAL_OK) {
		priv_handle->async_in_progress = false;
		priv_handle->current_transfer = NULL;
		switch (hal_ret) {
		case HAL_BUSY:
			return -EBUSY;
		default:
			return -EIO;
		}
	}

	return 0;
}

/**
 * @brief Recover the I2C bus by reinitializing the peripheral.
 * @param handle - Pointer to the I2C controller handle.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_i2c_recover_bus(struct capi_i2c_controller_handle *handle)
{
	struct stm32_i2c_priv_handle *priv_handle;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv_handle = handle->priv;

	HAL_I2C_DeInit(&priv_handle->hi2c);

	if (HAL_I2C_Init(&priv_handle->hi2c) != HAL_OK)
		return -EIO;

	return 0;
}

/**
 * @brief Register the I2C controller as a target device.
 * @param handle - Pointer to the I2C controller handle.
 * @param addr - Target device address (7-bit or 10-bit).
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_i2c_register_target(struct capi_i2c_controller_handle
		*handle,
		uint16_t addr)
{
	struct stm32_i2c_priv_handle *priv_handle;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv_handle = handle->priv;

	HAL_I2C_DeInit(&priv_handle->hi2c);

	priv_handle->hi2c.Init.OwnAddress1 = addr << 1;

	if (HAL_I2C_Init(&priv_handle->hi2c) != HAL_OK)
		return -EIO;

	return 0;
}

/**
 * @brief Unregister the I2C controller from target mode.
 * @param handle - Pointer to the I2C controller handle.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_i2c_unregister_target(struct capi_i2c_controller_handle
		*handle)
{
	struct stm32_i2c_priv_handle *priv_handle;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv_handle = handle->priv;

	HAL_I2C_DeInit(&priv_handle->hi2c);

	priv_handle->hi2c.Init.OwnAddress1 = 0;

	if (HAL_I2C_Init(&priv_handle->hi2c) != HAL_OK)
		return -EIO;

	return 0;
}

/**
 * @brief I2C interrupt service routine handler.
 * @param handle - Pointer to the I2C controller handle.
 */
static void stm32_capi_i2c_isr(void *handle)
{
	struct capi_i2c_controller_handle *ctrl_handle;
	struct stm32_i2c_priv_handle *priv_handle;

	if (!handle)
		return;

	ctrl_handle = (struct capi_i2c_controller_handle *)handle;
	priv_handle = ctrl_handle->priv;

	HAL_I2C_EV_IRQHandler(&priv_handle->hi2c);
	HAL_I2C_ER_IRQHandler(&priv_handle->hi2c);
}

/**
 * @brief HAL I2C master transmit complete callback.
 * @param hi2c - Pointer to the HAL I2C handle.
 */
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	struct stm32_i2c_priv_handle *priv_handle;

	priv_handle = find_priv_handle(hi2c);
	if (!priv_handle)
		return;

	priv_handle->async_in_progress = false;
	priv_handle->current_transfer = NULL;

	if (priv_handle->callback)
		priv_handle->callback(CAPI_I2C_XFR_DONE, priv_handle->callback_arg, 0);
}

/**
 * @brief HAL I2C master receive complete callback.
 * @param hi2c - Pointer to the HAL I2C handle.
 */
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	struct stm32_i2c_priv_handle *priv_handle;

	priv_handle = find_priv_handle(hi2c);
	if (!priv_handle)
		return;

	priv_handle->async_in_progress = false;
	priv_handle->current_transfer = NULL;

	if (priv_handle->callback)
		priv_handle->callback(CAPI_I2C_XFR_DONE, priv_handle->callback_arg, 0);
}

/**
 * @brief HAL I2C memory transmit complete callback.
 * @param hi2c - Pointer to the HAL I2C handle.
 */
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	struct stm32_i2c_priv_handle *priv_handle;

	priv_handle = find_priv_handle(hi2c);
	if (!priv_handle)
		return;

	priv_handle->async_in_progress = false;
	priv_handle->current_transfer = NULL;

	if (priv_handle->callback)
		priv_handle->callback(CAPI_I2C_XFR_DONE, priv_handle->callback_arg, 0);
}

/**
 * @brief HAL I2C memory receive complete callback.
 * @param hi2c - Pointer to the HAL I2C handle.
 */
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	struct stm32_i2c_priv_handle *priv_handle;

	priv_handle = find_priv_handle(hi2c);
	if (!priv_handle)
		return;

	priv_handle->async_in_progress = false;
	priv_handle->current_transfer = NULL;

	if (priv_handle->callback)
		priv_handle->callback(CAPI_I2C_XFR_DONE, priv_handle->callback_arg, 0);
}

/**
 * @brief HAL I2C error callback.
 * @param hi2c - Pointer to the HAL I2C handle.
 */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
	struct stm32_i2c_priv_handle *priv_handle;
	enum capi_i2c_async_event event = CAPI_I2C_NAKD;
	uint32_t error;

	priv_handle = find_priv_handle(hi2c);
	if (!priv_handle)
		return;

	priv_handle->async_in_progress = false;
	priv_handle->current_transfer = NULL;

	error = HAL_I2C_GetError(hi2c);

	if (error & HAL_I2C_ERROR_AF)
		event = CAPI_I2C_NAKD;
	else if (error & HAL_I2C_ERROR_ARLO)
		event = CAPI_I2C_ALOSS;
	else if (error & HAL_I2C_ERROR_OVR)
		event = CAPI_I2C_RXOVER;

	if (priv_handle->callback)
		priv_handle->callback(event, priv_handle->callback_arg, (int)error);
}

/**
 * @brief HAL I2C abort complete callback.
 * @param hi2c - Pointer to the HAL I2C handle.
 */
void HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef *hi2c)
{
	struct stm32_i2c_priv_handle *priv_handle;

	priv_handle = find_priv_handle(hi2c);
	if (!priv_handle)
		return;

	priv_handle->async_in_progress = false;
	priv_handle->current_transfer = NULL;

	if (priv_handle->callback)
		priv_handle->callback(CAPI_I2C_NAKD, priv_handle->callback_arg, -ECANCELED);
}

const struct capi_i2c_ops stm32_capi_i2c_ops = {
	.init = stm32_capi_i2c_init,
	.deinit = stm32_capi_i2c_deinit,
	.transmit = stm32_capi_i2c_transmit,
	.receive = stm32_capi_i2c_receive,
	.register_callback = stm32_capi_i2c_register_callback,
	.configure_bus_speed = stm32_capi_i2c_configure_bus_speed,
	.transmit_async = stm32_capi_i2c_transmit_async,
	.receive_async = stm32_capi_i2c_receive_async,
	.recover_bus = stm32_capi_i2c_recover_bus,
	.register_target = stm32_capi_i2c_register_target,
	.unregister_target = stm32_capi_i2c_unregister_target,
	.isr = stm32_capi_i2c_isr,
};
