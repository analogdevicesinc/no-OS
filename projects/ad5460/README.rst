AD5460 no-OS Example Project
============================

.. contents::
    :depth: 3
    
Supported Evaluation Boards
---------------------------

- :adi:`EVAL-AD5460ARDZ`

Overview
--------

The EVAL-AD5460ARDZ is an evaluation board designed to facilitate the
use and understanding of the AD5460, a precision 16-bit
digital-to-analog converter (DAC). The board integrates seamlessly with
the Mbed OS platform via the EVAL-SDP-CK1Z, commonly referred to as the
SDP-K1, enabling rapid prototyping and system integration. It features a
user-friendly interface for easy programming and configuration,
supporting various power supply options to cater to diverse application
needs. The evaluation board is ideal for a range of applications,
particularly where high-resolution signal conversion is required.

Applications
------------

- Factory automation
- Actuator control
- Group isolated analog outputs
- PLC and DCS applications
- HART network connectivity

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD5460ARDZ evaluation board necessitates a power supply with a
voltage input between 19.2V and 28.8V. It employs the MAX17691B to
generate necessary isolated supply voltages: AVDD_H at 21V, AVDD_L at
5V, AVCC and DVCC each at 5V, and VASS at -16V. An optional external
reference of 2.5V is available via the ADR4525. Configuration options
include selecting the power supply input from a flyback or an external
connector, effectively catering to specific dual or single supply
setups. Proper header and jumper settings are critical in configuring
these supply rails before the board’s operation.

Pin Descriptions
~~~~~~~~~~~~~~~~

+-----------------------+-----------------------------------------------------+-----------------------------------+
| **Pin Name**          | **Function**                                        | **Default Configuration**         |
+-----------------------+-----------------------------------------------------+-----------------------------------+
| **JP1**               | SPI address configuration for AD5460                | Two jumpers both OPEN             |
+-----------------------+-----------------------------------------------------+-----------------------------------+
| **P7**                | Provides power to digital isolators on Arduino side | Pin 1 to Pin 2 (default)          |
+-----------------------+-----------------------------------------------------+-----------------------------------+
| **P8**                | Selects RESET or LDAC signal from Arduino           | RESET connected                   |
+-----------------------+-----------------------------------------------------+-----------------------------------+
| **VIOUT_A/B/C/D**     | Analog output channels (A, B, C, D)                 | N/A                               |
+-----------------------+-----------------------------------------------------+-----------------------------------+
| **+VSENSEP_x /        | Provides sense terminals for 2-wire or 3-wire       | N/A                               |
| −VSENSEN_x**          | feedback                                            |                                   |
+-----------------------+-----------------------------------------------------+-----------------------------------+
| **CHx (P11)**         | HART input header for channel modem connectivity    | N/A                               |
+-----------------------+-----------------------------------------------------+-----------------------------------+
| **P24, P32, P25,      | 2-wire/3-wire output feedback configuration headers | 2-wire feedback (default)         |
| P33**                 | for channels A–D                                    |                                   |
+-----------------------+-----------------------------------------------------+-----------------------------------+
| **P12/P16, P13/P17,   | RSET configuration for internal/external reference  | Internal RSET (default)           |
| P14/P18, P15/P19**    | resistor selection of Channels A–D                  |                                   |
+-----------------------+-----------------------------------------------------+-----------------------------------+
| **P22, P23, P26,      | Headers for connecting output to snubber networks   | Snubber networks disconnected     |
| P29**                 | for inductive loads on Channels A–D                 | (default)                         |
+-----------------------+-----------------------------------------------------+-----------------------------------+
| **P20, P30, P21, P31**| CCOMP configuration for VIOOUT_A–D output           | CCOMP connected (default)         |
|                       | compensation                                        |                                   |
+-----------------------+-----------------------------------------------------+-----------------------------------+
| **GND, GND0–GND3,     | Ground and test points for voltage and signal       | N/A                               |
| PGND, TP1**           | probing                                             |                                   |
+-----------------------+-----------------------------------------------------+-----------------------------------+
| **AVCC5V, AVDDHI,     | Additional voltage test points                      | N/A                               |
| AVDDLO, AVSS, LDO1V8, |                                                     |                                   |
| REFIN, REFOUT**       |                                                     |                                   |
+-----------------------+-----------------------------------------------------+-----------------------------------+

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The AD5460 no-OS project includes several supported examples namely:

