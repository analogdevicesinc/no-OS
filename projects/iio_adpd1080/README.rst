IIO_ADPD1080 no-OS Example Project
==================================

.. contents::
    :depth: 3

Supported Evaluation Board
--------------------------

- :adi:`EVAL-CN0569-PMDZ`

Supported Carrier Board
-----------------------

- :adi:`EVAL-ADICUP3029`

Overview
--------

The EVAL-CN0569-PMDZ is a 4-layer printed circuit board (PCB) designed
in a PMOD form factor that enables evaluation of the CN0569 Infrared
Light Angle Sensor Module for Gesture Recognition. The board integrates
the ADPD1080 photometric front end operating in tandem with two ADPD2140
infrared light angle sensors to create a complete gesture sensing
solution. Using the I2C Pmod hardware interface with a compact PCB
footprint of 2.7 in × 0.8 in, the evaluation board connects to a
controller such as the EVAL-ADICUP3029 to run firmware that initializes
the hardware and launches an IIO server. A companion Python application
running on a host computer communicates over a USB serial connection to
sample the photodiode signals and execute gesture sensing algorithms,
enabling detection of directional gestures through the analysis of
infrared light reflected from the surrounding environment.

Applications
------------

- Wearable health and fitness monitors
- Clinical measurements (e.g., SpO₂)
- Industrial monitoring
- Background light measurements
- Gesture recognition
- Proximity sensing

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-CN0569-PMDZ is powered entirely through the Pmod VCC pin of its
I²C Pmod connector (P1), which is supplied by the connected controller
board (e.g., EVAL-ADICUP3029). The board accepts a VCC input consistent
with standard Pmod voltage levels (typically 3.3V). An on-board
low-dropout (LDO) voltage regulator steps down the Pmod VCC to generate
a regulated 1.8V supply (accessible at the 1V8 test point), which
powers the ADPD1080’s AVDD and DVDD rails. The infrared LED supply
(VLED) is shorted to Pmod VCC by default, though it can be separated for
custom LED drive configurations. No additional external power supply is
required during normal operation.

Connections
~~~~~~~~~~~

The following on-board connectors and test points are available on the
EVAL-CN0569-PMDZ:

**I²C Pmod Connector (P1)**

The primary interface connector. A 1 × 6, 0.1 in. (2.54 mm) right-angle
male header following the standard I²C Pmod specification used to
connect the evaluation board to a controller board.

+-----------------------+-----------------------+-----------------------+
| **Pin Number**        | **Pin Name**          | **Pin Function on     |
|                       |                       | EVAL-CN0569-PMDZ**    |
+-----------------------+-----------------------+-----------------------+
| 1                     | **INT**               | General-Purpose       |
|                       |                       | Input/Output 0.       |
|                       |                       | Actual function       |
|                       |                       | depends on ADPD1080   |
|                       |                       | GPIO0 configuration.  |
+-----------------------+-----------------------+-----------------------+
| 2                     | **RESET**             | No connection.        |
+-----------------------+-----------------------+-----------------------+
| 3                     | **SCL**               | I²C SCL. I²C Clock    |
|                       |                       | from the controller   |
|                       |                       | board to the          |
|                       |                       | ADPD1080.             |
+-----------------------+-----------------------+-----------------------+
| 4                     | **SDA**               | I²C SDA. I²C Data     |
|                       |                       | from the controller   |
|                       |                       | board to the          |
|                       |                       | ADPD1080.             |
+-----------------------+-----------------------+-----------------------+
| 5                     | **GND**               | Ground.               |
+-----------------------+-----------------------+-----------------------+
| 6                     | **VCC**               | Pmod VCC. Input power |
|                       |                       | to the                |
|                       |                       | EVAL-CN0569-PMDZ.     |
+-----------------------+-----------------------+-----------------------+

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken from the 
:git-no-OS:`Project Source Path </projects/iio_adpd1080/src/>`.

IIO Example
~~~~~~~~~~~

The IIO example launches an IIOD (IIO Daemon) server on the
EVAL-ADICUP3029, allowing a host computer to connect via a serial USB
context and interact with the ADPD1080 device through any IIO-compatible
client. Using the IIO Oscilloscope application, users can access and
adjust device settings such as sampling frequency, channel offsets, and
data scale factors for each photodiode input, enabling real-time
visualization and acquisition of the optical measurement data captured
by the ADPD1080 and ADPD2140 sensors.

