#!/usr/bin/python3
# Copyright 2026(c) Analog Devices, Inc.
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

"""
Nios V (Agilex 5) capture helper: the Intel/Ashling analog of the Xilinx
tools/scripts/platform/xilinx/capture.tcl.

The Xilinx flow reads the DMA capture buffer over JTAG with xsct/mrd and
de-interleaves it into capture_chN.csv. On Nios V there is no xsct; the debug
path is the Ashling RiscFree GDB server plus riscv32-unknown-elf-gdb -- the
same pair niosv-download drives to load the ELF. This script launches that
server, has gdb dump the buffer straight out of on-chip memory, and writes the
same capture_chN.csv files, so plot.py (copied here from
tools/scripts/platform/xilinx/plot.py and extended) plots the result unchanged.

Optionally it also dumps the transmit (DAC) buffer so the played tone can be
plotted next to the captured one. The two buffers have different layouts:

  * Rx (adc_buffer_dma): int16 samples, num_channels converters interleaved.
    -> capture_ch{1..N}.csv
  * Tx (dac_buffer_dma): uint32 words, each packing one complex channel as
    I in the low 16 bits and Q in the high 16 bits, tx_channels complex
    channels interleaved. Each word is unpacked into two int16 streams (I, Q).
    -> capture_tx_ch{1..2*tx_channels}.csv

Prerequisites:
  * The application is running and parked (the ad9088 dma_example ends in a
    while(1) precisely so the capture buffers stay intact and readable).
  * ash-riscv-gdb-server and <prefix>-gdb are on PATH (run from a niosv-shell,
    or export the Quartus/niosv/riscfree bin dirs).
  * No juart-terminal open on the same cable: the JTAG UART holds the
    USB-Blaster and would block the debug tap. The buffers are already in
    memory by the time the app parks, so close the console before capturing.

Addresses and geometry are printed by the app (DMA_EXAMPLE Rx/Tx lines) and are
also readable from the ELF, e.g.:
  riscv32-unknown-elf-nm build/ad9088 | grep _buffer_dma
For the AD9084-EBZ Agilex build: Rx 0x1013a800 (8 ch x 4096), Tx 0x1011a800
(4 complex ch x 8192, uint32 I/Q):
  python capture.py 0x1013a800 32768 8 --tx-addr 0x1011a800
  python plot.py 8
"""

import argparse
import array
import os
import re
import shutil
import subprocess
import sys
import time

ASHLING_GDB_SERVER = 'ash-riscv-gdb-server'
DEFAULT_RISCV_TRIPLE = 'riscv32-unknown-elf'
SERVER_READY_RE = re.compile(r'Waiting for debugger connection on port (\d+)')
SERVER_READY_TIMEOUT_S = 30


def die(msg):
    print('capture: error: {}'.format(msg), file=sys.stderr)
    sys.exit(1)


