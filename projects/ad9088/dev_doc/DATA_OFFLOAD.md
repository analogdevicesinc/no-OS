# The AXI data offload in the RX capture path

Status: **implemented and committed** as `ee20c37387 project:ad9088: Add
data_offload reset` (2026-08-19). Two register writes in `dma_example.c`, one
base address in `parameters.h`. Hardware confirmation of the capture itself is
not recorded here yet — fill in the run below when it lands.

Written after the RX capture was found to time out or return pre-link content
because nothing in the firmware ever touched the receive `axi_data_offload`,
even though the HDL has had one in the RX path all along.

Grounded in:

- `~/Documents/GitHub/ADI_hdl/library/data_offload/data_offload_regmap.v` — the
  register map, and the only authority on the reset values
- `~/Documents/GitHub/ADI_hdl/library/data_offload/data_offload.v`,
  `library/util_do_ram/util_do_ram.v` — the storage FSM
- `system.hwh` inside `system_top_vcu118_ila.xsa` — what this design actually
  synthesised
- `projects/adrv903x/src/examples/{dma_example,iio_example}/` — where the
  sequence was ported from, and whose comments are partly wrong for this build

---

## 1. The one thing to understand first

The offload is **not** a passive FIFO on the way to the DMAC. It is a
store-and-forward buffer with its own request/response FSM, and in this design
it starts running at power-on, long before the JESD204 link exists.

Three synthesis parameters decide its behaviour, and this design's values are
not the ones the adrv903x comments assume. From `system.hwh`, identical on both
the `apollo_rx_data_offload` and `apollo_tx_data_offload` instances:

| Parameter | Value | Consequence |
|---|---|---|
| `MEM_SIZE_LOG2` | 19 | 512 KiB of storage, not adrv903x's 256 KiB |
| `AUTO_BRINGUP` | 1 | comes out of reset **by itself** at power-on |
| `HAS_BYPASS` | false | bypass mode is not built; store-and-forward always |
| `MEM_TYPE` | 0 | FPGA RAM, not external memory |
| `SRC_DATA_WIDTH` / `DST_DATA_WIDTH` | 1024 / 1024 | 128-byte beats both sides |
| `TX_OR_RXN_PATH` | 0 (rx) / 1 (tx) | selects the one-shot default, below |

To read them back for a different XSA:

```bash
cd /tmp && unzip -oq <project>/system_top_*.xsa system.hwh
python3 -c "
import re; d=open('system.hwh',encoding='utf8',errors='replace').read()
i=d.find('FULLNAME=\"/apollo_rx_data_offload/i_data_offload\"')
blk=d[d.rfind('<MODULE',0,i):d.find('</MODULE>',i)]
print('\n'.join('%s = %s'%(m.group(1),m.group(2)) for m in
  re.finditer(r'<PARAMETER NAME=\"(MEM_SIZE_LOG2|AUTO_BRINGUP|HAS_BYPASS|MEM_TYPE|TX_OR_RXN_PATH|SRC_DATA_WIDTH)\" VALUE=\"([^\"]*)\"',blk)))"
```

## 2. Reset values, from the HDL

`data_offload_regmap.v:126-134`, the `up_rstn == 1'b0` branch. These are the
values the firmware inherits if it writes nothing:

```verilog
up_sw_resetn       <= AUTO_BRINGUP;              // line 128  -> 1 here
up_oneshot         <= ~TX_OR_RXN_PATH;           // line 129  -> 1 on RX, 0 on TX
up_bypass          <= 'd0;                       // line 130
up_transfer_length <= {MEM_SIZE_LOG2{1'b1}};     // line 133  -> whole memory
```

