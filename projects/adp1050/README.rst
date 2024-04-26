Evaluating the ADP1050
======================

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `ADP1050DC1-EVALZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-adp1050.html#eb-documentation>`_

Overview
--------

The evaluation board allows the ADP1050 to be exercised without the need for
external components. The board is set up to act as an isolated PSU,
outputting a rated load of 12 V, 20 A from a 36 V dc to 75 V dc source.

Full performance details are provided in the ADP1050 data sheet, which should
be consulted in conjunction with user guide.

Hadrware Specifications
-----------------------

Power Supply Requirments
^^^^^^^^^^^^^^^^^^^^^^^^

For this specific project an external power supply is used to provide a 20.1V
and 12V voltages for the ADP1050DC1-EVALZ daughter board to simulate a parallel
circuit to the ADP1050 as well as a +5V Power Supply for the PMBUS/I2C
interface.

**Pin Description**

	Please see the following table for the pin assignments for the interface
	connectors (J1 and J2).

	J1:

	+-----+----------+-------------------------------------------+
	| Pin |   Name 	 | Description				     |
	+-----+----------+-------------------------------------------+
	| 1   | 10V_VCC  | Do Not Connect			     |
	+-----+----------+-------------------------------------------+
	| 2   | VS-      | Connect to Ground			     |
	+-----+----------+-------------------------------------------+
	| 3   | VS+      | Power Supply, +12V DC		     |
	+-----+----------+-------------------------------------------+
	| 4   | CS2-     | Do Not Connect			     |
	+-----+----------+-------------------------------------------+
	| 5   | CS2+	 | Do Not Connect			     |
	+-----+----------+-------------------------------------------+
	| 6   | VF	 | Power Supply, +20.1V DC		     |
	+-----+----------+-------------------------------------------+
	| 7   | CS1	 | Connect to Ground			     |
	+-----+----------+-------------------------------------------+
	| 8   | SR1	 | Connect to Oscilloscope (Scopy)	     |
	+-----+----------+-------------------------------------------+
	| 9   | SR2	 | Connect to Oscilloscope (Scopy)	     |
	+-----+----------+-------------------------------------------+
	| 10  | OUTA     | Connect to Oscilloscope (Scopy)	     |
	+-----+----------+-------------------------------------------+
	| 11  | OUTB	 | Connect to Oscilloscope (Scopy)	     |
	+-----+----------+-------------------------------------------+
	| 12  | OUTC     | Do Not Connect			     |
	+-----+----------+-------------------------------------------+
	| 13  | OUTD	 | Do Not Connect			     |
	+-----+----------+-------------------------------------------+
	| 14  | SCL	 | I2C Serial Clock (Connect to J2 instead)  |
	+-----+----------+-------------------------------------------+
	| 15  | SDA	 | I2C Serial Data (Connect to J2 instead)   |
	+-----+----------+-------------------------------------------+
	| 16  | CTRL	 | Do Not Connect			     |
	+-----+----------+-------------------------------------------+
	| 17  | PG/ALT#  | Power-Good Signal (GPIO)		     |
	+-----+----------+-------------------------------------------+
	| 18  | FLAGIN   | Do Not Connect			     |
	+-----+----------+-------------------------------------------+
	| 19  | 3.3V	 | Do Not Connect			     |
	+-----+----------+-------------------------------------------+
	| 20  | AGND     | Connect to Ground			     |
	+-----+----------+-------------------------------------------+
	| 21  | RTD	 | Do Not Connect			     |
	+-----+----------+-------------------------------------------+
	| 22  | OVP      | Do Not Connect			     |
	+-----+----------+-------------------------------------------+

	J2:

	+-----+----------+-------------------------------------------+
	| Pin |   Name 	 | Description				     |
	+-----+----------+-------------------------------------------+
	| 1   | 10V_VDD  | Power Supply, +5V			     |
	+-----+----------+-------------------------------------------+
	| 2   | SCL      | I2C Serial Clock			     |
	+-----+----------+-------------------------------------------+
	| 3   | SDA      | I2C Serial Data			     |
	+-----+----------+-------------------------------------------+
	| 4   | GND      | Connect to Ground			     |
	+-----+----------+-------------------------------------------+

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adp1050/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adp1050/src/platform>`_

Basic example
^^^^^^^^^^^^^

This is a simple example that initializes the ADP1050, unlock its CHIP, EEPROM
and TRIM registers, sets the device in a closed loop state, and then : sets the
VOUT transition rate, VOUT scale and value of the VOUT_COMMAND and VOUT_MAX.

For the PWM it sets the duty cycle on the OUTA channel and then enables the OUTA
channel with a 49KHz frequency, then the DUTY_CYCLE data is read as well as all
the statuses.

In order to build the basic example make sure you have the following configuration in the Makefile
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adp1050/Makefile>`_

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	BASIC_EXAMPLE = y
	IIO_EXAMPLE = n

