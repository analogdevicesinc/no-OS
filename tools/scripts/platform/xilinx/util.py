#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 Analog Devices, Inc. All rights reserved.
"""
Xilinx/AMD platform utilities for no-OS build system.

Drives BSP/FSBL generation and JTAG programming via the Vitis 2025+ Python
API (xsdb/hsi). Invoked via: vitis -s util.py <function> <args...>

Arguments (positional):
  function  - Function to call: get_arch, create_project, create_ide_workspace,
               create_fsbl, clean_build, upload
  ws        - Workspace/project path
  hw_path   - Hardware definition directory
  hw_file   - Hardware file name (e.g. system_top.xsa)
  binary    - ELF binary path (for create_ide_workspace: the manifest JSON path)
  target    - Target CPU filter (empty or "0" for auto-select)
  template  - App template name (unused in 2025+ flow)
  fsbl_file - FSBL ELF path (optional, for upload)
  jtagtarget - JTAG cable identifier (optional, for upload)
"""

import os
import re
import sys
import time
import subprocess

from hsi import HwManager


# ---------------------------------------------------------------------------
# Target filter dictionaries (PL bitstream / PS core name patterns)
# ---------------------------------------------------------------------------

PL_DICT = {
    "ps7_cortexa9_0": "xc7z*",
    "psu_cortexa53_0": "PSU",
    "sys_cips_pspmc_0_psv_cortexa72_0": "Versal*",
    "sys_mb": "xc*",
}

PS_DICT = {
    "ps7_cortexa9_0": "*Cortex-A9 MPCore #0*",
    "psu_cortexa53_0": "*Cortex-A53 #0*",
    "sys_cips_pspmc_0_psv_cortexa72_0": "*Cortex-A72 #0*",
    "sys_mb": "*MicroBlaze #*",
}


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


def _vitis_cpu_name(cpu):
    """Convert HSI cell name to Vitis API processor name.

    For Versal, HSI returns the full CIPS hierarchy path
    (e.g. 'sys_cips_pspmc_0_psv_cortexa72_0') but the Vitis
    create_platform_component API expects the lopper-generated
    short name ('psv_cortexa72_0').
    Non-Versal names (psu_cortexa53_0, ps7_cortexa9_0, sys_mb)
    pass through unchanged.
    """
    idx = cpu.find("_psv_")
    if idx >= 0:
        return cpu[idx + 1:]
    return cpu


def _build_filter(name_pattern, jtagtarget=None):
    """Build a targets() filter expression.

    The xsdb Python API filter parser requires:
    - No quotes around values (unlike xsct Tcl)
    - Parentheses around each condition
    - 'and'/'or' instead of '&&'/'||'
    """
    if jtagtarget:
        return (f'(name =~ {name_pattern}) and '
                f'(jtag_cable_name =~ *{jtagtarget}*)')
    return f'(name =~ {name_pattern})'


# ---------------------------------------------------------------------------
# Fabric interrupt macro generation
# ---------------------------------------------------------------------------

# GIC IRQ base for PS platforms. The concat output connects to pl_ps_irq,
# which maps to these GIC IRQ IDs based on the platform.
# Zynq-7000: sys_concat_intc[15:0] -> IRQ_F2P[15:0] -> GIC 61-68 (0-7), 84-91 (8-15)
# ZynqMP: sys_concat_intc_0[7:0] -> pl_ps_irq0 -> GIC 121-128
#         sys_concat_intc_1[7:0] -> pl_ps_irq1 -> GIC 136-143
# Versal: sys_cips/pl_ps_irq[15:0] -> GIC 116-131
FABRIC_IRQ_BASE = {
    "ps7_cortexa9_0": {
        "sys_concat_intc": lambda idx: 61 + idx if idx < 8 else 84 + (idx - 8),
    },
    "psu_cortexa53_0": {
        "sys_concat_intc_0": lambda idx: 121 + idx,
        "sys_concat_intc_1": lambda idx: 136 + idx,
    },
}


def _generate_fabric_irq_macros(xsa_path, cpu):
    """Extract PL interrupt connections and generate XPAR_FABRIC_* macros.

    The Vitis 2025+ Python API (create_platform_component) does not generate
    XPAR_FABRIC_*_INTR macros for PL peripherals. This function replicates what
    the old HSI generate_bsp command did by tracing interrupt signals from
    peripheral IRQ pins through the concat blocks to the PS interrupt ports.

    For MicroBlaze, generates XPAR_AXI_INTC_*_INTR macros instead.

    Returns a list of C #define lines.
    """
    hw_design = HwManager.open_hw_design(xsa_path)
    defines = []

    # MicroBlaze uses AXI interrupt controller with different naming
    # Check for any MicroBlaze CPU (sys_mb, or custom names like IOP1_IOP1_mb)
    if cpu == "sys_mb" or "_mb" in cpu.lower():
        defines = _generate_mb_irq_macros(hw_design)
        hw_design.close()
        return defines

    # Versal has a different interrupt topology (direct pl_ps_irq connections)
    if "psv_cortexa72" in cpu:
        defines = _generate_versal_irq_macros(hw_design)
        hw_design.close()
        return defines

    # PS platforms (Zynq-7000, ZynqMP): trace through concat blocks
    irq_base = FABRIC_IRQ_BASE.get(cpu)
    if not irq_base:
        hw_design.close()
        return defines

    # Find all interrupt concat blocks and their input connections
    cells = hw_design.get_cells(hierarchical='true')
    concat_map = {}  # concat_name -> {input_idx -> net_name}

    for cell in cells:
        cell_name = cell.get('NAME')
        if cell_name not in irq_base:
            continue

        concat_map[cell_name] = {}
        pins = hw_design.get_pins(of_objects=cell)
        if not pins:
            continue

        for pin in pins:
            pin_name = pin.get('NAME')
            if not pin_name.startswith('In'):
                continue
            try:
                idx = int(pin_name[2:])
            except ValueError:
                continue

            nets = hw_design.get_nets(of_objects=pin)
            if nets:
                concat_map[cell_name][idx] = nets[0].get('NAME')

    # Find all PL peripherals with IRQ outputs and trace to concat inputs
    for cell in cells:
        cell_name = cell.get('NAME')
        pins = hw_design.get_pins(of_objects=cell)
        if not pins:
            continue

        for pin in pins:
            # Detect interrupt outputs via TYPE property, with fallback to
            # common pin names for custom IPs that don't set TYPE properly
            pin_type = pin.get('TYPE')
            pin_dir = pin.get('DIRECTION')
            pin_name = pin.get('NAME')
            is_irq_output = (pin_type == 'INTERRUPT' and pin_dir == 'O')
            if not is_irq_output and pin_dir == 'O':
                is_irq_output = pin_name.lower() in ('irq', 'interrupt')
            if not is_irq_output:
                continue

            nets = hw_design.get_nets(of_objects=pin)
            if not nets:
                continue
            net_name = nets[0].get('NAME')

            # Find which concat input this net connects to
            for concat_name, inputs in concat_map.items():
                for idx, input_net in inputs.items():
                    if input_net == net_name:
                        irq_id = irq_base[concat_name](idx)
                        macro = f"XPAR_FABRIC_{cell_name.upper()}_{pin_name.upper()}_INTR"
                        defines.append(f"#define {macro} {irq_id}U")

    hw_design.close()
    return defines


