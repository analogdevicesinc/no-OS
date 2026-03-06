ADE7816 no-OS Example Project
=============================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-ADE7816EBZ <https://www.analog.com/EVAL-ADE7816>`_

Overview
--------

The `ADE7816 <https://www.analog.com/ADE7816>`_ is a highly accurate, multichannel
energy metering IC capable of measuring one voltage channel and up to six current
channels. It measures line voltage and current and calculates active and reactive
energy, as well as instantaneous RMS voltage and current. The device incorporates
seven sigma-delta ADCs with a high accuracy energy measurement core. The six current
input channels allow multiple loads to be measured simultaneously.

The EVAL-ADE7816EBZ is an evaluation board designed to facilitate the assessment
and prototyping of the ADE7816. This board provides access to the various
functionalities of the ADE7816, allowing for comprehensive evaluation of its
performance in monitoring electric properties such as voltage, current, and power.
The evaluation board supports interfacing via test points, connectors, and
communication ports, making it suitable for both standalone operation and integration
into broader testing environments.

Applications
------------

* Energy metering
* Power monitoring systems
* Polyphase multifunction metering

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The EVAL-ADE7816EBZ requires a 3.3 V power supply. By default, power is supplied
via a USB connection, feeding the NXP LPC2368 microcontroller and one side of the
isocouplers through the JP24 jumper positioned at 2, 3. For external power use,
adjust JP24 to position 1, 2 and connect a 3.3 V source to the P12 connector.
This configuration maintains the same power domain as the default USB setup.

Device Interrupt
^^^^^^^^^^^^^^^^

To run the ADE7816 no-OS project with the AD-APARD32690-SL controller board,
the IRQ pin from the EVAL-ADE7816EBZ must be connected to a GPIO pin on the
AD-APARD32690-SL platform. This GPIO pin is configured to detect interrupts
and trigger an interrupt service routine (ISR) when the IRQ pin is activated,
signaling events like data ready or fault conditions. The ISR handles the
interrupt by reading the ADE7816 status, processing the data, and clearing
the interrupt flag.

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ade7816/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ade7816/src/platform>`_

Basic Example
^^^^^^^^^^^^^

This is a simple example that:

* Initializes the ADE7816 device over SPI
* Configures zero-crossing detection on channel A
* Sets phase calibration for all six current channels at 50 Hz
* Reads active and reactive energy values for channel A
* Converts and displays RMS current and voltage readings in microamperes and microvolts
* Uses interrupt-driven measurement control via the IRQ0 pin

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used Hardware**

* `EVAL-ADE7816EBZ <https://www.analog.com/EVAL-ADE7816>`_
* `AD-APARD32690-SL <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/ad-apard32690-sl.html>`_

**Connections**

Supplying a regulated 3.3 V DC to the EVAL-ADE7816EBZ board is achieved via
connector P9, which powers the ADE7816 IC and the non-isolated side of the
circuit. Adjust the jumper JP24 to position 1, 2 if using an external power
source like P12, as per the user guide. SPI is the default communication
protocol configured for interfacing the ADE7816 with the AD-APARD32690-SL.
The evaluation kit supports SPI and I2C interfaces, but the included example
defaults to SPI.

.. important::

	* Connect the IRQ pin from the EVAL-ADE7816EBZ to a GPIO pin on the AD-APARD32690-SL.
	* Configure the GPIO on the AD-APARD32690-SL to detect the IRQ pin interrupt signal.

==========  ============================  ==============================
Signal      EVAL-ADE7816 (P17 Header)     AD-APARD32690-SL (P4 Header)
==========  ============================  ==============================
MOSI        P17.1                         P4.4
MISO        P17.7                         P4.3
SCLK        P17.3                         P4.2
SS          P17.5                         P4.1
==========  ============================  ==============================

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset
	# to build the project
	make PLATFORM=maxim TARGET=max32690
	# to flash the code
	make run
