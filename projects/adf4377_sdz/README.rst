Evaluating the ADF4377 Wideband Synthesizer
===========================================

Contents
--------

.. contents:: Table of Contents
	:depth: 3

Supported Evaluation Boards
----------------------------

* `EVAL-ADF4377 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-adf4377.html>`_

Overview
--------

The ADF4377 is a high performance, ultralow jitter, dual output
integer-N phased locked loop (PLL) with an integrated voltage 
controlled oscillator (VCO) ideally suited for data converter and mixed
signal front end (MxFE) clock applications. The high performance
PLL has a figure of merit of −239 dBc/Hz, ultralow 1/f noise, and
a high phase frequency detector (PFD) frequency that can achieve
ultralow in-band noise and integrated jitter. The fundamental VCO
and output divider of the ADF4377 generate frequencies from 800
MHz to 12.8 GHz. The ADF4377 integrates all necessary power
supply bypass capacitors, saving board space on compact boards.

For multiple data converter and MxFE clock applications, the
ADF4377 simplifies clock alignment and calibration routines required 
with other clock solutions by implementing the automatic
reference to output synchronization feature, the matched reference
to output delays across process, voltage, and temperature feature,
and the less than ±0.1 ps, jitter free reference to output delay
adjustment capability feature.

These features allow for predictable and precise multichip clock and
system reference (SYSREF) alignment. JESD204B and JESD204C
Subclass 1 solutions are supported by pairing the ADF4377 with
an integrated circuit (IC) that distributes pairs of reference and
SYSREF signals.

Applications
------------
* High performance data converters
* Wireless infrastructure (MC-GSM, 5G)
* Test and measurement equipment

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^
ADF4377 is powered by 6V supply voltage connected to J12 SMA connector 
or a banana plug, J14 and GND to banana plug, J15.

Reference Clock Input
^^^^^^^^^^^^^^^^^^^^^
The EV-ADF4377 has an on-board 125MHz ultralow phase noise sine wave 
oscillator to drive the `EVAL-ADF4377 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-adf4377.html>`_ reference input. 
The reference input can also be driven externally via a pair of SMA
connectors, REFN (J4) and REFP (J11). The on-board oscillator supply must 
be disabled when using an external reference. The on-board oscillator is used by default.

The ADF4377 has a configurable reference input buffer whose
performance can be optimized for different reference slew rates,
amplitudes, and frequencies. Refer to the `EVAL-ADF4377 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-adf4377.html>`_ data sheet for
more information on the REF_SEL bit, BST_REF bit, and FILT_REFbit.
Refer to the ADF4377 data sheet for detailed reference buffer
amplitude and frequency considerations.

Pin Descriptions
^^^^^^^^^^^^^^^^^
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
    | 43  | GPIO0             | Connected to LKDET of the ADF4377        |
    +-----+-------------------+------------------------------------------+
    | 44  | GPIO2             | Connected to ENCLK2 of the ADF4377       |
    +-----+-------------------+------------------------------------------+
    | 45  | GPIO4             | Connected to MUXOUT of the ADF4377       |
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
    | 76  | GPIO5             | Connected to CE of the ADF4377           |
    +-----+-------------------+------------------------------------------+
    | 77  | GPIO3             | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 78  | GPIO1             | Connected to ENCLK1 of the ADF4377       |
    +-----+-------------------+------------------------------------------+
    | 79  | SCL_0             | Connected to the SCL of the EEPROM       |
    +-----+-------------------+------------------------------------------+
    | 80  | SDA_0             | Connected to the SDA of the EEPROM       |
    +-----+-------------------+------------------------------------------+
    | 81  | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 82  | SPI_CLK           | Connected to SCLK of the ADF4377         |
    +-----+-------------------+------------------------------------------+
    | 83  | SPI_MISO          | Connected to SDO of the ADF4377          |
    +-----+-------------------+------------------------------------------+
    | 84  | SPI_MOSI          | Connected to SDIO of the ADF4377         |
    +-----+-------------------+------------------------------------------+
    | 85  | SPI_SEL_A_N       | Connected to CSB of the ADF4377          |
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
    | 1   | CSB               | Connected to CSB of the ADF4377          |
    +-----+-------------------+------------------------------------------+
    | 2   | NC                | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 3   | SDIO              | Connected to SDIO of the ADF4377         |
    +-----+-------------------+------------------------------------------+
    | 4   | NC                | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 5   | SDO               | Connected to SDO of the ADF4377          |
    +-----+-------------------+------------------------------------------+
    | 6   | NC                | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 7   | SCLK              | Connected to SCLK of the ADF4377         |
    +-----+-------------------+------------------------------------------+
    | 8   | CSB                | Connected to CSB of the ADF4377         |
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
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adf4377_sdz/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adf4377_sdz/src/platform>`_

Basic example
^^^^^^^^^^^^^

This example initializes the ADF4377 or ADF4378 with the configurations 
provided in the  above mentioned common files and applies them to the IC. 
By default reference frequency is provided by the local oscillator of the 
board, the differential output path is powered up and the RF Output power 
level is set to 420mv. Subsequently, the example sets a test frequency of 10 GHz.


In order to build the basic example make sure you are using this command:

.. code-block:: bash

	make EXAMPLE=basic

IIO example
^^^^^^^^^^^

This project provide an IIOD demo for the ADF4377 and ADF4378. It launches an 
IIOD server on the mother platform it runs on allowing the user to connect 
via an IIOD client. Using IIO-Oscilloscope, the user can configure the ADF4377 
or ADF4378. 

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

The No-OS IIO Application together with the No-OS IIO ADF4377 driver take care
of all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adf4377_sdz/src/examples/iio_example>`_

In order to build the IIO project make sure you are using this command:

.. code-block:: bash

	make EXAMPLE=iio_example

No-OS Supported Platforms
-------------------------

Mbed Platform
^^^^^^^^^^^^^

**Used hardware**

* `EVAL-ADF4377 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-adf4377.html>`_
* `SDP-K1 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/sdp-k1.html>`_

**Connections**:

The SDP connector of the SDP-K1 needs to be connected to SDP header of the 
Evaluation board.
Additionally a 6V power supply needs to be connected to either J12 
(the SMA interface).

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset PLATFORM=mbed
	# to build the basic project
	make EXAMPLE=basic PLATFORM=mbed
	# to build the IIO project
	make EXAMPLE=iio_example PLATFORM=mbed
	# copy the adf4377_sdz.bin to the mounted SDP-K1
	cp build/adf5611.bin </path/to/SDP-K1/mounted/folder>

Xilinx platform
^^^^^^^^^^^^^^^

**Used hardware**

* `EVAL-ADF4377 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-adf4377.html>`_
* `Zedboard <https://www.analog.com/en/resources/reference-designs/powering-zynq-evaluation-development-board-zedboard.html>`_


**Build Command**

.. code-block:: bash

        cp <SOME_PATH>/system_top.xsa .
        # to delete current build
        make reset PLATFORM=xilinx
		# to build the basic project
		make EXAMPLE=basic PLATFORM=xilinx
		# to build the IIO project
		make EXAMPLE=iio_example PLATFORM=xilinx
        # to flash the code
        make run
