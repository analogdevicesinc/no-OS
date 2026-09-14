/***************************************************************************//**
 * @file camera_riscv.c
 * @brief MAX78000 CPU1 QVGA camera capture and streaming statistics.
 ******************************************************************************
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/

#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "camera.h"
#include "cameraif.h"
#include "dma.h"
#include "dma_regs.h"
#include "mxc_device.h"
#include "mxc_errors.h"
#include "tmr.h"

#include "camera_ipc.h"
#include "maxim_coprocessor_riscv.h"
#include "maxim_ipc.h"

#define CAMERA_CLOCK_HZ       10000000U
#define RISCV_CLOCK_HZ        60000000U
#define OV7692_CLKRC_REG       0x11U
#define OV7692_QVGA_CLKRC       0x00U
#define OV7692_BGAIN_REG        0x01U
#define OV7692_RGAIN_REG        0x02U
#define OV7692_GAIN_REG          0x00U
#define OV7692_AECH_REG          0x0fU
#define OV7692_AECL_REG          0x10U
#define OV7692_COM8_REG         0x13U
#define OV7692_COM8_AEC_ENABLE   0x01U
#define OV7692_COM8_AGC_ENABLE   0x04U
#define OV7692_COM8_AUTO         0xf7U
#define OV7692_COM8_MANUAL_WB    0xf5U
#define OV7692_REG15_REG         0x15U
#define OV7692_AEC_WPT_REG       0x24U
#define OV7692_AEC_BPT_REG       0x25U
#define OV7692_AEC_VPT_REG       0x26U
#define OV7692_SDE_CTRL_REG      0xd2U
#define OV7692_CONTRAST_SIGN_REG 0xd3U
#define OV7692_CONTRAST_GAIN_REG 0xd4U
#define OV7692_CONTRAST_CTRL_REG 0xd5U
#define OV7692_SATURATION_U_REG  0xd8U
#define OV7692_SATURATION_V_REG  0xd9U
#define CNN_QUAD0_DSRAM_START     0x50400000UL
#define CNN_QUAD1_DSRAM_START     0x50800000UL
#define CNN_QUAD2_DSRAM_START     0x50c00000UL
#define CNN_QUAD3_DSRAM_START     0x51000000UL
#define CNN_DSRAM_QUAD_BYTES      0x00020000UL
#define CNN_FRAME_BANK_COUNT       2U
#define CAMERA_LINE_BYTES          (CAMERA_FRAME_WIDTH * CAMERA_BYTES_PER_PIXEL)
#define CNN_ROWS_PER_QUAD          (CNN_DSRAM_QUAD_BYTES / CAMERA_LINE_BYTES)
#define CAMERA_STATS_SCALE         4U
#define CAMERA_STATS_WIDTH         (CAMERA_FRAME_WIDTH / CAMERA_STATS_SCALE)
#define CAMERA_STATS_HEIGHT        (CAMERA_FRAME_HEIGHT / CAMERA_STATS_SCALE)
#define CAMERA_STATS_PIXELS        (CAMERA_STATS_WIDTH * CAMERA_STATS_HEIGHT)

_Static_assert(CAMERA_FRAME_BYTES <= 2U * CNN_DSRAM_QUAD_BYTES,
	       "QVGA RGB565 frame exceeds the reserved CNN SRAM quadrants");
_Static_assert(!(CAMERA_FRAME_WIDTH % CAMERA_STATS_SCALE) &&
	       !(CAMERA_FRAME_HEIGHT % CAMERA_STATS_SCALE),
	       "statistics sampling scale must divide the frame dimensions");
_Static_assert(!(CAMERA_MOTION_SCALE % CAMERA_STATS_SCALE),
	       "statistics sampling must include every motion-grid pixel");

extern uint32_t SystemCoreClock;

static volatile struct camera_ipc *const ipc =
	(volatile struct camera_ipc *)CAMERA_IPC_ADDR;
static uint8_t motion_history[CAMERA_MOTION_WIDTH * CAMERA_MOTION_HEIGHT];
static uint8_t capture_line[CAMERA_FRAME_WIDTH * CAMERA_BYTES_PER_PIXEL]
__attribute__((aligned(4)));
static volatile uint32_t pause_requested;
static volatile uint32_t direct_dma_frame_done;
static uint32_t direct_dma_overflows;
static int direct_dma_channel;
static uint32_t dropped_frames;
static volatile enum camera_white_balance requested_white_balance =
	CAMERA_WB_INCANDESCENT;
static volatile enum camera_exposure requested_exposure =
	CAMERA_EXPOSURE_POSITIVE_1;
static volatile uint32_t requested_saturation = CAMERA_SATURATION_MAX;
static volatile enum camera_tone_curve requested_tone_curve = CAMERA_TONE_FLAT;
static volatile uint32_t ae_relock_requested;
static enum camera_ae_state ae_state = CAMERA_AE_RESET;
static uint32_t ae_settle_frames;

static int init_riscv_timer(void)
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

	ret = MXC_TMR_Init(MXC_TMR0, &config, false);
	if (ret)
		return ret;
	MXC_TMR_Start(MXC_TMR0);
	return 0;
}

static uint32_t riscv_now_us(void)
{
	/* IBRO/128 is 57,600 Hz; 1,000,000 / 57,600 = 625 / 36. */
	return (uint32_t)(((uint64_t)MXC_TMR_GetCount(MXC_TMR0) * 625U) /
			  36U);
}

