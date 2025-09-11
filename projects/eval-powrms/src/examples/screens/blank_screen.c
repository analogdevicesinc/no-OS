/***************************************************************************//**
 *   @file   blank_screen.c
 *   @brief  Blank screen implementation for display blanking functionality
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "blank_screen.h"
#include "../example/buttons.h"


void blank_screen(void)
{
    int input_char;
    int ret;
    // Set MCU_LED poin to low

    ret = no_os_gpio_direction_output(mcu_led_gpio_desc, NO_OS_GPIO_LOW);
    if (ret) {
        printf("Failed to set MCU LED GPIO direction.\n\r");
    }

    // --------------------- SETUP SCREEN ------------------

    // Create a completely blank screen object
    lv_obj_t *scr = lv_obj_create(NULL);

    // Set the screen background to black
    lv_obj_set_style_bg_color(scr, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN);

    // Load the blank screen
    lv_scr_load(scr);
    lv_timer_handler();

    while (display_entry == DISPLAY_ENTRY_BLANK) {
        uint8_t tmp = read_input();
        switch (tmp) {
        case 1: // NEXT
        case 2: // BACK
        case 3: // ENTER
        case 4: // BLANCK SCREEN
            break;
        case 5:
            display_entry = DISPLAY_INPUT_FREQUENCY;
            // Set MCULED to high
            ret = no_os_gpio_direction_output(mcu_led_gpio_desc, NO_OS_GPIO_HIGH);
            if (ret) {
                printf("Failed to set MCU LED GPIO direction.\n\r");
            }
            lv_obj_del(scr);
            return;
        default:
            break;
        }
    }

    // Cleanup screen if we exit the loop abnormally
    lv_obj_del(scr);
}
