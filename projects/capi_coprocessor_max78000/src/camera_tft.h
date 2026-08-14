/***************************************************************************//**
 * @file camera_tft.h
 * @brief Lossless row-batched SSD2119 RGB565 camera writer.
 ******************************************************************************
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/

#ifndef CAMERA_TFT_H
#define CAMERA_TFT_H

#include <stdint.h>

void camera_tft_show_rgb565(int x0, int y0, uint8_t *image,
			    int width, int height);

#endif /* CAMERA_TFT_H */
