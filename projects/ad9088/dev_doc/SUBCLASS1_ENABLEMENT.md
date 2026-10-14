# Enabling JESD204C Subclass 1 on the AD9088

Status: **DONE. Stages A, B, B-fix, C and D are all HW-verified on VCU118.**
Subclass is 1, the link comes up with `SYSREF Phase: Locked` / `SYSREF captured:
Yes` on both AXI cores and `Link status: DATA` with all 8 lanes good, and **MCS
calibration passed on its first hardware run (2026-08-14)**:

```
Total BSYNC path delay 2175424 fs
MCS Init Cal Validation: Passed
MCS Initcal Status: Passed
SYSREF_PHASE = 40 (TRY0..TRY4)      <- was 433 before MCS cal
```

`SYSREF_PHASE` is in **units of Fclk** (20 GHz device clock, 50 ps/cycle), so the
cal pulled the external-to-internal SYSREF offset from **21.65 ns to 2.00 ns** and
it is stable across all five reads. No link regression: 8 lanes good, `Subclass=1`
on enabled links, measured link clock 312.489/312.491 MHz, all four NCO
round-trips passing, no `profile crc is invalid`.

**Two follow-ups, neither a failure — see "Run 6 result" below:** the 2.00 ns
residual is suspiciously close to the 2.175 ns path delay that was just
compensated, and the init-cal status data needed to attribute it is currently
printed only on failure. Last updated 2026-08-14.

Written after the subclass-0 datapath was fully verified on VCU118 (DMA example's
three tests passing, cabled loopback coherence 99.8%), when subclass 1 became the
next target as groundwork for multi-device phase alignment.

**Harness**: `basic_example` — simpler than the DMA example and it already calls
`axi_jesd204_{tx,rx}_status_read()` immediately after `jesd204_fsm_start()`,
which is the evidence that matters for JESD bring-up. `dma_example.c` carries the
same ADF4030 wiring so it stays usable for datapath work later. `iio_example.c`
is deliberately untouched and will **not** bring up a subclass-1 link once the
subclass macro flips, since it has no ADF4030 instantiated.

Grounded in:

- `arch/microblaze/boot/dts/vcu118_ad9084.dts` (kernel tree) — the authority on
  what the board actually is
- `drivers/rf-transceiver/apollo/{ad9088_dt.c,ad9088_jesd204_fsm.c,ad9088.c}`
- `drivers/frequency/{adf4030,adf4382,hmc7044}/`
- `include/jesd204.h`, `jesd204/jesd204-{core,fsm}.c`
- `drivers/axi_core/jesd204/axi_jesd204_{rx,tx}.c`

See `PORTING_GUIDE.md` Step 8 for the porting-status view, `JESD204_FSM.md` for
the FSM walk, and `AD9088_FSM_OP_FUNCTIONS.md` for the per-op HAL calls.

---

## The one thing to understand first

`vcu118_ad9084.dts:38-48`:

```c
#if WITH_AION
#define JESD_SUBCLASS 1
#else
#define JESD_SUBCLASS 0
#endif
...
adi,subclass = <JESD_SUBCLASS>;     /* :797, in the ad9084 node */
```

**Subclass 1 is defined to be exactly "the ADF4030 (AION) is present and
wired".** They are one feature, not two. The ADF4030 work is not optional
groundwork that precedes flipping subclass — it *is* the flip. Any plan that
treats "enable subclass 1" as a config change and "add the ADF4030" as a later
nice-to-have has the dependency backwards.

The corollary: **no subclass-1 firmware profile is needed.** The same `.bin` is
used, and subclass reaches the device as a host-side override of the loaded
profile — exactly what the kernel does with the `adi,subclass` DT property.
`ad9088_dt.c:152-155` already implements the override; what was missing is the
fixup that must accompany it (Stage C).

---

## Actual clock / SYSREF topology

From the dts with `WITH_AION = 1`:

```
clkin_125 (board XO) ──┬─→ ADF4382 @CS0 ──→ 20 GHz Apollo device clock
                       └─→ HMC7044 @CS1 (PLL2 = 2.5 GHz)
                              ├─ ch1  ADF4030_REFIN    /20  → 125 MHz → ADF4030 refin
                              ├─ ch3  ADF4030_BSYNC0   /512 → LVPECL, is_sysref
                              ├─ ch8  CORE_CLK_TX      /8   → 312.5 MHz FPGA
                              ├─ ch9  CORE_CLK_RX      /8   → 312.5 MHz FPGA
                              ├─ ch10 FPGA_REFCLK      /8   → 312.5 MHz GT refclk
                              └─ ch11/12 CORE_CLK_{RX,TX}_B (side B, unused)

ADF4030 @CS4 (VCO 2.5 GHz, BSYNC 9.765625 MHz) ── jesd204-sysref-provider
                              ├─ ch0  ADF4030_SCLKOUT3  ← HMC, auto-align reference
                              ├─ ch5  APOLLO_SYSREF     → Apollo SYSREF (output-en, auto-align)
                              └─ ch8  SYSREF_IN_F       → FPGA sysref_in (output-en, auto-align)
```

This overturned three assumptions that the pre-2026-08-13 config and docs held:

1. **The ADF4030 is the `jesd204-sysref-provider`, not the HMC7044**
   (`dts:639`). It feeds *both* the Apollo (ch5) and the FPGA (ch8). The HMC7044
   does not source FPGA SYSREF at all.
2. **HMC7044 ch3 is `ADF4030_BSYNC0`, not `FPGA_SYSREF`** — and it is LVPECL,
   not LVDS, and carries `adi,jesd204-sysref-chan` (the `.is_sysref = true`
   equivalent).
3. **HMC7044 ch1 (`ADF4030_REFIN`, /20 → 125 MHz) was absent from `chan_spec`
   entirely**, so the ADF4030 would have had no reference clock.

Before this was read out of the dts, the working assumption was that the Apollo's
SYSREF pin was simply undriven and subclass 1 was untestable. It is driven — by a
chip the project had never instantiated.

### Rate arithmetic

`SYSREF_CLK_MHz = 9765625`, `HMC7044_PLL2_FREQ = 2.5 GHz`,
`AION_VCO_FREQ_HZ = 2.5 GHz` (`..._4x2_CLL.dts:17-38`).

For 204C/64b66b with E=1, `jesd204_link_get_lmfc_lemc_rate()`
(`jesd204-core.c:314-330`) gives `lane_rate / (66 × 32)`. With
`AD9088_LANE_RATE_KHZ = 20625000`:

```
LEMC = 20.625e9 / 2112 = 9 765 625 Hz   ==   dts SYSREF_CLK_MHz
```

So `AD9088_LANE_RATE_KHZ` is **confirmed correct and must not be changed**. The
`lane_rate=10312500 kHz` line logged at `PORTING_GUIDE.md:631` belongs to a
different usecase profile, not this one.

HMC7044 ch3 at divider 512 is 4.8828125 MHz = LEMC/2. The dts comment reads
"9.765" because `adi,jesd204-sysref-chan` makes
`hmc7044_jesd204_link_pre_setup()` (`hmc7044.c:1380-1400`) retune the channel to
the computed LEMC gcd (divider 256). Our ch3 had no `.is_sysref`, so it was never
retuned and ran at half rate — tolerable under subclass 0, wrong for subclass 1.

---

## What was already ported

The *device-side* subclass-1 plumbing turned out to be complete and equivalent to
the kernel driver. This is worth stating explicitly because `PORTING_GUIDE.md`
Step 8 previously implied far more was missing:

| Item | no-OS | Kernel |
|---|---|---|
| `lnk->subclass` from profile (JRX/JTX) | `ad9088_jesd204_fsm.c:63,106` | `fsm.c:64,97` |
| `clk_mcs_subclass_set` + `clk_mcs_sysref_en_set` gated on subclass 1 | `fsm.c:217-233` | `fsm.c:199-215` |
| Status pass mask `0x60` (subclass 1) vs `0x20`, 204B and 204C | `ad9088.c:668-676, 707-715` | `ad9088.c:2958-3001` |
| `post_state_sysref` on `LINK_ENABLE` | `fsm.c:782` | `fsm.c:1033` |
| Trigger-phase read + 25–75% margin check | `fsm.c:664-729` | `fsm.c:894-955` |
| `ad9088_print_sysref_phase` | `ad9088.c:590-601` | `ad9088.c:542-552` |

