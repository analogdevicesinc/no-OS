#include <stdio.h>
#include <string.h>
#include <errno.h>

/* UART includes */
#include "capi_uart.h"
#include "stm32_capi_uart.h"

/* SPI includes - CAPI implementation */
#include "capi_spi.h"
#include "stm32_capi_spi.h"
//
/* I2C includes - CAPI implementation */
#include "capi_i2c.h"
#include "stm32_capi_i2c.h"

/* DMA includes - CAPI implementation */
#include "capi_dma.h"
#include "stm32_capi_dma.h"
//
/* Timer includes - CAPI implementation */
#include "capi_timer.h"
#include "stm32_capi_timer.h"

/* IRQ includes - CAPI implementation */
#include "capi_irq.h"
#include "stm32_capi_irq.h"
//
/* PWM includes - CAPI implementation */
#include "stm32_capi_pwm.h"

/* Time includes - CAPI implementation */
#include "capi_time.h"

/* GPIO includes - CAPI implementation */
#include "capi_gpio.h"
#include "stm32_capi_gpio.h"

/* Function to get SPI peripheral input clock (APB clock) */
static uint32_t get_spi_input_clock(void)
{
	/* SPI1 is on APB2 for most STM32 families */
	return HAL_RCC_GetPCLK2Freq();
}

/* External HAL handles (configured by CubeMX) */
extern UART_HandleTypeDef huart3;

/* SPI handle - may not exist if SPI1 not enabled in CubeMX */
/* Using weak reference to avoid linker error if not configured */
__attribute__((weak)) SPI_HandleTypeDef hspi1;

/* I2C handle - may not exist if I2C1 not enabled in CubeMX */
__attribute__((weak)) I2C_HandleTypeDef hi2c1;

/* Global UART handle for direct debug output */
static struct capi_uart_handle *g_uart_handle = NULL;

/* Direct debug print that bypasses stdio buffering */
static void debug_print(const char *msg)
{
	if (g_uart_handle) {
		capi_uart_transmit(g_uart_handle, (uint8_t *)msg, strlen(msg));
	}
}

/* Print an integer value for debugging */
static void debug_print_int(const char *prefix, int value)
{
	char buf[32];
	int len = 0;
	int v = value;
	int i;

	/* Copy prefix */
	while (prefix[len] && len < 20) {
		buf[len] = prefix[len];
		len++;
	}

	/* Handle negative */
	if (v < 0) {
		buf[len++] = '-';
		v = -v;
	}

	/* Convert to string (reverse) */
	char tmp[12];
	i = 0;
	if (v == 0) {
		tmp[i++] = '0';
	} else {
		while (v > 0 && i < 10) {
			tmp[i++] = '0' + (v % 10);
			v /= 10;
		}
	}

	/* Reverse into buf */
	while (i > 0) {
		buf[len++] = tmp[--i];
	}

	buf[len++] = '\n';
	buf[len++] = '\r';
	buf[len] = '\0';

	debug_print(buf);
}

/* Print a byte array in hex format */
static void debug_print_hex(const char *prefix, const uint8_t *data, size_t len)
{
	char buf[128];
	size_t pos = 0;
	size_t i;
	const char hex_chars[] = "0123456789ABCDEF";

	/* Copy prefix */
	while (*prefix && pos < 60) {
		buf[pos++] = *prefix++;
	}

	/* Print each byte as hex */
	for (i = 0; i < len && pos < 120; i++) {
		buf[pos++] = '0';
		buf[pos++] = 'x';
		buf[pos++] = hex_chars[(data[i] >> 4) & 0x0F];
		buf[pos++] = hex_chars[data[i] & 0x0F];
		if (i < len - 1) {
			buf[pos++] = ' ';
		}
	}

	buf[pos++] = '\n';
	buf[pos++] = '\r';
	buf[pos] = '\0';

	debug_print(buf);
}

/* Compare two byte arrays and return 0 if identical */
static int verify_data(const uint8_t *tx, const uint8_t *rx, size_t len)
{
	size_t i;
	for (i = 0; i < len; i++) {
		if (tx[i] != rx[i]) {
			return -1;
		}
	}
	return 0;
}

/******************************************************************************/
/************************* CAPI SPI Example ***********************************/
/******************************************************************************/

/**
 * @brief Example using the CAPI SPI interface
 * @return 0 on success, negative error code on failure
 */
static int capi_spi_example(void)
{
	struct capi_spi_controller_handle *spi_handle = NULL;
	int ret;

	debug_print("=== CAPI SPI Example ===\n\r");

	/* Check if SPI1 is configured (hspi1.Instance will be non-NULL if initialized) */
	if (hspi1.Instance == NULL) {
		debug_print("[CAPI] SPI1 not configured in CubeMX, skipping\n\r");
		return 0;
	}

	debug_print("[CAPI] Setting up config structures...\n\r");

	struct stm32_spi_extra_config stm32_spi_extra = {
		.hspi = &hspi1,
		.chip_select_port = 0,  /* Will be set per-device */
		.get_input_clock = get_spi_input_clock,
		.alternate = 0,
		.dma_handle = NULL,
		.rxdma_ch_id = 0,
		.txdma_ch_id = 0,
	};

	debug_print("[CAPI] stm32_spi_extra configured\n\r");

	struct capi_spi_config spi_config = {
		.ops = &stm32_capi_spi_ops,
		.identifier = 1,  /* SPI1 */
		.dma_handle = NULL,
		.three_pin_mode = false,
		.loopback = false,
		.clk_freq_hz = 1000000,  /* 1 MHz */
		.extra = &stm32_spi_extra,
	};

	debug_print("[CAPI] spi_config configured, calling init...\n\r");

	/* Initialize SPI controller */
	ret = capi_spi_init(&spi_handle, &spi_config);
	if (ret) {
		debug_print("CAPI SPI init FAILED\n\r");
		return ret;
	}
	debug_print("CAPI SPI initialized OK\n\r");

	/* Configure a device on the SPI bus */
	struct stm32_spi_device_extra dev_extra = {
		.chip_select_port = (uint32_t)GPIOA,  /* CS on GPIOA */
		.chip_select_pin = 4,                  /* Pin 4 */
		.cs_delay_first = 0,
		.cs_delay_last = 0,
		.cs_change_delay = 0,
	};
	struct capi_spi_device spi_device = {
		.controller = spi_handle,
		.max_speed_hz = 1000000,
		.mode = CAPI_SPI_MODE_0,
		.native_cs = 0,
		.cs_gpio = NULL,
		.cs_gpio_num = 0,
		.non_continuous_mode = false,
		.lsb_first = false,
		.extra = &dev_extra,
	};

	/* Prepare test data */
	uint8_t tx_data[] = {0xAA, 0x55, 0x12, 0x34};
	uint8_t rx_data[4] = {0};

	struct capi_spi_transfer transfer = {
		.tx_buf = tx_data,
		.rx_buf = rx_data,
		.tx_size = sizeof(tx_data),
		.rx_size = sizeof(rx_data),
		.timeout = 1000,
		.xfer_delay_clk_cycles = 0,
	};

	debug_print("[CAPI] Performing CAPI SPI transfer...\n\r");
	debug_print_hex("[CAPI] TX data: ", tx_data, sizeof(tx_data));

	/* Perform SPI transfer */
	ret = capi_spi_transceive(&spi_device, &transfer);
	if (ret) {
		debug_print("CAPI SPI transceive FAILED\n\r");
		debug_print_int("Error code: ", ret);
		capi_spi_deinit(spi_handle);
		return ret;
	}

	debug_print("CAPI SPI transfer OK\n\r");
	debug_print_hex("[CAPI] RX data: ", rx_data, sizeof(rx_data));

	/* Verify loopback: TX data should match RX data */
	if (memcmp(tx_data, rx_data, sizeof(tx_data)) == 0) {
		debug_print("[CAPI] Loopback verification PASSED\n\r");
	} else {
		debug_print("[CAPI] Loopback verification FAILED\n\r");
		capi_spi_deinit(spi_handle);
		return -1;
	}

	/* Clean up */
	ret = capi_spi_deinit(spi_handle);
	if (ret) {
		debug_print("CAPI SPI deinit FAILED\n\r");
		return ret;
	}
	debug_print("CAPI SPI example completed\n\r");

	return 0;
}

/******************************************************************************/
/************************* CAPI I2C Example ***********************************/
/******************************************************************************/

/*
 * TODO: Implement a proper I2C example with external hardware.
 *
 * I2C requires an external slave device to properly test transactions.
 * This example only tests init/deinit and verifies that error handling
 * works correctly when no device is present.
 *
 * For a complete I2C test, connect an I2C device (e.g., EEPROM, sensor)
 * and implement read/write verification.
 */

/**
 * @brief Example using the CAPI I2C interface
 *
 * Tests init/deinit and error handling only.
 * Full transaction testing requires external I2C hardware.
 *
 * @return 0 on success, negative error code on failure
 */