IIO example
^^^^^^^^^^^

This project is actually a IIOD demo for ADP1050DC1-EVALZ evaluation board.
The project launches a IIOD server on the board so that the user may connect
to it via an IIO client.
Using IIO-Oscilloscope, the user can configure the IMU and view the measured data on a plot.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The No-OS IIO Application together with the No-OS IIO ADP1050 driver take care of
all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adp1050/src/examples/iio_example>`_

In order to build the IIO project make sure you have the following configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adp1050/Makefile>`_

.. code-block:: bash

        # Select the example you want to enable by choosing y for enabling and n for disabling
        BASIC_EXAMPLE = n
        IIO__EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**

* `ADP1050DC1-EVALZ daughter board <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-adp1050.html#eb-overview>`_
* `AD-APARD32690-SL <https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/ad-apard32690-sl.html>`_

**Connections**:

J1:

+-----------------------------+------------+----------------------------------------------+-----------------------------+
| ADP1050DC1-EVALZ Pin Number |  Mnemonic  | Function					  | AD-APARD32690-SL Pin Number |
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 2			      | VS-	   | Ground					  | GND			        |
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 3			      | VS+	   | External Power Supply, 12VDC (5mA current)   | Do Not Connect	        |
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 6			      | VF	   | External Power Supply, 20.1VDC (5mA current) | Do Not Connect		|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 7			      | CS1	   | Ground					  | GND				|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 8			      | SR1	   | SR1 channel output (May connect to Scopy)	  | Do Not Connect		|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 9			      | SR2	   | SR2 channel output (May connect to Scopy)	  | Do Not Connect		|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 10			      | OUTA	   | OUTA channel output (May connect to Scopy)	  | Do Not Connect		|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 11			      | OUTB	   | OUTB channel output (May connect to Scopy)	  | Do Not Connect		|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 17			      | PG/ALT#    | Power-Good Signal Logic OUTPUT		  | P0_24			|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 20			      | AGND	   | Ground					  | GND				|
+-----------------------------+------------+----------------------------------------------+-----------------------------+

J2:

+-----------------------------+------------+----------------------------------------------+-----------------------------+
| ADP1050DC1-EVALZ Pin Number |  Mnemonic  | Function					  | AD-APARD32690-SL Pin Number |
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 1			      | 10V_VDD	   | Power Supply, +5VDC			  | 5V0			        |
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 2			      | SCL	   | I2C Serial Clock				  | I2C0_SCL		        |
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 3			      | SDA	   | I2C Serial Data				  | I2C0_SDA			|
+-----------------------------+------------+----------------------------------------------+-----------------------------+
| 4			      | GND	   | Ground					  | GND				|
+-----------------------------+------------+----------------------------------------------+-----------------------------+

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset
	# to build the project
	make PLATFORM=maxim TARGET=max32690
	# to flash the code
	make run
