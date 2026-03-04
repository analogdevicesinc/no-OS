MAX22196 no-OS Example Project
==============================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`MAX22196EVKIT`

Overview
--------

The MAX22196 Evaluation Kit enables evaluation of the MAX22194 and
MAX22196 digital input devices for industrial applications. It
facilitates USB-to-SPI communication, supported by a Pmod™-compatible
connector for extended interfacing with microcontrollers or Field
Programmable Gate Arrays (FPGA). The kit includes a Graphical User
Interface (GUI) for device configuration and status monitoring,
supporting input types such as Type 1/3, Type 2, TTL, and HTL.
Compliance with IEC 61000 ensures transient immunity. Key features
include configurable logic levels, current limits, daisy-chaining
support, and integrated filter settings for inputs, making it adaptable
to various industrial contexts. Power options include USB or an external
24V supply.

Applications
------------

- Programmable Logic Controllers
- Factory Automation
- Process Control

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The MAX22196EVKIT requires power input through two domains: the logic
side and the field side. The logic side operates on a regulated 3.3V
supply from a USB connection, while the field side relies on an external
24V DC supply via the VF24 and GND test points. The kit includes an
internal regulator enabling operation with input voltages between 8V
and 36V. Additionally, a MAX1556 DC-DC converter converts the USB’s 5V
to 3.3V for the logic side. Power management is adaptable, allowing for
external supply options to meet diverse industrial application demands.

Connections
~~~~~~~~~~~

+-----------------------+-----------------------+-----------------------+
| Connector             | Description           | Notes                 |
+-----------------------+-----------------------+-----------------------+
| T1                    | Terminal block for    | Pins 1, 3, 5, 7 are   |
|                       | digital inputs IM1 to | field-side ground.    |
|                       | IM4.                  |                       |
+-----------------------+-----------------------+-----------------------+
| T2                    | Terminal block for    | Pins 1, 3, 5, 7 are   |
|                       | digital inputs IM5 to | field-side ground.    |
|                       | IM8.                  |                       |
+-----------------------+-----------------------+-----------------------+
| PMOD1                 | 12-pin                | For connecting to a   |
|                       | Pmod-compatible male  | microcontroller or    |
|                       | connector.            | FPGA.                 |
+-----------------------+-----------------------+-----------------------+
| PMOD2                 | 12-pin                | For daisy chaining    |
|                       | Pmod-compatible       | with another device.  |
|                       | female connector.     |                       |
+-----------------------+-----------------------+-----------------------+
| CONN1                 | Barrel connector for  | Connect +24 V dc or   |
|                       | main power supply.    | between VF24 and      |
|                       |                       | GND_TP1.              |
+-----------------------+-----------------------+-----------------------+
| X1                    | Micro-USB connector.  | Connect to PC USB     |
|                       |                       | port.                 |
+-----------------------+-----------------------+-----------------------+

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from: 
:git-no-OS:`Project Common Data Path </projects/max22196/src/common>`

The macros used in Common Data are defined in platform specific files
found in: 
:git-no-OS:`Project Platform Configuration Path </projects/max22196/src/platform>`

Basic example
~~~~~~~~~~~~~

The basic example code for the MAX22196 project initializes and
configures a MAX22196 digital input serializer. The
``basic_example_main`` function sets up the MAX22196 device by
configuring channel 0 to operate in source mode. It then applies a
specific filter delay to the channel. After configuration, the code
retrieves and prints the channel’s filter delay and counter values to
ensure the correct setup. This example provides a practical guide for
integrating the MAX22196 into applications, highlighting essential
operations using the project’s API and no-OS functions suitable for
non-OS environments.

In order to build the basic example, make sure you have the following
configuration in the :git-no-OS:`Makefile </projects/max22196/Makefile>`:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   BASIC_EXAMPLE = y
   IIO_EXAMPLE = n

IIO example
~~~~~~~~~~~

The IIO example code for the MAX22196 project within the no-OS framework
initializes and executes an Industrial Input/Output (IIO) application
specifically for the MAX22196 device. The code structure comprises
initialization of device descriptors and parameters, followed by
configuring the IIO application using these configurations. The
``iio_example_main`` function orchestrates this setup, defining an array
of IIO devices and setting UART parameters. It then executes the IIO
application via ``iio_app_run``. Error handling ensures proper cleanup
through ``iio_app_remove`` and ``max22196_iio_remove``, returning a
status code reflecting the execution outcome. Preprocessor directives
facilitate configuration management, enabling only the IIO example to
compile, thus offering a focused demonstration of the MAX22196’s
functionalities within the IIO framework.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with ADI IIO Oscilloscope Client, please take a
look at: `IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

To build the IIOD demo, add the following flag when invoking make. This
will build the IIOD server and the IIO section of the driver:

.. code-block:: bash

   # Configure the example you want to enable by setting 'y' for enable and 'n' for disable
   BASIC_EXAMPLE = n
   IIO_EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim
~~~~~

Hardware Used
^^^^^^^^^^^^^

- MAX22196EVKIT
- MAX32666FTHR (MAX32665) or AD-APARD32690-SL (MAX32690)

Connections
^^^^^^^^^^^

Connect the MAX22196EVKIT to the MAX32666FTHR board using the
PMOD-compatible SPI interface. The EVKIT exposes SPI pins (MISO,
MOSI, SCLK, CS) plus power and ground, which map directly to the
MAX32666FTHR’s PMOD header pins.

+----------------------+----------------------+----------------------+
| MAX22196EVKIT (PMOD  | MAX32666FTHR (PMOD   | Function             |
| Header)              | Header)              |                      |
+----------------------+----------------------+----------------------+
| Pin 1 – CS           | D0                   | SPI Chip Select      |
+----------------------+----------------------+----------------------+
| Pin 2 – MISO         | D1                   | SPI MISO (Master In, |
|                      |                      | Slave Out)           |
+----------------------+----------------------+----------------------+
| Pin 3 – MOSI         | D2                   | SPI MOSI (Master     |
|                      |                      | Out, Slave In)       |
+----------------------+----------------------+----------------------+
| Pin 4 – SCLK         | D3                   | SPI Clock            |
+----------------------+----------------------+----------------------+
| Pin 5 – GND          | GND                  | Ground               |
+----------------------+----------------------+----------------------+
| Pin 6 – VCC (3.3V)   | 3V3                  | Power Supply         |
+----------------------+----------------------+----------------------+

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project for running basic example
   make EXAMPLE=basic PLATFORM=maxim TARGET=max32690
   # to build the project for running iio example
   make EXAMPLE=iio_example PLATFORM=maxim TARGET=max32690
   # to flash the code
   make run
