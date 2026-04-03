IIO_ADPD1080 no-OS Example Project
===================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* `EVAL-CN0569-PMDZ <https://www.analog.com/EVAL-CN0569-PMDZ>`_
* `EVAL-ADICUP3029 <https://www.analog.com/EVAL-ADICUP3029>`_ (carrier board)

Overview
--------

The EVAL-CN0569-PMDZ is a 4-layer printed circuit board (PCB) designed
in a PMOD form factor that enables evaluation of the CN0569 Infrared
Light Angle Sensor Module for Gesture Recognition. The board integrates
the ADPD1080 photometric front end operating in tandem with two ADPD2140
infrared light angle sensors to create a complete gesture sensing
solution. Using the I2C Pmod hardware interface with a compact PCB
footprint of 2.7 in x 0.8 in, the evaluation board connects to a
controller such as the EVAL-ADICUP3029 to run firmware that initializes
the hardware and launches an IIO server. A companion application running
on a host computer communicates over a USB serial connection to sample
the photodiode signals and execute gesture sensing algorithms.

Applications
------------

* Wearable health and fitness monitors
* Clinical measurements
* Industrial monitoring
* Gesture recognition
* Proximity sensing

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-CN0569-PMDZ is powered entirely through the Pmod VCC pin of
its I2C Pmod connector (P1), which is supplied by the connected
controller board (e.g., EVAL-ADICUP3029). The board accepts a VCC input
consistent with standard Pmod voltage levels (typically 3.3 V). An
on-board low-dropout (LDO) voltage regulator steps down the Pmod VCC to
generate a regulated 1.8 V supply, which powers the ADPD1080 AVDD and
DVDD rails. The infrared LED supply (VLED) is shorted to Pmod VCC by
default. No additional external power supply is required.

Board Connectors
~~~~~~~~~~~~~~~~

**I2C Pmod Connector (P1)**

A 1 x 6, 0.1 in (2.54 mm) right-angle male header following the
standard I2C Pmod specification.

+---------+---------+-----------------------------------------------+
| Pin     | Name    | Function                                      |
+---------+---------+-----------------------------------------------+
| 1       | INT     | ADPD1080 GPIO0 (interrupt / general-purpose)  |
+---------+---------+-----------------------------------------------+
| 2       | RESET   | No connection                                 |
+---------+---------+-----------------------------------------------+
| 3       | SCL     | I2C clock                                     |
+---------+---------+-----------------------------------------------+
| 4       | SDA     | I2C data                                      |
+---------+---------+-----------------------------------------------+
| 5       | GND     | Ground                                        |
+---------+---------+-----------------------------------------------+
| 6       | VCC     | Pmod VCC input power                          |
+---------+---------+-----------------------------------------------+

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Source Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/iio_adpd1080/src>`__.

IIO Example
~~~~~~~~~~~

The IIO example launches an IIOD server on the EVAL-ADICUP3029, allowing
a host computer to connect via a serial USB context and interact with
the ADPD1080 device through any IIO-compatible client. Using IIO
Oscilloscope, users can access and adjust device settings such as
sampling frequency, channel offsets, and data scale factors for each
photodiode input.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The IIO example is the default build configuration (``IIOD=y`` is set in
``src.mk``). Two variants are available:

**IIO over UART** (default):

.. code-block:: bash

   # IIOD is enabled by default, no additional flags needed
   IIOD = y

**IIO over WiFi:**

.. code-block:: bash

   # Enable networking for WiFi-based IIO communication
   IIOD = y
   NETWORKING = y

No-OS Supported Platforms
-------------------------

ADuCM
~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-CN0569-PMDZ <https://www.analog.com/EVAL-CN0569-PMDZ>`_
* `EVAL-ADICUP3029 <https://www.analog.com/EVAL-ADICUP3029>`_
* Micro-USB to USB cable
* Breadboard wire (for INT/GPIO0 connection)

Connections
^^^^^^^^^^^

**Step 1 - Pmod Connector (P9):**

Connect the EVAL-CN0569-PMDZ to the EVAL-ADICUP3029 via the P9 PMOD
connector. Use pins 3 to 6 of the CN0569 PMOD connector (P1) to connect
to the top row of the P9 PMOD connector on the ADICUP3029 board.

+-----------------+-----------------+-----------------+-----------------+
| CN0569 P1 Pin   | CN0569 Signal   | ADICUP3029 P9   | ADICUP3029      |
|                 |                 | Pin             | Signal          |
+-----------------+-----------------+-----------------+-----------------+
| Pin 3           | SCL (I2C Clock) | Top Row, Pin 1  | SCL             |
+-----------------+-----------------+-----------------+-----------------+
| Pin 4           | SDA (I2C Data)  | Top Row, Pin 2  | SDA             |
+-----------------+-----------------+-----------------+-----------------+
| Pin 5           | GND             | Top Row, Pin 5  | GND             |
+-----------------+-----------------+-----------------+-----------------+
| Pin 6           | VCC             | Top Row, Pin 6  | VCC (3.3 V)     |
+-----------------+-----------------+-----------------+-----------------+

**Step 2 - INT/GPIO0 Wire Connection:**

Use a breadboard wire to connect pin 1 of the CN0569 PMOD connector
(P1, INT / ADPD1080 GPIO0) to pin 5 of the ADICUP3029 P7 header. This
connects the ADPD1080 GPIO0 signal to an interrupt-capable GPIO on the
ADuCM3029 microcontroller, which is required for clock calibration.

+-----------------+-----------------+-----------------+-----------------+
| CN0569          | CN0569 Pin /    | ADICUP3029      | ADICUP3029 Pin  |
| Connector       | Signal          | Connector       | / Signal        |
+-----------------+-----------------+-----------------+-----------------+
| P1 (Pmod)       | Pin 1 / INT     | P7 Header       | Pin 5 / IRQ     |
|                 | (GPIO0)         |                 |                 |
+-----------------+-----------------+-----------------+-----------------+

**Step 3 - USB Connection:**

Connect a micro-USB cable from the P10 connector of the EVAL-ADICUP3029
to a USB port on the host computer. This provides power to the board and
establishes the serial communication link used by the IIO server.

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # remove sp characters from the build directory
   make reset
   # build the project
   make PLATFORM=aducm3029
   # flash the code
   make run
