#!/usr/bin/env python3
"""
PQM Waveform Stream Client

Reads SINC4 waveform data from USB and PQM metrology from UART2 in parallel.
Demonstrates the full host workflow: IIO configure -> stream -> stop.

Prerequisites:
    pip install pyserial

Usage:
    python pqmon_stream_client.py <USB_PORT> [UART2_PORT] [options]

    python pqmon_stream_client.py COM34 COM19 --duration 10
    python pqmon_stream_client.py COM34 COM19 --duration 5 --oneshot 500
    python pqmon_stream_client.py COM34 --duration 5   (USB only, no UART2)

Options:
    --duration N     Capture duration in seconds (default: 10)
    --oneshot N      Use oneshot mode with N blocks (otherwise continuous)
    --output PREFIX  Output file prefix (default: "capture")
    --no-save        Do not save CSV files
"""
import sys
import time
import struct
import threading
import argparse
import subprocess

try:
    import serial
except ImportError:
    print("ERROR: pyserial not found. Install: pip install pyserial")
    sys.exit(1)

import csv as csv_mod

# Protocol constants (must match firmware usb_raw_stream.h)
SYNC_WORD = 0xDEADBEEF
SYNC_BYTES = struct.pack('<I', SYNC_WORD)
HEADER_BYTES = 16
SAMPLES_PER_BLOCK = 128
NUM_CHANNELS = 6  # ia, va, ib, vb, ic, vc (interleaved per sample)
DATA_SAMPLES = SAMPLES_PER_BLOCK * NUM_CHANNELS  # 768
DATA_BYTES = DATA_SAMPLES * 4  # 3072
FRAME_BYTES = HEADER_BYTES + DATA_BYTES  # 3088

CHANNELS = ["ia", "va", "ib", "vb", "ic", "vc"]
UART2_BAUD = 460800


def iio_attr_write(port, attr, value, expect_disconnect=False):
    """Write an IIO device attribute via iio_attr CLI.

    Args:
        expect_disconnect: If True, ignore EIO errors (expected when the
            write triggers a mode switch that kills the IIO connection).
    """
    uri = f"serial:{port},115200,8n1"
    cmd = ["iio_attr", "-u", uri, "-d", "pqm", attr, str(value)]
    result = subprocess.run(cmd, capture_output=True, text=True, timeout=5)
    if result.returncode != 0 and not expect_disconnect:
        print(f"  WARNING: iio_attr {attr}={value} returned: {result.stderr.strip()}")
    return result.returncode == 0


def iio_attr_read(port, attr):
    """Read an IIO device attribute via iio_attr CLI."""
    uri = f"serial:{port},115200,8n1"
    cmd = ["iio_attr", "-u", uri, "-d", "pqm", attr]
    result = subprocess.run(cmd, capture_output=True, text=True, timeout=5)
    if result.returncode == 0:
        return result.stdout.strip()
    return None


def configure_and_start(port, oneshot_blocks=None):
    """Configure IIO and enable raw streaming."""
    print(f"Configuring via IIO on {port}...")

    if oneshot_blocks:
        iio_attr_write(port, "waveform_capture_mode", "oneshot")
        iio_attr_write(port, "waveform_oneshot_blocks", oneshot_blocks)
        print(f"  Mode: oneshot ({oneshot_blocks} blocks)")
    else:
        print("  Mode: continuous")

    iio_attr_write(port, "raw_stream_enable", 1, expect_disconnect=True)
    print("  raw_stream_enable = 1")
    print("Streaming started.\n")


def uart2_csv_reader(port, frames, stop_event):
    """Read PQM metrology CSV frames from UART2."""
    try:
        ser = serial.Serial(port, UART2_BAUD, timeout=1)
    except serial.SerialException as e:
        print(f"  UART2: Could not open {port}: {e}")
        return

    line_buf = ""
    while not stop_event.is_set():
        data = ser.read(ser.in_waiting or 1)
        if not data:
            continue
        line_buf += data.decode('ascii', errors='replace')
        while '\n' in line_buf:
            line, line_buf = line_buf.split('\n', 1)
            line = line.strip()
            if not line.startswith('PQM,'):
                continue
            fields = line.split(',')
            if len(fields) < 10:
                continue
            try:
                frames.append({
                    'seq_1012': int(fields[1]),
                    'block_count': int(fields[2]),
                    'va_rms': float(fields[3]),
                    'timestamp': time.time(),
                    'raw_line': line,
                })
            except (ValueError, IndexError):
                continue

    ser.close()


