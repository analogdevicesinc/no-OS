AD-ACEVSECRDSET-SL no-OS Example Project
========================================

.. contents::
    :depth: 3

Supported Evaluation Boards
----------------------------

- `AD-ACEVSECRDSET-SL <https://www.analog.com/ad-bct2ade9113-sl.html>`__

Overview
--------

The AD-ACEVSECRDSET-SL is a Type 2 electric vehicle supply equipment
(EVSE) solution for evaluating and prototyping EV charging systems at a
3.6 kW capacity. It includes components such as the ADE9153 for voltage
and current measurement and the MAX32655 ARM Cortex-M4 microcontroller
for control and communication, equipped with Bluetooth via the MAX32656.
This user guide highlights its open-source software stack designed to
support electrical applications while aligning with safety standards
IEC61851 and IEC62752. The documentation is structured for clear
technical understanding, focusing on the device’s capabilities and
compliance standards.

Applications
-------------

- EV Charging

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The AD-ACEVSECRDSET-SL requires an input voltage range of 195V to 264V
for a 230V nominal setup and 102V to 138V for a 120V nominal setup, both
with a ±15% tolerance. It supports a maximum current capacity of 16A,
with 10A available for lower power settings. The system ensures
operational safety by conducting self-tests and monitoring voltage
limits, relay states, and both AC and DC residual current devices
(RCDs). It incorporates overcurrent measurement and temperature
monitoring as part of its compliance with EV charging standards such as
IEC61851 and IEC62752.

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

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
-------------------------

The initialization data used in the example is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad-acevsecrdset-sl/src/common>`__

The macros used in Common Data are defined in platform specific files
found in the `Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad-acevsecrdset-sl/src/platform>`__

Example Project for Rev. D board
--------------------------------

The no-OS project for the AD-ACEVSECRDSET-SL evaluation board includes
reference firmware that demonstrates how to interface with the
MAX32655PICO microcontroller and Analog Devices peripherals.

In order to build the example project, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/ad-acevsecrdset-sl/Makefile>`__ :

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   REV_A = n
   REV_D = y

No-OS Supported Platforms
-------------------------

Maxim Platform
~~~~~~~~~~~~~~~

Hardware Used
^^^^^^^^^^^^^

- AD-ACEVSECRDSET-SL board (Rev. D)
- :adi:`MAX32625PICO microcontroller <MAX32625PICO>`

Connections
^^^^^^^^^^^^

- Connect the MAX32625PICO to the AD-ACEVSECRDSET-SL board using a USB
  to micro-USB cable.
- Attach a 10-pin SWD Cable for programming and debugging.
- Connect the AD-ACEVSECRDSET-SL board to a power source using a
  suitable 3-wire cable for mains electricity (195V to 264V).

**Software Setup**

*Install the MAX32655 Firmware*

To flash the MAX32655 firmware image:

- Download the latest firmware image for the MAX32655FTHR board:
  `https://github.com/analogdevicesinc/max32625pico-firmware-images/raw/master/bin/max32625_max32655fthr_if_crc_swd_v1.0.3.bin`

- Connect the MAX32625PICO board to your computer.

- Hold the main button while powering on to mount it as a ``MAINTENANCE``
  drive.

- Drag and drop the firmware image to the ``MAINTENANCE`` drive.

- Eject the drive safely and reboot the board.

**Basic Example Project**

The AD-ACEVSECRDSET-SL basic example project showcases the
implementation of a Type 2 EVSE solution using the MAX32655
microcontroller. This setup includes the ADE9153 for voltage and current
measurement and features compliance with IEC61851 and IEC62752 safety
standards. It highlights the use of various hardware interface modules
for effective communication and control, such as SPI, UART, and I2C.
Central to the project is a state machine managing charging states and
interfacing with pilot, relay, and RCD functions, allowing control over
the charging process.

In order to build the basic example project, make sure you have the
following configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/ad-acevsecrdset-sl/Makefile>`__ :

.. code-block:: bash

   # Select the example you want to enable
   REV_D = y

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # to delete the current build
   make reset
   # to build the project
   make PLATFORM=maxim TARGET=MAX32625 EXAMPLE=basic
   # to flash the code
   make run
