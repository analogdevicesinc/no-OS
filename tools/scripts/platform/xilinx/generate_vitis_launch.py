#!/usr/bin/env python3
"""
Generate Vitis 2025.1+ launch.json configuration for no-OS projects.

This script automatically creates debug configurations for the Vitis Unified IDE,
eliminating the need for manual configuration steps.
"""

import argparse
import json
import os
import sys
from pathlib import Path


def detect_arch_config(arch):
    """
    Detect architecture-specific configuration based on ARCH string.

    Returns: dict with debugType, targetCpu, useFsbl, initType
    """
    arch_lower = arch.lower()

    if 'cortexa53' in arch_lower or 'psu_cortexa53' in arch_lower:
        return {
            'debugType': 'baremetal-zu',
            'targetCpu': 'psu_cortexa53_0',
            'useFsbl': True,
            'initType': 'zu'
        }
    elif 'cortexr5' in arch_lower or 'psu_cortexr5' in arch_lower:
        return {
            'debugType': 'baremetal-zu',
            'targetCpu': 'psu_cortexr5_0',
            'useFsbl': True,
            'initType': 'zu'
        }
    elif 'cortexa9' in arch_lower or 'ps7_cortexa9' in arch_lower:
        return {
            'debugType': 'baremetal-zynq',
            'targetCpu': 'ps7_cortexa9_0',
            'useFsbl': True,
            'initType': 'zynq'
        }
    elif 'microblaze' in arch_lower or 'sys_mb' in arch_lower:
        return {
            'debugType': 'baremetal-mb',
            'targetCpu': 'microblaze_0',
            'useFsbl': False,
            'initType': 'mb'
        }
    elif 'cortexa72' in arch_lower or 'psv_cortexa72' in arch_lower:
        return {
            'debugType': 'baremetal-versal',
            'targetCpu': 'psv_cortexa72_0',
            'useFsbl': False,
            'initType': 'versal'
        }
    else:
        print(f"Warning: Unknown architecture '{arch}'. Using default ZynqMP settings.", file=sys.stderr)
        return {
            'debugType': 'baremetal-zu',
            'targetCpu': 'psu_cortexa53_0',
            'useFsbl': True,
            'initType': 'zu'
        }


def generate_zu_init(fsbl_path, xsa_basename):
    """Generate ZynqMP (Zynq Ultrascale+) initialization section."""
    return {
        "zuInitialization": {
            "isFsbl": True,
            "usingFSBL": {
                "initWithFSBL": True,
                "fsblExitSymbol": "XFsbl_Exit",
                "fsblFile": f"${{workspaceFolder}}/{fsbl_path}"
            },
            "usingPsuInit": {
                "runPsuInit": True,
                "plPowerup": True,
                "psuInitTclFile": f"${{workspaceFolder}}/_ide/{xsa_basename}/psu_init.tcl"
            }
        },
        "zuTraceOptions": {
            "isSelected": False,
            "scratchAddress": "0x60000",
            "scratchSize": "0x60000",
            "traceOutputPath": ""
        }
    }


def generate_zynq_init(fsbl_path, xsa_basename):
    """Generate Zynq-7000 initialization section."""
    return {
        "zynqInitialization": {
            "isFsbl": True,
            "usingFSBL": {
                "initWithFSBL": True,
                "fsblExitSymbol": "FsblExit",
                "fsblFile": f"${{workspaceFolder}}/{fsbl_path}"
            },
            "usingPsInit": {
                "runPsInit": True,
                "plPowerup": True,
                "psInitTclFile": f"${{workspaceFolder}}/_ide/{xsa_basename}/ps_init.tcl"
            }
        }
    }


def generate_mb_init():
    """Generate MicroBlaze initialization section (no FSBL)."""
    return {
        "mbInitialization": {
            "initWithFSBL": False
        }
    }


def generate_versal_init():
    """Generate Versal initialization section (uses PLM, not FSBL)."""
    return {
        "versalInitialization": {
            "usePLM": True,
            "plmFile": ""  # Typically extracted from PDI
        }
    }


def generate_download_elf(target_cpu, elf_path):
    """Generate downloadElf section."""
    return [
        {
            "core": target_cpu,
            "resetProcessor": False,
            "elfFile": elf_path,
            "stopAtEntry": False,
            "isSelfRelocatingApp": False,
            "relativeAddress": ""
        }
    ]