Register addresses are byte offsets, i.e. the regmap's `14'hNN` times four:

| Offset | Name | Access | Reset value here |
|---|---|---|---|
| 0x0014 | `MEMORY_SIZE_LSB` | RO | 524288 (`MEM_SIZE[31:0]`) |
| 0x001C | `TRANSFER_LENGTH` | RW | all ones — the whole memory |
| 0x0084 | `RESETN_OFFLOAD` | RW bit 0 | **1**, because `AUTO_BRINGUP = 1` |
| 0x0088 | `CONTROL` | RW bit 0 bypass, bit 1 one-shot | bypass 0, **one-shot 1 on RX** |
| 0x0100 | `SYNC` | W, self-clearing | — |

## 3. Why the capture failed

Two of those defaults combine badly, and neither is the one the adrv903x
comments blame.

**Auto bring-up.** `up_sw_resetn <= AUTO_BRINGUP` means the IP is *already
running* when the firmware starts. It began its store phase at power-on, against
whatever the transport layer was emitting before the link came up.

**One-shot on the receive path.** `up_oneshot <= ~TX_OR_RXN_PATH` is 1 for the
RX instance. One store, one forward, then it sits holding that fill. It does not
re-arm on its own.

So by the time bring-up finishes and the DMAC asks for samples, the memory holds
a fill captured before the link existed and the store phase is long over. What
the DMAC gets is pre-link content, and if the forward already completed against
no reader it gets nothing at all and times out.

This is worth stating plainly because the adrv903x comment —
*"Leave the IP in power-on reset (RESETN=0) until the first capture"* — is
**false for this design**. It is true only where `AUTO_BRINGUP = 0`. Reading it
as a general property of the IP is what makes the failure hard to see.

## 4. The fix

A single low-high edge on `RESETN_OFFLOAD` immediately before starting the RX
DMAC. It drops the stale fill and starts a fresh store from the current
converter phase; the DMAC is started after it and back-pressures until the store
completes, so what it reads is contiguous from the first sample.

`dma_example.c`, just before `axi_dmac_transfer_start(rx_dmac, ...)`:

```c
no_os_axi_io_write(RX_DATA_OFFLOAD_BASEADDR, AXI_DO_REG_RESETN_OFFLOAD, 0);
no_os_axi_io_write(RX_DATA_OFFLOAD_BASEADDR, AXI_DO_REG_RESETN_OFFLOAD, 1);
```

plus `RX_DATA_OFFLOAD_BASEADDR` in `src/platform/xilinx/parameters.h`, mapping to
`XPAR_APOLLO_RX_DATA_OFFLOAD_I_DATA_OFFLOAD_BASEADDR` (0x7c450000 on the VCU118
ILA build). The base address was already in the generated `xparameters.h`; only
the TX one had ever been lifted into `parameters.h`.

That is the whole change. The three things deliberately **not** done are the
more useful half of this document.

### Not done: clamping the transfer to the memory size

In store-and-forward the offload can never hand over more than it holds, so a
capture longer than the memory would leave the DMAC waiting on a tail that never
arrives. Real failure mode, unreachable here:

```
capture ceiling = ADC_BUFFER_SAMPLES x LOOPBACK_CONVERTERS x 2 bytes
                = 16384 x 4 x 2 = 131072 = 128 KiB
