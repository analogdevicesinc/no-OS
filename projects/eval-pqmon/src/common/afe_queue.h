/*******************************************************************************
 *   @file   afe_queue.h
 *   @brief  ISR to main loop queues for ADE9430 input data
 *   @author Radu Etz (radu.etz@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __AFE_QUEUE_H__
#define __AFE_QUEUE_H__

#include <stdbool.h>
#include <stdint.h>
#include "adi_pqlib.h"
#include "pqlib_example.h"

/* Pool depths - must be powers of two */
#define AFE_Q_CYCLE_DEPTH	16
#define AFE_Q_PAGE_DEPTH	4
#define AFE_Q_1012_DEPTH	2
#define AFE_Q_ORDER_DEPTH	32

#define AFE_Q_PAGE_SAMPLES	(ADI_PQLIB_WAVEFORM_BLOCK_SIZE * \
				 ADI_PQLIB_TOTAL_WAVEFORM_CHANNELS)

/* Item types, in the order queue */
enum afe_q_type {
	AFE_Q_CYCLE,
	AFE_Q_PAGE,
	AFE_Q_1012,
	AFE_Q_ENERGY,
	AFE_Q_NUM_TYPES
};

/* Diagnostic counters */
struct afe_q_stats {
	/* Items dropped because a pool or the order queue was full */
	uint32_t overflow[AFE_Q_NUM_TYPES];
	/* Highest number of queued items seen in the order queue */
	uint32_t orderHighWater;
	/* SPI errors in the ISR */
	uint32_t errors;
	/* Main loop services of a missed IRQ edge */
	uint32_t kicks;
};

/* One-cycle input slot (only the AFE fields are used) */
typedef ADI_PQLIB_CYCLE_INPUT AFE_Q_CYCLE_ITEM;

/* 10/12 cycle input slot (only the AFE fields are used) */
typedef ADI_PQLIB_1012_CYCLE_INPUT AFE_Q_1012_ITEM;

extern struct afe_q_stats afeQueueStats;

/* Producer (ISR) side: get a free slot, fill it, then publish it */
AFE_Q_CYCLE_ITEM *afe_q_cycle_slot(void);
int16_t *afe_q_page_slot(void);
AFE_Q_1012_ITEM *afe_q_1012_slot(void);
POWER_ENERGY_DATA *afe_q_energy_slot(void);
void afe_q_publish(enum afe_q_type type);
void afe_q_drop(enum afe_q_type type);

/* Consumer (main loop) side */
bool afe_q_peek(enum afe_q_type *type);
const AFE_Q_CYCLE_ITEM *afe_q_cycle_front(void);
const int16_t *afe_q_page_front(void);
const AFE_Q_1012_ITEM *afe_q_1012_front(void);
const POWER_ENERGY_DATA *afe_q_energy_front(void);
void afe_q_pop(void);

/* Discard all queued items. Call with the AFE IRQ disabled. */
void afe_q_flush(void);

#endif /* __AFE_QUEUE_H__ */
