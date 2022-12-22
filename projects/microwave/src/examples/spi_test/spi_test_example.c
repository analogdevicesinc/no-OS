#include "common_data.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include "platform_includes.h"

struct hmc6300_dev {
	struct no_os_gpio_desc *en;
	struct no_os_gpio_desc *clk;
	struct no_os_gpio_desc *data;
	struct no_os_gpio_desc *scanout;
};

int hmc6300_write(struct hmc6300_dev *dev, bool rx, uint8_t reg, uint8_t val)
{
	int b;
	uint32_t send = 0;
	send = val; // data
	send |= ((uint32_t)reg & 0x3f) << 8; // array address
	send |= 1 << 14; // write
	send |= ((uint32_t)0x6 | rx) << 15;  // chip address
	const int delay = 1;

	no_os_gpio_set_value(dev->en, NO_OS_GPIO_LOW);
	for (b = 0; b < 18; b++) {
		no_os_gpio_set_value(dev->data, send & 0x1);
		no_os_udelay(delay);
		no_os_gpio_set_value(dev->clk, NO_OS_GPIO_HIGH);
		no_os_udelay(delay);
		no_os_gpio_set_value(dev->clk, NO_OS_GPIO_LOW);
		send >>= 1;
	}
	no_os_gpio_set_value(dev->data, NO_OS_GPIO_LOW);
	no_os_gpio_set_value(dev->en, NO_OS_GPIO_HIGH);

	return 0;
}

int hmc6300_read(struct hmc6300_dev *dev, bool rx, uint8_t reg, uint8_t *val)
{
	int b;
	uint8_t recv = 0;
	uint32_t send = 0;
	send |= ((uint32_t)reg & 0x3f) << 8;
	send |= ((uint32_t)0x6 | rx) << 15;
	const int delay = 1;

	no_os_gpio_set_value(dev->en, NO_OS_GPIO_LOW);
	for (b = 0; b < 18; b++) {
		no_os_gpio_set_value(dev->data, send & 0x1);
		no_os_udelay(delay);
		no_os_gpio_set_value(dev->clk, NO_OS_GPIO_HIGH);
		no_os_udelay(delay);
		no_os_gpio_set_value(dev->clk, NO_OS_GPIO_LOW);
		send >>= 1;
	}
	no_os_gpio_set_value(dev->data, NO_OS_GPIO_LOW);
	no_os_gpio_set_value(dev->en, NO_OS_GPIO_HIGH);

	// extra pulse while cs is high
	no_os_udelay(delay);
	no_os_gpio_set_value(dev->clk, NO_OS_GPIO_HIGH);
	no_os_udelay(delay);
	no_os_gpio_set_value(dev->clk, NO_OS_GPIO_LOW);
	no_os_udelay(delay);

	*val = 0;
	no_os_gpio_set_value(dev->en, NO_OS_GPIO_LOW);
	for (b = 0; b < 8; b++) {
		// scanout changes on sck rising edge
		no_os_gpio_set_value(dev->clk, NO_OS_GPIO_HIGH);
		no_os_udelay(delay);
		// sample scanout along with sck faling edge
		no_os_gpio_set_value(dev->clk, NO_OS_GPIO_LOW);
		no_os_gpio_get_value(dev->scanout, &recv);
		no_os_udelay(delay);
		*val |= recv << b;
	}
	no_os_gpio_set_value(dev->en, NO_OS_GPIO_HIGH);

	return 0;
}

const uint8_t hmc6300_admv9621_default[] = {
	0x00,  //row 0
	74,      //row 1
	242,    //row 2
	247,    //row 3
	0x00,  //row 4
	191,    //row 5
	108,    //row 6
	223,    //row 7
	143,    //row 8
	0x00,  //row 9
	82,      //row 10
	243,    //row 11
	0x00,  //row 12
	0x00,  //row 13
	0x00,  //row 14
	0x00,  //row 15
	54,      //row 16
	187,    //row 17
	70,      //row 18
	0x02,  //row 19
	29,      //row 20
	18,      //row 21
	4,       //row 22
	98,      //row 23
	143, //row 24
	31, //row 25
	0, //row 26
	7, //row 27
};

int main()
{
	int ret;
	struct no_os_gpio_desc *reset, *en, *clk, *data, *scanout_tx, *scanout_rx;
	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		return ret;

	maxim_uart_stdio(uart_desc);

	printf("Hello world\n");

	reset_gpio_ip.extra = &xgpio;
	en_gpio_ip.extra = &xgpio;
	clk_gpio_ip.extra = &xgpio;
	data_gpio_ip.extra = &xgpio;
	scanout_tx_gpio_ip.extra = &xgpio;
	scanout_rx_gpio_ip.extra = &xgpio;

	// initialize gpios
	ret = no_os_gpio_get(&reset, &reset_gpio_ip);
	if (ret)
		goto error;

	ret = no_os_gpio_get(&en, &en_gpio_ip);
	if (ret)
		goto error;

	ret = no_os_gpio_get(&clk, &clk_gpio_ip);
	if (ret)
		goto error;

	ret = no_os_gpio_get(&data, &data_gpio_ip);
	if (ret)
		goto error;

	ret = no_os_gpio_get(&scanout_tx, &scanout_tx_gpio_ip);
	if (ret)
		goto error;

	ret = no_os_gpio_get(&scanout_rx, &scanout_rx_gpio_ip);
	if (ret)
		goto error;

	// set gpio direction
	ret = no_os_gpio_direction_output(reset, NO_OS_GPIO_LOW);
	if (ret)
		goto error;

	ret = no_os_gpio_direction_output(en, NO_OS_GPIO_HIGH);
	if (ret)
		goto error;


	ret = no_os_gpio_direction_output(clk, NO_OS_GPIO_LOW);
	if (ret)
		goto error;

	ret = no_os_gpio_direction_output(data, NO_OS_GPIO_LOW);
	if (ret)
		goto error;

	ret = no_os_gpio_direction_input(scanout_tx);
	if (ret)
		goto error;

	ret = no_os_gpio_direction_input(scanout_rx);
	if (ret)
		goto error;

	// TODO: replace this with proper init()
	struct hmc6300_dev tx;
	tx.en = en;
	tx.clk = clk;
	tx.data = data;
	tx.scanout = scanout_tx;

	struct hmc6300_dev rx;
	rx.en = en;
	rx.clk = clk;
	rx.data = data;
	rx.scanout = scanout_rx;

	uint8_t regval;

	// reset
	no_os_gpio_set_value(reset, NO_OS_GPIO_HIGH);
	no_os_mdelay(1);
	no_os_gpio_set_value(reset, NO_OS_GPIO_LOW);
	no_os_mdelay(1);

	int r;
	for (r = 1; r < 24; r++) {
		regval = hmc6300_admv9621_default[r];
		ret = hmc6300_write(&tx, false, r, regval);
		if (ret < 0)
			printf("hmc6300_write: %d\n", ret);

		printf("Wrote %d: 0x%x\n", r, regval);
	}
	no_os_mdelay(1);

	for (r = 1; r < 24; r++) {
		ret = hmc6300_read(&tx, false, r, &regval);
		if (ret < 0)
			printf("hmc6300_read: %d\n", ret);

		if (regval != hmc6300_admv9621_default[r])
			printf("Mismatch at register %d: 0x%x, expected 0x%x\n", r, regval,
			       hmc6300_admv9621_default[r]);

		printf("Register %d: 0x%x\n", r, regval);
	}

error:
	printf("%s returned with %d\n", __func__, ret);
	return ret;
}
