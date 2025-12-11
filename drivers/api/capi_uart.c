
#include "no_os_error.h"
#include "capi_uart.h"

int capi_uart_init(struct capi_uart_handle **handle,
		   const struct capi_uart_config *config)
{
	int ret;

	if (!config || !config->ops)
		return -EINVAL;

	if (!config->ops->init)
		return -ENOSYS;

	(*handle)->ops = config->ops;

	return config->ops->init(handle, config);
}

int capi_uart_deinit(struct capi_uart_handle **handle)
{
	return (*handle)->ops->deinit(handle);
}

int capi_uart_transmit(struct capi_uart_handle *handle,
		       uint8_t *buf,
		       uint32_t len)
{
	if (!handle || !handle->ops || !buf)
		return -EINVAL;

	if (!handle->ops->transmit)
		return -ENOSYS;

	return handle->ops->transmit(handle, buf, len);
}

int capi_uart_extended(int (*func)(void *), void *arg)
{
	// locking management
	return func(arg);
}
