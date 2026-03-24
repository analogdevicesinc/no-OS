ADRV9009 no-OS Example Project
==============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* `ADRV9009-W/PCBZ <https://www.analog.com/ADRV9009>`_

Overview
--------

The ADRV9009-W/PCBZ is an FMC radio module designed for evaluating the
ADRV9009, a highly integrated RF transceiver offering wideband transmit,
receive, and observation receiver functionality. The board interfaces
with FPGA carrier boards via the FMC connector and enables developers to
evaluate and prototype RF signal chains for a variety of applications.

The ADRV9009 features dual transmitters, dual receivers, and dual
observation receivers covering a frequency range of 75 MHz to 6 GHz.
It integrates a JESD204B interface for high-speed digital data transfer,
on-chip DPD (digital pre-distortion) support, and ARM Cortex-M3 based
firmware for device control. The device operates on multiple RF profiles
configurable through the Talise Evaluation Software (TES).

Applications
------------

* 3G, 4G, and 5G TDD macrocell base stations
* TDD active antenna systems
* Massive multiple input, multiple output (MIMO)
* Phased array radar
* Electronic warfare
* Military communications
* Portable test equipment

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The ADRV9009-W/PCBZ evaluation board receives power through the FMC
connector from the attached FPGA carrier board. On-board regulators
generate the required voltage domains for the ADRV9009 transceiver.

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

For full system-level design setup, visit:
`ADRV9009/ADRV9008 No-OS System Level Design Setup <https://wiki.analog.com/resources/eval/user-guides/adrv9009/no-os-setup>`__

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adrv9009/src>`__.

Demo example
~~~~~~~~~~~~

This is the default example which initializes the ADRV9009 device,
configures the clocking, JESD204B links, and RF signal paths. It
performs device calibrations and verifies link status.

In order to build the demo example, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/adrv9009/Makefile>`__:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   IIOD ?= n

DMA example
~~~~~~~~~~~

The DMA example demonstrates direct memory access data transfer for
the ADRV9009. It initializes the JESD204B links, DMA controllers,
and data converter cores, then exercises the ADC/DAC data paths for
test and verification.

In order to build the DMA example, add the following flag:

.. code-block:: bash

   NEW_CFLAGS=-DDMA_EXAMPLE

IIO example
~~~~~~~~~~~

This project is an IIOD demo for the ADRV9009 evaluation board. The
project launches the IIOD server on the board so that the user may
connect to it via an IIO client application. Using IIO-Oscilloscope,
the user can configure and stream data from the device.

If you are not familiar with ADI IIO Application, please take a look at:
:dokuwiki:`IIO No-OS </resources/tools-software/no-os-software/iio>`

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
:dokuwiki:`IIO Oscilloscope </resources/tools-software/linux-software/iio_oscilloscope>`

In order to build the IIO project, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/adrv9009/Makefile>`__:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   IIOD = y

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `ADRV9009-W/PCBZ <https://www.analog.com/ADRV9009>`_
* `ZC706 <https://www.xilinx.com/ZC706>`_
* `ZCU102 <https://www.xilinx.com/ZCU102>`_

Connections
^^^^^^^^^^^

Connect the ADRV9009-W/PCBZ to the ZC706 or ZCU102 using the FMC HPC
connector. The JESD204B serial lanes interface with the FPGA
transceivers via the FMC connector. The ADRV9009 is configured over
SPI from the FPGA.

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
