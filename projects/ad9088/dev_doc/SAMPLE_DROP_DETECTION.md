# Detecting dropped samples in a captured tone

Status: **analysis and prototype only — nothing implemented.** Written 2026-08-11
for later reference, after a question about whether sample drops can be *measured*
rather than eyeballed.

The short answer is yes, two independent ways: a signal-domain metric computed from
the capture buffer alone, and hardware latches that already exist in the AXI/JESD204
chain but are never read. Both are specified below, with the numbers that back them.

---

## 1. What the existing statistics already tell you

`no_os_tone_coherence()` returns coherence, amplitude and envelope spread. Between
them they already *react* to dropped samples — they just do not quantify them.
Measured at a tone of Fs/256, N = 16384, amplitude 11583, 16-bit full scale.

### Scattered drops to floor level

Coherence is **linear in the surviving fraction**, and independent of where the
drops land:

```
coherence ~= 1000 * (surviving fraction)
```

Zeroed samples contribute nothing to either the correlation or the total power, so
both scale by the same factor and the ratio lands exactly on the survival rate.

| dropped | coherence | amplitude | spread |
|---|---|---|---|
| 0 | 999 | 11582 | 0 |
| 1 sample (0.006%) | 999 | 11582 | **1000** |
| 0.1% | 998 | 11577 | **1000** |
| 1% | 989 | 11524 | **1000** |
| 5% | 951 | 11296 | **1000** |
| 10% | 905 | 11022 | **1000** |
| 25% | 777 | 10213 | **1000** |

**Envelope spread is the real detector here.** It pins to full scale on a *single*
dropped sample in 16384 — it is a min/max extreme statistic, so it does not care
how rare the event is. Drops landing at a ±60 LSB noise floor rather than exact zero
give 997-999, effectively the same.

Note what this means for the gates: at `NO_OS_TONE_COHERENCE_PASS 900`, a **10%
dropout rate passes on coherence alone**. Spread is the only thing catching it,
which is why it belongs in the verdict.

### One drop, then a phase slip

Two segments with a phase discontinuity between them add in quadrature. For a break
a fraction `f` through the capture and a 90° slip after it:

```
coherence ~= 1000 * (f^2 + (1-f)^2)
```

A V, floored at **500 mid-capture**, recovering toward 1000 at either edge:

| break at | coherence | spread (with hole) | spread (compacted) |
|---|---|---|---|
| 1% | 976 | 1000 | 0 |
| 5% | 900 | 1000 | 0 |
| 10% | 815 | 1000 | 0 |
| **50%** | **497** | 1000 | 0 |
| 75% | 625 | 1000 | 0 |
| 99% | 983 | 1000 | 0 |

Slip magnitude at mid-capture: 30° → 932 (**passes**), 45° → 853, 90° → 499,
180° → 0.

Amplitude is nearly useless for this fault — 11560 vs 11582 for a 64-sample hole, a
0.2% dip, because it is an RMS over the whole capture.

### The blind spot

**A compacted phase slip in the first or last few percent of the capture passes
clean.** Coherence reads 976-983, and if the FIFO dropped samples without leaving a
hole (contiguous buffer, samples simply missing) spread reads **0**. Neither
statistic sees it. This is the specific gap the metrics in §2 close.

### Telling the two faults apart

| | scattered drops | single slip |
|---|---|---|
| coherence | tracks surviving fraction | V-shaped, floors at ~500 |
| spread | **1000, always** | 1000 with a hole, **0** if compacted |
| amplitude | falls in proportion | essentially unchanged |

---

## 2. Proposed signal-domain metrics

Additive fields on the result struct — existing callers keep compiling:

```c
struct no_os_tone_result {
	uint32_t coherence;
	uint32_t amplitude;
	uint32_t spread;
	uint32_t dropouts;    /* samples whose envelope collapsed */
	uint32_t slips;       /* phase discontinuities between valid samples */
	uint32_t first_slip;  /* index of the first, or NO_OS_TONE_NO_SLIP */
	uint32_t longest_gap; /* longest run of consecutive dropouts */
};
```