struct camera_ae_registers {
	uint8_t com8;
	uint8_t gain;
	uint8_t aech;
	uint8_t aecl;
	uint8_t reg15;
};

struct frame_accumulator {
	uint32_t luma_sum;
	uint32_t red_sum;
	uint32_t green_sum;
	uint32_t blue_sum;
	uint32_t shadow_pixels;
	uint32_t highlight_pixels;
	uint32_t luma_histogram[32];
	uint32_t edge_pixels;
	uint32_t motion_pixels;
	uint32_t motion_min_x;
	uint32_t motion_min_y;
	uint32_t motion_max_x;
	uint32_t motion_max_y;
	uint32_t processing_us;
};

struct capture_context {
	struct frame_accumulator accumulator;
	uint32_t bank;
	uint32_t frame_number;
	uint32_t start_us;
	uint32_t analysis_row;
	uint32_t dma_complete;
	int error;
};

static int capture_poll(struct capture_context *capture);

static uintptr_t cnn_frame_row_address(uint32_t bank, uint32_t row)
{
	uintptr_t first_quad = bank ? CNN_QUAD2_DSRAM_START :
			       CNN_QUAD0_DSRAM_START;
	uintptr_t second_quad = bank ? CNN_QUAD3_DSRAM_START :
				CNN_QUAD1_DSRAM_START;

	if (row < CNN_ROWS_PER_QUAD)
		return first_quad + row * CAMERA_LINE_BYTES;
	return second_quad +
	       (row - CNN_ROWS_PER_QUAD) * CAMERA_LINE_BYTES;
}

static void cnn_frame_read_rows(uint8_t *destination, uint32_t bank,
				uint32_t first_row, uint32_t rows)
{
	while (rows--) {
		volatile const uint32_t *source =
			(volatile const uint32_t *)cnn_frame_row_address(bank,
					first_row++);
		uint32_t *destination_words = (uint32_t *)destination;
		uint32_t index;

		for (index = 0; index < CAMERA_LINE_BYTES / sizeof(uint32_t);
		     index++)
			destination_words[index] = source[index];
		destination += CAMERA_LINE_BYTES;
	}
}

static void direct_dma_start_frame(uint32_t bank)
{
	volatile mxc_dma_ch_regs_t *channel = &MXC_DMA->ch[direct_dma_channel];
	uint32_t first_bytes = CNN_ROWS_PER_QUAD * CAMERA_LINE_BYTES;
	uint32_t second_bytes = CAMERA_FRAME_BYTES - first_bytes;
	uintptr_t first_quad = bank ? CNN_QUAD2_DSRAM_START :
			       CNN_QUAD0_DSRAM_START;
	uintptr_t second_quad = bank ? CNN_QUAD3_DSRAM_START :
				CNN_QUAD1_DSRAM_START;

	channel->ctrl &= ~(MXC_F_DMA_CTRL_EN | MXC_F_DMA_CTRL_RLDEN |
			   MXC_F_DMA_CTRL_CTZ_IE | MXC_F_DMA_CTRL_DIS_IE);
	channel->status = MXC_F_DMA_STATUS_CTZ_IF |
			  MXC_F_DMA_STATUS_RLD_IF |
			  MXC_F_DMA_STATUS_BUS_ERR | MXC_F_DMA_STATUS_TO_IF;
	direct_dma_frame_done = 0U;
	channel->dst = first_quad;
	channel->cnt = first_bytes;
	channel->dstrld = second_quad;
	channel->cntrld = MXC_F_DMA_CNTRLD_EN | second_bytes;
	channel->ctrl |= MXC_F_DMA_CTRL_RLDEN | MXC_F_DMA_CTRL_EN;
	MXC_PCIF->int_fl = MXC_PCIF->int_fl;
	MXC_PCIF_Start(MXC_PCIF_READMODE_SINGLE_MODE);
}

static int cnn_frame_self_test(void)
{
	volatile uint32_t *quad0 = (volatile uint32_t *)CNN_QUAD0_DSRAM_START;
	volatile uint32_t *quad1 = (volatile uint32_t *)CNN_QUAD1_DSRAM_START;

	*quad0 = 0x43514e30U;
	*quad1 = 0x43514e31U;
	if (*quad0 != 0x43514e30U || *quad1 != 0x43514e31U)
		return -EIO;
	return 0;
}

static void notify_host(enum camera_ipc_event event, uint32_t value)
{
	maxim_ipc_raw_mbox_to_host(CAMERA_EVENT_WORD(event, value));
	maxim_ipc_raw_ring_host();
}

