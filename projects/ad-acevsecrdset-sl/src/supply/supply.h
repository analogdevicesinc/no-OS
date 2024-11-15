/***************************************************************************//**
 *   @file   supply.h
 *   @brief  Power supply phase interface file.
 *   @author GMois (george.mois@analog.com), REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2023 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
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