Two caller-tunable fields on `struct no_os_tone_layout` (it already travels to both
entry points, so no new parameter is needed). Both fall back to a default when zero,
so existing `{num_conv, conv_i, conv_q}` initialisers keep working:

```c
	uint8_t  dropout_shift;  /* envelope below mean >> shift is a dropout */
	uint16_t slip_sin_tol;   /* sin(tolerance angle) in per-mille */
```

```c
#define NO_OS_TONE_DROPOUT_SHIFT	2	/* -6 dB, half amplitude */
#define NO_OS_TONE_SLIP_SIN_TOL		247	/* 0.25 rad, ~14 deg */
#define NO_OS_TONE_NO_SLIP		UINT32_MAX
```

### The slip test, integer only

No trig at runtime and no floats: it reuses the private Q15 table and
`no_os_sqrt64()` already in the module, and the caller supplies `sin(tolerance)` as
a constant.

```
idx = (step * gap) >> SIN_TABLE_PHASE_SHIFT   /* expected advance over the gap */
w   = z[prev] rotated by idx                  /* table lookup, >> SIN_TABLE_SHIFT */
crs = Q[i]*w_re - I[i]*w_im                   /* |z||w| sin(residual) */
dot = I[i]*w_re + Q[i]*w_im                   /* |z||w| cos(residual) */

slip if  |crs| * 1000 > |z[i]| * |w| * slip_sin_tol   OR   dot < 0
```

Overflow bounds: `|crs| * 1000` peaks near 2.1e12 and `m1 * m2 * sin_tol` near
1.1e12, both comfortably inside `uint64_t`.

### Two passes, not a buffered envelope array

The dropout threshold needs the mean envelope, which is only known once the whole
buffer has been visited. Keeping `e[]` would cost 128 KB for a 16384-sample capture
— unacceptable in a portable module. So:

- **Pass 1** — the existing loop, arithmetically **untouched**, so coherence,
  amplitude and spread stay bit-identical.
- **Pass 2** — recompute `I² + Q²` on the fly (two multiplies per sample), compare
  against `mean >> dropout_shift`, count dropouts and the longest run, and run the
  slip test between successive valid samples.

### Instrumentation, not gates

These fields should **not** join the pass/fail verdict. Making dropouts fatal would
change the verdict of captures that pass today, which is a behaviour change
disguised as instrumentation. Callers can gate on the fields themselves.

Suggested report line, printed only when non-zero so a clean log is unchanged:

```
  dropouts 164 (longest run 3)  slips 1 at sample 8192
```

---

## 3. Three traps — each was a real bug in the prototype

The most valuable part of this document. All three were found by measurement, and
any implementation must handle them.

**1. A zeroed sample makes an `atan2`-style test read as "no error".**
Both the dot and cross products go to zero, and `atan2(0, 0)` returns 0, which
happened to fall just inside tolerance. Observed directly:

```
    n      I      Q     |z|          dot          crs   d(rad)
 8191  11580   -284   11583    134130332      3291844   -0.000
 8192      0      0       0            0            0   -0.025  <-- degenerate
```

The envelope gate must run **first**, and the phase comparison must skip invalid
samples.

**2. Skipping is not enough — a slip behind a hole goes uncounted.**
Comparing only *adjacent* valid samples means the pair straddling a hole is never
examined, so a discontinuity on the far side is invisible. Measured: with the naive
gate, a hole of 1 / 8 / 64 samples all reported `slips 0` despite a real 90° slip.
The fix is to carry the last valid sample forward and **extrapolate the expected
phase across the gap** — that is what the `* gap` term in §2 is for.

**3. A sine-only residual test reports zero for a 180° slip.**
`sin(180°) = 0`, so the magnitude test alone cannot distinguish δ from 180° − δ.
Measured: slips of 15/30/45/90° detected, **180° missed**. The `dot < 0` term fixes
it, and with it every magnitude from 15° to 180° is caught.

