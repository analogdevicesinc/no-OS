---
name: no-os-project-readme
description: Write or update a no-OS project README.rst following the repo's standard structure and the current CMake/no_os_build.py build conventions. Use when creating documentation for a project under projects/<name>/, updating an existing project README, or aligning a README with the CMake build-guide cross-linking conventions.
---

# no-OS Project README

Generate or update `projects/<name>/README.rst` files for the no-OS repository so
they match the house structure and the current CMake / `no_os_build.py` build flow.

## When to use

- Creating a `README.rst` for a project that lacks one.
- Rewriting an old README to the current CMake build flow.
- Adding per-platform CMake build-guide links to align with the current convention.

## Inputs to gather first

Before writing, collect this. Ask the user only for what you cannot determine from
the repo or the device data sheet:

1. **Project directory** — `projects/<name>/`. Confirm it exists.
2. **Device / eval board(s)** — part number(s) and the ADI product page URL(s).
3. **Overview facts** — resolution, channels, interface (SPI/I2C), supply rails,
   key on-chip features, typical applications. Pull from the data sheet.
4. **Variants** — inspect the project's `Kconfig` and `*.conf` files and
   `src/` layout to list the build variants (the `--variant` values) and what
   each one does.
5. **Supported platforms and boards** — inspect `boards/`, `CMakeLists.txt`, and
   defconfigs to list platforms (Maxim, ADuCM3029, STM32, Xilinx, ...) and the
   `--board` values each supports, plus the toolchain env var each needs
   (`MAXIM_LIBRARIES`, `CCES_HOME`, ...).
6. **Connections** — bus pins, UART console port + baud rate, per board.

Do not invent specs. If a value is unknown, leave a clearly-marked `<...>`
placeholder rather than guessing.

## How to write it

1. Start from the template: `doc/project_readme_template.rst` (at the repo
   root). Copy its structure verbatim, then fill every `<...>` placeholder and
   delete the `..` guidance comment lines.
2. Keep the exact section order and RST heading underline convention:
   - `=` title (level 1), `-` section (level 2), `~` subsection (level 3),
     `^` sub-sub (level 4). Underlines must be **at least** as long as the text.
3. One `~~~` subsection per variant under **No-OS Supported Examples**.
4. One `~~~` subsection per platform under **No-OS Supported Platforms**, each
   with `^^^` sub-subsections: *Used Hardware*, *Connections*, *Build Command*.

## Build-command conventions (must match current repo state)

- All builds go through `python tools/scripts/no_os_build.py build --project ...
  --variant ... --board ...`.
- Set toolchain env vars with **Unix `export`** syntax first, then give the
  **PowerShell** equivalent (`$env:<VAR> = "..."`) immediately after so Windows
  users can copy-paste it too.
- Give two commands per platform: a plain build, and a build-and-flash with
  `--probe openocd --flash`.
- **Add a per-platform CMake build-guide link** in each *Build Command* section:

  ```rst
  For toolchain setup and prerequisites, see the
  `<Platform> CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_<platform>_cmake.html>`__.
  ```

  Guide filenames: `build_maxim_cmake.html`, `build_aducm3029_cmake.html`,
  `build_stm32_cmake.html`, `build_xilinx_cmake.html`. Make sure **every**
  platform section gets its matching link — do not link only some platforms.

- For IIO variants, include the IIO No-OS and IIO-Oscilloscope dokuwiki pointers
  (see the template's commented block).

## After writing

- Verify heading underlines are long enough (short underlines break the RST build).
- Verify each documented `--variant` / `--board` value actually exists in the
  project's Kconfig / boards, and that every platform section has its build-guide
  link.
- Do not run a git commit unless the user asks.
