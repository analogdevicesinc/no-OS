# IIOD Architecture Plan — k_proj (ZCU102 + high-speed "K" chip)

Status: **design proposal**. Target: turn the ad-hoc `basic_example.c` bring-up
sketch into a clean, IIOD-served control/data plane for the JESD-attached
high-speed chip. Nothing here is implemented yet; this is the blueprint.

---

## 1. Context & goals

The ZCU102 PS runs no-OS bare-metal and an **IIOD server** (`iio_app`) reachable
over **Ethernet/lwIP** (UART is the fallback transport). A high-speed chip is
attached with the **JESD204C link and its register "API" already up** (see
`basic_example.c` bring-up and `NETWORK_OVERVIEW.md`).

We must expose five concerns to a remote libiio client cleanly:

| # | Concern | Underlying HW mechanism today |
|---|---------|-------------------------------|
| 1 | **Large bitstream TX** over IIO buffers | `TX_DDR → tx_data_offload → tx_generator TPL → JESD TX` via `tx_dmac` |
| 2 | **Bitstream RX / capture** over IIO buffers | `JESD RX → rx_analyzer TPL → RX_DDR` via `rx_dmac` |
| 3 | **Register read/write** (chip + local AXI IP) | chip: `tx_generator` CMD protocol over `tx_cmd_dmac`; local: `no_os_axi_io` |
| 4 | **GPIO read/write** | `xilinx_gpio` (reset_b @ GPIO133, spare lines) |
| 5 | **Time** | `no_os_get_time()`, DMA/link timestamps |

Design principles:
- **One IIO device per concern**, so `iio_info` / `iiod` presents a self-describing
  context and each concern evolves independently.
- Reuse existing drivers (`axi_dmac`, `tx_generator`, `axi_adc_core`,
  `xilinx_gpio`) — the IIO layer is a thin adapter, no HW logic duplicated.
- Move all the register pokes now inlined in `basic_example.c` (data-offload
  arm, FSM poll, CMD sequencing) **into drivers**, exposed through IIO ops.

---

## 2. IIO context topology (the map)

```
IIO context "k_proj"
├── device  "k-tx"        (OUTPUT buffer)   large bitstream TX  ── §4
│     channels: voltage0..7 (le:s16/16, streamid attr)
│     buffer_attributes: stream_id, mode(oneshot|cyclic), tlen
│     ops: pre_enable / submit / post_disable  → tx_dmac + data_offload
│
├── device  "k-rx"        (INPUT buffer)    bitstream capture   ── §4
│     channels: voltage0..7 (le:s16/16, scan_index)
│     buffer_attributes: stream_id_expected, timeout_ms
│     ops: pre_enable / submit / post_disable  → rx_dmac
│
├── device  "k-regmap"    (control only)    chip register API   ── §5
│     debug_reg_read / debug_reg_write       → tx_generator CMD R/W
│     attributes: cmd_addr, cmd_op, cmd_numwords, last_response,
│                 streaming_read (triggers multi-word read)
│
├── device  "k-hdl-regs"  (control only)    local AXI IP regs   ── §5
│     debug_reg_read / debug_reg_write       → no_os_axi_io (windowed)
│     attributes: window (jesd_tx|jesd_rx|adxcvr|tpl_tx|tpl_rx|offload)
│
├── device  "k-gpio"      (control only)    GPIO R/W            ── §6
│     channels: gpioN (in/out), attribute value + direction
│
└── device  "k-time"      (control only)    time / stats        ── §7
      attributes: uptime_us, last_tx_us, last_rx_us,
                  tx_bytes, rx_bytes, link_status
```

Rationale for splitting `k-regmap` (chip, over JESD-CMD) from `k-hdl-regs`
(local PL registers, direct AXI): they have completely different transports,
latencies, and failure modes. Conflating them behind one address space hides
which side a failing access hit.

---

## 3. Module / file layout

