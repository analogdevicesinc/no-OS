#include "common_data.h"

struct no_os_uart_init_param adxl37x_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id    = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size      = NO_OS_UART_CS_8,
	.parity    = NO_OS_UART_PAR_NO,
	.stop      = NO_OS_UART_STOP_1_BIT,
	.extra     = UART_EXTRA,
	.platform_ops = UART_OPS,
};

struct adxl37x_init_param adxl37x_init_param = {
	.adxl_type      = ADXL372,
	.spi_init  = {
		.device_id    = SPI_DEVICE_ID,
		.max_speed_hz = SPI_BAUDRATE,
		.bit_order    = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode         = NO_OS_SPI_MODE_0,
		.platform_ops = SPI_OPS,
		.chip_select  = SPI_CS,
		.extra        = SPI_EXTRA,
	},
	.gpio_int1 = {
		.port         = GPIO_INT1_PORT,
		.number       = GPIO_INT1_PIN,
		.pull         = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra        = GPIO_EXTRA,
	},
	.gpio_int2 = {
		.port         = GPIO_INT2_PORT,
		.number       = GPIO_INT2_PIN,
		.pull         = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra        = GPIO_EXTRA,
	},
	.op_mode        = 	ADXL37x_FULL_BW_MEASUREMENT,
	.act_proc_mode  = ADXL37x_DEFAULT,
	.th_mode        = ADXL37x_INSTANT_ON_LOW_TH,
	.activity_th    = { .thresh = 0, .referenced = false, .enable = false },
	.activity2_th   = { .thresh = 0, .referenced = false, .enable = false },
	.inactivity_th  = { .thresh = 0, .referenced = false, .enable = false },
	.activity_time   = 0,
	.inactivity_time = 0,
	.fifo_config = {
		.fifo_mode    = ADXL37x_FIFO_STREAMED,
		.fifo_format  = ADXL37x_XYZ_FIFO,
		.fifo_samples = 128,
	},
	.int1_config = { .fifo_rdy = true },
	.int2_config = { 0 },
	.comm_type = ADXL37x_SPI
};
