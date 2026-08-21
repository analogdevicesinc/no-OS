EVAL-AD7191 no-OS Example Project
=================================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-AD7191 <https://www.analog.com/EVAL-AD7191>`_

Overview
--------

The AD7191 is a pin-programmable, low noise, 24-bit sigma-delta ADC intended
for bridge-sensor and low-frequency measurement applications. Unlike most
sigma-delta ADCs it has no SPI register map: the output data rate, PGA gain,
input channel, temperature-sensor selection, clock source and power-down/reset
are all controlled through dedicated GPIO pins. Conversions are read as a
24-bit offset-binary word over SPI, and the DOUT/RDY line doubles as the
data-ready indicator.

This project drives an EVAL-AD7191 from a Raspberry Pi 5 running Linux
userspace. SPI transfers use the spidev character device, all control pins use
the RP1 GPIO character device, and the DOUT/RDY line is polled through a second
GPIO (no interrupt is required).

Applications
------------

* Weigh scales
* Strain gauges and load cells
* Pressure measurement
* Temperature measurement

Hardware Setup
--------------

The values below match the defaults in
``src/platform/linux/parameters.h``. Adjust the macros there to match your own
wiring before building.

SPI
~~~

The AD7191 is accessed through ``/dev/spidev0.0`` (SPI0, chip select 0) at
1 MHz in SPI mode 3 (clock idle high, data sampled on the falling edge). The
DOUT/RDY output is the SPI MISO line.

GPIO
~~~~

All control pins live on the RP1 GPIO bank, exposed as ``/dev/gpiochip4`` on
the Raspberry Pi 5. The numbers are BCM offsets on the 40-pin header.

============  ============  =====================================
AD7191 Pin    BCM GPIO      Function
============  ============  =====================================
PGA1          5             PGA gain LSB
PGA2          6             PGA gain MSB
ODR1          13            Output data rate LSB
TEMP          16            Temperature sensor select
ODR2          19            Output data rate MSB
CLKSEL        20            Clock source select
PDOWN         21            Power-down / reset (also SPI CS)
RDY           25            Data ready (DOUT/RDY, polled)
CHAN          26            Input channel select
============  ============  =====================================

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-ad7191/src/common>`_

The macros used in Common Data are defined in the platform-specific files:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-ad7191/src/platform>`_

Basic example
~~~~~~~~~~~~~

The basic example initializes the AD7191, then continuously cycles through the
two differential input pairs (AIN1/AIN2 and AIN3/AIN4) and the internal
temperature sensor. For each channel it performs a single conversion
(configuring the pins, powering up, discarding the settling conversion and
reading the result) and prints the raw code together with the converted value
in microvolts or millidegrees Celsius.

This example is built by selecting the ``basic`` variant.

IIO example
~~~~~~~~~~~

The IIO example launches an iiod server on the Raspberry Pi 5, served over TCP,
so that an IIO client can connect and read the two differential voltage
channels and the temperature channel, and configure the sample rate and gain.

If you are not familiar with the ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with the ADI IIO-Oscilloscope client, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

This example is built by selecting the ``iio`` variant.

No-OS Supported Platforms
-------------------------

Linux Platform
~~~~~~~~~~~~~~

**Used hardware**

* `EVAL-AD7191 <https://www.analog.com/EVAL-AD7191>`_
* Raspberry Pi 5

**Connections**

Wire the EVAL-AD7191 to the Raspberry Pi 5 as described in the Hardware Setup
section above. Enable the SPI0 interface and the GPIO character device on the
Raspberry Pi (``raspi-config`` / device tree).

**Build Command**

The project is built natively on the Raspberry Pi 5 with the ``rpi5`` preset.

Available variants: ``basic``, ``iio``.

.. code-block:: bash

   cd no-OS

   # build the basic example
   python3 tools/scripts/no_os_build.py build \
      --project eval-ad7191 --variant basic --board rpi5

   # build the IIO example
   python3 tools/scripts/no_os_build.py build \
      --project eval-ad7191 --variant iio --board rpi5

The equivalent direct CMake invocation is:

.. code-block:: bash

   cmake --preset rpi5 -B build/eval-ad7191 \
      -DPROJECT_DEFCONFIG=eval-ad7191/basic.conf
   cmake --build build/eval-ad7191

The resulting ``eval-ad7191`` binary is placed under the build directory and is
run directly on the Raspberry Pi.