static void service_command(void)
{
	uint32_t command;

	if (!maxim_ipc_raw_pending_coproc())
		return;

	command = maxim_ipc_raw_mbox_from_host();
	maxim_ipc_raw_ack_coproc();

	switch (CAMERA_COMMAND_TYPE(command)) {
	case CAMERA_CMD_PAUSE:
	case CAMERA_CMD_STOP:
		pause_requested = 1;
		break;
	case CAMERA_CMD_RESUME:
		pause_requested = 0;
		break;
	case CAMERA_CMD_SET_WHITE_BALANCE:
		if (CAMERA_COMMAND_VALUE(command) < CAMERA_WB_COUNT) {
			requested_white_balance = CAMERA_COMMAND_VALUE(command);
			ae_relock_requested = 1U;
		}
		break;
	case CAMERA_CMD_SET_EXPOSURE:
		if (CAMERA_COMMAND_VALUE(command) < CAMERA_EXPOSURE_COUNT) {
			requested_exposure = CAMERA_COMMAND_VALUE(command);
			ae_relock_requested = 1U;
		}
		break;
	case CAMERA_CMD_SET_SATURATION:
		if (CAMERA_COMMAND_VALUE(command) <= CAMERA_SATURATION_MAX)
			requested_saturation = CAMERA_COMMAND_VALUE(command);
		break;
	case CAMERA_CMD_SET_TONE_CURVE:
		if (CAMERA_COMMAND_VALUE(command) < CAMERA_TONE_COUNT)
			requested_tone_curve = CAMERA_COMMAND_VALUE(command);
		break;
	case CAMERA_CMD_RELOCK_EXPOSURE:
		ae_relock_requested = 1U;
		break;
	default:
		break;
	}
}

static int apply_white_balance(enum camera_white_balance mode)
{
	int ret;

	/* Preserve the COM8 values already proven with this EVKIT camera module. */
	if (mode == CAMERA_WB_AUTO)
		return camera_write_reg(OV7692_COM8_REG, OV7692_COM8_AUTO);

	ret = camera_write_reg(OV7692_COM8_REG, OV7692_COM8_MANUAL_WB);
	if (ret)
		return ret;

	switch (mode) {
	case CAMERA_WB_INCANDESCENT:
		/* Increase blue and reduce red under warm indoor illumination. */
		ret = camera_write_reg(OV7692_BGAIN_REG, 0x66U);
		if (!ret)
			ret = camera_write_reg(OV7692_RGAIN_REG, 0x40U);
		break;
	case CAMERA_WB_DAYLIGHT:
		ret = camera_write_reg(OV7692_BGAIN_REG, 0x43U);
		if (!ret)
			ret = camera_write_reg(OV7692_RGAIN_REG, 0x5dU);
		break;
	case CAMERA_WB_CLOUDY:
		ret = camera_write_reg(OV7692_BGAIN_REG, 0x48U);
		if (!ret)
			ret = camera_write_reg(OV7692_RGAIN_REG, 0x63U);
		break;
	default:
		ret = -1;
		break;
	}

	return ret;
}

static int apply_exposure(enum camera_exposure exposure)
{
	static const uint8_t targets[CAMERA_EXPOSURE_COUNT][3] = {
		{ 0x50U, 0x40U, 0xa2U },
		{ 0x70U, 0x60U, 0xa2U },
		{ 0x86U, 0x76U, 0xb3U },
		{ 0xa8U, 0xa0U, 0xc4U },
		{ 0xc0U, 0xb8U, 0xe6U },
	};
	int ret;

	if (exposure >= CAMERA_EXPOSURE_COUNT)
		return -EINVAL;

	ret = camera_write_reg(OV7692_AEC_WPT_REG, targets[exposure][0]);
	if (!ret)
		ret = camera_write_reg(OV7692_AEC_BPT_REG, targets[exposure][1]);
	if (!ret)
		ret = camera_write_reg(OV7692_AEC_VPT_REG, targets[exposure][2]);
	return ret;
}

static int read_ae_registers(struct camera_ae_registers *regs)
{
	int ret;

	ret = camera_read_reg(OV7692_COM8_REG, &regs->com8);
	if (!ret)
		ret = camera_read_reg(OV7692_GAIN_REG, &regs->gain);
	if (!ret)
		ret = camera_read_reg(OV7692_AECH_REG, &regs->aech);
	if (!ret)
		ret = camera_read_reg(OV7692_AECL_REG, &regs->aecl);
	if (!ret)
		ret = camera_read_reg(OV7692_REG15_REG, &regs->reg15);
	return ret;
}

static void publish_ae_registers(const struct camera_ae_registers *regs)
{
	ipc->ae_exposure_lines = ((uint32_t)regs->aech << 8) | regs->aecl;
	/* GAIN is the low 8 bits; REG15[1:0] supplies the digital-gain bits. */
	ipc->ae_gain_raw = ((uint32_t)(regs->reg15 & 0x03U) << 8) |
			   regs->gain;
	ipc->ae_com8 = regs->com8;
}

