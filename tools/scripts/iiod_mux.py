#!/usr/bin/env python3
# Copyright 2026(c) Analog Devices, Inc.
#
# SPDX-License-Identifier: BSD-3-Clause
"""IIOD multiplexer for simulation: lets libiio network clients (iio_info,
iio_readdev, osc) use a no-OS IIOD that sits behind a single UART stream.

Typical use: an iio example running in QEMU
(tools/scripts/platform/xilinx/qemu/run_qemu.sh) with its UART on one TCP
socket (UART=tcp:127.0.0.1:30431,server=on,wait=off). libiio's ip backend opens
several connections (one for the context, more for buffers), which a UART
link can't serve. This proxy accepts any number of client connections on
LISTEN_PORT and forwards their requests one at a time over the one upstream
connection, routing each reply back to the client that asked.

It understands the IIOD text protocol, the only one no-OS IIOD speaks:
  request:  one line ending in \\n; WRITE and WRITEBUF are followed by <len>
            payload bytes (len = last token of the line)
  reply:    one integer line N, then
            PRINT/READ/GETTRIG: N > 0 -> N bytes + "\\n"
            READBUF:            N > 0 -> mask line + N bytes (repeated until the
                                requested byte count is reached or N <= 0)
            VERSION:            the whole reply is one line
            everything else:    just N

Usage: iiod_mux.py [listen_port] [upstream_port] [-v]
       defaults 30432 30431; listens on all interfaces, so osc on Windows can
       use ip:localhost:30432 (WSL2 forwards localhost).
"""
import socket, sys, threading

LISTEN_PORT = int(sys.argv[1]) if len(sys.argv) > 1 and sys.argv[1].isdigit() else 30432
UP_PORT = int(sys.argv[2]) if len(sys.argv) > 2 and sys.argv[2].isdigit() else 30431
VERBOSE = "-v" in sys.argv

lock = threading.Lock()
up = socket.create_connection(("127.0.0.1", UP_PORT))
upf = up.makefile("rb")


def log(*a):
    if VERBOSE:
        print(*a, flush=True)


def read_exact(f, n):
    data = b""
    while len(data) < n:
        chunk = f.read(n - len(data))
        if not chunk:
            raise ConnectionError("upstream closed")
        data += chunk
    return data


def up_int_line():
    line = upf.readline()
    if not line:
        raise ConnectionError("upstream closed")
    return line, int(line.strip() or b"0")


def transact(req, payload):
    """Send one request upstream, return the full raw reply."""
    words = req.split()
    cmd = words[0].upper() if words else b""
    up.sendall(req + payload)

    if cmd == b"VERSION":
        return upf.readline()

    line, n = up_int_line()
    reply = line
    if cmd in (b"PRINT", b"READ", b"GETTRIG") and n > 0:
        reply += read_exact(upf, n + 1)              # data + "\n"
    elif cmd == b"READBUF":
        want = int(words[2]) if len(words) > 2 else n
        while n > 0:
            reply += upf.readline()                   # channel mask
            reply += read_exact(upf, n)
            want -= n
            if want <= 0:
                break
            line, n = up_int_line()
            reply += line
    return reply


def serve(cli, addr):
    f = cli.makefile("rb")
    log("client", addr, "connected")
    try:
        while True:
            req = f.readline()
            if not req:
                break
            words = req.split()
            payload = b""
            if words and words[0].upper() in (b"WRITE", b"WRITEBUF"):
                payload = read_exact(f, int(words[-1]))
            if words and words[0].upper() == b"EXIT":
                break                                 # don't close the UART side
            with lock:
                reply = transact(req, payload)
            log(addr, req.strip()[:60], "->", reply[:40])
            cli.sendall(reply)
    except (ConnectionError, OSError, ValueError) as e:
        log("client", addr, "error:", e)
    finally:
        cli.close()
        log("client", addr, "closed")


srv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
srv.bind(("0.0.0.0", LISTEN_PORT))
srv.listen(8)
print("iiod_mux: clients on port %d -> UART stream 127.0.0.1:%d"
      % (LISTEN_PORT, UP_PORT), flush=True)
while True:
    c, a = srv.accept()
    c.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
    threading.Thread(target=serve, args=(c, a), daemon=True).start()
