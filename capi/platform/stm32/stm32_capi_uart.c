/*
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "stm32_capi_uart_priv.h"
#include "capi_alloc.h"
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>

/** Maximum number of UART instances tracked for HAL callback routing.
 * 12 covers all known STM32 families (up to 10 on H7/H5, plus headroom). */
#define STM32_CAPI_UART_MAX_INSTANCES 12

/** Lookup table: maps HAL UART handle → CAPI handle for ISR dispatch */
static struct capi_uart_handle *uart_handle_map[STM32_CAPI_UART_MAX_INSTANCES];
static UART_HandleTypeDef *uart_hal_map[STM32_CAPI_UART_MAX_INSTANCES];

static void uart_map_register(UART_HandleTypeDef *huart,
			      struct capi_uart_handle *handle)
{
	unsigned int i;

	for (i = 0; i < STM32_CAPI_UART_MAX_INSTANCES; i++) {
		if (!uart_hal_map[i] || uart_hal_map[i] == huart) {
			uart_hal_map[i] = huart;
			uart_handle_map[i] = handle;
			return;
		}
	}
}

static void uart_map_unregister(UART_HandleTypeDef *huart)
{
	unsigned int i;

	for (i = 0; i < STM32_CAPI_UART_MAX_INSTANCES; i++) {
		if (uart_hal_map[i] == huart) {
			uart_hal_map[i] = NULL;
			uart_handle_map[i] = NULL;
			return;
		}
	}
}

static struct capi_uart_handle *uart_map_lookup(UART_HandleTypeDef *huart)
{
	unsigned int i;

	for (i = 0; i < STM32_CAPI_UART_MAX_INSTANCES; i++) {
		if (uart_hal_map[i] == huart)
			return uart_handle_map[i];
	}
	return NULL;
}

/**
 * @brief Initialize the STM32 UART peripheral.
 * @param handle - Pointer to the UART handle to be allocated and initialized.
 * @param config - Pointer to the UART configuration structure.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_uart_init(struct capi_uart_handle **handle,
				const struct capi_uart_config *config)
{
	struct capi_uart_handle *uart_handle;
	struct stm32_uart_priv_handle *uart_priv_handle;
	struct stm32_uart_extra_config *uart_extra_config;
	int ret = 0;

	if (!handle || !config)
		return -EINVAL;

	if (*handle == NULL) {
		uart_handle = (struct capi_uart_handle *)capi_calloc(1, sizeof(*uart_handle));
		if (!uart_handle)
			return -ENOMEM;

		uart_priv_handle = (struct stm32_uart_priv_handle *)capi_calloc(1,
				   sizeof(*uart_priv_handle));
		if (!uart_priv_handle) {
			capi_free(uart_handle);
			return -ENOMEM;
		}

		uart_handle->priv = uart_priv_handle;
		uart_handle->init_allocated = true;
	} else {
		uart_handle = *handle;
		uart_handle->init_allocated = false;

		if (!uart_handle->priv)
			return -EINVAL;

		uart_priv_handle = uart_handle->priv;
	}
	uart_handle->ops = config->ops;

	uart_extra_config = config->extra;

	uart_priv_handle->huart = uart_extra_config->huart;

	uart_priv_handle->huart->Init.BaudRate = config->line_config->baudrate;

	switch (config->line_config->size) {
#ifdef UART_WORDLENGTH_7B
	case CAPI_UART_DATA_BITS_7:
		uart_priv_handle->huart->Init.WordLength = UART_WORDLENGTH_7B;
		break;
#endif
	case CAPI_UART_DATA_BITS_8:
		uart_priv_handle->huart->Init.WordLength = UART_WORDLENGTH_8B;
		break;
	/* TODO: add 9 bits option to CAPI */
	/* case CAPI_UART_DATA_BITS_9:
		uart_priv_handle->huart->Init.WordLength = UART_WORDLENGTH_9B;
		break; */
	default:
		ret = -EINVAL;
		goto error;
	};

	switch (config->line_config->parity) {
	case CAPI_UART_PARITY_NONE:
		uart_priv_handle->huart->Init.Parity = UART_PARITY_NONE;
		break;
	case CAPI_UART_PARITY_ODD:
		uart_priv_handle->huart->Init.Parity = UART_PARITY_ODD;
		break;
	case CAPI_UART_PARITY_EVEN:
		uart_priv_handle->huart->Init.Parity = UART_PARITY_EVEN;
		break;
	default:
		ret = -EINVAL;
		goto error;
	};

	uart_priv_handle->huart->Init.StopBits =
		config->line_config->stop_bits == CAPI_UART_STOP_1_BIT ?
		UART_STOPBITS_1 : UART_STOPBITS_2;

	uart_priv_handle->huart->Init.Mode = uart_extra_config->huart->Init.Mode;
	uart_priv_handle->huart->Init.HwFlowCtl =
		uart_extra_config->huart->Init.HwFlowCtl;
	uart_priv_handle->huart->Init.OverSampling =
		uart_extra_config->huart->Init.OverSampling;

	ret = HAL_UART_Init(uart_priv_handle->huart);
	if (ret != HAL_OK) {
		ret = -EIO;
		goto error;
	}

	*handle = uart_handle;

	uart_map_register(uart_priv_handle->huart, uart_handle);

	return 0;