static int arm_auto_exposure(void)
{
	uint8_t com8;
	int ret;

	ret = camera_read_reg(OV7692_COM8_REG, &com8);
	if (!ret) {
		com8 |= OV7692_COM8_AEC_ENABLE | OV7692_COM8_AGC_ENABLE;
		ret = camera_write_reg(OV7692_COM8_REG, com8);
	}
	if (ret)
		return ret;

	ae_state = CAMERA_AE_SETTLING;
	ae_settle_frames = 0U;
	ipc->ae_com8 = com8;
	ipc->ae_settle_frames = 0U;
	camera_ipc_compiler_barrier();
	ipc->ae_state = CAMERA_AE_SETTLING;
	return 0;
}

static int finish_auto_exposure_frame(void)
{
	struct camera_ae_registers regs;
	uint8_t locked_com8;
	int ret;

	if (ae_state != CAMERA_AE_SETTLING)
		return 0;

	/* Snapshot after a complete frame; overflow frames never reach here. */
	ret = read_ae_registers(&regs);
	if (ret)
		return ret;
	publish_ae_registers(&regs);
	ae_settle_frames++;
	ipc->ae_settle_frames = ae_settle_frames;
	if (ae_settle_frames < CAMERA_AE_SETTLE_FRAMES)
		return 0;

	/*
	 * COM8's manual selection retains the sensor's current exposure and gain.
	 * Do not rewrite GAIN/REG15/AECH/AECL here: those multi-register writes can
	 * change line/frame timing immediately before the next PCIF capture. The
	 * sampled values above are diagnostic snapshots only.
	 */
	locked_com8 = regs.com8 &
		      (uint8_t)~(OV7692_COM8_AEC_ENABLE | OV7692_COM8_AGC_ENABLE);
	ret = camera_write_reg(OV7692_COM8_REG, locked_com8);
	if (ret)
		return ret;

	/* Read once after disabling the loop so telemetry is the frozen state. */
	ret = read_ae_registers(&regs);
	if (ret)
		return ret;
	publish_ae_registers(&regs);
	ae_state = CAMERA_AE_LOCKED;
	camera_ipc_compiler_barrier();
	ipc->ae_state = CAMERA_AE_LOCKED;
	return 0;
}

static int apply_saturation(uint32_t saturation)
{
	uint8_t gain;
	int ret;

	if (saturation > CAMERA_SATURATION_MAX)
		return -EINVAL;

	gain = (uint8_t)(saturation << 4);
	ret = camera_write_reg(OV7692_SATURATION_U_REG, gain);
	if (!ret)
		ret = camera_write_reg(OV7692_SATURATION_V_REG, gain);
	if (!ret)
		ret = camera_write_reg(OV7692_SDE_CTRL_REG, 0x02U);
	return ret;
}

static int apply_tone_curve(enum camera_tone_curve tone_curve)
{
	/*
	 * A3-B1 are input endpoints for fixed output values 0x04, 0x08, 0x10,
	 * 0x20, ..., 0xd0; B2 is the final slope. FLAT therefore uses endpoints
	 * equal to those output values for a genuinely linear gamma transform.
	 * BALANCED uses the sensor reset curve and PUNCHY uses a vendor curve.
	 */
	static const uint8_t curves[CAMERA_TONE_COUNT][16] = {
		{
			0x40U, 0x04U, 0x08U, 0x10U, 0x20U, 0x28U, 0x30U, 0x38U,
			0x40U, 0x48U, 0x50U, 0x60U, 0x70U, 0x90U, 0xb0U, 0xd0U
		},
		{
			0x20U, 0x10U, 0x12U, 0x35U, 0x5aU, 0x69U, 0x76U, 0x80U,
			0x88U, 0x8fU, 0x96U, 0xa3U, 0xafU, 0xc4U, 0xd7U, 0xe8U
		},
		{
			0x06U, 0x0bU, 0x15U, 0x2aU, 0x51U, 0x63U, 0x74U, 0x83U,
			0x91U, 0x9eU, 0xaaU, 0xbeU, 0xceU, 0xe5U, 0xf3U, 0xfbU
		},
	};
	static const uint8_t registers[16] = {
		0xb2U, 0xa3U, 0xa4U, 0xa5U, 0xa6U, 0xa7U, 0xa8U, 0xa9U,
		0xaaU, 0xabU, 0xacU, 0xadU, 0xaeU, 0xafU, 0xb0U, 0xb1U,
	};
	/* D5/D4/D3 values correspond to the OV7692 -2, -1 and 0 contrast modes. */
	static const uint8_t contrast[CAMERA_TONE_COUNT][3] = {
		{ 0x20U, 0x18U, 0x48U },
		{ 0x20U, 0x1cU, 0x20U },
		{ 0x20U, 0x20U, 0x00U },
	};
	uint32_t index;
	int ret = 0;

	if (tone_curve >= CAMERA_TONE_COUNT)
		return -EINVAL;

	for (index = 0; index < 16U && !ret; index++)
		ret = camera_write_reg(registers[index], curves[tone_curve][index]);
	if (!ret)
		ret = camera_write_reg(OV7692_CONTRAST_CTRL_REG,
				       contrast[tone_curve][0]);
	if (!ret)
		ret = camera_write_reg(OV7692_CONTRAST_GAIN_REG,
				       contrast[tone_curve][1]);
	if (!ret)
		ret = camera_write_reg(OV7692_CONTRAST_SIGN_REG,
				       contrast[tone_curve][2]);
	if (!ret)
		/* Keep both the saturation (bit 1) and contrast (bit 2) stages on. */
		ret = camera_write_reg(OV7692_SDE_CTRL_REG, 0x06U);
	return ret;
}

