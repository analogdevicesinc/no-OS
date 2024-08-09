Evaluating the LT7182S
======================

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `DC2836A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc2836a.html>`_

Overview
--------

The evaluation board allows the LT7182S to be powered up to default settings and
produces power based on configuration resistors without the need for any serial
bus communication. The board by default is set to output voltages of 1.2 V and
3.3 V for channel 0 and channel 1, respectively. Each output can supply 6A
continuous and 9A maximum transient load current.

Full performance details are provided in the LT7182S data sheet, which should
be consulted in conjunction with user guide.

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

For this specific project an external power supply with output of 4V to 20V can
be used to power up the demo board.

**Pin Description**

	+-----+----------+-------------------------------------------+
	| Pin |   Name 	 | Description				     |
	+-----+----------+-------------------------------------------+
	| 1   | VEMI     | Power Supply, 4V - 20V		     |
	+-----+----------+-------------------------------------------+
	| 2   | GND      | Connect to Ground			     |
	+-----+----------+-------------------------------------------+
	| 3   | VOUT0    | Connect to Load 0			     |
	+-----+----------+-------------------------------------------+
	| 4   | VOUT1    | Connect to Load 1			     |
	+-----+----------+-------------------------------------------+
	| 5   | ALERT	 | GPIO (Optional)			     |
	+-----+----------+-------------------------------------------+
	| 6   | EXTVCC	 | Do Not Connect			     |
	+-----+----------+-------------------------------------------+
	| 7   | PGOOD0	 | GPIO (Optional)			     |
	+-----+----------+-------------------------------------------+
	| 8   | PGOOD1	 | GPIO (Optional)			     |
	+-----+----------+-------------------------------------------+
	| 9   | SYNC	 | Do Not Connect			     |
	+-----+----------+-------------------------------------------+
	| 10  | SHARE_CLK| Do Not Connect			     |
	+-----+----------+-------------------------------------------+
	| 11  | SCL	 | I2C Serial Clock			     |
	+-----+----------+-------------------------------------------+
	| 12  | SDA      | I2C Serial Data			     |
	+-----+----------+-------------------------------------------+

**Hardware Bringup**

For reference, follow the Quick Start Procedure section of the demo board
`user guide <https://www.analog.com/media/en/technical-documentation/user-guides/dc2836a.pdf>`_.

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/lt7182s/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/lt7182s/src/platform>`_

Basic example
^^^^^^^^^^^^^

This is a simple example that initializes the LT7182S, and performs telemetry
readings of the voltage, current and temperature of each output channel. Status
bytes/words are also monitored in the example.

In order to build the basic example make sure you have the following configuration in the Makefile
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/lt7182s/Makefile>`_

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	BASIC_EXAMPLE = y
	IIO_EXAMPLE = n

IIO example
^^^^^^^^^^^

This project is actually a IIOD demo for DC2836A evaluation board.
The project launches a IIOD server on the board so that the user may connect
to it via an IIO client.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The No-OS IIO Application together with the No-OS IIO LT7182S driver take care of
all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/lt7182s/src/examples/iio_example>`_

In order to build the IIO project make sure you have the following configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/lt7182s/Makefile>`_

.. code-block:: bash

        # Select the example you want to enable by choosing y for enabling and n for disabling
        BASIC_EXAMPLE = n
        IIO__EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**

* `DC2836A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc2836a.html>`_
* `MAX32666FTHR <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32666fthr.html>`_

**Connections**:

+-----------------------------+------------+----------------------------------------------+-----------------------------+
| LT7182SDC1-EVALZ Pin Number |  Mnemonic  | Function					  | MAX32666FTHR Pin Number	|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 1			      | VEMI	   | External Power Supply, 4V - 20V		  | Do Not Connect	        |
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 2			      | END	   | Connect to Ground				  | GND			        |
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 3			      | VOUT0	   | May connect to Scopy/Load			  | Do Not Connect		|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 4			      | VOUT1	   | May connect to Scopy/Load			  | Do Not Connect		|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 11			      | SCL	   | I2C Serial Clock				  | I2C0_SCL			|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 12			      | SDA	   | I2C Serial Data				  | I2C0_SDA			|
+-----------------------------+------------+----------------------------------------------+-----------------------------+

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset
	# to build the project
	make PLATFORM=maxim TARGET=max32665
	# to flash the code
	make run
