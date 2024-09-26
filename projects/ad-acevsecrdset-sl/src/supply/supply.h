/***************************************************************************//**
 *   @file   supply.h
 *   @brief  Power supply phase interface file.
 *   @author GMois (george.mois@analog.com), REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2023 Analog Devices, Inc.
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
#ifndef __SUPPLY_H__
#define __SUPPLY_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "state_machine.h"
#include "parameters.h"
#include "no_os_irq.h"

#define V1_MULT                 (1000000ul)
#define V1_DIVIDER              (1328233ul)
#define V2_DIVIDER              (1996007ul)

/*! Initial values for the rms filters */
#define INITIAL 4096
#define SAMPLES 4096

/*! Get zero crossing flag value */
int get_zero_cross_flag_state(void);

/*! Reset zero crossing flag value */
void reset_zero_cross_flag_state(void);

/*! Power supply phase init */
int supply_init(struct ade9113_dev **device);

/*! Scale V1 value */
int32_t supply_scale_v1(int32_t val);

/*! Scale V2 value */
int32_t supply_scale_v2(int32_t val);

/*! Calculate v1 rms */
int64_t rms_filter_v1(int32_t sample);

/*! Calculate v2 rms */
int64_t rms_filter_v2(int32_t sample);

/*! Calculate i rms */
int64_t rms_filter_i(int32_t sample);

/*! Update the rms and adc values */
int rms_adc_values_read(struct stout *stout, struct rms_adc_values *rms);

#endif /* __SUPPLY_H__ */