/* Set a debugger breakpoint here after requesting CAMERA_CMD_PAUSE. */
__attribute__((noinline, used))
void rv_debug_safe_point(void)
{
	__asm__ volatile("nop");
}

static void wait_while_paused(void)
{
	if (!pause_requested)
		return;

	ipc->state = CAMERA_STATE_PAUSED;
	camera_ipc_compiler_barrier();
	notify_host(CAMERA_EVENT_PAUSED, ipc->stats.frame_number);

	while (pause_requested) {
		rv_debug_safe_point();
		service_command();
	}

	ipc->state = CAMERA_STATE_RUNNING;
	camera_ipc_compiler_barrier();
	notify_host(CAMERA_EVENT_RESUMED, ipc->stats.frame_number);
}

static uint8_t rgb565_luma(const uint8_t *pixel, uint32_t *red_out,
			   uint32_t *green_out, uint32_t *blue_out)
{
	uint32_t red = pixel[0] & 0xf8U;
	uint32_t green = (((uint32_t)pixel[0] & 0x07U) << 5) |
			 (((uint32_t)pixel[1] & 0xe0U) >> 3);
	uint32_t blue = ((uint32_t)pixel[1] & 0x1fU) << 3;

	*red_out = red;
	*green_out = green;
	*blue_out = blue;

	/* Integer approximation of 0.299R + 0.587G + 0.114B. */
	return (uint8_t)((77U * red + 150U * green + 29U * blue) >> 8);
}

static void process_line(const uint8_t *line, uint32_t y,
			 struct frame_accumulator *acc, uint32_t frame_number)
{
	uint32_t previous_luma = 0;
	uint32_t x;

	for (x = 0; x < CAMERA_FRAME_WIDTH; x += CAMERA_STATS_SCALE) {
		uint32_t red;
		uint32_t green;
		uint32_t blue;
		uint32_t luma = rgb565_luma(&line[x * CAMERA_BYTES_PER_PIXEL],
					    &red, &green, &blue);

		acc->luma_sum += luma;
		acc->red_sum += red;
		acc->green_sum += green;
		acc->blue_sum += blue;
		if (luma < 16U)
			acc->shadow_pixels++;
		if (luma > 239U)
			acc->highlight_pixels++;
		acc->luma_histogram[luma >> 3]++;
		if (x && (luma > previous_luma ? luma - previous_luma :
			  previous_luma - luma) > CAMERA_EDGE_THRESHOLD)
			acc->edge_pixels++;
		previous_luma = luma;

		if (!(y % CAMERA_MOTION_SCALE) && !(x % CAMERA_MOTION_SCALE)) {
			uint32_t sx = x / CAMERA_MOTION_SCALE;
			uint32_t sy = y / CAMERA_MOTION_SCALE;
			uint32_t index = sy * CAMERA_MOTION_WIDTH + sx;
			uint32_t old = motion_history[index];
			uint32_t delta = luma > old ? luma - old : old - luma;

			motion_history[index] = (uint8_t)luma;
			if (frame_number && delta > CAMERA_MOTION_THRESHOLD) {
				acc->motion_pixels++;
				if (sx < acc->motion_min_x)
					acc->motion_min_x = sx;
				if (sy < acc->motion_min_y)
					acc->motion_min_y = sy;
				if (sx > acc->motion_max_x)
					acc->motion_max_x = sx;
				if (sy > acc->motion_max_y)
					acc->motion_max_y = sy;
			}
		}
	}
}

static volatile struct camera_tile_desc *acquire_tile(uint32_t sequence,
		uint32_t y, uint32_t *slot,
		struct capture_context *capture)
{
	volatile struct camera_tile_desc *tile;

	*slot = sequence % CAMERA_TILE_COUNT;
	tile = &ipc->tile[*slot];
	while (tile->state != CAMERA_TILE_FREE) {
		service_command();
		if (capture)
			(void)capture_poll(capture);
	}

	tile->sequence = sequence;
	tile->y = y;
	tile->rows = 0;
	return tile;
}

static void publish_tile(volatile struct camera_tile_desc *tile, uint32_t slot,
			 uint32_t rows)
{
	tile->rows = rows;
	camera_ipc_compiler_barrier();
	tile->state = CAMERA_TILE_READY;
	camera_ipc_compiler_barrier();
	notify_host(CAMERA_EVENT_TILE, slot);
}

static uint32_t luma_percentile(const struct frame_accumulator *acc,
				uint32_t percentile)
{
	uint32_t target = (CAMERA_STATS_PIXELS * percentile) / 100U;
	uint32_t count = 0;
	uint32_t bin;

	for (bin = 0; bin < 32U; bin++) {
		count += acc->luma_histogram[bin];
		if (count >= target)
			return bin * 8U + 4U;
	}

	return 255U;
}

