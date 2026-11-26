#!/usr/bin/env python3
# Copyright 2026(c) Analog Devices, Inc.
#
# SPDX-License-Identifier: BSD-3-Clause
"""Simulation helper: capture an IIO buffer from a no-OS IIOD whose UART is on a
TCP socket, e.g. an iio example running in QEMU via
tools/scripts/platform/xilinx/qemu/run_qemu.sh with
UART=tcp:127.0.0.1:30431,server=on,wait=off. Speaks the IIOD text protocol over
the single stream and writes capture_ch1..N.csv for plot.py.

libiio can't do this here: its serial backend (libserialport) refuses
pseudo-terminals (needs /sys/class/tty/<name>), and its ip backend opens a
second TCP connection for buffers, which a UART link doesn't have.

Usage: iio_capture.py [samples_per_channel] [device] [mask_hex] [port]
defaults: 2048 iio:device0 ff 30431   (8 x 16-bit RX channels)
"""
import socket, struct, sys

samples = int(sys.argv[1]) if len(sys.argv) > 1 else 2048
dev = sys.argv[2] if len(sys.argv) > 2 else "iio:device0"
mask = int(sys.argv[3], 16) if len(sys.argv) > 3 else 0xff
port = int(sys.argv[4]) if len(sys.argv) > 4 else 30431
ch = bin(mask).count("1")

s = socket.create_connection(("127.0.0.1", port)); s.settimeout(60)
f = s.makefile("rb")
def cmd(c):
    s.sendall(c.encode() + b"\r\n")
    return f.readline().strip()

r = cmd("OPEN %s %d %08x" % (dev, samples, mask))
if r != b"0":
    sys.exit("OPEN failed: %r" % r)
n = int(cmd("READBUF %s %d" % (dev, samples * ch * 2)))
if n < 0:
    sys.exit("READBUF failed: %d" % n)
f.readline()                                   # channel mask line
data = f.read(n)
cmd("CLOSE %s" % dev)
vals = struct.unpack("<%dH" % (len(data) // 2), data)
for c in range(ch):
    with open("capture_ch%d.csv" % (c + 1), "w") as out:
        out.write("\n".join(str(v) for v in vals[c::ch]) + "\n")
print("wrote capture_ch1..%d.csv (%d samples/channel) from %s"
      % (ch, len(vals) // ch, dev))