static int capi_i2c_example(void)
{
	struct capi_i2c_controller_handle *i2c_handle = NULL;
	int ret;

	debug_print("=== CAPI I2C Example ===\n\r");

	/* Check if I2C1 is configured */
	if (hi2c1.Instance == NULL) {
		debug_print("[CAPI] I2C1 not configured in CubeMX, skipping\n\r");
		return 0;
	}

	debug_print("[CAPI] Setting up I2C config structures...\n\r");

	struct stm32_i2c_extra_config stm32_i2c_extra = {
		.hi2c = &hi2c1,
		.i2c_timing = 0x10909CEC,  /* Default timing for 100kHz */
	};

	struct capi_i2c_config i2c_config = {
		.ops = &stm32_capi_i2c_ops,
		.identifier = 1,  /* I2C1 */
		.clk_freq_hz = 100000,  /* 100 kHz */
		.initiator = true,
		.address = 0,
		.device = NULL,
		.dma_handle = NULL,
		.extra = &stm32_i2c_extra,
	};

	/* Test 1: Initialize I2C controller */
	debug_print("[CAPI] Test 1: I2C init...\n\r");

	ret = capi_i2c_init(&i2c_handle, &i2c_config);
	if (ret) {
		debug_print("CAPI I2C init FAILED\n\r");
		debug_print_int("Error code: ", ret);
		return ret;
	}
	debug_print("CAPI I2C init OK\n\r");

	/*
	 * Note: Error handling test (transaction to non-existent device) is skipped.
	 * The stm32_capi_i2c driver uses HAL_MAX_DELAY for timeout, which causes
	 * the transmit to hang indefinitely when no device responds.
	 * TODO: Add a timeout parameter to capi_i2c_transfer or use a reasonable
	 * default timeout in the driver.
	 */
	debug_print("[CAPI] Skipping transaction test (no external device)\n\r");

	/* Test 2: Deinitialize I2C controller */
	debug_print("[CAPI] Test 2: I2C deinit...\n\r");

	ret = capi_i2c_deinit(i2c_handle);
	if (ret) {
		debug_print("CAPI I2C deinit FAILED\n\r");
		debug_print_int("Error code: ", ret);
		return ret;
	}
	debug_print("CAPI I2C deinit OK\n\r");

	debug_print("CAPI I2C example completed\n\r");
	debug_print("[CAPI] Note: Full I2C testing requires external hardware\n\r");

	return 0;
}

/******************************************************************************/
/************************* CAPI DMA Example ***********************************/
/******************************************************************************/

/* DMA test buffers - aligned for DMA access */
static uint8_t dma_src_buffer[64] __attribute__((aligned(4)));
static uint8_t dma_dst_buffer[64] __attribute__((aligned(4)));

/* DMA handle for memory-to-memory transfers */
static DMA_HandleTypeDef hdma_memtomem;

/* Flag for async completion */
static volatile bool capi_dma_transfer_done = false;

static void capi_dma_xfer_complete(uint32_t event, void *ctx)
{
	(void)event;
	(void)ctx;
	capi_dma_transfer_done = true;
}

/* DMA channel pointer for IRQ handler */
static struct capi_dma_chan *g_dma_chan = NULL;

/* DMA2 Stream0 IRQ handler - calls CAPI DMA ISR */
void DMA2_Stream0_IRQHandler(void)
{
	if (g_dma_chan)
		capi_dma_isr_chan(g_dma_chan);
}

/**
 * @brief Example using the CAPI DMA interface
 * @return 0 on success, negative error code on failure
 */
static int capi_dma_example(void)
{
	struct capi_dma_handle *dma_handle = NULL;
	struct capi_dma_chan *dma_chan = NULL;
	int ret;
	uint32_t i;
	uint32_t timeout;

	debug_print("=== CAPI DMA Example ===\n\r");

	/* Enable DMA2 clock (required for memory-to-memory on STM32F7) */
	__HAL_RCC_DMA2_CLK_ENABLE();

	/* Initialize source buffer with test pattern */
	for (i = 0; i < sizeof(dma_src_buffer); i++)
		dma_src_buffer[i] = (uint8_t)(i ^ 0xAA);

	/* Clear destination buffer */
	memset(dma_dst_buffer, 0, sizeof(dma_dst_buffer));

	debug_print("[CAPI] Setting up DMA config structures...\n\r");

	struct capi_dma_config dma_config = {
		.ops = &stm32_capi_dma_ops,
		.id = 2,  /* DMA2 supports memory-to-memory */
		.num_chans = 8,
		.irq_handle = NULL,
		.extra = NULL,
	};

	ret = capi_dma_init(&dma_handle, &dma_config);
	if (ret) {
		debug_print("CAPI DMA init FAILED\n\r");
		debug_print_int("Error code: ", ret);
		return ret;
	}
	debug_print("CAPI DMA initialized OK\n\r");

	/* Initialize a DMA channel */
	ret = capi_dma_init_chan(dma_handle, &dma_chan, 0);
	if (ret) {
		debug_print("CAPI DMA init_chan FAILED\n\r");
		debug_print_int("Error code: ", ret);
		capi_dma_deinit(dma_handle);
		return ret;
	}
	debug_print("CAPI DMA channel initialized OK\n\r");

	/* Register transfer complete callback */
	capi_dma_register_complete_callback(dma_chan, capi_dma_xfer_complete, NULL);

	/* Store channel pointer for IRQ handler and enable interrupt mode */
	g_dma_chan = dma_chan;
	dma_chan->irq_num = DMA2_Stream0_IRQn + 1;  /* Non-zero to enable interrupt mode */

	/* Enable DMA2 Stream0 interrupt in NVIC */
	HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

	/* Configure the HAL DMA handle with DMA2 Stream0 */
	hdma_memtomem.Instance = DMA2_Stream0;
	hdma_memtomem.Init.Channel = DMA_CHANNEL_0;
	hdma_memtomem.Init.Direction = DMA_MEMORY_TO_MEMORY;
	hdma_memtomem.Init.PeriphInc = DMA_PINC_ENABLE;
	hdma_memtomem.Init.MemInc = DMA_MINC_ENABLE;
	hdma_memtomem.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_memtomem.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	hdma_memtomem.Init.Mode = DMA_NORMAL;
	hdma_memtomem.Init.Priority = DMA_PRIORITY_LOW;
	hdma_memtomem.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

	/* Configure channel extra settings */
	struct stm32_dma_chan_extra_config chan_extra = {
		.hdma = &hdma_memtomem,
		.ch_num = DMA_CHANNEL_0,
		.mem_increment = true,
		.per_increment = true,
		.mem_data_alignment = CAPI_DMA_DATA_ALIGN_BYTE,
		.per_data_alignment = CAPI_DMA_DATA_ALIGN_BYTE,
		.dma_mode = CAPI_DMA_NORMAL_MODE,
		.trig = NULL,
	};

	/* Configure transfer */
	struct capi_dma_transfer dma_xfer = {
		.src = (capi_dma_glbl_addr_t)dma_src_buffer,
		.dst = (capi_dma_glbl_addr_t)dma_dst_buffer,
		.length = sizeof(dma_src_buffer),
		.xfer_type = CAPI_DMA_MEM_TO_MEM,
		.src_inc = CAPI_DMA_BYTE_INCREMENT,
		.dst_inc = CAPI_DMA_BYTE_INCREMENT,
		.irq_priority = 0,
		.user_data = NULL,
		.extra = &chan_extra,
	};

	capi_dma_transfer_done = false;

	debug_print("[CAPI] Configuring DMA transfer...\n\r");
	ret = capi_dma_config_xfer(dma_chan, &dma_xfer);
	if (ret) {
		debug_print("CAPI DMA config_xfer FAILED\n\r");
		debug_print_int("Error code: ", ret);
		capi_dma_deinit_chan(dma_chan);
		capi_dma_deinit(dma_handle);
		return ret;
	}
	debug_print("CAPI DMA config_xfer OK\n\r");

	debug_print("[CAPI] Starting DMA transfer...\n\r");
	debug_print_hex("[CAPI] Src data: ", dma_src_buffer, 8);

	/* Clean D-Cache to ensure source data is in main memory for DMA */
	SCB_CleanDCache_by_Addr((uint32_t *)dma_src_buffer, sizeof(dma_src_buffer));

	ret = capi_dma_xfer_start(dma_chan);
	if (ret) {
		debug_print("CAPI DMA xfer_start FAILED\n\r");
		debug_print_int("Error code: ", ret);
		capi_dma_deinit_chan(dma_chan);
		capi_dma_deinit(dma_handle);
		return ret;
	}

	/* Wait for transfer to complete (polling with timeout) */
	timeout = 100000;
	while (!capi_dma_chan_is_completed(dma_chan)) {
		if (--timeout == 0) {
			debug_print("CAPI DMA transfer timeout\n\r");
			capi_dma_deinit_chan(dma_chan);
			capi_dma_deinit(dma_handle);
			return -1;
		}
	}

	/* Invalidate D-Cache for destination to read fresh DMA data */
	SCB_InvalidateDCache_by_Addr((uint32_t *)dma_dst_buffer, sizeof(dma_dst_buffer));

	debug_print("CAPI DMA transfer completed\n\r");
	debug_print_hex("[CAPI] Dst data: ", dma_dst_buffer, 8);

	/* Verify data */
	if (verify_data(dma_src_buffer, dma_dst_buffer, sizeof(dma_src_buffer)) == 0) {
		debug_print("[CAPI] DMA data verification PASSED\n\r");
	} else {
		debug_print("[CAPI] DMA data verification FAILED\n\r");
		capi_dma_deinit_chan(dma_chan);
		capi_dma_deinit(dma_handle);
		return -1;
	}

	/* Clean up */
	capi_dma_deinit_chan(dma_chan);
	capi_dma_deinit(dma_handle);

	debug_print("CAPI DMA example completed\n\r");

	return 0;
}

