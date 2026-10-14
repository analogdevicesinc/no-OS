# JESD204 — Implemented States Per Device (AD9088 project)

This diagram shows, for **each device in the topology**, exactly which FSM ops
have a registered callback **today**. Ops with no callback for a device are not
shown — the FSM simply skips that device at that op.

Sourced directly from the `jesd204_dev_data` tables:

- `drivers/frequency/adf4030/adf4030.c` — `jesd204_adf4030_init`
- `drivers/frequency/hmc7044/hmc7044.c` — `jesd204_hmc7044_init`
- `drivers/axi_core/jesd204/axi_jesd204_rx.c` — `jesd204_axi_jesd204_rx_init`
- `drivers/axi_core/jesd204/axi_jesd204_tx.c` — `jesd204_axi_jesd204_tx_init`
- `drivers/rf-transceiver/apollo/ad9088_jesd204_fsm.c` — `jesd204_ad9088_init`

Legend: `[L]` = per_link callback · `[D]` = per_device callback · `{sysref}` =
provides `sysref_cb`.

---

## Per-device state maps

```
┌─ adf4030 (BSYNC/SYSREF provider) ──────────────── {sysref_cb: adf4030_jesd204_sysref} ┐
│                                                                                       │
│   LINK_SUPPORTED    [L]  adf4030_jesd204_link_supported (retune ODIVA to LEMC rate)   │
│   CLK_SYNC_STAGE4   [D]  adf4030_jesd204_clks_sync4 (align ch5/ch8 against ch0)       │
│                                                                                       │
│   sysref_cb is a deliberate no-op: BSYNC is continuous, so there is no pulse to       │
│   request. Alignment happens at CLK_SYNC_STAGE4 instead. HW-verified 2026-08-13.      │
│                                                                                       │
└───────────────────────────────────────────────────────────────────────────────────────┘

┌─ hmc7044 (clocking) ───────────────────────────── {sysref_cb: registered, NOT reached} ┐
│                                                                                        │
│   LINK_SUPPORTED    [L]  hmc7044_jesd204_link_supported                                │
│   CLK_SYNC_STAGE1   [D]  hmc7044_jesd204_clks_sync1                                    │
│   CLK_SYNC_STAGE2   [D]  hmc7044_jesd204_clks_sync2                                    │
│   CLK_SYNC_STAGE3   [D]  hmc7044_jesd204_clks_sync3                                    │
│   LINK_PRE_SETUP    [L]  hmc7044_jesd204_link_pre_setup (retunes ch3 = ADF4030_BSYNC0  │
│                          to the LEMC rate, divider 512 → 256, because .is_sysref)      │
│                                                                                        │
│   Still declares hmc7044_jesd204_sysref, but the topology names the ADF4030 as         │
│   .is_sysref_provider, so this callback is now unreachable (see SYSREF status).        │
│                                                                                        │
└────────────────────────────────────────────────────────────────────────────────────────┘

┌─ rx_jesd  (axi_jesd204_rx — link core, FRAMER_LINK_A0_RX) ─────────────────────────┐
│                                                                                     │
│   LINK_PRE_SETUP    [L]  axi_jesd204_rx_jesd204_link_pre_setup                      │
│   LINK_SETUP        [L]  axi_jesd204_rx_jesd204_link_setup                          │
│   CLOCKS_ENABLE     [L]  axi_jesd204_rx_jesd204_clks_enable                         │
│   LINK_ENABLE       [L]  axi_jesd204_rx_jesd204_link_enable                         │
│   LINK_RUNNING      [L]  axi_jesd204_rx_jesd204_link_running                        │
│                                                                                     │
└─────────────────────────────────────────────────────────────────────────────────┘

┌─ tx_jesd  (axi_jesd204_tx — link core, DEFRAMER_LINK_A0_TX) ───────────────────────┐
│                                                                                     │
│   LINK_PRE_SETUP    [L]  axi_jesd204_tx_jesd204_link_pre_setup                      │
│   LINK_SETUP        [L]  axi_jesd204_tx_jesd204_link_setup                          │
│   CLOCKS_ENABLE     [L]  axi_jesd204_tx_jesd204_clks_enable                         │
│   LINK_ENABLE       [L]  axi_jesd204_tx_jesd204_link_enable                         │
│   LINK_RUNNING      [L]  axi_jesd204_tx_jesd204_link_running                        │
│                                                                                     │
└─────────────────────────────────────────────────────────────────────────────────┘

┌─ ad9088_phy (TOP DEVICE — transport + device-side link) ───────────────────────────┐
│                                                                                     │
│   DEVICE_INIT       [D]  ad9088_jesd204_uninit   (no-op on INIT; teardown only)    │
│   LINK_INIT         [L]  ad9088_jesd204_link_init (fills jesd204_link from profile) │
│   LINK_SETUP        [D]  ad9088_jesd204_link_setup (JTx/JRx en, RxEn/TxEn, DP      │
│                          reset, subclass + MCS SYSREF, bgcal freeze, dyn-sync)     │
│   CLOCKS_ENABLE     [L]  ad9088_jesd204_clks_enable (SERDES JRx init-cal > 8 Gbps, │
│                          bgcal freeze/unfreeze > 16 Gbps, JTx framer en)           │
│   LINK_ENABLE       [L]  ad9088_jesd204_link_enable (JRx deframer en)              │
│                          + post_state_sysref = true → framework fires SYSREF       │
│   LINK_RUNNING      [L]  ad9088_jesd204_link_running (per-link status check:       │
│                          JTx PLL/PHASE/MODE, JRx phase-diff + per-lane "good")     │
│                          HW-verified VCU118 2026-07-27                             │
│   OPT_POST_SETUP_STAGE1 [D] ad9088_jesd204_post_setup_stage1 (MCS cal stub —      │
│                          "Skipping MCS calibration", deferred to Step 8)           │
│   OPT_POST_SETUP_STAGE2 [D] ad9088_jesd204_post_setup_stage2 (trig-sync arm —     │
│                          inert, trig_sync_en=false)                                │
│   OPT_POST_SETUP_STAGE3 [D] ad9088_jesd204_post_setup_stage3 (trig-req GPIO       │
│                          pulse — inert)                                            │
│   OPT_POST_RUNNING_STAGE [D] ad9088_jesd204_post_setup_stage4 (trig-phase check,  │
│                          SYSREF phase print, adc_bgcal_unfreeze,                   │
│                          is_initialized=true) HW-verified VCU118 2026-07-27        │
│                                                                                     │
└─────────────────────────────────────────────────────────────────────────────────┘
```

