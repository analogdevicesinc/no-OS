/***************************************************************************//**
 *   @file   example.h
 *   @brief  EVAL-POWRMS example header for powrms project
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

int ltc3556_write_init_command(struct no_os_i2c_desc *param);

#endif /* __EVAL_POWRMS_EXAMPLE__ */