def parse_args():
    p = argparse.ArgumentParser(
        description='Read a Nios V DMA capture buffer (and optionally the DAC '
                    'playback buffer) over JTAG via the Ashling GDB server and '
                    'de-interleave into capture_chN.csv / capture_tx_chN.csv.')
    p.add_argument('start_addr', help='Rx (adc_buffer_dma) address, hex (0x...) '
                                      'or decimal, as the app prints it.')
    p.add_argument('num_samples', type=int,
                   help='Total Rx samples across all channels '
                        '(samples/channel x channels).')
    p.add_argument('num_channels', type=int,
                   help='Number of interleaved Rx channels (converters).')
    p.add_argument('storage_bits', nargs='?', type=int, default=16,
                   help='Bits per stored Rx sample (default 16; 2 bytes for '
                        '<=16).')
    # Optional Tx (DAC) buffer.
    p.add_argument('--tx-addr', help='Tx (dac_buffer_dma) address. When given, '
                   'also dump and unpack the DAC playback buffer.')
    p.add_argument('--tx-words', type=int, default=32768,
                   help='Number of uint32 words in the Tx buffer (default '
                        '%(default)s = 128 KiB / 4).')
    p.add_argument('--tx-channels', type=int, default=4,
                   help='Complex channels interleaved in the Tx buffer, each a '
                        'uint32 (I low 16, Q high 16). Default %(default)s.')
    # Probe selection (usually auto).
    p.add_argument('--probe-serial', '-c', help='Debug probe serial number, '
                   'when more than one probe is connected. Default: the only '
                   'probe.')
    p.add_argument('--device', '-d', default='agilex-5',
                   help="Target device family name the Ashling server expects "
                        "(auto-scan, agilex, agilex-5, ...). Default "
                        "'%(default)s'.")
    p.add_argument('--core-number', '-i', help='Nios V core index on the '
                   'device (starts at 0). Default: auto-detect.')
    p.add_argument('--gdb-port', type=int, default=0,
                   help='GDB server port (default 0 = server picks one).')
    p.add_argument('--prefix', default=DEFAULT_RISCV_TRIPLE,
                   help="GDB toolchain prefix (default '%(default)s').")
    p.add_argument('--keep-bin', action='store_true',
                   help='Keep the raw dumps instead of removing them.')
    return p.parse_args()


def start_gdb_server(args):
    if shutil.which(ASHLING_GDB_SERVER) is None:
        die('{} is not on PATH. Run from a niosv-shell, or export the '
            'riscfree/quartus bin dirs.'.format(ASHLING_GDB_SERVER))

    server_args = [ASHLING_GDB_SERVER,
                   '--auto-detect', 'true',
                   '--probe-type', 'usb-blaster-2',
                   '--device', args.device,
                   '--gdb-port', str(args.gdb_port)]
    # Pass through the same selectors niosv-download uses, when given.
    if args.probe_serial is not None:
        server_args += ['--instance', args.probe_serial]
    if args.core_number is not None:
        server_args += ['--core-number', args.core_number]

    print('capture: starting GDB server: {}'.format(' '.join(server_args)))
    proc = subprocess.Popen(server_args, stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT)

    port = None
    deadline = time.time() + SERVER_READY_TIMEOUT_S
    while proc.poll() is None and time.time() < deadline:
        line = proc.stdout.readline().decode('utf-8', 'replace').rstrip()
        if line:
            print('[gdb-server] {}'.format(line))
        m = SERVER_READY_RE.search(line)
        if m:
            port = int(m.group(1))
            break

    if port is None:
        # Drain whatever the server printed so the failure is diagnosable.
        if proc.poll() is None:
            proc.terminate()
        for line in proc.stdout:
            print('[gdb-server] {}'.format(line.decode('utf-8', 'replace')
                                           .rstrip()))
        die('GDB server did not report a port (is the cable free -- no '
            'juart-terminal open -- and the board configured?).')

    return proc, port


def stop_gdb_server(proc):
    if proc.poll() is not None:
        return
    proc.terminate()
    for _ in range(10):
        if proc.poll() is not None:
            return
        time.sleep(0.2)
    proc.kill()


def dump_regions(args, port, regions):
    """Dump each (bin_path, start, length) region in a single gdb session."""
    gdb = '{}-gdb'.format(args.prefix)
    if shutil.which(gdb) is None:
        die("'{}' is not on PATH.".format(gdb))

    gdb_args = [gdb, '-batch',
                '-ex', 'set arch riscv:rv32',
                '-ex', 'set remotetimeout 60',
                '-ex', 'target extended-remote localhost:{}'.format(port)]
    for bin_path, start, length in regions:
        if os.path.exists(bin_path):
            os.remove(bin_path)
        gdb_args += ['-ex', 'dump binary memory {} 0x{:x} 0x{:x}'.format(
            bin_path, start, start + length)]
        print('capture: dumping 0x{:x}..0x{:x} ({} bytes) -> {}'.format(
            start, start + length, length, bin_path))
    gdb_args += ['-ex', 'detach']

    if subprocess.call(gdb_args) != 0:
        die('gdb dump failed.')

    for bin_path, _, length in regions:
        got = os.path.getsize(bin_path) if os.path.exists(bin_path) else 0
        if got < length:
            die('short dump: {} got {} of {} bytes.'.format(bin_path, got,
                                                            length))


