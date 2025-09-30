MAX11205PMB1 no-OS Example Project
==================================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
----------------------------

* `MAX11205PMB1 <https://www.analog.com/MAX11205PMB1>`_

Overview
--------

The MAX11205 is an ultra-low-power (< 300 uA max active current),
high-resolution, 16-bit delta-sigma ADC with a simple 2-wire serial
interface. It provides the highest resolution per unit power in the
industry, and is optimized for applications that require very high
dynamic range with low power, such as sensors on a 4 mA to 20 mA
industrial control loop. The device features fully differential signal
and reference inputs, an internal high-accuracy oscillator requiring no
external components, and a digital filter providing more than 80 dB
rejection of 50 Hz or 60 Hz line noise. The MAX11205 operates from a
2.7 V to 3.6 V analog supply with a separate 1.7 V to 3.6 V digital
supply, and is available in a 10-pin uMAX package operating over the
-40°C to +85°C temperature range.

Applications
-------------

* Sensor measurement (temperature and pressure)
* Portable instrumentation
* Battery applications
* Weigh scales

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The MAX11205PMB1 peripheral module board is powered from the host
platform via the 3.3 V supply pin on the PMOD connector. The MAX11205
requires an analog supply voltage (AVDD) between 2.7 V and 3.6 V and
a digital supply voltage (DVDD) between 1.7 V and 3.6 V. The reference
voltage is set to 3.3 V (3300 mV).

Pin Description
~~~~~~~~~~~~~~~~~~~

========== ============ ==========================
Pin Number Signal       Description
========== ============ ==========================
3          RDY/DOUT     Data-ready output/data-out
4          SCLK         Serial clock input
5          GND          Ground
6          VDD          Power supply (3.3 V)
========== ============ ==========================

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
-------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max11205pmb1/src/common>`_

The macros used in Common Data are defined in platform specific files
found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max11205pmb1/src/platform>`_

Basic Example
~~~~~~~~~~~~~

The basic example initializes the MAX11205 ADC by setting up SPI and
IRQ interfaces. A GPIO IRQ controller manages data-ready signals from
the ADC. The program continuously reads ADC data, converts it to
millivolts, and prints the results via UART.

In order to build the basic example, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/max11205pmb1/Makefile>`_:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   BASIC_EXAMPLE = y
   IIO_EXAMPLE = n

IIO Example
~~~~~~~~~~~

The IIO example configures and runs the Industrial I/O (IIO) application
with the MAX11205 ADC. It initializes the IIO application parameters,
including device-specific settings and a data buffer for storing ADC
data, and executes ``iio_app_run`` for data acquisition and processing.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with the ADI IIO Oscilloscope app, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

In order to build the IIO project, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   BASIC_EXAMPLE = n
   IIO_EXAMPLE = y

No-OS Supported Platforms
--------------------------

Maxim Platform
~~~~~~~~~~~~~~

**Used Hardware**

* `MAX11205PMB1 <https://www.analog.com/MAX11205PMB1>`_
* `MAX32655FTHR <https://www.analog.com/MAX32655>`_

**Connections**

The MAX32655FTHR does not have a PMOD interface. Use jumper wires to
make the required connections as shown below.

+-----------------+-----------------+-----------------+
| MAX11205PMB1    | MAX32655FTHR    | Function        |
| Pin Number      | Pin             |                 |
+-----------------+-----------------+-----------------+
| Pin 3 (DOUT)    | MISO            | SPI Data Out    |
+-----------------+-----------------+-----------------+
| Pin 4 (SCLK)    | SCLK            | Serial Clock    |
+-----------------+-----------------+-----------------+
| Pin 5 (GND)     | GND             | Ground          |
+-----------------+-----------------+-----------------+
| Pin 6 (VDD)     | 3.3 V           | Power Supply    |
+-----------------+-----------------+-----------------+
| Pin 3 (DRDY)    | P1_6            | Data Ready IRQ  |
+-----------------+-----------------+-----------------+

**Build Command**

.. code-block:: bash

   # to delete current build
   make reset PLATFORM=maxim TARGET=max32655
   # to build the project
   make PLATFORM=maxim TARGET=max32655
   # to flash the code
   make run PLATFORM=maxim TARGET=max32655