def _generate_mb_irq_macros(hw_design):
    """Generate XPAR_AXI_INTC_*_INTR macros for MicroBlaze designs."""
    defines = []
    cells = hw_design.get_cells(hierarchical='true')

    # Find the AXI interrupt controller by VLNV (more reliable than name)
    intc_cell = None
    for cell in cells:
        vlnv = cell.get('VLNV') or ''
        if 'axi_intc' in vlnv:
            intc_cell = cell
            break

    if not intc_cell:
        return defines

    # Trace the intc's intr pin to find the connected concat block
    intr_net_name = None
    pins = hw_design.get_pins(of_objects=intc_cell)
    if pins:
        for pin in pins:
            if pin.get('NAME') == 'intr':
                nets = hw_design.get_nets(of_objects=pin)
                if nets:
                    intr_net_name = nets[0].get('NAME')
                break

    if not intr_net_name:
        return defines

    # Find the concat block whose dout drives the intc's intr pin
    concat_cell = None
    for cell in cells:
        vlnv = cell.get('VLNV') or ''
        if 'xlconcat' in vlnv:
            pins = hw_design.get_pins(of_objects=cell)
            for pin in pins:
                if pin.get('NAME') == 'dout':
                    nets = hw_design.get_nets(of_objects=pin)
                    if nets and nets[0].get('NAME') == intr_net_name:
                        concat_cell = cell
                        break
            if concat_cell:
                break

    if not concat_cell:
        return defines

    # Build map of concat inputs to connected nets
    concat_map = {}
    concat_pins = hw_design.get_pins(of_objects=concat_cell)
    if concat_pins:
        for pin in concat_pins:
            pin_name = pin.get('NAME')
            if pin_name.startswith('In') and pin.get('DIRECTION') == 'I':
                try:
                    idx = int(pin_name[2:])
                except ValueError:
                    continue
                nets = hw_design.get_nets(of_objects=pin)
                if nets:
                    concat_map[idx] = nets[0].get('NAME')

    # Find peripherals and match their IRQ nets to concat inputs
    for cell in cells:
        cell_name = cell.get('NAME')
        pins = hw_design.get_pins(of_objects=cell)
        if not pins:
            continue

        for pin in pins:
            # Detect interrupt outputs via TYPE property, with fallback to
            # common pin names for custom IPs that don't set TYPE properly
            pin_type = pin.get('TYPE')
            pin_dir = pin.get('DIRECTION')
            pin_name = pin.get('NAME')
            is_irq_output = (pin_type == 'INTERRUPT' and pin_dir == 'O')
            if not is_irq_output and pin_dir == 'O':
                is_irq_output = pin_name.lower() in ('irq', 'interrupt')
            if not is_irq_output:
                continue

            nets = hw_design.get_nets(of_objects=pin)
            if not nets:
                continue
            net_name = nets[0].get('NAME')

            for idx, input_net in concat_map.items():
                if input_net == net_name:
                    macro = f"XPAR_AXI_INTC_{cell_name.upper()}_{pin_name.upper()}_INTR"
                    defines.append(f"#define {macro} {idx}U")

    return defines


def _generate_versal_irq_macros(hw_design):
    """Generate XPAR_FABRIC_*_INTR macros for Versal designs.

    Versal uses direct pl_ps_irq connections to the CIPS block rather than
    concat blocks. The GIC mapping is pl_ps_irq[N] -> GIC IRQ 116+N.
    """
    defines = []

    # Use non-hierarchical lookup for the top-level CIPS block to get correct
    # net names. The hierarchical lookup returns internal cells with different
    # net names that don't match peripheral IRQ outputs.
    top_cells = hw_design.get_cells()  # Non-hierarchical

    # Find the top-level CIPS block and its pl_ps_irq inputs
    cips_irq_map = {}  # irq_index -> net_name
    for cell in top_cells:
        cell_name = cell.get('NAME')
        if 'cips' not in cell_name.lower():
            continue

        pins = hw_design.get_pins(of_objects=cell)
        if not pins:
            continue

        for pin in pins:
            pin_name = pin.get('NAME')
            if not pin_name.startswith('pl_ps_irq'):
                continue
            try:
                idx = int(pin_name[9:])  # Extract number after 'pl_ps_irq'
            except ValueError:
                continue

            nets = hw_design.get_nets(of_objects=pin)
            if nets:
                cips_irq_map[idx] = nets[0].get('NAME')

    # Find peripherals (need hierarchical to find all IP) and match IRQs
    cells = hw_design.get_cells(hierarchical='true')
    for cell in cells:
        cell_name = cell.get('NAME')
        pins = hw_design.get_pins(of_objects=cell)
        if not pins:
            continue

        for pin in pins:
            # Detect interrupt outputs via TYPE property, with fallback to
            # common pin names for custom IPs that don't set TYPE properly
            pin_type = pin.get('TYPE')
            pin_dir = pin.get('DIRECTION')
            pin_name = pin.get('NAME')
            is_irq_output = (pin_type == 'INTERRUPT' and pin_dir == 'O')
            if not is_irq_output and pin_dir == 'O':
                is_irq_output = pin_name.lower() in ('irq', 'interrupt')
            if not is_irq_output:
                continue

            nets = hw_design.get_nets(of_objects=pin)
            if not nets:
                continue
            net_name = nets[0].get('NAME')

            for idx, input_net in cips_irq_map.items():
                if input_net == net_name:
                    irq_id = 116 + idx  # Versal GIC mapping
                    macro = f"XPAR_FABRIC_{cell_name.upper()}_{pin_name.upper()}_INTR"
                    defines.append(f"#define {macro} {irq_id}U")

    return defines


