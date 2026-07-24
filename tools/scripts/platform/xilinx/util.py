#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-2-Clause
# Copyright (c) 2026 Analog Devices, Inc. All rights reserved.
"""
Xilinx/AMD platform utilities for no-OS build system.

Drives BSP/FSBL generation and JTAG programming via the Vitis 2025+ Python
API (xsdb/hsi). Invoked via: vitis -s util.py <function> <args...>

Arguments (positional):
  function  - Function to call: get_arch, create_project, create_fsbl,
               clean_build, upload
  ws        - Workspace/project path
  hw_path   - Hardware definition directory
  hw_file   - Hardware file name (e.g. system_top.xsa)
  binary    - ELF binary path
  target    - Target CPU filter (empty or "0" for auto-select)
  template  - App template name (unused in 2025+ flow)
  fsbl_file - FSBL ELF path (optional, for upload)
  jtagtarget - JTAG cable identifier (optional, for upload)
"""

import os
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
    ld = app.get_ld_script()
    ld.set_heap_size('0x100000')

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

    vitis.dispose()
    print("INFO: Project created successfully")


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
    bit_files = _glob.glob(os.path.join(hw_path, "*.bit"))
    if not bit_files:
        # Fall back to name derived from XSA
        bitstream = os.path.join(hw_path,
                                 os.path.splitext(hw_file)[0] + ".bit")
    else:
        bitstream = bit_files[0]
    session.fpga(file=os.path.normpath(bitstream))


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

    init_tcl = os.path.join(hw_path, "ps7_init.tcl")
    if not os.path.exists(init_tcl):
        # Extract from XSA if not already present
        xsa = os.path.join(hw_path, hw_file)
        subprocess.run(["unzip", "-o", "-q", xsa, "ps7_init.tcl",
                        "-d", hw_path], check=False)

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

    init_tcl = os.path.join(hw_path, "psu_init.tcl")
    if not os.path.exists(init_tcl):
        xsa = os.path.join(hw_path, hw_file)
        subprocess.run(["unzip", "-o", "-q", xsa, "psu_init.tcl",
                        "-d", hw_path], check=False)

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
    xsa_path = os.path.join(hw_path, hw_file)

    # Extract PDI from XSA if no .pdi files present
    import glob as _glob
    if not _glob.glob(os.path.join(hw_path, "*.pdi")):
        subprocess.run(["unzip", "-o", "-q", xsa_path, "*.pdi",
                        "-d", hw_path], check=False)

    # Find the extracted PDI (name inside XSA may differ from XSA filename)
    pdi_files = _glob.glob(os.path.join(hw_path, "*.pdi"))
    if not pdi_files:
        print(f"ERROR: No PDI file found in {hw_path}")
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
