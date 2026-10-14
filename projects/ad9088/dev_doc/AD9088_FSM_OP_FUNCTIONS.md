# AD9088 JESD204 FSM Ops — Call-by-Call Reference

Detailed documentation of **every FSM op the AD9088 (top device) implements**,
and **what each function called inside each op callback does**.

Source: `drivers/rf-transceiver/apollo/ad9088_jesd204_fsm.c`

The op table (`jesd204_ad9088_init`, near the end of the file) registers these
callbacks:

| FSM op | Callback | Mode | Notes |
|--------|----------|------|-------|
| `DEVICE_INIT` | `ad9088_jesd204_uninit` | per_device | teardown-only |
| `LINK_INIT` | `ad9088_jesd204_link_init` | per_link | fill `jesd204_link` from profile |
| `LINK_SETUP` | `ad9088_jesd204_link_setup` | per_device | JTx/JRx en, RxEn/TxEn, DP reset, subclass, MCS, dyn-sync |
| `OPT_SETUP_STAGE1` | `ad9088_jesd204_setup_stage1` | per_device | clk-cond cal, phase adj, start ADC cal |
| `OPT_SETUP_STAGE2` | `ad9088_jesd204_setup_stage2` | per_device | wait ADC cal, re-sync links |
| `CLOCKS_ENABLE` | `ad9088_jesd204_clks_enable` | per_link | SERDES JRx init-cal, framer enable |
| `LINK_ENABLE` | `ad9088_jesd204_link_enable` | per_link | deframer enable + SYSREF |
| `LINK_RUNNING` | `ad9088_jesd204_link_running` | per_link | per-link status check: JTx PLL/PHASE/MODE, JRx phase-diff + per-lane "good" (HW-verified 2026-07-27) |
| `OPT_POST_SETUP_STAGE1` | `ad9088_jesd204_post_setup_stage1` | per_device | MCS calibration (stub — deferred to Step 8) |
| `OPT_POST_SETUP_STAGE2` | `ad9088_jesd204_post_setup_stage2` | per_device | trig-sync enable sequence (inert, `trig_sync_en=false`) |
| `OPT_POST_SETUP_STAGE3` | `ad9088_jesd204_post_setup_stage3` | per_device | trig-req GPIO pulse (inert) |
| `OPT_POST_RUNNING_STAGE` | `ad9088_jesd204_post_setup_stage4` | per_device | trig-phase check, SYSREF phase print, `adc_bgcal_unfreeze`, `is_initialized=true` (HW-verified 2026-07-27) |

The FSM runs ops in a fixed order; the AD9088 is the **top device** so it runs
last within each op. The AD9088 now implements the **full** op sequence
(DEVICE_INIT → OPT_POST_RUNNING_STAGE); no op slots are left unimplemented.

The `adi_apollo_*` functions are the vendor Apollo HAL, identical between the
Linux kernel driver and no-OS — the port is wiring, not logic. Descriptions are
taken from the HAL doxygen headers under
`drivers/rf-transceiver/apollo/public/inc/`.

---

## Conventions used throughout

**Callback shapes.** A `per_link` callback receives `struct jesd204_link *lnk`
and runs once for each active link; a `per_device` callback (requires
`.mode = JESD204_STATE_OP_MODE_PER_DEVICE`) runs once for the whole chip.

**Reason gate.** Every callback first checks `reason`. On
`JESD204_STATE_OP_REASON_INIT` it does forward bring-up work; on any other
reason (mainly `JESD204_STATE_OP_REASON_UNINIT`, teardown) most callbacks simply
return `JESD204_STATE_CHANGE_DONE`. Exceptions (`clks_enable`) do explicit
teardown work on `UNINIT`.

**Return values.** `JESD204_STATE_CHANGE_DONE` (=1) advances the FSM; a negative
errno aborts the transition.

**`ad9088_check_apollo_error(int ret, const char *api_name)`** — no-OS wrapper
(`ad9088.c:183`). Maps a HAL `API_CMS_ERROR_*` code into a no-OS errno and logs
`api_name` on failure. Used for the calibration calls whose HAL codes need
decoding; simpler calls are checked directly against non-zero.

**`ad9088_to_link(uint8_t linkid)`** — no-OS helper (`ad9088.c:194`). LUT that
maps the 0-7 topology link IDs (DEFRAMER A0/A1/B0/B1, FRAMER A0/A1/B0/B1) onto
the HAL `ADI_APOLLO_LINK_A0/A1/B0/B1` bit masks.

