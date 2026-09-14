#!/usr/bin/python3
# Copyright 2023(c) Analog Devices, Inc.
#
# SPDX-License-Identifier: BSD-3-Clause

import matplotlib
import csv
import os
import sys

# Use non-interactive backend when no display is available (WSL, SSH, headless)
if not os.environ.get('DISPLAY') and sys.platform != 'win32':
    matplotlib.use('Agg')

import matplotlib.pyplot as plt

def main():
    if len(sys.argv) < 2 or len(sys.argv) > 3:
        print("Usage: python plot.py <num_channels> [num_samples]")
        return

    try:
        num_channels = int(sys.argv[1])
        num_samples = int(sys.argv[2]) if len(sys.argv) == 3 else None
    except ValueError:
        print("Error: Invalid number format")
        return

    fig, axs = plt.subplots(num_channels, 1, sharex=True, sharey=True,
                            figsize=(14, 2 * num_channels + 2))
    for ch in range(num_channels):
        y = []
        csvpath = os.path.join(os.getcwd(), "capture_ch{}.csv".format(ch + 1))
        with open(csvpath, 'r') as f:
            for row in csv.reader(f):
                v = int(row[0])
                y.append(v - 65536 if v >= 32768 else v)

        if num_samples is not None:
            y = y[:num_samples]

        axsch = axs if num_channels == 1 else axs[ch]
        axsch.set_ylabel("rx{}{}".format(ch // 2 + 1, 'i' if ch % 2 == 0 else 'q'))
        axsch.plot(y)
        axsch.grid(True)

    title = "First {} samples".format(num_samples) if num_samples is not None else "All samples"
    plt.suptitle(title)
    plt.tight_layout()
    outfile = os.path.join(os.getcwd(), 'capture.png')
    plt.savefig(outfile, dpi=100)
    print("Saved {}".format(outfile))
    if matplotlib.get_backend().lower() != 'agg':
        plt.show()

if __name__ == "__main__":
    main()

