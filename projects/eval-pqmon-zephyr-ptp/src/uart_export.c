/********************************************************************************
 *   @file   uart_export.c
 *   @brief  UART2 CSV data export implementation
 *   @author Radu Etz (radu.etz@analog.com)
 ********************************************************************************
 * Copyright (c) 2026 Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#include "uart_export.h"
#include "pqlib_example.h"
#include "pqlib_convert.h"
#include "iio_pqm.h"
#include "pqm.h"

#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

LOG_MODULE_REGISTER(uart_export, LOG_LEVEL_INF);

static const struct device *uart2_dev;

#define UART_EXPORT_BUF_SIZE 6144
static char uart_export_buf[UART_EXPORT_BUF_SIZE];

static void uart_send_str(const struct device *dev, const char *str,
			  int len)
{
	for (int i = 0; i < len; i++) {
		uart_poll_out(dev, str[i]);
	}
}

int uart_export_init(void)
{
	uart2_dev = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(uart2));
	if (!uart2_dev) {
		LOG_WRN("UART2 not found in device tree — CSV export disabled");
		return -ENODEV;
	}
	if (!device_is_ready(uart2_dev)) {
		LOG_WRN("UART2 not ready — CSV export disabled");
		uart2_dev = NULL;
		return -ENODEV;
	}

	LOG_INF("UART2 CSV export initialized");
	return 0;
}

void uart_export_send(void)
{
	int off = 0;
	int rem = UART_EXPORT_BUF_SIZE;
	int n;
	int i;
	ADI_PQLIB_OUTPUT *out;
	EXAMPLE_CONFIG *cfg;
	float vscale, iscale;

	if (!uart2_dev)
		return;

	if (pqlibExample.state != PQLIB_STATE_RUNNING)
		return;

	out = pqlibExample.output;
	cfg = &pqlibExample.exampleConfig;
	vscale = cfg->voltageScale;
	iscale = cfg->currentScale;

	/* HEADER */
	n = snprintf(uart_export_buf + off, rem, "PQM");
	off += n;
	rem -= n;

	/* RMS: VA, VB, VC */
	for (i = 0; i < 3; i++) {
		n = snprintf(uart_export_buf + off, rem, ",%.2f",
			     convert_rms_type(out->params1012Cycles.voltageParams[i].mag,
					      vscale));
		off += n;
		rem -= n;
	}

	/* RMS: IA, IB, IC */
	for (i = 0; i < 3; i++) {
		n = snprintf(uart_export_buf + off, rem, ",%.2f",
			     convert_rms_type(out->params1012Cycles.currentParams[i].mag,
					      iscale));
		off += n;
		rem -= n;
	}

	/* RMS: IN (neutral current, index 3) */
	n = snprintf(uart_export_buf + off, rem, ",%.2f",
		     convert_rms_type(out->params1012Cycles.currentParams[3].mag,
				      iscale));
	off += n;
	rem -= n;

	/* THD: VA, VB, VC */
	for (i = 0; i < 3; i++) {
		n = snprintf(uart_export_buf + off, rem, ",%.2f",
			     (float)out->params1012Cycles.voltageParams[i].thd / 100.0f);
		off += n;
		rem -= n;
	}

	/* THD: IA, IB, IC */
	for (i = 0; i < 3; i++) {
		n = snprintf(uart_export_buf + off, rem, ",%.2f",
			     (float)out->params1012Cycles.currentParams[i].thd / 100.0f);
		off += n;
		rem -= n;
	}

	/* Angles: ANGL_VA_VB, ANGL_VA_VC, ANGL_IA_IB, ANGL_IA_IC */
	n = snprintf(uart_export_buf + off, rem, ",%u,%u,%u,%u",
		     (unsigned)convert_angle_type(pqlibExample.inputCycle.ANGL_VA_VB),
		     (unsigned)convert_angle_type(pqlibExample.inputCycle.ANGL_VA_VC),
		     (unsigned)convert_angle_type(pqlibExample.inputCycle.ANGL_IA_IB),
		     (unsigned)convert_angle_type(pqlibExample.inputCycle.ANGL_IA_IC));
	off += n;
	rem -= n;

	/* Voltage harmonics H2..H50 for VA, VB, VC (49 values each) */
	for (i = 0; i < 3; i++) {
		for (int h = 0; h < PQLIB_MAX_HARMONICS - 1; h++) {
			n = snprintf(uart_export_buf + off, rem, ",%.2f",
				     convert_pct_type(
					     out->params1012Cycles.voltageParams[i].harmonics[h]));
			off += n;
			rem -= n;
		}
	}

	/* Current harmonics H2..H50 for IA, IB, IC (49 values each) */
	for (i = 0; i < 3; i++) {
		for (int h = 0; h < PQLIB_MAX_HARMONICS - 1; h++) {
			n = snprintf(uart_export_buf + off, rem, ",%.2f",
				     convert_pct_type(
					     out->params1012Cycles.currentParams[i].harmonics[h]));
			off += n;
			rem -= n;
		}
	}

	/* Symmetrical components (only if enabled) */
