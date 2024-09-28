Evaluating the MAX31827
=======================

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `MAX31827EVKIT <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max31827evkit.html>`_

Overview
--------

The MAX31827 evaluation kit (EV kit) demonstrates the MAX31827 I2C temperature
switch and sensor with hardware-selectable address and alarm. The MAX31827 EV
kit comes with the 6-pin WLP MAX31827ANTABRPF+ installed.

Hardware Specifications
-----------------------

Power Supply Requirments
^^^^^^^^^^^^^^^^^^^^^^^^

For this specific project, the MAX31827EVKIT is powered by the 3V3 supply from
the MAX32666FTHR.

Board Connector and Jumper Settings
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Pin Description**

	Please see the following table for the pin assignments:

	+----------+-------------------------------------------+
	| Name     | Description			       |
	+----------+-------------------------------------------+
	| VDD      | Connect to 3V3 supply		       |
	+----------+-------------------------------------------+
	| GND      | Connect to Ground			       |
	+----------+-------------------------------------------+
	| SCL      | Connect to I2C Clock (SCL)		       |
	+----------+-------------------------------------------+
	| SDA      | Connect to I2C Data (SDA)		       |
	+----------+-------------------------------------------+

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max31827-evkit/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max31827-evkit/src/platform>`_

Basic example
^^^^^^^^^^^^^

This is a simple example that initializes the MAX31827 and reads input
temperature periodically.

In order to build the basic example make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max31827-evkit/Makefile>`_

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	BASIC_EXAMPLE = y
	IIO_EXAMPLE = n

IIO example
^^^^^^^^^^^

This project is actually a IIOD demo for MAX31827EVKIT. The project launches a
IIOD server on the board so that the user may connect to it via an IIO client.

Using IIO-Oscilloscope, the user can configure the device.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The No-OS IIO Application together with the No-OS IIO MAX31827 driver take care
of all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max31827-evkit/src/examples/iio_example>`_

In order to build the IIO project make sure you have the following configuration
in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max31827-evkit/Makefile>`_

.. code-block:: bash

        # Select the example you want to enable by choosing y for enabling and n for disabling
        BASIC_EXAMPLE = n
        IIO__EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**

* `MAX31827EVKIT <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max31827evkit.html>`_
* `MAX32666FTHR <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32666fthr.html>`_

**Connections**:

+-----------------------+-----------------------+------------------+
| MAX31827EVKIT Pin	| Function		| MAX32666FTHR Pin |
+-----------------------+-----------------------+------------------+
| VDD                   | VDD			| 3V3              |
+-----------------------+-----------------------+------------------+
| SCL                   | I2C Clock (SCL)	| P0_6 (I2C0_SCL)  |
+-----------------------+-----------------------+------------------+
| SDA                   | I2C Data (SDA)	| P0_7 (I2C0_SDA)  |
+-----------------------+-----------------------+------------------+
| GND                   | Ground (GND) 		| GND              |
+-----------------------+-----------------------+------------------+

**Build Command**

.. code-block:: bash

	# to delete current build
	make PLATFORM=maxim TARGET=max32665 reset
	# to build the project and flash the code
	make PLATFORM=maxim TARGET=max32665 run
