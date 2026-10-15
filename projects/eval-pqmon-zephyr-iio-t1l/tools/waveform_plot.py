#!/usr/bin/env python3
"""
waveform_plot.py — Decode and plot ADE9430 binary waveform frames from UART2.

Binary frame format (all little-endian):
  SYNC[2]:  0x57 0x46 ("WF")
  SEQ[4]:   uint32_t block sequence number
  SEC[4]:   uint32_t PTP timestamp seconds
  NSEC[4]:  uint32_t PTP timestamp nanoseconds
  MASK[1]:  channel bitmask (bits 0-6 = IA,VA,IB,VB,IC,VC,IN)
  NSAMP[2]: uint16_t samples per channel (256)
  DATA[N]:  int16_t samples, interleaved per sample

NOTE: The ADE9430 WFB burst (BURST_ALL_CHAN) emits channels in the fixed
hardware order IA, VA, IB, VB, IC, VC, IN — NOT VA,VB,VC,IA,IB,IC,IN.
CHANNEL_NAMES below matches that hardware order.

Each block is 256 resampled samples = 50 ms = ~2.5 cycles at 50 Hz
(COH_PAGE_RDY fires every 256 samples). Use --batch N to capture and
concatenate N contiguous blocks into one longer plot.

Usage:
  python3 waveform_plot.py /dev/ttyUSB0            # live plot
  python3 waveform_plot.py /dev/ttyUSB0 --save      # save to PNG files
  python3 waveform_plot.py /dev/ttyUSB0 --batch 10  # capture+concat 10 blocks then plot

Copyright (c) 2026 Analog Devices, Inc.
SPDX-License-Identifier: BSD-3-Clause
"""

import argparse
import struct
import sys
import threading
import time
from datetime import datetime, timezone

import serial
import numpy as np

SYNC = bytes([0x57, 0x46])
HEADER_SIZE = 17  # SYNC(2) + SEQ(4) + SEC(4) + NSEC(4) + MASK(1) + NSAMP(2)

# ADE9430 WFB BURST_ALL_CHAN hardware order (NOT VA,VB,VC,IA,...).
CHANNEL_NAMES = ["IA", "VA", "IB", "VB", "IC", "VC", "IN"]


def mask_to_channels(mask):
    """Return list of (index, name) for active channels."""
    channels = []
    for i in range(7):
        if mask & (1 << i):
            channels.append((i, CHANNEL_NAMES[i]))
    return channels


def format_ptp_time(sec, nsec):
    """Format PTP timestamp as human-readable string."""
    if sec == 0 and nsec == 0:
        return "no PTP"
    try:
        dt = datetime.fromtimestamp(sec, tz=timezone.utc)
        return f"{dt.strftime('%H:%M:%S')}.{nsec // 1000000:03d}"
    except (OSError, ValueError):
        return f"{sec}.{nsec:09d}"


def sync_to_frame(ser):
    """Read bytes until we find the SYNC pattern."""
    buf = b""
    while True:
        b = ser.read(1)
        if not b:
            continue
        buf += b
        if len(buf) >= 2 and buf[-2:] == SYNC:
            return


def read_frame(ser):
    """Read one complete binary waveform frame after sync.
    Returns (seq, ptp_sec, ptp_nsec, channels, nsamp, data_dict)
    or None on error."""
    # We already consumed SYNC; read rest of header
    hdr_rest = ser.read(HEADER_SIZE - 2)
    if len(hdr_rest) < HEADER_SIZE - 2:
        return None

    header = SYNC + hdr_rest
    seq = struct.unpack_from("<I", header, 2)[0]
    ptp_sec = struct.unpack_from("<I", header, 6)[0]
    ptp_nsec = struct.unpack_from("<I", header, 10)[0]
    mask = header[14]
    nsamp = struct.unpack_from("<H", header, 15)[0]

    # Sanity check — reject obviously corrupted headers
    if nsamp == 0 or nsamp > 1024 or mask == 0 or mask > 0x7F:
        return None

    channels = mask_to_channels(mask)
    num_ch = len(channels)
    data_bytes = nsamp * num_ch * 2

    raw = ser.read(data_bytes)
    if len(raw) < data_bytes:
        return None

    # Parse interleaved int16 samples into per-channel arrays
    samples = np.frombuffer(raw, dtype="<i2").reshape(nsamp, num_ch)
    data_dict = {}
    for i, (ch_idx, ch_name) in enumerate(channels):
        data_dict[ch_name] = samples[:, i].copy()

    return seq, ptp_sec, ptp_nsec, channels, nsamp, data_dict


