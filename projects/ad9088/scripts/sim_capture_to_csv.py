#!/usr/bin/env python3
"""Convert a raw capture from the ad9088 host simulation to capture.tcl CSVs.

The simulation (-DAD9088_SIM=ON, see README.rst) writes the dma_example RX
buffer to $AD9088_SIM_CAPTURE and the TX buffer to the same path plus ".tx".
Each is the raw buffer: little-endian 16-bit samples, channels interleaved.
This splits one into capture_ch<N>.csv (unsigned, one value per line), the
layout tools/scripts/platform/altera/capture.tcl writes from the board, so
plot_loopback.py next to it reads it as is.

  projects/ad9088/scripts/sim_capture_to_csv.py /tmp/sim_cap.bin    /tmp/sim_plot/rx
  projects/ad9088/scripts/sim_capture_to_csv.py /tmp/sim_cap.bin.tx /tmp/sim_plot/tx
"""

import argparse
import os
import struct
import sys


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument('capture', help='raw capture file')
    ap.add_argument('outdir', help='directory for capture_ch<N>.csv')
    ap.add_argument('--channels', type=int, default=8,
                    help='interleaved channels in the buffer (default 8)')
    args = ap.parse_args()

    with open(args.capture, 'rb') as f:
        data = f.read()

    frame = 2 * args.channels
    if not data or len(data) % frame:
        sys.exit('%s: %d bytes is not a whole number of %d-channel frames'
                 % (args.capture, len(data), args.channels))

    vals = struct.unpack('<%dH' % (len(data) // 2), data)
    os.makedirs(args.outdir, exist_ok=True)
    for ch in range(args.channels):
        path = os.path.join(args.outdir, 'capture_ch%d.csv' % (ch + 1))
        with open(path, 'w') as f:
            f.write('\n'.join(str(v) for v in vals[ch::args.channels]))
            f.write('\n')

    print('%s -> %s: %d channels x %d samples'
          % (args.capture, args.outdir, args.channels,
             len(vals) // args.channels))


if __name__ == '__main__':
    main()
