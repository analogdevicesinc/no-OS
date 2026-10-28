/*******************************************************************************
 *   @file   pqlib_afe.c
 *   @brief  Analog frontend data polling process
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include "pqlib_afe.h"
#include "iio_pqm.h"
#include "no_os_irq.h"
#include "no_os_gpio.h"
#include "maxim_irq.h"
#include "common_data.h"
#include "parameters.h"
#include "no_os_delay.h"
#include "afe_queue.h"
#include "max32650.h"
#include <string.h>

/* STATUS0 bits that drive the AFE IRQ (MASK0) */
#define AFE_IRQ_SOURCES		(BITM_STATUS0_RMSONERDY | \
				 BITM_STATUS0_COH_PAGE_RDY | \
				 BITM_STATUS0_RMS1012RDY | \
				 BITM_STATUS0_EGYRDY)

/* Max STATUS0 re-reads per ISR call */
#define AFE_ISR_MAX_PASSES	4

static struct no_os_irq_ctrl_desc *afe_irq_desc = NULL;
static struct no_os_gpio_desc *afe_irq_gpio = NULL;
static bool afe_irq_ready;

/*
 * Waveform time: the ISR takes one system tick [us] per STATUS0 pass.
 * Cycle items carry it in their (otherwise unused) timestamp field, pages
 * in pageTick[], a FIFO in step with the page pool. After each processed
 * cycle, PQLib time (end of that cycle) minus its tick gives the offset
 * that turns a page tick into PQLib (UTC) time.
 */
static uint64_t isrTickUs;
static uint64_t pageTick[AFE_Q_PAGE_DEPTH];
static volatile uint8_t pageTickWr, pageTickRd;
static uint64_t cycleTickUs;
static int64_t tickToTimeUs;
static bool tickToTimeValid;
static uint32_t anchorBlock;
static ADI_PQLIB_TIME anchorMs;


static uint64_t tick_us(void)
{
	struct no_os_time t = no_os_get_time();

	return (uint64_t)t.s * 1000000 + t.us;
}

/* Called after each waveform block written to the IIO buffer */
static void waveform_anchor(uint64_t tick)
{
	anchorBlock = pqm_desc_global->waveform_block_count;
	anchorMs = tickToTimeValid ?
		   (ADI_PQLIB_TIME)(((int64_t)tick + tickToTimeUs) / 1000) : 0;
}

#ifdef PQM_EVENT_DEBUG
/* Time spent in the AFE ISR [us], read and reset by the rate print */
volatile uint32_t afeIsrBusyUs;
#endif

/*
 * Write one waveform block to the IIO buffer. If the host is not reading
 * (e.g. Scopy between single captures, buffer still enabled), drop the
 * oldest block instead of overrunning: with IIO_IGNORE_BUFF_OVERRUN_ERR the
 * reader does not recover from an overrun, it stays behind the writer and
 * the host gets about one block per second.
 */
static void waveform_cb_push(uint8_t *data, uint32_t len)
{
	struct no_os_circular_buffer *cb = pqlibExample.no_os_cb_desc;
	uint32_t used = 0, skip, n;
	void *p;

	if (no_os_cb_size(cb, &used)) {
		/* Overrun already happened: restart empty */
		cb->read.idx = cb->write.idx;
		cb->read.spin_count = cb->write.spin_count;
		used = 0;
	}

	for (skip = used + len > cb->size ? len : 0; skip; skip -= n) {
		if (no_os_cb_prepare_async_read(cb, skip, &p, &n) || !n)
			break;
		no_os_cb_end_async_read(cb);
	}

	no_os_cb_write(cb, data, len);
}

static void waveform_cb_write_and_oneshot(uint8_t *data, uint16_t len)
{
	waveform_cb_push(data, len);
	pqm_desc_global->waveform_block_count++;

	if (pqm_desc_global->waveform_capture_mode ==
	    WAVEFORM_CAPTURE_ONESHOT) {
		if (pqm_desc_global->oneshot_blocks_remaining > 0)
			pqm_desc_global->oneshot_blocks_remaining--;
		if (pqm_desc_global->oneshot_blocks_remaining == 0) {
			pqm_desc_global->waveform_streaming_active = false;
			pqm_desc_global->waveform_capture_mode =
				WAVEFORM_CAPTURE_DISABLED;
			pqm_desc_global->oneshot_running = false;
		}
	}
}

/**
 * @brief Read the one-cycle registers into a queue slot.
 * @param status0 - STATUS0 value that reported RMSONERDY
 * @return 0 on success
 */
static int afe_isr_read_cycle(uint32_t status0)
{
	AFE_Q_CYCLE_ITEM *c = afe_q_cycle_slot();
	int status;

	if (!c) {
		afe_q_drop(AFE_Q_CYCLE);
		return 0;
	}

	c->STATUS0 = status0;
	c->timestamp = (ADI_PQLIB_TIME)isrTickUs;
	status = afe_read_rms_one((uint32_t *)&c->AVRMSONE, 3);
	if (status == 0)
		status = afe_read_angle((uint16_t *)&c->ANGL_VA_VB, 3,
					ANGLE_VOLTAGE);
	if (status == 0)
		status = afe_read_angle((uint16_t *)&c->ANGL_IA_IB, 3,
					ANGLE_CURRENT);
	if (status == 0)
		status = afe_read_period((uint32_t *)&c->PERIOD, 1);
	if (status == 0)
		status = afe_read_status_1((uint32_t *)&c->STATUS1);
	if (status == 0)
		afe_q_publish(AFE_Q_CYCLE);

	return status;
}