---

## 4. Measured validation

Integer-only prototype, tone at Fs/256, N = 16384, amplitude 11583. **Reproduce
these before trusting any port.**

### Drop counting is exact

Indices were drawn with replacement, so "unique" is the true number of distinct
samples corrupted — the metric matches it, not the request count:

| injected | unique | dropouts | slips | longest_gap |
|---|---|---|---|---|
| 1 | 1 | **1** | 0 | 1 |
| 5 | 5 | **5** | 0 | 1 |
| 16 | 16 | **16** | 0 | 1 |
| 164 | 164 | **164** | 0 | 1 |
| 819 | 802 | **802** | 0 | 3 |

Exact at ±60 and ±300 LSB added noise as well.

### Slips are caught regardless of hole size, and located

True break at sample 8192:

| hole | dropouts | slips | first_slip |
|---|---|---|---|
| 0 | 0 | 1 | 8192 |
| 1 | 1 | 1 | 8193 |
| 8 | 8 | 1 | 8200 |
| 64 | 64 | 1 | 8256 |

Swept break position — 0.1%, 1%, 10%, 50%, 99% — all report `slips 1` with
`first_slip` matching the true index exactly, **including the edge positions that
coherence and spread both miss.**

Both faults together (50 drops + a mid-capture 90° slip) resolve independently:
`dropouts 50, slips 1, first_slip 8192`.

### Sensitivity and false-positive envelope

Detection floor at 0.25 rad: **15° detected, 10° not.**

| noise ±LSB | SNR | false slips (0.25 rad) | false slips (0.5 rad) |
|---|---|---|---|
| 300 | 36 dB | 0 | 0 |
| 1000 | 26 dB | 0 | 0 |
| 1400 | 23 dB | 45 | 0 |
| 2500 | 18 dB | 2842 | 11 |
| 3000 | 16 dB | 4190 | 144 |

So 0.25 rad is clean down to about **26 dB SNR**; 0.5 rad extends that to ~20 dB at
the cost of missing slips below ~30°. For reference the AD9088 cabled loopback
measured 2220 LSB against a 57 LSB floor — roughly **32 dB**, inside the clean
region with margin.

---

## 5. Hardware cross-check inventory

The signal metrics say *how many samples are corrupt and where*. Hardware latches
say *whether the link caused it*. These are project- and platform-specific, so they
belong in an example rather than in `util/`.

The usable pattern for all of them: **clear the latch before
`axi_dmac_transfer_start()`, read it after the capture** — the delta then covers
exactly that capture window.

