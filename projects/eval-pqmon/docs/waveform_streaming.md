# Waveform Streaming - Client Guide

## Overview

The eval-pqmon firmware provides high-throughput SINC4 waveform capture from the
ADE9430 AFE alongside PQLib metrology data. Two data paths are available:

| Path | Transport | Data | Rate |
|------|-----------|------|------|
| USB raw stream | CDC-ACM (COM port) | Binary waveform frames | ~740 KB/s, 250 fps |
| UART2 CSV | Serial (460800 baud) | PQM metrology + harmonics | 5 fps |

Both can run simultaneously. The USB port is shared between IIO (config/calibration)
and raw waveform streaming - they are mutually exclusive.

## Architecture

```
 Host PC                          MAX32650 + ADE9430
 ========                         ==================

 +-----------+    USB CDC-ACM     +------------------+
 | iio_attr  |<--- EP3 IN/OUT -->| IIO (config/cal) |
 +-----------+    COM port        +------------------+
       |                                  |
       | raw_stream_enable=1              | (exclusive switch)
       v                                  v
 +-----------+    USB CDC-ACM     +------------------+     +--------+
 | pyserial  |<--- EP3 IN ------| Raw waveform DMA |<----| ADE9430|
 | (binary)  |    same COM port   | 32kHz SINC4      |     | WFB    |
 +-----------+                    +------------------+     +--------+
                                          |
 +-----------+    UART2 (460800)  +------------------+
 | pyserial  |<--- TX ----------| UART CSV export  |<-- PQLib output
 | (CSV)     |    separate port   +------------------+     (5x/sec)
 +-----------+
```

## Prerequisites

```bash
pip install pyserial
```

For IIO attribute control, one of:
- `iio_attr` CLI (from libiio, usually in PATH after installing libiio)
- `pylibiio` Python package

## Frame Protocol

Each waveform frame is **3088 bytes**:

```
Offset  Size  Field          Description
------  ----  -----          -----------
0       4     sync_word      0xDEADBEEF (little-endian)
4       4     block_count    Monotonic frame counter (1, 2, 3, ...)
8       4     seq_cycle      PQLib 10/12-cycle sequence number
12      4     seq_1012       PQLib 1012-cycle sequence number
16      3072  samples        128 samples x 6 channels x 4 bytes (int32 LE)
```

### Sample layout (3072 bytes = 768 int32 values)

Samples are **interleaved per time step** (sample-major order):

```
[s0_IA, s0_VA, s0_IB, s0_VB, s0_IC, s0_VC,
 s1_IA, s1_VA, s1_IB, s1_VB, s1_IC, s1_VC,
 ...
 s127_IA, s127_VA, s127_IB, s127_VB, s127_IC, s127_VC]
```

Channel order per sample: IA, VA, IB, VB, IC, VC (current/voltage pairs per phase).

To access channel `ch` at sample `s`: `samples[s * 6 + ch]`

Sample values are raw 24-bit SINC4 filter output from ADE9430, sign-extended to int32.

### Timing

- Sample rate: 32 kHz per channel
- Block size: 128 samples (4 ms per block)
- Frame rate: ~250 fps
- Throughput: 250 x 3088 = ~752 KB/s (actual ~715 KB/s due to USB scheduling)

## IIO Attributes

Control streaming via IIO before enabling the raw stream:

| Attribute | R/W | Values | Description |
|-----------|-----|--------|-------------|
| `raw_stream_enable` | RW | 0, 1 | Start/stop raw streaming |
| `raw_stream_drop_count` | R | uint32 | Frames dropped (TX busy) |
| `waveform_capture_mode` | RW | disabled, continuous, oneshot | Capture mode |
| `waveform_oneshot_blocks` | RW | uint32 | Blocks to capture in oneshot mode |
| `waveform_block_count` | R | uint32 | Current block counter |

## Host Workflow

### 1. Connect and verify IIO

