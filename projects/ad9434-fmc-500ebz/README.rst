AD9434-FMC-500EBZ no-OS Example Project
=======================================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`AD9434-FMC-500EBZ`

Overview
--------

The AD9434-FMC-500EBZ evaluation board is a versatile platform designed
for testing the AD9434 analog-to-digital converter (ADC). It features
high-speed digital data output through an FPGA interface for efficient
data capture and analysis. The board supports various input signal
conditions and offers flexible clocking and data-capture options,
accommodating diverse testing scenarios. It integrates seamlessly with
existing development platforms and includes signal integrity assessment
tools, power management configurations, and options for internal or
external clock sources, making it well-suited for high-speed signal
acquisition design and development.

Applications
------------

- Wireless and wired broadband communications
- Cable reverse path
- Communications test equipment
- Radar and satellite subsystems
- Power amplifier linearization

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~~

The AD9434-FMC-500EBZ evaluation board requires a 12V power supply to
operate efficiently. This voltage level is essential to support all the
onboard components, including the AD9434 ADC and any supplementary devices 
integrated into the system. Userscshould ensure that the power supply is stable 
and capable of deliveringcthe necessary current to maintain optimal performance 
during dataccapture and processing tasks. Proper cabling and connections are
critical to prevent power loss and to maintain the system’s reliability
during testing and evaluation processes.

On-board Connectors
~~~~~~~~~~~~~~~~~~~~

+-----------------------------------+-----------------------------------+
| Connector Type                    | Description                       |
+-----------------------------------+-----------------------------------+
| FPGA Connector (J1)               | High speed data output connector  |
|                                   | used to interface with an FPGA    |
|                                   | for data capturing.               |
+-----------------------------------+-----------------------------------+
| SMA Connector (J2)                | Clock input connector for         |
|                                   | external clock source or          |
|                                   | oscillator connection.            |
+-----------------------------------+-----------------------------------+
| P1 Header                         | Used for configuring various      |
|                                   | signal conditions and test        |
|                                   | points.                           |
+-----------------------------------+-----------------------------------+
| USB Connector (J6)                | Power and communication interface |
|                                   | with the computer or test setup.  |
+-----------------------------------+-----------------------------------+

No-OS Build Setup
-------------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
-------------------------

The initialization data used in the examples is taken out from the
`Project Source Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9434-fmc-500ebz/src>`__

IIO example
~~~~~~~~~~~~

The IIO example code in the AD9434-FMC-500EBZ project facilitates the
interfacing of the AD9434 high-speed ADC via the IIO framework using an
FPGA. It begins by initializing and configuring the SPI interface for
communication with the ADC, enabling precise control over data
transmission. The ADC core and DMA controller are also set up for
seamless data acquisition. For integration with the IIO framework, the
code includes specific configurations that initialize UART communication
and establish IIO application descriptors.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with ADI IIO Oscilloscope Client, please take a
look at: `IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

The no-OS IIO Application together with the AD9434 driver take care of
all the back-end logic needed to setup the IIO server.

To build the IIO Demo example, use this configuration on the builds.json
file:

.. code-block::bash

   IIOD = y

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~~

Hardware Used
^^^^^^^^^^^^^

- AD9434-FMC-500EBZ Evaluation Board
- AMD Zynq™ 7000 SoC ZC706 Evaluation Kit

Connections
^^^^^^^^^^^

1. Locate the FMC HPC connector labeled J37 on the ZC706 board.

2. Align the FMC edge connector of the AD9434-FMC-500EBZ with the J37
   connector on the ZC706. The connectors are keyed to prevent improper
   orientation; the components on the FMC board should face away from
   the ZC706. Gently but firmly press the AD9434-FMC-500EBZ into the J37
   connector until it is fully seated.

4. Ensure the ZC706 is powered off before connecting or disconnecting
   the FMC card.

5. The ZC706 provides VADJ to the J37 FMC connector as required for
   FMC-compliant boards.

6. The AD9434-FMC-500EBZ will be automatically added to the JTAG chain
   when installed, due to electronically controlled switches on the
   ZC706.

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
