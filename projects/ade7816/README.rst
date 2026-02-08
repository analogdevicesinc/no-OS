ADE7816 no-OS Example Project
=============================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-ADE7816EBZ`

Overview
--------

The EVAL-ADE7816 is an evaluation board designed to facilitate the
assessment and prototyping of the ADE7816 energy metering IC from Analog
Devices. This board provides users with access to the various
functionalities of the ADE7816, allowing for comprehensive evaluation of
its performance in monitoring electric properties such as voltage,
current, and power. Configured for simple connectivity with other
systems, the evaluation board supports interfacing via test points,
connectors, and communication ports, making it suitable for both
standalone operation and integration into broader testing environments.
The board aids in analyzing the precision of energy measurements and
testing the device in real-world scenarios without the need for
additional complex setups.

Applications
------------

- Energy metering applications
- Power monitoring systems
- Application in electronic designs requiring precise power measurement

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-ADE7816 Evaluation Board requires a 3.3V power supply. By
default, power is supplied via a USB connection, feeding the NXP LPC2368
microcontroller and one side of the isocouplers through the JP24 jumper
positioned at 2, 3. For external power use, adjust JP24 to position 1, 2
and connect a 3.3V source to the P12 connector. This configuration
maintains the same power domain as the default USB setup. The board is
ready for COM port detection without an external supply if jumper
settings are correctly configured.

Device Interrupt
~~~~~~~~~~~~~~~~
To run the ADE7816 no-OS project with the AD-APARD32690-SL controller board, 
the IRQ pin from the EVAL-ADE7816 must be connected to a GPIO pin on the 
AD-APARD32690-SL platform. This GPIO pin is configured to detect interrupts 
and trigger an interrupt service routine (ISR) when the IRQ pin is activated, 
signaling events like data ready or fault conditions. The ISR handles the interrupt 
by reading the ADE7816’s status, processing the data, and clearing the interrupt flag. 
This setup ensures real-time communication between the ADE7816 and the controller 
for efficient data processing and error management.


No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`.

No-OS Supported Examples
------------------------

The initialization data used in the example is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ade7816/src/common>`__.

The macros used in Common Data are defined in platform specific files
found in the 
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ade7816/src/platform>`__.

Basic Example
~~~~~~~~~~~~~

The basic example code for the ADE7816 involves initializing the SPI
interface, which configures necessary parameters such as device ID, SPI
mode, clock speed, and chip select. GPIO setup includes managing
interrupt handling crucial for energy measurement operations. The code
demonstrates measuring energy by configuring the ADE7816 for
zero-crossing detection across different channels to set phases and
reading active/reactive energy values. It converts raw RMS readings into
microamperes or microvolts for display. Interrupts control the
measurement cycle, pausing operations when triggered to ensure precise
energy readings.

No-OS Supported Platforms
-------------------------

Maxim
~~~~~

Hardware Used
^^^^^^^^^^^^^

- EVAL-ADE7816
- AD-APARD32690-SL

Connections
^^^^^^^^^^^

Supplying a regulated 3.3V DC to the EVAL-ADE7816 board is achieved via
connector P9, which powers the ADE7816 IC and the non-isolated side of
the circuit; the setup requires adjusting the jumper JP24 to position 1,
2 if using an external power source like P12, as per the user guide. SPI
is the default communication protocol configured for interfacing the
ADE7816 with the AD-APARD32690-SL, using pins for SCLK (SPI
clock), MOSI (data to ADE7816), MISO (data from ADE7816), and SS (SPI
chip select). The evaluation kit supports SPI and I2C interfaces, but
the included example defaults to SPI.

**Important**

- Connect the IRQ Pin from the ADE7816 to a GPIO pin on the AD-APARD32690-SL.
- Configure the GPIO on the AD-APARD32690-SL to detect the IRQ pin's interrupt signal.

+----------+-----------------------------+------------------------------+
| Signal   | EVAL-ADE7816 (P17 Header)   | AD-APARD32690-SL (P4 Header) |
+==========+=============================+==============================+
| MOSI     | P17.1                       | P4.4                         |
+----------+-----------------------------+------------------------------+
| MISO     | P17.7                       | P4.3                         |
+----------+-----------------------------+------------------------------+
| SCLK     | P17.3                       | P4.2                         |
+----------+-----------------------------+------------------------------+
| SS       | P17.5                       | P4.1                         |
+----------+-----------------------------+------------------------------+

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project
   make PLATFORM=maxim TARGET=max32690
   # to flash the code
   make run
