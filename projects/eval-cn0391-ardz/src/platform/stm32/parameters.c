#include "parameters.h"

struct stm32_uart_init_param cn0391_uart_extra_ip = {
	.huart = UART_INSTANCE,
};

struct stm32_spi_init_param cn0391_spi_extra_ip = {
	.chip_select_port = SPI_CS_PORT,
	.get_input_clock = HAL_RCC_GetPCLK2Freq,
};

struct stm32_gpio_init_param adin1110_reset_gpio_extra_ip = {
	.mode = GPIO_MODE_OUTPUT_OD,
	.speed = GPIO_SPEED_FREQ_VERY_HIGH,
};

struct stm32_spi_init_param adin1110_spi_extra_ip = {
	.chip_select_port = ADIN1110_SPI_CS_PORT,
};
