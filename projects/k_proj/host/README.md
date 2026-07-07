# k_proj host scripts

Host-side Python tools for talking to the k_proj IIOD server running on the
ZCU102. See `../IIOD_ARCHITECTURE.md` for the device model.

## Requirements

- Python 3.7+
- libiio Python bindings: `python3 -m pip install pylibiio`
- The board running the `iiod_example` firmware, reachable over the network
  (Ethernet/GEM is the IIOD transport). Find its IP from the UART boot log
  (`IP address: ...`).

## Files

| Script | Purpose |
|---|---|
| `gen_example_bin.py` | Generate a test `example.bin` (ramp / prbs / const pattern). |
| `send_large_bin.py`  | Stream a binary file to the `k-tx` OUTPUT IIO device, block by block. |

## Quick start

```bash
# 1. Make a 1 MiB test file (interleaved 8ch x le:s16 ramp).
python3 gen_example_bin.py -s 1M -o example.bin

# 2. Send it to the board (replace with your board's IP).
python3 send_large_bin.py -u ip:192.168.1.50 -f example.bin -m raw

# Sanity-check the device is visible first:
iio_attr -u ip:192.168.1.50 -d k-tx
```

## Payload modes (mirror `k-tx`'s `payload_mode` attribute)

- `raw` — file bytes are sent through unchanged (opaque blob). Use this to load
  an arbitrary bitstream/firmware blob to the chip.
- `samples` — the file is interpreted as interleaved 16-bit samples across the
  8 output channels (`voltage0..7`, `le:s16/16`). Use this for waveform data.

## Useful options

```bash
# Cyclic waveform (hardware repeats the last block forever, zero CPU):
python3 send_large_bin.py -u ip:<ip> -m samples --cyclic -f wave.bin

# Larger blocks for higher throughput on big files:
python3 send_large_bin.py -u ip:<ip> --block-bytes 262144 -f big.bin

# Pick the stream id carried in the stream (1=VEC, 3=PASS):
python3 send_large_bin.py -u ip:<ip> --stream-id 1 -f vec.bin
```

## Notes

- Large files are streamed one block at a time, so host memory stays bounded
  regardless of file size.
- The script auto-detects the libiio API version (v1 `iio.Block` vs legacy
  `Buffer.push`).
- On the firmware side, the `k-tx` `submit()` data path is still a Phase-3 TODO;
  until it lands, transfers will be accepted by IIOD but the device returns
  `-ENOSYS` from `submit()`. The script is ready for when that path is filled in.
