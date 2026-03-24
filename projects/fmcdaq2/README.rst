FMCDAQ2 no-OS Example Project
=============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* `AD-FMCDAQ2-EBZ <https://www.analog.com/AD-FMCDAQ2-EBZ>`_

Overview
--------

The AD-FMCDAQ2-EBZ is an FMC board featuring the AD9144 high-speed DAC
and the AD9680 high-speed ADC. The AD9144 is a quad, 16-bit, 2.8 GSPS
DAC, and the AD9680 is a dual, 14-bit, 1 GSPS ADC. Both devices
interface with the FPGA carrier board through the JESD204B serial data
interface. Clocking is managed by the AD9523-1, a low jitter clock
generator that provides synchronized clock signals for the data
converters.

The board receives power through the FMC connector from the attached
FPGA carrier board, with on-board regulators providing the required
voltage domains. The evaluation board is compatible with multiple
Xilinx FPGA platforms.

Applications
------------

* Electronic test and measurement equipment
* General-purpose software radios
* Radar systems
* Ultra wideband satellite receivers
* Signals intelligence (SIGINT)
* Point to point communication systems

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The AD-FMCDAQ2-EBZ evaluation board receives power through the FMC
connector from the attached FPGA carrier board. The board features
multiple power management components including the ADP2384 step-down
switcher and multiple low dropout regulators (ADP7104, ADP150, ADP1740,
ADP1741, ADP1755) to meet the voltage requirements of the AD9144 DAC,
AD9680 ADC, and AD9523-1 clock generator. No external power supply is
required.

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/fmcdaq2/src/app>`__.

Demo example
~~~~~~~~~~~~

This is the default example which initializes the AD9680 ADC, AD9144
DAC, and AD9523-1 clock generator, configures the JESD204B links, and
verifies the link status. The AXI ADC/DAC cores and AXI DMA are
initialized for data capture and generation.

In order to build the demo example, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/fmcdaq2/Makefile>`__:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   IIOD ?= n

DMA example
~~~~~~~~~~~

The DMA example demonstrates direct memory access data transfer. It
initializes the JESD204B links, DMA controllers, and data converter
cores. A sine wave data source is loaded into memory and transmitted
through the AD9144 DAC via DMA, while the AD9680 ADC captures data
into DDR memory.

In order to build the DMA example, add the following flag:

.. code-block:: bash

   NEW_CFLAGS=-DDMA_EXAMPLE

IIO example
~~~~~~~~~~~

This project is an IIOD demo for the AD-FMCDAQ2-EBZ evaluation board.
The project launches the IIOD server on the board so that the user may
connect to it via an IIO client application. Using IIO-Oscilloscope,
the user can configure and stream data from the device.

If you are not familiar with ADI IIO Application, please take a look at:
:dokuwiki:`IIO No-OS </resources/tools-software/no-os-software/iio>`

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
:dokuwiki:`IIO Oscilloscope </resources/tools-software/linux-software/iio_oscilloscope>`

In order to build the IIO project, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/fmcdaq2/Makefile>`__:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   IIOD = y

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `AD-FMCDAQ2-EBZ <https://www.analog.com/AD-FMCDAQ2-EBZ>`_
* `ZC706 <https://www.xilinx.com/ZC706>`_
* `ZCU102 <https://www.xilinx.com/ZCU102>`_
* `KCU105 <https://www.xilinx.com/KCU105>`_

Connections
^^^^^^^^^^^

Connect the AD-FMCDAQ2-EBZ to the FPGA carrier board using the FMC HPC
connector. The JESD204B serial lanes interface with the FPGA
transceivers via the FMC connector. The AD9144, AD9680, and AD9523-1
are configured over SPI from the FPGA.

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