static void publish_stats(const struct frame_accumulator *acc,
			  uint32_t frame_number, uint32_t frame_us,
			  uint32_t dma_overflows)
{
	volatile struct camera_frame_stats *stats = &ipc->stats;
	uint32_t sequence = stats->sequence;

	stats->sequence = sequence + 1U;
	camera_ipc_compiler_barrier();
	stats->frame_number = frame_number;
	stats->capture_fps_x10 = frame_us ? 10000000U / frame_us : 0;
	stats->processing_us = acc->processing_us;
	stats->dma_overflows = dma_overflows;
	stats->dropped_frames = dropped_frames;
	stats->mean_luma = acc->luma_sum / CAMERA_STATS_PIXELS;
	stats->mean_red = acc->red_sum / CAMERA_STATS_PIXELS;
	stats->mean_green = acc->green_sum / CAMERA_STATS_PIXELS;
	stats->mean_blue = acc->blue_sum / CAMERA_STATS_PIXELS;
	stats->shadow_percent_x10 = (acc->shadow_pixels * 1000U) /
				    CAMERA_STATS_PIXELS;
	stats->highlight_percent_x10 = (acc->highlight_pixels * 1000U) /
				       CAMERA_STATS_PIXELS;
	stats->luma_p10 = luma_percentile(acc, 10U);
	stats->luma_p50 = luma_percentile(acc, 50U);
	stats->luma_p90 = luma_percentile(acc, 90U);
	/* Scale the sampled count to an approximate full-frame edge count. */
	stats->edge_pixels = acc->edge_pixels * CAMERA_STATS_SCALE *
			     CAMERA_STATS_SCALE;
	stats->motion_percent_x10 = (acc->motion_pixels * 1000U) /
				    (CAMERA_MOTION_WIDTH * CAMERA_MOTION_HEIGHT);

	if (acc->motion_pixels) {
		stats->roi_x = acc->motion_min_x * CAMERA_MOTION_SCALE;
		stats->roi_y = acc->motion_min_y * CAMERA_MOTION_SCALE;
		stats->roi_w = (acc->motion_max_x - acc->motion_min_x + 1U) *
			       CAMERA_MOTION_SCALE;
		stats->roi_h = (acc->motion_max_y - acc->motion_min_y + 1U) *
			       CAMERA_MOTION_SCALE;
	} else {
		stats->roi_x = 0;
		stats->roi_y = 0;
		stats->roi_w = 0;
		stats->roi_h = 0;
	}

	camera_ipc_compiler_barrier();
	stats->sequence = sequence + 2U;
}

static void publish_dropped_frame(uint32_t frame_number, uint32_t frame_us,
				  uint32_t dma_overflows)
{
	volatile struct camera_frame_stats *stats = &ipc->stats;
	uint32_t sequence = stats->sequence;

	dropped_frames++;
	stats->sequence = sequence + 1U;
	camera_ipc_compiler_barrier();
	stats->frame_number = frame_number;
	stats->capture_fps_x10 = frame_us ? 10000000U / frame_us : 0;
	stats->dma_overflows = dma_overflows;
	stats->dropped_frames = dropped_frames;
	camera_ipc_compiler_barrier();
	stats->sequence = sequence + 2U;
}

static void capture_begin(struct capture_context *capture, uint32_t bank,
			  uint32_t frame_number)
{
	memset(capture, 0, sizeof(*capture));
	capture->accumulator.motion_min_x = CAMERA_MOTION_WIDTH;
	capture->accumulator.motion_min_y = CAMERA_MOTION_HEIGHT;
	capture->bank = bank;
	capture->frame_number = frame_number;
	capture->start_us = riscv_now_us();
	direct_dma_start_frame(bank);
}

static int capture_poll(struct capture_context *capture)
{
	volatile mxc_dma_ch_regs_t *channel = &MXC_DMA->ch[direct_dma_channel];

	if (capture->error)
		return capture->error;

	if (!capture->dma_complete) {
		if (channel->status & MXC_F_DMA_STATUS_BUS_ERR) {
			MXC_PCIF_Stop();
			MXC_DMA_Stop(direct_dma_channel);
			direct_dma_overflows++;
			capture->error = -EOVERFLOW;
			return capture->error;
		}
		if (channel->ctrl & MXC_F_DMA_CTRL_EN) {
			if (riscv_now_us() - capture->start_us > 250000U) {
				MXC_PCIF_Stop();
				MXC_DMA_Stop(direct_dma_channel);
				direct_dma_overflows++;
				capture->error = -EOVERFLOW;
			}
			return capture->error;
		}

		direct_dma_frame_done = 1U;
		MXC_PCIF_Stop();
		capture->dma_complete = 1U;
	}

	/* Consume one sampled row whenever TFT backpressure leaves CPU1 idle. */
	if (capture->analysis_row < CAMERA_FRAME_HEIGHT) {
		uint32_t row_start_us = riscv_now_us();

		cnn_frame_read_rows(capture_line, capture->bank,
				    capture->analysis_row, 1U);
		process_line(capture_line, capture->analysis_row,
			     &capture->accumulator, capture->frame_number);
		capture->analysis_row += CAMERA_STATS_SCALE;
		capture->accumulator.processing_us +=
			riscv_now_us() - row_start_us;
	}

	return 0;
}