// /******************************************************************************/
// /************************* CAPI Timer Example *********************************/
// /******************************************************************************/
//
// #ifdef HAL_TIM_MODULE_ENABLED
//
// /* Timer handle from generated_main.c */
// extern TIM_HandleTypeDef htim2;
//
// /* Get TIM2 input clock - on STM32F7, timer clock is 2x APB1 when APB1 prescaler > 1 */
// static uint32_t get_tim2_input_clock(void)
// {
// 	RCC_ClkInitTypeDef clk_config;
// 	uint32_t flash_latency;
// 	uint32_t apb1_freq;
//
// 	HAL_RCC_GetClockConfig(&clk_config, &flash_latency);
// 	apb1_freq = HAL_RCC_GetPCLK1Freq();
//
// 	/* If APB1 prescaler > 1, timer clock is 2x APB1 */
// 	if (clk_config.APB1CLKDivider != RCC_HCLK_DIV1)
// 		return apb1_freq * 2;
//
// 	return apb1_freq;
// }
//
// /* Timer callback flags */
// static volatile bool timer_overflow_occurred = false;
// static volatile bool timer_compare_occurred = false;
// static volatile uint32_t timer_overflow_count = 0;
//
// static void timer_event_callback(uint32_t event, void *arg, int event_extra)
// {
// 	(void)arg;
// 	(void)event_extra;
//
// 	if (event == CAPI_TIMER_GLOBAL_EVENT_COUNTER_OVERFLOW) {
// 		timer_overflow_occurred = true;
// 		timer_overflow_count++;
// 	}
// }
//
// static void timer_channel_callback(uint32_t event, uint32_t chan, void *arg,
// 				   int event_extra)
// {
// 	(void)arg;
// 	(void)event_extra;
// 	(void)chan;
//
// 	if (event == CAPI_TIMER_CHANNEL_EVENT_COMPARE) {
// 		timer_compare_occurred = true;
// 	}
// }
//
// /**
//  * @brief Example using the CAPI Timer interface
//  * @return 0 on success, negative error code on failure
//  */
// static int capi_timer_example(void)
// {
// 	struct capi_timer_handle *timer_handle = NULL;
// 	int ret;
// 	uint32_t ticks;
//
// 	debug_print("=== CAPI Timer Example ===\n\r");
//
// 	/* Check if TIM2 is configured */
// 	if (htim2.Instance == NULL) {
// 		debug_print("[CAPI] TIM2 not configured in CubeMX, skipping\n\r");
// 		return 0;
// 	}
//
// 	debug_print("[CAPI] Setting up timer config structures...\n\r");
//
// 	struct stm32_capi_timer_extra_config timer_extra = {
// 		.htim = &htim2,
// 		.get_input_clock = get_tim2_input_clock,
// 		.irq_num = 0,
// 	};
//
// 	struct capi_timer_config timer_config = {
// 		.ops = &stm32_capi_timer_ops,
// 		.identifier = 2,
// 		.input_clock_identifier = 0,
// 		.input_clock_hz = 0,
// 		.output_freq_hz = 1000000,
// 		.extra = &timer_extra,
// 	};
//
// 	debug_print("[CAPI] timer_config configured, calling init...\n\r");
//
// 	ret = capi_timer_init(&timer_handle, &timer_config);
// 	if (ret) {
// 		debug_print("CAPI Timer init FAILED\n\r");
// 		debug_print_int("Error code: ", ret);
// 		return ret;
// 	}
// 	debug_print("CAPI Timer initialized OK\n\r");
//
// 	ret = capi_timer_register_event_callback(timer_handle,
// 						 timer_event_callback, NULL);
// 	if (ret) {
// 		debug_print("CAPI Timer register_event_callback FAILED\n\r");
// 		capi_timer_deinit(timer_handle);
// 		return ret;
// 	}
//
// 	struct capi_timer_counter_config counter_config = {
// 		.direction = CAPI_TIMER_COUNT_UP,
// 		.min = 0,
// 		.max = 10000,
// 		.rollover = true,
// 		.extra = NULL,
// 	};
//
// 	ret = capi_timer_counter_config(timer_handle, &counter_config);
// 	if (ret) {
// 		debug_print("CAPI Timer counter_config FAILED\n\r");
// 		capi_timer_deinit(timer_handle);
// 		return ret;
// 	}
// 	debug_print("CAPI Timer counter configured OK\n\r");
//
// 	debug_print("[CAPI] Starting timer...\n\r");
// 	ret = capi_timer_start(timer_handle);
// 	if (ret) {
// 		debug_print("CAPI Timer start FAILED\n\r");
// 		capi_timer_deinit(timer_handle);
// 		return ret;
// 	}
// 	debug_print("CAPI Timer started OK\n\r");
//
// 	/* Counter verification test */
// 	debug_print("[CAPI] Starting counter verification test...\n\r");
//
// 	uint32_t counter_start, counter_end, elapsed;
// 	uint32_t delay_ms = 8;
// 	uint32_t expected_ticks = delay_ms * 1000; /* 1 MHz = 1000 ticks/ms */
// 	uint32_t tolerance = expected_ticks / 5;   /* 20% tolerance for HAL_GetTick granularity */
//
// 	/* Stop and reconfigure timer to reset counter to 0 */
// 	capi_timer_stop(timer_handle);
// 	capi_timer_counter_config(timer_handle, &counter_config);
// 	capi_timer_start(timer_handle);
//
// 	uint32_t tick_start, tick_end;
//
// 	tick_start = HAL_GetTick();
// 	ret = capi_timer_counter_get(timer_handle, &counter_start);
// 	if (ret) {
// 		debug_print("CAPI Timer counter_get (start) FAILED\n\r");
// 		capi_timer_deinit(timer_handle);
// 		return ret;
// 	}
//
// 	HAL_Delay(delay_ms);
//
// 	ret = capi_timer_counter_get(timer_handle, &counter_end);
// 	tick_end = HAL_GetTick();
// 	if (ret) {
// 		debug_print("CAPI Timer counter_get (end) FAILED\n\r");
// 		capi_timer_deinit(timer_handle);
// 		return ret;
// 	}
//
// 	/* Print after both reads to avoid UART time affecting measurement */
// 	debug_print_int("[CAPI] Counter start: ", counter_start);
// 	debug_print_int("[CAPI] Counter end: ", counter_end);
// 	debug_print_int("[CAPI] HAL ticks elapsed: ", tick_end - tick_start);
//
// 	/* Handle rollover (counter max is 10000) */
// 	if (counter_end >= counter_start) {
// 		elapsed = counter_end - counter_start;
// 	} else {
// 		elapsed = (10000 - counter_start) + counter_end;
// 	}
//
// 	/* Calculate expected based on actual HAL ticks elapsed (more accurate) */
// 	uint32_t hal_elapsed = tick_end - tick_start;
// 	expected_ticks = hal_elapsed * 1000; /* 1 MHz = 1000 ticks/ms */
// 	tolerance = expected_ticks / 5;      /* 20% tolerance for HAL_GetTick granularity */
//
// 	debug_print_int("[CAPI] Elapsed ticks: ", elapsed);
// 	debug_print_int("[CAPI] Expected ticks: ", expected_ticks);
//
// 	/* Verify elapsed ticks within tolerance */
// 	if (elapsed >= (expected_ticks - tolerance) &&
// 	    elapsed <= (expected_ticks + tolerance)) {
// 		debug_print("[CAPI] Counter verification PASSED\n\r");
// 	} else {
// 		debug_print("[CAPI] Counter verification FAILED\n\r");
// 		capi_timer_deinit(timer_handle);
// 		return -1;
// 	}
//
// 	debug_print("[CAPI] Testing time conversion...\n\r");
// 	ret = capi_timer_nsec_to_ticks(timer_handle, 1000000, &ticks);
// 	if (ret == 0) {
// 		debug_print_int("[CAPI] 1ms = ticks: ", ticks);
// 	}
//
// 	debug_print("[CAPI] Configuring compare channel 0...\n\r");
// 	ret = capi_timer_channel_init(timer_handle, 0);
// 	if (ret) {
// 		debug_print("CAPI Timer channel_init FAILED\n\r");
// 	} else {
// 		struct capi_timer_channel_config ch_config = {
// 			.mode = CAPI_TIMER_COMPARE_MODE,
// 			.config.compare = {
// 				.generate_pulse_on_match = false,
// 				.output_identifier = 0,
// 				.polarity = CAPI_TIMER_ON_COMPARE_KEEP,
// 				.stop_enabled = false,
// 				.match_value = 5000,
// 				.extra = NULL,
// 			},
// 			.extra = NULL,
// 		};
//
// 		ret = capi_timer_channel_config(timer_handle, 0, &ch_config);
// 		if (ret) {
// 			debug_print("CAPI Timer channel_config FAILED\n\r");
// 		} else {
// 			debug_print("CAPI Timer compare channel configured OK\n\r");
//
// 			ret = capi_timer_channel_register_callback(timer_handle, 0,
// 								   timer_channel_callback,
// 								   NULL);
// 			if (ret == 0) {
// 				debug_print("CAPI Timer channel callback registered OK\n\r");
// 			}
//
// 			ret = capi_timer_channel_enable(timer_handle, 0);
// 			if (ret == 0) {
// 				debug_print("CAPI Timer compare channel enabled OK\n\r");
// 			}
//
// 			uint32_t compare_val;
// 			ret = capi_timer_channel_compare_get(timer_handle, 0, &compare_val);
// 			if (ret == 0) {
// 				debug_print_int("[CAPI] Compare value: ", compare_val);
// 			}
// 		}
// 	}
//
// 	debug_print("[CAPI] Configuring PWM on channel 1...\n\r");
// 	ret = capi_timer_channel_init(timer_handle, 1);
// 	if (ret) {
// 		debug_print("CAPI Timer channel 1 init FAILED\n\r");
// 	} else {
// 		struct capi_timer_channel_config pwm_config = {
// 			.mode = CAPI_TIMER_PWM_MODE,
// 			.config.pwm = {
// 				.output_identifier = 0,
// 				.inverted_polarity = false,
// 				.period_ns = 1000000,
// 				.active_ns = 500000,
// 				.offset_ns = 0,
// 				.extra = NULL,
// 			},
// 			.extra = NULL,
// 		};
//
// 		ret = capi_timer_channel_config(timer_handle, 1, &pwm_config);
// 		if (ret) {
// 			debug_print("CAPI Timer PWM channel_config FAILED\n\r");
// 			debug_print_int("Error code: ", ret);
// 		} else {
// 			debug_print("CAPI Timer PWM channel configured OK\n\r");
//
// 			ret = capi_timer_channel_enable(timer_handle, 1);
// 			if (ret == 0) {
// 				debug_print("CAPI Timer PWM channel enabled OK\n\r");
// 			}
// 		}
// 	}
//
// 	debug_print("[CAPI] Stopping timer...\n\r");
// 	ret = capi_timer_stop(timer_handle);
// 	if (ret) {
// 		debug_print("CAPI Timer stop FAILED\n\r");
// 	} else {
// 		debug_print("CAPI Timer stopped OK\n\r");
// 	}
//
// 	ret = capi_timer_channel_disable(timer_handle, 0);
// 	ret = capi_timer_channel_deinit(timer_handle, 0);
// 	ret = capi_timer_channel_disable(timer_handle, 1);
// 	ret = capi_timer_channel_deinit(timer_handle, 1);
//
// 	ret = capi_timer_deinit(timer_handle);
// 	if (ret) {
// 		debug_print("CAPI Timer deinit FAILED\n\r");
// 		return ret;
// 	}
// 	debug_print("CAPI Timer example completed\n\r");
//
// 	return 0;
// }
//
// #endif /* HAL_TIM_MODULE_ENABLED */

