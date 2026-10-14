# Building the AD9088 basic example for the VCK190

How to build `projects/ad9088` (`basic_example`) for the VCK190 (Versal AI
Core, `psv_cortexa72_0`) with the CMake + Kconfig build. For the reasoning
behind the board support (the differences from the VCU118, lane maps, GT reset
path), see `VCK190_ENABLEMENT.md`.

Only `basic_example` is supported on the VCK190. There is no
`boards/dma_example/vck190.conf`, so `dma_example` will not configure for this
board.

## Prerequisites

- **Vitis 2025.1** at `/xilinx/2025.1/Vitis`. The BSP is generated through the
  System Device Tree flow; the project sets `NOOS_XILINX_SDT ON`, so `-DSDT`
  reaches the app automatically when the BSP is SDT.
- **CMake** (presets version 7 or later) and **Ninja**. Every Xilinx preset
  uses the Ninja generator. Sourcing Vitis puts its own `ninja` on `PATH`, so
  you may not notice if the system one is missing.
- **Python 3 with `venv`**. On the first configure CMake creates `.no_os_venv/`
  at the repo root and installs `tools/scripts/requirements.txt` into it
  (`kconfiglib` and others).
- **The VCK190 XSA**, `projects/ad9088/system_top_vck190.xsa`. The root `*.xsa`
  rule in `.gitignore` excludes it, so it is a local file, not a tracked one.
  It must be built from the `ad9084_ebz` HDL project for the VCK190: the board
  config assumes one 8-lane 204C `axi_jesd204_rx`/`_tx` pair carrying two M4/L4
  links, no `axi_adxcvr`, clock-chip SPI on PS SPI0 and the Apollo SPI on
  `axi_spi_2`.

## What the configuration resolves to

The Kconfig fragments are merged in this order, and the **last one wins**:

1. `board_configs/xilinx/vck190_defconfig`: Xilinx platform drivers (UART,
   SPI, I2C, GPIO, GPIO IRQ, IRQ, timer).
2. `projects/ad9088/boards/basic_example/vck190.conf`: sets
   `CONFIG_AD9088_PROFILE="204C_M4_L4_NP16_20p0_4x4.bin"`.
3. `projects/ad9088/basic_example.conf`: generic subsystems, the AD9088,
   ADF4030, ADF4382 and HMC7044 drivers, the AXI cores, and
   `CONFIG_AD9088_BASIC_EXAMPLE=y`.

The profile is set in the board conf, not the variant conf, because the
VCU118 uses a different one (`204C_M4_L8_NP16_20p0_4x2.bin`). The variant conf
is merged last, so a profile set there would override the board conf for every
board.

`projects/ad9088/CMakeLists.txt` turns `BOARD=vck190` into `-DBOARD_VCK190`,
which selects the Versal branch of `src/platform/xilinx/parameters.h` and the
VCK190 lane maps in `src/common/common_data.h`.

## Build

Every command runs from the repo root, in one shell where Vitis has been
sourced first:

```bash
source /xilinx/2025.1/Vitis/settings64.sh
```

### Option 1: the `no_os_build.py` helper

```bash
python tools/scripts/no_os_build.py list --project ad9088

python tools/scripts/no_os_build.py build \
    --project ad9088 --variant basic_example --board vck190 \
    --hardware "$PWD/projects/ad9088/system_top_vck190.xsa"
```

`list` should show `vck190` under `basic_example`. The helper derives the
build directory, `build/ad9088-basic_example-vck190/`.

### Option 2: raw CMake with the preset

```bash
cmake -B build/ad9088-basic_example-vck190 --preset vck190 \
      -DPROJECT_DEFCONFIG=ad9088/basic_example.conf \
      -DHARDWARE="$PWD/projects/ad9088/system_top_vck190.xsa"

cmake --build build/ad9088-basic_example-vck190 --target ad9088
```

### Option 3: raw CMake without presets

Use this when presets will not parse (see Troubleshooting). It passes the
`vck190` preset's cache variables directly:

