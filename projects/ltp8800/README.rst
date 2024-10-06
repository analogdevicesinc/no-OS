Evaluating the LTP8800
======================

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `DC3190A-A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc3190a-a.html>`_
* `DC3190A-B <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc3190a-b.html>`_
* `DC3190B-E <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc3190b-e.html>`_

Overview
--------

The evaluation board allows the LTP8800 to be powered up to default settings and
produces power based on configuration resistors without the need for any serial
bus communication. The board by default is set to output voltages of 0.8V for
LTP8800-1A and LTP8800-4A, 0.75V for LTP8800-2. Each output is rated at 150A/
200A/135A for LTP8800-1A/4A/2, respectively.

Full performance details are provided in the LTP8800 data sheet, which should
be consulted in conjunction with user guide.

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

For this specific project an external power supply with outputs 3.3V, 7V and 54V
must be used to power up the demo board.

**Pin Description**

	+-----+----------+-------------------------------------------+
	| Pin |   Name 	 | Description				     |
	+-----+----------+-------------------------------------------+
	| 1   | SMBALERT | PMBus SMBALERT Signal		     |
	+-----+----------+-------------------------------------------+
	| 2   | ISHARE   | Analog Current Sharing Input/Output	     |
	+-----+----------+-------------------------------------------+
	| 3   | CTRL	 | Hardware ON/OFF control		     |
	+-----+----------+-------------------------------------------+
	| 4   | ADD	 | Address select			     |
	+-----+----------+-------------------------------------------+
	| 5   | VS+	 | Non-inverting voltage sense input	     |
	+-----+----------+-------------------------------------------+
	| 6   | VS-	 | Inverting voltage sense input	     |
	+-----+----------+-------------------------------------------+
	| 7   | SYNC	 | Synchronization input signal		     |
	+-----+----------+-------------------------------------------+
	| 8   | 3V3	 | Internal module power supply (7V)	     |
	+-----+----------+-------------------------------------------+
	| 9   | GND	 | Connect to Ground			     |
	+-----+----------+-------------------------------------------+
	| 10  | 7V0	 | Internal module power supply (7V)	     |
	+-----+----------+-------------------------------------------+
	| 11  | VOUT	 | Output Voltage			     |
	+-----+----------+-------------------------------------------+
	| 12  | VOUT	 | Output Voltage			     |
	+-----+----------+-------------------------------------------+
	| 13  | VOUT	 | Output Voltage			     |
	+-----+----------+-------------------------------------------+
	| 14  | GND	 | Ground				     |
	+-----+----------+-------------------------------------------+
	| 15  | GND	 | Ground				     |
	+-----+----------+-------------------------------------------+
	| 16  | VOUT	 | Output Voltage			     |
	+-----+----------+-------------------------------------------+
	| 17  | VOUT	 | Output Voltage			     |
	+-----+----------+-------------------------------------------+
	| 18  | VOUT	 | Output Voltage			     |
	+-----+----------+-------------------------------------------+
	| 19  | GND	 | Ground				     |
	+-----+----------+-------------------------------------------+
	| 20  | SCL	 | Serial Clock				     |
	+-----+----------+-------------------------------------------+
	| 21  | SDA	 | Serial Data				     |
	+-----+----------+-------------------------------------------+
	| 22  | VIN	 | Input Power Supply (45V to 65V)	     |
	+-----+----------+-------------------------------------------+

**Hardware Bringup**

For reference, follow the Quick Start Procedure section of the corresponding
demo board.
`user guide <https://www.analog.com/media/en/technical-documentation/user-guides/dc3190a-a.pdf>`_.

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltp8800/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltp8800/src/platform>`_

Basic example
^^^^^^^^^^^^^

This is a simple example that initializes the LTP8800, and performs telemetry
readings of the voltage, current and temperature of each output channel. Status
bytes/words are also monitored in the example.

In order to build the basic example make sure you have the following configuration in the Makefile
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltp8800/Makefile>`_

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	BASIC_EXAMPLE = y
	IIO_EXAMPLE = n

IIO example
^^^^^^^^^^^

This project is actually a IIOD demo for DC3190A-A evaluation board.
The project launches a IIOD server on the board so that the user may connect
to it via an IIO client.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The No-OS IIO Application together with the No-OS IIO LTP8800 driver take care of
all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltp8800/src/examples/iio_example>`_

In order to build the IIO project make sure you have the following configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltp8800/Makefile>`_

.. code-block:: bash

        # Select the example you want to enable by choosing y for enabling and n for disabling
        BASIC_EXAMPLE = n
        IIO__EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**

* `DC3190A-A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc3190a-a.html>`_
* `MAX32666FTHR <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32666fthr.html>`_

**Connections**:

+-----------------------------+------------+----------------------------------------------+-----------------------------+
| DC3190A-A Pin Number	      |  Mnemonic  | Function					  | MAX32666FTHR Pin Number	|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 1			      | SMBALERT   | Do Not Connect				  | Do Not Connect	        |
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 2			      | ISHARE	   | Do Not Connect				  | Do Not Connect	        |
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 3			      | CTRL	   | Hardware ON/OFF control			  | P0_5/IO3			|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 7			      | SYNC	   | Do Not Connect				  | Do Not Connect		|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 8			      | 3V3	   | Power Supply (3V3)				  | 3V3 (optional, use external)|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 9			      | GND	   | Ground					  | GND				|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 10			      | BIAS	   | Connect to External Power Supply (7V0)	  | Do Not Connect		|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 11			      | VOUT	   | May connect to oscilloscope/load		  | Do Not Connect		|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 20			      | SCL	   | I2C Serial Clock				  | I2C0_SCL			|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 21			      | SDA	   | I2C Serial Data				  | I2C0_SDA			|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 22			      | VIN	   | Connect to External Power Supply (54V)	  | Do Not Connect		|
+-----------------------------+------------+----------------------------------------------+-----------------------------+

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset
	# to build the project
	make PLATFORM=maxim TARGET=max32665
	# to flash the code
	make run