**Link ID map** (`ad9088.h:45-52`): DEFRAMER_LINK_A0_TX=0, A1_TX=1, B0_TX=2,
B1_TX=3, FRAMER_LINK_A0_RX=4, A1_RX=5, B0_RX=6, B1_RX=7. "TX" links are
device deframers (data into AD9088); "RX" links are device framers (data out).

---

# 1. `DEVICE_INIT` → `ad9088_jesd204_uninit`

**Runs:** per_device. **Forward pass:** no-op.

The callback only does work on `JESD204_STATE_OP_REASON_UNINIT`, and even then it
just logs and returns `JESD204_STATE_CHANGE_DONE`. It is registered on
`DEVICE_INIT` so the device *has* a teardown hook slot, but there is currently
nothing to tear down here. No HAL calls.

---

# 2. `LINK_INIT` → `ad9088_jesd204_link_init`

**Runs:** per_link (once per active link). **What it does:** populates the
framework's `struct jesd204_link *lnk` with the JESD parameters for this link,
read from the firmware-loaded profile (`phy->profile`). No hardware is touched —
this is pure data prep so the generic framework knows the link geometry.

The `lnk->link_id` selects a branch:

- **Deframer links** (`DEFRAMER_LINK_A0_TX` … `B1_TX`): source is
  `phy->profile.jrx[sideIdx].rx_link_cfg[linkIdx]`. `sideIdx`/`linkIdx` are
  derived from `link_id - DEFRAMER_LINK_A0_TX`. Sets `is_transmit = 1`.
- **Framer links** (`FRAMER_LINK_A0_RX` … `B1_RX`): source is
  `phy->profile.jtx[sideIdx].tx_link_cfg[linkIdx]`. Sets `is_transmit = 0`.

For each it copies the JESD geometry from the profile's `*_minus1` fields
(stored as value-1) back to natural counts: `num_lanes` (L), `num_converters`
(M), `octets_per_frame` (F), `frames_per_multiframe` (K),
`num_of_multiblocks_in_emb` (E), `bits_per_sample` (Np), `converter_resolution`
(N), `ctrl_bits_per_sample` (CS), `samples_per_conv_frame` (S). It also sets
JESD version (204B/204C), encoder (8B10B vs 64B66B), subclass, scrambling,
high-density, and `sample_rate` (DAC rate for deframer, ADC rate for framer) with
its divider.

On the deframer branch it also clears `priv->serdes_jrx_cal_run = false` so the
later SERDES cal in `CLOCKS_ENABLE` knows it must run.

### Function called: `jesd204_link_get_rate_khz(lnk, &lane_rate_kbps)`

**What it does:** no-OS JESD framework helper (`jesd204/jesd204-core.c:216`).
Computes the serial lane rate in kHz from the link parameters just filled in
(M, Np, S, F, K, encoder overhead). Here it is called to validate that the
geometry is self-consistent — a non-zero return aborts `LINK_INIT`. The value is
used later (in `CLOCKS_ENABLE`) to decide whether SERDES cal is required.

---

# 3. `LINK_SETUP` → `ad9088_jesd204_link_setup`

**Runs:** per_device. **What it does:** the device-side link bring-up — enables
the Apollo JTx/JRx links, powers up the converter datapaths, resets datapaths,
programs subclass + MCS SYSREF, freezes the ADC background cal, and runs the two
MCS dynamic-sync sequences. Executed once for the whole chip.

### 1-2. `adi_apollo_jtx_link_enable_set(device, links, link_en)` (×2)

**What it does:** enables or disables the JTx (framer, device→FPGA) links.
`links` is a mask of `adi_apollo_jesd_link_select_e`; `link_en` 1=enable,
0=disable. Called twice: enable `A0 | B0`, disable `A1 | B1` (only the primary
links per side are used in this topology).

### 3-4. `adi_apollo_jrx_link_enable_set(device, links, link_en)` (×2)

**What it does:** enables/disables the JRx (deframer, FPGA→device) links.
`link_en` 1=LINK_ENABLE, 0=LINK_POWER_DOWN. Same pattern: enable `A0 | B0`,
disable `A1 | B1`.

### 5. `adi_apollo_rxen_pwrup_ctrl_set(device, ADI_APOLLO_RXEN_ADC_ALL, &rxen_config)`

**What it does:** configures the RxEn power-up control block for all ADCs — the
logic that generates enable signals for the converters and Rx digital blocks.
The `rxen_config` here selects SPI-driven enable (`spi_rxen = 1`,
`spi_rxen_en = 1`) with the state machine off (`sm_en = 0`), i.e. Rx is turned on
by register write rather than by pin. `sm_clk_rate` sets the control-block clock
to Fs/32.

### 6. `adi_apollo_txen_pwrup_ctrl_set(device, ADI_APOLLO_TXEN_DAC_ALL, &txen_config)`