| Mechanism | Location | State today |
|---|---|---|
| `ADXCVR_BUFSTATUS_UNDERFLOW` BIT(5) / `ADXCVR_BUFSTATUS_OVERFLOW` BIT(6) of `ADXCVR_REG_STATUS` 0x0014 | `drivers/axi_core/jesd204/axi_adxcvr.c:51-54` | Sticky, resettable via `ADXCVR_BUFSTATUS_RST` (`:49`). Read **only at bring-up**, in `adxcvr_clk_enable()` `:486-514`, gated on pcore >= 17.5a. The truest elastic-buffer-slip detector in the chain. |
| `JESD204_RX_REG_LANE_ERRORS(x)` = `x*32 + 0x308` | `axi_jesd204_rx.c:82`; getter `axi_jesd204_rx_get_lane_errors()` `:288` | Cumulative per-lane error counter. Getter exists but is **not declared in the header**, and on a healthy lane the counter is never read at all. `axi_jesd204_rx_status_read()` does not touch it. |
| Apollo JRX 204C IRQ latches: `DATA_RDY_LOST` 0x10, `RM_FIFO_EMPTY` 0x20, `RM_FIFO_FULL` 0x40, `LANE_FIFO_EMPTY` 0x80, `LANE_FIFO_FULL` 0x100, `INVALID_SAMPLE` 0x200 | `drivers/rf-transceiver/apollo/public/inc/adi_apollo_jrx_types.h:42-51` (`adi_apollo_jrx_j204c_irq_e`, `:41-53`); APIs `adi_apollo_jrx.h:364`, `:394`, `:408` | Sticky W1C. **`ad9088.c` never calls `j204c_irq_get/_clear/_enable_set`.** Named almost exactly for this purpose — the closest thing to a device-side dropped-sample flag. |
| `AXI_ADC_OVER_RANGE` BIT(0) of `AXI_ADC_REG_CHAN_STATUS(c)`; `AXI_ADC_MUX_OVER_RANGE` BIT(1) of `AXI_ADC_REG_STATUS` | `drivers/axi_core/axi_adc_core/axi_adc_core.h:97` and `:73` | Sticky W1C, per channel and all-channel OR. **Zero readers.** Caution: `axi_adc_pn_mon()` writes 0xff to clear, which would also wipe over-range evidence. |
| `AXI_DMAC_REG_TRANSFER_DONE` 0x428, `AXI_DMAC_REG_TRANSFER_ID` 0x404 | `drivers/axi_core/axi_dmac/axi_dmac.h:67` and `:56` | **Zero readers.** The driver trusts its own software `transfer_done` bool instead of the hardware bitmap. |
| `DMA_PARTIAL_REPORTING_EN` | `axi_dmac.h:84` | Set by two projects, then **dropped** — `spi_engine_offload_init()` inspects only `DMA_CYCLIC`, and `axi_dmac_transfer_start()` never writes bit 2. The corresponding partial-length/ID registers (0x44C/0x450 in the HDL map) are not even defined here. Partial reporting is precisely the mechanism that says "this capture ended short". |
| `xilinx_xcvr_prbs_err_cnt_get()` | `drivers/axi_core/jesd204/xilinx_transceiver.c:2132` | Fully implemented, declared, **zero callers**. Per-lane bit-error counting if the transceiver is put in PRBS mode. |
| SYSREF alignment error, BIT(1) of `JESD204_RX_REG_SYSREF_STATUS` 0x108 | `axi_jesd204_rx.c:65` | Sticky W1C, printed at `:271-272` but never acted on programmatically. |

### Two pre-existing bugs found while surveying

Unrelated to drop detection — recorded so they are not rediscovered from scratch.

**`axi_dmac.c:534` compares `IRQ_PENDING` for exact equality**, not with a mask:

```c
while (reg_val != (AXI_DMAC_IRQ_SOT | AXI_DMAC_IRQ_EOT)) {
```

If the HDL ever asserts a third pending bit, this loop spins to timeout and reports
a generic `"Error transferring data using DMA."`. The ISRs read `IRQ_PENDING` and
write it straight back, so any unhandled status bit is cleared and discarded without
being examined.

**`axi_jesd204_rx.c:459` prints an uninitialised buffer.** `error_str` (`:439`) is
only filled inside the `PCORE_VERSION_MINOR(jesd->version) >= 2` branch at `:454`,
but is printed unconditionally at `:459`.

---

## 6. If this gets implemented

Files that would change:

| File | Change |
|---|---|
| `include/no_os_tone.h` | 4 result fields, 2 layout fields, 3 defines, docs |
| `util/no_os_tone.c` | second pass in `no_os_tone_coherence()`; one report line |
| `projects/ad9088/src/examples/dma/dma_example.c` | nothing required — additive |

Verification bar:

1. **Coherence, amplitude and spread must stay bit-identical.** Capture
   `tone_harness.c` and `tone_report_preview.c` output before touching anything and
   diff after. 22/22 checks, 5 MHz probe still exactly 76.
2. Re-run the §4 matrix and confirm every number.
3. Regression the three traps in §3 explicitly — each was a real bug.
4. Clean compile as C and C++; target build with zero warnings; astyle `Unchanged`
   on the second pass; all lines within 80 columns.
5. Confirm an existing 3-field layout initialiser still compiles.

Related: `COHERENCE_MATH.md` for the estimator derivation, and
`projects/ad9088/tmp/tone_harness.c` for the math harness.
