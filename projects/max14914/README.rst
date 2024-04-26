Evaluating the MAX14914
=======================

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `MAX14914PMB <https://www.analog.com/MAX14914PMB>`_

Overview
--------

The MAX14914 peripheral module provides the hardware to evaluate the MAX14914
high-side/push-pull driver that operates as both an industrial digital
output (DO) and an industrial digital input (DI).

Refer to the MAX14914 IC data sheet for detailed information regarding
operation of the IC.

Applications
------------

* Configurable Digital Input/Output
* Industrial Digital Outputs and Inputs Modules
* Motor Control
* Safety Systems

Hardware Specifications
-----------------------

Power Supply Requirments
^^^^^^^^^^^^^^^^^^^^^^^^

The MAX14914PMB device has to be supplied with a 24V DC output AC adapter and
with voltage of 3.3V.

**Pin Description**

	Please see the following table for the pin assignments for the interface
	connector (T3).

	+-----+----------+-------------------------------------------+
	| Pin |   Name 	 | Description				     |
	+-----+----------+-------------------------------------------+
	| 1   | DIDO_LVL | Open-Drain DOI Level Output(GPIO)	     |
	+-----+----------+-------------------------------------------+
	| 2   | FAULT    | Open-Drain Fault Output(GPIO)             |
	+-----+----------+-------------------------------------------+
	| 3   | DI_ENA	 | Digital Input Mode Logic Input(GPIO)      |
	+-----+----------+-------------------------------------------+
	| 4   | OV_VDD	 | Open-Drain Overvoltage Output  	     |
	+-----+----------+-------------------------------------------+
	| 5   | DO_SET   | Switch Control Input (GPIO)		     |
	+-----+----------+-------------------------------------------+
	| 6   | DNC      | Do Not Connect		             |
	+-----+----------+-------------------------------------------+
	| 7   | DO_PP	 | Push-Pull DO or DI Type Select Input(GPIO)|
	+-----+----------+-------------------------------------------+
	| 8   | DNC	 | Do Not Connect			     |
	+-----+----------+-------------------------------------------+
	| 9   | GND	 | Connect to GROUND			     |
	+-----+----------+-------------------------------------------+
	| 10  | GND	 | Connect to GROUND			     |
	+-----+----------+-------------------------------------------+
	| 11  | VL	 | Power Supply, +3.3V			     |
	+-----+----------+-------------------------------------------+
	| 12  | VL	 | Power Supply, +3.3V			     |
	+-----+----------+-------------------------------------------+

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max14914/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max14914/src/platform>`_

Basic example
^^^^^^^^^^^^^

This is a simple example which initializes the max14914 and sets the state of
the device to a DIGITAL INPUT IEC TYPE 2.

In order to build the basic example make sure you have the following configuration in the Makefile
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max14914/Makefile>`_

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	BASIC_EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**

* `MAX14914 PMB <https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/max14914pmb.html#eb-overview>`_
* `AD-APARD32690-SL <https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/ad-apard32690-sl.html>`_

**Connections**:

+------------------------+----------+-------------------------------------------+-----------------------------+
| MAX14914PMB Pin Number | Mnemonic | Function					| AD-APARD32690-SL Pin Number |
+------------------------+----------+-------------------------------------------+-----------------------------+
| 1			 | DIDO_LVL | Open-Drain DOI Level Output(GPIO)		| P1_0			      |      
+------------------------+----------+-------------------------------------------+-----------------------------+
| 2			 | FAULT    | Fault Logic Output(GPIO)			| P2_21		      	      |
+------------------------+----------+-------------------------------------------+-----------------------------+
| 3			 | DI_ENA   | Digital Input Mode Logic Input(GPIO)	| P1_1		      	      |
+------------------------+----------+-------------------------------------------+-----------------------------+
| 5			 | DO_SET   | Switch Control Logic Input (GPIO)		| P1_2		      	      |
+------------------------+----------+-------------------------------------------+-----------------------------+
| 7			 | DO_PP    | Push-Pull DO or DI Type Select Input(GPIO)| P1_3			      |
+------------------------+----------+-------------------------------------------+-----------------------------+
| 9			 | GND      | Ground					| GND			      |
+------------------------+----------+-------------------------------------------+-----------------------------+
| 11			 | VL	    | Power Supply, +3.3V			| 3v3			      |
+------------------------+----------+-------------------------------------------+-----------------------------+

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset
	# to build the project
	make PLATFORM=maxim TARGET=max32690
	# to flash the code
	make run
