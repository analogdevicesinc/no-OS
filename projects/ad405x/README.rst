AD405X no-OS Example Project STM32
==================================

.. no-os-doxygen::

Supported Evaluation Boards
---------------------------

`EVAL-AD4050-ARDZ <https://www.analog.com/EVAL-AD4050-ARDZ>`_
`EVAL-AD4052-ARDZ <https://www.analog.com/EVAL-AD4052-ARDZ>`_
`EVAL-AD4060-ARDZ <https://www.analog.com/EVAL-AD4060-ARDZ>`_
`EVAL-AD4062-ARDZ <https://www.analog.com/EVAL-AD4062-ARDZ>`_

Overview
--------

Applications
------------

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

For the ``.ioc``, copy the desired carrier target file from the *carrier* folder
to the project directory.

No-OS Supported Examples
------------------------

Basic example
^^^^^^^^^^^^^

This example prints sample data out to the uart.

Here is an example on how see the sample data of the basic example:

.. code-block:: bash

   minicom -b 115200 -D /dev/ttyACM0 -C ./serial.dat
   make run
   cat ./serial.dat | grep  ADC | cut -d ' ' -f 2 > ./plot.dat
   echo "set terminal svg; set output './o.svg';plot './plot.dat' with lines" | gnuplot

Basic I3C example
^^^^^^^^^^^^^^^^^

This interactive example prints data out to the uart and waits an event, in
this sequence:

* 50 samples.
* Waits for threshold event.

Here is an example on how to run interactively:

.. code-block:: bash

   picocom -b 115200 -y n -p 1 -f n -d 8 --imap lfcrlf /dev/ttyACM0

IIO example
^^^^^^^^^^^

This project is a IIOD demo for AD405X device.
The project launches a IIOD server on the board so that the user may connect
to it via an IIO client.

At a host, use IIO utilities, for example:

.. code-block:: bash

   iio_info -u serial:/dev/ttyACM0,115200,8n1

Using IIO-Oscilloscope, the user can configure the ADC and view the measured data
on a plot.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The no-OS IIO Application together with the No-OS IIO AD405X driver take care of
all the back-end logic needed to set up the IIO server.

The read buffer is used for storing the burst data which shall be retrieved
by any LibIIO client.

No-OS Supported Platforms
-------------------------

STM32 Platform
^^^^^^^^^^^^^^

**Used hardware:**

* `STM32 NUCLEO-H503RB <https://www.st.com/en/evaluation-tools/nucleo-h503rb.html>`__
* `STM32 NUCLEO-H563ZI <https://www.st.com/en/evaluation-tools/nucleo-h563zi.html>`__
* ST debugger

**Prerequisites**

* export STM32CUBEMX=<path/to/stm32cubemx>
* export STM32CUBEIDE=<path/to/stm32cubeide>
* firmware for the target platform (download on stm32cubemx beforehand)

**Build and flash**

* Find the target carrier configuration at the carrier folder (or add your own)
* make run EXAMPLE=basic_i3c_example DEV_TYPE=AD4060 HARDWARE=carrier/nucleo-h563zi.ioc

Project Options
----------------

* | Use basic interactive example that prints samples to uart:
  | EXAMPLE = basic_example, basic_i3c_example, i3c_dma_example

* | Or IIOD example:
  | EXAMPLE = iio_example

* | Specify the AD405X/AD406X part and instance ID (I3C only) in use:
  | DEV_TYPE = AD4050, AD4052, AD4056, AD4058, AD4060, AD4062
  | INSTANCE_ID = 0-7 (range)

* | Specify the carrier in use:
  | HARDWARE = carrier/nucleo-h563zi.ioc, carrier/nucleo-h503rb.ioc

