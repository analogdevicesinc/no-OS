/***************************************************************************//**
 * @file main.c
 * @brief CAPI-managed QVGA camera coprocessor demo for MAX78000EVKIT.
 ******************************************************************************
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "mxc_device.h"
#include "mxc_sys.h"
#include "gcfr_regs.h"
#include "gpio.h"
#include "icc.h"
#include "tmr.h"
#include "tft_ssd2119.h"

#include "no_os_uart.h"
#include "no_os_delay.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"

#include "capi_coprocessor.h"
#include "capi_mailbox.h"
#include "maxim_capi_coprocessor.h"
#include "maxim_ipc.h"

#include "camera_ipc.h"
#include "camera_overlay.h"
#include "camera_tft.h"

#define UART_DEVICE_ID          0
#define UART_IRQ_ID             UART0_IRQn
#define UART_BAUDRATE           115200
#define BUTTON_HOLD_US          1000000U

#define DEBUG_BUTTON_PIN        MXC_GPIO_PIN_6
#define PARK_BUTTON_PIN         MXC_GPIO_PIN_7

extern uint8_t _riscv_boot;

static volatile struct camera_ipc *const ipc =
	(volatile struct camera_ipc *)CAMERA_IPC_ADDR;
static struct capi_coprocessor_handle *coprocessor;
static struct capi_mailbox_handle *mailbox;
static enum capi_coprocessor_mode displayed_mode = CAPI_COPROCESSOR_MODE_RUN;
static uint32_t ui_fps_x10;
static uint32_t last_ui_us;
static uint32_t last_tone_report_us;
static bool park_pending;
static bool parked;
static uint8_t tft_boot_tile[CAMERA_TILE_BYTES];

static int init_host_timer(void)
{
	mxc_tmr_cfg_t config = {
		.pres = MXC_TMR_PRES_128,
		.mode = MXC_TMR_MODE_CONTINUOUS,
		.bitMode = MXC_TMR_BIT_MODE_32,
		.clock = MXC_TMR_IBRO_CLK,
		.cmp_cnt = UINT32_MAX,
		.pol = 0,
	};
	int ret;

	/* IBRO/128 is a clock-independent 57,600 Hz timebase. */
	ret = MXC_TMR_Init(MXC_TMR2, &config, false);
	if (ret)
		return ret;
	MXC_TMR_Start(MXC_TMR2);
	return 0;
}

static uint32_t host_now_us(void)
{
	/* 1,000,000 / 57,600 reduces exactly to 625 / 36. */
	return (uint32_t)(((uint64_t)MXC_TMR_GetCount(MXC_TMR2) * 625U) /
			  36U);
}

static struct max_uart_init_param uart_extra_ip = {
	.flow = MAX_UART_FLOW_DIS,
};

static struct no_os_uart_init_param uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = &uart_extra_ip,
	.platform_ops = &max_uart_ops,
};

static const mxc_gpio_cfg_t buttons = {
	.port = MXC_GPIO2,
	.mask = DEBUG_BUTTON_PIN | PARK_BUTTON_PIN,
	.func = MXC_GPIO_FUNC_IN,
	.pad = MXC_GPIO_PAD_PULL_UP,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
	.drvstr = MXC_GPIO_DRVSTR_0,
};

static const char *mode_name(enum capi_coprocessor_mode mode)
{
	switch (mode) {
	case CAPI_COPROCESSOR_MODE_RUN:
		return "RUN";
	case CAPI_COPROCESSOR_MODE_PARKED:
		return "PARK";
	case CAPI_COPROCESSOR_MODE_STANDBY:
		return "STBY";
	case CAPI_COPROCESSOR_MODE_LOW_POWER:
		return "LOW";
	case CAPI_COPROCESSOR_MODE_OFF:
		return "OFF";
	default:
		return "?";
	}
}

static const char *white_balance_name(uint32_t mode)
{
	switch (mode) {
	case CAMERA_WB_AUTO:
		return "AUTO";
	case CAMERA_WB_INCANDESCENT:
		return "INC";
	case CAMERA_WB_DAYLIGHT:
		return "DAY";
	case CAMERA_WB_CLOUDY:
		return "CLD";
	default:
		return "?";
	}
}

static const char *exposure_name(uint32_t exposure)
{
	static const char *const names[CAMERA_EXPOSURE_COUNT] = {
		"-2", "-1", "0", "+1", "+2",
	};

	return exposure < CAMERA_EXPOSURE_COUNT ? names[exposure] : "?";
}

