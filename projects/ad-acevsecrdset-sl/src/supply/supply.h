/***************************************************************************//**
 *   @file   supply.h
 *   @brief  Power supply phase interface file.
 *   @author GMois (george.mois@analog.com), REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2023 Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
