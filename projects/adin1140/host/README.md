# Raw L2 throughput tools (host side)

Small `AF_PACKET`/`SOCK_RAW` programs to measure the true layer-2 throughput of
the 10BASE-T1S link with no IP/TCP/UDP overhead. They pair with the board's
`frame_blast` example (`CONFIG_ADIN1140_FRAME_BLAST_EXAMPLE`).

Both sides use EtherType **0x88B5** (IEEE 802 local-experimental) and 1514-byte
frames (14-byte header + 1500 payload). Root (or `CAP_NET_RAW`) is required.

## Build (on the iMX93 / any Linux host)

```
cc -O2 -o l2_recv l2_recv.c
cc -O2 -o l2_send l2_send.c
```

## Measure board -> host (board TX)

1. Host: `sudo ./l2_recv <iface>`   (e.g. `eth0`)
   - Runs **forever**; leave it running. It prints one `burst:` line per test.
   - Counting starts at the first frame and ends after 1 s of silence, i.e.
     right after the board stops sending - so the board's TX duration defines
     the measured window, not a fixed host timer. This idle gap is a
     compile-time constant (`IDLE_GAP_S` in `l2_recv.c`); change it and rebuild
     if you need a different value.
2. Board menu: select `1) TX blast`, set the duration, the frame size (bytes,
   64-1514; default 1514) and the target rate in **Mbit/s** (accepts a decimal
   such as `2.5`; `0` = full line rate).

Each board run produces one `burst:` line on the host with L2 Mbit/s. The board
also prints an on-wire estimate that adds the 20-byte per-frame preamble+SFD+IFG.

## Measure host -> board (board RX)

1. Board menu: select `2) RX count`, duration `10`.
2. Host: `sudo ./l2_send <iface> 10`.

The board prints the L2 Mbit/s it received over its active window.

## Notes

- Find the T1S interface with `ip link`; it's the one the ADIN/MAC-PHY bridge
  presents (often `eth1`). Bring it up: `sudo ip link set <iface> up`.
- `l2_recv` and the board RX mode both measure over the *active* window (first
  frame to last), so pre-test idle time doesn't dilute the number.
- These count the L2 frame (header + payload, no FCS). The on-wire rate is a bit
  higher due to preamble/SFD/IFG; the board's TX mode reports that estimate too.
- If the host NIC can't keep up at small IFG, `l2_send` may report a rate below
  the link ceiling - that's the host, not the board. Compare against the board's
  own TX-blast number for the board's true send capability.