def _generate_ddr_macros(xsa_path, cpu):
    """Generate DDR memory base address macros for MicroBlaze designs.

    Vitis 2025+ doesn't generate the XPAR_AXI_DDR_CNTRL_* macros that projects
    expect. This function extracts DDR controller information from the XSA
    and generates compatible macros.

    Returns a list of C #define lines.
    """
    # Check for any MicroBlaze CPU (sys_mb, or custom names like IOP1_IOP1_mb)
    if cpu != "sys_mb" and "_mb" not in cpu.lower():
        return []

    hw_design = HwManager.open_hw_design(xsa_path)
    defines = []
    seen = set()

    # Find MicroBlaze processor
    mb_cell = None
    cells = hw_design.get_cells()
    for cell in cells:
        vlnv = cell.get('VLNV') or ''
        if 'microblaze' in vlnv.lower():
            mb_cell = cell
            break

    if not mb_cell:
        hw_design.close()
        return defines

    # Get memory ranges from MicroBlaze's perspective
    mem_ranges = hw_design.get_mem_ranges(of_objects=mb_cell)
    for mem in mem_ranges:
        instance_obj = mem.get('INSTANCE')
        # INSTANCE returns an HwCell object, get its NAME
        instance = instance_obj.get('NAME') if instance_obj else ''
        base = mem.get('BASE_VALUE')
        high = mem.get('HIGH_VALUE')

        if base is None or not instance:
            continue

        # Check if this is a DDR/MIG controller by looking at the instance name
        instance_lower = instance.lower()
        if 'ddr' not in instance_lower and 'mig' not in instance_lower:
            continue

        # Skip duplicates (same instance can appear multiple times)
        if instance in seen:
            continue
        seen.add(instance)

        # Generate the old-style macro that projects expect
        # Format: XPAR_AXI_DDR_CNTRL_C0_DDR4_MEMORY_MAP_BASEADDR
        old_macro = f"XPAR_{instance.upper()}_C0_DDR4_MEMORY_MAP_BASEADDR"
        defines.append(f"#define {old_macro} {base}")

        # Also generate simpler macros
        simple_macro = f"XPAR_{instance.upper()}_BASEADDR"
        defines.append(f"#define {simple_macro} {base}")

        if high is not None:
            simple_high = f"XPAR_{instance.upper()}_HIGHADDR"
            defines.append(f"#define {simple_high} {high}")

    hw_design.close()
    return defines


# ---------------------------------------------------------------------------
# get_arch
# ---------------------------------------------------------------------------

def get_arch(hw_path, hw_file, target):
    """Extract processor architecture from XSA and write to arch.txt."""
    hw_full = os.path.join(hw_path, hw_file)
    hw_design = HwManager.open_hw_design(hw_full)
    cpu = _get_processor(hw_design, target)
    arch_file = os.path.join(hw_path, "arch.txt")
    with open(arch_file, "w") as f:
        f.write(cpu)


# ---------------------------------------------------------------------------
# create_project
# ---------------------------------------------------------------------------

