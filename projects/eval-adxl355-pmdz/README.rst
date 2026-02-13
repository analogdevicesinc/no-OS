EVAL-ADXL355-PMDZ no-OS Example Project
=======================================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-ADXL355-PMDZ`

Overview
--------

The EVAL-ADXL355-PMDZ is an accelerometer evaluation board designed for
high precision and low power consumption in a 3-axis MEMS configuration.
It offers selectable measurement ranges of ±2g, ±4g, and ±8g, catering
to various applications including inertial measurement units and
structural health monitoring. Powered by a 3.3V host board supply, it
supports up to 100 mA current and offers interrupt management through
programmable interrupt and data-ready pins connected via an SPI PMOD
header. Compatible with both no-OS and Linux drivers, the board allows
for flexible integration. It is designed for stability and reliability,
featuring guaranteed temperature stability and hermetic packaging.

Applications
------------

- Inertial measurement units (IMUs)/attitude and heading reference
  systems (AHRSs)
- Platform stabilization systems
- Structural health monitoring
- Seismic imaging
- Tilt sensing
- Robotics
- Condition monitoring

Hardware Specifications
------------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-ADXL355-PMDZ evaluation board operates with a power supply of
3.3V from a host board, consuming up to 100 mA of current. This
specification supports its functions as a precision 3-axis accelerometer
with selectable measurement ranges of ±2g, ±4g, and ±8g. Adequate power
management ensures its stability and reliability, important features
given its usage in applications like inertial measurement units and
structural health monitoring.

Digital Communication Pins
~~~~~~~~~~~~~~~~~~~~~~~~~~

========== =================== ========
Pin Number Pin Function        Mnemonic
1          Chip Select         CS
2          Master Out Slave In MOSI
3          Master In Slave Out MISO
4          Serial Clock        SCLK
5          Digital Ground      DGND
6          Digital Power       VDD
7          Interrupt 1         INT1
8          Not Connected       NC
9          Interrupt 2         INT2
10         Data Ready          DRDY
11         Digital Ground      DGND
12         Digital Power       VDD
========== =================== ========

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The EVAL-ADXL355-PMDZ no-OS project supports several examples tailored
for different configurations and platforms. These include the
“dummy_example” for basic continuous data reading from the ADXL355
sensor, the “iio_example” using the Industrial I/O (IIO) framework for
standardized sensor interfacing, and the “iio_trigger_example” for
triggered data acquisition. These examples are organized in sections
within the ``builds.json`` file, supporting platforms like STM32, Maxim,
Pico, and ADUCM3029.

The initialization data used in the examples is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adxl355-pmdz/src/common>`__

The macros used in Common Data are defined in platform specific files
found in the 
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adxl355-pmdz/src/platform>`__

Dummy example
~~~~~~~~~~~~~~

The ``dummy_example`` code in the no-OS project for the EVAL-ADXL355-PMDZ
evaluates the basic operation of the ADXL355 accelerometer. It
initializes the UART interface and the ADXL355 sensor, setting the
sensor to measurement mode with a defined output data rate. It
continuously reads the x, y, and z axis accelerometer data, formats it,
and outputs via UART. The example also periodically accesses the
sensor’s FIFO buffer to gather multiple data sets in one cycle. This
simple yet effective loop showcases real-time data capture without added
complexity.

In order to build the dummy example, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   DUMMY_EXAMPLE = y
   IIO_EXAMPLE = n
   IIO_TRIGGER_EXAMPLE = n

IIO example
~~~~~~~~~~~~

The IIO example code for the EVAL-ADXL355-PMDZ integrates with the
Industrial I/O (IIO) framework to manage sensor interfacing, enabling
the setup of a generic IIO device service for the ADXL355 accelerometer.
It initializes the sensor with relevant SPI and UART parameters, sets up
data buffers, and configures hardware parameters to facilitate real-time
data acquisition. The code includes initialization of the IIO
application, device descriptors, and communication interfaces, launching
the IIO application to allow user interaction via an IIO client.

The IIO demo is a standard example, provided in most `no-OS
projects <https://github.com/analogdevicesinc/no-OS/tree/main/projects>`__,
that launches an IIOD server on the board so that the user may connect
to it via an IIO client. Using the IIO Oscilloscope application, users
can access device settings like sampling frequency, scaling, and offsets
efficiently.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__.

