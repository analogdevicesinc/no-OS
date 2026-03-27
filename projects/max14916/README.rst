MAX14916 no-OS Example Project
==============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* `MAX14916EVKIT <https://www.analog.com/MAX14916EVKIT>`_

Overview
--------

The `MAX14916 <https://www.analog.com/MAX14916>`_ is a compact industrial octal
high-side switch with integrated diagnostics. Each of the eight channels
features an on-resistance of only 120 mOhm (typ) and handles up to 1.1 A of
continuous current. Two adjacent channels can be combined to form four 2.4 A
switches. The device is controlled via SPI with built-in chip addressing for
up to four devices on a common chip select.

The SPI interface provides per-channel configuration and diagnostics including
undervoltage detection, open-wire detection (in both on and off states),
overload and current limiting reporting, and thermal shutdown protection.
Integrated surge protection (IEC61000-4-5) requires only a single external TVS
on VDD. The device operates from -40 C to +125 C and is available in a compact
48-pin 6 mm x 6 mm QFN package.

Applications
------------

* Industrial digital outputs
* PLC systems

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

* Absolute maximum VDD: 65 V
* Supply current: 2 mA (typ)
* On-resistance: 250 mOhm (max) at 125 C ambient
* Logic voltage interface: 2.5 V to 5.5 V

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max14916/src/common>`__.

The macros used in Common Data are defined in platform specific files
found in the
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max14916/src/platform>`__.

Basic example
~~~~~~~~~~~~~

The basic example initializes the MAX14916 device via SPI and demonstrates
LED control by cycling the status LEDs on each channel. It also configures
device parameters such as the flash LED time, watchdog timer, open-wire OFF
current source, and short-to-VDD threshold, then reads back the configuration
registers for verification.

In order to build the basic example, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/max14916/Makefile>`__:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   BASIC_EXAMPLE = y
   IIO_EXAMPLE = n

IIO example
~~~~~~~~~~~

The IIO example initializes the MAX14916 device and configures the IIO
application descriptor with output channels, enabling users to interact
with the device using IIO-based tools.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with ADI IIO Oscilloscope Client, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

In order to build the IIO example, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/max14916/Makefile>`__:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   BASIC_EXAMPLE = n
   IIO_EXAMPLE = y

No-OS Supported Platforms
--------------------------

Maxim
~~~~~

Used Hardware
^^^^^^^^^^^^^

* `MAX14916EVKIT <https://www.analog.com/MAX14916EVKIT>`_
* `AD-APARD32690-SL <https://www.analog.com/AD-APARD32690-SL>`_ or
  `MAX32666FTHR <https://www.analog.com/MAX32666FTHR>`_

Connections
^^^^^^^^^^^

The MAX14916EVKIT communicates via SPI. Connect the SPI signals (SCLK, SDI,
SDO, CS) and GND between the MAX14916EVKIT and the target microcontroller
board.

+-----------------+---------------------+------------------+
| SPI Parameter   | AD-APARD32690-SL    | MAX32666FTHR     |
+=================+=====================+==================+
| SPI Device ID   | 4                   | 1                |
+-----------------+---------------------+------------------+
| SPI Chip Select | 0                   | 0                |
+-----------------+---------------------+------------------+
| SPI Baudrate    | 100000              | 100000           |
+-----------------+---------------------+------------------+

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project (for AD-APARD32690-SL)
   make PLATFORM=maxim TARGET=max32690
   # to flash the code
   make run

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project (for MAX32666FTHR)
   make PLATFORM=maxim TARGET=max32665
   # to flash the code
   make run

Linux
~~~~~

Used Hardware
^^^^^^^^^^^^^

* `MAX14916EVKIT <https://www.analog.com/MAX14916EVKIT>`_
* Any SPI-capable Linux host

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project
   make PLATFORM=linux
   # to flash the code
   make run