def create_project(ws, hw_path, hw_file, target):
    """Create BSP, FSBL, and linker script using the Vitis 2025+ Python API.

    Uses vitis.create_platform_component() for BSP + FSBL generation,
    then vitis.create_app_component() for linker script generation.
    Copies outputs to the paths the Makefile expects (adapter pattern).
    """
    import shutil
    import vitis

    # Read CPU from arch.txt (written by get_arch).
    # arch.txt has the full HSI cell name (e.g. sys_cips_pspmc_0_psv_cortexa72_0
    # for Versal). The Makefile uses this for compiler selection and BSP paths.
    arch_file = os.path.join(hw_path, "arch.txt")
    with open(arch_file, "r") as f:
        cpu = f.read().strip()

    # The Vitis API needs the lopper-style short name (psv_cortexa72_0)
    # rather than the full HSI hierarchy path.
    vcpu = _vitis_cpu_name(cpu)

    xsa = os.path.join(hw_path, hw_file)
    out_dir = os.path.join(ws, "tmp", "output")

    # Clean entire output workspace — Vitis refuses to create a platform
    # if workspace metadata from a previous run exists.
    if os.path.exists(out_dir):
        shutil.rmtree(out_dir)

    # Read XPAR_* IRQ/DDR macros from the .xsa before the client exists, so HSI
    # uses the direct path (like get_arch); appended to xparameters.h below.
    fabric_irq_defines = _generate_fabric_irq_macros(xsa, cpu)
    ddr_defines = _generate_ddr_macros(xsa, cpu)

    # --- Step 1: Platform build (BSP + FSBL) ---
    print(f"INFO: Creating platform component (cpu={vcpu})...")
    client = vitis.create_client(workspace=out_dir)
    client.create_platform_component(
        name="hw0", hw_design=xsa, cpu=vcpu, os="standalone")

    # Build the platform. No need to reconnect - just get the component.
    print("INFO: Building platform (BSP + FSBL)...")
    platform = client.get_component(name="hw0")
    platform.build()

    # --- Step 2: App component (linker script) ---
    xpfm = os.path.join(out_dir, "hw0", "export", "hw0", "hw0.xpfm")
    print("INFO: Creating app component for linker script...")
    app = client.create_app_component(
        name="app", platform=xpfm, template="empty_application")

    # Dispose now and patch lscript.ld directly: app.get_ld_script() would
    # launch XSDB and hang with no JTAG attached.
    vitis.dispose()

    _ld_src = os.path.join(out_dir, "app", "src", "lscript.ld")
    if os.path.exists(_ld_src):
        with open(_ld_src, "r") as _f:
            _ld_text = _f.read()
        _ld_text = re.sub(
            r'(_HEAP_SIZE\s*=\s*DEFINED\(_HEAP_SIZE\)\s*\?\s*_HEAP_SIZE\s*:\s*)0x[0-9a-fA-F]+',
            r'\g<1>0x100000',
            _ld_text)
        with open(_ld_src, "w") as _f:
            _f.write(_ld_text)

    # --- Step 3: Copy BSP to Makefile-expected paths ---
    # Platform output uses the short CPU name; Makefile expects arch.txt name.
    export_sw = os.path.join(out_dir, "hw0", "export", "hw0", "sw")
    bsp_export = os.path.join(export_sw, f"standalone_{vcpu}")
    bsp_inc_src = os.path.join(bsp_export, "include")
    bsp_lib_src = os.path.join(bsp_export, "lib")

    bsp_inc_dst = os.path.join(ws, "bsp", cpu, "include")
    bsp_lib_dst = os.path.join(ws, "bsp", cpu, "lib")

    os.makedirs(bsp_inc_dst, exist_ok=True)
    os.makedirs(bsp_lib_dst, exist_ok=True)
    shutil.copytree(bsp_inc_src, bsp_inc_dst, dirs_exist_ok=True)
    shutil.copytree(bsp_lib_src, bsp_lib_dst, dirs_exist_ok=True)

    # Vitis 2025 BSP (cmake-based) drops xtime_l.h from the public include
    # directory. The APIs moved to xiltimer.h. Create a compatibility shim
    # so existing code that includes xtime_l.h still compiles.
    xtime_shim = os.path.join(bsp_inc_dst, "xtime_l.h")
    if not os.path.exists(xtime_shim):
        with open(xtime_shim, "w") as f:
            f.write("/* Compatibility shim: xtime_l.h APIs moved to "
                    "xiltimer.h in Vitis 2025 */\n"
                    "#ifndef XTIME_H\n"
                    "#define XTIME_H\n"
                    "#include \"xiltimer.h\"\n"
                    "#endif\n")

    # Patch xparameters.h to auto-include compat defines.  This ensures ALL
    # projects get the Vitis 2025+ compatibility fallbacks (DEVICE_ID, interrupt
    # renames, SPI clock frequency renames) without per-project changes.
    compat_src = os.path.join(os.path.dirname(__file__), "..", "..", "..", "..",
                              "drivers", "platform", "xilinx",
                              "xilinx_compat.h")
    compat_dst = os.path.join(bsp_inc_dst, "xilinx_compat.h")
    shutil.copy2(compat_src, compat_dst)
    xpar_h = os.path.join(bsp_inc_dst, "xparameters.h")
    if os.path.exists(xpar_h):
        with open(xpar_h, "a") as f:
            f.write('\n/* Vitis 2025+ compatibility defines */\n')
            f.write('#include "xilinx_compat.h"\n')
            if fabric_irq_defines:
                f.write('\n/* Fabric interrupt defines (generated from XSA) */\n')
                f.write('\n'.join(fabric_irq_defines) + '\n')
            if ddr_defines:
                f.write('\n/* DDR memory defines (generated from XSA) */\n')
                f.write('\n'.join(ddr_defines) + '\n')
        print(f"INFO: Generated {len(fabric_irq_defines)} fabric IRQ, "
              f"{len(ddr_defines)} DDR macros")

    print(f"INFO: BSP copied to bsp/{cpu}/")

    # --- Step 4: Copy linker script ---
    app_src_dir = os.path.join(ws, "app", "src")
    os.makedirs(app_src_dir, exist_ok=True)

    # Search for lscript.ld in the app component output
    ld_src = os.path.join(out_dir, "app", "src", "lscript.ld")
    if not os.path.exists(ld_src):
        # Fallback: search recursively in output
        for root, dirs, files in os.walk(os.path.join(out_dir, "app")):
            if "lscript.ld" in files:
                ld_src = os.path.join(root, "lscript.ld")
                break
    if os.path.exists(ld_src):
        shutil.copy2(ld_src, os.path.join(app_src_dir, "lscript.ld"))
        print("INFO: Linker script copied to app/src/lscript.ld")
    else:
        print("WARNING: lscript.ld not found in app component output")

    # --- Step 5: Xilinx.spec (cortexa9 only) ---
    if "cortexa9" in cpu:
        vitis_dir = os.environ.get("XILINX_VITIS", "")
        spec_src = os.path.join(vitis_dir, "..", "data", "embeddedsw",
                                "scripts", "specs", "arm", "Xilinx.spec")
        spec_src = os.path.normpath(spec_src)
        if os.path.exists(spec_src):
            shutil.copy2(spec_src, os.path.join(app_src_dir, "Xilinx.spec"))
        else:
            print(f"WARNING: Xilinx.spec not found at {spec_src}")

    # --- Step 6: Copy FSBL to Makefile-expected path ---
    # Vitis Python API generates FSBL at sw/boot/fsbl.elf but the Makefile
    # expects it at sw/hw0/boot/fsbl.elf. Copy to match.
    fsbl_src = os.path.join(export_sw, "boot", "fsbl.elf")
    fsbl_dst_dir = os.path.join(export_sw, "hw0", "boot")
    if os.path.exists(fsbl_src):
        os.makedirs(fsbl_dst_dir, exist_ok=True)
        shutil.copy2(fsbl_src, os.path.join(fsbl_dst_dir, "fsbl.elf"))
        print("INFO: FSBL copied to Makefile-expected path")
    else:
        print(f"WARNING: FSBL not found at {fsbl_src}")

    print("INFO: Project created successfully")


# ---------------------------------------------------------------------------
# create_ide_workspace
# ---------------------------------------------------------------------------

