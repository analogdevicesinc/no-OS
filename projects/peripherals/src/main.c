
#include <stdio.h>
#include "no_os_uart.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"
#include "no_os_pwm.h"
#include "maxim_pwm.h"
#include "no_os_gpio.h"
#include "maxim_gpio.h"
#include "no_os_spi.h"
#include "maxim_spi.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_alloc.h"

struct ili9341 {
	struct no_os_gpio_desc	*cs;
	struct no_os_gpio_desc	*dc;
	struct no_os_gpio_desc	*rst;
	struct no_os_pwm_desc 	*bl;
	struct no_os_spi_desc	*spi;
};

struct ili9341_init_param {
	struct no_os_gpio_init_param	*cs_init;
	struct no_os_gpio_init_param	*dc_init;
	struct no_os_gpio_init_param	*rst_init;
	struct no_os_pwm_init_param	*bl_init;
	struct no_os_spi_init_param	*spi_init;
};

void ili9341_cmd(struct ili9341 *ili, uint8_t cmd)
{
	no_os_gpio_set_value(ili->cs, NO_OS_GPIO_LOW);
	no_os_gpio_set_value(ili->dc, NO_OS_GPIO_LOW);
	no_os_spi_write_and_read(ili->spi, &cmd, 1);
	no_os_gpio_set_value(ili->dc, NO_OS_GPIO_HIGH);
}

void ili9341_data(struct ili9341 *ili, uint8_t data)
{
	no_os_gpio_set_value(ili->cs, NO_OS_GPIO_LOW);
	no_os_gpio_set_value(ili->dc, NO_OS_GPIO_HIGH);
	no_os_spi_write_and_read(ili->spi, &data, 1);
	no_os_gpio_set_value(ili->dc, NO_OS_GPIO_HIGH);
}

int32_t ili9341_setup(struct ili9341 *ili)
{
	int ret;

	ret = no_os_gpio_direction_output(ili->rst, NO_OS_GPIO_LOW);
	if (ret < 0)
		return ret;

	no_os_mdelay(10);

	ret = no_os_gpio_direction_output(ili->rst, NO_OS_GPIO_HIGH);
	if (ret < 0)
		return ret;

	no_os_mdelay(120);

	ili9341_cmd(ili, 0x11); //Sleep out
	ili9341_cmd(ili, 0xCF);
	ili9341_data(ili, 0x00);
	ili9341_data(ili, 0xC1);
	ili9341_data(ili, 0X30);
	ili9341_cmd(ili, 0xED);
	ili9341_data(ili, 0x64);
	ili9341_data(ili, 0x03);
	ili9341_data(ili, 0X12);
	ili9341_data(ili, 0X81);
	ili9341_cmd(ili, 0xE8);
	ili9341_data(ili, 0x85);
	ili9341_data(ili, 0x00);
	ili9341_data(ili, 0x79);
	ili9341_cmd(ili, 0xCB);
	ili9341_data(ili, 0x39);
	ili9341_data(ili, 0x2C);
	ili9341_data(ili, 0x00);
	ili9341_data(ili, 0x34);
	ili9341_data(ili, 0x02);
	ili9341_cmd(ili, 0xF7);
	ili9341_data(ili, 0x20);
	ili9341_cmd(ili, 0xEA);
	ili9341_data(ili, 0x00);
	ili9341_data(ili, 0x00);
	ili9341_cmd(ili, 0xC0); //Power control
	ili9341_data(ili, 0x1D); //VRH[5:0]
	ili9341_cmd(ili, 0xC1); //Power control
	ili9341_data(ili, 0x12); //SAP[2:0];BT[3:0]
	ili9341_cmd(ili, 0xC5); //VCM control
	ili9341_data(ili, 0x33);
	ili9341_data(ili, 0x3F);
	ili9341_cmd(ili, 0xC7); //VCM control
	ili9341_data(ili, 0x92);
	ili9341_cmd(ili, 0x3A); // Memory Access Control
	ili9341_data(ili, 0x55);
	ili9341_cmd(ili, 0x36); // Memory Access Control
	ili9341_data(ili, 0x08);
	ili9341_cmd(ili, 0xB1);
	ili9341_data(ili, 0x00);
	ili9341_data(ili, 0x12);
	ili9341_cmd(ili, 0xB6); // Display Function Control
	ili9341_data(ili, 0x0A);
	ili9341_data(ili, 0xA2);

	ili9341_cmd(ili, 0x44);
	ili9341_data(ili, 0x02);

	ili9341_cmd(ili, 0xF2); // 3Gamma Function Disable
	ili9341_data(ili, 0x00);
	ili9341_cmd(ili, 0x26); //Gamma curve selected
	ili9341_data(ili, 0x01);
	ili9341_cmd(ili, 0xE0); //Set Gamma
	ili9341_data(ili, 0x0F);
	ili9341_data(ili, 0x22);
	ili9341_data(ili, 0x1C);
	ili9341_data(ili, 0x1B);
	ili9341_data(ili, 0x08);
	ili9341_data(ili, 0x0F);
	ili9341_data(ili, 0x48);
	ili9341_data(ili, 0xB8);
	ili9341_data(ili, 0x34);
	ili9341_data(ili, 0x05);
	ili9341_data(ili, 0x0C);
	ili9341_data(ili, 0x09);
	ili9341_data(ili, 0x0F);
	ili9341_data(ili, 0x07);
	ili9341_data(ili, 0x00);
	ili9341_cmd(ili, 0XE1); //Set Gamma
	ili9341_data(ili, 0x00);
	ili9341_data(ili, 0x23);
	ili9341_data(ili, 0x24);
	ili9341_data(ili, 0x07);
	ili9341_data(ili, 0x10);
	ili9341_data(ili, 0x07);
	ili9341_data(ili, 0x38);
	ili9341_data(ili, 0x47);
	ili9341_data(ili, 0x4B);
	ili9341_data(ili, 0x0A);
	ili9341_data(ili, 0x13);
	ili9341_data(ili, 0x06);
	ili9341_data(ili, 0x30);
	ili9341_data(ili, 0x38);
	ili9341_data(ili, 0x0F);
	ili9341_cmd(ili, 0x29); //Display on

	no_os_mdelay(100);

	return 0;
}

