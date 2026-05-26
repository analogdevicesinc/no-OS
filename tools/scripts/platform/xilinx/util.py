#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 Analog Devices, Inc. All rights reserved.
"""
Xilinx/AMD platform utilities for no-OS build system.

Replacement for util.tcl using the Vitis 2025+ Python API (xsdb/hsi).
Invoked via: vitis -s util.py <function> <args...>

Arguments (positional, matching util.tcl):
  function  - Function to call: get_arch, create_project, create_fsbl,
               clean_build, upload
  ws        - Workspace/project path
  hw_path   - Hardware definition directory
  hw_file   - Hardware file name (e.g. system_top.xsa)
  binary    - ELF binary path
  target    - Target CPU filter (empty or "0" for auto-select)
  template  - App template name (unused in 2025+ flow)
  fsbl_file - FSBL ELF path (optional)
  jtagtarget - JTAG cable identifier (optional)
"""

import os
import sys

from hsi import HwManager


def _get_processor(hw_design, target):
    """Get processor name from hardware design, optionally filtered by target."""
    cells = hw_design.get_cells(hierarchical='true',
                                filter='IP_TYPE==PROCESSOR')
    if not cells:
        return "0"

    if len(cells) > 1 and target:
        for cell in cells:
            if target in str(cell):
                return str(cell)
        print(f"Warning: Target cpu '{target}' unavailable")

    return str(cells[0])


def get_arch(hw_path, hw_file, target):
    """Extract processor architecture from XSA and write to arch.txt."""
    hw_full = os.path.join(hw_path, hw_file)
    hw_design = HwManager.open_hw_design(hw_full)
    cpu = _get_processor(hw_design, target)
    arch_file = os.path.join(hw_path, "arch.txt")
    with open(arch_file, "w") as f:
        f.write(cpu)


# ---------------------------------------------------------------------------
# Entry point
# ---------------------------------------------------------------------------

def main():
    if len(sys.argv) < 2:
        print("Usage: vitis -s util.py <function> [args...]")
        sys.exit(1)

    function = sys.argv[1]
    ws       = sys.argv[2] if len(sys.argv) > 2 else ""
    hw_path  = sys.argv[3] if len(sys.argv) > 3 else ""
    hw_file  = sys.argv[4] if len(sys.argv) > 4 else ""
    binary   = sys.argv[5] if len(sys.argv) > 5 else ""
    target   = sys.argv[6] if len(sys.argv) > 6 else ""
    template = sys.argv[7] if len(sys.argv) > 7 else ""
    fsbl_file = sys.argv[8] if len(sys.argv) > 8 else ""
    jtagtarget = sys.argv[9] if len(sys.argv) > 9 else ""

    # Normalize empty/zero target to empty string (matches Tcl: if {$target == 0})
    if target == "0":
        target = ""

    dispatch = {
        "get_arch": lambda: get_arch(hw_path, hw_file, target),
    }

    if function not in dispatch:
        print(f"Error: Unknown function '{function}'")
        print(f"Available: {', '.join(dispatch.keys())}")
        sys.exit(1)

    dispatch[function]()


if __name__ == "__main__":
    main()
