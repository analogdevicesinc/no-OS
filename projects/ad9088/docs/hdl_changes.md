# HDL changes required for the ad9088 Agilex 5 / Nios V build

These are the HDL-side changes the no-OS `ad9088` project on Agilex 5 depends
on. They live in the HDL repo on branch **`dev_agilex5_ad9084`**
(`projects/ad9084_ebz/nios_a5e` and `projects/common/nios_a5e`).

> **Regenerate the BSP after taking these.** Two of the changes move Nios V
> peripheral base addresses and enlarge on-chip memory, so `system.h` and the
> linker script change — an old BSP will not match the new bitstream.

## 1. SYSREF port renamed `sysref_out` -> `sysref_in` (committed / pending)

`projects/ad9084_ebz/nios_a5e/system_project.tcl`

The CLK2 bidirectional pins (PIN_T65 / PIN_P65) are used as an **input** on the
AD9084-EBZ (SYSREF is driven into the FPGA). The port, its `IO_STANDARD`,
`INPUT_TERMINATION` and both pin-location assignments were renamed from
`sysref_out` to `sysref_in` to match. No pin locations changed.

## 2. On-chip memory 256 KB -> 1.5 MB, peripherals relocated (pending)

`projects/common/nios_a5e/nios_a5e_system_qsys.tcl`

The Nios V/g `instruction_manager` only reaches on-chip memory (+ the debug
module), so the full ad9088 application must fit in OCM. `sys_int_mem` was
enlarged from 256 KB (`262144`) to 1.5 MB (`1572864`). Because OCM now ends at
`0x1017FFFF`, three agents were moved up out of the way:

| Agent                     | Old base     | New base     |
|---------------------------|--------------|--------------|
| `sys_cpu.dm_agent`        | `0x10100000` | `0x10300000` |
| `sys_cpu.timer_sw_agent`  | `0x10110000` | `0x10310000` |
| `sys_ddr_window.cntl`     | `0x10120000` | `0x10320000` |

Unchanged: `sys_int_mem` base `0x10000000`; `sys_ddr_window.windowed_slave`
(256 KB DDR view) at `0x10200000`.

## 3. JESD204B fix (committed)

Commit `0fb20f251` "projects: ad9084_ebz: a5e: Fix JESD204B" on the same
branch. Required for the 204B link on this board.

## Build

Clean HDL rebuild after any Qsys `.tcl` edit so the generated Qsys is fully
regenerated, then rebuild the BSP:

```
cd hdl/projects/ad9084_ebz/nios_a5e
ADI_IGNORE_VERSION_CHECK=1 make clean && ADI_IGNORE_VERSION_CHECK=1 make
```

See [hdl_noos_build_flow.md](hdl_noos_build_flow.md) for the full
HDL -> BSP -> no-OS handoff.
