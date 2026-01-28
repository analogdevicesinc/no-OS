LTC3337 no-OS Example Project
=============================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`DC2973A`

Overview
--------

The DC2973A demonstration circuit highlights the LTC3337’s capabilities
as a primary battery State of Health (SOH) monitor, featuring a
precision coulomb counter. It functions within a 1.8V to 5.5V input
voltage range to accurately measure battery health through Coulomb
counting and Battery Series Resistance (BSR) metrics. The board supports
configurable peak currents up to 100 mA, with a low quiescent current of
100 nA. Through its integrated GUI, users can monitor battery voltage,
track charge accumulation, and assess battery series resistance, making
it ideal for power-sensitive application development.

Applications
------------

- Low Power Primary Battery Powered Systems (e.g., 1× LiSOCl2, 2–3×
  Alkaline)
- Remote Industrial Sensors (e.g., Meters, Alarms)
- Asset Trackers
- Electronic Door Locks
- Keep-Alive Supplies/Battery Backup
- SmartMesh® Applications

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The DC2973A requires a power supply within a voltage range of 1.8V to
5.5V, supporting configurable peak currents up to 100 mA. It draws a low
quiescent current of 100 nA, making it suitable for battery-operated,
low-power applications. For optimal performance, the board should be
powered by a 5V supply with a minimum current capacity of 100 mA. This
configuration ensures efficient operation in various applications such
as remote sensors and asset trackers, particularly when utilizing the
demonstration circuit to showcase the LTC3337’s battery monitoring
capabilities.

Connections
~~~~~~~~~~~

================== ====== ========================================
Connector          Type   Description
J1                 USB    Micro USB 2.0, 5-pin, 1 port
J2                 Header 2x5, 1.27mm pitch, vertical, male header
J3                 Header 1x5, 2.54mm pitch, vertical, male header
JP1, JP2, JP3, JP4 Header 1x3, 2mm pitch, vertical, male header
================== ====== ========================================

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from: 
:git-no-OS:`Project Common Data Path </projects/ltc3337/src/common>`

The macros used in Common Data are defined in platform specific files
found in: :git-no-OS:`Project Platform Configuration Path </projects/ltc3337/src/platform>`

Basic example
~~~~~~~~~~~~~

The LTC3337 no-OS example project initializes the LTC3337
energy harvesting IC to monitor metrics like accumulated charge, input
and output voltages, and die temperature. It starts with the
``ltc3337_init`` function for device initialization, followed by a
continuous loop where metrics are fetched using specific functions:
``ltc3337_get_accumulated_charge`` for charge measurement,
``ltc3337_get_voltage_mv`` for voltage readings, and
``ltc3337_get_temperature_c`` for temperature monitoring. This process
logs the data for analysis, thereby illustrating the practical use of
the LTC3337 in monitoring and managing energy harvesting systems
effectively.

In order to build the basic example, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   BASIC_EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim
~~~~~

Hardware Used
^^^^^^^^^^^^^

- DC2973A Evaluation Board
- MAX32666FTHR

Connections
^^^^^^^^^^^

The DC2973A communicates via I²C/SMBus, so only SCL, SDA, and GND are
required for data communication. Use jumper wires to connect the
following pins:

+-----------------+-----------------+-----------------+-----------------+
| Signal /        | DC2973A (Header | MAX32666FTHR    | Notes           |
| Purpose         | J3)             | Pin             |                 |
+-----------------+-----------------+-----------------+-----------------+
| SCL (I²C Clock) | SCL             | P0_6 (I2CM0     | Connect SCL ↔   |
|                 |                 | SCL)            | SCL             |
+-----------------+-----------------+-----------------+-----------------+
| SDA (I²C Data)  | SDA             | P0_7 (I2CM0     | Connect SDA ↔   |
|                 |                 | SDA)            | SDA             |
+-----------------+-----------------+-----------------+-----------------+
| Ground          | GND             | Any GND pin on  | Common ground   |
| (GND)           |                 | Feather header  | required        |
+-----------------+-----------------+-----------------+-----------------+
| Power           | Battery input   | USB or Li+      | Boards powered  |
|                 | or board power  | battery (JST)   | separately;     |
|                 | pins            |                 | only GND shared |
+-----------------+-----------------+-----------------+-----------------+

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project for running basic example
   make EXAMPLE=basic PLATFORM=maxim TARGET=max32655
   # to flash the code
   make run
