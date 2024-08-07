Evaluating AD400x on ZED and SDP-K1
====================================

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
Basic example
^^^^^^^^^^^^^

This example prints sample data out to the uart.

Here is an example on how see the sample data of the basic example using zedboard:

| minicom -b 115200 -D /dev/ttyACM1 -C ~/serial.dat
| make run
| cat ~/serial.dat | grep  ADC | cut -d ' ' -f 2 > ~/plot.dat
| echo "set terminal jpeg; set output '~/o.jpeg';plot '~/plot.dat' with lines" | gnuplot

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
ZEDBOARD Platform
^^^^^^^^^^^^^^^^^

**Used hardware:**

* `EVAL-AD400x <https://www.analog.com/eval-ad400x-fmcz.html>`_
* `Zedboard <https://www.analog.com/en/resources/reference-designs/powering-zynq-evaluation-development-board-zedboard.html>`_

**Prerequisites**

* Vitis/Vivado software installed
* export PATH=$PATH:"<YOUR_PATH>/Xilinx/Vivado/2023.2/bin"
* export PATH=$PATH:"<YOUR_PATH>/Xilinx/Vitis/2023.2/bin"
* (optional) export ADI_IGNORE_VERSION_CHECK=1
* HDL code (**system_top.xsa**) in the porject folder

**Build and flash**

* make
* make run

STM32 Platform
^^^^^^^^^^^^^^

**Used hardware:**

* `EVAL-AD400x <https://www.analog.com/eval-ad400x-fmcz.html>`_
* `SDP-K1 <https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/sdp-k1.html>`_
* ST debugger

**Prerequisites**

* Vitis/Vivado software installed
* HDL code (**system_top.xsa**) placed in the porject folder
* export STM32CUBEMX=<YOUR_PATH>/STMicroelectronics/STM32Cube/STM32CubeMX/
* export STM32CUBEIDE=<YOUR_PATH>/stm32cubeide_1.14.0/
* sdp-ck1z.ioc file in the porject folder

**Build and flash**

* make
* openocd -f ./build/ad400x-fmcz.elf.openocd -c "program ./build/ad400x-fmcz.elf verify reset exit"

Project Options
----------------
* Use basic example that prints samples to uart (zed-only)s
    | ./Makefile
    |   BASIC_EXAMPLE = n
    |   IIO_EXAMPLE = y

* Specify the 400x part in use:
    | src/common/common_data.c:
    |   struct ad400x_init_param ad400x_init_param = {
    |       .dev_id = **ID_AD4020**,
