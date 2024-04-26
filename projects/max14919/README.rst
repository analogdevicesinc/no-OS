Evaluating the MAX14919
=======================

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `MAX14919PMB <https://www.analog.com/MAX14919PMB>`_

Overview
--------

The MAX14919 PMB provides a proven design to evaluate the MAX14919 industrial-grade quad low-side
switch with 140mΩ (typ) RON and ±1kV/42Ω surge protection.

Applications
------------

* Industrial Digital Outputs
* Motor Control
* PLC and DCS Systems
* Relay and Solenoid Drivers

Hardware Specifications
-----------------------

Power Supply Requirments
^^^^^^^^^^^^^^^^^^^^^^^^

The MAX14919PMB device needs to be supplied with a 3.3V voltage.

**Pin Description**

	Please see te following table for the pin assignments for the
	following connectors:

	T1:

	+-----+----------+-------------------------------------------+
	| Pin |   Name 	 | Description				     |
	+-----+----------+-------------------------------------------+
	| 1   | IN1	 | IN1 Logic Input (GPIO)		     |
	+-----+----------+-------------------------------------------+
	| 2   | IN2      | IN2 Logic Input (GPIO)	             |
	+-----+----------+-------------------------------------------+
	| 3   | IN3	 | IN3 Logic Input (GPIO)		     |
	+-----+----------+-------------------------------------------+
	| 4   | IN4	 | IN4 Logic Input (GPIO)		     |
	+-----+----------+-------------------------------------------+
	| 5   | GND	 | Connect to Ground.			     |
	+-----+----------+-------------------------------------------+
	| 6   | VL       | Power Supply, +3.3V		             |
	+-----+----------+-------------------------------------------+
	| 7   | INRUSH	 | IN_RUSH Logic Input (GPIO)		     |
	+-----+----------+-------------------------------------------+
	| 8   | FAULT	 | FAULT Logic Output (GPIO)		     |
	+-----+----------+-------------------------------------------+
	| 9   | REV	 | REV Logic Output (GPIO)		     |
	+-----+----------+-------------------------------------------+
	| 10  | DNC	 | Do Not Connect			     |
	+-----+----------+-------------------------------------------+
	| 11  | GND	 | Connect to GROUND			     |
	+-----+----------+-------------------------------------------+
	| 12  | VL	 | Power Supply, +3.3V			     |
	+-----+----------+-------------------------------------------+

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max14919/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max14919/src/platform>`_

Basic example
^^^^^^^^^^^^^

This is a simple example which initializes the max14919 and sets the output
channel 2 on, after which is detects faults, reverse polarity and sets 2x
current limit for the device.


In order to build the basic example make sure you have the following configuration in the Makefile
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max14919/Makefile>`_

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	BASIC_EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**

* `MAX14919PMB <https://www.analog.com/MAX14919PMB>`_
* `AD-APARD32690-SL <https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/ad-apard32690-sl.html>`_

+------------------------+----------+-------------------------------------------+-----------------------------+
| MAX14919PMB Pin Name   | Mnemonic | Function					| AD-APARD32690-SL Pin Number |
+------------------------+----------+-------------------------------------------+-----------------------------+
| IN1(1)		 | IN1	    | IN1 LOGIC INPUT(GPIO)			| P1_0			      |      
+------------------------+----------+-------------------------------------------+-----------------------------+
| IN1(2)		 | IN2      | IN2 LOGIC INPUT(GPIO)			| P1_1	      		      |
+------------------------+----------+-------------------------------------------+-----------------------------+
| IN1(3)		 | IN3      | IN3 LOGIC INPUT(GPIO)			| P1_2		      	      |
+------------------------+----------+-------------------------------------------+-----------------------------+
| IN1(4)		 | IN4      | IN LOGIC INPUT(GPIO)			| P1_3		      	      |
+------------------------+----------+-------------------------------------------+-----------------------------+
| FAULT			 | FAULT    | FAULT LOGIC OUTPUT(GPIO)			| P1_6			      |
+------------------------+----------+-------------------------------------------+-----------------------------+
| REV			 | REV	    | REV LOGIC OUTPUT(GPIO)			| P1_4			      |
+------------------------+----------+-------------------------------------------+-----------------------------+
| INRUSH		 | IN_RUSH  | INRUSH LOGIC INPUT(GPIO)			| P2_21			      |
+------------------------+----------+-------------------------------------------+-----------------------------+
| GND			 | GND      | Ground					| GND			      |
+------------------------+----------+-------------------------------------------+-----------------------------+
| GNDB			 | GND      | Ground					| GND			      |
+------------------------+----------+-------------------------------------------+-----------------------------+
| VL			 | VL	    | Power Supply, +3.3V			| 3V3			      |
+------------------------+----------+-------------------------------------------+-----------------------------+

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset
	# to build the project
	make PLATFORM=maxim TARGET=max32690
	# to flash the code
	make run