error:
	if (uart_handle->init_allocated) {
		capi_free(uart_priv_handle);
		capi_free(uart_handle);
	}

	return ret;
}

/**
 * @brief Deinitialize the STM32 UART peripheral and free resources.
 * @param handle - Pointer to the UART handle.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_uart_deinit(struct capi_uart_handle *handle)
{
	struct stm32_uart_priv_handle *uart_priv_handle;

	if (!handle)
		return -EINVAL;

	uart_priv_handle = handle->priv;

	if (uart_priv_handle) {
		uart_map_unregister(uart_priv_handle->huart);
		HAL_UART_DeInit(uart_priv_handle->huart);
	}

	if (handle->init_allocated) {
		capi_free(uart_priv_handle);
		capi_free(handle);
	}

	return 0;
}

/**
 * @brief Get the current UART line configuration from the STM32 HAL handle.
 * @param handle - Pointer to the UART handle.
 * @param line_config - Pointer to the structure to fill with current config.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_uart_get_line_config(struct capi_uart_handle *handle,
		struct capi_uart_line_config *line_config)
{
	struct stm32_uart_priv_handle *priv;

	if (!handle || !handle->priv || !line_config)
		return -EINVAL;

	priv = handle->priv;

	line_config->baudrate = priv->huart->Init.BaudRate;

	switch (priv->huart->Init.WordLength) {
#ifdef UART_WORDLENGTH_7B
	case UART_WORDLENGTH_7B:
		line_config->size = CAPI_UART_DATA_BITS_7;
		break;
#endif
	case UART_WORDLENGTH_8B:
		line_config->size = CAPI_UART_DATA_BITS_8;
		break;
	default:
		line_config->size = CAPI_UART_DATA_BITS_8;
		break;
	}

	switch (priv->huart->Init.Parity) {
	case UART_PARITY_ODD:
		line_config->parity = CAPI_UART_PARITY_ODD;
		break;
	case UART_PARITY_EVEN:
		line_config->parity = CAPI_UART_PARITY_EVEN;
		break;
	default:
		line_config->parity = CAPI_UART_PARITY_NONE;
		break;
	}

	line_config->stop_bits = (priv->huart->Init.StopBits == UART_STOPBITS_2) ?
				 CAPI_UART_STOP_2_BIT : CAPI_UART_STOP_1_BIT;

	switch (priv->huart->Init.HwFlowCtl) {
	case UART_HWCONTROL_RTS:
		line_config->flow_control = CAPI_UART_FLOW_CONTROL_RTS;
		break;
	case UART_HWCONTROL_CTS:
		line_config->flow_control = CAPI_UART_FLOW_CONTROL_CTS;
		break;
	case UART_HWCONTROL_RTS_CTS:
		line_config->flow_control = CAPI_UART_FLOW_CONTROL_RTSCTS;
		break;
	default:
		line_config->flow_control = CAPI_UART_FLOW_CONTROL_NONE;
		break;
	}

	line_config->address_mode = CAPI_UART_ADDRESS_MODE_DISABLED;
	line_config->device_address = 0;
	line_config->sticky_parity = false;
	line_config->loopback = false;

	return 0;
}

/**
 * @brief Set a new UART line configuration on the STM32 peripheral.
 * @param handle - Pointer to the UART handle.
 * @param line_config - Pointer to the new line configuration to apply.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_uart_set_line_config(struct capi_uart_handle *handle,
		struct capi_uart_line_config *line_config)
{
	struct stm32_uart_priv_handle *priv;
	int ret;

	if (!handle || !handle->priv || !line_config)
		return -EINVAL;

	priv = handle->priv;

	priv->huart->Init.BaudRate = line_config->baudrate;

	switch (line_config->size) {
#ifdef UART_WORDLENGTH_7B
	case CAPI_UART_DATA_BITS_7:
		priv->huart->Init.WordLength = UART_WORDLENGTH_7B;
		break;
#endif
	case CAPI_UART_DATA_BITS_8:
		priv->huart->Init.WordLength = UART_WORDLENGTH_8B;
		break;
	default:
		return -EINVAL;
	}

	switch (line_config->parity) {
	case CAPI_UART_PARITY_NONE:
		priv->huart->Init.Parity = UART_PARITY_NONE;
		break;
	case CAPI_UART_PARITY_ODD:
		priv->huart->Init.Parity = UART_PARITY_ODD;
		break;
	case CAPI_UART_PARITY_EVEN:
		priv->huart->Init.Parity = UART_PARITY_EVEN;
		break;
	default:
		return -EINVAL;
	}

	priv->huart->Init.StopBits = (line_config->stop_bits == CAPI_UART_STOP_2_BIT) ?
				     UART_STOPBITS_2 : UART_STOPBITS_1;

	switch (line_config->flow_control) {
	case CAPI_UART_FLOW_CONTROL_NONE:
		priv->huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
		break;
	case CAPI_UART_FLOW_CONTROL_RTS:
		priv->huart->Init.HwFlowCtl = UART_HWCONTROL_RTS;
		break;
	case CAPI_UART_FLOW_CONTROL_CTS:
		priv->huart->Init.HwFlowCtl = UART_HWCONTROL_CTS;
		break;
	case CAPI_UART_FLOW_CONTROL_RTSCTS:
		priv->huart->Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
		break;
	default:
		return -EINVAL;
	}

	ret = HAL_UART_Init(priv->huart);
	if (ret != HAL_OK)
		return -EIO;

	return 0;
}

/**
 * @brief Transmit data over the STM32 UART peripheral.
 * @param handle - Pointer to the UART handle.
 * @param buf - Pointer to the data buffer to transmit.
 * @param len - Number of bytes to transmit.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_uart_transmit(struct capi_uart_handle *handle,
				    uint8_t *buf, uint32_t len)
{
	struct stm32_uart_priv_handle *uart_priv_handle;
	int ret;

	if (!handle || !handle->priv || !buf)
		return -EINVAL;

	if (!len)
		return 0;

	uart_priv_handle = handle->priv;

	ret = HAL_UART_Transmit(uart_priv_handle->huart, buf, len, HAL_MAX_DELAY);
	switch (ret) {
	case HAL_OK:
		break;
	case HAL_BUSY:
		return -EBUSY;
	case HAL_TIMEOUT:
		return -ETIMEDOUT;
	default:
		return -EIO;
	};

	return 0;
}

/**
 * @brief Receive data from the STM32 UART peripheral.
 * @param handle - Pointer to the UART handle.
 * @param buf - Pointer to the buffer to store received data.
 * @param len - Number of bytes to receive.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_uart_receive(struct capi_uart_handle *handle,
				   uint8_t *buf, uint32_t len)
{
	struct stm32_uart_priv_handle *uart_priv_handle;
	int ret;

	if (!handle || !handle->priv || !buf)
		return -EINVAL;

	if (!len)
		return 0;

	uart_priv_handle = handle->priv;

	ret = HAL_UART_Receive(uart_priv_handle->huart, buf, len, HAL_MAX_DELAY);
	switch (ret) {
	case HAL_OK:
		break;
	case HAL_BUSY:
		return -EBUSY;
	case HAL_TIMEOUT:
		return -ETIMEDOUT;
	default:
		return -EIO;
	};

	return 0;
}

/**
 * @brief Register a user callback for UART async events.
 * @param handle - Pointer to the UART handle.
 * @param callback - User callback function.
 * @param callback_arg - User-provided argument forwarded to the callback.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_uart_register_callback(struct capi_uart_handle *handle,
		capi_uart_callback const callback,
		void *const callback_arg)
{
	struct stm32_uart_priv_handle *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;
	priv->callback = callback;
	priv->callback_arg = callback_arg;

	return 0;
}

/**
 * @brief Transmit data asynchronously (interrupt-driven).
 * @param handle - Pointer to the UART handle.
 * @param buf - Pointer to the data buffer to transmit.
 * @param len - Number of bytes to transmit.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_uart_transmit_async(struct capi_uart_handle *handle,
		uint8_t *buf, uint32_t len)
{
	struct stm32_uart_priv_handle *priv;
	int ret;

	if (!handle || !handle->priv || !buf)
		return -EINVAL;

	if (!len)
		return 0;

	priv = handle->priv;

	ret = HAL_UART_Transmit_IT(priv->huart, buf, (uint16_t)len);
	switch (ret) {
	case HAL_OK:
		return 0;
	case HAL_BUSY:
		return -EBUSY;
	default:
		return -EIO;
	}
}

/**
 * @brief Receive data asynchronously (interrupt-driven).
 * @param handle - Pointer to the UART handle.
 * @param buf - Pointer to the buffer for incoming data.
 * @param len - Number of bytes to receive.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_uart_receive_async(struct capi_uart_handle *handle,
		uint8_t *buf, uint32_t len)
{
	struct stm32_uart_priv_handle *priv;
	int ret;

	if (!handle || !handle->priv || !buf)
		return -EINVAL;

	if (!len)
		return 0;

	priv = handle->priv;

	ret = HAL_UART_Receive_IT(priv->huart, buf, (uint16_t)len);
	switch (ret) {
	case HAL_OK:
		return 0;
	case HAL_BUSY:
		return -EBUSY;
	default:
		return -EIO;
	}
}

/**
 * @brief Get the reason for the most recent UART interrupt.
 * @param handle - Pointer to the UART handle.
 * @param reason - Output: the interrupt reason.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_uart_get_interrupt_reason(
	struct capi_uart_handle *handle,
	enum capi_uart_interrupt_reason *reason)
{
	struct stm32_uart_priv_handle *priv;

	if (!handle || !handle->priv || !reason)
		return -EINVAL;

	priv = handle->priv;
	*reason = priv->last_interrupt_reason;

	return 0;
}

/**
 * @brief Get accumulated line status flags.
 * @param handle - Pointer to the UART handle.
 * @param status_flags - Output: bitmask of capi_uart_line_status_flags.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_uart_get_line_status(struct capi_uart_handle *handle,
		uint32_t *status_flags)
{
	struct stm32_uart_priv_handle *priv;

	if (!handle || !handle->priv || !status_flags)
		return -EINVAL;

	priv = handle->priv;
	*status_flags = priv->last_line_status;
	priv->last_line_status = 0;

	return 0;
}

/**
 * @brief UART ISR entry point — delegates to HAL_UART_IRQHandler.
 * @param handle - Pointer to the UART handle (cast from void *).
 */
