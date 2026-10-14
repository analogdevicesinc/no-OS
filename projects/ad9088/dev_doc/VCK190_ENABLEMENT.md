# Enabling the VCK190 for the AD9088 project

Status: **CODE COMPLETE AND BUILDING. Sections A-D are implemented, committed,
and the VCK190 ELF links under the real Vitis 2025.1 flow. Nothing has been run
on hardware yet — that is the next step, and the whole point of the exercise is
still unproven until the link comes up.** Last updated 2026-10-15.

All five commits are on `dev/ad9088_rebase`:

```
6e4764dfa cmake: Add missing defconfig and board conf
4d983e4ee project:ad9088: Drop the adxcvr on Versal carriers
948a02855 project:ad9088: Add vck190 platform parameters
4eb1cd413 project:ad9088: functionality clean up
a8cfd4c88 cmake: Add support for the vck190
```

## First real build — 2026-10-15

```
build/ad9088-basic_example-vck190/build/ad9088.elf
  ELF 64-bit LSB executable, ARM aarch64, statically linked, 4.7 MB
```

Every assumption that could not be checked without Vitis held:

- `get_arch` returns `sys_cips_pspmc_0_psv_cortexa72_0`; the new `cortexa72`
  arm selects `aarch64-none-elf` and finds the toolchain.
- The BSP generates and is auto-detected as SDT, so `-DSDT` reaches both the
  library and the app.
- `XPAR_XUARTPSV_0_INTR` resolves, which vindicates the `xilinx_compat.h`
  fallback added in A.5, as do `XPAR_XUARTPSV_0_BASEADDR`,
  `XPAR_XSPIPS_0_BASEADDR`, `XPAR_XSPI_0_BASEADDR` and `XPAR_XGPIO_0_BASEADDR`.
- `xgpiops.h` is present, so `CONFIG_GPIO_IRQ_XILINX` in the board defconfig
  compiles, as `gpiops.yaml` predicted.

Confirmed from `compile_commands.json` that the app really was built with
`-DBOARD_VCK190 -DSDT -DXILINX_PLATFORM`.

### Build had to bypass CMake presets

`CMakeUserPresets.json` (untracked, local) includes
`projects/ad9088/CMakeAgilexPreset.json`, which in turn includes
`board_configs/altera/CMakePresets.json` — a path that does not exist in this
tree. CMake refuses to parse **any** presets when one include is missing, so
`--preset vck190`, `--preset vcu118` and `no_os_build.py build` are all broken
for every project until that is fixed or the file is moved aside. The build
above used the preset's cache variables directly instead:

```bash
source /xilinx/2025.1/Vitis/settings64.sh
cmake -B build/ad9088-basic_example-vck190 -G Ninja \
      -DBOARD=vck190 -DPLATFORM=xilinx -DUSE_VENDOR_TOOLCHAIN=ON \
      -DBOARD_CONFIG_FILE=vck190_defconfig \
      -DPROJECT_DEFCONFIG=ad9088/basic_example.conf \
      -DHARDWARE="$PWD/projects/ad9088/system_top_vck190.xsa"
cmake --build build/ad9088-basic_example-vck190 --target ad9088
```

Worth knowing when that fails: `no_os_build.py` crashes in its own error
handler rather than reporting the cause. `append_log()` (`no_os_build.py:510`)
opens `<build_dir>/build.log` for append, but on a configure failure that
directory was never created, so you get a `FileNotFoundError` traceback while
cmake's real stderr stays swallowed by `capture_output=True`.

### Correction: the predicted format warnings do not appear

An earlier revision of this document claimed the VCK190 build would surface
eight `%lld` / `int64_t` warnings from `basic_example.c` that the VCU118 build
does not. It does not, and the reason is not the one that would be assumed:
`int64_t` really is `long` under `aarch64-none-elf`, so the warnings are
genuine — they simply never fire, because **project sources are not compiled
with `-Wall` at all**. `CMakeLists.txt:286` applies `-Wall` as `PRIVATE` to the
`no-os` library target, so `basic_example.c` and `common_data.c` get neither
`-Wall` nor `-Wextra`. CLAUDE.md's "must build clean with `-Wall -Wextra`" is
therefore not enforced for project sources by this build system, and the eight
warnings remain latent. They are still worth fixing (`PRId64`, or a
`(long long)` cast) before upstreaming.