static int afe_isr_read_page(uint32_t status0)
{
	int16_t *page = afe_q_page_slot();
	int status;

	if (!page) {
		afe_q_drop(AFE_Q_PAGE);
		return 0;
	}

	status = afe_read_waveform((uint16_t *)page, AFE_Q_PAGE_SAMPLES);
	if (status == 0) {
		pageTick[pageTickWr++ % AFE_Q_PAGE_DEPTH] = isrTickUs;
		afe_q_publish(AFE_Q_PAGE);
	}

	return status;
}

static int afe_isr_read_1012(uint32_t status0)
{
	AFE_Q_1012_ITEM *r = afe_q_1012_slot();
	int status;

	if (!r) {
		afe_q_drop(AFE_Q_1012);
		return 0;
	}

	status = afe_read_rms_1012((uint32_t *)&r->AIRMS1012, 7);
	if (status == 0)
		afe_q_publish(AFE_Q_1012);

	return status;
}

static int afe_isr_read_energy(uint32_t status0)
{
	POWER_ENERGY_DATA *e = afe_q_energy_slot();
	int status;

	if (!e) {
		afe_q_drop(AFE_Q_ENERGY);
		return 0;
	}

	status = afe_read_power_energy(e);
	if (status == 0)
		afe_q_publish(AFE_Q_ENERGY);

	return status;
}

/**
 * @brief Read every ready AFE item into the queues. Each STATUS0 bit is
 * cleared right after its data has been read. STATUS0 is read again until
 * no IRQ source is left, so IRQ0 goes high and the next item gives a new
 * falling edge. Runs in the AFE ISR, or from the main loop with the IRQ
 * disabled.
 */
/* ISR readers, in the order they are served */
static const struct {
	uint32_t bit;
	int (*read)(uint32_t status0);
} afe_isr_readers[] = {
	/* One-cycle data first: a 10/12 window ends with its last cycle */
	{ BITM_STATUS0_RMSONERDY, afe_isr_read_cycle },
	{ BITM_STATUS0_COH_PAGE_RDY, afe_isr_read_page },
	{ BITM_STATUS0_RMS1012RDY, afe_isr_read_1012 },
	{ BITM_STATUS0_EGYRDY, afe_isr_read_energy },
};

static void afe_isr_service(void)
{
	uint32_t status0;
	int status;

	for (int pass = 0; pass < AFE_ISR_MAX_PASSES; pass++) {
		if (afe_read_status0(&status0)) {
			afeQueueStats.errors++;
			return;
		}
		if (!(status0 & AFE_IRQ_SOURCES))
			return;
		isrTickUs = tick_us();

		for (size_t i = 0; i < NO_OS_ARRAY_SIZE(afe_isr_readers); i++) {
			if (!(status0 & afe_isr_readers[i].bit))
				continue;
			status = afe_isr_readers[i].read(status0);
			if (status == 0)
				status = afe_clear_status0(afe_isr_readers[i].bit);
			if (status) {
				afeQueueStats.errors++;
				return;
			}
		}
	}
}

static void waveform_irq_handler(void *ctx)
{
#ifdef PQM_EVENT_DEBUG
	uint32_t t0 = pqm_debug_cycles();
#endif

	afe_isr_service();

#ifdef PQM_EVENT_DEBUG
	afeIsrBusyUs += pqm_debug_us(pqm_debug_cycles() - t0);
#endif
}

/**
 * @brief Serve a missed IRQ edge: IRQ0 is still low but no ISR is coming
 * because the line never went high. Runs the ISR body with the IRQ off.
 */
static void afe_irq_kick(void)
{
	uint8_t level = NO_OS_GPIO_HIGH;

	if (!afe_irq_ready || !afe_irq_gpio)
		return;
	if (no_os_gpio_get_value(afe_irq_gpio, &level) ||
	    level != NO_OS_GPIO_LOW)
		return;

	afe_irq_suspend();
	afe_isr_service();
	afe_irq_resume();
	afeQueueStats.kicks++;
}

void afe_input_stop(void)
{
	afe_irq_suspend();
}

void afe_input_start(void)
{
	if (!afe_irq_ready)
		return;

	afe_q_flush();
	pageTickWr = 0;
	pageTickRd = 0;
	tickToTimeValid = false;
	afe_irq_resume();
}

void afe_time_cycle_done(ADI_PQLIB_TIME time)
{
	tickToTimeUs = (int64_t)time * 1000 - (int64_t)cycleTickUs;
	tickToTimeValid = true;
}

void afe_waveform_time_anchor(uint32_t *block, ADI_PQLIB_TIME *time)
{
	*block = anchorBlock;
	*time = anchorMs;
}

