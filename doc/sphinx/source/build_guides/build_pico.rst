*************************************
Raspberry Pi Pico Build Prerequisites
*************************************

Prior to building a no-OS project for Raspberry Pi Pico, the bundled Pico SDK
and the debugging tools have to be set up. The Pico SDK is vendored as a git
submodule, so cloning it manually is no longer required.

.. note::
   Pico platform builds are supported on Linux only. The build targets the
   Raspberry Pi Pico (RP2040).

Prerequisites
=============

- Install the ``arm-none-eabi`` GCC toolchain (``arm-none-eabi-gcc``).
- Initialize the Pico SDK submodule from the root of the no-OS repository:

    .. code-block:: bash

        $ git submodule update --init libraries/pico-sdk

  .. note::
     Use ``--init`` (not ``--recursive``): an RP2040 no-OS build does not need
     the Pico SDK's own submodules (tinyusb, cyw43, lwip, ...).

  .. note::
     The build always uses this vendored copy: ``PICO_SDK_PATH`` is pinned to
     ``libraries/pico-sdk`` internally, so any externally exported value is
     ignored. This avoids a stale ``PICO_SDK_PATH`` in the environment breaking
     the build.

- (Optional) Install `picotool <https://github.com/raspberrypi/picotool>`_ to
  generate the ``.uf2`` image used for USB Mass Storage flashing. When picotool
  is unavailable, the ``.elf`` and ``.hex`` are still produced.

  picotool is a separate tool from the Pico SDK and is not vendored, so it has
  to be built from source. Install its build dependencies (``cmake``, a host C/C++
  compiler and ``libusb-1.0`` development headers), then clone and build it
  against the Pico SDK. Use the same version as the vendored SDK (2.2.0):

    .. code-block:: bash

        # Build dependencies (Debian/Ubuntu)
        $ sudo apt install cmake build-essential pkg-config libusb-1.0-0-dev

        # Clone and build picotool against the Pico SDK
        $ git clone --branch 2.2.0 https://github.com/raspberrypi/picotool.git
        $ cd picotool
        $ mkdir build && cd build
        $ PICO_SDK_PATH=/path/to/no-OS/libraries/pico-sdk cmake ..
        $ make -j$(nproc)

  This produces the ``picotool`` binary in the ``build`` directory. Make it
  available to the no-OS build either by adding that directory to ``PATH`` (it is
  looked up via ``command -v picotool``) or by exporting its full path:

    .. code-block:: bash

        $ export PICOTOOL=/path/to/picotool/build/picotool

  .. note::
     The picotool binary must be built for the host architecture. If
     ``PICOTOOL`` points at a missing or non-runnable binary (e.g. a
     wrong-architecture build), it is treated as unset and the ``.uf2`` step is
     skipped without failing the build (the ``.elf`` and ``.hex`` are still
     produced).

Debug/flash probe
-----------------

Flashing and debugging support two probes, selected with the ``PROBE`` make
variable:

- ``PROBE=cmsis-dap`` (default) - a second Raspberry Pi Pico running the
  ``debugprobe``/``picoprobe`` firmware (a CMSIS-DAP probe), driven by OpenOCD.
- ``PROBE=jlink`` - a SEGGER J-Link probe, driven by the native SEGGER tools.

Install only the tools for the probe you intend to use.

CMSIS-DAP / picoprobe probe (default)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Install OpenOCD (RP2040 support is included in 0.12.0 and later):

    .. code-block:: bash

        $ sudo apt install openocd

- Flash a spare Raspberry Pi Pico with the ``debugprobe`` firmware to turn it
  into a CMSIS-DAP probe: download ``debugprobe_on_pico.uf2`` from the
  `debugprobe releases <https://github.com/raspberrypi/debugprobe/releases>`_,
  hold the BOOTSEL button while connecting the probe Pico over USB, and copy the
  ``.uf2`` onto the mounted USB Mass Storage device.

- Wire the probe Pico to the target board's SWD pads:

    ===============  ===========================
    Probe Pico       Target Pico
    ===============  ===========================
    GP2              SWCLK (left SWD pad)
    GP3              SWDIO (right SWD pad)
    GND              GND (middle SWD pad)
    VSYS / 3V3       VSYS / 3V3 (to power target)
    ===============  ===========================

  OpenOCD's cmsis-dap driver handles the RP2040 SWD multidrop correctly, so this
  probe works through OpenOCD (unlike the J-Link driver). The OpenOCD settings
  can be overridden with these variables (defaults shown)::

      OPENOCD               = openocd
      OPENOCD_INTERFACE     = interface/cmsis-dap.cfg
      OPENOCD_TARGET        = target/rp2040.cfg
      OPENOCD_ADAPTER_SPEED = 5000          # SWD speed in kHz
      OPENOCD_SCRIPTS       =               # optional -s search dir

J-Link probe (``PROBE=jlink``)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Install the `J-Link software <https://www.segger.com/downloads/jlink/#J-LinkSoftwareAndDocumentationPack>`_.
  The native SEGGER J-Link tools (``JLinkExe`` and ``JLinkGDBServer``) are used
  rather than OpenOCD: OpenOCD's J-Link driver fails the RP2040 SWD multidrop
  selection (``Failed to connect multidrop rp2040.dap0``), while the native
  tools connect reliably.

