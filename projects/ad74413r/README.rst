AD74413R no-OS Example Project
==============================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-AD74413R`

Overview
--------

The EVAL-AD74413R is a quad-channel, software-configurable evaluation
board designed to assess the AD74413R device. This board supports
multiple power sources, including AVDD to power the ADP2360 switching
regulator and DVCC through the ADP1720, with jumper configurations
enabling specific power paths and functionalities. The device features
SPI connectivity and an on-board 2.5V ADR525 reference. It interfaces
with a PC via the SDP-S board, providing a 3.3V supply necessary for
communication. The EV-AD74413RSDZ requires an AVDD operating supply
of 14V to 28.8V. The board includes several jumper settings for
flexibility in testing different configurations and digital input/output
functionalities. It targets applications like process control, factory
automation, motor drives, and building control systems, enabling
engineers to evaluate device performance efficiently.

Applications
------------

- Process control
- Factory automation
- Motor drives
- Building control systems

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD74413R board requires multiple power sources for optimal
operation. The EV-AD74413RSDZ requires an AVDD operating supply of 14V
to 28.8V. When the EV-AD74413RSDZ is connected to the PC, the PC powers
the SDP-S. AVDD powers the ADP2360 switching regulator when JP1 is
inserted. To provide 5V via this regulator, JP3 should also be inserted.
The DVCC supply comes from the ADP1720 if JP6 is in place. For the
communication interface, the SDP-S board, connected through JP10,
supplies 3.3V. Jumper settings, including JP8 and JP9, configure digital
input/output functions and manage the AD74413R's internal reference,
allowing users to enable or disable specific power paths as required.

Connections
~~~~~~~~~~~

========= =========================================== ================
Connector Function                                    Default Position
JP1       Powers the ADP2360 from AVDD supply         Inserted
JP2       Supplies 5V via Arduino connector           Not Inserted
JP3       Supplies 5V via ADP2360                     Inserted
JP4       Powers the ADP1720 with 5V                  Inserted
JP5       Supplies DVCC via Arduino connector         Not Inserted
JP6       Supplies DVCC via ADP1720                   Inserted
JP7       Connects IOVDD to DVCC                      Inserted
JP8       REFIN pin to AD74413R output                Not Inserted
JP9       REFIN pin to REFOUT (internal reference)    Inserted
JP10      SDP-S board provides 3.3V                   Inserted
JP11      Arduino connector provides 3.3V             Not Inserted
JP12      Triggers reset via Arduino reset function   Not Inserted
JP13      Resets AD74413R via EVAL-AD74413RSDZ button Inserted
JP14      Triggers reset via Arduino GPIO             Not Inserted
JP15      Supplies 5V to SDP-S board                  Not Inserted
========= =========================================== ================

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
:git-no-OS:`Project Common Data Path </projects/ad74413r/src/common>`.

The macros used in Common Data are defined in platform specific files
found in
:git-no-OS:`Project Platform Configuration Path </projects/ad74413r/src/platform>`.

Dummy example
~~~~~~~~~~~~~

The ``dummy_example.c`` file in the AD74413R project demonstrates the
initialization and operation of the AD74413R device using the
``ad74413r_init_param`` for setup. This includes enabling ADC channels
and configuring them for digital input, voltage input and output, and
current input. In this example, once initialized, the device enters an
infinite loop, continuously reading channel statuses to measure voltage,
current, and temperature for functionality validation. Connected via
UART, it logs channel measurements and statuses for real-time
monitoring. 

In order to build the basic example, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

   EXAMPLE = dummy

IIO Example
~~~~~~~~~~~

The IIO example code for the AD74413R project illustrates the setup and
operation of the IIO subsystem with the ADI IIO Oscilloscope. It
involves initializing the IIO descriptor via ``ad74413r_iio_init``,
configuring interrupt controllers and hardware triggers using
``no_os_irq_ctrl_init`` and ``iio_hw_trig_init``, and defining device
channels for tasks like digital I/O. A data buffer is set up with
``iio_data_buffer`` for IIO activities. The ``iio_app_init`` function
initializes the IIO application with pre-configured devices and
triggers. Post-initialization, ``iio_app_run`` facilitates continuous
execution, managing data processing and communication with the IIO
Oscilloscope, enabling real-time data acquisition and analysis.

.. code-block:: bash

   EXAMPLE = iio_example

IIO Trigger Example
~~~~~~~~~~~~~~~~~~~

The iio trigger example code for the AD74413R initializes the device
with configured channels, sets up hardware triggers using GPIO
interrupts, and integrates these triggers into the Industrial I/O (IIO)
subsystem. The main routine configures input and output channels,
initializes the required IIO, interrupt, and trigger descriptors, and
attaches the hardware trigger to GPIO events. Then, it runs the IIO
application loop with iio_app_run, which processes data acquisition and
communication based on hardware trigger events, demonstrating
synchronized data capture and IIO trigger integration for the AD74413R.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with ADI IIO Oscilloscope Client, please take a
look at: `IIO
Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

