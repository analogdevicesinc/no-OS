EVAL-ADXL367Z no-OS Example Project
====================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* `EVAL-ADXL367Z <https://www.analog.com/EVAL-ADXL367Z>`_

Overview
--------

The `ADXL367 <https://www.analog.com/ADXL367>`_ is an ultralow power, 3-axis
MEMS accelerometer that consumes only 0.89 uA at a 100 Hz output data rate
and 180 nA in motion-triggered wake-up mode. Unlike accelerometers that use
power duty cycling to achieve low power consumption, the ADXL367 does not
alias input signals by undersampling, but samples the full bandwidth of the
sensor at all data rates.

The device provides 14-bit output resolution with selectable measurement
ranges of +/-2 g, +/-4 g, and +/-8 g (0.25 mg/LSB on the +/-2 g range).
It features a deep 512-sample FIFO, built-in temperature sensor, single-tap
and double-tap detection, and supports both SPI (4-wire) and I2C digital
interfaces. The ADXL367 operates on a 1.1 V to 3.6 V supply range.

The EVAL-ADXL367Z evaluation board provides a simple platform for evaluating
the ADXL367 accelerometer.

Applications
------------

* 24/7 sensing
* Hearing aids
* Vital signs monitoring devices
* Motion-enabled power save switches
* Motion-enabled metering devices
* Smart watch with single-cell operation
* Smart home

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The ADXL367 operates on a supply voltage range of 1.1 V to 3.6 V (VDD),
with a separate digital interface supply (VDDIO). Current consumption is
0.89 uA at 100 Hz ODR (2.0 V supply), 180 nA in motion-activated wake-up
mode, and 40 nA in standby.

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adxl367z/src/common>`__.

The macros used in Common Data are defined in platform specific files
found in the
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adxl367z/src/platform>`__.

Dummy example
~~~~~~~~~~~~~

The dummy example initializes the ADXL367 accelerometer, performs a
self-test, enables temperature reading, sets the output data rate to
200 Hz, and configures the FIFO for data collection. The device is then
switched to measurement mode, and acceleration data along the x, y, and
z axes along with temperature data is read from the FIFO buffer and
printed to the console.

In order to build the dummy example, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/eval-adxl367z/Makefile>`__:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   DUMMY_EXAMPLE = y
   IIO_EXAMPLE = n

IIO example
~~~~~~~~~~~

The IIO example initializes the ADXL367 accelerometer and sets up an
IIO device server that allows users to interact with the sensor via an
IIO client such as IIO Oscilloscope.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with ADI IIO Oscilloscope Client, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

In order to build the IIO example, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/eval-adxl367z/Makefile>`__:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   DUMMY_EXAMPLE = n
   IIO_EXAMPLE = y

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-ADXL367Z <https://www.analog.com/EVAL-ADXL367Z>`_
* `ZedBoard <https://www.xilinx.com/products/boards-and-kits/1-8dyf-11.html>`_

Connections
^^^^^^^^^^^

Connect the EVAL-ADXL367Z to the ZedBoard via the SPI interface. The
ADXL367 communicates over SPI using device ID 0 as configured in
``parameters.h``.

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # copy the Xilinx hardware description file
   cp <SOME_PATH>/system_top.xsa .
   # to delete current build
   make reset
   # to build the project
   make
   # to flash the code
   make run
