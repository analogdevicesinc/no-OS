# JESD204 FSM — State Diagram and Explanation (no-OS)

This document describes the no-OS JESD204 link bring-up state machine as it
runs for the AD9088 (Apollo) project. It is grounded in the actual framework
code:

- `include/jesd204.h` — the op enum (`enum jesd204_dev_op`) and structs
- `jesd204/jesd204-core.c` — topology construction
- `jesd204/jesd204-fsm.c` — the FSM start/stop loops
- `drivers/rf-transceiver/apollo/ad9088_jesd204_fsm.c` — the AD9088 callbacks

See `PORTING_GUIDE.md` (Step 5) for the porting status and the layer analysis.

---

## Two state machines, don't conflate them

There are **two** distinct state machines involved in JESD204 bring-up:

1. **The software orchestration FSM** (this document, `jesd204-fsm.c`) — walks a
   sequence of ~20 *operations*, calling each device's registered callbacks in
   lock-step so the FPGA cores and the converter are configured in the right
   order. This is what `jesd204_fsm_start()` drives.

2. **The hardware link state machine** (inside `axi_jesd204_rx`) — a 4-state
   machine reported by the link status register:
   `RESET → WAIT_BS → BLOCK_SYNC → DATA` (64b66b), or
   `RESET → CGS → ILAS → DATA` (8b10b). This reports *link health* and is
   reached during the enable/running operations of the software FSM.

The `WAIT_BS` / `DATA` etc. states are hardware states of machine (2). The
`LINK_SETUP`, `CLOCKS_ENABLE` etc. below are operations of machine (1).

---

## State diagram (software orchestration FSM)

All 21 operations, in the exact order `jesd204_fsm_start` walks them
(`include/jesd204.h:197-217`). Each op is one "state"; the FSM advances to the
next only after **every device** has finished the current one.

> `CLK_SYNC_STAGE4` was added to the enum on 2026-08-13 for the ADF4030, which
> registers its BSYNC alignment there (matching the kernel driver). Every op after
> it shifted up by one index.

