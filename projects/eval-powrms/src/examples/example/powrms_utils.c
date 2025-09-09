/***************************************************************************//**
 *   @file   example.h
 *   @brief  Ssd1306 example header for ssd1306 project
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

#include "powrms_utils.h"
#include "buttons.h"

uint8_t read_input()
{
    char input_char;
    int button_state;
    
    // Check for button input first (hardware buttons take priority)
    button_state = buttons_read();
    if (button_state > 0) {
#ifdef POWRMS_UART_DEBUG
        const char* btn_names[] = {"", "NEXT", "BACK", "ENTER"};
        printf("Button %s pressed\n\r", btn_names[button_state]);
#endif
        return button_state;
    }

    // Fall back to UART input for debugging/development
    if (no_os_uart_read(demo_uart_desc, &input_char, 1) > 0) {
        if (input_char == 'q') {
#ifdef POWRMS_UART_DEBUG
            printf("NEXT\n\r");
#endif
            return 1;
        } else if (input_char == 'w') {
#ifdef POWRMS_UART_DEBUG
            printf("BACK\n\r");
#endif
            return 2;
        } else if (input_char == 'e') {
#ifdef POWRMS_UART_DEBUG
            printf("ENTER\n\r");
#endif
            return 3;
        }
    }
    return 0;
}


void powrms_float_to_str(float var, char* text_buf, uint8_t int_precision, uint8_t float_precision)
{
    uint8_t i = -1;
    if (!text_buf)
        return;
    // Integer part
    int int_part = (int)var;
    int abs_int_part = int_part < 0 ? -int_part : int_part;

    for (uint8_t j = int_precision; j > 0; j--) {
        int divisor = 1;
        for (uint8_t k = 1; k < j; k++)
            divisor *= 10;
        text_buf[++i] = '0' + (abs_int_part / divisor) % 10;
    }
    // text_buf[++i] = '.';

    // Fractional part
    float frac = var - (float)int_part;
    if (frac < 0)
        frac = -frac;
    for (uint8_t j = 1; j <= float_precision; j++) {
        frac *= 10;
        text_buf[++i] = '0' + ((int)frac % 10);
    }
    text_buf[++i] = '\0';
}

void powrms_int_to_str(int var, char* text_buf)
{
    uint8_t i = -1;
    if (!text_buf)
        return;

#if INTEGER_PRECISION == 4
    text_buf[++i] = '0' + (uint16_t)(var / 1000) % 10;
#endif
#if INTEGER_PRECISION == 3
    text_buf[++i] = '0' + (uint16_t)(var / 100) % 10;
#endif
#if INTEGER_PRECISION == 2
    text_buf[++i] = '0' + (uint16_t)(var / 10) % 10;
#endif
#if INTEGER_PRECISION == 1
    text_buf[++i] = '0' + (uint16_t)(var / 1) % 10;
    text_buf[++i] = '\0';
#endif
}


int update_adc_data()
{
    int ret = 0;

// 	ret = ad7091r5_read_one(adc_desc, ADC_VRMSF_CHANNEL_NO,
// 				&(adc_data_input.adc_vrmsf));
// 	if (ret) {
// #ifdef POWRMS_UART_DEBUG
// 		printf("Failed to read ADC channel %d\n\r", ADC_VRMSF_CHANNEL_NO);
// #endif
// 		return ret;
// 	}

// 	adc_data_input.adc_vrmsf = (adc_data_input.adc_vrmsf * ADC_V_REF) /
// 				   ADC_COUNTER_MAX;

// 	ret = ad7091r5_read_one(adc_desc, ADC_VRMSR_CHANNEL_NO,
// 				&(adc_data_input.adc_vrmsr));
// 	if (ret) {
// #ifdef POWRMS_UART_DEBUG
// 		printf("Failed to read ADC channel %d\n\r", ADC_VRMSR_CHANNEL_NO);
// #endif
// 		return ret;
// 	}

// 	adc_data_input.adc_vrmsr = (adc_data_input.adc_vrmsr * ADC_V_REF) /
// 				   ADC_COUNTER_MAX;

// #ifdef POWRMS_UART_DEBUG
// 	printf("#");
// #endif
    return 0;
}