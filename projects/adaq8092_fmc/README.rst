ADAQ8092_FMC no-OS Example Project
===================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-ADAQ8092-FMCZ <https://www.analog.com/EVAL-ADAQ8092>`_

Overview
--------

The EVAL-ADAQ8092-FMCZ is an evaluation board designed for the ADAQ8092,
a high-speed, 14-bit, 105 MSPS data acquisition system. This board
provides a platform to test and develop solutions using the ADAQ8092 by
offering connections compatible with FPGA Mezzanine Card (FMC) host
boards and interfaces for simplified integration into custom FPGA
configurations. Key features include support for direct power supply
connections, multiple clocking options, and configurable sample rates,
which facilitate streamlined performance evaluation and system
prototyping.

Applications
------------

* Communications
* Cellular base stations
* GPS receiver
* Non-destructive testing
* Portable medical imaging
* Multichannel data acquisition

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-ADAQ8092-FMCZ evaluation board requires a +12V supply,
connected first to a data capture board like the ZedBoard. This board's
power management system incorporates low dropout regulators (LDOs) to
convert the 12V input into 3.3V and 1.8V outputs needed for operation.
Internal supply decoupling capacitors ensure stable power delivery
across the board.

Board Connectors
~~~~~~~~~~~~~~~~

====================== ==============================
Connector              Function
====================== ==============================
SMA Connectors         RF Connections
FMC Connector          Interface with other hardware
Male-Male SMA Adapter  Signal Transfer
J5 and J6              Used as inputs or outputs
====================== ==============================

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken from the
`Project Data Source Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adaq8092_fmc/src>`__.

Demo Example
~~~~~~~~~~~~

The demo example code initializes and configures the ADAQ8092 ADC,
sets up SPI and GPIO communication, performs a power-up sequence, and
adjusts settings such as clock polarity and LVDS output current. The
example showcases read and write operations on the device registers via
SPI, facilitating control over various modes and configurations of the
ADC. This example serves as a foundational guide for integrating the
ADAQ8092 into user applications.

IIO Example
~~~~~~~~~~~

The IIO example handles initialization, configuration, and data
acquisition of the ADAQ8092 ADC via an AXI interface. It begins by
initializing the SPI and GPIO interfaces for communication with the
ADAQ8092, configuring the ADC with parameters like power mode, clock,
and test settings. The ADC core and DMAC are then set up for data
capture, and a buffer is allocated for storing the sampled data. If IIO
support is enabled, the code initializes an IIO application with a UART
interface, enabling continuous data streaming through the IIO framework
for remote access and control.

If you are not familiar with ADI IIO Application, please take a look at:
:dokuwiki:`IIO No-OS </resources/tools-software/no-os-software/iio>`

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
:dokuwiki:`IIO Oscilloscope </resources/tools-software/linux-software/iio_oscilloscope>`

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-ADAQ8092-FMCZ <https://www.analog.com/EVAL-ADAQ8092>`_
* `ZedBoard <https://www.avnet.com/wps/portal/us/products/avnet-boards/avnet-board-families/zedboard/>`_

Connections
^^^^^^^^^^^

Connect the EVAL-ADAQ8092-FMCZ to the ZedBoard via the FMC connector.
The AXI ADC core and AXI DMAC IP blocks in the HDL design handle data
capture from the ADAQ8092.

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``demo``, ``iio``.
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
       --project adaq8092_fmc --variant demo --board zed \
       --hardware /path/to/system_top.xsa

   # Build the IIO example for ZedBoard
   python tools/scripts/no_os_build.py build \
       --project adaq8092_fmc --variant iio --board zed \
       --hardware /path/to/system_top.xsa

   # Build and flash via JTAG
   python tools/scripts/no_os_build.py build \
       --project adaq8092_fmc --variant iio --board zed \
       --hardware /path/to/system_top.xsa \
       --probe openocd --flash
