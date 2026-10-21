/*******************************************************************************
 *   @file   afe_queue.c
 *   @brief  ISR to main loop queues for ADE9430 input data
 *   @author Radu Etz (radu.etz@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

/*
 * Single producer (AFE ISR) / single consumer (main loop) queues.
 *
 * Each data type has its own pool of slots, consumed in FIFO order. The
 * order queue records the type of every published item, so the main loop
 * hands the items to PQLib in exactly the order the AFE produced them.
 *
 * Only the producer writes a head index and only the consumer writes a
 * tail index. Indexes run freely and are masked on use, so the queues need
 * no locking. A memory barrier makes the slot data visible before the
 * head index that publishes it.
 */

#include "afe_queue.h"
#include "max32650.h"

struct afe_q_ring {
	volatile uint32_t head;
	volatile uint32_t tail;
};

static AFE_Q_CYCLE_ITEM cyclePool[AFE_Q_CYCLE_DEPTH];
static int16_t pagePool[AFE_Q_PAGE_DEPTH][AFE_Q_PAGE_SAMPLES];
static AFE_Q_1012_ITEM rms1012Pool[AFE_Q_1012_DEPTH];
/* Energy is a once-per-second value, one slot is enough */
static POWER_ENERGY_DATA energyPool[1];

static uint8_t orderPool[AFE_Q_ORDER_DEPTH];

static struct afe_q_ring rings[AFE_Q_NUM_TYPES];
static struct afe_q_ring orderRing;

static const uint32_t depth[AFE_Q_NUM_TYPES] = {
	[AFE_Q_CYCLE] = AFE_Q_CYCLE_DEPTH,
	[AFE_Q_PAGE] = AFE_Q_PAGE_DEPTH,
	[AFE_Q_1012] = AFE_Q_1012_DEPTH,
	[AFE_Q_ENERGY] = 1,
};

struct afe_q_stats afeQueueStats;

static bool ring_full(const struct afe_q_ring *r, uint32_t size)
{
	return (r->head - r->tail) >= size;
}

static bool slot_free(enum afe_q_type type)
{
	return !ring_full(&rings[type], depth[type]) &&
	       !ring_full(&orderRing, AFE_Q_ORDER_DEPTH);
}

AFE_Q_CYCLE_ITEM *afe_q_cycle_slot(void)
{
	if (!slot_free(AFE_Q_CYCLE))
		return NULL;

	return &cyclePool[rings[AFE_Q_CYCLE].head & (AFE_Q_CYCLE_DEPTH - 1)];
}

int16_t *afe_q_page_slot(void)
{
	if (!slot_free(AFE_Q_PAGE))
		return NULL;

	return pagePool[rings[AFE_Q_PAGE].head & (AFE_Q_PAGE_DEPTH - 1)];
}

AFE_Q_1012_ITEM *afe_q_1012_slot(void)
{
	if (!slot_free(AFE_Q_1012))
		return NULL;

	return &rms1012Pool[rings[AFE_Q_1012].head & (AFE_Q_1012_DEPTH - 1)];
}

POWER_ENERGY_DATA *afe_q_energy_slot(void)
{
	if (!slot_free(AFE_Q_ENERGY))
		return NULL;

	return &energyPool[0];
}

void afe_q_publish(enum afe_q_type type)
{
	uint32_t queued;

	orderPool[orderRing.head & (AFE_Q_ORDER_DEPTH - 1)] = type;
	__DMB();
	rings[type].head++;
	orderRing.head++;

	queued = orderRing.head - orderRing.tail;
	if (queued > afeQueueStats.orderHighWater)
		afeQueueStats.orderHighWater = queued;
}

void afe_q_drop(enum afe_q_type type)
{
	afeQueueStats.overflow[type]++;
}

bool afe_q_peek(enum afe_q_type *type)
{
	if (orderRing.head == orderRing.tail)
		return false;

	__DMB();
	*type = orderPool[orderRing.tail & (AFE_Q_ORDER_DEPTH - 1)];

	return true;
}

const AFE_Q_CYCLE_ITEM *afe_q_cycle_front(void)
{
	return &cyclePool[rings[AFE_Q_CYCLE].tail & (AFE_Q_CYCLE_DEPTH - 1)];
}

const int16_t *afe_q_page_front(void)
{
	return pagePool[rings[AFE_Q_PAGE].tail & (AFE_Q_PAGE_DEPTH - 1)];
}

const AFE_Q_1012_ITEM *afe_q_1012_front(void)
{
	return &rms1012Pool[rings[AFE_Q_1012].tail & (AFE_Q_1012_DEPTH - 1)];
}

const POWER_ENERGY_DATA *afe_q_energy_front(void)
{
	return &energyPool[0];
}

void afe_q_pop(void)
{
	enum afe_q_type type;

	if (!afe_q_peek(&type))
		return;

	__DMB();
	rings[type].tail++;
	orderRing.tail++;
}

void afe_q_flush(void)
{
	for (int i = 0; i < AFE_Q_NUM_TYPES; i++)
		rings[i].tail = rings[i].head;
	orderRing.tail = orderRing.head;
}