**What it does:** the Tx counterpart — configures the TxEn power-up control block
for all DACs, SPI-driven (`spi_txen`/`spi_txen_en`), SM off, Fs/32 clock.

### 7-10. `adi_apollo_rxmisc_dp_reset` / `adi_apollo_txmisc_dp_reset` (assert then deassert)

**What they do:** toggle the Rx/Tx **datapath reset** for all sides
(`ADI_APOLLO_SIDE_ALL`). `reset_en = 1` asserts, `0` clears. The sequence is
assert Rx, assert Tx, clear Rx, clear Tx — a clean datapath reset pulse so the
converter datapaths start from a known state before sync. These are called
without error checking (return value ignored) matching the kernel.

### 11. `adi_apollo_clk_mcs_subclass_set(device, subclass)`

**What it does:** programs the MCS (multi-chip sync) block with the JESD
subclass. `subclass` is computed as 1 if any JTx/JRx link config reports
subclass, else 0. Subclass 1 enables deterministic latency via SYSREF.

### 12. `adi_apollo_clk_mcs_sysref_en_set(device, enable)`

**What it does:** enables or disables the MCS **SYSREF receiver**. Enabled only
when `subclass == 1` (SYSREF is meaningless in subclass 0). This lets the device
capture the SYSREF pulse that hmc7044 will provide for alignment.

### 13. `adi_apollo_adc_bgcal_freeze(device, adcs)`

**What it does:** freezes (halts) the ADC **background** calibration for all
ADCs (`ADI_APOLLO_ADC_ALL` or `..._4T4R` per `is_8t8r`). Background cal is frozen
during link sync so it doesn't perturb timing while the FIFOs and dividers are
being aligned.

### 14. `adi_apollo_clk_mcs_dyn_sync_sequence_run(device)`

**What it does:** runs the **default** MCS dynamic-sync sequence, which
synchronizes the **digital root clocks** while minimizing supply transients. On
entry all sync masks must be clear; on exit they are clear again. This is the
first half of the two-part sync.

### 15. `adi_apollo_clk_mcs_dyn_sync_rxtxlinks_sequence_run(device)`

**What it does:** runs the SerDes-link dynamic-sync sequence — syncs ADC & DAC
FIFOs, Rx & Tx digital, loopback FIFOs, and the JTx/JRx links (A0/A1, B0/B1),
with the digital root clocks masked. Completes the alignment started by the
previous call. (This same function is re-run in stage 2 after calibration.)

---

# 4. `OPT_SETUP_STAGE1` → `ad9088_jesd204_setup_stage1`

**Runs:** per_device. **What it does:** runs the clock-conditioning calibration
(a hard prerequisite for the SERDES cal in `CLOCKS_ENABLE`), inspects the
negotiated link config, applies deterministic-latency phase adjustments, and
**kicks off** the long ADC init calibration (completed in stage 2).

Before any HAL call it records datapath enable masks (`phy->rx_en_mask`,
`phy->tx_en_mask`) — plain struct writes, no hardware.

### 1. `adi_apollo_adc_nyquist_zone_set(device, n_adc, nyquist_zone)`

**What it does:** sets the ADC Nyquist zone (1 = first Nyquist, 2 = second),
controlling the digital spectral inversion/mixing on captured data.
`nyquist_zone = phy->rx_nyquist_zone[0][0] + 1`.

> **no-OS deviation:** `ad9088_dt.c` stores `rx_nyquist_zone` as `zone - 1`
> (0/1); the HAL wants 1/2 — hence `+ 1`.

### 2. `adi_apollo_cfg_clk_cond_cal_cfg_set(device, cc_cal_cfg)`

**What it does:** selects *how* the clock-conditioning cal will run; does not run
it. `cc_cal_cfg` is `adi_apollo_sysclock_cond_cfg_e`: `ENABLED`(0)=run from
scratch (used), `DISABLED`(1), `ENABLED_WARMBOOT_FROM_USER`(2),
`DISABLED_WARMBOOT_FROM_USER`(3).

> **no-OS deviation:** warmboot-from-user not ported → hardcoded `ENABLED`.

### 3. `adi_apollo_sysclk_cond_cal(device)`

**What it does:** executes the **foreground ADC clock-conditioning
calibration** — tunes the internal clock distribution/conditioning so the
converter and SERDES clocks are clean. Blocking (up to ~10 s). **Its absence was
the root cause of the earlier SERDES JRx cal failure** (`-85`/`0x207`): the
SERDES CDR cannot train against an unconditioned clock. Wrapped in
`ad9088_check_apollo_error`.