All K-specific code lives **inside the project** (it is bound to this HDL /
board, so it does not belong in the shared `drivers/` tree — cf. the
`iio_admt_evb` / `iio_pqm` precedent). The reusable AXI/JESD drivers
(`iio_axi_adc`, `axi_dmac`, `tx_generator`, …) are still pulled from `$(DRIVERS)`.

```
projects/k_proj/src/
  k_iio/                       # the IIOD control + data plane
    k_data.c / k_data.h        # TX/RX buffer engine over axi_dmac + data_offload
    iio_k_tx.c / iio_k_tx.h    # IIO OUTPUT device  → k_data TX
    iio_k_rx.c / iio_k_rx.h    # IIO INPUT device   → k_data RX
    iio_k_regmap.c/.h          # IIO device → tx_generator CMD (chip registers)
    iio_k_hdlregs.c/.h         # IIO device → no_os_axi_io (local IP registers)
    iio_k_gpio.c/.h            # IIO device → xilinx_gpio
    iio_k_time.c/.h            # IIO device → no_os_get_time + shared stats
  devices/ad9081k/             # the (dummy) chip driver + its IIO wrapper
    ad9081k.c / ad9081k.h
    iio_ad9081k.c / iio_ad9081k.h
```

`k_data.[ch]` is the key new abstraction: it owns the DDR staging regions
(`TX_DDR_BASEADDR`, `RX_DDR_BASEADDR`, …), the data-offload arm/poll helpers
(`do_arm_oneshot`, `do_poll_fsm_idle` lifted verbatim from `basic_example.c`),
and cache maintenance. Both the IIO TX/RX devices and any non-IIO test code
call into it — no register offsets leak into the IIO layer.

`src/examples/`:
```
src/examples/iiod_example/iiod_example.c/.h   # replaces basic_example as the
                                              # production entry; wires context
```
`basic_example.c` stays as the manual bring-up/debug harness.

---

## 4. Large bitstream transmit & capture over IIO buffers

This is the centerpiece. libiio streams data as a sequence of **blocks**; the
device `submit()` callback is invoked per block. We map that onto the DMA path.

### 4.1 TX ("k-tx", OUTPUT) — the "large bitstream transmit"

Flow for a client `iio_buffer` push:

```
libiio client ──(iio buffer, N samples)──► iiod
  iio_app → iio.c → k-tx.pre_enable(active_mask)
      k_data_tx_prepare(): select stream_id, reset+arm data_offload
  loop per block:
    k-tx.submit(iio_device_data):
        iio_buffer_pop_scan()  → copy block into TX_DDR staging window
        Xil_DCacheFlush(range)
        k_data_tx_arm_offload(oneshot, tlen = block_bytes-1)
        axi_dmac_transfer_start(tx_dmac, {src=TX_DDR, size=block_bytes})
        axi_dmac_transfer_wait_completion(tx_dmac, timeout)
        k_data_tx_poll_offload_idle()
  k-tx.post_disable(): quiesce offload, stop dmac
```

Design decisions for **large** (multi-MB) transfers:

1. **Chunking / double-buffer.** `TX_DDR` staging window is bounded
   (currently a 1 MB slot in low DDR). A large client buffer is consumed
   block-by-block through a **ping-pong pair** of staging sub-windows so DMA of
   block *k* overlaps the memcpy/flush of block *k+1*. Block size =
   `min(offload_mem_bytes, staging_window/2)`, rounded to the L×64-bit beat.
2. **Oneshot vs cyclic** selected by a `mode` buffer attribute:
   - `oneshot` (default): each block played once → correct for finite files.
   - `cyclic`: last block armed cyclic in the data-offload → hardware repeats a
     waveform with zero CPU (maps to libiio `cyclic` buffers,
     `iio_buffer->cyclic_info`).
3. **stream_id** is a `buffer_attribute` (VEC=1 / PASS=3 / custom). Set once in
   `pre_enable`; the low bits of word[0] carry it exactly as the current
   `tx_vector`/`tx_passthrough` test streams do.
