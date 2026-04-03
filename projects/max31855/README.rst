MAX31855 no-OS Example Project
===============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* `MAX31855PMB1 <https://www.analog.com/MAX31855PMB1>`_

Overview
--------

The MAX31855PMB peripheral module provides the necessary hardware to
interface the MAX31855 cold-junction compensated thermocouple-to-digital
converter to any system that utilizes Pmod-compatible expansion ports.
The MAX31855 performs cold-junction compensation and digitizes the signal
from a thermocouple. Versions of it are available that operate with a
K-, J-, N-, T-, S-, R-, or E-type thermocouple.

This module is set up to operate with a K-type thermocouple. The data is
output in a signed 14-bit, SPI-compatible, read-only format. This
converter resolves temperatures to 0.25 degrees C, allows readings as
high as +1800 degrees C and as low as -270 degrees C, and exhibits
thermocouple accuracy of +/-2 degrees C for temperatures ranging from
-200 degrees C to +700 degrees C for K-type thermocouples.

Applications
------------

* Appliances
* Automotive
* HVAC
* Industrial

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

When using the MAX31855 PMOD board, the 3.3 V power comes directly from
the host board it is connected to. The power from the host is generally
capable of providing up to 100 mA at 3.3 V.

Board Connectors
~~~~~~~~~~~~~~~~

The MAX31855PMB1 peripheral module can plug directly into a
Pmod-compatible port (configured for SPI) through connector J1.

+---------+---------+-----------------------------------------------+
| Pin     | Signal  | Description                                   |
+---------+---------+-----------------------------------------------+
| 1       | SS      | Chip enable (active low)                      |
+---------+---------+-----------------------------------------------+
| 2       | N.C.    | Not connected                                 |
+---------+---------+-----------------------------------------------+
| 3       | MISO    | Serial data output                            |
+---------+---------+-----------------------------------------------+
| 4       | SCK     | Serial clock input                            |
+---------+---------+-----------------------------------------------+
| 5       | GND     | Ground                                        |
+---------+---------+-----------------------------------------------+
| 6       | VCC     | Power supply                                  |
+---------+---------+-----------------------------------------------+

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max31855/src/common>`__.

The macros used in Common Data are defined in platform-specific files
found in the
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max31855/src/platform>`__.

Basic Example
~~~~~~~~~~~~~

The basic example initializes the MAX31855 thermocouple-to-digital
converter, reads thermocouple and ambient temperature data in a loop,
prints the results to the console, and updates readings every two
seconds. If initialization or data reading fails, it logs an error and
cleans up resources.

In order to build the basic example, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/max31855/Makefile>`__:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   BASIC_EXAMPLE = y
   IIO_EXAMPLE = n

IIO Example
~~~~~~~~~~~

This project is an IIOD demo for the MAX31855PMB1 evaluation board. The
project launches the IIOD server on the board so that the user may
connect to it via an IIO client application. Using IIO-Oscilloscope, the
user can read temperature samples from the device.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

In order to build the IIO project make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/max31855/Makefile>`__:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   BASIC_EXAMPLE = n
   IIO_EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim Platform
~~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* MAX31855PMB1
* MAX32655FTHR or MAX32650FTHR
* FTHR-PMD-INTZ
* MAX32625PICO (required for MAX32650FTHR debugging)

Connections
^^^^^^^^^^^

**MAX32655FTHR**

1. Connect the MAX32655FTHR with the FTHR-PMD-INTZ. The Maxim feather
   board should have stacking headers where the interposer board will be
   connected.

2. Connect the MAX31855PMB1 to the FTHR-PMD-INTZ SPI PMOD connector.

===================== =================
MAX31855PMB1          FTHR-PMD-INTZ SPI
===================== =================
Pin 1 (Chip Enable)   CS
Pin 2 (Not connected) MOSI
Pin 3 (MISO)          MISO
Pin 4 (SCK)           SCK
Pin 5 (GND)           GND
Pin 6 (VCC)           VCC
===================== =================

3. Power up the MAX32655FTHR by connecting it to the PC using a
   micro-USB cable.

**MAX32650FTHR**

1. Using a 10-pin ribbon cable, connect the MAX32625PICO to the
   MAX32650FTHR.

2. Connect the MAX32650FTHR to the FTHR-PMD-INTZ.

3. Connect the MAX31855PMB1 to the FTHR-PMD-INTZ SPI PMOD connector
   using the same pinout table above.

4. Power up the MAX32650FTHR by connecting it to the PC using
   micro-USB. Connect the MAX32625PICO to the PC as well.

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # remove sp characters from the build directory
   make reset
   # build the project
   make PLATFORM=maxim TARGET=max32650
   # flash the code
   make run