def _open_channel_csvs(prefix, n):
    files = []
    for ch in range(n):
        path = os.path.join(os.getcwd(), '{}ch{}.csv'.format(prefix, ch + 1))
        if os.path.exists(path):
            os.remove(path)
        files.append(open(path, 'w'))
    return files


def deinterleave_rx(bin_path, length, num_channels):
    samples = array.array('h')  # signed 16-bit
    with open(bin_path, 'rb') as f:
        samples.frombytes(f.read(length))
    if sys.byteorder == 'big':
        samples.byteswap()  # dump is little-endian

    files = _open_channel_csvs('capture_', num_channels)
    try:
        for i in range(len(samples)):
            files[i % num_channels].write('{}\n'.format(samples[i]))
    finally:
        for f in files:
            f.close()
    per_channel = len(samples) // num_channels
    print('capture: wrote capture_ch1..{}.csv, {} samples/channel'.format(
        num_channels, per_channel))


def deinterleave_tx(bin_path, length, complex_channels):
    words = array.array('I')  # unsigned 32-bit words, I low / Q high
    with open(bin_path, 'rb') as f:
        words.frombytes(f.read(length))
    if sys.byteorder == 'big':
        words.byteswap()  # dump is little-endian

    real_channels = complex_channels * 2
    files = _open_channel_csvs('capture_tx_', real_channels)
    try:
        for i in range(len(words)):
            c = i % complex_channels
            w = words[i]
            iq = w & 0xFFFF
            qq = (w >> 16) & 0xFFFF
            if iq >= 0x8000:
                iq -= 0x10000
            if qq >= 0x8000:
                qq -= 0x10000
            files[2 * c].write('{}\n'.format(iq))       # I -> tx{c+1}i
            files[2 * c + 1].write('{}\n'.format(qq))   # Q -> tx{c+1}q
    finally:
        for f in files:
            f.close()
    per_channel = len(words) // complex_channels
    print('capture: wrote capture_tx_ch1..{}.csv, {} samples/channel'.format(
        real_channels, per_channel))


def main():
    args = parse_args()

    rx_bytes_per_sample = max(2, (args.storage_bits + 7) // 8)
    rx_len = args.num_samples * rx_bytes_per_sample
    if args.num_samples % args.num_channels:
        die('num_samples {} is not a multiple of num_channels {}.'.format(
            args.num_samples, args.num_channels))

    rx_bin = 'capture.bin'
    regions = [(rx_bin, int(str(args.start_addr), 0), rx_len)]

    tx_bin = None
    tx_len = 0
    if args.tx_addr is not None:
        tx_bin = 'capture_tx.bin'
        tx_len = args.tx_words * 4  # uint32 words
        regions.append((tx_bin, int(str(args.tx_addr), 0), tx_len))

    server = None
    try:
        server, port = start_gdb_server(args)
        dump_regions(args, port, regions)
    finally:
        if server is not None:
            stop_gdb_server(server)

    deinterleave_rx(rx_bin, rx_len, args.num_channels)
    if tx_bin is not None:
        deinterleave_tx(tx_bin, tx_len, args.tx_channels)

    if not args.keep_bin:
        for path in (rx_bin, tx_bin):
            if path and os.path.exists(path):
                os.remove(path)

    here = os.path.dirname(__file__) or '.'
    print('capture: done. Plot with: python3 {} {}'.format(
        os.path.join(here, 'plot.py'), args.num_channels))


if __name__ == '__main__':
    main()