### 4. `ad9088_inspect_jrx_link_all(phy)` / `ad9088_inspect_jtx_link_all(phy)`

**What they do:** no-OS helpers (`ad9088.c`) that call
`adi_apollo_jrx_link_inspect` / `adi_apollo_jtx_link_inspect` per link, read back
negotiated params (L/M/F/S/Np/CS, enabled state), and `pr_info` a summary. The
JRx variant also populates `phy->jrx_lanes[]`/`jrx_lanes_used` from the profile
crossbar. Diagnostic + bookkeeping — confirms the loaded profile matches
hardware before calibrating.

### 5. `adi_apollo_jrx_phase_adjust_calc(device, A0|B0, margin, &jrx_phase_adjust)`

**What it does:** computes the JRx LMFC/LEMC phase-adjustment value that
equalizes cross-link arrival-time differences, plus a safety `margin`
(`ADI_APOLLO_JRX_PHASE_ADJ_MARGIN_DEFAULT` = 2). Result is a count in `conv_clk`
cycles (max `K*S/NS - 1`). Calculates only; applies nothing.

### 6. `adi_apollo_jrx_phase_adjust_set(device, A0|B0, jrx_phase_adjust)`

**What it does:** applies the value from step 5. Each link's phase counter is
reset by SYSREF and increments per `conv_clk`; this offsets it so the deframer
samples at the aligned LMFC phase — the core of subclass-1 deterministic latency
on the receive (into-AD9088) path.

### 7. `adi_apollo_jtx_phase_adjust_set(device, A0|B0, 0)`

**What it does:** the JTx (framer, out-of-AD9088) counterpart — sets the output
LMFC phase adjustment. `0` = normal operation (non-zero is debug-only).

### 8. `adi_apollo_adc_init_cal_start(device, adc_cal_chans, init_cal_cfg)`

**What it does:** **starts** the ADC foreground init calibration and returns
immediately without waiting. Sets up cal config and issues the mailbox command;
the wait happens in stage 2, letting the FSM overlap other work.
`init_cal_cfg = ADI_APOLLO_INIT_CAL_ENABLED` (run from scratch, up to ~100 s).

> **no-OS deviation:** NVM/user warmboot paths not ported → hardcoded `ENABLED`.
> Wrapped in `ad9088_check_apollo_error`.

---

# 5. `OPT_SETUP_STAGE2` → `ad9088_jesd204_setup_stage2`

**Runs:** per_device. **What it does:** waits for the ADC init cal started in
stage 1, re-applies the Nyquist zone the cal may have disturbed, and re-runs the
JESD link MCS sync.

### 1. `adi_apollo_adc_init_cal_complete(device, n_adc)`

**What it does:** **blocks/polls until the ADC init cal completes** for the
selected ADCs. Logs progress and, on failure/timeout, prints detailed
per-channel error codes. Partner to `adc_init_cal_start` in stage 1 (start there,
join here). Wrapped in `ad9088_check_apollo_error`.

### 2. `adi_apollo_adc_nyquist_zone_set(device, n_adc, nyquist_zone)`

**What it does:** same call as stage 1 step 1 — re-issued because the ADC init
cal can reset the zone setting, so the intended value is written again after cal.
Same `+ 1` no-OS convention.

### 3. `adi_apollo_clk_mcs_dyn_sync_rxtxlinks_sequence_run(device)`

**What it does:** re-runs the SerDes-link dynamic-sync sequence (same call as
`LINK_SETUP` step 15) to re-align the JESD link FIFOs/dividers that the
calibrations perturbed, so links are coherently synced before `CLOCKS_ENABLE`
trains the SERDES.

### 4. (deferred) block-memory CDDC/FDDC sample-delay calibration

The kernel runs a block-memory sample-delay cal on the coarse/fine DDCs here.
**Not ported** — `phy->cddc_sample_delay_en`/`fddc_sample_delay_en` don't exist
in no-OS yet. Left as a comment stub (lines 393-397).

---

# 6. `CLOCKS_ENABLE` → `ad9088_jesd204_clks_enable`

**Runs:** per_link. **What it does:** the SERDES bring-up. For deframer (TX)
links above 8 Gbps it runs the mandatory SERDES JRx init-cal; above 16 Gbps it
also manages the SERDES background cal freeze/unfreeze. For framer (RX) links it
enables the JTx framer. This is also where teardown freezes the fast-lane bg cal.

First it maps `lnk->link_id` to a SERDES 12-pack select: A0/A1 → `12PACK_A`,
B0/B1 → `12PACK_B`, else `12PACK_NONE`. `lane_rate_khz` comes from the profile
(`jrx[0].common_link_cfg.lane_rate_kHz`).