class FrameReader(threading.Thread):
    """Background thread that continuously reads frames from serial port.

    Keeps only the latest frame so the plot thread always gets fresh data
    without blocking serial reads during matplotlib rendering.
    """

    def __init__(self, ser):
        super().__init__(daemon=True)
        self.ser = ser
        self.lock = threading.Lock()
        self.latest_frame = None
        self.last_seq = None
        self.received = 0
        self.dropped = 0
        self.running = True

    def run(self):
        while self.running:
            try:
                sync_to_frame(self.ser)
                result = read_frame(self.ser)
                if result is None:
                    continue

                seq, ptp_sec, ptp_nsec, channels, nsamp, data_dict = result

                # Reject frames with unexpected nsamp (likely corrupted)
                if nsamp != 256:
                    continue

                self.received += 1
                ts_str = format_ptp_time(ptp_sec, ptp_nsec)

                # Check for dropped blocks
                if self.last_seq is not None:
                    gap = seq - self.last_seq - 1
                    if gap > 0:
                        self.dropped += gap
                self.last_seq = seq

                ch_names = [ch[1] for ch in channels]
                print(f"Block seq={seq:6d}  PTP={ts_str}  "
                      f"ch={ch_names}  nsamp={nsamp}  "
                      f"recv={self.received} drop={self.dropped}",
                      end="\r")

                with self.lock:
                    self.latest_frame = result

            except Exception:
                if not self.running:
                    break

    def get_latest(self):
        """Get the most recent frame (or None if no new frame)."""
        with self.lock:
            frame = self.latest_frame
            self.latest_frame = None
            return frame

    def stop(self):
        self.running = False


