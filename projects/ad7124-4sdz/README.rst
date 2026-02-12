AD7124-4SDZ no-OS Example Project
=================================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-AD7124-4SDZ`

Overview
--------

The EVAL-AD7124-4SDZ evaluation board is a platform for testing the
AD7124-4, a 4-channel, 24-bit sigma-delta ADC. It features low noise,
low power consumption, and includes an internal instrumentation
amplifier and reference. The board supports diverse input
configurations, suitable for high-precision measurements, and connects
to PCs via the Analog Devices System Demonstration Platform
(EVAL-SDP-CB1Z) for standalone data analysis. It requires a 7V to 9V
power supply and offers connectors for power and signals, allowing users
to connect sensors and conduct ADC diagnostics.

Applications
------------

- Temperature Measurement
- Pressure Measurement
- Industrial Process Control
- Instrumentation
- Smart Transmitters

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD7124-4SDZ evaluation board requires an external power supply
ranging from 7V to 9V. Power can be supplied via the J3 connector
using a bench top supply or the J5 connector with a DC plug adapter.

On-board Connectors
~~~~~~~~~~~~~~~~~~~

+-----------------------+-----------------------+-----------------------+
| **Connector**         | **Type / Location**   | **Function**          |
+-----------------------+-----------------------+-----------------------+
| J3                    | Barrel jack (bench    | 7V–9V input; selected |
|                       | top supply)           | via LK2 jumper        |
|                       |                       | (Position A)          |
+-----------------------+-----------------------+-----------------------+
| J5                    | Barrel jack (wall     | 7V–9V input; selected |
|                       | wart supply)          | via LK2 jumper        |
|                       |                       | (Position B)          |
+-----------------------+-----------------------+-----------------------+
| J6                    | Analog input header   | Connect analog        |
|                       |                       | sensors/signals       |
+-----------------------+-----------------------+-----------------------+
| J11                   | Analog input header   | Additional analog     |
|                       |                       | input options         |
+-----------------------+-----------------------+-----------------------+
| A0 / A1               | SMA/SMB footprints    | Differential analog   |
|                       |                       | input channels        |
+-----------------------+-----------------------+-----------------------+
| CLK IN                | SMB/SMA jack (not     | Optional external     |
|                       | inserted by default)  | master clock input    |
+-----------------------+-----------------------+-----------------------+
| Test Points           | SPI signal access     | CS, SCLK, DIN,        |
|                       |                       | DOUT/RDY — accessible |
|                       |                       | by removing R9–R13 0Ω |
|                       |                       | links                 |
+-----------------------+-----------------------+-----------------------+

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Source Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad7124-4sdz/src>`__.

Application example
~~~~~~~~~~~~~~~~~~~

In the example code for the AD7124-4SDZ no-OS project, the
operation begins with initializing the SPI interface specific to the
Xilinx platform. The AD7124 ADC device is then configured using the
``ad7124_setup()`` function, which prepares the ADC by writing the
necessary settings to its registers. The program proceeds by reading
each register to confirm these settings. Entering an operational loop,
the code waits for ADC conversions to become available with
``ad7124_wait_for_conv_ready()`` and reads the conversion result via
``ad7124_read_data()``. The obtained raw ADC data is continuously
printed to the console, demonstrating the integration process of the
AD7124 ADC within a no-OS environment.

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~~

Hardware Used
^^^^^^^^^^^^^

- :adi:`EVAL-AD7124-4SDZ`
- ZedBoard

Connections
^^^^^^^^^^^

+-----------------------+-----------------------+-----------------------+
| **EVAL-AD7124-4SDZ    | **ZedBoard Pin**      | **Function / Notes**  |
| Pin**                 |                       |                       |
+-----------------------+-----------------------+-----------------------+
| CS                    | SPI_CS                | Chip Select           |
+-----------------------+-----------------------+-----------------------+
| SCLK                  | SPI_CLK               | Serial Clock          |
+-----------------------+-----------------------+-----------------------+
| DIN                   | SPI_MOSI              | Data In (Master Out   |
|                       |                       | Slave In)             |
+-----------------------+-----------------------+-----------------------+
| DOUT/RDY              | SPI_MISO              | Data Out / Data Ready |
|                       |                       | (Master In Slave Out) |
+-----------------------+-----------------------+-----------------------+
| R9 to R13             | —                     | Remove 0Ω links to    |
|                       |                       | access SPI test       |
|                       |                       | points directly       |
+-----------------------+-----------------------+-----------------------+
| Test Points           | ZedBoard Expansion    | Wire SPI signals to   |
|                       |                       | Pmod or FMC header    |
|                       |                       | pins                  |
+-----------------------+-----------------------+-----------------------+

Consult the ZedBoard User Guide for Pmod or FMC header pinouts. The
ZedBoard provides several Pmod (2x6) headers and an FMC-LPC connector
suitable for digital I/O.

Double-check that all grounds are connected and consistent between the
two boards.

Verify the logic voltage levels are compatible. The AD7124-4 supports
digital I/O supply (IOVDD) between 1.65V and 3.6V.

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
