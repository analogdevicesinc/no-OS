CAPI QVGA Camera Coprocessor Demo
=================================

.. no-os-doxygen::

.. contents:: Table of Contents
   :depth: 3

Supported Hardware
------------------

* MAX78000EVKIT
* The supplied OVM7692 parallel camera module
* The EVKIT 320 x 240 SSD2119 TFT
* Optional Olimex JTAG probe connected to the RISC-V debug header

Overview
--------

This example turns the MAX78000 RISC-V core (CPU1) into a real camera I/O
coprocessor. CPU1 captures RGB565 video from the OVM7692 through PCIF and DMA,
alternates complete frames between two pairs of CNN data-SRAM quadrants,
computes image statistics after capture, and publishes two alternating
eight-row tiles in shared SRAM. The Cortex-M4 (CPU0) manages CPU1 and the CNN
SRAM resource
through the CAPI coprocessor API, drains those tiles to the TFT, and renders
three lines of text over the top of the live QVGA image.

A 320 x 240 RGB565 frame requires 153,600 bytes, so it cannot fit in CPU1's
48 KiB data SRAM. Instead, CPU0 powers and configures the CNN memory through
CAPI and the official MAX78000 image-capture power sequence. A DMA reload
descriptor writes rows 0 through 203 directly to one CNN quadrant and rows 204
through 239 to the next, leaving the final 512 bytes of the first quadrant
unused so a scanline never crosses the physical quadrant gap. While DMA fills
one frame bank, CPU1 publishes the preceding bank and uses TFT-backpressure
waits to analyze the newly completed frame. TFT latency therefore cannot make
the camera skip rows or overwrite the frame being displayed.

Displayed Statistics
--------------------

The top 24 image rows are replaced with an opaque three-line status band::

  RV:RUN WB:INC X:+1 S:8 T:F A:L C:10.1 U:10.1
  P:22400US F:1842 O:0 D:0 RGB:117,120,116
  M:7.8% L:116 E:3910 ROI:043,071 086X052

``WB`` is the active sensor white-balance preset, ``X`` is the AEC exposure
target (-2 through +2), ``S`` is the OV7692 chroma-saturation level, and ``T``
selects the flat, balanced, or punchy sensor tone curve. ``C`` is the CPU1
capture rate and ``U`` is the completed TFT frame rate. ``A`` reports the
automatic-exposure state: reset (``R``), settling (``S``), or locked (``L``).
``P`` measures CPU1 pixel
processing time, ``F`` is the frame number, ``O`` is the current camera DMA
overflow count, and ``D`` counts incomplete frames discarded without stopping
the stream. ``RGB`` gives the frame's mean red, green, and blue values. ``M``,
``L``, and ``E`` report temporal motion, mean luminance, and horizontal-edge
count. ``ROI`` reports the motion extent in 320 x 240 coordinates.

Once per second the UART also reports the percentages of output pixels below
luma 16 (``shadow``) and above luma 239 (``highlight``), plus the 10th, 50th,
and 90th percentile luminance values (``p10``, ``p50``, and ``p90``). These
distribution metrics make tone-curve and exposure changes measurable instead
of relying only on the frame mean.

The OV7692 AEC/AGC loop is deliberately bounded. It runs for two complete
captured frames, then CPU1 snapshots the live exposure-time and gain registers
and clears only the COM8 AEC and AGC enable bits. The sensor's manual mode
retains those values; CPU1 deliberately does not rewrite the timing registers.
DMA
overflow frames do not count toward the settling window. The UART appends
``ae``, ``lines``, raw ``gain``, and ``com8`` values so a gradual exposure
drift is visible without judging the TFT by eye. Changing white balance or the
AEC target re-arms the two-frame sequence and locks again.

The established EVKIT COM8 setup remains unchanged while auto control is
active: ``0xF5`` for manual white balance and ``0xF7`` for automatic white
balance. Locking clears only bits 0 and 2, producing ``0xF0`` or ``0xF2``.

The four blocks at the right of the first line are a fixed red, green, blue,
white TFT color key. If those blocks are correct but the camera image is
yellow, the problem is in sensor white balance; incorrect blocks instead point
to RGB565/TFT ordering. Glyph and color-key rows are pre-mirrored to match the
SSD2119 camera-write helper, which consumes each source row from right to left.