def read_usb_raw(port, duration, callback=None):
    """Read USB waveform stream, parsing frames in real-time.

    Each complete frame is appended to the returned lists immediately.
    If callback is provided, it is called with (frame_dict, samples_tuple)
    for each frame — useful for live visualization or processing.

    Args:
        port: COM port string
        duration: capture duration in seconds (0 = indefinite, stop with Ctrl+C)
        callback: optional function(frame, samples) called per frame in real-time

    Returns:
        (usb_frames, frame_data) — lists of parsed frames and sample tuples
    """
    try:
        ser = serial.Serial(port, baudrate=115200, timeout=1)
    except serial.SerialException as e:
        print(f"  USB: Could not open {port}: {e}")
        return [], []

    ser.reset_input_buffer()
    buf = bytearray()
    usb_frames = []
    frame_data = []
    resyncs = 0
    total_bytes = 0
    reads = 0
    t0 = time.time()

    try:
        while True:
            if duration > 0 and (time.time() - t0) >= duration:
                break

            waiting = ser.in_waiting
            if waiting > 0:
                chunk = ser.read(min(waiting, 65536))
            else:
                chunk = ser.read(4096)
            if not chunk:
                continue

            buf.extend(chunk)
            total_bytes += len(chunk)
            reads += 1

            # Parse all complete frames from buffer
            while len(buf) >= FRAME_BYTES:
                idx = buf.find(SYNC_BYTES)
                if idx < 0:
                    buf = buf[-3:]
                    break
                if idx > 0:
                    resyncs += 1
                    buf = buf[idx:]

                if len(buf) < FRAME_BYTES:
                    break

                block_count, seq_cycle, seq_1012 = struct.unpack_from(
                    '<III', buf, 4)
                samples = struct.unpack_from(
                    f'<{DATA_SAMPLES}i', buf, HEADER_BYTES)

                if block_count != 0:
                    frame = {
                        "block_count": block_count,
                        "seq_cycle": seq_cycle,
                        "seq_1012": seq_1012,
                    }
                    usb_frames.append(frame)
                    frame_data.append(samples)
                    if callback:
                        callback(frame, samples)

                buf = buf[FRAME_BYTES:]

    except KeyboardInterrupt:
        print("\n  USB reader interrupted.")

    ser.close()
    elapsed = time.time() - t0

    print(f"\nCollected {total_bytes} bytes in {elapsed:.2f}s "
          f"({total_bytes/1024/elapsed:.1f} KB/s)")
    print(f"Read calls: {reads}")
    if resyncs:
        print(f"Resyncs: {resyncs}")

    return usb_frames, frame_data


