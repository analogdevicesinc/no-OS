MAX14916 no-OS Example Project
==============================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`MAX14916EVKIT`

Overview
--------

The MAX14916EVKIT serves as a comprehensive evaluation platform for the
MAX14916, an octal 1A/Quad 2A high-side switch with integrated
diagnostics. It is designed for efficient prototyping and development,
providing essential SPI communication through a host controller. The kit
supports various application scenarios, including fault detection,
open-wire detection, and operational status monitoring. It also
facilitates flexible operations in MicroPython environments, compatible
with hardware like the TMCM-0960 MotionPy V2.1, enabling users to test
and evaluate functionality seamlessly.

Applications
------------

- Motion control and drives units
- PLC safety digital I/O modules
- PLC standard digital I/O modules

Hardware Specifications
-------------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The MAX14916EVKIT requires a 24V DC power supply, as indicated by
configuration scripts and initialization files. It operates using an SPI
interface with a clock speed up to 100 kHz, reflecting standard digital
communication power usage. Configurable parameters include voltage
thresholds, watchdog timers, and current settings, such as the Open Wire
OFF current source, adjustable through MicroPython scripting. Precise
power control supports diagnostic and fault detection functions,
essential for the kit’s high-side switching operations and SPI
communication.

On-board Connectors
~~~~~~~~~~~~~~~~~~~

+-----------------------------------+-----------------------------------+
| Connector                         | Description                       |
+-----------------------------------+-----------------------------------+
| J9                                | VLED supplied from VDD. If open,  |
|                                   | use an external VLED source       |
|                                   | between VLED test point TP8 and   |
|                                   | GND (TP9).                        |
+-----------------------------------+-----------------------------------+
| J6                                | VL is supplied from USB2GPIO      |
|                                   | board (3.3 V). Optionally use an  |
|                                   | external source between VL (TP6)  |
|                                   | and GND (TP5).                    |
+-----------------------------------+-----------------------------------+
| J8                                | Internal 3.3 V VA regulator       |
|                                   | enabled. If 1-2 position, the     |
|                                   | internal regulator is disabled    |
|                                   | (REGEN = GND).                    |
+-----------------------------------+-----------------------------------+
| J4                                | Set address bit A0 = 0 by         |
|                                   | default. Connect 1-2 to set       |
|                                   | address bit A0 = 1.               |
+-----------------------------------+-----------------------------------+
| J5                                | Set address bit A1 = 0 by         |
|                                   | default. Connect 1-2 to set       |
|                                   | address bit A1 = 1.               |
+-----------------------------------+-----------------------------------+
| J3                                | Connection to SPI bus,            |
|                                   | facilitates communication with    |
|                                   | external devices.                 |
+-----------------------------------+-----------------------------------+
| J7                                | Alternative SPI bus connector,    |
|                                   | used for standalone applications. |
+-----------------------------------+-----------------------------------+
| J10                               | Terminal block for connecting     |
|                                   | loads to each switch channel.     |
+-----------------------------------+-----------------------------------+

Digital Communication Pins
~~~~~~~~~~~~~~~~~~~~~~~~~~

======== ========================== =========================
Pin Name Pin Role                   Description
SPI_CLK  Clock Input                Serial Clock Line
SPI_CS   Chip Select                Activates the Device
SPI_MOSI Master Output, Slave Input Data From Master to Slave
SPI_MISO Master Input, Slave Output Data From Slave to Master
======== ========================== =========================

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from: 
:git-no-OS:`Project Common Data Path </projects/max14916/src/common>`

The macros used in Common Data are defined in platform specific files
found in: 
:git-no-OS:`Project Platform Configuration Path </projects/max14916/src/platform>`

Basic example
~~~~~~~~~~~~~

The basic example code for the MAX14916 project demonstrates device
initialization and control via SPI communication. It initializes the
MAX14916, making use of functions like ``max14916_init`` to establish a
communication link. LED control is implemented in a loop, showcasing
on/off operations for functional testing. Key device parameters, such as
the Flash LED time, Watch-Dog Timer, and voltage thresholds, are
configured using methods like ``max14916_set_wd``. Configuration
settings are validated by reading from the device registers, and any
errors are logged, providing a practical example of device interfacing
and diagnostics.

