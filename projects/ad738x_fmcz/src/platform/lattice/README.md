# Lattice Propel Toolchain Environment #########################################

# Tested with Lattice Propel SDK version: 2025.2 ###############################

# Tested with RISCV-RX core; VLNV: latticesemi.com:ip:riscv_rtos:2.8.0 #########
# Supported RISC-V extentions: M, A ############################################

# Reference HDL project:
https://github.com/analogdevicesinc/hdl/tree/main/projects/ad738x_fmc/lfcpnx

The Lattice RX platform build expects the Propel SDK command-line tools and the
bundled RISC-V GCC toolchain to be on your `PATH`. If they are not exported
globally, prepend the following directories before running `make`:

```bash
export PATH="<your_path>/lscc/propel/<version>/sdk/tools/bin":$PATH
export PATH="<your_path>/lscc/propel/<version>/sdk/riscv-none-embed-gcc/bin":$PATH
export PATH="<your_path>/lscc/propel/<version>/sdk/openocd/bin":$PATH
```

Adjust the base path (`<your_path>/lscc/propel/<version>/`) to match the Propel
release and installation directory present on your machine. After exporting,
confirm that the toolchain is visible:

```bash
which riscv-none-embed-gcc
riscv-none-embed-gcc --version
which srec_cat
```

The `.mem` image produced by `lattice_post_build` uses `srec_cat`; it ships with
the Propel SDK under `sdk/tools/bin`, so make sure that directory is exported on
PATH.

For persistent access, add the exports to your shell profile (for example `~/.bashrc`
when using Cygwin or WSL) before invoking the Lattice platform make targets.

Use the Lattice Radiant Programmer to load the `*.bit` file, then you can use
`make run` to build and download the `*.elf` file to the board using `openocd`.
