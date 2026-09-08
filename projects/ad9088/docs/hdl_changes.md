# HDL changes required for the ad9088 Agilex 5 / Nios V build

These are the HDL-side changes the no-OS `ad9088` project on Agilex 5 depends
on. They live in the HDL repo on branch **`dev_agilex5_ad9084`**
(`projects/ad9084_ebz/nios_a5e` and `projects/common/nios_a5e`).

> **Regenerate the BSP after taking these.** Two of the changes move Nios V
> peripheral base addresses and enlarge on-chip memory, so `system.h` and the
> linker script change — an old BSP will not match the new bitstream.

## 1. SYSREF port `sysref_out` used as an input (committed)

`projects/ad9084_ebz/nios_a5e/system_top.v`, `system_project.tcl`,
`system_constr.sdc`

Commit `21262e71d` "projects: ad9084_ebz: a5e: Use sysref_out port as SYSREF
input" on the same branch. On the AD9084-EBZ, SYSREF is generated externally
(ADF4030) and driven **into** the FPGA, so the CLK2 bidirectional pins
(PIN_T65 / PIN_P65) are used as an **input**. The top-level port keeps the base
design's name `sysref_out` but is declared `input`, carries
`INPUT_TERMINATION DIFFERENTIAL`, and feeds both the JESD204 PHY
`rx_sysref_export` and `tx_sysref_export`. (An earlier revision renamed this
port to `sysref_in`; that rename has since been reverted - the name is
`sysref_out` again, still wired as an input. No pin locations changed.)

## 2. On-chip memory 256 KB -> 2 MB, peripherals relocated (committed)

`projects/common/nios_a5e/nios_a5e_system_qsys.tcl`, commit `9941339e7`
"projects: common: nios_a5e: Enlarge OCM to 1.5 MB for full app" (later grown
to 2 MB and made dual-port on the same branch).

The Nios V/g `instruction_manager` only reaches on-chip memory (+ the debug
module), so the full ad9088 application must fit in OCM. `sys_int_mem` was
enlarged from 256 KB (`262144`) to 2 MB (`2097152`) and given a second port
(`s2`) for the DMA / JTAG masters. Because OCM now ends at `0x101FFFFF`, the
CPU agents and the DDR window were moved up out of the way:

| Agent                             | Old base     | New base     |
|-----------------------------------|--------------|--------------|
| `sys_cpu.dm_agent`                | `0x10100000` | `0x10400000` |
| `sys_cpu.timer_sw_agent`          | `0x10110000` | `0x10410000` |
| `sys_ddr_window.cntl`             | `0x10120000` | `0x10420000` |
| `sys_ddr_window.windowed_slave`   | `0x10200000` | `0x10500000` |

Unchanged: `sys_int_mem` base `0x10000000`. The DDR window
(`windowed_slave`) is now a 1 MB view (`1048576`).

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