4. **Flow control / backpressure** is the DMA completion wait — the submit loop
   naturally throttles the client to line rate; no extra signalling needed.
5. **Channel/scan model:** 8 channels × `le:s16/16` (matches
   `AD9081K_TX_JESD_CONVS_PER_DEVICE=8`, 16-bit). `active_mask` from
   `pre_enable` chooses which converters are fed; interleave handled in the
   staging copy.

> For a truly opaque "load this firmware/bitstream blob" (not sample data), keep
> the byte-stream path too: a `k-regmap`/`k-tx` **binary buffer** is preferred
> over the current attribute-based `load_file` (attributes are control-channel,
> size-capped, and slow). The `load_file` stub in `iio_ad9081k.c` becomes a thin
> convenience wrapper that just pushes one oneshot block through `k_data`.

### 4.2 RX ("k-rx", INPUT) — capture

Mirror of TX, reusing the proven `iio_axi_adc` pattern (already in `src.mk`):

```
k-rx.submit(iio_device_data):
    iio_buffer_get_block(&buff)
    axi_dmac_transfer_start(rx_dmac, {dest=RX_DDR, size=block_bytes})
    axi_dmac_transfer_wait_completion(rx_dmac, timeout)
    Xil_DCacheInvalidateRange(RX_DDR, block_bytes)
    memcpy staging → buff   (or DMA straight into the block address)
    iio_buffer_block_done()
```

**Recommendation:** for RX, first try wiring the existing
`iio_axi_adc` + `rx_dmac` (the commented-out block in `basic_example.c:1011`)
before writing a bespoke `iio_k_rx.c`. It already provides channels, scan
types, and `dcache_invalidate_range`. Only fork it if the K stream framing
(streamid in word[0], PATT/ASYNC discrimination) needs custom handling — in
which case `iio_k_rx.c` wraps `iio_axi_adc` and post-processes the header.

### 4.3 DMA layer (shared, §"DMA")

All four DMAs (`tx_dmac`, `rx_dmac`, `tx_cmd_dmac`, `rx_cmd_dmac`) are already
`axi_dmac` instances. The plan:
- Keep them **owned by `k_data`**, created once at init, handed to the IIO
  devices by pointer (as `iio_axi_adc_init_param` already does).
- **IRQ mode:** they are `IRQ_DISABLED` today (poll + `wait_completion`). For
  streaming throughput, migrate TX/RX data DMAs to `IRQ_ENABLED` so `submit()`
  can block on a completion semaphore instead of busy-polling — lower CPU, lets
  lwIP service the socket. CMD DMAs can stay polled (tiny, infrequent).
- Cache coherency is centralized in `k_data` (flush before TX, invalidate after
  RX), so no IIO device open-codes `Xil_DCache*`.

---

## 5. Register read/write

Two devices, two transports.

### 5.1 `k-regmap` — chip registers via the JESD CMD "API"

The chip API *is* the `tx_generator` CMD channel. Map IIO debug register access
straight onto it so a client can `iio_reg_read/iio_reg_write` (libiio
`--direct-reg-access`) to poke the remote chip:

```
debug_reg_write(dev, reg, val):
    tx_generator_cmd_write(txgen, addr=reg, numwords=1, OP_WRITE, val)
    # optionally DMA the CMD word out via tx_cmd_dmac if stream_src=DMA

debug_reg_read(dev, reg, *val):
    tx_generator_cmd_write(txgen, addr=reg, numwords=1, OP_READ, 0)
    (wait) → read rx_analyzer saved regs 0x0C0 (data) / 0x0C4 (addr|op|stream)
    *val = data ; validate addr echo matches reg
```

