MAX11205PMB1 no-OS Example Project
==================================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`MAX11205PMB1`

Overview
--------

The MAX11205 Evaluation Boards, equipped with the MAX11205 16-bit
delta-sigma ADC, are tailored for high-precision data acquisition. The
boards include parameters for UART, SPI, GPIO, and IRQ interfaces. These
configurations aid the seamless initialization and data reading
processes, converting raw ADC data into millivolts. This setup supports
modular integration and ease of maintenance, ideal for developers using
Analog Devices ADC in FPGA projects.

Applications
------------

- Sensor Measurement (Temperature and Pressure)
- Portable Instrumentation
- Battery Applications
- Weigh Scales

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The MAX11205PMB1 development board requires a 3.3V reference voltage,
specified as 3300 millivolts by the ``MAX11205_VREF_MV`` macro in the
project’s configuration. The board’s GPIO interface operates at the
VDDIOH level, set through the ``max11205_gpio_extra_ip`` variable in
``parameters.c``. It requires stable voltage levels for communication
interfaces like UART, GPIO, and SPI. The SPI is configured for a single
subordinate device, maintaining consistency at VDDIOH voltage, as
outlined in the ``max11205_spi_extra_ip`` structure. UART communication
on this board is set without hardware flow control (``UART_FLOW_DIS``).
These configurations are essential for the board’s proper operation in
embedded applications, ensuring reliable power and communication
interfaces.

Digital Communication Pins
~~~~~~~~~~~~~~~~~~~~~~~~~~

========== ====== ==========================
Pin Number Signal Description
========== ====== ==========================
3          MISO   Data-ready output/data-out
4          SCK    Serial Clock
5          GND    Ground
6          VCC    Power Supply
========== ====== ==========================

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from: 
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max11205pmb1/src/common>`__

The macros used in Common Data are defined in platform specific files
found in: 
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max11205pmb1/src/platform>`__

Basic example
~~~~~~~~~~~~~

The ``basic_example.c`` file initializes the MAX11205 ADC by setting up
SPI and IRQ configurations. A UART interface is configured for debug
printing, and SPI lines are connected to the ADC. A GPIO IRQ controller
manages data-ready signals from the ADC. The program continuously reads
ADC data in an infinite loop, converts this data to millivolts, and
prints the results via UART. Build configurations in the
``examples_src.mk`` include necessary source files, and Makefile
settings enable the ``BASIC_EXAMPLE`` flag for compilation.

In order to build the basic example, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   BASIC_EXAMPLE = y
   IIO_EXAMPLE = n

IIO example
~~~~~~~~~~~

The IIO example code for the MAX11205PMB1 project demonstrates
configuring and running the Industrial I/O (IIO) application with the
MAX11205 ADC. The ``iio_example_main`` function initializes hardware
interfaces such as SPI and GPIO to facilitate communication with the
ADC. It configures the IIO application parameters, including
device-specific settings and a data buffer for storing ADC data. The
example centers on executing ``iio_app_run``, which handles data
acquisition and processing tasks. Key components include the setup of an
IIO device descriptor and UART initialization, ensuring smooth data flow
and hardware interaction. The code structure allows flexibility in
handling various device communication requirements using macros for
customizable buffer sizes.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

In order to build the IIO project, make sure you have the following
configuration in the Makefile:

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

- :adi:`MAX11205PMB1`
- :adi:`MAX32655FTHR`

Connections
^^^^^^^^^^^

The MAX32655 does not have a PMOD interface, but you may use Dupont male-female cables 
to make the required connections. The following table shows how the connection between 
MAX11205PMB1 and MAX32655 is realized in this project example.

+-----------------+-----------------+-----------------+-----------------+
| P1 MAX11205PMB1 | MAX32655 Pin    | Function        | Mnemonic        |
| Pin Number      | Number          |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| Pin 1           | N.C.            | -               | -               |
+-----------------+-----------------+-----------------+-----------------+
| Pin 2           | N.C.            | -               | -               |
+-----------------+-----------------+-----------------+-----------------+
| Pin 3           | MISO            | Master In Slave | MISO            |
|                 |                 | Out             |                 |
+-----------------+-----------------+-----------------+-----------------+
| Pin 4           | SCLK            | Serial Clock    | SCLK            |
+-----------------+-----------------+-----------------+-----------------+
| Pin 5           | GND             | Digital Ground  | DGND            |
+-----------------+-----------------+-----------------+-----------------+
| Pin 6           | POWER 3.3V      | Digital Power   | VDD             |
+-----------------+-----------------+-----------------+-----------------+
| Pin 3           | P1_6            | Data Ready      | DRDY            |
+-----------------+-----------------+-----------------+-----------------+

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project
   make PLATFORM=maxim TARGET=max32655
   # to flash the code
   make run
   