```
        ┌─────────────────────────────────────────────┐
        │  jesd204_fsm_start(reason = INIT)            │
        │  outer loop: op = 0 … __JESD204_MAX_OPS      │
        └─────────────────────────────────────────────┘
                          │
                          ▼
   ╔══════════════════════════════════════╗
   ║ 0.  DEVICE_INIT            (per-dev)  ║  allocate/reset device-level state
   ╚══════════════════════════════════════╝
                          │
                          ▼
   ╔══════════════════════════════════════╗
   ║ 1.  LINK_INIT             (per-link) ║  fill jesd204_link from profile   ◀── AD9088 impl'd
   ╚══════════════════════════════════════╝      (pure data, no HW)
                          │
                          ▼
   ╔══════════════════════════════════════╗
   ║ 2.  LINK_SUPPORTED        (per-link) ║  validate link params are legal
   ╚══════════════════════════════════════╝
                          │
                          ▼
   ╔══════════════════════════════════════╗
   ║ 3.  LINK_PRE_SETUP        (per-link) ║  set lane clock rate (adxcvr)
   ╚══════════════════════════════════════╝
                          │
                          ▼
   ┌──────────────────────────────────────┐
   │ 4.  CLK_SYNC_STAGE1       (per-link) │ ┐
   │ 5.  CLK_SYNC_STAGE2       (per-link) │ ├─ clock/SYSREF alignment
   │ 6.  CLK_SYNC_STAGE3       (per-link) │ ┘   (subclass-1 deterministic latency)
   │ 7.  CLK_SYNC_STAGE4       (per-dev)  │ ──  ADF4030 BSYNC align  ◀── impl'd
   └──────────────────────────────────────┘
                          │
                          ▼
   ╔══════════════════════════════════════╗
   ║ 8.  LINK_SETUP            (per-dev)   ║  enable JTX/JRX, datapath reset,   ◀── AD9088 impl'd
   ╚══════════════════════════════════════╝      MCS subclass, dyn-sync
                          │
                          ▼
   ┌──────────────────────────────────────┐
   │ 9.  OPT_SETUP_STAGE1      (per-dev)   │ ┐  Nyquist, clock-cond cal,
   │ 10. OPT_SETUP_STAGE2      (per-dev)   │ │  ADC init cal, bmem delay,
   │ 11. OPT_SETUP_STAGE3      (per-dev)   │ ├─ re-sync … (device-specific;
   │ 12. OPT_SETUP_STAGE4      (per-dev)   │ │   AD9088 uses STAGE1+STAGE2)
   │ 13. OPT_SETUP_STAGE5      (per-dev)   │ ┘
   └──────────────────────────────────────┘
                          │
                          ▼
   ╔══════════════════════════════════════╗
   ║ 14. CLOCKS_ENABLE         (per-link)  ║  SERDES JRX init-cal (>8 Gbps!),  ◀── AD9088 impl'd
   ╚══════════════════════════════════════╝      bgcal freeze/unfreeze, JTX en
                          │
                          ▼
   ╔══════════════════════════════════════╗
   ║ 15. LINK_ENABLE           (per-link)  ║  JRX enable                       ◀── AD9088 impl'd
   ║   (generic: top dev sets              ║      (AD9088 sets post_state_sysref;
   ║    post_state_sysref → SYSREF pulse)  ║       provider is the ADF4030, whose
   ╚══════════════════════════════════════╝       sysref_cb is a no-op — see note)
                          │
                          ▼
   ╔══════════════════════════════════════╗
   ║ 16. LINK_RUNNING          (per-link)  ║  poll link status → must reach DATA
   ╚══════════════════════════════════════╝      (this is where WAIT_BS is caught)
                          │
                          ▼
   ┌──────────────────────────────────────┐
   │ 17. OPT_POST_SETUP_STAGE1 (per-dev)   │ ┐
   │ 18. OPT_POST_SETUP_STAGE2 (per-dev)   │ ├─ link status check, IRQ setup,
   │ 19. OPT_POST_SETUP_STAGE3 (per-dev)   │ ┘   trigger-sync map
   └──────────────────────────────────────┘
                          │
                          ▼
   ╔══════════════════════════════════════╗
   ║ 20. OPT_POST_RUNNING_STAGE (per-dev)  ║  set is_initialized, final SYSREF,
   ╚══════════════════════════════════════╝      print chip/link info
                          │
                          ▼
                   ┌──────────────┐
                   │  LINK RUNNING │  (steady state — data flowing)
                   └──────────────┘

   ── jesd204_fsm_stop(reason = UNINIT) walks 20 → 0 in reverse ──▶ back to reset
```

---

## How to read the diagram

### per-device vs per-link

- A **`per_device`** op runs **once** per device per state (guarded by
  `per_device_op_done[]` in `jesd204-fsm.c`).
- A **`per_link`** op runs once **for each active link**. In the AD9088 basic
  example the active links are `FRAMER_LINK_A0_RX` (ADC → FPGA) and
  `DEFRAMER_LINK_A0_TX` (FPGA → DAC).
- Within any state, **participant devices run first** and the **top device
  (AD9088) runs last**. This ordering is what lets the FPGA cores be configured
  before the converter finalizes each stage.

### The `◀──` markers

These show where the AD9088's own callbacks plug in:

| Op | AD9088 status |
|----|---------------|
| `DEVICE_INIT` | implemented (no-op cleanup handler) |
| `LINK_INIT` | implemented (fills `jesd204_link` from profile) |
| `LINK_SETUP` | implemented (JTx/JRx en, DP reset, MCS subclass, dyn-sync) |
| `OPT_SETUP_STAGE1/2` | implemented (Nyquist, clock-cond cal, ADC init cal) |
| `CLOCKS_ENABLE` | implemented (SERDES JRx init-cal, mandatory > 8 Gbps) |
| `LINK_ENABLE` | implemented (JRX enable, `post_state_sysref = true`) |
| `LINK_RUNNING` | implemented (per-link status → must reach DATA) |
| `OPT_POST_SETUP_STAGE1/2/3` | implemented (MCS-cal stub, trig-sync arm, trig-req) |
| `OPT_POST_RUNNING_STAGE` | implemented (trig-phase check, bgcal unfreeze, `is_initialized`) |

