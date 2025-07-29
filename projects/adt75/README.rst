ADT75 no-OS Example Project
===========================

.. contents::
    :depth: 3

Supported Evaluation Board
--------------------------

- :adi:`EVAL-ADT75`

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

- Isolated sensors
- Environmental control systems
- Computer thermal monitoring
- Thermal protection
- Industrial process control
- Power-system monitors
- Hand-held applications

Hardware Specifications
------------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The ADT75 requires a power supply voltage ranging from 2.7V to 5.5V.
It's recommended to use a 0.1µF ceramic capacitor between VDD and GND
for decoupling to minimize power supply noise and ensure accurate
readings.

Pin Description
~~~~~~~~~~~~~~~~

+-----------------+-----------------+-----------------+-----------------+
| Pin Number      | Name            | Description     | Additional      |
|                 |                 |                 | Notes           |
+-----------------+-----------------+-----------------+-----------------+
| 1               | SDA             | Serial Data     | Used for        |
|                 |                 | Line for I2C    | bidirectional   |
|                 |                 | communication.  | data transfer   |
|                 |                 |                 | between devices |
|                 |                 |                 | on the bus.     |
+-----------------+-----------------+-----------------+-----------------+
| 2               | SCL             | Serial Clock    | Synchronizes    |
|                 |                 | Line for I2C    | data            |
|                 |                 | communication.  | transmission on |
|                 |                 |                 | the I2C bus.    |
+-----------------+-----------------+-----------------+-----------------+
| 3               | OS/ALERT        | Configurable as | Active polarity |
|                 |                 | comparator or   | and output mode |
|                 |                 | interrupt       | (comparator or  |
|                 |                 | output.         | interrupt)      |
|                 |                 |                 | adjustable via  |
|                 |                 |                 | configuration   |
|                 |                 |                 | register.       |
+-----------------+-----------------+-----------------+-----------------+
| 4               | GND             | Ground          | Connect to the  |
|                 |                 | reference.      | common system   |
|                 |                 |                 | ground.         |
+-----------------+-----------------+-----------------+-----------------+
| 8               | VDD             | Power supply    | Power the       |
|                 |                 | voltage ranging | device within   |
|                 |                 | from 2.7V to    | specified       |
|                 |                 | 5.5V.           | voltage levels. |
+-----------------+-----------------+-----------------+-----------------+
| N/A             | A0, A1, A2      | Address         | Facilitates     |
|                 |                 | selection pins  | identification  |
|                 |                 | for setting the | of multiple     |
|                 |                 | I2C address of  | devices on the  |
|                 |                 | the ADT75.      | bus.            |
+-----------------+-----------------+-----------------+-----------------+

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
-------------------------

The ADT75 no-OS project comprises two primary examples for integrating
the ADT75 temperature sensor in applications. These examples serve as
practical guides for developers to incorporate ADT75 sensors into their
projects, detailing initialization, data extraction, and system setups
with no-OS utilities and structures.

The initialization data used in the example is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adt75/src/common>`__

The macros used in Common Data are defined in platform specific files
found in the `Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adt75/src/platform>`__

Basic Example
~~~~~~~~~~~~~

The **basic example** (``basic_example.c``) demonstrates initializing
the ADT75 sensor, reading temperature data, and displaying the readings
continuously via microcontroller interaction over the I2C protocol. This
example highlights initialization, data retrieval, and error handling
for straightforward sensor operation.

To build the Basic Example, configure the 
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/adt75/Makefile>`__ with:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   BASIC_EXAMPLE = y
   IIO_EXAMPLE = n

IIO Example
~~~~~~~~~~~

The **IIO example** (``iio_example.c``) provides a more advanced
application involving the Industrial Input/Output (IIO) subsystem. It
includes setting up the IIO environment, initializing the sensor’s IIO
descriptor, and running an IIO application that continuously processes
sensor data using defined buffers and parameters. This serves as a
template for integrating IIO devices into more extensive systems like
industrial control setups or complex sensor networks.

If you are not familiar with the ADI IIO framework, please take a look
at: `IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with the ADI IIO Oscilloscope app, please take a
look at: `IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

In order to build the IIO project, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   BASIC_EXAMPLE = n
   IIO_EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim Platform
~~~~~~~~~~~~~~

Hardware Used
^^^^^^^^^^^^^

- EVAL-ADT75
- MAX32650EVKIT

Connections
^^^^^^^^^^^^

To connect the EVAL-ADT75 board to the MAX32650EVKIT, follow these
steps:

- Connect MAX32650EVKIT’s I2C SDA and SCL lines to the ADT75’s SDA and
  SCL pins with 10kΩ pull-up resistors to VDD.

- Connect both boards’ GND together.

- Provide supply voltage (VDD) to the ADT75 (3.3V or 5V).

- If desired, connect the OS/ALERT pin from the ADT75 to a GPIO pin of
  the MAX32650EVKIT for interrupt or overtemperature alert
  functionality.

+-----------------+------------------+-----------------+-----------------+
| ADT75 Pin       | Function         | MAX32650EVKIT   | Notes           |
|                 |                  | Pin             |                 |
+-----------------+------------------+-----------------+-----------------+
| SDA (Pin 1)     | I2C Data Line    | I2C SDA         | Pull up to VDD  |
|                 |                  |                 | with 10kΩ       |
|                 |                  |                 | resistor        |
+-----------------+------------------+-----------------+-----------------+
| SCL (Pin 2)     | I2C Clock Line   | I2C SCL         | Pull up to VDD  |
|                 |                  |                 | with 10kΩ       |
|                 |                  |                 | resistor        |
+-----------------+------------------+-----------------+-----------------+
| OS/ALERT 3      | Over/            | GPIO (optional) | For temperature |
|                 | Undertemperature |                 | interrupt/alert |
|                 | Output (option)  |                 | signaling       |
|                 |                  |                 | (optional)      |
+-----------------+------------------+-----------------+-----------------+
| GND (Pin 4)     | Ground           | GND             |                 |
+-----------------+------------------+-----------------+-----------------+
| A2 (Pin 5)      | I2C Address      | GND or VDD      | Configure per   |
|                 | Selection        |                 | desired I2C     |
|                 |                  |                 | address         |
+-----------------+------------------+-----------------+-----------------+
| A1 (Pin 6)      | I2C Address      | GND or VDD      | Configure per   |
|                 | Selection        |                 | desired I2C     |
|                 |                  |                 | address         |
+-----------------+------------------+-----------------+-----------------+
| A0 (Pin 7)      | I2C Address      | GND or VDD      | Configure per   |
|                 | Selection        |                 | desired I2C     |
|                 |                  |                 | address         |
+-----------------+------------------+-----------------+-----------------+
| VDD (Pin 8)     | Power Supply     | 3.3V or 5V rail | Decouple with   |
|                 | (2.7V to 5.5V)   |                 | 0.1µF capacitor |
|                 |                  |                 | close to the    |
|                 |                  |                 | ADT75 VDD pin   |
+-----------------+------------------+-----------------+-----------------+

**Additional Notes**

- Place a 0.1µF decoupling capacitor as close as possible to the ADT75’s
  VDD pin.

- Ensure both SDA and SCL lines have required pull-up resistors.

- Confirm the MAX32650EVKIT is configured to use the correct I2C port
  and address in your firmware.

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # to delete current build
   make reset 
   # to build the project
   make TARGET=max32650
   # to flash the code
   make run
   # to debug the code
   make debug