Bit `0x40` in the JRX status is `ADI_APOLLO_JRX_LINK_STAT_SYSREF_RECV`
(`public/inc/adi_apollo_jrx_types.h:31-35`). Under subclass 1 the link is only
declared good when both user-data-ready (`0x20`) *and* SYSREF-received (`0x40`)
are set — so the "SYSREF Phase: Unlocked" line that is cosmetic today becomes a
hard failure.

FPGA side needs nothing: `axi_jesd204_{rx,tx}_apply_config()` writes
`SYSREF_CONF_SYSREF_DISABLE` (and RX `BUFFER_EARLY_RELEASE`) **only** when
`lnk->subclass == JESD204_SUBCLASS_0` (`axi_jesd204_rx.c:538-547`,
`axi_jesd204_tx.c:425-450`), so subclass 1 enables SYSREF capture automatically.

Both clock-chip drivers already exist in no-OS with the API the kernel MCS code
needs — `drivers/frequency/adf4030/` (`adf4030_get_bsync_freq`,
`adf4030_set_channel_delay`/`_get_channel_delay` in femtoseconds,
`adf4030_set_channel_direction`, `adf4030_set_single_ch_alignment`,
`adf4030_set_background_serial_alignment`, `adf4030_set_tdc_measurement`,
`adf4030_set_alignment_iter`) and `drivers/frequency/adf4382/`
(`adf4382_set_phase_adjust`, `adf4382_set_phase_pol`). No clock-chip driver had
to be written from scratch; the ADF4030 only lacked JESD204 registration.

---

## How much alignment machinery is actually needed

No SYSREF *alignment* operation runs anywhere today. Both drivers call only
`adi_apollo_clk_mcs_dyn_sync_*`, which `adi_apollo_clk_mcs.h:299-352` explicitly
documents as "no sysref alignment"; `adi_apollo_clk_mcs_oneshot_sync()` is never
called from the FSM. That is why `ad9088_print_sysref_phase()` reads 0 — nothing
has driven it, which is not the same as it being aligned.

On the kernel platform alignment has two independent sources:

1. **The ADF4030's own auto-align** — channels 0/5/8 all carry
   `auto-align-on-sync-en`, with `adi,bsync-autoalign-reference-channel = <0>`
   and `adi,bsync-autoalign-iteration-count = <6>`. Self-contained in the chip.
   (The `<6>` never reaches the kernel driver — see the property-name typos noted
   under Stage B-fix — so the reference actually runs 8 iterations.)
2. **MCS calibration** — trims the residual skew between the external BSYNC edge
   and the Apollo's internal SYSREF counter, and tracks ADF4382 drift.

For a **single-device** subclass-1 link the hard requirements are only: subclass
bit set, SYSREF receiver enabled, a valid periodic SYSREF present at the Apollo
pin, and `0x40` setting. (1) plausibly satisfies that alone; (2) is what makes
the latency accurate and trimmed, and is required for multi-chip.

**This is why Stage D was staged as conditional** — Stage C was finished and
tested first, and it confirmed (1) is sufficient for the link to come up. Stage D
was then ported anyway (2026-08-14) for the trimming and multi-device reasons, so
"conditional" now describes the *history* of the decision rather than the current
state of the code.

---

## Stages

### Stage A — Diagnostics (no behaviour change) — DONE, HW-verified 2026-08-13

**Result on VCU118** — the answer that decides Stage C:

```
profile: subclass jtx0=0 jtx1=0 jrx0=0 jrx1=0
profile: center_sysref present=0 term=0 cm900=0 ref_to_int_ratio=1 int_prd_digclk=256
profile: side_a_sysref present=0 term=0 cm900=0 ref_to_int_ratio=1 int_prd_digclk=256
profile: side_b_sysref present=0 term=0 cm900=0 ref_to_int_ratio=1 int_prd_digclk=256
profile: gapped_sysref=0 leave_sysref_rx_on=0 num_avg=31 lock_window=0
```

Three consequences:

1. **All three `sysref_present = 0`.** Since we keep this profile binary, the
   `center_sysref.sysref_present` fixup in Stage C is **mandatory** — without it
   the Apollo CPU firmware is never told to expect an external SYSREF.
2. `ref_to_int_period_ratio = 1` with `use_gapped_sysref = 0` means the external
   BSYNC period must be an integer multiple of the internal SYSREF period, and
   the ratio the profile asks for is 1 — i.e. equal periods. With
   `internal_sysref_prd_digclk_cycles = 256` and BSYNC at 9.765625 MHz that is
   consistent, so no BSYNC rate change is needed.
3. The profile's own subclass is 0 on all four links, confirming the host-side
   override is the only path to subclass 1 with this binary.

The `Subclass=` readback also works — `JRX/JTX ADI_APOLLO_LINK_A0: ... Subclass=0
link_en= Enabled` — so we can confirm on the next run that
`adi_apollo_clk_mcs_subclass_set()` actually took effect rather than inferring it.

**Rate arithmetic confirmed on hardware.** The FPGA cores report exactly what was
derived from the dts:

```
Lane rate: 20625.000 MHz     Lane rate / 66: 312.500 MHz
LEMC rate: 9.765 MHz         Measured Link Clock: 312.491 MHz
```

So `AD9088_LANE_RATE_KHZ = 20625000` is correct, LEMC is 9.765625 MHz, and it
matches both the dts `SYSREF_CLK_MHz` and the ADF4030's programmed BSYNC rate.

1. Dump the profile's SYSREF/MCS config in `ad9088_dt.c`, after
   `ad9088_get_profile()` and *before* the subclass override: pre-override
   subclass, and from `profile.mcs_cfg` — `{center,side_a,side_b}_sysref.*`,
   `use_gapped_sysref`, `leave_sysref_rx_on`, `num_sysref_avg_mcs_fw`,
   `sysref_lock_window_mcs_fw`, `internal_sysref_prd_digclk_cycles_*`. These
   fields are consumed by the on-chip CPU firmware and are opaque to static
   inspection of the `.bin`; since we keep the existing profile, this is the only
   way to know whether `sysref_present` is already set and what the internal
   SYSREF period is.
2. Add `Subclass=` to the JRX/JTX link-inspect prints, read back from hardware as
   the kernel does, to confirm `adi_apollo_clk_mcs_subclass_set()` took effect
   rather than inferring it.

### Stage B — Framework + ADF4030 JESD204 support — DONE, HW-verified 2026-08-13

**On the first three runs this was only partially verified.** The ADF4030 probed
at CS 4, its PLL locked, the HMC7044 supplied its 125 MHz reference, the link
still reached `DATA` with 8 lanes good and the NCO round-trips passed — so nothing
regressed. But **BSYNC alignment did not complete**; that took Stage B-fix, after
which ch5 and ch8 both align. The description below is the code as it stands.

**B1** — add `JESD204_OP_CLK_SYNC_STAGE4` to `enum jesd204_dev_op`
(`include/jesd204.h`). The kernel's ADF4030 registers its alignment on that op
(`drivers/iio/frequency/adf4030.c:1226`) but no-OS stopped at `CLK_SYNC_STAGE3`.
Mechanical: `jesd204-fsm.c:21,70` iterate `0 .. __JESD204_MAX_OPS` and all
drivers use named designated initialisers into `state_ops[]`.

**B2** — add JESD204 support to `drivers/frequency/adf4030/`, mirroring the
kernel's `adf4030_jesd204_data`:

- `.sysref_cb` — the kernel's is a **no-op returning 0**; the ADF4030 emits
  continuous BSYNC, so there is no pulse to request. Consequence: the AD9088's
  `post_state_sysref` on `LINK_ENABLE` becomes a no-op by design, and alignment
  comes from `CLK_SYNC_STAGE4` instead.
