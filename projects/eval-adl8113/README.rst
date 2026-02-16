EVAL-ADL8113 no-OS Example Project
==================================

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `ADL8113 <https://www.analog.com/ADL8113>`_

Overview
--------

The ADL8113 is a wideband, digitally controlled variable gain amplifier (DVGA)
that operates from 10 MHz to 14 GHz. The device provides precise gain control
through a parallel digital interface,trough VA and VB pins. This project demonstrates basic control
of the ADL8113 using GPIO pins on the MAX32655FTHR platform.

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The ADL8113 requires proper supply voltages for operation.
For this project, the digital control interface is powered from the
MAX32655FTHR 3.3V rail.

Board Connector and Jumper Settings
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Pin Description**

	Please see the following table for the pin assignments:

	+----------+-------------------------------------------+
	| Name     | Description                               |
	+----------+-------------------------------------------+
	| VDD      | Connect to supply voltage                 |
	+----------+-------------------------------------------+
	| GND      | Connect to Ground                         |
	+----------+-------------------------------------------+
	| VA       | Data bit 0 (GPIO controlled)              |
	+----------+-------------------------------------------+
	| VB       | Data bit 1 (GPIO controlled)              |
	+----------+-------------------------------------------+

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adl8113/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adl8113/src/platform>`_

Basic Example
^^^^^^^^^^^^^

This is a simple example that:

* Initializes the ADL8113
* Cycles trough every mode of the chip,trough button 1 on MAX2655FTHR,using a state machine
* Demonstrates gain control and switch control via GPIO pins

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used Hardware**

* `ADL8113 <https://www.analog.com/ADL8113>`_
* `MAX32655FTHR <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32655fthr.html>`_

**Connections**:

+-----------------+---------------------------------+-------------------------+
| ADL8113 Pin     | Function                        | MAX32655FTHR Pin        |
+-----------------+---------------------------------+-------------------------+
| VDD             | Power Supply                    | Use a 5V power supply   |
+-----------------+---------------------------------+-------------------------+
| VDD_SW          | Dual Power Supply               | Use a 3,3V power supply |
+-----------------+---------------------------------+-------------------------+
| VSS             | Dual Power Supply               | Use a -3.3V power supply|
+-----------------+---------------------------------+-------------------------+
| GND             | Ground                          | GND                     |
+-----------------+---------------------------------+-------------------------+
| VA              | Data bit 0                      | P1_6                    |
+-----------------+---------------------------------+-------------------------+
| VB              | Data bit 1                      | P1_7                    |
+-----------------+---------------------------------+-------------------------+

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset
	# to build the project
	make
	# to flash the code
	make flash
