EVAL-ADXL313Z no-OS Example Project
===================================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-ADXL312Z`
- :adi:`EVAL-ADXL313Z`
- :adi:`EVAL-ADXL314Z`

Overview
--------

The EVAL-ADXL312Z, EVAL-ADXL313Z, and EVAL-ADXL314Z are simple breakout
boards that enable easy connection of the mounted accelerometer into an
existing system. These boards support the ADXL312, ADXL313, and ADXL314
3-axis digital accelerometers respectively, providing SPI 3-wire
communication interface. The ADXL313 is a small, thin, low power, 3-axis
accelerometer with high resolution (13-bit) measurement up to ±4 g,
measuring static acceleration of gravity in tilt-sensing applications as
well as dynamic acceleration resulting from motion or shock. Due to its
small form factor, the breakout board has minimal effect on
accelerometer performance, making it ideal for evaluation in an existing
system.

Applications
------------

- Car alarms
- Hill start aid (HSA) systems
- Electronic parking brakes
- Data recorders (black boxes)

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The breakout boards must be supplied with a power between 2.0V and 
3.6V. The host system should be capable of providing a 3.3V supply 
to the VS and VDDIO pins of the breakout board.

Connections
~~~~~~~~~~~

The following table lists all the on-board connectors and digital pins
of the supported evaluation boards via the P1 connector (SPI 3-Wire
communication):

================= ================================ ============
P1 Pin Number     Pin Function                     Mnemonic
Pin 1             Supply Voltage                   VS
Pin 2             Digital Interface Supply Voltage VDDIO
Pin 3             Not Connected                    NC
Pin 4             Not Connected                    NC
Pin 5             Serial Data Output               SDO
Pin 6             Serial Data Input                SDI
Pin 7             Serial Communications Clock      SCLK
Pin 8             Chip Select                      CS
Pin 9             Ground                           GND
Pin 10            Interrupt 1 Output               INT1
================= ================================ ============

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
:git-no-OS:`Project Source Path </projects/eval-adxl313z/src/>`.

Basic Example
~~~~~~~~~~~~~

The basic example initializes the ADXL313 driver in 3-Wire SPI mode and
performs a self-test routine for the device. It then configures the
accelerometer and periodically reads the FIFO contents, displaying raw
and converted acceleration data (in m/s²) for all three axes to the host
terminal via a UART serial connection. Interrupt sources such as
DATA_READY, ACTIVITY, INACTIVITY, WATERMARK, and OVERRUN are also
reported.

In order to build the basic example, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

    # Configure the example you want to enable by setting ‘y’ for enable and ‘n’ for disable
    BASIC_EXAMPLE = y
    IIO_EXAMPLE = n

IIO Example
~~~~~~~~~~~

The IIO demo is a standard example, provided in most no-OS projects that
launches an IIOD server on the board so that the user may connect to it
via an IIO client. Using the IIO Oscilloscope application, users can
access device settings like calibration bias, sampling frequency, and
measurement range/scale, facilitating seamless data management and
real-time visualization of 3-axis acceleration data measured by the
ADXL312/ADXL313/ADXL314 accelerometers.

If you are not familiar with ADI IIO Application, please take a look at:
`https://wiki.analog.com/resources/tools-software/no-os-software/iio`

To build the IIOD demo, add the following flag when invoking make. This
will build the IIOD server and the IIO section of the driver:

.. code-block:: bash

    # Configure the example you want to enable by setting ‘y’ for enable and ‘n’ for disable
    BASIC_EXAMPLE = n
    IIO_EXAMPLE = y

No-OS Supported Platforms
-------------------------

STM32
~~~~~

Hardware Used
^^^^^^^^^^^^^

- EVAL-ADXL312Z / EVAL-ADXL313Z / EVAL-ADXL314Z
- STM32 NUCLEO-F401RE

Connections
^^^^^^^^^^^

The following table shows the required physical connections between the
accelerometer breakout board and the NUCLEO-F401RE:

+-----------------+-----------------+-----------------+-----------------+
| **P1 Pin No.**  | **NUCLEO-F401RE | **Function      | **Mnemonic**    |
|                 | Pin**           | (Breakout       |                 |
|                 |                 | Board)**        |                 |
+-----------------+-----------------+-----------------+-----------------+
| Pin 1           | POWER 3.3V      | Supply Voltage  | VS              |
+-----------------+-----------------+-----------------+-----------------+
| Pin 2           | POWER 3.3V      | Digital Intf.   | VDDIO           |
|                 |                 | Supply Voltage  |                 |
+-----------------+-----------------+-----------------+-----------------+
| Pin 5           | PA6             | Serial Data     | SDO             |
|                 |                 | Output          |                 |
+-----------------+-----------------+-----------------+-----------------+
| Pin 6           | PA7             | Serial Data     | SDI             |
|                 |                 | Input           |                 |
+-----------------+-----------------+-----------------+-----------------+
| Pin 7           | PB3             | Serial Clock    | SCLK            |
+-----------------+-----------------+-----------------+-----------------+
| Pin 8           | PA4             | Chip Select     | CS              |
+-----------------+-----------------+-----------------+-----------------+
| Pin 9           | GND             | Ground          | GND             |
+-----------------+-----------------+-----------------+-----------------+
| Pin 10          | PA10            | Interrupt 1     | INT1            |
+-----------------+-----------------+-----------------+-----------------+

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

    # To build the project for STM32 platform
    make PLATFORM=stm32
    # To delete current build
    make reset
    # To build the project
    make
    # To flash the code
    make run