/******************************************************************************/
/************************* CAPI IRQ Example ***********************************/
/******************************************************************************/

/* IRQ callback counter */
static volatile uint32_t irq_callback_count = 0;

/* IRQ test callback */
static void irq_test_callback(void *arg)
{
	(void)arg;
	irq_callback_count++;
}

/* External CAPI IRQ dispatch function */
extern void stm32_capi_irq_handler(uint32_t irq);

/* Define IRQ handler for our test interrupt - calls CAPI dispatch */
void EXTI15_10_IRQHandler(void)
{
	stm32_capi_irq_handler(EXTI15_10_IRQn);
}

/**
 * @brief Example using the CAPI IRQ interface
 * @return 0 on success, negative error code on failure
 */
static int capi_irq_example(void)
{
	int ret;
	uint32_t priority;
	uint32_t status;

	debug_print("=== CAPI IRQ Example ===\n\r");

	debug_print("[CAPI] Setting up IRQ config structures...\n\r");

	struct stm32_capi_irq_extra_config irq_extra = {
		.priority_grouping = NVIC_PRIORITYGROUP_4,
		.default_preempt_priority = 5,
		.default_sub_priority = 0,
	};

	struct capi_irq_config irq_config = {
		.irq_ctrl_id = 0,
		.extra = &irq_extra,
	};

	debug_print("[CAPI] Calling capi_irq_init...\n\r");

	ret = capi_irq_init(&irq_config);
	if (ret) {
		debug_print("CAPI IRQ init FAILED\n\r");
		debug_print_int("Error code: ", ret);
		return ret;
	}
	debug_print("CAPI IRQ initialized OK\n\r");

	/* Test global enable/disable */
	debug_print("[CAPI] Testing global IRQ enable/disable...\n\r");

	ret = capi_irq_global_disable();
	if (ret == 0) {
		debug_print("CAPI IRQ global disable OK\n\r");
	}

	ret = capi_irq_global_enable();
	if (ret == 0) {
		debug_print("CAPI IRQ global enable OK\n\r");
	}

	/* Test priority functions with SysTick IRQ (always present) */
	debug_print("[CAPI] Testing IRQ priority functions...\n\r");

	ret = capi_irq_get_priority(SysTick_IRQn, &priority);
	if (ret == 0) {
		debug_print_int("[CAPI] SysTick priority: ", priority);
	}

	ret = capi_irq_set_priority(SysTick_IRQn, 3);
	if (ret == 0) {
		debug_print("CAPI IRQ set_priority OK\n\r");
	}

	ret = capi_irq_get_priority(SysTick_IRQn, &priority);
	if (ret == 0) {
		debug_print_int("[CAPI] SysTick new priority: ", priority);
	}

	/* Test clear/set pending */
	debug_print("[CAPI] Testing IRQ pending functions...\n\r");

	ret = capi_irq_get_status(SysTick_IRQn, &status);
	if (ret == 0) {
		debug_print_int("[CAPI] SysTick status: ", status);
	}

	ret = capi_irq_clear_pending(SysTick_IRQn);
	if (ret == 0) {
		debug_print("CAPI IRQ clear_pending OK\n\r");
	}

	/* Test callback with software-triggered interrupt */
	debug_print("[CAPI] Testing IRQ callback with software trigger...\n\r");

	/* Use an unused IRQ for testing - EXTI15_10 is typically available */
	#define TEST_SW_IRQ EXTI15_10_IRQn

	/* Reset callback counter */
	irq_callback_count = 0;

	/* Connect callback to test IRQ */
	ret = capi_irq_connect(TEST_SW_IRQ, irq_test_callback, NULL);
	if (ret == 0) {
		debug_print("CAPI IRQ connect OK\n\r");
	} else {
		debug_print("CAPI IRQ connect FAILED\n\r");
		debug_print_int("Error code: ", ret);
		capi_irq_deinit();
		return ret;
	}

	ret = capi_irq_set_priority(TEST_SW_IRQ, 5);
	if (ret == 0) {
		debug_print("CAPI IRQ set_priority OK\n\r");
	}

	/* Enable the IRQ in NVIC */
	ret = capi_irq_enable(TEST_SW_IRQ);
	if (ret == 0) {
		debug_print("CAPI IRQ enable OK\n\r");
	}

	/* Check if enabled */
	bool enabled;
	ret = stm32_capi_irq_is_enabled(TEST_SW_IRQ, &enabled);
	if (ret == 0) {
		debug_print_int("[CAPI] IRQ enabled: ", enabled);
	}

	/* Trigger the interrupt via software multiple times */
	debug_print("[CAPI] Triggering IRQ via software...\n\r");
	for (int i = 0; i < 5; i++) {
		NVIC_SetPendingIRQ(TEST_SW_IRQ);
		/* Small delay to allow interrupt to fire */
		for (volatile int j = 0; j < 1000; j++);
	}

	debug_print_int("[CAPI] IRQ callback count: ", irq_callback_count);

	/* Verify callbacks were received */
	if (irq_callback_count >= 1) {
		debug_print("[CAPI] IRQ callback verification PASSED\n\r");
	} else {
		debug_print("[CAPI] IRQ callback verification FAILED\n\r");
		capi_irq_disable(TEST_SW_IRQ);
		stm32_capi_irq_disconnect(TEST_SW_IRQ);
		capi_irq_deinit();
		return -1;
	}

	/* Disable the test IRQ */
	ret = capi_irq_disable(TEST_SW_IRQ);
	if (ret == 0) {
		debug_print("CAPI IRQ disable OK\n\r");
	}

	/* Disconnect the callback */
	ret = stm32_capi_irq_disconnect(TEST_SW_IRQ);
	if (ret == 0) {
		debug_print("CAPI IRQ disconnect OK\n\r");
	}

	/* Deinitialize */
	ret = capi_irq_deinit();
	if (ret) {
		debug_print("CAPI IRQ deinit FAILED\n\r");
		return ret;
	}
	debug_print("CAPI IRQ example completed\n\r");

	return 0;
}

/******************************************************************************/
/************************* CAPI GPIO Example **********************************/
/******************************************************************************/