```bash
iio_info -u serial:COM34,115200,8n1
```

### 2. Enable raw streaming

```bash
# Simple: just enable the stream directly
iio_attr -u serial:COM34,115200,8n1 -d pqm raw_stream_enable 1
```

Or for oneshot (auto-stops after N blocks):
```bash
iio_attr -u serial:COM34,115200,8n1 -d pqm waveform_capture_mode oneshot
iio_attr -u serial:COM34,115200,8n1 -d pqm waveform_oneshot_blocks 500
iio_attr -u serial:COM34,115200,8n1 -d pqm raw_stream_enable 1
```

### 3. Read binary data from the same COM port

Once streaming is enabled, the COM port outputs raw binary frames instead of
IIO protocol. Open it with pyserial and read frames by syncing on `0xDEADBEEF`.

### 4. Stop streaming

Options:
- **Oneshot**: automatically stops after the configured number of blocks
- **Disconnect**: close the COM port - firmware detects USB disconnect and stops
- **Reconnect + IIO**: after stopping, IIO resumes on the same COM port

## Stopping and Recovery

When streaming stops (oneshot complete or USB disconnect):
1. Firmware sets `waveform_streaming_active = false`
2. `usb_raw_stream_stop()` is called
3. `iio_step()` resumes in the main loop
4. Host can reconnect and use IIO normally

## UART2 CSV Format

Parallel to USB waveform, UART2 outputs PQM metrology at ~5 fps (460800 baud).
Each line is a comma-separated frame with 350 fields:

```
PQM,<seq_1012>,<block_count>,
VA_RMS, VB_RMS, VC_RMS, IA_RMS, IB_RMS, IC_RMS, IN_RMS,
VA_THD, VB_THD, VC_THD, IA_THD, IB_THD, IC_THD,
ANGL_VA_VB, ANGL_VA_VC, ANGL_IA_IB, ANGL_IA_IC,
VA_H2..VA_H50, VB_H2..VB_H50, VC_H2..VC_H50,
IA_H2..IA_H50, IB_H2..IB_H50, IC_H2..IC_H50,
U2, U0, I2, I0,
SPOS_V, SNEG_V, SZRO_V, SPOS_I, SNEG_I, SZRO_I,
DIP_COUNT, SWELL_COUNT, RVC_COUNT, INTRP_COUNT,
AP_A, AP_B, AP_C,
AE_A, AE_B, AE_C,
RE_A, RE_B, RE_C,
PF_A, PF_B, PF_C,
FAP_A, FAP_B, FAP_C,
FAE_A, FAE_B, FAE_C,
FRE_A, FRE_B, FRE_C,
DPF_A, DPF_B, DPF_C\r\n
```

### Field breakdown (350 total)

| Offset | Count | Fields |
|--------|-------|--------|
| 0 | 1 | Header: `PQM` |
| 1 | 1 | `seq_1012` — PQLib 1012-cycle sequence number |
| 2 | 1 | `block_count` — waveform block counter (correlates with USB stream) |
| 3–9 | 7 | RMS: VA, VB, VC, IA, IB, IC, IN (volts / amps) |
| 10–15 | 6 | THD: VA, VB, VC, IA, IB, IC (%) |
| 16–19 | 4 | Phase angles: VA-VB, VA-VC, IA-IB, IA-IC (degrees) |
| 20–68 | 49 | VA harmonics H2..H50 (% of fundamental) |
| 69–117 | 49 | VB harmonics H2..H50 |
| 118–166 | 49 | VC harmonics H2..H50 |
| 167–215 | 49 | IA harmonics H2..H50 |
| 216–264 | 49 | IB harmonics H2..H50 |
| 265–313 | 49 | IC harmonics H2..H50 |
| 314–317 | 4 | Unbalance ratios: U2, U0, I2, I0 (%) |
| 318–323 | 6 | Sequence magnitudes: SPOS_V, SNEG_V, SZRO_V, SPOS_I, SNEG_I, SZRO_I |
| 324–327 | 4 | Event counters: DIP_COUNT, SWELL_COUNT, RVC_COUNT, INTRP_COUNT |
| 328–330 | 3 | Active Power: AP_A, AP_B, AP_C (watts) |
| 331–333 | 3 | Active Energy: AE_A, AE_B, AE_C (watt-hours) |
| 334–336 | 3 | Reactive Energy: RE_A, RE_B, RE_C (VAR-hours) |
| 337–339 | 3 | Power Factor: PF_A, PF_B, PF_C (ratio) |
| 340–342 | 3 | Fundamental Active Power: FAP_A, FAP_B, FAP_C (watts) |
| 343–345 | 3 | Fundamental Active Energy: FAE_A, FAE_B, FAE_C (watt-hours) |
| 346–348 | 3 | Fundamental Reactive Energy: FRE_A, FRE_B, FRE_C (VAR-hours) |
| 349–351 | 3 | Displacement Power Factor: DPF_A, DPF_B, DPF_C (ratio) |