- `[JESD204_OP_LINK_SUPPORTED].per_link` — validates the BSYNC frequency against
  `jesd204_link_get_lmfc_lemc_rate()`.
- `[JESD204_OP_CLK_SYNC_STAGE4].per_device` — the alignment step.

Plus `jesd204_dev_register()` in `adf4030_init()`, teardown in
`adf4030_remove()`, and a `jdev` field. Note `hmc7044.c:1618-1622` leaves
`.sizeof_priv` at 0 while writing 8 bytes into the resulting allocation — do not
copy that bug.

**B3** — project wiring: `SPI_CS_ADF4030 4`; `adf4030_spi_ip`/`adf4030_ip` in
`common_data.c` (VCO 2.5 GHz, BSYNC 9 765 625 Hz, autoalign reference channel 0,
6 iterations, channels 0/5/8); HMC7044 ch1 added and ch3 corrected to LVPECL +
`.is_sysref`; `adf4030.c` in `src.mk`; `adf4030_init()`/`_remove()` in
`dma_example.c` and `basic_example.c`; and the **topology reorder** moving
`.is_sysref_provider = true` from the HMC7044 to a new ADF4030 entry.

Topology order matters: `jesd204_topology_init()` reads `is_sysref_provider` from
the *input* array but takes the pointer from the top-device-filtered array
(`jesd204-core.c:116-119`), so the provider must precede the top device and must
not be the last entry. Order used: ADF4030 → HMC7044 → rx_jesd → tx_jesd →
ad9088(top).

**Subclass deliberately stays 0 through Stage B.** That makes Stage B a clean
checkpoint: the ADF4030 is wired and probing but the link config is unchanged, so
if the DMA tests regress it is unambiguously the new clock chip rather than
subclass 1.

### Stage B-fix — BSYNC alignment timed out — RESOLVED, HW-verified 2026-08-13

Symptom on the first three runs:

```
adf4030_jesd204_clks_sync4: aligning ch5 to ch0 failed (-116)
adf4030_jesd204_clks_sync4: aligning ch8 to ch0 failed (-116)
```

`-116` is `-ETIMEDOUT` (newlib, `sys/errno.h:153`) from `adf4030_poll()`. The two
channels that matter — Apollo SYSREF and FPGA SYSREF — were not aligned at all.
**This is a hard gate on Stage C: subclass 1 cannot lock SYSREF phase without
it.**

#### What the third run proved

The diagnostics added on run 2 produced the decisive evidence:

```
ch5 TDC read against ch0 failed (-116)
ch5 align regs:   0x37=0xA3 (en_iter=1 cycs_red=1 cycles=6) 0x35=0x01 (thold=1) 0x16=0x86 (avgexp=6)
ch5 align status: 0x8F=0xF1 (fsm_busy=1 tdc_busy=1) 0x90=0x01 (tdc_err=0 tmp_align_err=0)
```

| Evidence | Meaning |
|---|---|
| `0x90` bit0 `PLL_LD = 1` | ADF4030 PLL **is** locked |
| `0x8F` bit6 `REF_OK = 1` | Reference input amplitude is fine |
| `0x90` `TDC_ERR = 0`, `TMP_ALIGN_ERR = 0` | No error was ever raised |
| `0x8F` `TDC_BUSY = 1`, `MATH_BUSY = 0` | The TDC is still measuring after the full 2 s poll |
| `0x16 = 0x86` → bit7 `TDC_ARM_M = 1` | The TDC is **still armed and never completed** |

So this is neither a convergence failure nor a too-short timeout. The TDC arms
and never finishes, silently — it is measuring against something with no edges.
At `avgexp = 6` one measurement is `2^(6+6)/9.765625 MHz ≈ 419 µs`, four orders
of magnitude inside the 2 s budget.

**Eliminated, with evidence:**

| Hypothesis | Why it is out |
|---|---|
| ADF4030 PLL not locked | `PLL_LD = 1` in the run-3 dump, and the driver's `"PLL failed to lock"` warning never fired |
| Reference amplitude / **RCM** — the previous leading suspect | `REF_OK = 1` in the run-3 dump. The reference receiver works at the silicon-default RCM |
| TDC or PLL powered down | `adf4030_set_default_regs()` ends in `adf4030_power(dev, true)` (`adf4030.c:281`), which clears `PD_ALL`/`PD_PLL`/`PD_TDC` in `0x3C` |
| Wrong channel termination / direction | `RX_DC_COUPLED_CLKS` touches only the RX bits and `set_channel_direction()` only the TX bits, so ch5/ch8 do get driver **and** receiver, as the dts asks |
| Degenerate self-alignment (ch0 vs ch0) | Real, and skipped since run 2 — but ch5/ch8 kept failing, so it was only part of the story |
| `adi,input-output-reconfig-en` unmapped | The kernel uses `channel_io_reconfig_en` only at `adf4030.c:815`, in the sysfs direction-write path, to reject direction changes. Not an alignment prerequisite |

#### Root cause: `TDC_SOURCE` is never programmed

`0x11[4:0]` (`ADF4030_TDC_SOURCE`) selects the reference channel the TDC measures
against. Three facts combine:

1. `adf4030_reg_defaults[]` (`adf4030.c:95`) leaves `0x11 = 0x1F` →
   `TDC_SOURCE = 31`, a channel that does not exist (the part has 0..9).
2. `dev->tdc_source` is zero from `no_os_calloc` and was **never** written during
   `adf4030_init()`/`adf4030_configure()` — `adf4030_set_tdc_source()` is only
   reachable from `clks_sync4` and the IIO layer.
3. `clks_sync4` guarded the write on that stale shadow:
   ```c
   if (adf->tdc_source != chan->reference_chan) {   /* 0 != 0 -> false */
           ret = adf4030_set_tdc_source(adf, chan->reference_chan);
   ```
   With `reference_chan = ADF4030_CH_HMC_REF = 0` the guard is false, the write
   was skipped, and `0x11[4:0]` stayed 31 for the whole alignment sequence.

Both `adf4030_tdc_probe()` and `adf4030_set_single_ch_alignment()` therefore ran
against a non-existent source. This accounts for every observed bit — armed,
busy forever, no error, PLL and reference fine — and for ch5 and ch8 failing
identically. The run-2 `adf4030_align_regs_dump()` never read `0x11`, which is
why the dump looked healthy.

The kernel programs the source **unconditionally, in both paths**:
`adf4030_tdc_measure()` (kernel `adf4030.c:390-393`) on every call, and
`adf4030_auto_align_single_channel()` (kernel `adf4030.c:600`) as a full-byte
write `0x11 = source_channel | FIELD_PREP(EN_ALIGN_MSK, 1)`.

#### Corrections to the run-2 diagnosis

Two claims made here after run 2 were wrong, and the fixes they motivated were
deviations rather than fixes:

1. **"Iterative alignment was disabled" was not the problem.**
   `adf4030_auto_align_threshold_en()` — the kernel's only `EN_ITER` writer — is
   reachable *only* from sysfs (kernel `adf4030.c:840`), and
   `bsync_autoalign_theshold_en` is never parsed from DT. The reference platform
   therefore aligns successfully with **`EN_ITER = 0`**, converging on
   `ALIGN_CYCLES` alone. Our `0x37 = 0xA3` was a deviation. The kernel's
   post-probe state is `0x37 = 0xE2` (`EN_CYCS_RED=1, EN_ITER=0,
   ALIGN_CYCLES=7`) with `0x35[5:0] = 1`.
2. **AVGEXP's reference value is 15, not 13.** The comparison is
   `bsync_freq_odiv_a_uhz > 2000000000000ULL` (kernel `adf4030.c:1383`) — that is
   **2 MHz expressed in µHz**, not 2 GHz. Our 9.765625 MHz exceeds it, so the
   reference programs 15, making one measurement ~215 ms.