If you are not familiar with ADI IIO Application, please take a look at: 
:dokuwiki:`IIO No-OS </resources/tools-software/no-os-software/iio>`

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
:dokuwiki:`IIO Oscilloscope </resources/tools-software/linux-software/iio_oscilloscope>`

The No-OS IIO Application together with the No-OS IIO ADPD1080 driver
take care of all the backend logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
:git-no-OS:`IIO Example </projects/iio_adpd1080/src/examples/iio_example>`.

In order to build the IIO project make sure you have the following
configuration in the :git-no-OS:`Makefile </projects/iio_adpd1080/Makefile>`:

.. code-block:: bash

   IIOD=y

No-OS Supported Platforms
-------------------------

ADuCM
~~~~~

Hardware Used
^^^^^^^^^^^^^

The following hardware is required to run the IIO_ADPD1080 no-OS project
on the supported carrier board:

- EVAL-CN0569-PMDZ
- EVAL-ADICUP3029
- Micro USB to USB cable
- Mother-to-father breadboard wire (for INT/GPIO0 connection)
- PC or laptop with a USB port

Connections
^^^^^^^^^^^

Follow these steps to connect the EVAL-CN0569-PMDZ to the
EVAL-ADICUP3029 carrier board:

**Step 1 – Pmod Connector (P9):**

Connect the EVAL-CN0569-PMDZ to the EVAL-ADICUP3029 via the P9 PMOD
connector. Use pins 3 to 6 of the CN0569 PMOD connector (P1) to connect to
the top row of the P9 PMOD connector on the ADICUP3029 board. Pins 1 and
2 of P1 will remain unconnected toward the center of the motherboard
(due to the older 2×4 Pmod header standard on the ADICUP3029).

+-----------------+-----------------+-----------------+-----------------+
| **CN0569 P1     | **CN0569        | **ADICUP3029 P9 | **ADICUP3029    |
| Pin**           | Signal**        | Pin**           | Signal**        |
+-----------------+-----------------+-----------------+-----------------+
| Pin 3           | SCL (I²C Clock) | Top Row, Pin 1  | SCL             |
+-----------------+-----------------+-----------------+-----------------+
| Pin 4           | SDA (I²C Data)  | Top Row, Pin 2  | SDA             |
+-----------------+-----------------+-----------------+-----------------+
| Pin 5           | GND             | Top Row, Pin 5  | GND             |
+-----------------+-----------------+-----------------+-----------------+
| Pin 6           | VCC             | Top Row, Pin 6  | VCC (3.3V)      |
+-----------------+-----------------+-----------------+-----------------+

**Step 2 – INT/GPIO0 Wire Connection:**

Use a mother-to-father breadboard wire to connect pin 1 of the CN0569
PMOD connector (P1, INT / ADPD1080 GPIO0) to pin 5 of the ADICUP3029 P7
header. This connects the ADPD1080 GPIO0 signal to an interrupt-capable
GPIO on the ADuCM3029 microcontroller, which is required for clock
calibration.

+-----------------+-----------------+-----------------+-----------------+
| **CN0569        | **CN0569 Pin /  | **ADICUP3029    | **ADICUP3029    |
| Connector**     | Signal**        | Connector**     | Pin / Signal**  |
+-----------------+-----------------+-----------------+-----------------+
| P1 (Pmod)       | Pin 1 / INT     | P7 Header       | Pin 5 / IRQ     |
|                 | (GPIO0)         |                 | (Interrupt      |
|                 |                 |                 | GPIO)           |
+-----------------+-----------------+-----------------+-----------------+

**Step 3 – USB Connection:**

Connect a micro-USB cable from the P10 connector of the EVAL-ADICUP3029
to a USB port on the host computer. This connection provides power to
the board and establishes the serial communication link used by the IIO
server.

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   cd no-OS/projects/iio_adpd1080

   export PLATFORM=aducm3029
   # to delete current build 
   make reset 
   # to build the project 
   make
   # to flash the code 
   make run
