#!/usr/bin/python3
# Copyright 2023(c) Analog Devices, Inc.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of Analog Devices, Inc. nor the names of its
#    contributors may be used to endorse or promote products derived from this
#    software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
# EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Plots the de-interleaved capture into capture.png. Reads the Rx channels
# from capture_ch{N}.csv (as the Xilinx flow does); if capture_tx_ch{N}.csv
# files are also present (written by the altera capture.py --tx-addr option),
# the Tx (DAC playback) channels are plotted in the same figure above the Rx
# channels so the sent tone can be compared against the captured one.
#
# Usage: python plot.py <num_rx_channels> [num_samples]

import matplotlib
import csv
import glob
import os
import re
import sys

# Use non-interactive backend when no display is available (WSL, SSH, headless)
if not os.environ.get('DISPLAY') and sys.platform != 'win32':
    matplotlib.use('Agg')

import matplotlib.pyplot as plt


def load_channel(csvpath, num_samples):
    y = []
    with open(csvpath, 'r') as f:
        for row in csv.reader(f):
            if not row:
                continue
            v = int(row[0])
            y.append(v - 65536 if v >= 32768 else v)
    if num_samples is not None:
        y = y[:num_samples]
    return y


def discover_tx_channels():
    """Return capture_tx_ch{N}.csv paths, ordered by N."""
    paths = glob.glob(os.path.join(os.getcwd(), 'capture_tx_ch*.csv'))

    def idx(p):
        m = re.search(r'capture_tx_ch(\d+)\.csv$', p)
        return int(m.group(1)) if m else 0

    return sorted(paths, key=idx)


def main():
    if len(sys.argv) < 2 or len(sys.argv) > 3:
        print("Usage: python plot.py <num_rx_channels> [num_samples]")
        return

    try:
        num_rx = int(sys.argv[1])
        num_samples = int(sys.argv[2]) if len(sys.argv) == 3 else None
    except ValueError:
        print("Error: Invalid number format")
        return

    # Build the row list: Tx channels (if captured) first, then Rx channels.
    rows = []  # (label, series)
    for path in discover_tx_channels():
        m = re.search(r'capture_tx_ch(\d+)\.csv$', path)
        ch = int(m.group(1)) - 1
        label = "tx{}{}".format(ch // 2 + 1, 'i' if ch % 2 == 0 else 'q')
        rows.append((label, load_channel(path, num_samples)))

    for ch in range(num_rx):
        path = os.path.join(os.getcwd(), "capture_ch{}.csv".format(ch + 1))
        label = "rx{}{}".format(ch // 2 + 1, 'i' if ch % 2 == 0 else 'q')
        rows.append((label, load_channel(path, num_samples)))

    if not rows:
        print("No capture_ch*.csv found in {}".format(os.getcwd()))
        return

    n = len(rows)
    # Tx and Rx buffers can differ in depth, so don't share the x axis.
    fig, axs = plt.subplots(n, 1, sharex=False, sharey=True,
                            figsize=(14, 2 * n + 2))
    axs = [axs] if n == 1 else axs
    for ax, (label, y) in zip(axs, rows):
        ax.set_ylabel(label)
        ax.plot(y)
        ax.grid(True)

    title = "First {} samples".format(num_samples) if num_samples is not None \
        else "All samples"
    plt.suptitle(title)
    plt.tight_layout()
    outfile = os.path.join(os.getcwd(), 'capture.png')
    plt.savefig(outfile, dpi=100)
    print("Saved {}".format(outfile))
    if matplotlib.get_backend().lower() != 'agg':
        plt.show()


if __name__ == "__main__":
    main()
