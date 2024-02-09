Evaluating the ADF4382
======================

Contents
--------

.. contents:: Table of Contents
	:depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-ADF4382A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-adf4382a.html>`_

Overview
--------
The EV-ADF4382ASD2Z evaluates the performance of the ADF4382A frequency
synthesizer with an integrated voltage-con-trolled oscillator (VCO) for
phase-locked loops (PLLs). It  contains the ADF4382A frequency synthesizer with
an integrated VCO, a USB interface, power supply connectors, on-board reference
oscillator, propagation delay calibration paths, and Subminiature Version A (SMA)
connectors. The outputs of the EV-ADF4382ASD2Z are AC-coupled with 50 Ω
transmission lines, making these outputs suitable to drive 50 Ω impedance
instruments.

Full specifications for the ADF4382A frequency synthesizer are available in the
ADF4382A data sheet, which must be consulted in conjunction with this user guide
when working with the EV-ADF4382ASD2Z.

Applications
------------

* High Performance Data Converter Clocking
* Wireless infrastructure (MC-GSM, 5G, 6G)
* Test and Measurement

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The EV-ADF4382ASD2Z is powered by a 6 V power supply connected to the J18 SMA
connector, or the J14 banana plug and GND to the J15 banana plug.

Reference Input Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The EV-ADF4382ASD2Z has an on-board, 125 MHz, ultra low phase noise, sine-wave
oscillator to drive the ADF4382A reference input. Single-ended oscillator output
is connected to the REFP pin, and the REFN pin is AC grounded. This oscillator
is used by default on the board. 

The reference input can also be driven externally via a pair of SMA connectors,
REFN (J4) and REFP (J11). When using the external reference, disable the
on-board oscillator supply. This will require board modifications, please refer
to the `EVAL-ADF4382A User Guide <https://www.analog.com/media/en/technical-documentation/user-guides/eval-adf4382a-ug-2185.pdf>`_ for details.

Pin Description
^^^^^^^^^^^^^^^

Please see the following table for the pin assignments for the interface
connectors (P5 and P2).

	P5:

    +-----+-------------------+------------------------------------------+
    | Pin | Name              | Description                              |
    +-----+-------------------+------------------------------------------+
    | 1   | VIN               | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 2   | NC                | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 3   | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 4   | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 5   | USB_VBUS          | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 6   | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 7   | PAR_D23           | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 8   | PAR_D21           | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 9   | PAR_D19           | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 10  | PAR_D17           | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 11  | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 12  | PAR_D14           | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 13  | PAR_D13           | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 14  | PAR_D11           | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 15  | PAR_D9            | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 16  | PAR_D7            | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 17  | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 18  | PAR_D5            | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 19  | PAR_D3            | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 20  | PAR_D1            | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 21  | PAR_RD_N          | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 22  | PAR_CD_N          | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 23  | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 24  | PAR_A3            | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 25  | PAR_A1            | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 26  | PAR_FS3           | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 27  | PAR_FS1           | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 28  | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 29  | SPORT_TDV0        | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 30  | SPORT_TVD1        | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 31  | SPORT_DR1         | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 32  | SPORT_DT1         | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 33  | SPI_D2            | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 34  | SPI_D3            | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 35  | SERIAL_INT        | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 36  | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 37  | SPI_SEL_B_N       | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 38  | SPI_SEL_C_N       | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 39  | SPI_SEL1/SPI_SS_N | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 40  | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 41  | SDA_1             | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 42  | SCL_1             | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 43  | GPIO0             | Connected to MUXOUT of the ADF4382       |
    +-----+-------------------+------------------------------------------+
    | 44  | GPIO2             | Connected to DELSTR of the ADF4382       |
    +-----+-------------------+------------------------------------------+
    | 45  | GPIO4             | Connected to LKDET of the ADF4382        |
    +-----+-------------------+------------------------------------------+
    | 46  | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 47  | GPIO6             | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 48  | TMR_A             | Looped back to TMR_B                     |
    +-----+-------------------+------------------------------------------+
    | 49  | TMR_C             | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 50  | NC                | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 51  | NC                | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 52  | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 53  | NC                | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 54  | NC                | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 55  | NC                | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 56  | EEPROM_A0         | Connected to A0 of the EEPROM            |
    +-----+-------------------+------------------------------------------+
    | 57  | RESET_OUT_N       | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 58  | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 59  | UART_RX           | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 60  | RESET_IN_N        | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 61  | BMODE1            | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 62  | UART_TX           | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 63  | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 64  | SLEEP_N           | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 65  | WAKE_N            | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 66  | NC                | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 67  | NC                | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 68  | NC                | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 69  | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 70  | NC                | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 71  | CLKOUT            | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 72  | TMR_D             | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 73  | TMR_B             | Looped back to TMR_A                     |
    +-----+-------------------+------------------------------------------+
    | 74  | GPIO7             | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 75  | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 76  | GPIO5             | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 77  | GPIO3             | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 78  | GPIO1             | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 79  | SCL_0             | Connected to the SCL of the EEPROM       |
    +-----+-------------------+------------------------------------------+
    | 80  | SDA_0             | Connected to the SDA of the EEPROM       |
    +-----+-------------------+------------------------------------------+
    | 81  | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 82  | SPI_CLK           | Connected to SCLK of the ADF4382         |
    +-----+-------------------+------------------------------------------+
    | 83  | SPI_MISO          | Connected to SDO of the ADF4382          |
    +-----+-------------------+------------------------------------------+
    | 84  | SPI_MOSI          | Connected to SDIO of the ADF4382         |
    +-----+-------------------+------------------------------------------+
    | 85  | SPI_SEL_A_N       | Connected to CSB of the ADF4382          |
    +-----+-------------------+------------------------------------------+
    | 86  | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 87  | SPORT_TSCLK       | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 88  | SPORT_DT0         | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 89  | SPORT_TFS         | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 90  | SPORT_RFS         | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 91  | SPORT_DR0         | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 92  | SPORT_RSCLK       | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 93  | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 94  | PAR_CLK           | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 95  | PAR_FS2           | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 96  | PAR_A0            | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 97  | PAR_A2            | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 98  | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 99  | PAR_INT           | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 100 | PAR_WR_N          | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 101 | PAR_D0            | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 102 | PAR_D2            | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 103 | PAR_D4            | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 104 | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 105 | PAR_D6            | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 106 | PAR_D8            | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 107 | PAR_D10           | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 108 | PAR_D12           | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 109 | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 110 | PAR_D15           | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 111 | PAR_D16           | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 112 | PAR_D18           | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 113 | PAR_D20           | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 114 | PAR_D22           | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 115 | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 116 | VIO               | Connected to 3V3                         |
    +-----+-------------------+------------------------------------------+
    | 117 | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 118 | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 119 | NC                | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 120 | NC                | Not connected                            |
    +-----+-------------------+------------------------------------------+

	P2:

    +-----+-------------------+------------------------------------------+
    | Pin | Name              | Description                              |
    +-----+-------------------+------------------------------------------+
    | 1   | CSB               | Connected to CSB of the ADF4382          |
    +-----+-------------------+------------------------------------------+
    | 2   | NC                | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 3   | SDIO              | Connected to SDIO of the ADF4382         |
    +-----+-------------------+------------------------------------------+
    | 4   | NC                | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 5   | SDO               | Connected to SDO of the ADF4382          |
    +-----+-------------------+------------------------------------------+
    | 6   | NC                | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 7   | SCLK              | Connected to SCLK of the ADF4382         |
    +-----+-------------------+------------------------------------------+
    | 8   | CSB                | Connected to CSB of the ADF4382         |
    +-----+-------------------+------------------------------------------+
    | 9   | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 10  | NC                | Not connected                            |
    +-----+-------------------+------------------------------------------+

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/master/projects/adf4382/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/master/projects/adf4382/src/platform>`_

Basic example
^^^^^^^^^^^^^

This example initializes the ADF4382 with the configurations provided in the
above mentioned common files and applies them to the IC. By default reference
frequency is provided by the local oscillator of the board and channel 0 is
active with a power level of 9. Subsequently the example sets a test frequency
of 3.1 GHz and adjusts the phase by 1ns.


In order to build the basic example make sure you have the following
configuration in the Makefile
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/master/projects/adf4382/Makefile>`_

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	BASIC_EXAMPLE = y
	IIO_EXAMPLE = n

