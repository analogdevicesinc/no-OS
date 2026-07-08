AD7768-EVB no-OS Example Project
=================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-AD7768FMCZ <https://www.analog.com/ad7768>`_

Overview
--------

The AD7768 is an 8-channel, simultaneously sampling, 24-bit sigma-delta ADC
with power scaling and a 110.8 kHz input bandwidth. Each channel has a
dedicated sigma-delta modulator and digital filter, enabling synchronized
sampling of both AC and DC signals. The AD7768 achieves 108 dB dynamic range
at maximum input bandwidth, with typical performance of +/-2 ppm INL,
+/-50 uV offset error, and +/-30 ppm gain error. Three power modes are
available: fast (256 kSPS, 110.8 kHz bandwidth, 51.5 mW per channel), median
(128 kSPS, 55.4 kHz bandwidth, 27.5 mW per channel), and low power
(32 kSPS, 13.8 kHz bandwidth, 9.375 mW per channel).

The EVAL-AD7768FMCZ evaluation board provides full-featured evaluation of the
AD7768 with eight differential analog inputs via SMA connectors, signal
routing through ADA4896 amplifiers, and a precise 4.096 V ADR444 reference.
Configuration and operation are managed through a 4-wire SPI interface. The
board connects to a Xilinx carrier board via the FMC LPC connector; an AXI ADC
core and AXI DMAC on the FPGA handle data capture.

Applications
------------

* Data acquisition systems: USB/PXI/Ethernet
* Instrumentation and industrial control loops
* Audio testing and measurement
* Vibration and asset condition monitoring
* 3-phase power quality analysis
* Sonar
* High-precision medical EEG, EMG, and ECG

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD7768FMCZ requires an external 7 V to 9 V supply connected to J1
(dc plug) or J3 (bench top supply connector). On-board ADP7118 LDOs regulate
the input to 5 V (for AVDD1 and amplifiers) and 3.3 V (for IOVDD). Alternatively,
the board can be powered from the EVAL-SDP-CH1Z 12 V rail by setting LK2 to
Position A. The IOVDD logic voltage is 2.25 V to 3.6 V; a 1.8 V mode is available
by bypassing the internal LDO.

On-board Connectors
~~~~~~~~~~~~~~~~~~~

===================================  ==========================================================
Connector                            Description
===================================  ==========================================================
J1                                   Wall wart (dc plug) power supply input (7 V to 9 V)
J3                                   Bench top power supply input (7 V to 9 V)
J2                                   External supply for AVDD1, AVDD2, IOVDD, AVSS, AGND
J6, J7                               SMA/SMB connectors for clock input (XTAL2/CLKIN, XTAL1)
J8, J9                               8-pin connector for analog input to Channel 4-7
J13, J14                             8-pin connector for analog input to Channel 0-3
AI0+ to AI7+, AI0- to AI7-           SMA connectors for differential analog inputs per channel
P1                                   FMC LPC connector (underside) that mates with carrier board
===================================  ==========================================================

No-OS Supported Examples
------------------------

This project is organized around the no-OS variant-based build flow.
Selecting a variant at build time (``--variant <name>``) chooses which
application is compiled. Shared initialization data is defined in
`src <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad7768-evb/src>`__.

Demo Example
~~~~~~~~~~~~

The demo example initializes the AD7768 ADC using SPI and GPIO interfaces.
The AXI ADC core is set up for channel management, and an AXI DMAC is
employed for data acquisition to DDR memory.

IIO Example
~~~~~~~~~~~

The IIO example initializes and configures the AD7768 ADC and enables
interaction with libiio clients via the serial backend. It manages data
acquisition using AXI DMA for sample collection, cyclic data transfers,
and communication with DDR memory.

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

* `EVAL-AD7768FMCZ <https://www.analog.com/ad7768>`_
* `ZedBoard <https://www.analog.com/en/resources/reference-designs/powering-zynq-evaluation-development-board-zedboard.html>`_

Connections
^^^^^^^^^^^

* Mount the EVAL-AD7768FMCZ to the ZedBoard through the FMC LPC connector.
* Connect a 7 V to 9 V power supply to J1 (dc plug) or J3 (bench top) on
  the EVAL-AD7768FMCZ.
* Connect a micro-USB cable to the ZedBoard for UART communication (115200 baud).
* Connect desired input signals to the evaluation board via the SMA connectors.
* Power on the ZedBoard. Verify that the green power LED is illuminated.

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the ``no_os_build.py``
helper script. Available variants: ``demo``, ``iio``.
Available boards: ``zed``.

For toolchain setup and prerequisites, see the
`Xilinx CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_xilinx_cmake.html>`__.

.. code-block:: bash

   # Source the Vitis environment (adjust path for your installation):
   source ~/.xilinx/2025.1/Vitis/settings64.sh
   # PowerShell (Windows) equivalent:
   #   & "$env:XILINX_VITIS\settings64.bat"

   cd no-OS

   # Build the demo example on zed
   python tools/scripts/no_os_build.py build \
      --project ad7768-evb --variant demo --board zed \
      --hardware /path/to/ad7768evb_zed/system_top.xsa

   # Build the IIO example on zed
   python tools/scripts/no_os_build.py build \
      --project ad7768-evb --variant iio --board zed \
      --hardware /path/to/ad7768evb_zed/system_top.xsa

   # Build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project ad7768-evb --variant demo --board zed \
      --hardware /path/to/ad7768evb_zed/system_top.xsa \
      --probe openocd --flash
