#!/usr/bin/env python3
"""
Package a built no-OS Xilinx project into an SD-card BOOT.BIN.

The CMake build only emits <project>.elf. A bootable image also needs the FSBL
and the PL bitstream; this module collects the three and runs bootgen.

Everything about the build comes from <build_dir>/bootinfo.json, which the
Xilinx CMake flow writes at configure time (cmake/xilinx/xilinx_boot.cmake):
the arch, the .xsa, the ELF, the already-extracted bitstream and the FSBL path.
Nothing here re-derives what CMake has already resolved. The only work left is
generating the FSBL (a Vitis platform build, minutes) and running bootgen.

Importable from no_os_build.py:

    import boot_bin
    boot_bin.make_boot_bin(build_dir)

or standalone:

    tools/scripts/platform/xilinx/boot_bin.py --build-dir build/adate500-iio-zcu102
"""

import argparse
import json
import os
import shutil
import subprocess
import sys
from pathlib import Path


SCRIPT_DIR = Path(__file__).resolve().parent
BOOT_INFO_NAME = "bootinfo.json"
SUPPORTED_INFO_VERSION = 1

# HSI cell name (bootinfo "arch") -> bootgen -arch, BIF destination_cpu.
ARCH_MAP = {
    "cortexa9": ("zynq", "a9-0"),
    "cortexa53": ("zynqmp", "a53-0"),
    "cortexr5": ("zynqmp", "r5-0"),
}

# Architectures that boot some other way, with the reason to report.
UNSUPPORTED_ARCH = {
    "cortexa72": "Versal boots from a PDI built by Vivado, not from an FSBL BIF",
    "sys_mb": "MicroBlaze has no BOOT.BIN; program the bitstream+ELF as an .mcs",
    "microblaze": "MicroBlaze has no BOOT.BIN; program the bitstream+ELF as an .mcs",
}


class BootBinError(Exception):
    """Anything that stops a BOOT.BIN from being produced."""


def _vitis_tool(name):
    """Absolute path to a Vitis tool, honouring PATH then XILINX_VITIS."""
    found = shutil.which(name)
    if found:
        return found
    root = os.environ.get("XILINX_VITIS")
    if not root:
        raise BootBinError(
            f"{name} not found and XILINX_VITIS is not set; source settings64.sh "
            "from your Vitis install")
    # Vitis ships .bat wrappers on Windows; the extensionless names there are
    # bash scripts that CreateProcess cannot run.
    names = [name + ".bat", name + ".exe", name] if os.name == "nt" else [name]
    for candidate in names:
        path = Path(root) / "bin" / candidate
        if path.is_file():
            return str(path)
    raise BootBinError(f"{name} not found under {root}/bin")


def load_boot_info(build_dir):
    """Read and validate <build_dir>/bootinfo.json."""
    path = Path(build_dir) / BOOT_INFO_NAME
    if not path.is_file():
        raise BootBinError(
            f"{path} not found. It is written when a Xilinx project is "
            "configured; configure the build first.")
    try:
        info = json.loads(path.read_text())
    except json.JSONDecodeError as e:
        raise BootBinError(f"{path} is not valid JSON: {e}") from e

    version = info.get("version")
    if version != SUPPORTED_INFO_VERSION:
        raise BootBinError(
            f"{path} has schema version {version}, expected "
            f"{SUPPORTED_INFO_VERSION}; reconfigure the build.")
    return info


def resolve_arch(arch):
    """Map an HSI cell name to (bootgen -arch, BIF destination_cpu)."""
    for key, reason in UNSUPPORTED_ARCH.items():
        if key in arch:
            raise BootBinError(f"{arch}: {reason}")
    for key, mapping in ARCH_MAP.items():
        if key in arch:
            return mapping
    raise BootBinError(f"unsupported architecture: {arch}")


def generate_fsbl(info, quiet=False):
    """Build the FSBL if it is not already there. Returns its path.

    create_fsbl builds a throwaway Vitis platform component just for the boot
    loader, which takes minutes. It writes into the same workspace the 'flash'
    target uses, so the two share one FSBL.
    """
    fsbl = Path(info["fsbl"])
    if fsbl.is_file():
        return fsbl

    if not quiet:
        print("==> Generating FSBL (this takes a few minutes)", flush=True)
    ws = Path(info["fsbl_ws"])
    hw_path = ws / "tmp"
    # CMake staged arch.txt and the .xsa copy here at configure time.
    cmd = [
        _vitis_tool("vitis"), "-s", str(SCRIPT_DIR / "util.py"), "create_fsbl",
        str(ws), str(hw_path), Path(info["xsa"]).name,
        info["elf"], "0", "Empty Application(C)",
    ]
    try:
        subprocess.run(cmd, check=True)
    except subprocess.CalledProcessError as e:
        raise BootBinError(f"create_fsbl failed (exit code {e.returncode})") from e

    if not fsbl.is_file():
        raise BootBinError(f"create_fsbl reported success but {fsbl} is missing")
    return fsbl


