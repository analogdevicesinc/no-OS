/***************************************************************************//**
 *   @file   camera_overlay.h
 *   @brief  Tiny RGB565 text renderer for the camera statistics band.
 *   @author Victor Pascu (victor.pascu@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef CAMERA_OVERLAY_H_
#define CAMERA_OVERLAY_H_

#include <stdint.h>

#include "camera_ipc.h"

void camera_overlay_line(uint8_t *tile_pixels, const char *text);
void camera_overlay_color_key(uint8_t *tile_pixels);

#endif /* CAMERA_OVERLAY_H_ */
