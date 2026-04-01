EVAL-ADXL313Z no-OS Example Project
===================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-ADXL312Z <https://www.analog.com/EVAL-ADXL312Z>`_
* `EVAL-ADXL313Z <https://www.analog.com/EVAL-ADXL313Z>`_
* `EVAL-ADXL314Z <https://www.analog.com/EVAL-ADXL314Z>`_

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

* Car alarms
* Hill start aid (HSA) systems
* Electronic parking brakes
* Data recorders (black boxes)

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
================= ================================ ============
Pin 1             Supply Voltage                   VS
Pin 2             Digital Interface Supply Voltage  VDDIO
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

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adxl313z/src>`__.

Basic Example
~~~~~~~~~~~~~

The basic example initializes the ADXL313 driver in 3-Wire SPI mode and
performs a self-test routine for the device. It then configures the
accelerometer and periodically reads the FIFO contents, displaying raw
and converted acceleration data (in m/s^2) for all three axes to the host
terminal via a UART serial connection. Interrupt sources such as
DATA_READY, ACTIVITY, INACTIVITY, WATERMARK, and OVERRUN are also
reported.

In order to build the basic example, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/eval-adxl313z/Makefile>`__:

.. code-block:: bash

    # Select the example you want to enable by choosing y for enabling and n for disabling
    BASIC_EXAMPLE = y
    IIO_EXAMPLE = n

IIO Example
~~~~~~~~~~~~

The IIO demo is a standard example, provided in most no-OS projects that
launches an IIOD server on the board so that the user may connect to it
via an IIO client. Using the IIO Oscilloscope application, users can
access device settings like calibration bias, sampling frequency, and
measurement range/scale, facilitating seamless data management and
real-time visualization of 3-axis acceleration data measured by the
ADXL312/ADXL313/ADXL314 accelerometers.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

In order to build the IIO project, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/eval-adxl313z/Makefile>`__:

.. code-block:: bash

    # Select the example you want to enable by choosing y for enabling and n for disabling
    BASIC_EXAMPLE = n
    IIO_EXAMPLE = y

No-OS Supported Platforms
-------------------------

STM32
~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-ADXL312Z <https://www.analog.com/EVAL-ADXL312Z>`_ /
  `EVAL-ADXL313Z <https://www.analog.com/EVAL-ADXL313Z>`_ /
  `EVAL-ADXL314Z <https://www.analog.com/EVAL-ADXL314Z>`_
* `NUCLEO-F401RE <https://www.st.com/en/evaluation-tools/nucleo-f401re.html>`_

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
