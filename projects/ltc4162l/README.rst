Evaluating the LTC4162L
=======================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------
* `DC2038A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc2038a.html>`_
* `DC2654A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc2654a.html>`_

Overview
--------

Demonstration circuit DC2038A shows the LTC4162 multicell step-down battery
charger with PowerPath™ and I2C telemetry operating with a configurable
chemistry and cell count. The LTC4162 supports up to eight cells for Li-Ion,
nine cells for LiFePO4, and 6V, 12V, 18V, or 24V lead-acid batteries.
Programmable and fully autonomous charge algorithms can be chosen for each of
the chemistries.

The DC2038A is populated with one of the six adjustable LTC4162 variants, each 
suited to a specific battery chemistry and default MPPT enabled state.
For more information, refer to the DC2038A demo manual.

Hardware Specifications
-----------------------

Power Supply Requirments
^^^^^^^^^^^^^^^^^^^^^^^^

For this specific project, the DC2038A is powered by the 3V3 supply from the
MAX32666FTHR.

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
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc4162l/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc4162l/src/platform>`_

Basic Example
^^^^^^^^^^^^^

This is a simple example that:

* initializes the LTC4162L
* gets the battery count
* gets the chemistry type
* reads battery voltage
* reads average battery voltage
* reads battery current

In order to build the basic example make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc4162l/Makefile>`_

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	BASIC_EXAMPLE = y
	IIO_EXAMPLE = n

IIO Example
^^^^^^^^^^^

This project is actually a IIOD demo for DC2038A. The project launches a IIOD
server on the board so that the user may connect to it via an IIO client.

Using IIO-Oscilloscope, the user can configure the device.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The No-OS IIO Application together with the No-OS IIO LTC4162L driver take care
of all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc4162l/src/examples/iio_example>`_

In order to build the IIO project make sure you have the following configuration
in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc4162l/Makefile>`_

.. code-block:: bash

        # Select the example you want to enable by choosing y for enabling and n for disabling
        BASIC_EXAMPLE = n
        IIO__EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**

* `DC2038A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc2038a.html>`_
* `MAX32666FTHR <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32666fthr.html>`_

**Connections**:

+-----------------------+-----------------------+------------------+
| LTC4162L Pin		| Function		| MAX32666FTHR Pin |
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