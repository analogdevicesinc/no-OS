EVAL-ADE9153A no-OS Example Project
===================================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EV-ADE9153ASHIELDZ`

Overview
--------

The ADE9153A is an energy metering integrated circuit featuring
autocalibration using a shunt resistor, eliminating the need for a
reference meter during calibration. It adheres to IEC 62053-21 standards
for active energy and IEC 62053-23 for reactive energy. The IC includes
three high-performance ADCs with an 88 dB signal-to-noise ratio,
suitable for applications such as single-phase energy meters, smart
power distribution, and machine health monitoring. The operational
temperature ranges from -40°C to +85°C, supporting various industrial
applications.

Applications
------------

- Single-phase energy meters
- Energy and power measurement
- Street lighting
- Smart power distribution system
- Machine health

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EV-ADE9153ASHIELDZ can be powered using a 5V or 3.3V DC supply,
depending on the outputs of the connected Arduino board. It integrates
with the Arduino's power infrastructure through headers, requiring
specific jumper settings to select the appropriate voltage. The shield
supports standard Arduino form factors, ensuring compatibility with a
variety of microcontrollers, including the ESP8266 SoC, allowing for
flexible connection to additional sensors and hardware. Current
specifications are not detailed, but the design supports seamless
integration with the target environment.

Pin Description
~~~~~~~~~~~~~~~

=========== ================= =================
Arduino Pin ADE9153A Signal   Type
ICSP-SCK    SCLK              SPI
ICSP-MISO   MISO              SPI
ICSP-MOSI   MOSI              SPI
GPIO 8      SS (pulled up)    SPI output
GPIO 4      RESET             Output
GPIO 5      User button input Input
GPIO 2      ZX/DREADY/CF2     Input (interrupt)
GPIO 3      IRQ               Input (interrupt)
5 V         Not applicable    Power
3.3 V       Not applicable    Power
=========== ================= =================

No-OS Build Setup
~~~~~~~~~~~~~~~~~

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Data Source Path </projects/eval-ade9153a/src/common>`.

Basic example
~~~~~~~~~~~~~

The eval-ade9153a no-OS example project demonstrates how to interface
with and operate the ADE9153A energy metering IC using the no-OS
platform. The application, implemented in main.c, initializes hardware
interfaces such as UART, GPIO, SPI, and interrupt controllers to
establish communication with the ADE9153A. It configures device
parameters like PGA gain, frequency settings, and energy accumulation,
and performs device-specific initialization.

After setup, the main loop periodically reads measurement data from the
ADE9153A and toggles an LED to indicate activity. The code also handles
device autocalibration, temperature sampling configuration, and error
management.

No-OS Supported Platforms
-------------------------

Maxim
~~~~~

Hardware Used
^^^^^^^^^^^^^

- EV-ADE9153ASHIELDZ
- MAX32690 (AD-APARD32690-SL Development Platform)
- MAX32625PICO microcontroller (flashed with firmware image)

Connections
^^^^^^^^^^^

**Mounting the Shield:**

- The EV-ADE9153ASHIELDZ is mechanically and electrically compatible
  with Arduino R3 headers.

- Align the shield with the Arduino-compatible headers on the
  AD-APARD32690-SL and press it firmly into place.

**Power Configuration:**

- By default, the shield is set for 5V operation (Arduino Uno). The
  AD-APARD32690-SL supplies 3.3V on its Arduino headers. Therefore,
  remove the 5V SPI jumper on the shield and fit the 3V SPI jumper to
  provide the correct voltage to the SPI interface.

**Signal Connections:**

- The ADE9153A shield uses the following header pins:

================== =============== =================
Arduino Header Pin ADE9153A Signal Type
ICSP-SCK           SCLK            SPI clock
ICSP-MISO          MISO            SPI MISO
ICSP-MOSI          MOSI            SPI MOSI
D8                 SS (pulled up)  SPI chip select
D4                 RESET           Output
D2                 ZX/DREADY/CF2   Input (interrupt)
D3                 IRQ             Input (interrupt)
5V                 -               Power (if needed)
3.3V               -               Power
GND                -               Ground
================== =============== =================

**Power Supply:**

- Connect a 5V to 28V external power supply to the AD-APARD32690-SL (via
  P14 for external power or P10 for USB-C).

**Programming:**

- Use a MAX32625PICO or compatible programmer to program the
  AD-APARD32690-SL via its SWD connector (P9).

Build Command
~~~~~~~~~~~~~

.. code-block:: bash

   # to delete current build
   make reset 
   # to build the project
   make TARGET=max32690
   # to flash the code
   make run