IIO example
^^^^^^^^^^^

This project provide an IIOD demo for the ADF4382. It launches an IIOD server on
the mother platform it runs on allowing the user to connect via an IIOD client. 
Using IIO-Oscilloscope, the user can configure the ADF4382A. 

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

The No-OS IIO Application together with the No-OS IIO ADF4382 driver take care
of all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <https://github.com/analogdevicesinc/no-OS/tree/master/projects/adf4382/src/examples/iio_example>`_

In order to build the IIO project make sure you have the following configuration in:
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/master/projects/adf4382/Makefile>`_

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	BASIC_EXAMPLE = n
	IIO_EXAMPLE = y

No-OS Supported Platforms
-------------------------

STM32 Platform
^^^^^^^^^^^^^^

**Used hardware**

* `EVAL-ADF4382A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-adf4382a.html>`_
* `SDP-K1 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/sdp-k1.html>`_

**Connections**:

The SDP connector of the SDP-K1 needs to be connected to P5 of the EVAL-ADF4382A
board. Additionally a 6V power supply needs to be connected to either J14 - J15
or to J18 (the SMA interface).

**Build Command**

.. code-block:: bash

	# set the path to STM32CUBEMX
	export STM32CUBEMX=</path/to/STM32CubeMX>
	# set the path to STM32CUBEIDE
	export STM32CUBEIDE=</path/to/stm32cubeide>
	# add the arm gcc to the PATH variable
	export PATH=</path/to/arm/gcc/bin>:$PATH
	# to delete current build
	make reset
	# to build the project
	make PLATFORM=stm32
	# to flash the code
	make run

Mbed Platform
^^^^^^^^^^^^^

**Used hardware**

* `EVAL-ADF4382A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-adf4382a.html>`_
* `SDP-K1 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/sdp-k1.html>`_

**Connections**:

The SDP connector of the SDP-K1 needs to be connected to P5 of the EVAL-ADF4382A
board. Additionally a 6V power supply needs to be connected to either J14 - J15
or to J18 (the SMA interface).

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
	# copy the adf4382.bin to the mounted SDP-K1
	cp build/adf4382.bin </path/to/SDP-K1/mounted/folder>