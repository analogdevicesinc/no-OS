AD713x_FMCZ no-OS Example Project
=================================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-AD7134FMCZ`

Overview
--------

The EVAL-AD7134FMCZ evaluation board is tailored for the AD7134, a
24-bit, 4-channel, simultaneous sampling ADC capable of 1.5 MSPS. It
integrates two AD7134 ADCs for multidevice sampling demonstrations,
connecting to a PC via the SDP-H1 board or operating standalone.
Required input power ranges from 8V to 14V, regulated internally to 5V, 
1.8V, and 1.2V by onboard regulators. Distinct 4.096V references
are provided by the ADR444 for each ADC. Clock inputs can be sourced
from a crystal, MEMS oscillator, or CMOS input, enhancing its
application versatility.

Applications
------------

- Electrical test and measurement
- Audio test
- 3-phase power quality analysis
- Control and hardware in loop verification
- Sonars
- Condition monitoring for predictive maintenance
- Acoustic and material science research and development

Hardware Specifications
------------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD7134FMCZ evaluation board requires an input power supply
ranging from 8V to 14V. The onboard regulators internally convert this
to the necessary operating voltages of 5V, 1.8V, and 1.2V needed for
the system’s components. These power levels are essential for the proper
functioning of the AD7134 ADCs on the board, which feature distinct
4.096V references provided by the ADR444 to ensure accurate data
conversion.

On-board Connectors
~~~~~~~~~~~~~~~~~~~

+-----------------------+-----------------------+-----------------------+
| Connector             | Function              | Connector Type        |
+-----------------------+-----------------------+-----------------------+
| J1                    | SMB connector for     | Straight printed      |
|                       | XTAL2, external clock | circuit board (PCB)   |
|                       | input                 | mount                 |
+-----------------------+-----------------------+-----------------------+
| J2, J17               | Analog input SMB      | SMB jacks             |
|                       | terminals for Channel |                       |
|                       | 0 to Channel 7        |                       |
+-----------------------+-----------------------+-----------------------+
| P1, P7                | 8-pin connectors for  | Fixed terminal        |
|                       | the eight             | blocks, 2-pin, 3.81   |
|                       | differential analog   | mm, 90°               |
|                       | inputs                |                       |
+-----------------------+-----------------------+-----------------------+
| P3                    | Benchtop power supply | Fixed terminal        |
|                       | voltage input         | blocks, 2-pin 2.54    |
|                       |                       | mm, 90°               |
+-----------------------+-----------------------+-----------------------+
| P5, P9                | Optional external     | Vertical sockets      |
|                       | connectors for        |                       |
|                       | driver, Channel 0 and |                       |
|                       | 4                     |                       |
+-----------------------+-----------------------+-----------------------+
| P6, P10               | Optional external     | Vertical sockets      |
|                       | connectors for        |                       |
|                       | driver, Channel 0 and |                       |
|                       | 1                     |                       |
+-----------------------+-----------------------+-----------------------+
| P8                    | FMC Connector         | 160-pin, 10 mm, male, |
|                       |                       | VITA 57 connector     |
+-----------------------+-----------------------+-----------------------+

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The supported no-OS examples in the ad713x_fmcz project cover
configurations for interfacing with IIO and UART on the Xilinx platform.
Utilizing the AD713x driver, the examples demonstrate initializing and
configuring ADC devices for SPI communication, managing device
attributes, and handling data transfers through the IIO framework. The
examples also include configurations for GPIO and SPI chip select
management and setting storage for a clock reference frequency necessary
for SPI communication. This project provides insight into dual AD7134
device initialization, detailing the setup of channels and efficient
data transfer processes.

The initialization data used in the examples is taken out from the
`Project Source Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad713x_fmcz/src>`__.

UART example
~~~~~~~~~~~~

The UART example code in the ad713x_fmcz project illustrates UART
communication by configuring parameters such as device IDs, SPI engine
settings, GPIO configurations, and initializing ADC devices. UART is set
at a baud rate of 115200 using ``xil_uart_ops``, followed by SPI and PWM
configurations. The AD7134 ADC devices are initialized for data
communication over UART. Data is transmitted and stored in memory
buffers, with synchronization ensuring integrity.

In order to build the basic example, make sure you have the following
configuration in the 
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/ad713x_fmcz/Makefile>`__.

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   UART = y
   IIOD = n

IIO example
~~~~~~~~~~~

The IIO demo is a standard example, provided in most `no-OS
projects <https://github.com/analogdevicesinc/no-OS/tree/main/projects>`__,
that launches an IIOD server on the board so that the user may connect
to it via an IIO client. Using the IIO Oscilloscope application, users
can access device settings like offset, sampling frequency, and data
scale, facilitating seamless data management and conversion of raw ADC
readings into voltage signals.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with ADI IIO Oscilloscope Client, please take a
look at: `IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

To build the IIOD demo, add the following flag when invoking make. This
will build the IIOD server and the IIO section of the driver:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   UART = n
   IIOD = y

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~~

Hardware Used
^^^^^^^^^^^^^

- EVAL-AD7134FMCZ
- ZedBoard

Connections
^^^^^^^^^^^^

- Ensure both the ZedBoard and EVAL-AD7134FMCZ boards are powered off.

- Align the EVAL-AD7134FMCZ board over the ZedBoard so that the P8
  connector matches the ZedBoard’s FMC LPC socket.

- Firmly press the EVAL-AD7134FMCZ board onto the FMC LPC socket on the
  ZedBoard, ensuring a secure and straight connection.

- Supply the ZedBoard with 12VDC using its barrel jack (J20). Power
  for the EVAL-AD7134FMCZ is supplied via the FMC connector from the
  ZedBoard.

- Connect a MicroUSB cable from the ZedBoard’s USB-UART port (J14) to
  your PC.

- Ensure the ZedBoard’s boot jumpers (JP7 to JP11) and MIO0 (JP6) are set
  for your desired boot mode (typically SD Card for Linux demos).

- Power on the ZedBoard using the power switch (SW8). The
  EVAL-AD7134FMCZ will power on via the FMC connector. Confirm that
  status LEDs illuminate as expected on both boards.

Build Command
^^^^^^^^^^^^^^

.. code-block:: bash

   cp <SOME_PATH>/system_top.xsa .
   # to delete current build
   make reset
   # to build the project
   make
   # to flash the code
   make run
