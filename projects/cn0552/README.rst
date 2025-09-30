CN0552 no-OS Example Project
============================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-CN0552-PMDZ <https://www.analog.com/CN0552>`_

Overview
--------

The `EVAL-CN0552-PMDZ <https://www.analog.com/CN0552>`_ is a PMOD form-factor
evaluation board designed for the
`AD7746 <https://www.analog.com/AD7746>`_ high resolution, sigma-delta
capacitance-to-digital converter (CDC). The AD7746 provides 24-bit resolution
with up to 21-bit effective resolution and is factory calibrated to +-4 fF
accuracy with +-0.01% linearity. It supports a capacitance measurement range
of +-4.096 pF (up to +-50 pF in extended mode) and includes an on-chip
temperature sensor with 0.1 degrees C resolution. The board communicates via
a standard I2C PMOD interface.

Applications
------------

* Capacitive sensor interfacing
* Environmental monitoring
* Liquid level monitoring
* Pressure measurement
* Position sensing
* Flowmeters
* Humidity sensing

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-CN0552-PMDZ evaluation board operates with an I/O voltage between
2.7V and 5.5V, supplied through the standard I2C PMOD connector. This range
supports integration with various platforms, including Xilinx and STM32.

On-board Connectors
~~~~~~~~~~~~~~~~~~~

All the analog and digital input/output pins available on the
EVAL-CN0552-PMDZ are brought out to two separate 8-row 0.1" through-hole
connectors P8 and P13.

============ ====== ============= ======
Connector P8 Pin(s) P13           Pin(s)
============ ====== ============= ======
EXCA         1      GND           1
EXCB         2      VDD           2
EXC_EXTENDED 3      GND           3
REFIN_POS    4      VIN_NEG       4
REFIN_NEG    5      VIN_POS       5
GND          6      GND           6
CIN1_NEG     7      CIN2_NEG      7
CIN1_POS     8      CIN2_POS      8
============ ====== ============= ======

Digital Connections
~~~~~~~~~~~~~~~~~~~

The digital communication on the EVAL-CN0552-PMDZ is accomplished using
a standard I2C PMOD port.

**Connector P2**

============ ======
Description  Pin(s)
============ ======
INT          6
RST          5
SCL          4
SDA          3
GND          2
IOVDD        1
============ ======

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Data Source Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/cn0552/src/app>`_

IIO Example
~~~~~~~~~~~~

This is a simple example that initializes the AD7746 capacitive-to-digital
converter over I2C and starts an IIO application, allowing the device to be
controlled and monitored via UART using an IIO client such as
iio-oscilloscope.

In order to build the IIO example, use the following configuration:

.. code-block:: bash

	# IIOD is enabled by default in src.mk
	make

No-OS Supported Platforms
-------------------------

ADuCM Platform
~~~~~~~~~~~~~~

Hardware Used
^^^^^^^^^^^^^

* `EVAL-ADICUP3029 <https://www.analog.com/EVAL-ADICUP3029>`_
* `EVAL-CN0552-PMDZ <https://www.analog.com/CN0552>`_

Connections
^^^^^^^^^^^

* Attach the EVAL-CN0552-PMDZ to the female header connector **P9** of
  the EVAL-ADICUP3029.

* Connect a micro-USB cable to the **P10** port on the EVAL-ADICUP3029
  and then to your PC.

* The EVAL-CN0552-PMDZ is powered through the PMOD connector when
  attached to the ADICUP3029.

**Sensor and Capacitor Setup**

* For demo use, place a **2 pF capacitor** between the EXCA pin and
  CIN1(+) pin on the CN0552-PMDZ for the first channel.

* Place another **2 pF capacitor** between the EXCB pin and CIN2(+) pin
  for the second channel.

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

	# remove previous build artifacts
	make clean
	# build the project for ADuCM3029
	make aducm3029
	# flash the firmware onto the device
	make flash
