/***************************************************************************//**
 * @file camera_tft.c
 * @brief Stock SSD2119 driver used by the QVGA camera demo.
 ******************************************************************************
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/

#include "camera_ipc.h"
#include "camera_tft.h"

/* Keep the MSDK implementation and its private transfer framing unmodified. */
#include "tft_ssd2119.c"

void camera_tft_show_rgb565(int x0, int y0, uint8_t *image,
			    int width, int height)
{
	MXC_TFT_ShowImageCameraRGB565(x0, y0, image, width, height);
}
