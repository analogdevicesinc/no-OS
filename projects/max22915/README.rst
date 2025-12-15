MAX22915 no-OS Example Project
==============================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `MAX22915EVKIT <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max22915evkit.html>`_

Overview
--------

The MAX22915 evaluation kit provides the hardware to evaluate the MAX22915
industrial octal high-side switch with diagnostics. The device is specified
for operation with a supply voltage tolerant up to 65V and delivers up to
700mA (min) continuous current per channel with low on-resistance of 120mΩ (max).

Refer to the MAX22915 IC data sheet for detailed information regarding
operation of the IC.

Applications
------------

* Industrial Digital Output Systems
* PLC Systems
* Distributed Control Systems (DCS)

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The MAX22915 evaluation kit needs to be supplied with:

* VL: 3.3V to 5V logic supply
* VDD: 12V to 65V high-voltage supply

**Pin Description**

	Please see the following table for the pin assignments for the SDP-K1
	interface connector. Connect the following pins on the J14 of the MAX22915EVKIT
	to the SPI pins on the P6 of the SDP-K1.

	+-----+----------+-------------------------------------------+
	| Pin |   Name 	 | Description                               |
	+-----+----------+-------------------------------------------+
	| 1   | CS       | SPI Chip Select (GPIO)                    |
	+-----+----------+-------------------------------------------+
	| 2   | SCLK     | SPI Clock                                 |
	+-----+----------+-------------------------------------------+
	| 3   | MISO     | SPI Master In Slave Out                   |
	+-----+----------+-------------------------------------------+
	| 4   | MOSI     | SPI Master Out Slave In                   |
	+-----+----------+-------------------------------------------+
	| 5   | GND      | Connect to GROUND                         |
	+-----+----------+-------------------------------------------+

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max22915/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max22915/src/platform>`_

Basic example
^^^^^^^^^^^^^

This is a simple example which initializes the MAX22915 device and demonstrates:

* Device initialization with SPI communication
* Channel configuration and control
* LED status indicators (FLED and SLED)
* Diagnostic features (open-wire detection, short to VDD detection)
* Current and temperature measurement configuration

The example configures all 8 channels as high-side switches and demonstrates
basic switch control operations along with diagnostic capabilities.

In order to build the basic example make sure you have the following configuration in the Makefile
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max22915/Makefile>`_

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	BASIC_EXAMPLE = y

No-OS Supported Platforms
-------------------------

STM32 Platform
^^^^^^^^^^^^^^

**Used hardware**

* `MAX22915EVKIT <https://www.analog.com/MAX22915EVKIT>`_
* `SDP-K1 <https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/sdp-k1.html>`_

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset
	# to build the project for STM32 platform
	make PLATFORM=stm32 EXAMPLE=basic_example
	# to flash the code
	make run

Hardware Setup
--------------

For proper operation of the MAX22915 evaluation kit:

1. **Power Supply Configuration**:
   - Connect VDD to 12V-65V high-voltage supply (external)
   - Ensure proper grounding between all boards

2. **Load Configuration**:
   - Connect appropriate loads to the output channels (CH0-CH7)
   - Loads should be rated for the operating voltage and current
   - Use LEDs with series resistors for basic testing

3. **SPI Communication**:
   - Verify SPI connections between SDP-K1 and MAX22915EVKIT
   - Ensure proper signal integrity with short, direct connections

4. **Safety Considerations**:
   - Use appropriate safety measures when working with high voltages
   - Ensure proper insulation and grounding
   - Follow all safety guidelines in the evaluation kit documentation

Example Features
----------------

The basic example demonstrates the following MAX22915 features:

* **8-Channel High-Side Switching**: Control of all 8 output channels
* **Diagnostic Capabilities**:
  - Open-wire detection (on/off states)
  - Short to VDD detection
  - Overcurrent protection
  - Thermal protection
* **LED Status Indicators**:
  - Fault LEDs (FLED) for error indication
  - Status LEDs (SLED) for channel state indication
* **Current Measurement**: ADC-based current sensing for each channel
* **Temperature Monitoring**: On-chip temperature measurement
* **CRC Protection**: Data integrity verification for SPI communication

Usage Notes
-----------

* Ensure proper power sequencing: VL before VDD
* Monitor thermal conditions during high-current operation
* Verify load compatibility with 65V maximum voltage rating
* Use the diagnostic features to ensure system reliability