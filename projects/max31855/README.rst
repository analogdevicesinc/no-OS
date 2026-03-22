MAX31855 no-OS Example Project
==============================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`MAX31855PMB1`

Overview
--------

The MAX31855PMB peripheral module provides the necessary hardware 
to interface the MAX31855 cold-junction compensated
thermocouple-to-digital converter to any system that utilizes
Pmod-compatible expansion ports. The MAX31855 performs cold-junction
compensation and digitizes the signal from a thermocouple. Versions of
it are available that operate with a K-, J-, N-, T-, R-, or E-type
thermocouple.

This module is set up to operate with a K-type thermocouple. The data is
output in a signed 14-bit, SPI-compatible, read-only format. This
converter resolves temperatures to 0.25°C, allows readings as high as
+1800°C and as low as -270°C, and exhibits thermocouple accuracy of ±2°C
for temperatures ranging from -200°C to +700°C for K-type thermocouples.

Applications
------------

- Appliances
- Automotive
- HVAC
- Industrial

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

When using the MAX31855 PMOD board, the 3.3V power comes directly from
the host board it is connected to. The power from the host is generally
capable of providing up to 100 mA at 3.3V.

Digital Interface (PMOD)
~~~~~~~~~~~~~~~~~~~~~~~~

The MAX31855PMB1 peripheral module can plug directly into a
Pmod-compatible port (configured for SPI) through connector J1.
Connector J1 provides connection of the module to the host.

Pin Definitions
~~~~~~~~~~~~~~~

+-----------------------+-----------------------+-----------------------+
| PIN                   | SIGNAL                | DESCRIPTION           |
+-----------------------+-----------------------+-----------------------+
| 1                     | SS                    | Chip enable. Must be  |
|                       |                       | asserted low to       |
|                       |                       | enable the SPI        |
|                       |                       | interface.            |
+-----------------------+-----------------------+-----------------------+
| 2                     | N.C.                  | Not connected         |
+-----------------------+-----------------------+-----------------------+
| 3                     | MISO                  | Serial‑data output    |
+-----------------------+-----------------------+-----------------------+
| 4                     | SCK                   | Serial‑clock input    |
+-----------------------+-----------------------+-----------------------+
| 5                     | GND                   | Ground                |
+-----------------------+-----------------------+-----------------------+
| 6                     | VCC                   | Power supply          |
+-----------------------+-----------------------+-----------------------+

No-OS Build Setup
-----------------

See the build instructions at: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is in:
:git-no-OS:`no-OS/projects/max31855/src/common`.
  
The macros used in Common Data are defined in platform-specific files
in: :git-no-OS:`no-OS/projects/max31855/src/platform`.

Basic Example
~~~~~~~~~~~~~

The Basic Example initializes the MAX31855 thermocouple-to-digital
converter, reads thermocouple and ambient temperature data in a loop,
prints the results to the console, and updates readings every two
seconds. If initialization or data reading fails, it logs an error and
cleans up resources. This example serves as a simple demonstration of
real-time temperature acquisition and reporting using the MAX31855
device.

In order to build the basic example, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

   # Enable the Basic example and disable the IIO example
   BASIC_EXAMPLE=y 
   IIO_EXAMPLE=n

IIO Example
~~~~~~~~~~~

The IIO example in the MAX31855 no-OS project demonstrates how to
interface the MAX31855 thermocouple-to-digital converter with the
Industrial Input/Output (IIO) framework. The implementation initializes
the MAX31855 IIO device and sets up the necessary device and application
parameters, including a data buffer for IIO operations. Using structures
such as max31855_iio_dev_init_param and iio_app_device, the example
configures the IIO application context and devices. The main function,
iio_example_main, initializes the required resources and then runs the
application loop with iio_app_run, allowing continuous communication
between the MAX31855 and IIO framework clients.

If you are not familiar with ADI IIO Application, please take a look at: 
:dokuwiki:`IIO No-OS </resources/tools-software/no-os-software/iio>`

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at 
:dokuwiki:`IIO Oscilloscope </resources/tools-software/linux-software/iio_oscilloscope>`.

The No-OS IIO Application together with the No-OS IIO MAX31855 driver
take care of all the back-en logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown
in the :git-no-OS:`IIO Example </projects/max31855/src/examples/iio_example>`.

In order to build the IIO project make sure you have the following
configuration in the :git-no-OS:`Makefile </projects/max31855/Makefile>`

.. code-block:: bash

   # Enable the IIO example and disable the Basic example
   BASIC_EXAMPLE=n 
   IIO_EXAMPLE=y

No-OS Supported Platforms
-------------------------

Maxim Platform
~~~~~~~~~~~~~~

Hardware Used
^^^^^^^^^^^^^

- MAX31855PMB1
- MAX32655FTHR / MAX32650FTHR
- MAX32625PICO
- FTHR-PMD-INTZ

Connections
^^^^^^^^^^^

**MAX32655FTHR**

1. Connect **MAX32655FTHR** with the **FTHR-PMD-INTZ**. Note that MAXIM
   feather board should have stacking headers for feather board where the
   interposer board will be connected.

2. Connect **MAX31855PMB1** to the **FTHR-PMD-INTZ.**

===================== =================
MAX31855PMB1          FTHR-PMD-INTZ SPI
Pin 1 (Chip Enable)   CS
Pin 2 (Not connected) MOSI
Pin 3 (MISO)          MISO
Pin 4 (SCK)           SCK
Pin 5 (GND)           GND
Pin 6 (VCC)           VCC
===================== =================

3. Power up the **MAX32655FTHR** by connecting it to your laptop using a
   micro-USB cable.

--------------

**MAX32650FTHR**

1. Using a 10-pin ribbon cable, connect the **MAX32625PICO** to the
   **MAX32650FTHR**.

2. Connect **MAX32650FTHR** to the **FTHR-PMD-INTZ**.

3. Connect **MAX31855PMB1** to the **FTHR-PMD-INTZ.**

===================== =================
MAX31855PMB1          FTHR-PMD-INTZ SPI
Pin 1 (Chip Enable)   CS
Pin 2 (Not connected) MOSI
Pin 3 (MISO)          MISO
Pin 4 (SCK)           SCK
Pin 5 (GND)           GND
Pin 6 (VCC)           VCC
===================== =================

4. Power up the **MAX32650FTHR** by connecting it to your laptop using
   micro-USB. Connect **MAX32625PICO** to your laptop as well.

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # Delete the current build
   make reset
   # Build the project
   make PLATFORM=maxim TARGET=max32650
   # Flash the code
   make run
