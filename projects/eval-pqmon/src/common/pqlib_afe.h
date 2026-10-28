/*******************************************************************************
 *   @file   pqlib_afe.h
 *   @brief  Analog frontend data polling header file
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#ifndef __PQLIB_AFE_H__
#define __PQLIB_AFE_H__

#include "ade9430.h"
#include "adi_pqlib.h"
#include "afe_config.h"
#include "pqlib_example.h"
#include "status.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#endif

/** @brief angle for voltage*/
#define ANGLE_VOLTAGE 0
/** @brief angle for current*/
#define ANGLE_CURRENT 1

/**
 * @details Get AFE input
 * @return 0 on Success
 *         1 on Failure
 */
int get_afe_input();

/**
 * @details Initialize the AFE IRQ on ADE9430 IRQ0 (P0.30). The ISR reads
 *          one-cycle data, waveform pages, 10/12 cycle RMS and energy into
 *          queues, so no input is lost while PQLib is busy.
 * @return 0 on Success
 */
int waveform_irq_init(void);

/**
 * @details Stop reading AFE input (AFE IRQ off). Use while the AFE is
 *          restarted or calibrated.
 */
void afe_input_stop(void);

/**
 * @details Discard queued input and restart reading AFE input.
 */
void afe_input_start(void);

/**
 * @details Note the PQLib time of the end of the cycle just processed.
 *          Waveform block times are derived from it.
 * @param time - adi_pqlib_GetTime() right after adi_pqlib_ProcessOneCycle()
 */
void afe_time_cycle_done(ADI_PQLIB_TIME time);

/**
 * @details Latest waveform block written to the IIO buffer and the PQLib
 *          (UTC) time at which the AFE finished it.
 * @param block - waveform_block_count of that block (1 = first block after
 *                the buffer was enabled)
 * @param time - ms, 0 if PQLib time is not known yet
 */
void afe_waveform_time_anchor(uint32_t *block, ADI_PQLIB_TIME *time);

/**
 * @details Check if AFE input is waiting in the queues.
 * @return true if get_afe_input() has an item to deliver
 */
bool afe_input_pending(void);

#endif /* __PQLIB_AFE_H__ */