### Teardown path (UNINIT, transmit link, > 16 Gbps): `adi_apollo_serdes_jrx_bgcal_freeze(device, serdes)`

**What it does:** freezes (halts) the JRx SERDES **background** calibration for
the selected 12-pack. Done on teardown for fast lanes so the bg cal isn't left
running as the link comes down.

### Init path (INIT, transmit link, > 8 Gbps):

**`adi_apollo_serdes_jrx_init_cal(device, serdes, init_cal)`** — **what it
does:** executes the JRx SERDES **foreground init calibration** (CDR + equalizer
training) for the selected 12-pack. Mandatory above 8 Gbps. **The FPGA must be
transmitting data for this to succeed** — which is why the framer/link cores are
already up and SYSREF/clock-conditioning are done first. `init_cal =
ADI_APOLLO_INIT_CAL_ENABLED` (from scratch; warmboot-from-FW not ported).
Wrapped in `ad9088_check_apollo_error`. *This is the call that was failing with
`0x207` before the lane-crossbar and `&phy` bugs were fixed.*

**`adi_apollo_serdes_jrx_bgcal_unfreeze(device, serdes)`** (only if > 16 Gbps) —
**what it does:** unfreezes (starts/restarts) the JRx SERDES background
calibration after the init-cal, so the equalizer keeps tracking drift during
operation at very high lane rates.

### Framer path (RX / `!is_transmit`): `adi_apollo_jtx_link_enable_set(device, ad9088_to_link(lnk->link_id), enable)`

**What it does:** enables (INIT) or disables (UNINIT) the JTx framer for this
specific link. `ad9088_to_link` converts the 0-7 link ID to the HAL link mask;
`enable` is `reason == JESD204_STATE_OP_REASON_INIT`. Wrapped in
`ad9088_check_apollo_error`.

---

# 7. `LINK_ENABLE` → `ad9088_jesd204_link_enable`

**Runs:** per_link. **`post_state_sysref = true`** — after this op completes the
framework calls `jesd204_sysref_async()`, which fires SYSREF through hmc7044's
`sysref_cb`. That alignment pulse is what lets the deframer leave `WAIT_BS`.

### Deframer path (TX / `is_transmit`): `adi_apollo_jrx_link_enable_set(device, ad9088_to_link(lnk->link_id), enable)`

**What it does:** enables (INIT) or disables (UNINIT) the JRx **deframer** for
this link — the final step that turns on data reception into the AD9088 now that
the SERDES is calibrated. `enable = reason == JESD204_STATE_OP_REASON_INIT`.
Wrapped in `ad9088_check_apollo_error`. Framer (RX) links do nothing here — they
were already enabled in `CLOCKS_ENABLE`.

After this op the framework pulses SYSREF (via `post_state_sysref`), links leave
`WAIT_BS`, and the FSM proceeds toward `LINK_RUNNING`.

---

# 8. `LINK_RUNNING` → `ad9088_jesd204_link_running`

**Runs:** per_link. **No `.mode`, no `.post_state_sysref`.** Validates and prints
per-link JESD status once the link is running; the last op the AD9088 implements
before the (deferred) POST stages. On teardown (`reason != INIT`) it clears
`phy->is_initialized` and returns `JESD204_STATE_CHANGE_DONE`.

### Deframer path (TX / `is_transmit`): phase + JRx status

- **`ad9088_print_link_phase(phy, lnk)`** → `adi_apollo_jrx_phase_diff_get` —
  prints the JRx phase difference.
- **`ad9088_jesd_rx_link_status_print(phy, lnk, 3)`** → polls
  `adi_apollo_jrx_link_status_get` (up to 3 retries, `no_os_mdelay(20)` between).
  Pass/fail mask is subclass-dependent: `0x60` for subclass 1, `0x20`
  (User-status only, SYSREF-phase bit ignored) for subclass 0. Then loops over
  lanes reading `adi_apollo_jrx_j204c/b_lane_status_get` for the physical lane
  from `profile.jrx[...].rx_link_cfg[...].lane_xbar[i]`, printing "Link is good"
  (204C lane state `0x6`) per lane. Returns `< 0` → `JESD204_STATE_CHANGE_ERROR`.

### Framer path (RX): JTx status

- **`ad9088_jesd_tx_link_status_print(phy, lnk, 3)`** → polls
  `adi_apollo_jtx_link_status_get`; 204C checks `(stat & 0x60)==0x60`, 204B
  checks `(stat & 0xF0)==0x70`. Returns `< 0` → `JESD204_STATE_CHANGE_ERROR`.