```bash
cmake -B build/ad9088-basic_example-vck190 -G Ninja \
      -DBOARD=vck190 -DPLATFORM=xilinx -DUSE_VENDOR_TOOLCHAIN=ON \
      -DBOARD_CONFIG_FILE=vck190_defconfig \
      -DPROJECT_DEFCONFIG=ad9088/basic_example.conf \
      -DHARDWARE="$PWD/projects/ad9088/system_top_vck190.xsa"

cmake --build build/ad9088-basic_example-vck190 --target ad9088
```

`HARDWARE` must be an **absolute** path. The toolchain step and the flash step
read it from different working directories, so a relative path breaks one of
them.

## Output

```
build/ad9088-basic_example-vck190/build/ad9088.elf
  ELF 64-bit LSB executable, ARM aarch64, statically linked (~4.7 MB)
```

On a good configure:

- BSP generation resolves the CPU as `sys_cips_pspmc_0_psv_cortexa72_0`, and
  the `cortexa72` toolchain branch picks `aarch64-none-elf`.
- `compile_commands.json` shows the app built with
  `-DBOARD_VCK190 -DSDT -DXILINX_PLATFORM`.
- The firmware blobs and `usecase.bin` (the 4x4 profile) are linked into the
  ELF. The symbols are `_binary_drivers_rf_transceiver_ad9088_firmware_*`.

The library side prints about 60 warnings, from the vendor Apollo sources,
`hmc7044.c`, `no_os_circular_buffer.c` and `no_os_print_log.h`. They are
expected. Project sources are not compiled with `-Wall`, because `-Wall` is
`PRIVATE` to the `no-os` library target, so a clean project build does not
prove the project code is warning-free.

## Other targets

```bash
# Edit the merged config interactively (e.g. to try another profile)
cmake --build build/ad9088-basic_example-vck190 --target menuconfig

# Program the board: PDI via vivado_lab, then the ELF via xsdb
cmake --build build/ad9088-basic_example-vck190 --target flash
```

On Versal, `flash` programs the PDI (extracted from the XSA at configure time)
with `vivado_lab`, then initialises the A72 and downloads the ELF with xsdb.
For a board attached to a remote `hw_server`, set `XSCT_REMOTE_HOST` and
`XSCT_REMOTE_PORT` before running it. Both the PDI and the ELF steps use that
endpoint. Without them, both default to `localhost:3121`.

Console: PS UART0 (`XUartPsv`), 115200 8N1.

`CONFIG_AD9088_PROFILE` takes a bare file name, resolved against
`drivers/rf-transceiver/ad9088/firmware/`, or an absolute path. Whichever is
used, it must match the HDL. The VCK190 design is two links of M4/L4, so
`204C_M4_L4_NP16_20p0_4x4.bin` is the only in-tree profile that fits.

## Troubleshooting

**Every `--preset` fails, and `no_os_build.py build` fails too.** CMake stops
parsing all presets if any included preset file is missing. The local,
untracked `CMakeUserPresets.json` includes
`projects/ad9088/CMakeAgilexPreset.json`, which includes
`board_configs/altera/CMakePresets.json`. That file does not exist in this
tree, so presets are broken for every project. Either move
`CMakeUserPresets.json` aside, or use Option 3.

**`no_os_build.py` crashes with `FileNotFoundError` on `build.log`.** This
hides a configure failure. On error, the script appends to
`<build_dir>/build.log`, but a failed configure never created that directory,
and cmake's real stderr is captured and lost. Re-run the raw `cmake` line the
script echoed to see the actual error.

**`ModuleNotFoundError: kconfiglib`.** `.no_os_venv` is half-built, or the
system `python3` changed version under it. CMake only creates the venv when
`.no_os_venv/bin/python3` is missing, so it will not repair a broken one.
Recreate it with a versioned interpreter:

```bash
rm -rf .no_os_venv
/usr/bin/python3.13 -m venv .no_os_venv
.no_os_venv/bin/python3 -m pip install -r tools/scripts/requirements.txt
```

**Peripheral init fails, or only the first SPI/GPIO instance works.** `SDT` is
not reaching the app. Under SDT, `X*_LookupConfig()` takes a base address, and
a zero argument silently matches the first instance. Check
`compile_commands.json` for `-DSDT`.

**Stale configuration after editing a `.conf`.** Delete
`build/ad9088-basic_example-vck190/` and configure again. Kconfig fragments are
merged at configure time.
