ADAQ7980_SDZ no-OS Example Project
==================================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-ADAQ7980SDZ`

Overview
--------

The EVAL-ADAQ7980SDZ evaluation board showcases the ADAQ7980, 
a low-power, 16-bit, 1 MSPS module designed for data acquisition systems.
It integrates key signal processing blocks into a compact SiP design,
facilitating various applications in data acquisition and signal
conditioning. The board offers a user-friendly interface for system
evaluation and can be modified to test the ADAQ7980 by adjusting the
sample rate, demonstrating flexibility in evaluating related components.

Applications
------------

- Automated test equipment (ATE)
- Battery powered instrumentation
- Communications
- Data acquisition
- Process control
- Medical instruments

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-ADAQ7980SDZ evaluation board requires a +9V DC power supply
provided in the kit. It employs multiple voltage regulators, such as the
ADP7118, ADP2370, and ADP7182, to convert this input voltage into
necessary supply voltages of +7.5V, +5.0V, and +3.3V. Integrated
reference circuitry includes the ADR4550 and ADR3433, providing stable
5V and 3.3V outputs. Power distribution on the board allows users to
choose between internal and external voltage references using jumpers
and links. Dynamic power scaling features are available to optimize
power consumption by adjusting the ADC driver and reference buffer.

Connections
~~~~~~~~~~~

+-----------------------------------+-----------------------------------+
| Connector                         | Description                       |
+-----------------------------------+-----------------------------------+
| P1, P2, P3, P4                    | SMA connectors for connecting a   |
|                                   | low-noise analog signal source.   |
+-----------------------------------+-----------------------------------+
| P6                                | Terminal block for alternative    |
|                                   | bench top power supply inputs.    |
+-----------------------------------+-----------------------------------+
| P8                                | 120-pin connector interfacing     |
|                                   | with controller board.            |
+-----------------------------------+-----------------------------------+
| J1, J2, J3                        | Links for ADC driver              |
|                                   | configuration, adjusting input    |
|                                   | circuit arrangements and filter   |
|                                   | settings.                         |
+-----------------------------------+-----------------------------------+
| J4, J5                            | Links for reference buffer and    |
|                                   | ADC driver power-down signals,    |
|                                   | allowing software controlled      |
|                                   | dynamic power scaling.            |
+-----------------------------------+-----------------------------------+
| J6, J7                            | Control links for LDO power-down  |
|                                   | signal and setting reference      |
|                                   | source input voltage.             |
+-----------------------------------+-----------------------------------+
| J9, J10                           | Settings for on-board and         |
|                                   | external reference voltages.      |
+-----------------------------------+-----------------------------------+
| J13, J14                          | Supply source settings for V_SDP  |
|                                   | and VDRIVE.                       |
+-----------------------------------+-----------------------------------+

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the 
:git-no-OS:`Project Source Path: </projects/adaq7980_sdz/src>`.

Demo example
~~~~~~~~~~~~

The example project initializes the ADAQ7980 device over the SPI
interface, configures the SPI clock, chip select, and conversion control
signals, and then performs continuous high-speed ADC conversions by
toggling the CNV signal and reading back 16-bit conversion results via
SPI. The raw ADC codes are printed to the serial console, allowing users
to verify correct operation and evaluate the full-scale and noise
performance of the ADAQ7980 at the configured sampling rate.

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~~

Hardware Used
^^^^^^^^^^^^^^

- EVAL-ADAQ7980SDZ
- ZedBoard

Connections
^^^^^^^^^^^

Manually wire the digital interface signals from the EVAL-ADAQ7980SDZ to
suitable ZedBoard pins (FMC or Pmod) that provide SPI functionality. All
digital logic levels are 3.3V. Do not exceed this voltage on any digital
line.

+-----------------------+-----------------------+-----------------------+
| EVAL-ADAQ7980SDZ      | ZedBoard Connector    | Description           |
|                       | (FMC or Pmod)         |                       |
+-----------------------+-----------------------+-----------------------+
| SCLK                  | SPI_CLK               | Serial clock          |
+-----------------------+-----------------------+-----------------------+
| SDI                   | SPI_MOSI              | Data in               |
+-----------------------+-----------------------+-----------------------+
| SDO                   | SPI_MISO              | Data out              |
+-----------------------+-----------------------+-----------------------+
| CNV                   | GPIO                  | Conversion start      |
+-----------------------+-----------------------+-----------------------+
| VIO                   | 3.3V supply pin       | IO voltage            |
+-----------------------+-----------------------+-----------------------+
| GND                   | GND                   | Ground                |
+-----------------------+-----------------------+-----------------------+

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
