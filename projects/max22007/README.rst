MAX22007EVKIT no-OS Example Project
===================================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `MAX22007EVKIT <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max22007evkit.html>`_

Overview
--------

The MAX22007EVKIT is a fully featured evaluation kit for the MAX22007.
This board operates in standalone mode or in conjunction with SDP-K1

Hardware Specifications
-----------------------

The MAX22007EVKIT device has to be supplied with a 7-9V DC voltage and
a 3V3 VDRIVE voltage from the microcontroller (for the logic level
voltage of the SPI communication).

**Pin Description**

	Please see the following table for the pin assignments for the
	interface connector (SPI Test Points).

	+------------+--------+----------------------------------+
	| Pin        | Name   | Description                      |
	+------------+--------+----------------------------------+
	| PMOD1 pin1 | CSB    | SPI Chip-Select                  |
	+------------+--------+----------------------------------+
	| PMOD1 pin2 | SDO    | Master-Out Slave-In (MOSI)       |
	+------------+--------+----------------------------------+
	| PMOD1 pin3 | SDI    | Master-In Slave-Out (MISO)       |
	+------------+--------+----------------------------------+
	| PMOD1 pin4 | SCLK   | Serial Clock                     |
	+------------+--------+----------------------------------+

	Please ensure to open all the switches on SW1, to ensure compatibility 
	with an external MCU

	Please refer to the `MAX22007EVKIT Board user guide <https://www.analog.com/media/en/technical-documentation/data-sheets/max22007evkit.pdf>`_ for more details on jumper positions.

No-OS Build Setup
-----------------

`Please see: <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max22007/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max22007/src/platform>`_

Basic Example
^^^^^^^^^^^^^

This is a simple example which initializes the MAX22007 and writes to the data register of channel 0 and 3 with a value 
equivalent to 2v and 4v respectively.

In order to build the basic example make sure you have the following configuration in the Makefile
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max22007/Makefile>`_

.. code-block:: bash

	EXAMPLE ?= basic_example


No-OS Supported Platforms
-------------------------

STM32 Platform
^^^^^^^^^^^^^^

**Used Hardware**

* `MAX22007EVKIT <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max22007evkit.html>`_
* `SDP-K1 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/sdp-k1.html>`_

**Connections**:

+---------------------+--------+----------------------------------+--------------------+
| MAX22007EVKIT Pin   | Signal | Function                         | SDP-K1 Pin Number  |
+---------------------+--------+----------------------------------+--------------------+
| PMOD1 pin1          | CSB    | SPI interface Chip-Select        | D10                |
+---------------------+--------+----------------------------------+--------------------+
| PMOD1 pin2          | SDO    | Master-Out Slave-In (MOSI)       | D11                |
+---------------------+--------+----------------------------------+--------------------+
| PMOD1 pin3          | SDI    | Master-In Slave-Out (MISO)       | D12                |
+---------------------+--------+----------------------------------+--------------------+
| PMOD1 pin4          | SCLK   | SPI interface Serial Clock Line  | D13                |
+---------------------+--------+----------------------------------+--------------------+
| GND                 | GND    | Ground                           | GND                |
+---------------------+--------+----------------------------------+--------------------+

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset
	# to build the project
	make PLATFORM=stm32 HARDWARE=sdp-k1.ioc
	# to flash the code
	make run