(Faithful to the kernel: the RX status helper runs on the `is_transmit` branch
and the TX helper on the framer branch.)

**HW-verified on VCU118 (2026-07-27):** Link A0 — JTx `PLL locked, PHASE
established, MODE valid`; JRx phase diff 23, all 8 lanes "Link is good", User
status Ready. The `SYSREF Phase: Unlocked` line was cosmetic under subclass 0
(bit `0x40` excluded from the mask).

**Subclass 1 HW-verified 2026-08-13:** the mask is now `0x60`, and the line reads
`SYSREF Phase: Locked`. The lock did **not** require MCS cal — the cal stub still
prints `Skipping MCS calibration`. `Phase Difference` (from
`adi_apollo_jrx_phase_diff_get()`, printed as 10 on that run) also becomes
meaningful for the first time here: it counts conv clock cycles since the last
SYSREF and is reset by each SYSREF, so under subclass 0 nothing reset it.

---

# 9. `OPT_POST_SETUP_STAGE1` → `ad9088_jesd204_post_setup_stage1`

**Runs:** per_device. **What it does:** in the kernel this runs the MCS
(multi-chip sync) delta-t calibration, which needs the adf4030/adf4382 clock
chips wired as IIO channels. This no-OS config has neither
(`phy->iio_adf4030`/`iio_adf4382` are always NULL), so on INIT the callback
prints `Skipping MCS calibration` and returns `JESD204_STATE_CHANGE_DONE`. It is
an **effective stub** — the MCS-cal body is deferred to **Step 8**. No HAL calls.

**HW-verified on VCU118 (2026-07-27):** log shows `Skipping MCS calibration`.

---

# 10. `OPT_POST_SETUP_STAGE2` → `ad9088_jesd204_post_setup_stage2`

**Runs:** per_device. **What it does:** arms the trigger-based multi-chip sync,
guarded by `phy->trig_sync_en`. With `trig_sync_en = false` (this profile) the
whole body is skipped and it returns DONE; the HAL sequence below is ported for
fidelity and future multi-sync use. On teardown it disables trig sync and clears
the trig reset.

### 1. `adi_apollo_clk_mcs_sync_trig_map(device, ADI_APOLLO_RX_TX_ALL, ADI_APOLLO_TRIG_PIN_A0)`

**What it does:** routes trigger pin **A0** as the sync source for both Rx and Tx
so a single external trigger aligns both datapaths.

### 2. `adi_apollo_clk_mcs_trig_sync_enable(device, 0)`

**What it does:** clears the trig-sync arm bit first, so the subsequent resync
starts from a known state (resync Rx/Tx digital only during trig sync).

### 3. `adi_apollo_clk_mcs_trig_reset_disable(device)`

**What it does:** disables the (previous) trigger-driven reset before re-arming.

### 4. `adi_apollo_clk_mcs_trig_reset_dsp_enable(device)`

**What it does:** enables the DSP/FSRC reset to be driven by the incoming
trigger, so the framer/sample-rate counters reset on the trigger edge.

### 5. `adi_apollo_clk_mcs_trig_sync_enable(device, 1)`

**What it does:** arms trig sync. Apollo then **waits for a trigger from the
FPGA**; on receipt the FSRC is reset. `trig_sync` is **not self-clearing** (it is
cleared later in stage 4 / on teardown). Each call is `pr_err`-checked.

---

# 11. `OPT_POST_SETUP_STAGE3` → `ad9088_jesd204_post_setup_stage3`

**Runs:** per_device. **What it does:** if `phy->triq_req_gpio` is present **and**
`trig_sync_en` is set, it pulses the trigger-request GPIO high for ~1 µs then low
(`no_os_gpio_set_value(1)` / `no_os_udelay(1)` / `no_os_gpio_set_value(0)`) — the
FPGA-side request that produces the trigger stage 2 armed for. Inert for this
profile (`trig_sync_en = false`, no `triq_req_gpio` wired). No HAL calls; pure
GPIO.

---

# 12. `OPT_POST_RUNNING_STAGE` → `ad9088_jesd204_post_setup_stage4`

**Runs:** per_device. **What it does:** the final bring-up op. Optionally
validates the trigger-sync phase (only if `trig_sync_en`), prints the SYSREF
phase, **unfreezes the ADC background calibration**, and marks the device
initialized. On teardown it clears `phy->is_initialized` and returns.

### Trig-phase check block (only if `phy->trig_sync_en`, inert here)

- **`adi_apollo_hal_bf_wait_to_set(device, BF_TRIGGER_SYNC_DONE_A0_INFO(MCS_SYNC_MCSTOP0), 1000000, 100)`**
  — polls the "trigger sync done" bitfield until set (timeout/interval args),
  i.e. waits for the trigger armed in stage 2 to actually land.
