/*******************************************************************************
 *   @file   iio_pqm.h
 *   @brief  IIO pqm interfacing header file
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __IIO_PQM_H__
#define __IIO_PQM_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "common_data.h"
#include "pqlib_afe.h"
#include "pqlib_convert.h"
#include "pqlib_example.h"
#include "status.h"
#include <inttypes.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

// Device specific attributes
#define ATTR_U2                         0
#define ATTR_U0                         1
#define SNEG_VOLTAGE                    2
#define SPOS_VOLTAGE                    3
#define SZRO_VOLTAGE                    4
#define ATTR_I2                         5
#define ATTR_I0                         6
#define SNEG_CURRENT                    7
#define SPOS_CURRENT                    8
#define SZRO_CURRENT                    9
#define NOMINAL_VOLTAGE                 10
#define VOLTAGE_SCALE                   11
#define CURRENT_SCALE                   12
#define I_CONSEL_ENABLE                 13
#define DIP_THRESHOLD                   14
#define DIP_HYSTERESIS                  15
#define SWELL_THRESHOLD                 16
#define SWELL_HYSTERESIS                17
#define INTERP_THRESHOLD                18
#define INTERP_HYSTERESIS               19
#define RVC_THRESHOLD                   20
#define RVC_HYSTERESIS                  21
#define MSV_CARRIER_FREQUENCY           22
#define MSV_RECORDING_LENGTH            23
#define MSV_THRESHOLD                   24
#define SAMPLING_FREQUENCY              25
#define V_CONSEL                        26
#define V_CONSEL_AVAILABLE              27
#define FLICKER_MODEL                   28
#define FLICKER_MODEL_AVAILABLE         29
#define NOMINAL_FREQUENCY               30
#define NOMINAL_FREQUENCY_AVAILABLE     31
#define PROCESS_DATA                    32
#define FW_VERSION_NR                   33

// Channel specific attributes
#define CHAN_RMS                        0
#define CHAN_ANGLE                      1
#define CHAN_HARMONICS                  2
#define CHAN_INTER_HARMONICS            3
#define CHAN_SCALE                      4
#define CHAN_OFFSET                     5
#define CHAN_THD                        6
#define CHAN_RAW                        7
#define CHAN_VOLTAGE_UNDER_DEV          8
#define CHAN_VOLTAGE_OVER_DEV           9
#define CHAN_VOLTAGE_PINST              10
#define CHAN_VOLTAGE_PST                11
#define CHAN_VOLTAGE_PLT                12


#define RESAMPLED_WAVEFORM_FULL_SCALE   18196

extern struct iio_device pqm_iio_descriptor;
extern volatile bool configChanged;
extern volatile bool processData;

#endif