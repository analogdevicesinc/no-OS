#include <stdint.h>
#include <stdlib.h>

#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "maxq1065.h"

static struct maxq1065_desc *maxq1065_local_desc;

mxq_err_t maxq1065_reset(void)
{
	no_os_gpio_set_value(maxq1065_local_desc->reset_gpio, NO_OS_GPIO_LOW);
	no_os_mdelay(300);
	no_os_gpio_set_value(maxq1065_local_desc->reset_gpio, NO_OS_GPIO_HIGH);
	no_os_mdelay(700);

	return MXQ_OK;
}

int maxq1065_init(struct maxq1065_desc **desc, struct maxq1065_init_param *param)
{
	struct maxq1065_desc *descriptor;
	uint8_t rdy_val;
	mxq_status_t s;
	mxq_err_t r;
	int ret;

	if (!param)
		return -EINVAL;

	descriptor = calloc(1, sizeof(*descriptor));
	if  (!descriptor)
		return -ENOMEM; 

	maxq1065_local_desc = descriptor;

	ret = no_os_spi_init(&descriptor->comm_desc, &param->comm_param);
	if (ret)
		goto free_descriptor;

	ret = no_os_gpio_get(&descriptor->rdy_gpio, &param->rdy_gpio_param);
	if (ret)
		goto free_spi;
	
	ret = no_os_gpio_direction_input(descriptor->rdy_gpio);
	if (ret)
		goto free_spi;

	ret = no_os_gpio_get(&descriptor->reset_gpio, &param->reset_gpio_param);
	if (ret)
		goto free_spi;

	ret = no_os_gpio_direction_output(descriptor->reset_gpio, NO_OS_GPIO_LOW);
	if (ret)
		goto free_reset;

	maxq1065_reset();

	ret = no_os_gpio_get_value(descriptor->rdy_gpio, &rdy_val);
	if (ret)
		return ret;

	// ret = no_os_gpio_get(&descriptor->mode_gpio, &param->mode_gpio_param);
	// if (ret)
	// 	goto free_reset;

	// ret = no_os_gpio_direction_output(descriptor->mode_gpio, NO_OS_GPIO_HIGH);
	// if (ret)
	// 	goto free_mode;

	HOST_set_interface(&maxq1065_ops);

	ret = MXQ_Module_Init();
	if (ret)
		return ret;

	ret = no_os_gpio_get_value(descriptor->rdy_gpio, &rdy_val);
	ret = no_os_gpio_get_value(descriptor->rdy_gpio, &rdy_val);

	MXQ_TLS_Enable();

	r = MXQ_Ping(32);
	if (r != MXQ_OK)
		return r;

	r = MXQ_GetStatus(&s);
	if (r != MXQ_OK)
		return r;

	MXQ_DisplayStatus(&s);

	*desc = descriptor;

	return 0;


free_reset:
	no_os_gpio_remove(descriptor->reset_gpio);
free_spi:
	no_os_spi_remove(descriptor->comm_desc);
free_descriptor:
	free(descriptor);

	return ret;
}

int maxq1065_remove(struct maxq1065_desc *desc)
{
	free(desc);
}

mxq_err_t maxq1065_init_spi(void)
{
	return MXQ_OK;
}

mxq_err_t maxq1065_init_serial(void)
{
	return MXQ_OK;
}

mxq_err_t maxq1065_exchange_bytes_spi(const mxq_u1* src, mxq_length len, mxq_u1* dest)
{
	uint8_t buff;
	uint32_t crc;
	struct no_os_spi_msg xfer = {
		.tx_buff = maxq1065_local_desc->tx_buff,
		.rx_buff = maxq1065_local_desc->rx_buff,
		.bytes_number = len,
		.cs_change = 1,
	};
	uint8_t rdy;
	int ret;

	memset(maxq1065_local_desc->rx_buff, 0, 512);
	memset(maxq1065_local_desc->tx_buff, 0, 512);
	memcpy(xfer.tx_buff, src, len);

	// for (int i = 0; i < len; i++) {
	// 	buff = no_os_bit_swap_constant_8(xfer.tx_buff[i]);
	// 	xfer.tx_buff[i] = buff;
	// }

	// no_os_swap(xfer.tx_buff[len - 1], xfer.tx_buff[len - 2]);

	ret = no_os_spi_transfer(maxq1065_local_desc->comm_desc, &xfer, 1);
	if (ret)
		return ret;

	memcpy(dest, maxq1065_local_desc->rx_buff, len);

	// for (int i = 0; i < len; i++)
	// 	dest[i] = no_os_bit_swap_constant_8(maxq1065_local_desc->rx_buff[i]);

	return len;
}

mxq_err_t maxq1065_send_bytes_spi(const mxq_u1* src, mxq_length len)
{
	uint8_t rx[len];
	int ret;

	ret = maxq1065_exchange_bytes_spi(src, len, rx);

	return ret;
}

mxq_err_t maxq1065_receive_bytes_spi(mxq_u1* dest, mxq_length len)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = maxq1065_local_desc->tx_buff,
		.rx_buff = maxq1065_local_desc->rx_buff,
		.bytes_number = len,
		.cs_change = 1,
	};
	int ret;

	memset(maxq1065_local_desc->rx_buff, 0, 512);
	memset(maxq1065_local_desc->tx_buff, 0, 512);
	ret = no_os_spi_transfer(maxq1065_local_desc->comm_desc, &xfer, 1);
	if (ret)
		return ret;

	// for (int i = 0; i < len; i++)
	// 	dest[i] = no_os_bit_swap_constant_8(maxq1065_local_desc->rx_buff[i]);

	memcpy(dest, maxq1065_local_desc->rx_buff, len);

	return len;
}

mxq_err_t maxq1065_first_bytes_spi(mxq_u1* dest, mxq_length len)
{
	uint8_t rx[10] = {0};
	struct no_os_spi_msg xfer = {
		.tx_buff = maxq1065_local_desc->tx_buff,
		.rx_buff = maxq1065_local_desc->rx_buff,
		.bytes_number = 1,
		.cs_change = 1,
	};
	uint32_t timeout = 1000;
	mxq_u1 dummy = 0xCC;
	uint8_t rdy = 0;
	int ret;

	while (maxq1065_local_desc->rx_buff[0] != 0x55 && timeout) {
		no_os_spi_transfer(maxq1065_local_desc->comm_desc, &xfer, 1);
		timeout--;
		no_os_mdelay(1);
	}

	if (!timeout)
		return -ETIMEDOUT;

	dest[0] = 0x55;

	ret = maxq1065_receive_bytes_spi(&dest[1], len - 1);
	if (ret < 0)
		return ret;

	return len;
}

mxq_err_t maxq1065_activate_tls(void)
{
	return MXQ_OK;
}

mxq_err_t maxq1065_activate_gcu(void)
{
	return MXQ_OK;
}

mxq_err_t maxq1065_sleep(mxq_u4 us)
{
	no_os_udelay((uint32_t)us);

	return MXQ_OK;
}

const struct maxq1065_no_os_ops maxq1065_ops = {
	.init_spi = maxq1065_init_spi,
	.init_serial = maxq1065_init_serial,
	.send_bytes_spi = maxq1065_send_bytes_spi,
	.exchange_bytes_spi = maxq1065_exchange_bytes_spi,
	.receive_bytes_spi = maxq1065_receive_bytes_spi,
	.receive_bytes_spi_first = maxq1065_first_bytes_spi,
	.reset = maxq1065_reset,
	.activate_tls = maxq1065_activate_tls,
	.activate_gcu = maxq1065_activate_gcu,
	.sleep_us = maxq1065_sleep,
};