The AD9088 op table is now fully populated — the device-side FSM port is
complete. `OPT_SETUP_STAGE3/4/5` are intentionally NULL (the kernel does not use
them either). `CLK_SYNC_STAGE1-3` belong to the hmc7044 and `CLK_SYNC_STAGE4` to
the adf4030, not the AD9088. See `JESD204_DEVICE_STATES.md` for the full
per-device matrix.

### Optional (`OPT_*`) states

If **no device** registers a callback for an `OPT_*` slot, the FSM passes
straight through it. That is why the kernel driver only defines a subset of the
stages — the framework tolerates gaps. Mandatory ops (`LINK_INIT`,
`LINK_SETUP`, `CLOCKS_ENABLE`, `LINK_ENABLE`, `LINK_RUNNING`) are the backbone.

### post_state_sysref

**Generic framework behavior:** an op can set `post_state_sysref = true` on the
**top device's** op entry. After that op completes, the framework calls
`jesd204_sysref_async()` (`jesd204-fsm.c:45-52`), which dispatches to exactly one
device — `jdev_top->jdev_sysref`, assigned from whichever topology entry carries
`is_sysref_provider` (`jesd204-core.c:116-119`). The driver does **not** fire
SYSREF manually; it declares the flag and the framework does the rest.

**Current state of this port (2026-08-13):** the AD9088 *does* set
`post_state_sysref = true` on `LINK_ENABLE`, matching the kernel. But the
provider on this board is the **ADF4030**, not the HMC7044 — per
`vcu118_ad9084.dts:639` — and `adf4030_jesd204_sysref()` is a **deliberate
no-op**: the ADF4030 emits BSYNC continuously, so there is no pulse to request.
The real alignment event is `CLK_SYNC_STAGE4`.

So the `LINK_ENABLE` SYSREF request is inert here, and that is correct rather
than a gap. The Apollo leaves `WAIT_BS` via `adi_apollo_clk_mcs_dyn_sync_*`
instead, which is why the link reaches `DATA` with 8 lanes good and no pulse ever
issued. `hmc7044_jesd204_sysref()` remains registered but unreachable in this
topology.

This is unchanged under **subclass 1** (HW-verified 2026-08-13): the link comes up
with `SYSREF Phase: Locked` while `post_state_sysref` is still a no-op. Subclass 1
changes what the status mask requires (`0x60` rather than `0x20`), not how the
deframer advances.

> The claim that this SYSREF "is the alignment event the deframer needs to
> advance out of `WAIT_BS`" appeared in earlier revisions of this doc and is not
> true for this board. See `JESD204_DEVICE_STATES.md` for the per-device op
> tables and the full provider explanation.

### Error handling / teardown

- Any callback returning an error **aborts the whole start** — the FSM stops
  advancing at that op.
- `jesd204_fsm_stop(reason = UNINIT)` walks the ops in **reverse** (20 → 0),
  calling each callback with `reason = UNINIT` so drivers can tear down in the
  opposite order they set up. This is why every callback begins with
  `if (reason != ...INIT) return JESD204_STATE_CHANGE_DONE;` (or the UNINIT
  equivalent) — the same function handles both directions.

---

## The FSM loop in code (`jesd204-fsm.c:21-54`)

```
for each OP (DEVICE_INIT -> LINK_INIT -> ... -> POST_RUNNING):
    for each link_id in the top device's link list:
        for each participant device that lists this link_id:
            if per_device callback (and not yet run this op) -> call once
            if per_link  callback                            -> call for this link
        # top device (AD9088) runs LAST for this link:
        if top device per_link for this op -> call it
            if op marked post_state_sysref -> fire SYSREF via provider
    # finally the top device per_device callback for this op:
    if top device per_device for this op -> call it
        if post_state_sysref -> fire SYSREF
```