def create_ide_workspace(ws, hw_path, hw_file, manifest_path):
    """Build a persistent Vitis workspace populated with the no-OS sources.

    Unlike create_project (which builds into a throwaway tmp/output and copies
    only the BSP + linker script out), this materializes a workspace that Vitis
    can open directly with `vitis -w <workspace>`:

      - an 'hw0' platform component (BSP), and
      - an 'app' application component whose sources are the no-OS files the
        CMake build actually compiled, referenced in place (not copied) via
        import_files(is_skip_copy_sources=True) -- the modern equivalent of the
        legacy `make` symlink mirror under build/app.

    The include paths and compile definitions are taken from the CMake build so
    Vitis's indexer resolves headers/macros exactly as ninja did. CMake still
    owns the real build (the flashed ELF); this component exists for browsing
    and (optionally) building inside the GUI.

    manifest_path points at a JSON file written by no_os_build.py:
        {"sources": [...abs paths...],
         "includes": [...abs dirs...],
         "defines": ["NAME=VAL", "NAME", ...]}
    """
    import json
    import vitis

    with open(manifest_path, "r") as f:
        manifest = json.load(f)
    sources = manifest.get("sources", [])
    includes = manifest.get("includes", [])
    defines = manifest.get("defines", [])
    force_includes = manifest.get("force_includes", [])

    # Arch comes from the manifest (no_os_build reads XILINX_ARCH from the CMake
    # cache); fall back to a staged arch.txt if present. Unlike create_project,
    # this path may run long after the BSP was generated, so we don't rely on
    # config_xilinx_sdk having just written arch.txt into hw_path.
    cpu = manifest.get("arch")
    if not cpu:
        arch_file = os.path.join(hw_path, "arch.txt")
        with open(arch_file, "r") as f:
            cpu = f.read().strip()
    vcpu = _vitis_cpu_name(cpu)

    xsa = os.path.join(hw_path, hw_file)
    # Persistent workspace: sibling of tmp/ so it survives after this run and a
    # subsequent create_project (which only wipes tmp/output) leaves it intact.
    out_dir = os.path.join(ws, "ide", "workspace")

    xpfm = os.path.join(out_dir, "hw0", "export", "hw0", "hw0.xpfm")

    # --- Step 1: Platform (BSP). Skip the slow rebuild if it already exists. ---
    if not os.path.exists(xpfm):
        print(f"INFO: Creating IDE platform component (cpu={vcpu})...")
        client = vitis.create_client(workspace=out_dir)
        client.create_platform_component(
            name="hw0", hw_design=xsa, cpu=vcpu, os="standalone")
        vitis.dispose()

        print("INFO: Building IDE platform (BSP)...")
        client = vitis.create_client(workspace=out_dir)
        platform = client.get_component(name="hw0")
        platform.build()
        vitis.dispose()
    else:
        print("INFO: IDE platform component already present; reusing.")

    # --- Step 2: App component populated with the no-OS sources ---
    # Recreate the app component every run so a re-run of --open picks up source
    # or flag changes from a refreshed manifest. The app references sources in
    # place (no compile here), so this is cheap; the slow platform build above
    # is what we preserve. Vitis refuses create_app_component if the component
    # dir already exists, so remove it first.
    import shutil
    app_dir = os.path.join(out_dir, "app")
    if os.path.exists(app_dir):
        shutil.rmtree(app_dir)
    print("INFO: Creating app component and importing no-OS sources...")
    client = vitis.create_client(workspace=out_dir)
    app = client.create_app_component(
        name="app", platform=xpfm, template="empty_application")

    # Reference each source in place (no copy). import_files takes a common
    # from_loc + file list, so group by containing directory to keep paths
    # absolute and avoid copying the whole tree.
    from collections import defaultdict
    by_dir = defaultdict(list)
    for src in sources:
        if os.path.exists(src):
            by_dir[os.path.dirname(src)].append(os.path.basename(src))
    for from_loc, files in by_dir.items():
        app.import_files(from_loc=from_loc, files=files,
                         is_skip_copy_sources=True)

    if includes:
        app.set_app_config(key="USER_INCLUDE_DIRECTORIES", values=includes)
    if defines:
        app.set_app_config(key="USER_COMPILE_DEFINITIONS", values=defines)

    # Link math library (-lm) for projects that use log10, pow, etc.
    app.set_app_config(key="USER_LINK_LIBRARIES", values=["m"])

    ld = app.get_ld_script()
    ld.set_heap_size('0x100000')

    vitis.dispose()

    # Patch UserConfig.cmake to add force-include flags for Kconfig-generated
    # headers (e.g., -include no_os_config.h). The Vitis API doesn't support
    # arbitrary compiler flags, so we edit the file directly after Vitis
    # creates it.
    if force_includes:
        user_config = os.path.join(out_dir, "app", "src", "UserConfig.cmake")
        if os.path.exists(user_config):
            with open(user_config, "r") as f:
                content = f.read()
            flags = " ".join(f"-include {h}" for h in force_includes)
            # Replace empty USER_COMPILE_OTHER_FLAGS with the force-include flags
            content = content.replace(
                "set(USER_COMPILE_OTHER_FLAGS )",
                f"set(USER_COMPILE_OTHER_FLAGS {flags})")
            with open(user_config, "w") as f:
                f.write(content)

    # Copy the pre-staged debug artifacts (launch.json + extracted bitstream /
    # ps7_init.tcl) into the workspace now that Vitis has initialized it. They
    # must NOT be present before create_client, or Vitis refuses the workspace
    # ("cannot recognize the workspace version"). Staged by ide_vitis_configure
    # at <ws>/ide/staging/_ide.
    staged_ide = os.path.join(ws, "ide", "staging", "_ide")
    if os.path.isdir(staged_ide):
        shutil.copytree(staged_ide, os.path.join(out_dir, "_ide"),
                        dirs_exist_ok=True)
        print("INFO: Debug configuration (_ide/launch.json) staged in workspace.")

    print(f"INFO: IDE workspace ready: {out_dir}")


# ---------------------------------------------------------------------------
# create_fsbl
# ---------------------------------------------------------------------------