int waveform_irq_init(void)
{
	int status;
	struct no_os_irq_ctrl_desc *nvic_desc;
	uint32_t mask0;

	/* Level 1: Enable GPIO port 0 interrupt in NVIC */
	struct no_os_irq_init_param nvic_ip = {
		.platform_ops = &max_irq_ops,
	};

	status = no_os_irq_ctrl_init(&nvic_desc, &nvic_ip);
	if (status)
		return status;

	status = no_os_irq_set_priority(nvic_desc, GPIO0_IRQn, 1);
	if (status)
		return status;

	/* Level 2: Configure GPIO pin as input */
	status = no_os_gpio_get(&afe_irq_gpio, &intr_gpio_ip);
	if (status)
		return status;

	status = no_os_gpio_direction_input(afe_irq_gpio);
	if (status)
		return status;

	/* Level 2: GPIO IRQ controller - register callback on pin */
	status = no_os_irq_ctrl_init(&afe_irq_desc, &afe_callback_ctrl_ip);
	if (status)
		return status;

	afe0_callback_desc.callback = waveform_irq_handler;
	afe0_callback_desc.ctx = afe_irq_desc;

	status = no_os_irq_register_callback(afe_irq_desc, INTR_GPIO_PIN_NUM,
					     &afe0_callback_desc);
	if (status)
		return status;

	status = no_os_irq_trigger_level_set(afe_irq_desc, INTR_GPIO_PIN_NUM,
					     NO_OS_IRQ_EDGE_FALLING);
	if (status)
		return status;

	status = no_os_irq_enable(afe_irq_desc, INTR_GPIO_PIN_NUM);
	if (status)
		return status;

	/* IRQ0 reports every input the main loop needs */
	mask0 = BITM_MASK0_RMSONERDY | BITM_MASK0_COH_PAGE_RDY |
		BITM_MASK0_RMS1012RDY | BITM_MASK0_EGYRDY;
	status = afe_write_32bit_reg(REG_MASK0, &mask0);
	if (status)
		return status;

	afe_irq_ready = true;
	afe_input_start();

	return 0;
}

/**
 * @details Get AFE input: move the oldest queued AFE item into its PQLib
 * input structure. Waveform pages also go to the IIO buffer when the host
 * is streaming.
 */
int get_afe_input()
{
	ADI_PQLIB_CYCLE_INPUT *pOneCycle = &(pqlibExample.inputCycle);
	ADI_PQLIB_WAVEFORM *pWaveform = &(pqlibExample.inputWaveform);
	ADI_PQLIB_1012_CYCLE_INPUT *p1012Cycle = &(pqlibExample.input1012Cycles);
	const AFE_Q_CYCLE_ITEM *c;
	const AFE_Q_1012_ITEM *r;
	enum afe_q_type type;
	uint32_t seq;
	ADI_PQLIB_TIME ts;
	uint64_t tick;

	if (!afe_q_peek(&type)) {
		afe_irq_kick();
		return 0;
	}

	switch (type) {
	case AFE_Q_CYCLE:
		c = afe_q_cycle_front();
		cycleTickUs = (uint64_t)c->timestamp;
		seq = pOneCycle->sequenceNumber;
		ts = pOneCycle->timestamp;
		*pOneCycle = *c;
		pOneCycle->timestamp = ts;
		pOneCycle->sequenceNumber = seq + 1;
		pOneCycle->isDataProcessed = 0;
		break;
	case AFE_Q_PAGE:
		memcpy(pWaveform->waveform, afe_q_page_front(),
		       sizeof(pWaveform->waveform));
		pWaveform->isDataProcessed = 0;
		pWaveform->sequenceNumber++;
		tick = pageTick[pageTickRd++ % AFE_Q_PAGE_DEPTH];

		if (pqlibExample.no_os_cb_desc && pqm_desc_global &&
		    pqm_desc_global->waveform_streaming_active) {
			waveform_cb_write_and_oneshot(
				(uint8_t *)pWaveform->waveform,
				sizeof(pWaveform->waveform));
			waveform_anchor(tick);
		} else if (pqlibExample.no_os_cb_desc && !processData &&
			   pqm_desc_global && pqm_desc_global->active_ch) {
			/* Only while the host has the buffer enabled: in
			 * between, the buffer would fill with old pages */
			waveform_cb_push((uint8_t *)pWaveform->waveform,
					 sizeof(pWaveform->waveform));
			pqm_desc_global->waveform_block_count++;
			waveform_anchor(tick);
		}
		break;
	case AFE_Q_1012:
		r = afe_q_1012_front();
		seq = p1012Cycle->sequenceNumber;
		*p1012Cycle = *r;
		p1012Cycle->sequenceNumber = seq + 1;
		p1012Cycle->isDataProcessed = 0;
		break;
	case AFE_Q_ENERGY:
		pqlibExample.powerEnergy = *afe_q_energy_front();
		break;
	default:
		break;
	}
	afe_q_pop();

	return 0;
}

bool afe_input_pending(void)
{
	enum afe_q_type type;

	return afe_q_peek(&type);
}
