Evaluating the LT7170
======================

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-LT7170-AZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-lt7170-az.html>`_
* `EVAL-LT7170-1-AZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-lt7170-1-az.html>`_
* `EVAL-LT7171-AZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-lt7171-az.html>`_
* `EVAL-LT7171-1-AZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-lt7171-1-az.html>`_

Overview
--------

The EVAL-LT7170-AZ and EVAL-LT7170-1-AZ power up to default settings and 
produce power based on the NVM configuration without requiring any serial bus 
communication. Both can supply up to 20A continuous load current.

The EVAL-LT7171-AZ and EVAL-LT7171-1-AZ also power up to default settings and 
produce power based on the NVM configuration without requiring any serial bus 
communication. Both can supply up to 40A continuous load current.

For full performance details, refer to the LT7170/LT7170-1 and LT7171/LT7171-1 
data sheets, which should be consulted in conjunction with the user guide.

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
	| 1   | VEMI     | Power Supply, 2.9V - 16V		     |
	+-----+----------+-------------------------------------------+
	| 2   | GND      | Connect to Ground			     |
	+-----+----------+-------------------------------------------+
	| 3   | VOUT     | Connect to Load			     |
	+-----+----------+-------------------------------------------+
	| 4   | EXTVCC	 | Do Not Connect			     |
	+-----+----------+-------------------------------------------+
	| 5   | PGOOD	 | GPIO (Optional)			     |
	+-----+----------+-------------------------------------------+
	| 6   | ALERT	 | GPIO (Optional)			     |
	+-----+----------+-------------------------------------------+
	| 7   | FAULT	 | GPIO (Optional)			     |
	+-----+----------+-------------------------------------------+
	| 8   | SYNC	 | Do Not Connect			     |
	+-----+----------+-------------------------------------------+
	|  9  | SHARE_CLK| Do Not Connect			     |
	+-----+----------+-------------------------------------------+
	| 10  | SCL	 | I2C Serial Clock			     |
	+-----+----------+-------------------------------------------+
	| 11  | SDA      | I2C Serial Data			     |
	+-----+----------+-------------------------------------------+

**Hardware Bringup**

For reference, consult the Quick Start Procedure section in the user guide for the corresponding demo board:
`EVAL-LT7170-AZ user guide <https://www.analog.com/media/en/technical-documentation/user-guides/eval-lt7170-az.pdf>`_,
`EVAL-LT7170-1-AZ user guide <https://www.analog.com/media/en/technical-documentation/user-guides/eval-lt7170-1-az.pdf>`_,
`EVAL-LT7171-AZ user guide <https://www.analog.com/media/en/technical-documentation/user-guides/eval-lt7171-az-ug.pdf>`_, and
`EVAL-LT7171-1-AZ user guide <https://www.analog.com/media/en/technical-documentation/user-guides/eval-lt7171-1-az.pdf>`_.

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/lt7170/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/lt7170/src/platform>`_

Basic example
^^^^^^^^^^^^^

This is a simple example that initializes the LT7170, and performs telemetry
readings of the voltage, current and temperature. Status bytes/words are also 
monitored in the example.

In order to build the basic example make sure you have the following 
configuration in the Makefile
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/lt7170/Makefile>`_

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

The No-OS IIO Application together with the No-OS IIO LT7170 driver take care of
all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/lt7170/src/examples/iio_example>`_

In order to build the IIO project make sure you have the following 
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/lt7170/Makefile>`_

.. code-block:: bash

        # Select the example you want to enable by choosing y for enabling and n for disabling
        BASIC_EXAMPLE = n
        IIO__EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**

* `EVAL-LT7170-AZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-lt7170-az.html>`_
* `EVAL-LT7170-1-AZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-lt7170-1-az.html>`_
* `EVAL-LT7171-AZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-lt7171-az.html>`_
* `EVAL-LT7171-1-AZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-lt7171-1-az.html>`_
* `MAX32666FTHR <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32666fthr.html>`_

**Connections**:

+-----------------------------+------------+----------------------------------------------+-----------------------------+
| EVAL-LT7170-AZ Pin Number   |  Mnemonic  | Function					  | MAX32666FTHR Pin Number	|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 1			      | VEMI	   | External Power Supply, 2.9V - 16V		  | Do Not Connect	        |
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 2			      | END	   | Connect to Ground				  | GND			        |
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 3			      | VOUT	   | May connect to Scopy/Load			  | Do Not Connect		|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 10			      | SCL	   | I2C Serial Clock				  | I2C0_SCL			|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 11			      | SDA	   | I2C Serial Data				  | I2C0_SDA			|
+-----------------------------+------------+----------------------------------------------+-----------------------------+

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset
	# to build the project
	make PLATFORM=maxim TARGET=max32665
	# to flash the code
	make run