def create_fsbl(ws, hw_path, hw_file, target):
    """Generate FSBL using the Vitis 2025+ Python API.

    Simplified version of create_project: only builds the platform
    component to produce fsbl.elf, without BSP/linker copies.
    Used by the 'make run' path.
    """
    import shutil
    import vitis

    arch_file = os.path.join(hw_path, "arch.txt")
    with open(arch_file, "r") as f:
        cpu = f.read().strip()

    vcpu = _vitis_cpu_name(cpu)
    xsa = os.path.join(hw_path, hw_file)
    out_dir = os.path.join(ws, "tmp", "output")

    if os.path.exists(out_dir):
        shutil.rmtree(out_dir)

    # Platform create + build (separate sessions — same gRPC workaround
    # as create_project)
    print(f"INFO: Creating platform component for FSBL (cpu={vcpu})...")
    client = vitis.create_client(workspace=out_dir)
    client.create_platform_component(
        name="hw0", hw_design=xsa, cpu=vcpu, os="standalone")
    vitis.dispose()

    print("INFO: Building platform (FSBL)...")
    client = vitis.create_client(workspace=out_dir)
    platform = client.get_component(name="hw0")
    platform.build()
    vitis.dispose()

    # Copy FSBL to Makefile-expected path
    export_sw = os.path.join(out_dir, "hw0", "export", "hw0", "sw")
    fsbl_src = os.path.join(export_sw, "boot", "fsbl.elf")
    fsbl_dst_dir = os.path.join(export_sw, "hw0", "boot")
    if os.path.exists(fsbl_src):
        os.makedirs(fsbl_dst_dir, exist_ok=True)
        shutil.copy2(fsbl_src, os.path.join(fsbl_dst_dir, "fsbl.elf"))
        print("INFO: FSBL generated successfully")
    else:
        print(f"ERROR: FSBL not found at {fsbl_src}")
        sys.exit(1)


# ---------------------------------------------------------------------------
# clean_build
# ---------------------------------------------------------------------------

def clean_build(ws):
    """Remove and recreate app/src directory."""
    import shutil
    src_dir = os.path.join(ws, "app", "src")
    if os.path.exists(src_dir):
        shutil.rmtree(src_dir)
    os.makedirs(src_dir, exist_ok=True)


# ---------------------------------------------------------------------------
# upload
# ---------------------------------------------------------------------------

def _cpu_reset(session, cpu, jtagtarget):
    """Reset the CPU before FPGA programming."""
    if cpu == "sys_mb":
        return
    if "cortexa72" in cpu:
        # Versal: PLM handles reset
        return

    session.targets('-s', filter=_build_filter('APU*', jtagtarget))
    session.stop()

    if "cortexa9" in cpu:
        session.rst()
    elif "cortexa53" in cpu:
        session.rst(type='system')


def _write_pl(session, cpu, hw_path, hw_file, jtagtarget):
    """Program the FPGA bitstream."""
    if "cortexa72" in cpu:
        # Versal: handled separately via device_program
        return

    name = PL_DICT.get(cpu, "xc*")
    session.targets('-s', filter=_build_filter(name, jtagtarget))

    import glob as _glob
    # The bitstream is extracted from the .xsa at configure time (CMake
    # extract_xsa_members -> hw_path); just pick it up here.
    bit_files = _glob.glob(os.path.join(hw_path, "*.bit"))
    if not bit_files:
        print(f"ERROR: No .bit bitstream found in {hw_path}. It should have "
              "been extracted from the .xsa at configure time.")
        sys.exit(1)
    session.fpga(file=os.path.normpath(bit_files[0]))


def _run_init_sequence(session, init_data):
    """Execute a PS init sequence, working around Vitis 2025.1 bugs.

    Batches consecutive mask_write commands via init_ps() (fast, single
    TCF transaction per batch) and handles mask_poll/mask_delay manually.
    Monkey-patches mask_write to use -force, fixing write-only register
    access errors.
    """
    import types

    def _fixed_mask_write(self, address=None, mask=None, words=None):
        if mask == 0xFFFFFFFF:
            self.mwr('-f', address=address, words=[words])
        else:
            cur = self.mrd('-f', '-v', address=address)
            if isinstance(cur, list):
                cur = cur[0]
            self.mwr('-f', address=address,
                     words=[(cur & ~mask) | (words & mask)])

    session.mask_write = types.MethodType(_fixed_mask_write, session)

    # Split init_data into chunks: batch mask_write via init_ps,
    # handle mask_poll/mask_delay individually.
    write_batch = []
    for line in init_data:
        if line.startswith("mask_write "):
            write_batch.append(line)
        else:
            # Flush pending writes
            if write_batch:
                session.init_ps(init_data=write_batch)
                write_batch = []
            # Handle poll/delay manually
            parts = line.split()
            if parts[0] == "mask_poll":
                addr = int(parts[1], 0)
                mask = int(parts[2], 0)
                for _ in range(100):
                    val = session.mrd('-f', '-v', address=addr)
                    if isinstance(val, list):
                        val = val[0]
                    if val is not None and (val & mask):
                        break
                    time.sleep(0.1)
            elif parts[0] == "mask_delay":
                # mask_delay <addr> <val>: Tcl polls a hw timer.
                # val is a small cycle count (typically 1); just sleep.
                delay_val = int(parts[2]) if len(parts) > 2 else 1
                time.sleep(delay_val / 1000)

    # Flush remaining writes
    if write_batch:
        session.init_ps(init_data=write_batch)


def _init_ps_zynq(session, hw_path, hw_file, jtagtarget):
    """Initialize PS for Zynq-7000 (cortexa9).

    Parses ps7_init.tcl to extract mask_write/mask_poll sequences.
    """
    session.targets('-s', filter=_build_filter('APU*', jtagtarget))

    # ps7_init.tcl is extracted from the .xsa at configure time (CMake
    # extract_xsa_members -> hw_path).
    init_tcl = os.path.join(hw_path, "ps7_init.tcl")
    if os.path.exists(init_tcl):
        init_data = _parse_ps_init_tcl(init_tcl, proc_name="ps7_init")
        if init_data:
            _run_init_sequence(session, init_data)
        # ps7_post_config is a subset — typically just enables level
        # shifters. Extract and run separately if present.
        post_data = _parse_ps_init_tcl(init_tcl, proc_name="ps7_post_config")
        if post_data:
            _run_init_sequence(session, post_data)
    else:
        print("WARNING: ps7_init.tcl not found, skipping PS initialization")