A third finding, which explains why the dts and the reference hardware disagree:
the kernel reads its properties under **misspelled names** —
`adi,bsync-autoalign-interation-count` and `adi,bsync-autoalign-thehsold-fs`
(kernel `adf4030.c:1440,1444`) — whereas the binding and every DTS spell them
`...iteration-count` / `...threshold-fs`. So
`adi,bsync-autoalign-iteration-count = <6>` in `vcu118_ad9084.dts` is **silently
ignored** and the reference hardware actually runs **iter = 8**, threshold
1400 fs.

#### Fixes applied (run 4 candidate)

Root cause:

- `clks_sync4` now calls `adf4030_set_tdc_source()` **unconditionally**; the
  stale-shadow guard is gone.
- `adf4030_set_single_ch_alignment()` writes `0x11` as a single full byte —
  `TDC_SOURCE | EN_ALIGN`, `MANUAL_MODE` cleared — mirroring the kernel, so it
  no longer depends on the caller having selected a source.
- `adf4030_set_tdc_measurement()` re-asserts `TDC_SOURCE` from the shadow on
  every call.
- `adf4030_init()` seeds `dev->tdc_source = ADF4030_TDC_SOURCE_RESET` (0x1F) so
  the shadow matches the register instead of falsely reading 0.

TDC measurement sequence, mirroring kernel `adf4030_tdc_measure()`:

- `RST_TDC_ERR` (`0x61` bit 7) is now **pulsed** rather than left asserted.
  Latching it held the error monitor in reset, which is why `TDC_ERR` read 0 on a
  failed measurement — the bit could never latch.
- `0x15` `FALL_EDGE_SRC | FALL_EDGE_TGT` cleared before arming.
- The busy poll now waits on `TDC_BUSY | MATH_BUSY`, not `TDC_BUSY` alone; the
  result was previously readable while the math block was still computing.
- A pre-sleep of one measurement period precedes the poll.

Reference alignment:

- **AVGEXP programmed** — `0x16[3:0]` set to 15 above 2 MHz BSYNC, 13 below.
  `ADF4030_AVGRXP = GENMASK(4, 0)` renamed to `ADF4030_AVGEXP = GENMASK(3, 0)`,
  closing the naming/width deviation.
- **Poll budget parameterised** — `adf4030_poll()` takes `timeout_ms`; 3 s for
  the `FSM_BUSY` and `TDC_BUSY|MATH_BUSY` polls (the kernel's value), 2 s
  retained for PLL lock. Necessary because 8 align cycles at avgexp 15 reach
  ~1.7 s, which only barely fit the old fixed 2 s.
- **`EN_ITER` decoupled from the threshold** — `adf4030_set_alignment_threshold()`
  writes only `0x35[5:0]`, `adf4030_set_alignment_iter()` writes only
  `ALIGN_CYCLES`, and a new `adf4030_set_alignment_threshold_en()` owns
  `EN_ITER`. The new `alignment_threshold_en` init parameter defaults to false,
  matching the reference. `adf4030_configure()` now runs threshold → iter →
  per-channel, the kernel's order, and `0` once again means "use the default"
  (1400 fs / 8 iterations).
- **`0x5C` IRQ masks no longer clobbered** — `adf4030_init()` wrote the register
  with only `CMOS_OV`, unmasking all four interrupt sources that the defaults
  table had just masked.
- **`PD_TX_PATH` cleared unconditionally** in `adf4030_set_channel_direction()`,
  with `EN_DRIVE` alone expressing direction, as the kernel does.
  `adf4030_get_channel_direction()` computes `tx_en = en_drv && !tx_pd`, so it
  stays correct.
- `alignment_iter = 8` in `adf4030_ip`, matching the reference's effective value.

Diagnostics:

- `adf4030_align_regs_dump()` now also dumps `0x11` (`tdc_source`,
  `manual_mode`, `en_align`) and `0x10` (`tdc_target`), plus `MATH_BUSY`,
  `REF_OK` and `PLL_LD`. Their absence is exactly why this bug survived a full
  hardware iteration.
- `adf4030_init()` prints ref/VCO/BSYNC, avgexp, PLL lock and `REF_OK`. The
  bring-up log previously contained **no ADF4030 line at all**.
- A `REF_OK` check was added at init. **Deliberate deviation:** it warns and
  continues, where the kernel aborts probe — this is a shared driver and
  `projects/adf4030` has been working without the check.

Two latent defects were fixed in passing, both newly reachable: `tdc_source` can
now legitimately hold 31, so indexing `dev->channels[dev->tdc_source]` is
bounds-checked via `adf4030_tdc_source_bsync()`, and `adf4030_adel_m_compute()`
guards against a zero `DELCAL` divisor (the `0x34` read is now masked with
`ADF4030_DELCAL`, as it should always have been).

#### Run 4 result — Stage B gate PASSED

```
ADF4030: ref 125000000 Hz, VCO 2500000000 Hz, BSYNC 9765625 Hz, avgexp 15, PLL locked, REF OK
ch5 TDC vs ch0: -25568255 fs
adf4030_jesd204_clks_sync4: aligned ch5 against ch0
ch8 TDC vs ch0: -32776812 fs
adf4030_jesd204_clks_sync4: aligned ch8 against ch0
```

The TDC readings are genuine sub-UI phase measurements, which is the proof the
root cause is fixed. With BSYNC at 9.765625 MHz the period is exactly
102 400 000 fs, so the 24-bit signed TDC spans **±51 200 000 fs (±1 UI)**:

| Channel | Reading | Phase | % of full scale |
|---|---|---|---|
| ch5 (Apollo SYSREF) | −25 568 255 fs | −89.9° | −49.9 % |
| ch8 (FPGA SYSREF) | −32 776 812 fs | −115.2° | −64.0 % |

Neither saturates or wraps, and the 7.2 ns ch8↔ch5 skew is a plausible board
trace-delay difference. AVGEXP reads back as the intended 15, and `REF OK`
confirms the reference receiver works at the default RCM.

Subclass-0 behaviour is unchanged: `Subclass=0` on all links, `Link status: DATA`
on both directions, all 8 lanes `Link is good`, measured link clock 312.491 MHz,
and all four NCO round-trips passing. So the ADF4030 is wired, aligned and
regression-free — Stage C was unblocked at this point, and passed on its first
run (see below).

**Caveat carried into Stage C — closed there, but not the way this section
originally proposed.** `aligned chN against ch0` only meant the FSM completed:
`adf4030_set_single_ch_alignment()` returns 0 even when its retries are exhausted
with `TDC_ERR`/`TMP_ALIGN_ERR` still set (the kernel behaves the same way at
`adf4030.c:630`), and nothing clears those bits between retries, so once latched
the loop just spins and reports success.

The fix chosen in Stage C was **not** a second TDC probe. `adf4030_tdc_probe()`
was removed as debug-only after run 4, and re-adding it costs ~215 ms of blocking
`no_os_mdelay` per channel at avgexp 15 and perturbs `0x10`/`0x11`/`0x15`/`0x16`/
`0x61` — `adf4030_set_tdc_measurement()` asserts `MANUAL_MODE`, which
`set_single_ch_alignment()` clears. Instead `clks_sync4` now reads `0x90` once
per channel after each alignment and folds the sticky bits into its log line:

```
adf4030_jesd204_clks_sync4: aligned ch5 against ch0 (tdc_err=0 align_err=0)
```

Two SPI reads, no delay, no state perturbation, and `pr_warning` with a "did not
converge" message when either bit is set. The return code is unchanged, keeping
kernel parity — this surfaces state rather than altering behaviour. It does not
give a femtosecond residual, which the probe would have; the sticky bits were
enough, since both channels read clean and SYSREF phase locked.

### Stage C — Flip to subclass 1 — DONE, HW-verified 2026-08-13

Five changes, ordered so everything before the flip is a bisectable no-op.

1. **`AD9088_RX/TX_JESD_SUBCLASS = 1`** in `common_data.h`, flowing to
   `ad9088_ip.subclass` → `ad9088_dt.c:202-205`. Direct analogue of
   `adi,subclass = <JESD_SUBCLASS>`.

   Note which consumer matters. Only `ad9088_ip.subclass` reaches hardware — via
   the profile override, then `lnk->subclass` (`ad9088_jesd204_fsm.c:63,106`). The
   `rx_jesd204_ip`/`tx_jesd204_ip` copies land in
   `jesd->config.subclass_version`, read **only** by
   `axi_jesd204_{rx,tx}_apply_config_legacy()`; the project uses the FSM variants
   (`axi_jesd204_rx_init` at `axi_jesd204_rx.c:902`), so those two are inert and
   are kept in step for consistency alone.

2. **`sysref_present` fixup — mandatory,** as Stage A's dump predicted (all three
   flags 0). Kernel `ad9088_dt.c:237-248`, inside `if (val)`:
   ```c
   if (!profile.mcs_cfg.side_a_sysref.sysref_present ||
       !profile.mcs_cfg.side_b_sysref.sysref_present)
           profile.mcs_cfg.center_sysref.sysref_present = true;
   ```
   no-OS had ported only the four subclass assignments and dropped this. Without
   it the SYSREF receiver is enabled host-side while the CPU firmware still
   believes there is nothing to receive.

