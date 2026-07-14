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

No-OS Supported Examples
------------------------

This project is organized around the no-OS variant based build flow.
Selecting a variant at build time (``--variant <name>``) chooses which
application is compiled. The platform ``main()`` is a thin dispatcher that
calls ``example_main()``, provided by the selected example. The
initialization data used in the examples is defined in
`src/common <https://github.com/analogdevicesinc/no-OS/tree/main/projects/cn0531/src/common>`_,
and the platform-specific macros in
`src/platform <https://github.com/analogdevicesinc/no-OS/tree/main/projects/cn0531/src/platform>`_.

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

This example is built by selecting the ``iio`` variant (see the Build
Command below).

IIO Example over Wi-Fi
^^^^^^^^^^^^^^^^^^^^^^^

Same IIO example as above, but the IIOD transport runs over an ESP8266
Wi-Fi module (UART AT commands) instead of the serial backend. The board
connects to the configured access point and the IIO client connects to
the reported IP address.

Set the network credentials in the ``iio_wifi`` defconfig
`iio_wifi.conf <https://github.com/analogdevicesinc/no-OS/tree/main/projects/cn0531/iio_wifi.conf>`_
via ``CONFIG_WIFI_SSID`` and ``CONFIG_WIFI_PWD`` before building, for example:

.. code-block:: bash

	CONFIG_WIFI_SSID="MyNetwork"
	CONFIG_WIFI_PWD="MyPassword"

This example is built by selecting the ``iio_wifi`` variant (see the Build
Command below). It is available on the ADuCM/eval-adicup3029 target only.

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

For toolchain setup and prerequisites, see the
`ADuCM3029 CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_aducm3029_cmake.html>`__.

Available variants: ``iio``, ``iio_wifi``.
Available boards: ``eval-adicup3029``.
Replace ``--variant`` / ``--board`` accordingly.

.. code-block:: bash

	# point at the CrossCore Embedded Studio install (only if not auto-detected)
	export CCES_HOME=/opt/analog/cces/3.0.3
	# Windows (PowerShell): $env:CCES_HOME = "C:\analog\cces\3.0.3"

	cd no-OS

	# build the iio example on the EVAL-ADICUP3029
	python tools/scripts/no_os_build.py build \
		--project cn0531 --variant iio --board eval-adicup3029

	# build the iio example over Wi-Fi (ESP8266)
	python tools/scripts/no_os_build.py build \
		--project cn0531 --variant iio_wifi --board eval-adicup3029

	# build and flash (requires a connected debug probe)
	python tools/scripts/no_os_build.py build \
		--project cn0531 --variant iio --board eval-adicup3029 \
		--probe openocd --flash
