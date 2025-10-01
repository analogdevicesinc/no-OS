AD719X no-OS Example Project
============================

.. no-os-doxygen::

.. contents:: Table of Contents
	:depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-AD7190ASDZ <https://www.analog.com/EVAL-AD7190ASDZ>`_

Overview
--------

The EVAL-AD7190ASDZ evaluation kit features the AD7190, a 4.8 kHz
ultralow noise 24-bit sigma-delta ADC. The on-chip low noise gain
stage means that signals of small amplitude can interface directly to
the ADC. The internal clock option provides a compact solution for low
bandwidth requirements.

Applications
------------

* Weigh scales
* Strain gauge transducers
* Pressure measurement
* Temperature measurement
* Chromatography
* PLC/DCS analog input modules
* Data acquisition
* Medical and scientific instrumentation

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

============================  =====================================================
Connector                     Description
============================  =====================================================
120-pin Connector              Connection to EVAL-SDP-CK1Z / EVAL-SDP-CB1Z
Arduino Connectors             Alternative connection method for external boards
AGND, AVDD, DGND, REF(+)      Power supply and reference connections to the AD7190
P0/REFIN2, P1/REFIN1           External reference input/output connections
MUX                            Signal routing to the ADC
Digital Logic                  Serial interface for control and data communication
Temp Sensor                    On-board temperature monitoring
Clock Circuitry                Timing signal inputs for ADC operations
============================  =====================================================

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad719x/src/common>`_

The macros used in Common Data are defined in platform specific files
found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad719x/src/platform>`_

Dummy Example
~~~~~~~~~~~~~

This is a simple example that initializes the AD7190, reads the
temperature through the ``ad719x_temperature_read`` function, and
continuously averages 100 samples from the ADC to calculate a mean
voltage. The temperature and average voltage are then displayed to
validate basic ADC functionality.

In order to build the dummy example, add the following flags to the
make command:

.. code-block:: bash

	DUMMY_EXAMPLE=y

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

**Used Hardware**

* `EVAL-AD7190ASDZ <https://www.analog.com/EVAL-AD7190ASDZ>`_
* `ZedBoard <https://www.xilinx.com/products/boards-and-kits/1-8dyf-11.html>`_

**Connections**

Connect the SPI lines of the EVAL-AD7190ASDZ to the ZedBoard's PMOD
header using jumper wires. The ZedBoard operates at a 3.3V logic level.

+-----------------------+-----------------------+-----------------------+
| AD7190 Signal         | ZedBoard              | Notes / Requirements  |
+-----------------------+-----------------------+-----------------------+
| CS                    | A GPIO or SPI_CSn     | Active low; drive low |
|                       | line                  | before transaction    |
+-----------------------+-----------------------+-----------------------+
| SCLK                  | SPI clock line        | Ensure rise/fall      |
|                       |                       | times are within spec |
+-----------------------+-----------------------+-----------------------+
| SDI / MOSI            | SPI Master to AD7190  | Data from Zynq to     |
|                       | SDI                   | AD7190                |
+-----------------------+-----------------------+-----------------------+
| SDO / MISO            | AD7190 to SPI master  | Data from AD7190      |
+-----------------------+-----------------------+-----------------------+
| DVDD / AVDD           | Provide power and     | AVDD: 4.75V to 5.25V  |
|                       | ground                | DVDD: 2.7V to 5.25V   |
+-----------------------+-----------------------+-----------------------+
| GND                   | Ground                | Connect to board GND  |
+-----------------------+-----------------------+-----------------------+

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset
	# to build the project
	make
	# to flash the code
	make run