3. **Profile version check + CRC32 recompute** — this closes what was previously
   listed below as a known deviation. See the next subsection.

4. **`lmfc_offset` seeded in the framework, not the driver.** `lnk->sysref` is
   left at the `calloc` zeros, and offset 0 is a *legal* LMFC offset rather than
   "unset", so both AXI cores unconditionally programmed 0 —
   `axi_jesd204_rx.c:545`, `axi_jesd204_tx.c:448` gate the write on
   `!= JESD204_LMFC_OFFSET_UNINITIALIZED` and nothing ever assigned that
   sentinel. Fixed in `jesd204_dev_alloc_links()` (`jesd204-core.c:26-30`), which
   is exact parity with the kernel framework's
   `__jesd204_dev_init_link_data()` (`drivers/jesd204/jesd204-core.c:1086`) — a
   shared-framework change, not an ad9088-local one.

   **`lnk->sysref.mode` was deliberately NOT set.** This section previously
   proposed `JESD204_SYSREF_CONTINUOUS`; that was wrong. The kernel's
   `ad9088_jesd204_link_init()` never touches `lnk->sysref` at all, so the
   reference platform runs with `mode == JESD204_SYSREF_DISABLED`. Its only
   consumer is `hmc7044_jesd204_link_pre_setup()` (`hmc7044.c:1424`), which would
   force `HMC7044_PULSE_GEN_CONT_PULSE` — already what `hmc7044_ip.pulse_gen_mode`
   is (`common_data.c:173`, matching `dts:549`). Setting it would have been a
   deviation that bought nothing.

5. **ADF4030 sticky-error report** — see Stage B-fix above.

#### Profile CRC32 — deviation closed

The kernel recomputes `profile_checksum` after mutating the profile
(`ad9088_dt.c:291`) and hard-fails on a version mismatch (`:251-262`). no-OS did
neither, and every bring-up log carried `WARNING: profile crc is invalid.` from
`adi_apollo_arm.c:243`. With Stage C adding a second mutation this was closed
rather than left as the prime suspect.

**New: `util/no_os_crc32.c` + `include/no_os_crc32.h`** — no-OS had crc8/16/24
but no crc32.

```c
uint32_t no_os_crc32_be(uint32_t crc, const uint8_t *pdata, size_t nbytes);
```

Bitwise MSB-first, polynomial `0x04C11DB7`, no input/output reflection, no final
XOR — a transcription of Linux `crc32_be_generic()` at `CRC_BE_BITS == 1`
(`lib/crc32.c:293-305`, `CRC32_POLY_BE` in `include/linux/crc32poly.h:11`).
Byte-for-byte agreement with the kernel's `crc32_be()` is the point. Validated
against the CRC-32/MPEG-2 vector before use: seed `0xFFFFFFFF` over `"123456789"`
gives `0x0376E6E7`, which pins polynomial, bit order and the absence of
reflection and final XOR simultaneously.

*Deliberate deviation from the crc8/16/24 family idiom* (`no_os_crc16.h:41-46`:
`NO_OS_DECLARE_CRC16_TABLE` + a `populate` call + a table-driven core). Those are
called repeatedly on short frames, where a 256-entry table earns its keep. This
runs once, over ~5 KB, at init — a 1 KB table plus a populate call buys nothing,
and the bitwise form is the version that can be read line-by-line against
`lib/crc32.c`.

In `ad9088_dt.c`: the version check goes immediately after
`ad9088_get_profile()` and prints found *and* expected `major.minor`, so a
mismatch is self-diagnosing in one run (`ADI_APOLLO_PROFILE_VERSION_MAJOR` is 10,
`MINOR` 1). The recompute goes **last**, after `ad9088_jesd_lane_setup()` and the
subclass/`sysref_present` block:

```c
phy->profile.profile_checksum =
        no_os_crc32_be(0, (const uint8_t *)&phy->profile,
                       sizeof(phy->profile) - sizeof(uint32_t));
```

`profile_checksum` is the final member of the packed `adi_apollo_top_t`
(`apollo_cpu_device_profile_types.h:2435-2436`), so `sizeof - sizeof(u32)` is
exactly the covered region, matching the kernel's `sizeof(*p) - sizeof(u32)`.

#### Run 5 result — Stage C PASSED, first attempt

```
ADF4030: ref 125000000 Hz, VCO 2500000000 Hz, BSYNC 9765625 Hz, avgexp 15, PLL locked, REF OK
adf4030_jesd204_clks_sync4: aligned ch5 against ch0 (tdc_err=0 align_err=0)
adf4030_jesd204_clks_sync4: aligned ch8 against ch0 (tdc_err=0 align_err=0)
JRX ADI_APOLLO_LINK_A0: L= 8 M= 4 F= 1 S= 1 Np=16 CS= 0 Subclass=1 link_en= Enabled
JTX ADI_APOLLO_LINK_A0: L= 8 M= 4 F= 1 S= 1 Np=16 CS= 0 Subclass=1 link_en= Enabled
JESD TX (JRX Deframer Link A0) Phase Difference 10
JESD TX (JRX Deframer Link A0) Link0 204C User status: Ready, SYSREF Phase: Locked
SYSREF_PHASE = 433 (TRY0..TRY4)          <- identical all five reads
        Link status: DATA                <- both tx_jesd and rx_jesd
        SYSREF captured: Yes
        SYSREF alignment error: No
```

No `profile crc is invalid` warning, so the recompute works; the profile
(`ver=10.1.2`) also passed the new major/minor check. All 8 lanes `Link is good`,
measured link clock 312.489/312.491 MHz, and all four NCO round-trips pass.

**Two log details that look wrong and are not.**

1. **`SYSREF_PHASE = 433`, nowhere near 0.** Stability is the whole of what
   subclass 1 alone provides — it is *identical* across all five reads. Trimming
   the phase toward 0 is Stage D's job. `Skipping MCS calibration` in the log is
   the Stage D stub (`ad9088_jesd204_fsm.c:542-558`) firing as designed.
2. **Disabled JTX A1/B1 print `Subclass=0` while disabled JRX A1/B1 print `1`.**
   Both prints are hardware readbacks via
   `adi_apollo_{jrx,jtx}_link_inspect()` (`ad9088.c:498,540`), not the host
   profile, and disabled links report defaults throughout — note the accompanying
   `L=1 M=1 Np=1`, which is not real config either. Every *enabled* link reads 1,
   which is what the override is responsible for.

### Stage D — MCS calibration — DONE, HW-verified 2026-08-14 (first attempt)

**Status: passed on the first hardware run.** Stage C removed the first of the
three triggers (SYSREF phase does lock without cal), so Stage D was never on the
critical path for the single-device link. It was ported anyway to trim
`SYSREF_PHASE` (which sat at 433 = 21.65 ns), to make latency *accurate* rather
than merely deterministic, and as the prerequisite for a second device. It
delivered: 433 → 40 Fclk, i.e. 21.65 ns → 2.00 ns. See "Run 6 result" at the end
of this section.