static int capture_finish(struct capture_context *capture, uint32_t *frame_us)
{
	while (!capture->error &&
	       (!capture->dma_complete ||
		capture->analysis_row < CAMERA_FRAME_HEIGHT)) {
		service_command();
		(void)capture_poll(capture);
	}

	*frame_us = riscv_now_us() - capture->start_us;
	if (capture->error) {
		publish_dropped_frame(capture->frame_number, *frame_us,
				      direct_dma_overflows);
		notify_host(CAMERA_EVENT_FRAME, capture->frame_number);
		return capture->error;
	}

	publish_stats(&capture->accumulator, capture->frame_number, *frame_us,
		      direct_dma_overflows);
	notify_host(CAMERA_EVENT_FRAME, capture->frame_number);
	return 0;
}

static void publish_cnn_frame(uint32_t bank, uint32_t *tile_sequence,
			      struct capture_context *capture)
{
	uint32_t y;

	/* Capture targets the other CNN bank, so TFT backpressure loses no rows. */
	for (y = 0; y < CAMERA_FRAME_HEIGHT; y += CAMERA_TILE_ROWS) {
		volatile struct camera_tile_desc *tile;
		uint32_t slot;
		uint32_t rows = CAMERA_TILE_ROWS;

		if (y + rows > CAMERA_FRAME_HEIGHT)
			rows = CAMERA_FRAME_HEIGHT - y;
		tile = acquire_tile(*tile_sequence, y, &slot, capture);
		cnn_frame_read_rows((uint8_t *)ipc->pixels[slot], bank, y, rows);
		publish_tile(tile, slot, rows);
		(*tile_sequence)++;
		if (capture)
			(void)capture_poll(capture);
	}
}

int Camera_Power(int on)
{
	/* The EVKIT camera socket has no software-controlled power switch. */
	(void)on;
	return E_NO_ERROR;
}

static void stop_with_fault(uint32_t cause, uint32_t epc, uint32_t tval)
{
	ipc->fault_cause = cause;
	ipc->fault_epc = epc;
	ipc->fault_tval = tval;
	ipc->state = CAMERA_STATE_FAULT;
	ipc->magic = CAMERA_IPC_MAGIC;
	camera_ipc_compiler_barrier();
	notify_host(CAMERA_EVENT_FAULT, cause);
	for (;;)
		__asm__ volatile("wfi");
}

__attribute__((used))
void camera_riscv_fault(void)
{
	uint32_t mcause;
	uint32_t mepc;
	uint32_t mtval;

	__asm__ volatile("csrr %0, mcause" : "=r"(mcause));
	__asm__ volatile("csrr %0, mepc" : "=r"(mepc));
	__asm__ volatile("csrr %0, mtval" : "=r"(mtval));
	stop_with_fault(mcause, mepc, mtval);
}

void illegal_insn_handler(void) __attribute__((alias("camera_riscv_fault")));
void ecall_insn_handler(void) __attribute__((alias("camera_riscv_fault")));