def write_bif(path, fsbl, bitstream, elf, dest_cpu, pmufw=None):
    """Write the bootgen image description. Returns the .bif path."""
    lines = ["the_ROM_image:", "{"]
    if pmufw:
        lines.append(f"    [pmufw_image] {pmufw}")
    lines.append(f"    [bootloader, destination_cpu={dest_cpu}] {fsbl}")
    if bitstream:
        lines.append(f"    [destination_device=pl] {bitstream}")
    lines.append(f"    [destination_cpu={dest_cpu}] {elf}")
    lines.append("}")
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text("\n".join(lines) + "\n")
    return path


def make_boot_bin(build_dir, output=None, pmufw=None, bitstream=None, quiet=False):
    """Package the build in `build_dir` into a BOOT.BIN. Returns its path.

    Raises BootBinError with an actionable message on any failure.
    """
    build_dir = Path(build_dir)
    info = load_boot_info(build_dir)
    boot_arch, dest_cpu = resolve_arch(info["arch"])
    if not quiet:
        print(f"==> {info['arch']} -> bootgen -arch {boot_arch}, "
              f"destination_cpu={dest_cpu}", flush=True)

    elf = Path(info["elf"])
    if not elf.is_file():
        raise BootBinError(f"application ELF not found: {elf} (build it first)")

    bitstream = Path(bitstream) if bitstream else (
        Path(info["bitstream"]) if info.get("bitstream") else None)
    if bitstream and not bitstream.is_file():
        raise BootBinError(f"bitstream not found: {bitstream}")
    if not bitstream and not quiet:
        # A PL-less design is legal; a missing .bit in a design that needs one
        # produces an image that boots and then hangs on the first PL access.
        print("==> No bitstream in the .xsa; packaging PS-only", flush=True)

    fsbl = generate_fsbl(info, quiet=quiet)
    if pmufw:
        pmufw = Path(pmufw)
        if not pmufw.is_file():
            raise BootBinError(f"pmufw not found: {pmufw}")

    out_dir = Path(output).parent if output else Path(info["output_dir"])
    out = Path(output) if output else out_dir / "BOOT.BIN"
    bif = write_bif(out_dir / f"{info['project']}.bif",
                    fsbl, bitstream, elf, dest_cpu, pmufw)

    if not quiet:
        print("==> Running bootgen", flush=True)
    cmd = [_vitis_tool("bootgen"), "-arch", boot_arch,
           "-image", str(bif), "-o", str(out), "-w"]
    try:
        subprocess.run(cmd, check=True)
    except subprocess.CalledProcessError as e:
        raise BootBinError(f"bootgen failed (exit code {e.returncode})") from e
    # bootgen drops a profiling report in the working directory; not an artifact.
    Path("dfx_runtime.txt").unlink(missing_ok=True)

    if not quiet:
        print(f"\nBOOT.BIN  {out}")
        print(f"  fsbl      {fsbl}")
        print(f"  bitstream {bitstream or '(none)'}")
        print(f"  app       {elf}")
        if pmufw:
            print(f"  pmufw     {pmufw}")
        print("\nCopy it to the FAT32 SD card root, set the board to SD boot "
              "mode, power-cycle.")
    return out


def main():
    parser = argparse.ArgumentParser(
        description=__doc__.strip().split("\n")[0],
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument(
        "--build-dir", required=True,
        help=f"Configured build directory (the one holding {BOOT_INFO_NAME})")
    parser.add_argument("-o", "--output", help="Output image (default: <build>/boot/BOOT.BIN)")
    parser.add_argument(
        "--bitstream", help="Use this .bit instead of the one named in bootinfo.json")
    parser.add_argument(
        "--pmufw",
        help="PMU firmware to embed (ZynqMP). Not included by default; the "
             "platform export leaves one in .../export/hw0/sw/qemu/")
    args = parser.parse_args()

    try:
        make_boot_bin(args.build_dir, output=args.output, pmufw=args.pmufw,
                      bitstream=args.bitstream)
    except BootBinError as e:
        sys.exit(f"error: {e}")


if __name__ == "__main__":
    main()
