ADT7420-PMDZ no-OS Example Project
==================================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-ADT7420-PMDZ <https://www.analog.com/EVAL-ADT7420-PMDZ>`_

Overview
--------

The `EVAL-ADT7420-PMDZ <https://www.analog.com/EVAL-ADT7420-PMDZ>`_ is a PMOD
evaluation board for the ADT7420, a high accuracy digital temperature sensor.
The ADT7420 provides 16-bit resolution (0.0078 degrees C) and communicates via
an I2C interface. Its factory-set I2C address is 0x48, with options for
modification via solder link configurations on pins A0 and A1, allowing up to
four unique addresses. The ADT7420 operates over a supply voltage range of
2.7 V to 5.5 V with a typical supply current of 210 uA at 3.3 V. The device
features programmable INT and CT open-drain outputs for over-temperature and
critical temperature alert notifications.

Applications
------------

* RTD and thermistor replacement
* Thermocouple cold junction compensation
* Medical equipment
* Industrial control and test
* Food transportation and storage
* Environmental monitoring and HVAC
* Laser diode temperature control

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-ADT7420-PMDZ is powered via the PMOD connector from the host board.
The host board must provide a 3.3 V supply. The ADT7420 draws a typical supply
current of 210 uA in normal operation mode. An onboard LED near component R5
indicates when power is applied.

Digital Interface
~~~~~~~~~~~~~~~~~

The EVAL-ADT7420-PMDZ uses an extended I2C PMOD interface, providing SCL, SDA,
VDD, and GND connections through the standard 2x4 PMOD header (P1).

P1 Pin Configuration
^^^^^^^^^^^^^^^^^^^^^

========  ==============  =========
Pin No.   Pin Function    Mnemonic
========  ==============  =========
1         Serial Clock    SCL
2         Serial Clock    SCL
3         Serial Data     SDA
4         Serial Data     SDA
5         Digital Ground  DGND
6         Digital Ground  DGND
7         Digital Power   VDD
8         Digital Power   VDD
========  ==============  =========

Jumper Configuration
~~~~~~~~~~~~~~~~~~~~

The EVAL-ADT7420-PMDZ provides I2C address selection through solder links JP1
and JP2.

=============  =============  ========  ========  ===========
JP1 Position   JP2 Position   A1 Logic  A0 Logic  I2C Address
=============  =============  ========  ========  ===========
2 & 3 Short    2 & 3 Short    Low       Low       0x48
2 & 3 Short    1 & 2 Short    Low       High      0x49
1 & 2 Short    2 & 3 Short    High      Low       0x4A
1 & 2 Short    1 & 2 Short    High      High      0x4B
=============  =============  ========  ========  ===========

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adt7420-pmdz/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adt7420-pmdz/src/platform>`_

Dummy example
~~~~~~~~~~~~~

This is a simple example that initializes the ADT7420 temperature sensor via
I2C, reads the high and low temperature limit registers, calculates the current
temperature using ``adt7420_get_temperature()``, and configures the hysteresis
register. The example runs in a continuous loop, periodically printing the
current temperature and threshold values.

In order to build the dummy example, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adt7420-pmdz/Makefile>`_

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	DUMMY_EXAMPLE = y
	IIO_EXAMPLE = n

IIO example
~~~~~~~~~~~

This project is actually a IIOD demo for EVAL-ADT7420-PMDZ. The project
launches a IIOD server on the board so that the user may connect to it via an
IIO client.

Using IIO-Oscilloscope, the user can configure the device.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The No-OS IIO Application together with the No-OS IIO ADT7420 driver take care
of all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adt7420-pmdz/src/examples/iio_example>`_

In order to build the IIO project make sure you have the following configuration
in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adt7420-pmdz/Makefile>`_

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	DUMMY_EXAMPLE = n
	IIO_EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim Platform
~~~~~~~~~~~~~~

**Used hardware**

* `EVAL-ADT7420-PMDZ <https://www.analog.com/EVAL-ADT7420-PMDZ>`_
* Maxim platform board (MAX32650, MAX32655, MAX32660, MAX32665, MAX78000, or
  MAX32690)

**Connections**

Connect the EVAL-ADT7420-PMDZ PMOD connector to the I2C PMOD header on the
Maxim platform board. The I2C SDA, SCL, VDD (3.3 V), and GND signals are routed
through the PMOD connector.

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset
	# to build the project
	make PLATFORM=maxim TARGET=max32690
	# to flash the code
	make run

Mbed Platform
~~~~~~~~~~~~~

**Used hardware**

* `EVAL-ADT7420-PMDZ <https://www.analog.com/EVAL-ADT7420-PMDZ>`_
* Arduino UNO compatible board

**Connections**

=====================  ===============  ==================================
EVAL-ADT7420-PMDZ Pin  Arduino UNO Pin  Description
=====================  ===============  ==================================
VDD                    5V               Power supply for the sensor
GND                    GND              Common ground connection
SCL                    A5               I2C clock line
SDA                    A4               I2C data line
=====================  ===============  ==================================

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset
	# to build the project
	make PLATFORM=mbed
	# to flash the code
	make run
