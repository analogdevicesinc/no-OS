AD6676-EBZ no-OS Example Project
=================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-AD6676 <https://www.analog.com/EVAL-AD6676>`_

Overview
--------

The AD6676 is a highly integrated, wideband IF receiver subsystem that can
digitize radio frequency (RF) bands up to 160 MHz in width centered on an
intermediate frequency (IF) of 70 MHz to 450 MHz. It features digital signal
processing with on-chip NCOs and a quadrature digital downconverter with
decimation factors of 12, 16, 24, and 32. High-speed data transfer is
facilitated by JESD204B single or dual-lane outputs. The device operates with
supply voltages of 1.1 V and 2.5 V, consuming as little as 1.20 W. It includes
an integrated attenuator with a 27 dB range, adjustable in 1 dB increments, and
supports automatic gain control (AGC) via an AGC data port.

The EVAL-AD6676 evaluation board provides a full-featured platform for
evaluating the AD6676. It includes an FMC connector for interfacing with
FPGA carrier boards, an SPI interface for ADC setup and control, and a
quad frequency oscillator with a default 200 MHz reference clock. The
board receives its power via the FMC connector from the carrier board.

Applications
------------

* Wideband cellular infrastructure equipment and repeaters
* Point-to-point microwave equipment
* Instrumentation (spectrum and communication analyzers)
* Software defined radio

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD6676 evaluation board utilizes onboard low-dropout (LDO)
regulators powered via the FMC interface from the carrier board. The
board requires a 200 MHz reference clock (configurable to 125 MHz,
150 MHz, or 250 MHz via the on-board quad frequency oscillator), with
ADC frequencies up to 3.2 GHz. No additional external power supply is
needed when the board is correctly mounted on a supported carrier.

On-board Connectors
~~~~~~~~~~~~~~~~~~~

===============================  =====================================================
Connector                        Description
===============================  =====================================================
FMC Connector                    HPC FMC interface for connecting to the carrier board
SPI Interface                    Provides setup and control for the ADC
JESD204B Interface Outputs       High-speed data outputs (single or dual-lane)
CLK+/CLK-                        Differential clock inputs for ADC synchronization
VIN+/VIN-                        Differential analog signal inputs
GPIO                             General-purpose I/O for configuration and monitoring
AGC Pins                         Automatic gain control loop interface
Power Supply Connectors          Supply rails (VDD1, VDDL, VDDQ, etc.)
===============================  =====================================================

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad6676-ebz/src>`_

Demo Example
~~~~~~~~~~~~

The demo example initializes the AD6676 hardware by configuring GPIO and SPI
interfaces for communication with the device. It sets up the JESD204B
transport layer using AXI XCVR and JESD cores, initializes the ADC core,
and performs PRBS verification (PN9 and PN23 sequences). Finally, it
executes DMA transfers to capture ADC data.

IIO Example
~~~~~~~~~~~

The IIO example configures the AD6676 device to interact with an IIO client
for data capture and processing. It initializes platform resources including
GPIO and SPI, sets up the JESD204B interface and ADC core, and runs an IIO
daemon that enables real-time data capture through the IIO client interface.

If you are not familiar with ADI IIO Application, please take a look at:
:dokuwiki:`IIO No-OS </resources/tools-software/no-os-software/iio>`

If you are not familiar with ADI IIO Oscilloscope Client, please take a
look at:
:dokuwiki:`IIO Oscilloscope </resources/tools-software/linux-software/iio_oscilloscope>`

No-OS Supported Platforms
--------------------------

Xilinx Platform
~~~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-AD6676 <https://www.analog.com/EVAL-AD6676>`_
* `ZC706 <https://www.xilinx.com/products/boards-and-kits/ek-z7-zc706-g.html>`_

Connections
^^^^^^^^^^^

* Plug the EVAL-AD6676 FMC connector into the HPC (J37) connector on the
  ZC706 board. The connector and board are keyed to ensure correct
  orientation; the evaluation board should face away from the main ZC706
  PCB.
* The EVAL-AD6676 receives its power directly via the FMC connector from
  the ZC706 board. No additional external power is needed if correctly
  mounted.
* Connect a USB-UART cable to the ZC706 UART port for serial console
  output (115200 baud, 8N1).

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``demo``, ``iio``.
Available boards: ``zc706``.

For toolchain setup and prerequisites, see the
`Xilinx CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_xilinx_cmake.html>`__.

.. code-block:: bash

   # source the Vitis environment (adjust path to your installation)
   source /path/to/Vitis/settings64.sh
   # PowerShell (Windows) equivalent:
   #   & "C:\path\to\Vitis\settings64.bat"

   cd no-OS

   # build the demo example on the ZC706 board
   python tools/scripts/no_os_build.py build \
      --project ad6676-ebz --variant demo --board zc706 \
      --hardware /path/to/ad6676evb_zc706/system_top.xsa

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project ad6676-ebz --variant demo --board zc706 \
      --hardware /path/to/ad6676evb_zc706/system_top.xsa \
      --probe openocd --flash