To build the IIOD demo, add the following flag when invoking make. This
will build the IIOD server and the IIO section of the driver:

.. code-block:: bash

   EXAMPLE = iio_trigger_example

No-OS Supported Platforms
------------------------

- Maxim
- STM32

Maxim
~~~~~~

Hardware Used
^^^^^^^^^^^^^

- EV-AD74413RSDZ evaluation board
- Maxim Board like MAX32655 or MAX78000

Connections
^^^^^^^^^^^

+-----------------------+-----------------------+-----------------------+
| Description           | EVAL-AD74413R Pin     | MAX32655 Connection   |
+-----------------------+-----------------------+-----------------------+
| Serial clock for      | SCLK                  | Connected to SPI SCLK |
| synchronous data      |                       |                       |
| communication         |                       |                       |
+-----------------------+-----------------------+-----------------------+
| Master In Slave Out   | MISO                  | Connected to SPI MISO |
| for data reception    |                       |                       |
+-----------------------+-----------------------+-----------------------+
| Master Out Slave In   | MOSI                  | Connected to SPI MOSI |
| for data transmission |                       |                       |
+-----------------------+-----------------------+-----------------------+
| Chip Select to enable | CS                    | Connected to a        |
| the device            |                       | selected GPIO         |
+-----------------------+-----------------------+-----------------------+
| Alert pin for         | ALERT                 | Connected to an       |
| signaling data        |                       | interrupt-capable     |
| readiness             |                       | GPIO                  |
+-----------------------+-----------------------+-----------------------+
| To perform a reset on | Reset (S1)            | Connected to a        |
| the AD74413R device   |                       | dedicated GPIO for    |
|                       |                       | reset control         |
+-----------------------+-----------------------+-----------------------+
| General Purpose       | GPO_A                 | Connected to a        |
| Output A              |                       | specific GPIO or      |
|                       |                       | control pin           |
+-----------------------+-----------------------+-----------------------+

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project, uncomment to select target
   make PLATFORM=maxim # TARGET=max32655 # TARGET=max78000
   # to flash the code
   make run

--------------

STM32
~~~~~

Hardware Used
^^^^^^^^^^^^^

- EV-AD74413RSDZ evaluation board
- STM32 microcontroller board

Connections
^^^^^^^^^^^

+-----------------------+-----------------------+-----------------------+
| EV-AD74413RSDZ Pin    | STM32 Connection      | Function              |
+-----------------------+-----------------------+-----------------------+
| SCLK                  | SPI SCLK              | Serial clock for SPI  |
|                       |                       | communication         |
+-----------------------+-----------------------+-----------------------+
| MISO                  | SPI MISO              | Master In Slave Out   |
+-----------------------+-----------------------+-----------------------+
| MOSI                  | SPI MOSI              | Master Out Slave In   |
+-----------------------+-----------------------+-----------------------+
| CS                    | GPIO (configured for  | Chip select signal to |
|                       | chip select)          | enable device         |
+-----------------------+-----------------------+-----------------------+
| GPIO Interrupt        | Interrupt-capable     | Alert for data        |
|                       | GPIO                  | readiness             |
+-----------------------+-----------------------+-----------------------+
| S1                    | Dedicated GPIO        | Resets the AD74413R   |
|                       |                       | device                |
+-----------------------+-----------------------+-----------------------+

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   cd no-OS/projects/ad74413r

   # to delete current build
   make reset
   # to build the project
   make PLATFORM=stm32
   # to flash the code
   make run
   # to debug the code
   make debug
