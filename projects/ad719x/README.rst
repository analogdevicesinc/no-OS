AD719X no-OS Example Project
============================

.. no-os-doxygen::
.. contents::
    :depth: 3
    
Supported Evaluation Boards
---------------------------

- :adi:`EVAL-AD7190ASDZ`

Overview
--------

The EVAL-AD7190ASDZ evaluation kit features the AD7190 which is 4.8 kHz
ultralow noise 24-bit sigma-delta (Σ-Δ) ADCs. The on-chip low noise gain
stage means that signals of small amplitude can interface directly to
the ADC. The internal clock option provides a compact solution for low
BW requirements.

Applications
-------------

- Weigh scales
- Strain gauge transducers
- Pressure measurement
- Temperature measurement
- Chromatography
- PLC/DCS analog input modules
- Data acquisition
- Medical and scientific instrumentation

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD7190ASDZ evaluation board, centered around the AD7190 24-bit
sigma-delta ADC, requires a stable power supply to maintain precision.
The ADC operates on an analog supply voltage (AVDD) between 4.75V and
5.25V, and a digital supply voltage (DVDD) ranging from 2.7V to 5.25V,
consuming about 6 mA.

Connectors
~~~~~~~~~~

+-----------------------+-----------------------+-----------------------+
| Connector Type        | Function/Connection   | Details               |
+-----------------------+-----------------------+-----------------------+
| 120-pin Connector     | Connection to         | Allows connection to  |
|                       | Controller Board      | EVAL-SDP-CKIZ /       |
|                       |                       | EVAL-SDP-CB1Z         |
+-----------------------+-----------------------+-----------------------+
| Arduino Connectors    | Alternative           | Provides a secondary  |
|                       | connection method     | connection option for |
|                       |                       | specific use cases    |
+-----------------------+-----------------------+-----------------------+
| AGND, AVDD, DGND,     | Power and Ground      | Ensures proper power  |
| REF(+)                | Connections           | supply and reference  |
|                       |                       | to the AD7190         |
+-----------------------+-----------------------+-----------------------+
| P0/REFIN2, P1/REFIN1  | Reference             | Used for external     |
|                       | Inputs/Outputs        | reference setup       |
+-----------------------+-----------------------+-----------------------+
| MUX                   | Signal Routing        | Directs signals to    |
|                       |                       | the ADC               |
+-----------------------+-----------------------+-----------------------+
| Digital Logic         | Includes Serial       | For control logic,    |
| Connections           | Interface             | facilitating data     |
|                       |                       | communication         |
+-----------------------+-----------------------+-----------------------+
| Temp Sensor           | Temperature           | Monitors board        |
|                       | Measurement           | temperature for       |
|                       |                       | operational stability |
+-----------------------+-----------------------+-----------------------+
| Clock Circuitry       | Timing Inputs         | Manages timing        |
|                       |                       | signals for ADC       |
|                       |                       | operations            |
+-----------------------+-----------------------+-----------------------+

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the 
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad719x/src/common>`__.

The macros used in Common Data are defined in platform specific files
found in: 
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad719x/src/platform>`__.

Dummy example
~~~~~~~~~~~~~~

The ``dummy example`` in the EVAL-AD7190ASDZ evaluation kit documentation
demonstrates the initial setup for the AD7190 ADC by providing essential
operations without application-specific extensions. It initializes the
AD7190, reads temperature data through the ``ad719x_temperature_read``
function, and continuously averages 100 samples from the ADC to
calculate a mean voltage. The temperature and average voltage are then
displayed to validate basic ADC functionality. Configuration is managed
through build files, specifically enabling the dummy example via flags
for hardware testing purposes.

In order to build the example, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

   DUMMY_EXAMPLE=y

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~~

Hardware Used
^^^^^^^^^^^^^

- EVAL-AD7190ASDZ
- ZedBoard

Connections
^^^^^^^^^^^

Connect the SPI lines of the EVAL-AD7190ASDZ to the ZedBoard’s PMOD
header using jumper wires while observing the following connections to
ensure signal integrity and voltage compatibility given the ZedBoard
operates at a 3.3V logic level.

+-----------------------+-----------------------+-----------------------+
| AD7190 Signal         | ZedBoard              | Notes / Requirements  |
+-----------------------+-----------------------+-----------------------+
| **CS**                | A GPIO or SPI_CSn     | Active low; drive it  |
|                       | line                  | low for the AD7190    |
|                       |                       | before transaction    |
+-----------------------+-----------------------+-----------------------+
| **SCLK**              | SPI clock line        | Make sure rise/fall   |
|                       |                       | times, duty cycle are |
|                       |                       | within spec           |
+-----------------------+-----------------------+-----------------------+
| **SDI / MOSI**        | SPI Master → AD7190   | Data from Zynq to     |
|                       | SDI                   | AD7190                |
+-----------------------+-----------------------+-----------------------+
| **SDO / MISO (or RDY  | AD7190 → SPI master / | Data from AD7190      |
| + data)**             | FPGA input            |                       |
+-----------------------+-----------------------+-----------------------+
| **DVDD / IOVDD / AVDD | Provide power and     | Match I/O voltages    |
| or GND**              | ground                | likely 3.3V; ensure   |
|                       |                       | stable, clean         |
|                       |                       | supplies              |
+-----------------------+-----------------------+-----------------------+
| **Reference inputs    | If external reference | May need wires to     |
| (REFIN)**             | used                  | your reference        |
|                       |                       | generator             |
+-----------------------+-----------------------+-----------------------+
| **DRDY / RDY pin**    | (Optional) interrupt  | Useful to know when   |
|                       | line to Zynq          | conversion is ready   |
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
