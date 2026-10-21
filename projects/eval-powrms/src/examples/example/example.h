/***************************************************************************//**
 *   @file   example.h
 *   @brief  EVAL-POWRMS example header for powrms project
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __EVAL_POWRMS_EXAMPLE__
#define __EVAL_POWRMS_EXAMPLE__

#include "common_data.h"
#include "no_os_util.h"
#include <stdint.h>
#include <stdio.h>
#include "no_os_delay.h"
#include "no_os_i2c.h"
#include "display.h"
#include "ssd_1306.h"
#include "lvgl.h"
#include "lv_types.h"
#include "lv_display.h"
#include "lv_display_private.h"
#include "no_os_init.h"
#include <string.h>
#include "no_os_display.h"
#include "no_os_error.h"

#include "powrms_utils.h"
#include "subscreen_main_menu_screen.h"
#include "subscreen_show_screen.h"
#include "subscreen_settings_screen.h"
#include "powrms_data_processing.h"

// #define POWRMS_UART_DEBUG

int example_main();

#endif /* __EVAL_POWRMS_EXAMPLE__ */