static const char *tone_curve_name(uint32_t tone_curve)
{
	static const char *const names[CAMERA_TONE_COUNT] = {
		"F", "B", "P",
	};

	return tone_curve < CAMERA_TONE_COUNT ? names[tone_curve] : "?";
}

static const char *ae_state_name(uint32_t state)
{
	switch (state) {
	case CAMERA_AE_RESET:
		return "RST";
	case CAMERA_AE_SETTLING:
		return "SET";
	case CAMERA_AE_LOCKED:
		return "LOCK";
	default:
		return "?";
	}
}

static const char *ae_state_short_name(uint32_t state)
{
	switch (state) {
	case CAMERA_AE_RESET:
		return "R";
	case CAMERA_AE_SETTLING:
		return "S";
	case CAMERA_AE_LOCKED:
		return "L";
	default:
		return "?";
	}
}

static int init_tft(void)
{
	mxc_tft_spi_config spi_config = {
		.regs = MXC_SPI0,
		.gpio = {
			MXC_GPIO0,
			MXC_GPIO_PIN_5 | MXC_GPIO_PIN_6 | MXC_GPIO_PIN_7 |
			MXC_GPIO_PIN_11,
			MXC_GPIO_FUNC_ALT1,
			MXC_GPIO_PAD_NONE,
			MXC_GPIO_VSSEL_VDDIOH,
			MXC_GPIO_DRVSTR_0,
		},
		.freq = 25000000,
		.ss_idx = 1,
	};
	mxc_gpio_cfg_t reset = {
		MXC_GPIO0,
		MXC_GPIO_PIN_19,
		MXC_GPIO_FUNC_OUT,
		MXC_GPIO_PAD_NONE,
		MXC_GPIO_VSSEL_VDDIOH,
		MXC_GPIO_DRVSTR_0,
	};
	int ret;

	ret = MXC_TFT_PreInit(&spi_config, &reset, NULL);
	if (ret)
		return ret;
	ret = MXC_TFT_Init();
	if (ret)
		return ret;

	MXC_TFT_SetRotation(SCREEN_NORMAL);
	MXC_TFT_SetBackGroundColor(0);
	MXC_TFT_ClearScreen();
	return 0;
}

static void show_tft_boot_diagnostic(void)
{
	camera_overlay_line(tft_boot_tile, "TFT BOOT STOCK");
	camera_overlay_color_key(tft_boot_tile);
	MXC_TFT_ShowImageCameraRGB565(0, 0, tft_boot_tile,
				      CAMERA_FRAME_WIDTH, CAMERA_TILE_ROWS);
	printf("TFT boot diagnostic drawn through stock SSD2119 writer\n");
	no_os_mdelay(500);
}

static int init_mailbox(void)
{
	const struct capi_mailbox_config config = {
		.identifier = MAX_IPC_HOST_ID,
		.ops = &max_mailbox_ops,
	};

	return capi_mailbox_init(&mailbox, &config);
}

static int init_cnn_frame_store(void)
{
	int ret;

	/* Power/reset sequence from the MSDK MAX78000 ImgCapture example. */
	MXC_GCFR->reg3 = 0x0fU;
	MXC_GCFR->reg1 = 0x0fU;
	MXC_GCFR->reg0 = 0x0fU;
	MXC_GCFR->reg2 = 0x00U;
	MXC_GCFR->reg3 = 0x00U;
	MXC_GCR->pclkdiv =
		(MXC_GCR->pclkdiv &
		 ~(MXC_F_GCR_PCLKDIV_CNNCLKDIV | MXC_F_GCR_PCLKDIV_CNNCLKSEL)) |
		MXC_S_GCR_PCLKDIV_CNNCLKDIV_DIV1;

	/* Exercise the accelerator resource through CAPI, not a private clock call. */
	ret = capi_coprocessor_resource_set_enabled(coprocessor,
			CAPI_COPROCESSOR_RESOURCE_ACCELERATOR, 0, true);
	if (ret)
		return ret;

	*((volatile uint32_t *)0x50001000UL) = 0x00000000U;
	*((volatile uint32_t *)0x50100000UL) = 0x00100008U;
	*((volatile uint32_t *)0x50100004UL) = 0x0000040eU;
	*((volatile uint32_t *)0x50500000UL) = 0x00100008U;
	*((volatile uint32_t *)0x50500004UL) = 0x0000040eU;
	return 0;
}

