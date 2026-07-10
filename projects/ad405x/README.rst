AD405X no-OS Example Project STM32
==================================

.. no-os-doxygen::

Supported Evaluation Boards
---------------------------

* `EVAL-AD4050-ARDZ <https://www.analog.com/EVAL-AD4050-ARDZ>`_
* `EVAL-AD4052-ARDZ <https://www.analog.com/EVAL-AD4052-ARDZ>`_
* `EVAL-AD4060-ARDZ <https://www.analog.com/EVAL-AD4060-ARDZ>`_
* `EVAL-AD4062-ARDZ <https://www.analog.com/EVAL-AD4062-ARDZ>`_

Overview
--------

Applications
------------

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

Select the target carrier by pointing ``HARDWARE`` at the desired ``.ioc``
file in the project directory (or add your own).

No-OS Supported Examples
------------------------

Basic example
^^^^^^^^^^^^^

This example prints sample data out to the uart.

Here is an example on how see the sample data of the basic example:

.. code-block:: bash

   minicom -b 115200 -D /dev/ttyACM0 -C ./serial.dat
   # (build and flash the basic variant first, see "Build and flash" below)
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

**Build Command**

Available variants: ``basic``, ``basic_i3c``, ``i3c_dma``, ``iio``.
Available boards: ``nucleo-h503rb``, ``nucleo-h563zi``.
Replace ``--variant`` / ``--board`` accordingly. Not every variant is available
on every board; see the combination list with
``python tools/scripts/no_os_build.py list --project ad405x``.

.. code-block:: bash

   # set the path to STM32CubeMX and STM32CubeIDE (only if they are not
   # in a default install location)
   export STM32CUBEMX=</path/to/stm32cubemx>
   export STM32CUBEIDE=</path/to/stm32cubeide>
   # Windows (PowerShell):
   #   $env:STM32CUBEMX = "C:\ST\STM32CubeMX"
   #   $env:STM32CUBEIDE = "C:\ST\STM32CubeIDE"

   cd no-OS

   # build the project (basic example on the NUCLEO-H563ZI board)
   python tools/scripts/no_os_build.py build \
      --project ad405x --variant basic --board nucleo-h563zi

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project ad405x --variant basic --board nucleo-h563zi \
      --probe openocd --flash

Project Options
---------------

The ``basic``, ``basic_i3c``, and ``i3c_dma`` variants are interactive examples
that print samples to UART; ``iio`` is the IIOD example.

The AD405X/AD406X part and instance ID (I3C only) are fixed per variant via the
``AD405X_DEV_TYPE`` / ``AD405X_INSTANCE_ID`` compile definitions set in
``CMakeLists.txt``. Edit those definitions (or add a new variant) to target a
different part:

* DEV_TYPE = AD4050, AD4052, AD4056, AD4058, AD4060, AD4062
* INSTANCE_ID = 0-7 (range)

