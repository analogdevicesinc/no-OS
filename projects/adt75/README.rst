ADT75 no-OS Example Project
===========================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
----------------------------

* `EVAL-ADT75 <https://www.analog.com/EVAL-ADT75>`_

Overview
--------

The EVAL-ADT75 evaluation board allows designers to easily evaluate all
features of the ADT75 temperature sensor. The ADT75 is a temperature
monitoring device in an 8-lead MSOP and SOIC package, featuring a
bandgap temperature sensor and a 12-bit ADC for digitizing temperature
with a 0.0625°C resolution. It is compatible with LM75 and AD7416
registers, operates on a 2.7V to 5.5V supply, and typically consumes
200µA at 3.3V. The operating temperature range is -55°C to +125°C,
with a shutdown mode reducing power consumption to 3µA. It includes
address selection pins (A0, A1, A2) and an OS/ALERT pin for over- or
under-temperature alerts, configurable in comparator or interrupt mode.

Applications
------------

* Isolated sensors
* Environmental control systems
* Computer thermal monitoring
* Thermal protection
* Industrial process control
* Power-system monitors
* Hand-held applications

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The ADT75 requires a power supply voltage ranging from 2.7V to 5.5V.
For this specific project, the EVAL-ADT75 is powered by the 3.3V supply
from the MAX32650EVKIT. It is recommended to use a 0.1µF ceramic capacitor
between VDD and GND for decoupling to minimize power supply noise and
ensure accurate readings.

Board Connector and Jumper Settings
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Pin Description**

Please see the following table for the pin assignments:

========  ==========================================
Name      Description
========  ==========================================
VDD       Connect to 3.3V or 5V supply
GND       Connect to Ground
SCL       Connect to I2C Clock (SCL)
SDA       Connect to I2C Data (SDA)
OS/ALERT  Overtemperature indicator (optional)
A0-A2     I2C address selection pins
========  ==========================================

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adt75/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adt75/src/platform>`_

Basic Example
^^^^^^^^^^^^^

This is a simple example that initializes the ADT75 sensor, reads
temperature data, and displays the readings continuously via UART.

In order to build the basic example make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adt75/Makefile>`_

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	BASIC_EXAMPLE = y
	IIO_EXAMPLE = n

IIO Example
^^^^^^^^^^^

This project is actually a IIOD demo for ADT75. The project launches a
IIOD server on the board so that the user may connect to it via an IIO client.

Using IIO-Oscilloscope, the user can configure the device.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The No-OS IIO Application together with the No-OS IIO ADT75 driver take care
of all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adt75/src/examples/iio_example>`_

In order to build the IIO project make sure you have the following configuration
in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adt75/Makefile>`_

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	BASIC_EXAMPLE = n
	IIO_EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used Hardware**

* `EVAL-ADT75 <https://www.analog.com/EVAL-ADT75>`_
* `MAX32650EVKIT <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32650evkit.html>`_

**Connections**:

================  ==================  ==================
EVAL-ADT75 Pin    Function            MAX32650EVKIT Pin
================  ==================  ==================
SDA (Pin 1)       I2C Data (SDA)      I2C1_SDA
SCL (Pin 2)       I2C Clock (SCL)     I2C1_SCL
GND (Pin 4)       Ground              GND
VDD (Pin 8)       Power Supply        3V3
================  ==================  ==================

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset PLATFORM=maxim TARGET=max32650
	# to build the project
	make PLATFORM=maxim TARGET=max32650
	# to flash the code
	make run PLATFORM=maxim TARGET=max32650
