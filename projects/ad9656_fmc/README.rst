AD9656_FMC no-OS Example Project
================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `AD9656-125EBZ <https://www.analog.com/AD9656>`_

Overview
--------

The `AD9656-125EBZ <https://wiki.analog.com/resources/eval/ad9656-125ebz>`_
is an evaluation board for the AD9656, a quad, 16-bit, 125 MSPS
analog-to-digital converter (ADC) designed for high performance with low
power and ease of use. It incorporates an on-chip sample and hold
circuit, operating at up to 125 MSPS. The ADC features a signal-to-noise
ratio (SNR) of 79.9 dBFS at 16 MHz and 78.1 dBFS at 64 MHz, and a
spurious-free dynamic range (SFDR) of 86 dBc to Nyquist. The analog
input range is flexible, from 2.0 V p-p to 2.8 V p-p, with a low power
consumption of 197 mW per channel. The device supports JESD204B
Subclass 1 coded serial digital outputs, individual channel power-down
modes, and built-in digital test pattern generation.

Applications
------------

* Medical ultrasound and MRI
* High-speed imaging
* Quadrature radio receivers
* Diversity radio receivers
* Portable test equipment

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The AD9656-125EBZ can be powered in multiple ways:

* **Via FMC connector (default):** The board receives power from the
  carrier board (ZCU102 or HSC-ADC-EVALEZ) through the FMC connector.
  Jumpers P101 and P103 must have pins 1-2 connected. Do not connect
  the external 6 V wall supply in this configuration.

* **External wall supply (6 V, 2 A):** A wall-mountable 6 V, 2 A
  switching power supply can power the board via the 2.1 mm barrel jack
  at P102. Set jumpers P101 and P103 to pins 2-3. On-board LDO
  regulators generate the 1.8 V supply for the AD9656 core.

Board Connectors
~~~~~~~~~~~~~~~~

* **P2** -- FMC HPC connector, carries JESD204B differential data lanes,
  SPI, power, and clocking signals to/from the carrier board
* **J302** -- SMA connector for external single-ended clock input
  (AC-coupled, 50 ohm terminated)
* **J304** -- Jumper to enable/disable the on-board 125 MHz crystal
  oscillator (Y801); remove for external clock
* **CHA-CHD** -- SMA connectors for differential analog inputs per channel
* **P101, P103** -- Power source selection jumpers
* **P102** -- 2.1 mm barrel jack for 6 V external power supply

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Source Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9656_fmc/src>`__.

The default example initializes the AD9656 ADC along with the AD9508
clock distribution IC and the AD9553 clock synthesizer over SPI,
configures the JESD204B receive link between the FPGA and the ADC, runs
PN9 and PN23 sequence verification on all four channels, then performs a
DMA capture of 16384 samples into DDR memory.

IIO example
~~~~~~~~~~~

The IIO example launches an IIOD server on the ZCU102 board so that the
user may connect to it via an IIO client. Using the IIO Oscilloscope
application, users can access device settings such as sampling
frequency, channel data scale, JESD204B link parameters, and other
AD9656 configuration attributes, facilitating seamless data management
and real-time streaming of the quad-channel, 16-bit ADC data over the
JESD204B interface.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO Oscilloscope Client, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

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

* `AD9656-125EBZ <https://www.analog.com/AD9656>`_
* `ZCU102 <https://www.xilinx.com/ZCU102>`_

Connections
^^^^^^^^^^^

Connect the AD9656-125EBZ to the ZCU102 evaluation board through the FMC
HPC0 connector. The JESD204B differential output lanes, SPI
configuration bus, and power signals are all routed through this single
FMC connector.

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