- **`adi_apollo_clk_mcs_trig_phase_get(device, ADI_APOLLO_TRIG_PIN_A0, &phase, &phase1)`**
  — reads back the measured trigger phase relative to the internal SYSREF.
- `period_fs` is computed from `dev_clk_freq_Hz` via `no_os_div64_u64_rem`, and
  the measured `phase` is validated against a 25%–75%-of-period safe window
  (centered on the ideal 50%); outside it a `pr_warning` flags risk of ±1 SYSREF
  cycle latency jitter. Then `trig_sync_enable(0)` + `trig_reset_disable`
  disarm the trigger path.

### Unconditional tail (always runs on INIT)

- **`ad9088_print_sysref_phase(phy)`** (`ad9088.c`) — loops 5×, calls
  `adi_apollo_clk_mcs_sysref_phase_get` and prints
  `no_os_sign_extend32(sysref_phase, 9)` as `SYSREF_PHASE = N (TRYn)`. With no
  MCS/trig-sync active it reads 0; becomes meaningful after Step 8.
- **`adi_apollo_adc_bgcal_unfreeze(device, ADI_APOLLO_ADC_ALL | ..._4T4R)`** —
  the counterpart to the `adc_bgcal_freeze` done in `LINK_SETUP`: now that the
  link is up and calibrated, background ADC cal is resumed to track drift during
  operation. `pr_err`-checked.
- **`phy->is_initialized = true`** — marks the chip fully brought up.

**HW-verified on VCU118 (2026-07-27):** log shows `SYSREF_PHASE = 0 (TRY0..4)`,
`adc_bgcal_unfreeze` returns OK, `is_initialized` set, and the FSM reaches
"Project configured" with no `STATE_CHANGE_ERROR`.

> **no-OS deviations:** kernel MCS cal, adf4030 background serial alignment,
> `hsci_disable_after_initial_configuration`, and DEBUG-only sync_input_count /
> mcs status prints are **not ported** (deferred / un-ported infrastructure).
> `sign_extend32`→`no_os_sign_extend32`, `div64_u64`→`no_os_div64_u64_rem`,
> `udelay`→`no_os_udelay`; `period_fs` cast to `(unsigned int)` for `%u`.

---

## Full call sequence, in FSM order