def generate_launch_json(args):
    """Generate the complete launch.json file."""

    # Detect architecture configuration
    arch_config = detect_arch_config(args.arch)

    # Extract XSA basename (e.g., "system_top" from "system_top.xsa")
    xsa_basename = Path(args.xsa_path).stem

    # Generate bitstream path
    bitstream_path = f"_ide/{xsa_basename}/{xsa_basename}.bit"

    # Generate configuration name
    config_name = f"{args.project_name}_app_hw_1"

    # Generate architecture-specific initialization section
    if arch_config['initType'] == 'zu':
        init_section = generate_zu_init(args.fsbl_path, xsa_basename)
    elif arch_config['initType'] == 'zynq':
        init_section = generate_zynq_init(args.fsbl_path, xsa_basename)
    elif arch_config['initType'] == 'mb':
        init_section = generate_mb_init()
    elif arch_config['initType'] == 'versal':
        init_section = generate_versal_init()
    else:
        init_section = {}

    # Build the complete configuration
    config = {
        "version": "0.2.0",
        "configurations": [
            {
                "type": "tcf-debug",
                "request": "launch",
                "name": config_name,
                "debugType": arch_config['debugType'],
                "xsaPath": f"${{workspaceFolder}}/{args.xsa_path}",
                "attachToRunningTargetOptions": {
                    "targetSetupMode": "standalone",
                    "executeScript": True,
                    "scriptPath": ""
                },
                "autoAttachProcessChildren": False,
                "target": {
                    "targetConnectionId": "Local",
                    "peersIniPath": "../_ide/.peers.ini",
                    "context": "Device"
                },
                "pathMap": [],
                "targetSetup": {
                    "resetSystem": True,
                    "programDevice": True,
                    "partialBitstream": False,
                    "skipRevisionCheck": False,
                    "device": {
                        "plDevice": "Auto Detect",
                        "psDevice": "Auto Detect"
                    },
                    "enableRPUSplitMode": False,
                    "resetAPU": False,
                    "resetRPU": False,
                    "bitstreamFile": f"${{workspaceFolder}}/{bitstream_path}",
                    **init_section,
                    "downloadElf": generate_download_elf(arch_config['targetCpu'], args.elf_path),
                    "crossTriggerBreakpoints": {
                        "isSelected": False,
                        "breakpoints": []
                    }
                },
                "internalConsoleOptions": "openOnSessionStart"
            }
        ]
    }

    return config


def main():
    parser = argparse.ArgumentParser(
        description='Generate Vitis 2025.1+ launch.json for no-OS projects'
    )
    parser.add_argument('--arch', required=True,
                        help='Architecture string (e.g., psu_cortexa53_0, ps7_cortexa9_0)')
    parser.add_argument('--project-name', required=True,
                        help='Project name (e.g., adrv904x)')
    parser.add_argument('--xsa-path', required=True,
                        help='XSA file path relative to project (e.g., system_top.xsa)')
    parser.add_argument('--elf-path', required=True,
                        help='Absolute path to ELF file')
    parser.add_argument('--fsbl-path', required=True,
                        help='FSBL path relative to project (e.g., build/tmp/output/hw0/export/hw0/sw/hw0/boot/fsbl.elf)')
    parser.add_argument('--project-dir', required=True,
                        help='Project directory (absolute path)')
    parser.add_argument('--output', required=True,
                        help='Output path for launch.json')

    args = parser.parse_args()

    # Generate the launch configuration
    launch_config = generate_launch_json(args)

    # Ensure output directory exists
    output_path = Path(args.output)
    output_path.parent.mkdir(parents=True, exist_ok=True)

    # Write the JSON file
    with open(output_path, 'w') as f:
        json.dump(launch_config, f, indent='\t')
        f.write('\n')  # Add trailing newline

    print(f"Generated Vitis launch configuration: {output_path}")
    print(f"  Architecture: {args.arch}")
    arch_config = detect_arch_config(args.arch)
    print(f"  Debug type: {arch_config['debugType']}")
    print(f"  Target CPU: {arch_config['targetCpu']}")
    print(f"  Uses FSBL: {arch_config['useFsbl']}")


if __name__ == '__main__':
    main()
