AD5460 no-OS Example Project
============================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-AD5460ARDZ <https://www.analog.com/EVAL-AD5460ARDZ>`_

Overview
--------

The EVAL-AD5460ARDZ is an evaluation board designed to facilitate the
use and understanding of the AD5460, a quad-channel, voltage, and
current output, 16-bit digital-to-analog converter (DAC). The board
integrates seamlessly with the Mbed OS platform via the EVAL-SDP-CK1Z,
commonly referred to as the SDP-K1, enabling rapid prototyping and
system integration. It features a user-friendly interface for easy
programming and configuration, supporting various power supply options
to cater to diverse application needs. The evaluation board is ideal for
a range of applications, particularly where high-resolution signal
conversion is required.

Applications
------------

* Factory automation
* Actuator control
* Group isolated analog outputs
* PLC and DCS applications
* HART network connectivity

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The EVAL-AD5460ARDZ evaluation board necessitates a power supply with a
voltage input between 19.2V and 28.8V. It employs the MAX17691B to
generate necessary isolated supply voltages: AVDD_H at 21V, AVDD_L at
5V, AVCC and DVCC each at 5V, and VASS at -16V. An optional external
reference of 2.5V is available via the ADR4525. Configuration options
include selecting the power supply input from a flyback or an external
connector, effectively catering to specific dual or single supply
setups. Proper header and jumper settings are critical in configuring
these supply rails before the board's operation.

Board Connector and Jumper Settings
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

==============================  ================================================================
Pin Name                        Description
==============================  ================================================================
JP1                             SPI address configuration (default: both OPEN)
P7                              Digital isolator power select (default: Pin 1 to Pin 2)
P8                              RESET or LDAC signal select (default: RESET connected)
VIOUT_A/B/C/D                   Analog output channels A, B, C, D
VSENSEP_x / VSENSEN_x           Sense terminals for 2-wire or 3-wire feedback
CHx (P11)                       HART input header for channel modem connectivity
P24, P32, P25, P33              2-wire/3-wire output feedback config (default: 2-wire)
P12-P19                         RSET internal/external reference resistor (default: internal)
P22, P23, P26, P29              Snubber network for inductive loads (default: disconnected)
P20, P30, P21, P31              CCOMP output compensation config (default: connected)
GND, GND0-GND3, PGND, TP1       Ground and test points
Voltage test points             AVCC5V, AVDDHI, AVDDLO, AVSS, LDO1V8, REFIN, REFOUT
==============================  ================================================================

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad5460/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad5460/src/platform/mbed>`_

Basic Example
^^^^^^^^^^^^^

The basic example code demonstrates the initialization and basic
operation of the AD5460 device, including setting a general-purpose
Input Output (GPIO) pin to a high state.

In order to build the basic example, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

	RELEASE=y EXAMPLE=basic

Channel Output Example
^^^^^^^^^^^^^^^^^^^^^^

The ``channel_output_example`` features the AD5460 DAC setup for channel
output tasks, focusing on output voltage configuration. Encapsulated in
the ``example_main`` function, it involves initializing the AD5460
device, setting up the channel for voltage output, and defining the
output voltage range. The example includes enabling DAC slew,
configuring thermal reset, and managing GPIO and diagnostic settings. It
retrieves the DAC active code and live status, converting the desired
voltage or current output into DAC codes for channel 0. The example
supports both voltage and current modes, allowing users to customize
output requirements and includes error handling to address
initialization and configuration failures, ensuring efficient execution.

To enable this example, use the following setting in the Makefile:

.. code-block:: bash

	RELEASE=y EXAMPLE=channel_output_example

Current Output Example
^^^^^^^^^^^^^^^^^^^^^^

The ``current_output_example`` shows how to configure the AD5460 DAC for
current output tasks. The code includes initializing the AD5460 device
and setting the channel for current output. Key configurations include
setting the output current range, enabling DAC slew for smoother
transitions, and managing thermal resets. It also covers GPIO and
diagnostic settings to retrieve active DAC codes and status.
Error handling mechanisms are incorporated to ensure reliable execution
in case of initialization or configuration failures.

To enable this example, use the following setting in the Makefile:

.. code-block:: bash

	RELEASE=y EXAMPLE=current_output_example

Voltage Output Example
^^^^^^^^^^^^^^^^^^^^^^

The ``voltage_output_example`` demonstrates the initialization and
configuration of the AD5460 DAC for voltage output operations. It
commences with initializing the AD5460, setting its functionality for
voltage output, and defining the output voltage parameters. The file
converts a specific voltage level into a corresponding DAC code,
subsequently setting this code to the DAC channel, enabling the precise
output of the intended voltage level. Additionally, it includes functions
to read the active DAC code and verify the DAC's operational output,
serving as a practical guide for utilizing and testing the AD5460's
voltage output features in a no-OS environment.

To enable this example, use the following setting in the Makefile:

.. code-block:: bash

	RELEASE=y EXAMPLE=voltage_output_example

No-OS Supported Platforms
-------------------------

Mbed Platform
^^^^^^^^^^^^^

**Used Hardware**

* `EVAL-AD5460ARDZ <https://www.analog.com/EVAL-AD5460ARDZ>`_
* `EVAL-SDP-CK1Z <https://www.analog.com/EVAL-SDP-CK1Z>`_ (SDP-K1)

**Connections**:

Connect the EVAL-AD5460ARDZ to the EVAL-SDP-CK1Z via the Arduino socket headers.

* EVAL-AD5460ARDZ P37 (VIN): Connect 24 V power supply (19.2 V to 28.8 V)
* EVAL-SDP-CK1Z P14 (VIO_ADJUST): Set to 3.3 V (near USB-C connector)
* EVAL-SDP-CK1Z USB-C Port: Connect to PC for power and interface

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset PLATFORM=mbed
	# to build the project
	make PLATFORM=mbed
	# to flash the code, copy the ad5460.bin to the mounted SDP-K1
	cp build/ad5460.bin </path/to/SDP-K1/mounted/folder>
