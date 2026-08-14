/***************************************************************************//**
 * @file camera_ipc.h
 * @brief Shared QVGA camera transport between MAX78000 CPU0 and CPU1.
 ******************************************************************************
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/

#ifndef CAPI_COPROCESSOR_MAX78000_CAMERA_IPC_H
#define CAPI_COPROCESSOR_MAX78000_CAMERA_IPC_H

#include <stdint.h>

#define CAMERA_IPC_MAGIC             0x43414D52UL /* "CAMR" */
#define CAMERA_IPC_ABI_VERSION       6UL

/*
 * SRAM2 is split deliberately:
 *   0x20010000..0x20010fff  OpenOCD RISC-V work area
 *   0x20011000..0x20013fff  camera IPC window
 *   0x20014000..0x2001bfff  RISC-V private data/heap/stack
 */
#define CAMERA_IPC_ADDR              0x20011000UL
#define CAMERA_IPC_WINDOW_SIZE       0x00003000UL

#define CAMERA_FRAME_WIDTH           320U
#define CAMERA_FRAME_HEIGHT          240U
#define CAMERA_BYTES_PER_PIXEL       2U
#define CAMERA_FRAME_BYTES           (CAMERA_FRAME_WIDTH * CAMERA_FRAME_HEIGHT * \
				      CAMERA_BYTES_PER_PIXEL)
#define CAMERA_TILE_ROWS             8U
#define CAMERA_TILE_COUNT            2U
#define CAMERA_TILE_BYTES            (CAMERA_FRAME_WIDTH * CAMERA_TILE_ROWS * \
				      CAMERA_BYTES_PER_PIXEL)

#define CAMERA_MOTION_WIDTH          80U
#define CAMERA_MOTION_HEIGHT         60U
#define CAMERA_MOTION_SCALE          4U
#define CAMERA_MOTION_THRESHOLD      20U
#define CAMERA_EDGE_THRESHOLD        32U

enum camera_ipc_state {
	CAMERA_STATE_RESET = 0,
	CAMERA_STATE_BOOTING,
	CAMERA_STATE_RUNNING,
	CAMERA_STATE_PAUSED,
	CAMERA_STATE_FAULT,
};

enum camera_ipc_command {
	CAMERA_CMD_NONE = 0,
	CAMERA_CMD_PAUSE,
	CAMERA_CMD_RESUME,
	CAMERA_CMD_STOP,
	CAMERA_CMD_SET_WHITE_BALANCE,
	CAMERA_CMD_SET_EXPOSURE,
	CAMERA_CMD_SET_SATURATION,
	CAMERA_CMD_SET_TONE_CURVE,
	CAMERA_CMD_RELOCK_EXPOSURE,
};

enum camera_ae_state {
	CAMERA_AE_RESET = 0,
	CAMERA_AE_SETTLING,
	CAMERA_AE_LOCKED,
};

/* Count only complete, non-overflow frames before freezing AEC and AGC. */
#define CAMERA_AE_SETTLE_FRAMES      2U

enum camera_white_balance {
	CAMERA_WB_AUTO = 0,
	CAMERA_WB_INCANDESCENT,
	CAMERA_WB_DAYLIGHT,
	CAMERA_WB_CLOUDY,
	CAMERA_WB_COUNT,
};

/* Stored as an unsigned index so it can travel in the 24-bit command value. */
enum camera_exposure {
	CAMERA_EXPOSURE_NEGATIVE_2 = 0,
	CAMERA_EXPOSURE_NEGATIVE_1,
	CAMERA_EXPOSURE_ZERO,
	CAMERA_EXPOSURE_POSITIVE_1,
	CAMERA_EXPOSURE_POSITIVE_2,
	CAMERA_EXPOSURE_COUNT,
};

#define CAMERA_SATURATION_MIN        0U
#define CAMERA_SATURATION_MAX        8U

enum camera_tone_curve {
	CAMERA_TONE_FLAT = 0,
	CAMERA_TONE_BALANCED,
	CAMERA_TONE_PUNCHY,
	CAMERA_TONE_COUNT,
};