static void stm32_capi_uart_isr(void *handle)
{
	struct capi_uart_handle *uart_handle = (struct capi_uart_handle *)handle;
	struct stm32_uart_priv_handle *priv;

	if (!uart_handle || !uart_handle->priv)
		return;

	priv = uart_handle->priv;

	HAL_UART_IRQHandler(priv->huart);
}

/*
 * STM32 HAL weak-override callbacks.
 *
 * The HAL calls these from HAL_UART_IRQHandler().  We look up the CAPI
 * handle via the mapping table and forward the event to the user callback.
 */

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	struct capi_uart_handle *handle = uart_map_lookup(huart);
	struct stm32_uart_priv_handle *priv;

	if (!handle)
		return;

	priv = handle->priv;
	if (priv->callback)
		priv->callback(CAPI_UART_EVENT_TX_DONE, priv->callback_arg, 0);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	struct capi_uart_handle *handle = uart_map_lookup(huart);
	struct stm32_uart_priv_handle *priv;

	if (!handle)
		return;

	priv = handle->priv;
	if (priv->callback)
		priv->callback(CAPI_UART_EVENT_RX_DONE, priv->callback_arg, 0);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	struct capi_uart_handle *handle = uart_map_lookup(huart);
	struct stm32_uart_priv_handle *priv;
	uint32_t error;

	if (!handle)
		return;

	priv = handle->priv;
	error = huart->ErrorCode;

	/* Map HAL error bits to CAPI line status flags */
	if (error & HAL_UART_ERROR_PE)
		priv->last_line_status |= CAPI_UART_LINE_STAT_PARITY_ERROR;
	if (error & HAL_UART_ERROR_FE)
		priv->last_line_status |= CAPI_UART_LINE_STAT_FRAMING_ERROR;
	if (error & HAL_UART_ERROR_ORE)
		priv->last_line_status |= CAPI_UART_LINE_STAT_OVERRUN_ERROR;

	priv->last_interrupt_reason = CAPI_UART_INTR_RX_LINE_STATUS;

	if (priv->callback)
		priv->callback(CAPI_UART_EVENT_INTERRUPT, priv->callback_arg,
			       (int)error);
}