def print_summary(usb_frames, uart_frames, elapsed, resyncs=0):
    """Print stream analysis summary."""
    print(f"\n{'='*60}")
    print(f"  USB WAVEFORM STREAM")
    print(f"{'='*60}")
    print(f"  Frames received:  {len(usb_frames)}")

    if usb_frames:
        first_blk = usb_frames[0]['block_count']
        last_blk = usb_frames[-1]['block_count']
        expected = last_blk - first_blk + 1
        drops = 0
        for i in range(1, len(usb_frames)):
            diff = usb_frames[i]['block_count'] - usb_frames[i-1]['block_count']
            if diff != 1:
                drops += abs(diff - 1)
        continuity = 100 * len(usb_frames) / expected if expected > 0 else 0
        throughput = len(usb_frames) * FRAME_BYTES / elapsed

        print(f"  First block#:     {first_blk}")
        print(f"  Last block#:      {last_blk}")
        print(f"  Resyncs:          {resyncs}")
        print(f"  Gaps (drops):     {drops}")
        print(f"  Continuity:       {len(usb_frames)}/{expected} ({continuity:.1f}%)")
        print(f"  Throughput:       {throughput/1024:.1f} KB/s")
        print(f"  Frame rate:       {len(usb_frames)/elapsed:.1f} fps")
        print(f"  Seq cycle:        "
              f"{usb_frames[0]['seq_cycle']} .. {usb_frames[-1]['seq_cycle']}")
        print(f"  Seq 1012:         "
              f"{usb_frames[0]['seq_1012']} .. {usb_frames[-1]['seq_1012']}")

    if uart_frames:
        print(f"\n{'='*60}")
        print(f"  UART2 METROLOGY")
        print(f"{'='*60}")
        print(f"  Frames received:  {len(uart_frames)}")
        print(f"  Seq 1012:         "
              f"{uart_frames[0]['seq_1012']} .. {uart_frames[-1]['seq_1012']}")
        print(f"  Block count:      "
              f"{uart_frames[0]['block_count']} .. {uart_frames[-1]['block_count']}")
        if len(uart_frames) >= 2:
            dt = uart_frames[-1]['timestamp'] - uart_frames[0]['timestamp']
            rate = (len(uart_frames) - 1) / dt if dt > 0 else 0
            print(f"  Frame rate:       {rate:.1f} fps")
            print(f"  VA RMS (last):    {uart_frames[-1]['va_rms']:.2f} V")

    # Correlation
    if usb_frames and uart_frames:
        print(f"\n{'='*60}")
        print(f"  CORRELATION")
        print(f"{'='*60}")
        prev_seq = usb_frames[0]['seq_1012']
        blocks_per_seq = []
        count = 1
        for f in usb_frames[1:]:
            if f['seq_1012'] != prev_seq:
                blocks_per_seq.append(count)
                prev_seq = f['seq_1012']
                count = 1
            else:
                count += 1
        if blocks_per_seq:
            avg = sum(blocks_per_seq) / len(blocks_per_seq)
            print(f"  Blocks per seq_1012: avg={avg:.1f}, "
                  f"min={min(blocks_per_seq)}, max={max(blocks_per_seq)}")
            print(f"  Expected: ~50 blocks/seq (200ms at 250fps)")


def save_waveform_csv(usb_frames, frame_data, filename):
    """Save waveform samples to CSV (one row per sample, interleaved channels).

    Format: sample_idx, block_count, gap_from_prev, seq_cycle, seq_1012, ia, va, ib, vb, ic, vc
    """
    with open(filename, 'w', newline='') as f:
        writer = csv_mod.writer(f)
        writer.writerow(["sample_idx", "block_count", "gap_from_prev",
                         "seq_cycle", "seq_1012"] + CHANNELS)
        sample_idx = 0
        for i, (fr, samples) in enumerate(zip(usb_frames, frame_data)):
            blk = fr['block_count']
            seq_c = fr['seq_cycle']
            s1012 = fr['seq_1012']
            gap = 0 if i == 0 else (blk - usb_frames[i - 1]['block_count'])
            for s in range(SAMPLES_PER_BLOCK):
                row = [sample_idx, blk, gap if s == 0 else 0,
                       seq_c if s == 0 else '', s1012 if s == 0 else '']
                for ch in range(NUM_CHANNELS):
                    row.append(samples[s * NUM_CHANNELS + ch])
                writer.writerow(row)
                sample_idx += 1

    print(f"  Waveform: {sample_idx} samples ({len(usb_frames)} blocks "
          f"x {SAMPLES_PER_BLOCK} samples/ch) -> {filename}")


