#!/bin/bash
# Copyright 2026(c) Analog Devices, Inc.
#
# SPDX-License-Identifier: BSD-3-Clause
#
# Run a no-OS ELF for the VPK180 (Versal) in AMD's QEMU, for simulation
# without a board. Only the processor side (PS) is emulated; the FPGA
# registers must be served by the project's simulation models (for example
# projects/ad9082 built with -DAD9082_SIM=ON).
#
# Usage: run_qemu.sh a72|r5 <elf> [timeout_s]
# Needs: XILINX_VITIS (source <Vitis>/settings64.sh first).
#
# UART: on this terminal by default. Quit with Ctrl-A, then X (Ctrl-C goes
# to the guest). If the terminal looks odd afterwards, type "reset".
#   UART=tcp:127.0.0.1:30431,server=on,wait=off  -> UART on a TCP socket
#                                                   (e.g. for IIOD clients)
#   UART=pty                                      -> UART on a pseudo-terminal
# A QMP control socket /tmp/no_os_qemu_qmp_<pid>.sock is opened for
# qemu_capture.py.
#
# How it boots without the platform management controller (PMC): the VPK180
# device tree shipped with Vitis is built, then the chosen core's halt and
# reset/power gating is removed, so QEMU starts the ELF directly.
set -e

CORE=${1:?usage: run_qemu.sh a72|r5 <elf> [timeout_s]}
ELF=${2:?usage: run_qemu.sh a72|r5 <elf> [timeout_s]}
SECS=${3:-600}

: "${XILINX_VITIS:?source <Vitis>/settings64.sh first}"
EMU="$XILINX_VITIS/../data/emulation/qemu/comp"
QEMU="$EMU/qemu/sysroots/x86_64-petalinux-linux/usr/bin/qemu-system-aarch64"
DTC="$XILINX_VITIS/bin/dtc"
[ -x "$QEMU" ] || { echo "QEMU not found: $QEMU" >&2; exit 1; }
[ -f "$ELF" ] || { echo "ELF not found: $ELF" >&2; exit 1; }

# Generated files go next to the ELF, i.e. into the build directory.
WORK="$(cd "$(dirname "$ELF")" && pwd)/qemu"
mkdir -p "$WORK/stub"
# Missing from the DTS set shipped with Vitis; only describes GT NPI nodes.
: > "$WORK/stub/versal-gty-npi.dtsi"

if [ ! -f "$WORK/vpk180.dts" ]; then
	(cd "$EMU/dts" && cpp -nostdinc -I . -I include -I "$WORK/stub" \
		-undef -x assembler-with-cpp board-versal-ps-vpk180.dts \
		-o "$WORK/vpk180.pp")
	"$DTC" -q -I dts -O dtb -o "$WORK/vpk180.dtb" "$WORK/vpk180.pp"
	"$DTC" -q -I dtb -O dts -o "$WORK/vpk180.dts" "$WORK/vpk180.dtb"
fi

case "$CORE" in
a72)
	NODE="apu_cpu@0 {"; CPU=0
	FILT='/qemu,halt/||/reset-gpios/||/power-gpios/'
	;;
r5)
	NODE="rpu_cpu@0 {"; CPU=2
	FILT='/qemu,halt/||/^\t+gpios = /||/gpio-names = "reset/'
	;;
*)
	echo "core must be a72 or r5" >&2; exit 1
	;;
esac
a=$(grep -n "$NODE" "$WORK/vpk180.dts" | cut -d: -f1)
awk -v a="$a" "NR>a && NR<a+25 && ($FILT) {next} {print}" \
	"$WORK/vpk180.dts" > "$WORK/run_$CORE.dts"
"$DTC" -q -I dts -O dtb -o "$WORK/run_$CORE.dtb" "$WORK/run_$CORE.dts"

QMP="/tmp/no_os_qemu_qmp_$$.sock"
trap 'rm -f "$QMP"' EXIT

# PS UART0 is the third -serial of this machine model. "timeout --foreground"
# keeps QEMU in the terminal's foreground group so it receives the keys.
timeout --foreground "$SECS" "$QEMU" -M arm-generic-fdt -display none \
	-nographic -hw-dtb "$WORK/run_$CORE.dtb" \
	-serial null -serial null -serial "${UART:-mon:stdio}" \
	-qmp "unix:$QMP,server=on,wait=off" \
	-device loader,file="$ELF",cpu-num=$CPU || true