The 63 warnings the build does emit are all from the no-OS library side — the
vendor Apollo sources, `hmc7044.c`, `no_os_circular_buffer.c` and format
expansions in `no_os_print_log.h`. None originate in `projects/ad9088/src`.

### Not yet done

- **No VCU118 regression build.** This change touches shared files — both
  toolchain.cmake copies, `xilinx_compat.h`, and all three ad9088 sources — so
  the VCU118 build is the real regression test and has not been run. There is
  no plain `system_top_vcu118.xsa` in the project directory, only `.orig` and
  `.back` variants, so which one to use is a judgement call.
- **No hardware run.** See the Verification section below for what to look for.

`projects/ad9088` builds only for the VCU118 (MicroBlaze). A VCK190 XSA now sits
at `projects/ad9088/system_top_vck190.xsa` — gitignored by the root `*.xsa` rule,
so it stays a local artifact handed to the build as `HARDWARE`.

Scope agreed before starting: **full bring-up, `basic_example` only**, device
profile `204C_M4_L4_NP16_20p0_4x4.bin` (already in-tree). No `dma_example` board
conf and no `README.rst` changes in this pass.

Grounded in:

- `projects/ad9088/system_top_vck190.xsa` — `system.hwh` read directly out of the
  archive; the authority on what the design actually instantiates
- `arch/arm64/boot/dts/xilinx/versal-vck190-reva-ad9084.dts` (kernel tree) — the
  authority on the board wiring, chip selects and GPIO numbering
- `arch/microblaze/boot/dts/vcu118_ad9084_204C_M4_L8_NP16_20p0_4x2_CLL.dts` — the
  VCU118 counterpart the current no-OS config was derived from
- `drivers/axi_core/jesd204/axi_jesd204_{rx,tx}.c`, `jesd204_clk.c`
- `drivers/platform/xilinx/{toolchain.cmake,xilinx_spi.c,xilinx_uart.c,xilinx_compat.h}`
- `tools/scripts/platform/xilinx/util.py`

## What the VCK190 design actually is

Read out of `system.hwh`. One `axi_jesd204_rx` / `axi_jesd204_tx` pair,
`LINK_MODE = 2` (204C), `NUM_LANES = 8`; TPL cores `NUM_CHANNELS = 8`,
`NUM_LANES = 8`, `CONVERTER_RESOLUTION = 16`, `SAMPLES_PER_FRAME = 1`. The DTS
confirms this is **two links of M4/L4**, not one wide link —
`jesd204-link-ids = <DEFRAMER_LINK_A0_TX DEFRAMER_LINK_B0_TX FRAMER_LINK_A0_RX
FRAMER_LINK_B0_RX>` with separate `jrx0`/`jrx1` and `jtx0`/`jtx1` lane maps. That
is what makes `204C_M4_L4_NP16_20p0_4x4.bin` the right profile, and it is the
default the VCK190 DTS itself carries.

Differences from the VCU118 that reach the firmware:

| | VCU118 | VCK190 |
|---|---|---|
| CPU | MicroBlaze `sys_mb` | `sys_cips_pspmc_0_psv_cortexa72_0` |
| Clock-chip SPI | `axi_spi` (PL), CS 0/1/4 | **PS SPI0** (`XSpiPs`), CS 0/1/2 |
| Apollo SPI | `axi_spi_2` (PL), XSpi #1 | `axi_spi_2` (PL), **XSpi #0** — only PL XSpi |
| Transceiver | `axi_adxcvr` RX/TX | **none** — `gtwiz_versal` + `jesd204_versal_gt_adapter_*`, reset over `axi_gpio` |
| Apollo reset | `axi_gpio` 62 (ch2 b30) | `axi_gpio` **30** (ch1 b30) |
| Lanes / converters | M4/L8 x2 links | M4/L4 x2 links |
| Data offload | `MEM_SIZE_LOG2 = 19` | `MEM_SIZE_LOG2 = 20` (read at runtime, no code change) |