#if ADI_PQLIB_CFG_DISABLE_SYMM_COMP == 0
	n = snprintf(uart_export_buf + off, rem,
		     ",%.2f,%.2f,%.2f,%.2f",
		     convert_pct_type(out->params1012Cycles.voltageUnb.negUnbRatio),
		     convert_pct_type(out->params1012Cycles.voltageUnb.zeroUnbRatio),
		     convert_pct_type(out->params1012Cycles.currentUnb.negUnbRatio),
		     convert_pct_type(out->params1012Cycles.currentUnb.zeroUnbRatio));
	off += n;
	rem -= n;

	n = snprintf(uart_export_buf + off, rem,
		     ",%.4f,%.4f,%.4f,%.4f,%.4f,%.4f",
		     convert_fract_type(out->params1012Cycles.voltageUnb.posSeqMag),
		     convert_fract_type(out->params1012Cycles.voltageUnb.negSeqMag),
		     convert_fract_type(out->params1012Cycles.voltageUnb.zeroSeqMag),
		     convert_fract_type(out->params1012Cycles.currentUnb.posSeqMag),
		     convert_fract_type(out->params1012Cycles.currentUnb.negSeqMag),
		     convert_fract_type(out->params1012Cycles.currentUnb.zeroSeqMag));
	off += n;
	rem -= n;
