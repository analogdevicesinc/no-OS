#!/usr/bin/env python3
# Copyright 2026(c) Analog Devices, Inc.
#
# SPDX-License-Identifier: BSD-3-Clause
"""Simulation helper: dump a capture buffer from a running run_qemu.sh session
(QEMU, no board) and write capture_ch1..N.csv in the current directory, the
same files capture.tcl writes on hardware, so ../plot.py works unchanged.

Usage (after the example printed "DMA_EXAMPLE Rx: address=..."):
  qemu_capture.py [address] [samples_total] [channels]
  defaults: 0x14000000 131072 8   (dma_example RX buffer, 16-bit samples)
"""
import glob, json, os, socket, struct, sys, tempfile

# QMP socket of the newest running run_qemu.sh (override: QMP_SOCK=...)
socks = sorted(glob.glob("/tmp/no_os_qemu_qmp_*.sock"), key=os.path.getmtime)
SOCK = os.environ.get("QMP_SOCK") or (socks[-1] if socks else None)
if not SOCK:
    sys.exit("no running QEMU found (start run_qemu.sh first)")

def qmp(cmds):
    s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    s.connect(SOCK)
    f = s.makefile("rwb")
    json.loads(f.readline())                       # greeting
    for c in [{"execute": "qmp_capabilities"}] + cmds:
        f.write((json.dumps(c) + "\n").encode()); f.flush()
        while True:
            r = json.loads(f.readline())
            if "return" in r or "error" in r:
                break
        if "error" in r:
            sys.exit("QMP error: %s" % r["error"])
    s.close()

addr = int(sys.argv[1], 0) if len(sys.argv) > 1 else 0x14000000
total = int(sys.argv[2], 0) if len(sys.argv) > 2 else 131072
ch = int(sys.argv[3], 0) if len(sys.argv) > 3 else 8

fd, tmp = tempfile.mkstemp(suffix=".bin"); os.close(fd)
qmp([{"execute": "pmemsave",
      "arguments": {"val": addr, "size": total * 2, "filename": tmp}}])
data = open(tmp, "rb").read(); os.unlink(tmp)
vals = struct.unpack("<%dH" % (len(data) // 2), data)
for c in range(ch):
    with open("capture_ch%d.csv" % (c + 1), "w") as out:
        out.write("\n".join(str(v) for v in vals[c::ch]) + "\n")
print("wrote capture_ch1..%d.csv (%d samples/channel) from 0x%x"
      % (ch, total // ch, addr))
