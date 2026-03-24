AD4110 no-OS Example Project
============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* `EVAL-AD4110-1SDZ <https://www.analog.com/EVAL-AD4110-1>`_

Overview
--------

The EVAL-AD4110-1SDZ evaluation board is designed for evaluating the
AD4110-1, a complete, single-channel, universal input analog-to-digital
front end for industrial process control systems. The AD4110-1 is fully
software configurable for current or voltage signals and allows direct
interface to all standard industrial analog signal sources such as
±20 mA, ±4 mA to ±20 mA, ±10 V, and all thermocouple types. It also
includes excitation current sources for RTD sensors.

The integrated analog front end features a programmable gain amplifier
(PGA) with 16 gain settings from 0.2 to 24, and a precision 24-bit
sigma-delta ADC offering conversion rates from 5 SPS to 125 kSPS with
simultaneous 50 Hz and 60 Hz noise rejection. The board requires an
external power supply voltage between ±12 V and ±20 V and operates via
a 4-wire SPI interface.

Applications
------------

* Process control and industrial automation
* Remote and distributed control systems
* Instrumentation and measurement
* Sensor and data acquisition

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD4110-1SDZ evaluation board requires an external power supply
of ±15 V (recommended), connected via connector J14. Acceptable supply
range is ±12 V to ±20 V. Connect +15 V to VDD, -15 V to VSS, and 0 V
to GND. The 5 V supply required by the AD4110-1 is generated on-board
by the ADP7102 LDO from the VDD supply.

On-board Connectors
~~~~~~~~~~~~~~~~~~~

========= =====================================================
Connector Function
========= =====================================================
J14       Power supply (VDD, VSS, GND)
J6        High voltage universal input
J10       Thermocouple input
J8        Low voltage inputs (ADC only)
J2        SPI PMOD (CS, DIN, DOUT, SCLK, GND, 5 V)
J12       External DVDD supply for IOVDD and AVDD5 (optional)
J1        120-pin connector for SDP-B controller board
========= =====================================================

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad4110/src/app>`__.

Basic example
~~~~~~~~~~~~~

This is the default example which initializes the AD4110-1 device via
SPI, configures the IRQ controller, sets up the voltage reference and
data format, and performs continuous data reading. The acquired samples
are printed to the console via UART.

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-AD4110-1SDZ <https://www.analog.com/EVAL-AD4110-1>`_
* `ZedBoard <https://www.xilinx.com/products/boards-and-kits/1-8dyf-11.html>`_

Connections
^^^^^^^^^^^

Connect the EVAL-AD4110-1SDZ to the ZedBoard via the SPI PMOD connector
(J2). The J2 pinout is: 1 = CS, 2 = DIN, 3 = DOUT, 4 = SCLK, 5 = GND,
6 = 5 V external power. Connect a ±15 V power supply to J14 (VDD, VSS,
GND).

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
