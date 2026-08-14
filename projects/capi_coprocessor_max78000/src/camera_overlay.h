/***************************************************************************//**
 * @file camera_overlay.h
 * @brief Tiny RGB565 text renderer for the camera statistics band.
 ******************************************************************************/

#ifndef CAPI_COPROCESSOR_MAX78000_CAMERA_OVERLAY_H
#define CAPI_COPROCESSOR_MAX78000_CAMERA_OVERLAY_H

#include <stdint.h>

#include "camera_ipc.h"

void camera_overlay_line(uint8_t *tile_pixels, const char *text);
void camera_overlay_color_key(uint8_t *tile_pixels);

#endif /* CAPI_COPROCESSOR_MAX78000_CAMERA_OVERLAY_H */