def _init_ps_zynqmp(session, fsbl_file, jtagtarget):
    """Initialize PS for ZynqMP (cortexa53) via FSBL handoff."""
    session.targets('-s', '--nocase',
                    filter=_build_filter('*A53*#0', jtagtarget))
    session.rst(type='processor')
    session.dow(file=os.path.normpath(fsbl_file))
    bp = session.bpadd(addr='&XFsbl_Exit')
    session.con('-b', timeout=60)
    bp.remove()
    session.targets('-s', filter=_build_filter('Cortex-A53 #0', jtagtarget))


def _init_ps_cortexr5(session, hw_path, hw_file, jtagtarget):
    """Initialize PS for Cortex-R5.

    Parses psu_init.tcl to extract mask_write/mask_poll sequences.
    """
    session.targets('-s', '--nocase',
                    filter=_build_filter('PSU', jtagtarget))

    # psu_init.tcl is extracted from the .xsa at configure time (CMake
    # extract_xsa_members -> hw_path).
    init_tcl = os.path.join(hw_path, "psu_init.tcl")
    if os.path.exists(init_tcl):
        # psu_init
        init_data = _parse_ps_init_tcl(init_tcl, proc_name="psu_init")
        if init_data:
            _run_init_sequence(session, init_data)
        time.sleep(1)

        # psu_ps_pl_isolation_removal
        iso_data = _parse_ps_init_tcl(init_tcl,
                                      proc_name="psu_ps_pl_isolation_removal")
        if iso_data:
            _run_init_sequence(session, iso_data)
        time.sleep(1)

        # psu_ps_pl_reset_config
        rst_data = _parse_ps_init_tcl(init_tcl,
                                      proc_name="psu_ps_pl_reset_config")
        if rst_data:
            _run_init_sequence(session, rst_data)
    else:
        print("WARNING: psu_init.tcl not found, skipping PS initialization")

    session.targets('-s', filter=_build_filter('Cortex-R5 #0', jtagtarget))


def _init_ps_versal(session, jtagtarget):
    """Initialize PS for Versal (cortexa72) after PDI programming."""
    filt = _build_filter('Cortex-A72*0', jtagtarget)
    session.targets('-s', filter=filt)
    session.rst('-c', type='processor')
    time.sleep(2)
    session.configparams('force-mem-accesses', 1)


def _init_ps_microblaze(session, cpu, jtagtarget):
    """Initialize target for MicroBlaze (no PS init needed)."""
    name = PL_DICT.get(cpu, "xc*")
    session.targets('-s', filter=_build_filter(name, jtagtarget))


def _write_ps(session, cpu, binary, jtagtarget):
    """Download ELF and start execution."""
    name = PS_DICT.get(cpu)
    if not name:
        print(f"ERROR: Unknown CPU '{cpu}' for PS download")
        sys.exit(1)

    session.targets('-s', filter=_build_filter(name, jtagtarget))
    time.sleep(2)
    session.dow(file=os.path.normpath(binary))
    time.sleep(2)
    session.con()
    session.disconnect()


def _connect_session(session):
    """Connect to hw_server, honoring XSCT_REMOTE_HOST/PORT env vars."""
    # Suppress known AMD bug: TCF event dispatch calls _get_target_state
    # on None node during async target enumeration after connect.
    from tcf import protocol as tcf_protocol
    class _LogFilter:
        _SUPPRESS = ("Unhandled exception in TCF event dispatch",
                     "TCF channel terminated")
        def log(self, msg, x=None):
            if any(s in str(msg) for s in self._SUPPRESS):
                return
            sys.stderr.write(str(msg) + '\n')
            if x:
                sys.stderr.write(f"{type(x).__name__}: {x}\n")
    tcf_protocol.setLogger(_LogFilter())

    remote_host = os.environ.get("XSCT_REMOTE_HOST")
    remote_port = os.environ.get("XSCT_REMOTE_PORT")
    if remote_host and remote_port:
        session.connect(host=remote_host, port=int(remote_port))
    else:
        session.connect()
    time.sleep(1)
    session.targets()


def _parse_ps_init_tcl(tcl_file, proc_name=None):
    """Parse a ps*_init.tcl file and extract mask_write/mask_poll/mask_delay.

    If proc_name is given, only extract commands from that proc and the
    data variables it references. Otherwise extract all top-level data.

    The ps*_init.tcl files use two patterns:
    1. Inline: mask_write 0xADDR 0xMASK 0xVALUE  (in Tcl data variables)
    2. Proc body: mask_write 0xADDR 0xMASK 0xVALUE  (in proc definitions)

    Both use the same format that session.init_ps() expects.
    The 'mwr -force 0xADDR 0xVALUE' pattern is converted to mask_write
    with a full mask (0xFFFFFFFF).

    Returns a list of strings for session.init_ps(init_data=...).
    """
    if not os.path.exists(tcl_file):
        return []

    with open(tcl_file, "r") as f:
        content = f.read()

    commands = []

    if proc_name:
        # Find the proc body and extract commands from it
        # Pattern: proc <name> {} { ... }
        # Also handles procs that call data variable sub-procs
        import re
        # Find proc definition
        proc_pat = re.compile(
            rf'proc\s+{re.escape(proc_name)}\s+\{{\}}\s*\{{(.*?)\n\}}',
            re.DOTALL)
        match = proc_pat.search(content)
        if not match:
            return []

        proc_body = match.group(1)
        # Extract mask_write/mask_poll/mask_delay from proc body
        for line in proc_body.splitlines():
            line = line.strip()
            cmd = _parse_init_line(line)
            if cmd:
                commands.append(cmd)

        # If proc body calls sub-procs (e.g., psu_init calls
        # psu_pll_init_data, psu_clock_init_data, etc.), recursively
        # extract their data too.
        # Detect pattern: variable_name or [sub_proc_name]
        for line in proc_body.splitlines():
            line = line.strip()
            # Skip comments and empty lines
            if not line or line.startswith('#'):
                continue
            # If it's not a mask_write/mask_poll/mwr command,
            # it might be a sub-proc call
            if not line.startswith(('mask_', 'mwr')):
                # Try as a data variable reference or sub-proc call
                sub_name = line.split()[0] if line.split() else ""
                if sub_name and sub_name not in ('set', 'if', 'return',
                                                  'after', 'variable'):
                    sub_cmds = _parse_ps_init_tcl(tcl_file,
                                                  proc_name=sub_name)
                    commands.extend(sub_cmds)
    else:
        # Extract all mask_write/mask_poll/mask_delay from the file
        for line in content.splitlines():
            line = line.strip()
            cmd = _parse_init_line(line)
            if cmd:
                commands.append(cmd)

    return commands


