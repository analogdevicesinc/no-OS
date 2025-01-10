Evaluating the ADF5611
======================

Contents
--------

.. contents:: Table of Contents
	:depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-ADF5611 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-adf5611.html>`_

Overview
--------

The EV-ADF5611SD1Z evaluates the performance of the ADF5611 fractional
frequency synthesizer with an integrated voltage controlled oscillator
(VCO) for phase-locked loops (PLLs). The EVADF5611SD1Z contains the ADF5611
frequency synthesizer with an integrated VCO, a USB interface, power supply 
connectors, on-board reference oscillator, propagation delay calibration paths,
and Subminiature Version A (SMA) connectors. The output is AC-coupled with 
50 Ω transmission lines making the outputs suitable to drive 50 Ω impedance 
instruments. 

The above also applies to the ADF5612 frequency synthesizers.

Full specifications on the ADF5611 and ADF5612 frequency synthesizer are 
available in the ADF5611 and ADF5612 data sheet available from 
Analog Devices Inc. and must be consulted with their user guide when using 
the EV-ADF5611SD1Z and ADF5612SD1Z evaluation boards.


Applications
------------

* Military and Defense
* Test equipment
* Clock generation
* Wireless infrastructure
* Satellite and very small aperture terminal (VSAT)
* Microwave radio

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The EV-ADF5612D1Z and ADF5612SD1Z are both powered by a 6 V power supply 
connected to the J4 SMA connector.

Reference Input Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The EV-ADF5611SD1Z and ADF5612SD1Z has an on-board, 122.88 MHz, ultra low phase noise, 
sine-wave oscillator to drive the ADF5611 and ADF5612 reference input respectively. 
Single-ended oscillator output is connected to the REFP pin, and the REFN pin is AC 
grounded. This oscillator is used by default on the board. 

The reference input can also be driven externally via a pair of SMA connectors,
REFIN (J5). When using the external reference, disconnect the on-board oscillator path. 
This will require board modifications, please refer to the `EVAL-ADF5611 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-adf5611.html>`_ or `EVAL-ADF5612 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-adf5612.html>`_ 
for details.

Pin Description
^^^^^^^^^^^^^^^

Please see the following table for the pin assignments for the Arduino interface
connectors (P2 and P4).

	P1: ANALOG IN

    +-----+-------------------+------------------------------------------+
    | Pin | Name              | Description                              |
    +-----+-------------------+------------------------------------------+
    | 1   | A0                | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 2   | A1                | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 3   | A2                | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 4   | A3                | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 5   | A4                | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 6   | A5                | Not connected                            |
    +-----+-------------------+------------------------------------------+
    
    P2: POWER
    +-----+-------------------+------------------------------------------+
    | 1   | NC                | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 2   | IOREF             | Connected to VIO                         |
    +-----+-------------------+------------------------------------------+
    | 3   | RESET             | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 4   | +3V3              | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 5   | +5V               | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 6   | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 7   | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 8   | VIN               | Not connected                            |
    +-----+-------------------+------------------------------------------+
    
    P4: DIGITAL 1
    +-----+-------------------+------------------------------------------+
    | 1   | 8                 | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 2   | 9/PWM             | Connected to CEN                         |
    +-----+-------------------+------------------------------------------+
    | 3   | 10/PWM/CS         | Connected to CSB                         |
    +-----+-------------------+------------------------------------------+
    | 4   | 11/PWM/MOSI       | Connected to STD_SPI_MOSI                |
    +-----+-------------------+------------------------------------------+
    | 5   | 12/MISO           | Connected to STD_SPI_MISO                |
    +-----+-------------------+------------------------------------------+
    | 6   | 13/SCK            | Connected to STD_SPI_CLK                 |
    +-----+-------------------+------------------------------------------+
    | 7   | GND               | Connected to Ground                      |
    +-----+-------------------+------------------------------------------+
    | 8   | AREF              | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 9   | SDA               | Connected to EEPROM_SDA                  |
    +-----+-------------------+------------------------------------------+
    | 10  | SCL               | Connected to EEPROM_SCL                  |
    +-----+-------------------+------------------------------------------+
    
    P5: DIGITAL 0
    +-----+-------------------+------------------------------------------+
    | 1   | RX + 0            | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 2   | TX + 1            | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 3   | 2                 | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 4   | 3/PWM             | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 5   | 4                 | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 6   | 5/PWM             | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 7   | 6/PWM             | Not connected                            |
    +-----+-------------------+------------------------------------------+
    | 8   | 7                 | Not connected                            |
    +-----+-------------------+------------------------------------------+

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adf5611/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adf5611/src/platform>`_

Basic example
^^^^^^^^^^^^^

This example initializes the ADF5611 or ADF5612 with the configurations 
provided in the  above mentioned common files and applies them to the IC. 
By default reference frequency is provided by the local oscillator of the 
board, the differential output path is powered down and the RF Output power 
level is set to 3. Subsequently the example sets a test frequency of 12 GHz.


In order to build the basic example make sure you are using this command:

.. code-block:: bash

	make EXAMPLE=basic

IIO example
^^^^^^^^^^^

This project provide an IIOD demo for the ADF5611 and ADF5612. It launches an 
IIOD server on the mother platform it runs on allowing the user to connect 
via an IIOD client. Using IIO-Oscilloscope, the user can configure the ADF5611 
or ADF5612. 

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

The No-OS IIO Application together with the No-OS IIO ADF5611 driver take care
of all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adf5611/src/examples/iio_example>`_

In order to build the IIO project make sure you are using this command:

.. code-block:: bash

	make EXAMPLE=iio_example

No-OS Supported Platforms
-------------------------

Mbed Platform
^^^^^^^^^^^^^

**Used hardware**

* `EVAL-ADF5611 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-adf5611.html>`_ or `EVAL-ADF5612 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-adf5612.html>`_
* `SDP-K1 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/sdp-k1.html>`_

**Connections**:

The Arduino of the SDP-K1 needs to be connected to Arduino header of the 
Evaluation board.
Additionally a 6V power supply needs to be connected to either J4 
(the SMA interface).

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset
	# to build the basic project
	make EXAMPLE=basic
	# to build the IIO project
	make EXAMPLE=iio_example
	# copy the adf5611.bin to the mounted SDP-K1
	cp build/adf5611.bin </path/to/SDP-K1/mounted/folder>