Everything else is the same board-level clock tree — 125 MHz reference, HMC7044
PLL2 at 2.5 GHz, `HMC7044_CORE_CLK_DIV 8` → 312.5 MHz device clock, 20.625 Gbps
lanes, 9.765625 MHz SYSREF, ADF4030 channels 0/5/8, `adi,spi-3wire-enable` on
both the ADF4382 and the ADF4030. So the clocking block in `common_data.c`
carries over untouched, which is most of the risk gone.

The DMA, data-offload, TPL and JESD base addresses need **no** board branch: the
block-design instance names are identical across the two XSAs, so the generated
`XPAR_*_BASEADDR` names match.

## What already exists in shared code

Worth stating plainly, because it is why this is mostly a project-level job and
nothing new is needed under `drivers/`:

- `axi_jesd204_{rx,tx}` already accept `gt_reset_pll` / `gt_reset_dp` /
  `gt_reset_done` GPIO init params and fire `axi_jesd_ext_reset()` from the FSM
  `clks_enable` and `link_enable` ops. `init` even sets the done pin to input.
- `jesd204_clk_{enable,disable,set_rate}` all guard on `clk->xcvr`, so a NULL
  xcvr is a supported configuration — the lane-clock plumbing stays as-is.
- `xilinx_uart.c` already aliases the whole `XUartPsv` API onto `XUartPs`.
- `xilinx_spi.c`'s `SPI_PS` path takes `xinit->base_addr` under SDT and reads the
  input clock from the config, so it has no `XPAR_*_CLK_FREQ_HZ` dependency.
- `util.py` already has `_upload_versal()` (PDI via `vivado_lab`, xsdb for the
  ELF), `_generate_versal_irq_macros()` and `_vitis_cpu_name()`.

The gap is the toolchain arch branch, the board preset/defconfig, and the
project's own board assumptions.

## Section A — shared build infrastructure

Without this, `--board vck190` fails before it compiles.

1. `drivers/platform/xilinx/toolchain.cmake` — add a `cortexa72` arm to the arch
   if-chain, which currently ends in `FATAL_ERROR "Unsupported Xilinx arch"`.
   `get_arch` returns `sys_cips_pspmc_0_psv_cortexa72_0`, so `MATCHES "cortexa72"`
   is unambiguous against the a9/a53/r5/sys_mb arms. Same settings as the
   `cortexa53` arm: `aarch64-none-elf`, `gnu/aarch64/lin/aarch64-none/bin`, no
   extra CPU flags, `elf64-littleaarch64` / `aarch64` for the firmware-blob
   objcopy targets.
2. `capi/platform/xilinx/toolchain.cmake` — the same arm; the two files carry
   duplicate if-chains and drift otherwise.
3. `board_configs/xilinx/vck190_defconfig` — new, mirroring `zcu102_defconfig`
   rather than `vcu118_defconfig`: this is a PS board, so it keeps
   `CONFIG_GPIO_IRQ_XILINX`.

   This one needed checking rather than assuming. `GPIO_IRQ_XILINX` is
   `depends on GPIO && IRQ`, and the ad9088 variant conf sets **both**, so the
   symbol really does come out `y` and `xilinx_gpio_irq.c` really does get
   compiled — which is why `vcu118_defconfig` omits it (MicroBlaze BSPs have no
   `xgpiops.h`, which that file includes unconditionally). Confirmed safe on
   Versal: `gpiops_v3_14/data/gpiops.yaml` in the Vitis install lists
   `xlnx,versal-gpio-1.0` among its supported compatibles, and this design
   exports `LPD_GPIO` from `sys_cips`, so the BSP will generate the driver.
4. `board_configs/xilinx/CMakePresets.json` — a `vck190` preset inheriting
   `xilinx-base`. This is what makes the combination discoverable: board/variant
   discovery in `no_os_build.py` is driven entirely off the presets plus
   `projects/<p>/boards/<variant>/<board>.conf`.
5. `drivers/platform/xilinx/xilinx_compat.h` — `XPAR_XUARTPSV_{0,1}_INTR` ←
   `..._INTERRUPTS` fallbacks, mirroring the existing `XPAR_XUARTPS_*` block.
   Added during section C, when the Versal branch of `parameters.h` started
   referencing `XPAR_XUARTPSV_0_INTR`. The value is inert here (this project
   installs no UART IRQ handler) but the macro still has to resolve. If the
   Versal BSP turns out to emit neither spelling, the first build will say so
   and `UART_IRQ_ID` needs a local default instead.