---

## Combined timeline — who fires at each op

Columns are the five devices; a marker means that device has a callback at that
op. The FSM runs ops top-to-bottom; within an op, participants run first and the
**top device (ad9088) runs last**.

```
 OP (in FSM order)        adf4030   hmc7044   rx_jesd   tx_jesd   ad9088(top)
 ──────────────────────   ───────   ───────   ───────   ───────   ───────────
 DEVICE_INIT                 ·         ·         ·         ·       [D] no-op
 LINK_INIT                   ·         ·         ·         ·       [L] fill link
 LINK_SUPPORTED             [L]       [L]        ·         ·         ·
 LINK_PRE_SETUP              ·        [L]       [L]       [L]        ·
 CLK_SYNC_STAGE1             ·        [D]        ·         ·         ·
 CLK_SYNC_STAGE2             ·        [D]        ·         ·         ·
 CLK_SYNC_STAGE3             ·        [D]        ·         ·         ·
 CLK_SYNC_STAGE4            [D]        ·         ·         ·         ·   ← BSYNC align
 LINK_SETUP                  ·         ·        [L]       [L]       [D] JTx/JRx en, dyn-sync
 OPT_SETUP_STAGE1-5          ·         ·         ·         ·         ·
 CLOCKS_ENABLE               ·         ·        [L]       [L]       [L] SERDES JRx init-cal
 LINK_ENABLE                 ·         ·        [L]       [L]       [L] JRx en + SYSREF*
 LINK_RUNNING                ·         ·        [L]       [L]       [L] status check (HW-ok)
 OPT_POST_SETUP_1            ·         ·         ·         ·        [D] MCS cal stub (Step 8)
 OPT_POST_SETUP_2            ·         ·         ·         ·        [D] trig-sync arm (inert)
 OPT_POST_SETUP_3            ·         ·         ·         ·        [D] trig-req GPIO (inert)
 OPT_POST_RUNNING            ·         ·         ·         ·        [D] bgcal unfreeze, init (HW-ok)

  [L]=per_link  [D]=per_device  ·=no callback (skipped)
  * LINK_ENABLE sets post_state_sysref=true → framework calls the provider's
    sysref_cb, which is the ADF4030's no-op. See SYSREF status below.
```

Ordering note: `jesd204_topology_init()` reads `is_sysref_provider` from the
*input* array but takes the pointer from the top-device-filtered array
(`jesd204-core.c:116-119`), so the provider must precede the top device and must
not be the last entry. The project uses **adf4030 → hmc7044 → rx_jesd → tx_jesd →
ad9088(top)**.

---

## SYSREF status

**The provider changed on 2026-08-13** — this section previously said the
`LINK_ENABLE` SYSREF pulse went to the hmc7044. It no longer does.

- The framework only calls `jesd204_sysref_async()` when the **top device** sets
  `post_state_sysref` on an op (`jesd204-fsm.c:45-52`). The AD9088 sets it on
  `LINK_ENABLE`, matching the kernel.
- `jesd204_sysref_async()` dispatches to exactly one device:
  `jdev_top->jdev_sysref` (`jesd204-core.c:359-362`), which
  `jesd204_topology_init()` assigns from whichever topology entry carries
  `is_sysref_provider` (`jesd204-core.c:116-119`).
