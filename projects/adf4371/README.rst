ADF4371 no-OS Example Project
=============================

.. no-os-doxygen::

Contents
--------

.. contents:: Table of Contents
	:depth: 1

Supported Evaluation Boards
---------------------------

* `EVAL-ADF4371 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-adf4371.html>`_

Overview
--------
The EV-ADF4371SD2Z evaluates the performance of the ADF4371 frequency synthesizer
with an integrated voltage controlled oscillator (VCO) for phase-locked loops (PLLs).
The evaluation board contains the ADF4371 frequency synthesizer with an integrated
VCO, a USB interface, power supply connectors, and subminiature Version A (SMA)
connectors.

Full specifications for the ADF4371 frequency synthesizer are available in the
ADF4371 data sheet, which must be consulted in conjunction with this user guide
when working with the evaluation board.

Applications
------------

* Wireless infrastructure (multicarrier global system for mobile communication (MC-GSM), 5 G)
* Test equipment and instrumentation
* Clock generation
* Aerospace and defense


Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The EV-ADF4371SD2Z is powered by a 6 V power supply connected to the VUSPPL SMA
connector, or the P2 banana plug and GND to the P4 banana plug.

Reference Input Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The EV-ADF4371SD2Z board is supplied with a low noise
100 MHz crystal oscillator (XO) from Crystek (CCHD-575-50-
100.000).

To use an external single-ended REF IN, connect a low noise
reference source to the REFP SMA connector. Remove
Resistor R19 (0 Ω) and Resistor R20 (0 Ω) to remove power
from the crystal and break the connection to the REFP input.

Pin Description
^^^^^^^^^^^^^^^

Please see the following table for the pin assignments for the interface
connector (P1).

	P1:

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
    | 43  | GPIO0             | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 44  | GPIO2             | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 45  | GPIO4             | Not connected                            |
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
    | 56  | EEPROM_A0         | Not connected                            |
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
    | 79  | SCL_0             | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 80  | SDA_0             | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 81  | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 82  | SPI_CLK           | Connected to SCLK of the ADF4371         |
    +-----+-------------------+------------------------------------------+
    | 83  | SPI_MISO          | Connected to MUXOUT (SDO) of the ADF4371 |
    +-----+-------------------+------------------------------------------+
    | 84  | SPI_MOSI          | Connected to SDIO (SDI) of the ADF4371   |
    +-----+-------------------+------------------------------------------+
    | 85  | SPI_SEL_A_N       | Connected to CSB of the ADF4371          |
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

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/master/projects/adf4371/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/master/projects/adf4371/src/platform>`_

Basic example
^^^^^^^^^^^^^

This example initializes the ADF4371 with the configurations provided in the
above mentioned common files and applies them to the IC. By default reference
frequency is provided by the local oscillator of the board. Subsequently the
example sets a test frequency of 8 GHz to channel 0 (RF8).

In order to build the basic example make sure you have the following
configuration in the Makefile
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/master/projects/adf4371/Makefile>`_

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	EXAMPLE ?= basic

No-OS Supported Platforms
-------------------------

STM32 Platform
^^^^^^^^^^^^^^

**Used hardware**

* `EVAL-ADF4371 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-adf4371.html>`_
* `SDP-K1 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/sdp-k1.html>`_

**Connections**:

The SDP connector of the SDP-K1 needs to be connected to P1 of the EVAL-ADF4371
board. Additionally a 6V power supply needs to be connected to either P2 - P4
or to VSUPPL (the SMA interface).

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