void HAL_UART_AbortCpltCallback(UART_HandleTypeDef *huart)
{
	struct capi_uart_handle *handle = uart_map_lookup(huart);
	struct stm32_uart_priv_handle *priv;

	if (!handle)
		return;

	priv = handle->priv;
	if (priv->callback)
		priv->callback(CAPI_UART_EVENT_TX_ABORTED, priv->callback_arg,
			       0);
}

const struct capi_uart_ops stm32_capi_uart_ops = {
	.init = stm32_capi_uart_init,
	.deinit = stm32_capi_uart_deinit,
	.get_line_config = stm32_capi_uart_get_line_config,
	.set_line_config = stm32_capi_uart_set_line_config,
	.transmit = stm32_capi_uart_transmit,
	.receive = stm32_capi_uart_receive,
	.register_callback = stm32_capi_uart_register_callback,
	.transmit_async = stm32_capi_uart_transmit_async,
	.receive_async = stm32_capi_uart_receive_async,
	.get_interrupt_reason = stm32_capi_uart_get_interrupt_reason,
	.get_line_status = stm32_capi_uart_get_line_status,
	.isr = stm32_capi_uart_isr,
};

static struct capi_uart_handle *uart_stdio_handle = NULL;

/**
 * @brief Enable stdio redirection over the STM32 UART peripheral.
 * @param handle - Pointer to an initialized UART handle.
 * @return 0 on success, negative error code otherwise.
 */
