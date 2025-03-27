Evaluating the ADMT4000
=======================


Contents
--------

.. contents:: Table of Contents
	:depth: 3

Supported Evaluation Boards
---------------------------

* NOT YET AVAILABLE IN PRODUCT PAGE

Overview
--------

The ADMT4000 is a magnetic turn, counter sensor capable of recording turns of an 
external magnetic field with zero power. The absolute position, including the 
number of turns, is reported via a serial peripheral interface (SPI). 

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

A power supply of 5V via USB Type-C is needed.

**Pin Description**:

	+-----+----------+-------------------------------------------+
	| Pin | Name 	 | Description				     |
	+-----+----------+-------------------------------------------+
	| 1   | DV18	 | Decoupling cap		    	     |
	+-----+----------+-------------------------------------------+
	| 2   | ACALC    | Output signal	   		     |
	+-----+----------+-------------------------------------------+
	| 3   | GPIO4    | I/O	    				     |
	+-----+----------+-------------------------------------------+
	| 4   | BOOTLOAD | I2C Serial Clock			     |
	+-----+----------+-------------------------------------------+
	| 5   | VDD      | Supply                                    |
	+-----+----------+-------------------------------------------+
	| 15  | SDO	 | Serial Data out	   		     |
	+-----+----------+-------------------------------------------+
	| 16  | SDI	 | Serial Data input    		     |
	+-----+----------+-------------------------------------------+
	| 17  | SCLK     | Serial Clock	    			     |
	+-----+----------+-------------------------------------------+
	| 18  | CS       | Chip Select     			     |
	+-----+----------+-------------------------------------------+
	| 19  | BUSY     | Busy signal	     			     |
	+-----+----------+-------------------------------------------+
	| 20  | CNV      | Conversion signal	     		     |
	+-----+----------+-------------------------------------------+
	| 21  | VDRIVE   | Logic power supply     		     |
	+-----+----------+-------------------------------------------+
	| 22  | RESET    | GMR Reset	     			     |
	+-----+----------+-------------------------------------------+


No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/lt3074/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/lt3074/src/platform>`_

IIO example
^^^^^^^^^^^

The IIO device driver contains the IIO specific implementations for the ADMT4000. 
The functions in this driver invoke functions from the ADMT4000 driver; however, 
formatted in such a way that it is compatible with IIO hosts, such as IIO oscilloscope. 
Items such as, but not limited to, available measurement channels, channel attribute 
implementations, device-specific IIO attributes, IIO device name, and register debug functions 
are included in the IIO drivers. The IIO driver works together with the IIO example 
project to create firmware that transforms the ADMT4000 and the controller to a valid IIO device.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

In order to build the IIO project:

.. code-block:: bash

		make EXAMPLE=iio_example

		
No-OS Supported Platforms
-------------------------

MBED Platform
^^^^^^^^^^^^^^

**Used hardware**

* `SDP-K1 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/sdp-k1.html>`_

**Connections**:

P5:
+----------+-----------------------+
| ADMT4000 | SDP-K1 Arduino header |
+----------+-----------------------+
| SPI_SCK  | D13                   |
+----------+-----------------------+
| SPI_MISO | D12                   |
+----------+-----------------------+
| SPI_MOSI | D11                   |
+----------+-----------------------+
| SPI_CS   | D10                   |
+----------+-----------------------+

P4:
+----------+-----------------------+
| ADMT4000 | SDP-K1 Arduino header |
+----------+-----------------------+
| ACALC    | D6                    |
+----------+-----------------------+
| CNV      | D5                    |
+----------+-----------------------+
| COIL_RS  | D4                    |
+----------+-----------------------+
| BUSY     | D3                    |
+----------+-----------------------+
| SHDN_N   | D2                    |
+----------+-----------------------+

**Build Command**

.. code-block:: bash

		# to delete current build
		make reset
		# to build the project
		make PLATFORM=mbed
		# to flash the code
		make run