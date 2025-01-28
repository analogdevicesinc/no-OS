
Evaluating the LTC7871
======================

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `DC2886A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc2886a.html>`_

Overview
--------

Demonstration circuit DC2886A is a high power, high efficiency six-phase bidirectional converter featuring the LTC7871 and LTC7060. 
The terminals labeled VHIGH and VLOW are either inputs or outputs depending on the direction of power flow. When the switch at SW1 
is in the BUCK position, VLOW provides a 14V output and VHIGH is the input with a range of 30V to 70V. When the switch is in 
the BOOST position, VHIGH provides a 48V output and VLOW is the input with a range of 10V to 14V. The maximum VLOW current 
is 180A in both directions. The maximum output power is 2.5kW when operating as a buck within the input voltage range. 
When operating as a boost, the maximum output power is 2.4kW when the input voltage is 14V. External airflow is required 
when operating at maximum power. The MOSFETs in each phase are driven by the LTC7060 half bridge driver. Each LTC7060 gate 
driver is placed next to the MOSFETs and receives a PWM signal from the LTC7871.

The inductor current for each phase is sensed with a low 1mΩ sense resistor using a highly accurate AC/DC current 
sensing architecture with low power dissipation. The LTC7871’s constant-current loop provides a DC current limit for 
the current flowing in or out of the VLOW terminal depending on the state of the BUCK pin. This current can be programmed 
with the SETCUR pin and monitored with the IMON pin.

The DC2886A provides a SPI compatible serial port. With a DC2026C (Linduino® One) demo board and QuikEval™ software 
on a host computer, the user can:

Margin the regulated VLOW or VHIGH up or down.
Margin the DC current limit up or down with the SETCUR pin.
Adjust the frequency spread and modulation rate of the spread spectrum circuit, when spread spectrum is enabled.
Monitor the operational status and faults of the LTC7871.

Additional features of the DC2886A include:

Pin selectable light load operating modes
Buck: FCM, pulse-skipping and Burst Mode® Operation
Boost: FCM and pulse-skipping
SYNC and CLKOUT pins
Optional jumper to enable spread spectrum modulation
RUN, FAULT, PGOOD and PWNEN pins
BUCK pin to externally control the direction of power flow
Footprint for an optional heatsink

Hadrware Specifications
-----------------------

Power Supply Requirments
^^^^^^^^^^^^^^^^^^^^^^^^

DC2886A can be operated as buck or boost converter. Using buck converter, supply is connected to the Vhigh that could 
range from 30V to 70V with a typical value of 48V. During boost mode operation, supply is connected to Vlow that 
can operate form 10V to 14V with a typical value of 12V.

Board Connector and Jumper Settings
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The (JP4) jumper toggles Run pin. Connect to ON for normal oepration.

The (SW1) DPDT has two positions: Boost or Buck mode.

**Pin Description**

Several Pins are exposed on the board

1. Communication Pins(SDA, CSB, SDO, SCL)
2. Fault pin
3. PWMEN pin
4. PGOOD pin
5. RUN pin
6. SYNC adn CLKOUT pin
7. SETCUR pin
8. IMON pin


No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc7871/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc7871/src/platform>`_

Basic example
^^^^^^^^^^^^^

This is a simple example that initializes the LTC7871, reading configuration setup of the board.

In order to build the basic example make sure you are using this command:

.. code-block:: bash

	make EXAMPLE=basic

IIO example
^^^^^^^^^^^

This project is actually an IIOD demo for DC2886A evaluation board.
The project launches a IIOD server on the board so that the user may connect
to it via an IIO client.

Using IIO-Oscilloscope, the user can configure the device.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The No-OS IIO Application together with the No-OS IIO LTC7871 driver take care of
all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc7871/src/examples/iio_example>`_

In order to build the IIO project make sure you you are using this command:

.. code-block:: bash

    make EXAMPLE=iio_example

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**

* `DC2886A evaluation board <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/dc2886a.html>`_
* `MAX32690EVKIT <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/MAX32690EVKIT.html>`_
* `Bi-directional Logic Level Converter - BSS138 <https://www.adafruit.com/product/757>`_

**Connections**:

+---------------------------------+------------------------------+------------------------------+---------------------------+
|  Function                       | MAX32690EVKIT Pin            |        Voltage shifter       |     DC2886A Pin           |
+---------------------------------+------------------------------+------------------------------+---------------------------+
|SPI Clock (SCK)                  | P2.29 (SPI0B_SCK)            |                              |           SCL             |
+---------------------------------+------------------------------+------------------------------+---------------------------+
|SPI Master In Slave Out (MISO)	  | P2.27 (SPI0B_MISO)	         |             A1               |                           |
+                                 +------------------------------+------------------------------+---------------------------+
|                                 |                              |             B1               |           SDO*            |
+---------------------------------+------------------------------+------------------------------+---------------------------+
|SPI Chip Select (CS)             | P2.26 (SPI0B_SS0) 	         |                              |           CSB             |
+---------------------------------+------------------------------+------------------------------+---------------------------+
|SPI Master Out Slave In (MOSI)	  | P2.28 (SPI0B_MOSI)           |                              |           SDA             |
+---------------------------------+------------------------------+------------------------------+---------------------------+
|GPIO (PWMEN Pin)                 | P4.0 (GPIO)       	         |                              |           PWMEN           |
+---------------------------------+------------------------------+------------------------------+---------------------------+
|Ground (GND)                     | GND               	         |             GND              |           GND             |
+---------------------------------+------------------------------+------------------------------+---------------------------+
|3.3V                             | Vaux                         |             LV               |                           |
+---------------------------------+------------------------------+------------------------------+---------------------------+
|5V                               |                              |             HV               |            V5             |
+---------------------------------+------------------------------+------------------------------+---------------------------+

'* - DC2886A SDO's external pull-up resistor should be replaced with 1kohm resistor

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset
	# to build the project for running basic example
	make EXAMPLE=basic TARGET=max32690 MXC_SPI_VERSION=v1
	# to build the project for running iio example
	make EXAMPLE=iio_example TARGET=max32690 MXC_SPI_VERSION=v1
	# to flash the code
	make run