Architecture
------------

::

  OVM7692 -> PCIF -> DMA reload -> CNN frame bank A/B
                                      |          |
                           capture next          publish previous
                                      |          |
                               RISC-V statistics + shared tile[2]
                                                       |
                                               CAPI mailbox event
                                                       |
                                      Cortex-M4 -> stock SPI0 TFT writer

The single-word SEMA mailbox carries commands and notifications; image bytes
and statistics remain in the fixed shared-memory ABI described by
``src/camera_ipc.h``. Tile state is published last. Neither core has a data
cache, so volatile accesses plus compiler ordering are sufficient.

Capture and TFT publication are double-buffered. DMA writes only the current
CNN frame bank while shared tiles are copied from the preceding bank. Analysis
starts only after the current DMA has completed, but individual sampled rows
are processed while CPU1 is otherwise waiting for a shared tile slot. This
keeps statistics associated with complete frames without serializing the
camera and display. CPU1 timer 0 provides an independent IBRO/128 timebase;
``C`` measures the completed pipeline period and ``P`` accumulates only active
analysis time.

CPU0 timer 2 measures the completed TFT frame rate and the one-second button
holds from the independent 7.3728 MHz IBRO clock. The MAX78000 Cortex-M4
advertises ``DWT_CTRL.NOCYCCNT``, so the DWT cycle counter cannot be used for
either function. Using IBRO also avoids the SDK's ambiguous APB-clock
conversion after CPU0 changes its system clock.

The OV7692 runs at native ``CLKRC=0x00``. Direct two-block DMA removes the
per-row CPU copy/rearm deadline that previously required a divide-by-3 or
divide-by-9 sensor clock. Hardware runs have captured every row with zero DMA
overflow or dropped frames at this setting.

Statistics use a uniform 4 x 4 sampling grid (4,800 pixels per frame), which is
also the motion grid. Edge counts are scaled to a full-frame estimate. This
reduces CPU1 analysis time without subsampling or otherwise changing any of the
76,800 pixels sent to the TFT.

The live display intentionally delegates every tile to the unmodified MSDK
``MXC_TFT_ShowImageCameraRGB565`` implementation. The SSD2119's serial timing
table specifies a 15 MHz maximum clock, and RGB565 requires two 9-bit serial
characters per pixel. A spec-compliant full-screen link therefore tops out at
about 10.85 frames/s before command and chip-select overhead. The EVKIT MSDK
configuration requests 25 MHz, but the exact stock writer still measures about
10.1 complete QVGA frames/s. A genuine 15--20 full-frame display target needs a
different display transport, such as a controller supporting at least a
21 MHz sustained serial data clock or a parallel/RGB interface; it cannot be
obtained safely by changing SSD2119 RAM-access modes.

Memory Layout
-------------

CPU1 SRAM2 is deliberately partitioned as follows:

===========================  ========  =====================================
Address range                Size      Purpose
===========================  ========  =====================================
``0x20010000-0x20010fff``     4 KiB     OpenOCD RISC-V work area
``0x20011000-0x20013fff``    12 KiB     Camera IPC, statistics and two tiles
``0x20014000-0x2001bfff``    32 KiB     CPU1 data, BSS, heap and stack
``0x2001c000-0x2001ffff``    16 KiB     RISC-V instruction cache/code region
===========================  ========  =====================================

The custom CPU1 memory map is generated by
``maxim_add_riscv_coprocessor(SRAM2_RESERVE 0x4000)``. The first 4 KiB matches
the work area in the Maxim ``target/max78000_riscv.cfg`` OpenOCD script. The
ARM linker keeps its generic ``.shared`` runtime objects in SRAM0 so an OpenOCD
operation cannot overwrite ``SystemCoreClock``.

Each complete RGB565 frame occupies 153,600 bytes across a pair of CNN data
SRAM quadrants. Two pairs are used so capture and publication can overlap:

===========================  ==========  =====================================
Address range                Used size   Purpose
===========================  ==========  =====================================
``0x50400000-0x5041fdff``    127.5 KiB   Bank A, rows 0 through 203
``0x50800000-0x508059ff``     22.5 KiB   Bank A, rows 204 through 239
``0x50c00000-0x50c1fdff``    127.5 KiB   Bank B, rows 0 through 203
``0x51000000-0x510059ff``     22.5 KiB   Bank B, rows 204 through 239
===========================  ==========  =====================================

CPU1 runs a sentinel read/write self-test in all four quadrants before starting
the camera. A failure is reported as coprocessor fault ``0x80000002``.

CAPI and Button Controls
------------------------

* A short PB1 press requests a frame-boundary camera pause while leaving CPU1
  clocked. This is the safe state for attaching the RISC-V debugger or
  inspecting DMA state. Press PB1 again to resume.
* Hold PB1 for one second to cycle ``AUTO``, ``INC`` (incandescent), ``DAY``
  (daylight), and ``CLD`` (cloudy) white balance. Sensor registers are changed
  only between frames. ``INC`` is the power-on default because it removes the
  strong yellow cast seen under warm indoor lighting.
* Hold PB2 for one second to cycle sensor AEC targets ``-2`` through ``+2``.
  The default is ``+1`` for the EVKIT indoor scene. A target change re-arms
  AEC/AGC for two valid
  frames and then freezes the measured exposure and gain; it does not alter
  TFT output levels.
* Hold PB1 and PB2 together for one second to cycle ``F`` (flat), ``B``
  (balanced), and ``P`` (punchy) OV7692 tone curves. Each profile programs both
  the gamma table and the separate sensor contrast stage. ``F`` is the default:
  it uses a true linear gamma table followed by contrast -2, compressing the
  range without crushing the midtones. ``B`` uses the sensor reset gamma and
  contrast -1; ``P`` uses a vendor gamma curve and contrast 0. OV7692
  saturation starts at its sensor-side maximum level 8.
* A short PB2 press requests the same frame-boundary pause and then changes CPU1 to
  ``CAPI_COPROCESSOR_MODE_PARKED``. Press PB2 again to select ``RUN`` and
  resume capture.

CPU1 exposes the non-inlined ``rv_debug_safe_point()`` symbol specifically for
a debugger breakpoint. Unexpected RISC-V traps are published to CPU0 with
``mcause``, ``mepc`` and ``mtval`` instead of silently spinning.

Building
--------

.. code-block:: bash

  cmake --preset max78000evkit -B build-max78000-camera \
      -DPROJECT_DEFCONFIG=capi_coprocessor_max78000/demo.conf
  cmake --build build-max78000-camera \
      --target capi_coprocessor_max78000

The build produces both the combined image and the symbol-rich CPU1 ELF:

* ``build-max78000-camera/build/capi_coprocessor_max78000.elf``
* ``build-max78000-camera/projects/capi_coprocessor_max78000/coprocessor/riscv.elf``

The RISC-V firmware is already embedded in the ARM ELF; flash only the combined
image.

RISC-V Debugging with Olimex/OpenOCD
------------------------------------

First press PB1 so CPU1 enters ``rv_debug_safe_point()`` at a complete-frame
boundary, outside both DMA capture and shared-tile publication. Then start the
Maxim OpenOCD build with the
interface file matching the exact Olimex probe. An ARM-USB-OCD-H can use the
checked-in configuration directly:

.. code-block:: bash

  openocd -s ${MAXIM_PATH}/Tools/OpenOCD/scripts \
      -f projects/capi_coprocessor_max78000/debug/olimex-arm-usb-ocd-h-riscv.cfg

Then load CPU1 symbols and the checked-in diagnostic commands without
reflashing or resetting CPU1:

.. code-block:: bash

  riscv-none-elf-gdb \
      build-max78000-camera/projects/capi_coprocessor_max78000/coprocessor/riscv.elf \
      -x projects/capi_coprocessor_max78000/debug/riscv_camera.gdb
  (gdb) camera_status
  (gdb) camera_tiles
  (gdb) camera_fault

Halting CPU1 at an arbitrary point can freeze a shared tile in an intermediate
state or delay servicing a completed PCIF/DMA transaction. A timed-out or
bus-error frame increments ``O`` and ``D`` and is discarded; the next frame
continues automatically. The frame-boundary safe point avoids both conditions.