- That flag now belongs to the **ADF4030**, per `vcu118_ad9084.dts:639`
  (`jesd204-sysref-provider` on `adf4030@4`). The hmc7044 does not source SYSREF
  on this board at all.
- **Consequence: the `LINK_ENABLE` SYSREF pulse is now a no-op by design.**
  `adf4030_jesd204_sysref()` returns 0 without touching the part, because the
  ADF4030 emits BSYNC *continuously* — there is no pulse to request. The real
  alignment event is `CLK_SYNC_STAGE4`, which trims each output channel's delay
  against the reference channel. Both mirror the kernel's ADF4030 driver.
- `hmc7044_jesd204_sysref()` is still registered but unreachable in this
  topology. Leave it alone; it is the correct callback for boards where the
  HMC7044 *is* the provider.

Hardware confirms the reading: the link reaches `DATA` with all 8 lanes good and
no SYSREF pulse is ever issued, because the Apollo leaves `WAIT_BS` via
`adi_apollo_clk_mcs_dyn_sync_*`, not SYSREF. The earlier claim that the pulse "is
the alignment event the deframer needs to leave `WAIT_BS`" was overstated even
before the provider moved.

**Still true under subclass 1 (verified 2026-08-13).** With subclass 1 the link
comes up with `SYSREF Phase: Locked` and `SYSREF captured: Yes`, yet
`post_state_sysref` remains a no-op — the ADF4030 emits BSYNC continuously, so
there is no pulse to request, and the alignment that matters happens in
`CLK_SYNC_STAGE4` instead. Subclass 1 changes what the *status mask* requires
(`0x60` instead of `0x20`), not how the deframer leaves `WAIT_BS`.

---

## Summary

| Device | Ops implemented | Role in bring-up |
|--------|-----------------|------------------|
| adf4030 | LINK_SUPPORTED, CLK_SYNC_STAGE4 (+no-op sysref_cb) | BSYNC/SYSREF provider — aligns Apollo (ch5) and FPGA (ch8) SYSREF against ch0 |
| hmc7044 | LINK_SUPPORTED, CLK_SYNC_STAGE1-3, LINK_PRE_SETUP (sysref_cb registered but unreachable) | clocking — PLL2 outputs, and retunes ch3 (ADF4030_BSYNC0) to the LEMC rate |
| rx_jesd | LINK_PRE_SETUP, LINK_SETUP, CLOCKS_ENABLE, LINK_ENABLE, LINK_RUNNING | FPGA RX link core (full) |
| tx_jesd | LINK_PRE_SETUP, LINK_SETUP, CLOCKS_ENABLE, LINK_ENABLE, LINK_RUNNING | FPGA TX link core (full) |
| ad9088_phy | DEVICE_INIT, LINK_INIT, LINK_SETUP, CLOCKS_ENABLE, LINK_ENABLE, LINK_RUNNING, OPT_POST_SETUP_STAGE1/2/3, OPT_POST_RUNNING_STAGE | top device (full FSM — core bring-up + link status + POST stages) |

The FPGA link cores are fully populated, and the converter (ad9088) now
implements the **full** FSM bring-up path: data prep (LINK_INIT), device-side
link enable + MCS dyn-sync (LINK_SETUP), SERDES JRx init-cal + framer enable
(CLOCKS_ENABLE), deframer enable + SYSREF request (LINK_ENABLE,
`post_state_sysref` — a no-op with the ADF4030 as provider),
per-link status validation (LINK_RUNNING), and the POST stages — MCS-cal stub
(POST_SETUP_STAGE1, deferred to Step 8), trig-sync arm / trig-req GPIO
(POST_SETUP_STAGE2/3, inert while `trig_sync_en=false`), and the final
POST_RUNNING stage (SYSREF phase print, `adc_bgcal_unfreeze`,
`is_initialized=true`). **Verified on HW (VCU118, 2026-07-24): link reaches
DATA, WAIT_BS cleared. LINK_RUNNING HW-verified 2026-07-27: all 8 lanes "Link is
good", User status Ready. POST stages HW-verified 2026-07-27: FSM reaches "Project
configured" with `is_initialized` set and ADC bg cal unfrozen. ADF4030
CLK_SYNC_STAGE4 HW-verified 2026-08-13: ch5 and ch8 align against ch0 with sub-UI
TDC readings. Subclass 1 HW-verified 2026-08-13: `Subclass=1` on all enabled
links, `SYSREF Phase: Locked`, `SYSREF captured: Yes` on both AXI cores, and
`SYSREF_PHASE` stable at 433 across five reads — with the MCS-cal stub still
skipping.** The device-side FSM port is now complete and running subclass 1;
remaining work is MCS calibration (Step 8, now optional — see
`SUBCLASS1_ENABLEMENT.md` Stage D) and runtime control. See `PORTING_GUIDE.md`
(Step 5 sub-steps) for the topology/init wiring and `JESD204_FSM.md` for the full
generic op sequence.
