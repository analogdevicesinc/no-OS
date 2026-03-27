IIO Demo FreeRTOS no-OS Example Project
=======================================

.. no-os-doxygen:: projects/iio_demo_freertos

.. contents:: Table of Contents
    :depth: 3

Overview
--------

The IIO Demo FreeRTOS project demonstrates running an IIO (Industrial
I/O) server on the FreeRTOS real-time operating system using no-OS
drivers. The application creates two FreeRTOS tasks: an IIOD server task
that exposes demo ADC and DAC devices to IIO clients, and an LED
blinking task that demonstrates multithreading capability.

The demo ADC and DAC use loopback buffers, allowing data written to the
DAC to be read back from the ADC. This project serves as a reference
for integrating IIO applications with FreeRTOS on Maxim microcontrollers.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with ADI IIO Oscilloscope Client, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/iio_demo_freeRTOS/src/common>`__.

The macros used in Common Data are defined in platform specific files
found in the
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/iio_demo_freeRTOS/src/platform>`__.

IIO example
~~~~~~~~~~~

This is the default and only example for this project. It initializes
demo ADC and DAC device descriptors, configures the IIO application, and
runs the IIOD server in a FreeRTOS task. A second task blinks an LED to
demonstrate concurrent execution.

In order to build the IIO example, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/iio_demo_freeRTOS/Makefile>`__:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   IIO_EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim
~~~~~

Used Hardware
^^^^^^^^^^^^^

* Any Maxim microcontroller evaluation kit with FreeRTOS support
  (e.g., MAX32690EVKIT, MAX32655EVKIT)

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project
   make PLATFORM=maxim TARGET=max32690
   # to flash the code
   make run
