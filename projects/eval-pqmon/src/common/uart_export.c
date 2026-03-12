/*******************************************************************************
 *   @file   uart_export.c
 *   @brief  UART2 CSV export for PQM harmonics & metrology data
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
 *******************************************************************************/

#include "uart_export.h"
#include "common_data.h"
#include "pqlib_example.h"
#include "pqlib_convert.h"
#include "no_os_uart.h"
#include <stdio.h>
#include <string.h>

#define UART_EXPORT_BUF_SIZE 6144

static struct no_os_uart_desc *uart_export_desc;
static char uart_export_buf[UART_EXPORT_BUF_SIZE];

int uart_export_init(void)
{
	return no_os_uart_init(&uart_export_desc, &uart_export_ip);
}

int uart_export_send(void)
{
	int off = 0;
	int rem = UART_EXPORT_BUF_SIZE;
	int n;
	int i;
	ADI_PQLIB_OUTPUT *out;
	EXAMPLE_CONFIG *cfg;
	float vscale, iscale;

	if (pqlibExample.state != PQLIB_STATE_RUNNING)
		return 0;

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

	/* Event counts: dip, swell, rvc, intrp */
	n = snprintf(uart_export_buf + off, rem, ",%u,%u,%u,%u",
		     (unsigned)out->events.dipCount,
		     (unsigned)out->events.swellCount,
		     (unsigned)out->events.rvcCount,
		     (unsigned)out->events.intrpCount);
	off += n;
	rem -= n;

	/* Line terminator */
	n = snprintf(uart_export_buf + off, rem, "\r\n");
	off += n;

	return no_os_uart_write(uart_export_desc, (uint8_t *)uart_export_buf, off);
}

void uart_export_remove(void)
{
	if (uart_export_desc) {
		no_os_uart_remove(uart_export_desc);
		uart_export_desc = NULL;
	}
}