static int init_coprocessor(void)
{
	struct maxim_capi_coprocessor_config extra = {
		.keep_accelerator_on = true,
		.enable_debug = true,
	};
	const struct capi_coprocessor_config config = {
		.identifier = MAXIM_CAPI_COPROCESSOR_RV32,
		.extra = &extra,
		.ops = &maxim_capi_coprocessor_ops,
	};
	int ret;

	ret = capi_coprocessor_init(&coprocessor, &config);
	if (ret)
		return ret;
	ret = capi_coprocessor_set_vector_base(coprocessor,
					       (uint64_t)(uintptr_t)&_riscv_boot);
	if (ret)
		return ret;
	ret = init_cnn_frame_store();
	if (ret)
		return ret;
	return capi_coprocessor_boot(coprocessor);
}

static int send_command_value(enum camera_ipc_command command, uint32_t value)
{
	uint32_t word = CAMERA_COMMAND_WORD(command, value);
	struct capi_mailbox_transaction transaction = {
		.dest_id = MAX_IPC_COPROC_ID,
		.buf = (uint8_t *)&word,
		.size = sizeof(word),
		.msg_len = sizeof(word),
	};

	ipc->command = word;
	return capi_mailbox_transmit_async(mailbox, &transaction);
}

static int send_command(enum camera_ipc_command command)
{
	return send_command_value(command, 0);
}

static void receive_events(void)
{
	uint32_t word;
	struct capi_mailbox_transaction transaction = {
		.dest_id = MAX_IPC_HOST_ID,
		.buf = (uint8_t *)&word,
		.size = sizeof(word),
	};
	int ret;

	ret = capi_mailbox_receive_async(mailbox, &transaction);
	if (ret)
		return;
	capi_mailbox_acknowledge(mailbox, MAX_IPC_HOST_ID);

	if (CAMERA_EVENT_TYPE(word) == CAMERA_EVENT_FAULT)
		printf("CPU1 fault: mcause=0x%08lx mepc=0x%08lx mtval=0x%08lx\n",
		       (unsigned long)ipc->fault_cause,
		       (unsigned long)ipc->fault_epc,
		       (unsigned long)ipc->fault_tval);
}

static bool snapshot_stats(struct camera_frame_stats *snapshot)
{
	struct camera_frame_stats candidate;
	uint32_t before;
	uint32_t after;
	uint32_t attempt;

	for (attempt = 0; attempt < 16U; attempt++) {
		before = ipc->stats.sequence;
		if (before & 1U)
			continue;
		camera_ipc_compiler_barrier();
		candidate.frame_number = ipc->stats.frame_number;
		candidate.capture_fps_x10 = ipc->stats.capture_fps_x10;
		candidate.processing_us = ipc->stats.processing_us;
		candidate.dma_overflows = ipc->stats.dma_overflows;
		candidate.dropped_frames = ipc->stats.dropped_frames;
		candidate.mean_luma = ipc->stats.mean_luma;
		candidate.mean_red = ipc->stats.mean_red;
		candidate.mean_green = ipc->stats.mean_green;
		candidate.mean_blue = ipc->stats.mean_blue;
		candidate.shadow_percent_x10 = ipc->stats.shadow_percent_x10;
		candidate.highlight_percent_x10 = ipc->stats.highlight_percent_x10;
		candidate.luma_p10 = ipc->stats.luma_p10;
		candidate.luma_p50 = ipc->stats.luma_p50;
		candidate.luma_p90 = ipc->stats.luma_p90;
		candidate.edge_pixels = ipc->stats.edge_pixels;
		candidate.motion_percent_x10 = ipc->stats.motion_percent_x10;
		candidate.roi_x = ipc->stats.roi_x;
		candidate.roi_y = ipc->stats.roi_y;
		candidate.roi_w = ipc->stats.roi_w;
		candidate.roi_h = ipc->stats.roi_h;
		camera_ipc_compiler_barrier();
		after = ipc->stats.sequence;
		if (before != after || (after & 1U))
			continue;

		candidate.sequence = after;
		*snapshot = candidate;
		return true;
	}

	return false;
}

