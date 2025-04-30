Evaluating the ADF4030
======================

Contents
--------

.. contents:: Table of Contents
	:depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-ADF4030 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-adf4030.html>`_

Overview
--------

- EV-ADF4030SD1Z evaluation board including the ADF4030 10channel precision synchronizer, Arduino interface, and voltage regulators
- Allows multiple ways to provide a reference clock to the ADF4030
- Single-ended from a signal generator
- Differential from another device or additional ADF4030 devices
- Gives access to all 10 ADF4030 BSYNC bidirectional I/O lines using twinax and SMA connectors
- Facilitates interfacing the ADF4030 clocks with an oscilloscope or other boards using an interposer board
- SDP-K1 controller board functions as an interface between a PC and the evaluation board
- Up to 16 EV-ADF4030SD1Z evaluation boards can be stacked up using the Arduino connector


Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The EV-ADF4030SD1Z is powered by a 12 V power supply connected to the TP1 banana plug and GND 
to the TP2 banana plug or one 12 V, 24 W AC/DC external wall mount (Class II) adapter

Reference Input Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The EV-ADF4030SD1Z has two ways to create the ADF4030 RE-FIN clock, a clock between 10 MHz and 250 MHz as
follows:


* Using a J1 Subminiature Version A (SMA) connector REF_CLK S/E in. Apply a 4 dBm signal, which means a 1 V p-p clock. A 50 Ω load resistor to ground (R1) is populated on board.

* Using a P1 twinax connector (REFIN DIFF). Apply a 320 mV p-p to 2 V p-p differential voltage. 

This must be a AC-coupled input to the ADF4030. This connector is suitable for
connecting the reference clock from another EV-ADF4030SD1Z.


	P5:

    +-----+-------------------+------------------------------------------+
    | Pin | Name              | Description                              |
    +-----+-------------------+------------------------------------------+
    | 1   | GPIO_3            | Connected to RF switches but default DNI |
    +-----+-------------------+------------------------------------------+
    | 2   | SPI_CS_B          | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 3   | SPI_CS_A          | Connected to CS_N of the ADF4030         |
    +-----+-------------------+------------------------------------------+
    | 4   | SPI_SDIO          | Connected to SDIO of the ADF4030         |
    +-----+-------------------+------------------------------------------+
    | 5   | SPI_SDO           | Connected to SDO of the ADF4030          |
    +-----+-------------------+------------------------------------------+
    | 6   | SPI_SCLK          | Connected to SCLK of the ADF4030         |
    +-----+-------------------+------------------------------------------+
    | 7   | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 8   | GND               | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 9   | SDA_USB           | Connected to the SCL of the EEPROM       |
    +-----+-------------------+------------------------------------------+
    | 10  | SCL_USB           | Connected to the SDA of the EEPROM       |
    +-----+-------------------+------------------------------------------+

	P17:

    +-----+-------------------+------------------------------------------+
    | Pin | Name              | Description                              |
    +-----+-------------------+------------------------------------------+
    | 1   | GND               | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 2   | GND               | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 3   | GPIO_0            | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 4   | GND               | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 5   | GPIO_1            | Connected to IRQ_N of the ADF4030        |
    +-----+-------------------+------------------------------------------+
    | 6   | SPI_CS_D          | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 7   | SPI_CS_C          | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 8   | GPIO_2            | Not connected                            |
    +-----+-------------------+------------------------------------------+

	P18:

    +-----+-------------------+------------------------------------------+
    | Pin | Name              | Description                              |
    +-----+-------------------+------------------------------------------+
    | 1   | GND               | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 2   | GND               | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 2   | GND               | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 4   | GND               | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 5   | GND               | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 6   | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 7   | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 8   | GND               | Not connected                            |
    +-----+-------------------+------------------------------------------+

	P16:

    +-----+-------------------+------------------------------------------+
    | Pin | Name              | Description                              |
    +-----+-------------------+------------------------------------------+
    | 1   | GPIO_4            | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 2   | GPIO_5            | Connected to RF switches but default DNI |
    +-----+-------------------+------------------------------------------+
    | 3   | GPIO_6            | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 4   | GPIO_7            | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 5   | GND               | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 6   | GND               | Not connected                            |
    +-----+-------------------+------------------------------------------+


No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/master/projects/adf4030/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/master/projects/adf4030/src/platform>`_



Basic example
^^^^^^^^^^^^^

This example initializes the ADF4030 with the configurations provided in the
above mentioned common files and applies them to the IC. By default reference
frequency is provided as 125 MHz. Example assign BSYNC1 and BSYNC 2 channels 
as TX and align them with the 200 ps delay. Then it perform a TDC measurement
to check the delay between the two channels. 

In order to build the basic example make sure you are using this command:

.. code-block:: bash

    make EXAMPLE=basic


IIO example
^^^^^^^^^^^

This project provide an IIOD demo for the ADF4030. It launches an IIOD server on
the mother platform it runs on allowing the user to connect via an IIOD client. 
Using IIO-Oscilloscope, the user can configure the ADF4030. 

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

The No-OS IIO Application together with the No-OS IIO ADF4030 driver take care
of all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <https://github.com/analogdevicesinc/no-OS/tree/master/projects/adf4030/src/examples/iio_example>`_

In order to build the IIO project make sure you you are using this command:

.. code-block:: bash

    make EXAMPLE=iio_example


No-OS Supported Platforms
-------------------------

Mbed Platform
^^^^^^^^^^^^^
**Used hardware**

* `EVAL-ADF4030 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-adf4030.html>`_
* `SDP-K1 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/sdp-k1.html>`_


**Connections**:

The SDP connector of the SDP-K1 needs to be connected to P5, P17, P18 and P16 of the EVAL-ADF4030
board. 12V power supply needs to be connected. Reference clock can be provided using the SMA connector.

**Build Command**

.. code-block:: bash

	# add the arm gcc to the PATH variable
	export PATH=</path/to/arm/gcc/bin>:$PATH
	# to delete current build
	make reset
	# to build the project
	make PLATFORM=mbed
	# to flash the code
	make
	# copy the adf4030.bin to the mounted SDP-K1
	cp build/adf4030.bin </path/to/SDP-K1/mounted/folder>