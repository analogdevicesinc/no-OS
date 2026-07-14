EVAL-ADG2404 no-OS Example Project
==================================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `ADG2404 <https://www.analog.com/ADG2404>`_

Overview
--------

The ADG2404 is a CMOS analog multiplexer comprising four differential channels.
The device operates as a 4:1 multiplexer.
This project demonstrates basic control of the ADG2404 using GPIO pins
on the MAX32655FTHR platform.

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The ADG2404 requires a single supply voltage (VDD) between 1.8V and 5.5V.
For this project, the device is powered from the MAX32655FTHR 3.3V rail.

Board Connector and Jumper Settings
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Pin Description**

	Please see the following table for the pin assignments:

	+----------+-------------------------------------------+
	| Name     | Description                               |
	+----------+-------------------------------------------+
	| VDD      | Connect to 5V supply                      |
	+----------+-------------------------------------------+
	| VSS      | Connect to -5V supply                     |
	+----------+-------------------------------------------+
	| GND      | Connect to Ground                         |
	+----------+-------------------------------------------+
	| A0       | Address bit 0 (GPIO controlled)           |
	+----------+-------------------------------------------+
	| A1       | Address bit 1 (GPIO controlled)           |
	+----------+-------------------------------------------+
	| EN       | Enable pin (active high)                  |
	+----------+-------------------------------------------+

.. warning::

   Input signal range is VSS to VDD-2V for safe handling.

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adg2404/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adg2404/src/platform>`_

Basic Example
^^^^^^^^^^^^^

This is a simple example that:

* Initializes the ADG2404
* Enables the multiplexer
* Cycles through all four channels, enabling the channel for 3 seconds, then moving to the next one
* Demonstrates channel selection via GPIO control

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used Hardware**

* `EVAL-ADG2404 <https://www.analog.com/ADG2404>`_
* `MAX32655FTHR <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32655fthr.html>`_

**Connections**:

+-----------------+---------------------------------+-------------------------+
| ADG2404 Pin     | Function                        | MAX32655FTHR Pin        |
+-----------------+---------------------------------+-------------------------+
| VDD             | Power Supply (5V)               | Use a dual power supply |
+-----------------+---------------------------------+-------------------------+
| VSS             | Power Supply (-5V)              | Use a dual power supply |
+-----------------+---------------------------------+-------------------------+
| GND             | Ground                          | GND                     |
+-----------------+---------------------------------+-------------------------+
| A0              | Address bit 0                   | P1_6                    |
+-----------------+---------------------------------+-------------------------+
| A1              | Address bit 1                   | P1_7                    |
+-----------------+---------------------------------+-------------------------+
| EN              | Enable (active high)            | P1_8                    |
+-----------------+---------------------------------+-------------------------+

**Build Command**

For toolchain setup and prerequisites, see the
`Maxim CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_maxim_cmake.html>`__.

Available variants: ``basic``.
Available boards: ``max32655fthr``.
Replace ``--variant`` / ``--board`` accordingly.

.. code-block:: bash

   export MAXIM_LIBRARIES=</path/to/MaximSDK/Libraries>
   # Windows (PowerShell): $env:MAXIM_LIBRARIES = "C:\MaximSDK\Libraries"

   cd no-OS

   # build the project (basic example on the max32655fthr board)
   python tools/scripts/no_os_build.py build \
      --project eval-adg2404 --variant basic --board max32655fthr

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project eval-adg2404 --variant basic --board max32655fthr \
      --probe openocd --flash
