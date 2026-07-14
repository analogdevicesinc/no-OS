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

As it is the only example, it is selected by default (``--variant iio``).
See the **Build Command** section below for the build instructions.

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

For toolchain setup and prerequisites, see the
`Maxim CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_maxim_cmake.html>`__.

Available variants: ``iio``.
Available boards: ``ad-apard32690-sl``, ``max32655fthr``.
Replace ``--variant`` / ``--board`` accordingly.

.. code-block:: bash

   export MAXIM_LIBRARIES=</path/to/MaximSDK/Libraries>
   # Windows (PowerShell): $env:MAXIM_LIBRARIES = "C:\MaximSDK\Libraries"

   cd no-OS

   # build the project (iio example on the ad-apard32690-sl board)
   python tools/scripts/no_os_build.py build \
      --project iio_demo_freeRTOS --variant iio --board ad-apard32690-sl

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project iio_demo_freeRTOS --variant iio --board ad-apard32690-sl \
      --probe openocd --flash
