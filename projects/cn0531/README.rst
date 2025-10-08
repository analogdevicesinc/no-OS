CN0531 no-OS Example Project
============================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-CN0531-PMDZ`
- :adi:`EVAL-ADICUP3029`

Overview
---------

The EVAL-CN0531-PMDZ is a precision DAC module designed for applications
such as medical instrumentation, test and measurement equipment,
industrial control, and scientific research. It features a 20-bit DAC
with minimal 1ppm accuracy and ±1 LSB Integral Non-Linearity (INL). It
provides flexibility with support for multiple voltage references and
power options, allowing use of onboard or external power supplies and
voltage references. The module’s compatibility with the SPI Pmod
interface enables versatile and precise analog output applications.

Applications
-------------

- Instrumentation and measurement equipment
- DC Sources and Power Supply Solutions
- Analytical Instrument Solutions

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-CN0531-PMDZ module requires a power supply that can be
configured to use either onboard power converters or external power
sources. The onboard converters can generate the necessary voltage
levels from a single external input, which is usually supplied by a
connected development platform. Alternatively, users can choose to
supply power directly through external connections, which can be
beneficial for integration into systems with existing power
infrastructure. The module’s voltage range spans from -5V to +5V,
allowing it to be flexible for use in various high-precision
applications like scientific instrumentation and industrial control. The
high power supply rejection ratio and low operating drift provided by
the onboard voltage regulators enhance its stability and precision.

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
-------------------------

The initialization data used in the example is taken out from the
`Project Data Source Path: <https://github.com/analogdevicesinc/no-OS/tree/main/projects/cn0531/src>`__

IIO example
~~~~~~~~~~~

The IIO example code for the AD5791 DAC initializes SPI and UART
interfaces, configuring essential peripherals for operation. It sets
parameters like SPI mode, chip select, and speed, and establishes UART
communication with specified baud rate and device ID. The AD5791 DAC is
controlled via SPI and interfaced using IIO descriptors to integrate
within the application. The main function manages the setup and
execution of the IIO application, utilizing IIO buffers for data
transfers, enabling efficient data operations with the DAC within a data
acquisition system.

This project is an IIOD demo for EVAL-CN0531-PMDZ board. The project
launches a IIOD server on the board so that the user may connect to it
via an IIO client. Using IIO Oscilloscope, the user can configure the
ADC and view the measured data on a plot.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with ADI IIO Oscilloscope Client, please take a
look at: 
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

The No-OS IIO Application together with the AD5791 no-OS drivers take
care of all the back-end logic needed to setup the IIO server.

In order to build the IIO project, make sure you have the following
configuration in the 

`Makefile: <https://github.com/analogdevicesinc/no-OS/blob/main/projects/cn0531/Makefile>`__

.. code-block:: bash

   IIOD=y

No-OS Supported Platform
-------------------------

ADuCM Platform
~~~~~~~~~~~~~~~

Hardware Used
^^^^^^^^^^^^^

- :adi:`EVAL-CN0531-PMDZ`
- :adi:`EVAL-ADICUP3029`

Connections
^^^^^^^^^^^

- Connect the EVAL-CN0531-PMDZ to the EVAL-ADICUP3029 using the P8 connector. 

+-----------+-------------------+-----------------------------+
| **Pin     | **EVAL-CN0531-    | **EVAL-ADICUP3029           |
| No.**     | PMDZ Connector**  | PMOD Header**               |
+-----------+-------------------+-----------------------------+
| 8-1       | VCC (3.3V Power   | +3.3V                       |
|           | Supply)           |                             |
+-----------+-------------------+-----------------------------+
| 8-2       | GND (Ground)      | DGND                        |
+-----------+-------------------+-----------------------------+
| 8-3       | UART_TX (Data     | UART0_RX / GPIO11           |
|           | from CN0531 to    |                             |
|           | ADICUP)           |                             |
+-----------+-------------------+-----------------------------+
| 8-4       | UART_RX (Data     | UART0_TX / GPIO10           |
|           | from ADICUP to    |                             |
|           | CN0531)           |                             |
+-----------+-------------------+-----------------------------+

- Plug the USB Type A connector into your PC and the micro USB connector into the EVAL-ADICUP3029. 

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project
   make PLATFORM=aducm3029
   # to flash the code
   make run
   # to debug the code
   make debug