- basic_example
- current_output_example
- voltage_output_example
- channel_output_example

The initialization data used in the examples is taken out from the
`Project Common Data Path: <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad5460/src/common>`__

The macros used in Common Data are defined in platform specific files
found in the 
`Project Platform Configuration Path: <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad5460/src/platform/mbed>`__

Basic Example
~~~~~~~~~~~~~~

The basic example code is designed to serve as a basic example for the
``eval-ad5460`` project, which likely involves interfacing with the
AD5460 DAC.
This file is intended to be an executable component, as indicated by the
presence of the ``example_main`` function, which acts as the entry point
for execution. The code demonstrates the initialization and basic
operation of the AD5460 device, including setting a general-purpose
Input Output (GPIO) pin to a high state.

In order to build the basic example, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

   RELEASE=y EXAMPLE=basic

Channel Output Example
~~~~~~~~~~~~~~~~~~~~~~~

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

To enable this example, use below setting in the Makefile configuration:

.. code-block:: bash

   RELEASE=y EXAMPLE=channel_output_example

Current Output Example
~~~~~~~~~~~~~~~~~~~~~~~

The ``channel_output_example`` shows how to configure the AD5460 DAC for
channel output tasks, specifically focused on output voltage control.
The code includes initializing the AD5460 device and setting the channel
for voltage output. Key configurations include setting the output
voltage range, enabling DAC slew for smoother transitions, and managing
thermal resets. It also covers GPIO and diagnostic settings to retrieve
active DAC codes and status, supporting both voltage and current modes.
Error handling mechanisms are incorporated to ensure reliable execution
in case of initialization or configuration failures.

To enable this example, use this setting on the Makefile:

.. code-block:: bash

   RELEASE=y EXAMPLE=current_output_example

Voltage Output Example
~~~~~~~~~~~~~~~~~~~~~~

The ``voltage_output_example.c`` file in the AD5460 no-OS project is
crafted to exhibit the initialization and configuration of the AD5460
DAC for voltage output operations. It commences with initializing the
AD5460, setting its functionality for voltage output, and defining the
output voltage parameters. The file converts a specific voltage level
into a corresponding DAC code, subsequently setting this code to the DAC
channel, enabling the precise output of the intended voltage level.
Additionally, it includes functions to read the active DAC code and
verify the DAC’s operational output, serving as a practical guide for
utilizing and testing the AD5460’s voltage output features in a no-OS
environment.

Use below Makefile configuration to enable this example:

.. code-block:: bash

   RELEASE=y EXAMPLE=voltage_output_example 

No-OS Supported Platform
------------------------

Mbed
~~~~

Hardware Used
^^^^^^^^^^^^^

- :adi:`EVAL-AD5460ARDZ`
- :adi:`EVAL-SDP-CK1Z` (SDP-K1)

Connections
^^^^^^^^^^^

+---------------------+-----------------------------+---------------------+-----------------------------+
| EVAL-AD5460ARDZ     | Function                    | EVAL-SDP-CK1Z       | Function                    |
| Pin/Terminal        |                             | Pin/Terminal        |                             |
+=====================+=============================+=====================+=============================+
| Arduino Socket      | Connects to SDP-K1 Arduino  | Arduino Header      | Mates with EVAL-AD5460ARDZ  |
|                     | headers for communication   |                     | Arduino socket              |
|                     | and isolation power         |                     |                             |
+---------------------+-----------------------------+---------------------+-----------------------------+
| P37 (VIN)           | Connect 24V power supply    | P14 (VIO_ADJUST)    | Set to 3.3V (near USB-C     |
|                     | (19.2V to 28.8V)            |                     | connector)                  |
+---------------------+-----------------------------+---------------------+-----------------------------+
|                     | Configure as per user guide | USB-C Port          | Connect to PC for power and |
|                     | defaults                    |                     | interface                   |
+---------------------+-----------------------------+---------------------+-----------------------------+

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # to delete current build
   make reset PLATFORM=mbed
   # to build the project
   make PLATFORM=mbed
   # to flash the code, copy the ad5460.bin to the mounted SDP-K1
   cp build/ad5460.bin </path/to/SDP-K1/mounted/folder>