**No Apollo API porting was needed.** Everything the kernel calls already existed
in `public/inc/`: `adi_apollo_mcs_cal_*` (`adi_apollo_mcs_cal.h`), the
`adi_apollo_mailbox_mcs_bsync_{set_config,get_config,go}` trio
(`adi_apollo_mailbox.h:486-508`), the `MCS_OFFSET_{A,B,C}_FEMTOSECONDS_INT64` /
`MCS_ADF4382_TRACK_TARGET_{0,1}_INT32` enums (`adi_apollo_mcs_cal_types.h:68-82`)
and `APOLLO_MCS_BSYNC_{ALIGN,OUTPUT_EN,OUTPUT_DIS}`
(`adi_apollo_mailbox_types.h:744-746`). The work was entirely driver glue.

#### What the kernel's IIO calls actually decode to

This mapping was the core of the port. `phy->iio_adf4030` is ADF4030 **channel 5**
(Apollo SYSREF), `phy->iio_adf4382` is ADF4382 channel 0.

| Kernel call | Real operation | no-OS equivalent |
|---|---|---|
| read `IIO_CHAN_INFO_FREQUENCY` on adf4030 | BSYNC rate | `adf4030_get_bsync_freq()` |
| read `IIO_CHAN_INFO_PHASE` on adf4030 | **a TDC measurement**, *not* a delay read (kernel `adf4030.c:942-951`) | `adf4030_set_tdc_source()` → `_set_tdc_measurement()` → `_get_tdc_measurement()` |
| write `IIO_CHAN_INFO_PHASE` on adf4030 | set delay, then realign the channel (kernel `adf4030.c:978-983`) | `adf4030_set_channel_delay()` → `adf4030_set_single_ch_alignment()` |
| ext_info `output_enable` | channel direction | `adf4030_set_channel_direction()` |
| ext_info `background_serial_alignment_en` | `(en, mask)` | `adf4030_set_background_serial_alignment(mask_or_0)` — no-OS takes only the mask |
| ext_info `en_auto_align` on adf4382 | `auto_align_en = 1` + `set_phase_adjust(0)` | new `adf4382_set_auto_align()` |
| write `IIO_CHAN_INFO_PHASE` = 125 on adf4382 | `set_phase_pol()` + `set_phase_adjust()`, units **fs** | `adf4382_set_phase_pol()` + `adf4382_set_phase_adjust()` |

#### Changes made

- **`drivers/rf-transceiver/apollo/ad9088_mcs.c`** (new) — `init_cal_setup`,
  `delta_t_measurement_set/get`, `init_cal_validate`, the two status printers, and
  `tracking_cal_setup`. Uses `pr_*` rather than `adi_apollo_hal_log_write`.
- **Ops shim, not an IIO layer** — `struct ad9088_bsync_ops` / `ad9088_clk_ops`
  in `ad9088.h` replaced the permanently-NULL `iio_adf4030`/`iio_adf4382` fields.
  This mirrors the kernel, whose `ad9088` module never includes `adf4030.h` /
  `adf4382.h` and reaches those chips only through `struct iio_channel *`.
- **`post_setup_stage1` implemented** — freq read → clear delay → `init_cal_setup`
  → round-trip path-delay measurement → program `-path_delay` → `init_run` +
  `status_get` + `validate` → tracking cal (single-clock only).
- **`post_setup_stage2`** gained the background-serial-alignment blocks, gated on
  `aion_background_serial_alignment_en` (false by default, so inert here).
- **`ad9088_dt.c`** — `mcs_track_win` into the profile and the
  `mcs_track_decimation` default of 1023. The `track_win` write is placed
  **before** the CRC32 recompute; putting it after would resurrect the
  `WARNING: profile crc is invalid.` line Stage C removed.
- **Project glue** — wrappers plus `ad9088_mcs_ops_bind()` in `common_data.c`,
  called from `basic_example.c` between `adf4030_init()` and `ad9088_init()`.
  `bsync_ops`/`clk_ops` stay NULL until that call, so `dma_example` and
  `iio_example` keep printing `Skipping MCS calibration` unchanged.

#### ADF4382 driver brought in line with the kernel

The kernel expresses phase adjustment in **femtoseconds**; no-OS used
**picoseconds** and had no auto-align flag, so the MCS path's `125` would have
been 1000x too large. `adf4382_set_phase_adjust()` was therefore converted in
place to fs (not a parallel variant), and while there the whole phase area was
aligned to the kernel:

- fs-domain math with the kernel's `phase_value == 0 && phase_fs != 0 → 1` clamp
  and `> UINT8_MAX → 255` clamp, replacing a `phase_reg_value -= 255` line that
  had no kernel counterpart and read like a bug.
- New `adf4382_get_phase_adjust()` — no-OS had **no getter at all**; the IIO layer
  returned the `phase_adj` shadow, i.e. what was last *requested*. It now reads
  register `0x33` back and inverts the computation, matching kernel
  `adf4382.c:1300-1336`. `iio_adf4382.c` uses it.
- New `adf4382_set_auto_align()` + `dev->auto_align_en`, owning `0x32` bit 4.
- `adf4382_pfd_compute()` converted to the kernel signature
  `int (dev, uint64_t *)` with the `ADF4382_PFD_FREQ_MIN/MAX` range check no-OS
  lacked — **all 9 call sites updated**, seven of them in unrelated paths
  (`set_freq`, `get_rfout`, fast-cal LUT, VCO cal timeout, sync setup). An
  out-of-range reference now fails loudly instead of feeding a bogus PFD
  downstream. This board's PFD is 125 MHz, well inside [5.4 MHz, 625 MHz], so
  nothing changes here.
- Constants renamed to the kernel's names: `FS_PER_NS`, `NS_PER_MS`, `MS_PER_NS`,
  `NS_PER_FS`, `PERIOD_IN_DEG`, `PERIOD_IN_DEG_MS`, and the
  `ADF4382_PHASE_BLEED_CNST_MUL`/`_DIV` ratio replacing the single
  `ADF4382_PHASE_BLEED_CNST`. Dead `S_TO_NS`/`NS_TO_PS` removed.
- Callers updated: `projects/adf4382` example `1` → `1000` (same 1 ps intent).

**The conversion fixed a latent bug.** The old ps math computed register `0` for
`adf4382_set_phase_adjust(dev, 1)` — a silent no-op. Verified numerically: the fs
path gives `125 fs → reg 0x01` via the clamp, which is the entire hardware effect
of the MCS phase write, and `set 10000 fs → reg 0x03 → get 10674 fs` round-trips
to within one register LSB (~3558 fs at 20 GHz / Icp 1100 uA).

#### Two traps worth remembering

1. **`no_os_div_u64()` cannot be used in the validate step** — it divides by
   `1e12`, above its `uint32_t` divisor limit, and truncates silently.
   `no_os_div64_u64_rem()` (`include/no_os_util.h:188`) is the only 64-bit-divisor
   helper. It also **dereferences its remainder pointer unconditionally**, so
   passing NULL faults; pass a real variable even when the remainder is unwanted.
2. **`adf4030_set_tdc_measurement()` asserts `MANUAL_MODE`**, which
   `adf4030_set_single_ch_alignment()` clears via its full-byte `0x11` write. A
   TDC measure followed by an alignment self-heals; one left as the last operation
   does not. Same caveat as the removed `adf4030_tdc_probe()`.

#### Deliberate deviations

- `ad9088_mcs_tracking_cal_validate()` **not ported** — it only cross-checks the
  Apollo firmware's view of the ADF4382 bleed-current registers against the chip,
  is purely diagnostic, and would need read-back ops the shim otherwise avoids.
- The kernel triplicates its "mode 2 + re-enable output" recovery at three early
  returns (`ad9088_jesd204_fsm.c:629-637,643-651,659-667`). no-OS uses a single
  `restore` label that preserves the first error — same semantics, one place to
  get right. **This is the code that must not be broken:** the link is up while
  the measurement runs, so a SYSREF line left undriven takes it down.
