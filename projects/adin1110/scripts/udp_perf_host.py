#!/usr/bin/env python3
# Copyright 2026(c) Analog Devices, Inc.
#
# SPDX-License-Identifier: BSD-3-Clause
"""PC-side companion script for the adin1110 lwip_udp_perf example.

Sends UDP datagrams to the board at a configurable rate while receiving the
datagrams the board streams back on the same port, and periodically prints the
UDP payload throughput in both directions. The board's datagrams start with a
32-bit big-endian sequence number, which is used to count lost datagrams.

Example usage:
    python3 udp_perf_host.py 192.168.80.51
    python3 udp_perf_host.py 192.168.80.51 --rate-mbps 0      # receive only
    python3 udp_perf_host.py 192.168.80.51 --rate-mbps 5 --interval 2
"""

import argparse
import socket
import struct
import threading
import time


class Stats:
    def __init__(self):
        self.lock = threading.Lock()
        self.tx_datagrams = 0
        self.tx_bytes = 0
        self.rx_datagrams = 0
        self.rx_bytes = 0
        self.rx_lost = 0
        self.last_seq = None

    def add_tx(self, nbytes):
        with self.lock:
            self.tx_datagrams += 1
            self.tx_bytes += nbytes

    def add_rx(self, data):
        with self.lock:
            self.rx_datagrams += 1
            self.rx_bytes += len(data)
            if len(data) >= 4:
                seq = struct.unpack_from('!I', data)[0]
                if self.last_seq is not None:
                    gap = (seq - self.last_seq - 1) & 0xFFFFFFFF
                    if gap < 0x80000000:
                        self.rx_lost += gap
                self.last_seq = seq

    def pop(self):
        with self.lock:
            snap = (self.tx_datagrams, self.tx_bytes, self.rx_datagrams,
                    self.rx_bytes, self.rx_lost)
            self.tx_datagrams = self.tx_bytes = 0
            self.rx_datagrams = self.rx_bytes = self.rx_lost = 0
            return snap


def tx_thread(sock, dest, stats, payload_len, rate_mbps, stop):
    payload = bytearray(payload_len)
    period = payload_len * 8 / (rate_mbps * 1e6)
    seq = 0
    next_time = time.perf_counter()

    while not stop.is_set():
        struct.pack_into('!I', payload, 0, seq)
        try:
            sock.sendto(payload, dest)
            stats.add_tx(payload_len)
        except OSError:
            pass
        seq = (seq + 1) & 0xFFFFFFFF

        next_time += period
        delay = next_time - time.perf_counter()
        if delay > 0:
            time.sleep(delay)
        elif delay < -0.1:
            # Fell far behind (host scheduling): resync instead of bursting
            next_time = time.perf_counter()


def rx_thread(sock, stats, stop):
    while not stop.is_set():
        try:
            data, _ = sock.recvfrom(65535)
        except socket.timeout:
            continue
        stats.add_rx(data)


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('board_ip', help="IP address of the board")
    parser.add_argument('--port', type=int, default=5001,
                        help="UDP port, used in both directions (default: 5001)")
    parser.add_argument('--rate-mbps', type=float, default=10.0,
                        help="UDP payload rate sent to the board, 0 to only receive (default: 10)")
    parser.add_argument('--payload-len', type=int, default=1472,
                        help="UDP payload bytes per datagram (default: 1472, one full frame)")
    parser.add_argument('--interval', type=float, default=5.0,
                        help="Seconds between reports (default: 5)")
    args = parser.parse_args()

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 4 * 1024 * 1024)
    sock.bind(('', args.port))
    sock.settimeout(0.5)

    stats = Stats()
    stop = threading.Event()
    threads = [threading.Thread(target=rx_thread, args=(sock, stats, stop), daemon=True)]
    if args.rate_mbps > 0:
        threads.append(threading.Thread(target=tx_thread,
                                        args=(sock, (args.board_ip, args.port), stats,
                                              args.payload_len, args.rate_mbps, stop),
                                        daemon=True))
    for t in threads:
        t.start()

    print(f"Sending {args.payload_len} byte datagrams to {args.board_ip}:{args.port} "
          f"at {args.rate_mbps} Mbps, receiving on port {args.port}. Ctrl+C to stop.",
          flush=True)
    try:
        while True:
            time.sleep(args.interval)
            tx_d, tx_b, rx_d, rx_b, lost = stats.pop()
            print(f"UDP TX: {tx_d} datagrams, {tx_b * 8 / args.interval / 1e6:.2f} Mbps | "
                  f"RX: {rx_d} datagrams, {rx_b * 8 / args.interval / 1e6:.2f} Mbps, "
                  f"{lost} lost", flush=True)
    except KeyboardInterrupt:
        pass
    finally:
        stop.set()


if __name__ == '__main__':
    main()