## Section B — board-selection seam

6. `projects/ad9088/CMakeLists.txt` emits `BOARD_VCU118` / `BOARD_VCK190` from
   `${BOARD}`, following the idiom `projects/ad9361/CMakeLists.txt` already uses
   for `PLATFORM_MB`. Preferred over the `#ifdef _XPARAMETERS_PS_H_` test already
   sitting in `parameters.h`: that branch has never been exercised and is not a
   reliable Versal signal.
7. Per-board profile. The merge order is `board_configs/<platform>/<board>_defconfig`
   → `projects/<p>/boards/<variant>/<board>.conf` → `projects/<p>/<variant>.conf`
   (`CMakeLists.txt:248-284`), applied by repeated
   `kconf.load_config(..., replace=False)` in `generate_config.py`, so **last
   wins** — the variant conf overrides the board conf, not the other way round.
   A profile left in `basic_example.conf` would therefore override every board.
   Fix by moving ownership down: drop `CONFIG_AD9088_PROFILE` from
   `basic_example.conf`, put the VCU118 value in
   `boards/basic_example/vcu118.conf` (previously an empty marker) and the
   VCK190 value in the new `boards/basic_example/vck190.conf`.
   `dma_example.conf` is untouched and stays VCU118-only.

   Note when reproducing this by hand: `generate_config.py` only merges
   `--defconfig` files when `--update` is also passed (the loop is inside
   `if args.update:`). Without it the script silently emits bare Kconfig
   defaults, which looks exactly like a board conf that failed to apply.

## Section C — platform parameters

8. `src/platform/xilinx/parameters.h` — replace the `_XPARAMETERS_PS_H_` /
   `PLATFORM_MB` split with a `BOARD_VCK190` / `BOARD_VCU118` split. VCU118
   values unchanged. VCK190 adds: UART `XPAR_XUARTPSV_0_*`; clock SPI
   `XPAR_XSPIPS_0_*`; Apollo SPI `XPAR_XSPI_0_*` (index **0**, not 1 — `axi_spi_2`
   is the only PL XSpi here); `SPI_CS_ADF4382 0` / `SPI_CS_HMC7044 1` /
   `SPI_CS_ADF4030 2`; `GPIO_OFFSET 0` with `GPIO_RESET_N 30`; GT-reset pin
   numbers RX done/pll/dp = 32/36/38 and TX = 33/37/39. `AD9088_ADXCVR` is
   defined **only** for the VCU118, and `RX_XCVR_BASEADDR` / `TX_XCVR_BASEADDR`
   move inside that guard — `XPAR_AXI_APOLLO_{RX,TX}_XCVR_BASEADDR` do not exist
   in the VCK190 BSP.
9. `src/platform/xilinx/parameters.c` — stays on `PLATFORM_MB`, which is the
   right test here because these really are PS-vs-PL questions: `UART_PL` vs
   `UART_PS`, `SPI_PL` vs `SPI_PS`, and the matching `XPAR_XUARTLITE_0` /
   `XPAR_XSPI_0` vs `XPAR_XUARTPSV_0` / `XPAR_XSPIPS_0` base addresses. The only
   genuine bug fixed here is the UART `base_addr`, which was
   `XPAR_XUARTLITE_0_BASEADDR` unconditionally (MicroBlaze-only) and now has a
   PS arm.

   The one board fact this file needs — the Apollo's PL XSpi index, #1 on the
   VCU118 but #0 on Versal where the clock chips vacated the PL bus — comes in
   as `APOLLO_SPI_BASEADDR` from `parameters.h`, so no `BOARD_*` macro appears
   in the `.c` at all.

### Where the board/CPU line is drawn, and why

Worth recording, because the first cut of this got it wrong.