#define CAMERA_COMMAND_WORD(command, value) \
	(((uint32_t)(command) & 0xffU) | (((uint32_t)(value) & 0x00ffffffU) << 8))
#define CAMERA_COMMAND_TYPE(word)       ((word) & 0xffU)
#define CAMERA_COMMAND_VALUE(word)      (((word) >> 8) & 0x00ffffffU)

enum camera_tile_state {
	CAMERA_TILE_FREE = 0,
	CAMERA_TILE_READY,
	CAMERA_TILE_DISPLAYING,
};

enum camera_ipc_event {
	CAMERA_EVENT_BOOT = 1,
	CAMERA_EVENT_TILE,
	CAMERA_EVENT_FRAME,
	CAMERA_EVENT_PAUSED,
	CAMERA_EVENT_RESUMED,
	CAMERA_EVENT_FAULT,
};

#define CAMERA_EVENT_WORD(event, value) \
	((((uint32_t)(event) & 0xffU) << 24) | ((uint32_t)(value) & 0x00ffffffU))
#define CAMERA_EVENT_TYPE(word)       (((word) >> 24) & 0xffU)
#define CAMERA_EVENT_VALUE(word)      ((word) & 0x00ffffffU)

struct camera_tile_desc {
	volatile uint32_t state;
	volatile uint32_t sequence;
	volatile uint32_t y;
	volatile uint32_t rows;
};

struct camera_frame_stats {
	/* Even sequence values are stable; odd values are being updated. */
	volatile uint32_t sequence;
	volatile uint32_t frame_number;
	volatile uint32_t capture_fps_x10;
	volatile uint32_t processing_us;
	volatile uint32_t dma_overflows;
	volatile uint32_t dropped_frames;
	volatile uint32_t mean_luma;
	volatile uint32_t mean_red;
	volatile uint32_t mean_green;
	volatile uint32_t mean_blue;
	volatile uint32_t shadow_percent_x10;
	volatile uint32_t highlight_percent_x10;
	volatile uint32_t luma_p10;
	volatile uint32_t luma_p50;
	volatile uint32_t luma_p90;
	volatile uint32_t edge_pixels;
	volatile uint32_t motion_percent_x10;
	volatile uint32_t roi_x;
	volatile uint32_t roi_y;
	volatile uint32_t roi_w;
	volatile uint32_t roi_h;
};

struct camera_ipc {
	volatile uint32_t magic;
	volatile uint32_t abi_version;
	volatile uint32_t state;
	volatile uint32_t command;
	volatile uint32_t camera_id;
	volatile uint32_t white_balance;
	volatile uint32_t exposure;
	volatile uint32_t saturation;
	volatile uint32_t tone_curve;
	volatile uint32_t sensor_clock_divider;
	volatile uint32_t ae_state;
	volatile uint32_t ae_settle_frames;
	volatile uint32_t ae_exposure_lines;
	volatile uint32_t ae_gain_raw;
	volatile uint32_t ae_com8;
	volatile uint32_t fault_cause;
	volatile uint32_t fault_epc;
	volatile uint32_t fault_tval;
	struct camera_frame_stats stats;
	struct camera_tile_desc tile[CAMERA_TILE_COUNT];
	uint8_t pixels[CAMERA_TILE_COUNT][CAMERA_TILE_BYTES];
};

_Static_assert(sizeof(struct camera_ipc) <= CAMERA_IPC_WINDOW_SIZE,
	       "camera IPC ABI exceeds its reserved SRAM2 window");

static inline volatile struct camera_ipc *camera_ipc_get(void)
{
	return (volatile struct camera_ipc *)CAMERA_IPC_ADDR;
}

/* Compiler ordering is sufficient: neither MAX78000 core has a data cache. */
static inline void camera_ipc_compiler_barrier(void)
{
	__asm__ volatile("" ::: "memory");
}

#endif /* CAPI_COPROCESSOR_MAX78000_CAMERA_IPC_H */
