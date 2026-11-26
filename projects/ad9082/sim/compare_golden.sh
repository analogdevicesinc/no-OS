#!/bin/bash
# Copyright 2026(c) Analog Devices, Inc.
#
# SPDX-License-Identifier: BSD-3-Clause
#
# Simulation check: compare the console log of a QEMU run (built with
# -DAD9082_SIM=ON) against a board log in sim/golden/.
# Usage: compare_golden.sh <golden.txt> <run.log>
# Lines printed by the simulation models (SIM:) and by QEMU are dropped, and
# the values that also vary between board runs (measured JESD clocks, the TPL
# interface rate) are masked; everything else must match line by line.
# Exit 0 = match, 1 = differences (printed as a diff).
set -e
mask() {
	tr -d '\r' < "$1" | grep -v -E '^SIM:|^qemu-system' | expand -t8 |
	sed -E 's/(Measured (Link|Device) Clock: )[0-9.]+ MHz/\1<measured> MHz/;
		s/(Successfully initialized \()[0-9]+( Hz\))/\1<rate>\2/'
}
diff <(mask "$1") <(mask "$2") && echo "MATCH: $2 == $1 (measured values masked)"
