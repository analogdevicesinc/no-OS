/*******************************************************************************
 *   @file   iio_pqm.h
 *   @brief  IIO pqm interfacing header file
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/
#ifndef __IIO_PQM_H__
#define __IIO_PQM_H__

#include "common_data.h"
#include "pqlib_afe.h"
#include "pqlib_convert.h"
#include "pqlib_example.h"
#include "afe_calibration.h"
#include "status.h"
#include <inttypes.h>

// Device specific attributes
#define NEG_UNB_VOLTAGE_RATIO           0
#define ZERO_UNB_VOLTAGE_RATIO          1
#define SNEG_VOLTAGE                    2
#define SPOS_VOLTAGE                    3
#define SZRO_VOLTAGE                    4
#define NEG_UNB_CURRENT_RATIO           5
#define ZERO_UNB_CURRENT_RATIO          6
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
#define SNEG_VOLTAGE_ANGLE              34
#define SPOS_VOLTAGE_ANGLE              35
#define SZRO_VOLTAGE_ANGLE              36
#define SNEG_CURRENT_ANGLE              37
#define SPOS_CURRENT_ANGLE              38
#define SZRO_CURRENT_ANGLE              39

// Calibration attributes
#define CAL_TYPE                        40
#define CAL_TYPE_AVAILABLE              41
#define CAL_STATUS                      42
#define CAL_PHASE                       43
#define CAL_NOMINAL_CURRENT             44
#define CAL_NOMINAL_VOLTAGE             45
#define CAL_OFFSET_CURRENT              46
#define CAL_OFFSET_VOLTAGE              47
#define CAL_PHASE_AVAILABLE             48
#define CAL_START                       49
// Gain calibration errors
#define CAL_GAIN_I_ERROR_BEFORE         50
#define CAL_GAIN_V_ERROR_BEFORE         51
#define CAL_GAIN_I_ERROR_AFTER          52
#define CAL_GAIN_V_ERROR_AFTER          53
// Offset calibration errors
#define CAL_OFFSET_I_ERROR_BEFORE       54
#define CAL_OFFSET_V_ERROR_BEFORE       55
#define CAL_OFFSET_I_ERROR_AFTER        56
#define CAL_OFFSET_V_ERROR_AFTER        57

// Flash storage attributes
#define FLASH_CAL_SAVE                  58
#define FLASH_CAL_LOAD                  59
#define FLASH_CAL_ERASE                 60
#define FLASH_STAT                      61
#define FLASH_CAL_DATA                  62

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
#define CHAN_EVENT_COUNT                13
#define CHAN_EVENT_START_TIME           14
#define CHAN_EVENT_END_TIME             15
#define CHAN_EVENT_DURATION_IN_CYCL     16
#define CHAN_EVENT_MIN_MAG              17
#define CHAN_EVENT_MAX_MAG              18
#define CHAN_EVENT_DELTA_U_MAX          19
#define CHAN_EVENT_DELTA_U_SS           20
#define CHAN_VOLTAGE_MAGNITUDE1012      21
#define CHAN_VOLTAGE_MAX_MAGNITUDE      22

#define RESAMPLED_WAVEFORM_FULL_SCALE   18196

extern struct iio_device pqm_iio_descriptor;
extern volatile bool configChanged;
extern volatile bool processData;

#endif