static void make_overlay(char *line, size_t size, uint32_t overlay_row,
			 const struct camera_frame_stats *stats)
{
	switch (overlay_row) {
	case 0:
		snprintf(line, size,
			 "RV:%s WB:%s X:%s S:%lu T:%s A:%s C:%lu.%lu U:%lu.%lu",
			 mode_name(displayed_mode),
			 white_balance_name(ipc->white_balance),
			 exposure_name(ipc->exposure),
			 (unsigned long)ipc->saturation,
			 tone_curve_name(ipc->tone_curve),
			 ae_state_short_name(ipc->ae_state),
			 (unsigned long)(stats->capture_fps_x10 / 10U),
			 (unsigned long)(stats->capture_fps_x10 % 10U),
			 (unsigned long)(ui_fps_x10 / 10U),
			 (unsigned long)(ui_fps_x10 % 10U));
		break;
	case 1:
		snprintf(line, size, "P:%luUS F:%lu O:%lu D:%lu RGB:%03lu,%03lu,%03lu",
			 (unsigned long)stats->processing_us,
			 (unsigned long)stats->frame_number,
			 (unsigned long)stats->dma_overflows,
			 (unsigned long)stats->dropped_frames,
			 (unsigned long)stats->mean_red,
			 (unsigned long)stats->mean_green,
			 (unsigned long)stats->mean_blue);
		break;
	default:
		snprintf(line, size, "M:%lu.%lu%% L:%lu E:%lu ROI:%03lu,%03lu %03luX%03lu",
			 (unsigned long)(stats->motion_percent_x10 / 10U),
			 (unsigned long)(stats->motion_percent_x10 % 10U),
			 (unsigned long)stats->mean_luma,
			 (unsigned long)stats->edge_pixels,
			 (unsigned long)stats->roi_x,
			 (unsigned long)stats->roi_y,
			 (unsigned long)stats->roi_w,
			 (unsigned long)stats->roi_h);
		break;
	}
}

static void display_ready_tiles(void)
{
	static struct camera_frame_stats stats;
	uint32_t slot;

	for (slot = 0; slot < CAMERA_TILE_COUNT; slot++) {
		volatile struct camera_tile_desc *tile = &ipc->tile[slot];
		uint32_t y;
		uint32_t rows;

		if (tile->state != CAMERA_TILE_READY)
			continue;

		tile->state = CAMERA_TILE_DISPLAYING;
		camera_ipc_compiler_barrier();
		y = tile->y;
		rows = tile->rows;

		if (y < 3U * CAMERA_TILE_ROWS) {
			char line[54];

			(void)snapshot_stats(&stats);
			make_overlay(line, sizeof(line), y / CAMERA_TILE_ROWS, &stats);
			camera_overlay_line((uint8_t *)ipc->pixels[slot], line);
			if (!y)
				camera_overlay_color_key((uint8_t *)ipc->pixels[slot]);
		}

		camera_tft_show_rgb565(0, (int)y,
				       (uint8_t *)ipc->pixels[slot], CAMERA_FRAME_WIDTH, (int)rows);
		camera_ipc_compiler_barrier();
		tile->state = CAMERA_TILE_FREE;

		if (y + rows == CAMERA_FRAME_HEIGHT) {
			uint32_t now = host_now_us();
			uint32_t elapsed = now - last_ui_us;

			if (last_ui_us && elapsed)
				ui_fps_x10 = 10000000U / elapsed;
			last_ui_us = now;
			if (!last_tone_report_us ||
			    now - last_tone_report_us >= 1000000U) {
				printf("tone=%s exposure=%s sat=%lu ae=%s(%lu/%u) "
				       "capture=%lu.%lu ui=%lu.%lu processing=%luus "
				       "lines=%lu gain=0x%03lx com8=0x%02lx luma=%lu "
				       "p10=%lu p50=%lu p90=%lu "
				       "shadow=%lu.%lu%% highlight=%lu.%lu%%\n",
				       tone_curve_name(ipc->tone_curve),
				       exposure_name(ipc->exposure),
				       (unsigned long)ipc->saturation,
				       ae_state_name(ipc->ae_state),
				       (unsigned long)ipc->ae_settle_frames,
				       CAMERA_AE_SETTLE_FRAMES,
				       (unsigned long)(stats.capture_fps_x10 / 10U),
				       (unsigned long)(stats.capture_fps_x10 % 10U),
				       (unsigned long)(ui_fps_x10 / 10U),
				       (unsigned long)(ui_fps_x10 % 10U),
				       (unsigned long)stats.processing_us,
				       (unsigned long)ipc->ae_exposure_lines,
				       (unsigned long)ipc->ae_gain_raw,
				       (unsigned long)ipc->ae_com8,
				       (unsigned long)stats.mean_luma,
				       (unsigned long)stats.luma_p10,
				       (unsigned long)stats.luma_p50,
				       (unsigned long)stats.luma_p90,
				       (unsigned long)(stats.shadow_percent_x10 / 10U),
				       (unsigned long)(stats.shadow_percent_x10 % 10U),
				       (unsigned long)(stats.highlight_percent_x10 / 10U),
				       (unsigned long)(stats.highlight_percent_x10 % 10U));
				last_tone_report_us = now;
			}
		}
	}
}

