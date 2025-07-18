AD-ACEVSECRDSET-SL no-OS Example Project
=========================================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
----------------------------

* `AD-ACEVSECRDSET-SL <https://www.analog.com/ad-acevsecrdset-sl>`_

Overview
--------

The AD-ACEVSECRDSET-SL is a Type 2 electric vehicle supply equipment
(EVSE) reference design for evaluating and prototyping EV charging systems
at a 3.6 kW capacity. It includes the ADE9113 3-channel isolated
sigma-delta ADC for voltage and current measurement and the MAX32655
ultralow power ARM Cortex-M4 microcontroller with integrated Bluetooth 5.2
for control and communication. The design supports input voltage ranges
of 195 V to 264 V (230 V nominal) and 102 V to 138 V (120 V nominal),
with a maximum current capacity of 16 A. It incorporates safety features
including 6 mA DC/30 mA rms residual current detection (RCD), relay
soldered contacts detection, overvoltage, undervoltage, overcurrent,
and overtemperature protection, and upstream/downstream protective earth
(PE) detection. The open-source software stack is designed to align with
safety standards IEC 61851 and IEC 62752.

Applications
-------------

* EV Charging
* EVSE Prototyping
* Smart Energy Management

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The AD-ACEVSECRDSET-SL requires an input voltage range of 195 V to 264 V
for a 230 V nominal setup and 102 V to 138 V for a 120 V nominal setup,
both with a +/-15% tolerance. It supports a maximum current capacity of
16 A, with 10 A available for lower power settings. The system ensures
operational safety by conducting self-tests and monitoring voltage
limits, relay states, and both AC and DC residual current devices
(RCDs). It incorporates overcurrent measurement and temperature
monitoring as part of its compliance with EV charging standards such as
IEC 61851 and IEC 62752.

On-board Connections
~~~~~~~~~~~~~~~~~~~~

+-----------------+-----------------+-----------------+-----------------+
| Connector Type  | Purpose         | Location on     | Associated      |
|                 |                 | Board           | Components      |
+-----------------+-----------------+-----------------+-----------------+
| Grid Connector  | AC Power Input  | Top left of     | Neutral (N),    |
|                 |                 | board           | Live (L),       |
|                 |                 |                 | Protective      |
|                 |                 |                 | Earth (PE)      |
+-----------------+-----------------+-----------------+-----------------+
| EV Connector    | Vehicle         | Mid-right of    | Control Pilot   |
|                 | Connection      | board           | (CP), Neutral   |
|                 |                 |                 | (N), Line (L),  |
|                 |                 |                 | PE              |
+-----------------+-----------------+-----------------+-----------------+
| BLE Antenna     | Wireless        | Part of control | Bluetooth 5.2   |
|                 | Communication   | module          |                 |
+-----------------+-----------------+-----------------+-----------------+
| Debug/          | System          | Near            | RS-232, USB     |
| Programming     | Debugging and   | microcontroller | connections     |
| Connector       | Programming     | unit            |                 |
+-----------------+-----------------+-----------------+-----------------+

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
-------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad-acevsecrdset-sl/src/common>`_

The macros used in Common Data are defined in platform specific files
found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad-acevsecrdset-sl/src/platform>`_

Basic Example
~~~~~~~~~~~~~

The AD-ACEVSECRDSET-SL basic example project showcases the
implementation of a Type 2 EVSE solution using the MAX32655
microcontroller. This setup includes the ADE9113 for voltage and current
measurement and features compliance with IEC 61851 and IEC 62752 safety
standards. It highlights the use of various hardware interface modules
for effective communication and control, such as SPI, UART, and I2C.
Central to the project is a state machine managing charging states and
interfacing with pilot, relay, and RCD functions, allowing control over
the charging process.

In order to build the basic example, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/ad-acevsecrdset-sl/Makefile>`_:

.. code-block:: bash

   # Select the board revision
   REV_A = n
   REV_D = y

No-OS Supported Platforms
-------------------------

Maxim Platform
~~~~~~~~~~~~~~

**Used Hardware**

* `AD-ACEVSECRDSET-SL <https://www.analog.com/ad-acevsecrdset-sl>`_
* `MAX32625PICO <https://www.analog.com/MAX32625PICO>`_ (debug adapter)

**Connections**

* Connect the MAX32625PICO to the AD-ACEVSECRDSET-SL board via USB.
* Attach a 10-pin SWD cable for programming and debugging.
* Connect the AD-ACEVSECRDSET-SL board to a power source using a
  suitable 3-wire cable for mains electricity (195 V to 264 V).

**Software Setup**

To flash the MAX32625PICO with the correct firmware image:

* Download the latest firmware image for the MAX32655FTHR board:
  ``https://github.com/analogdevicesinc/max32625pico-firmware-images/raw/master/bin/max32625_max32655fthr_if_crc_swd_v1.0.3.bin``
* Connect the MAX32625PICO board to your computer.
* Hold the main button while powering on to mount it as a ``MAINTENANCE``
  drive.
* Drag and drop the firmware image to the ``MAINTENANCE`` drive.
* Eject the drive safely and reboot the board.

**Build Command**

.. code-block:: bash

   # to delete current build
   make reset PLATFORM=maxim TARGET=max32655
   # to build the project
   make PLATFORM=maxim TARGET=max32655
   # to flash the code
   make run PLATFORM=maxim TARGET=max32655