- **Streaming read** (`OP_STREAMING_READ`, e.g. NUMWORDS=3 as in
  `tx_cmd_streaming_read`) is exposed as an attribute pair
  (`cmd_addr`, `cmd_numwords`) + a `streaming_read` trigger attribute returning
  the N words — this is the multi-register burst path, backed by `rx_cmd_dmac`
  into `RX_CMD_DDR`.
- All the CMD sequencing currently inlined in `basic_example.c:620-719` moves
  into `iio_k_regmap.c` / a `tx_generator_cmd_read()` helper added to the
  driver. The `rx_analyzer` response decode (data/addr/op/fifo/stream) becomes a
  `last_response` read-only attribute for debugging.

### 5.2 `k-hdl-regs` — local PL/IP registers via direct AXI

For JESD/adxcvr/TPL/data-offload debugging, expose the local AXI IP register
space through `no_os_axi_io`, windowed by a `window` attribute to avoid a flat
multi-MB address map:

```
window ∈ {jesd_tx, jesd_rx, tx_adxcvr, rx_adxcvr, tpl_tx, tpl_rx,
          tx_offload, rx_offload}
debug_reg_read(dev, off, *val)  → no_os_axi_io_read(base[window], off, val)
debug_reg_write(dev, off, val)  → no_os_axi_io_write(base[window], off, val)
```

This replaces the scattered `no_os_axi_io_read/write` diagnostics
(`data-offload 0x084/0x088/0x200`, FSM polls) with a uniform, remotely
scriptable interface — the same registers `do_arm_oneshot`/`do_poll_fsm_idle`
touch.

---

## 6. GPIO read/write — `k-gpio`

Wrap `xilinx_gpio` (`xil_gpio_ops`, `xil_gpio_param`). Two viable IIO shapes:

- **Recommended:** one IIO device with **one channel per exposed line**
  (`reset_b` = GPIO133, plus any spare board GPIO). Each channel carries:
  - `raw` attribute → `no_os_gpio_get_value` / `no_os_gpio_set_value`
  - `direction` attribute → `no_os_gpio_direction_input/output`
  This matches how Linux industrial-io / `iio:gpio` consumers expect per-line
  channels and lets `iio_attr` script them by name.
- Init pulls the reset sequence currently inlined at `basic_example.c:212-230`
  into `iio_k_gpio` (or leaves reset in the boot path and only exposes runtime
  control). Decide per line whether it is boot-only or client-controllable.

---

## 7. Time — `k-time`

A control-only device carrying global attributes, backed by `no_os_get_time()`:

- `uptime_us` — monotonic since boot.
- `last_tx_us` / `last_rx_us` — timestamp of the last completed buffer, stamped
  by `k_data` in the `submit()` completion path.
- `tx_bytes` / `rx_bytes` — running counters (throughput = Δbytes/Δt client-side).
- `link_status` — cached JESD TX/RX status (`axi_jesd204_*_status_read`).

If per-sample timestamping is later required, add a timestamp scan channel to
`k-rx` (IIO convention: `le:s64/64` channel with `scan_index` last), filled from
a free-running PL counter or `no_os_get_time` at block completion. Not needed
for v1.

Optionally back `uptime`/timeouts with `xilinx_timer` (`drivers/platform/xilinx/
xilinx_timer.c`) for higher-resolution DMA duration measurement than
`no_os_get_time` gives.

---

## 8. Initialization & integration

New entry `iiod_example()` (or a cleaned `example_main`) sequence:

```
1. Board/clock bring-up  (Si5391, adxcvr, JESD TX/RX, TPLs, ad9081k, FSM)
      → unchanged logic, lifted out of #if 0 into a k_hw_bringup() helper
2. k_data_init(&kd, {tx_dmac, rx_dmac, tx_cmd_dmac, rx_cmd_dmac,
                     tx_generator, rx_adc, data-offload bases, DDR windows})
3. iio_k_regmap_init  (needs tx_generator + rx_adc + rx_cmd_dmac)
   iio_k_hdlregs_init  (needs base-address table)
   iio_k_gpio_init     (needs xil_gpio_param)
   iio_k_time_init     (needs shared stats block in k_data)
   iio_k_tx_init / iio_k_rx_init  (need k_data)  [rx may = iio_axi_adc]
4. iio_devices[] = { k-tx, k-rx, k-regmap, k-hdl-regs, k-gpio, k-time }
5. iio_app_init(app_init_param{ devices, uart_init, lwip_param })
6. return iio_app_run(app)
```