- Set the ``JLINK_SERVER_PATH`` environment variable to the JLinkGDBServerCLExe
  path. On a default Linux install the J-Link tools live under
  ``/opt/SEGGER/JLink/`` (a versioned directory such as
  ``/opt/SEGGER/JLink/JLink_Linux_V950_x86_64/`` may be used instead):

    .. code-block:: bash

        $ export JLINK_SERVER_PATH=/opt/SEGGER/JLink/JLinkGDBServerCLExe

  .. note::
     The ``make run PROBE=jlink`` (flashing) target uses ``JLinkExe``, which is
     located automatically in the same directory as ``JLINK_SERVER_PATH`` (with
     a fallback to ``PATH``). Exporting ``JLINK_SERVER_PATH`` is therefore enough
     for both flashing and debugging. To override the probe settings, the
     following variables are available (with their defaults)::

         JLINK_EXE     = $(dir of JLINK_SERVER_PATH)/JLinkExe
         JLINK_DEVICE  = RP2040_M0_0
         JLINK_SPEED   = 4000        # SWD speed in kHz

- Wire the J-Link's SWD pins (standard 20-pin J-Link connector) to the target
  Pico's SWD pads:

    ==================  ==============  ===========================
    J-Link 20-pin       Signal          Target Pico
    ==================  ==============  ===========================
    Pin 1 (VTref)       VTref           VSYS / 3V3 (sense only)
    Pin 7               SWDIO           SWDIO (right SWD pad)
    Pin 9               SWCLK           SWCLK (left SWD pad)
    Pin 4 (or 6/8/...)  GND             GND (middle SWD pad)
    Pin 15 (optional)   nRESET          RUN (pin 30)
    ==================  ==============  ===========================

  .. note::
     ``VTref`` (pin 1) is a voltage-sense input: the J-Link uses it to detect the
     target's I/O voltage and does **not** power the target through it. Power the
     Pico separately over its USB connector (or via ``VSYS``). The ``nRESET``
     connection is optional; the J-Link can also reset the RP2040 over SWD.

Common to both probes
~~~~~~~~~~~~~~~~~~~~~~~

- For debugging (the ``make debug`` target), the ``gdb-multiarch`` GDB client
  must be installed and available on ``PATH``; ``make run`` (flashing) does not
  need it. The Pico SDK does not bundle a debugger, so install it separately,
  for example:

    .. code-block:: bash

        $ sudo apt install gdb-multiarch

- (Optional) For visual debugging and building, install Visual Studio Code, and
  the Cortex-Debug extension.

Building a project
==================

To build a project, type:

.. code-block:: bash

    $ make PLATFORM=pico

The build process creates a **build** directory in the project folder:

.. code-block::

    build
    ├── app                  # mirrored no-OS and SDK sources
    ├── generated            # auto-generated SDK headers and linker fragment
    ├── objs                 # compiled object files
    ├── project_name.elf
    ├── project_name.hex
    └── project_name.uf2     # only when picotool is available

The ``.uf2`` image is only generated when picotool is available; otherwise the
``.elf`` and ``.hex`` are still produced.

Running/Debugging
=================

Once the **.elf** file has been generated, make sure the board is powered on and
the probe is connected to the RP2040 SWD pads (SWDIO, SWCLK, GND), then upload
the program with:

.. code-block:: bash

    $ make run

By default this uses the CMSIS-DAP/picoprobe probe (a second Pico over OpenOCD).
To flash with a SEGGER J-Link instead, select the probe:

.. code-block:: bash

    $ make run PROBE=jlink

To start an interactive debug session (launches a GDB server on port 3333 and
connects the GDB client to it):

.. code-block:: bash

    $ make debug                  # CMSIS-DAP/picoprobe (default)
    $ make debug PROBE=jlink      # SEGGER J-Link

The ``cmsis-dap`` probe launches OpenOCD and the ``jlink`` probe launches
``JLinkGDBServer``; both listen on port 3333.

Alternatively, if picotool is available, the generated **.uf2** image can be
flashed by holding the BOOTSEL button while connecting the board over USB and
copying the **.uf2** file onto the mounted USB Mass Storage device.

Debugging with Visual Studio Code
=================================

The build system can generate a Visual Studio Code debug configuration (via the
Cortex-Debug extension) for either probe. From the project folder, run:

.. code-block:: bash

    $ make PLATFORM=pico sdkopen                # CMSIS-DAP/picoprobe (default)
    $ make PLATFORM=pico sdkopen PROBE=jlink    # SEGGER J-Link

This generates the ``.vscode/launch.json`` (a *Pico Debug* configuration) and
``.vscode/tasks.json`` (*Pico Build* and *Pico Run* tasks) in the project folder
and then opens the project in Visual Studio Code. The generated configuration
uses:

- ``interface: swd`` and ``device: RP2040_M0_0``;
- for the CMSIS-DAP probe, ``servertype: openocd`` with the ``configFiles``
  ``interface/cmsis-dap.cfg`` and ``target/rp2040.cfg``;
- for the J-Link probe, ``servertype: jlink`` with ``serverpath`` set from
  ``JLINK_SERVER_PATH`` (so make sure it is exported, see the Prerequisites);
- ``gdbPath`` set to ``gdb-multiarch``;
- the RP2040 ``.svd`` file from the Pico SDK, enabling the peripheral register
  view while debugging.

.. note::
   The ``sdkopen`` target requires the ``code`` command to be available on
   ``PATH`` (install Visual Studio Code and the Cortex-Debug extension first).

Once Visual Studio Code is open, select the **Pico Debug** configuration in the
Run and Debug view and start debugging. The configuration runs the **Pico
Build** task first, flashes the board over SWD and breaks at ``main``.

.. note::
   The generated ``.vscode`` files contain absolute, machine-specific paths and
   are regenerated by ``sdkopen``; they are not meant to be committed.
