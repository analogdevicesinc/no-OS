AD5766-SDZ no-OS Example Project
================================

.. no-os-doxygen::
.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-AD5766SDZ`

Overview
--------

The EVAL-AD5766SD2Z and EVAL-AD5767SD2Z are evaluation boards designed
for the assessment of AD5766 and AD5767 DACs respectively. Both boards
feature a 16-channel denseDAC® with 16-bit/12-bit resolution and voltage
output. They include the ADP5071 regulator which converts a 3.3V input
to a dual power supply, facilitating a DAC output range of -20V to +6V.
Users can utilize an on-board connector or the EVAL-SDP-CB1Z platform
for DAC control, interfacing via ACE software for device configuration
and data management.

Applications
------------

- Mach Zehnder modulator bias control
- Optical networking
- Instrumentation
- Industrial automation
- Data acquisition systems
- Analog output modules

Hardware Specifications
-----------------------

The EVAL-AD5766SDZ evaluation board utilizes an on-board ADP5071
switching regulator to generate the necessary 8V and -22V supplies from
a single 3.3V input. Using jumper settings, the board can be configured
to derive power for both AVCC and VLOGIC from a single supply or
separate supplies, providing operational flexibility. Alternatively,
regulated bench supplies can power the board. Care must be taken to
ensure that the voltage between AVDD and AVSS does not exceed 34V, to
avoid device failure.

========= ========================================
Connector Function
J1        Connection to EVAL-SDP-CB1Z board
J2        Header pins for voltage outputs and AGND
J3        External supply for AvDD and Avcc
J4        Peripheral Module (PMOD) connection pins
J5        External supply for Vcc pin
J6        3.3 V supply for Avcc, Vo, and ADP5071
J7        External supply for Vout pin
J10       PMOD connector for SPI communication
========= ========================================

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the example project is taken out from
the `Project Source Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad5766-sdz/src>`__

Application example
~~~~~~~~~~~~~~~~~~~

The example code initializes and configures the AD5766 DAC using SPI and
GPIO interfaces on a Xilinx platform. It enables instruction and data
caches, essential for performance, and sets the SPI engine to a 50 MHz
clock frequency. The DAC’s initialization covers power settings, voltage
span, and specifically disables the daisy chain mode. It utilizes DMA
for efficient data transfer from a sine wave lookup table, reducing CPU
load while allowing smooth waveform output. This setup ensures precise
timing and synchronization, optimizing DAC performance on the hardware
platform.

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Hardware Used
^^^^^^^^^^^^^

- EVAL-AD5766SD2Z
- ZedBoard

Connections
^^^^^^^^^^^

Use a 12-pin (2x6) PMOD cable to connect EVAL-AD5766SD2Z (via J10) to a
PMOD header on the Zedboard. Utilize the following pin mapping:

=========================== ========= =================
EVAL-AD5766SD2Z J10 Pin     Signal    Zedboard PMOD Pin
1                           SYNC      D0
2                           SDIN/MOSI D1
3                           SDO/MISO  D2
4                           SCLK      D3
5                           DGND      GND
6                           VLOGIC    VCC (3.3V)
8                           RESET     D4
11                          DGND      GND
12                          VLOGIC    VCC (3.3V)
7, 9, 10                    NC        Not Connected
=========================== ========= =================

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
   