CN0565 no-OS Example Project
============================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-CN0565-ARDZ <https://www.analog.com/EVAL-CN0565-ARDZ>`_

Overview
--------

The EVAL-CN0565-ARDZ is a specialized evaluation board for Electrical
Impedance Tomography (EIT). Designed to be cost-effective and
non-radiative, it utilizes impedance measurements to visualize internal
structures. The board can handle setups with up to 24 electrodes,
supported by a 12-channel crosspoint switch for efficient
multi-electrode operation. Enhanced safety and reliability are
prioritized through its power isolation features. Aligning with Arduino
standards, this board is compatible with various development platforms
and serves well in applications requiring detailed electrical impedance
analysis, particularly beneficial in non-invasive medical imaging.

Applications
------------

* Electrical impedance tomography
* Bioimpedance applications
* Medical imaging

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The EVAL-CN0565-ARDZ board relies on a 5 V supply provided by the host
platform. This supply powers an ADuM5020 isolated DC-to-DC converter,
which generates an isolated 5V output at 500 mW. This isolated output
drives the impedance measurement circuitry. Additionally, the 5V is
regulated to 3.3V by the ADM7150 to supply 120 mA for components such
as the AD5940. The AD8829 inverter converts the isolated 5V to a ±5V
dual supply for other parts of the circuit, ensuring stable and isolated
power vital for accurate impedance measurements.

On-board Connectors
^^^^^^^^^^^^^^^^^^^

**EVAL-CN0565-ARDZ Connectors**

=============== ===========================================================
Connector       Description
=============== ===========================================================
P1              DUT Connections - Common for various device under test
                connections
P7              Commonly used for power and communication interface
                connections
=============== ===========================================================

**EVAL-ADICUP3029 Connectors**

=============== ===========================================================
Connector       Description
=============== ===========================================================
P10             Micro-USB power connection to the Carrier Board
=============== ===========================================================

**EVAL-CN0565-ARDZ Digital Communication Pins**

========== =================== ==========================================
Pin Name   Mnemonic            Function
========== =================== ==========================================
P5-2       IOVDD               Power - regulated supply for sensor
P5-4       3V                  Power Supply, 3V
P5-5       5V                  Power Supply, 5V
P7-2       MOSI                Master Out, Slave In - transmit data
P7-3       MISO                Master In, Slave Out - receive data
P7-4       SCK                 SPI Clock - synchronizes data transfer
P7-6       GND                 Ground
========== =================== ==========================================

**Carrier Board - Digital Communication Pins**

======================= ==========================
Arduino DIO High        ADuCM3029 Pin
Pin Name                Function
======================= ==========================
SCL                     I2C0_SCL / GPIO04
SDA                     I2C0_SDA / GPIO05
AREF                    VREF+
AGND                    Analog Ground
SCLK                    SPI0_CLK / GPIO00
MISO                    SPI0_MISO / GPIO02
MOSI                    SPI0_MOSI / GPIO01
CS                      SPI10_CS1 / GPIO26
RDY                     SPI_RDY / GPIO30
IO28                    GPIO28
======================= ==========================

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Data Source Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/cn0565/src>`__.

IIO Example
^^^^^^^^^^^^

The IIO example code for the EVAL-CN0565-ARDZ board in the no-OS project
interfaces with the AD5940 and ADG2128 components to facilitate EIT.
The code initializes the IIO interfaces for both components,
configures UART parameters, and sets up an array of devices passed
to the IIO application initialization function. It supports various platforms
like STM32 and ADuCM3029, allowing data streaming and interaction
with host systems through the IIO Daemon (IIOD) for real-time data exchange
and device control, crucial for detailed impedance analysis.

This project is actually an IIOD demo for EVAL-CN0565-ARDZ board. The
project launches a IIOD server on the board so that the user may connect
to it via an IIO client. Using IIO Oscilloscope, the user can configure
the ADC and view the measured data on a plot.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__.

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__.

The No-OS IIO Application together with the No-OS IIO AD5940 and ADG2128
drivers take care of all the back-end logic needed to setup the IIO
server.

In order to build the IIO project, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/cn0565/Makefile>`__:

.. code-block:: bash

	IIOD=y

No-OS Supported Platforms
-------------------------

ADuCM Platform
^^^^^^^^^^^^^^

**Used Hardware**

* EVAL-CN0565-ARDZ evaluation board
* EVAL-ADICUP3029 development platform

**Connections**

=============================================== ===============================================
Component                                       Connection Details
=============================================== ===============================================
EVAL-CN0565-ARDZ                                Align with EVAL-ADICUP3029 so connectors match
Arduino Headers                                 Use male and female header pins to securely
                                                attach EVAL-CN0565-ARDZ on top of ADICUP3029
Micro USB Cable                                 Connect to EVAL-ADICUP3029 from USB power
                                                source or PC
UART Switch (S2) on EVAL-ADICUP3029             Set to middle position to enable UART via
                                                Arduino connector
DS2 LED on EVAL-CN0565-ARDZ                     Should turn green to confirm power is received
=============================================== ===============================================

**Build Command**

.. code-block:: bash

	cd no-OS/projects/cn0565

	export PLATFORM=aducm3029
	# to build the project
	make
	# to flash the code
	make run
	# to debug the code
	make debug

STM32 Platform
^^^^^^^^^^^^^^

**Used Hardware**

* EVAL-CN0565-ARDZ evaluation board
* EVAL-SDP-CK1Z (SDP-K1) development platform

**Connections**

=============================================== ===============================================
Component                                       Connection Details
=============================================== ===============================================
EVAL-CN0565-ARDZ                                Connect to SDP-K1 via 120-pin connector
VIO_ADJUST Header (SDP-K1)                      Configure based on CN0565 voltage requirements
                                                (defaults to 1.8V if jumper is absent)
Power Source                                    Use USB mini-B cable or DC jack (7V to 12V,
                                                center positive, >=300 mA)
USB Communication                               Connect SDP-K1 to PC via USB
=============================================== ===============================================

**Build Command**

.. code-block:: bash

	cd no-OS/projects/cn0565

	# to delete current build
	make reset
	# to build the project
	make PLATFORM=stm32
	# to flash the code
	make run
	# to debug the code
	make debug
