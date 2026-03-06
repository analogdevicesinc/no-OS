CN0531 no-OS Example Project
============================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
----------------------------

* `EVAL-CN0531-PMDZ <https://www.analog.com/EVAL-CN0531-PMDZ>`_

Overview
--------

The `EVAL-CN0531-PMDZ <https://www.analog.com/EVAL-CN0531-PMDZ>`_
is a precision DAC module designed for applications
such as medical instrumentation, test and measurement equipment,
industrial control, and scientific research. It features the
`AD5791 <https://www.analog.com/AD5791>`_, a 20-bit DAC
with 1 ppm accuracy and ±1 LSB Integral Non-Linearity (INL). It
provides flexibility with support for multiple voltage references and
power options, allowing use of onboard or external power supplies and
voltage references. The module's compatibility with the SPI Pmod
interface enables versatile and precise analog output applications.

Applications
------------

* Medical instrumentation
* Test and measurement equipment
* Industrial control
* Scientific and aerospace instrumentation

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The EVAL-CN0531-PMDZ module requires a power supply that can be
configured to use either onboard power converters or external power
sources. The onboard converters can generate the necessary voltage
levels from a single external input, which is usually supplied by a
connected development platform. Alternatively, users can choose to
supply power directly through external connections, which can be
beneficial for integration into systems with existing power
infrastructure. The module's voltage range spans from -5V to +5V,
allowing it to be flexible for use in various high-precision
applications like scientific instrumentation and industrial control. The
high power supply rejection ratio and low operating drift provided by
the onboard voltage regulators enhance its stability and precision.

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Data Source Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/cn0531/src>`_

IIO Example
^^^^^^^^^^^^

This project is an IIOD demo for the EVAL-CN0531-PMDZ board. The
IIO example code initializes the AD5791 DAC over SPI and sets up
UART communication for IIO transport. The AD5791 DAC is
controlled via IIO descriptors, and the main function manages the
setup and execution of the IIO application, utilizing IIO buffers
for data transfers with the DAC.

The project launches a IIOD server on the board so that the user
may connect to it via an IIO client. Using IIO Oscilloscope, the
user can configure the DAC and view the output data on a plot.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO Oscilloscope Client, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The No-OS IIO Application together with the AD5791 no-OS driver takes
care of all the back-end logic needed to setup the IIO server.

In order to build the IIO project, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/cn0531/Makefile>`_:

.. code-block:: bash

	# remove comment below to enable IIO
	IIOD=y

No-OS Supported Platforms
-------------------------

ADuCM Platform
^^^^^^^^^^^^^^

**Used Hardware**

* `EVAL-CN0531-PMDZ <https://www.analog.com/EVAL-CN0531-PMDZ>`_
* `EVAL-ADICUP3029 <https://www.analog.com/EVAL-ADICUP3029>`_

**Connections**

Connect the EVAL-CN0531-PMDZ to the EVAL-ADICUP3029 using the P8 connector.

Plug the USB Type A connector into your PC and the micro USB connector
into the EVAL-ADICUP3029.

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset
	# to build the project
	make PLATFORM=aducm3029
	# to flash the code
	make run
	# to debug the code
	make debug