If you are not familiar with ADI IIO Oscilloscope Client, please take a
look at: 
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__.

To build the IIOD demo, add the following flag when invoking make. This
will build the IIOD server and the IIO section of the driver:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   DUMMY_EXAMPLE = n
   IIO_EXAMPLE = y
   IIO_TRIGGER_EXAMPLE = n

IIO Trigger Example
~~~~~~~~~~~~~~~~~~~

The ``iio_trigger_example`` code demonstrates a triggered data
acquisition using the IIO framework. It initializes the ADXL355
accelerometer, sets up an interrupt-driven GPIO trigger, and configures
hardware components for data capture. The GPIO trigger is responsive to
rising edge events, ensuring precise timing for data sampling when an
interrupt signal occurs. This setup includes a list of IIO devices and
triggers, which are initialized by the IIO application for continuous
operation, resulting in efficient and consistent data acquisition. This
method reduces power consumption by waking the system only when data is
available, optimizing the data collection process.

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   DUMMY_EXAMPLE = n
   IIO_EXAMPLE = n
   IIO_TRIGGER_EXAMPLE = y

No-OS Supported Platforms
-------------------------

- ADuCM
- Maxim
- STM32
- Raspberry Pi Pico

ADuCM Platform
~~~~~~~~~~~~~~~

Hardware Used
^^^^^^^^^^^^^

- :adi:`EVAL-ADXL355-PMDZ`
- :adi:`EVAL-ADICUP3029`

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   export PLATFORM=aducm3029
   # to build the project
   make
   # to flash the code
   make run

Connections
^^^^^^^^^^^

- The EVAL-ADICUP3029 has a PMOD interface, so simply connect the
  EVAL-ADXL355-PMDZ through the PMOD connector.

Maxim Platform
~~~~~~~~~~~~~~

Hardware Used
^^^^^^^^^^^^^

- :adi:`EVAL-ADXL355-PMDZ`
- :adi:`MAX32655FTHR`

Connections
^^^^^^^^^^^

For connecting the EVAL-ADXL355-PMDZ to the MAX32655 using Dupont
male-female cables, the following pin configuration is critical for the
SPI interface:

+-----------------+-----------------+-----------------+-----------------+
| EVAL-ADXL355    | MAX32655FTHR    | Function        | Description     |
+-----------------+-----------------+-----------------+-----------------+
| Pin 1           | SS0             | Chip Select     | Initiates       |
|                 |                 | (CS)            | communication   |
|                 |                 |                 | with sensor     |
+-----------------+-----------------+-----------------+-----------------+
| Pin 2           | MOSI            | Master Out      | Transfers data  |
|                 |                 | Slave In (MOSI) | from MAX32655   |
|                 |                 |                 | to ADXL355      |
+-----------------+-----------------+-----------------+-----------------+
| Pin 3           | MISO            | Master In Slave | Receives data   |
|                 |                 | Out (MISO)      | from ADXL355    |
+-----------------+-----------------+-----------------+-----------------+
| Pin 4           | SCLK            | Serial Clock    | Synchronizes    |
|                 |                 | (SCLK)          | the data        |
|                 |                 |                 | transmission    |
+-----------------+-----------------+-----------------+-----------------+
| Pin 5           | GND             | Digital Ground  | Provides common |
|                 |                 | (DGND)          | ground          |
|                 |                 |                 | reference       |
+-----------------+-----------------+-----------------+-----------------+
| Pin 6           | POWER           | 3.3V (VDD)      | Supplies power  |
|                 |                 |                 | to the sensor   |
+-----------------+-----------------+-----------------+-----------------+
| Pin 10          | P1_9            | Data Ready      | Indicates new   |
|                 |                 | (DRDY)          | data is         |
|                 |                 |                 | available       |
+-----------------+-----------------+-----------------+-----------------+