/* Test GPIO pin number (PG0) */
#define TEST_GPIO_PIN 0

/**
 * @brief Example using the CAPI GPIO interface
 * @return 0 on success, negative error code on failure
 */
static int capi_gpio_example(void)
{
	struct capi_gpio_port_handle *gpio_port = NULL;
	int ret;
	uint8_t value;
	uint8_t direction;
	uint64_t port_value;

	debug_print("=== CAPI GPIO Example ===\n\r");

	/* Test 1: Initialize GPIO port with pull-up configuration */
	debug_print("[CAPI] Test 1: Port init with pull-up...\n\r");

	struct stm32_capi_gpio_port_config port_extra = {
		.mode = GPIO_MODE_INPUT,
		.speed = GPIO_SPEED_FREQ_LOW,
		.alternate = 0,
		.pull = GPIO_PULLUP,
	};

	struct capi_gpio_port_config port_config = {
		.ops = &stm32_capi_gpio_ops,
		.identifier = 6,  /* GPIOG = port 6 */
		.num_pins = 16,
		.flags = NULL,
		.extra = &port_extra,
	};

	ret = capi_gpio_port_init(&gpio_port, &port_config);
	if (ret) {
		debug_print("CAPI GPIO port_init FAILED\n\r");
		debug_print_int("Error code: ", ret);
		return ret;
	}
	debug_print("CAPI GPIO port initialized OK\n\r");

	/* Create a pin descriptor for testing */
	struct capi_gpio_pin test_pin = {
		.port_handle = gpio_port,
		.number = TEST_GPIO_PIN,
		.flags = CAPI_GPIO_ACTIVE_HIGH,
	};

	/* Test 2: Set pin as input and read with pull-up */
	debug_print("[CAPI] Test 2: Input with pull-up...\n\r");

	ret = capi_gpio_pin_set_direction(&test_pin, CAPI_GPIO_INPUT);
	if (ret) {
		debug_print("CAPI GPIO pin_set_direction FAILED\n\r");
		capi_gpio_port_deinit(&gpio_port);
		return ret;
	}

	ret = capi_gpio_pin_get_direction(&test_pin, &direction);
	if (ret == 0) {
		debug_print_int("[CAPI] Direction (1=input): ", direction);
	}

	ret = capi_gpio_pin_get_value(&test_pin, &value);
	if (ret == 0) {
		debug_print_int("[CAPI] Pin value with pull-up: ", value);
		if (value == CAPI_GPIO_HIGH) {
			debug_print("[CAPI] Pull-up verification PASSED\n\r");
		} else {
			debug_print("[CAPI] Pull-up verification FAILED (expected HIGH)\n\r");
		}
	}

	/* Test 3: Reinitialize with pull-down and verify */
	debug_print("[CAPI] Test 3: Reinit with pull-down...\n\r");

	capi_gpio_port_deinit(&gpio_port);

	port_extra.pull = GPIO_PULLDOWN;
	gpio_port = NULL;

	ret = capi_gpio_port_init(&gpio_port, &port_config);
	if (ret) {
		debug_print("CAPI GPIO port_init (pull-down) FAILED\n\r");
		return ret;
	}

	/* Update pin descriptor with new port handle */
	test_pin.port_handle = gpio_port;

	ret = capi_gpio_pin_set_direction(&test_pin, CAPI_GPIO_INPUT);
	if (ret) {
		debug_print("CAPI GPIO pin_set_direction FAILED\n\r");
		capi_gpio_port_deinit(&gpio_port);
		return ret;
	}

	ret = capi_gpio_pin_get_value(&test_pin, &value);
	if (ret == 0) {
		debug_print_int("[CAPI] Pin value with pull-down: ", value);
		if (value == CAPI_GPIO_LOW) {
			debug_print("[CAPI] Pull-down verification PASSED\n\r");
		} else {
			debug_print("[CAPI] Pull-down verification FAILED (expected LOW)\n\r");
		}
	}

	/* Test 4: Output mode - set value and read back */
	debug_print("[CAPI] Test 4: Output mode readback...\n\r");

	ret = capi_gpio_pin_set_direction(&test_pin, CAPI_GPIO_OUTPUT);
	if (ret) {
		debug_print("CAPI GPIO pin_set_direction (output) FAILED\n\r");
		capi_gpio_port_deinit(&gpio_port);
		return ret;
	}

	ret = capi_gpio_pin_get_direction(&test_pin, &direction);
	if (ret == 0) {
		debug_print_int("[CAPI] Direction (0=output): ", direction);
	}

	/* Set HIGH and verify */
	ret = capi_gpio_pin_set_value(&test_pin, CAPI_GPIO_HIGH);
	if (ret) {
		debug_print("CAPI GPIO pin_set_value FAILED\n\r");
		capi_gpio_port_deinit(&gpio_port);
		return ret;
	}

	ret = capi_gpio_pin_get_value(&test_pin, &value);
	if (ret == 0) {
		debug_print_int("[CAPI] Output HIGH readback: ", value);
		if (value == CAPI_GPIO_HIGH) {
			debug_print("[CAPI] Output HIGH verification PASSED\n\r");
		} else {
			debug_print("[CAPI] Output HIGH verification FAILED\n\r");
			capi_gpio_port_deinit(&gpio_port);
			return -1;
		}
	}

	/* Set LOW and verify */
	ret = capi_gpio_pin_set_value(&test_pin, CAPI_GPIO_LOW);
	if (ret) {
		debug_print("CAPI GPIO pin_set_value FAILED\n\r");
		capi_gpio_port_deinit(&gpio_port);
		return ret;
	}

	ret = capi_gpio_pin_get_value(&test_pin, &value);
	if (ret == 0) {
		debug_print_int("[CAPI] Output LOW readback: ", value);
		if (value == CAPI_GPIO_LOW) {
			debug_print("[CAPI] Output LOW verification PASSED\n\r");
		} else {
			debug_print("[CAPI] Output LOW verification FAILED\n\r");
			capi_gpio_port_deinit(&gpio_port);
			return -1;
		}
	}

	/* Test 5: Port-level operations */
	debug_print("[CAPI] Test 5: Port-level operations...\n\r");

	/* Set all pins as output */
	ret = capi_gpio_port_set_direction(gpio_port, 0x0000); /* 0 = output */
	if (ret) {
		debug_print("CAPI GPIO port_set_direction FAILED\n\r");
	} else {
		debug_print("CAPI GPIO port_set_direction OK\n\r");
	}

	uint64_t dir_mask;
	ret = capi_gpio_port_get_direction(gpio_port, &dir_mask);
	if (ret == 0) {
		debug_print_int("[CAPI] Port direction mask: ", (int)dir_mask);
	}

	/* Set alternating pattern */
	ret = capi_gpio_port_set_value(gpio_port, 0xAAAA);
	if (ret) {
		debug_print("CAPI GPIO port_set_value FAILED\n\r");
	} else {
		debug_print("CAPI GPIO port_set_value (0xAAAA) OK\n\r");
	}

	ret = capi_gpio_port_get_value(gpio_port, &port_value);
	if (ret == 0) {
		debug_print_int("[CAPI] Port value readback: ", (int)(port_value & 0xFFFF));
		if ((port_value & 0xFFFF) == 0xAAAA) {
			debug_print("[CAPI] Port value verification PASSED\n\r");
		} else {
			debug_print("[CAPI] Port value verification FAILED\n\r");
		}
	}

	/* Test 6: ACTIVE_LOW flag */
	debug_print("[CAPI] Test 6: ACTIVE_LOW flag...\n\r");

	test_pin.flags = CAPI_GPIO_ACTIVE_LOW;
	ret = capi_gpio_pin_set_direction(&test_pin, CAPI_GPIO_OUTPUT);

	/* Set logical HIGH (should write physical LOW due to ACTIVE_LOW) */
	ret = capi_gpio_pin_set_value(&test_pin, CAPI_GPIO_HIGH);
	if (ret == 0) {
		uint8_t raw_val;
		ret = capi_gpio_pin_get_raw_value(&test_pin, &raw_val);
		if (ret == 0) {
			debug_print_int("[CAPI] Set logical HIGH, raw value: ", raw_val);
			if (raw_val == CAPI_GPIO_LOW) {
				debug_print("[CAPI] ACTIVE_LOW verification PASSED\n\r");
			} else {
				debug_print("[CAPI] ACTIVE_LOW verification FAILED\n\r");
			}
		}
	}

	/* Clean up */
	ret = capi_gpio_port_deinit(&gpio_port);
	if (ret) {
		debug_print("CAPI GPIO port_deinit FAILED\n\r");
		return ret;
	}
	debug_print("CAPI GPIO example completed\n\r");

	return 0;
}

/******************************************************************************/
/************************* UART Example ***************************************/
/******************************************************************************/

/**
 * @brief Example using the CAPI UART interface
 * @return 0 on success, negative error code on failure
 */
