AD469x no-OS Example Project
=============================

.. no-os-doxygen::

.. contents:: Table of Contents
   :depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-AD4696 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad4696.html>`_

Overview
--------

The EVAL-AD4696 is designed to demonstrate AD4696 performance and provide
access to its many configuration options via an easy-to-use ACE plugin
graphical interface. The AD4696 is a 16-channel, 16-bit, 1 MSPS, multiplexed
successive approximation register (SAR) analog-to-digital converter (ADC) that
enables high performance data acquisition of multiple signals in a small form
factor. The AD4696 employs easy drive features and on-chip channel sequencing
that simplify hardware and software designs.

The EVAL-AD4696 allows users to quickly evaluate the performance of the
AD4696 with no or minimal hardware modifications. The hardware includes two
externally driven analog input channels for evaluating AC performance and 14
channels with DC levels generated on board for evaluating DC and settling
performance.

Applications
------------

* Precision multichannel data acquisition
* Industrial process monitoring
* Instrumentation
* Optical transceiver module monitoring

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD4696 board is powered through the FMC connector on the ZedBoard.
The board uses the ZedBoard's 12V supply rail for its onboard regulators.

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`__

No-OS Supported Examples
------------------------

This project uses the no-OS variant-based build flow. Selecting a variant
at build time (``--variant <name>``) chooses which application is compiled.
The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad469x_evb/src/common>`__

Basic Example
~~~~~~~~~~~~~

This example initializes the AD469x ADC in single-cycle conversion mode,
captures samples via AXI DMA, and prints the results over UART.

Standard Sequence Example
~~~~~~~~~~~~~~~~~~~~~~~~~

This example initializes the AD469x ADC in standard sequence conversion
mode (``AD469X_SEQ_STANDARD``), captures samples via AXI DMA, and prints
results over UART.

Advanced Sequence Example
~~~~~~~~~~~~~~~~~~~~~~~~~

This example initializes the AD469x ADC in advanced sequence conversion
mode (``AD469X_SEQ_ADVANCED``), captures samples via AXI DMA, and prints
results over UART.

IIO Example
~~~~~~~~~~~

This example launches an IIOD server on the board so that the user may
connect to it via an IIO client. Using IIO-Oscilloscope, the user can
configure the ADC and view the measured data on a plot. Uses standard
sequence conversion mode.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-AD4696 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad4696.html>`_
* `ZedBoard <https://www.analog.com/en/resources/reference-designs/powering-zynq-evaluation-development-board-zedboard.html>`_

Connections
^^^^^^^^^^^

Connect the EVAL-AD4696 to the ZedBoard via the FMC LPC connector. Connect
a USB cable to the ZedBoard USB-UART port. Power the ZedBoard via the 12V
barrel jack (J20).

The UART console appears on the ZedBoard USB-UART port at 115200 baud, 8N1.

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``basic``,
``standard_seq``, ``advance_seq``, ``iio``.
Available boards: ``zed``.

For toolchain setup and prerequisites, see the
`Xilinx CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_xilinx_cmake.html>`__.

.. code-block:: bash

   # Source the Vitis environment (sets XILINX_VITIS and adds tools to PATH)
   source /path/to/Vitis/settings64.sh
   # PowerShell (Windows) equivalent:
   #   $env:XILINX_VITIS = "<C:\path\to\Vitis>"

   cd no-OS

   # build the example on the ZedBoard (replace --variant as needed)
   python tools/scripts/no_os_build.py build \
      --project ad469x_evb --variant iio --board zed \
      --hardware /path/to/system_top.xsa

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project ad469x_evb --variant iio --board zed \
      --hardware /path/to/system_top.xsa \
      --probe openocd --flash
