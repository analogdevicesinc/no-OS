ADT7420-PMDZ no-OS Example Project
==================================

.. no-os-doxygen::
.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-ADT7420-PMDZ`

Overview
--------

The EVAL-ADT7420-PMDZ is an evaluation board designed for the ADT7420
temperature sensor featuring an extended I2C interface capable of
chaining multiple I2C devices. Its factory-set I2C address is 0x48, with
options for modification via solder link configurations. The board
operates with a 3.3 V power supply and an average supply current of 210
μA. It is compatible with platforms like Maxim and Mbed, including
initialization parameters for UART and I2C, which aid in seamless
integration. The board supports functionality for alerting
high-temperature conditions via INT and CT connections. This setup
enhances temperature monitoring capabilities and is optimized for
various system integrations.

Applications
------------

- RTD and thermistor replacement
- Thermocouple cold junction compensation
- Medical equipment
- Industrial control and test
- Food transportation and storage
- Environmental monitoring and HVAC
- Laser diode temperature control

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

When using the ADT7420 PMOD board, the 3.3V power for the PMOD is
sourced directly from the host board it connects to. Ensure that the
host is capable of providing up to 100 mA at 3.3V as per the board
specifications. To confirm power to the EVAL-ADT7420-PMDZ, check the
onboard LED located near component R5, which will be illuminated when
powered.

Digital Interface (PMOD)
~~~~~~~~~~~~~~~~~~~~~~~~

The PMOD interface supports several standardized digital communication
protocols, including SPI, I2C, and UART, as outlined by Digilent
standards. The EVAL-ADT7420-PMDZ specifically uses an extended I2C
interface, allowing for additional flexibility in daisy-chaining I2C
devices on the same bus.

P1 Pin Configuration
^^^^^^^^^^^^^^^^^^^^

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

The EVAL-ADT7420-PMDZ provides optional operation modes through solder
links JP1 and JP2, which assist in managing I2C address configurations.
Here is the configuration table:

+---------------+---------------+----------+----------+-------------+
| JP1 Position  | JP2 Position  | A1 Logic | A0 Logic | I2C Address |
+===============+===============+==========+==========+=============+
| 2 & 3 Short   | 2 & 3 Short   | Low      | Low      | 0x48        |
+---------------+---------------+----------+----------+-------------+
| 2 & 3 Short   | 1 & 2 Short   | Low      | High     | 0x49        |
+---------------+---------------+----------+----------+-------------+
| 1 & 2 Short   | 2 & 3 Short   | High     | Low      | 0x4A        |
+---------------+---------------+----------+----------+-------------+
| 1 & 2 Short   | 1 & 2 Short   | High     | High     | 0x4B        |
+---------------+---------------+----------+----------+-------------+

Temperature Alerts
~~~~~~~~~~~~~~~~~~

The ADT7420 monitors temperature conditions and provides alerts for
Over/Under and Critical temperature notifications via the INT and CT
connections. These thresholds are software-configurable, and event
monitoring can be extended by accessing the respective pins relative to
GND for indication of alert conditions.

No-OS Build Setup
------------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adt7420-pmdz/src/common>`__.

