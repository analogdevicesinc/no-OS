Evaluating the LTM4700
======================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
----------------------------

* `DC2702B-A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc2702b-a.html>`_

Overview
--------

The DC2702B-A is a dual-output, high efficiency, high density µModule® regulator
demonstration circuit featuring the LTM4700EY. The board accepts a 4.5V to 16V
input and each output can supply up to 50A maximum load current.

The DC2702B-A powers up to default settings and produces power based on
configuration resistors without the need for any serial bus communication,
allowing easy evaluation of the DC/DC converter. The board provides two default
output voltages: VOUT0 = 1.0V and VOUT1 = 1.5V.

Applications
------------

* Point-of-load power supplies requiring dual 50A or single 100A outputs
* High current density digital power management
* Systems requiring PMBus-based digital power system management
* Telemetry monitoring of voltage, current, temperature, and fault status

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The DC2702B-A requires an input supply of 4.5V to 16V connected to VIN and GND.

**Performance Summary** (TA = 25°C)

+----------------------------------+-----+------+-----+-------+
| Parameter                        | Min | Typ  | Max | Units |
+==================================+=====+======+=====+=======+
| Input Voltage Range              | 4.5 |      | 16  | V     |
+----------------------------------+-----+------+-----+-------+
| Output Voltage, VOUT0            | 0.5 | 1.0  | 1.8 | V     |
+----------------------------------+-----+------+-----+-------+
| Maximum Output Current, IOUT0    |     | 50   |     | A     |
+----------------------------------+-----+------+-----+-------+
| Output Voltage, VOUT1            | 0.5 | 1.5  | 1.8 | V     |
+----------------------------------+-----+------+-----+-------+
| Maximum Output Current, IOUT1    |     | 50   |     | A     |
+----------------------------------+-----+------+-----+-------+
| Default Switching Frequency      |     | 500  |     | kHz   |
+----------------------------------+-----+------+-----+-------+

**Default Jumper Settings**

+------+----------+----------------------------------------------------------+
| JP   | Position | Description                                              |
+======+==========+==========================================================+
| JP1  | ON       | Default configuration                                    |
+------+----------+----------------------------------------------------------+
| JP2  | ON       | Default configuration                                    |
+------+----------+----------------------------------------------------------+
| JP3  | ON       | Enables internal bias circuit when VIN > 7V              |
+------+----------+----------------------------------------------------------+

**Hardware Bringup**

For reference, consult the Quick Start Procedure section in the DC2702B-A demo
manual:
`DC2702B-A Demo Manual <https://www.analog.com/media/en/technical-documentation/user-guides/dc2702b-a.pdf>`_

1. With the power off, connect the input power supply to VIN (4.5V to 16V) and
   GND.
2. Connect the 1.0V output load between VOUT0 and GND (initial load: no load).
3. Connect the 1.5V output load between VOUT1 and GND (initial load: no load).
4. Set JP1, JP2, and JP3 to ON.
5. Turn on the input power supply and verify VOUT0 = 1.0V ±0.5% and
   VOUT1 = 1.5V ±0.5%.
6. Adjust loads within operating range and observe output voltage regulation,
   ripple voltage, and other parameters.

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
-------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltm4700/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltm4700/src/platform/maxim>`_

Basic example
^^^^^^^^^^^^^

This is a simple example that initializes the LTM4700, and performs telemetry
readings of voltage, current, and temperature for both output channels. The
readings are printed to the UART console at a 500ms interval.

In order to build the basic example make sure you have the following
configuration in the Makefile
`Basic Example Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltm4700/Makefile>`_

.. code-block:: bash

	EXAMPLE ?= basic

IIO example
^^^^^^^^^^^

This project is a IIOD demo for the MAX32655FTHR evaluation board. The project
launches a IIOD server on the board so that the user may connect to it via an
IIO client.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The No-OS IIO Application together with the No-OS IIO LTM4700 driver take care of
all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltm4700/src/examples/iio_example>`_

In order to build the IIO project make sure you have the following configuration
in the
`IIO Example Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltm4700/Makefile>`_

.. code-block:: bash

	EXAMPLE ?= iio_example

No-OS Supported Platforms
--------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**

* `DC2702B-A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc2702b-a.html>`_
* `MAX32655FTHR <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32655fthr.htm>`_

**Connections**:

+---------------------------+------------+-------------------------------+----------+-------------------------+
| DC2702B-A Pin             |  Mnemonic  | Function                      | Mnemonic | MAX32655FTHR Pin        |
+===========================+============+===============================+==========+=========================+
| J1-1 (SDA)                | SDA        | PMBus/I2C Serial Data         | I2C1_SDA | P0.6                    |
+---------------------------+------------+-------------------------------+----------+-------------------------+
| J1-2 (SCL)                | SCL        | PMBus/I2C Serial Clock        | I2C1_SCL | P0.7                    |
+---------------------------+------------+-------------------------------+----------+-------------------------+
| GND                       | GND        | Connect to Ground             |          | GND                     |
+---------------------------+------------+-------------------------------+----------+-------------------------+
| VIN                       | VIN        | Input Power Supply 4.5V–16V   |          | Do Not Connect          |
+---------------------------+------------+-------------------------------+----------+-------------------------+
| VOUT0                     | VOUT0      | Output Channel 0 (1.0V def.)  |          | Do Not Connect          |
+---------------------------+------------+-------------------------------+----------+-------------------------+
| VOUT1                     | VOUT1      | Output Channel 1 (1.5V def.)  |          | Do Not Connect          |
+---------------------------+------------+-------------------------------+----------+-------------------------+

**Build Command**

.. code-block:: bash

	# to delete current build
	make TARGET=max32655 reset && make TARGET=max32655 clean
	# to build the project
	make TARGET=max32655
	# to flash the code
	make TARGET=max32655 run