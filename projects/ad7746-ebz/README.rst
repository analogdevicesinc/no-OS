AD7746-EBZ no-OS Example Project
================================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
----------------------------

* `EVAL-AD7746EBZ <https://www.analog.com/EVAL-AD7746EBZ>`_

Overview
--------

The `AD7746 <https://www.analog.com/AD7746>`_ is a high resolution,
sigma-delta capacitance-to-digital converter (CDC). The capacitance input
range is +/-4 pF (measured with respect to on-chip digital-to-capacitance
DACs), with a common-mode capacitance up to 17 pF. The AD7746 also
provides an on-chip temperature sensor with +/-2 degrees C accuracy and
0.1 degrees C resolution. Communication with the device is handled via a
2-wire I2C-compatible serial interface.

The EVAL-AD7746EBZ evaluation board provides a platform for evaluating
the AD7746 capacitance-to-digital converter. It features SMB connectors
for all capacitive input and excitation signals, and supports both
single-ended and differential capacitance measurements.

Applications
------------

* Pressure measurement
* Position sensing
* Level sensing
* Flowmeters
* Humidity sensing
* Impurity detection

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The EVAL-AD7746EBZ evaluation board operates on a 5V power supply through
USB connector (J1), utilizing an on-board 3.3V regulator (ADP3303) to
deliver the regulated voltage needed for the AD7746. The board also
supports alternative power supply options of either 3V or 5V via an
external input (J2). The AVDD voltage is selected through jumper LK1.

Board Connector and Jumper Settings
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**On-board Connectors**

	Please see the following table for the connector descriptions:

	======================  ==========================================================
	Connector               Description
	======================  ==========================================================
	USB (J1)                Provides 5 V power supply and data communication interface
	External Power (J2)     Alternative power supply option of 3 V or 5 V
	CIN1+, CIN1- (SMB)      Capacitive input connectors for measurement
	EXCA, EXCB (SMB)        Excitation output connectors for capacitive sensing
	LK1                     Jumper to select AVDD voltage (3.3 V or 5 V)
	LK2                     I2C header (SDA, SCL, RDY) for external controller access
	======================  ==========================================================

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad7746-ebz/src/app>`_

Capacitive Length Demo
^^^^^^^^^^^^^^^^^^^^^^

This is the default example that:

* Initializes the AD7746 capacitance-to-digital converter
* Calibrates the capacitive input using a two-point calibration (0 mm and 51 mm)
* Continuously reads and displays the ruler position in mm and internal temperature in degrees C
* Outputs measurements every 2 seconds over UART

IIO Example
^^^^^^^^^^^^

This example initializes an IIOD server on the AD7746 evaluation board,
enabling it to connect with IIO clients. The IIO application initiates
the AD7746 sensor for capacitive and temperature measurements, including
enabling capacitance channels and setting temperature read modes. Users
can connect using an IIO client such as the IIO Oscilloscope to manage
device settings, including offset, sampling frequency, and raw data
conversion. Communication is handled via a UART interface.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO Oscilloscope Client, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

To build the IIO example, add the following flag when invoking make:

.. code-block:: bash

	# to build the IIO example
	make IIOD=y

No-OS Supported Platforms
-------------------------

ADuCM3029 Platform
^^^^^^^^^^^^^^^^^^

**Used Hardware**

* `EVAL-AD7746EBZ <https://www.analog.com/EVAL-AD7746EBZ>`_
* `EVAL-ADICUP3029 <https://www.analog.com/EVAL-ADICUP3029>`_

**Connections**

Before connecting the EVAL-AD7746EBZ to the EVAL-ADICUP3029, remove the
links on LK2 to disconnect the on-board USB microcontroller from the I2C
bus.

+---------------------+---------------------+-----------------------------+
| EVAL-AD7746EBZ Pin  | EVAL-ADICUP3029 Pin | Function                    |
+=====================+=====================+=============================+
| VDD (3.3V)          | 3.3V                | Power supply                |
+---------------------+---------------------+-----------------------------+
| GND                 | GND                 | Ground                      |
+---------------------+---------------------+-----------------------------+
| SDA (LK2)           | P0.14               | I2C data line               |
+---------------------+---------------------+-----------------------------+
| SCL (LK2)           | P0.15               | I2C clock line              |
+---------------------+---------------------+-----------------------------+

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset PLATFORM=aducm3029
	# to build the project
	make PLATFORM=aducm3029
	# to flash the code
	make run PLATFORM=aducm3029
