Evaluating the LT3074
======================


Contents
--------

.. contents:: Table of Contents
	:depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-LT3074 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-LT3074.html#eb-documentation>`_

Overview
--------

The EVAL-LT3074-AZ evaluation board features the LT3074, a 3A, ultralow noise,
high power-supply rejection ratio (PSRR), 45mV dropout ultrafast linear
regulator with PMBus capability. The evaluation board allows the LT3074 to be
configured without the use of external components.

Full performance details are provided in the LT3074 data sheet, which should
be consulted in conjunction with user guide.

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

An external power supply between 2.4V to 5.5V can be used for this project.

**Pin Description**

	J11:

	+-----+----------+-------------------------------------------+
	| Pin |   Name 	 | Description				     |
	+-----+----------+-------------------------------------------+
	| 1   | VDD3P3	 | Do Not Connect			     |
	+-----+----------+-------------------------------------------+
	| 2   | GND      | Connect to Ground			     |
	+-----+----------+-------------------------------------------+
	| 3   | SDA      | I2C Serial Data			     |
	+-----+----------+-------------------------------------------+
	| 4   | SCL 	 | I2C Serial Clock			     |
	+-----+----------+-------------------------------------------+
	| 5   | EN	 | Do Not Connect			     |
	+-----+----------+-------------------------------------------+
	| 6   | ALERT	 | Do Not Connect			     |
	+-----+----------+-------------------------------------------+

	Other connections:

	+-----+----------+-------------------------------------------+
	| Pin |   Name 	 | Description				     |
	+-----+----------+-------------------------------------------+
	| 1   | VI+	 | Power Supply, +2.4V - +5.5V		     |
	+-----+----------+-------------------------------------------+
	| 2   | GND      | Ground				     |
	+-----+----------+-------------------------------------------+
	| 3   | EXTVBIAS | Power Supply, +2.4V - +5.5V		     |
	+-----+----------+-------------------------------------------+
	| 4   | VOUT     | Connect to Load			     |
	+-----+----------+-------------------------------------------+
	| 5   | POWERGOOD| Do Not Connect			     |
	+-----+----------+-------------------------------------------+
	

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/lt3074/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/lt3074/src/platform>`_

Basic example
^^^^^^^^^^^^^

This is a simple example that initializes the LT3074, sets the output margin and
a warning limit, and performs telemetry.

In order to build the basic example:

.. code-block:: bash

	make EXAMPLE=basic

IIO example
^^^^^^^^^^^

This project is actually a IIOD demo for EVAL-LT3074 evaluation board.
The project launches a IIOD server on the board so that the user may connect
to it via an IIO client.
Using IIO-Oscilloscope, the user can configure the IMU and view the measured data on a plot.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The No-OS IIO Application together with the No-OS IIO LT3074 driver take care of
all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/lt3074/src/examples/iio_example>`_

In order to build the IIO project:

.. code-block:: bash

	make EXAMPLE=iio_example

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**

* `EVAL-LT3074 evaluation board <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-LT3074.html#eb-overview>`_
* `MAX32666EVKIT <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32666evkit.html>`_

**Connections**:

J11:

+-----------------------------+------------+----------------------------------------------+-----------------------------+
| EVAL-LT3074 Pin Number      |  Mnemonic  | Function					  | MAX32666EVKIT Pin Number	|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 2			      | GND	   | Ground					  | GND			        |
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 3			      | SDA	   | External Power Supply, 12VDC (5mA current)   | P0.15		        |
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 4			      | SCL	   | External Power Supply, 20.1VDC (5mA current) | P0.14			|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 5			      | EN	   | Enable pin					  | Do Not Connect		|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 6			      | ALERT	   | Fault alert pin				  | Do Not Connect		|
+-----------------------------+------------+----------------------------------------------+-----------------------------+

Other connection:

+-----------------------------+------------+----------------------------------------------+-----------------------------+
| EVAL-LT3074 Pin Number      |  Mnemonic  | Function					  | MAX32666EVKIT Pin Number	|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 5			      | POWERGOOD  | Power Good 				  | Do Not Connect		|
+-----------------------------+------------+----------------------------------------------+-----------------------------+

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset
	# to build the project
	make PLATFORM=maxim TARGET=max32665
	# to flash the code
	make run
