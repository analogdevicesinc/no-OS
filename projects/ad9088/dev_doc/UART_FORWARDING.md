# Forwarding a board UART from a remote host

How to reach the UART of a board that is plugged into a *different* machine, so
that both the console (`printf`) and an IIO client run on your local
workstation.

The result is a local pseudo-terminal that behaves like a directly attached
serial port, so `picocom`, `iio_info`, `iio_readdev` and anything else that
opens a `/dev` node all work unchanged.

```
board ── USB-UART ──> remote host ── ser2net ──> TCP:5555
                                                   │
                                            SSH -L tunnel
                                                   │
   local pty  <── socat <── TCP:127.0.0.1:5555 ─────┘
        │
        ├── picocom            (console)
        └── libiio serial      (IIO control and buffers)
```

Throughout, replace `user@remote` with your remote host and `5555` with any
free port.

## Prerequisites

- `ser2net` 4.x on the remote host (or `socat`, see step 2b).
- `socat` locally, to turn the TCP stream back into a device node.
- `picocom`, `minicom` or similar locally.
- SSH access to the remote host.
- For the IIO part, `libiio` locally with the serial backend enabled.

Run the SSH client **inside** WSL if you are on WSL2, so that `127.0.0.1` means
the same thing to the tunnel and to your tools.

## Step 1 — identify the UART on the remote host

```bash
ls -l /dev/serial/by-id/
dmesg | grep -iE 'cp210|ftdi|ttyUSB'
```

Evaluation boards usually expose **several** channels on one USB bridge — the
processor UART, and often a separate system-controller UART. Identify the right
one empirically: open each and reset the board, watching for boot output.

A plain `/dev/ttyUSBn` path is fine for a working session, and is the quicker
way to get going. The reason to prefer `/dev/serial/by-id/...` for anything
long-lived is not the loud failure but the quiet one: every channel on the
bridge opens successfully, so if re-enumeration shifts the numbering you
silently attach to the *system-controller* UART instead of the processor UART.
That presents as a hung board, not as a config error, and costs real time.

If the `by-id` string is unwieldy, a udev rule is the middle ground:

```bash
udevadm info -a -n /dev/ttyUSB1 | grep -E 'idVendor|idProduct|serial|ID_USB_INTERFACE_NUM'
```

Feed those into `/etc/udev/rules.d/99-board-uart.rules` with
`SYMLINK+="ttyBOARD"`.

## Step 2 — expose the port on the remote host

Bind to loopback only, so the port is reachable through the SSH tunnel but is
not open on the network.

### Step 2a — ser2net (what this project uses)

Check the version first. YAML config arrived in **4.x**; 3.x uses an unrelated
line-based format in `/etc/ser2net.conf`, in which case prefer step 2b.

```bash
ser2net -v
```

Write a **private** config file rather than editing the system one — see "Why a
private instance" below. `~/ser2net-ad9088.yaml`:

```yaml
%YAML 1.1
---
define: &confver 1.0

connection: &ad9088
  accepter: tcp,localhost,5555
  connector: serialdev,/dev/ttyUSB1,115200n81,local
  options:
    max-connections: 1
```

Run it:

```bash
sudo ser2net -n -c ~/ser2net-ad9088.yaml
```

Four things in that config matter:

- **`accepter: tcp,...` is raw.** Plain `tcp` applies no telnet negotiation.
  This is the single most important choice for IIO — see the warning below.
- **`115200n81`** is baud / parity `n`one / `8` data bits / `1` stop bit.
- **`local`** tells ser2net to ignore modem-control lines, so it will not sit
  waiting for DCD on a 3-wire connection.
- **No `rtscts`.** Omitting it leaves hardware flow control off, which is
  required for AXI UART Lite — it has no RTS/CTS lines at all.

`%YAML 1.1` plus `define: &confver 1.0` selects the v1 schema. ser2net also has
a v2 schema using a `connections:` map, and **the two cannot be mixed** — check
which one any example you copy was written for.

`-n` means *do not fork*, so the command holds the terminal. Run it under `tmux`
if you want it to outlive your SSH session, or drop `-n` to let it daemonise.

**Verify it bound where you think it did:**

```bash
ss -ltnp | grep 5555
```

You want `127.0.0.1:5555`. `0.0.0.0:5555` means the port is exposed to the
network. Nothing at all means ser2net loaded a config with no usable connection
block — see troubleshooting.

For config errors, run it in the foreground with logging to stdout:

```bash
sudo ser2net -d -c ~/ser2net-ad9088.yaml
```

#### Never use `telnet(rfc2217)` here

ser2net's RFC2217 accepter is tempting, because it lets a client change baud
remotely over the network. **Do not use it for IIO.** Telnet escapes any 0xFF
byte as IAC, and libiio's serial backend expects a transparent binary channel.
Attribute reads would mostly work while buffer captures corrupted intermittently
— whenever sample data happened to contain 0xFF, which for real converter data
is constantly. Stay on plain `tcp`.

#### Why a private instance

The packaged `ser2net.service` reads a system config whose path varies by
distro. Ubuntu's unit uses `/etc/ser2net.yaml` where the man page documents
`/etc/ser2net/ser2net.yaml`. Confirm with:

```bash
systemctl cat ser2net | grep ExecStart
```

On a shared lab host that file is shared state and may already carry other
people's connections — possibly ones that fail to bind and spam the journal with
`Address already in use` every 10 seconds. Editing it risks disrupting someone
else's setup, and its pre-existing breakage is easy to mistake for your own. A
private file plus an explicit `-c` avoids both. Making the bridge a permanent
system service is the lab admin's call.

### Step 2b — socat (no root, no config file)

Equivalent, and preferable when you cannot install or configure ser2net. Needs
only membership of `dialout`, writes nothing under `/etc`, and dies with your
session:

```bash
PORT=/dev/ttyUSB1
BAUD=115200

while true; do
    socat -d TCP-LISTEN:5555,bind=127.0.0.1,reuseaddr \
        "$PORT",b$BAUD,cs8,parenb=0,cstopb=0,raw,echo=0,crtscts=0,ixon=0,ixoff=0,clocal=1
    sleep 1
done
```

**No `fork`** is deliberate: only one process may own a tty, and with `fork` a
second client would open the same device concurrently and interleave the two
streams into garbage. Without it, socat serves one client and exits, so the
`while` loop re-arms the listener.

## Step 3 — tunnel the port

One SSH invocation can carry both the UART and the Xilinx `hw_server` port used
by `make run`:

```bash
ssh -N \
    -L 3122:127.0.0.1:3121 \
    -L 5555:127.0.0.1:5555 \
    user@remote
```

Check that both arrived:

```bash
timeout 5 bash -c 'cat </dev/null >/dev/tcp/127.0.0.1/3122'   # hw_server
timeout 5 bash -c 'cat </dev/null >/dev/tcp/127.0.0.1/5555'   # UART
```

## Step 4 — create a local pseudo-terminal

```bash
mkdir -p ~/dev

while true; do
    socat -d PTY,link=$HOME/dev/ttyBOARD,raw,echo=0,waitslave TCP:127.0.0.1:5555
    sleep 1
done
```

The symlink lives under `$HOME` rather than `/dev` because the latter needs
root. Any path works — both `picocom` and `libiio` only need to open it.

The loop matters while bringing things up: socat exits whenever the remote end
closes the connection, and without it you relaunch by hand after every failure.

## Step 5 — console

```bash
picocom -b 115200 --imap lfcrlf ~/dev/ttyBOARD
```

`--imap lfcrlf` prevents stair-stepped output when the firmware emits bare `\n`.

Exit with `Ctrl-A Ctrl-X`. **Detach before starting an IIO client** — see "One
owner at a time" below.

## Step 6 — IIO over the same link

```bash
iio_info -u serial:/home/<user>/dev/ttyBOARD,115200,8n1
```

The URI form is `serial:<port>[,<baud>[,<config>]]`. When the firmware uses
`iio_app`, it prints the exact settings to use on the console immediately before
switching the UART over to the IIO protocol (`iio/iio_app/iio_app.c:123`), so
read that line rather than guessing.

## Console-only shortcut

If you only need to watch `printf` and do not care about IIO:

```bash
ssh -t user@remote 'picocom -b 115200 --imap lfcrlf /dev/ttyUSB1'
```

Thirty seconds to set up, but it produces no local device node, so it is a dead
end for libiio. Use the full bridge if you will need both.

## Choosing a baud rate

libiio's serial backend enforces a minimum of **110** baud, documents a range of
**110 to 1,000,000**, and defaults to **115200**. Rates above 1 Mbaud may work
but are outside the documented range.

no-OS itself documents no minimum. `UART_BAUDRATE_DEFAULT` in
`iio/iio_app/iio_app.c:79` is 115200, but its comment scopes it to console
messages — it is not a protocol floor. In-tree projects that build IIO examples
run from 57600 (`ad74413r`, `adp1050`) through 115200 (most) to 230400
(`ad7616-st`) and 921600 (`ad9172`, `ad9361`).

**The binding constraint is not the baud rate but a fixed timeout.** The serial
backend sets `default_timeout_ms = 10000` and does not scale it with baud. At
8N1 you spend 10 bits per byte, so the bytes carried in one 10 second window
numerically equal the baud rate:

| Baud | Bytes per 10 s window |
| --- | --- |
| 57600 | ~58 kB |
| 115200 | ~115 kB |
| 230400 | ~230 kB |
| 921600 | ~920 kB |

Any single IIOD response larger than that is abandoned mid-transfer. Size buffer
captures against this, not against available memory.

The timeout is client-side only: `serial_ops` in libiio exposes no
`set_timeout`, so the IIOD `TIMEOUT` command is never sent over a serial link.
`iio_context_set_timeout()` is therefore the lever to raise it — worth verifying
empirically, since the backend has no corresponding op. On the device side no-OS
would reject the command anyway: `iio/iio.c` supplies no `set_timeout`, so it
falls through to `dummy_set_timeout`, which returns `-EINVAL`
(`iio/iiod.c:309`). That matters for the network backend, not for serial.

## One owner at a time

