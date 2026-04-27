#include <stdlib.h>
#include <errno.h>
#include "stm32_capi_i2c.h"
#include "no_os_alloc.h"

#define MAX_I2C_INSTANCES 4
static struct {
	I2C_HandleTypeDef *hi2c;
	struct stm32_i2c_priv_handle *priv_handle;
} i2c_handle_lookup[MAX_I2C_INSTANCES];
static uint8_t i2c_handle_count = 0;

static struct stm32_i2c_priv_handle *find_priv_handle(I2C_HandleTypeDef *hi2c)
{
	for (uint8_t i = 0; i < i2c_handle_count; i++) {
		if (i2c_handle_lookup[i].hi2c == hi2c)
			return i2c_handle_lookup[i].priv_handle;
	}
	return NULL;
}

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

static void unregister_handle_mapping(I2C_HandleTypeDef *hi2c)
{
	for (uint8_t i = 0; i < i2c_handle_count; i++) {
		if (i2c_handle_lookup[i].hi2c == hi2c) {
			if (i < i2c_handle_count - 1)
				i2c_handle_lookup[i] = i2c_handle_lookup[i2c_handle_count - 1];
			i2c_handle_count--;
			break;
		}
	}
}

/**
 * @brief Get I2C base address from identifier
 */
static I2C_TypeDef *get_i2c_base_from_identifier(uint64_t identifier)
{
	if (identifier >= 0x40000000)
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

int stm32_capi_i2c_init(struct capi_i2c_controller_handle **handle,
			const struct capi_i2c_config *config)
{
	struct capi_i2c_controller_handle *i2c_handle;
	struct stm32_i2c_priv_handle *priv_handle;
	struct stm32_i2c_extra_config *extra_config;
	I2C_TypeDef *base;
	int ret;

	if (!handle || !config)
		return -EINVAL;

	i2c_handle = no_os_calloc(1, sizeof(*i2c_handle));
	if (!i2c_handle)
		return -ENOMEM;

	priv_handle = no_os_calloc(1, sizeof(*priv_handle));
	if (!priv_handle) {
		ret = -ENOMEM;
		goto error_free_handle;
	}

	i2c_handle->init_allocated = true;
	i2c_handle->priv = priv_handle;
	i2c_handle->ops = config->ops;

	extra_config = config->extra;

	if (extra_config && extra_config->hi2c) {
		priv_handle->hi2c = *extra_config->hi2c;
	} else {
		base = get_i2c_base_from_identifier(config->identifier);
		if (!base) {
			ret = -EINVAL;
			goto error_free_priv;
		}
		priv_handle->hi2c.Instance = base;
	}

#if defined(STM32F4) || defined(STM32F1) || defined(STM32F2) || defined(STM32L1)
	priv_handle->hi2c.Init.ClockSpeed = config->clk_freq_hz ?
					    config->clk_freq_hz : 100000;
	priv_handle->hi2c.Init.DutyCycle = I2C_DUTYCYCLE_2;
#else
	if (extra_config)
		priv_handle->hi2c.Init.Timing = extra_config->i2c_timing;
	else
		priv_handle->hi2c.Init.Timing = 0x10909CEC;
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
		goto error_free_priv;
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
error_free_priv:
	no_os_free(priv_handle);
error_free_handle:
	no_os_free(i2c_handle);
	return ret;
}

int stm32_capi_i2c_deinit(struct capi_i2c_controller_handle *handle)
{
	struct stm32_i2c_priv_handle *priv_handle;

	if (!handle)
		return -EINVAL;

	priv_handle = handle->priv;

	if (priv_handle) {
		unregister_handle_mapping(&priv_handle->hi2c);
		HAL_I2C_DeInit(&priv_handle->hi2c);
		no_os_free(priv_handle);
	}

	if (handle->init_allocated)
		no_os_free(handle);

	return 0;
}

int stm32_capi_i2c_transmit(struct capi_i2c_device *device,
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

	dev_addr = (transfer->target_addr ? transfer->target_addr : device->address) << 1;

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
			while (HAL_I2C_GetState(&priv_handle->hi2c) != HAL_I2C_STATE_READY)
				;
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

int stm32_capi_i2c_receive(struct capi_i2c_device *device,
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

	dev_addr = (transfer->target_addr ? transfer->target_addr : device->address) << 1;

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
			while (HAL_I2C_GetState(&priv_handle->hi2c) != HAL_I2C_STATE_READY)
				;
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

int stm32_capi_i2c_register_callback(struct capi_i2c_controller_handle *handle,
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

int stm32_capi_i2c_configure_bus_speed(struct capi_i2c_controller_handle *handle,
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
		clock_speed = 100000;
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

int stm32_capi_i2c_transmit_async(struct capi_i2c_device *device,
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

	dev_addr = (transfer->target_addr ? transfer->target_addr : device->address) << 1;

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

int stm32_capi_i2c_receive_async(struct capi_i2c_device *device,
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

	dev_addr = (transfer->target_addr ? transfer->target_addr : device->address) << 1;

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

int stm32_capi_i2c_recover_bus(struct capi_i2c_controller_handle *handle)
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

int stm32_capi_i2c_register_target(struct capi_i2c_controller_handle *handle,
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

int stm32_capi_i2c_unregister_target(struct capi_i2c_controller_handle *handle)
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

void stm32_capi_i2c_isr(void *handle)
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
