ADF5902 no-OS Example Project
==============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `EV-ADF5902SD1Z <https://www.analog.com/EV-ADF5902SD1Z>`_

Overview
--------

The ADF5902 is a 24 GHz ISM band VCO and PGA with 2-channel transmitter
designed for FMCW radar applications. It features a 24 GHz to 24.25 GHz
VCO, a 2-channel 24 GHz power amplifier with 8 dBm output, and an on-chip
temperature sensor. The device supports frequency ramp generation and
calibration for automotive and industrial radar systems.

The EV-ADF5902SD1Z evaluation board enables characterization of the ADF5902
device. It interfaces with an FPGA platform via SPI and GPIO using the
SDP-I-FMC interposer board.

Applications
------------

* Automotive radars
* Industrial radars
* Microwave radar sensors

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken from the
`Project Source Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adf5902_sdz/src>`__.

Demo Example
~~~~~~~~~~~~

This is the default example which initializes the ADF5902 device, performs
calibration using ``adf5902_recalibrate``, computes the locked frequency
with ``adf5902f_compute_frequency``, and reads the on-chip temperature
sensor with ``adf5902_read_temp``.

IIO Example
~~~~~~~~~~~

The IIO example exposes the ADF5902 device through the IIO framework over
UART, enabling host-side configuration and monitoring of the radar frontend
via IIO Oscilloscope or other IIO client applications.

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

* `EV-ADF5902SD1Z <https://www.analog.com/EV-ADF5902SD1Z>`_
* `ZedBoard <https://www.avnet.com/wps/portal/us/products/avnet-boards/avnet-board-families/zedboard/>`_
* `SDP-I-FMC Interposer <https://www.analog.com/EVAL-SDP-I-FMC>`_

Connections
^^^^^^^^^^^

Connect the EV-ADF5902SD1Z to the ZedBoard via the SDP-I-FMC interposer
board. The SPI interface and GPIO signals (CE, TX_DATA) are managed by
the Xilinx PS7 peripherals.

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``demo``, ``iio``.
Available boards: ``zed``.

A Xilinx XSA hardware description file is required. Pass its path via
the ``--hardware`` flag (local dev) or set the ``HARDWARE`` environment
variable. The HDL design used is ``adv7511`` (a generic ZedBoard design
providing PS SPI and GPIO).

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
       --project adf5902_sdz --variant demo --board zed \
       --hardware /path/to/system_top.xsa

   # Build the IIO example for ZedBoard
   python tools/scripts/no_os_build.py build \
       --project adf5902_sdz --variant iio --board zed \
       --hardware /path/to/system_top.xsa

   # Build and flash via JTAG
   python tools/scripts/no_os_build.py build \
       --project adf5902_sdz --variant demo --board zed \
       --hardware /path/to/system_top.xsa \
       --probe openocd --flash
