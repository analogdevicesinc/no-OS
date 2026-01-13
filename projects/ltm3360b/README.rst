Evaluating the LTM3360B
=======================

.. no-os-doxygen::

.. contents:: Table of Contents
	:depth: 3

Supported Evaluation Boards
---------------------------

* EVAL-LTM3360B

Overview
--------

The EVAL-LTM3360B evaluation board features the LTM3360B, a high-current
step-down DC-to-DC µModule regulator with integrated I2C interface for digital
control and monitoring. This complete power system delivers up to 33A of
continuous output current at output voltages as low as 300mV.

Full performance details are provided in the LTM3360B data sheet, which should
be consulted in conjunction with this user guide.

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

An external power supply of 5V (2.9 to 5.5V) is required for this project.

**Pin Description**

I2C Interface (P2):

+-----+----------+-------------------------------------------+
| Pin |   Name   | Description                               |
+-----+----------+-------------------------------------------+
| 1   | GND      | Ground                                    |
+-----+----------+-------------------------------------------+
| 2   | SDA      | I2C Serial Data                           |
+-----+----------+-------------------------------------------+
| 3   | SCL      | I2C Serial Clock                          |
+-----+----------+-------------------------------------------+

Power Connections (TP3 and TP4):

+-----+----------+-------------------------------------------+
| Pin |   Name   | Description                               |
+-----+----------+-------------------------------------------+
| 1   | VIN      | Input Power Supply, +5V                   |
+-----+----------+-------------------------------------------+
| 2   | GND      | Ground                                    |
+-----+----------+-------------------------------------------+


No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltm3360b/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltm3360b/src/platform>`_

Basic Example
^^^^^^^^^^^^^

This is a simple example that demonstrates the LTM3360B capabilities:

* Initializes the LTM3360B with default settings
* Configures the output voltage to 1000mV
* Enables the ADC for monitoring
* Performs ADC reads from multiple channels (VOUT_P, VOUT_N, VIN)
* Demonstrates basic functionality and communication

Steps of the basic example:

1. Initialize UART for console communication
2. Initialize the LTM3360B device with I2C interface
3. Set output voltage to 1000mV
4. Enable ADC for telemetry
5. Read ADC values from VOUT+, VOUT-, and VIN channels
6. Display results and exit

In order to build the basic example:

.. code-block:: bash

	make EXAMPLE=basic

No-OS Supported Platforms
-------------------------

STM32 Platform
^^^^^^^^^^^^^^

**Used hardware**

* `EVAL-LTM3360B evaluation board <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ltm3360b.html#eb-overview>`_
* `EVAL-SDP-CK1Z <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/sdp-k1.html>`_ or compatible STM32F4 development board

**Connections**:

I2C Interface:

+-----------------------------+------------+---------------------+-------------------+
| EVAL-LTM3360B Pin           |  Mnemonic  | Function            | SDP-K1 Pin        |
+-----------------------------+------------+---------------------+-------------------+
| P2.1                        | GND        | Ground              | GND               |
+-----------------------------+------------+---------------------+-------------------+
| P2.2                        | SDA        | I2C Serial Data     | D14 (I2C1_SDA)    |
+-----------------------------+------------+---------------------+-------------------+
| P2.3                        | SCL        | I2C Serial Clock    | D15 (I2C1_SCL)    |
+-----------------------------+------------+---------------------+-------------------+

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset
	# to build the project
	make EXAMPLE=basic HARDWARE=sdp-k1.ioc

**Hardware Setup Notes**

1. **Power Supply**: Connect a 5V DC power supply to VIN (TP3- TP4)
2. **Load**: Connect an appropriate load to VOUT (TP16- TP17) for testing
3. **Console**: Connect STM32 USB port for console communication via UART5

**Troubleshooting**

* Check power supply voltage and current capability
* Ensure proper grounding between all boards
* Verify console UART settings (115200 baud, 8N1)