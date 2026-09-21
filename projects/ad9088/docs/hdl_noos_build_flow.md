# Quartus/HDL → no-OS build flow (ad9084_ebz / Nios V, Agilex 5)

How the AD9084-EBZ FPGA design (Quartus/HDL) connects to the ad9088 no-OS
software build for the Agilex 5 Nios V/g soft core: what the HDL produces, what
the software consumes, and exactly which facts must be kept in sync between them.

## Big picture: three stages, one handoff

```
┌─ STAGE 1: HDL / Quartus ────────────┐   ┌─ STAGE 2: BSP ──────┐   ┌─ STAGE 3: no-OS app ─┐
   *.tcl + ADI hdl lib + Intel IP          niosv-bsp reads the       CMake reads the BSP
        │  make → quartus_sh                hardware handoff          (headers + linker +
        ▼                                        │                     HAL archive)
   qsys-generate ──► system_bd.qsys  ───────────►│                          │
        │           system_bd.sopcinfo           ▼                          ▼
        ▼           (THE HANDOFF)           bsp/ (system.h,           build/ad9088 (.elf)
   synth/fit/asm ──► .sof (bitstream)       linker.x, HAL/, …)       (runs on the .sof)
```

The **hardware handoff** (`system_bd.qsys` + `system_bd/system_bd.sopcinfo`) is
the single contract between HDL and software. Everything no-OS needs about the
fabric comes from it, funnelled through the BSP.

## Stage 1 — HDL / Quartus build

Command:

```
cd hdl/projects/ad9084_ebz/nios_a5e && [ADI_IGNORE_VERSION_CHECK=1] make
```

`make` runs `quartus_sh --64bit -t system_project.tcl`, which sources the Qsys
tcl, runs `qsys-generate`, then `execute_flow -compile`.
(`ADI_IGNORE_VERSION_CHECK=1` is needed when the installed Quartus Pro is 26.1.1
but the project pins 26.1.0.)

### Inputs

| Input | Role |
|---|---|
| `system_project.tcl` | top: device, pins, SDC, JESD defaults, `execute_flow -compile` |
| `system_qsys.tcl` (project) | apollo_spi @0xEA000, GTS PLLs, sysid; sources the common tcl |
| `common/ad9084_ebz_qsys.tcl` | JESD datapath: DMA, TPL, link_reconfig/mgmt, data_offload, PHY, IRQs, device clocks |
| `common/nios_a5e/nios_a5e_system_qsys.tcl` | the CPU carrier: Nios V/g, `sys_int_mem` OCM, dm_agent, timer, uart, gpio, sys_spi, DDR window |
| `system_top.v`, `*.sdc`, board pin tcl | RTL wrapper, timing, pin assignments |
| `LIB_DEPS` / `INTEL_LIB_DEPS` (Makefile) | ADI IP (axi_dmac, jesd204 tpl, util_pack, sysid) + Intel IP (adi_jesd204, jesd204_phy) built under `hdl/library/` |
| `ad_project_params` | JESD mode/rate/M/L/S/NP, clock rates |

### Outputs

| Output | Consumed by |
|---|---|
| `ad9084_ebz_nios_a5e.sof` | JTAG config of the FPGA (`quartus_pgm`) |
| `system_bd.qsys` | ← `niosv-bsp -s` (Stage 2 input) |
| `system_bd/system_bd.sopcinfo` | human/tool-readable handoff (NOT accepted by niosv-bsp) |
| `.qpf` / `.qsf` | ← `niosv-bsp -p` (Stage 2 input) |
| `system_bd/` IP tree, reports | fitter/timing artifacts |

`make clean` deletes `system_bd.qsys`, `system_bd/`, `.sopcinfo`, `.sof`,
`.qpf` — the handoff is regenerated from tcl every build; never hand-edit it.

## Stage 2 — BSP generation (the bridge)

Command (what the no-OS CMake integration runs, and what you run by hand to
pre-build a BSP):

```
niosv-bsp -c -t=hal -p=<project.qpf> -s=<system_bd.qsys> <out>/settings.bsp
```

Inputs = the `.qpf` + `system_bd.qsys` handoff. Output = a BSP directory whose
files carry the HDL facts into software:

| BSP file | What it carries (the HDL→SW contract) |
|---|---|
| `system.h` | THE memory map: every `*_BASE`, `*_SPAN`, `*_IRQ`, `*_NAME`; CPU arch/caches; `ALT_CPU_RESET_ADDR`, `ALT_CPU_MTIME_OFFSET`, clock freqs |
| `linker.x` + `linker.h` | memory regions (OCM origin/length) and section→region placement; reset vector; alt_load setting |
| `toolchain.cmake` | ISA/ABI: `-march=rv32im_zicbom -mabi=ilp32`, `-nostdlib`, prefix `riscv32-unknown-elf-` |
| `alt_sys_init.c` | generated device-init table (which devices exist, in what order) |
| `HAL/` + `drivers/` | Altera HAL + Avalon device drivers/headers |
| `CMakeLists.txt` | builds the BSP into `libhal2_bsp.a` |
| `memory.gdb` | gdb memory-map (for debug sessions) |
| `settings.bsp`, `summary.html` | the settings used / a readable report |