STM32 Platform
~~~~~~~~~~~~~~

Hardware Used
^^^^^^^^^^^^^^

- :adi:`EVAL-ADXL355-PMDZ`
- :adi:`SDP-K1`
- STM32 debugger

Connections
^^^^^^^^^^^

To connect the EVAL-ADXL355-PMDZ accelerometer evaluation board with
the SDP-K1, use Dupont male-female cables, given that the SDP-K1 does
not come with a native PMOD interface. The table below details the
specific pin connections required:

+-----------------+-----------------+-----------------+-----------------+
| EVAL-ADXL355    | SDP-K1 Pin      | Function        | Mnemonic        |
+-----------------+-----------------+-----------------+-----------------+
| Pin 1           | DIGITAL 10      | Chip Select     | CS              |
+-----------------+-----------------+-----------------+-----------------+
| Pin 2           | DIGITAL 11      | Master Out      | MOSI            |
|                 |                 | Slave In        |                 |
+-----------------+-----------------+-----------------+-----------------+
| Pin 3           | DIGITAL 12      | Master In Slave | MISO            |
|                 |                 | Out             |                 |
+-----------------+-----------------+-----------------+-----------------+
| Pin 4           | DIGITAL 13      | Serial Clock    | SCLK            |
+-----------------+-----------------+-----------------+-----------------+
| Pin 5           | DIGITAL GND     | Digital Ground  | DGND            |
+-----------------+-----------------+-----------------+-----------------+
| Pin 6           | POWER 3.3V      | Digital Power   | VDD             |
+-----------------+-----------------+-----------------+-----------------+
| Pin 10          | ANALOG IN A0    | Data Ready      | DRDY            |
+-----------------+-----------------+-----------------+-----------------+

**Note:** It is crucial to set the ``VIO_ADJUST`` on the SDP-K1 to 3.3V
to match the operating voltage of the EVAL-ADXL355-PMDZ board.

--------------

Raspberry Pi Pico
~~~~~~~~~~~~~~~~~

Hardware Used
^^^^^^^^^^^^^

- EVAL-ADXL355-PMDZ
- Raspberry Pi Pico

Connections
^^^^^^^^^^^

The following table provides pin connections when using Dupont cables to
interface the EVAL-ADXL355-PMDZ evaluation board with a Raspberry Pi
Pico:

+------------------+------------------+------------------+----------+
| EVAL-ADXL355     | Raspberry Pico   | Function         | Mnemonic |
+------------------+------------------+------------------+----------+
| Pin 1            | GP17             | Chip Select      | CS       |
+------------------+------------------+------------------+----------+
| Pin 2            | GP19             | Master Out Slave | MOSI     |
|                  |                  | In (MOSI)        |          |
+------------------+------------------+------------------+----------+
| Pin 3            | GP16             | Master In Slave  | MISO     |
|                  |                  | Out (MISO)       |          |
+------------------+------------------+------------------+----------+
| Pin 4            | GP18             | Serial Clock     | SCLK     |
+------------------+------------------+------------------+----------+
| Pin 5            | GND              | Digital Ground   | DGND     |
+------------------+------------------+------------------+----------+
| Pin 6            | 3.3V             | Digital Power    | VDD      |
+------------------+------------------+------------------+----------+
| Pin 10           | GP20             | Data Ready       | DRDY     |
+------------------+------------------+------------------+----------+

Ensure connections are accurately made to maintain the operational
integrity of the accelerometer when interfaced with the Raspberry Pi
Pico.

Build Command
^^^^^^^^^^^^^

**Note: When invoking the make command, ensure to specify the platform
you are building the project for (e.g., make TARGET=Maxim)**

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project
   make
   # to flash the code
   make run
