ADAQ8092_FMC no-OS Example Project
==================================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi::`EVAL-ADAQ8092-FMCZ`

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

- Communications
- Cellular base stations
- GPS receiver
- Non-destructive testing
- Portable medical imaging
- Multichannel data acquisition

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

===================== =============================
Connector              Function
SMA Connectors         RF Connections
FMC Connector          Interface with other hardware
Male-Male SMA Adapter  Signal Transfer
J5 and J6              Used as inputs or outputs
===================== =============================

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
:git-no-OS:`Project Data Source Path </projects/adaq8092_fmc/src>`.

Basic example
~~~~~~~~~~~~~

The basic example code in the no-OS repository for the ADAQ8092 ADC
demonstrates how to initialize and configure the device, which sets up
SPI and GPIO communication, performs a power-up sequence, and adjusts
settings such as clock polarity and LVDS output current. The example
showcases read and write operations on the device registers via SPI,
facilitating control over various modes and configurations of the ADC.
This example serves as a foundational guide for integrating the ADAQ8092
into user applications, illustrating the modular and flexible control
over the device’s operation.

In order to build the basic example, make sure you have the following
configuration in the :git-no-OS:`Makefile </projects/adaq8092_fmc/Makefile>`:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   BASIC_EXAMPLE=y
   IIOD=n

IIO example
~~~~~~~~~~~

The IIO example code handles the initialization, configuration, and data
acquisition of the ADAQ8092 ADC via an AXI interface. It begins by
initializing the SPI and GPIO interfaces for communication with the
ADAQ8092, configuring the ADC with parameters like power mode, clock,
and test settings. The ADC core and DMAC are then set up for data
capture, and a buffer is allocated for storing the sampled data. Data
acquisition is executed through a DMA transfer, with the buffer being
invalidated from the cache and printed post-transfer. If IIO support is
enabled, the code initializes an IIO application with a UART interface,
enabling continuous data streaming through the IIO framework for remote
access and control.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__.

In order to build the IIO project, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

   # Configure the example you want to enable by setting 'y' for enable and 'n' for disable
   BASIC_EXAMPLE=n
   IIOD=y

No-OS Supported Platforms
-------------------------

Xilinx Platform
~~~~~~~~~~~~~~~

Hardware Used
^^^^^^^^^^^^^

- EVAL-ADAQ8092-FMCZ
- ZedBoard

Connections
^^^^^^^^^^^

+-----------------------------------+-----------------------------------+
| Connection Interface              | Description                       |
+-----------------------------------+-----------------------------------+
| FMC Connector                     | Connect the EVAL-ADAQ8092-FMCZ to |
|                                   | the ZedBoard using the FMC        |
|                                   | connector (P1).                   |
+-----------------------------------+-----------------------------------+
| USB OTG Port                      | Connect a micro-USB cable from    |
|                                   | the PC to the USB OTG port on the |
|                                   | ZedBoard for data communication.  |
+-----------------------------------+-----------------------------------+
| Power Input                       | Supply 12V using the power        |
|                                   | adapter included with the         |
|                                   | ZedBoard.                         |
+-----------------------------------+-----------------------------------+
| SMA Cables                        | Optional: Connect SMA cables for  |
|                                   | clock and input signals as        |
|                                   | required.                         |
+-----------------------------------+-----------------------------------+
| Ethernet Cable                    | Optional: Use an Ethernet cable   |
|                                   | for network connection.           |
+-----------------------------------+-----------------------------------+
| GPIO                              | Configure general purpose I/O     |
|                                   | connections as needed for         |
|                                   | specific applications.            |
+-----------------------------------+-----------------------------------+

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   cp <SOME_PATH>/system_top.xsa .
   # to delete current build
   make reset
   # to build the project
   make
   # to flash the code
   make run