int32_t ili9341_init(struct ili9341 **ili,
		     struct ili9341_init_param *init_param)
{
	struct ili9341 *dev;
	int ret;

	dev = (struct ili9341 *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_gpio_get(&dev->cs, init_param->cs_init);
	if (ret < 0)
		goto error_1;

	ret = no_os_gpio_get(&dev->dc, init_param->dc_init);
	if (ret < 0)
		goto error_2;

	ret = no_os_gpio_get(&dev->rst, init_param->rst_init);
	if (ret < 0)
		goto error_3;

	ret = no_os_pwm_init(&dev->bl, init_param->bl_init);
	if (ret < 0)
		goto error_4;

	ret = no_os_spi_init(&dev->spi, init_param->spi_init);
	if (ret < 0)
		goto error_5;

	ret = no_os_gpio_direction_output(dev->cs, NO_OS_GPIO_HIGH);
	if (ret < 0)
		goto error_6;
	ret = no_os_gpio_direction_output(dev->dc, NO_OS_GPIO_HIGH);
	if (ret < 0)
		goto error_6;
	ret = no_os_gpio_direction_output(dev->rst, NO_OS_GPIO_HIGH);
	if (ret < 0)
		goto error_6;

	ili9341_setup(dev);

	*ili = dev;

	return 0;

error_6:
	no_os_spi_remove(dev->spi);
error_5:
	no_os_pwm_remove(dev->bl);
error_4:
	no_os_gpio_remove(dev->rst);
error_3:
	no_os_gpio_remove(dev->dc);
error_2:
	no_os_gpio_remove(dev->cs);
error_1:
	no_os_free(dev);

	return ret;
}

int main(void)
{
	struct max_uart_init_param uart_init_max = {
		.flow = UART_FLOW_DIS
	};
	struct no_os_uart_init_param uart_init = {
		.device_id = 0,
		.baud_rate = 57600,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.platform_ops = &max_uart_ops,
		.extra = &uart_init_max,
	};
	struct max_gpio_init_param gpio_init_max = {
		.vssel = MXC_GPIO_VSSEL_VDDIOH,
	};
	struct no_os_gpio_init_param cs_init ={
		.port = 2,
		.number = 0,
		.extra = &gpio_init_max,
		.platform_ops = &max_gpio_ops,
		.pull = NO_OS_PULL_UP,
	};
	struct no_os_gpio_init_param dc_init ={
		.port = 2,
		.number = 1,
		.extra = &gpio_init_max,
		.platform_ops = &max_gpio_ops,
		.pull = NO_OS_PULL_UP,
	};
	struct no_os_gpio_init_param rst_init ={
		.port = 2,
		.number = 2,
		.extra = &gpio_init_max,
		.platform_ops = &max_gpio_ops,
		.pull = NO_OS_PULL_UP,
	};
	struct max_pwm_init_param pwm_init_max = {
		.vssel = MXC_GPIO_VSSEL_VDDIOH,
	};
	struct no_os_pwm_init_param bl_init= {
		.id = 3,
		.period_ns = 1000000,
		.duty_cycle_ns = 800000,
		.polarity = NO_OS_PWM_POLARITY_LOW,
		.platform_ops = &max_pwm_ops,
		.extra = &pwm_init_max,
	};
	struct max_spi_init_param spi_init_max  = {
		.num_slaves = 1,
		.polarity = SPI_SS_POL_LOW,
		.vssel = MXC_GPIO_VSSEL_VDDIOH,
	};
	struct no_os_spi_init_param spi_init = {
		.device_id = 0,
		.max_speed_hz = 10000000,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_0,
		.platform_ops = &max_spi_ops,
		.chip_select = 0,
		.extra = &spi_init_max,
	};
	struct ili9341_init_param ili_init = {
		.cs_init = &cs_init,
		.dc_init = &dc_init,
		.rst_init = &rst_init,
		.bl_init = &bl_init,
		.spi_init = &spi_init,
	};
	struct no_os_uart_desc *uart;
	struct ili9341 *ili;
	int ret;
	int i;

	ret = no_os_uart_init(&uart, &uart_init);
	if (ret < 0)
		return ret;
	
	no_os_uart_stdio(uart);

	printf("Hello\n\r");

	ili9341_init(&ili, &ili_init);

	ili9341_cmd(ili, 0x2C); // Write to RAM
	for (i = 0; i < 240 * 106; i++) {
		ili9341_data(ili, 0x001F >> 8);
		ili9341_data(ili, 0x001F & 0xff);
	}
	for (i = 0; i < 240 * 106; i++) {
		ili9341_data(ili, 0xFFE0 >> 8);
		ili9341_data(ili, 0xFFE0 & 0xff);
	}
	for (i = 0; i < 240 * 108; i++) {
		ili9341_data(ili, 0xF800 >> 8);
		ili9341_data(ili, 0xF800 & 0xff);
	}

	printf("Bye\n\r");

	return 0;
}