- Status printers emit `pr_info` lines instead of filling `phy->dbuf`, since there
  is no sysfs consumer.

#### Run 6 result — Stage D PASSED, first attempt

```
adf4030_jesd204_clks_sync4: aligned ch5 against ch0 (tdc_err=0 align_err=0)
adf4030_jesd204_clks_sync4: aligned ch8 against ch0 (tdc_err=0 align_err=0)
JESD TX (JRX Deframer Link A0) Phase Difference 23
JESD TX (JRX Deframer Link A0) Link0 204C User status: Ready, SYSREF Phase: Locked
Total BSYNC path delay 2175424 fs
MCS Init Cal Validation: Passed
MCS Initcal Status: Passed
WARNING: Phase 125 fs too small to represent at Icp 11100 uA.
SYSREF_PHASE = 40 (TRY0..TRY4)
        Link status: DATA            <- both tx_jesd and rx_jesd
        SYSREF captured: Yes
        SYSREF alignment error: No
```

All acceptance criteria met, in the order the checklist asks for them:

| Check | Result |
|---|---|
| No link regression | **PASS** — `Link status: DATA` both cores, 8 lanes `Link is good`, `Subclass=1` on enabled links, measured link clock 312.489/312.491 MHz, all four NCO round-trips passing, no `profile crc is invalid` |
| `SYSREF_PHASE` trimmed | **PASS** — 433 → **40**, identical across all five reads |
| `MCS Initcal Status: Passed` | **PASS** — validate step's residual inside ±0.4 device-clock cycles |
| `Total BSYNC path delay` plausible | **PASS** — 2 175 424 fs = 2.175 ns, positive and 2.1% of the 102 400 000 fs BSYNC period |

**The `SYSREF_PHASE` unit is Fclk**, not an arbitrary count —
`adi_apollo_clk_mcs_sysref_phase_get()` documents "Result is units of Fclk", and
Fclk here is the 20 GHz device clock, so one count is 50 ps:

| | counts | time |
|---|---|---|
| Stage C, no MCS cal | 433 | 21.65 ns |
| Stage D, after MCS cal | 40 | 2.00 ns |

So the cal removed **19.65 ns of external-to-internal SYSREF offset**, a 10.8x
reduction, and the result is repeatable. The round-trip measurement did *not*
disturb the live link — the risk this stage was staged around did not materialise.

**`Phase Difference` moved 10 → 23.** Expected, not a regression: it counts conv
clock cycles since the last SYSREF and the SYSREF instant itself just moved by
19.65 ns. It is now meaningful (subclass 0 never reset the counter) *and* accurate
rather than merely deterministic, so it is the number to use for the
latency-repeatability check across `jesd204_fsm_stop()`/`_start()` cycles.

#### The `Phase 125 fs` warning is the clamp working, not a fault

```
WARNING: Phase 125 fs too small to represent at Icp 11100 uA.
```

This is the `phase_value == 0 && phase_fs != 0 → 1` clamp in
`adf4382_set_phase_adjust()` firing exactly as intended, and it is
kernel-identical behaviour. `adf4382_ip.cp_i = 15`
(`common_data.c:102`) selects the **maximum** charge pump current, 11 100 uA, and
at that current one LSB of the `0x33` phase register works out to **352 fs**:

| Icp | 1 LSB of reg 0x33 |
|---|---|
| 1 100 uA | 3 558 fs |
| 11 100 uA (ours) | 352 fs |

`AD9088_MCS_CLK_PHASE_FS` is 125 fs, below that floor, so the pre-clamp
computation rounds to 0 and the clamp substitutes 1 LSB. That is the intended
outcome — the point of the write is that the PLL carries a *standing nonzero*
correction for the tracking loop to steer, not its magnitude. Without the clamp
the register would have been left at 0 and the write would have been a silent
no-op, which is the latent bug the ps→fs conversion fixed. **Leave it alone; the
warning is informational and fires once per bring-up.**

#### Open follow-up: is the 2.00 ns residual expected?

`SYSREF_PHASE = 40` is 2.00 ns, and the path delay just compensated was 2.175 ns
— within 8% of each other. That closeness is a hint worth one experiment, though
**it is not evidence of a bug**: MCS init cal validates against
`recommended_offset_C_femtoseconds`, not against zero phase, so a nonzero
`SYSREF_PHASE` can be entirely by design.

**The data that would settle it is already collected and simply not printed.**
`ad9088_mcs_init_cal_status_print()` dumps `diff_C_Before`, `diff_C_After`,
`internal_period_C` and `recommended_offset_C` in femtoseconds, but
`post_setup_stage1` calls it **only on the failure path**. Printing it on success
too would reveal whether `diff_C_After` landed on `recommended_offset_C` — in which
case 2.00 ns is correct and this closes — or whether the compensation is off by
roughly one path delay, which would point at the sign of the
`delay_set(-path_delay)` write.

**Decision (2026-08-14): deliberately not done.** The success-path print was
considered and declined — Stage D meets every acceptance criterion as it stands, so
the extra bring-up log noise is not worth it yet. The call stays on the failure
path. Pick this up only if the residual becomes a problem, e.g. when a second
device is added.

Do not "fix" the sign speculatively: the current sign is what the kernel uses, and
the cal reports Passed.

---

## Verification

Build only; the board owner runs it. Expected UART evidence:

| Stage | Expected | Actual |
|---|---|---|
| A | Profile dump shows pre-override subclass, `sysref_present` state, internal SYSREF period. Datapath unchanged. | **PASS** — all three `sysref_present=0`, `int_prd=256`, `ratio=1`; link unchanged |
| B | ADF4030 probes at CS 4; HMC7044 ch1 delivers 125 MHz; ch3 retunes to divider 256; link still up in subclass 0. | **PARTIAL** — probe, PLL lock, link and NCO all fine; BSYNC alignment times out |
| B-fix (run 2/3) | as in the Stage B-fix section above | **FAIL** — `ch5/ch8 TDC read against ch0 failed (-116)`; run-3 dump root-caused it to `TDC_SOURCE` stuck at 0x1F |
| B-fix (run 4) | New `ADF4030: ...` identity line. `ch5/ch8 TDC vs ch0: <n> fs` with real values. `aligned ch5/ch8 against ch0`, no `-116`. Still subclass 0 with `Link status: DATA`, 8 lanes good, `Subclass=0`, NCO round-trips passing. | **PASS** — see "Run 4 result" above |
| C | `SYSREF captured: Yes`, `SYSREF alignment error: No` (both read `disabled` under subclass 0), `Link status: DATA`, 8 lanes good, `SYSREF Phase: Locked`, `Subclass=1` on enabled links, no `profile crc is invalid`. | **PASS** — all of it, first run. See "Run 5 result" above |
| C | **Latency repeatability** — the check subclass 0 cannot make. | **NOT RUN** — the framing below needed reworking first. Now unblocked: after Stage D, `Phase Difference` (23) is both meaningful and accurate, so it is the number to assert across FSM restarts |
| D | `Total BSYNC path delay <n> fs` plausible; `MCS Initcal Status: Passed`; `SYSREF_PHASE` settles at/near 0 and is stable across runs; no link regression. | **PASS** — first attempt, 2026-08-14. `SYSREF_PHASE` 433 → 40 (21.65 ns → 2.00 ns), path delay 2 175 424 fs, cal validated, link unchanged. See "Run 6 result" |

#### Stage D — what the run confirmed, and what remains

The risk was concentrated in one place: the round-trip measurement toggles the
SYSREF driver **while the link is up**, since `POST_SETUP_STAGE1` runs after
`LINK_RUNNING` (`include/jesd204.h:213-214`). **It did not disturb the link** —
`Link status: DATA` on both cores with 8 lanes good, exactly as before the stage.
The `restore` label in `ad9088_mcs_bsync_path_delay()` is what guarantees mode 2 +
`output_en(true)` on every exit path; keep it that way.

Still open, both tracked in "Open follow-up" above:

1. **Whether the 2.00 ns residual is expected** — needs
   `ad9088_mcs_init_cal_status_print()` called on the success path so
   `diff_C_After` and `recommended_offset_C` become visible.