def _parse_init_line(line):
    """Parse a single line from ps*_init.tcl into init_ps format.

    Returns a string like 'mask_write 0xADDR 0xMASK 0xVALUE' or None.
    """
    line = line.strip()
    if line.startswith("mask_write "):
        parts = line.split()
        if len(parts) >= 4:
            return f"mask_write {parts[1]} {parts[2]} {parts[3]}"
    elif line.startswith("mask_poll "):
        parts = line.split()
        if len(parts) >= 3:
            # mask_poll may have 3 or 4 args; init_ps expects 4
            expected = parts[3] if len(parts) >= 4 else parts[2]
            return f"mask_poll {parts[1]} {parts[2]} {expected}"
    elif line.startswith("mask_delay "):
        parts = line.split()
        if len(parts) >= 2:
            return f"mask_delay {parts[1]}"
    elif line.startswith("mwr -force ") or line.startswith("mwr "):
        # Convert mwr to mask_write with full mask
        parts = line.split()
        # Skip the -force flag if present
        idx = 1
        if parts[idx] == "-force":
            idx += 1
        if len(parts) > idx + 1:
            addr = parts[idx]
            value = parts[idx + 1]
            return f"mask_write {addr} 0xFFFFFFFF {value}"
    return None


def upload(hw_path, hw_file, binary, target, fsbl_file, jtagtarget):
    """Upload ELF to target via JTAG.

    Supports: ZynqMP (cortexa53), Zynq-7000 (cortexa9),
    Cortex-R5, MicroBlaze, Versal (cortexa72).
    """
    import xsdb

    # Determine CPU from arch.txt
    arch_file = os.path.join(hw_path, "arch.txt")
    with open(arch_file, "r") as f:
        cpu = f.read().strip()
    print(f"INFO: Upload target: {cpu}")

    session = xsdb.start_debug_session()

    if "cortexa72" in cpu:
        # Versal flow: device_program for PDI, then ELF download
        _upload_versal(session, hw_path, hw_file, cpu, binary, jtagtarget)
    else:
        # Standard flow: connect, reset, bitstream, PS init, ELF
        _upload_standard(session, hw_path, hw_file, cpu, binary,
                         fsbl_file, jtagtarget)

    xsdb.dispose()


def _upload_versal(session, hw_path, hw_file, cpu, binary, jtagtarget):
    """Upload flow for Versal: device_program + A72 init + ELF download."""
    # The PDI is extracted from the .xsa at configure time (CMake
    # extract_xsa_members -> hw_path). Its name inside the XSA may differ from
    # the XSA filename, so glob for it.
    import glob as _glob
    pdi_files = _glob.glob(os.path.join(hw_path, "*.pdi"))
    if not pdi_files:
        print(f"ERROR: No PDI file found in {hw_path}. It should have been "
              "extracted from the .xsa at configure time.")
        sys.exit(1)
    pdi_path = pdi_files[0]

    # Program PDI via vivado_lab (included in Vitis Embedded Kit).
    # xsdb device_program is unreliable for Versal (ROM errors).
    print("INFO: Programming Versal PDI via vivado_lab...")
    pdi_norm = os.path.normpath(pdi_path)
    vivado_script = f'''
open_hw_manager
connect_hw_server -url TCP:localhost:3121
open_hw_target
current_hw_device [lindex [get_hw_devices xcv*] 0]
set_property PROGRAM.FILE {{{pdi_norm}}} [current_hw_device]
program_hw_devices [current_hw_device]
close_hw_manager
exit
'''
    result = subprocess.run(
        ["vivado_lab", "-mode", "tcl", "-nolog", "-nojournal"],
        input=vivado_script, text=True, capture_output=True
    )
    if result.returncode != 0 or "ERROR" in result.stdout:
        print(f"ERROR: PDI programming failed:\n{result.stdout}\n{result.stderr}")
        sys.exit(1)
    print("INFO: PDI programmed successfully. Waiting for PLM...")
    time.sleep(5)

    # Connect xsdb session for ELF download
    _connect_session(session)
    time.sleep(3)

    # Initialize A72 and download ELF
    _init_ps_versal(session, jtagtarget)
    _write_ps(session, cpu, binary, jtagtarget)


def _upload_standard(session, hw_path, hw_file, cpu, binary,
                     fsbl_file, jtagtarget):
    """Upload flow for Zynq-7000, ZynqMP, Cortex-R5, MicroBlaze."""
    _connect_session(session)
    time.sleep(3)

    # Reset CPU
    _cpu_reset(session, cpu, jtagtarget)

    # Program bitstream
    _write_pl(session, cpu, hw_path, hw_file, jtagtarget)

    # Initialize PS
    if "cortexa53" in cpu:
        _init_ps_zynqmp(session, fsbl_file, jtagtarget)
    elif "cortexa9" in cpu:
        _init_ps_zynq(session, hw_path, hw_file, jtagtarget)
    elif "cortexr5" in cpu:
        _init_ps_cortexr5(session, hw_path, hw_file, jtagtarget)
    elif "sys_mb" in cpu:
        _init_ps_microblaze(session, cpu, jtagtarget)

    # Download ELF and run
    _write_ps(session, cpu, binary, jtagtarget)


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
        "create_project": lambda: create_project(ws, hw_path, hw_file, target),
        "create_ide_workspace": lambda: create_ide_workspace(
            ws, hw_path, hw_file, binary),
        "create_fsbl": lambda: create_fsbl(ws, hw_path, hw_file, target),
        "upload": lambda: upload(hw_path, hw_file, binary, target,
                                 fsbl_file, jtagtarget),
        "clean_build": lambda: clean_build(ws),
    }

    if function not in dispatch:
        print(f"Error: Unknown function '{function}'")
        print(f"Available: {', '.join(dispatch.keys())}")
        sys.exit(1)

    dispatch[function]()


if __name__ == "__main__":
    main()
