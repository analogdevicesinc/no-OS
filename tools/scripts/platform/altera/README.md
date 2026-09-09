# Nios V (Agilex 5) capture helpers

`capture.py` and `plot.py` read a running no-OS application's DMA capture
buffers out of on-chip memory over JTAG and plot them, without a debugger IDE.
They are the Intel/Ashling analog of the Xilinx
`tools/scripts/platform/xilinx/` flow: `capture.py` replaces `capture.tcl`
(which uses `xsct`/`mrd`), and `plot.py` is the Xilinx `plot.py` extended to
also draw the transmit (DAC) buffer.

## Prerequisites

- **The application is running and parked.** The ad9088 `dma_example` ends in a
  `while(1)` precisely so the capture buffers stay intact and readable.
- **`ash-riscv-gdb-server` and `riscv32-unknown-elf-gdb` are on `PATH`.** Run
  from a `niosv-shell`, or export the Quartus / niosv / RiscFree `bin` dirs.
- **No `juart-terminal` open on the same cable.** The JTAG UART holds the
  USB-Blaster and would block the debug tap. The buffers are already in memory
  by the time the app parks, so close the console before capturing.

## 1. Find the buffer addresses and geometry

- **Rx** (`adc_buffer_dma`): `int16` samples, `num_channels` converters
  interleaved.
- **Tx** (`dac_buffer_dma`): `uint32` words, each packing one complex channel
  as I (low 16 bits) and Q (high 16 bits), `tx_channels` complex channels
  interleaved.

### The two addresses

`<rx_addr>` and `<tx_addr>` are the link-time addresses of the `adc_buffer_dma`
and `dac_buffer_dma` arrays. They are **not fixed constants** -- the linker
places them, so they move whenever the app is rebuilt. Never copy the example
values below; always read the ones for your own build, either way:

- **From the running app**, which prints them itself:

  ```
  DMA_EXAMPLE Rx: address=0x... samples=... channels=... bits=...
  DMA_EXAMPLE Tx: address=0x... samples=... channels=... bits=...
  ```

- **From the ELF**:

  ```
  riscv32-unknown-elf-nm build/ad9088 | grep _buffer_dma
  ```

### The other arguments

Unlike the addresses, these follow from the buffer geometry and are stable for a
given build config. The app also echoes them on the `DMA_EXAMPLE Rx:` line:

- `<num_samples>` is the **total** Rx samples across all channels =
  samples/converter x converters (e.g. `ADC_BUFFER_SAMPLES` 4096 x 8 = 32768 on
  the Agilex build). It must be a multiple of `<num_channels>`; the script
  divides it back out to samples/channel.
- `<num_channels>` is the number of interleaved Rx converters -- the same value
  the app reports as `channels=`.
- `[storage_bits]` defaults to 16 (int16, 2 bytes/sample) and is omitted when
  that holds.
- `--tx-words` / `--tx-channels` describe the Tx buffer the same way; their
  defaults (32768 words = 128 KiB / 4, 4 complex channels) match the Agilex
  build, so `--tx-addr` alone is usually enough.

## 2. Capture

```
capture.py <rx_addr> <num_samples> <num_channels> [storage_bits] [options]
```

See [section 1](#1-find-the-buffer-addresses-and-geometry) for where each
argument comes from.

Rx only:

```
python3 capture.py 0x1013a800 32768 8
```

Rx + Tx (also dump and unpack the DAC playback buffer):

```
python3 capture.py 0x1013a800 32768 8 --tx-addr 0x1011a800
```

Useful options:

| Option | Meaning |
| --- | --- |
| `--tx-addr ADDR` | Also dump the Tx buffer. |
| `--tx-words N` | uint32 words in the Tx buffer (default 32768 = 128 KiB/4). |
| `--tx-channels N` | Complex channels interleaved in the Tx buffer (default 4). |
| `--device NAME` | Target family the Ashling server expects (default `agilex-5`). |
| `--probe-serial S` / `-c` | Pick a probe when more than one is connected. |
| `--core-number N` / `-i` | Nios V core index (default: auto-detect). |
| `--keep-bin` | Keep the raw `.bin` dumps instead of removing them. |

Output (written to the current directory):

- `capture_ch{1..N}.csv` — the Rx converters.
- `capture_tx_ch{1..2*tx_channels}.csv` — the Tx I/Q streams, when `--tx-addr`
  is given.

## 3. Plot

```
python3 plot.py <num_rx_channels> [num_samples]
```

Reads `capture_ch*.csv` (and `capture_tx_ch*.csv` if present) from the current
directory and writes `capture.png`. When Tx captures are present they are drawn
above the Rx channels so the sent tone can be compared against the captured one.
`num_samples` optionally limits the plot to the first N samples per channel.

A non-interactive backend is selected automatically when no display is available
(WSL, SSH, headless), so `capture.png` is always written. On some WSL setups the
system libstdc++ shadows the one matplotlib needs; if the import fails, run it as
`env -u LD_LIBRARY_PATH python3 plot.py 8`.

## End-to-end example (AD9084-EBZ Agilex build, 8 Rx ch, 4 complex Tx ch)

```
python3 capture.py 0x1013a800 32768 8 --tx-addr 0x1011a800
python3 plot.py 8
```

The `.csv` and `capture.png` files are throwaway run artifacts — leave them out
of commits.