static void toggle_park(void)
{
	if (parked) {
		printf("CAPI mode PARKED -> RUN\n");
		capi_coprocessor_set_mode(coprocessor,
					  CAPI_COPROCESSOR_MODE_RUN);
		displayed_mode = CAPI_COPROCESSOR_MODE_RUN;
		parked = false;
		send_command(CAMERA_CMD_RESUME);
	} else {
		printf("requesting frame-safe CAPI PARKED mode\n");
		park_pending = true;
		send_command(CAMERA_CMD_PAUSE);
	}
}

static void handle_buttons(void)
{
	static uint32_t previous = DEBUG_BUTTON_PIN | PARK_BUTTON_PIN;
	static uint32_t debug_press_us;
	static uint32_t park_press_us;
	static uint32_t chord_press_us;
	static bool debug_button_down;
	static bool debug_long_press;
	static bool park_button_down;
	static bool park_long_press;
	static bool chord_down;
	static bool chord_long_press;
	uint32_t current = MXC_GPIO_InGet(MXC_GPIO2,
					  DEBUG_BUTTON_PIN | PARK_BUTTON_PIN);
	uint32_t pressed = previous & ~current;
	uint32_t released = ~previous & current;
	uint32_t now = host_now_us();

	previous = current;
	if (pressed & DEBUG_BUTTON_PIN) {
		debug_button_down = true;
		debug_long_press = false;
		debug_press_us = now;
	}
	if (pressed & PARK_BUTTON_PIN) {
		park_button_down = true;
		park_long_press = false;
		park_press_us = now;
	}

	if (!(current & DEBUG_BUTTON_PIN) && !(current & PARK_BUTTON_PIN)) {
		if (!chord_down) {
			chord_down = true;
			chord_long_press = false;
			chord_press_us = now;
			debug_long_press = true;
			park_long_press = true;
		}
		if (!chord_long_press && now - chord_press_us >= BUTTON_HOLD_US) {
			uint32_t tone_curve = ipc->tone_curve + 1U;

			if (tone_curve >= CAMERA_TONE_COUNT)
				tone_curve = CAMERA_TONE_FLAT;
			if (parked) {
				printf("tone curve ignored while CPU1 is PARKED\n");
			} else {
				printf("tone curve -> %s\n",
				       tone_curve_name(tone_curve));
				send_command_value(CAMERA_CMD_SET_TONE_CURVE,
						   tone_curve);
			}
			chord_long_press = true;
		}
	} else {
		chord_down = false;
	}

	if (debug_button_down && !(current & DEBUG_BUTTON_PIN) &&
	    !debug_long_press &&
	    now - debug_press_us >= BUTTON_HOLD_US) {
		uint32_t mode = ipc->white_balance + 1U;

		if (mode >= CAMERA_WB_COUNT)
			mode = CAMERA_WB_AUTO;
		if (parked) {
			printf("white balance ignored while CPU1 is PARKED\n");
		} else {
			printf("white balance -> %s\n", white_balance_name(mode));
			send_command_value(CAMERA_CMD_SET_WHITE_BALANCE, mode);
		}
		debug_long_press = true;
	}
	if (park_button_down && !(current & PARK_BUTTON_PIN) &&
	    !park_long_press && now - park_press_us >= BUTTON_HOLD_US) {
		uint32_t exposure = ipc->exposure + 1U;

		if (exposure >= CAMERA_EXPOSURE_COUNT)
			exposure = CAMERA_EXPOSURE_NEGATIVE_2;
		if (parked) {
			printf("exposure ignored while CPU1 is PARKED\n");
		} else {
			printf("exposure -> %s\n", exposure_name(exposure));
			send_command_value(CAMERA_CMD_SET_EXPOSURE, exposure);
		}
		park_long_press = true;
	}
	if ((released & DEBUG_BUTTON_PIN) && debug_button_down) {
		debug_button_down = false;
		if (!debug_long_press) {
			if (parked)
				printf("PB1 ignored while CPU1 is PARKED; use PB2 to resume\n");
			else if (ipc->state == CAMERA_STATE_PAUSED) {
				printf("debug-safe capture resume requested\n");
				send_command(CAMERA_CMD_RESUME);
			} else {
				printf("debug-safe pause requested; breakpoint rv_debug_safe_point\n");
				send_command(CAMERA_CMD_PAUSE);
			}
		}
	}

	if ((released & PARK_BUTTON_PIN) && park_button_down) {
		park_button_down = false;
		if (!park_long_press)
			toggle_park();
	}

	if (park_pending && ipc->state == CAMERA_STATE_PAUSED) {
		capi_coprocessor_set_mode(coprocessor,
					  CAPI_COPROCESSOR_MODE_PARKED);
		displayed_mode = CAPI_COPROCESSOR_MODE_PARKED;
		park_pending = false;
		parked = true;
		printf("CPU1 parked safely at a frame boundary\n");
	}
}

