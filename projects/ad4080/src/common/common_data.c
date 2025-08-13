#include <common_data.h>

struct no_os_gpio_init_param afe_ctrl_init_param = {
	.port = AFE_CTRL_PORT,
	.number = AFE_CTRL_NUMBER,
	.pull = AFE_CTRL_PULL,
	.platform_ops = AFE_CTRL_OPS,
	.extra = AFE_CTRL_EXTRA,
};

struct no_os_gpio_init_param gate_ctrl_q4_init_param = {
	.port = GATE_CTRL_Q4_PORT,
	.number = GATE_CTRL_Q4_NUMBER,
	.pull = GATE_CTRL_Q4_PULL,
	.platform_ops = GATE_CTRL_Q4_OPS,
	.extra = GATE_CTRL_Q4_EXTRA,
};

struct no_os_gpio_init_param gate_ctrl_q3_init_param = {
	.port = GATE_CTRL_Q3_PORT,
	.number = GATE_CTRL_Q3_NUMBER,
	.pull = GATE_CTRL_Q3_PULL,
	.platform_ops = GATE_CTRL_Q3_OPS,
	.extra = GATE_CTRL_Q3_EXTRA,
};

struct no_os_gpio_init_param gate_ctrl_q2_init_param = {
	.port = GATE_CTRL_Q2_PORT,
	.number = GATE_CTRL_Q2_NUMBER,
	.pull = GATE_CTRL_Q2_PULL,
	.platform_ops = GATE_CTRL_Q2_OPS,
	.extra = GATE_CTRL_Q2_EXTRA,
};

struct no_os_gpio_init_param gate_ctrl_q1_init_param = {
	.port = GATE_CTRL_Q1_PORT,
	.number = GATE_CTRL_Q1_NUMBER,
	.pull = GATE_CTRL_Q1_PULL,
	.platform_ops = GATE_CTRL_Q1_OPS,
	.extra = GATE_CTRL_Q1_EXTRA,
};

struct no_os_gpio_init_param gp3_init_param = {
	.port = GP3_PORT,
	.number = GP3_NUMBER,
	.pull = GP3_PULL,
	.platform_ops = GP3_OPS,
	.extra = GP3_EXTRA,
};

struct no_os_gpio_init_param data_ss_init_param = {
	.port = DATA_SWSS_PORT,
	.number = DATA_SWSS_NUMBER,
	.pull = DATA_SWSS_PULL,
	.platform_ops = DATA_SWSS_OPS,
	.extra = DATA_SWSS_EXTRA,
};

struct no_os_spi_init_param data_spi_init_param = {
	.device_id = DATA_SPI_DEVICE_ID,
	.max_speed_hz = DATA_SPI_SPEED,
	.chip_select = DATA_SPI_SS,
	.mode= DATA_SPI_MODE,
	.bit_order = DATA_SPI_BIT_ORDER,
	.lanes = DATA_SPI_LANES,
	.platform_ops = DATA_SPI_OPS,
	.extra = DATA_SPI_EXTRA,
};

struct no_os_gpio_init_param cfg_ss_init_param = {
	.port = CFG_SWSS_PORT,
	.number = CFG_SWSS_NUMBER,
	.pull = CFG_SWSS_PULL,
	.platform_ops = CFG_SWSS_OPS,
	.extra = CFG_SWSS_EXTRA,
};

struct no_os_spi_init_param cfg_spi_init_param = {
	.device_id = CFG_SPI_DEVICE_ID,
	.max_speed_hz = CFG_SPI_SPEED,
	.chip_select = CFG_SPI_SS,
	.mode= CFG_SPI_MODE,
	.bit_order = CFG_SPI_BIT_ORDER,
	.lanes = CFG_SPI_LANES,
	.platform_ops = CFG_SPI_OPS,
	.extra = CFG_SPI_EXTRA,
};

struct no_os_uart_init_param uart_init_param = {
	.device_id = IIO_UART_DEVICE_ID,
	.irq_id = IIO_UART_IRQ_ID,
	.asynchronous_rx = IIO_UART_ASYNC_RX,
	.baud_rate = IIO_UART_BAUD_RATE,
	.size = IIO_UART_SIZE,
	.parity = IIO_UART_PARITY,
	.stop = IIO_UART_STOP_BIT,
	.platform_ops = IIO_UART_OPS,
	.extra = IIO_UART_EXTRA,
};