static int capi_uart_example(void)
{
	struct stm32_uart_extra_config stm32_uart_extra_init = {
		.huart = &huart3,
	};
	struct capi_uart_line_config stm32_line_config = {
		.baudrate = 115200,
		.size = CAPI_UART_DATA_BITS_8,
		.parity = CAPI_UART_PARITY_NONE,
		.stop_bits = CAPI_UART_STOP_1_BIT,
		.sticky_parity = false,
		.loopback = false,
	};
	struct capi_uart_config stm32_uart_config = {
		.identifier = 3,
		.dma_handle = NULL,
		.clk_freq_hz = 0,
		.line_config = &stm32_line_config,
		.extra = &stm32_uart_extra_init,
		.ops = &stm32_capi_uart_ops,
	};
	int ret;

	ret = capi_uart_init(&g_uart_handle, &stm32_uart_config);
	if (ret)
		return ret;

	ret = capi_uart_transmit(g_uart_handle, (uint8_t *)"Hello\n\r", 7);
	if (ret)
		return ret;

	ret = stm32_uart_stdio_enable(g_uart_handle);
	if (ret)
		return ret;

	return 0;
}

/******************************************************************************/
/****************** CAPI UART Line Config Example *****************************/
/******************************************************************************/

/**
 * @brief Example exercising capi_uart_set_line_config / capi_uart_get_line_config
 *
 * Because the UART under test is also the debug console, the test must
 * restore the original configuration before returning so that subsequent
 * debug_print() calls continue to work.
 *
 * @return 0 on success, negative error code on failure
 */
static int capi_uart_line_config_example(void)
{
	struct capi_uart_line_config original_cfg;
	struct capi_uart_line_config readback_cfg;
	struct capi_uart_line_config new_cfg;
	int ret;
	int result = 0;

	debug_print("=== CAPI UART Line Config Example ===\n\r");

	if (!g_uart_handle) {
		debug_print("[CAPI] UART not initialized, skipping\n\r");
		return 0;
	}

	/* Test 1: get_line_config – read the current (init-time) settings */
	debug_print("[CAPI] Test 1: get_line_config (initial)...\n\r");

	ret = capi_uart_get_line_config(g_uart_handle, &original_cfg);
	if (ret) {
		debug_print("CAPI UART get_line_config FAILED\n\r");
		debug_print_int("Error code: ", ret);
		return ret;
	}
	debug_print("CAPI UART get_line_config OK\n\r");
	debug_print_int("[CAPI] Baudrate: ", original_cfg.baudrate);
	debug_print_int("[CAPI] Data bits (0=8,1=7): ", original_cfg.size);
	debug_print_int("[CAPI] Parity (0=none): ", original_cfg.parity);
	debug_print_int("[CAPI] Stop bits (0=1,1=2): ", original_cfg.stop_bits);
	debug_print_int("[CAPI] Flow ctrl: ", original_cfg.flow_control);

	/* Verify readback matches what we configured at init */
	if (original_cfg.baudrate != 115200 ||
	    original_cfg.size != CAPI_UART_DATA_BITS_8 ||
	    original_cfg.parity != CAPI_UART_PARITY_NONE ||
	    original_cfg.stop_bits != CAPI_UART_STOP_1_BIT) {
		debug_print("[CAPI] Initial config readback FAILED\n\r");
		return -1;
	}
	debug_print("[CAPI] Initial config readback PASSED\n\r");

	/* Test 2: set_line_config – change baudrate and stop bits */
	debug_print("[CAPI] Test 2: set_line_config (9600, 2 stop bits)...\n\r");

	new_cfg = original_cfg;
	new_cfg.baudrate = 9600;
	new_cfg.stop_bits = CAPI_UART_STOP_2_BIT;

	ret = capi_uart_set_line_config(g_uart_handle, &new_cfg);
	if (ret) {
		debug_print("CAPI UART set_line_config FAILED\n\r");
		debug_print_int("Error code: ", ret);
		return ret;
	}

	/* Test 3: get_line_config – verify the new settings took effect */
	ret = capi_uart_get_line_config(g_uart_handle, &readback_cfg);
	if (ret) {
		result = ret;
		goto restore;
	}

	if (readback_cfg.baudrate != 9600 ||
	    readback_cfg.stop_bits != CAPI_UART_STOP_2_BIT) {
		result = -1;
		goto restore;
	}

restore:
	/* Restore original config so the debug console keeps working */
	ret = capi_uart_set_line_config(g_uart_handle, &original_cfg);
	if (ret) {
		/* Cannot even report this via UART – just return */
		return ret;
	}

	if (result) {
		debug_print("[CAPI] Set/Get roundtrip FAILED\n\r");
		return result;
	}
	debug_print("[CAPI] Set/Get roundtrip PASSED\n\r");

	/* Test 4: set_line_config with even parity */
	debug_print("[CAPI] Test 3: set_line_config (even parity)...\n\r");

	new_cfg = original_cfg;
	new_cfg.parity = CAPI_UART_PARITY_EVEN;

	ret = capi_uart_set_line_config(g_uart_handle, &new_cfg);
	if (ret) {
		debug_print("CAPI UART set_line_config (parity) FAILED\n\r");
		debug_print_int("Error code: ", ret);
		return ret;
	}

	ret = capi_uart_get_line_config(g_uart_handle, &readback_cfg);
	if (ret) {
		result = ret;
		goto restore_parity;
	}

	if (readback_cfg.parity != CAPI_UART_PARITY_EVEN)
		result = -1;

restore_parity:
	ret = capi_uart_set_line_config(g_uart_handle, &original_cfg);
	if (ret)
		return ret;

	if (result) {
		debug_print("[CAPI] Parity roundtrip FAILED\n\r");
		return result;
	}
	debug_print("[CAPI] Parity roundtrip PASSED\n\r");

	/* Test 5: NULL-pointer rejection */
	debug_print("[CAPI] Test 4: NULL pointer rejection...\n\r");

	ret = capi_uart_set_line_config(g_uart_handle, NULL);
	if (ret != -EINVAL) {
		debug_print("[CAPI] NULL rejection FAILED (set)\n\r");
		return -1;
	}

	ret = capi_uart_get_line_config(g_uart_handle, NULL);
	if (ret != -EINVAL) {
		debug_print("[CAPI] NULL rejection FAILED (get)\n\r");
		return -1;
	}
	debug_print("[CAPI] NULL pointer rejection PASSED\n\r");

	debug_print("CAPI UART Line Config example completed\n\r");

	return 0;
}

/******************************************************************************/
/***************** CAPI UART Async / Callback / ISR Example *******************/
/******************************************************************************/

static volatile bool uart_tx_done;
static volatile bool uart_rx_done;
static volatile bool uart_error_occurred;
static volatile int  uart_last_event_extra;

static void uart_async_callback(enum capi_uart_async_event event, void *arg,
				int event_extra)
{
	(void)arg;

	switch (event) {
	case CAPI_UART_EVENT_TX_DONE:
		uart_tx_done = true;
		break;
	case CAPI_UART_EVENT_RX_DONE:
		uart_rx_done = true;
		break;
	case CAPI_UART_EVENT_INTERRUPT:
		uart_error_occurred = true;
		uart_last_event_extra = event_extra;
		break;
	default:
		break;
	}
}

void USART3_IRQHandler(void)
{
	if (g_uart_handle)
		capi_uart_isr(g_uart_handle);
}

/**
 * @brief Example exercising the async transmit/receive, callback registration,
 *        ISR dispatch, and line-status reporting paths.
 *
 * The test performs a loopback-style async TX (the bytes go out on the wire;
 * we just verify the completion callback fires).  Full loopback verification
 * would require either hardware loopback wiring or a second UART.
 *
 * @return 0 on success, negative error code on failure
 */