The macros used in Common Data are defined in platform specific files
found in: 
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adt7420-pmdz/src/platform>`__

Dummy example
~~~~~~~~~~~~~

The ``Dummy example`` for the EVAL-ADT7420-PMDZ board demonstrates
continuous operation with the ADT7420 temperature sensor. It initializes
the sensor via I2C, reads temperature limits, calculates the current
temperature, and configures hysteresis for temperature threshold
management. This example showcases reading register values to handle
alert conditions, aiding users in basic sensor setup and operations. The
continuous loop facilitates ongoing temperature monitoring, providing a
fundamental understanding of sensor interaction and configuration on the
evaluation board.

In order to build the basic example, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   DUMMY_EXAMPLE = y
   IIO_EXAMPLE = n

IIO example
~~~~~~~~~~~

The IIO example code for the ADT7420 evaluation board integrates the
ADT7420 temperature sensor with the Industrial Input/Output (IIO)
interface. It initializes the sensor, configures a data buffer for
sensor data reading, and executes the main loop via the ``iio_app_run``
function. An IIOD server operates on the board for communication between
the host and the ADT7420 device, allowing for management and
configuration via the IIO Oscilloscope application. Users can adjust
settings like offset and sampling frequency and convert ADC readings to
voltage signals for analysis, thus supporting continuous real-time data
acquisition and processing.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__.

If you are not familiar with ADI IIO Oscilloscope Client, please take a
look at: 
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__.

To build the IIOD demo, add the following flag when invoking make. This
will build the IIOD server and the IIO section of the driver:

.. code-block:: bash

   # Configure the example you want to enable by setting 'y' for enable and 'n' for disable
   DUMMY_EXAMPLE = n
   IIO_EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim
~~~~~

Hardware Used
^^^^^^^^^^^^^

- EVAL-ADT7420-PMDZ
- Maxim Platform Board either MAX32650/5, MAX32660/5, MAX78000, or
  MAX32690

Connections
^^^^^^^^^^^

+-----------------------+-----------------------+-----------------------+
| **ADT7420 Pin**       | **Maxim Board Pin**   | **Description**       |
+-----------------------+-----------------------+-----------------------+
| **VDD**               | **3.3V or 5V**        | Connect to            |
|                       |                       | appropriate voltage   |
|                       |                       | output based on       |
|                       |                       | system requirements.  |
|                       |                       | ADT7420 operates at   |
|                       |                       | either 3.3V or 5V.    |
+-----------------------+-----------------------+-----------------------+
| **GND**               | **GND**               | Ground connection for |
|                       |                       | completing the power  |
|                       |                       | circuit.              |
+-----------------------+-----------------------+-----------------------+

**Note:** Ensure the Maxim carrier board supplies sufficient power. The
ADT7420 typically requires **210 μA at 3.3V**.

**Signal Connections**

+-----------------------+-----------------------+-----------------------+
| **ADT7420 Pin**       | **Maxim Board Pin**   | **Description**       |
+-----------------------+-----------------------+-----------------------+
| SDA                   | SDA                   | I2C Serial Data line. |
|                       |                       | Connect to the SDA    |
|                       |                       | pin on the Maxim      |
|                       |                       | platform.             |
+-----------------------+-----------------------+-----------------------+
| SCL                   | SCL                   | I2C Serial Clock      |
|                       |                       | line. Connect to the  |
|                       |                       | SCL pin on the Maxim  |
|                       |                       | platform.             |
+-----------------------+-----------------------+-----------------------+

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # to delete the current build
   make reset
   # to build the project
   make PLATFORM=maxim TARGET=MAX32625
   # to flash the code
   make run

Mbed
~~~~

Hardware Used
^^^^^^^^^^^^^

- EVAL-ADT7420-PMDZ
- Arduino UNO

Connections
^^^^^^^^^^^

+-----------------------+-----------------------+-----------------------+
| **EVAL-ADT7420-PMDZ   | **Arduino UNO Pin**   | **Description**       |
| Pin**                 |                       |                       |
+-----------------------+-----------------------+-----------------------+
| VDD                   | 5V                    | Powers the sensor.    |
|                       |                       | ADT7420 supports both |
|                       |                       | 3.3V and 5V; Arduino  |
|                       |                       | UNO uses 5V.          |
+-----------------------+-----------------------+-----------------------+
| GND                   | GND                   | Common ground         |
|                       |                       | connection.           |
+-----------------------+-----------------------+-----------------------+
| SCL                   | A5                    | I2C clock line.       |
|                       |                       | Arduino UNO uses A5   |
|                       |                       | for SCL.              |
+-----------------------+-----------------------+-----------------------+
| SDA                   | A4                    | I2C data line.        |
|                       |                       | Arduino UNO uses A4   |
|                       |                       | for SDA.              |
+-----------------------+-----------------------+-----------------------+
| ADDR                  | GND                   | Sets I2C address to   |
|                       |                       | ``0x48``. Can be      |
|                       |                       | changed by connecting |
|                       |                       | to VDD or other logic |
|                       |                       | levels.               |
+-----------------------+-----------------------+-----------------------+

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project
   make PLATFORM=mbed
   # to flash the code
   make
