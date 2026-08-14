/***************************************************************************//**
 *   @file   camera_tft.h
 *   @brief  Lossless row-batched SSD2119 RGB565 camera writer.
 *   @author Victor Pascu (victor.pascu@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef CAMERA_TFT_H
#define CAMERA_TFT_H

#include <stdint.h>

void camera_tft_show_rgb565(int x0, int y0, uint8_t *image,
			    int width, int height);

/* Hold the SSD2119 reset line low for 20 ms and wait for it to restart. */
void camera_tft_hard_reset(void);

/*
 * Hard reset and reprogram the SSD2119 without touching its SPI setup. The
 * panel is black until the next frame is drawn.
 */
void camera_tft_reinit(void);

#endif /* CAMERA_TFT_H */