def main():
    parser = argparse.ArgumentParser(
        description="Decode and plot ADE9430 waveform frames from UART2")
    parser.add_argument("port", help="Serial port (e.g. /dev/ttyUSB0)")
    parser.add_argument("--baud", type=int, default=921600,
                        help="Baud rate (default: 921600)")
    parser.add_argument("--batch", type=int, default=0,
                        help="Capture N blocks then plot (0 = live)")
    parser.add_argument("--save", action="store_true",
                        help="Save plots as PNG instead of showing")
    parser.add_argument("--no-plot", action="store_true",
                        help="Just decode and print stats, no plotting")
    args = parser.parse_args()

    ser = serial.Serial(args.port, args.baud, timeout=1.0)
    print(f"Opened {args.port} at {args.baud} baud")
    print("Waiting for sync...")

    if args.batch > 0:
        # Batch mode: collect N frames then plot
        last_seq = None
        dropped = 0
        received = 0
        frames = []

        # Lock onto a frame boundary, then flush the OS/pyserial input buffer.
        # Otherwise bytes that piled up during port-open / interpreter warmup
        # cause a one-time burst "drop" that is purely host-side, not firmware.
        sync_to_frame(ser)
        ser.reset_input_buffer()

        try:
            while len(frames) < args.batch:
                sync_to_frame(ser)
                result = read_frame(ser)
                if result is None:
                    continue

                seq, ptp_sec, ptp_nsec, channels, nsamp, data_dict = result
                received += 1
                ts_str = format_ptp_time(ptp_sec, ptp_nsec)

                if last_seq is not None:
                    gap = seq - last_seq - 1
                    if gap > 0:
                        dropped += gap
                        print(f"  DROPPED {gap} blocks "
                              f"(seq {last_seq+1}..{seq-1})")
                last_seq = seq

                ch_names = [ch[1] for ch in channels]
                print(f"Block seq={seq:6d}  PTP={ts_str}  "
                      f"ch={ch_names}  nsamp={nsamp}  "
                      f"recv={received} drop={dropped}", end="\r")

                frames.append((seq, ptp_sec, ptp_nsec, data_dict))

        except KeyboardInterrupt:
            print(f"\n\nStopped. Received={received} Dropped={dropped}")

        ser.close()

        if frames and not args.no_plot:
            import matplotlib.pyplot as plt

            # Concatenate the longest run of contiguous (gap-free) blocks
            # so the plot shows many cycles instead of a single 2.5-cycle
            # block. A sequence gap (dropped block) breaks the run.
            best_run = []
            cur_run = [frames[0]]
            for prev, cur in zip(frames, frames[1:]):
                if cur[0] == prev[0] + 1:          # seq contiguous
                    cur_run.append(cur)
                else:
                    if len(cur_run) > len(best_run):
                        best_run = cur_run
                    cur_run = [cur]
            if len(cur_run) > len(best_run):
                best_run = cur_run

            ch_names = list(best_run[0][3].keys())
            concat = {ch: np.concatenate([f[3][ch] for f in best_run])
                      for ch in ch_names}

            start_seq = best_run[0][0]
            end_seq = best_run[-1][0]
            ts_str = format_ptp_time(best_run[0][1], best_run[0][2])
            nblk = len(best_run)
            cycles = nblk * 2.5  # ~2.5 cycles per 256-sample block at 50 Hz

            fig, axes = plt.subplots(len(ch_names), 1,
                                     figsize=(12, 2 * len(ch_names)),
                                     sharex=True)
            if len(ch_names) == 1:
                axes = [axes]

            for ax, ch_name in zip(axes, ch_names):
                ax.plot(concat[ch_name], linewidth=0.5)
                ax.set_ylabel(ch_name)
                ax.grid(True, alpha=0.3)
            axes[-1].set_xlabel("Sample")
            fig.suptitle(f"Waveform seq {start_seq}..{end_seq}  "
                         f"({nblk} blocks ≈ {cycles:.0f} cycles)  "
                         f"PTP={ts_str}  "
                         f"[{received} recv, {dropped} drop]")
            plt.tight_layout()

            if args.save:
                fname = f"waveform_seq{start_seq}_{end_seq}.png"
                plt.savefig(fname, dpi=150)
                print(f"Saved {fname}")
            else:
                plt.show()
        return

    # Live mode or no-plot mode: use background reader thread
    reader = FrameReader(ser)
    reader.start()

    try:
        if args.no_plot:
            # Just let the reader thread print stats
            while True:
                time.sleep(1.0)
        else:
            import matplotlib.pyplot as plt

            fig = None
            axes = None
            lines = None
            title_text = None
            expected_nsamp = None

            plt.ion()

            while True:
                frame = reader.get_latest()
                if frame is not None:
                    seq, ptp_sec, ptp_nsec, channels, nsamp, data_dict = frame
                    ts_str = format_ptp_time(ptp_sec, ptp_nsec)

                    if fig is None:
                        expected_nsamp = nsamp
                        fig, axes = plt.subplots(
                            len(channels), 1,
                            figsize=(12, 2 * len(channels)),
                            sharex=True)
                        if len(channels) == 1:
                            axes = [axes]
                        lines = []
                        for ax, (ch_idx, ch_name) in zip(axes, channels):
                            line, = ax.plot(data_dict[ch_name],
                                            linewidth=0.5)
                            ax.set_ylabel(ch_name)
                            ax.grid(True, alpha=0.3)
                            lines.append(line)
                        axes[-1].set_xlabel("Sample")
                        title_text = fig.suptitle(
                            f"Waveform seq={seq}  PTP={ts_str}")
                        plt.tight_layout()
                    elif nsamp == expected_nsamp:
                        # Update existing lines in-place
                        for line, (ch_idx, ch_name) in zip(lines, channels):
                            line.set_ydata(data_dict[ch_name])
                        title_text.set_text(
                            f"Waveform seq={seq}  PTP={ts_str}")
                        for ax in axes:
                            ax.relim()
                            ax.autoscale_view()

                    fig.canvas.draw_idle()
                    fig.canvas.flush_events()

                plt.pause(0.05)

    except KeyboardInterrupt:
        print(f"\n\nStopped. Received={reader.received} "
              f"Dropped={reader.dropped}")

    reader.stop()
    ser.close()


if __name__ == "__main__":
    main()
