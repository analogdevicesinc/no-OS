ADIN1110 no-OS Example Project
==============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-ADIN1110 <https://www.analog.com/EVAL-ADIN1110>`_

Overview
--------

The EVAL-ADIN1110EBZ is a rapid evaluation platform for the ADIN1110
(40-pin) and ADIN1111 (32-pin) 10BASE-T1L MAC-PHY, facilitating 10Mbps
Ethernet over 1.7 km of cable. It operates in standalone mode with
hardware configuration or in connected mode via USB for detailed
register access and diagnostics through a serial interface. The board
includes LEDs for status indication, an Arduino interface, SPI for
configuration and data access, a prototyping area, and electrical signal
test points. The ADIN1111 mirrors the ADIN1110's functionality with a
compact design that includes integrated hybrid and termination
resistors.

Applications
------------

* Field instruments
* Building automation and fire safety
* Factory automation
* Edge sensors and actuators
* Condition monitoring and machine connectivity

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-ADIN1110EBZ evaluation board operates with a wide input voltage
range of 5V to 32V DC, requiring a power supply that can be delivered
via a USB connection or an external DC source. Its power consumption is
approximately 0.6W. The board manages its power supply internally
through options such as an on-chip linear regulator (LDO) or an external
step-down regulator (LTC3547) for generating a 1.1V supply.
Additionally, it provides power connections for 3.3V and 1.8V rails,
accommodating the microcontroller and VDDIO requirements. The selection
of the power source is configurable via jumpers, supporting various
development or testing configurations. The kit includes a USB-A to Micro
USB-B cable for connection purposes.

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the example is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adin1110/src/common>`__.

The macros used in Common Data are defined in platform specific files
found in the
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adin1110/src/platform/maxim>`__.

frame_rx_tx example
~~~~~~~~~~~~~~~~~~~

The ``frame_rx_tx`` example in the ADIN1110 no-OS driver involves
transmitting and receiving Ethernet frames using ``adin1110_write_fifo``
and ``adin1110_read_fifo`` functions. The ``adin1110_write_fifo``
function writes an Ethernet frame to the TX FIFO after ensuring
sufficient space and padding the frame to meet Ethernet size standards,
then conducts an SPI transfer. Conversely, ``adin1110_read_fifo`` reads
a frame from the RX FIFO by checking port validity and preparing the SPI
message to read the data. Both functions rely on SPI communication with
the ADIN1110, integrating descriptors and error checking into the
control flow. Dependencies include other functions for SPI transfers and
register operations essential for the communication process.

In order to build the example, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/adin1110/Makefile>`__:

.. code-block:: bash

   # Select the example
   EXAMPLE = frame_rx_tx

No-OS Supported Platforms
-------------------------

Maxim Platform
~~~~~~~~~~~~~~

Hardware Used
^^^^^^^^^^^^^

* `EVAL-ADIN1110 <https://www.analog.com/EVAL-ADIN1110>`_
* MAX32650FTHR

Connections
^^^^^^^^^^^

Connect the SPI and control signals between the MAX32650FTHR and
EVAL-ADIN1110EBZ:

========= ================ ====================
Signal    MAX32650FTHR Pin EVAL-ADIN1110EBZ Pin
========= ================ ====================
GND       GND              P402_9 or P402_10
SCLK      P2_29            P402_7
SS (CS_N) P2_26            P402_1
MOSI      P2_28            P402_3
MISO      P2_27            P402_5
========= ================ ====================

Ensure voltage levels are compatible (typically 3.3V).

**Board Configuration**

* On EVAL-ADIN1110EBZ:

  * Jumper J301: Position 1-2 (GND) to hold onboard MCU in reset.
  * Jumper J302: Position 1-2 (3.3V) for correct signal voltage.
  * DIP Switch S201: All OFF.

**Power Supply**

* Power both boards via USB or external 5V supply.
* Use jumper J1 on the ADIN1110 board to select power source.

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project
   make PLATFORM=maxim TARGET=max32650
   # to flash the code
   make run
