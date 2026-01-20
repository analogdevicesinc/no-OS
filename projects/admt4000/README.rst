Evaluating the ADMT4000
=======================


Contents
--------

.. contents:: Table of Contents
	:depth: 3

Supported Evaluation Boards
---------------------------

* EVAL-ADMT4000ARD1Z
* EVAL-ADMT4000ARD2Z

Overview
--------

The ADMT4000 is a magnetic turn, counter sensor capable of recording turns of an 
external magnetic field with zero power. The absolute position, including the 
number of turns, is reported via a serial peripheral interface (SPI). 

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

For the EVAL-ADMT4000ARD1Z, the power supply is provided by the SDP-K1 via the 5V
from the USB-C connector. At least 9V DC power supply is also required via the 
EVAL-ADMT4000ARD1Z's DC barrel jack to power the motor assembly. 

For the EVAL-ADMT4000ARD2Z, since there is no motor in this kit, only the 5V power
supply from the USB-C connector is required.

**Pin Description**:

**P1 Header**

	==============  ==============  ========================================
	Pin             Name            Description
	==============  ==============  ========================================
	A0              GPIO5           ADMT4000 GPIO5/BOOTLOAD pin
	A1              SHDN_N          Power control pin for Coil Driver
	A2              RSTB            ADMT4000 Reset pin
	A3              V_EN            ADMT4000 Supply Enable pin
	==============  ==============  ========================================

**P2 Header**

	==============  ==============  ========================================
	Pin             Name            Description
	==============  ==============  ========================================
	+3.3V	   	3V3             Board 3.3V Supply
	+5V             5V            	Board 5V Supply
	GND             GND             Board GND
	GND             GND             Board GND
	VIN				Not in Used
	==============  ==============  ========================================

**P4 Header**

	==============  ==============  ========================================
	Pin             Name            Description
	==============  ==============  ========================================
	D8              GPIO4           ADMT4000 GPIO4/FAULT pin
	D9              DIAG1           TMC5240 Diag1 pin (EVAL-ADMT4000ARD1Z)
	D10             CS_MT           Chip Select for ADMT4000
	D11             MOSI            SPI MOSI
	D12             MISO            SPI MISO
	D13             SCK             SPI SCK
	GND             GND             Board GND
	SDA             I2C SDA         EEPROM I2C SDA
	SCL             I2C SCL         EEPROM I2C SCL
	==============  ==============  ========================================

**P5 Header**

	==============  ==============  ========================================
	Pin             Name            Description
	==============  ==============  ========================================
	D2              GPIO2           ADMT4000 GPIO2 pin
	D3              GPIO0           ADMT4000 GPIO0/Busy pin
	D4              COIL_RS         Coil drive pin for magnetic reset
	D5              GPIO1           ADMT4000 GPIO1/CNV pin
	D6              GPIO3           ADMT4000 GPIO3/ACALC pin
	D7              CS_MOTOR        CS for TMC5240 (EVAL-ADMT4000ARD1Z)
	==============  ==============  ========================================

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/admt4000/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/admt4000/src/platform>`_

Basic example
^^^^^^^^^^^^^

The basic example demonstrates the basic functionalities of the ADMT4000, and
performs capture of turns, angles, temperature, sine, cosine, radius, GPIO states,
and fault from the ADMT4000 in a while loop with a minimum interval of 500 ms.
Data is printed in the UART console. Thus, tools like tera-term, putty, and other 
terminals can be used to view the data by setting the UART baud rate to 230400.

To build the basic example:

.. code-block:: bash

        # to delete current build
        make reset
        # to build the project
        make PLATFORM=stm32 EXAMPLE=basic_example
        # to flash the code to the controller
        make PLATFORM=stm32 EXAMPLE=basic_example run

IIO example
^^^^^^^^^^^

The IIO device driver contains the IIO specific implementations for the ADMT4000.
This example project runs an IIOD server on the board, which can be accessed by 
users from host computers via IIO clients through libiio. IIO utilities such as
iio_info, iio_attr, and iio_readdev can be used to interact with the IIO device.
IIO Oscilloscope and Scopy 2.0 can also be used for simple data visualization.

To learn more about ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

To learn more about ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The no-OS IIO driver for the ADMT4000 and the IIO application handles all the
back-end logic needed to setup the IIOD server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Trigger Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/admt4000/src/examples/iio_trigger_example>`_

This IIO project supports exposes the ADMT4000 as an IIO device. Details of the
IIO implementation is documented in IIO section of the ADMT4000 driver:
`ADMT4000 Drivers <https://github.com/analogdevicesinc/no-OS/tree/main/drivers/position/admt4000/README.rst>`_

This IIO project also supports the TMC5240 as an IIO device if TMC_DEVICE is
defined as TMC5240 in the Makefile. EVAL-ADMT4000ARD1Z has an onboard TMC5240. 
Details of the IIO implementation for the TMC5240 is documented in the IIO TMC5240 driver:
`TMC5240 Drivers <https://github.com/analogdevicesinc/no-OS/tree/main/drivers/motor/tmc5240/README.rst>`_

In order to build the IIO project:

.. code-block:: bash

        # to delete current build
        make reset
        # to build the project
        make PLATFORM=stm32 EXAMPLE=iio_trigger_example TMC_DEVICE=TMC5240

No-OS Supported Platforms
-------------------------

STM32 Platform
^^^^^^^^^^^^^^

**Used hardware**

* `SDP-K1 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/sdp-k1.html>`_

.. code-block:: bash

        make PLATFORM=stm32 <other build parameters>
