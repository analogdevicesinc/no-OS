/*
 * uart_export.c — UART2 CSV data export
 *
 * Zephyr adaptation of no-OS eval-pqmon UART export.
 * Outputs PQ measurements as CSV at 1Hz over UART2.
 */

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

#if IS_ENABLED(CONFIG_WAVEFORM_UART_EXPORT)
#include "waveform_buffer.h"
#endif

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

	/* When waveform binary export is active, it owns UART2 exclusively.
	 * CSV data is still available via IIO attributes. */
#if IS_ENABLED(CONFIG_WAVEFORM_UART_EXPORT)
	return;
#endif

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

#if IS_ENABLED(CONFIG_WAVEFORM_UART_EXPORT)

/* Binary waveform frame format (all little-endian):
 *   SYNC[2]:  0x57 0x46 ("WF")
 *   SEQ[4]:   uint32_t block sequence number
 *   SEC[4]:   uint32_t PTP timestamp seconds
 *   NSEC[4]:  uint32_t PTP timestamp nanoseconds
 *   MASK[1]:  channel bitmask (bits 0-6 = IA,VA,IB,VB,IC,VC,IN)
 *   NSAMP[2]: uint16_t samples per channel (256)
 *   DATA[N]:  int16_t samples, interleaved per sample
 *
 * Channel order matches the ADE9430 WFB BURST_ALL_CHAN hardware order
 * (IA,VA,IB,VB,IC,VC,IN) — the raw burst is streamed without reordering.
 */

#define WF_SYNC_0        0x57  /* 'W' */
#define WF_SYNC_1        0x46  /* 'F' */
#define WF_HEADER_SIZE   17    /* SYNC(2) + SEQ(4) + SEC(4) + NSEC(4) + MASK(1) + NSAMP(2) */

#define WF_EXPORT_STACK_SIZE  4096
/* PREEMPT(8): higher priority than the IIO server thread (PREEMPT 10) so a
 * concurrent IIO/libiio client cannot starve the UART drain. Lower priority
 * than the afe_sync listener (PREEMPT 5) and all COOP threads (AFE COOP(2),
 * 1PPS/RTC-sync COOP(0)), so it never delays AFE polling, the sync handshake,
 * or PTP timestamp capture. (Lower number = higher priority in Zephyr.) */
#define WF_EXPORT_PRIORITY    K_PRIO_PREEMPT(8)

static K_THREAD_STACK_DEFINE(wf_export_stack, WF_EXPORT_STACK_SIZE);
static struct k_thread wf_export_thread_data;

static int16_t wf_export_block[WF_BLOCK_SAMPLES];

static void uart_send_bytes(const struct device *dev, const uint8_t *buf,
			    int len)
{
	for (int i = 0; i < len; i++) {
		uart_poll_out(dev, buf[i]);
	}
}

static void wf_export_thread_fn(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	uint8_t header[WF_HEADER_SIZE];
	uint32_t seq;
	struct wf_timestamp ts;
	uint8_t chan_mask = CONFIG_WAVEFORM_CHANNEL_MASK;
	uint16_t nsamp = WF_SAMPLES_PER_CH;
	int num_active_ch;
	int active_ch[WF_NUM_CHANNELS];

	LOG_INF("Waveform UART export thread started (mask=0x%02x)", chan_mask);

	while (1) {
		int ret = waveform_buffer_get(wf_export_block, &seq, &ts);

		if (ret != 0) {
			continue;
		}
		if (!uart2_dev) {
			continue;
		}

		/* Determine active channels from mask */
		num_active_ch = 0;
		for (int ch = 0; ch < WF_NUM_CHANNELS; ch++) {
			if (chan_mask & (1 << ch)) {
				active_ch[num_active_ch++] = ch;
			}
		}
		if (num_active_ch == 0) {
			continue;
		}

		/* Build header */
		header[0] = WF_SYNC_0;
		header[1] = WF_SYNC_1;
		header[2] = (uint8_t)(seq & 0xFF);
		header[3] = (uint8_t)((seq >> 8) & 0xFF);
		header[4] = (uint8_t)((seq >> 16) & 0xFF);
		header[5] = (uint8_t)((seq >> 24) & 0xFF);
		header[6] = (uint8_t)(ts.sec & 0xFF);
		header[7] = (uint8_t)((ts.sec >> 8) & 0xFF);
		header[8] = (uint8_t)((ts.sec >> 16) & 0xFF);
		header[9] = (uint8_t)((ts.sec >> 24) & 0xFF);
		header[10] = (uint8_t)(ts.nsec & 0xFF);
		header[11] = (uint8_t)((ts.nsec >> 8) & 0xFF);
		header[12] = (uint8_t)((ts.nsec >> 16) & 0xFF);
		header[13] = (uint8_t)((ts.nsec >> 24) & 0xFF);
		header[14] = chan_mask;
		header[15] = (uint8_t)(nsamp & 0xFF);
		header[16] = (uint8_t)((nsamp >> 8) & 0xFF);

		/* Send header */
		uart_send_bytes(uart2_dev, header, WF_HEADER_SIZE);

		/* Send sample data — interleaved, only active channels.
		 * Source data is interleaved as:
		 *   sample[0]*7+ch0, sample[0]*7+ch1, ..., sample[255]*7+ch6 */
		for (int s = 0; s < WF_SAMPLES_PER_CH; s++) {
			for (int c = 0; c < num_active_ch; c++) {
				int ch = active_ch[c];
				int16_t val = wf_export_block[s * WF_NUM_CHANNELS + ch];

				uart_poll_out(uart2_dev, (uint8_t)(val & 0xFF));
				uart_poll_out(uart2_dev, (uint8_t)((val >> 8) & 0xFF));
			}
		}
	}
}

void waveform_uart_export_init(void)
{
	waveform_buffer_init();

	k_thread_create(&wf_export_thread_data, wf_export_stack,
			K_THREAD_STACK_SIZEOF(wf_export_stack),
			wf_export_thread_fn, NULL, NULL, NULL,
			WF_EXPORT_PRIORITY, 0, K_NO_WAIT);
	k_thread_name_set(&wf_export_thread_data, "wf_uart");

	LOG_INF("Waveform UART export initialized");
}

#endif /* CONFIG_WAVEFORM_UART_EXPORT */