In order to build the basic example, make sure you have the following
configuration in the Makefile:


.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   BASIC_EXAMPLE = y
   IIO_EXAMPLE = n

IIO example
~~~~~~~~~~~

The IIO example code for the MAX14916 platform showcases initializing
and utilizing the IIO interface to communicate with the MAX14916 device
via SPI. It configures the ``max14916_iio_desc`` structure by enabling
specific channels for device configuration. The application is set up
using ``iio_app_init_param`` for managing settings, including UART
initialization. The ``max14916_iio_init`` function prepares the device,
and the IIO framework executes the application. Device descriptors
perform specific IIO operations, ensuring accurate SPI communication,
diagnostics, and real-time data acquisition, with error handling and
cleanup for reliable operation.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

In order to build the IIO project, make sure you have the following
configuration in the :git-no-OS:`Makefile </projects/max14916/Makefile>`:

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

- MAX14916EVKit
- MAX32666FTHR (MAX32665) or AD-APARD32690-SL (MAX32690)

Connections
^^^^^^^^^^^

+-----------------+-----------------+-----------------+-----------------+
| Function /      | MAX14916 EV Kit | MAX32666FTHR    | Notes           |
| Signal          | Pin             | Pin             |                 |
+-----------------+-----------------+-----------------+-----------------+
| **Main Power    | VDD (J10)       | External +24V   | Required to     |
| Input**         |                 | supply          | power MAX14916  |
|                 |                 |                 | outputs         |
+-----------------+-----------------+-----------------+-----------------+
| **Ground        | GND (J10 / J3 / | Any GND pin     | Must share      |
| (GND)**         | J7)             |                 | common ground   |
+-----------------+-----------------+-----------------+-----------------+
| **Logic Supply  | VDDIO (J3/J7)   | 3.3V or 5V pin  | Logic level     |
| (VDDIO)**       |                 |                 | selectable      |
|                 |                 |                 | (2.5–5.5V       |
|                 |                 |                 | supported)      |
+-----------------+-----------------+-----------------+-----------------+
| **SPI Clock     | SCLK (J3/J7)    | SCLK/SCK pin    | SPI clock from  |
| (SCLK)**        |                 |                 | MAX32666FTHR    |
+-----------------+-----------------+-----------------+-----------------+
| **MOSI (SDI)**  | SDI (J3/J7)     | MOSI pin        | Master → Slave  |
|                 |                 |                 | data            |
+-----------------+-----------------+-----------------+-----------------+
| **MISO (SDO)**  | SDO (J3/J7)     | MISO pin        | Slave → Master  |
|                 |                 |                 | data            |
+-----------------+-----------------+-----------------+-----------------+
| **Chip Select   | CS (J3/J7)      | Any GPIO        | Configure GPIO  |
| (CS)**          |                 | (user‑assigned) | as CS in        |
|                 |                 |                 | firmware        |
+-----------------+-----------------+-----------------+-----------------+
| **Optional:     | EN (J3/J7)      | Any GPIO        | Enables device  |
| EN**            |                 |                 | features        |
+-----------------+-----------------+-----------------+-----------------+
| **Optional:     | SYNCH (J3/J7)   | Any GPIO        | Synchronization |
| SYNCH**         |                 |                 | input           |
+-----------------+-----------------+-----------------+-----------------+
| **Optional:     | WD (J3/J7)      | Any GPIO        | Watchdog        |
| WD**            |                 |                 | monitoring      |
+-----------------+-----------------+-----------------+-----------------+
| **Jumpers**     | J4, J5          | —               | Set SPI address |
|                 |                 |                 | (default 00)    |
+-----------------+-----------------+-----------------+-----------------+

============= ==============================
MAX14916EVKit AD-APARD32690-SL SPI PMOD (P8)
SCLK (J3/J7)  SCLK
SDI (J3/J7)   MOSI
SDO (J3/J7)   MISO
CS (J3/J7)    CS
GND           GND
============= ==============================

Build Command
^^^^^^^^^^^^^

when MAX32665 (MAX32666FTHR) is used as controller:

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project
   make PLATFORM=maxim TARGET=max32655
   # to flash the code
   make run

when AD-APARD32690-SL is used as controller:

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project
   make PLATFORM=maxim TARGET=max32690
   # to flash the code
   make run
