/***************************************************************************//**
 *   @file   iio_powrms.h
 *   @brief  Header file for IIO POWRMS example
 *   @author robert.budai@analog.com
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
#ifndef __IIO_PWRMS_H__
#define __IIO_PWRMS_H__

#define INTC_DEVICE_ID              1

#define IIO_BUFF_TYPE               int32_t
#define TOTAL_PWM_CHANNELS          4
#define POWRMS_CIRC_BUFF_SIZE       256
#define MAX_SIZE_BASE_ADDR          (TOTAL_PWM_CHANNELS * POWRMS_CIRC_BUFF_SIZE)

#define BUFFER_VAL_PRECISSION       1000.0

#include "iio_app.h"
#include "no_os_uart.h"
#include "no_os_circular_buffer.h"

extern struct iio_device powrms_iio_descriptor;
extern IIO_BUFF_TYPE iio_data_buffer_loc[MAX_SIZE_BASE_ADDR];
extern struct no_os_circular_buffer *powrms_vin0_buffer;
extern struct no_os_circular_buffer *powrms_vin1_buffer;
extern struct no_os_circular_buffer *powrms_vrmsf_buffer;
extern struct no_os_circular_buffer *powrms_vrmsr_buffer;

int powrms_buffers_init(void);
void powrms_buffers_cleanup(void);
int powrms_push_vin0(float value);
int powrms_push_vin1(float value);
int powrms_push_vrmsf(float value);
int powrms_push_vrmsr(float value);
int powrms_get_latest_vin0(float *value);
int powrms_get_latest_vin1(float *value);
int powrms_get_latest_vrmsf(float *value);
int powrms_get_latest_vrmsr(float *value);

#endif