Key properties:

1. **Stage-synchronized across devices** — every device finishes op *N* before
   anyone starts op *N+1*.
2. **`per_device` runs once per op** (guarded by `per_device_op_done[dev]`).
3. **The top device (AD9088) always runs last** within each op.
4. **SYSREF is fired by the framework** when an op sets `post_state_sysref`
   (via `jesd204_sysref_async` → the sysref provider's `sysref_cb`). Note: in
   *this port* no op currently sets that flag, so no automatic SYSREF fires yet
   — see the post_state_sysref note above.
5. `jesd204_fsm_stop` mirrors this in reverse with `reason = UNINIT`.

The `reason` argument threaded through every callback is
`JESD204_STATE_OP_REASON_INIT` on start / `UNINIT` on stop.

---

## Historical: why an early AD9088 build stalled at WAIT_BS

**Resolved 2026-07-24.** Kept because the failure signature is a useful reference
if the link ever regresses.

At that point the AD9088 `dev_data` (`jesd204_ad9088_init`) populated only
`state_ops[DEVICE_INIT]` and `state_ops[LINK_INIT]`. When the outer loop reached
`LINK_SETUP`, `CLOCKS_ENABLE` and `LINK_ENABLE`, the AD9088 slots were **NULL**
and the framework skipped them.

The FPGA `rx_jesd` / `tx_jesd` *did* run their versions of those ops, but with no
matching device-side link enable or SERDES cal from the AD9088, the converter's
framer never streamed synced data:

- **Link layer** (`axi_jesd204_rx`): stuck at `WAIT_BS` (state 1 of 4).
- **PHY layer** (`adxcvr`): elastic buffer underflows, status `0x21`.

The fix was filling in the AD9088 half of the op table, plus the board lane
crossbar. The op table is now complete and the link reaches `DATA`.

---

## Layer map (which struct is which layer)

| Struct | Layer | Role |
|---|---|---|
| `adf4382` | clocking | 20 GHz Apollo device clock |
| `hmc7044` | clocking | PLL2 fan-out: FPGA core/ref clocks, ADF4030 refin + BSYNC0 |
| `adf4030` | clocking | BSYNC/SYSREF provider — Apollo SYSREF (ch5), FPGA sysref_in (ch8) |
| `rx_adxcvr` / `tx_adxcvr` (`adxcvr`) | PHY | GTY SERDES + QPLL, elastic buffer |
| `rx_jesd` / `tx_jesd` (`axi_jesd204_rx/tx`) | link | 64b66b block sync, alignment |
| `ad9088_phy` | transport + link (device side) | framer/deframer, datapath |
| `rx_dmac` / `tx_dmac` | application | moves samples to/from memory |

- `buffer under/overflow` errors are **PHY-layer** (`adxcvr`).
- `WAIT_BS` / `BLOCK_SYNC` / `DATA` states are **link-layer**
  (`axi_jesd204_rx`).

A JESD link only runs when **both** ends — the FPGA `rx_jesd` and the AD9088
framer — are enabled and SYSREF-aligned together. That is the whole reason the
coordinated software FSM exists.

---

## Hardware link status states (machine 2, for reference)

Reported by the `axi_jesd204_rx` link status register
(`axi_jesd204_rx.c:109-128`):

| Value | 8b10b label | 64b66b label | Meaning |
|-------|-------------|--------------|---------|
| 0 | RESET | RESET | core in reset |
| 1 | WAIT FOR PHY | WAIT_BS | waiting for PHY / block sync |
| 2 | CGS | BLOCK_SYNC | code group / block sync achieved |
| 3 | DATA | DATA | link up, valid data flowing ✅ |

Bring-up succeeds when every active link reaches **DATA** during the
`LINK_RUNNING` op.