`parameters.h`'s original `#ifdef _XPARAMETERS_PS_H_` arm could never fire on
Versal: that macro is the include guard of `xparameters_ps.h`, and
`lib/bsp/standalone_v9_2/src/xilstandalone.cmake:81-88` in the Vitis install
sets `XPAR_PS_INCLUDE` (the thing that includes it) only for `cortexr5`,
`cortexr52` and `cortexa9` — not `cortexa72`. Both arms would be false on a
VCK190 BSP and every macro would go undefined. The same file shows Versal gets
`versal`/`VERSAL` rather than any `PLATFORM_ZYNQ*`, and that `PLATFORM_MB` is
still emitted for MicroBlaze, so `PLATFORM_MB` itself is sound.

So the split is: **`PLATFORM_MB` for PS-vs-PL mechanics** (`parameters.c`), and
**`BOARD_*` only for facts that come from the board and HDL** (`parameters.h`):
chip selects, GPIO pin numbers, the Apollo XSpi index, and whether an
`axi_adxcvr` exists at all. Using `BOARD_*` for the PS/PL mechanics as well was
tidier-looking but less portable, and has been reverted.

The Versal arm is `#if defined(BOARD_VCK190) || defined(BOARD_VPK180)`. Checked
against `versal-vpk180-reva-ad9084.dts`: the VPK180 is identical to the VCK190
in every field the arm sets — `fmc_spi spi0`, CS 0/1/2, Apollo reset on
`axi_gpio` 30, GT resets 32/33/36/37/38/39, no adxcvr. Enabling the VPK180 later
should therefore need only a preset, a board defconfig and a board `.conf`, not
a source change. **It is not enabled here** and has not been built or run.

**Verification done for C**, short of an actual compile: `parameters.h` was run
through `gcc -E -dM` for both boards against a stub `xparameters.h`, confirming
the VCK190 branch yields CS 0/1/2, `GPIO_OFFSET 0` and the six GT-reset pins with
no `AD9088_ADXCVR`, the VCU118 branch yields CS 0/1/4, `GPIO_OFFSET 32`,
`AD9088_ADXCVR` and the two XCVR base addresses, and that the `#else` arm's
`#error` fires when neither board macro is set. A script comparing the two
branches' `#define` sets showed the only asymmetry is that intended pair. astyle
reports all three touched files unchanged and no added line exceeds 80 columns.

## Section D — common data and the example

10. `src/common/common_data.h` — board-conditional lane maps, from the DTS:

    ```
    jrx0 phys  {5, 1, 3, 7, 11, 11, 11, 11, 11, 11, 11, 11}
    jtx0 logic {11, 11, 11, 1, 11, 11, 11, 11, 2, 3, 11, 0}
    jrx1 phys  {1, 7, 10, 3, 5, 8, 9, 11, 11, 11, 11, 11}
    jtx1 logic {11, 5, 1, 0, 11, 2, 11, 3, 4, 7, 11, 6}
    ```

    Lane rate, device clock, SYSREF frequency and the ADF4030 channel numbers are
    identical on both boards and stay shared. The
    `extern struct adxcvr_init {rx,tx}_adxcvr_ip` declarations get guarded.

    **The lane maps must not be folded in with the VPK180** the way the
    `parameters.h` arm was. The two Versal boards share `jrx0`/`jtx0` but differ
    on link B: VPK180 `jrx1` is `<1 7 10 3 11 11 ...>` against VCK190's
    `<1 7 10 3 5 8 9 11 ...>`, and `jtx1` differs to match. So this is a real
    three-way split even though `parameters.h` is a two-way one.
11. `src/common/common_data.c` — `{rx,tx}_adxcvr_ip` compiled only under
    `AD9088_ADXCVR`; otherwise six `no_os_gpio_init_param` instances for the
    GT resets, wired into `rx_jesd204_ip` / `tx_jesd204_ip` through the existing
    `gt_reset_*` fields (`axi_jesd204_rx.h:82-84`).
12. `src/examples/basic/basic_example.c` — bracket the two `adxcvr_init` calls,
    the two `*_jesd_clk.xcvr =` assignments, the two `struct adxcvr *`
    declarations and the two `adxcvr_remove` calls (with their `error_*` labels)
    in `#ifdef AD9088_ADXCVR`. The `goto` after a failed
    `axi_jesd204_rx_init()` also has to switch target, since `error_rx_adxcvr`
    does not exist on Versal; it falls back to `error_tx_dmac`. The GT reset then
    happens inside the FSM instead, where `axi_jesd204_rx_jesd204_clks_enable`
    and `..._link_enable` call `axi_jesd_ext_reset()`.

