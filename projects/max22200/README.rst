MAX22200 no-OS Example Project
==============================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`MAX22200EVKIT`

Overview
--------

The MAX22200 Evaluation Kit (MAX22200EVKIT) is designed for evaluating
the MAX22200 integrated circuit, which functions as an eight
half-bridge, serial-controlled solenoid driver. This kit provides a
serial-controlled interface enabling both high-side and low-side
operations for solenoid drivers, suitable for latched valves and brushed
DC motors. It supports a voltage range of +4V to +36V, with built-in
power management that supplies the microcontroller from a +5V USB VBUS.

Applications
------------

- Relays Driver
- Solenoid, Valves, Electromagnetic Drivers
- Generic Low-Side and High-Side Switch Applications
- Latched (Bi-Stable) Solenoid Valve Drivers
- Brushed DC Motor Driver

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The MAX22200EVKIT is powered from a wide input voltage range of +4V to
+36V. It incorporates a built-in power management system that
efficiently supplies a microcontroller using a +5V USB VBUS. This
design ensures flexible power supply options, supporting the solenoid
driver's operations without requiring external power conversion for the
microcontroller. The evaluation kit's capability to handle various input
voltages makes it suitable for diverse application environments,
including industrial and consumer electronics.

Connections
~~~~~~~~~~~

============== =============================
Designator     Type
J1             Micro USB B Receptacle (SMT)
J2             Terminal Strip (SMT)
J3, J6         Test Point
J4             Terminal Block (Through Hole)
J7, J8, J13-15 Test Point, Ground
J9, J10        Terminal Block (Through Hole)
J11            Header (Through Hole)
J12            Terminal Block (Through Hole)
J16            Header (Through Hole)
============== =============================

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
:git-no-OS:`Project Common Data Path </projects/max22200/src/common>`.

The macros used in Common Data are defined in platform specific files
found in
:git-no-OS:`Project Platform Configuration Path </projects/max22200/src/platform>`.

Basic example
~~~~~~~~~~~~~

The MAX22200EVKIT basic example code initializes a serial communication
interface to configure the MAX22200 device for driving two channels in
half-bridge mode. It begins by setting up the MAX22200 descriptor and
assigns a chopping frequency of 80 kHz. Channels 0 and 1 are configured
to operate with low-side current drive in half-bridge mode with a
frequency division setting, and fault masking is enabled to ensure
proper operation. The configuration is verified through operation
settings logging, allowing rapid setup for driving solenoids or DC
motors on the evaluation board without an operating system.

In order to build the basic example, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   BASIC_EXAMPLE = y
   IIO_EXAMPLE = n

IIO example
~~~~~~~~~~~

The IIO example code for the MAX22200 device in the no-OS project
demonstrates setting up an IIO application by initializing an IIO
descriptor for the MAX22200 device, configuring essential application
parameters, and launching an IIOD server. Users can connect via IIO
clients, such as the IIO Oscilloscope app, to interact with device
settings like offset, sampling frequency, and data scale. The code
manages data conversion from raw ADC readings to voltage signals and
supports seamless device interactions through a UART interface,
incorporating error handling for reliable initialization, execution, and
resource cleanup.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with ADI IIO Oscilloscope Client, please take a
look at: 
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

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

- MAX22200EVKIT
- MAX32665

Connections
^^^^^^^^^^^

=============== ================= ============ =======================
Connection Name MAX22200EVKIT Pin MAX32665 Pin Purpose
UART_TX         P0_0              P5_1         Transmit Data
UART_RX         P0_1              P5_0         Receive Data
SPI_MOSI        P2_0              P1_1         SPI Master Out Slave In
SPI_MISO        P2_1              P1_0         SPI Master In Slave Out
SPI_CLK         P2_2              P1_2         SPI Clock
SPI_CS          P2_3              P1_3         Chip Select
=============== ================= ============ =======================

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project
   make PLATFORM=maxim TARGET=max32665
   # to flash the code
   make run