__attribute__((used))
int main(void)
{
	struct capture_context capture;
	uint32_t tile_sequence = 0;
	uint32_t frame_number = 0;
	uint32_t frame_us = 0;
	uint32_t display_bank = 0;
	uint32_t slot;
	int camera_id = 0;
	int dma_channel;
	int ret;

	maxim_riscv_icc_enable();
	SystemCoreClock = RISCV_CLOCK_HZ;

	ipc->magic = 0;
	ipc->abi_version = CAMERA_IPC_ABI_VERSION;
	ipc->state = CAMERA_STATE_BOOTING;
	ipc->command = CAMERA_CMD_NONE;
	ipc->camera_id = 0;
	ipc->white_balance = CAMERA_WB_COUNT;
	ipc->exposure = CAMERA_EXPOSURE_COUNT;
	ipc->saturation = CAMERA_SATURATION_MAX + 1U;
	ipc->tone_curve = CAMERA_TONE_COUNT;
	ipc->sensor_clock_divider = OV7692_QVGA_CLKRC;
	ipc->ae_state = CAMERA_AE_RESET;
	ipc->ae_settle_frames = 0U;
	ipc->ae_exposure_lines = 0U;
	ipc->ae_gain_raw = 0U;
	ipc->ae_com8 = 0U;
	ipc->fault_cause = 0;
	ipc->fault_epc = 0;
	ipc->fault_tval = 0;
	memset((void *)&ipc->stats, 0, sizeof(ipc->stats));
	for (slot = 0; slot < CAMERA_TILE_COUNT; slot++) {
		ipc->tile[slot].state = CAMERA_TILE_FREE;
		ipc->tile[slot].sequence = 0;
		ipc->tile[slot].y = 0;
		ipc->tile[slot].rows = 0;
	}
	memset(motion_history, 0, sizeof(motion_history));
	camera_ipc_compiler_barrier();
	ipc->magic = CAMERA_IPC_MAGIC;
	notify_host(CAMERA_EVENT_BOOT, 0);

	MXC_DMA_Init();
	ret = init_riscv_timer();
	if (ret)
		stop_with_fault(0x80000003U, 0, (uint32_t)ret);
	ret = cnn_frame_self_test();
	if (ret)
		stop_with_fault(0x80000002U, 0, (uint32_t)ret);
	dma_channel = MXC_DMA_AcquireChannel();
	if (dma_channel != 0)
		stop_with_fault(0x80000001U, 0, (uint32_t)dma_channel);

	ret = camera_init(CAMERA_CLOCK_HZ);
	if (!ret)
		ret = camera_get_product_id(&camera_id);
	if (!ret)
		ret = camera_setup(CAMERA_FRAME_WIDTH, CAMERA_FRAME_HEIGHT,
				   PIXFORMAT_RGB565, FIFO_FOUR_BYTE,
				   STREAMING_DMA, dma_channel);
	if (!ret)
		ret = camera_write_reg(OV7692_CLKRC_REG, OV7692_QVGA_CLKRC);
	if (!ret) {
		direct_dma_channel = dma_channel;
		ret = MXC_DMA_SetCallback(dma_channel, NULL);
		if (!ret)
			ret = MXC_DMA_DisableInt(dma_channel);
	}
	if (!ret)
		ret = apply_white_balance(requested_white_balance);
	if (!ret)
		ret = apply_exposure(requested_exposure);
	if (!ret)
		ret = apply_saturation(requested_saturation);
	if (!ret)
		ret = apply_tone_curve(requested_tone_curve);
	if (!ret)
		ret = arm_auto_exposure();
	if (ret)
		stop_with_fault(0x80010000U | ((uint32_t)(-ret) & 0xffffU),
				0, (uint32_t)ret);

	ipc->camera_id = (uint32_t)camera_id;
	ipc->white_balance = requested_white_balance;
	ipc->exposure = requested_exposure;
	ipc->saturation = requested_saturation;
	ipc->tone_curve = requested_tone_curve;
	ipc->state = CAMERA_STATE_RUNNING;
	notify_host(CAMERA_EVENT_RESUMED, 0);

	/* Prime one complete bank before capture/display overlap begins. */
	for (;;) {
		capture_begin(&capture, display_bank, frame_number);
		ret = capture_finish(&capture, &frame_us);
		if (ret != -EOVERFLOW)
			break;
		frame_number++;
	}
	if (ret)
		stop_with_fault(0x80020000U | ((uint32_t)(-ret) & 0xffffU),
				0, (uint32_t)ret);
	ret = finish_auto_exposure_frame();
	if (ret)
		stop_with_fault(0x80080000U | ((uint32_t)(-ret) & 0xffffU),
				0, (uint32_t)ret);
	frame_number++;

	for (;;) {
		uint32_t controls_rearm_ae = 0U;

		service_command();
		wait_while_paused();
		if (ipc->white_balance != requested_white_balance) {
			ret = apply_white_balance(requested_white_balance);
			if (ret)
				stop_with_fault(0x80030000U |
						((uint32_t)(-ret) & 0xffffU), 0,
						(uint32_t)ret);
			ipc->white_balance = requested_white_balance;
			controls_rearm_ae = 1U;
		}
		if (ipc->exposure != requested_exposure) {
			ret = apply_exposure(requested_exposure);
			if (ret)
				stop_with_fault(0x80040000U |
						((uint32_t)(-ret) & 0xffffU), 0,
						(uint32_t)ret);
			ipc->exposure = requested_exposure;
			controls_rearm_ae = 1U;
		}
		if (ipc->saturation != requested_saturation) {
			ret = apply_saturation(requested_saturation);
			if (ret)
				stop_with_fault(0x80050000U |
						((uint32_t)(-ret) & 0xffffU), 0,
						(uint32_t)ret);
			ipc->saturation = requested_saturation;
		}
		if (ipc->tone_curve != requested_tone_curve) {
			ret = apply_tone_curve(requested_tone_curve);
			if (ret)
				stop_with_fault(0x80060000U |
						((uint32_t)(-ret) & 0xffffU), 0,
						(uint32_t)ret);
			ipc->tone_curve = requested_tone_curve;
		}
		if (controls_rearm_ae || ae_relock_requested) {
			ae_relock_requested = 0U;
			ret = arm_auto_exposure();
			if (ret)
				stop_with_fault(0x80070000U |
						((uint32_t)(-ret) & 0xffffU), 0,
						(uint32_t)ret);
		}
		publish_cnn_frame(display_bank, &tile_sequence, NULL);
		service_command();
		wait_while_paused();
		capture_begin(&capture, display_bank, frame_number);
		ret = capture_finish(&capture, &frame_us);
		if (ret == -EOVERFLOW) {
			frame_number++;
			continue;
		}
		if (ret)
			stop_with_fault(0x80020000U | ((uint32_t)(-ret) & 0xffffU),
					0, (uint32_t)ret);
		ret = finish_auto_exposure_frame();
		if (ret)
			stop_with_fault(0x80080000U | ((uint32_t)(-ret) & 0xffffU),
					0, (uint32_t)ret);
		frame_number++;
	}
}