### Building the BSP archive

`niosv-bsp` emits BSP **source**; the no-OS link needs the compiled archive:

```
cd <bsp> && cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build
# → <bsp>/build/libhal2_bsp.a
```

### alt_load must be OFF for JTAG download

By default the BSP enables `alt_load()`, which sets section LMAs after `.text`
for a boot-copy. For the JTAG-download workflow the loader writes each segment to
its own address, so LMA must equal VMA. Turn alt_load off before building:

```
niosv-bsp --update <bsp>/settings.bsp \
  "--cmd=set_setting hal.linker.enable_alt_load false" \
  "--cmd=set_setting hal.linker.enable_alt_load_copy_rodata false" \
  "--cmd=set_setting hal.linker.enable_alt_load_copy_rwdata false"
```

(Use the `--cmd=...` equals form; the space form fails. Verify afterwards that
`linker.x` no longer prints the "alt_load facility is enabled" banner, and that
`readelf -l` shows PhysAddr == VirtAddr on the LOAD segments.)

## Stage 3 — how no-OS consumes the BSP

`cmake/altera/altera_platform_sdk.cmake` (`config_altera_sdk`):

- Resolves the BSP dir from `ALTERA_BSP_DIR` (CMake cache var **or** env; cache
  var wins), else auto-generates one from `ALTERA_NIOSV_REPO_DIR`.
- Adds includes: `${BSP}`, `${BSP}/HAL/inc`, `${BSP}/drivers/inc` (+ the project
  `src/platform/altera`).
- Links the BSP archive (`ALTERA_BSP_LIB` cache var or env, else auto-located),
  with `-nostdlib -T${BSP}/linker.x`.

The application ISA/ABI is set in `drivers/platform/altera/toolchain.cmake`
(Nios V branch): `-march=rv32im_zicbom -mabi=ilp32`, matching the core and the
BSP archive.

### One-command build (this checkout)

The machine-specific BSP/toolchain paths are wired in an untracked
`CMakeUserPresets.json` preset `agilex5-ebz` (inherits the tracked `agilex5`
preset), so:

```
cmake --preset agilex5-ebz
cmake --build build_agilex5_ebz --target ad9088
```

## What must be kept in sync between HDL and no-OS

Everything the app needs about the fabric is in `system.h` and `linker.x`:

| # | Handoff item | Where no-OS uses it | State |
|---|---|---|---|
| 1 | Peripheral base addresses (`*_BASE`) | `src/platform/altera/parameters.{h,c}`, `src/common/common_data.c` `.base` | Historically hardcoded (reverse-engineered); the values match the EBZ `system.h` `*_BASE` exactly, so they can be sourced from `system.h`. |
| 2 | IRQ numbers (`*_IRQ`), interrupt controller (CLIC) | UART/DMA/JESD IRQs | Partly hardcoded |
| 3 | Memory regions (OCM origin/len, reset) | the linker script the app links against | Now the EBZ `linker.x` via `ALTERA_BSP_DIR` |
| 4 | ISA/ABI (`-march=rv32im_zicbom -mabi=ilp32`) | app compile/link flags | Set in the Altera toolchain (Nios V branch) |
| 5 | CPU clock / mtime base | timer/delay drivers | `ALT_CPU_MTIME_OFFSET` from `system.h` |

### Sync failure modes

- Wrong `*_BASE` → SPI/GPIO/DMA read garbage or hang.
- Wrong `linker.x` region → app doesn't fit / won't load / fetches from
  unreachable memory (the Nios V instruction master reaches only OCM + dm_agent).
- Wrong `-march` → illegal-instruction traps, or missed mul/div/cache ops.
- Wrong IRQ / controller model → interrupts never fire.

## Notes specific to this design

- Nios V/g is Harvard-like on AXI: the **instruction master reaches only
  `sys_int_mem` (OCM) + dm_agent**; the data master reaches everything; DDR is
  data-master-only. The full app therefore has to live in OCM (this design
  enlarges OCM to 1.5 MB to fit it — see the Qsys carrier tcl).
- Interrupt controller is **CLIC** (not CLINT/PLIC), 48 interrupts.
- Core is `rv32im` + Zicbom (4 KB I/D caches, 32 B lines); no FPU → soft-float.
- Software is JTAG-downloaded (`niosv-download`), not booted from the bitstream;
  the reset vector stays in OCM.
