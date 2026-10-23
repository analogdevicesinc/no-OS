/***************************************************************************//**
 *   @file   global_screen.c
 *   @brief  Global screen implementation for system-wide display management in eval-powrms project
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "global_screen.h"
#include "lvgl.h"

// Global variables
lv_obj_t * screen_global_ptr = NULL;

/**
 * @brief Get or create the global screen object
 *
 * @return lv_obj_t* Pointer to the global screen object
 */
lv_obj_t * get_global_screen(void)
{
	if (!screen_global_ptr)
		screen_global_ptr = lv_obj_create(NULL);

	return screen_global_ptr;
}

/**
 * @brief Delete the global screen resources
 *
 * @return int 0 on success, negative error code otherwise
 */
int delete_global_screen(void)
{
	if (screen_global_ptr) {
		lv_obj_del(screen_global_ptr);
		screen_global_ptr = NULL;
	}

	return 0;
}