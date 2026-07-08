AD796x no-OS Example Project
=============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-AD7960FMCZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad7960.html>`_

Overview
--------

The EVAL-AD7960FMCZ is a fully featured evaluation kit designed to demonstrate
the performance of the AD7960, a low-power 18-bit, 5 MSPS PulSAR differential
ADC. The device uses an LVDS serial interface for high-speed data transfer and
is configured through a simple 3-wire SPI port. The evaluation board connects to
a Xilinx carrier board (such as the ZedBoard) via the FMC LPC connector, where
an AXI ADC core, AXI DMAC, and AXI PWM generator on the FPGA manage data
capture, conversion triggering, and clocking.

Applications
------------

* High-speed data acquisition systems
* Communications and instrumentation
* Medical imaging
* Radar and electronic warfare
* Test and measurement

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD7960FMCZ is powered from the carrier board FMC connector.
No external power supply is required when connected to a compatible carrier.

On-board Connectors
~~~~~~~~~~~~~~~~~~~

========= =========================================================
Connector Description
========= =========================================================
P1        FMC LPC connector (underside) that mates with carrier
J1, J2    SMA connectors for differential analog input (IN+, IN-)
========= =========================================================

No-OS Supported Examples
------------------------

This project is organized around the no-OS variant-based build flow.
Selecting a variant at build time (``--variant <name>``) chooses which
application is compiled. Shared initialization data is defined in
`src <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad796x_fmcz/src>`__.

IIO Example
~~~~~~~~~~~

The IIO example exposes the AD796x ADC through the IIO framework using
iio_axi_adc and AXI DMAC, enabling host-side data capture over UART.
The project launches an IIOD server so that the user may connect via an
IIO client such as IIO-Oscilloscope or Scopy.

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

* `EVAL-AD7960FMCZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad7960.html>`_
* `ZedBoard <https://www.analog.com/en/resources/reference-designs/powering-zynq-evaluation-development-board-zedboard.html>`_

Connections
^^^^^^^^^^^

Mount the EVAL-AD7960FMCZ to the ZedBoard via the FMC LPC connector. The AXI
ADC core on the Zynq PL communicates with the AD796x through the LVDS interface
routed via the FMC connector. Connect a micro-USB cable to the ZedBoard UART
(115200 baud) to interact with the IIO server.

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the ``no_os_build.py``
helper script. Available variants: ``iio``.
Available boards: ``zed``.

For toolchain setup and prerequisites, see the
`Xilinx CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_xilinx_cmake.html>`__.

.. code-block:: bash

   # Source the Vitis environment (adjust path for your installation):
   source ~/.xilinx/2025.1/Vitis/settings64.sh
   # PowerShell (Windows) equivalent:
   #   & "$env:XILINX_VITIS\settings64.bat"

   cd no-OS

   # Build the IIO example on zed
   python tools/scripts/no_os_build.py build \
      --project ad796x_fmcz --variant iio --board zed \
      --hardware /path/to/pulsar_lvds_adc_zed/system_top.xsa

   # Build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project ad796x_fmcz --variant iio --board zed \
      --hardware /path/to/pulsar_lvds_adc_zed/system_top.xsa \
      --probe openocd --flash
