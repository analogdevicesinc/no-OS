AD9172 no-OS Example Project
============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-AD917x <https://www.analog.com/eval-ad9172>`_

Overview
--------

The EVAL-AD917x evaluation board family (AD9171-FMC-EBZ, AD9172-FMC-EBZ,
AD9173-FMC-EBZ) are FMC form-factor boards with FMC connectors that
comply with the VITA 57.1 standard, designed for evaluating the AD917x
family of high-performance, dual, 16-bit RF DACs. The AD9172, for
example, supports DAC sample rates up to 12.6 GSPS with an 8-lane, 15
Gbps JESD204B data input interface and features three complex,
bypassable data input channels per RF DAC, each with configurable gain,
interpolation filters, and numerically controlled oscillators (NCOs) for
flexible multiband frequency planning. To operate the evaluation board,
the user must attach it to a compatible FMC carrier board such as the
Xilinx ZC706 or ZCU102; alternatively, Analog Devices provides the
ADS7-V2 FPGA carrier as a data source. A low-noise HMC7044 clock
synthesizer on the evaluation board provides all required clocking,
including SYSREF for JESD204B Subclass 1 operation, while an optional
external clock input SMA allows users to supply their own low-jitter
clock source.

Applications
------------

* Wireless communications infrastructure
* Instrumentation, automatic test equipment (ATE)
* Radars and jammers

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD917x board is primarily powered from the FMC carrier board
when connected to the Xilinx ZC706 or ZCU102. The carrier board supplies
12V and 3.3V rails through the FMC connector to the evaluation board,
where on-board switching regulators and LDOs generate the internal
supply rails: 1.8V for the AD9172 analog and I/O power domains and
1.2V for the AD9172 digital core. The HMC7044 clock chip and associated
circuitry are also powered from these on-board regulated rails. When
using the board in standalone NCO-only mode (without an FMC carrier), an
external +12V laboratory power supply can be connected to TP41 (red) and
GND to TP42 (black) as described in the
`EVAL-AD917x user guide <https://wiki.analog.com/resources/eval/dpg/ad917x-fmc-ebz>`_.

Board Connectors
~~~~~~~~~~~~~~~~

* **J1** -- SMA connector for DAC1 RF output
* **J2** -- SMA connector for DAC0 RF output
* **J3** -- SMA connector for CLKOUT
* **J34** -- SMA connector for direct external DAC clock input
* **J41** -- SMA connector for external HMC7044 PLL1 reference clock
* **P1** -- FMC HPC connector (VITA 57.1), carries JESD204B data lanes, SPI,
  clocking, and power signals to/from the carrier board

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Source Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9172/src>`__.

DMA example
~~~~~~~~~~~

The DMA example initializes the AD9172 dual RF DAC device over the SPI
interface, configures the JESD204B high-speed serial link between the
FPGA and the DAC, and uses the AXI DMA controller on the Xilinx FPGA
fabric to transfer a pre-generated DAC data vector (such as a multi-tone
or sine wave LUT) from DDR memory directly to the AD9172 data input at
full speed. The example demonstrates wideband signal generation by
streaming I/Q samples through the JESD204B lanes, verifying lock of the
SERDES PLL, code group synchronization, and frame sync status, allowing
the user to observe the expected RF waveform on a spectrum analyzer
connected to the DAC output SMA connectors.

In order to build the DMA example, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

   NEW_CFLAGS=-DDMA_EXAMPLE

IIO example
~~~~~~~~~~~

The IIO example launches an IIOD server on the Xilinx FPGA carrier board
so that the user may connect to it via an IIO client. Using the IIO
Oscilloscope application, users can access device settings such as DAC
interpolation rate, channel NCO frequency offset, output scale, and
JESD204B link parameters, facilitating seamless configuration and
real-time control of the AD9172 RF DAC output signal.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

To build the IIO project, add the following flag when invoking make:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   IIOD = y

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-AD917x <https://www.analog.com/eval-ad9172>`_
* `ZC706 <https://www.xilinx.com/ZC706>`_
* `ZCU102 <https://www.xilinx.com/ZCU102>`_

Connections
^^^^^^^^^^^

Connect the EVAL-AD917x FMC board to the Xilinx carrier board FMC HPC
connector (J3 on ZC706, J5/HPC0 on ZCU102). The FMC connector routes
all JESD204B high-speed serial lanes, SPI interface, clock signals, and
power rails between the two boards.

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # copy the Xilinx hardware description file
   cp <SOME_PATH>/system_top.xsa .
   # to delete current build
   make reset
   # to build the project
   make
   # to flash the code
   make run