storage         = 1 << MEM_SIZE_LOG2 = 512 KiB
```

`num_conv` above `LOOPBACK_CONVERTERS` is already rejected at
`dma_example.c:597`, so `transfer_size` cannot exceed `sizeof(adc_buffer_dma)`.
Four times the headroom, enforced by a guard that already exists. A clamp would
be a branch and an error path that nothing exercises, and if it ever did fire it
would silently shorten the capture — worse than failing loudly, for a coherence
measurement. Revisit only if a profile pushes `ADC_BUFFER_SAMPLES` past 65536
per converter, or a future XSA drops `MEM_SIZE_LOG2` below 17.

### Not done: writing TRANSFER_LENGTH

The reset default is the whole memory, which is what a capture shorter than the
memory wants; the DMAC stops at its own count. Writing it buys a shorter store
phase (~250 us at 512 KiB and 2 GB/s, against a 1000 ms timeout) and costs three
ways to get it wrong:

**The field width is build-dependent.** `data_offload_regmap.v:144`:

```verilog
up_transfer_length <= {up_wdata[MEM_SIZE_LOG2-7:0], {6{1'b1}}};
```

The writable field is `MEM_SIZE_LOG2 - 6` bits — 13 here, 12 on adrv903x's
256 KiB build. adrv903x's `dma_example` hit exactly this: it wrote `0x1000` for
a full 256 KiB, the 12-bit field wrapped to zero, and the offload forwarded an
empty memory instantly. That "12 bits" is a property of their XSA, not the IP.

**The value is a length-minus-one, twice over.** The low six bits are forced to
ones, so writing `n` stores `(n+1) x 64 - 1`. That then gets re-scaled by the
source beat width in `util_do_ram.v:138,141`:

```verilog
wr_length    <= wr_request_length[LENGTH_WIDTH-1:SRC_ADDR_ALIGN];
wr_last_beat <= ... (wr_addr == wr_length) ...
```

With `SRC_DATA_WIDTH = 1024`, `SRC_ADDR_ALIGN` is 7 (128-byte beats), so the
bytes actually stored are `(floor(n/2) + 1) x 128`. To store `bytes` you write
`bytes/64 - 2`, not `bytes >> 6`. adrv903x's `iio_example` writes `bytes >> 6`
and consequently over-stores by one beat — harmless there, since the DMAC takes
what it asked for and stops, but not a formula to copy.

**It does not survive the reset the way the adrv903x comment claims.** The
sequencing advice — write the length first "so it persists through the reset
cycle" — is about `up_sw_resetn`, which is a software reset of the datapath, not
of the register file; `up_transfer_length` only reloads on `up_rstn`. The
ordering is harmless but the stated reason is not the mechanism.

### Not done: writing CONTROL

`HAS_BYPASS` is false, and `data_offload_regmap.v:164` gates the bit on it:

```verilog
up_bypass <= up_wdata[0] & HAS_BYPASS;
```

Bypass is unwritable in this design. The K=64 CDC-FIFO priming glitch that both
adrv903x comments warn about cannot occur here. One-shot (bit 1) is left at its
RX default of 1, which is what the re-arm sequence above assumes.

## 5. Loose ends

**The SYNC register (0x0100) is unexplored.** `data_offload_regmap.v:167` is a
self-clearing pulse, and the design carries `SYNC_EXT_ADD_INTERNAL_CDC = true`,
so the IP can gate the start of a store on an external trigger. That is the
mechanism a triggered or multi-device-aligned capture would want, rather than
the software edge used here. Nothing in no-OS drives it today.

**There is no driver.** The `AXI_DO_REG_*` offsets now exist in three places
with identical values — `projects/ad9088/src/examples/dma/dma_example.c`,
`projects/adrv903x/src/examples/dma_example/dma_example.c` and that project's
`iio_example.c` — and there is no `drivers/axi_core/axi_data_offload`. A small
driver holding the register map, the `MEM_SIZE_LOG2`-derived field width and a
`_rearm()` helper is the obvious consolidation, and would have prevented both
the wrong-field-width and the wrong-reset-assumption bugs above. Left alone
deliberately: it is an upstream-facing change and PR scope is not decided here.

**The TX offload is untouched**, and correctly so. `TX_OR_RXN_PATH = 1` gives it
`up_oneshot = 0`, so it replays continuously without re-arming. The example
still reads its `MEMORY_SIZE_LSB` to size the transmit buffer, which is the
long-standing use of `AXI_DO_REG_MEMORY_SIZE_LSB`.

**`iio_example.c` has the same hole**, if it is ever revived. It has no offload
handling at all, and would need the same re-arm — per capture, in a `read_dev`
wrapper, the way `projects/adrv903x/src/examples/iio_example/iio_example.c` does
it. It cannot bring up a link today for unrelated reasons; see
`SUBCLASS1_ENABLEMENT.md`.
