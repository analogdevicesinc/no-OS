AD4110 no-OS Example Project
============================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-AD4110-1SDZ`

Overview
--------

The EVAL-AD4110-1SDZ evaluation board assesses the AD4110-1 universal
input analog front end (AFE) for industrial control systems. It supports
voltage, current, thermocouple, and RTD sensor inputs. The board
interfaces with Windows-based PC software and requires an external power
supply voltage between ±12V and ±20V. It operates via SPI communication,
enabling integration with peripheral devices. Essential setup includes
power-up sequences and software installations for accurate sensor signal
processing. The board features connectors for power supply, voltage
input, universal input, and thermocouple input, making it versatile for
various industrial applications. It's designed to manage both voltage
and current modes efficiently, catering to diverse industrial needs.

Applications
------------

- Process control and industrial automation
- Remote and distributed control systems
- Instrumentation and measurement
- Sensor and data acquisition

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD4110-1SDZ evaluation board requires an external power supply
with voltage between ±12V and ±20V, connected via the power supply
connector. This configuration is essential for enabling both voltage and
current mode inputs on the board's analog front end. It also allows the
board to power a current output sensor connected across its input pins,
which is beneficial for applications requiring external sensor power.

On-board Connectors
~~~~~~~~~~~~~~~~~~~

========= ==============================
Connector Function
J14       Power Supply Connector
J2        Voltage Input Connector
J6        Universal Input Connector
J1        Thermocouple Input Connector
J7        Peripheral Interface Connector
========= ==============================

Digital Communication Pins
~~~~~~~~~~~~~~~~~~~~~~~~~~

======= ======== ============================
Pin No. Mnemonic Description
14      DOUT/RDY Serial Interface Data Output
15      DIN      Serial Interface Data Input
16      SCLK     Serial Interface Clock
17      CS       Serial Interface Chip Select
======= ======== ============================

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the 
:git-no-OS:`Project Source Path </projects/ad4110/src/app>`.

Basic example
~~~~~~~~~~~~~

The basic example code for the AD4110 in no-OS projects involves setting
up and operating the SPI and IRQ configurations for continuous data
reading on a Xilinx platform. The ``main.c`` file initializes SPI with
specified device ID, speed, and mode, and sets up IRQ controllers for
managing interrupts. The AD4110 device is configured for voltage
reference and data settings. Data is continuously read and stored in a
buffer, ``data_buf``, with outputs printed to the console. Cache
management is employed by enabling and disabling instruction and data
caches to enhance performance. This example demonstrates integration
between hardware and software components in an embedded system
environment, focusing on efficient data acquisition and output.

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~~

Hardware Used
^^^^^^^^^^^^^

- :adi:`EVAL-AD4110-1SDZ`
- ZedBoard

Connections
^^^^^^^^^^^

+-----------------------------------+-----------------------------------+
| Connector/Component               | Connection Details                |
+-----------------------------------+-----------------------------------+
| J1                                | 120-pin connector to mate with    |
|                                   | the SDP-B board, facilitating     |
|                                   | communication and power           |
|                                   | connectivity                      |
+-----------------------------------+-----------------------------------+
| J2 (SPI PMOD)                     | 1 = CS, 2 = DIN, 3 = DOUT, 4 =    |
|                                   | SCLK, 5 = GND, 6 = 5 V external   |
|                                   | power                             |
+-----------------------------------+-----------------------------------+
| J6                                | High voltage universal input for  |
|                                   | analog signals                    |
+-----------------------------------+-----------------------------------+
| J8                                | Low voltage inputs for the ADC    |
|                                   | only                              |
+-----------------------------------+-----------------------------------+
| J10                               | Thermocouple input for            |
|                                   | temperature measurement           |
+-----------------------------------+-----------------------------------+
| J12                               | Optionally supply DVDD for IOVDD  |
|                                   | and AVDD5                         |
+-----------------------------------+-----------------------------------+
| J14                               | Power supply block; connect VDD,  |
|                                   | VSS, and GND to power the board   |
+-----------------------------------+-----------------------------------+
| LED Indicators                    | LED1: Green - SDP connection      |
|                                   | valid; LED2: Red - Error on the   |
|                                   | analog input or ADC overrange;    |
|                                   | LED3: Green - 5 V supply active   |
+-----------------------------------+-----------------------------------+
| Power Supply Setup                | Connect +15 V to VDP, -15 V to    |
|                                   | VSS, and 0V to GND to ensure      |
|                                   | proper powering                   |
+-----------------------------------+-----------------------------------+
| Default Mode Setup                | Upon power-up, operates in        |
|                                   | current mode, accepting input     |
|                                   | currents unless reconfigured      |
|                                   | through the software interface    |
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