The console and IIOD share a single UART. `iio_app` prints "disconnecting the
serial terminal you use to view this message" before handing the port over, and
it means it. A terminal left attached will corrupt the IIO session.

The `max-connections: 1` (ser2net) and absent `fork` (socat) choices above make
this a clean connection refusal instead of an intermittent data fault.

## Platform notes

### MicroBlaze with AXI UART Lite

The baud rate is fixed **in the bitstream**, not in software. Check the
generated BSP header:

```bash
grep BAUDRATE projects/ad9088/build/bsp/sys_mb/include/xparameters.h
```

For the current VCU118 design this reads `XPAR_AXI_UART_BAUDRATE 0x1c200`, i.e.
115200.

`drivers/platform/xilinx/xilinx_uart.c` calls `XUartLite_CfgInitialize()` only;
there is no runtime baud call on this path, so `UART_BAUDRATE` in the project's
`parameters.h` has no effect. Changing the rate means editing the AXI UART Lite
IP's `C_BAUDRATE` in Vivado and regenerating the bitstream.

UART Lite also has no flow-control lines, so a burst of host-to-device traffic
can overrun the receive FIFO. Truncated commands with an otherwise healthy link
point here.

### Where console output actually comes from

`no_os_uart_stdio()` is a weak no-op (`drivers/api/no_os_uart.c:216`) and has no
Xilinx implementation. `printf` therefore reaches the console through the
**BSP's** stdout peripheral, not through the no-OS UART descriptor. A working
console is consequently *not* evidence that the no-OS UART driver is functional
— IIO needs the descriptor for bidirectional transfers and can fail while
`printf` looks perfectly healthy.

### Versal (VCK190, VPK180)

Versal's PS UART is a PL011, served by the `uartpsv` driver
(`compatible: arm,pl011`). The `uartps` driver is Zynq-7000 and ZynqMP only
(`compatible: cdns,uart-r1p12`, `xlnx,zynqmp-uart`).

Consequences for no-OS as it stands:

- **Console works with no changes**, via the BSP's `uartpsv` stdout.
- **IIO does not.** `xilinx_uart.c` implements only `UART_PS` (guarded by
  `#ifdef XUARTPS_H`, Cadence) and `UART_PL` (`#ifdef XUARTLITE_H`). On Versal
  neither macro is defined. Worse, the `break` for `UART_PS` sits inside the
  `#ifdef`, so the case falls through to `UART_PL`, whose body also vanishes —
  `no_os_uart_init()` returns 0 with `*desc` never assigned. The failure is
  silent until something tries to read.
- Projects that set `.base_addr` under `#ifdef SDT` need that line guarded per
  platform. Vitis 2025+ always compiles with `-DSDT`
  (`tools/scripts/xilinx.mk:202`), and pointing the PS lookup at a UART Lite
  base address is wrong even when the macro happens to exist.

Adding an `XUartPsv` case is the fix. The upside is real: PL011 exposes
`XUartPsv_SetBaudRate()`, so baud becomes runtime-settable and 921600 is
available without a Vivado run.

## Troubleshooting

Work outwards from your machine: each failure below tells you how far along the
chain you got.

| Symptom | Where it failed | Cause |
| --- | --- | --- |
| `channel N: open failed: connect failed: Connection refused` | SSH tunnel up, remote end dead | Nothing listening on the remote. `systemctl is-active ser2net`, `ss -ltnp \| grep 5555` |
| ser2net `active` but nothing in `ss` | Daemon up, no ports | It loaded a config with no connection blocks — usually the wrong file. Check `systemctl cat ser2net \| grep ExecStart` |
| `Unable to startup network port <name>: Address already in use` | Daemon up, that port taken | If `<name>` is not one you wrote, it is someone else's connection in the shared system config. Harmless to your port; do not chase it. Otherwise check `pgrep -a ser2net` for a stale instance |
| `Device open failure: Value or file not found` | Tunnel *and* ser2net fine | Arrives as terminal data, so the whole TCP path works. ser2net cannot open the serial device — wrong `connector:` path, or a placeholder left unedited |
| `FATAL: read zero bytes from port`, socat exits | Remote closed the connection | Downstream of one of the above. Fix that, then relaunch socat — it has no `fork`, hence the loop in step 4 |
| Connects, then silence forever | Wrong bridge channel | Every channel opens successfully. Recheck step 1 against a board reset |
| Output appears, host input ignored | Link up, one direction | Hardware flow control enabled somewhere; force `crtscts=0` / omit `rtscts` on every hop |
| Garbled or interleaved characters | Link up | Two clients on one tty, or a baud mismatch against the bitstream value |
| Stair-stepped lines | Cosmetic | Missing `--imap lfcrlf` |
| IIO session corrupt from the start | Link up | Terminal still attached; detach first |
| IIO transfer dies partway, reproducibly | Link up | Response exceeds the 10 s window — reduce the buffer or raise the baud |
| Console fine, IIO cannot open the device | Firmware | Platform lacks a working no-OS UART driver path (see Versal above) |
| Port vanishes after replug | Remote | Using `/dev/ttyUSBn` where the numbering shifted; switch to `/dev/serial/by-id/` |