The `seq_1012` and `block_count` fields allow correlation with USB waveform frames
(each seq_1012 period contains ~50 waveform blocks at 250 fps).

## Example: Parallel Streaming Client

See `pqmon_stream_client.py` for a complete working example.

### Usage

```bash
# Basic: USB waveform + UART2 metrology, 10 seconds
python docs/pqmon_stream_client.py COM34 COM19 --duration 10

# USB only, 5 seconds
python docs/pqmon_stream_client.py COM34 --duration 5

# Oneshot mode (auto-stops after 500 blocks)
python docs/pqmon_stream_client.py COM34 COM19 --duration 10 --oneshot 500

# Custom output prefix
python docs/pqmon_stream_client.py COM34 COM19 --duration 5 --output mytest

# No file save (just print summary)
python docs/pqmon_stream_client.py COM34 COM19 --duration 5 --no-save
```

### Output files

By default, saves two CSV files:
- `capture_waveform.csv` — one row per sample:
  `sample_idx, block_count, gap_from_prev, seq_cycle, seq_1012, ia, va, ib, vb, ic, vc`
- `capture_metrology.csv` — raw UART2 PQM lines (350 fields per line, see format above)

### Real-time callback

For live processing, the `read_usb_raw()` function accepts a callback:

```python
from pqmon_stream_client import read_usb_raw, configure_and_start

configure_and_start("COM34")

def on_frame(frame, samples):
    va_rms = sum(abs(samples[s*6 + 1]) for s in range(128)) / 128
    print(f"Block {frame['block_count']}: VA_avg={va_rms:.0f}")

# duration=0 streams indefinitely (Ctrl+C to stop)
frames, data = read_usb_raw("COM34", duration=0, callback=on_frame)
```

### Example output

```
Configuring via IIO on COM34...
  Mode: continuous
  raw_stream_enable = 1
Streaming started.

Reading for 5.0s...

Collected 3864096 bytes in 5.28s (715.0 KB/s)
Read calls: 1251

============================================================
  USB WAVEFORM STREAM
============================================================
  Frames received:  1251
  First block#:     1
  Last block#:      1251
  Continuity:       1251/1251 (100.0%)
  Throughput:       714.8 KB/s
  Frame rate:       237.0 fps

============================================================
  UART2 METROLOGY
============================================================
  Frames received:  25
  Frame rate:       5.0 fps
  VA RMS (last):    252.06 V

============================================================
  CORRELATION
============================================================
  Blocks per seq_1012: avg=50.0, min=49, max=53
  Expected: ~50 blocks/seq (200ms at 250fps)

Saving data:
  Waveform: 160128 samples (1251 blocks x 128 samples/ch) -> capture_waveform.csv
  Metrology: 25 frames -> capture_metrology.csv

Done. USB port closed - firmware returns to IIO mode.
```
