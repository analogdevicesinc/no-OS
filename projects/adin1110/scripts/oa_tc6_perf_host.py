#!/usr/bin/env python3
# Copyright 2026(c) Analog Devices, Inc.
#
# SPDX-License-Identifier: BSD-3-Clause
"""PC-side companion script for the adin1110 oa_tc6_perf example.

Continuously transmits Ethernet test frames to the board's ADIN1110 (which
must be running the oa_tc6_perf firmware example) at a configurable rate
(default 10 Mbps), while concurrently receiving and measuring the frames
the board transmits back. Both directions are reported periodically, so
this benchmarks the full duplex throughput of the link.

Uses raw AF_PACKET sockets, so it only runs on Linux and requires root
privileges (or CAP_NET_RAW on the interpreter).

Example usage:
    sudo python3 oa_tc6_perf_host.py eth0
    sudo python3 oa_tc6_perf_host.py eth0 --rate-mbps 10 --interval 5
"""

import argparse
import fcntl
import socket
import struct
import sys
import threading
import time

ETH_HDR_LEN = 14
BROADCAST_MAC = b"\xff\xff\xff\xff\xff\xff"
SIOCGIFHWADDR = 0x8927


def get_iface_mac(ifname):
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
        info = fcntl.ioctl(s.fileno(), SIOCGIFHWADDR,
                            struct.pack('256s', ifname[:15].encode()))
    return info[18:24]


class Stats:
    def __init__(self):
        self.lock = threading.Lock()
        self.tx_frames = 0
        self.tx_bytes = 0
        self.rx_frames = 0
        self.rx_bytes = 0

    def add_tx(self, nbytes):
        with self.lock:
            self.tx_frames += 1
            self.tx_bytes += nbytes

    def add_rx(self, nbytes):
        with self.lock:
            self.rx_frames += 1
            self.rx_bytes += nbytes

    def pop(self):
        with self.lock:
            vals = (self.tx_frames, self.tx_bytes, self.rx_frames, self.rx_bytes)
            self.tx_frames = self.tx_bytes = self.rx_frames = self.rx_bytes = 0
            return vals


def build_frame(src_mac, ethertype, payload_len):
    header = BROADCAST_MAC + src_mac + struct.pack('!H', ethertype)
    payload = bytes(4) + bytes([0xA5]) * (payload_len - 4)
    return bytearray(header + payload)


def tx_thread(sock, stats, src_mac, ethertype, payload_len, rate_mbps,
             stop_event):
    frame = build_frame(src_mac, ethertype, payload_len)
    frame_len = len(frame)
    interval = (frame_len * 8) / (rate_mbps * 1e6)

    seq = 0
    start = time.monotonic()
    while not stop_event.is_set():
        struct.pack_into('!I', frame, ETH_HDR_LEN, seq)
        try:
            sock.send(frame)
        except OSError:
            continue

        stats.add_tx(frame_len)
        seq = (seq + 1) & 0xFFFFFFFF

        target = start + seq * interval
        now = time.monotonic()
        if target > now:
            time.sleep(target - now)


def rx_thread(sock, stats, ethertype, stop_event):
    sock.settimeout(0.5)
    while not stop_event.is_set():
        try:
            data = sock.recv(65535)
        except socket.timeout:
            continue
        except OSError as exc:
            print(f"rx_thread: socket error, stopping receive: {exc}",
                  file=sys.stderr)
            break

        if len(data) < ETH_HDR_LEN:
            continue

        if struct.unpack('!H', data[12:14])[0] != ethertype:
            continue

        stats.add_rx(len(data))


def stats_thread(stats, interval, stop_event):
    while not stop_event.wait(interval):
        tx_frames, tx_bytes, rx_frames, rx_bytes = stats.pop()
        tx_mbps = tx_bytes * 8 / interval / 1e6
        rx_mbps = rx_bytes * 8 / interval / 1e6
        print(f"TX: {tx_frames} frames, {tx_bytes} bytes, {tx_mbps:.2f} Mbps | "
              f"RX: {rx_frames} frames, {rx_bytes} bytes, {rx_mbps:.2f} Mbps")


def main():
    parser = argparse.ArgumentParser(
        description="ADIN1110 OA TC6 full duplex performance host script")
    parser.add_argument('iface',
                        help="Network interface connected to the board's ADIN1110")
    parser.add_argument('--rate-mbps', type=float, default=10.0,
                        help="Target transmit rate in Mbps (default: 10)")
    parser.add_argument('--payload-len', type=int, default=1500,
                        help="Ethernet payload length in bytes (default: 1500)")
    parser.add_argument('--ethertype', type=lambda x: int(x, 0), default=0x88B5,
                        help="EtherType used to identify test frames (default: 0x88B5)")
    parser.add_argument('--interval', type=float, default=5.0,
                        help="Throughput print interval in seconds (default: 5)")
    args = parser.parse_args()

    if args.payload_len < 4:
        parser.error("--payload-len must be at least 4 (sequence number)")

    try:
        src_mac = get_iface_mac(args.iface)
    except OSError as exc:
        sys.exit(f"Could not read MAC address of {args.iface}: {exc}")

    try:
        tx_sock = socket.socket(socket.AF_PACKET, socket.SOCK_RAW,
                                socket.htons(args.ethertype))
        tx_sock.bind((args.iface, args.ethertype))

        rx_sock = socket.socket(socket.AF_PACKET, socket.SOCK_RAW,
                                socket.htons(args.ethertype))
        rx_sock.bind((args.iface, args.ethertype))
    except PermissionError:
        sys.exit("Permission denied opening a raw socket - re-run as root "
                  "(or grant CAP_NET_RAW).")

    stop_event = threading.Event()
    stats = Stats()

    threads = [
        threading.Thread(target=tx_thread, args=(
            tx_sock, stats, src_mac, args.ethertype, args.payload_len,
            args.rate_mbps, stop_event)),
        threading.Thread(target=rx_thread, args=(
            rx_sock, stats, args.ethertype, stop_event)),
        threading.Thread(target=stats_thread, args=(
            stats, args.interval, stop_event)),
    ]

    print(f"Transmitting on {args.iface} at {args.rate_mbps} Mbps "
          f"({args.payload_len} byte payload), EtherType 0x{args.ethertype:04X}. "
          "Press Ctrl+C to stop.")

    for t in threads:
        t.start()

    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        pass
    finally:
        stop_event.set()
        for t in threads:
            t.join()
        tx_sock.close()
        rx_sock.close()


if __name__ == '__main__':
    main()
