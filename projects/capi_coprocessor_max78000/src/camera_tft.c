/***************************************************************************//**
 *   @file   camera_tft.c
 *   @brief  Stock SSD2119 driver used by the QVGA camera demo.
 *   @author Victor Pascu (victor.pascu@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "camera_ipc.h"
#include "camera_tft.h"

/* Keep the MSDK implementation and its private transfer framing unmodified. */
#include "tft_ssd2119.c"

void camera_tft_show_rgb565(int x0, int y0, uint8_t *image,
			    int width, int height)
{
	MXC_TFT_ShowImageCameraRGB565(x0, y0, image, width, height);
}

void camera_tft_hard_reset(void)
{
	if (!reset_pin.port)
		return;

	/*
	 * The panel's analog state can collapse (washed-out picture, flicker) and
	 * survive the stock 20 us reset pulse, but not the board reset. Hold the
	 * reset line low for milliseconds, then give the boosters time to start.
	 */
	MXC_GPIO_OutClr(reset_pin.port, reset_pin.mask);
	MXC_Delay(20000);
	MXC_GPIO_OutSet(reset_pin.port, reset_pin.mask);
	MXC_Delay(150000);
}

void camera_tft_reinit(void)
{
	camera_tft_hard_reset();
	displayInit();
	/* The reset leaves undefined (white) memory; show black until redraw. */
	MXC_TFT_SetBackGroundColor(0);
}
