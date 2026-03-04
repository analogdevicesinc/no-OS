IIO_demo_freeRTOS no-OS Example Project
=======================================

Overview
--------

The IIO_demo_freeRTOS project implements an IIO example on the FreeRTOS
operating system, aimed at establishing an IIOD server that communicates
with IIO clients and hardware like ADCs and DACs on the Maxim platform.
The project's execution flow includes initializing ADC and DAC
descriptors, setting IIO parameters, and running ``iio_app_run`` to
maintain continuous IIO operations unless an error occurs. Essential for
real-time data handling, this project accommodates embedded systems
engineers by providing a flexible framework to manage device data
through the IIO Oscilloscope application, where users can adjust
settings such as offset, sampling frequency, and data scale.

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
:git-no-OS:`Project Common Data Path </projects/iio_demo_freeRTOS/src>`.

The macros used in Common Data are defined in platform specific files
found in the 
:git-no-OS:`Project Platform Configuration Path </projects/iio_demo_freeRTOS/src/platform>`.

IIO example
~~~~~~~~~~~

The ``iio_demo_freeRTOS`` project demonstrates a basic implementation of
an IIO (Industrial I/O) example operating on the FreeRTOS platform
within a no-OS environment. The core functionality involves initializing
Analog-to-Digital Converter (ADC) and Digital-to-Analog Converter (DAC)
descriptors and setting up IIO application descriptors with these
devices. Execution is initiated through the ``iio_example_main()``
function, configuring the ADC/DAC components and starting the IIO
application. The project relies on the continuous operation of
``iio_app_run``, which manages real-time data processing. Error-handling
ensures system integrity by cleaning up resources if any initialization
fails. Primarily developed for the Maxim hardware platform, this project
supports parameter adjustments like offset and sampling frequency,
offering a flexible structure to connect IIO clients to ADC/DAC devices.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with ADI IIO Oscilloscope Client, please take a
look at: 
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

To build the IIOD demo, add the following flag when invoking make. This
will build the IIOD server and the IIO section of the driver:

.. code-block:: bash

   # Configure the example you want to enable by setting 'y' for enable and 'n' for disable
   IIO_EXAMPLE = y
   LIBRARIES += freertos

No-OS Supported Platforms
-------------------------

Maxim
~~~~~

Hardware Used
^^^^^^^^^^^^^

- MAX32690/MAX32665/MAX78000

1. Connect a **JTAG or SWD programmer** (e.g., SEGGER J-Link) to the
   board.

2. Power up your MCU board.

3. Flash using Make.

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   cd no-OS/projects/iio_demo_freeRTOS

   # to delete current build
   make reset
   # to build the project
   make PLATFORM TARGET=max32690 # you can also use max32655 max78000
   # to flash the code
   make run
