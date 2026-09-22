# Altera (Nios V) capture & plot scripts

Data-acquisition helpers for the Agilex 5 / Nios V no-OS builds — the counterpart
of `tools/scripts/platform/xilinx/`. They read a capture buffer out of on-chip
memory over the **JTAG-to-Avalon master** using System Console, so the Nios V CPU
is left running (the application stays parked in its `while(1)` and the buffer is
untouched).

> **Why System Console and not a gdb server?** On WSL2 the `ash-riscv-gdb-server`
> path cannot lock the USB-Blaster over usbip. System Console is jtagd-based and
> works over usbip, so on this setup it is the only capture path that works.

## Prerequisites

- The FPGA is configured and the cable is visible to `jtagconfig` (only the local
  cable should be present — drop any remote JTAG server first).
- `system-console` is **not on PATH** by default in Quartus Pro. It lives under
  `syscon/bin`, e.g.:
  ```bash
  SC=<quartus>/syscon/bin/system-console
  ```
- Close `juart-terminal` before capturing — System Console must claim the
  USB-Blaster, and a held cable shows up as "no master service".
- The application must be parked (past its DMA capture, sitting in `while(1)`).
- Python 3 with `numpy` and `matplotlib` for `plot_loopback.py` (Agg backend, so
  it runs headless).

## `memcheck.tcl` — verify the master reaches the buffer first

Prove the JTAG master can read/write the buffer address the app prints, before
trusting a capture:

```bash
$SC --cli --script=memcheck.tcl [buffer_addr]
```

It lists every master service, reads the reset vector at the on-chip-memory base
(0x10000000 — its first word identifies which ELF is loaded), and round-trips a
pattern through the tail of the buffer. Run this whenever a capture comes back
all-zero.

## `capture.tcl` — pull the buffer to CSV

```bash
$SC --cli --script=capture.tcl <start_addr> <num_of_samples> <num_of_channels> [storage_bits]
```

- `start_addr` — the address the app prints, e.g. `DMA_EXAMPLE Rx: address=0x...`.
- `num_of_samples` — **per channel**.
- `num_of_channels` — number of interleaved channels in the buffer.
- `storage_bits` — optional, **16 only** (the sole supported width).

Writes `capture_ch<N>.csv` (unsigned raw; the plotter folds to signed) into the
current directory. Each 32-bit word holds two consecutive channels' 16-bit
samples (low word → channel, high word → next channel).

## `plot_loopback.py` — FFT / loopback analysis

Reads `capture_ch<N>.csv` from a **transmit** and a **receive** directory and
produces a 4-panel figure (TX trace, RX trace, RX envelope, RX spectrum with
SYSREF-harmonic spurs, plus RX-vs-TX phase/gain).

```bash
python3 plot_loopback.py --tx <tx_dir> --rx <rx_dir> [--out zoom.png] \
                         [--channels 8] [--bin N] [--sysref-div 128]
```

- `--tx` / `--rx` — **required**; each directory must hold `capture_ch{1..N}.csv`.
- `--out` — output image (default `zoom.png`).
- `--channels` — default 8.
- `--bin` — analysis FFT bin (auto-detected from RX if omitted).
- `--sysref-div` — SYSREF divider for spur annotation (default 128).

## End-to-end example (dma_example)

```bash
cd <no-OS>
mkdir -p dma_test/rx dma_test/tx
SC=<quartus>/syscon/bin/system-console
CAP=tools/scripts/platform/altera/capture.tcl

# Addresses & geometry come from the app's console lines
# ("DMA_EXAMPLE Rx/Tx: address=0x... samples=... channels=...").
( cd dma_test/rx && "$SC" --cli --script="$CAP" <rx_addr> 16384 8 16 )
( cd dma_test/tx && "$SC" --cli --script="$CAP" <tx_addr> 16384 8 16 )

python3 tools/scripts/platform/altera/plot_loopback.py \
        --tx dma_test/tx --rx dma_test/rx --out dma_test/zoom.png
```