static int capi_uart_async_example(void)
{
	int ret;
	uint8_t tx_buf[] = "CAPI_ASYNC";
	uint32_t timeout;
	uint32_t status_flags;
	enum capi_uart_interrupt_reason reason;

	debug_print("=== CAPI UART Async Example ===\n\r");

	if (!g_uart_handle) {
		debug_print("[CAPI] UART not initialized, skipping\n\r");
		return 0;
	}

	/* Test 1: Register callback */
	debug_print("[CAPI] Test 1: Register callback...\n\r");

	ret = capi_uart_register_callback(g_uart_handle,
					  uart_async_callback, NULL);
	if (ret) {
		debug_print("CAPI UART register_callback FAILED\n\r");
		debug_print_int("Error code: ", ret);
		return ret;
	}
	debug_print("CAPI UART register_callback OK\n\r");

	/* Test 2: Async transmit — verify TX_DONE callback fires */
	debug_print("[CAPI] Test 2: Async transmit...\n\r");

	/* Enable USART3 interrupt in NVIC so the HAL IT path can fire */
	HAL_NVIC_SetPriority(USART3_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(USART3_IRQn);

	uart_tx_done = false;

	ret = capi_uart_transmit_async(g_uart_handle, tx_buf, sizeof(tx_buf) - 1);
	if (ret) {
		HAL_NVIC_DisableIRQ(USART3_IRQn);
		debug_print("CAPI UART transmit_async FAILED\n\r");
		debug_print_int("Error code: ", ret);
		return ret;
	}

	/* Poll for completion */
	timeout = 1000000;
	while (!uart_tx_done && --timeout)
		;

	if (!uart_tx_done) {
		/* Abort the in-flight IT transfer so the HAL state returns to READY
		 * and subsequent blocking HAL_UART_Transmit (debug_print) works. */
		HAL_UART_Abort(&huart3);
		HAL_NVIC_DisableIRQ(USART3_IRQn);
		debug_print("[CAPI] Async TX timeout\n\r");
		return -1;
	}

	debug_print("[CAPI] Async TX completed via callback PASSED\n\r");

	/* Small delay to let any in-flight TX finish before next test */
	HAL_Delay(10);

	/* Test 3: get_line_status — should return 0 (no errors) after clean TX */
	debug_print("[CAPI] Test 3: get_line_status (clean)...\n\r");

	ret = capi_uart_get_line_status(g_uart_handle, &status_flags);
	if (ret) {
		debug_print("CAPI UART get_line_status FAILED\n\r");
		debug_print_int("Error code: ", ret);
		return ret;
	}
	debug_print_int("[CAPI] Line status flags: ", (int)status_flags);

	if (status_flags == 0) {
		debug_print("[CAPI] Clean line status PASSED\n\r");
	} else {
		debug_print("[CAPI] Unexpected line status flags\n\r");
	}

	/* Test 4: get_interrupt_reason — verify API returns without error */
	debug_print("[CAPI] Test 4: get_interrupt_reason...\n\r");

	ret = capi_uart_get_interrupt_reason(g_uart_handle, &reason);
	if (ret) {
		debug_print("CAPI UART get_interrupt_reason FAILED\n\r");
		debug_print_int("Error code: ", ret);
		return ret;
	}
	debug_print_int("[CAPI] Interrupt reason: ", (int)reason);
	debug_print("[CAPI] get_interrupt_reason OK\n\r");

	/* Test 5: NULL-pointer rejection on new APIs */
	debug_print("[CAPI] Test 5: NULL pointer rejection (async APIs)...\n\r");

	ret = capi_uart_register_callback(g_uart_handle, NULL, NULL);
	if (ret) {
		debug_print("[CAPI] NULL callback rejected unexpectedly\n\r");
		return -1;
	}

	ret = capi_uart_transmit_async(g_uart_handle, NULL, 5);
	if (ret != -EINVAL) {
		debug_print("[CAPI] NULL buf rejection FAILED (transmit_async)\n\r");
		return -1;
	}

	ret = capi_uart_receive_async(g_uart_handle, NULL, 5);
	if (ret != -EINVAL) {
		debug_print("[CAPI] NULL buf rejection FAILED (receive_async)\n\r");
		return -1;
	}

	ret = capi_uart_get_line_status(g_uart_handle, NULL);
	if (ret != -EINVAL) {
		debug_print("[CAPI] NULL rejection FAILED (get_line_status)\n\r");
		return -1;
	}

	ret = capi_uart_get_interrupt_reason(g_uart_handle, NULL);
	if (ret != -EINVAL) {
		debug_print("[CAPI] NULL rejection FAILED (get_interrupt_reason)\n\r");
		return -1;
	}
	debug_print("[CAPI] NULL pointer rejection PASSED\n\r");

	/* Disable USART3 NVIC and unregister callback — return to polling mode
	 * so that subsequent blocking debug_print calls work normally. */
	HAL_NVIC_DisableIRQ(USART3_IRQn);
	capi_uart_register_callback(g_uart_handle, NULL, NULL);

	debug_print("CAPI UART Async example completed\n\r");

	return 0;
}

/******************************************************************************/
/************************* CAPI Time Example **********************************/
/******************************************************************************/

/**
 * @brief Example exercising capi_wait_us, capi_wait_ms and capi_uptime.
 *
 * Test 1: capi_uptime returns success and a non-zero value after boot.
 * Test 2: capi_wait_ms — verify that a 50 ms delay produces an uptime
 *         delta within a reasonable tolerance window.
 * Test 3: capi_wait_us — verify that a 500 us delay is measurably
 *         non-zero (at least 100 us of elapsed uptime).
 * Test 4: Monotonicity — two consecutive capi_uptime calls never go
 *         backwards.
 *
 * @return 0 on success, negative error code on failure
 */
static int capi_time_example(void)
{
	int ret;
	uint64_t t0, t1, elapsed;

	debug_print("=== CAPI Time Example ===\n\r");

	/* Test 1: capi_uptime basic functionality */
	debug_print("[CAPI] Test 1: capi_uptime...\n\r");

	ret = capi_uptime(&t0);
	if (ret) {
		debug_print("capi_uptime FAILED\n\r");
		debug_print_int("Error code: ", ret);
		return ret;
	}
	debug_print_int("[CAPI] Uptime (us, low 32): ", (int)(t0 & 0xFFFFFFFF));
	debug_print("[CAPI] capi_uptime OK\n\r");

	/* Test 2: capi_wait_ms — 50 ms delay, then measure elapsed */
	debug_print("[CAPI] Test 2: capi_wait_ms (50 ms)...\n\r");

	ret = capi_uptime(&t0);
	if (ret)
		return ret;

	capi_wait_ms(50);

	ret = capi_uptime(&t1);
	if (ret)
		return ret;

	elapsed = t1 - t0;
	debug_print_int("[CAPI] Elapsed (us): ", (int)elapsed);

	/* Accept 30–80 ms: generous tolerance for SysTick granularity */
	if (elapsed >= 30000 && elapsed <= 80000) {
		debug_print("[CAPI] capi_wait_ms PASSED\n\r");
	} else {
		debug_print("[CAPI] capi_wait_ms FAILED (out of tolerance)\n\r");
		return -1;
	}

	/* Test 3: capi_wait_us — 500 us delay */
	debug_print("[CAPI] Test 3: capi_wait_us (500 us)...\n\r");

	ret = capi_uptime(&t0);
	if (ret)
		return ret;

	capi_wait_us(500);

	ret = capi_uptime(&t1);
	if (ret)
		return ret;

	elapsed = t1 - t0;
	debug_print_int("[CAPI] Elapsed (us): ", (int)elapsed);

	if (elapsed >= 100) {
		debug_print("[CAPI] capi_wait_us PASSED\n\r");
	} else {
		debug_print("[CAPI] capi_wait_us FAILED (too short)\n\r");
		return -1;
	}

	/* Test 4: Monotonicity — t1 must be >= t0 */
	debug_print("[CAPI] Test 4: Monotonicity...\n\r");

	ret = capi_uptime(&t0);
	if (ret)
		return ret;
	ret = capi_uptime(&t1);
	if (ret)
		return ret;

	if (t1 >= t0) {
		debug_print("[CAPI] Monotonicity PASSED\n\r");
	} else {
		debug_print("[CAPI] Monotonicity FAILED\n\r");
		return -1;
	}

	/* Test 5: NULL-pointer rejection */
	debug_print("[CAPI] Test 5: NULL pointer rejection...\n\r");

	ret = capi_uptime(NULL);
	if (ret != -EINVAL) {
		debug_print("[CAPI] NULL rejection FAILED\n\r");
		return -1;
	}
	debug_print("[CAPI] NULL pointer rejection PASSED\n\r");

	debug_print("CAPI Time example completed\n\r");

	return 0;
}

/******************************************************************************/
/************************* CAPI PWM Example ***********************************/
/******************************************************************************/

#ifdef HAL_TIM_MODULE_ENABLED

/**
 * @brief Example using the CAPI PWM convenience wrapper over capi_timer
 *
 * Tests init, enable, disable, set/get period, set/get duty cycle, set/get
 * polarity, and remove.  Uses TIM2 channel 0 (TIM_CHANNEL_1) which is
 * available on most STM32 boards.
 *
 * @return 0 on success, negative error code on failure
 */
static int capi_pwm_example(void)
{
	struct stm32_capi_pwm_desc *pwm = NULL;
	int ret;
	uint64_t val;
	bool pol;

	debug_print("=== CAPI PWM Example ===\n\r");

	struct stm32_capi_pwm_config pwm_cfg = {
		.timer_id = 2,
		.channel = 0,
		.period_ns = 1000000,
		.duty_cycle_ns = 500000,
		.inverted_polarity = false,
		.input_clock_hz = 0,
		.get_input_clock = NULL,
	};

	/* Test 1: Init */
	debug_print("[CAPI] Test 1: PWM init...\n\r");

	ret = stm32_capi_pwm_init(&pwm, &pwm_cfg);
	if (ret) {
		debug_print("CAPI PWM init FAILED\n\r");
		debug_print_int("Error code: ", ret);
		return ret;
	}
	debug_print("CAPI PWM init OK\n\r");

	/* Test 2: Enable */
	debug_print("[CAPI] Test 2: PWM enable...\n\r");

	ret = stm32_capi_pwm_enable(pwm);
	if (ret) {
		debug_print("CAPI PWM enable FAILED\n\r");
		debug_print_int("Error code: ", ret);
		stm32_capi_pwm_remove(pwm);
		return ret;
	}
	debug_print("CAPI PWM enable OK\n\r");

	/* Test 3: Get period — should match init value */
	debug_print("[CAPI] Test 3: get_period...\n\r");

	ret = stm32_capi_pwm_get_period(pwm, &val);
	if (ret) {
		debug_print("CAPI PWM get_period FAILED\n\r");
		stm32_capi_pwm_remove(pwm);
		return ret;
	}
	debug_print_int("[CAPI] Period (ns): ", (int)val);

	if (val != 1000000) {
		debug_print("[CAPI] Period readback FAILED\n\r");
		stm32_capi_pwm_remove(pwm);
		return -1;
	}
	debug_print("[CAPI] Period readback PASSED\n\r");

	/* Test 4: Get duty cycle — should match init value */
	debug_print("[CAPI] Test 4: get_duty_cycle...\n\r");

	ret = stm32_capi_pwm_get_duty_cycle(pwm, &val);
	if (ret) {
		debug_print("CAPI PWM get_duty_cycle FAILED\n\r");
		stm32_capi_pwm_remove(pwm);
		return ret;
	}
	debug_print_int("[CAPI] Duty cycle (ns): ", (int)val);

	if (val != 500000) {
		debug_print("[CAPI] Duty cycle readback FAILED\n\r");
		stm32_capi_pwm_remove(pwm);
		return -1;
	}
	debug_print("[CAPI] Duty cycle readback PASSED\n\r");

	/* Test 5: Set new period and verify roundtrip */
	debug_print("[CAPI] Test 5: set_period (2000000 ns)...\n\r");

	ret = stm32_capi_pwm_set_period(pwm, 2000000);
	if (ret) {
		debug_print("CAPI PWM set_period FAILED\n\r");
		stm32_capi_pwm_remove(pwm);
		return ret;
	}

	ret = stm32_capi_pwm_get_period(pwm, &val);
	if (ret || val != 2000000) {
		debug_print("[CAPI] Period set/get roundtrip FAILED\n\r");
		stm32_capi_pwm_remove(pwm);
		return -1;
	}
	debug_print("[CAPI] Period set/get roundtrip PASSED\n\r");

	/* Test 6: Set new duty cycle and verify roundtrip */
	debug_print("[CAPI] Test 6: set_duty_cycle (750000 ns)...\n\r");

	ret = stm32_capi_pwm_set_duty_cycle(pwm, 750000);
	if (ret) {
		debug_print("CAPI PWM set_duty_cycle FAILED\n\r");
		stm32_capi_pwm_remove(pwm);
		return ret;
	}

	ret = stm32_capi_pwm_get_duty_cycle(pwm, &val);
	if (ret || val != 750000) {
		debug_print("[CAPI] Duty cycle set/get roundtrip FAILED\n\r");
		stm32_capi_pwm_remove(pwm);
		return -1;
	}
	debug_print("[CAPI] Duty cycle set/get roundtrip PASSED\n\r");

	/* Test 7: Set polarity and verify roundtrip */
	debug_print("[CAPI] Test 7: set_polarity (inverted)...\n\r");

	ret = stm32_capi_pwm_set_polarity(pwm, true);
	if (ret) {
		debug_print("CAPI PWM set_polarity FAILED\n\r");
		stm32_capi_pwm_remove(pwm);
		return ret;
	}

	ret = stm32_capi_pwm_get_polarity(pwm, &pol);
	if (ret || !pol) {
		debug_print("[CAPI] Polarity set/get roundtrip FAILED\n\r");
		stm32_capi_pwm_remove(pwm);
		return -1;
	}
	debug_print("[CAPI] Polarity set/get roundtrip PASSED\n\r");

	/* Test 8: Disable */
	debug_print("[CAPI] Test 8: PWM disable...\n\r");

	ret = stm32_capi_pwm_disable(pwm);
	if (ret) {
		debug_print("CAPI PWM disable FAILED\n\r");
		stm32_capi_pwm_remove(pwm);
		return ret;
	}
	debug_print("CAPI PWM disable OK\n\r");

	/* Test 9: NULL pointer rejection */
	debug_print("[CAPI] Test 9: NULL pointer rejection...\n\r");

	ret = stm32_capi_pwm_init(NULL, &pwm_cfg);
	if (ret != -EINVAL) {
		debug_print("[CAPI] NULL rejection FAILED (init desc)\n\r");
		stm32_capi_pwm_remove(pwm);
		return -1;
	}

	ret = stm32_capi_pwm_get_period(NULL, &val);
	if (ret != -EINVAL) {
		debug_print("[CAPI] NULL rejection FAILED (get_period)\n\r");
		stm32_capi_pwm_remove(pwm);
		return -1;
	}

	ret = stm32_capi_pwm_get_duty_cycle(pwm, NULL);
	if (ret != -EINVAL) {
		debug_print("[CAPI] NULL rejection FAILED (get_duty_cycle)\n\r");
		stm32_capi_pwm_remove(pwm);
		return -1;
	}

	ret = stm32_capi_pwm_get_polarity(pwm, NULL);
	if (ret != -EINVAL) {
		debug_print("[CAPI] NULL rejection FAILED (get_polarity)\n\r");
		stm32_capi_pwm_remove(pwm);
		return -1;
	}
	debug_print("[CAPI] NULL pointer rejection PASSED\n\r");

	/* Clean up */
	ret = stm32_capi_pwm_remove(pwm);
	if (ret) {
		debug_print("CAPI PWM remove FAILED\n\r");
		return ret;
	}
	debug_print("CAPI PWM example completed\n\r");

	return 0;
}

#endif /* HAL_TIM_MODULE_ENABLED */

/******************************************************************************/
/************************* Main Function **************************************/
/******************************************************************************/

int main(void)
{
	int ret;

	/* Initialize STM32 HAL */
	stm32_init();

	/* Initialize UART for printf output */
	ret = capi_uart_example();
	if (ret) {
		return ret;
	}

	/* Use direct UART output for critical debug messages */
	debug_print("\n\r========================================\n\r");
	debug_print("STM32 UART Example\n\r");
	debug_print("========================================\n\r\n\r");

	debug_print("About to run CAPI UART Line Config example...\n\r");

	/* Run CAPI UART Line Config example */
	ret = capi_uart_line_config_example();
	if (ret) {
		debug_print("CAPI UART Line Config example FAILED\n\r");
	} else {
		debug_print("CAPI UART Line Config example completed OK\n\r");
	}

	debug_print("\n\r");
	debug_print("About to run CAPI UART Async example...\n\r");

	/* Run CAPI UART Async example */
	ret = capi_uart_async_example();
	if (ret) {
		debug_print("CAPI UART Async example FAILED\n\r");
	} else {
		debug_print("CAPI UART Async example completed OK\n\r");
	}

	debug_print("\n\r");
	debug_print("About to run CAPI Time example...\n\r");

	/* Run CAPI Time example */
	ret = capi_time_example();
	if (ret) {
		debug_print("CAPI Time example FAILED\n\r");
	} else {
		debug_print("CAPI Time example completed OK\n\r");
	}

	debug_print("\n\r");
	debug_print("About to run CAPI SPI example...\n\r");

	/* Run CAPI SPI example */
	ret = capi_spi_example();
	if (ret) {
		debug_print("CAPI SPI example FAILED\n\r");
	} else {
		debug_print("CAPI SPI example completed OK\n\r");
	}

	debug_print("\n\r");
	debug_print("About to run CAPI I2C example...\n\r");

// 	/* Run CAPI I2C example */
// 	ret = capi_i2c_example();
// 	if (ret) {
// 		debug_print("CAPI I2C example FAILED\n\r");
// 	} else {
// 		debug_print("CAPI I2C example completed OK\n\r");
// 	}
//
	debug_print("\n\r");
	debug_print("About to run CAPI DMA example...\n\r");

	/* Run CAPI DMA example */
	ret = capi_dma_example();
	if (ret) {
		debug_print("CAPI DMA example FAILED\n\r");
	} else {
		debug_print("CAPI DMA example completed OK\n\r");
	}

	debug_print("\n\r");
	debug_print("About to run CAPI IRQ example...\n\r");

	/* Run CAPI IRQ example */
	ret = capi_irq_example();
	if (ret) {
		debug_print("CAPI IRQ example FAILED\n\r");
	} else {
		debug_print("CAPI IRQ example completed OK\n\r");
	}

	debug_print("\n\r");
	debug_print("About to run CAPI GPIO example...\n\r");
//
// 	/* Run CAPI GPIO example */
	ret = capi_gpio_example();
	if (ret) {
		debug_print("CAPI GPIO example FAILED\n\r");
	} else {
		debug_print("CAPI GPIO example completed OK\n\r");
	}
//
#ifdef HAL_TIM_MODULE_ENABLED
	debug_print("\n\r");
	debug_print("About to run CAPI PWM example...\n\r");

	/* Run CAPI PWM example */
	ret = capi_pwm_example();
	if (ret) {
		debug_print("CAPI PWM example FAILED\n\r");
	} else {
		debug_print("CAPI PWM example completed OK\n\r");
	}
#else
	debug_print("\n\r");
	debug_print("[CAPI] Timer module not enabled, skipping PWM\n\r");
#endif /* HAL_TIM_MODULE_ENABLED */

	(void)ret; /* Suppress unused variable warning */

	debug_print("========================================\n\r");
	debug_print("All examples completed\n\r");
	debug_print("========================================\n\r");

	/* Infinite loop to keep running */
	while (1) {
		/* Do nothing - keep processor running */
	}

	return 0;
}
