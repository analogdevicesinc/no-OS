ADAQ7980_SDZ no-OS Example Project
===================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-ADAQ7980SDZ <https://www.analog.com/EVAL-ADAQ7980SDZ>`_

Overview
--------

The `EVAL-ADAQ7980SDZ <https://www.analog.com/EVAL-ADAQ7980SDZ>`_
evaluation board showcases the ADAQ7980, a low-power, 16-bit, 1 MSPS
micro-module data acquisition system that integrates key signal processing
and conditioning blocks into a compact system-in-package (SiP) design.
The board provides versatile analog signal conditioning circuitry,
on-board reference and reference buffers, and ADC drivers, facilitating
evaluation of the ADAQ7980 across a variety of data acquisition
applications.

Applications
------------

* Automated test equipment (ATE)
* Battery powered instrumentation
* Communications
* Data acquisition
* Process control
* Medical instruments

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-ADAQ7980SDZ evaluation board requires a +9 V DC power supply
(included in the kit). On-board voltage regulators (ADP7118, ADP2370,
ADP7182) convert this input voltage into the necessary supply rails of
+7.5 V, +5.0 V, and +3.3 V. Integrated reference circuitry (ADR4550
and ADR3433) provides stable 5 V and 3.3 V reference outputs. Power
distribution on the board allows users to choose between internal and
external voltage references using jumper links. Optionally, an external
bench power supply (+7.5 V / -2.5 V) can be connected via the P6
terminal block.

Board Connectors
~~~~~~~~~~~~~~~~

* **P1, P2, P3, P4** -- SMA connectors for low-noise analog signal source
* **P6** -- Terminal block for alternative bench top power supply inputs
* **P8** -- 120-pin connector interfacing with the controller board
  (EVAL-SDP-CB1Z or FPGA carrier)
* **J1, J2, J3** -- Links for ADC driver configuration and input circuit
  filter settings
* **J4, J5** -- Links for reference buffer and ADC driver power-down
  control (dynamic power scaling)
* **J6, J7** -- Control links for LDO power-down and reference source
  selection
* **J9, J10** -- Settings for on-board and external reference voltages
* **J13, J14** -- Supply source settings for V_SDP and VDRIVE

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken from the
`Project Source Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adaq7980_sdz/src>`__.

Demo Example
~~~~~~~~~~~~

The demo example initializes the ADAQ7980 device using the SPI Engine
IP core and configures the AXI PWM generator to produce the CNV
(conversion start) trigger signal at the desired sampling rate. The SPI
Engine offload mode is then used to perform continuous high-speed ADC
conversions synchronized to the PWM trigger, with the conversion results
transferred via DMA. The raw 16-bit ADC codes are printed to the serial
console, allowing users to verify correct operation and evaluate the
performance of the ADAQ7980.

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-ADAQ7980SDZ <https://www.analog.com/EVAL-ADAQ7980SDZ>`_
* `ZedBoard <https://www.avnet.com/wps/portal/us/products/avnet-boards/avnet-board-families/zedboard/>`_

Connections
^^^^^^^^^^^

Connect the EVAL-ADAQ7980SDZ digital interface signals to the ZedBoard
FPGA carrier board. The project uses an FPGA HDL design that includes
the SPI Engine IP core and AXI PWM generator for high-speed data
acquisition. All digital logic levels are 3.3 V.

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``demo``.
Available boards: ``zed``.

A Xilinx XSA hardware description file is required. Pass its path via
the ``--hardware`` flag (local dev) or set the ``HARDWARE`` environment
variable.

For toolchain setup and prerequisites, see the
`Xilinx CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_xilinx_cmake.html>`__.

.. code-block:: bash

   # Source the Vitis environment (adjust path to your installation)
   source /path/to/Vitis/2025.1/settings64.sh
   # PowerShell (Windows) equivalent:
   #   & "C:\path\to\Vitis\2025.1\settings64.bat"

   cd no-OS

   # Build the demo example for ZedBoard (supply your own .xsa)
   python tools/scripts/no_os_build.py build \
       --project adaq7980_sdz --variant demo --board zed \
       --hardware /path/to/system_top.xsa

   # Build and flash via JTAG
   python tools/scripts/no_os_build.py build \
       --project adaq7980_sdz --variant demo --board zed \
       --hardware /path/to/system_top.xsa \
       --probe openocd --flash