**Verification done for D.** With a stubbed `xparameters.h` and the driver's
real include list (`drivers/rf-transceiver/CMakeLists.txt:84-90`, b0 only),
`common_data.c`, `basic_example.c` and `parameters.c` all compile under
`gcc -Wall -Wextra` for both `BOARD_VCK190` and `BOARD_VCU118`. Compiling
`common_data.c` to an object and reading the symbol table confirms the guards
do what they claim: VCK190 emits six `*_gt_reset_*_ip` symbols and zero
`*_adxcvr_ip`, VCU118 the reverse. The lane-map `#error` was exercised for all
four cases — VCK190 and VCU118 resolve, VPK180 and "no board macro" both fail
loudly, which is the point: the VPK180's link-B crossbar differs and must not
silently inherit the VCK190's.

astyle reports all three files unchanged, no added line exceeds 80 columns.

### Pre-existing warnings that only the VCK190 build will show

`basic_example.c` prints NCO frequencies with `%lld` against `int64_t`. On the
32-bit MicroBlaze `int64_t` is `long long`, so the VCU118 build is clean; under
the aarch64 LP64 model it is `long`, so the same eight lines become
`-Wformat=` warnings on the VCK190. Confirmed pre-existing, not introduced here:
the unmodified file at HEAD produces the identical eight warnings under the same
flags. Left alone as out of scope — the fix is `PRId64` or a `(long long)` cast
at the eight call sites.

## Verification

Build side:

```bash
source /xilinx/2025.1/Vitis/settings64.sh
python tools/scripts/no_os_build.py list --project ad9088      # vck190 appears
python tools/scripts/no_os_build.py build --project ad9088 \
    --variant basic_example --board vck190 \
    --hardware "$PWD/projects/ad9088/system_top_vck190.xsa"
```

- BSP generation resolves `psv_cortexa72_0` and compiles with `-DSDT`
  (`NOOS_XILINX_SDT ON` is already set in the project CMakeLists).
- ELF at `build/ad9088-basic_example-vck190/build/ad9088.elf`.
- Regression: rebuild `--board vcu118` for both variants and confirm the profile
  still resolves to `204C_M4_L8_NP16_20p0_4x2.bin` after the conf move.
- `astyle` (in-tree, `build/astyle/build/gcc/bin/astyle`) clean on every touched
  file; no new `-Wall -Wextra` warnings.

On hardware (run by the user; this tree never flashes):

- SPI IDs read back for ADF4382 / HMC7044 / ADF4030 over PS SPI0 — proves both
  the CS remap and the PL→PS bus switch.
- Apollo reset over `axi_gpio` 30, then "Project configured".
- No `GT reset-done timeout` from `axi_jesd_ext_reset()`, then
  `axi_jesd204_{rx,tx}_status_read()` reporting the link up. This is the real
  test that dropping adxcvr for the GPIO reset path works.

## Risks and open points

- **`reset-done` GPIO direction.** `axi_jesd204_rx_init` calls
  `no_os_gpio_direction_input()` on the done pin and the design's tri-state
  default is all-input, so this should be fine — but it is the first time the
  Versal GT reset path runs in no-OS, so it is the first suspect if the reset
  times out.
- ~~**`xgpiops.h` in a Versal BSP**~~ — resolved, see A.3: the Vitis `gpiops`
  driver declares `xlnx,versal-gpio-1.0`.
- **MCS / subclass 1.** `ad9088_mcs_ops_bind()` and the ADF4030-as-SYSREF-provider
  wiring are board-agnostic and should carry over, but the VCK190 has not been
  through the subclass-1 bring-up the VCU118 has (see `SUBCLASS1_ENABLEMENT.md`).
  If the link comes up but SYSREF does not lock, that is the next thing to look
  at — out of scope for this pass.
- **HSCI is ignored.** The VCK190 design instantiates `axi_hsci` + `hsci_clkgen`
  and the DTS sets `adi,axi-hsci-connected` / `adi,hsci-auto-linkup-mode-en` on
  the Apollo. no-OS has no HSCI driver and control runs over SPI, so this is left
  alone deliberately.
