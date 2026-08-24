**********************
CMake Build Cheatsheet
**********************

A compact reference for the no-OS CMake build flow: discover targets, configure,
build, flash, and debug. Commands are run from the repository root
(``cd path/to/no-OS``).

1. Discover projects & targets
==============================

List every project / variant / board combination:

.. code-block::

    tools/scripts/no_os_build.py list

.. code-block::

    <p>           <v>          <b>
    PROJECT       VARIANT      BOARD             PLATFORM
    iio_demo      iio          zed               xilinx
    ad463x_fmcz   basic        sdp-ck1z          stm32
    ad463x_fmcz   iio          sdp-ck1z          stm32
    ad4692_ardz   iio          ad-apard32690-sl  maxim
    ad7124-8pmdz  iio_example  eval-adicup3029   aducm3029
    adxrs290-pmdz iio          rpi-pico          pico

Filter the list by any combination of project, board, and variant:

.. code-block::

    tools/scripts/no_os_build.py list --project ad74413r
    tools/scripts/no_os_build.py list --board sdp-ck1z
    tools/scripts/no_os_build.py list --variant iio
    tools/scripts/no_os_build.py list --project ad74413r --board sdp-ck1z
    tools/scripts/no_os_build.py list --project ad74413r --variant iio
    tools/scripts/no_os_build.py list --board sdp-ck1z --variant iio
    tools/scripts/no_os_build.py list --project ad74413r --variant iio --board sdp-ck1z

2. Configure & build
====================

**CMake directly (preset-based).** ``-B <dir>`` is the build directory CMake
creates and configures.

.. code-block::

    # configure
    cmake -B path/to/build-<p> --preset <b> -DPROJECT_DEFCONFIG=<p>/<v>.conf
    # build
    cmake --build path/to/build-<p> --target <p> -j$(nproc)
    
    # example
    cmake -B build-ad74413r --preset nucleo-f756zg -DPROJECT_DEFCONFIG=ad74413r/iio.conf
    cmake --build build-ad74413r --target ad74413r -j$(nproc)

**no_os_build.py (one-liner).** Configures and builds in one step:

.. code-block::

    tools/scripts/no_os_build.py build --project <p> --variant <v> --board <b>

**Build output paths.** Everything lands under the ``build-<p>/`` directory you
passed to ``-B``. The firmware images are in the ``build/`` subdirectory; the
compiled objects are under ``CMakeFiles/no-os.dir/``:

=============================================  ===================================================
Path (under ``build-<p>/``)                    Contents
=============================================  ===================================================
``build/<p>.elf``                              linked ELF (used by GDB / debug)
``build/<p>.hex``                              Intel HEX image (flashing)
``build/<p>.bin``                              raw binary image (flashing)
``build/lib/*.a``                              static libraries (e.g. mbedtls)
``build/no_os_config.h``                       generated C ``#define`` macros
``build/config``                               Kconfig ``.config`` state
``build/config.cmake``                         Kconfig values as CMake ``set()`` calls
``CMakeFiles/no-os.dir/**/*.c.o``              compiled object files (mirror the source tree)
``compile_commands.json``                      compile database (clangd / IntelliSense)
``CMakeCache.txt``                             cached CMake / Kconfig variables
``build.log``                                  build log
=============================================  ===================================================

3. Flash
========

Via CMake targets (reuse the configured build directory):

.. code-block::

    cmake --build path/to/build-<p> --target flash
    cmake --build path/to/build-<p> --target debug          # GDB server + session
    cmake --build path/to/build-<p> --target erase
    cmake --build path/to/build-<p> --target debug_server   # OpenOCD GDB server on :50000

Via no_os_build.py:

.. code-block::

    tools/scripts/no_os_build.py build --project <p> --variant <v> --board <b> --probe openocd --flash
    # J-Link instead:  --probe jlink

Probe selection with CMake (default is openocd):

.. code-block::

    cmake -B path/to/build-<p> --preset <b> -DPROBE=openocd
    cmake -B path/to/build-<p> --preset <b> -DPROBE=jlink

4. IDE & debug
==============

Configure generates the IDE files at the **repository root** (not in the build
directory), targeting whichever build you configured last. Re-configuring a
different project/board regenerates them to point at that build.

**VS Code** — open the generated multi-root workspace from the repo root:

.. code-block::

    code no-os.code-workspace

It generates ``no-os.code-workspace`` plus ``.vscode/{launch,settings,
c_cpp_properties}.json`` at the repo root. The launch configs use the
cortex-debug extension with OpenOCD on port 50000 (press ``F5`` to run).

**Manual GDB + OpenOCD:**

.. code-block::

    # terminal 1 — GDB server
    cmake --build path/to/build-<p> --target debug_server
    # terminal 2 — GDB client
    arm-none-eabi-gdb build-<p>/build/<p>.elf
    (gdb) target remote localhost:50000
    (gdb) load
    (gdb) monitor reset halt
    (gdb) tb main
    (gdb) c

**Open straight from the build script:**

.. code-block::

    tools/scripts/no_os_build.py build --project <p> --variant <v> --board <b> --open

5. Configuration (Kconfig)
==========================

Interactive text menu:

.. code-block::

    cmake --build path/to/build-<p> --target menuconfig

Config files are loaded in this order (later overrides earlier):

.. code-block::

    1. board_configs/<platform>/<b>_defconfig
    2. projects/<p>/<v>.conf
    3. projects/<p>/boards/<v>/<b>.conf

Any config change regenerates ``build/config`` (``.config`` state),
``build/config.cmake`` (CMake ``set()`` calls), and ``build/no_os_config.h``
(C ``#define`` macros).

6. Cleanup 
==========

CMake gives you standalone cleanup commands. ``no_os_build.py`` has no
delete-only mode — its ``--clean`` and ``--fresh`` are *build* modifiers that
wipe first and then re-configure and build in the same run.

Standalone cleanup (CMake):

.. code-block::

    # clean: remove build artifacts (.o objects, .elf/.hex/.bin, .a libraries);
    #        keeps the CMake cache + Kconfig state, so no re-configure next build
    cmake --build path/to/build-<p> --target clean

    # re-configure: drop only CMakeCache.txt + CMakeFiles/, keep built artifacts,
    #               force a fresh configure without a full wipe
    cmake -B path/to/build-<p> --preset <b> --fresh -DPROJECT_DEFCONFIG=<p>/<v>.conf

    # reset: remove the whole build dir (artifacts, cache, generated config,
    #        IDE projects, compile database, logs)
    rm -rf path/to/build-<p>

Clean rebuild in one step (``no_os_build.py``) — each of these deletes, then
re-configures **and builds**:

.. code-block::

    # --clean deletes the build dir first, then builds (equivalent to rm -rf + build)
    tools/scripts/no_os_build.py build --project <p> --variant <v> --board <b> --clean

    # --fresh drops CMakeCache.txt + CMakeFiles/ first, then builds
    tools/scripts/no_os_build.py build --project <p> --variant <v> --board <b> --fresh


