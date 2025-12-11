
#include "capi_uart.h"
#include "stm32_capi_uart.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include <sys/stat.h>

int stm32_capi_uart_init(struct capi_uart_handle **handle,
			 const struct capi_uart_config *config)
{
	struct capi_uart_handle *uart_handle;
	struct stm32_uart_priv_handle *uart_priv_handle;
	struct stm32_uart_extra_config *uart_extra_config;
	int ret = 0;

	uart_handle = (struct capi_uart_handle *) no_os_calloc(1, sizeof(*uart_handle));
	if (!uart_handle)
		return -ENOMEM;

	uart_priv_handle = (struct stm32_uart_priv_handle *) no_os_calloc(1, sizeof(*uart_priv_handle));
	if (!uart_priv_handle) {
		ret = -ENOMEM;
		goto error;
	}

	uart_handle->init_allocated = true;
	uart_handle->priv = uart_priv_handle;
	uart_handle->ops = config->ops;

	uart_extra_config = config->extra;

	uart_priv_handle->huart = uart_extra_config->huart;

	uart_priv_handle->huart->Init.BaudRate = config->line_config->baudrate;

	switch (config->line_config->size) {
	case CAPI_UART_DATA_BITS_7:
		uart_priv_handle->huart->Init.WordLength = UART_WORDLENGTH_7B;
		break;
	case CAPI_UART_DATA_BITS_8:
		uart_priv_handle->huart->Init.WordLength = UART_WORDLENGTH_8B;
		break;
// TODO: add 9 bits option to CAPI
//	case CAPI_UART_DATA_BITS_9:
//		uart_priv_handle->huart->Init.WordLength = UART_WORDLENGTH_9B;
//		break;
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
		config->line_config->stop_bits = CAPI_UART_STOP_1_BIT ?
			UART_STOPBITS_1 : UART_STOPBITS_2;

	uart_priv_handle->huart->Init.Mode = uart_extra_config->huart->Init.Mode;
	uart_priv_handle->huart->Init.HwFlowCtl = uart_extra_config->huart->Init.HwFlowCtl;
	uart_priv_handle->huart->Init.OverSampling = uart_extra_config->huart->Init.OverSampling;

	ret = HAL_UART_Init(uart_priv_handle->huart);
	if (ret != HAL_OK) {
		ret = -EIO;
		goto error;
	}

	*handle = uart_handle;

	return 0;
error:
	no_os_free(uart_priv_handle);
	no_os_free(uart_handle);

	return ret;
}

// TODO: double pointer for handle is needed
int stm32_capi_uart_deinit(struct capi_uart_handle **handle)
{
	if (!handle)
		return -ENOMEM;

	if ((*handle)->init_allocated) {
		no_os_free((*handle)->priv);
		no_os_free(*handle);
		*handle = NULL;
	}

	return 0;
}

int stm32_capi_uart_transmit(struct capi_uart_handle *handle,
			     uint8_t *buf, uint32_t len)
{
	struct stm32_uart_priv_handle *uart_priv_handle;
	int ret;

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

int stm32_capi_uart_receive(struct capi_uart_handle *handle,
			    uint8_t *buf, uint32_t len)
{
	return 0;
}

struct capi_uart_ops stm32_capi_uart_ops = {
	.init = &stm32_capi_uart_init,
	.deinit = &stm32_capi_uart_deinit,
	.transmit = &stm32_capi_uart_transmit,
	.receive = &stm32_capi_uart_receive,
};

static struct capi_uart_handle *uart_stdio_handle = NULL;

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

int _isatty(int fd)
{
	if (fd >= STDIN_FILENO && fd <= STDERR_FILENO)
		return 1;

	errno = EBADF;
	return 0;
}

int _write(int fd, char* ptr, int len)
{
	int ret;

	if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
		ret = stm32_capi_uart_transmit(uart_stdio_handle, (uint8_t *)ptr, len);
		if (ret < 0) {
			errno = -ret;
			return -1;
		}

		return ret;
	}
	errno = EBADF;
	return -1;
}

int _close(int fd)
{
	if (fd >= STDIN_FILENO && fd <= STDERR_FILENO)
		return 0;

	errno = EBADF;
	return -1;
}

int _lseek(int fd, int ptr, int dir)
{
	(void) fd;
	(void) ptr;
	(void) dir;

	errno = EBADF;
	return -1;
}

int _read(int fd, char* ptr, int len)
{
	int ret;

	if (fd == STDIN_FILENO) {
		ret = stm32_capi_uart_receive(uart_stdio_handle, (uint8_t *)ptr, 1);
		if (ret < 0) {
			errno = -ret;
			return -1;
		}

		return ret;
	}
	errno = EBADF;
	return -1;
}

int _fstat(int fd, struct stat* st)
{
	if (fd >= STDIN_FILENO && fd <= STDERR_FILENO) {
		st->st_mode = S_IFCHR;
		return 0;
	}

	errno = EBADF;
	return 0;
}