def save_metrology_csv(uart_frames, filename):
    """Save UART2 metrology lines to CSV (raw PQM format)."""
    harmonics_v = [f"VA_H{h}" for h in range(2, 51)] + \
                  [f"VB_H{h}" for h in range(2, 51)] + \
                  [f"VC_H{h}" for h in range(2, 51)]
    harmonics_i = [f"IA_H{h}" for h in range(2, 51)] + \
                  [f"IB_H{h}" for h in range(2, 51)] + \
                  [f"IC_H{h}" for h in range(2, 51)]
    power_energy = ["AP_A", "AP_B", "AP_C",
                    "AE_A", "AE_B", "AE_C",
                    "RE_A", "RE_B", "RE_C",
                    "PF_A", "PF_B", "PF_C",
                    "FAP_A", "FAP_B", "FAP_C",
                    "FAE_A", "FAE_B", "FAE_C",
                    "FRE_A", "FRE_B", "FRE_C",
                    "DPF_A", "DPF_B", "DPF_C"]
    header = ["HDR", "SEQ_1012", "BLOCK_COUNT",
              "VA_RMS", "VB_RMS", "VC_RMS", "IA_RMS", "IB_RMS", "IC_RMS", "IN_RMS",
              "VA_THD", "VB_THD", "VC_THD", "IA_THD", "IB_THD", "IC_THD",
              "ANGL_VA_VB", "ANGL_VA_VC", "ANGL_IA_IB", "ANGL_IA_IC"] + \
             harmonics_v + harmonics_i + \
             ["U2", "U0", "I2", "I0",
              "SPOS_V", "SNEG_V", "SZRO_V", "SPOS_I", "SNEG_I", "SZRO_I",
              "DIP_COUNT", "SWELL_COUNT", "RVC_COUNT", "INTRP_COUNT"] + \
             power_energy

    with open(filename, 'w', newline='') as f:
        f.write(",".join(header) + "\n")
        for frame in uart_frames:
            f.write(frame['raw_line'] + "\n")

    print(f"  Metrology: {len(uart_frames)} frames -> {filename}")


def main():
    parser = argparse.ArgumentParser(
        description="PQM waveform stream client - reads USB waveform + UART2 metrology")
    parser.add_argument('usb_port', help='USB COM port (e.g., COM34)')
    parser.add_argument('uart_port', nargs='?', default=None,
                        help='UART2 COM port for metrology (e.g., COM19). Optional.')
    parser.add_argument('--duration', type=float, default=10.0,
                        help='Capture duration in seconds (default: 10)')
    parser.add_argument('--oneshot', type=int, default=None,
                        help='Oneshot mode: number of blocks to capture')
    parser.add_argument('--output', type=str, default='capture',
                        help='Output file prefix (default: "capture" -> '
                             'capture_waveform.csv, capture_metrology.csv)')
    parser.add_argument('--no-save', action='store_true',
                        help='Do not save CSV files')
    args = parser.parse_args()

    # Step 1: Configure via IIO (uses the USB port before streaming takes over)
    configure_and_start(args.usb_port, args.oneshot)

    # Small delay to let firmware switch from IIO to raw stream
    time.sleep(0.2)

    # Step 2: Start UART2 reader in background thread
    uart_frames = []
    stop_event = threading.Event()
    uart_thread = None

    if args.uart_port:
        uart_thread = threading.Thread(
            target=uart2_csv_reader,
            args=(args.uart_port, uart_frames, stop_event))
        uart_thread.start()

    # Step 3: Read USB waveform (blocking, collects all then parses)
    print(f"Reading for {args.duration}s...")
    usb_frames, frame_data = read_usb_raw(args.usb_port, args.duration)

    # Step 4: Stop UART reader
    stop_event.set()
    if uart_thread:
        uart_thread.join(timeout=3)

    # Step 5: Report results
    elapsed = args.duration
    print_summary(usb_frames, uart_frames, elapsed)

    # Step 6: Save CSV files
    if not args.no_save:
        print(f"\nSaving data:")
        if usb_frames:
            save_waveform_csv(usb_frames, frame_data,
                              f"{args.output}_waveform.csv")
        if uart_frames:
            save_metrology_csv(uart_frames, f"{args.output}_metrology.csv")

    # Note: streaming stops automatically on USB disconnect (port closed).
    # On next connection, IIO will be available again for configuration.
    print("\nDone. USB port closed - firmware returns to IIO mode.")


if __name__ == '__main__':
    main()