| Op | # | Function | Role |
|----|---|----------|------|
| DEVICE_INIT | — | *(no-op on init)* | teardown slot only |
| LINK_INIT | 1 | `jesd204_link_get_rate_khz` | validate/compute lane rate |
| LINK_SETUP | 1-4 | `adi_apollo_jtx/jrx_link_enable_set` | enable A0\|B0, disable A1\|B1 |
| LINK_SETUP | 5 | `adi_apollo_rxen_pwrup_ctrl_set` | SPI-driven Rx enable |
| LINK_SETUP | 6 | `adi_apollo_txen_pwrup_ctrl_set` | SPI-driven Tx enable |
| LINK_SETUP | 7-10 | `adi_apollo_rx/txmisc_dp_reset` | datapath reset pulse |
| LINK_SETUP | 11 | `adi_apollo_clk_mcs_subclass_set` | program JESD subclass |
| LINK_SETUP | 12 | `adi_apollo_clk_mcs_sysref_en_set` | enable SYSREF rx (subclass 1) |
| LINK_SETUP | 13 | `adi_apollo_adc_bgcal_freeze` | freeze ADC bg cal for sync |
| LINK_SETUP | 14 | `adi_apollo_clk_mcs_dyn_sync_sequence_run` | sync digital root clocks |
| LINK_SETUP | 15 | `adi_apollo_clk_mcs_dyn_sync_rxtxlinks_sequence_run` | sync FIFOs + links |
| STAGE1 | 1 | `adi_apollo_adc_nyquist_zone_set` | select Nyquist zone |
| STAGE1 | 2 | `adi_apollo_cfg_clk_cond_cal_cfg_set` | clk-cond cal mode |
| STAGE1 | 3 | `adi_apollo_sysclk_cond_cal` | **run** clk-cond cal (SERDES prereq) |
| STAGE1 | 4 | `ad9088_inspect_jrx/jtx_link_all` | read back + print link cfg |
| STAGE1 | 5 | `adi_apollo_jrx_phase_adjust_calc` | compute JRx phase offset |
| STAGE1 | 6 | `adi_apollo_jrx_phase_adjust_set` | apply JRx phase offset |
| STAGE1 | 7 | `adi_apollo_jtx_phase_adjust_set` | JTx phase offset (0) |
| STAGE1 | 8 | `adi_apollo_adc_init_cal_start` | **start** ADC cal (non-blocking) |
| STAGE2 | 1 | `adi_apollo_adc_init_cal_complete` | **wait** for ADC cal |
| STAGE2 | 2 | `adi_apollo_adc_nyquist_zone_set` | re-apply Nyquist zone |
| STAGE2 | 3 | `adi_apollo_clk_mcs_dyn_sync_rxtxlinks_sequence_run` | re-sync links |
| STAGE2 | 4 | *(deferred)* bmem CDDC/FDDC delay cal | not ported |
| CLOCKS_ENABLE | a | `adi_apollo_serdes_jrx_bgcal_freeze` | teardown, >16 Gbps |
| CLOCKS_ENABLE | b | `adi_apollo_serdes_jrx_init_cal` | **SERDES JRx cal**, >8 Gbps |
| CLOCKS_ENABLE | c | `adi_apollo_serdes_jrx_bgcal_unfreeze` | restart bg cal, >16 Gbps |
| CLOCKS_ENABLE | d | `adi_apollo_jtx_link_enable_set` | enable framer (RX links) |
| LINK_ENABLE | 1 | `adi_apollo_jrx_link_enable_set` | enable deframer (TX links) |
| LINK_ENABLE | * | framework SYSREF (`post_state_sysref`) | leave WAIT_BS |
| LINK_RUNNING | a | `adi_apollo_jrx_phase_diff_get` | JRx phase diff (TX links) |
| LINK_RUNNING | b | `adi_apollo_jrx_link_status_get` + `jrx_j204c/b_lane_status_get` | JRx link + per-lane status (TX links) |
| LINK_RUNNING | c | `adi_apollo_jtx_link_status_get` | JTx link status (RX links) |
| POST_SETUP_STAGE1 | — | *(stub)* | print "Skipping MCS calibration" (Step 8) |
| POST_SETUP_STAGE2 | 1 | `adi_apollo_clk_mcs_sync_trig_map` | route trig pin A0 (if trig_sync_en) |
| POST_SETUP_STAGE2 | 2-5 | `adi_apollo_clk_mcs_trig_sync_enable`/`trig_reset_disable`/`trig_reset_dsp_enable` | arm trig sync (if trig_sync_en) |
| POST_SETUP_STAGE3 | 1 | `no_os_gpio_set_value` pulse | trig-req GPIO (if triq_req_gpio && trig_sync_en) |
| POST_RUNNING | a | `adi_apollo_hal_bf_wait_to_set` + `adi_apollo_clk_mcs_trig_phase_get` | trig-phase check (if trig_sync_en) |
| POST_RUNNING | b | `ad9088_print_sysref_phase` → `adi_apollo_clk_mcs_sysref_phase_get` | print SYSREF phase ×5 |
| POST_RUNNING | c | `adi_apollo_adc_bgcal_unfreeze` | resume ADC bg cal |
| POST_RUNNING | d | `phy->is_initialized = true` | mark chip initialized |

---

## no-OS deviations summary

1. **Nyquist zone `+1`** (STAGE1/2) — no-OS stores `zone - 1`; HAL wants 1/2.
2. **Warmboot/NVM cal paths not ported** — clk-cond, ADC init, and SERDES
   init-cal configs hardcoded to run-from-scratch (`ENABLED`).
3. **Block-memory CDDC/FDDC sample-delay cal deferred** (STAGE2 step 4).
4. **`ad9088_check_apollo_error` is 2-arg** in no-OS (drops the kernel's `dev`).
5. **POST-stage MCS calibration deferred** (POST_SETUP_STAGE1 → Step 8); adf4030
   serial alignment, hsci disable-after-config, and DEBUG mcs prints not ported.
6. **Trig-sync paths compiled but inert** (`trig_sync_en = false`) — POST stages
   2/3 and the phase-check in stage 4 are ported for fidelity, not exercised.

Verified on VCU118 (2026-07-24): the full path DEVICE_INIT → LINK_ENABLE brings
the link to DATA with `WAIT_BS` cleared. `LINK_RUNNING` (status check) added and
**HW-verified 2026-07-27** (all 8 lanes "Link is good", User status Ready). The
`OPT_POST_SETUP_STAGE1/2/3` + `OPT_POST_RUNNING_STAGE` ops are now implemented and
**HW-verified 2026-07-27** — the AD9088 runs the full FSM to "Project configured"
with `is_initialized` set and ADC bg cal unfrozen. See
`JESD204_DEVICE_STATES.md` for the per-device participation matrix and
`PORTING_GUIDE.md` (Step 5) for the topology/init wiring.
