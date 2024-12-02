Evaluating the LTM4686
======================

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `DC2722A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc2722a.html>`_
* `DC3089A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc3089a.html>`_
* `DC2810A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc2810a.html>`_

Overview
--------

The DC2722A/DC3089A are dual-output, high efficiency, high density, µModule
regulator with 2.7V to 17V/5.75V input range. Each output can supply 10A/14A
maximum load current. Meanwhile, the DC2810A is a wide input and output voltage
range, high efficiency and  power density, quad output DC/DC step-down 
µModule regulator with default input range of 4.5V to 15V.

Full performance details are provided in the corresponding data sheet for
LTM4686, LTM4686B and LTM4673, which should be consulted in conjunction with
user guide.

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

This specific project makes use of the DC2722A powered up by a 12V power supply.

**Pin Description**

	+-----+----------+-------------------------------------------+
	| Pin |   Name 	 | Description				     |
	+-----+----------+-------------------------------------------+
	|  1  | VDD25	 | 2.5V Power supply output		     |
	+-----+----------+-------------------------------------------+
	|  2  | SGND	 | Signal ground			     |
	+-----+----------+-------------------------------------------+
	|  3  | RUN1	 | Enable pin for Channel 1		     |
	+-----+----------+-------------------------------------------+
	|  4  | GPIO1	 | General Purpose Input Output		     |
	+-----+----------+-------------------------------------------+
	|  5  | SDA	 | I2C Serial Data Line			     |
	+-----+----------+-------------------------------------------+
	|  6  | SCL	 | I2C Serial Clock Line		     |
	+-----+----------+-------------------------------------------+
	|  7  | ALERT	 | Alert output pins			     |
	+-----+----------+-------------------------------------------+
	|  8  | SGND	 | Signal ground			     |
	+-----+----------+-------------------------------------------+
	|  9  | AUXP	 | Auxiliary I2C input supply		     |
	+-----+----------+-------------------------------------------+
	| 10  | GND1_SNS | Sense Ground				     |
	+-----+----------+-------------------------------------------+
	| 11  | VO1_SNS	 | Channel 1 sense voltage		     |
	+-----+----------+-------------------------------------------+
	| 12  | VO0_SNS	 | Channel 0 sense voltage		     |
	+-----+----------+-------------------------------------------+
	| 13  | GND0_SNS | Sense Ground				     |
	+-----+----------+-------------------------------------------+
	| 14  | IOSTEP_CLK| IOSTEP clock			     |
	+-----+----------+-------------------------------------------+
	| 15  | GND	 | Ground				     |
	+-----+----------+-------------------------------------------+
	| 16  | VDD33	 | 3.3V Power supply output		     |
	+-----+----------+-------------------------------------------+
	| 17  | RUN0	 | Enable pin for Channel 0		     |
	+-----+----------+-------------------------------------------+
	| 18  | GPIO0	 | General Purpose Input Output		     |
	+-----+----------+-------------------------------------------+
	| 19  | SYNC	 | Clock synchronization input		     |
	+-----+----------+-------------------------------------------+
	| 20  | INTVCC	 | Internal regulator 5V output		     |
	+-----+----------+-------------------------------------------+
	| 21  | SHARE_CLK| Bidirectional clock sharing pin	     |
	+-----+----------+-------------------------------------------+
	| 22  | GND	 | Ground				     |
	+-----+----------+-------------------------------------------+
	| 23  | VIN	 | Voltage input			     |
	+-----+----------+-------------------------------------------+
	| 24  | GND	 | Ground				     |
	+-----+----------+-------------------------------------------+

**Hardware Bringup**

For reference, follow the Quick Start Procedure section of the corresponding
demo board.
`user guide <https://www.analog.com/media/en/technical-documentation/user-guides/DC2722A_UG-1397.pdf>`_.

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltm4686/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltm4686/src/platform>`_

Basic example
^^^^^^^^^^^^^

This is a simple example that initializes the ltm4686, and performs telemetry
readings of the voltage, current and temperature of each output channel. Status
bytes/words are also monitored in the example.

In order to build the basic example make sure you have the following configuration in the Makefile
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltm4686/Makefile>`_

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

The No-OS IIO Application together with the No-OS IIO ltm4686 driver take care of
all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltm4686/src/examples/iio_example>`_

In order to build the IIO project make sure you have the following configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltm4686/Makefile>`_

.. code-block:: bash

        # Select the example you want to enable by choosing y for enabling and n for disabling
        BASIC_EXAMPLE = n
        IIO__EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**

* `DC2722A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc2722a.html>`_
* `MAX32666FTHR <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32666fthr.html>`_

**Connections**:

+-----------------------------+------------+----------------------------------------------+-----------------------------+
| DC2722A Pin Number	      |  Mnemonic  | Function					  | MAX32666FTHR Pin Number	|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 5			      | SDA	   | I2C Serial Data				  | I2C0_SCL		        |
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 6			      | SCL	   | I2C Serial Clock				  | I2C0_SDA		        |
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 11			      | VO1_SNS	   | Connect to load				  | Do Not Connect		|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 12			      | VO0_SNS	   | Connect to load				  | Do Not Connect		|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 15			      | GND	   | Ground					  | GND				|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 23			      | VIN	   | Connect to external power supply (12V)	  | Do Not Connect		|
+-----------------------------+------------+----------------------------------------------+-----------------------------+

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset
	# to build the project
	make PLATFORM=maxim TARGET=max32665
	# to flash the code
	make run
