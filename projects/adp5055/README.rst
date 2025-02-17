ADP5055 no-OS Example Project
=============================

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `ADP5055-EVALZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-adp5055.html>`_

Overview
--------

The ADP5055-EVALZ is a full-featured evaluation board showcasing three high performance
buck regulators with no need for external components. Channels 1 and 2 feature 7A synchronous
buck regulators or as a 14A output in parallel operation. Channel 3 features a 3A synchronous
buck regulator. The board has a wide input voltage range of 2.75V to 18V and a bias input
voltage range of 4.5V to 18V. The adjustable switching frequency range is from 250kHz to 2500kHz.

Full performance details are provided in the ADP5055 data sheet, which should
be consulted in conjunction with user guide.

Hadrware Specifications
-----------------------

Power Supply Requirments
^^^^^^^^^^^^^^^^^^^^^^^^

For this specific project an external power supply is used to provide a 5V
input at J11, capable of supplying power to all 3 output channels at the following load settings:
VOUT1 = 1.0V at up to 7A continuous load current, VOUT2 = 1.3V at up to 7A continuous load current,
VOUT3 = 1.8V at up to 3A continuous load current

**Pin Description**

	Please see the following table for the pin assignments for the interface
	connector J4.

	J4:

	+-----+----------+-------------------------------------------+
	| Pin |   Name 	 | Description				     |
	+-----+----------+-------------------------------------------+
	| 1   | GND  | Do Not Connect			     |
	+-----+----------+-------------------------------------------+
	| 2   | SDA     | Connect to Ground			     |
	+-----+----------+-------------------------------------------+
	| 3   | SCL     | Power Supply, +12V DC		     |
	+-----+----------+-------------------------------------------+

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adp5055/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adp5055/src/platform>`_

Basic example
^^^^^^^^^^^^^

This is a simple example that initializes the ADP5055, exercising and setting output features at channel 1, such as enabling dynamic voltage scaling (DVS) and overcurrent protection (OCP) blanking.
Then, the output reference voltage limits and value are configured to set the output voltage defined in:
`Project Common Data Header <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adp5055/src/common>`_

.. code-block:: bash
	
	// Target Output Voltage
	#define EXAMPLE_VOUT1 1.2
	#define EXAMPLE_VOUT2 1.3
	#define EXAMPLE_VOUT3 1.8

The external feedback resistor values are also defined.

.. code-block:: bash
	
	// External Feedback Resistors
	#define EXAMPLE_RTOP1 20000.0
	#define EXAMPLE_RBOT1 30100.0
	#define EXAMPLE_RTOP2 20000.0
	#define EXAMPLE_RBOT2 16900.0
	#define EXAMPLE_RTOP3 20000.0
	#define EXAMPLE_RBOT3 10000.0

In order to build the basic example make sure you have the following configuration in the Makefile
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adp5055/Makefile>`_

.. code-block:: bash

	EXAMPLE = basic

IIO example
^^^^^^^^^^^

This project is actually a IIOD demo for ADP5055-EVALZ evaluation board.
The project launches a IIOD server on the board so that the user may connect
to it via an IIO client.

Using IIO-Oscilloscope, the user can configure global and channel attributes.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The No-OS IIO Application together with the No-OS IIO ADP5055 driver take care of
all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adp5055/src/examples/iio_example>`_

In order to build the IIO project make sure you have the following configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adp5055/Makefile>`_

.. code-block:: bash

        # Select the example you want to build by passing one of the following to make
        EXAMPLE = iio_example

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**

* `ADP5055-EVALZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-adp5055.html>`_
* `AD-APARD32690-SL <https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/ad-apard32690-sl.html>`_

**Connections**:

Terminal Connections:

+-----------------------------+------------+----------------------------------------------+-----------------------------+
| Terminal | Mnemonic   | Function					                                                |
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| J11	   | PVIN1	    | External Supply Positive Input Terminal				                    |
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| J12	   | GND	    | External Supply Ground Terminal                                           |
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| J13	   | VOUT1	    | Channel 1 Output (May connect to oscilloscope or voltmeter positive lead) |
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| J14	   | GND	    | Ground (May connect oscilloscope ground or voltmeter negative lead)	    |
+-----------------------------+------------+----------------------------------------------+-----------------------------+

J4:

+-----------------------------+------------+----------------------------------------------+-----------------------------+
| ADP1050DC1-EVALZ Pin Number |  Mnemonic  | Function			| AD-APARD32690-SL Pin Number |
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 1			                  | GND	       | Ground		        | GND			              |
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 2			                  | SCL	       | I2C Serial Clock   | I2C0_SCL		              |
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 3			                  | SDA	       | I2C Serial Data	| I2C0_SDA			          |
+-----------------------------+------------+----------------------------------------------+-----------------------------+

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset
	# to build the project
	make PLATFORM=maxim TARGET=max32690 EXAMPLE=basic
	# to flash the code
	make run
