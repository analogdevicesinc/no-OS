Evaluating AD400x on SDP-K1
===========================

Contents
--------

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `AD400x <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/EVAL-AD400x-FMCZ.html>`_

Overview
--------

Features and Benefits

Product Details
The AD4000/AD4001/AD4002/AD4003/AD4020 family of evaluation boards enable quick,
simplified evaluation of the AD4000 family of 16-/18-/20-bit, precision
successive approx-imation register (SAR) analog-to-digital converters (ADCs).

The AD4000/AD4001/AD4002/AD4003/AD4020 are low power, 16-bit/18-bit/20-bit,
precision SAR ADCs that offer high performance with throughputs up to 2 MSPS
(1.8 MSPS for the AD4020). The evaluation board demonstrates the performance
of the AD4000/AD4001/AD4002/AD4003/AD4020 family of ADCs and provides a
simplified interface for a variety of system applications.

Applications
------------

* Industrial Automation Technology
* Instrumentation and Measurement Solutions
* Intelligent Building Solutions
* Healthcare Solutions
* Aerospace and Defense Systems
* Wireless Communication Solutions

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

IIO example
^^^^^^^^^^^

This project is actually a IIOD demo for EVAL-AD400X device.
The project launches a IIOD server on the board so that the user may connect
to it via an IIO client.
Using IIO-Oscilloscope, the user can configure the ADC and view the measured data
on a plot.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The No-OS IIO Application together with the No-OS IIO AD400X driver take care of
all the back-end logic needed to setup the IIO server.

The read buffer is used for storing the burst data which shall be retrieved
by any LibIIO client.

No-OS Supported Platforms
-------------------------

STM32 Platform
^^^^^^^^^^^^^^

**Used hardware**:
* Arduinio-FMC interposer
* `EVAL-AD400x <https://www.analog.com/eval-ad400x-fmcz.html>`_ with
* `SDP-K1 <https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/sdp-k1.html>`_ and
* ST debugger

interposer jumper settings:
SPI_MOSI = 02P
SPI_MOSO = 01P
SPI_CLK = 00P
SPI_CS = 00N


**Build Command**

.. code-block:: bash

        # to delete current build
        make reset
        # to build the project
        make
        # to flash the code
        make run
        # to debug the code
        make debug