int main(void)
{
	struct no_os_uart_desc *uart;
	uint32_t timeout;
	int ret;

	MXC_ICC_Enable(MXC_ICC0);
	MXC_SYS_Clock_Select(MXC_SYS_CLOCK_IPO);
	SystemCoreClockUpdate();

	ret = no_os_uart_init(&uart, &uart_ip);
	if (ret)
		return ret;
	no_os_uart_stdio(uart);

	printf("\nMAX78000 CAPI QVGA camera coprocessor demo\n");
	printf("CPU1 image: 0x%08lx, IPC: 0x%08lx\n",
	       (unsigned long)&_riscv_boot, (unsigned long)CAMERA_IPC_ADDR);

	ret = init_tft();
	if (ret) {
		printf("TFT initialization failed: %d\n", ret);
		return ret;
	}
	show_tft_boot_diagnostic();

	MXC_GPIO_Config(&buttons);
	/* CYCCNT is not implemented on MAX78000; timer 2 drives UI timing. */
	ret = init_host_timer();
	if (ret) {
		printf("host timer initialization failed: %d\n", ret);
		return ret;
	}

	ret = init_mailbox();
	if (ret) {
		printf("mailbox initialization failed: %d\n", ret);
		return ret;
	}
	ret = init_coprocessor();
	if (ret) {
		printf("CPU1 initialization failed: %d\n", ret);
		return ret;
	}

	for (timeout = 0; timeout < 2000U; timeout++) {
		if (ipc->magic == CAMERA_IPC_MAGIC &&
		    (ipc->state == CAMERA_STATE_RUNNING ||
		     ipc->state == CAMERA_STATE_FAULT))
			break;
		no_os_mdelay(1);
	}
	if (ipc->magic != CAMERA_IPC_MAGIC ||
	    ipc->abi_version != CAMERA_IPC_ABI_VERSION ||
	    ipc->state == CAMERA_STATE_BOOTING) {
		printf("timed out waiting for CPU1 camera ABI\n");
		return -ETIMEDOUT;
	}
	if (ipc->state == CAMERA_STATE_FAULT) {
		printf("CPU1 camera initialization fault: mcause=0x%08lx\n",
		       (unsigned long)ipc->fault_cause);
		return -EIO;
	}

	printf("OVM camera detected as 0x%04lx; streaming 320x240 RGB565\n",
	       (unsigned long)ipc->camera_id);
	printf("display transport: shared tiles, stock MSDK SSD2119 writer\n");
	printf("capture buffer: complete RGB565 frame in CNN data SRAM\n");
	printf("PB1 short: debug pause; hold: WB. PB2 short: park; hold: exposure.\n");
	printf("Hold PB1+PB2: tone curve F/B/P. Saturation starts at sensor max 8.\n");
	printf("AEC/AGC: two complete frames, then lock; WB/exposure changes relock.\n");

	for (;;) {
		receive_events();
		display_ready_tiles();
		handle_buttons();
	}
}
