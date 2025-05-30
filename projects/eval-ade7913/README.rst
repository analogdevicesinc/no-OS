EVAL-ADE7913 no-OS Project
===========================

Contents
--------

.. contents:: Table of Contents
	:depth: 3

Supported Evaluation Boards
---------------------------

- `EVAL-ADE7913`

Overview
--------

The EVAL-ADE7913 provides a complete evaluation platform for the
ADE7913 isolated sigma-delta ADC, enabling the prototyping,
testing, and assessment of high-performance energy metering systems in
realistic three-phase meter scenarios. The platform integrates
hardware-specific configurations and interfaces—including SPI, UART,
GPIO, and NVIC interrupt management—with robust application logic to
demonstrate features such as dynamic RMS computation, burst mode read
operations, and synchronized multi-device operations.

Applications
------------

- Shunt-based polyphase meters
- Power quality monitoring
- Solar inverters
- Process monitoring
- Protective devices
- Isolated sensor interfaces
- Industrial PLCs

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

====================== ================
Parameter              Specification
Nominal Voltage        3.3 V
Tolerance              ±10%
Absolute Voltage Range 2.97 V to 3.63 V
====================== ================

================ ===================
Operating Mode   Current Consumption
Normal Operation 12.5 mA to 19 mA
Low Power Mode   2.7 mA to 3 mA
Deep Power-Down  ~50 µA
================ ===================

Pin Description
~~~~~~~~~~~~~~~

Phase Voltage Sense Inputs
^^^^^^^^^^^^^^^^^^^^^^^^^^

+-----------------+-----------------+-----------------+-----------------+
| Pin Number      | Pin Name        | Pin Function    | Description     |
+-----------------+-----------------+-----------------+-----------------+
| TP14/TP11       | V1PIN Phase A   | Phase Voltage   | Connect the     |
|                 |                 | Sense           | phase voltage   |
|                 |                 |                 | between TP14    |
|                 |                 |                 | and TP11. The   |
|                 |                 |                 | attenuated      |
|                 |                 |                 | signal is       |
|                 |                 |                 | achieved using  |
|                 |                 |                 | a resistor      |
|                 |                 |                 | divider network |
|                 |                 |                 | with an RC      |
|                 |                 |                 | antialiasing    |
|                 |                 |                 | filter.         |
+-----------------+-----------------+-----------------+-----------------+
| TP9/TP6         | V1PIN Phase B   | Phase Voltage   | Connect the     |
|                 |                 | Sense           | phase voltage   |
|                 |                 |                 | between TP9 and |
|                 |                 |                 | TP6.            |
+-----------------+-----------------+-----------------+-----------------+
| TP4/TP1         | V1PIN Phase C   | Phase Voltage   | Connect the     |
|                 |                 | Sense           | phase voltage   |
|                 |                 |                 | between TP4 and |
|                 |                 |                 | TP1.            |
+-----------------+-----------------+-----------------+-----------------+

Auxiliary Voltage Sense Inputs
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

+-----------------+-----------------+-----------------+-----------------+
| Pin Number      | Pin Name        | Pin Function    | Description     |
+-----------------+-----------------+-----------------+-----------------+
| TP15/TP11       | V2PIN Phase A   | Auxiliary       | Connect the     |
|                 |                 | Voltage Sense   | auxiliary       |
|                 |                 |                 | voltage between |
|                 |                 |                 | TP15 and TP11.  |
|                 |                 |                 | This path uses  |
|                 |                 |                 | a resistor      |
|                 |                 |                 | divider network |
|                 |                 |                 | and filtering   |
|                 |                 |                 | similar to the  |
|                 |                 |                 | phase voltage   |
|                 |                 |                 | inputs.         |
+-----------------+-----------------+-----------------+-----------------+
| TP10/TP6        | V2PIN Phase B   | Auxiliary       | Connect the     |
|                 |                 | Voltage Sense   | auxiliary       |
|                 |                 |                 | voltage between |
|                 |                 |                 | TP10 and TP6.   |
+-----------------+-----------------+-----------------+-----------------+
| TP5/TP1         | V2PIN Phase C   | Auxiliary       | Connect the     |
|                 |                 | Voltage Sense   | auxiliary       |
|                 |                 |                 | voltage between |
|                 |                 |                 | TP5 and TP1.    |
+-----------------+-----------------+-----------------+-----------------+

No-OS Build Setup
-----------------

Please see: :dokuwiki:`No-OS Build Setup </resources/no-os/build>`

No-OS Supported Examples
------------------------

The initialization data used in the example is sourced from the
:git-no-OS:`Project Common Data Path </projects/eval-ade7913/src/common>`. 
The macros used in Common Data are defined in platform-specific files found in:
:git-no-OS:`Project Platform Configuration Path </projects/eval-ade7913/src/platform>`.

ADE7913 Evaluation Example
--------------------------

The ADE7913 example project demonstrates how to initialize and
communicate with ADE7913 devices using no-OS drivers. It covers hardware
interface initialization, interrupt management, and data processing, and
it showcases key functionalities such as SPI and GPIO configuration.

In order to build the basic example make sure you have the following configuration 
in the :git-no-OS:`Makefile </projects/eval-ade7913/Makefile>`.  

.. code-block:: bash

   # Enable the ADE7913 evaluation example
   ADE7913_EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim Platform
~~~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

- :adi:`EVAL-ADE7913`
- :adi:`MAX32690EVKIT`

Connections
^^^^^^^^^^^

The table below summarizes the required physical connections between the
EVAL-ADE7913 board and the MAX32690EVKIT evaluation board for proper
operation of the ADE7913 example project.

============ ================================
EVAL-ADE7913 MAX32690EVKIT via Arduino Header
SCLK         SPI0 SCLK (P0.5)
SDI (MOSI)   SPI0 MOSI (P0.7)
SDO (MISO)   SPI0 MISO (P0.6)
CS           SPI0 CS (P0.4)
3.3V         3.3V Power
GND          GND
============ ================================

Build Command
^^^^^^^^^^^^^^

.. code-block:: bash

   # Build the ade7913_example project for MAX32690
   make ade7913_example

   # To remove previous build artifacts, run:
   make clean
