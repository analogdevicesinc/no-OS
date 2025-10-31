Evaluating the MAX17616
=======================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `MAX17616EVKIT# <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max17616evkit.html>`_
* `MAX17616AEVKIT# <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max17616aevkit.html>`_

Overview
--------

The MAX17616EVKIT# evaluation circuit features the MAX17616 eFuse setup for 5V
to 28V operation with a 7A auto-retry current limit. A 3.3V buck regulator
provides a voltage for pull-ups and logic to run indicator LEDs on the board.

While the MAX17616AEVKIT# evaluation kit (EV kit) features the MAX17616A eFuse
setup for 5V to 28V operation with a 2A auto-retry current limit. A 3.3V buck
regulator provides a voltage for pull-ups and logic to run indicator LEDs on the
board.

Both boards include an additional external N-channel FET for reverse conduction
protection.

Applications
------------

* Input Voltage and Output Overcurrent Protections
* Loss of Ground Protection
* Surge Protection

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

For this specific project an external power supply with output of +4V to +75V
can be used to power up the demo board.

**Pin Description**

	+----------+----------------------------+
	| Name	   | Description		|
	+----------+----------------------------+
	| VIN      | Power Supply, +4V - +75V	|
	+----------+----------------------------+
	| GND      | Connect to Ground		|
	+----------+----------------------------+
	| VOUT     | Connect to Load		|
	+----------+----------------------------+
	| IMON	   | Do Not Connect		|
	+----------+----------------------------+
	| SETI	   | Do Not Connect		|
	+----------+----------------------------+
	| SCL	   | I2C Serial Clock		|
	+----------+----------------------------+
	| SDA      | I2C Serial Data		|
	+----------+----------------------------+

**Hardware Bringup**

For reference, consult the Quick Start Procedure section in the user guide for the corresponding demo board:
`MAX17616EVKIT user guide <https://www.analog.com/media/en/technical-documentation/user-guides/max17616evkit.pdf>`_
`MAX17616AEVKIT user guide <https://www.analog.com/media/en/technical-documentation/user-guides/max17616aevkit.pdf>`_

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <GitHub link TBD>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <GitHub link TBD>`_

Basic example
^^^^^^^^^^^^^

This is a simple example that initializes the MAX17616, and performs telemetry
readings of the voltage, current and temperature. Status bytes/words are also
monitored in the example.

In order to build the basic example make sure you have the following
configuration in the Makefile
`Basic Example Makefile <GitHub link TBD>`_

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	BASIC_EXAMPLE = y
	IIO_EXAMPLE = n

IIO example
^^^^^^^^^^^

This project is actually a IIOD demo for MAX32655FTHR evaluation board.
The project launches a IIOD server on the board so that the user may connect
to it via an IIO client.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The No-OS IIO Application together with the No-OS IIO MAX17616 driver take care of
all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <GitHub Link TBD>`_

In order to build the IIO project make sure you have the following
configuration in the
`IIO Example Makefile <GitHub Link TBD>`_

.. code-block:: bash

        # Select the example you want to enable by choosing y for enabling and n for disabling
        BASIC_EXAMPLE = n
        IIO__EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**

* `MAX32655FTHR <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32655fthr.htm>`_

**Connections**:

+-----------------------------+------------+------------------------------------+----------+-----------------------------+
| MAX17616EVKIT# Pin Number   |  Mnemonic  | Function				| Mnemonic | MAX32655FTHR Pin Number	 |
+-----------------------------+------------+------------------------------------+----------+-----------------------------+
| 1			      | VEMI	   | External Power Supply, 4V - 75V	|	   | Do Not Connect		 |
+-----------------------------+------------+------------------------------------+----------+-----------------------------+
| 2			      | GND	   | Connect to Ground			|	   | GND			 |
+-----------------------------+------------+------------------------------------+----------+-----------------------------+
| 3			      | VOUT	   | May connect to Scopy/Load		|	   | Do Not Connect		 |
+-----------------------------+------------+------------------------------------+----------+-----------------------------+
| 10			      | SCL	   | I2C Serial Clock			| I2C2_SCL | P0_30			 |
+-----------------------------+------------+------------------------------------+----------+-----------------------------+
| 11			      | SDA	   | I2C Serial Data			| I2C2_SDA | P0_31			 |
+-----------------------------+------------+------------------------------------+----------+-----------------------------+

**Build Command**

.. code-block:: bash

	# to delete current build
	make TARGET=max32655 reset && make TARGET=max32655 clean
	# to build the project
	make TARGET=max32655
	# to flash the code
	make TARGET=max32655 run