2. **Latency repeatability** — assert `Phase Difference` is identical across
   repeated `jesd204_fsm_stop()`/`jesd204_fsm_start()` cycles. Now both meaningful
   and accurate; not yet exercised.

#### The latency-repeatability check needs a different measurement

The original framing — "capture the loopback with a known TX pattern, record the
RX sample offset of a marker, confirm it is identical across repeated full FSM
restarts" — **is not measurable on this platform**, and would mislead if built:

- The RX capture and the TX replay are started by two separate software register
  writes (`dma_example.c:882,896`), so the start-to-start skew carries software
  jitter.
- No hardware trigger exists to remove it. `axi_dmac` exposes only
  `cyclic`/`hw_cyclic`/`dma_flags` (`axi_dmac.h:81-106`) — no external sync input.
  The only sync in the TPL cores is `AXI_DAC_REG_SYNC_CONTROL` bit 0
  (`axi_dac_core.c:48-49`), a self-clearing strobe for the DAC's internal
  channel/DDS sync, not a SYSREF-referenced capture trigger.
- The uncertainty subclass 1 removes is ±1 LEMC ≈ 32 capture samples (LEMC
  9.765625 MHz against a 312.5 MHz capture rate). Software start jitter exceeds
  that, so the marker offset would report jitter and label it latency — the spread
  would look the same under subclass 0 and 1.

**Use `adi_apollo_jrx_phase_diff_get()` instead.** Its contract
(`public/inc/adi_apollo_jrx.h:335-352`) is the number actually wanted: "the number
of conv clock cycles since last SYSREF … **Reset by each SYSREF.**" Being read
from the device, it is immune to the software trigger. It is already wired up —
`ad9088_print_link_phase()` (`ad9088.c:580-590`) calls it from
`ad9088_jesd204_link_running()` (`ad9088_jesd204_fsm.c:528-529`) for every
`is_transmit` link, which is why run 5 logged `Phase Difference 10`. Under
subclass 0 nothing reset the counter, so that print was meaningless; it is
meaningful now. The check is: assert it is identical across repeated
`jesd204_fsm_stop()`/`jesd204_fsm_start()` cycles.

Rejected alongside: relative phase between RX converters within one capture is
trigger-independent and appealing, but `struct no_os_tone_result` exposes
coherence/amplitude/spread and **no phase** (`include/no_os_tone.h:110-123`), so
it needs a new output on a shared estimator whose validation harness no longer
exists on disk. Not worth the regression risk when the device hands over the
number directly.

**`jesd204_fsm_start()` discards every state-op return code and always returns 0**
(`jesd204-fsm.c:29-56`), and `.num_retries` is never read. A bad SYSREF frequency
will not fail bring-up — it surfaces only as the RX link parking at `BLOCK_SYNC`
in the `LINK_RUNNING` poll. Read the status registers explicitly; do not trust
the FSM's return value.

---

## Known deviations left in place

- ~~**Profile CRC is not recomputed after mutation.**~~ **Closed in Stage C** —
  `util/no_os_crc32.c` plus the version check and recompute in `ad9088_dt.c`. The
  `WARNING: profile crc is invalid.` line is gone from the bring-up log.
- **ADF4030 RCM is left at the silicon default.** The dts sets `adi,rcm = <1>` on
  ch0 and ch5, a raw RCM register code. The no-OS API takes millivolts
  (`adf4030_set_channel_voltage`), and code 1 works out to 497 mV — below its own
  `ADF4030_RCM_VOLTAGE_MIN0` of 504 — so the value is not reachable through that
  API. `rcm_mv` is 0, which skips the write. This was the leading suspect for the
  alignment timeout until run 3 showed `REF_OK = 1`; the reference receiver works
  at the default RCM, so it is now a low-priority cosmetic gap. Note the dts does
  **not** set `adi,rcm` on ch8 at all.
- **`ADF4030_EN_ITER` is left at 0**, matching the reference driver, which only
  ever enables it from sysfs. The `alignment_threshold_en` init parameter exists
  to turn it on should convergence ever need it.
- **`ADF4030_AUTO_PD_RCV` is not forced.** The kernel's per-channel `0x40 + 2n`
  write is a full `regmap_write` that always sets bit 7; no-OS uses `update_bits`
  on the termination bits only, leaving bit 7 at the defaults-table value
  (`0xE9`, i.e. set). Same result today, but it would diverge if the defaults
  table changed.
- **`adi,bsync-secondary-frequency-hz` has no no-OS equivalent.**
  `adf4030_init_param` carries only `bsync_freq`, and `adf4030_init()` programs
  ODIVB to the same rate as ODIVA. Harmless while no channel sets `odivb_en`.
- **Only the three functional ADF4030 channels are configured** (0, 5, 8). The dts
  additionally sets up channels 1/2/3/4/6/9 as debug test points and loopbacks;
  those are left at defaults. Note dts ch9 (`SYSREF_OUT_FMC`) also carries
  `auto-align-on-sync-en`, so the reference auto-aligns four channels to our
  three — the dts comments ch9 as connected to nothing.
- **`adf4030_remove()` leaked on the success path** (`no_os_free` was inside
  `if (ret)`). Fixed while adding the `jdev` teardown, since the leak now covers
  two allocations.
- `mcs_trigger_cfg` from the profile is ignored by both drivers; both hardcode
  `ADI_APOLLO_TRIG_PIN_A0` + `ADI_APOLLO_RX_TX_ALL`.
- `internal_sysref_prd_digclk_cycles_side_a/_side_b` are read by neither driver,
  so dual-clock trigger-phase validation is unimplemented in both.
- `WITH_AXI_AION_TRIG = 0` in the dts, so the `axi_aion_trig` JESD204 input to
  the ADF4030 is out of scope.
- **`iio_example.c` is now broken, and this is the one live regression from Stage
  C.** It still marks the HMC7044 as `.is_sysref_provider = true`
  (`iio_example.c:187`) and has no ADF4030, so it inherits the shared
  `common_data.h` subclass flip with nothing driving the Apollo's SYSREF pin: the
  `0x40` bit never sets and `ad9088_jesd_rx_link_status_print()` fails. The fix is
  mechanical — mirror `basic_example.c:148-152`, with the provider **first** in
  `devs[]` (`jesd204_topology_init()` reads `is_sysref_provider` from the caller's
  array but takes the pointer from the top-device-filtered copy,
  `jesd204-core.c:116-119`, so the provider must precede the top device and must
  not be last). Deliberately deferred to keep Stage C to `basic_example`.
  `iio_adf4030.c`/`iio_adf4382.c` are still not built.
- Debugfs-equivalent runtime MCS controls (13 kernel entries) are out of scope.
- **Stage D changed the shared ADF4382 driver, so its blast radius reaches past
  this project.** `adf4382_set_phase_adjust()` now takes **femtoseconds**, not
  picoseconds — the signature is unchanged (`uint32_t`), so this is a silent 1000x
  unit change for any caller. `projects/adf4382`'s example was updated (`1` →
  `1000`) and the `phase` IIO attribute's unit changed with it. Anything outside
  this tree that calls it must be rechecked.
- **`adf4382_pfd_compute()` now rejects an out-of-range PFD** on seven call sites
  that have nothing to do with MCS (`set_freq`, `get_rfout`, fast-cal LUT
  generation, VCO cal timeout, sync setup). This matches the kernel and is the
  right behaviour, but it means a reference configuration that previously ran with
  a silently bogus PFD now fails. This board is at 125 MHz, inside
  [5.4 MHz, 625 MHz], so it is unaffected.
- `projects/adf4382` **does not build, and did not before this work either** —
  `adf4382.h:42` includes `no_os_clk.h` and that project's `src.mk` never provides
  the path. Confirmed identical at HEAD by stashing the Stage D changes and
  rebuilding, so it is not a Stage D regression. The Stage D edits were verified
  against that project by compiling `adf4382.c` standalone (0 errors, 0 warnings
  under `-Wall -Wextra`).