#endif

	/* Event counts: dip, swell, rvc, intrp (accumulated window) */
	n = snprintf(uart_export_buf + off, rem, ",%u,%u,%u,%u",
		     (unsigned)accumulated_events.dipCount,
		     (unsigned)accumulated_events.swellCount,
		     (unsigned)accumulated_events.rvcCount,
		     (unsigned)accumulated_events.intrpCount);
	off += n;
	rem -= n;

	/* Event details: most recent event's startTime, endTime,
	 * durationInCycles for each type (dip, swell, intrp, rvc).
	 * Uses accumulated_events which persists across GetOutput clears. */
	{
		uint8_t dc = accumulated_events.dipCount;
		uint8_t sc = accumulated_events.swellCount;
		uint8_t ic = accumulated_events.intrpCount;
		uint8_t rc = accumulated_events.rvcCount;

		/* Dip: startTime, endTime, durationInCycles */
		n = snprintf(uart_export_buf + off, rem,
			     ",%" PRId64 ",%" PRId64 ",%u",
			     dc ? accumulated_events.dips[dc - 1].startTime : (int64_t)0,
			     dc ? accumulated_events.dips[dc - 1].endTime : (int64_t)0,
			     dc ? (unsigned)accumulated_events.dips[dc - 1].durationInCycles : 0u);
		off += n;
		rem -= n;

		/* Swell: startTime, endTime, durationInCycles */
		n = snprintf(uart_export_buf + off, rem,
			     ",%" PRId64 ",%" PRId64 ",%u",
			     sc ? accumulated_events.swells[sc - 1].startTime : (int64_t)0,
			     sc ? accumulated_events.swells[sc - 1].endTime : (int64_t)0,
			     sc ? (unsigned)accumulated_events.swells[sc - 1].durationInCycles : 0u);
		off += n;
		rem -= n;

		/* Interruption: startTime, endTime, durationInCycles */
		n = snprintf(uart_export_buf + off, rem,
			     ",%" PRId64 ",%" PRId64 ",%u",
			     ic ? accumulated_events.intrp[ic - 1].startTime : (int64_t)0,
			     ic ? accumulated_events.intrp[ic - 1].endTime : (int64_t)0,
			     ic ? (unsigned)accumulated_events.intrp[ic - 1].durationInCycles : 0u);
		off += n;
		rem -= n;

		/* RVC: startTime, endTime, durationInCycles */
		n = snprintf(uart_export_buf + off, rem,
			     ",%" PRId64 ",%" PRId64 ",%u",
			     rc ? accumulated_events.rvc[rc - 1].startTime : (int64_t)0,
			     rc ? accumulated_events.rvc[rc - 1].endTime : (int64_t)0,
			     rc ? (unsigned)accumulated_events.rvc[rc - 1].durationInCycles : 0u);
		off += n;
		rem -= n;

		/* Lifetime total event counts (since power-on) */
		n = snprintf(uart_export_buf + off, rem, ",%u,%u,%u,%u",
			     (unsigned)total_dip_count,
			     (unsigned)total_swell_count,
			     (unsigned)total_intrp_count,
			     (unsigned)total_rvc_count);
		off += n;
		rem -= n;
	}

	/* Power, energy, and power factor per phase (A, B, C) */
	{
		POWER_ENERGY_DATA *pe = &pqlibExample.powerEnergy;
		float pscale = vscale * iscale;

		/* Active Power: AP_A, AP_B, AP_C */
		for (i = 0; i < 3; i++) {
			n = snprintf(uart_export_buf + off, rem, ",%.4f",
				     convert_power_type(pe->activePower[i], pscale));
			off += n;
			rem -= n;
		}
		/* Active Energy: AE_A, AE_B, AE_C */
		for (i = 0; i < 3; i++) {
			n = snprintf(uart_export_buf + off, rem, ",%.6f",
				     convert_energy_type(pe->activeEnergyHi[i], pscale));
			off += n;
			rem -= n;
		}
		/* Reactive Energy: RE_A, RE_B, RE_C */
		for (i = 0; i < 3; i++) {
			n = snprintf(uart_export_buf + off, rem, ",%.6f",
				     convert_energy_type(pe->reactiveEnergyHi[i], pscale));
			off += n;
			rem -= n;
		}
		/* Power Factor: PF_A, PF_B, PF_C */
		for (i = 0; i < 3; i++) {
			n = snprintf(uart_export_buf + off, rem, ",%.4f",
				     compute_power_factor(pe->activeEnergyHi[i],
							  pe->reactiveEnergyHi[i]));
			off += n;
			rem -= n;
		}
		/* Fundamental Active Power: FAP_A, FAP_B, FAP_C */
		for (i = 0; i < 3; i++) {
			n = snprintf(uart_export_buf + off, rem, ",%.4f",
				     convert_power_type(pe->fundActivePower[i], pscale));
			off += n;
			rem -= n;
		}
		/* Fundamental Active Energy: FAE_A, FAE_B, FAE_C */
		for (i = 0; i < 3; i++) {
			n = snprintf(uart_export_buf + off, rem, ",%.6f",
				     convert_energy_type(pe->fundActiveEnergyHi[i], pscale));
			off += n;
			rem -= n;
		}
		/* Fundamental Reactive Energy: FRE_A, FRE_B, FRE_C */
		for (i = 0; i < 3; i++) {
			n = snprintf(uart_export_buf + off, rem, ",%.6f",
				     convert_energy_type(pe->fundReactiveEnergyHi[i], pscale));
			off += n;
			rem -= n;
		}
		/* Displacement Power Factor: DPF_A, DPF_B, DPF_C */
		for (i = 0; i < 3; i++) {
			n = snprintf(uart_export_buf + off, rem, ",%.4f",
				     compute_power_factor(pe->fundActiveEnergyHi[i],
							  pe->fundReactiveEnergyHi[i]));
			off += n;
			rem -= n;
		}
	}

	/* Line terminator */
	n = snprintf(uart_export_buf + off, rem, "\r\n");
	off += n;

	uart_send_str(uart2_dev, uart_export_buf, off);
}