`app_init_param` (lwIP + UART) is already correct in `basic_example.c:1046-1055`
— reuse verbatim. `read_buff` (the `iio_data_buffer` at `ADC_DDR_BASEADDR`) is
supplied per buffered device in the `IIO_APP_DEVICE(...)` macro.

---

## 9. Data-flow summary

```
                         ZCU102 PS (no-OS + IIOD)                         chip
 ┌───────────┐  eth/lwIP  ┌──────────┐   ┌──────────┐  AXI   ┌─────────┐  JESD
 │  libiio   │◄──────────►│  iiod    │──►│ k-tx     │──DMA──►│tx_offload│════►
 │  client   │  (or UART) │ iio_app  │   │ submit() │        │+tx_gen  │
 │           │            │          │◄──│ k-rx     │◄─DMA──│rx_analyz │◄════
 │           │            │          │   │ submit() │        └─────────┘
 │ reg_read  │            │          │──►│ k-regmap │──CMD DMA──► chip regs
 │ reg_write │            │          │──►│k-hdl-regs│──AXI──► PL IP regs
 │ attrs     │            │          │──►│ k-gpio   │────────► GPIO pins
 │           │            │          │──►│ k-time   │  no_os_get_time
 └───────────┘            └──────────┘   └──────────┘
```

---

## 10. Phased implementation plan

**Phase 0 — refactor (no behavior change).**
Lift the working bring-up + offload/CMD helpers out of `#if 0` in
`basic_example.c` into `k_hw_bringup()` and `k_data.[ch]`. Verify the manual
example still passes TX CMD/VEC/PASS + RX (the last-known-good state per git log).

**Phase 1 — control plane.**
`k-regmap` (chip reg R/W via CMD) + `k-hdl-regs` (local AXI) + `k-gpio`
+ `k-time`. All control-only, no buffers → quickest path to a useful remote
context. Validate with `iio_attr`, `iio_reg`.

**Phase 2 — RX capture.**
Wire `iio_axi_adc` + `rx_dmac` (uncomment `basic_example.c:1011-1044`). Validate
with `iio_readdev`. Fork to `iio_k_rx.c` only if framing needs it.

**Phase 3 — large TX bitstream.**
`iio_k_tx` OUTPUT device with chunked ping-pong `submit()`, oneshot + cyclic
modes, stream_id attribute. Validate with `iio_writedev` on a multi-MB file;
confirm throughput and offload FSM returns to idle between blocks.

**Phase 4 — hardening.**
Switch data DMAs to IRQ-driven completion; add timeouts/error attributes; wire
`k-time` throughput counters; document the context in the README.

---

## 11. Open decisions (need input)

1. **Transport priority:** Ethernet-primary with UART fallback (current), or
   UART-only for bring-up? Affects whether Phase 1 needs the GEM3/DHCP fix from
   `NETWORK_OVERVIEW.md` to be usable at all.
2. **RX:** reuse stock `iio_axi_adc`, or custom `iio_k_rx` for K stream framing
   (streamid/PATT/ASYNC discrimination in word[0])?
3. **"Bitstream" meaning:** is the large TX payload *sample data* (→ channel/
   scan model, §4.1) or an *opaque firmware blob* to the chip (→ single binary
   buffer / byte device)? This changes the channel definition.
4. **GPIO scope:** which lines are client-controllable vs boot-only reset?
5. **Register width/atomicity:** does the chip CMD API need >32-bit or
   read-modify-write atomicity guarantees exposed through IIO?
```
