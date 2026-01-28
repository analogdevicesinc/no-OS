MAX22190 no-OS Example Project
==============================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`MAX22190EVKIT`

Overview
--------

The MAX22190 Evaluation Kit facilitates testing of the MAX22190 Octal
Industrial Digital Input device. Equipped with a Pmod™ compatible
connector and a USB2PMB2# adapter board, it interfaces with a PC via USB
for SPI communication. The kit supports Single-Channel, Independent
Subordinate, and Daisy-Chain modes, offering flexibility in evaluating
the device’s features. It includes two diagnostic-enabled MAX22190
devices (U1 and U2) and uses USB power with galvanic isolation for
safety, adhering to IEC 61131-2 Type 1, 2, and 3 standards.

Applications
------------

- Programmable Logic Controllers
- Industrial Automation
- Process Automation
- Building Automation

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The MAX22190 Evaluation Kit (MAX22190EVKIT) is powered by either an
external 24V supply or a 3.0V to 5.5V external supply connected
through the VDD pin. It features the USB2PMB2# adapter board, which
converts USB 5V to a regulated +3.3V supply for the kit's logic
components. Independent isolated power supplies test galvanic isolation,
and digital isolators such as the MAX14483 and MAX12931 provide up to
5.5V isolation to ensure compliance with IEC 61131-2 standards.

On-board Connectors
~~~~~~~~~~~~~~~~~~~

+-----------------------------------+-----------------------------------+
| Connector Name                    | Description                       |
+-----------------------------------+-----------------------------------+
| Pmod™ Connector                   | Used for SPI communication.       |
|                                   | Allows connection between the     |
|                                   | MAX22190 EV kit and the USB2PMB2# |
|                                   | adapter for SPI interface.        |
+-----------------------------------+-----------------------------------+
| USB2PMB2# Adapter Board           | Interfaces the EV kit with a PC   |
|                                   | through USB for communication,    |
|                                   | providing power and SPI interface |
|                                   | semiconductors.                   |
+-----------------------------------+-----------------------------------+
| Micro-USB Cable                   | Connects the USB2PMB2# adapter to |
|                                   | a PC, enabling USB-powered        |
|                                   | communication.                    |
+-----------------------------------+-----------------------------------+
| Terminal Blocks T1 and T2         | Connect eight Type 1 or 3 digital |
|                                   | inputs for U1 device              |
|                                   | configuration.                    |
+-----------------------------------+-----------------------------------+
| Terminal Blocks T3 and T4         | Connect four Type 2 digital       |
|                                   | inputs for U2 device              |
|                                   | configuration.                    |
+-----------------------------------+-----------------------------------+
| Connector X2                      | Used for probing digital signals  |
|                                   | on the field side of the          |
|                                   | isolation barrier.                |
+-----------------------------------+-----------------------------------+

Digital Communication Pins
~~~~~~~~~~~~~~~~~~~~~~~~~~

+-----------------------------------+-----------------------------------+
| Pin Name                          | Function                          |
+-----------------------------------+-----------------------------------+
| CS                                | Chip-Select Input for SPI         |
|                                   | interface.                        |
+-----------------------------------+-----------------------------------+
| SCLK                              | Serial Clock Input for SPI        |
|                                   | communication.                    |
+-----------------------------------+-----------------------------------+
| SDI                               | Serial Data Input for SPI, data   |
|                                   | is clocked in on SCLK’s rising    |
|                                   | edge.                             |
+-----------------------------------+-----------------------------------+
| SDO                               | Serial Data Output for SPI, data  |
|                                   | is updated on SCLK’s falling      |
|                                   | edge.                             |
+-----------------------------------+-----------------------------------+
| LATCH                             | Controls data latch at input      |
|                                   | serializer; transparent when CS   |
|                                   | and LATCH are high.               |
+-----------------------------------+-----------------------------------+
| FAULT                             | Active-Low Fault Indicator        |
|                                   | output, indicates fault status.   |
+-----------------------------------+-----------------------------------+
| READY                             | Indicates readiness by pulling    |
|                                   | low when MAX22190 is powered.     |
+-----------------------------------+-----------------------------------+


No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
:git-no-OS:`Project Common Data Path </projects/max22190/src/common>`.

The macros used in Common Data are defined in platform specific files
found in: 
:git-no-OS:`Project Platform Configuration Path </projects/max22190/src/platform>`.

Basic example
~~~~~~~~~~~~~

The basic example code for the MAX22190 no-OS project initializes the
MAX22190 device and iterates through its channels to detect and log
digital inputs. It allows configuration of channel delays, management of
channel states, and demonstrates the onboard diagnostics capabilities by
enabling settings such as fault and wire break detection. This example
ensures developers can observe the operations by providing feedback
throughout the process, essential for verifying behaviors in industrial
applications.

In order to build the basic example, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   BASIC_EXAMPLE = y
   IIO_EXAMPLE = n

IIO example
~~~~~~~~~~~

The IIO example code for the MAX22190 no-OS project operates by
utilizing the Industrial I/O (IIO) library to manage data channels of
the MAX22190 Octal Industrial Digital Input device. The code establishes 
communication between the device and IIO clients over SPI and UART interfaces. 
It focuses on real-time data exchange through the IIO application and
max22190-specific IIO descriptors, providing functionalities for device
configuration, data diagnostics, and manipulation, which are vital for
industrial automation processes.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

In order to build the IIO project, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

   # Configure the example you want to enable by setting 'y' for enable and 'n' for disable
   BASIC_EXAMPLE = n
   IIO_EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim
~~~~~

Hardware Used
^^^^^^^^^^^^^

- MAX22190 EV Kit
- AD-APARD32690-SL (MAX32690)

Connections
^^^^^^^^^^^

+-----------------+-----------------+------------------+-----------------+
| Function /      | MAX22190EV Kit  | AD-APARD32690-SL | Notes           |
| Signal          | Pin             | Pin (Arduino     |                 |
|                 |                 | Mega Header)     |                 |
+-----------------+-----------------+------------------+-----------------+
| **MOSI**        | DIN             | D51 (MOSI)       | MCU sends data  |
|                 |                 |                  | to MAX22190     |
+-----------------+-----------------+------------------+-----------------+
| **MISO**        | DOUT            | D50 (MISO)       | MAX22190 sends  |
|                 |                 |                  | data back to    |
|                 |                 |                  | MCU             |
+-----------------+-----------------+------------------+-----------------+
| **SCLK**        | SCLK            | D52 (SCK)        | Clock provided  |
|                 |                 |                  | by MCU          |
+-----------------+-----------------+------------------+-----------------+
| **CS**          | CS              | D53 (SS)         | Active low,     |
|                 |                 |                  | selects         |
|                 |                 |                  | MAX22190        |
+-----------------+-----------------+------------------+-----------------+
| **IRQ           | IRQ             | Any GPIO (e.g.,  | Signals events  |
| (Interrupt)**   |                 | D2)              | from MAX22190   |
|                 |                 |                  | to MCU          |
+-----------------+-----------------+------------------+-----------------+
| **GND**         | GND             | GND              | Common ground   |
|                 |                 |                  | reference       |
+-----------------+-----------------+------------------+-----------------+
| **VCC (3.3V)**  | VCC             | 3.3V             | Shared power    |
|                 |                 |                  | supply          |
+-----------------+-----------------+------------------+-----------------+

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   cp <SOME_PATH>/system_top.xsa .
   # to delete current build
   make reset
   # to build the project, uncomment the selected target
   make PLATFORM=maxim # TARGET=max32690 or # TARGET=max32665 run
   # to flash the code
   make run
