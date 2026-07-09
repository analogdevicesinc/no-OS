AD9083 no-OS Example Project
=============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-AD9083 <https://www.analog.com/EVAL-AD9083>`_

Overview
--------

The AD9083 is a 16-channel, 125 MHz bandwidth, continuous time sigma-delta
(CTSD) ADC with an integrated programmable single-pole antialiasing filter
and termination resistor. Each ADC core features a first-order CTSD modulator
with background nonlinearity correction and self-cancelling dither. The analog
input and clock signals are differential inputs with a variety of
user-selectable input ranges.

Each ADC has a signal processing tile containing a cascaded integrator comb
(CIC) filter and a quadrature digital downconverter (DDC) with multiple FIR
decimation filters for out-of-band noise rejection and sample rate reduction.
The device features high-speed serialized outputs via a Subclass 1 JESD204B
interface, supporting various lane configurations (up to four).

The EVAL-AD9083 evaluation board integrates the AD9083 ADC alongside an
AD9528 JESD204B clock generator with a 100 MHz VCXO reference and on-board
power management using the LTM8074 Silent Switcher regulator. Synchronization
across multiple units is possible using SYSREF, TRIG, and SYNCINB pins. The
AD9083 is configurable via a 1.8 V capable 3-wire SPI interface.

Applications
------------

* Millimeter wave imaging
* Electronic beam forming and phased arrays
* Multichannel wideband receivers
* Electronic support measures

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD9083 evaluation board requires an external 12 V supply. On-board
regulators generate the required voltage domains for the AD9083.

+------------+-------------+-------------------------------+
| Test Point | Voltage (V) | Description                   |
+------------+-------------+-------------------------------+
| TP19       | 12.0        | Board input supply            |
+------------+-------------+-------------------------------+
| TP6        | 1.0         | V1P0V (analog and digital)    |
+------------+-------------+-------------------------------+
| TP14       | 1.8         | V1P8V (analog and digital)    |
+------------+-------------+-------------------------------+

Board Connectors
~~~~~~~~~~~~~~~~

* **J20** — SMA connector for analog signal input
* **J2** — SMA connector for external trigger input (1.8 V active high)
* **J3** — SMA connector for system ready indicator
* **P1** — FMC HPC connector that mates with the carrier board

No-OS Supported Examples
------------------------

This project is organized around the no-OS variant-based build flow.
Selecting a variant at build time (``--variant <name>``) chooses which
application is compiled. Shared initialization data is defined in
`src <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9083/src>`__.

Demo Example
~~~~~~~~~~~~

The demo example initializes the AD9083 device, configures the AD9528 clock
generator, sets up the JESD204B link, and verifies link status. The AXI ADC
core and AXI DMA are initialized for data capture to DDR memory.

IIO Example
~~~~~~~~~~~

The IIO example launches an IIOD server on the carrier board so that the user
may connect via an IIO client application. Using IIO-Oscilloscope or Scopy,
the user can read samples from the device.

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

* `EVAL-AD9083 <https://www.analog.com/EVAL-AD9083>`_
* `ZCU102 <https://www.xilinx.com/ZCU102>`_

Connections
^^^^^^^^^^^

Connect the EVAL-AD9083 to the ZCU102 using the FMC HPC connector. The
JESD204B serial lanes interface with the FPGA transceivers via the FMC
connector. The AD9083 is configured over SPI from the FPGA. Ensure the FMC
I/O voltage levels are set to 1.8 V on the ZCU102. Connect a USB-to-serial
cable to the ZCU102 UART (115200 baud) to interact with the console or IIO
server.

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the ``no_os_build.py``
helper script. Available variants: ``demo``, ``iio``.
Available boards: ``zcu102``.

The HDL design used is ``ad9083_evb``. The hardware name composed for the XSA
download is ``ad9083_evb_zcu102``.

For toolchain setup and prerequisites, see the
`Xilinx CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_xilinx_cmake.html>`__.

.. code-block:: bash

   # Source the Vitis environment (adjust path for your installation):
   source ~/.xilinx/2025.1/Vitis/settings64.sh
   # PowerShell (Windows) equivalent:
   #   & "$env:XILINX_VITIS\settings64.bat"

   cd no-OS

   # Build the demo example on zcu102
   python tools/scripts/no_os_build.py build \
      --project ad9083 --variant demo --board zcu102 \
      --hardware /path/to/ad9083_evb_zcu102/system_top.xsa

   # Build the IIO example on zcu102
   python tools/scripts/no_os_build.py build \
      --project ad9083 --variant iio --board zcu102 \
      --hardware /path/to/ad9083_evb_zcu102/system_top.xsa

   # Build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project ad9083 --variant demo --board zcu102 \
      --hardware /path/to/ad9083_evb_zcu102/system_top.xsa \
      --probe openocd --flash