int stm32_uart_stdio_enable(struct capi_uart_handle *handle)
{
	if (!handle)
		return -EINVAL;

	uart_stdio_handle = handle;

	/* Disable I/O buffering for STDOUT stream, so that
	* chars are sent out as soon as they are printed. */
	setvbuf(stdout, NULL, _IONBF, 0);

	return 0;
}

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

/**
 * @brief Check if a file descriptor refers to a terminal.
 * @param fd - File descriptor to check.
 * @return 1 if the descriptor is a terminal, 0 otherwise.
 */
int _isatty(int fd)
{
	if (fd >= STDIN_FILENO && fd <= STDERR_FILENO)
		return 1;

	errno = EBADF;
	return 0;
}

/**
 * @brief Write data to a file descriptor.
 * @param fd - File descriptor to write to.
 * @param ptr - Pointer to the data buffer.
 * @param len - Number of bytes to write.
 * @return Number of bytes written on success, -1 on error.
 */
int _write(int fd, char *ptr, int len)
{
	int ret;

	if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
		ret = stm32_capi_uart_transmit(uart_stdio_handle, (uint8_t *)ptr, len);
		if (ret < 0) {
			errno = -ret;
			return -1;
		}

		return len;
	}
	errno = EBADF;
	return -1;
}

/**
 * @brief Close a file descriptor.
 * @param fd - File descriptor to close.
 * @return 0 on success, -1 on error.
 */
int _close(int fd)
{
	if (fd >= STDIN_FILENO && fd <= STDERR_FILENO)
		return 0;

	errno = EBADF;
	return -1;
}

/**
 * @brief Reposition file descriptor offset.
 * @param fd - File descriptor.
 * @param ptr - Offset value.
 * @param dir - Seek direction.
 * @return -1 always, seeking is not supported.
 */
int _lseek(int fd, int ptr, int dir)
{
	(void) fd;
	(void) ptr;
	(void) dir;

	errno = EBADF;
	return -1;
}

/**
 * @brief Read data from a file descriptor.
 * @param fd - File descriptor to read from.
 * @param ptr - Pointer to the buffer for incoming data.
 * @param len - Number of bytes to read.
 * @return Number of bytes read on success, -1 on error.
 */
int _read(int fd, char *ptr, int len)
{
	int ret;

	if (fd == STDIN_FILENO) {
		ret = stm32_capi_uart_receive(uart_stdio_handle, (uint8_t *)ptr, 1);
		if (ret < 0) {
			errno = -ret;
			return -1;
		}

		return 1;
	}
	errno = EBADF;
	return -1;
}

/**
 * @brief Get file status.
 * @param fd - File descriptor.
 * @param st - Pointer to the stat structure to fill.
 * @return 0 on success, -1 on error.
 */
int _fstat(int fd, struct stat *st)
{
	if (fd >= STDIN_FILENO && fd <= STDERR_FILENO) {
		st->st_mode = S_IFCHR;
		return 0;
	}

	errno = EBADF;
	return 0;
}
