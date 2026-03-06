AD7124-4SDZ no-OS Example Project
=================================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-AD7124-4SDZ <https://www.analog.com/EVAL-AD7124-4SDZ>`_

Overview
--------

The `EVAL-AD7124-4SDZ <https://www.analog.com/EVAL-AD7124-4SDZ>`_
evaluation board is a platform for evaluating the
`AD7124-4 <https://www.analog.com/AD7124-4>`_, a low power, low noise,
completely integrated analog front end for high precision measurement
applications. The AD7124-4 provides 4 differential or 7 pseudo
differential input channels, a 24-bit sigma-delta ADC, an on-chip 2.5 V
reference, and internal oscillator. The device also integrates a
programmable gain amplifier (PGA) and excitation current sources. The
board supports diverse input configurations, suitable for high-precision
measurements. It requires a 7V to 9V external power supply and offers
connectors for power and analog signals, allowing users to connect
sensors and conduct ADC diagnostics.

Applications
------------

* Temperature Measurement
* Pressure Measurement
* Industrial Process Control
* Instrumentation
* Smart Transmitters

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The EVAL-AD7124-4SDZ evaluation board requires an external power supply
ranging from 7V to 9V. Power can be supplied via the J3 connector
using a bench top supply or the J5 connector with a DC plug adapter.

Board Connector and Jumper Settings
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**On-board Connectors**

	Please see the following table for the connector descriptions:

	==============  ====================================================================
	Connector       Description
	==============  ====================================================================
	J3              Barrel jack (bench top) 7 V to 9 V input; LK2 Position A
	J5              Barrel jack (wall wart) 7 V to 9 V input; LK2 Position B
	J6              Analog input header for sensors/signals
	J11             Additional analog input header
	A0 / A1         SMA/SMB footprints for differential analog input channels
	CLK IN          SMB/SMA jack for optional external master clock input
	Test Points     SPI signal access (CS, SCLK, DIN, DOUT/RDY); remove R9-R13
	==============  ====================================================================

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Source Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad7124-4sdz/src>`_

Application Example
^^^^^^^^^^^^^^^^^^^

This is a simple example that initializes the AD7124-4 ADC via
``ad7124_setup()`` and reads all device registers to confirm the
configuration. It then enters a continuous loop where it waits for
ADC conversions to complete using ``ad7124_wait_for_conv_ready()``
and reads the conversion result via ``ad7124_read_data()``. The
obtained raw ADC data is printed to the console.

No-OS Supported Platforms
-------------------------

Xilinx Platform
^^^^^^^^^^^^^^^

**Used Hardware**

* `EVAL-AD7124-4SDZ <https://www.analog.com/EVAL-AD7124-4SDZ>`_
* `ZedBoard <https://www.amd.com/en/products/adaptive-socs-and-fpgas/evaluation-boards/zedboard-zynq-7000.html>`_

**Connections**

The EVAL-AD7124-4SDZ connects to the ZedBoard via the SPI test points
on the evaluation board. Remove the 0 Ohm links R9 to R13 to access the
SPI signals directly.

======================  ====================  ==============================
EVAL-AD7124-4SDZ Pin    ZedBoard Pin          Function
======================  ====================  ==============================
CS                      SPI_CS                Chip Select
SCLK                    SPI_CLK               Serial Clock
DIN                     SPI_MOSI              Data In (Master Out Slave In)
DOUT/RDY                SPI_MISO              Data Out / Data Ready
GND                     GND                   Ground
======================  ====================  ==============================

Consult the ZedBoard User Guide for Pmod or FMC header pinouts. Verify
that the logic voltage levels are compatible; the AD7124-4 supports a
digital I/O supply (IOVDD) between 1.65V and 3.6V.

**Build Command**

.. code-block:: bash

	cp <SOME_PATH>/system_top.xsa .
	# to delete current build
	make reset
	# to build the project
	make
	# to flash the code
	make run
